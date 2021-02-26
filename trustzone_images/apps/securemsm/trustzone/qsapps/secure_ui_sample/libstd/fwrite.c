/* This is not supported in QSEE. */
#include "io_file.h"
 
size_t fwrite(const void *restrict src, size_t size, size_t nmemb, FILE *restrict f)
{
    return 0;
}
