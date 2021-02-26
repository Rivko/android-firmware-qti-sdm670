/*=========================================================================

                       TPDM HAL

GENERAL DESCRIPTION
   This file implements generic HAL functionalities for a TPDM




      Copyright (c) 2015 Qualcomm Technologies Incorporated.
      All Rights Reserved.
      Qualcomm Confidential and Proprietary
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/core.qdsp6/2.1/debugtrace/tpdm/hal/src/halqdss_tpdm.c#1 $
==========================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include "hal_qdss_tpdm.h"
#include "hwio_qdss_tpdm.h"
#include "HALhwio.h"



static volatile uint32 QDSS_TPDM_BLOCK_BASE;

void HAL_qdss_tpdm_SetBaseAddress(uint32 virtAddr)
{
   QDSS_TPDM_BLOCK_BASE=virtAddr;
}

void HAL_qdss_tpdm_cmb_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_CMB_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_cmb_get_enable_disable(void)
{
   return HWIO_INF(TPDM_CMB_CR,E);
}


void HAL_qdss_tpdm_dsb_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_DSB_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_enable_disable(void)
{
   return HWIO_INF(TPDM_DSB_CR,E);
}


void HAL_qdss_tpdm_bc_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_BC_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_bc_get_enable_disable(void)
{
   return HWIO_INF(TPDM_BC_CR,E);
}


void HAL_qdss_tpdm_tc_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_TC_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_tc_get_enable_disable(void)
{
   return HWIO_INF(TPDM_TC_CR,E);
}


void HAL_qdss_tpdm_impldef_set_enable_disable(uint32 enable_bit)
{
   /*not implemented*/
   return;
}

void HAL_qdss_tpdm_cmb_set_mode(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_CR,MODE,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_mode(void)
{
   return HWIO_INF(TPDM_CMB_CR,MODE);
}


void HAL_qdss_tpdm_cmb_set_flow_control(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_CR,FLOWCTRL,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_flow_control(void)
{
   return HWIO_INF(TPDM_CMB_CR,FLOWCTRL);
}


void HAL_qdss_tpdm_cmb_set_patt_match(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_TIER,PATT_TSENAB,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_patt_match(void)
{
   return HWIO_INF(TPDM_CMB_TIER,PATT_TSENAB);
}

void HAL_qdss_tpdm_cmb_set_ext_trigger_on_off(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_TIER,XTRIG_TSENAB,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_ext_trigger_on_off(void)
{
   return HWIO_INF(TPDM_CMB_TIER,XTRIG_TSENAB);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_val_lsw(uint32 val)
{
   HWIO_OUTI(TPDM_CMB_TPRn,0,val);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_lsw(void)
{
   return HWIO_INI(TPDM_CMB_TPRn,0);
}

void HAL_qdss_tpdm_cmb_set_ts_patt_val_msw(uint32 val)
{
   HWIO_OUTI(TPDM_CMB_TPRn,1,val);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_msw(void)
{
   return HWIO_INI(TPDM_CMB_TPRn,1);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_mask_lsw(uint32 mask)
{
   HWIO_OUTI(TPDM_CMB_TPMRn,0,mask);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_lsw(void)
{
   return HWIO_INI(TPDM_CMB_TPMRn,0);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_mask_msw(uint32 mask)
{
   HWIO_OUTI(TPDM_CMB_TPMRn,1,mask);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_msw(void)
{
   return HWIO_INI(TPDM_CMB_TPMRn,1);
}

void HAL_qdss_tpdm_dsb_set_mode(uint32 mode)
{
   HWIO_OUTF(TPDM_DSB_CR,MODE,mode);
}

uint32 HAL_qdss_tpdm_dsb_get_mode(void)
{
   return HWIO_INF(TPDM_DSB_CR,MODE);
}


void HAL_qdss_tpdm_dsb_set_patt_type(uint8 type)
{
   HWIO_OUTF(TPDM_DSB_TIER,PATT_TYPE,type);
}

uint8 HAL_qdss_tpdm_dsb_get_patt_type(void)
{
   return HWIO_INF(TPDM_DSB_TIER,PATT_TYPE);
}


void HAL_qdss_tpdm_dsb_set_ext_trigger_on_off(uint32 on_off_bit)
{
   HWIO_OUTF(TPDM_DSB_TIER,XTRIG_TSENAB,on_off_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_ext_trigger_on_off(void)
{
   return HWIO_INF(TPDM_DSB_TIER,XTRIG_TSENAB);
}


void HAL_qdss_tpdm_dsb_set_patt_match(uint32 on_off_bit)
{
   HWIO_OUTF(TPDM_DSB_TIER,PATT_TSENAB,on_off_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_patt_match(void)
{
   return HWIO_INF(TPDM_DSB_TIER,PATT_TSENAB);
}

void HAL_qdss_tpdm_dsb_set_edcmr(uint32 index,uint32 val)
{
   HWIO_OUTI(TPDM_DSB_EDCMRn,index,val);
}

uint32 HAL_qdss_tpdm_dsb_get_edcmr(uint32 index)
{
   return HWIO_INI(TPDM_DSB_EDCMRn,index);
}


void HAL_qdss_tpdm_dsb_set_tpmr(uint32 index,uint32 val)
{
   HWIO_OUTI(TPDM_DSB_TPMRn,index,val);
}



#define TPDM_GPR_NUM_REGS 160 //refer arch. spec   

uint32 HAL_qdss_tpdm_SetGPR(uint32 GPRIndex,uint32 val)
{
   uint32 ret=1;

   if (GPRIndex<TPDM_GPR_NUM_REGS )
   {
      HWIO_OUTI(TPDM_GPRn,GPRIndex,val);
      ret=0;
   }
   return ret;
}

uint32 HAL_qdss_tpdm_GetGPR(uint32 GPRIndex)
{
   uint32 ret=0xDEADBEEF;

   if (GPRIndex<TPDM_GPR_NUM_REGS )
   {
      ret=HWIO_INI(TPDM_GPRn,GPRIndex);
   }
   return ret;
}


void HAL_qdss_tpdm_unlock_access(void)
{
   HWIO_OUT(TPDM_LAR,0xC5ACCE55);
}

void HAL_qdss_tpdm_lock_access(void)
{
   HWIO_OUT(TPDM_LAR,~0xC5ACCE55);
}
