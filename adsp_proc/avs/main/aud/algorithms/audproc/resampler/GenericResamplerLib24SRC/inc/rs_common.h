/*===========================================================================* 
Copyright (c) 2007-2008, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*===========================================================================*/
/* FILE NAME: rs_common.h                                                    *
 * DESCRIPTION:                                                              *
 *   Header file for resampler project.  Contains typedefs, macros,          *
 *   variables and function declarations used in resampler project.          *
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   08/30/05    swang    initial release of the file.                       *
 *   09/30/05    swang    modified for second release.                       *
 *                        removed MIN_FREQ, added Q18_SHIFT                  *
 *   05/03/10    swang    adding more functions for 32 bit support           *
 *   08/05/11    swang    Release for 16/32 bit resampler.                   *
 *===========================================================================*/

#ifndef  _RS_COMMON_H_
#define  _RS_COMMON_H_

/*===========================================================================*
 *           TYPE DEFINITIONS                                                *
 *===========================================================================*/

/*===========================================================================*
 *           MACROS, CONSTANTS, STRUCTURES, AND FUNCTION DECLARATIONS        *
 *           FOR UP-SAMPLER                                                  *
 *===========================================================================*/
#define     MAX_FREQ            		384000
#define     MAX_CHAN        			16
#define     Q15_SHIFT           		15
#define     Q20_SHIFT           		20
#define     MAX_Q15             		32767
#define     MIN_Q15             		-32768
#define     MAX_UPSAMPFILT_LEN_16     	32
#define     MAX_UPSAMPFILT_LEN_32     	48
#define     MAX_DNSAMPFILT_LEN_16		768
#define     MAX_DNSAMPFILT_LEN_32		1104
#define 	OSR_160 					160 // Total phase steps
#define		CONV_LEN_441_48				48 //convolution size for each phase
#define 	THD110_REMAPPED_COEFF_LEN	3840    // coeff size in words
#define     SCRA_MEM_LEN    			512
#define 	ENABLE						1
#define 	DISABLE						0
#define CHANNEL_MEM_GENERIC				0 
#define INSTANCE_MEM_GENERIC			1
#define INSTANCE_MEM_HIGH_THD	2
#define CHANNEL_MEM_HIGH_THD		3
#define     COEF_QFAC   23
#define     HALF_QFAC   ((int32)1 << (COEF_QFAC-1))
#define     MEM_LEN     48
//#define     SPECIAL_MODE    

//////////////////////////////////////////////////////////////////////////////
// ENUMERATIONS
//////////////////////////////////////////////////////////////////////////////
/*
* This type is used to identify the Generic Resampler Type.
* Fixed Input type or Fixed Output type
*/
typedef enum
{
    GEN_RESAMP_FIXED_INPUT,   ///< Resampler Type: Fixed Input
    GEN_RESAMP_FIXED_OUTPUT   ///< Resampler Type: Fixed Output
} ResampType;

/*
* This type is used to identify the Generic Resampler operating quality
* High Quality, Super Low Power and Low Power operating modes.
* Before changing the type of Resampler, the module has to be re initialized.
*/
typedef enum
{
    GEN_RESAMP_HQ_LPH,      ///< Resampler Quality: High Quality Linear Phase
	GEN_RESAMP_HQ_MPH,		///< Resampler Quality: High Quality Minimum Phase
    GEN_RESAMP_LP_LPH,     	///< Resampler Quality: Low Power Linear Phase
	GEN_RESAMP_LP_MPH,      	///< Resampler Quality: Low Power Minimum Phase
	GEN_RESAMP_LP_LPH_SPKRPROT	///< Resampler Quality: Low Power Linear Phase speaker Prot
} ResampQuality_16;

typedef enum
{
    GEN_RESAMP_HQ_LPH_70dB,      	///< Resampler Quality: High Quality Linear Phase 70dB
	GEN_RESAMP_HQ_LPH_90dB,			///< Resampler Quality: High Quality Linear Phase 90dB
    GEN_RESAMP_HQ_LPH_105dB,     	///< Resampler Quality: High Quality Linear Phase 105dB
	GEN_RESAMP_HQ_LPH_105dB_48      ///< Resampler Quality: High Quality Linear Phase 105dB_48
} ResampQuality_32;

typedef struct _ResampQuality
{
	ResampQuality_16 rsQual16;
	ResampQuality_32 rsQual32;
} ResampQuality;	
/*
* This enumeration contains return status values.
*/
typedef enum 
{
    GEN_RESAMP_SUCCESS=0,
    GEN_RESAMP_FAILURE,
    GEN_RESAMP_NOMEMORY,
	GEN_RESAMP_INVALIDFS
} ResampReturnType;


/*===========================================================================*
 *           EXTERNAL VARIABLES                                              *
 *===========================================================================*/
extern const int16 oneQ15, halfQ15;

/*===========================================================================*
 *           STRUCTURES                                                      *
 *===========================================================================*/
typedef struct _USPhase
{
    int16   curIntPh;       /* current integer phase                         */
    int16   intPhStep;      /* integer phase  update step size               */
    int32   curFracPh;      /* current fractional phase                      */
    int32   fracPhStep;     /* fractional phase update step size             */
    int32   inFreq;         /* input sampling rate                           */
    int32   outFreq;        /* output sampling rate                          */
    int16   fmemIndex;      /* filter memory index                           */
    int16   totalIntPh;     /* total integer phase                           */
    uint16  fracConst;      /* constant used to evaluate fractional phase    */
    int16   shiftConst;     /* shift constant for evaluating fractional phase*/	
}   USPhase;

typedef struct _DSPhase
{
    int32   inFreq;         /* input sampling rate                          */
    int32   medFreq;        /* intermediate sampling rate                   */
    int32   outFreq;        /* output sampling rate                         */
    int16   dnSampFac;      /* down-sampling factor                         */
    int16   dnSampStep;     /* down-sampling step size, used for loading    */
                            /* filter memory                                */
    int16   convIndex;      /* filter index for convolution                 */
    int16   loadIndex;      /* filter index for loading filter memory       */	
	int32   inputIndex;     /* filter index for tracking inputs             */	
}   DSPhase;

typedef struct _Filters
{
    int16   coefLen;        /* length of filter coefficient                 */
    int16   coefStep;       /* convolution step size of filter coefficient  */
    int32   halfQfac;       /* one half for a given Q factor                */
    int16   qFac;           /* Qfactor of the filter coefficients           */
    int16   memLen;         /* length of filter memory                      */    
	int32   convLength;
    void    *pCoef;         /* point to filter coefficient                  */
	void    *pMem;          /* point to filter memory                       */
	int16   memStep;        /* convolution step size of filter memmory      */
}   Filters;

/*===========================================================================*
 *           FUNCTION DECLARATIONS                                           *
 *===========================================================================*/
#endif /* _RS_COMMON_H_ */
/*===========================================================================* 
 * End of rs_common.h                                                        * 
 *===========================================================================*/
