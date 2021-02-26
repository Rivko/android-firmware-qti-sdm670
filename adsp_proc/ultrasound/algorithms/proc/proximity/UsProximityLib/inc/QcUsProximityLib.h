
#ifndef QC_US_PROXIMITY_LIB_H
#define QC_US_PROXIMITY_LIB_H

/*-----------------------------------------------------------------------
 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
03.22.2011   nd      Initial version
============================================================================*/

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#if !defined MATLAB && !defined __hexagon__ // android
#include <android/log.h>
#endif

#if defined __hexagon__
#include <hexagon_protos.h>
#if !defined(US_STANDALONE_ALG_TESTS)
// compiling regular LPASS image
#include "qurt_elite_diag.h"
#ifdef USND_ALG_DYN_MODULES
// This should be defined before including HAP_farf so that the
// level requested would be compiled in
#define FARF_HIGH 1
#include "HAP_farf.h"
#endif
#else // defined (US_STANDALONE_ALG_TESTS)
#endif
#else // not hexagon
#endif

#include "QcUsComDef.h"
#include "QcUsProximityLibApi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
// General
#define LPASS_SUPPORTED_VERSION_NUM                               1
#define VERSION_NUM                                            7401
#define CORRELATION_DELAY                                        20

#define RX_PATTERN_LEN_LOG2                                      10
#define RX_PATTERN_LEN                                           (1 << RX_PATTERN_LEN_LOG2)
#define RX_PATTERN_LEN_HALF                                      (1 << (RX_PATTERN_LEN_LOG2 - 1))
#define NUM_MICS                                                 1


#define STATE_WARMUP_DURATION                                    20

// Rx Pattern
#define FS_HZ                                                    192000

// Correlation
#define MAX_CORRELATION_LENGTH                                   RX_PATTERN_LEN
#define CORRELATION_LENGTH                                       400
#define MAX_NUM_FRAMES_IN_BUFFER                                 256
#define NUM_FRAMES_IN_BUFFER                                     80

//Jammer
#define JAMMER_FREQUENCY_ERR_MAX                                 (1 << (32 - RX_PATTERN_LEN_LOG2))

// Cfg
#define PATTERN_TYPE                                             0  // 0 randomOfdm, 1 chirp, 2 gauss
#define RX_START_FR_HZ                                           22000
#define RX_BANDWIDTH_HZ                                          20000
#define RX_MAX_AMPLITUDE                                         32767  //(2^15-1) backoff in CODEC using "RX Digital Volume"
#define RAMP_LOG2                                                7
#define QUAD_M                                                   5

#define CORR_PATTERN_MAX_AMPLITUDE                               32767  //(2^15-1)

#define JAMMER_DETECTOR_DILUTION                                 16
#define JAMMER_DETECT_PSD_LPF_BIT_SHIFT                          5
#define JAMMER_FRAME_MAX_2_MEAN_RATIO_TH                         20
#define JAMMER_DETECT_MAX_2_MEAN_RATIO_TH                        20
#define JAMMER_RELEASE_MAX_2_MEAN_RATIO_TH                       12
#define JAMMER_FREQUENCY_LPF_BIT_SHIFT                           6
#define JAMMER_RESEASE_COUNTER_MAX                               64
#define JAMMER_NOTCH_POLE_AMP16                                  29491
#define JAMMER_NOTCH_POLE_AMP12                                  1843
#define JAMMER_NOTCH_POLE_AMP16_SQ                               13271
#define JAMMER_NOTCH_BSHIFT                                      11
#define JAMMER_EXTEND_WINDOW                                     20

#define TRIGGER_PROXIMITY_CORRELATION_POWER_SHIFT                17

#define INSTANT_POWER_MAX_INDEX                                  16
#define INSTANT_POWER_MAX_TH                                     2e7
#define POWER_LPF_SHIFT                                          5
#define COVERED_POWER_TH                                         2e6

