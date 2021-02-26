#ifndef __TSENS_BSP_H__
#define __TSENS_BSP_H__
/*============================================================================
  @file TsensBsp.h

  Tsens BSP file.

                Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_THRESHOLD_DISABLED 0x7fff
#define TSENS_MAX_NUM_CHANNELS_PER_CONTROLLER 16
#define TSENS_MAX_NUM_TBCB_CLIENTS_PER_CONTROLLER 16

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   TSENS_BSP_SENSOR_CAL_NORMAL = 0,        /* Normal method, i.e. use QFPROM if
                                            * avail else use default char data */
   TSENS_BSP_SENSOR_CAL_IGNORE_DEVICE_CAL  /* Force using default char data */
} TsensBspSensorCalType;

typedef struct
{
   uint8 *pucTsensSROTPhys;    /* SROT region physical base address */
   uint8 *pucTsensTMPhys;      /* TM region physical base address */
   const uint32 *pauTbcbReqs;  /* Bitmask of sensors to request for TBCB client */
   uint32 uTsensSROTSize;      /* Size of SROT region */
   uint32 uTsensTMSize;        /* Size of TM region */
   uint32 uNumTbcbClients;     /* Number of TBCB clients */
   uint32 uPeriodActive;       /* Active period */
   uint32 uPeriodSleep;        /* Sleep period */
   uint32 uTSControl;          /* Analog TS control setting */
   uint32 uTSConfig;           /* Analog TS config setting */
   uint32 uSidebandEnMask;     /* Mask of sideband sensors to enable */
   uint32 uWatchdogLoadVal;    /* Load value for watchdog in sleep clock ticks */
   DALBOOL bPSHoldResetEn;     /* Whether to enable PS_HOLD reset */
   DALBOOL bAutoAdjustPeriod;  /* Whether to adjust period on sleep */
   DALBOOL bStandAlone;        /* Whether this is a stand alone controller */
   DALBOOL bPWMEn;             /* Whether to enable PWM */
   DALBOOL bWatchdogEn;        /* Whether to enable the watchdog */
   DALBOOL bWatchdogResetEn;   /* Whether or not watchdog can perform TSENS resets */
} TsensControllerCfgType;

typedef struct
{
   TsensBspSensorCalType eCal;        /* Which cal type to use */
   int16 nsCalPoint1CodeDefault;      /* Default TSENS code at calibration point nCalPoint1DeciDegC */
   int16 nsCalPoint2CodeDefault;      /* Default TSENS code at calibration point nCalPoint2DeciDegC */
   int16 nsThresholdMinDeciDegC;      /* Minimum temperature threshold for critical shutdown
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdLowerDeciDegC;    /* Default lower threshold for software interrupt or
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdUpperDeciDegC;    /* Default upper threshold for software interrupt or
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdCriticalDeciDegC; /* Default critical threshold for software interrupt or
                                       * use TSENS_THRESHOLD_DISABLED to disable */
   int16 nsThresholdMaxDeciDegC;      /* Maximum temperature threshold for critical shutdown
                                       * or use TSENS_THRESHOLD_DISABLED to disable */
   uint8 ucController;                /* Controller index */
   uint8 ucChannel;                   /* Channel index */
   uint8 ucSensorID;                  /* Sensor ID for TBCB */
   uint8 ucFuseIdx;                   /* Which fuse corresponds to this sensor */
} TsensSensorCfgType;

typedef struct
{
   const TsensControllerCfgType *paControllerCfgs;  /* Array of controller configs */
   const TsensSensorCfgType *paSensorCfgs;          /* Array of sensor configs */
   uint32 uNumControllers;                          /* Number of controllers */
   uint32 uNumSensors;                              /* Number of sensors */
   uint32 uSensorConvTime_us;                       /* Sensor conversion time in us */
   int32 nCalPoint1DeciDegC;                        /* Calibration point 1 in deci deg C */
   int32 nCalPoint2DeciDegC;                        /* Calibration point 2 in deci deg C */
   uint32 uShift;                                   /* Shift value */
} TsensBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef __TSENS_BSP_H__ */

