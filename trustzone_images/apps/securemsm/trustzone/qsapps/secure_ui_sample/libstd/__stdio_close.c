/* This is not supported in QSEE. */
#include "io_file.h"

int __stdio_close(FILE *f)
{
	return 0;
}
