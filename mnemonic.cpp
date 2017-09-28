#include "mnemonic.h"

#include "wally_core.h"
#include "wally_bip39.h"

#include <cstring>
#include <memory>
#include <stdlib.h>

namespace
{

Error* MakeError(const char* message)
{
    Error* result = new Error;
    result->message = message;
    return result;
}

}  // namespace

Error* init()
{
    return nullptr;
}

Error* deinit()
{
    return nullptr;
}

Error* make_mnemonic(EntropySource entropySource, const char ** mnemonic)
{
    static const size_t entropy_size = BIP39_ENTROPY_LEN_256;
    unsigned char entropy[entropy_size] = {'\0'};
    if (entropySource(entropy_size, &entropy) != entropy_size) {
        return MakeError("Unable to get required amount of entropy");
    }

    const words* dictionary = nullptr;
    int result = bip39_get_wordlist(nullptr, &dictionary);
    if (result != WALLY_OK) {
        return MakeError("Failed to obtain wordlist");
    }
    char* out = nullptr;
    result = bip39_mnemonic_from_bytes(dictionary, entropy, entropy_size, &out);
    if (result != WALLY_OK) {
        return MakeError("Failed to generated mnemonic");
    }
    *mnemonic = out;

    return nullptr;
}

Error* make_seed(const char* mnemonic, const char* password, BinaryData** seed)
{
    static const size_t max_seed_size = BIP39_SEED_LEN_512;
    size_t written = 0;

    std::unique_ptr<unsigned char[]> data(new unsigned char[max_seed_size]);
    int result = bip39_mnemonic_to_seed(mnemonic, password, data.get(), max_seed_size, &written);
    if (result != WALLY_OK) {
        return MakeError("Faield to generate seed");
    }

    BinaryData* out = new BinaryData;
    out->data = data.release();
    out->len = written;
    *seed = out;

    return nullptr;
}

void free_error(Error* error)
{
     if (!error) {
         return;
     }
     delete error;
}

void free_mnemonic(const char* mnemonic)
{
    if (!mnemonic) {
        return;
    }
    wally_free_string(const_cast<char*>(mnemonic));
}

void free_binarydata(BinaryData* data)
{
    if (!data) {
        return;
    }
    delete [] data->data;
    delete data;
}
