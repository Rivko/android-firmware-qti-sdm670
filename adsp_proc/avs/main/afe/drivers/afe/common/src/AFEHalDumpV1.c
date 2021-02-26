/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEHalDumpV1.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "LPASS_ADDRESS_FILE.h"

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEHalDump.h"
#include "AFEHalDumpFuncDefs.h"
#include "hwd_devcfg.h"

#ifdef AUDIOIF_REG_DUMP    


/**
* The following type must be used whenever a source/sink are to be represented.
*/

static qurt_mem_region_t         lpass_clk_qmem_region; 

static uint32                  lpass_clk_phy_base; /*LPASS_LPASS_CC_REG_BASE*/
static int32                   lpass_clk_virt_offset;

#define LCC_OFFSET(_reg_) ((_reg_)- LPASS_LPASS_CC_REG_BASE) 
#define LCC_REG_SIZE   0x20000

// cache the registers
typedef struct hal_clk_dump_v1
{
   uint32_t LPASS_AUDIO_CORE_LPAIF_DMA_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t  LPASS_AUDIO_CORE_LPAIF_CSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PRI_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX]; 
   uint32_t LPASS_LPAIF_PRI_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PRI_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PRI_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PRI_OSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SEC_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SEC_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SEC_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SEC_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_SEC_OSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_TER_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_TER_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_TER_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_TER_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_TER_OSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_QUAD_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_QUAD_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_QUAD_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_QUAD_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_QUAD_OSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SPKR_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SPKR_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SPKR_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_SPKR_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM0_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM0_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM0_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM0_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PCM0_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PCM0_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM1_CFG_RCGR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM1_M_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM1_N_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM1_D_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PCM1_IBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
   uint32_t LPASS_AUDIO_CORE_LPAIF_PCM1_EBIT_CBCR_DUMP[LPA_IF_SEQ_MAX];
} hal_clk_dump_v1_t;
static hal_clk_dump_v1_t afe_hal_clk_dump_v1;

static void hal_afe_dump_init_v1(uint32_t version);
static void hal_afe_dump_deinit_v1(void);
static void hal_afe_clock_dump_v1(uint32_t intf_type, uint32_t intf_index, uint32_t dump_seq);
  

void hal_afe_dump_get_func_table_v1(afe_hal_dump_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_afe_dump_get_func_table_v1 func_table is NULL"); 
     return;
   }
  
   func_table->hal_afe_dump_init_fptr = hal_afe_dump_init_v1;
   func_table->hal_afe_dump_deinit_fptr = hal_afe_dump_deinit_v1;
   func_table->hal_afe_clock_dump_fptr = hal_afe_clock_dump_v1; 
}


static void hal_afe_dump_init_v1(uint32_t version)
{
   ADSPResult result = ADSP_EOK;
   uint32_t  clk_virt_addr = 0; 

   lpass_clk_phy_base = LPASS_LPASS_CC_REG_BASE;

   //  2. create memory region for lpass clk register interface
   //   This information is need to be saved to  global state

   // mux register interface
   if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&lpass_clk_qmem_region,
            lpass_clk_phy_base, LCC_REG_SIZE, &clk_virt_addr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for lcc v1 dump register base");
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lcc v1 dump virtual addr 0x%x phy addr 0x%x",
      (unsigned int)clk_virt_addr, (unsigned int)lpass_clk_phy_base);
      ///< Calculate offset of virtual address in words (4 bytes)
      lpass_clk_virt_offset =  (clk_virt_addr - lpass_clk_phy_base)>>2;  
   }
   
}

static void hal_afe_dump_deinit_v1(void)
{
   /* release all resource allocated during init*/
   HwdDevCfg_DestoryVirtAddress(lpass_clk_qmem_region);
}

static void hal_afe_clock_dump_v1(uint32_t intf_type, uint32_t intf_index, uint32_t dump_seq)
{
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== start hal_afe_clock_dump_v1 intf_type[%d] index[%d] dump_seq[%d] ====", intf_type, intf_index, dump_seq);  

   ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_DMA_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_DMA_CBCR_DUMP[dump_seq]);
   ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_CSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CSR_CBCR_DUMP[dump_seq]);


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_DMA_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_DMA_CBCR_DUMP[dump_seq]);   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_CSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CSR_CBCR_DUMP[dump_seq]);  

   switch(intf_type)
   {

      case LPA_IF_I2S:
      {
         switch(intf_index)
         {
            case 0:
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PRI_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PRI_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PRI_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PRI_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PRI_OSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_OSR_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PRI_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PRI_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PRI_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PRI_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PRI_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PRI_OSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_OSR_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;
            case 1:
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SEC_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SEC_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SEC_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SEC_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_SEC_OSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_OSR_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SEC_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SEC_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SEC_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SEC_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SEC_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_SEC_OSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_OSR_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;
            case 2:
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_TER_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_TER_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_TER_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_TER_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_TER_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_TER_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_TER_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_TER_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_TER_OSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_OSR_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_TER_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_TER_CFG_RCGR_DUMP[dump_seq]);            
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_TER_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_TER_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_TER_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_TER_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_TER_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_TER_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_TER_OSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_OSR_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;
            case 3:
            {

               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_QUAD_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_QUAD_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_QUAD_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_QUAD_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_QUAD_OSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_OSR_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_QUAD_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_QUAD_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_QUAD_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_QUAD_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_QUAD_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_QUAD_OSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_OSR_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;         
            case 4:
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SPKR_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SPKR_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SPKR_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_SPKR_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SPKR_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SPKR_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SPKR_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_SPKR_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_SPKR_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;
            default:
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "==== Unknown intf_index (%d) defined. ====", intf_index); 
            break;
         }
      }
      break;
      case LPA_IF_PCM:
      {
         switch(intf_index)
         {
            case 0:
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM0_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM0_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM0_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM0_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PCM0_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM0_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PCM0_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM0_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM0_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM0_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM0_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM0_D = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM0_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PCM0_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM0_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PCM0_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM0_EBIT_CBCR_DUMP[dump_seq]); 
            }
            break;         
            case 1:   
            {
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM1_CFG_RCGR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_CFG_RCGR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM1_M), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_M_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM1_N), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_N_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_LPAIF_PCM1_D), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_D_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PCM1_IBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM1_IBIT_CBCR_DUMP[dump_seq]); 
               ReadRegister1(lpass_clk_phy_base + LCC_OFFSET(LPASS_AUDIO_CORE_LPAIF_PCM1_EBIT_CBCR), lpass_clk_virt_offset, &afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM1_EBIT_CBCR_DUMP[dump_seq]); 

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM1_CFG_RCGR = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_CFG_RCGR_DUMP[dump_seq]);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM1_M = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_M_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM1_N = 0x%x", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_N_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM1_D = 0x%x \n", afe_hal_clk_dump_v1.LPASS_LPAIF_PCM1_D_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PCM1_IBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM1_IBIT_CBCR_DUMP[dump_seq]);  
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_AUDIO_CORE_LPAIF_PCM1_EBIT_CBCR = 0x%x", afe_hal_clk_dump_v1.LPASS_AUDIO_CORE_LPAIF_PCM1_EBIT_CBCR_DUMP[dump_seq]);  
            }
            break;         
            default:
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Unknown intf_index (%d) defined. ====", intf_index); 
            break;
         }
      }
      break;   
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Undefined intf_type (%d) defined. ====", intf_type); 
      break;
   }   
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== end hal_afe_clock_dump_v1 ====");
}
              
#endif
               

