/*========================================================================
This file contains main interface to the AFE Spdif hal driver

  Copyright (c) 2009-2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/halspdif.c#1 $
 ====================================================================== */

#include "LPASS_ADDRESS_FILE_SPDIF.h"
#include "halspdif.h"
#include "common.h"
#include <assert.h>
#include "qurt_elite.h"

static uint32                  spdif_phy_base; 
static int32                   spdif_virt_offset;
static uint32                  spdif_reset_phy_base; 
static int32                   spdif_reset_virt_offset;
static uint32                  spdif_version;

#define HAL_SPDIF_DEBUG
#undef HAL_SPDIF_DEBUG

#define SPDIF_OFFSET(_reg_) ((_reg_)-LPASS_SPDIFTX_BASE)  

#define SPDIF_RESET_OFFSET(_reg_) ((_reg_)-LPASS_SPDIFTX_RESET_BASE)
 
void hal_spdif_init(uint32 version, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base)
{
   /* SDPIF version */
   spdif_version = version;   
   spdif_phy_base = phy_base;
   spdif_reset_phy_base = reset_phy_base;

   ///< Calculate offset of virtual address in words (4 bytes)
   spdif_virt_offset = (virt_base - phy_base)>>2; 

   spdif_reset_virt_offset = (reset_virt_base - reset_phy_base)>>2; 

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "### spdif version = 0x%x", (unsigned int)spdif_version);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "### spdifVirtOffset = 0x%x reset virt offset 0x%x", spdif_virt_offset, spdif_reset_virt_offset);
}

void hal_spdif_soft_reset(void)
{
#ifndef SIM
   uint32 mask,field_value, value;

   mask = SPDIFTX_CTL__SOFT_RESET___M;
   field_value = SPDIFTX_CTL__SOFT_RESET__ENABLE << SPDIFTX_CTL__SOFT_RESET___S;
   
   UpdateRegister1(spdif_reset_phy_base + SPDIF_RESET_OFFSET(LPASS_SPDIFTX_CTL),
        spdif_reset_virt_offset,mask,field_value,0); ///< update register
   do
   {
      ReadRegister1(spdif_reset_phy_base + SPDIF_RESET_OFFSET(LPASS_SPDIFTX_STAT),
              spdif_reset_virt_offset,&value);
   }while(SPDIFTX_STAT__SOFT_RESET_STAT__SOFT_RESET_PRESENT != (value & SPDIFTX_STAT__SOFT_RESET_STAT___M));

   mask = SPDIFTX_CTL__SOFT_RESET___M;
   field_value = SPDIFTX_CTL__SOFT_RESET__DISABLE << SPDIFTX_CTL__SOFT_RESET___S;
   
   UpdateRegister1(spdif_reset_phy_base + SPDIF_RESET_OFFSET(LPASS_SPDIFTX_CTL),
        spdif_reset_virt_offset,mask,field_value,0); ///< update register

   do
   {
      ReadRegister1(spdif_reset_phy_base + SPDIF_RESET_OFFSET(LPASS_SPDIFTX_STAT),
              spdif_reset_virt_offset,&value);
   }while(SPDIFTX_STAT__SOFT_RESET_STAT__NO_SOFT_RESET_PRESENT != (value & SPDIFTX_STAT__SOFT_RESET_STAT___M));
   
#endif
}


void hal_spdif_write_tx_cfg(spdif_tx_cfg_t tx_cfg)
{
   uint32 mask,field_value;

   mask = SPDIFTX_CFG__EN_INH_WTTS___M | SPDIFTX_CFG__DROP_LATE_BLOCKS___M | SPDIFTX_CFG__FRAME_SIZE___M;

   field_value = (tx_cfg.en_inh_wtts << SPDIFTX_CFG__EN_INH_WTTS___S) &  SPDIFTX_CFG__EN_INH_WTTS___M;
   field_value |= (tx_cfg.drop_late_frames << SPDIFTX_CFG__DROP_LATE_BLOCKS___S) & SPDIFTX_CFG__DROP_LATE_BLOCKS___M;
   field_value |=  ((tx_cfg.frame_size -1) << SPDIFTX_CFG__FRAME_SIZE___S) & SPDIFTX_CFG__FRAME_SIZE___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CFG),
	     spdif_virt_offset,mask,field_value,0); ///< update register

