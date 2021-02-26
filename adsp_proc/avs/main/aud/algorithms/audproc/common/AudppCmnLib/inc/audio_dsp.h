/*======================= COPYRIGHT NOTICE ==================================*
Copyright (c) 2007-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audio_dsp.h                 TYPE: C-header file              *]
[* DESCRIPTION: Header file for audio dsp and utilities.                     *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   02/21/06   xyp     Initial revision                                     *]
[*****************************************************************************/
#ifndef _AUDIO_DSP_H_
#define _AUDIO_DSP_H_

#include "AudioComdef.h"
#include "audpp_mathlib.h"
#include "audpp_converter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================
      Constants 
=============================================================================*/
#define MAX_BIQUAD_STAGES   4
#define MAX_FIR_TAPS       160

#if ((defined __hexagon__) || (defined __qdsp6__))
#define MAX_FIR_BLOCK_SIZE 512
#endif

/*=============================================================================
      Typedefs 
=============================================================================*/
/*----------------------------- panner struct -------------------------------*/


typedef struct _pannerStruct
{
    int16       targetGainL16Q15;       /* target gain to pan toward, L16Q15 */
    int32       deltaL32Q31;            /* gain increment per sample, L32Q31 */
    int32       sampleCounter;          /* total sample steps of the panner  */
    int32       delaySamples;           /* delay of samples to start panning */
} pannerStruct;

/*------------------------- angle panner struct -----------------------------*/
typedef struct _anglePannerStruct
{
    /* angle: integer part : 0~32, corresponding to 0~180 degrees            */
    /*       fractinoal part: UL16                                           */
    int32       targetAngleL32Q16;      /* target angle to pan toward, L32Q16*/
    int32       deltaL32Q24;            /* gain increment per sample, L32Q24 */
    int32       sampleCounter;          /* total sample steps of the panner  */
} anglePannerStruct;

/*--------------------------- one pole filter struct ------------------------*/
typedef struct _simpleLPFilt
{ 
    int16       c0L16Q14;               /* coefficient c0, L16Q14            */ 
    int16       c1MinusOneL16Q14;       /* coefficient c1-1, L16Q14          */
    int32       yL32;                   /* feedback memory, y(n-1), L32      */

/*                c0                                                         */
/*    H(z) =  ----------                                                     */
/*            1-c1*z**-1                                                     */
/*  filter difference equation:                                              */
/*    y(n) = c0*x(n) + c1*y(n-1)                                             */
/*         = c0*x(n) +( c1-1)*y(n-1) + y(n-1)                                */
/*  set one coeff as (c1-1) and add 32 bit version of y(n-1) to              */
/*  prevent accumulation of round-off errors                                 */
} simpleLPFilt;

/*----------------------------- delayline struct ----------------------------*/
typedef struct _delaylineStruct
{
    int32       delayIndex;             /* delayline index                   */
    int32       delayLength;            /* delayline length                  */
    int16      *delayBuf;               /* pointer to delayline buffer       */
} delaylineStruct; 

/*--------------------------- variable delay struct -------------------------*/
typedef struct _variableDelayStruct
{
    delaylineStruct     delayline;      /* delayline of variable delay       */
    bufRateCvtrStruct   converter;      /* buffer rate converter             */
    int32               currentDelay;   /* current delay amount, <= 32767    */
} variableDelayStruct;

/*---------------------------- biquad filter struct -------------------------*/

#if ((defined __hexagon__) || (defined __qdsp6__))

typedef struct _biquadFilter
{
    int16               coeffsL16Q13[4 + 4];           /*  a1, a2, -b1, -b2 L16Q13 */
    int16               states[4 + 4];                 /*  x(n-1) x(n-2) y(n-1) y(n-2)*/
    int16               coeffIndex;                    /*  8 byte aligned coeff index*/
    int16               memIndex;                      /*  8 byte aligned state index*/
    int16               coeff0;
    int32               yL32;                         /* 32 bit version of y(n-1)*/
} biquadFilter;


