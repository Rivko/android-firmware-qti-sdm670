/***************************************************************************************************

 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 
 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.
 
 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
 ***************************************************************************************************/

#include "llcc.h"
//#include "llcc_data.h"

//================================================================================================//
// LLCC boot time initialization
// supports single channel or ALL channel
//================================================================================================//
void LLCC_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
	// Napali/NapaliQ Binnary compatibility
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
	
	// Enable LLCC broadcast for all channels
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	
	// CSR one time settings
	LLCC_config(ddr, channel);
	
	// Initialize RAM
	LLCC_ram_init(ddr, channel);
	
	// configure beac credits
	LLCC_configure_beac_credit(ddr, channel);
	
	// setting LLCC to non-cacheable mode is already in PoR value (bypass mode)
	
	// Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);
}


//================================================================================================//
// llcc One-Time Settings
// supports ALL channel only
//================================================================================================//
void LLCC_config (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
    DDRSS_set_config(ddr, SEQ_DDR_SS_LLCC_BROADCAST_LLCC_OFFSET,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[LLCC_CFG_IDX][0]));
}


//================================================================================================//
// This function provides 3 options:
// 1. set override to non-cacheable, all traffic will bypass LLCC
// 2. set override to cacheable, all traffic will be send to cache regardless of cache attribute
// 3. no override, traffic will be assigned based on cache attribute
// assumption is no other pending interrupt, supports single channel or ALL channel
//================================================================================================//
/* To Do: Add NapaliQ binnary compatiility  */
void LLCC_cacheable_mode_override_ctrl ( DDR_CHANNEL channel, CACHEABLE_MODE cachable_mode)
{
	uint8  ch = 0x0;
	uint32 reg_offset_llcc_feac = 0;
	
	// Enable LLCC broadcast for all channels
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	ch = LLCC_CH_INX(channel);
	
	reg_offset_llcc_feac = REG_OFFSET_LLCC_FEAC(ch);
	
	if (cachable_mode == NON_CACHEABLE)
	{
	// put LLCC in non-cacheable mode override
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_C_AS_NC_OVERRIDE_CFG, 0xFFFFFFFF);
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_NC_AS_C_OVERRIDE_CFG, 0x0);
	}
	else if (cachable_mode == CACHEABLE)
	{
	// put LLCC in cacheable mode override
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_C_AS_NC_OVERRIDE_CFG, 0x0);
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_NC_AS_C_OVERRIDE_CFG, 0xFFFFFFFF);
	}
	else if (cachable_mode == NO_OVERRIDE)
	{
	// clear cacheable mode override/non-cacheable mode override
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_C_AS_NC_OVERRIDE_CFG, 0x0);
	HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_NC_AS_C_OVERRIDE_CFG, 0x0);
	}
	
	// Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);
}

