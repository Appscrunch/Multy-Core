/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#ifndef WALLET_CORE_INTERNAL_KEY_PTR_H
#define WALLET_CORE_INTERNAL_KEY_PTR_H

#include "multy_core/keys.h"

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

#endif // WALLET_CORE_INTERNAL_KEY_PTR_H