#else

typedef struct _biquadFilter
{
    int16       coeffsL16Q13[5];        /* a0, -b2, -b1-1, a2, a1, L16Q13    */
    int32       yL32;                   /* 32 bit version of y(n-1)          */
    int16       yL16[2];                /* y(n-2), y(n-1)                    */
    int16       xL16[2];                /* x(n-2), x(n-1)                    */
                                       
/*            a0 + a1*z**-1+ a2*z**-2                                        */
/*    H(z) =  -----------------------                                        */
/*            1 + b1*z**-1 + b2*z**-2                                        */
/*  filter difference equation:                                              */
/*    y(n) = a0*x(n) +  a1*x(n-1) + a2*x(n-2) - b1*y(n-1) - b2*y(n-1)        */
/*         = a0*x(n) +  a1*x(n-1) + a2*x(n-2)                                */
/*                                 y(n-1) + (-b1-1)*y(n-1) - b2*y(n-1)       */
/*  set one coeff as (-b1-1) and s16_add_s16_s16 32 bit version of y(n-1) to             */
/*  prevent accumulation of round-off errors                                 */
} biquadFilter;
#endif
/*--------------------- biquad filter coeff spec struct ---------------------*/
typedef int32 biquadSpec[5];            /* biquad filter coeffs L32Q23       */

/*--------------------- multi stage biquad filter struct --------------------*/
typedef struct _multiStageBiquadFilter
{
    int16               stages;         /* number of stages                  */
    biquadFilter        biquads[MAX_BIQUAD_STAGES]; /* array of biquads      */
} multiStageBiquadFilter;

/*------------------- multi stage biquad coeff spec struct ------------------*/
typedef struct _multiStageBiquadSpec
{
    int16               stages;         /* number of stages                  */
    const biquadSpec   *coeffs;         /* pointer to array of biquad specs  */
} multiStageBiquadSpec;

/*------------------------------ FIR filter struct --------------------------*/
typedef struct _firFilter
{
    int16   *coeffsL16;                 /* filter coefficients*/
#if ((defined __hexagon__) || (defined __qdsp6__))
    int16   xL16[MAX_FIR_TAPS + MAX_FIR_BLOCK_SIZE + 4];         /* filter history (circular)*/
#else
     int16   xL16[MAX_FIR_TAPS];
#endif
    
    int16   memIndex;                   /* current position in filter history*/
#if ((defined __hexagon__) || (defined __qdsp6__))
    int16   outIndex;
    int16   yL16[MAX_FIR_BLOCK_SIZE + 4];
#endif
    int16   taps;                       /* filter taps                       */
} firFilter;

/*----------------------------- FIR filter struct --------------------------*/
typedef struct fir_filter_t
{
    int32   mem_idx;                   /* filter memory index               */  
    int32   taps;                      /* filter taps                       */
    void    *history;                  /* filter memory (history)           */
    void    *coeffs;                   /* filter coefficients               */

#if ((defined __hexagon__) || (defined __qdsp6__))
    int16   reverse_coeff[MAX_FIR_TAPS];
#endif
} fir_filter_t;

/*=============================================================================
      Function Declarations 
=============================================================================*/
/*-------------------------------- buffers ----------------------------------*/
void buffer_empty
(
    int16           *buf,               /* buffer to be processed            */
    uint32           samples            /* number of samples in this buffer  */
);

void buffer_copy
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    uint32           samples            /* number of samples to process      */
);

