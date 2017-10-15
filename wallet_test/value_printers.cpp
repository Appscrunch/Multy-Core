#include "value_printers.h"

#include "utility.h"

#include "error.h"
#include "common.h"

namespace
{
using namespace test_utility;
} // namespace

namespace std
{

void PrintTo(const std::nullptr_t&, std::ostream* out)
{
    *out << "<nullptr>";
}

} // namespace std

void PrintTo(const Error& e, std::ostream* out)
{
    *out << "Error{ " << e.code << ", \"" << e.message << "\" }";
}

void PrintTo(const BinaryData& data, std::ostream* out)
{
    *out << "BinaryData{ " << (data.data ? to_hex(data) :  std::string("<null>")) << ", " << data.len << " }";
}
