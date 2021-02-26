/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#ifndef CELLINFO_TOOLS_H_INCLUDED
#define CELLINFO_TOOLS_H_INCLUDED

#include <chre_api/chre/wwan.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*cellinfo_dumpfn)(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

void cellinfo_dump_event(const struct chreWwanCellInfoResult *evt, cellinfo_dumpfn dumpfn);
struct chreWwanCellInfoResult * cellinfo_alloc_event(size_t ncells);
void cellinfo_free_event(const struct chreWwanCellInfoResult *evt);

#ifdef __cplusplus
}
#endif

#endif