void buffer_fill
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    uint32           samples            /* number of samples to process      */
);
#if ((defined __hexagon__) || (defined __qdsp6__))
void buffer_fill_asm
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    uint32           samples            /* number of samples to process      */
);
#endif
void buffer_mix
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    uint32           samples            /* number of samples to process      */
);
#if ((defined __hexagon__) || (defined __qdsp6__))
void buffer_mix_asm
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    uint32           samples            /* number of samples to process      */
);
#endif
void buffer_fill_mix
(
    int16           *src1,              /* Input1 buffer                     */
    int16           *src2,              /* input2 buffer                     */
    int16           *dst,               /* Output buffer                     */
    int16            gainL16Q15,        /* gain to the samples               */
    int16            samples            /* number of samples to process      */
);

#if ((defined __hexagon__) || (defined __qdsp6__))
void buffer_fill_mix_asm
(
    int16           *src1,           /* Input1 buffer                     */
    int16           *src2,           /* input2 buffer                     */
    int16           *dst,            /* Output buffer                     */
    int16            gainL16Q15,     /* gain to the samples               */
    int16            samples         /* number of samples to process      */
);
#endif

/*--------------------------------- delayline -------------------------------*/
void delayline_reset
(
    delaylineStruct *delayline          /* delayline struct                  */
);

boolean delayline_set
(
    delaylineStruct *delayLine,         /* delayline struct                  */
    int32 delayLen                      /* delay length                      */
);

void buffer_delay_fill
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    delaylineStruct *delayline,         /* delayline struct                  */
    int32            delay,             /* amount of delay in sample         */
    int32            samples            /* number of samples to process      */
);

void buffer_delay_mix
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    delaylineStruct *delayline,         /* delayline struct                  */
    int32            delay,             /* amount of delay in sample         */
    int32            samples            /* number of samples to process      */
);

void delayline_update
(
    delaylineStruct *delayline,         /* delayline struct                  */
    int16           *srcBuf,            /* input buffer (new samples)        */
    int32            samples            /* input buffer sample size          */
);

void delayline_copy
(
        delaylineStruct *dest,          /* The destination delay line        */
        delaylineStruct *source         /* The source delay line             */
);

/*--------------------------- variable delayline ----------------------------*/
void variable_delay_reset
(
    variableDelayStruct *variableDelay  /* variable delay struct             */
);

void variable_delay_setdelay
(
    variableDelayStruct *variableDelay, /* variable delay struct             */
    int16                newDelay,      /* new delay in sample               */
    int32                steps          /* sample dur. to change to new delay*/
);

uint32 variable_delay_process
(
    variableDelayStruct *variableDelay, /* variable delay struct             */
    int16               *destBuf,       /* output buffer                     */
    int16               *srcBuf,        /* input buffer                      */
    int16               *dlyTmpBuf,     /* scratch buffer 1                  */
    int16               *dlyMixBuf,     /* scratch buffer 2                  */
    int16                feedbackL16Q15,/* feedback gain                     */
    int32                samples        /* samples to process                */
);

/*--------------------------------- panners ---------------------------------*/
int16 panner_get_current 
(
    pannerStruct panner                 /* panner struct                     */
);

void panner_setup
(   
    pannerStruct    *panner,            /* panner struct                     */
    int16            newGainL16Q15,     /* new target panner gain            */
    int32            rampSamples,       /* number of samples in the ramp     */
    int32            newDelay           /* delay of ramping process          */
);      

void buffer_fill_with_panner
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    pannerStruct    *panner,            /* panner applied to the samples     */
    int32            samples            /* number of samples to process      */
);

void buffer_mix_with_panner
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    pannerStruct    *panner,            /* panner applied to the samples     */
    int32            samples            /* number of samples to process      */
);


/*--------------------------------- filters ---------------------------------*/
void biquad_setup
(
    biquadFilter    *filter,            /* biquad filter struct              */
    const int32      coeffs[5]          /* 32 bit filter coeffs              */
);

void biquad_reset
(
    biquadFilter    *filter             /* biquad filter struct              */
);

void biquad_process
(
    int16           *inplaceBuf,        /* inplace buffer                    */
    biquadFilter    *filter,            /* biquad filter struct              */
    int32            samples            /* number of samples to process      */
);

