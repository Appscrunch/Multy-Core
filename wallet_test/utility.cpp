/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "wallet_test/utility.h"

#include "multy_core/internal/key.h"
#include "wally_core.h"

#include <memory.h>
#include <string.h>

namespace
{
using namespace wallet_core::internal;
}

namespace test_utility
{

bytes from_hex(const char* hex_str)
{
    const size_t expected_size = strlen(hex_str) / 2;
    bytes result(expected_size);
    size_t bytes_written = 0;

    E(wally_hex_to_bytes(
            hex_str, result.data(), result.size(), &bytes_written));
    result.resize(bytes_written);

    return result;
}

std::string to_hex(const bytes& bytes)
{
    auto hex_str = null_unique_ptr<char>(free_string);
    E(wally_hex_from_bytes(bytes.data(), bytes.size(), reset_sp(hex_str)));
    return std::string(hex_str.get());
}

std::string to_hex(const BinaryData& data)
{
    if (!data.data)
    {
        throw std::runtime_error("BinaryData::data is null");
    }

    auto hex_str = null_unique_ptr<char>(free_string);
    E(wally_hex_from_bytes(data.data, data.len, reset_sp(hex_str)));
    return std::string(hex_str.get());
}

BinaryData to_binary_data(const bytes& data)
{
    return BinaryData{data.data(), data.size()};
}

} // namespace test_utility

bool operator==(const BinaryData& lhs, const BinaryData& rhs)
{
    return lhs.len == rhs.len && memcmp(lhs.data, rhs.data, lhs.len) == 0;
}

bool operator==(const Key& lhs, const Key& rhs)
{
    return memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}
