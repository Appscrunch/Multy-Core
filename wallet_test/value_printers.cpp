/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#include "value_printers.h"

#include "wallet_test/bip39_test_cases.h"
#include "wallet_test/utility.h"

#include "wallet_core/common.h"
#include "wallet_core/error.h"

#include "wallet_core/internal/account_base.h"
#include "wallet_core/internal/key.h"
#include "wallet_core/internal/utility.h"

#include "wally_elements.h"

namespace
{
using namespace test_utility;
using namespace wallet_core::internal;

std::ostream& operator<<(std::ostream& ostr, HDPath const& path)
{
    ostr << "m";
    for (const auto i : path)
    {
        ostr << "/" << i;
    }
    return ostr;
}

} // namespace

namespace std
{

void PrintTo(const std::nullptr_t&, std::ostream* out)
{
    *out << "<nullptr>";
}

} // namespace std

void PrintTo(const Error& e, std::ostream* out)
{
    *out << "Error{ " << e.code << ", \"" << e.message << "\" }";
}

void PrintTo(const BinaryData& data, std::ostream* out)
{
    *out << "BinaryData{ " << (data.data ? to_hex(data) : std::string("<null>"))
         << ", " << data.len << " }";
}

void PrintTo(const Key& key, std::ostream* out)
{
    unsigned char serialized_key[1024];
    E(bip32_key_serialize(&key.key, 0, serialized_key, sizeof(serialized_key)));
    *out << "Key{ " << serialized_key << " }";
}

void PrintTo(const BIP39TestCase& e, std::ostream* out)
{
    *out << "BIP39TestCase{\n"
         << "\tenropy: " << e.entropy << ",\n"
         << "\tmnemonic: \"" << e.mnemonic << "\",\n"
         << "\tseed: " << e.seed << ",\n"
         << "\troot key: " << e.root_key << "\n"
         << "}";
}

void PrintTo(const Account& a, std::ostream* out)
{
    *out << "Account{\n"
         << "\tcurrency: " << a.get_currency() << ",\n"
         << "\tpath: " << a.get_path_string() << "\n"
         << "}";
}

void PrintTo(const Currency& c, std::ostream* out)
{
    switch (c)
    {
        case CURRENCY_BITCOIN:
            *out << "CURRENCY_BITCOIN";
            break;
        case CURRENCY_ETHEREUM:
            *out << "CURRENCY_ETHEREUM";
            break;
        default:
            *out << "unknown currency " << c;
            break;
    }
}

void PrintTo(const AddressType& a, std::ostream* out)
{
    switch (a)
    {
        case ADDRESS_EXTERNAL:
            *out << "ADDRESS_EXTERNAL";
            break;
        case ADDRESS_INTERNAL:
            *out << "ADDRESS_INTERNAL";
            break;
        default:
            *out << "unknown address " << a;
            break;
    }
}