#define INDEX_LPF_SHIFT                                          5
#define INDEX_LPF_TH_LOW                                         2e5
#define INDEX_LPF_TH_HYSTERESIS_SHIFT                            2
#define INDEX_LPF_TH_HIGH                                        (((int) INDEX_LPF_TH_LOW) << INDEX_LPF_TH_HYSTERESIS_SHIFT)
#define INDEX_LPF_INIT                                           (((int) (INDEX_LPF_TH_LOW + INDEX_LPF_TH_HIGH)) >> 1)
#define INDEX_LPF_TH_KEEP_CORR_REF                               INDEX_LPF_TH_HIGH

#define DISTANCE_CM_TO_INDEX                                     11  //10*2*192/340
#define PPOXIMITY_DISTANCE_CM_DEFAULT                            4

#define BBCORR_DIFF_REF_POWER_MAX_INDEX_DELTA                    (80-PPOXIMITY_DISTANCE_CM_DEFAULT*DISTANCE_CM_TO_INDEX)
#define BBCORR_DIFF_REF_POWER_TH                                 1e4
#define BBCORR_DIFF_POWER_TH_SLOPE_SHIFT                         6
#define COUNTER_LPF_SHIFT                                        5
#define COUNTER_LPF_TH                                           500
#define DIFF_POWER_TH_HYSTERESIS_SHIFT                           2

// Saturation
#define SATURATION_VALUE_TH                                      32268   //2^15-500
#define SATURATION_NUM_SAMPLES_TH                                100
#define SATURATION_COUNTER_MAX                                   NUM_FRAMES_IN_BUFFER

// dist
#define DIST_DIFF_POWER_TH_MIN_MORE_SHIFT                        1
#define DIST_DIFF_POWER_INDEX_MAX_DELTA                          (120-PPOXIMITY_DISTANCE_CM_DEFAULT*DISTANCE_CM_TO_INDEX)
#define DIST_MAX_TO_TH_SHIFT                                     1
#define DIST_LPF_SHIFT                                           7
#define DIST_VALID_LPF_TH_SHIFT                                  1
#define INDEX_TO_DISTANCE_MM_SHIFT8                              227     //340/192/2*2^8

/*----------------------------------------------------------------------------
  Macros
----------------------------------------------------------------------------*/
/**
  Allocate len bytes from the heap contained within the memory pointed
  to by pData.  Pointer to new memory returned in pAlloc.
 */
#define MEM_ALLOC(pData, pAlloc, len)    \
  (pAlloc) = (void *) (pData)->pScratch; \
  (pData)->pScratch += (len);

