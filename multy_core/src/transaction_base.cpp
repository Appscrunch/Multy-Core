/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/transaction_base.h"

#include <sstream>

namespace multy_core
{
namespace internal
{

TransactionFeeBase::TransactionFeeBase()
    : m_properties(ERROR_SCOPE_TRANSACTION_FEE, "TransactionFee")
{
}

TransactionFeeBase::~TransactionFeeBase()
{
}

Properties& TransactionFeeBase::get_properties()
{
    return m_properties;
}

TransactionDestinationBase::TransactionDestinationBase()
    : m_properties(ERROR_SCOPE_TRANSACTION_DESTINATION, "TransactionDestination")
{
}

TransactionDestinationBase::~TransactionDestinationBase()
{
}

Properties& TransactionDestinationBase::get_properties()
{
    return m_properties;
}

TransactionSourceBase::TransactionSourceBase()
    : m_properties(ERROR_SCOPE_TRANSACTION_SOURCE, "TransactionSource")
{
}

TransactionSourceBase::~TransactionSourceBase()
{
}

Properties& TransactionSourceBase::get_properties()
{
    return m_properties;
}

TransactionBase::TransactionBase(BlockchainType blockchain_type)
    : m_properties(ERROR_SCOPE_TRANSACTION, "Transaction"),
      m_blockchain_type(blockchain_type),
      m_all_properties()
{
    register_properties("", m_properties);
}

BlockchainType TransactionBase::get_blockchain_type() const
{
    return m_blockchain_type;
}

Properties& TransactionBase::get_transaction_properties()
{
    return m_properties;
}

Properties& TransactionBase::register_properties(
        const std::string& name, Properties& properties)
{
    m_all_properties.push_back(std::make_pair(name, &properties));
    return properties;
}

bool TransactionBase::validate_all_properties(
        std::string* not_set_properties) const
{
    std::ostringstream sstr;
    std::vector<std::string> missing_props_raport;
    for (const auto& p : m_all_properties)
    {
        if (!p.second->validate(&missing_props_raport))
        {
            sstr << p.second->get_name() << p.first << ":\n";
            for (const auto& s : missing_props_raport)
            {
                sstr << "\t" << s << "\n";
            }
            missing_props_raport.clear();
        }
    }
    if (sstr.tellp() > 0)
    {
        *not_set_properties = sstr.str();
        return false;
    }
    return true;
}

} // namespace internal
} // namespaec multy_core
