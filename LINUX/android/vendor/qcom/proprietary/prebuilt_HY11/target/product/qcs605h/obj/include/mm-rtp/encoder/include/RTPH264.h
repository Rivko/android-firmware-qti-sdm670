#ifndef _RTPH264_H
#define _RTPH264_H
/* =======================================================================
   RTPH264.h
   DESCRIPTION
   H.264 specific definitions for RTP


   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */


/* =======================================================================
Edit History

$Header: $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/23/17   jz      Add H.264 support in RTP encoder

========================================================================== */




/* =======================================================================
 **               Includes and Public Data Declarations
 ** ======================================================================= */

/* ==========================================================================

   INCLUDE FILES FOR MODULE

   ========================================================================== */
#define H264_NAL_MAX_SIZE 1920 * 1080

typedef struct FuIndicator
{
    uint8 type:   5;
    uint8 nri:    2;
    uint8 f:      1;
} __attribute__ ((packed)) FuIndicatorT;

typedef enum RTPH264_MODE
{
    SINGLE_NAL_MODE,
    FU_A_MODE
} RTPH264_MODE;

typedef struct NaluHeader
{
    uint8 type:   5;
    uint8 nri:    2;
    uint8 f:      1;
} __attribute__ ((packed)) NaluHeaderT;

typedef struct FuHeader
{
    uint8 type:   5;
    uint8 r:      1;
    uint8 e:      1;
    uint8 s:      1;
} __attribute__ ((packed)) FuHeaderT;
#endif
