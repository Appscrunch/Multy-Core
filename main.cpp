#include "mnemonic.h"

#include "wally_core.h"

#include <string.h>
#include <iostream>
#include <memory>

size_t entropySource(size_t size, void* dest)
{
    /** Poor man's entropy, using uninitialized data from stack, which is:
     * - Fast;
     * - Non-secure;
     * - Somewhat predictable;
     * And hence SHOULD not be used in production, but Ok in POC driver program.
     */
    static const size_t entropy_max_size = 1024;
    unsigned char silly_entropy[entropy_max_size];

    if (size > entropy_max_size)
    {
        return 0;
    }

    memcpy(dest, silly_entropy, size);
    return size;
}

int main()
{
    const char* mnemonic = nullptr;
    std::unique_ptr<Error> e(make_mnemonic(&entropySource, &mnemonic));
    if (e) {
        std::cerr << "Got error: " << e->message << std::endl;
        return -1;
    }
    std::cout << "Generated mnemonic: " << mnemonic << std::endl;
    std::cout << "Enter password: ";
    std::string password;
    std::cin >> password;

    BinaryData* seed = nullptr;
    e.reset(make_seed(mnemonic, password.c_str(), &seed));
    if (e) {
        std::cerr << "Got error: " << e->message << std::endl;
        return -2;
    }
    char* seed_string = nullptr;
    if (wally_base58_from_bytes(seed->data, seed->len, 0, &seed_string) != WALLY_OK) {
        std::cerr << "Got error: " << "can't convert seed to printable format" << std::endl;
        return -3;
    }
    std::cout << "Seed: " << seed_string << std::endl;

    wally_free_string(seed_string);
    free_binarydata(seed);
    free_mnemonic(mnemonic);
    return 0;
}
