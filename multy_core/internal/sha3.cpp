/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/sha3.h"

#include "multy_core/common.h"
#include "multy_core/sha3.h"

#include "multy_core/internal/exception.h"
#include "multy_core/internal/utility.h"

extern "C" {
#include "keccak-tiny/keccak-tiny.h"
} // extern "C"

#include <sstream>

namespace
{
using namespace wallet_core::internal;

// Since on some Android SDK versions std::to_string() is missing.
template <typename T>
std::string my_to_string(const T& value)
{
    std::stringstream sstr;
    sstr << value;
    return sstr.str();
}

#define DO_SHA3(size, in, out)                                                 \
    THROW_IF_WALLY_ERROR(                                                      \
            ::sha3_##size(                                                     \
                    const_cast<uint8_t*>(output->data), output->len,           \
                    input.data, input.len),                                    \
            "Failed to SHA3-" #size " input data");

void do_sha3(const BinaryData& input, BinaryData* output)
{
    int error = 0;
    switch (output->len * 8)
    {
        case 224:
        {
            DO_SHA3(224, input, output);
            break;
        }
        case 256:
        {
            DO_SHA3(256, input, output);
            break;
        }
        case 384:
        {
            DO_SHA3(384, input, output);
            break;
        }
        case 512:
        {
            DO_SHA3(512, input, output);
            break;
        }
        default:
        {
            THROW_EXCEPTION("unsupported hash size: ")
                    << my_to_string(output->len);
        }
    }
}

} // namespace

namespace wallet_core
{
namespace internal
{

BinaryDataPtr sha3(size_t hash_size, const BinaryData& input)
{
    BinaryDataPtr result;

    throw_if_error(make_binary_data(hash_size / 8, reset_sp(result)));
    do_sha3(input, result.get());

    return result;
}

void sha3(const BinaryData& input, BinaryData* output)
{
    static const size_t SUPPORTED_HASH_SIZES[] = {
        SHA3_224, SHA3_256, SHA3_384, SHA3_512
    };

    static const size_t default_value = 0;

    if (!output)
    {
        THROW_EXCEPTION("output BinaryData is nullprt");
    }

    const size_t hash_size = find_max_value(
            SUPPORTED_HASH_SIZES, default_value, output->len);
    if (hash_size == default_value)
    {
        THROW_EXCEPTION("output BinaryData has not enough space available (")
                << my_to_string(output->len) << " bytes) to hold hash result.";
    }
    output->len = hash_size;
    do_sha3(input, output);
}

} // namespace internale
} // namespace wallet_core
