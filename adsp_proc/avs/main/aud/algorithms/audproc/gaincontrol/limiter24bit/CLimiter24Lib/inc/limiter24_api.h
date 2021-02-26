/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   limiter24_api.h                TYPE: C-header file           *]
[* DESCRIPTION: Public header file for the 24bit Limiter algorithm.          *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2012-1-12  AVS	    24-bit limiter first version                         *]
[*****************************************************************************/

#ifndef _LIMITER_API_H
#define _LIMITER_API_H

#if ((defined __hexagon__) || (defined __qdsp6__))
#define LIM_ASM
#endif
/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "AEEStdDef.h"

#define LIMITER_BLOCKSIZE 			480       /* Maximum frame Size */
#define LIM_MAX_NUM_CHANNELS 		2
#define MAX_LIM_DELAY_BUF_SIZE 		48*10 //10ms delay at 48KHz
#define LIM_MAX_DELAY 				327 //10ms = 0.01 in Q15 format

/*----------------------------------------------------------------------------
* Type Declarations for overall configuration and state data structures
* -------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/**
@brief Algorithm Tuning parameters to be read from the parameter file
*/
typedef enum
{		
	LIM_ENABLE             = 0,         // Q0 Enable word for the limiter
	LIM_MODE               = 1,         // Q0 Mode word for the limiter
	LIM_THRESH             = 2,         // Threshold value: Q3.15 for 16bit; Q8.23 for 32bit
	LIM_MAKEUP_GAIN        = 3,         // Makeup gain: Q7.8 
	LIM_GC                 = 4,         // Q15 Gain recovery coefficient
	LIM_DELAY              = 5,         // Q15 Delay in seconds
	LIM_MAX_WAIT           = 6,         // Q15 Maximum waiting time in seconds for delay-less limiter
	LIM_PAR_TOTAL          = 7          // Total number of configuration parameters
} LimParamsList;

typedef enum
{		
	FADE_INIT  = 0,   
	FADE_START,       
	FADE_STOP          

} FadeConstants;
/**
@brief Default values of tuning parameters and hardcoded parameters used in the algorithm
*/
typedef enum
{
	// Default parameters
	LIM_DISABLE_VAL         = 0x0000,          // Q0 Disable word for the limiter
	LIM_ENABLE_VAL          = 0x0001,          // Q0 Enable word for the limiter
	LIM_MODE_VAL            = 0x0001,          // Q0 Default mode word for the limiter
	LIM_THRESH_VAL32        = 93945856,        // Q8.27 (0.7) Default threshold: -3 dB
	LIM_THRESH_VAL16        = 22936,           // Q3.15 (0.7) Default threshold: -3 dB
	LIM_MAKEUP_GAIN_VAL     = 0x0100,          // Q7.8 (1) Default gain: 0 dB
	LIM_GC_VAL              = 32440,           // Q15 (0.99) Default gain recovery coefficient
	LIM_DELAY_VAL           = 82,              // Q15 (0.0025) Default delay in seconds
	LIM_MAX_WAIT_VAL        = 82               // Q15 (0.0025) Default waiting time in seconds
} LimParamsDefault;

/**
@brief configuration parameter structure per channel 
*/
typedef struct _LimCfgParams
{
   int32 limThresh;         // Threshold value: Q3.15 for 16bit; Q8.23 for 32bit 
   int32 limEnable;         // Q0 Enable word for the limiter feature
   int32 limMode;           // Q0 Mode word for the limiter: 1 enable limiter for the ba
   int32 limMakeUpGain;     // Q7.8 Makeup gain  
   int32 limGc;             // Q15 Gain Recovery coefficient
   int16 limDelay;          // Q15 Limiter delay in seconds
   int16 limMaxWait;        // Q15 Limiter waiting time in seconds

} LimCfgParams;

/**
@brief limiter configuration structure
*/
typedef struct _LimCfgType
{   
	 // Don't change the order of the first 5 fields which are used in ASM
	int32 threshL32Q15;
    int32 mGainL32;                // Left-shifted make-up gain in L32
    int32 limDelayMinusOne;        // Q0 Limiter delay in samples minus one sample
    int32 limGc;                          // Q15 Gain Recovery coefficient
    int32 limWaitMinusOne;         // Q0 Limiter Max wait in samples minus one sample

    LimCfgParams params;           /* Configuration struct for limiter parameters */

} LimCfgType;


/**
@brief limiter processing data structure
*/
typedef struct _LimDataType
{   // Don't change the order of the first 8 fields which are used in ASM    
    int32 *pInpBuffer;                      /* pointer to delay buffer array */
    int32 *pZcBuffer;                       /* pointer zero-crossing buffer array */
	int32 localMaxPeakL32;                  /* Local Maximum peak array*/
    int32 prevSampleL32;                    /* Previous sample in the buffer */
    int32 curIndex;                         /* Current sample index */       
    int32 prev0xIndex;                      /* Previous zero-crossing index array*/    
    int32 gainVarQ15;                       /* Q15 Limiter gain variable array */  
	int32 gainQ15;                          /* Q15 Limiter gain array */   
	int32 *ptempBuffer;    /* tempory buffer array*/
	int32 fadeFlag;                         /* Q0 Fade flag for transition purposes */
	int32 dummy; // for 8-byte alignment of the structure
} LimDataType;


typedef struct _CLimiterLib
{
	LimCfgType LimCfgStruct __attribute__ ((aligned (8)));
	LimDataType LimDataStruct __attribute__ ((aligned (8)));
	int32 *pDelayBuf;
	int32 *pZcBuf;
	int32 bitsPerSample;
	void (*lim_proc) (LimCfgType *pCfg,LimDataType *pData, int32 *in, void *out,int16 iSubFrameSize);
	void (*apply_makeup_gain)(LimCfgType *pCfg,LimDataType *pData,void** pOut,int16 iSubFrameSize);

}CLimiterLib;

/**
@brief Initialize LIM algorithm with default value

Performs initialization of data structures for the
LIM algorithm. A pointer to static memory is storage
is passed for configuring the LIM static configuration
structure.

@param params: [in] pointer to configuration structure
@param numChannels: [in] num of channels
@param bitsPerSample: [in] bits per sample
*/
int Lim_init_default(int32 *params, 
                     int16 numChannels,
                     int16 bitsPerSample);

/**
@brief Initialize LIM algorithm

Performs initialization of data structures for the
LIM algorithm. A pointer to static memory is storage
is passed for configuring the LIM static configuration
structure.

@param pLimStruct: [in] pointer to limiter library struct
@param params: [in] pointer to tuning params list
@param chIdx: [in] current channel index 
@param sampleRate: [in] Input sampling rate
@param bitsPerSample: [in] bits per sample
*/
int Lim_init(CLimiterLib *pLimStruct,
			 int32 *params,
			 int16 chIdx,
			 int32 sampleRate,
			 int32 bitsPerSample);

/**
@brief Re_Initialize LIM algorithm
A pointer to static memory storage is passed for 
configuring the LIM static configuration structure.

@param pLimCfgStruct: [in] pointer to configuration structure
@param params: [in] Pointer to tuning parameter list
@param sampleRate: [in] Input sampling rate
*/

int Lim_reinit(LimCfgType *pLimCfgStruct,
             int32 *params,
             int32 sampleRate);

/**
@brief Reset LIM data structure

Performs reset of data structures for the
LIM algorithm. A pointer to static memory in storage
is passed for resetting the LIM static configuration
structure.

@param pLimDataStruct: [in] pointer to data structure
@param pLimCfgStruct: [in] pointer to limiter cfg struct
*/
void Lim_DataStruct_Reset(LimDataType *pLimDataStruct,
						  LimCfgType *pLimCfgStruct);

/**
@brief Process input audio data with limiter algorithm

@param pLimStruct: [in] pointer to limiter library structure
@param pOut: [out] Pointer to 16-bit Q15 output channel data
@param pInp: [in] Pointer to 32-bit Q15 input channel data
@param frameSize: [in] Input frame size
@param bypass: [in] bypass flag 
*/
void Lim_process(CLimiterLib *pLimStruct,
				 void        *pOut, 
				 int32       *pInp, 
				 int16       frameSize,
				 int16 bypass);

/**
@brief        Returns the limiter library size for a given delay,samplerata,numchannels 
			  so that the library users can use this funtion to allocate the num of bytes
			  returned by this.
@param        limDelay: [in] limiter delay in sec Q15 format
@param		  sampleRate: [in] sampleRate
@param		  numChannles: [in] num of channels	 
@param        limLibSize: [out] total library size for the above input params
*/

int32 Lim_get_lib_size(int16 limDelay,
					   int32 sampleRate,
					   int32 numChannels);


/**
@brief   Places the library structures and buffers at the given input pointer/memory location
@param    pLimBufPtr: [in] the start address of the malloced memory
@param			  pLimiter: [in] address of the first element of the array of limiter pointers,
 						the array members will be filled with the input memory pointer
@param			  limDelay: [in] limiter delay in sec Q15 format
@param			  sampleRate: [in] sampleRate
@param			  chIdx: [in] channel index num
*/

void Lim_set_lib_memory(int8 *pLimBufPtr,
						CLimiterLib **pLimiter, 
						int16 limDelay,
						int32 sampleRate,
						int32 chIdx);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _LIMITER_API_H */