/**
  Logging macros
*/
#ifdef MATLAB
# define LOGE(...)   mexErrMsgTxt(__VA_ARGS__)
# define LOGI(...)   printf(__VA_ARGS__);printf("\n")
// fixed-argument macros for qdsp6 compatibility
# define LOGI_0(fmt)          LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__)
# define LOGI_1(fmt, a)         LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a)
# define LOGI_2(fmt, a, b)        LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a, b)
# define LOGI_3(fmt, a, b, c)     LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a, b, c)
# define LOGI_4(fmt, a, b, c, d)      LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a, b, c, d)
# define LOGI_5(fmt, a, b, c, d, e)   LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a, b, c, d, e)
# define LOGI_6(fmt, a, b, c, d, e, f)  LOGI("%s (%d): " ## fmt, __FUNCTION__, __LINE__, a, b, c, d, e, f)
#elif defined(__hexagon__)
# ifdef US_STANDALONE_ALG_TESTS
# define LOGI_0(fmt)          printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__)
# define LOGI_1(fmt, a)         printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a)
# define LOGI_2(fmt, a, b)        printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a, b)
# define LOGI_3(fmt, a, b, c)     printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a, b, c)
# define LOGI_4(fmt, a, b, c, d)    printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a, b, c, d)
# define LOGI_5(fmt, a, b, c, d, e)   printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a, b, c, d, e)
# define LOGI_6(fmt, a, b, c, d, e, f)  printf("%s (%d): " fmt "\n", __FUNCTION__, __LINE__, a, b, c, d, e, f)
#else
# ifdef USND_ALG_DYN_MODULES
#  define LOGI_0(fmt)            FARF(HIGH, fmt)
#  define LOGI_1(fmt, a)         FARF(HIGH, fmt, a)
#  define LOGI_2(fmt, a, b)        FARF(HIGH, fmt, a, b)
#  define LOGI_3(fmt, a, b, c)     FARF(HIGH, fmt, a, b, c)
#  define LOGI_4(fmt, a, b, c, d)      FARF(HIGH, fmt, a, b, c, d)
#  define LOGI_5(fmt, a, b, c, d, e)   FARF(HIGH, fmt, a, b, c, d, e)
#  define LOGI_6(fmt, a, b, c, d, e, f)  FARF(HIGH, fmt, a, b, c, d, e, f)
# else // !USND_ALG_DYN_MODULES
#  define LOGI_0(fmt)            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt)
#  define LOGI_1(fmt, a)         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a)
#  define LOGI_2(fmt, a, b)        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b)
#  define LOGI_3(fmt, a, b, c)     MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c)
#  define LOGI_4(fmt, a, b, c, d)      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d)
#  define LOGI_5(fmt, a, b, c, d, e)   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d, e)
#  define LOGI_6(fmt, a, b, c, d, e, f)  MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d, e, f)
# endif 
#endif
#else // ANDROID
# define LOG_TAG "libqcproximity"
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
# define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
// fixed-argument macros for qdsp6 compatibility
# define LOGI_0(fmt)            LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__)
# define LOGI_1(fmt, a)         LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a)
# define LOGI_2(fmt, a, b)        LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a, b)
# define LOGI_3(fmt, a, b, c)     LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a, b, c)
# define LOGI_4(fmt, a, b, c, d)      LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a, b, c, d)
# define LOGI_5(fmt, a, b, c, d, e)   LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a, b, c, d, e)
# define LOGI_6(fmt, a, b, c, d, e, f)  LOGI("%s (%d): " fmt , __FUNCTION__, __LINE__, a, b, c, d, e, f)
#endif


#ifndef __hexagon__
#define Q6_P_mpy_RR(a, b)         (((int64) a)*((int64) b))
#define Q6_P_mpyacc_RR(a, b, c)   (((int64) a) + ((int64) b)*((int64) c))
#define Q6_R_sat_P(a)             ((a) > 0x7FFFFFFF ? 0x7FFFFFFF: a)
#endif

#define mult_with_cast32(a, b)         (((int) a)*((int) b))

typedef enum
{
  GENERAL_STATE_INVALID = 0,
  GENERAL_STATE_INIT,
  GENERAL_STATE_WARMUP,
  GENERAL_STATE_SYNC_CHECK,
  GENERAL_STATE_WORKING,
  GENERAL_STATE_MAX = 0Xff
} QCUsProximityLibGeneralStateType;

typedef enum
{
  NO_PROXIMITY = 0,
  PROXIMITY_DETECTED
} ProximityDetectorOutputType;


// cgf types
typedef struct
{
  int                              Bshift;
  int                              b0;
  int                              b1;
  int                              b2;
  int                              a1;
  int                              a2;
} HpfCfgType;

typedef struct
{
  int                              ExtendWindow;
  int                              DetectorDilution;
  int                              PsdLpfBitShift;
  int                              FrameMax2MeanRatioTh;
  int                              DetectMax2MeanRatioTh;
  int                              ReleaseMax2MeanRatioTh;
  int                              FrequencyLpfBitShift;
  int                              ReseaseCounterMax;
  int                              NotchPoleAmp16;
  int                              NotchPoleAmp12;
  int                              NotchPoleAmp16SQ;
  int                              NotchBshift;
} JammerCfgType;

typedef struct
{
  int                              DiffPowerMinIndex;
  int                              DiffPowerMaxIndex;
  int                              DiffMaxPowerThreshold;
} ProximityDetectorCfgType;


