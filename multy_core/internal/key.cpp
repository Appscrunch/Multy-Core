#include "multy_core/internal/key.h"

#include "wally_bip32.h"

std::string ExtendedKey::to_string() const
{
    std::string result(BIP32_SERIALIZED_LEN + 1, '\0');
    bip32_key_serialize(
            &key, 0,
            const_cast<unsigned char*>(
                    reinterpret_cast<const unsigned char*>(result.data())),
            result.length());
    return result;
}

Key::~Key()
{
}

PrivateKey::~PrivateKey()
{
}

PublicKey::~PublicKey()
{
}
