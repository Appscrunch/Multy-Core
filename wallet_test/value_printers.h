#ifndef VALUE_PRINTERS_H
#define VALUE_PRINTERS_H

#include <memory>
#include <ostream>

struct Error;
struct BinaryData;

namespace std
{
void PrintTo(const std::nullptr_t&, std::ostream*);
} // namespace std;

void PrintTo(const Error&, std::ostream*);
void PrintTo(const BinaryData&, std::ostream*);

template <typename T, typename D>
inline void PrintTo(const std::unique_ptr<T, D>& up, std::ostream* out)
{
    if (up)
    {
        PrintTo(*up, out);
    }
    else
    {
        PrintTo(nullptr, out);
    }
}


#endif // VALUE_PRINTERS_H