//================================================================================================//
// trigger LLCC BEAC credit loading
// supports single channel or ALL channel
//================================================================================================//
void LLCC_configure_beac_credit (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{

	uint8  ch = 0x0;
	uint32 reg_offset_llcc_beac = 0;
	channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
	
	// Enable LLCC broadcast for all channels
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	ch = LLCC_CH_INX(channel);
	
	reg_offset_llcc_beac = REG_OFFSET_LLCC_BEAC(ch);
	
	// Load the above values
	HWIO_OUTX  (reg_offset_llcc_beac, LLCC_BEAC_FREQ_SWITCH_CFG, 1);
	// clear the trigger
	HWIO_OUTX  (reg_offset_llcc_beac, LLCC_BEAC_FREQ_SWITCH_CFG, 0);
	
	// Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);
}

//================================================================================================//
// Schedule llcc ram wakeup, implement init walkup for non-ret pcbs, 
// then double check wakeup status, 
// supports ANY channel combination
//================================================================================================//
void LLCC_ram_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch = 0x0; // channel is 1hot encoded, ch0~3:0x1,0x2,0x4,0x8
   uint32 all_pcb_mask = 0;

   uint32 reg_offset_llcc_trp = 0;
   uint32 reg_offset_llcc_drp = 0;
   uint32 reg_offset_llcc_pmgr = 0;
   uint32 reg_offset_llcc_feac = 0;
   uint32 reg_offset_llcc_common = 0;
   uint32 reg_offset_llcc_berc = 0;
   
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   all_pcb_mask = ddr->shrm_ddr_ptr->misc.all_pcb_mask;
   
   	// Enable LLCC broadcast for all channels TODO: review broadcast places
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	ch = LLCC_CH_INX(channel);
	
			reg_offset_llcc_drp = REG_OFFSET_LLCC_DRP(ch);
			reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
			reg_offset_llcc_pmgr = REG_OFFSET_LLCC_PMGR(ch);
			reg_offset_llcc_feac = REG_OFFSET_LLCC_FEAC(ch);
            if (((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (!(ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_WARLOCK)|| (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGON) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGONLC) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMAN) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMANLC)) {  
			reg_offset_llcc_common = REG_OFFSET_LLCC_COMMON(ch);
			reg_offset_llcc_berc = REG_OFFSET_LLCC_BERC(ch);
			
			// Assert SHUB clamp_mem (WA for slp_ret_n glitch issue)
			HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET, DPCC_REG_SHUB_CFG_GDSCR, GDSC_SPARE_CTRL_IN, 0x1);
			}
			// Schedule tag RAM wakeup 
			// wakeup_ways = 0xFFFF; // ret_ways | nret_ways | other ram
			HWIO_OUTXF  (reg_offset_llcc_pmgr, LLCC_PMGR_RAM_CMD, WAKEUP_COMMAND, 0x1);

			// Wait for Tag RAM wakeup cmd to complete
			while ((HWIO_INX (reg_offset_llcc_pmgr, LLCC_PMGR_RAM_SLP_STATUS0) & ddr->shrm_ddr_ptr->misc.ram_status0_mask) != 0x0); // Status_awake 4'b0000
			while ((HWIO_INX (reg_offset_llcc_pmgr, LLCC_PMGR_RAM_SLP_STATUS1) & ddr->shrm_ddr_ptr->misc.ram_status1_mask) != 0x0); // Status_awake 4'b0000

			// Clear wakeup command after all scheduled wakeups are finished
			HWIO_OUTXF  (reg_offset_llcc_pmgr, LLCC_PMGR_RAM_CMD, WAKEUP_COMMAND, 0x0);
			
            if (((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (!(ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_WARLOCK)|| (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGON) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGONLC) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMAN) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMANLC)) {
			// De-assert SHUB clamp_mem (WA for slp_ret_n glitch issue)
			HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET, DPCC_REG_SHUB_CFG_GDSCR, GDSC_SPARE_CTRL_IN, 0x0);
			
			//Wait for Tpd2
			seq_wait(36,NS);
			
			volatile int i = 0;
			for(i = 0; i < 50; i++);
			
			// Force clock to run by disable cgc, let clk run for some cycles
			HWIO_OUTXF (reg_offset_llcc_common, LLCC_COMMON_CGC_CFG, LLCC_CLK_ENABLE, 0x1);
			HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CLK_EN_CFG, HKE_CLK_EN, 0x1);
			HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CLK_EN_CFG, MRU_CLK_EN, 0x1);
			HWIO_OUTXF (reg_offset_llcc_berc, LLCC_BERC_CH0_CLK_CTRL, LFILL_DBUF_FIFO_CLK_EN, 0x1);
			
			HWIO_OUTXF (reg_offset_llcc_common, LLCC_COMMON_CGC_CFG, LLCC_CLK_ENABLE, 0x0);
			HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CLK_EN_CFG, HKE_CLK_EN, 0x0);
			HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CLK_EN_CFG, MRU_CLK_EN, 0x0);
			HWIO_OUTXF (reg_offset_llcc_berc, LLCC_BERC_CH0_CLK_CTRL, LFILL_DBUF_FIFO_CLK_EN, 0x0);
            }

			// 	WAY_EN_VEC=0xFFF
	        //  Write into LLCC_TRP_CFG1 to trigger the initialization with walker, ALL_SETS=1, INIT_TRIG=1;
			HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CINIT_CTRL_1, INIT_TRIG, 0x1);

	        //  Write into LLCC_TRP_CFG1 to clear the trigger bit
	        HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CINIT_CTRL_1, INIT_TRIG, 0x0);
			
			// software based wakeup, 200 ns per pcb
			HWIO_OUTXF (reg_offset_llcc_drp, LLCC_DRP_PCB_WAKEUP_SEL, SEL, all_pcb_mask); // done Qfix: replace with all_pcb_mask in shrm_ddr_struct: ddr->shrm_ddr_ptr->misc.all_pcb_mask
			HWIO_OUTXF  (reg_offset_llcc_drp, LLCC_DRP_PCB_CMD, WAKEUP_COMMAND, 0x1);

			// check each pcb are wakeup state
			while ((HWIO_INX (reg_offset_llcc_drp, LLCC_DRP_PCBSLP_STATUS) & ddr->shrm_ddr_ptr->misc.pcb_status_mask) != 0x0); // done Qfix: while ((HWIO_INX (reg_offset_llcc_drp, LLCC_DRP_PCBSLP_STATUS) & all_pcb_mask) != 0x0)

			// Clear wakeup command after all scheduled wakeups are finished
			HWIO_OUTXF  (reg_offset_llcc_drp, LLCC_DRP_PCB_CMD, WAKEUP_COMMAND, 0x0);
			
			// tag ram init step takes very long, 1k cycles, so put it after DRP wakeup
			while ((HWIO_INXF (reg_offset_llcc_trp, LLCC_TRP_CINIT_STATUS, DCI_BUSY)) != 0)
			; // put the semicolon on a separate line to silence this warning (while loop has empty body)
			 
			// Clear qactive override
			HWIO_OUTXF (reg_offset_llcc_pmgr, LLCC_PMGR_RCG_CTRL, RCG_OVERRIDE_QACTIVE, 0);

    // Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);

}


