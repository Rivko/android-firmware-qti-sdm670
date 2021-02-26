#ifndef _DTMF_GEN_EXPORT_H
#define _DTMF_GEN_EXPORT_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef int64 MicroSecs;

typedef struct {
   int16 increment;
   int16 tableIndex;
}DTMFToneGenParamsStruct;

typedef enum {
   EIGHT = 0,
   ELEVEN_QUARTER,
   TWELVE,
   SIXTEEN,
   TWENTYTWO_HALF,
   TWENTYFOUR,
   THIRTYTWO,
   FORTYFOUR,
   FORTYEIGHT
}SetOfSamplingFreqs;

typedef struct {
   DTMFToneGenParamsStruct tone1;
   DTMFToneGenParamsStruct tone2;
   int32 totalSamples;  // no of samples in the total duration
   int32 currentSamples; // no of samples generated
   int32 rampDownPreamble;
   int16 rampUpSamples;
   int16 rampDownSamples;
   int16 rampUpSampCnt;
   int16 rampUpGain;
   int16 rampUpStep;
   int16 rampDownSampCnt;
   int16 rampDownGain;
   int16 rampDownStep;
   int32 dtmfMode;
   int32 rampDownActive;
   SetOfSamplingFreqs samplingFreq;
}DTMFGenStruct;

typedef enum {
   CONTINUOUS = 0,
   BURST
} DtmfMode;

typedef enum {
   DTMF_STOP=0,
   DTMF_RUN
} DtmfState;

//numRampUp and numRampDown duration must be more than 8 samples
//and should be multiple of 2( Recommend: 8,16,32,64)
extern int32 dtmf_tone_init_general(DTMFGenStruct *dtmfGen,int32 dtmfFreq1,
      int32 dtmfFreq2,
      MicroSecs duration,
      SetOfSamplingFreqs samplingfreq,
      int16 isResetTable, int16 needRampUp);

extern int32 dtmf_tone_init(DTMFGenStruct *dtmfGen,int32 dtmfFreq1,
      int32 dtmfFreq2,
      MicroSecs duration,
      SetOfSamplingFreqs samplingfreq);

extern void dtmf_continue_tone(DTMFGenStruct *dtmfGen, 
      MicroSecs duration);

extern int32 dtmf_tone_gen(DTMFGenStruct *dtmfGen,
      int16 *outPtr,
      uint16 dtmfGain,
      int16 numSamples,
      int16* samplesLeftPostRampDownPtr,
      int32 dtmfMixEnable,
      int32 lastBlock);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _DTMF_GEN_EXPORT_H
