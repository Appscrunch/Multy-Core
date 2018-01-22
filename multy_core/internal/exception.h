/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_EXCEPTION_H
#define MULTY_CORE_INTERNAL_EXCEPTION_H

#include "multy_core/api.h"
#include "multy_core/error.h"

#include "multy_core/error.h"

#include <exception>
#include <string>

namespace wallet_core
{
namespace internal
{

class MULTY_CORE_API Exception : public std::exception
{
public:
    Exception(const char* message, CodeLocation location);
    virtual ~Exception();

    virtual Error* make_error() const;

    const char* what() const noexcept override;

    void append_message(const char* message) const;

private:
    mutable std::string m_message;
    const CodeLocation m_location;
};

#define THROW_EXCEPTION(msg) \
    throw wallet_core::internal::Exception(msg, MULTY_CODE_LOCATION)

} // namespace internal
} // namespace wallet_core

#endif // MULTY_CORE_INTERNAL_EXCEPTION_H