#ifdef HAL_SPDIF_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_CFG register done");
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "INFO: frame_size = 0x%x", (tx_cfg.frame_size - 1));
#endif

}
//****************************************************************//

void hal_spdif_write_tx_portsel(spdif_port_sel_t tx_port_sel)
{
   uint32 mask,field_value;

   mask = SPDIFTX_PORT_SEL__PARITY_ERR___M |
          SPDIFTX_PORT_SEL__PARITY_EN___M |
         // SPDIFTX_PORT_SEL__PREAMB_POL___M | 
          SPDIFTX_PORT_SEL__USER_DATA_EN___M |
          SPDIFTX_PORT_SEL__CH_STAT_EN___M; 
          //SPDIFTX_PORT_SEL__PORT_EN___M;

   //field_value = (tx_port_sel.parity_err << SPDIFTX_PORT_SEL__PARITY_ERR___S) & SPDIFTX_PORT_SEL__PARITY_ERR___M;
   field_value = (tx_port_sel.parity_e << SPDIFTX_PORT_SEL__PARITY_EN___S) & SPDIFTX_PORT_SEL__PARITY_EN___M;
   //field_value |= (tx_port_sel.preamble_polarity << SPDIFTX_PORT_SEL__PREAMB_POL___S) & SPDIFTX_PORT_SEL__PREAMB_POL___M;
   field_value |= (tx_port_sel.usr_data_e << SPDIFTX_PORT_SEL__USER_DATA_EN___S) & SPDIFTX_PORT_SEL__USER_DATA_EN___M;
   field_value |= (tx_port_sel.ch_sts_e << SPDIFTX_PORT_SEL__CH_STAT_EN___S) & SPDIFTX_PORT_SEL__CH_STAT_EN___M;
   //fieldValue |= (txPortSel.tx_port_e << SPDIFTX_PORT_SEL__PORT_EN___S) & SPDIFTX_PORT_SEL__PORT_EN___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_PORT_SEL),
	     spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_PORT_SEL register 0x%x", field_value);
#endif

}
//****************************************************************//

void hal_spdif_write_tx_portcfg(spdif_tx_port_cfg_t tx_port_cfg)
{
   uint32 mask,field_value;

   mask = SPDIFTX_PORT_CFG__MONO_SEL___M |
          SPDIFTX_PORT_CFG__SOFT_MUTE_AFTER_EOF___M |
          SPDIFTX_PORT_CFG__SOFT_MUTE___M |
          SPDIFTX_PORT_CFG__EMPTY_FIFO_REPEAT_MUTE___M |
          SPDIFTX_PORT_CFG__MONO___M |
          SPDIFTX_PORT_CFG__FRAMEDATA_WIDTH___M;

   field_value = (tx_port_cfg.mono_sel << SPDIFTX_PORT_CFG__MONO_SEL___S) & SPDIFTX_PORT_CFG__MONO_SEL___M;
   field_value |= (tx_port_cfg.soft_mute_after_eof << SPDIFTX_PORT_CFG__SOFT_MUTE_AFTER_EOF___S) & SPDIFTX_PORT_CFG__SOFT_MUTE_AFTER_EOF___M;
   field_value |= (tx_port_cfg.soft_mute << SPDIFTX_PORT_CFG__SOFT_MUTE___S) & SPDIFTX_PORT_CFG__SOFT_MUTE___M;
   field_value |= (tx_port_cfg.empty_fifo_repeat_mute << SPDIFTX_PORT_CFG__EMPTY_FIFO_REPEAT_MUTE___S) & SPDIFTX_PORT_CFG__EMPTY_FIFO_REPEAT_MUTE___M;
   field_value |= (tx_port_cfg.mono << SPDIFTX_PORT_CFG__MONO___S) & SPDIFTX_PORT_CFG__MONO___M;
   field_value |= (tx_port_cfg.bit_width << SPDIFTX_PORT_CFG__FRAMEDATA_WIDTH___S) & SPDIFTX_PORT_CFG__FRAMEDATA_WIDTH___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_PORT_CFG),
	     spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_PORT_CFG register done 0x%x", field_value);
#endif
}

//****************************************************************//

