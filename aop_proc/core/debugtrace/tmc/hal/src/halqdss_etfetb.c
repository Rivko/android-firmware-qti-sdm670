/*=============================================================================

FILE:         halqdss_etfetb.c

DESCRIPTION:  

================================================================================
                Copyright 2011,2018 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/tmc/hal/src/halqdss_etfetb.c#2 $
==============================================================================*/

#include "halqdss_etfetb.h"
#include "qdss_etfetb_regs.h"
#include "aop_services.h"

int g_abort_state = 0;
volatile uint32 QDSS_ETFETB_ETFETB_CXTMC_F128W8K_REG_BASE;
uint8 qdss_rpm_ts_wa_delay_us = 10; //QDSS flush workaround delay in microseconds
void busywait (uint32 pause_time_us);
void HAL_qdss_etfetb_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_ETFETB_ETFETB_CXTMC_F128W8K_REG_BASE  = nBaseAddr;
}


/*FUNCTION HAL_qdss_etfetb_AccessUnLock*/
void  HAL_qdss_etfetb_AccessUnLock(void)
{
   HWIO_OUT(QDSS_ETFETB_LAR,0xC5ACCE55);   
}

/*FUNCTION HAL_qdss_etfetb_AccessLock*/
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
   if (qdss_rpm_ts_wa_delay_us) {    // Workaround to wait for only 10 us and don't wait forever if FLUSHMAN stuck to 1
      busywait(qdss_rpm_ts_wa_delay_us);
   }
   #if 0
   do {
      //spin until flush is complete.
      //flush is fast, haven't seent it spin more than once.
   }while (HWIO_INF(QDSS_ETFETB_FFCR,FLUSHMAN));
   #endif
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

   if (qdss_rpm_ts_wa_delay_us) {    // Workaround to wait for only 10 us and don't wait forever if TMCREADY stuck to 1
      busywait(qdss_rpm_ts_wa_delay_us);
   }
   #if 0
   do {
      //spin until TMCReady is the specified value (0 or 1).
   }while (val != HWIO_INF(QDSS_ETFETB_STS,TMCREADY));
   #endif
}
