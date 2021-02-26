/*=============================================================================

FILE:         halqdss_csr.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/csr/hal/src/halqdss_csr.c#1 $
==============================================================================*/
#include "halqdss_csr.h"
#include "qdss_csr_regs.h"


volatile uint32 QDSS_QDSSCSR_REG_BASE;


void HAL_qdss_csr_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_QDSSCSR_REG_BASE = nBaseAddr;
}

void  HAL_qdss_csr_AccessUnLock(void)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS,0xC5ACCE55);   
}

void  HAL_qdss_csr_AccessLock(void)
{ 
   HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS,(~0xC5ACCE55));   
}


void HAL_qdss_csr_SetBlkSize(uint8 blksize)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBBAMCTRL,BLKSIZE,blksize);
}


void HAL_qdss_csr_SetPeriodicFlushThreshold(uint16 nETRBAMCycles)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,PERFLSHTHRS,nETRBAMCycles);
}

void HAL_qdss_csr_EnablePeriodicFlush(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,PERFLSH,1);
}

void HAL_qdss_csr_ManualEOT(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,EOT,1);
}

void HAL_qdss_csr_EnableUSB(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBBAMCTRL,USBENAB,1);
}


boolean  HAL_qdss_csr_IsUSBEnabled(void)
{

   return ((boolean) HWIO_INF(QDSS_CS_QDSSCSR_USBBAMCTRL,USBENAB));
}


void HAL_qdss_csr_SetAWIDs(void) 
{

#define STMAWID_DEFAULT   0
#define STMAWID_MODEM     1
#define STMAWID_BAMDMA    2
#define STMAWID_RESERVED  3

//We assign AWID0 to default, AWID1 to modem, AWID2 to BAMDMA and AWID3 for future use
#define STMAWID_ASSIGNMENTS 0x03020100  

   HWIO_OUT(QDSS_CS_QDSSCSR_STMAWIDCTRL,STMAWID_ASSIGNMENTS);
}





void HAL_qdss_csr_EnableModemMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB1,1);
}

void HAL_qdss_csr_DisableModemMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB1,0);
}

void HAL_qdss_csr_SetModemChannelOffset(uint32 offset) 
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMCHNOFST0,STMCHNOFST1,offset);
}
void HAL_qdss_csr_EnableBamDMAMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB2,1);
}

void HAL_qdss_csr_DisableBamDMAMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB2,0);
}

void HAL_qdss_csr_SetBamDMAChannelOffset(uint32 offset) 
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMCHNOFST0,STMCHNOFST2,offset);
}


void HAL_qdss_csr_EnableAllBurstMasters(void)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMTRANSCTRL,0xF);
}

void HAL_qdss_csr_DisableAllBurstMasters(void)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMTRANSCTRL,0x0);
}

void HAL_qdss_csr_DisableTimestampGenerator(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,TIMESTAMPENABLE,0);
}

void HAL_qdss_csr_EnableTimestampGenerator(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,TIMESTAMPENABLE,1);
}

void HAL_qdss_csr_TriggerTimestampCapture(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,0);
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,1);
}

uint64 HAL_qdss_csr_GetTimestampCounter(void) 
{
   return (((uint64)(HWIO_IN(QDSS_CS_QDSSCSR_QDSSTIMEVAL1)) << 32) |
           HWIO_IN(QDSS_CS_QDSSCSR_QDSSTIMEVAL0));
}


uint64 qdss_csr_GetTimestampCounter(void) 
{
   uint64 timestamp;

   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_DisableTimestampGenerator();
   HAL_qdss_csr_TriggerTimestampCapture();
   HAL_qdss_csr_EnableTimestampGenerator();
   timestamp = HAL_qdss_csr_GetTimestampCounter();
   HAL_qdss_csr_AccessLock();
   return timestamp;
}



void HAL_qdss_csr_SetTimestampCounter(uint64 timeval) 
{
   HWIO_OUT(QDSS_CS_QDSSCSR_QDSSTIMELOAD1,((uint32)(timeval >> 32)));
   HWIO_OUT(QDSS_CS_QDSSCSR_QDSSTIMELOAD0,((uint32)(timeval & 0xffffffff)));
}

void qdss_csr_SetTimestampCounter(uint64 timestamp) 
{
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_DisableTimestampGenerator();
   HAL_qdss_csr_SetTimestampCounter(timestamp);
   HAL_qdss_csr_EnableTimestampGenerator();
   HAL_qdss_csr_AccessLock();
}


uint64 HAL_qdss_csr_GetMPMCounter(void) 
{
   return (((uint64)(HWIO_INF(QDSS_CS_QDSSCSR_AOTIMEVAL1,TIMEVAL)) << 32) |
           HWIO_IN(QDSS_CS_QDSSCSR_AOTIMEVAL0));
}

void HAL_qdss_csr_mux_ctrl_0(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL0, val);
}

void HAL_qdss_csr_mux_ctrl_1(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL1, val);
}

void HAL_qdss_csr_mux_ctrl_2(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL2, val);
}

void HAL_qdss_csr_mux_ctrl_3(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL3, val);
}