void hal_spdif_write_tx_fifocntl(spdif_fifo_cntl_t tx_fifo_cntl)
{
   uint32 mask,field_value;

   mask = SPDIFTX_FIFO_CNTL__LJUSTIFIED___M |
          SPDIFTX_FIFO_CNTL__FIFP_JUMP_RD_PTR___M |
          SPDIFTX_FIFO_CNTL__FIFO_PRE_BUF_EN___M |
          SPDIFTX_FIFO_CNTL__WATERMARK___M |
          SPDIFTX_FIFO_CNTL__DWORD_WORD_SWAP___M |
          SPDIFTX_FIFO_CNTL__WORD_BYTE_SWAP___M;
   
   field_value = (tx_fifo_cntl.left_justified << SPDIFTX_FIFO_CNTL__LJUSTIFIED___S) & SPDIFTX_FIFO_CNTL__LJUSTIFIED___M;
   field_value |= (tx_fifo_cntl.fifo_jump_rd_ptr << SPDIFTX_FIFO_CNTL__FIFP_JUMP_RD_PTR___S) & SPDIFTX_FIFO_CNTL__FIFP_JUMP_RD_PTR___M;
   field_value |= (tx_fifo_cntl.fifo_pre_buf_en << SPDIFTX_FIFO_CNTL__FIFO_PRE_BUF_EN___S) & SPDIFTX_FIFO_CNTL__FIFO_PRE_BUF_EN___M;
   field_value |= (tx_fifo_cntl.watermark << SPDIFTX_FIFO_CNTL__WATERMARK___S) & SPDIFTX_FIFO_CNTL__WATERMARK___M;
   field_value |= (tx_fifo_cntl.dword_word_swap << SPDIFTX_FIFO_CNTL__DWORD_WORD_SWAP___S) & SPDIFTX_FIFO_CNTL__DWORD_WORD_SWAP___M;
   field_value |= (tx_fifo_cntl.word_byte_swap << SPDIFTX_FIFO_CNTL__WORD_BYTE_SWAP___S) & SPDIFTX_FIFO_CNTL__WORD_BYTE_SWAP___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_FIFO_CNTL),
	     spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_FIFO_CNTL register done 0x%x",field_value);
#endif
}

void hal_spdif_read_tx_release_level(uint32 *fill_level_release)
{
   uint32 value;

   
   ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_SUBBUF_FIFO_STA),
           spdif_virt_offset,&value);

   *fill_level_release = (value & SPDIFTX_SUBBUF_FIFO_STA__FILL_LEVEL_RELEASE___M) >> SPDIFTX_SUBBUF_FIFO_STA__FILL_LEVEL_RELEASE___S;
}

//****************************************************************//

void hal_spdif_write_tx_int_cntl(spdif_int_cnt_t tx_int_cntl)
{

   uint32 mask, field_value;

   mask = SPDIFTX_INT_CNTL__INT_MASK___M |
          SPDIFTX_INT_CNTL__TX_MC_ERR___M | 
          SPDIFTX_INT_CNTL__TX_TS_DIFF_INT_EN___M |
          SPDIFTX_INT_CNTL__TX_DROP_LATE_BLOCK_INT_EN___M |
          SPDIFTX_INT_CNTL__TX_PAUSE_INT_EN___M |
          SPDIFTX_INT_CNTL__TX_IDLE_INT_EN___M |
          SPDIFTX_INT_CNTL__TX_FIFO_UNDERFLOW_INT_EN___M |
          SPDIFTX_INT_CNTL__TX_BLOCK_DONE_INT_EN___M;

   field_value = (tx_int_cntl.int_mask << SPDIFTX_INT_CNTL__INT_MASK___S) & SPDIFTX_INT_CNTL__INT_MASK___M;
   field_value |= (tx_int_cntl.mc_err_int_e << SPDIFTX_INT_CNTL__TX_MC_ERR___S) & SPDIFTX_INT_CNTL__TX_MC_ERR___M;
   field_value |= (tx_int_cntl.ts_diff_int_e << SPDIFTX_INT_CNTL__TX_TS_DIFF_INT_EN___S) & SPDIFTX_INT_CNTL__TX_TS_DIFF_INT_EN___M;
   field_value |= (tx_int_cntl.drop_late_blk_int_e << SPDIFTX_INT_CNTL__TX_DROP_LATE_BLOCK_INT_EN___S) & SPDIFTX_INT_CNTL__TX_DROP_LATE_BLOCK_INT_EN___M;
   field_value |= (tx_int_cntl.pause_int_e << SPDIFTX_INT_CNTL__TX_PAUSE_INT_EN___S ) & SPDIFTX_INT_CNTL__TX_PAUSE_INT_EN___M ;
   field_value |= (tx_int_cntl.idle_int_e << SPDIFTX_INT_CNTL__TX_IDLE_INT_EN___S) & SPDIFTX_INT_CNTL__TX_IDLE_INT_EN___M;
   field_value |= (tx_int_cntl.fifo_uflow_int_e << SPDIFTX_INT_CNTL__TX_FIFO_UNDERFLOW_INT_EN___S) & SPDIFTX_INT_CNTL__TX_FIFO_UNDERFLOW_INT_EN___M;
   field_value |= (tx_int_cntl.tx_block_done_int_e << SPDIFTX_INT_CNTL__TX_BLOCK_DONE_INT_EN___S) & SPDIFTX_INT_CNTL__TX_BLOCK_DONE_INT_EN___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_INT_CNTL),
         spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_INT_CNTL register done 0x%x",field_value);
