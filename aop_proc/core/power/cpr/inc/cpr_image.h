/**
 * @file:  cpr_image.h
 * @brief:
 *
 * Copyright (c) 2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 */

#ifndef CPR_IMAGE_H
#define CPR_IMAGE_H

#include "cpr_types.h"
#include "cpr_cfg.h"
#include "cpr_image_defs.h"

#ifndef CPR_IMAGE_LOCK_CORE
    #define CPR_IMAGE_LOCK_CORE(...)
    #define CPR_IMAGE_UNLOCK_CORE(...)
#endif

#define AOP_SMEM_WORKAROUND

#ifdef AOP_SMEM_WORKAROUND

#define SMEM_BASE_PHY                   0x86000000
#define SMEM_FIRST_FIXED_BUFFER_SIZE    0x40
#define SMEM_VERSION_INFO_SIZE          0x80
#define SMEM_HEAP_INFO_SIZE             0x10
#define SMEM_ALLOCATION_TABLE_SIZE      0x10

typedef struct
{
  uint32           allocated;
  uint32           offset;
  uint32           size;
  uint32           base_ext;
} cpr_image_smem_alloc_table_type;

typedef enum
{
    CPR_SMEM_CFG = 473,
} cpr_image_smem_cfg_types;
#endif

void cpr_image_enable_clock(const char* clkId);
void* cpr_image_malloc(uint32 size);
void cpr_image_free(void* buf);
void cpr_image_memscpy(void* dst, void* src, uint32 size);
void cpr_image_open_remote_cfg(void** cfg, const uint32 addr);
void cpr_image_close_remote_cfg(void);
uint32 cpr_image_get_smem_addr(cpr_image_smem_cfg_types cfg_type);
#endif
