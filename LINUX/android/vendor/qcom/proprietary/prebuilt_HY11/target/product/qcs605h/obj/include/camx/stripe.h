#ifndef _STRIPE_H_
#define _STRIPE_H_

//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

//------------------------------------------------------------------------
// @file  stripe.h
// @brief Algorithm IPE\BPS Lower-layer
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */
#ifndef NDEBUG
#include <stdio.h>
#endif /* !NDEBUG */

#include "list.h"
#include "stripe_common.h"
#include "scalers.h"
#include "modules.h"


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

#ifndef AEE_SUCCESS
#define AEE_SUCCESS 0
#endif //AEE_SUCCESS

#ifndef AEE_EFAIL
#define AEE_EFAIL 1
#endif //AEE_EFAIL

#ifndef WITH_DEBUG_INFO
#define WITH_DEBUG_INFO 0
#endif /* WITH_DEBUG_INFO */

#ifndef DUMP_DIM_INFO
#define DUMP_DIM_INFO 0
#endif /* DUMP_DIM_INFO */

#ifndef CHECK_FORMAT_CONSTRAINT
#define CHECK_FORMAT_CONSTRAINT 0
#endif /* CHECK_FORMAT_CONSTRAINT */

#ifndef STRIPING_LIBRARY_STRIPE_TOP_OUTPUT
/**< Possible values:
 *    0 - print to file only
 *    1 - print to terminal only
 *    2 - print to both terminal and file
 */
#define STRIPING_LIBRARY_STRIPE_TOP_OUTPUT   0
#endif /* STRIPING_LIBRARY_STRIPE_TOP_OUTPUT */

#if !((STRIPING_LIBRARY_STRIPE_TOP_OUTPUT == 0) || (STRIPING_LIBRARY_STRIPE_TOP_OUTPUT == 1) || (STRIPING_LIBRARY_STRIPE_TOP_OUTPUT == 2))
#error Invalid value for STRIPING_LIBRARY_STRIPE_TOP_OUTPUT
#endif /* STRIPING_LIBRARY_STRIPE_TOP_OUTPUT value check */

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

#define PROVIDE_DEFAULT_FIRMWARE_INPUT() 0


// performance modeling flag
#define calculate_BW
#ifndef PERF
#define PERF
#endif


#ifndef ROUND_UP
#define ROUND_UP(value, grid) ((((value) + (grid)-1)/(grid))*(grid))
#endif

#ifndef ROUND_DOWN
#define ROUND_DOWN(value, grid) (((value)/(grid))*(grid))
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(num, den) (((num) + (den)-1)/(den))
#endif


#ifndef EXTEND_START
#define EXTEND_START ROUND_DOWN
#endif

#ifndef SHRINK_START
#define SHRINK_START ROUND_UP
#endif


#ifndef EXTEND_END
#define EXTEND_END(value, grid) (ROUND_DOWN(value, grid) + (grid-1))
#endif

#ifndef SHRINK_END
#define SHRINK_END(value, grid) (ROUND_DOWN((value)+1, grid) - 1)
#endif

#ifndef MPIX_PADDING_END
#define MPIX_PADDING_END(start, end, grid) ((start) + ROUND_UP(LENGTH((start), (end)), grid) - 1)
#endif




// scaling
#define CDS       2
#define CDS_SHIFT 1
#define QRT_SHIFT 2


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
    int32_t xStart;        // in pixels
    int32_t xEnd;
    int32_t yStart;        // in lines
    int32_t yEnd;
    int32_t frameStride;   // in #of bytes
    int16_t frameFormat;
    int16_t dummy_field_for_byte_padding;
}
StripeCoordinates;

