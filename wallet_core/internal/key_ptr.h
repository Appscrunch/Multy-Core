#ifndef WALLET_KEY_PTR_H
#define WALLET_KEY_PTR_H

#include "keys.h"

#include <memory>

namespace wallet_core
{

namespace internal
{
struct KeyDeleter
{
    inline void operator()(Key* key)
    {
        free_key(key);
    }
};
typedef std::unique_ptr<Key, KeyDeleter> KeyPtr;

} // namespace wallet_core

} // namespace internal

#endif // WALLET_KEY_PTR_H
