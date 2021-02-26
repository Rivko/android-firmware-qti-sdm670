/* This is not supported in QSEE. */
#include "io_file.h"

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
  return -1;
}
