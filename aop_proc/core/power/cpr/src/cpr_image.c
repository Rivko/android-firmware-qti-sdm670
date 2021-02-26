/*===========================================================================

Copyright (c) 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cpr_types.h"
#include "cpr_image.h"
#include "cpr_cfg.h"
#include "cpr_data.h"
#include "cpr.h"
#include "comdef.h"
#include "page_select.h"

/**
*
* <!-- cpr_image_malloc -->
*
*/
void* cpr_image_malloc(uint32 size)
{
    void* buf = calloc(1, size);

    CPR_ASSERT(buf);

    return buf;
}

/**
*
* <!-- cpr_image_free -->
*
*/
void cpr_image_free(void* buf)
{
    free(buf);
}

/**
*
* <!-- cpr_image_memscpy -->
*
*/
void cpr_image_memscpy(void* dst, void* src, uint32 size)
{
    //ToDo: Replace with 'memscpy(dst, size, src, size);'
    memcpy(dst, src, size);
}


/**
*
* <!-- cpr_image_open_remote_cfg -->
*
*/
void cpr_image_open_remote_cfg(void** cfg, const uint32 addr)
{
    *cfg = (void *)set_ddr_access((uint64_t) addr);
    CPR_ASSERT(*cfg);
}

/**
*
* <!-- cpr_image_close_remote_cfg -->
*
*/
void cpr_image_close_remote_cfg()
{
    set_page_select(0);
}

/**
*
* <!-- cpr_image_get_smem_addr -->
*
*/
uint32 cpr_image_get_smem_addr(cpr_image_smem_cfg_types cfg_type)
{
    uint32 cpr_addr = 0x0;
    uint32 table_addr = 0x0;
    cpr_image_smem_alloc_table_type *alloc_table = NULL;

    table_addr = SMEM_BASE_PHY + SMEM_FIRST_FIXED_BUFFER_SIZE +
        SMEM_VERSION_INFO_SIZE + SMEM_HEAP_INFO_SIZE;

    table_addr += cfg_type * sizeof(cpr_image_smem_alloc_table_type);

    cpr_image_open_remote_cfg((void **)&alloc_table, table_addr);

    if (alloc_table->allocated)
        cpr_addr = SMEM_BASE_PHY + alloc_table->offset;

    cpr_image_close_remote_cfg();

    return cpr_addr;
}

/**
*
* <!-- cpr_image_enable_clock -->
*
*/
void cpr_image_enable_clock(const char* clkId)
{
    //Not Supported in AOP
}
