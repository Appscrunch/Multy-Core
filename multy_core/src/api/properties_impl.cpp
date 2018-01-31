/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/properties_impl.h"

#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/common.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include <cassert>
#include <exception>
#include <string>
#include <vector>

#define THROW_PROPERTY_EXCEPTION(msg) throw_exception(msg , MULTY_CODE_LOCATION)

namespace
{
using namespace multy_core::internal;
typedef Properties::Binder Binder;

enum ValueType
{
    VALUE_TYPE_INT32,
    VALUE_TYPE_BIG_INT,
    VALUE_TYPE_STRING,
    VALUE_TYPE_BINARY_DATA,
    VALUE_TYPE_PRIVATE_KEY,
};

// If you find that compilation fails du to this function definition missing,
// you should add a new template specialization for your type.
template <typename T>
constexpr ValueType deduce_value_type();

template <>
constexpr ValueType deduce_value_type<int32_t>()
{
    return VALUE_TYPE_INT32;
}

template <>
constexpr ValueType deduce_value_type<std::string>()
{
    return VALUE_TYPE_STRING;
}

template <>
constexpr ValueType deduce_value_type<BigInt>()
{
    return VALUE_TYPE_BIG_INT;
}

template <>
constexpr ValueType deduce_value_type<BinaryData>()
{
    return VALUE_TYPE_BINARY_DATA;
}

template <>
constexpr ValueType deduce_value_type<PrivateKey>()
{
    return VALUE_TYPE_PRIVATE_KEY;
}

// Property::ValueType deduce_value_type(const PrivateKey&)
//{
//    return Property::VALUE_TYPE_PRIVATE_KEY;
//}

std::string get_value_type_string(ValueType type)
{
    switch (type)
    {
        case VALUE_TYPE_INT32:
            return "int32";
        case VALUE_TYPE_STRING:
            return "string";
        case VALUE_TYPE_BIG_INT:
            return "BigInt";
        case VALUE_TYPE_BINARY_DATA:
            return "BinaryData";
        case VALUE_TYPE_PRIVATE_KEY:
            return "PrivateKey";
    }
    return "UNKNOWN TYPE";
}

struct BinderBase : public Binder
{
    BinderBase(
            const Properties& properties,
            const std::string& name,
            const void* value,
            ValueType value_type,
            Property::Trait trait)
        : m_properties(properties),
          m_name(name),
          m_value(value),
          m_type(value_type),
          m_trait(trait),
          m_is_set(false)
    {
    }

    BinderBase(const BinderBase&) = delete;
    BinderBase& operator=(const BinderBase&) = delete;
    BinderBase(BinderBase&&) = delete;
    BinderBase& operator=(BinderBase&&) = delete;

    virtual ~BinderBase()
    {
    }

    void throw_exception(std::string message, const CodeLocation& location) const
    {
        m_properties.throw_exception(message, location);
    }

    template <typename T>
    void throw_unexpected_type(const T&) const
    {
        THROW_PROPERTY_EXCEPTION("Invalid value type \""
                + ::get_value_type_string(deduce_value_type<T>())
                + "\" for property \"" + m_name + "\" expected "
                + ::get_value_type_string(m_type));
    }

    void set_value(const int32_t& value)
    {
        throw_unexpected_type(value);
    }

    void set_value(const BigInt& value)
    {
        throw_unexpected_type(value);
    }

    void set_value(const std::string& value)
    {
        throw_unexpected_type(value);
    }

    void set_value(const BinaryData& value)
    {
        throw_unexpected_type(value);
    }

    void set_value(const PrivateKey& value)
    {
        throw_unexpected_type(value);
    }

    std::string get_name() const
    {
        return m_name;
    }

    ValueType get_value_type() const
    {
        return m_type;
    }
    Property::Trait get_trait() const
    {
        return m_trait;
    }

    std::string get_value_type_string() const
    {
        return ::get_value_type_string(m_type);
    }

    std::string get_property_spec() const
    {
        return m_name + " : " + get_value_type_string() + " "
                + (m_trait == Property::REQUIRED ? "mandatory" : "optional");
    }

    bool is_set() const
    {
        return m_is_set;
    }

    const void* get_value() const
    {
        return m_value;
    }