#endif
}
//****************************************************************//


void hal_spdif_write_tx_nonlinear_bypass(boolean en)
{
    uint32 mask,field_value;

    mask = SPDIFTX_MISC_CNTL__VALID_BIT_BYPASS_VALUE___M |
           SPDIFTX_MISC_CNTL__SW_NONLINPCM_BYPASS_SUPPORT__HW_NON_LINEAR_MODE_SUPPORT;

    if(en == TRUE)
    {
       field_value = (SPDIFTX_MISC_CNTL__VALID_BIT_BYPASS_VALUE__OVERWRITE_VALID_BIT_WITH_1 << SPDIFTX_MISC_CNTL__VALID_BIT_BYPASS_VALUE___S) |
                     (SPDIFTX_MISC_CNTL__SW_NONLINPCM_BYPASS_SUPPORT__SW_NON_LINEAR_MODE_SUPPORT << SPDIFTX_MISC_CNTL__SW_NONLINPCM_BYPASS_SUPPORT___S);
    }
    else
    {
       field_value = 0;
    }

    UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_MISC_CNTL),
          spdif_virt_offset,mask,field_value,0); ///< update register     

}


void hal_spdif_write_tx_subbuf_fifo(uint32 * sub_buf_pntr, uint32 fsa, uint32 wtts_flag)
{

   uint32 mask,field_value;

   mask = SPDIFTX_BUF_PNTR__PNTR___M;
   field_value = (uint32) sub_buf_pntr;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_BUF_PNTR),
         spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_SUBBUF_FIFO__WAIT_TILL_TS_FLAG___M |
          SPDIFTX_SUBBUF_FIFO__FRAME_SIZE_ADJUST___M;

   field_value = (wtts_flag << SPDIFTX_SUBBUF_FIFO__WAIT_TILL_TS_FLAG___S) & SPDIFTX_SUBBUF_FIFO__WAIT_TILL_TS_FLAG___M;
   field_value |= (fsa << SPDIFTX_SUBBUF_FIFO__FRAME_SIZE_ADJUST___S) & SPDIFTX_SUBBUF_FIFO__FRAME_SIZE_ADJUST___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_SUBBUF_FIFO),
         spdif_virt_offset,mask,field_value,0); ///< update register     

}

//****************************************************************//

void hal_spdif_write_tx_chsts_a(spdif_ch_sts_buf_t tx_ch_sts)
{

   uint32 mask,field_value;

   mask = SPDIFTX_CHA_STAT_BUF0___M;
   field_value = tx_ch_sts.ch_sts_buf0;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF0),
          spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHA_STAT_BUF1___M;
   field_value = tx_ch_sts.ch_sts_buf1;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF1),
           spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHA_STAT_BUF2___M;
   field_value = tx_ch_sts.ch_sts_buf2;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF2),
          spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHA_STAT_BUF3___M;
   field_value = tx_ch_sts.ch_sts_buf3;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF3),
           spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHA_STAT_BUF4___M;
   field_value = tx_ch_sts.ch_sts_buf4;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF4),
           spdif_virt_offset,mask,field_value,0); ///< update register    

   mask = SPDIFTX_CHA_STAT_BUF5___M;
   field_value = tx_ch_sts.ch_sts_buf5;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHA_STAT_BUF5),
           spdif_virt_offset,mask,field_value,0); ///< update register    


}
//****************************************************************//

//****************************************************************//

