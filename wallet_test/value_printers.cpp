#include "value_printers.h"

#include "error.h"
#include "common.h"

void PrintTo(const std::nullptr_t&, std::ostream* out)
{
    *out << "<nullptr>";
}

void PrintTo(const Error& e, std::ostream* out)
{
    *out << "Error{ " << e.code << ", \"" << e.message << "\" }";
}

void PrintTo(const BinaryData& data, std::ostream* out)
{
    *out << "BinaryData{ " << (data.data ? "<binary data>" : "<null>") << ", " << data.len << " }";
}
