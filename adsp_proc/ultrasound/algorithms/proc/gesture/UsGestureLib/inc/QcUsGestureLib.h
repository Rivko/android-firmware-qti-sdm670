
#ifndef QC_US_GESTURE_LIB_H
#define QC_US_GESTURE_LIB_H

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
#if !defined(US_STANDALONE_ALG_TESTS)
// compiling regular LPASS image
#include "qurt_elite.h"
#include "stringl.h"
#else
// compiling standalone algorithm tests with hexagon simulator
#include <hexagon_protos.h>
#endif
#else // not hexagon
// safe memcpy function, implemented for non-hexagon builds
size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);
#endif

#include "QcUsComDef.h"
#include "QcUsGestureLibApi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
// General
#define CONFIG_COMPATIBILITY_VERSION                             3
#define RX_PATTERN_LEN_LOG2                                      10
#define RX_PATTERN_LEN                                           (1 << RX_PATTERN_LEN_LOG2)
#define RX_PATTERN_LEN_HALF                                      (1 << (RX_PATTERN_LEN_LOG2 - 1))
#define NUM_MICS                                                 4
#define SELECT_GESTURE_DELAY_FOR_RECORDING                       20
#define STATE_WARMUP_DURATION                                    20

// Rx Pattern
#define FS_HZ                                                    192000

// Correlation
#define MAX_NUM_FRAMES_IN_BUFFER                                 256
#define MAX_CORRELATION_LENGTH                                   RX_PATTERN_LEN_HALF

//Jammer
#define JAMMER_FREQUENCY_ERR_MAX                                 (1 << (32 - RX_PATTERN_LEN_LOG2))

//trigger
//#define TRIGGER_MIC_INDEX                                        2

//trigger lrud
#define TRIGGER_MOTION_CORRELATION_POWER_SHIFT                   16
#define MAX_TRIGGER_MOTION_RATIO_DELAY                           32
#define MAX_TRIGGER_LRUD_WINDOW_LENGTH                           MAX_NUM_FRAMES_IN_BUFFER

//trigger select
#define TRIGGER_PROXIMITY_CORRELATION_POWER_SHIFT                24

//direction recognition
#define RECOGNITION_FIT_CURVE_NUM_COEF                           4
#define RECOGNITION_FIT_CURVE_NUM_IT                             20
#define RECOGNITION_FIT_CURVE_CG_NUM_IT                          5
#define RECOGNITION_FIT_CURVE_CG_F_ATT_LOG2                      8
#define RECOGNITION_FIT_CURVE_COEF_SCALE_LOG2                    6
#define RECOGNITION_FIT_CURVE_LMS_ALPHA_LOG2                     2
#define RECOGNITION_FIT_CURVE_R_BS_MAX                           ((1 << (31-(RECOGNITION_FIT_CURVE_COEF_SCALE_LOG2-RECOGNITION_FIT_CURVE_LMS_ALPHA_LOG2)))-1)
#define RECOGNITION_FIT_CURVE_COEF_3_REGULARIZATION              (1 << (2*RECOGNITION_FIT_CURVE_COEF_SCALE_LOG2))
#define RECOGNITION_LOGISTIC_REG_FEATURE0_LOG2                   8
#define RECOGNITION_LOGISTIC_REG_NUM_FEATURES                    13

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
# define LOGI_0(fmt)            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt)
# define LOGI_1(fmt, a)         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a)
# define LOGI_2(fmt, a, b)        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b)
# define LOGI_3(fmt, a, b, c)     MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c)
# define LOGI_4(fmt, a, b, c, d)      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d)
# define LOGI_5(fmt, a, b, c, d, e)   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d, e)
# define LOGI_6(fmt, a, b, c, d, e, f)  MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, fmt, a, b, c, d, e, f)
#endif
#else // ANDROID
# define LOG_TAG "libqcgesture"
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
  GENERAL_STATE_SPIKES_CHECK,
  GENERAL_STATE_SYNC_CHECK,
  GENERAL_STATE_WORKING,
  GENERAL_STATE_MAX = 0Xff
} QCUsGestureLibGeneralStateType;

typedef enum
{
  GESTURE_STATE_INVALID = 0,
  GESTURE_STATE_LOOK_FOR_TRIGGER,
  GESTURE_STATE_WAIT_SELECT_OFF,
  GESTURE_STATE_WAIT_AFTER_LRUD,
  GESTURE_STATE_MAX = 0Xff
} QCUsGestureLibGestureStateType;

