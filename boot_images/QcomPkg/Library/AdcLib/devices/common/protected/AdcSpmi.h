#ifndef __ADC_SPMI_H__
#define __ADC_SPMI_H__
/*============================================================================
  @file AdcSpmi.h

  Function and data structure declarations for ADC SPMI wrapper


                Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/common/protected/AdcSpmi.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "SpmiBus.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define ADC_SPMI_ADDR(perph, reg) ((((perph) << 8) & 0xff00) | ((reg) & 0xff))

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   uint32 uSlaveId;
   uint32 uPeripheralId;
   Spmi_AccessPriority ePriority;
} AdcSpmiCtxtType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
static __inline DALResult
AdcSpmiInit(AdcSpmiCtxtType *pCtxt, Spmi_AccessPriority ePriority, uint32 uSlaveId, uint32 uPeripheralId)
{
   Spmi_Result ret;

   pCtxt->uSlaveId = uSlaveId;
   pCtxt->uPeripheralId = uPeripheralId;
   pCtxt->ePriority = ePriority;

   ret = SpmiBus_Init();
   if (ret != SPMI_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

static __inline DALResult
AdcSpmiDeinit(AdcSpmiCtxtType *pCtxt)
{
   return DAL_SUCCESS;
}

static __inline DALResult
AdcSpmiRead(AdcSpmiCtxtType *pCtxt, uint32 uReg, uint8 *pucData, uint32 uLen, uint32 *puBytesRead)
{
   Spmi_Result ret;

   ret = SpmiBus_ReadLong(pCtxt->uSlaveId,
                          pCtxt->ePriority,
                          ADC_SPMI_ADDR(pCtxt->uPeripheralId, uReg),
                          pucData,
                          uLen,
                          puBytesRead);
   if (ret != SPMI_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

static __inline DALResult
AdcSpmiWrite(AdcSpmiCtxtType *pCtxt, uint32 uReg, uint8 *pucData, uint32 uLen)
{
   Spmi_Result ret;

   ret = SpmiBus_WriteLong(pCtxt->uSlaveId,
                           pCtxt->ePriority,
                           ADC_SPMI_ADDR(pCtxt->uPeripheralId, uReg),
                           pucData,
                           uLen);
   if (ret != SPMI_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

static __inline DALResult
AdcSpmiRegisterInterrupt(AdcSpmiCtxtType *pCtxt, uint8 ucMask, const SpmiIsr isr, const void *pIsrCtx)
{
   return DAL_ERROR;
}

static __inline DALResult
AdcSpmiReadAddr(AdcSpmiCtxtType *pCtxt, uint32 uAddr, uint8 *pucData, uint32 uLen, uint32 *puBytesRead)
{
   Spmi_Result ret;

   ret = SpmiBus_ReadLong(pCtxt->uSlaveId,
                          pCtxt->ePriority,
                          uAddr,
                          pucData,
                          uLen,
                          puBytesRead);
   if (ret != SPMI_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

#endif /* #ifndef __ADC_SPMI_H__ */

