#include "account_base.h"

#include "internal/utility.h"

namespace
{
using namespace wallet_core::internal;
constexpr uint32_t hardened_index(uint32_t index)
{
    return index | HARDENED_INDEX_BASE;
}

uint64_t to_map_key(AddressType type, uint32_t index)
{
    uint64_t result = type;
    result <<= 32;
    result |= index;
    return result;
}

const uint32_t BIP44_PURPOSE_CHAIN_CODE = hardened_index(44);
static_assert(BIP44_PURPOSE_CHAIN_CODE == 0x8000002C,
              "invalid hardened index derivation function");
} // namepace

namespace wallet_core
{
namespace internal
{

AccountAddress::AccountAddress(KeyPtr private_key, KeyPtr public_key)
    : private_key(std::move(private_key)),
      public_key(std::move(public_key))
{}

AccountAddress::~AccountAddress()
{}

} // namespace wallet_core
} // namespace internal

Account::Account(const Key& master_key, Currency currency, uint32_t index)
    : m_currency(currency)
{
    // BIP44 derive account key:
    // master key -> currency key -> account key.
    KeyPtr purpose_key, currency_key;
    const uint32_t currency_index =
            hardened_index(static_cast<uint32_t>(currency));

    const uint32_t account_index = hardened_index(index);

    throw_if_error(make_child_key(&master_key, CHILD_KEY_TYPE_PRIVATE,
                                  BIP44_PURPOSE_CHAIN_CODE,
                                  reset_sp(purpose_key)));
    throw_if_error(make_child_key(purpose_key.get(),
                                  CHILD_KEY_TYPE_PRIVATE,
                                  currency_index,
                                  reset_sp(currency_key)));
    throw_if_error(make_child_key(currency_key.get(),
                                  CHILD_KEY_TYPE_PRIVATE,
                                  account_index,
                                  reset_sp(m_account_key)));

    m_bip44_path[0] = BIP44_PURPOSE_CHAIN_CODE;
    m_bip44_path[1] = currency_index;
    m_bip44_path[2] = account_index;
}

Account::~Account()
{}

Currency Account::get_currency() const
{
    return m_currency;
}

const AccountAddress& Account::get_address(AddressType type, uint32_t index)
{
    const uint64_t address_map_key = to_map_key(type, index);
    {
        // get a cached AccountAddress first.
        auto address_iterator = m_addresses.find(address_map_key);
        if (address_iterator != m_addresses.end())
        {
            return *address_iterator->second;
        }
    }

    KeyPtr& key_ptr = m_type_keys.at(type);
    if (!key_ptr)
    {
        throw_if_error(make_child_key(m_account_key.get(),
                                      CHILD_KEY_TYPE_PRIVATE,
                                      static_cast<uint32_t>(type),
                                      reset_sp(key_ptr)));
    }

    AccountAddressPtr new_address = make_address(*key_ptr, index);
    // TODO: use glsl::not_null
    AccountAddress* result = new_address.get();
    m_addresses[address_map_key].swap(new_address);

    return *result;
}