typedef struct
{
    BOOL enable1;               //
    BOOL enable2;               // for 2nd plane if format is 2-plane format

    int32_t blockType1;         // 0: single plane;  1: cbcr;  (used for both non-ICA and ICA fetch)
    int32_t unpackMode1;        // non_ICA fetch only!!!
    // Bayer:  MIPI;
    // PDI: bypass 128bit;
    // TFI: Plain8
    BOOL    externAddrMode1;    // using ICA to fetch:   UBWC_TP10, UBWC_NV12, PD, PY
    // when ICA not in use:   Bayer, PDI, TFI
    int32_t externImageFormat1; //  ICA  fetch only !!!
    //  PY8:0;  PY10:1;   PD8: 2;  PD10: 3;
    // linear_NV12:4;  linear_TP10:5;   UBWC_NV12:6;   UBWC_TP10:7
    int32_t imageStride1;      // in bytes

    int32_t blockType2;             // 0: single plane;  1: cbcr;  (used for both non-ICA and ICA fetch)
    int32_t unpackMode2;            // non_ICA fetch only!!!
    // Bayer:  MIPI;
    // PDI: plain64 bit;
    // TFI: Plain8
    BOOL     externAddrMode2;       // using ICA to fetch:   UBWC_TP10, UBWC_NV12, PD, PY
    // when ICA not in use:   Bayer, PDI, TFI
    int32_t  externImageFormat2;    //  ICA  fetch only !!!
    //  PY8:0;  PY10:1;   PD8: 2;  PD10: 3;
    // linear_NV12:4;  linear_TP10:5;   UBWC_NV12:6;   UBWC_TP10:7
    int32_t  imageStride2;    // in bytes
}
FEPassParams;

typedef struct
{
    int32_t  bufOffset1;            // in bytes
    int32_t  stripeWidth1;          // before padding,  in number of bytes
    int32_t  stripeHeight1;         // before padding
    int32_t  stripeBlockWidth1;     // after padding;
    int32_t  stripeBlockHeight1;    // after padding;

    int32_t  bufOffset2;
    int32_t  stripeWidth2;          // before padding,   in number of bytes
    int32_t  stripeHeight2;         // before padding
    int32_t  stripeBlockWidth2;     // after padding;
    int32_t  stripeBlockHeight2;    // after padding;

    BOOL containsLeft;                      // is contain left-most column of the image
    BOOL containsRight;                     // is contain right-most column of the image
}
FEStripeParams;

typedef struct
{
    int32_t  hInit1;        // non-ubwc,   in #of bytes
    int32_t  vInit1;        // effective line number if PDI/PD
    int32_t  UBWCHInit1;    // ubwc,   in pixels
    int32_t  UBWCVInit1;    // ubwc,  in lines
    int32_t  stripeWidth1;  // stripe width in number of bytes.   non ubwc mode only
    int32_t  stripeHeight1;
    int32_t  hStep1;        // h_step=block_width; (stripe_width)    for both ubwc & non_ubwc case
    int32_t  vStep1;       // height

    int32_t  tilesPerRow1;       //
    int32_t  tilesPerCol1;
    int32_t  UBWCLeftPartialTile1;
    int32_t  UBWCRightPartialTile1;

    int32_t  hInit2;
    int32_t  vInit2;
    int32_t  UBWCHInit2;   // ubwc,  in pixels
    int32_t  UBWCVInit2;   // ubwc,  if cbcr,  y/2.

    int32_t  stripeWidth2;          // stripe width in number of bytes.
    int32_t  stripeHeight2;         // same as Y height,  h/w will divide by half automatically.
    int32_t  hStep2;                // h_step=block_width; (stripe_width)    for both ubwc & non_ubwc case
    int32_t  vStep2;
    int32_t  tilesPerRow2;          // same as Y
    int32_t  tilesPerCol2;          // Y /2
    int32_t  UBWCLeftPartialTile2;
    int32_t  UBWCRightPartialTile2;
}
WEStripeParams;

typedef struct
{
    BOOL enable1;       //  Y plane or single plane
    BOOL enable2;       // for 2nd plane if format is 2-plane format
    BOOL UBWCFormat;    // applicable for both planes
    int32_t AMSBC_mode; //0(IFE out), 1(TF out), 2(Video out)

    int32_t  bufStride1;    // if UBWC, then per tile row,  in non-ubwc,
    int32_t  UBWCMode1;
    //int32_t  frame_stride_1;

    int32_t  bufStride2;
    int32_t  UBWCMode2;     // 0=PCM,  1=NV12Y, 2=NV12UV,  3=TP10Y,  4=TP10UV
    //int32_t  frame_stride_2;

    int32_t  packerMode1;     // bypass mode for all except TFI ( plain8)
    int32_t  packerMode2;     // bypass mode for all except TFI ( plain8)
    int32_t  blocksPerRow1;   // fixed to be 1,  for both ubwc & non-ubwc mode
    int32_t  blocksPerCol1;   // fixed to be 1,  for both ubwc & non-ubwc mode
    int32_t  blocksPerRow2;   // fixed to be 1,  for both ubwc & non-ubwc mode
    int32_t  blocksPerCol2;   // fixed to be 1,  for both ubwc & non-ubwc mode
}
WEPassParams;