typedef enum
{
  NO_PROXIMITY = 0,
  PROXIMITY_DETECTED
} ProximityDetectorOutputType;

typedef enum
{
  SILENT = 0,
  NOISY,
  MOTION
} MotionDetectorOutputType;

// cgf types
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
  ProximityDetectorCfgType         ProximityDetectorCfg[NUM_MICS];
  int                              OldCorrDelayOn;
  int                              OldCorrDelayOff;
  int                              ResetCriteriaWord1;
  int                              IncrementCriteriaWord1;
  int                              ResetCriteriaWord2;
  int                              IncrementCriteriaWord2;
  int                              LpfTh;
  int                              ConsecutiveProximityOnTh;
  int                              ConsecutiveProximityOffTh;
} SelectTriggerCfgType;

typedef struct
{
  int                              DiffPowerMaxIndex;
  int                              DiffMaxPowerThreshold;
} MotionDetectorPerMicCfgType;

typedef struct
{
  int                              RatioDelay;
  int                              IndexLpfShift;
  int                              IndexLpfTh;
} MotionDetectorCommonCfgType;

typedef struct
{
  MotionDetectorPerMicCfgType      MotionDetectorPerMicCfg[NUM_MICS];
  MotionDetectorCommonCfgType      MotionDetectorCommonCfg;
  int                              MinNumMicsMotionDetection;
  int                              MinNumMicsSilentDetection;
  int                              FilterLength;
  int                              FilterTh;
  int                              SilentBeforeMotionDuration;
  int                              SilentAfterMotionDuration;
  int                              MotionSumMax;
} LRUDTriggerCfgType;

typedef struct
{
  int                              RampDoubleLog2;
  int                              ShortRangeIndexMax;
  int                              ValidThresholdMin;
  int                              MaxShiftForMiddle;
  int                              MaxPowerShiftForTh;
  int                              ThresholdMax;
  int                              ParabolaLimitShift;
  int                              ParabolaLimitOffset;
  int                              IndexMinLimit;
  int                              ValidIndexMax;
} DirectionRecognitionPerMicCfgType;

typedef struct
{
  int                              DpRemoveFramesLog2;
  int                              Dummy;
  int                              CorrelationPowerShift;
  int                              FitCurveCoefInit[RECOGNITION_FIT_CURVE_NUM_COEF];
  int                              FitCurveCoefMin[RECOGNITION_FIT_CURVE_NUM_COEF];
  int                              FitCurveCoefMax[RECOGNITION_FIT_CURVE_NUM_COEF];
  int                              FitCurveCoef0ValidMax;
  int                              FitCurveCoef2ValidMax;
  int                              MaxCenterDetlaTh;
  int                              HalfScreenPatchthCoef0ValidMax;
  int                              HalfScreenPatchthCoef1ValidMax;
  int                              HalfScreenPatchthCoef2ValidMax;
  int                              LogReg[4*RECOGNITION_LOGISTIC_REG_NUM_FEATURES];
  int                              DirectionPriorityLow;
  int                              DirectionPriorityHigh;
} DirectionRecognitionCommonCfgType;

typedef struct
{
  DirectionRecognitionPerMicCfgType  DirectionRecognitionPerMicCfg[NUM_MICS];
  DirectionRecognitionCommonCfgType  DirectionRecognitionCommonCfg;
} DirectionRecognitionCfgType;


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
  // state machine
  int                              StateMachineConsecutiveProximityMotionOffTh;
  int                              StateMachineConsecutiveProximityMotionOnTh;
  int                              StateMachineWaitAfterLrudDuration;
  //trigger select
  SelectTriggerCfgType             SelectTriggerCfg;
  //trigger lrud
  LRUDTriggerCfgType               LRUDTriggerCfg;
  //direction recognition
  DirectionRecognitionCfgType      DirectionRecognitionCfg;

  int                              SerialVersion;
} GestureCfgType;

// data types
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

typedef struct
{
  int                              ConsecutiveProximityCounter;
  int                              ConsecutiveProximityCounterAuxiliary[2];
} SelectTriggerDataType;