void hal_spdif_write_tx_chsts_b(spdif_ch_sts_buf_t tx_ch_sts)
{

    uint32 mask,field_value;
    
    mask = SPDIFTX_CHB_STAT_BUF0___M;
    field_value = tx_ch_sts.ch_sts_buf0;
    
    UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF0),
           spdif_virt_offset,mask,field_value,0); ///< update register     
    
    mask = SPDIFTX_CHB_STAT_BUF1___M;
    field_value = tx_ch_sts.ch_sts_buf1;
     
    UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF1),
            spdif_virt_offset,mask,field_value,0); ///< update register     


   mask = SPDIFTX_CHB_STAT_BUF2___M;
   field_value = tx_ch_sts.ch_sts_buf2;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF2),
          spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHB_STAT_BUF3___M;
   field_value = tx_ch_sts.ch_sts_buf3;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF3),
           spdif_virt_offset,mask,field_value,0); ///< update register     

   mask = SPDIFTX_CHB_STAT_BUF4___M;
   field_value = tx_ch_sts.ch_sts_buf4;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF4),
           spdif_virt_offset,mask,field_value,0); ///< update register    

   mask = SPDIFTX_CHB_STAT_BUF5___M;
   field_value = tx_ch_sts.ch_sts_buf5;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CHB_STAT_BUF5),
           spdif_virt_offset,mask,field_value,0); ///< update register    


}

void hal_spdif_write_tx_usrbits_a(spidf_usr_bits_buf_t user_data)
{

   uint32 mask,field_value;
    
   mask = SPDIFTX_USERA_DATA_BUF0___M;
   field_value = user_data.usr_bits_buf0;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF0),
           spdif_virt_offset,mask,field_value,0); ///< update register     
    
   mask = SPDIFTX_USERA_DATA_BUF1___M;
   field_value = user_data.usr_bits_buf1;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF1),
           spdif_virt_offset,mask,field_value,0); ///< update register     


   mask = SPDIFTX_USERA_DATA_BUF2___M;
   field_value = user_data.usr_bits_buf2;
      
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF2),
           spdif_virt_offset,mask,field_value,0); ///< update register  

   mask = SPDIFTX_USERA_DATA_BUF3___M;
   field_value = user_data.usr_bits_buf3;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF3),
           spdif_virt_offset,mask,field_value,0); ///< update register  


   mask = SPDIFTX_USERA_DATA_BUF4___M;
   field_value = user_data.usr_bits_buf4;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF4),
           spdif_virt_offset,mask,field_value,0); ///< update register  


   mask = SPDIFTX_USERA_DATA_BUF5___M;
   field_value = user_data.usr_bits_buf5;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERA_DATA_BUF5),
           spdif_virt_offset,mask,field_value,0); ///< update register  

}

void hal_spdif_write_tx_usrbits_b(spidf_usr_bits_buf_t user_data)
{
   uint32 mask,field_value;
    
   mask = SPDIFTX_USERB_DATA_BUF0___M;
   field_value = user_data.usr_bits_buf0;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF0),
           spdif_virt_offset,mask,field_value,0); ///< update register     
    
   mask = SPDIFTX_USERB_DATA_BUF1___M;
   field_value = user_data.usr_bits_buf1;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF1),
           spdif_virt_offset,mask,field_value,0); ///< update register     


   mask = SPDIFTX_USERB_DATA_BUF2___M;
   field_value = user_data.usr_bits_buf2;
      
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF2),
           spdif_virt_offset,mask,field_value,0); ///< update register  

   mask = SPDIFTX_USERB_DATA_BUF3___M;
   field_value = user_data.usr_bits_buf3;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF3),
           spdif_virt_offset,mask,field_value,0); ///< update register  


   mask = SPDIFTX_USERB_DATA_BUF4___M;
   field_value = user_data.usr_bits_buf4;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF4),
           spdif_virt_offset,mask,field_value,0); ///< update register  


   mask = SPDIFTX_USERB_DATA_BUF5___M;
   field_value = user_data.usr_bits_buf5;
     
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_USERB_DATA_BUF5),
           spdif_virt_offset,mask,field_value,0); ///< update register  
}

void hal_spdif_tx_enable(void)
{
   uint32 mask,field_value;

   mask = SPDIFTX_PORT_SEL__PORT_EN___M;

   field_value = SPDIFTX_PORT_SEL__PORT_EN__TRANSMITTER_ENABLED << SPDIFTX_PORT_SEL__PORT_EN___S;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_PORT_SEL),
            spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_PORT_ENABLE  bit to 1 done");
