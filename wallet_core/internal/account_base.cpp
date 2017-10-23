#include "wallet_core/internal/account_base.h"

#include "wallet_core/internal/utility.h"

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

uint32_t to_chain_code(Currency currency)
{
    // See: https://github.com/satoshilabs/slips/blob/master/slip-0044.md
    static const uint32_t currency_chain_codes[] =
    {
        0x80000000, // BITCOIN
        0x8000003c, // EHTER
    };

    if (array_size(currency_chain_codes) < static_cast<size_t>(currency))
    {
        throw std::runtime_error("Can't convert currency to the chain code");
    }
    return currency_chain_codes[currency];
}
// According to bip 44, complete address path looks like:
// m / purpose' / coin_type' / account' / change / address_index
const size_t BIP44_PURPOSE = 0;
const size_t BIP44_COIN_TYPE = 1;
const size_t BIP44_ACCOUNT = 2;
const size_t BIP44_ACCOUNT_PATH_DEPTH = BIP44_ACCOUNT + 1;

const uint32_t BIP44_PURPOSE_CHAIN_CODE = hardened_index(44);
static_assert(BIP44_PURPOSE_CHAIN_CODE == 0x8000002C,
              "invalid hardened index derivation function implementation");
} // namepace

namespace wallet_core
{
namespace internal
{

AccountAddress::AccountAddress(KeyPtr extended_key)
    : m_extended_key(std::move(extended_key))
{}

AccountAddress::AccountAddress(const HDPath& path, KeyPtr extended_key)
    : m_path(path),
      m_extended_key(std::move(extended_key))
{}

AccountAddress::~AccountAddress()
{}

std::string AccountAddress::get_path_string() const
{
    std::string result;
    result.reserve(m_path.size() * 10);
    result.append("m");
    for (const auto& p : get_path())
    {
        result.append("/");
        result.append(std::to_string(p));
    }
    return result;
}

const HDPath& AccountAddress::get_path() const
{
    return m_path;
}

const Key& AccountAddress::get_key() const
{
    return *m_extended_key;
}

HDPath make_child_path(HDPath parent_path, uint32_t child_chain_code)
{
    HDPath result(std::move(parent_path));
    result.push_back(child_chain_code);
    return result;
}

} // namespace wallet_core
} // namespace internal

Account::Account(const Key& master_key, Currency currency, uint32_t index)
    : m_currency(currency),
      m_bip44_path(BIP44_ACCOUNT_PATH_DEPTH)
{
    // BIP44 derive account key:
    // master key -> currency key -> account key.
    KeyPtr purpose_key, currency_key;
    const uint32_t currency_index = to_chain_code(currency);
    const uint32_t account_index = hardened_index(index);

    throw_if_error(make_child_key(&master_key, KEY_TYPE_PRIVATE,
                                  BIP44_PURPOSE_CHAIN_CODE,
                                  reset_sp(purpose_key)));
    throw_if_error(make_child_key(purpose_key.get(),
                                  KEY_TYPE_PRIVATE,
                                  currency_index,
                                  reset_sp(currency_key)));
    throw_if_error(make_child_key(currency_key.get(),
                                  KEY_TYPE_PRIVATE,
                                  account_index,
                                  reset_sp(m_account_key)));

    m_bip44_path[BIP44_PURPOSE] = BIP44_PURPOSE_CHAIN_CODE;
    m_bip44_path[BIP44_COIN_TYPE] = currency_index;
    m_bip44_path[BIP44_ACCOUNT] = account_index;
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
        // get cached AccountAddress first
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
                                      KEY_TYPE_PRIVATE,
                                      static_cast<uint32_t>(type),
                                      reset_sp(key_ptr)));
    }

    AccountAddressPtr new_address = make_address(*key_ptr, type, index);
    // TODO: use glsl::not_null
    if (!new_address)
    {
        throw std::runtime_error("Internal error: make_address() returned a null");
    }

    AccountAddress* result = new_address.get();
    m_addresses[address_map_key].swap(new_address);

    return *result;
}

const HDPath& Account::get_path_string() const
{
    return m_bip44_path;
}