void biquad_process_io
(
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    biquadFilter    *filter,            /* biquad filter struct              */
    int32            samples            /* number of sample to process       */
);

void multiStageBiquad_setup
(
    multiStageBiquadFilter      *filter,        /* multi stage biquad struct */
    const multiStageBiquadSpec   filterSpec     /* coeff struct              */
);

void multiStageBiquad_reset
(
    multiStageBiquadFilter      *filter         /* multi stage biquad struct */
);

void multiStageBiquad_process
(
    int16                       *inplaceBuf,    /* inplace buffer            */
    multiStageBiquadFilter      *filter,        /* multi stage biquad struct */
    int32                        samples        /* num of samples to process */
);

#if ((defined __hexagon__) || (defined __qdsp6__))
void fir_process_c16xd16(fir_filter_t *filter, int16 *dest, int16 *src, int32 samples, int16 qx);
void fir_process_c16xd32(fir_filter_t *filter, int32 *dest, int32 *src, int32 samples, int16 qx);
void fir_process_c16xd16_rnd(fir_filter_t *filter, int16 *dest, int16 *src, int32 samples, int16 qx);
#else
void fir_process_c16xd16(fir_filter_t *filter, int16 *dest, int16 *src, int32 samples, int16 qx);
void fir_process_c16xd32(fir_filter_t *filter, int32 *dest, int32 *src, int32 samples, int16 qx);
void fir_process_c16xd16_rnd(fir_filter_t *filter, int16 *dest, int16 *src, int32 samples, int16 qx);
#endif

uint32 fir_reset
(
    firFilter       *filter             /* fir filter struct                 */
);

// fir filter functions for 16/32 data and coeffs
void fir_reset_v1(fir_filter_t *filter, int32 data_width);
void fir_reset1(fir_filter_t *filter, int32 data_width);

/* the QDSP6 assembly optimized version of this filter requires the
   filter tap and samples to be multiples of 4, a more general version is
   added back below as fir_process_general */
void fir_process
(
    firFilter       *filter,            /* fir filter struct                 */
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            samples,           /* samples to process                */
    int16            Qx                 /* Q factor of filter coeffs         */
);

void fir_process_general
(
    firFilter       *filter,            /* fir filter struct                 */
    int16           *destBuf,           /* output buffer                     */
    int16           *srcBuf,            /* input buffer                      */
    int16            samples,           /* samples to process                */
    int16            Qx                 /* Q factor of filter coeffs         */
);

int32 get_cur_cir_scale 
(
    anglePannerStruct panner            /* angle panner struct               */
);

void cir_scale_setup
(   
    anglePannerStruct *panner,          /* angle panner struct               */
    int32            newAngleL32Q16,    /* new target angle                  */
    int32            rampSamples        /* number of samples in the ramp     */
);



/*--------------------------------- filters ---------------------------------*/
void simple_lp_reset
(
    simpleLPFilt    *filter            /* one-pole filter struct            */
);

void simple_lp_proc
(
    int16           *inplaceBuf,        /* inplace buffer                    */
    simpleLPFilt   *filter,            /* one-pole filter struct            */
    int16            samples            /* number of sample to process       */
);

void inplace_delay
(
    int16           *inplaceBuf,        /* inplace buffer                    */
    delaylineStruct *delayline,         /* delayline struct                  */
    int32            samples            /* number of samples to process      */
);

void dscale_setup
(   
    pannerStruct *panner,  /* panner struct                  */
    int16 newGainL16Q15,   /* new target panner gain         */
    int32 rampSamples,     /* number of samples in the ramp  */
    int32 newDelay         /* delay of ramping process       */
);      

void IIR_one_pole(int16 *xin, int32 *yL40, int32 c0, int32 c1, int16 nsamples);
void buffer_crossmix_panner(int16 *dest, int16 *src1, int16 *src2, pannerStruct *panner, int32 samples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _AUDIO_DSP_H_ */

