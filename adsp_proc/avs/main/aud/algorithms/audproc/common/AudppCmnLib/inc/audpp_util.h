/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2008, 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audpp_util.h                      TYPE: C-header file              *]
[* DESCRIPTION: Header file for various utility functions for post processor.*]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   03/30/06   xyp     Initial revision                                     *]
[*****************************************************************************/
#ifndef _AUDPP_UTIL_H_
#define _AUDPP_UTIL_H_

#include "audpp_mathlib.h"
#include "audpp_common.h"

#define Q13_UNITY 0x2000

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================
      Function Declarations 
=============================================================================*/
int16 find_freq
(
    int32           freqHz,                 /* input reference frequency     */
    int32  const   *designFreqArray,        /* available design freq array   */
    int16           arraySize               /* size of the design freq array */
);

int16 find_exact_freq
(
    int32           freqHz,                 /* input reference frequency     */
    int32  const   *designFreqArray,        /* available design freq array   */
    int16           arraySize               /* size of the design freq array */
);

int32 ms_to_sample
(
    int16           ms,                     /* value in millisecond          */
    uint32          sampleRate              /* samping rate                  */
);

int16 time_to_sample
(
    int16           time_100ns,             /* value in 100 ns               */
    uint32          sampleRate              /* sampling rate                 */
);

/*===========================================================================*/
/* FUNCTION : align_to_8_byte                                                */
/*                                                                           */
/* DESCRIPTION: Ceil to the next multiple of 8                               */
/*                                                                           */
/* INPUTS: num: Any number                                                   */
/* OUTPUTS:                                                                  */
/*                                                                           */
/* IMPLEMENTATION NOTES:                                                     */
/*===========================================================================*/
static inline uint32_t align_to_8_byte(const uint32_t num)
{
   return ((num + 7) & (0xFFFFFFF8));
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UTIL_H_ */
