#include "common.h"


void free_error(Error* error)
{
     if (!error) {
         return;
     }
     delete error;
}

void free_binarydata(BinaryData* data)
{
    if (!data) {
        return;
    }
    delete [] data->data;
    delete data;
}
