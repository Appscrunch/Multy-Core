#include "mnemonic.h"

#include "error.h"
#include "common.h"
#include "internal/utility.h"

#include "wally_core.h"
#include "wally_bip39.h"

#include <cstring>
#include <memory>
#include <stdlib.h>

Error* make_mnemonic(EntropySource entropy_source, const char ** mnemonic)
{
    static const size_t entropy_size = BIP39_ENTROPY_LEN_256;
    unsigned char entropy[entropy_size] = {'\0'};

    ARG_CHECK(entropy_source);
    ARG_CHECK(mnemonic);

    if (entropy_source(entropy_size, &entropy) != entropy_size)
    {
        return make_error(ERROR_BAD_ENTROPY,
                "Unable to get required amount of entropy");
    }

    const words* dictionary = nullptr;
    int result = bip39_get_wordlist(nullptr, &dictionary);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to obtain wordlist");
    }
    char* out = nullptr;
    result = bip39_mnemonic_from_bytes(dictionary, entropy, entropy_size, &out);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to generated mnemonic");
    }
    *mnemonic = out;

    return nullptr;
}

Error* make_seed(const char* mnemonic, const char* password, BinaryData** seed)
{
    static const size_t max_seed_size = BIP39_SEED_LEN_512;
    size_t written = 0;

    ARG_CHECK(mnemonic);
    ARG_CHECK(seed);

    std::unique_ptr<unsigned char[]> data(new unsigned char[max_seed_size]);
    int result = bip39_mnemonic_to_seed(mnemonic, password, data.get(),
            max_seed_size, &written);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Faield to generate seed");
    }

    BinaryData* out = new BinaryData;
    out->data = data.release();
    out->len = written;
    *seed = out;

    return nullptr;
}

Error* seed_to_string(const BinaryData* seed, const char** str)
{
    ARG_CHECK(seed);
    ARG_CHECK(str);

    char* out = nullptr;

    int result = wally_base58_from_bytes(seed->data, seed->len, 0, &out);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to convert seed to string");
    }
    *str = out;
    return nullptr;
}

void free_mnemonic(const char* mnemonic)
{
    if (!mnemonic)
    {
        return;
    }
    wally_free_string(const_cast<char*>(mnemonic));
}

void free_seed_string(const char* str)
{
    if (!str)
    {
        return;
    }
    wally_free_string(const_cast<char*>(str));
}