    void handle_exception(const char* action, const CodeLocation& location)
    {
        try
        {
            throw;
        }
        catch (const std::exception& e)
        {
            throw_exception(std::string(" Failed to ") + action
                    + " of property \"" + m_name + "\" : " + e.what(), location);
        }
        catch (...)
        {
            throw_exception(std::string("Failed to ") + action
                    + " of property \"" + m_name
                    + "\" due to unknown exception", location);
        }
    }

    virtual void reset_value() = 0;

protected:
    const Properties& m_properties;
    const std::string m_name;
    const void* m_value;
    const ValueType m_type;
    const Property::Trait m_trait;
    bool m_is_set;
};

template <typename T>
void copy_value(const T& from, T* to)
{
    *to = from;
}

void copy_value(const BinaryData& from, BinaryDataPtr* to)
{
    make_clone(from).swap(*to);
}

void copy_value(const PrivateKey& from, PrivateKeyPtr* to)
{
    from.clone().swap(*to);
}

template <typename T, typename D>
void copy_value(const T& from, std::unique_ptr<T, D>* to)
{
    make_clone(from).swap(*to);
}

template <typename T, typename ValuePredicate>
struct BinderT : public BinderBase
{
    typedef typename Property::PredicateArgTraits<T>::ArgumentType ArgumentType;
    static const ValueType VAL_TYPE = deduce_value_type<ArgumentType>();
    BinderT(const Properties& properties,
            const std::string& name,
            T* value,
            Property::Trait trait,
            ValuePredicate predicate = ValuePredicate())
        : BinderBase(properties, name, value, VAL_TYPE, trait),
          m_value(value),
          m_predicate(std::move(predicate))
    {
    }

    BinderT(const BinderT&) = delete;
    BinderT& operator=(const BinderT&) = delete;
    BinderT(BinderT&&) = delete;
    BinderT& operator=(BinderT&&) = delete;

    void set_value(const ArgumentType& new_value) override
    {
        try
        {
            if (m_predicate)
            {
                m_predicate(new_value);
            }

            copy_value(new_value, m_value);
            m_is_set = true;
            m_properties.set_dirty();
        }
        catch (...)
        {
            handle_exception("set value", MULTY_CODE_LOCATION);
        }
    }

    void reset_value() override
    {
        // TODO: Since BinaryData has no default constructor, it is plain
        // dangerous to reset value like that, need to have some template
        // "reset" function to do that properly.
        // copy_value(T(), value);
        m_is_set = false;
        m_properties.set_dirty();
    }

    T* m_value;
    ValuePredicate m_predicate;
};

} // namespace

Property::Property(Properties& properties, const void* value_ptr, Trait trait)
    : m_properties(properties),
      m_value_ptr(value_ptr),
      m_trait(trait)
{
}

Property::~Property()
{
    assert(m_properties.is_valid());
    if (m_properties.is_valid())
    {
        m_properties.unbind_property_by_value(m_value_ptr);
    }
}

void Property::throw_exception(std::string message, const CodeLocation& location) const
{
    return get_properties().throw_exception(message, location);
}

void Property::throw_if_unset(const void* property_var) const
{
    if ( m_trait == REQUIRED && !is_set(property_var))
    {
        const Properties::Binder& b
                = get_properties().get_property_by_value(property_var);
        THROW_PROPERTY_EXCEPTION(m_properties.get_name() + " property \""
                + b.get_name() + "\" is not set.");
    }
}
bool Property::is_set(const void* property_var) const
{
    return get_properties().is_set(property_var);
}

const Properties& Property::get_properties() const
{
    if (!m_properties.is_valid())
    {
        THROW_EXCEPTION("Properties object is unavaliable");
    }
    return m_properties;
}

Properties::Binder::~Binder()
{
}

Properties::Properties(const std::string& name)
    : m_name(name),
      m_properties(),
      m_property_name_by_value(),
      m_is_dirty(true)
{
}

void Properties::reset_property(const std::string& name)
{
    get_property(name).reset_value();
}

std::vector<const Binder*> Properties::get_all_properties() const
{
    std::vector<const Binder*> result;
    result.reserve(m_properties.size());
    for (const auto& i : m_properties)
    {
        result.push_back(i.second.get());
    }
    return result;
}

std::string Properties::get_name() const
{
    return m_name;
}