typedef struct
{
  int                              BBCorrDiffPowerMaxIndexPrev;
  int                              BBCorrDiffPowerMaxIndexDiffPowerLpf;
  int                              PowerRatioDelayBufferIndex;
  boolean                          PowerRatioMotionDelayBuffer[MAX_TRIGGER_MOTION_RATIO_DELAY];
  boolean                          PowerRatioSilentDelayBuffer[MAX_TRIGGER_MOTION_RATIO_DELAY];
} MotionDetectorDataType;

typedef struct
{
  MotionDetectorDataType           MotionDetectorData[NUM_MICS];
  int                              MotionDecisionWindowIndex;
  boolean                          MotionDecisionMotionWindow[MAX_TRIGGER_LRUD_WINDOW_LENGTH];
  boolean                          MotionDecisionSilentWindow[MAX_TRIGGER_LRUD_WINDOW_LENGTH];
  int                              MotionDecisionFilterWindow[MAX_TRIGGER_LRUD_WINDOW_LENGTH];
} LRUDTriggerDataType;



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

  uint32                           MissedFramesNum;

  int                              SpikeDetected;

  int                              iPrintCnt;

  int                              IsBusy;

  QCUsGestureLibGeneralStateType   GeneralState;
  int                              WaitCount;

  QCUsGestureLibGestureStateType   GestureState;

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
  int                              StateMachineConsecutiveProximityMotionOffTh;
  int                              StateMachineConsecutiveProximityMotionOnTh;
  int                              StateMachineWaitAfterLrudDuration;

  int                              FirstTone;
  int                              NumTones;

  //Jammer
  JammerCfgType                    JammerCfg;
  JammerStateType                  JammerState;

  // Trigger
  SelectTriggerCfgType             SelectTriggerCfg;
  SelectTriggerDataType            SelectTriggerData;
  LRUDTriggerCfgType               LRUDTriggerCfg;
  LRUDTriggerDataType              LRUDTriggerData;

  // Recognition
  DirectionRecognitionCfgType      DirectionRecognitionCfg;
  int                              NumFramesInRecognitionBuffer;
  int                              speed;

  int                              directionPriority[4];

#ifdef MATLABDBG
  int                              DebugBuf[NUM_MICS][2*RX_PATTERN_LEN];
  int                              DebugBufCoef[NUM_MICS][RECOGNITION_FIT_CURVE_NUM_COEF];
  int                              DebugLRUDTrigger;
#endif
} QcUsGestureLibDataType;

/**
  Proximity frame structure
*/
typedef struct
{
  uint32   frameHeader;
  uint32   frameLength;
  uint32   frameNumber;
  int16   frameData[NUM_MICS][RX_PATTERN_LEN];
} QcUsGestureLibTxFrameType;

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

void Gesture_gauss(int frequency_start_hz, int bandwith_hz, int frequency_sampling_hz, int rx_pattern_len_log2,
          int quad_m, int rx_max_amplitude, int16 * pattern , int * pattern_ifft, int * pattern_fft,
          int * pFirstTone, int * pNumTones);
void Gesture_chirp(int frequency_start_hz, int bandwith_hz, int frequency_sampling_hz, int rx_pattern_len_log2,
          int ramp_log2, int rx_max_amplitude, int16 * pattern, int * pFirstTone, int * pNumTones);

void Gesture_BBCorrelation(int16 *Input, int16 *PatternRfft , int FirstTone, int NumTones, int *Output);
boolean Gesture_BBCorrelation16(int16 *Input, int16 *PatternRfft , int FirstTone, int NumTones, int bitShift, int *Output);
void Gesture_PowerOfComplex(int *Input, int *Output, int OutputLenght, int ShiftBits);
int Gesture_FindIndexOfMax(int *ipBuff, int iBuffLen);
#ifdef __hexagon__
int Gesture_FindIndexOfMax64(int *ipBuff, int iBuffLen);
#else
#define Gesture_FindIndexOfMax64	Gesture_FindIndexOfMax
#endif
int Gesture_FindIndexOfFirstAboveThreshold(int *ipBuff, int iBuffLen, int threshold);
int FindIndexOfMiddle(int *ipBuff, int iBuffLen, int maxShift);
void Gesture_LPFilter(int *Input, int *Output, int OutputLenght, int InputShiftBits, int OutputShiftBits);
int64 Inverse3x3Matrix(int64 *a, int64 *b);

