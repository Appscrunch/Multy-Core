#ifndef WALLET_CORE_UTILITY_H
#define WALLET_CORE_UTILITY_H

/** Set of utility functions/classes/etc.
 * Not part of the interface, and should not be used by clients.
 */

#include "wallet_core/error.h"

#include <memory>

struct Error;

#define ARG_CHECK(arg) do {\
    if (!(arg)) {\
        return make_error(ERROR_INVALID_ARGUMENT, "Argument check failed: " #arg); \
    }\
} while (false)

namespace wallet_core
{

namespace internal
{
inline void throw_if_error(Error* err)
{
    if (err)
    {
        throw err;
    }
}

inline void throw_if_wally_error(int err_code, const char* message)
{
    if (err_code != 0)
    {
        throw_if_error(internal_make_error(err_code, message));
    }
}

template <typename T, size_t N>
constexpr size_t array_size(T (&)[N])
{
    return N;
}

/// Converts exception to a Error*, to be used inside a catch(...) block.
Error* exception_to_error();

/** Convenience function to copy a string.
 * @param str - string to copy, must not be null.
 * @return - copy of a string, must be freed with free_string(), can be null on error.
 */
char* copy_string(const char* str);

char* copy_string(const std::string& str);

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
    // TODO: change from reference to pointer
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
inline UniquePointerUpdater<SP> reset_sp(SP& sp)
{
    return UniquePointerUpdater<SP>(sp);
}

template <typename T, typename D>
inline std::unique_ptr<T, D> null_unique_ptr(D deleter)
{
    return std::unique_ptr<T, D>(nullptr, deleter);
}

} // namespace internal
} // namespace wallet_core

#endif // WALLET_CORE_UTILITY_H
