#include "mnemonic.h"

#include "common.h"
#include "error.h"

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

void throw_if_error(Error* err)
{
    if (err)
    {
        throw err;
    }
}

template <typename T, typename D>
std::unique_ptr<T, D> up(T* ptr, D deleter)
{
    return std::unique_ptr<T, D>(ptr, deleter);
}

/** Convenience to simplify passing C++ smart_pointers (like std::unique_ptr<T>)
 * to C-like functions than take T** and store address of new object there.
 * Should be used in conjunction with reset_sp() function.
 * Resets value of smart pointer if it was requested to be converted to pointer-to-pointer (T**)
 * and that value was modified during lifetime of the object.
 */
template <typename SP>
class UniquePointerUpdater
{
    typedef typename SP::pointer Pointer;
    SP& sp;
    mutable Pointer p;
public:
    explicit UniquePointerUpdater(SP& sp)
        : sp(sp),
          p(sp.get())
    {}
    ~UniquePointerUpdater()
    {
        if (p != sp.get())
        {
            sp.reset(p);
        }
    }
    operator Pointer*() const
    {
        p = sp.get();
        return &p;
    }
};

/// See UniquePointerUpdater.
template <typename SP>
UniquePointerUpdater<SP> reset_sp(SP& sp)
{
    return UniquePointerUpdater<SP>(sp);
}

template <typename T, typename D>
std::unique_ptr<T, D> null_unique_ptr(D deleter)
{
    return std::unique_ptr<T, D>(nullptr, deleter);
}

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
