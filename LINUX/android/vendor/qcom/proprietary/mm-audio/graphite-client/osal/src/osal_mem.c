/*
 * osal_mem.c
 *
 * This file contains memory operations support for POSIX.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <string.h>

void *osal_mem_alloc(size_t size)
{
    return malloc(size);
}

void *osal_mem_zalloc(size_t size)
{
    void *ptr;

    ptr = malloc(size);
    if (ptr != NULL)
        memset(ptr, 0, size);

    return ptr;
}

void *osal_mem_calloc(size_t items, size_t size)
{
    return calloc(items, size);
}

void *osal_mem_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void osal_mem_free(void *ptr)
{
    free(ptr);
}

void *osal_mem_cpy(void *dst, size_t dst_size __unused,
                   void *src, size_t src_size)
{
    return memcpy(dst, (const void *)src, src_size);
}

void *osal_mem_set(void *ptr, int val, size_t size)
{
    return memset(ptr, val, size);
}
