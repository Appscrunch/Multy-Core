#include "value_printers.h"

#include "utility.h"
#include "bip39_test_cases.h"

#include "internal/account_base.h"
#include "internal/key.h"
#include "internal/utility.h"

#include "error.h"
#include "common.h"

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
    *out << "BinaryData{ " << (data.data ? to_hex(data) :  std::string("<null>")) << ", " << data.len << " }";
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
         << "\tpath: " << a.get_path() << "\n"
         << "}";
}