//================================================================================================//
// supports ANY channel combination
// pxi_repeat is set to 0, client needs to call this function multiple times if they want to do multiple flush
//================================================================================================//
void LLCC_PXI_flush (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 start_inx, uint16 end_inx, uint16 way_vec)
{
	uint8  ch = 0x0; // channel is 1hot encoded, ch0~3:0x1,0x2,0x4,0x8
	//uint8  i = 0;
	uint8  num_ch = ddr->shrm_ddr_ptr->misc.num_channel;
	uint32 reg_offset_llcc_trp = 0;
	uint32 reg_offset_llcc_common = 0;

	// Configure LLCC_TRP_CFLUSH_CTRL_1 to trigger operation
	//ALL_SETS=0, 
	//PER_WAY=0
	//OP_CLEAN=0
	//OP_PXI=1
	//PXI_REPEAT=output
	//KEEP_WAY_ON=0
	//CLEAN_TRIG=1
	//CLEAN_WAY_VEC=<Ways to be flushed>

    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
 
   	// Enable LLCC broadcast for all channels
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	ch = LLCC_CH_INX(channel);

	reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
	reg_offset_llcc_common = REG_OFFSET_LLCC_COMMON(ch);
	
	// Disable flush complete interrupt TODO: it's done in LLCC_PXI_TCM_mode_ctrl, remove it after review
	// HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_INTERRUPT_1_ENABLE, FLUSH_DONE, 0);
	
	// Disable Set Index Hashing
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_MISC_CFG, HASH_EN, 0);
	
	// specify start and end index for clean operation
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_0, CLEAN_SET_NDX_START, start_inx);
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_0, CLEAN_SET_NDX_END, end_inx);
	
	// Configure LLCC_TRP_CFLUSH_CTRL_1 and trigger operation // TODO: make it OUTXF3
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, ALL_SETS, 0);
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, CLEAN_WAY_VEC, way_vec);
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, OP_PXI, 1);
	//HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, PXI_REPEAT, pxi_repeat);
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, CLEAN_TRIG, 1);
	
	// clear trigger operation
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, CLEAN_TRIG, 0);
	
	// cannot use AND broadcast mode, use per channel method
	for (ch = 0; ch < num_ch; ch++)
	{reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
		if ((channel >> ch) & 0x1)
		{	// see if flush complete
			while (HWIO_INXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_STATUS, DCC_DONE) != 0x1);
		}
	}
	ch = LLCC_CH_INX(DDR_CH_ALL);
	reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
	
	// change ALL_SETS back to 1 for future llcc_flush settings
	HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_CFLUSH_CTRL_1, ALL_SETS, 1);
	
	// Enable flush complete interrupt TODO: it's done in LLCC_PXI_TCM_mode_ctrl, remove after review
	//HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_INTERRUPT_1_ENABLE, FLUSH_DONE, 1);
   
   	// Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);

}

