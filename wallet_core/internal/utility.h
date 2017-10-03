#ifndef WALLET_UTILITY_H
#define WALLET_UTILITY_H

#include "error.h"

#include <memory>

struct Error;

namespace wallet_core
{

namespace internal
{
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
inline UniquePointerUpdater<SP> reset_sp(SP& sp)
{
    return UniquePointerUpdater<SP>(sp);
}

template <typename T, typename D>
inline std::unique_ptr<T, D> null_unique_ptr(D deleter)
{
    return std::unique_ptr<T, D>(nullptr, deleter);
}

inline void throw_if_error(Error* err)
{
    if (err)
    {
        throw err;
    }
}

} // namespace internal

} // namespace wallet_core

#define ARG_CHECK(arg) do {\
    if (!(arg)) {\
        return make_error(ERROR_INVALID_ARGUMENT, "Argument check failed: " #arg); \
    }\
} while (false)

#endif // UTILITY_H
