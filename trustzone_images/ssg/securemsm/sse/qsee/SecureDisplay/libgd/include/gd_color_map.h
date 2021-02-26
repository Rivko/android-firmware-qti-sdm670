#ifndef GD_COLOR_MAP_H
#define GD_COLOR_MAP_H

/*===========================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.       
   All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/sse/qsee/SecureDisplay/libgd/include/gd_color_map.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/02/13   ts       Initial version.

===========================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *color_name;
	int red;
	int green;
	int blue;
} gdColorMapEntry;

typedef struct {
	int num_entries;
	gdColorMapEntry *entries;
} gdColorMap;

extern BGD_EXPORT_DATA_PROT gdColorMap GD_COLOR_MAP_X11;

BGD_DECLARE(int) gdColorMapLookup(const gdColorMap color_map, const char *color_name, int *r, int *g, int *b);

#ifdef __cplusplus
}
#endif

#endif