bool Properties::validate(std::vector<std::string>* unset_properties_names) const
{
    bool all_required_properties_set = true;
    for (const auto& i : m_properties)
    {
        const Binder& property = *i.second;
        if ((property.get_trait() == Property::REQUIRED) && !property.is_set())
        {
            all_required_properties_set = false;
            if (unset_properties_names)
            {
                unset_properties_names->push_back(property.get_name());
            }
        }
    }
    m_is_dirty = !all_required_properties_set;
    return all_required_properties_set;
}

bool Properties::is_dirty() const
{
    return m_is_dirty;
}

void Properties::set_dirty() const
{
    m_is_dirty = true;
}

void Properties::bind_property(
        const std::string& name,
        int32_t* value,
        Property::Trait trait,
        Property::Predicate<int32_t> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        std::string* value,
        Property::Trait trait,
        Property::Predicate<std::string> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        BigInt* value,
        Property::Trait trait,
        Property::Predicate<BigInt> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        BinaryDataPtr* value,
        Property::Trait trait,
        Property::Predicate<BinaryData> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        PrivateKeyPtr* value,
        Property::Trait trait,
        Property::Predicate<PrivateKey> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

template <typename T, typename P>
void Properties::do_bind_property(
        const std::string& name, T* value, Property::Trait trait, P predicate)
{
    if (!value)
    {
        THROW_PROPERTY_EXCEPTION("Attempt to bind property to nullptr.");
    }

    make_property(name, value).reset(new BinderT<T, P>(
            *this, name, value, trait, std::move(predicate)));
}

bool Properties::unbind_property(const std::string& name)
{
    const auto p = m_properties.find(name);
    if (p == m_properties.end())
    {
        return false;
    }
    const auto p2 = m_property_name_by_value.find(p->second->get_value());
    m_properties.erase(p);
    if (p2 == m_property_name_by_value.end())
    {
        // Internal inconsistncy, incident should be logged.
    }
    else
    {
        m_property_name_by_value.erase(p2);
    }
    return true;
}

bool Properties::unbind_property_by_value(const void* value)
{
    const auto p2 = m_property_name_by_value.find(value);
    if (p2 == m_property_name_by_value.end())
    {
        return false;
    }

    auto p = m_properties.find(p2->second);
    m_property_name_by_value.erase(p2);
    if (p == m_properties.end())
    {
        // Internal inconsistncy, incident should be logged.
    }
    else
    {
        m_properties.erase(p);
    }
    return true;
}

Binder& Properties::get_property(const std::string& name)
{
    return const_cast<Binder&>(
            const_cast<const Properties*>(this)->get_property(name));
}

const Binder& Properties::get_property(const std::string& name) const
{
    auto i = m_properties.find(name);
    if (i == m_properties.end())
    {
        THROW_PROPERTY_EXCEPTION("Property with name \"" + name + "\" does not exists.");
    }
    return *i->second;
}

Binder& Properties::get_property_by_value(void* value)
{
    return const_cast<Binder&>(
            const_cast<const Properties*>(this)->get_property_by_value(value));
}

const Binder& Properties::get_property_by_value(const void* value) const
{
    auto p = m_property_name_by_value.find(value);
    if (p == m_property_name_by_value.end())
    {
        THROW_PROPERTY_EXCEPTION("Can't find property bound to value.");
    }

    return get_property(p->second);
}

Properties::BinderPtr& Properties::make_property(
        const std::string& name, const void* value)
{
    auto p = m_properties.insert(std::make_pair(name, BinderPtr()));
    if (!p.second)
    {
        THROW_PROPERTY_EXCEPTION("Property with name \"" + name + "\" already exists.");
    }

    auto p2 = m_property_name_by_value.insert(std::make_pair(value, name));
    if (!p2.second)
    {
        m_properties.erase(p.first);
        THROW_PROPERTY_EXCEPTION("Value is already bound to property \""
                + p2.first->second + "\".");
    }
    return p.first->second;
}

bool Properties::is_set(const void* value) const
{
    auto p = m_property_name_by_value.find(value);
    if (p != m_property_name_by_value.end())
    {
        return get_property(p->second).is_set();
    }
    return false;
}

void Properties::throw_exception(const std::string& message, const CodeLocation& location) const
{
    throw Exception(get_name().c_str(), location) << " : " << message;
}

const void* Properties::get_object_magic()
{
    RETURN_MAGIC();
}
