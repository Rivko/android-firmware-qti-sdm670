/*=========================================================================

                       TPDM HAL

GENERAL DESCRIPTION
   This file implements generic HAL functionalities for a TPDM




      Copyright (c) 2015 Qualcomm Technologies Incorporated.
      All Rights Reserved.
      Qualcomm Confidential and Proprietary
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tpda/hal/src/hal_qdss_tpda.c#1 $
==========================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include "hal_qdss_tpda.h"
#include "hwio_qdss_tpda.h"
#include "HALhwio.h"



static volatile uint32 QDSS_TPDA_BLOCK_BASE;

void HAL_qdss_tpda_SetBaseAddress(uint32 virtAddr)
{
   QDSS_TPDA_BLOCK_BASE=virtAddr;
   return;
}

void HAL_qdss_tpda_port_enable_disable(uint32 tpda_port_num,uint32 setbit)
{
   setbit&=1;
   HWIO_OUTFI(QDSS_TPDA_Pn_CR,tpda_port_num,E,setbit);
   return;
}

void HAL_qdss_tpda_port_set_cmbesize(uint32 tpda_port_num,uint8 esize)
{
  HWIO_OUTFI(QDSS_TPDA_Pn_CR,tpda_port_num,CMBESIZE,esize);
}


void HAL_qdss_tpda_set_freq_val(uint32 val)
{
   HWIO_OUTF(QDSS_TPDA_FREQREQ_VAL,VAL,val);
   return;
}

void HAL_qdss_tpda_freq_pkt_req(void)
{
   HWIO_OUTF(QDSS_TPDA_CR,FREQREQ,1);
   return;
}

void HAL_qdss_tpda_global_flush_req(void)
{
   HWIO_OUTF(QDSS_TPDA_CR,FLREQ,1);
   return;
}

void HAL_qdss_tpda_control_freq_timestamp_on_off(uint32 setbit)
{
   HWIO_OUTF(QDSS_TPDA_CR,FREQTS,setbit);
   return;
}

void HAL_qdss_tpda_set_atid(uint32 atid)
{
   HWIO_OUTF(QDSS_TPDA_CR,ATID,atid);
   return;
}

void HAL_qdss_tpda_set_sync_pkt_periodicity(uint32 mode,uint32 bytes)
{
   HWIO_OUTF(QDSS_TPDA_SYNCR,MODE,mode);
   if(mode)
   {
      if (bytes>27)
         bytes=27;
      else if (bytes<12)
         bytes=12;
      bytes=bytes<<7;
   }
   HWIO_OUTF(QDSS_TPDA_SYNCR,COUNT,bytes);
   return;
}

void HAL_qdss_tpda_set_avfreqen(uint8 setbit)
{
   HWIO_OUTF(QDSS_TPDA_CR,AVFREQEN,setbit);
}


void HAL_qdss_tpda_flag_pkt_via_CTI(uint32 setbit)
{
   HWIO_OUTF(QDSS_TPDA_CR,FLRIE,setbit);
   return;
}


void HAL_qdss_tpda_freq_pkt_via_CTI(uint32 setbit)
{
   HWIO_OUTF(QDSS_TPDA_CR,FRIE,setbit);
   return;
}

void HAL_qdss_tpda_sync_pkt_via_CTI(uint32 setbit)
{
   HWIO_OUTF(QDSS_TPDA_CR,SRIE,setbit);
   return;
}

void HAL_qdss_tpda_unlock_access(void)
{
   HWIO_OUT(QDSS_TPDA_LOCKACCESS,0xC5ACCE55);
   return;
}

void HAL_qdss_tpda_lock_access(void)
{
   HWIO_OUT(QDSS_TPDA_LOCKACCESS,~0xC5ACCE55);
   return;
}
