#pragma once
/*=============================================================================
  @file sns_cstruct_extn.c

  Extend a C structure

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include "sns_rc.h"

/**
 * This utility provides a mechanism for appending multiple
 * buffers to the end of a C structure.
 *
 * For example, consider a two C structures:
 * struct A
 * {
 *   int32_t x;
 *   int32_t y;
 *   sns_cstruct_extn extn;
 * }
 *
 * struct B
 * {
 *   char buffer[];
 * }
 *
 * Multiple instances of struct B can be appended to the end of
 * struct A. Here is an example to append 2 instances of B:
 *
 * size_t alloc_size = sns_cstruct_extn_compute_total_size(
 *    sizeof(struct A), 2, 2 * ALIGN_8(buffer1_size) +
 *    ALIGN_8(buffer2_size));
 *
 * struct A *example = malloc(alloc_size);
 * sns_cstruct_extn_init(&example->extn, example,
 *   sizeof(*example), 2);
 *
 * uint32_t b1_idx =
 *   sns_cstruct_extn_setup_buffer(&example->extn,
 *   buffer1_size);
 * uint32_t b2_idx =
 *   sns_cstruct_extn_setup_buffer(&example->extn,
 *   buffer2_size);
 *
 * void *b1 =  sns_cstruct_extn_get_buffer(&example->extn,
 *   b1_idx);
 * void *b2 =  sns_cstruct_extn_get_buffer(&example->extn,
 *   b2_idx);
 */

/* The C structure extender */
typedef struct
{
  /* Pointer to the original structure */
  void *original_struct;
  /* Size of the original structure */
  size_t original_size;
  /* Total size of all the buffers, metadata
     and the original structure */
  size_t total_size;
  /* Number of buffers appended to the end */
  uint8_t cnt;
}sns_cstruct_extn;

/**
 * Compute the total size of the C structure and all the
 * buffers
 *
 * @param[i] original_size    Size of the original C structure
 * @param[i] cnt              Number of buffers being appended
 * @param[i] sum_buffer_sizes The sum of the buffer sizes for
 *       all the buffers and the extra padding bytes added to
 *       each buffer for allignment
 *
 * @return combined size
 */
size_t sns_cstruct_extn_compute_total_size(size_t original_size, uint8_t cnt, size_t sum_buffer_sizes);

/**
 * Initialize the C structure extentions
 *
 * @param[i] this                 The C structure extender ptr
 * @param[i] original_struct      Ptr to the original structure
 * @param[i] original_struct_size Size of original struct
 * @param[i] cnt                  Number of buffers being
 *                                  appended
 *
 * @return SNS_RC_SUCCESS initialzation successful
 *         SNS_RC_FAILED  otherwise
 */
sns_rc sns_cstruct_extn_init(sns_cstruct_extn *this, void *original_struct,
  size_t original_struct_size, uint8_t cnt);

/**
 * Set up each buffer
 *
 * @param[i] this           The C structure extender ptr
 * @param[i] size           Size of the buffer being added
 *
 * @return
 *  UINT8_MAX  if buffer not setup correctly Index of buffer to
 *  be used with sns_cstruct_extn_get_buffer
 *    otherwise
 */
uint8_t sns_cstruct_extn_setup_buffer(sns_cstruct_extn *this, size_t size);

/**
 * Get an appended buffer from its index
 *
 * @param[i] this            The C structure extender ptr
 * @param[i] idx             Index of the buffer obtained from
 *                             sns_cstruct_extn_setup_buffer
 *
 * @return Ptr to the appended buffer if successful
 *         NULL otherwise
 */
void * sns_cstruct_extn_get_buffer(sns_cstruct_extn *this, uint8_t idx);

