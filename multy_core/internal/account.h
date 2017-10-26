#ifndef MULTY_CORE_INTERNAL_ACCOUNT_H
#define MULTY_CORE_INTERNAL_ACCOUNT_H

#include "multy_core/account.h"
#include "multy_core/api.h"

#include "multy_core/internal/key_ptr.h"
#include "multy_core/internal/hd_path.h"

// Declared a struct (and out of wallet_core::internal namespace)
// for consitency with a C-like interface.
struct MULTY_CORE_API Account
{
public:
    typedef wallet_core::internal::KeyPtr KeyPtr;
    typedef wallet_core::internal::HDPath HDPath;

    virtual ~Account();

    virtual Currency get_currency() const = 0;
    virtual std::string get_address() const = 0;
    virtual HDPath get_path() const = 0;
    // TODO: separate functions for priv/pub keys
    virtual KeyPtr get_key() const = 0;
};

#endif // MULTY_CORE_INTERNAL_ACCOUNT_H
