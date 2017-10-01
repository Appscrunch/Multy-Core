#include "common.h"


void free_binarydata(BinaryData* data)
{
    if (!data) {
        return;
    }
    delete [] data->data;
    delete data;
}
