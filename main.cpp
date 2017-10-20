#include "mnemonic.h"

#include "common.h"
#include "error.h"
#include "internal/utility.h"

#include <string.h>
#include <iostream>
#include <memory>

namespace
{

size_t entropySource(size_t size, void* dest)
{
    /** Poor man's entropy, using uninitialized data from stack, which is:
     * - Fast;
     * - Unsecure;
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

using namespace wallet_core::internal;

} // namespace

int main()
{
    try
    {
        auto mnemonic = null_unique_ptr<const char>(free_mnemonic);
        throw_if_error(make_mnemonic(&entropySource, reset_sp(mnemonic)));
        std::cout << "Generated mnemonic: " << mnemonic.get() << std::endl;

        std::cout << "Enter password: ";
        std::string password;
        std::cin >> password;

        auto seed = null_unique_ptr<BinaryData>(free_binarydata);
        throw_if_error(make_seed(mnemonic.get(), password.c_str(),
                reset_sp(seed)));

        auto seed_string = null_unique_ptr<const char>(free_string);
        throw_if_error(seed_to_string(seed.get(), reset_sp(seed_string)));
        std::cout << "Seed: " << seed_string.get() << std::endl;
    }
    catch(Error* e)
    {
        std::cerr << "Got error: " << e->message << std::endl;
        free_error(e);
        return -1;
    }
    return 0;
}