typedef struct
{
  int                              CompatibilityVersion;
  // rx
  int                              RxFrequencyStartHz;
  int                              RxBandwithHz;
  int                              RxMaxAmplitude;
  int                              RxRampLog2;
  // general
  int                              CorrelationDelay;
  int                              NumFramesInBuffer;
  int                              CorrelationLength;
  // jammer
  JammerCfgType                    JammerCfg;
  int                              SerialVersion;
} ProximityCfgType;

// data types
typedef struct
{
  int16                            x1;
  int16                            x2;
  int                              y1;
  int                              y2;
} HpfStateType;

typedef struct
{
  int16                            x1;
  int16                            x2;
  int                              y1;
  int                              y2;
} JammerRemoverStateType;

typedef struct
{
  JammerRemoverStateType           JammerRemoverState[NUM_MICS];
  int                              FirstTone;
  int                              NumTones;
  int                              SumPowerDivider;
  int                              DilutionCounter;
  int64                            PsdLpf[RX_PATTERN_LEN_HALF + 1];
  boolean                          EnableRemover;
  int                              ReseaseCounter;
  int                              Frequency;
  int                              FrequencyLpf;
  int                              FrequencyErr;
  int16                            b;
  int16                            a;
} JammerStateType;

typedef struct
{
  int                              *Data[NUM_MICS];
  boolean                          *Valid;
  int                              Index;
  int                              Size;
} CorrelationsBufferType;


/**
  Gesture library work structure
*/
typedef struct
{
  uint8                            *pScratch;                         // Pointer to beginning of work memory (moving pointer)
  uint8                            *pScratchStart;                    // Pointer to beginning of work memory (constant pointer)
  uint8                            *pWorkStateScratchStart;           // Pointer to beginning of work state memory (constant pointer)
  uint32                           iWorkStateMemSize;                 // Total work state scratch memory size
  uint32                           iPrevFrameNum;                     // Previous frame number
  uint32                           iTotalFrames;                      // Total number of frames so far
  uint32                           iWorkingFramesCount;               // Number of currently working frames

  int                              *GeneralFftIn;                     // alignment to 8*RX_PATTERN_LEN bytes
  int                              *GeneralFftOut;                    // alignment to 8 bytes

  uint32                           MissedFramesNum;

  int                              SpikeDetected;

  int                              iPrintCnt;

  QCUsProximityLibGeneralStateType   GeneralState;
  int                              WaitCount;

  int16                            piPatternRx[RX_PATTERN_LEN];
  int16                            *PatternRfft[NUM_MICS];

  CorrelationsBufferType           CorrelationsBuffer;

  int                              RxFrequencyStartHz;
  int                              RxBandwithHz;
  int                              RxMaxAmplitude;
  int                              RxRampLog2;
  int                              CorrelationDelay;
  int                              NumFramesInBuffer;
  int                              CorrelationLength;

  int                              FirstTone;
  int                              NumTones;

  //Hpf
  HpfCfgType                       HpfCfg;
  HpfStateType                     HpfState[NUM_MICS];

  //Jammer
  JammerCfgType                    JammerCfg;
  JammerStateType                  JammerState;

  //Proximity
  int                              *BBCorrRef[NUM_MICS];

  int                              BBCorrPowerMaxLpf;

  int                              BBCorrDiffPowerMaxIndexPrev;
  int                              BBCorrDiffPowerMaxIndexDiffPowerLpf;

  int                              BBCorrDiffRefPowermMaxIndex;
  int                              DiffPowerTh[RX_PATTERN_LEN];
  int                              DiffPowerThHysteresis[RX_PATTERN_LEN];
  int                              BBCorrDiffRefPowerCounterLpf;

  int                              DistDiffPowerMaxIndex;
  int                              DistDiffPowerThMin[RX_PATTERN_LEN];
  int                              DistIndexLpf;
  int                              DistValidLpf;
  int                              DistanceInMm;

  int                              SaturationConter;

  boolean                          ProximityState;
  boolean                          CoveredState;

  //dbg
  int                              BBCorrDiffPowerMaxIndex;
  int                              BBCorrDiffRefPowerCounter;

#ifdef MATLAB
  int                              piPatternTx[RX_PATTERN_LEN];
#endif

#ifdef MATLABDBG
  int                              DebugBuf[2*RX_PATTERN_LEN];
#endif
} QcUsProximityLibDataType;

