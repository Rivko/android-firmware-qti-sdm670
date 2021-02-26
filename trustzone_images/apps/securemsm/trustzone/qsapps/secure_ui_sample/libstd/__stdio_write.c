/* This is not supported in QSEE. */
#include "io_file.h"

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
  return 0;
}