int GestureConfig(GestureCfgType *GestureCfg, JammerCfgType *pJammerCfg, SelectTriggerCfgType *pSelectTriggerCfg,
                   LRUDTriggerCfgType *pLRUDTriggerCfg, DirectionRecognitionCfgType *pDirectionRecognitionCfg);

void SelectTriggerInit(SelectTriggerDataType *pSelectTriggerData);
void LRUDTriggerInit(LRUDTriggerDataType *pLRUDTriggerData);
void GestureStateMachineInit(void);
QcUsGestureLibResultType GestureStateMachine(void);

void SyncCheck(void);
void Gesture_CopyCorrelationToBuffer(CorrelationsBufferType *pCorrelationsBuffer);
ProximityDetectorOutputType Gesture_ProximityDetector(int *BBCorr,  int *BBCorrOld, ProximityDetectorCfgType *pProximityDetectorCfg);
boolean SelectTriggerOn(CorrelationsBufferType *pCorrelationsBuffer, MotionDetectorDataType *MotionDetectorData,
                                          SelectTriggerDataType *pSelectTriggerData, SelectTriggerCfgType *pSelectTriggerCfg);
boolean SelectTriggerOff(CorrelationsBufferType *pCorrelationsBuffer,
                                          SelectTriggerDataType *pSelectTriggerData, SelectTriggerCfgType *pSelectTriggerCfg);

MotionDetectorOutputType MotionDetector(int *BBCorr,  int *BBCorrPrev, MotionDetectorDataType *pMotionDetectorData,
                MotionDetectorPerMicCfgType *pMotionDetectorPerMicCfg, MotionDetectorCommonCfgType *pMotionDetectorCommonCfg);
boolean LRUDTrigger(CorrelationsBufferType *pCorrelationsBuffer, LRUDTriggerDataType *pLRUDTriggerData,
                                                                 LRUDTriggerCfgType *pLRUDTriggerCfg);

// Recognition
QcUsGestureLibResultType  DirectionRecognition(CorrelationsBufferType *pCorrelationsBuffer,
                                               DirectionRecognitionCfgType *pDirectionRecognitionCfg);
void Gesture_PowerOfComplexWithRamp(int *Input, int *Output, int OutputLenght, int RampDoubleLog2, int ShiftBits);
void CorrelationPowerNoDp(int *CorrelationsBufferData, int CorrelationsBufferIndex,
                          DirectionRecognitionPerMicCfgType *pDirectionRecognitionPerMicCfg,
                          DirectionRecognitionCommonCfgType * pDirectionRecognitionCommonCfg, int micIndex);
void FindFirstArrival(int *CorrelationsBufferData, int CorrelationsBufferIndex, int *firstArrivalIndex,
                      int *pframeIndexOfMiddle, DirectionRecognitionPerMicCfgType *pDirectionRecognitionPerMicCfg,
                      DirectionRecognitionCommonCfgType * pDirectionRecognitionCommonCfg, int micIndex);
void RegressionFunction(int *x, int *c, int num_points, int *yr, int *Derivative);
void ConjugateGradient(int *A, int *r, int *h);
boolean FitCurve(int *firstArrivalIndex, int frameIndexOfMiddle, int *coefficients,
                 DirectionRecognitionCommonCfgType * pDirectionRecognitionCommonCfg, int micIndex);
int LogisticRegression(int *coefficients, boolean *valid, DirectionRecognitionCommonCfgType * pDirectionRecognitionCommonCfg);
void UpdateSpeed(int *coefficients, boolean *valid, DirectionRecognitionCommonCfgType * pDirectionRecognitionCommonCfg);

// Jammer
void Gesture_JammerInit(JammerCfgType *pJammerCfg, JammerStateType *pJammerState);
int Gesture_FractionalFrequencyCalculator(int *Fft3points, int BitShift);
int16 Gesture_NotchCoefCalculator(int Frequency, int16 NotchPoleAmp16);
void Gesture_JammerDetector(int16 *Data, JammerCfgType *pJammerCfg, JammerStateType *pJammerState, boolean *CorrelationsBufferValid);
void Gesture_JammerRemover(int16 *Data, int DataLenght, JammerCfgType *pJammerCfg, JammerRemoverStateType *pJammerRemoverState,
                                                                                         JammerStateType *pJammerState);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef QC_US_GESTURE_LIB_H */