#pragma pack(pop)

#define resetCrop1D(pCrop)          memset(pCrop, 0, sizeof(*pCrop))

/*------------------------------------------------------------------------
*       External Variables
* ----------------------------------------------------------------------- */
extern const int q_factor;
extern const int q_mask;

/*------------------------------------------------------------------------
*       Function Declarations
* ----------------------------------------------------------------------- */

void alignOutData(int16_t * pStart, int16_t * pEnd, const int16_t constraint, const int16_t cummulativeWidth);

int16_t
limitLeft2(int16_t var1, int16_t var2);

int16_t
limitLeft3(int16_t var1, int16_t var2, int16_t var3);

int16_t
limitLeft4(int16_t var1, int16_t var2, int16_t var3, int16_t var4);

int16_t
limitRight2(int16_t var1, int16_t var2);

int16_t
limitRight3(int16_t var1, int16_t var2, int16_t var3);

int16_t
limitRight4(int16_t var1, int16_t var2, int16_t var3, int16_t var4);

void
clampToBuffer(int16_t * pStart, int16_t * pEnd, int16_t bufLength);

void clampToFrame(int16_t *pStart, int16_t *pEnd, int16_t frameSize);           //: to make the backward process more precise

#if 0  // TODO : remove dead code
void
applyOffsetBack(int16_t * pStart, int16_t * pEnd, int32_t offsetSize);
#endif /* 0 */

void applyOffsetForward(int16_t * pStart, int16_t * pEnd, const int32_t offsetSize);

void
applyKernelBack(int16_t * pStart, int16_t * pEnd, int16_t kernelSize);


void
scaleBack(int16_t * pStart, int16_t * pEnd, int32_t R, int16_t kernelSize);


void scaleBack_V20(int16_t * pStart,
                   int16_t * pEnd,
                   int32_t R,
                   const MNScaleDownInStruct_V20_1D * pFramecfg);

void scaleForward_V20(int16_t * pStart,
                 int16_t * pEnd,
                 int32_t R,
    const MNScaleDownInStruct_V20_1D * pFramecfg,
                 MNScaleDownInStruct_V20_1D * pStripecfg);


int16_t getFormatConstraint_MALaligned(int16_t format, BOOL horizontal);

int16_t genFormatMinOutputSize(int16_t format, BOOL horizontal);



void chroma_upsample_config(int input_l, int output_l,
    const BOOL is_horizontal, const BOOL cosited, const BOOL cosited_even, const UpscaleInStruct_V11_1D* frameCfg, ChromaUpv201dStripingOutputParams * pStripecfg);
void chromaUpScaleBack( int16_t *start, int16_t* end, const BOOL cosited, const BOOL cosited_even );
void adjustChromaUpForPostCropKernel
    (int *cropL_444, int *cropR_444, const BOOL edgeStripeLT, const BOOL edgeStripeRB, UpscaleInStruct_V11_1D * pStripecfg);
void chroma_upsample_Y_config(int input_l, int output_l,
    UpscaleInStruct_V11_1D* frameCfg, UpscaleInStruct_V11_1D * pStripecfg);

int32_t
    LTMDCStripeConfiguraion(
    const uint16_t xStartIdxDCframe,   // this is the dimension after down scaler
    const uint16_t xEndIdxDCframe,
    const uint16_t output_l,
    const BOOL firstStripe,
    const uint16_t ltmDCtableOption,
    LTMV13StripeDCParams* ltmDCStripeParams
    ) ;

int32_t
    LTMIPStripeConfiguraion(
        uint16_t  ip_StartIdxframe,      // reference to full frame
        BOOL  is_horizontal,             //  0: vertical  1: horizontal
        uint16_t  fullFrameSize,         // size on the same scale as 1:1 full frame,  both non-zoom & zoom cases
        LTMV13StripeIPParams_1D* ltmDCStripeParams
    ) ;


int generateLTMDCmode(const int widthin, const int heightin );
int generateLTMDCoutW( const int ds_fac );
int generateLTMDCoutH( const int ds_fac );
int genOffsetFrom2Outputs(const int offset1, const BOOL en1, const int offset2, const BOOL en2);
int16_t IsRgbFormat(const int16_t format);
int16_t IsBayerFormat(const int16_t format);


#ifdef __cplusplus
}
#endif

#endif /* _STRIPE_H_ */
