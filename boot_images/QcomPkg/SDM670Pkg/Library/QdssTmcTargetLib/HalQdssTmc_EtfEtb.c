/*=============================================================================

FILE:         HalQdssTmc_EtfEtb.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include "HalQdssTmc_EtfEtb.h"
#include "HwioQdssTmc_EtfEtb.h"
#include "HalQdssTmcConfig.h"

volatile uint32 QDSS_ETFETB_ETFETB_CXTMC_F128W8K_REG_BASE;

void HAL_qdss_etfetb_HalInit()
{
  QDSS_ETFETB_ETFETB_CXTMC_F128W8K_REG_BASE  = QDSS_ETFETB_BASE_PHYS;
}

/* FUNCTION HAL_qdss_etfetb_AccessUnLock*/
void  HAL_qdss_etfetb_AccessUnLock(void)
{
   HWIO_OUT(QDSS_ETFETB_LAR,0xC5ACCE55);
}

/* FUNCTION HAL_qdss_etfetb_AccessLock*/
void  HAL_qdss_etfetb_AccessLock(void)
{
   HWIO_OUT(QDSS_ETFETB_LAR,(~0xC5ACCE55));
}

void HAL_qdss_etfetb_SetMode(uint32 mode)
{
    HWIO_OUTF(QDSS_ETFETB_MODE,MODE,mode);
}

uint32 HAL_qdss_etfetb_GetMode()
{
   return HWIO_INF(QDSS_ETFETB_MODE,MODE);
}

uint32 HAL_qdss_etfetb_GetRamDepth(void)
{
   return HWIO_IN(QDSS_ETFETB_RSZ);
}

void  HAL_qdss_etfetb_SetWritePtr(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETFETB_RWP,dwVal);
}

uint32  HAL_qdss_etfetb_GetWritePtr()
{
   return HWIO_IN(QDSS_ETFETB_RWP);
}

void  HAL_qdss_etfetb_SetTriggerCounter(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETFETB_TRG,dwVal);
}

void  HAL_qdss_etfetb_SetReadPtr(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETFETB_RRP,dwVal);
}

uint32  HAL_qdss_etfetb_GetReadPtr()
{
  return HWIO_IN(QDSS_ETFETB_RRP);
}

void  HAL_qdss_etfetb_WriteData(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETFETB_RWD,dwVal);
}

uint32  HAL_qdss_etfetb_ReadData(void)
{
   return HWIO_IN(QDSS_ETFETB_RRD);
}

void HAL_qdss_etfetb_DisableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETFETB_CTL,TRACECAPTEN,0x0);
}

void HAL_qdss_etfetb_EnableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETFETB_CTL,TRACECAPTEN,0x1);
}

boolean  HAL_qdss_etfetb_IsEnabled(void)
{
   return HWIO_INF(QDSS_ETFETB_CTL,TRACECAPTEN);
}

uint32  HAL_qdss_etfetb_GetFFCR(void)
{
   return HWIO_IN(QDSS_ETFETB_FFCR);
}

void HAL_qdss_etfetb_SetFFCR(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETFETB_FFCR,dwVal);
}

void HAL_qdss_etfetb_SetFFCRETBDefault(void)
{
/*TrigOnFl|FOnTrigEvt|FOnFlIn|EnTI|EnFt*/
/*In agreement with HLOS team*/
   #define ETB_FFCR_DEFAULT 0x433
   HWIO_OUT(QDSS_ETFETB_FFCR,ETB_FFCR_DEFAULT);
}

void HAL_qdss_etfetb_StopOnFlush(uint8 bEnable)
{
   HWIO_OUTF(QDSS_ETFETB_FFCR,STOPONFL,bEnable);
}

void HAL_qdss_etfetb_Flush(void)
{
   HWIO_OUTF(QDSS_ETFETB_FFCR,FLUSHMAN_W,1);
}

void HAL_qdss_etfetb_FlushWait(void)
{
   do {
      //spin until flush is complete.
      //flush is fast, haven't seent it spin more than once.
   }while (HWIO_INF(QDSS_ETFETB_FFCR,FLUSHMAN_R));
}

void HAL_qdss_etfetb_Drain(void)
{
   HWIO_OUTF(QDSS_ETFETB_FFCR,DRAINBUFFER,1);
}

void HAL_qdss_etfetb_EnableFormatting(void)
{
   HWIO_OUTF(QDSS_ETFETB_FFCR,ENFT,1);
}

void HAL_qdss_etfetb_WaitForTMCReadyVal(uint8 val)
{
   do {
      //spin until TMCReady is the specified value (0 or 1).
   }while (val != HWIO_INF(QDSS_ETFETB_STS,TMCREADY));
}

