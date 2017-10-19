#ifndef UTILITY_H
#define UTILITY_H

#include "wallet_core/common.h"
#include "wallet_core/internal/utility.h"

#include <string>
#include <vector>

#define E(statement) wallet_core::internal::throw_if_wally_error((statement), #statement)

struct Key;

namespace test_utility
{
typedef std::vector<unsigned char> bytes;

bytes from_hex(const char* hex_str);
std::string to_hex(const bytes& bytes);
std::string to_hex(const BinaryData& data);
BinaryData to_binary_data(const bytes& data);
} // test_utility

bool operator==(const BinaryData& lhs, const BinaryData& rhs);
bool operator==(const Key& lhs, const Key& rhs);
inline bool operator!=(const BinaryData& lhs, const BinaryData& rhs)
{
    return !(lhs == rhs);
}

#endif // UTILITY_H
