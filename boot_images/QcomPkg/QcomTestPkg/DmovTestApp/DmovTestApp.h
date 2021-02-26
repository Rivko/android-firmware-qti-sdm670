/** @file DmovTestApp.h 
   
  This file contains definitions for Dmov test application

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 5/31/11    rl      Created.

=============================================================================*/
#ifndef __DMOV_TEST_APP_H__
#define __DMOV_TEST_APP_H__

#define SLAB_SIZE        0x1000      /*<-- chunk size to allocate */
#define DMOV_BUFFER_SIZE 0x40
#define UEFI_CHANNEL     3
#define PREFIX  "[DMOV_TEST] "
#define CPLE_SIZE  4                 /*<-- command pointer list entry size */

#define DMOV_TEST_ADM0 0
#define DMOV_TEST_ADM1 1
/* 
 * Structure to keep track of all the memory handles 
 */
typedef struct 
{
    UINT8 pCmdPtr[4];
    UINT8 reserved0[4];
    UINT8 pCmd[16];
    UINT8 pSourceBuffer[DMOV_BUFFER_SIZE];
    UINT8 pDestBuffer[DMOV_BUFFER_SIZE];
} DmovTestMemoryHandle;

/**
  Validate data to make sure a transfer happened by comparing source and
  desitation buffers.

  @param[in] s Pointer to source buffer
  @param[in] d Pointer to destination buffer

  @return      source buffers match TRUE
               source buffers mismatch FALSE 
**/
UINT32 IsDataValid(UINT8* s, UINT8* d);

/**
  Populate a source buffer with random numbers  

  @param[in] p pointer to source buffer

**/
VOID RandomizeSrcBuffers(UINT8* p);
#endif
