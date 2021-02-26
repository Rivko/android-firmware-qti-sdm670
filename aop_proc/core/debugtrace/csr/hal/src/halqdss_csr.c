/*=============================================================================

FILE:         halqdss_csr.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/csr/hal/src/halqdss_csr.c#1 $
==============================================================================*/
#include "halqdss_csr.h"
#include "qdss_swaocsr_hwio.h"


volatile uint32 QDSS_QDSSCSR_REG_BASE;


void HAL_qdss_csr_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_QDSSCSR_REG_BASE = nBaseAddr;
}

void  HAL_qdss_csr_AccessUnLock(void)
{
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_LOCKACCESS,0xC5ACCE55);   
}

void  HAL_qdss_csr_AccessLock(void)
{ 
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_LOCKACCESS,(~0xC5ACCE55));   
}

void HAL_qdss_csr_SetTimestampGenerator(uint8 enable)
{
   HWIO_OUTF(SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL,TIMESTAMPENABLE,(enable &0x1));
}


void HAL_qdss_csr_LoadTimestampCounter(uint64 timeval) 
{
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1,((uint32)(timeval >> 32)));
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0,((uint32)(timeval & 0xffffffff)));
}


uint64 HAL_qdss_csr_GetTimestampCounter(void) 
{

   HWIO_OUTF(SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL,CAPTURE,0);
   HWIO_OUTF(SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL,CAPTURE,1);

   return ( ((uint64)(HWIO_IN(SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1)) << 32) | 
            HWIO_IN(SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0));
}


void HAL_qdss_csr_SetHeartbeatPulse(uint32 mask) 
{
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO,mask);
   HWIO_OUT(SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO,mask);
}



#define QDSS_SWAO_PWRUP_WAIT_TIMEOUT 1000
uint32 qdss_swao_pwrup_wait_count;
void HAL_qdss_csr_SWAOPowerUp(void)
{
   uint32 swdbg_pwr_ack =0;
   HWIO_OUTF(SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL,SWDBGPWRUPREQ,1);

   qdss_swao_pwrup_wait_count = 0;

   do {
      swdbg_pwr_ack = HWIO_IN(SWAOCSR_CS_SWAOCSR_SWDBGPWRACK);
      qdss_swao_pwrup_wait_count++;
   } while ( (0==(swdbg_pwr_ack & 0x1)) &&
             (qdss_swao_pwrup_wait_count <QDSS_SWAO_PWRUP_WAIT_TIMEOUT));
}


void HAL_qdss_csr_SWAOPowerDown(void)
{
   HWIO_OUTF(SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL,SWDBGPWRUPREQ,0);
}