#endif
}
//****************************************************************//
void hal_spdif_tx_disable(void)
{
   uint32 mask,field_value;
   
   mask = SPDIFTX_PORT_SEL__PORT_EN___M;
   
   field_value = SPDIFTX_PORT_SEL__PORT_EN__TRANSMITTER_DISABLED << SPDIFTX_PORT_SEL__PORT_EN___M;
   
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_PORT_SEL),
           spdif_virt_offset,mask,field_value,0); ///< update register  

#ifdef HAL_SPDIF_DEBUG  
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_PORT_ENABLE  bit to 0 done");
#endif
}


void hal_spdif_get_tsdiff(uint64 *tsdiff)
{
   uint32 lsb32, msb32_1st, msb32_2nd;
   
   /*
    * LSB overflow cases.
    * 1. msb read just before lsb overflow, lsb read just before overflow.
    * 2. msb read just before lsb overflow, lsb read right after overflow. (we need to read updated msb)
    * 3. msb read after lsb overflow, lsb read after lsb overflow.
    */
   do 
   {
      ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_TS_DIFF_2), spdif_virt_offset,&msb32_1st);
      ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_TS_DIFF_1), spdif_virt_offset,&lsb32);
      ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_TS_DIFF_2), spdif_virt_offset,&msb32_2nd); 
 
   }while(msb32_1st != msb32_2nd);

   *tsdiff = ((uint64)msb32_1st<<32)|(lsb32);
}



//****************************************************************//
void hal_spdif_write_tx_crc(spdif_crc_debug_t crc_inf)
{
   uint32 mask,field_value;

   mask = SPDIFTX_DEBUG_CRC_COUNT__MC_DWORD_COUNT___M |
            SPDIFTX_DEBUG_CRC_COUNT__BLOCK_COUNT___M;

   field_value = (crc_inf.mcd_word_cnt << SPDIFTX_DEBUG_CRC_COUNT__MC_DWORD_COUNT___S) & SPDIFTX_DEBUG_CRC_COUNT__MC_DWORD_COUNT___M;
   field_value |= (crc_inf.crc_blk_cnt << SPDIFTX_DEBUG_CRC_COUNT__BLOCK_COUNT___S) & SPDIFTX_DEBUG_CRC_COUNT__BLOCK_COUNT___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_COUNT),
           spdif_virt_offset,mask,field_value,0); ///< update register     

#ifdef HAL_SPDIF_DEBUG  
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_DEBUG_CRC_COUNT register done 0x%x", field_value);
#endif


   mask = SPDIFTX_DEBUG_CRC_CNTL__CRC_CLEAR___M |
            SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___M;

   field_value = (crc_inf.crc_clr<< SPDIFTX_DEBUG_CRC_CNTL__CRC_CLEAR___S) & SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___M;
   field_value |= (crc_inf.crc_en << SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___S) & SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_CNTL),
           spdif_virt_offset,mask,field_value,0); ///< update register     

#ifdef HAL_SPDIF_DEBUG  
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: Programming SPDIFTX_DEBUG_CRC_CNTL register done 0x%x", field_value);
#endif

}
void hal_spdif_tx_clear_crc(void)
{
   uint32 mask,field_value;

   mask = SPDIFTX_DEBUG_CRC_CNTL__CRC_CLEAR___M;

   field_value = (1 << SPDIFTX_DEBUG_CRC_CNTL__CRC_CLEAR___S) & SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_CNTL),
           spdif_virt_offset,mask,field_value,0); ///< update register     


   field_value = (0 << SPDIFTX_DEBUG_CRC_CNTL__CRC_CLEAR___S) & SPDIFTX_DEBUG_CRC_CNTL__CRC_ENABLE___M;
    
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_CNTL),
            spdif_virt_offset,mask,field_value,0); ///< update register     

}

void hal_spdif_read_tx_crc_mc(uint32 *crc)
{
   uint32 value;
      
   ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_RAW),
               spdif_virt_offset,&value);
    
   *crc = (value & SPDIFTX_DEBUG_CRC_RAW__CRC32___M) >> SPDIFTX_DEBUG_CRC_RAW__CRC32___S;
}

