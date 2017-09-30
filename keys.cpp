#include "keys.h"

#include "wally_core.h"
#include "wally_bip32.h"

Error* make_master_key(const BinaryData* seed, Key* key)
{
    return nullptr;
}

Error* make_child_key(const Key* parent_key, Key* key)
{
    return nullptr;
}

Error* key_to_string(const Key*, const char **str)
{
    return nullptr;
}

void free_key(Key* /*root*/)
{
}

void free_key_strnig(char */*str*/)
{

}
