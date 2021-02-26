/*===========================================================================
              Unified Image Encryption (UIE) Environment Interface

GENERAL DESCRIPTION
  Definitions and types for UIE (document 80-NP914-1 Rev A) on the SBL.

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
#ifndef _UIE_ENV_H
#define _UIE_ENV_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "boot_extern_dal_interface.h" // DALSYS_Malloc, DALSYS_Free
#include <Library/BaseMemoryLib.h>           // memscpy
#include "secboot_hw.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/
#define UIE_MALLOC  uie_sbl_malloc
#define UIE_FREE    DALSYS_Free
#define UIE_MEMSCPY memscpy

#define UIE_LOG_MSG(fmt, ...)
#define UIE_LOG_HEX(label, buf, buflen)

size_t inline memscpy( void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;
  CopyMem(dst, src, copy_size);
  return copy_size;
}

static inline void *uie_sbl_malloc(uint32 num_bytes)
{
  void *address = NULL;
  DALSYS_Malloc(num_bytes, &address);
  return address;
}

/**
 * @brief Map the device memory into the MMU for access
 *
 * @return - 0 on success; otherwise error
 */
static inline int uie_map_fuse_area(uint32 addr, uint32 len)
{
  return 0;
}

/**
 * @brief Unmap the device memory into the MMU for access
 *
 * @return - 0 on success; otherwise error
 */
static inline void uie_unmap_fuse_area(uint32 addr, uint32 len)
{
  return;
}

/**
 * @brief return code segment
 *
 * @return - code segment
 */
static inline uint32 uie_get_secboot_hw_code_segment(void)
{
  return SECBOOT_HW_APPS_CODE_SEGMENT;
}

#endif
