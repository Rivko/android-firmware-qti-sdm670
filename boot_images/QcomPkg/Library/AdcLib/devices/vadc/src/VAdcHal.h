#ifndef __VADCHAL_H__
#define __VADCHAL_H__
/*============================================================================
  @file VAdcHal.h

  Function and data structure declarations for VADC HAL


                Copyright (c) 2012, 2015, 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/VAdcHal.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "VAdcBsp.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_INVALID_ADC_CODE 0x8000

#define VADC_INT_MASK_EOC                 0x1
#define VADC_INT_MASK_VBAT_MIN_THRESHOLD  0x2

#define VADC_HAL_NUM_REGISTERS_TO_DUMP 29

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   VADC_HAL_SUCCESS = 0,
   VADC_HAL_ERROR
} VAdcHalResultType;

typedef struct
{
   uint16 usDigitalRev;
   uint16 usAnalogRev;
   uint8 ucType;
   uint8 ucSubType;
} VAdcRevisionInfoType;

typedef enum
{
   VADC_CONVERSION_STATUS_INVALID = 0,
   VADC_CONVERSION_STATUS_COMPLETE,
   VADC_CONVERSION_STATUS_PENDING,
   VADC_CONVERSION_STATUS_WAITING
} VAdcConversionStatusType;

typedef enum
{
   VADC_CONVERSION_STATE_IDLE = 0,
   VADC_CONVERSION_STATE_WAIT_HOLDOFF_RST,
   VADC_CONVERSION_STATE_WAIT_HOLDOFF,
   VADC_CONVERSION_STATE_RST_CH_ARB,
   VADC_CONVERSION_STATE_GET_NEXT_CH,
   VADC_CONVERSION_STATE_WAIT_CAL,
   VADC_CONVERSION_STATE_WAIT_ADC_EOC,
   VADC_CONVERSION_STATE_GET_IRQ
} VAdcConversionStateType;

typedef struct
{
   VAdcConversionStatusType eConversionStatus;
   VAdcConversionStateType eConversionState;
} VAdcStatusType;

typedef enum
{
   VADC_INTERRUPT_CONFIG_LEVEL_HIGH = 0,
   VADC_INTERRUPT_CONFIG_LEVEL_LOW,
   VADC_INTERRUPT_CONFIG_RISING_EDGE,
   VADC_INTERRUPT_CONFIG_FALLING_EDGE
} VAdcInterruptConfigType;

typedef struct
{
   uint32 uChannel;
   VAdcCalMethodType eCalMethod;
   VAdcDecimationRatioType eDecimationRatio;
   VAdcAverageModeType eAverageMode;
   VAdcSettlingDelay eSettlingDelay;
} VAdcConversionParametersType;

typedef enum
{
   VADC_DISABLE = 0,
   VADC_ENABLE
} VAdcEnableType;

typedef struct
{
   uint8 ucOffset;
   uint8 ucVal;
} VAdcHalRegType;

typedef struct
{
   VAdcHalRegType aVAdcReg[VADC_HAL_NUM_REGISTERS_TO_DUMP];
} VAdcHalRegDumpType;

typedef struct
{
   void *pCtxt;
   VAdcHalResultType (*pfnWriteBytes)(void *pCtxt, uint32 uRegisterAddress, uint8 *pucData, uint32 uDataLen);
   VAdcHalResultType (*pfnReadBytes)(void *pCtxt, uint32 uRegisterAddress, uint8 *pucData, uint32 uDataLen);
} VAdcHalInterfaceType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
VAdcHalResultType VAdcHalGetRevisionInfo(VAdcHalInterfaceType *piVAdcHal, VAdcRevisionInfoType *pRevisionInfo);
VAdcHalResultType VAdcHalGetStatus(VAdcHalInterfaceType *piVAdcHal, VAdcStatusType *pStatus);
VAdcHalResultType VAdcHalConfigInterrupts(VAdcHalInterfaceType *piVAdcHal, uint8 ucIntrMask, VAdcInterruptConfigType eConfig);
VAdcHalResultType VAdcHalGetPendingInterrupts(VAdcHalInterfaceType *piVAdcHal, uint8 *pucIntrMask);
VAdcHalResultType VAdcHalClearInterrupts(VAdcHalInterfaceType *piVAdcHal, uint8 ucIntrMask);
VAdcHalResultType VAdcHalEnableInterrupts(VAdcHalInterfaceType *piVAdcHal, uint8 ucIntrMask);
VAdcHalResultType VAdcHalSetInterruptMid(VAdcHalInterfaceType *piVAdcHal, uint32 uIntrMid);
VAdcHalResultType VAdcHalSetEnable(VAdcHalInterfaceType *piVAdcHal, VAdcEnableType eEnable);
VAdcHalResultType VAdcHalRequestConversion(VAdcHalInterfaceType *piVAdcHal, VAdcConversionParametersType *pParams, uint32 *puConvTimeUs);
VAdcHalResultType VAdcHalGetConversionCode(VAdcHalInterfaceType *piVAdcHal, uint32 *puCode);
VAdcHalResultType VAdcHalDumpRegisters(VAdcHalInterfaceType *piVAdcHal, VAdcHalRegDumpType *pVAdcRegDump);

#endif /* #ifndef __VADCHAL_H__ */

