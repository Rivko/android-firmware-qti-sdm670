/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2008-2009, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audpp_converter.h                 TYPE: C-header file              *]
[* DESCRIPTION: Header file for rate conversion types and fns.               *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   08/01/06   xyp     Initial revision                                     *]
[*****************************************************************************/
#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include "audpp_mathlib.h"
#include "audpp_common.h"
#include "audio_basic_op_ext.h"

#define NORMAL_PLAYBACK_RATE 0x10000
#define CIRC_MAX_SIZE 32767

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================
  Typedefs
=============================================================================*/
typedef struct _rateConvertState
{
    int32              indexL32Q16;       /* current sample index, Q16     */
    int32              rateL32Q16;        /* current playback rate, Q16    */
    int32               accelL32Q16;        /* per-sample change in rate     */
} rateConvertState; 

typedef struct _ratePanner
{
    int32              targetRateL32Q16;  /* desired final playback rate   */
    int32               rateChangeL32Q16;   /* change from prev rate         */
    int32               sampleCounter;      /* samples left in rate change   */
    int32               stepRateL32Q16;     /* 1/steps in Q16                */
    int32              indexL32Q16;       /* current index in conversion   */
} ratePanner;

typedef struct _stereoRateConverter
{
    ratePanner          panner;             /* rate panner struct            */
    int16               oldInputs[6];       /* 3 interleaved stereo samples  */
} stereoRateConverter;

typedef struct _bufRateCvtrStruct
{
    rateConvertState    srcState;           /* rate convert state            */
    int16               oldInputs[2];       /* previous inputs for interp    */
} bufRateCvtrStruct;

/*=============================================================================
      Function Declarations 
=============================================================================*/
/*----------------------------- rate convert state --------------------------*/
int32 rateConvertState_predict_inputs
(    
    rateConvertState    *state,             /* rate convert state struct     */
    int32                outputs            /* output sample number          */
);

void rateConvertState_update_index
(    
    rateConvertState    *state,             /* rate convert state struct     */
    int32               *inputs             /* input sample number           */
);

/*-------------------------------- rate panner ------------------------------*/
void ratepanner_reset
(
    ratePanner     *panner                  /* rate panner struct            */
);

uint32 ratepanner_get_playback_rate
(
    ratePanner     *panner                  /* rate panner struct            */
);

uint32 ratepanner_set_playback_rate
(
    ratePanner     *panner,                 /* rate panner struct            */
    int32           newRateL32Q16,          /* target rate                   */
    int32           steps                   /* steps to reach target rate    */
);

void ratepanner_setup_convert
(
    ratePanner     *panner,                 /* rate panner struct            */
    rateConvertState *srcState              /* rate convert state struct     */
);

uint32 ratepanner_advance
(
    ratePanner     *panner,                 /* rate panner struct            */
    int32           outputs                 /* output sample numer           */
);

int32 ratepanner_predict_inputs
(
    ratePanner     *panner,                 /* rate panner struct            */
    int32           outputs                 /* output sample number          */
);


/*------------------------------- rate conversion ---------------------------*/
int32 convert_linear
(
    int16               *destBuf,           /* output buffer                 */
    int32                outputs,           /* number of output samples      */
    int16               *srcBuf,            /* input buffer                  */
    int32                inputs,            /* number of input samples       */
    rateConvertState    *state              /* buffer converter state        */
);

int32 convert_rate_stereo
(
    int16               *destBufL,          /* output buffer L               */
    int16               *destBufR,          /* output buffer R               */
    int32                outputs,           /* number of output samples      */
    int16               *srcBuf,            /* input buffer                  */
    int32                inputs,            /* number of input samples       */
    rateConvertState    *state              /* buffer converter state        */
);

uint32 stereo_converter_convert
(
    stereoRateConverter    *converter,      /* stereo rate converter struct  */
    int16                  *destBufL,       /* output buffer L               */
    int16                  *destBufR,       /* output buffer R               */
    int32                  *outputs,        /* output sample number          */
    int16                  *srcBuf,         /* input buffer                  */
    int16                   channels,       /* number of channels            */
    int32                  *inputs,         /* input sample number           */
    int32                   start           /* offset in buffer to start     */
);

int32 stereo_converter_switch_to_bypass
(
    stereoRateConverter    *converter,      /* stereo rate converter struct  */
    int16                  *destBufL,       /* output buffer L               */
    int16                  *destBufR,       /* output buffer R               */
    int16                   channel,        /* number of channels            */
    int32                   n,              /* number of samples to process  */
    int32                   start           /* offset in buffer to start     */
);

/*-------------------------- buffer rate converter --------------------------*/
void buf_rate_converter_reset
(
    bufRateCvtrStruct   *converter          /* buffer rate converter struct  */
);

void buf_rate_converter_setrate
(
    bufRateCvtrStruct   *converter,         /* buffer rate converter struct  */
    int32               playbackRateL32Q16/* playback rate in Q16          */
);

uint32 buf_rate_converter_convert
(
    int16               *destBuf,           /* output buffer                 */
    int32               *outputsPtr,        /* -> output number of samples   */
    int16               *srcBuf,            /* input buffer                  */
    int32               *inputsPtr,         /* -> input number of samples    */
    bufRateCvtrStruct   *converter          /* buffer rate converter struct  */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CONVERTER_H_ */