/**
  Proximity frame structure
*/
typedef struct
{
  uint32   frameHeader;
  uint32   frameLength;
  uint32   frameNumber;
  int16   frameData[NUM_MICS][RX_PATTERN_LEN];
} QcUsProximityLibTxFrameType;

/*----------------------------------------------------------------------------
  Forward declrations
----------------------------------------------------------------------------*/
void fft(CWord2x16 *input, int N, CWord2x16 *output);
void us_rfft16(Word16 *input, int N, CWord2x16 *output);
void us_fft32(CWord2x32 *input, int N, CWord2x32 *output);
void us_rfft32(Word32 *input, int N, CWord2x32 *output);
Word64 us_dsplib_approx_divide(Word32 numer,Word32 denom);
Word64 us_dsplib_approx_invert(Word32 input);
Word64 us_dsplib_newtons_invert(Word32 input);
Word32 us_dsplib_sin(Word32 x);
Word32 us_dsplib_cos(Word32 x);
#ifdef __hexagon__
#define cl0_ll    Q6_R_cl0_P
#else
Word32 cl0_ll(Word64 x);
#endif

size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

void randomOfdm(int frequency_start_hz, int bandwith_hz, int frequency_sampling_hz, int rx_pattern_len_log2,
          int rx_max_amplitude, int16 * pattern, int * pFirstTone, int * pNumTones);

void chirp(int frequency_start_hz, int bandwith_hz, int frequency_sampling_hz, int rx_pattern_len_log2,
          int ramp_log2, int rx_max_amplitude, int16 * pattern, int * pFirstTone, int * pNumTones);

void gauss(int frequency_start_hz, int bandwith_hz, int frequency_sampling_hz, int rx_pattern_len_log2,
          int quad_m, int rx_max_amplitude, int16 * pattern, int * pFirstTone, int * pNumTones);

void BBCorrelation(int16 *Input, int16 *PatternRfft , int FirstTone, int NumTones, int *Output);
boolean BBCorrelation16(int16 *Input, int16 *PatternRfft , int FirstTone, int NumTones, int bitShift, int *Output);
void PowerOfComplex(int *Input, int *Output, int OutputLenght, int ShiftBits);
void PowerOfComplexWithRamp(int *Input, int *Output, int OutputLenght, int RampDoubleLog2, int ShiftBits);
int FindIndexOfMax(int *ipBuff, int iBuffLen);
#ifdef __hexagon__
int FindIndexOfMax64(int *ipBuff, int iBuffLen);
#else
#define FindIndexOfMax64	FindIndexOfMax
#endif
int FindIndexOfFirstAboveThreshold(int *ipBuff, int iBuffLen, int threshold);
void LPFilter(int *Input, int *Output, int OutputLenght, int InputShiftBits, int OutputShiftBits);

void CopyCorrelationToBuffer(CorrelationsBufferType *pCorrelationsBuffer, boolean checkSync);
boolean ProximityDetector(CorrelationsBufferType *pCorrelationsBuffer);

// Jammer
void JammerInit(JammerCfgType *pJammerCfg, JammerStateType *pJammerState);
int FractionalFrequencyCalculator(int *Fft3points, int BitShift);
int16 NotchCoefCalculator(int Frequency, int16 NotchPoleAmp16);
void JammerDetector(int16 *Data, JammerCfgType *pJammerCfg, JammerStateType *pJammerState, boolean *CorrelationsBufferValid);
void JammerRemover(int16 *Data, int DataLenght, JammerCfgType *pJammerCfg, JammerRemoverStateType *pJammerRemoverState,
                                                                                         JammerStateType *pJammerState);

// Hpf
void Hpf(int16 *Data, int DataLenght, HpfCfgType *pHpfCfg, HpfStateType *pHpfState);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef QC_US_PROXIMITY_LIB_H */
