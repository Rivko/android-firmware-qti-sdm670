/*===========================================================================* 
 * Copyright (c) 2005 by Qualcomm Technologies INC.  All rights reserved.                 * 
 * All data and information contained in or disclosed by this document is    * 
 * confidential and proprietary information of Qualcomm Technologies INC.  And, all       * 
 * rights therein are expressly reserved.  By accepting this material the    * 
 * recipient agrees that this material and the information contained therein * 
 * is held in confidence and in trust and will not be used, copyied,         * 
 * reproducedin whole or in part, nor its contents revealed in any manner to * 
 * others without the expressed written premission of Qualcomm Technologies INC.          * 
 *===========================================================================*/
/* FILE NAME: resampler_32b_ext.h                                            *
 * DESCRIPTION:                                                              *
 *   External header file for resampler.c file.   Contains function          *
 *   declarations for library integration                                    *
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   08/05/10    swang    Initial release to support 32 bit processing.      *
 *===========================================================================*/

#ifndef     _RESAMPLER_32B_EXT_H_
#define     _RESAMPLER_32B_EXT_H_

/*===========================================================================*
 *           INCLUDE HEADER FILES                                            *
 *===========================================================================*/
#include "basic_op.h"

/*===========================================================================*
 *           Constants.                                                      *
 *===========================================================================*/
#define     MAX_FREQ            384000
#define     AV_SYNC_RANGE       20

typedef enum
{
    MODE,               /* SRC mode word                                 */
    IN_FREQ,            /* input frequency                               */
    OUT_FREQ,           /* output frequency                              */
    US_OSR,             /* up-sampler OSR                                */
    US_FLEN,            /* up-sampler filter length                      */
    US_MEM_LEN,         /* up-sampler memory length                      */
    US_QFAC,            /* up-sampler filter coefficient q-factor        */
    US_SYMMETRIC,       /* up-sampler filter symmetry indicator          */
    DS_OSR,             /* down-sampler OSR                              */
    DS_FLEN,            /* down-sampler filter length                    */
    DS_MEM_LEN,         /* down-sampler memory length                    */
    DS_QFAC,            /* down-sampler filter coefficient q-factor      */
    DS_SYMMETRIC,       /* down-sampler filter symmetry indicator        */
    DS_SCR_MLEN,        /* down-sampler scratch memory length            */
    MODE_COEF_BIT,      /* coefficient bit width indicator, 16 bit input */
                        /*   0 -- 16 bit filter coefficients             */
                        /*   2 -- 32 bit filter coefficients             */
    MODE_ASYNC,          /* asynchronous indicator                        */
    MODE_DYNAMIC,       /* dynamic resampler indicator                   */
    DYNAMIC_US_DELAY,   /* extra upsampler delay for dynamic resampler   */
    DYNAMIC_DS_DELAY    /* extra downsampler delay for dynamic resampler */
} PARAM;

/*===========================================================================*
 *           Variables.                                                      *
 *===========================================================================*/
/* Shared filter variables */
extern int16 upsampFiltLen, dnsampFiltLen, resampTotalPh;

/* filter variables for 16 bit implementation */
extern int16 *upsampCoef16, *dnsampCoef16;
extern int16 resampFiltLenGrp16[], resampTotalPhGrp16[], resampFiltQFac16[];
extern int16 upsampCoefHqLphQ15[], upsampCoefHqMphQ15[];
extern int16 upsampCoefLpLphQ15[];
extern int16 upsampCoef85dB96xQ15[];
extern int16 dnsampCoefQ15[];
extern int16 resampIsSymmetric16[], resampIsSymmetric[];

/* filter variables for 32 bit implementation */
extern int32 *resampCoef32;
extern int16 resampFiltLenGrp[], resampTotalPhGrp[], resampFiltQFac[];
extern int32 resampCoef105dBQ19[], resampCoef110dBQ19[];
extern int32 resampCoef105dBQ23[], resampCoef110dBQ23[];

/*===========================================================================*
 *           FUNCTION DECLARATIONS FOR BOTH UP-SAMPLER AND DOWN-SAMPLER      *
 *===========================================================================*/
void  resamp_size(int16 usFiltLen, int16 totalPh, int16 dsFiltLen, 
                  int16 isUsFiltSymmetric, int16 isDsFiltSymmetric, 
                  int16 *pUsSize, int16 *pUsMemSize, int16 *pDsSize, 
                  int16 *pDsMemSize);
int16 resamp_output_latency(void *pRS, int16 mode);
int16 resamp_input_latency(void *pRS, int16 mode);



/*===========================================================================*
 *           FUNCTION DECLARATIONS FOR UP-SAMPLER                            *
 *===========================================================================*/
int32 init_upsamp(void* pUS, int32* param, void* filtCoeff, void *filtMem);
int32 upsamp_calc_fixedout(void* pUS, int32 numOutSamps);
int32 upsamp_calc_fixedin(void* pUS, int32 numInSamps);
int32 upsamp_fixedin(void* pUS, void* ptrIn, void* ptrOut, int32 numInSamps);
int32 upsamp_fixedout(void* pUS, void* ptrIn, void* ptrOut, int32 numOutSamps);

/*===========================================================================*
 *           FUNCTION DECLARATIONS FOR DOWN_SAMPLER                          *
 *===========================================================================*/
int32 init_dnsamp(void*, int32* par, void*, void*, void*, void*, void*);
int32 dnsamp_fixedin(void* ptrDS, void* pInBuf, void* pOutBuf, int32 numInSamps);
int32 dnsamp_fixedout(void* ptrDS, void* pInBuf, void* pOutBuf, int32 numOutSamps);
int32 dnsamp_calc_fixedin(void*, int32);
int32 dnsamp_calc_fixedout(void*, int32);

#endif  /* _RESAMPLER_32B_EXT_H_ */
