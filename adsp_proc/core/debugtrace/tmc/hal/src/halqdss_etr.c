/*=============================================================================

FILE:         halqdss_etr.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tmc/hal/src/halqdss_etr.c#1 $
==============================================================================*/
#include "halqdss_etr.h"
#include "qdss_etr_regs.h"

volatile uint32 QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE;

void HAL_qdss_etr_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE = nBaseAddr;
}

uint32 HAL_qdss_etr_GetRAMSize(void)
{
   return HWIO_IN(QDSS_ETR_RSZ); 
}


void HAL_qdss_etr_SetRAMSize(uint32 ram_size)
{
   HWIO_OUT(QDSS_ETR_RSZ,ram_size); 
}

/*FUNCTION HAL_qdss_etr_AccessUnLock*/
void  HAL_qdss_etr_AccessUnLock(void)
{
   HWIO_OUT(QDSS_ETR_LAR,0xC5ACCE55);   
}

/*FUNCTION HAL_qdss_etr_AccessLock*/
void  HAL_qdss_etr_AccessLock(void)
{ 
   HWIO_OUT(QDSS_ETR_LAR,(~0xC5ACCE55));   
}

void HAL_qdss_etr_SetMode(uint8 mode)
{
   HWIO_OUTF(QDSS_ETR_MODE,MODE,mode);
}


void HAL_qdss_etr_SetWriteBurstLength(uint8 nLen)
{
   HWIO_OUTF(QDSS_ETR_AXICTL,WRBURSTLEN,nLen);
}

void HAL_qdss_etr_DisableScatterGather(void)
{
   HWIO_OUTF(QDSS_ETR_AXICTL,SCATTERGATHERMODE,0);
}

#define HWIO_QDSS_ETR_AXICTL_CACHEPROTCTRL_SHFT    0x0
#define HWIO_QDSS_ETR_AXICTL_CACHEPROTCTRL_BMSK    0x3F

void HAL_qdss_etr_SetCacheProtCtrl(void)
{

/* HPG recommendations : */
/*    CacheCtrlBit3..0 = 4’b0011 (no caching, bufferable, non-secure) */
/*    ProtCtrlBit1..0 = 2’b10 (non-secure, normal)- 001110  */ 

   HWIO_OUTF(QDSS_ETR_AXICTL,CACHEPROTCTRL,0xE);
}

void HAL_qdss_etr_SetDataBufferAddress(uint32 addr)
{

   HWIO_OUT(QDSS_ETR_DBALO,addr);
   HWIO_OUTF(QDSS_ETR_DBAHI,BUFADDRHI,0);
}


uint32 HAL_qdss_etr_GetDataBufferAddress(void)
{
   return HWIO_IN(QDSS_ETR_DBALO);
}




void HAL_qdss_etr_DisableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETR_CTL,TRACECAPTEN,0x0);
}


void HAL_qdss_etr_EnableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETR_CTL,TRACECAPTEN,0x1);
}

void HAL_qdss_etr_SetFFCR(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETR_FFCR,dwVal);
}

void HAL_qdss_etr_Flush(void)
{
   HWIO_OUTF(QDSS_ETR_FFCR,FLUSHMAN_W,1);
}


void HAL_qdss_etr_FlushWait(void)
{
   do {
      //spin until flush is complete.
      //flush is fast, haven't seent it spin more than once.
   }while (HWIO_INF(QDSS_ETR_FFCR,FLUSHMAN_R));
}


void HAL_qdss_etr_EnableFormatting(void)
{
   HWIO_OUTF(QDSS_ETR_FFCR,ENFT,1);
}


void HAL_qdss_etr_SetFFCRETBDefault(void)
{
/*TrigOnFl|FOnTrigEvt|FOnFlIn|EnTI|EnFt*/
/*In agreement with HLOS team*/
   #define ETB_FFCR_DEFAULT 0x433 
   HWIO_OUT(QDSS_ETR_FFCR,ETB_FFCR_DEFAULT);
}
