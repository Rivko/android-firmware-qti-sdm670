#ifndef __MMSTD_H__
#define __MMSTD_H__

/*
  Copyright (C) 2011-2012, 2013 QUALCOMM Technologies, Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //components/rel/avs.adsp/2.8.5/api/mmutils/mmstd.h#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

#define MMSTD_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

#define MMSTD_MAX( a, b ) ( ( a ) < ( b ) ? ( b ) : ( a ) )

/****************************************************************************
 * EXPORT MACROS                                                            *
 ****************************************************************************/

#ifdef __cplusplus
  #define MMSTD_INTERNAL extern "C"
#else
  #define MMSTD_INTERNAL extern
#endif /* __cplusplus */
  /**< This marks an internal API that is not intended for public use. */

#ifdef __cplusplus
  #define MMSTD_EXTERNAL extern "C"
#else
  #define MMSTD_EXTERNAL extern
#endif /* __cplusplus */
  /**< This marks an external API that is intended for public use. */

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

MMSTD_EXTERNAL uint32_t mmstd_strnlen (
  const char_t* src,
  uint32_t size
);

MMSTD_EXTERNAL int32_t mmstd_strncmp (
  const char* s1,
  uint32_t s1_size,
  const char* s2,
  uint32_t s2_size
);

MMSTD_EXTERNAL uint32_t mmstd_strlcpy (
  char_t* dst,
  const char_t* src,
  uint32_t size
);

MMSTD_EXTERNAL void* mmstd_memcpy (
  void* dst,
  uint32_t dst_size,
  const void* src,
  uint32_t src_size
);

MMSTD_EXTERNAL void* mmstd_memset (
  void* dst,
  int32_t c,
  uint32_t size
);

MMSTD_EXTERNAL int32_t mmstd_snprintf (
  char_t* dst,
  uint32_t size,
  const char_t* format,
  ...
);

#endif /* __MMSTD_H__ */

