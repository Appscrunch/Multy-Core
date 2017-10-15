#ifndef UTILITY_H
#define UTILITY_H

#include "common.h"
#include "internal/utility.h"

#include <string>
#include <vector>

#define E(statement) throw_if_wally_error((statement), #statement)

namespace test_utility
{
typedef std::vector<unsigned char> bytes;

bytes from_hex(const char* hex_str);
std::string to_hex(const bytes& bytes);
std::string to_hex(const BinaryData& data);
BinaryData to_binary_data(const bytes& data);

} // test_utility

#endif // UTILITY_H