//================================================================================================//
// supports ANY channel combination
// This function is per scid based activation, one single scid index need to be specified
// act_en = 1 means activate
// act_en = 0 means deactivate
//================================================================================================//
void LLCC_PXI_TCM_mode_ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint8 act_en) 
{
	uint8  ch = 0x0; // channel is 1hot encoded, ch0~3:0x1,0x2,0x4,0x8
	uint8  scid = 0; //scid is our configuration, 0 is fine
	uint32 reg_offset_llcc_trp = 0;
	uint32 reg_offset_llcc_fewc = 0;
	uint32 reg_offset_llcc_feac = 0;
	uint32 tcm0_cfg = 0;
	uint8  num_ch = ddr->shrm_ddr_ptr->misc.num_channel;

    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
	
   	// Enable LLCC broadcast for all channels
	//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
	ch = LLCC_CH_INX(channel);

	reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
	reg_offset_llcc_fewc = REG_OFFSET_LLCC_FEWC(ch);
	reg_offset_llcc_feac = REG_OFFSET_LLCC_FEAC(ch);

	// Activate per SCID 
	if (act_en)
	{
		// enabling TCM mode for that particular SCID 
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCT_n_ATTRIBUTE1_CFG, scid, CACHING_MODE, 1);
		
		// enable all target ways for the TCM SCID
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCT_n_ATTRIBUTE0_CFG, scid, RESRVED_WAYS, 0xFFF); // should be ok for Napali Q, can be replaced with all_way_mask in SHRM_DDR_STRUCT: ddr->shrm_ddr_ptr->misc.all_way_mask
		
		// Disable capacity based allocation for TCM access, this is one hot encoded
		HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_SCID_DISABLE_CAP_ALLOC, CAP_ALLOC_DISABLE, 1 << scid);
		
		// Active TCM sub-cache SCID
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, OPCODE, 1);
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, ACT_TRIG, 1);
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, ACT_TRIG, 0);
		
		// cannot use AND broadcast mode, use per channel method
		for (ch = 0; ch < num_ch; ch++)
		{reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);
			if ((channel >> ch) & 0x1)
			{	// See if activation takes effect
				while (HWIO_INXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_STATUS, scid, ACTIVE) != 0x1); 
			}
		}
		ch = LLCC_CH_INX(DDR_CH_ALL);
		reg_offset_llcc_trp = REG_OFFSET_LLCC_TRP(ch);

		// configure this bit only for boot TCM use case where PXI writes are narrower than 16-byte to suppress errors.
		HWIO_OUTXF (reg_offset_llcc_fewc, LLCC_FEWC_CFG0, TCM_WR_ERR_DISABLE, 1);
		
		// Config TCM base addr in the 3MB range
		HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM_PER_MB_MASK_CFG, PER_MB_MASK, 0xF8);
		
		// nagarwal - added NAPALI to the if condition to exclude WL
		if((ddr->shrm_ddr_ptr->misc.chip_version == 0x0100) && (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI)) // Napali V1
		{
			tcm0_cfg = 0x40000 | (scid << 13);
			HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, tcm0_cfg);
			
			//HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, ENABLE, 1);
			//HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, SCID, scid);
			//HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, ADDR_BASE, 0x0);
			HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_MASK_CFG, ADDR_MASK, 0x4);
		}
		else // Napali V2/Hana/Chiron/NapaliQ
		{
			tcm0_cfg = 0x40000 | (scid << 13);
			HWIO_OUTX (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, tcm0_cfg);
			HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_MASK_CFG, ADDR_MASK, 0x1000);
			HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM_ADDR_BASE_CFG, ADDR_BASE, 0x2B);
			HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM_ADDR_BASE_CFG, TCM_SIZE, 0xc00);
		}
		
		// Disable flush complete interrupt 
		HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_INTERRUPT_1_ENABLE, FLUSH_DONE, 0);
	}
	else
	{
		HWIO_OUTXF (reg_offset_llcc_feac, LLCC_FEAC_TCM0_CFG, ENABLE, 0);
		
		// Deactive TCM sub-cache SCID
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, OPCODE, 2);
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, ACT_TRIG, 1);
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_ACT_CTRL, scid, ACT_TRIG, 0);
		
		//DDRSS_Broadcast_LLCC_Config(channel, OR_RDATA);
		
		// See if deactivation takes effect
		while (HWIO_INXFI (reg_offset_llcc_trp, LLCC_TRP_SCID_n_STATUS, scid, ACTIVE) != 0x0); 
		
		HWIO_OUTXF (reg_offset_llcc_fewc, LLCC_FEWC_CFG0, TCM_WR_ERR_DISABLE, 0);
		
		// restore TCM mode for that particular SCID 
		HWIO_OUTXFI (reg_offset_llcc_trp, LLCC_TRP_SCT_n_ATTRIBUTE1_CFG, scid, CACHING_MODE, 0);
		
		// Enable flush complete interrupt
		HWIO_OUTXF (reg_offset_llcc_trp, LLCC_TRP_INTERRUPT_1_ENABLE, FLUSH_DONE, 1);
	}
	
   	// Disable LLCC broadcast
	//DDRSS_Broadcast_LLCC_Config(DDR_CH_NONE, NO_RDATA);


}

