#ifndef CHIPINFOIMAGE_H
#define CHIPINFOIMAGE_H

/*
 ===============================================================================

 FILE: ChipInfoImage.h

 DESCRIPTION: Internal ChipInfo header with common APIs to acc

 ===============================================================================

 Copyright (c) 2017 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary and Confidential

 ===============================================================================

 $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoImage.h#1 $
 $DateTime: 2017/09/18 09:26:13 $
 $Author: pwbldsvc $

 =============================================================================*/


/*=============================================================================
  Include Files
  ============================================================================*/
#include "PlatformInfoDefs.h"
#include "com_dtypes.h"

/*=============================================================================
  Macros
  =============================================================================*/


/*=============================================================================
  Type definitions
  =============================================================================*/


/*=============================================================================
  Data definitions
  =============================================================================*/


/*=============================================================================
  Interface declarations
  =============================================================================*/
/*
 * Common way to access the socinfo SMEM region,
 * since not all images have an SMEM driver for accessing socinfo.
 *
 * @return  A pointer to the socinfo structure if successful,
 *          NULL if not successful
 */
DalPlatformInfoSMemType *
ChipInfo_GetSocInfo
(
  void
);

/*
 * Unmap the SMEM region once it's no longer needed.
 */
void
ChipInfo_UnmapSMem
(
  void
);

/*
 * Common way to dynamically allocate memory on all images, since
 * different images might use different memory allocation functions.
 *
 * @param[in] nSize  The size in bytes to allocate
 *
 * @return  Pointer to allocated memory, if successful
 *          NULL if not successful.
 */
void *
ChipInfo_malloc
(
  uint32 nSize
);

/*
 * Common way to copy memory on all images, since not all images
 * use the same safe-memcpy function.
 *
 * This is a wrapper around the image's safe memcpy, and doesn't
 * provide any additional functionality other than some minor
 * input validation.
 *
 * @param[in,out] pDest The destination buffer
 * @param[in] nDestLen  The size of the destination buffer
 * @param[in] pSrc      The source buffer
 * @param[in] nSrcLen   The number of bytes to copy from pSrc into pDest
 *
 * @return  A pointer to the destination buffer, if successful
 *          NULL if any of the inputs is NULL or 0, or if the
 *            safe memcpy indicates an error
 */
void *
ChipInfo_memcpy
(
  void *restrict pDest,
  uint32 nDestLen,
  const void *restrict pSrc,
  uint32 nSrcLen
);

/*
 * Common way to copy strings on all images, since different images
 * use different versions of a safe string copy. This function is a
 * wrapper around the image's safe string copy, and doesn't
 * implement anything extra other than some minor input validation.
 *
 * @param[in,out] szDest    The destination buffer
 * @param[in]     szSrc     The NULL-terminated source buffer
 * @param[in]     mDestLen  The size of the destination buffer.
 *                          At most nDestLen-1 characters will
 *                          be copied into szDest, and the buffer
 *                          will be NULL-terminated.
 *
 * @return  A pointer to the (possibly truncated) destination buffer,
 *            if successful
 *          NULL if one of the inputs is NULL or 0
 */
char *
ChipInfo_strcpy
(
  char *restrict szDest,
  const char *restrict szSrc,
  uint32 nDestLen
);

/*=============================================================================
  Interface definitions
  =============================================================================*/


#endif /* CHIPINFOIMAGE_H */