void hal_spdif_crc_ready(uint32 *crc_ready)
{
   uint32 value;
      
   ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_CNTL),
               spdif_virt_offset,&value);

   *crc_ready= (value & SPDIFTX_DEBUG_CRC_CNTL__TRIG_READY___M) >> SPDIFTX_DEBUG_CRC_CNTL__TRIG_READY___S;
}

void hal_spdif_read_tx_crc_blk(uint32 *crc_blk)
{

    uint32 value;
       
    ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_DEBUG_CRC_ASM),
                spdif_virt_offset,&value);
    
    *crc_blk= (value & SPDIFTX_DEBUG_CRC_ASM__CRC32___M) >> SPDIFTX_DEBUG_CRC_ASM__CRC32___S;
}


void hal_spdif_read_tx_stats(uint32 * n_frm, uint32 * n_blk)
{
   uint32 value;
  
   ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_STATISTICS),
               spdif_virt_offset,&value);

   *n_frm = (value & SPDIFTX_STATISTICS__BLOCK_NUM___M) >> SPDIFTX_STATISTICS__BLOCK_NUM___S;
   *n_blk = (value & SPDIFTX_STATISTICS__FRAME_NUM___M) >> SPDIFTX_STATISTICS__FRAME_NUM___S;
}


//****************************************************************//

void hal_spdif_get_int_sta(uint32 *p_int_status)
{
 
   ReadRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_INT_STATUS),
               spdif_virt_offset,p_int_status);
}

void hal_spdif_write_int_sta_ack(uint32 val)
{
   uint32 mask;

   mask = SPDIFTX_INT_STATUS___M;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_INT_STATUS),
           spdif_virt_offset, mask, val,0); ///< update register     
}

void hal_spdif_tx_pause(boolean pause)
{
   uint32 mask,fieldValue;
   
   mask = SPDIFTX_CMD__PAUSE___M;

   if(pause)
   {
      fieldValue = 1 << SPDIFTX_CMD__PAUSE___S;
   }
   else
   {
      fieldValue = 0 << SPDIFTX_CMD__PAUSE___S;
   }
   
   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CMD),
           spdif_virt_offset,mask,fieldValue,0); ///< update register    

}

void hal_spdif_tx_clear_all(void)
{

    uint32 mask,field_value;
    
    mask = SPDIFTX_CMD__SUBBUF_FIFO_CLEAR_ALL___M;
    
    field_value = 1 << SPDIFTX_CMD__SUBBUF_FIFO_CLEAR_ALL___S;
    
    UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CMD),
            spdif_virt_offset, mask, field_value, 0); ///< update register     
}


void hal_spdif_tx_abort()
{
   uint32 mask,field_value;
   
   mask = SPDIFTX_CMD__ABORT_BLOCK___M;

   field_value = 1 << SPDIFTX_CMD__ABORT_BLOCK___S;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_CMD),
           spdif_virt_offset,mask,field_value,0); ///< update register     
}

void hal_spdif_tx_mute(boolean en)
{

    uint32 mask,field_value;
    
    mask = SPDIFTX_PORT_CFG__SOFT_MUTE___M;

    if(en)
    {
       field_value = (SPDIFTX_PORT_CFG__SOFT_MUTE__MUTE << SPDIFTX_PORT_CFG__SOFT_MUTE___S) & SPDIFTX_PORT_CFG__SOFT_MUTE___M;
    }
    else
    {
       field_value = (SPDIFTX_PORT_CFG__SOFT_MUTE__NO_MUTE << SPDIFTX_PORT_CFG__SOFT_MUTE___S) & SPDIFTX_PORT_CFG__SOFT_MUTE___M;
    }
    
    UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_PORT_CFG),
            spdif_virt_offset,mask,field_value,0); ///< update register     
}

void hal_spdif_write_timestamp(uint64 timestamp)
{

   uint32 mask,fieldValue;

   mask = SPDIFTX_TIME_STAMP_FIFO_1___M;

   fieldValue = (uint32) timestamp;

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_TIME_STAMP_FIFO_1),
           spdif_virt_offset,mask,fieldValue,0); ///< update register     


   mask = SPDIFTX_TIME_STAMP_FIFO_2___M;

   fieldValue = (uint32)(timestamp >> 32);

   UpdateRegister1(spdif_phy_base + SPDIF_OFFSET(LPASS_SPDIFTX_TIME_STAMP_FIFO_2),
           spdif_virt_offset,mask,fieldValue,0); ///< update register    
}


