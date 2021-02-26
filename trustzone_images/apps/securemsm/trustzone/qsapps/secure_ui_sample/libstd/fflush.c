/* This is not supported in QSEE. */
#include "io_file.h"

int fflush(FILE *f)
{
  return 0;
}
