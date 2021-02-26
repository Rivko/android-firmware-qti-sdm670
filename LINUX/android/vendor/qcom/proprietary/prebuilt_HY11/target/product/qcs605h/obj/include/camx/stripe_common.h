#ifndef _STRIPE_COMMON_H_
#define _STRIPE_COMMON_H_

//-------------------------------------------------------------------------
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

//------------------------------------------------------------------------
// @file  stripe_common.h
// @brief Common definitions between IFE and IPE/BPS striping
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */
#ifndef NDEBUG
#include <stdio.h>
#endif /* !NDEBUG */


//Break or Clean up dependencies on FW interface
#include "icpcomdef.h"
#include "ipdefs.h"


#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       Definitions and Constants
* ----------------------------------------------------------------------- */

#if defined(_MSC_VER)
#define WHILE_0        while(0,0)
#else
#define WHILE_0        while(0)
#endif /*  */

#if defined(_MSC_VER)
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(expr,...)      do{} WHILE_0
#else
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(expr,...)      do{} WHILE_0
#endif /* !_MSC_VER */

#ifndef NDEBUG
#define CSADBG_ERROR(FMT, ...) printf( "\n Camera ICP : %s : " FMT, __FUNCTION__, __VA_ARGS__ )
#define CSADBG_HIGH(FMT, ...) printf( "\n Camera ICP : %s : " FMT, __FUNCTION__, __VA_ARGS__ )
#else
#define CSADBG_ERROR(FMT, ...)
#define CSADBG_HIGH(FMT, ...)
#endif /* !NDEBUG */

#if (WITH_DEBUG_INFO == 1)
#define DEBUG_INFO(...) do {printf("%s: ", __FUNCTION__);printf(__VA_ARGS__);fflush(stdout);} WHILE_0
#define DEBUG_INFO_IF(expr,...)             \
    do {                                    \
        if ((expr))                         \
        {                                   \
            DEBUG_INFO(__VA_ARGS__);        \
        }                                   \
    } WHILE_0
#define SADBG
#else
#if defined(_MSC_VER)
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(expr,...)      do{} WHILE_0
#else
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(expr,...)      do{} WHILE_0
#endif /* !_MSC_VER */
#endif

#define BOOL                  int32_t

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef LENGTH
#define  LENGTH( start, end ) ((end) - (start) + 1)
#endif

// scaling
#define CDS       2
#define CDS_SHIFT 1
#define QRT_SHIFT 2

#define Q_FACTOR  21
#define MAX_SCALE 64 << Q_FACTOR
#define MIN_SCALE (1 << Q_FACTOR) / 64

// TODO : Remove below definitions and replace by enums from ImageFormat
#define     IMAGE_FORMAT_ICA_MASTER_OUT         (IMAGE_FORMAT_MAX + 1)
#define     IMAGE_FORMAT_YUV444                 (IMAGE_FORMAT_ICA_MASTER_OUT + 1)       // Same as LTMDC_DUMMY_FORMAT


#define YUV420_UBWC_NV12_MINWIDTH            130
#define YUV420_UBWC_NV12_4R_MINWIDTH         130
#define YUV420_UBWC_TP10_MINWIDTH            98

#define YUV420_UBWC_NV12_CONSTRAINT_H        2
#define YUV420_UBWC_NV12_4R_CONSTRAINT_H     2
#define YUV420_UBWC_TP10_CONSTRAINT_H        6
#define PDI_CONSTRAINT_H                     10
#define YUV420_PD10_CONSTRAINT_H_READ        4
#define YUV420_PD10_CONSTRAINT_H_WRITE       8
#define TFI_CONSTRAINT_H                     2
#define BAYER_PLAIN_CONSTRAINT_H             2
#define LTMDC_DUMMY_CONSTRAINT_H             1 // TODO : not sure what is the correct symbol: YUV444 or LTMDC_DUMMY
#define YUV420_LINEAR_NV12_CONSTRAINT_H      2
#define BAYER_MIPI10_CONSTRAINT_H            4
#define BAYER_MIPI12_CONSTRAINT_H            2
#define BAYER_MIPI14_CONSTRAINT_H            4
#define YUV420_LINEAR_P010_CONSTRAINT_H      2
#define YUV420_LINEAR_TP10_CONSTRAINT_H      6
#define YUV420_PLAIN_CONSTRAINT_H            2
#define YUV422_PLAIN16_CONSTRAINT_H          2
#define YUV420_UBWC_P010_CONSTRAINT_H        2
#define YUV420_ICA_MASTER_OUT_CONSTRAINT_H   2
#define YUV422_PLAIN8_CONSTRAINT_H           2

#define YUV420_UBWC_NV12_CONSTRAINT_V        2
#define YUV420_UBWC_NV12_4R_CONSTRAINT_V     2
#define YUV420_UBWC_TP10_CONSTRAINT_V        2
#define PDI_CONSTRAINT_V                     2
#define YUV420_PD10_CONSTRAINT_V             2
#define TFI_CONSTRAINT_V                     1
#define BAYER_PLAIN_CONSTRAINT_V             2
#define LTMDC_DUMMY_CONSTRAINT_V             1 // TODO : not sure what is the correct symbol: YUV444 or LTMDC_DUMMY
#define YUV420_LINEAR_NV12_CONSTRAINT_V      2
#define BAYER_MIPI10_CONSTRAINT_V            2
#define BAYER_MIPI12_CONSTRAINT_V            2
#define BAYER_MIPI14_CONSTRAINT_V            2
#define YUV420_LINEAR_P010_CONSTRAINT_V      2
#define YUV420_LINEAR_TP10_CONSTRAINT_V      2
#define YUV420_PLAIN_CONSTRAINT_V            2
#define YUV422_PLAIN16_CONSTRAINT_V          2
#define YUV420_UBWC_P010_CONSTRAINT_V        2
#define YUV420_ICA_MASTER_OUT_CONSTRAINT_V   2
#define YUV422_PLAIN8_CONSTRAINT_V           2
/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(4)

typedef struct
{
    int16_t enable;
    int16_t inDim;      // crop module input dimention
    int16_t firstOut;   // first output pixel (fill) - in relation to input
    int16_t lastOut;    // last output pixel (flush) - in relation to input
}
crop_1D;

#pragma pack(pop)

void
applyKernelForward(int16_t * pStart, int16_t * pEnd, int16_t kernelSize, BOOL edgeStripeLT, BOOL edgeStripeRB);

int16_t getFormatConstraint(int16_t format, BOOL horizontal, BOOL read);

void
scaleForward8Tap(int16_t * pStart,
                 int16_t * pEnd,
                 int16_t chromaStart,
                 int16_t chromaEnd,
                 int32_t R,
                 int32_t offset,
                 int16_t kernelSize,
                 BOOL edgeStripeLT,
                 BOOL edgeStripeRB,
                 crop_1D * cropLuma,
                 crop_1D * cropChroma);
int scaleBack_start_DS4(const int16_t outStart, int16_t kernelSize);
int scaleBack_end_DS4(const int16_t outEnd, int16_t kernelSize);


int32_t generate_r_square_init_abfv34( const int16_t xStart, const int16_t bx, const int16_t by );
int generate_r_square_shift(const int imageW, const int imageH );
int generate_r_square_scale(const int imageW, const int imageH, const int r_square_shft );


#ifdef __cplusplus
}
#endif

#endif /* _STRIPE_COMMON_H_ */
