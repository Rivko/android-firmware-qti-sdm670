/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
================================================================================*/

#include "mc_pxi.h"
#include "mc.h"

void MC_PXI_Payload_Config (uint8 ch, PXI_BLOCK block, uint8 num,\
                            uint32 rpt_cntr, uint32 nop_cntr, uint32 task_cntr,\
                            uint32 task_ptr, uint32 error_clr)
{
   uint32 value          = 0;
   uint32 reg_offset_pxi_secure = 0;
      
   value = ((error_clr << 30) & 0x40000000) /* PAYLOAD[30]    = ERROR_CLR */ |\
           ((task_ptr  << 25) & 0x3E000000) /* PAYLOAD[29:25] = TASK_PTR  */ |\
           ((task_cntr << 20) & 0x01F00000) /* PAYLOAD[24:20] = TASK_CNTR */ |\
           ((nop_cntr  << 12) & 0x0003F000) /* PAYLOAD[17:12] = NOP_CNTR  */ |\
           ((rpt_cntr  <<  0) & 0x000003FF) /* PAYLOAD[9:0]   = RPT_CNTR  */ ;  

   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_CAOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CAOP_n_PAYLOAD, num, value);
         break;
      case PXI_CAWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CADATA_n_PAYLOAD, num, value);
         break;	 
      case PXI_DQOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQOP_n_PAYLOAD, num, value);
         break;	 
      case PXI_DQWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQDATA_n_PAYLOAD, num, value);
         break;	 
      case PXI_DQRD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_PAYLOAD, num, value);
         break;	 
      default:
         break;
   }
}

void MC_PXI_Task_Config (uint8 ch, PXI_BLOCK block, uint8 num,\
                         uint32 alu_cfg, uint32 data0_ptr, uint32 data1_ptr,\
                         uint32 data0_sel, uint32 data1_sel, uint32 sel_spg,\
                         uint32 spg_mode, uint32 txvalid, uint32 mux_mode)
{
   uint32 value          = 0;
   uint32 reg_offset_pxi_secure = 0;
      
   value = ((mux_mode  << 27) & 0x18000000) /* TASK[28:27] = MUXING_MODE */ |\
           ((txvalid   << 26) & 0x04000000) /* TASK[26]    = TXAVLID     */ |\
           ((spg_mode  << 25) & 0x02000000) /* TASK[25]    = SPG_MODE    */ |\
           ((sel_spg   << 24) & 0x01000000) /* TASK[24]    = SELECT_SPG  */ |\
           ((data1_sel << 22) & 0x00C00000) /* TASK[23:22] = DATA1_SEL   */ |\
           ((data0_sel << 20) & 0x00300000) /* TASK[21:20] = DATA0_SEL   */ |\
           ((data1_ptr << 12) & 0x000FF000) /* TASK[19:12] = DATA1_PTR   */ |\
           ((data0_ptr <<  4) & 0x00C00FF0) /* TASK[11:4]  = DATA0_PTR   */ |\
           ((alu_cfg)         & 0x0000000F) /* TASK[3:0]   = ALU_CFG     */ ;

   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_CAOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CAOP_n_TASK, num, value);
         break;
      case PXI_CAWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CADATA_n_TASK, num, value);
         break;	 
      case PXI_DQOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQOP_n_TASK, num, value);
         break;	 
      case PXI_DQWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQDATA_n_TASK, num, value);
         break;	 
      case PXI_DQRD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_TASK, num, value);
         break;	 
      default:
         break;
   }
}

void MC_PXI_Opcode_Config (uint8 ch, PXI_BLOCK block, uint8 num,\
                           uint32 precalc_tstamp, uint32 precalc_opcode, uint32 phase,\
                           uint32 bl, uint32 cs, uint32 rnw_or_cke, uint32 op_type, uint32 ck_en)
{
   uint32 value          = 0;
   uint32 reg_offset_pxi_secure = 0;
    
   if (block == PXI_CAOP) {
      value = ((ck_en          << 20) & 0x00100000) /* OPCODE[20]    = CK_EN          */ |\
              ((op_type        << 16) & 0x000F0000) /* OPCODE[19:16] = OP_TYPE        */ |\
              ((rnw_or_cke     << 14) & 0x0000C000) /* OPCODE[15:14] = CKE            */ |\
              ((cs             << 12) & 0x00003000) /* OPCODE[13:12] = CS             */ |\
              ((bl             << 11) & 0x00000800) /* OPCODE[11]    = BL             */ |\
              ((phase          << 10) & 0x00000400) /* OPCODE[10]    = PHASE          */ |\
              ((precalc_opcode <<  5) & 0x000003E0) /* OPCODE[9:5]   = PRECALC_OPCODE */ |\
              ((precalc_tstamp)       & 0x00C0001F) /* OPCODE[4:0]   = PRECALC_TSTAMP */ ;
   }
   else if (block == PXI_DQOP) {
      value = ((rnw_or_cke     << 14) & 0x00004000) /* OPCODE[14]    = RNW            */ |\
              ((cs             << 12) & 0x00003000) /* OPCODE[13:12] = CS             */ |\
              ((bl             << 11) & 0x00000800) /* OPCODE[11]    = BL             */ |\
              ((phase          << 10) & 0x00000400) /* OPCODE[10]    = PHASE          */ |\
              ((precalc_opcode <<  5) & 0x000003E0) /* OPCODE[9:5]   = PRECALC_OPCODE */ |\
              ((precalc_tstamp)       & 0x00C0001F) /* OPCODE[4:0]   = PRECALC_TSTAMP */ ;
   }

   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_CAOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CAOP_n_DATA, num, value);
         break;
      case PXI_DQOP:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQOP_n_DATA, num, value);
         break;	 
      default:
         break;
   }
}

void MC_PXI_DQ_Data_Config (uint8 ch, PXI_BLOCK block, void *data)
{
   uint8  max_idx        = 0;   
   uint8  i              = 0;
   uint32 reg_offset_pxi_secure = 0;
   
   max_idx = 32;

   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_DQWD:
         for (i = 0; i < max_idx; i++) {
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQDATA_n_0_2_15_ENTRY, i, (*((uint64 *)data + i)) & 0xFFFFFFFF);
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQDATA_n_16_2_31_ENTRY, i, (*(uint64 *)data + i) >> 32);
         }
         break;
      case PXI_DQRD:
         for (i = 0; i < max_idx; i++) {	
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_0_2_15_ENTRY, i, (*((uint64 *)data + i)) & 0xFFFFFFFF);
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_16_2_31_ENTRY, i, (*(uint64 *)data + i) >> 32);
         }
         break;			   
      case PXI_DQDM:
         for (i = 0; i < max_idx; i++) {			
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DMDATA_n_DATA, i, *((uint8 *)data + i));
         }				  
         break;
      default:
         break;
   }
}

void MC_PXI_CA_Data_Config (uint8 ch, PXI_BLOCK block, uint8 num,\
                            uint32 ca_p0, uint32 ca_p1, uint32 cs_p0, uint32 cs_p1)
{
   uint16 data           = 0;
   uint32 reg_offset_pxi_secure = 0;

   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_CAWD:
            data = ((cs_p1 << 14) & 0xC000) /* CAWD[15:14] = CS_P1 */ |\
                   ((cs_p0 << 12) & 0x3000) /* CAWD[13:12] = CS_P0 */ |\
                   ((ca_p1 <<  6) & 0x0FC0) /* CAWD[11: 6] = CA_P1 */ |\
				   ((ca_p0      ) & 0x003F) /* CAWD[ 5: 0] = CA_P0 */ ;
            HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CADATA_n_DATA, num, data);
         break;
      default:
         break;
   }
}

void MC_PXI_GPR_Config (uint8 ch, PXI_BLOCK block, uint8 num, uint32 gpr)
{
   uint32 reg_offset_pxi_secure = 0;
 
   reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch); 

   switch (block) {
      case PXI_CAWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_CADATA_n_GPR, num, gpr);
         break;
      case PXI_DQWD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DQDATA_n_GPR, num, gpr);
         break;
      case PXI_DQDM:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_DMDATA_n_GPR, num, gpr);
         break;       
      case PXI_DQRD:
         HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_GPR, num, gpr);	  
      default:
         break;
   }
}
         
void MC_PXI_Latency_Config (uint8 ch)
{
   uint32 rl             = 0;
   uint32 wl             = 0;
   uint32 value          = 0;
   uint32 reg_offset_pxi = 0;
   uint32 reg_offset_dtc = 0;
 
   /* Read MC latency settings and assume the same settings apply to all channels */
   reg_offset_dtc = REG_OFFSET_MC_DTC(ch);         
   rl = HWIO_INXF(reg_offset_dtc, DTC_COL_TIMING_CFG_RL, IN_CK);
   wl = HWIO_INXF(reg_offset_dtc, DTC_COL_TIMING_CFG_WL, IN_CK); 

   value = (((wl + 4) << 8) & 0x00003F00) /* RDWR_LATENCY[13:8] = WR_LATENCY */ |\
            ((rl + 2)       & 0x0000003F) /* RDWR_LATENCY[5:0]  = RD_LATENCY */ ;

   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RDWR_LATENCY, value);
}

void MC_PXI_Setting_Config (uint8 ch, uint8 setting_idx)
{
   uint32 value                       = 0;
   uint32 reg_offset_pxi              = 0;
   struct pxi_setting_struct *setting = NULL;
 
   setting = &pxi_setting_table[setting_idx];

   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);

   value = setting->clk_on;
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_CLK_ON, value);

   value = ((setting->pxi_sel.raw_open << 2) & 0x00000004) /* PXI_SEL[2] = PXI_RAW_OPEN */ |\
           ((setting->pxi_sel.in_sel   << 1) & 0x00000002) /* PXI_SEL[1] = PXI_IN_SEL   */ |\
           ((setting->pxi_sel.out_sel      ) & 0x00000001) /* PXI_SEL[0] = PXI_OUT_SEL  */ ;
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_SEL, value);

   value = ((setting->opcode_sel.dly_caop_one_clk  << 18) & 0x00040000) /* PXI_OPCODE_SEL[18] = DLY_CAOP_ONE_CLK  */ |\
           ((setting->opcode_sel.dly_cawd_one_clk  << 17) & 0x00020000) /* PXI_OPCODE_SEL[17] = DLY_CAWD_ONE_CLK  */ |\
           ((setting->opcode_sel.dly_dqop_one_clk  << 16) & 0x00010000) /* PXI_OPCODE_SEL[16] = DLY_DQOP_ONE_CLK  */ |\
           ((setting->opcode_sel.dm_done_in_mc_sel <<  8) & 0x00000100) /* PXI_OPCODE_SEL[8]  = DM_DONE_IN_MC_SEL */ |\
           ((setting->opcode_sel.cq_timestamp_sel  <<  5) & 0x00000020) /* PXI_OPCODE_SEL[5]  = CQ_TIMESTAMP_SEL  */ |\
           ((setting->opcode_sel.dq_timestamp_sel  <<  4) & 0x00000010) /* PXI_OPCODE_SEL[4]  = DQ_TIMESTAMP_SEL  */ |\
           ((setting->opcode_sel.ca_opcode_sel     <<  1) & 0x00000002) /* PXI_OPCODE_SEL[1]  = CQ_OPCODE_SEL     */ |\
           ((setting->opcode_sel.dq_opcode_sel          ) & 0x00000001) /* PXI_OPCODE_SEL[0]  = DQ_OPCODE_SEL     */ ;
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_OPCODE_SEL, value);

   value = ((setting->start_addr.caop_addr << 24) & 0xFF000000) /* PXI_WR_START_ADDR[31:24] = CAOP */ |\
           ((setting->start_addr.cawd_addr << 16) & 0x00FF0000) /* PXI_WR_START_ADDR[23:16] = CAWD */ |\
           ((setting->start_addr.dqop_addr <<  8) & 0x0000FF00) /* PXI_WR_START_ADDR[15:8]  = DQOP */ |\
           ((setting->start_addr.dqwd_addr      ) & 0x000000FF) /* PXI_WR_START_ADDR[7:0]   = DQWD */ ;
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_WR_START_ADDR, value);

   value = (setting->start_addr.dqrd_addr & 0x000000FF) /* PXI_RD_START_ADDR[7:0] = DQRD */ ;
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RD_START_ADDR, value);
}

/* e.g. uint8 go = PXI_DQRD | PXI_DQWD | PXI_DQOP | PXI_CAWD | PXI_CAOP; */
void MC_PXI_Control_Config (uint8 ch, uint32 go, uint32 step, uint32 abort)
{
   uint32 value          = 0;
   uint32 reg_offset_pxi = 0;

   value = ((abort << 16) & 0x001F0000) /* PXI_CTL[20:16] = CAOW/CAWD/DQOP/DQWD/DQRD_ABORT */ |\
           ((step  << 8 ) & 0x00001F00) /* PXI_CTL[12:8]  = CAOW/CAWD/DQOP/DQWD/DQRD_STEP  */ |\
           ((go)          & 0x0000001F) /* PXI_CTL[4:0]   = CAOW/CAWD/DQOP/DQWD/DQRD_GO    */ ;

   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_CTL, value);
   
   if (go != 0)
       HWIO_OUTXF (reg_offset_pxi, DTTS_CSR_PXI_CGC_CFG, CFG_CLK_HOLD_DELAY, 0x1);
   else
       HWIO_OUTXF (reg_offset_pxi, DTTS_CSR_PXI_CGC_CFG, CFG_CLK_HOLD_DELAY, 0x0);
   
}

void MC_PXI_RD_Beat_Status_Clear (uint8 ch)
{
   uint32 reg_offset_pxi = 0;

   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RD_BEAT_CNTL, 0x1);
   HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RD_BEAT_CNTL, 0x0);
}

/* cnt_mode = PXI_BIT_ERR_MODE or PXI_BYTE_ERR_MODE */
void MC_PXI_ELOG_Config (uint8 ch, uint8 enable, PXI_ERR_MODE cnt_mode,\
                         uint32 cnt_wdw, uint32 threshold)
{
   uint32 value                 = 0;
   uint32 reg_offset_pxi        = 0;
   uint32 reg_offset_pxi_secure = 0;
   
   if (enable == 1) {
      // Disable MISR mode		 
	  reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RD_PATH_CTRL, 0x10);

      // Set error count mode and window position
	  reg_offset_pxi_secure = REG_OFFSET_MC_PXI_SECURE(ch);
      value = ((cnt_mode << 3) & 0x00000008) |\
              ((cnt_wdw      ) & 0x00000007) ;		    
      HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_ELOG_TABLE_RW, 0, value);

      // Set syndrome threshold
      HWIO_OUTXI (reg_offset_pxi_secure, DTTS_RAM_PXI_RD_n_ELOG_TABLE_RW, 1, threshold);
   }	    
}

void MC_PXI_MISR_Config (uint8 ch, uint8 enable, uint32 seed_msb, uint32 seed_lsb)
{
   uint32 reg_offset_pxi = 0;
   
   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
   if (enable == 1) {
      // Disable ELOG mode		 
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_RD_PATH_CTRL, 0x1);
      // Enable and configure MISR	 
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_MISR_SEED_MSB, seed_msb);
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_MISR_SEED_LSB, seed_lsb);
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_MISR_CNTL, 0x3);
   }
   else {
      // Disable MISR
      HWIO_OUTX (reg_offset_pxi, DTTS_CSR_PXI_MISR_CNTL, 0x0);
   }	    
}

void MC_PXI_MISR_Calc_1 (uint32 * misr_data, uint32 misr_input)
{

   *(misr_data)     = misr_input & 1;
   *(misr_data + 1) = (misr_input >> 1)  & 1;
   *(misr_data + 2) = (misr_input >> 2)  & 1;
   *(misr_data + 3) = (misr_input >> 22) & 1;
   *(misr_data + 4) = (misr_input & 0x3FFFFF) >> 3;
   *(misr_data + 5) = (misr_input & 0xFF800000) >> 23;
}

uint32 MC_PXI_MISR_Calc_2 (uint32 * misr_data, uint32 misr_input)
{
   uint32 result;

   result = (*(misr_data) ^ *(misr_data + 1))         |\
            (*(misr_data) ^ *(misr_data + 2))         |\
            (*(misr_data + 4) << 2)                   |\
            ((*(misr_data + 3) ^ *(misr_data)) << 21) |\
            (*(misr_data + 5) << 22)                  |\
            (*(misr_data) << 31)                      ;

   return (result ^ misr_input);
}


/* Only support single channel */
uint8 MC_PXI_MISR_Compare (uint8 ch, uint32 beats, uint32 * misr_in_lsb, uint32 * misr_in_msb)
{
   uint32 i              = 0;
   uint32 misr_out_lsb   = 0;
   uint32 misr_out_msb   = 0;
   uint32 data_lsb[6]    = {0};
   uint32 data_msb[6]    = {0};
   uint32 reg_offset_pxi = 0;

   reg_offset_pxi = REG_OFFSET_MC_PXI(ch);
   misr_out_lsb = HWIO_INX (reg_offset_pxi, DTTS_CSR_PXI_MISR_SEED_LSB);
   misr_out_msb = HWIO_INX (reg_offset_pxi, DTTS_CSR_PXI_MISR_SEED_MSB);
   
   MC_PXI_MISR_Calc_1(data_lsb, misr_out_lsb);
   MC_PXI_MISR_Calc_1(data_msb, misr_out_msb);

   for (i = 0; i < beats; i++) {     
      misr_out_lsb = MC_PXI_MISR_Calc_2(data_lsb, misr_in_lsb[i]);
      MC_PXI_MISR_Calc_1(data_lsb, misr_out_lsb);

      misr_out_msb = MC_PXI_MISR_Calc_2(data_msb, misr_in_msb[i]);
      MC_PXI_MISR_Calc_1(data_msb, misr_out_lsb);
   }

   // Check beat status
   while (HWIO_INX (reg_offset_pxi, DTTS_CSR_PXI_RD_BEAT_STATUS) != beats);

   // Compare calculated MISR results with design generated MISR
   if ((misr_out_lsb == HWIO_INX (reg_offset_pxi, DTTS_CSR_PXI_MISR_STATUS_LSB)) && \
       (misr_out_msb == HWIO_INX (reg_offset_pxi, DTTS_CSR_PXI_MISR_STATUS_MSB))) {
      return 0; // Return zero to indicate no error
   } 
   else {
      return 1; // Return non-zero value to indicate error
   }
}

//Fill All PXI tables for PXI Operation, wr_rd = 0 for write, 1 for read, 2 for both
void Initialize_PXI_Tables(BOOT_DDR_STRUCT *ddr, uint8 ch)
{   
    uint32 REG_OFFSET_MC_PXI              =  0;
    uint32 REG_OFFSET_MC_PXI_SECURE       =  0;
	
	uint8 napali_v1 = 0;
	uint32 rpt_ctr = 0;
	
    REG_OFFSET_MC_PXI            = REG_OFFSET_MC_PXI(ch);
    REG_OFFSET_MC_PXI_SECURE     = REG_OFFSET_MC_PXI_SECURE(ch);
	
	if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))
    {
        napali_v1 = 1;
        rpt_ctr = 0xe;
    }
    else
    {
        napali_v1 = 0;
        rpt_ctr = 0x1f;
    }

    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DQ_OPCODE_SEL, 0x3);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, CA_OPCODE_SEL, 0x3);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DQ_TIMESTAMP_SEL, 0x0);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, CA_TIMESTAMP_SEL, 0x0);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DM_DONE_IN_MC_SEL, 0x1);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DLY_DQOP_ONE_CLK, 0x0);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DLY_CAOP_ONE_CLK, 0x0);
    HWIO_OUTXF (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_OPCODE_SEL, DLY_CAWD_ONE_CLK, 0x0);
    HWIO_OUTXFI (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_ALU_n_CTRL, 0, RRBC_SEL, 0x2);
    HWIO_OUTXFI (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_ALU_n_VALUE, 0, INC_DEC, 0x20);
    HWIO_OUTXFI (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_ALU_n_CTRL, 0, BL, 0x1);
    
    // bit error count mode
    HWIO_OUTXI (REG_OFFSET_MC_PXI_SECURE ,DTTS_RAM_PXI_RD_n_ELOG_TABLE_RW, 0, 0x00000008);

    //capture first miscompare
    HWIO_OUTXI (REG_OFFSET_MC_PXI_SECURE ,DTTS_RAM_PXI_RD_n_ELOG_TABLE_RW, 1, 0x00000001);

    //disable MISR
    HWIO_OUTXF  (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_RD_PATH_CTRL, MISR_DISABLE , 0x1); 

    // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr)
    MC_PXI_Payload_Config (ch, PXI_CAOP, 0, 0x0, 0x1, 0x1, 0x5, 0x0);// CLK_ON

    // (uint8 num, uint8 alu_cfg, uint32 data0_ptr, uint32 data1_ptr uint8 data0_sel, uint8 data1_sel, uint8 sel_spg, uint8 spg_mode, uint8 txvalid,uint8 muxing_mode) 
    MC_PXI_Task_Config (ch, PXI_CAOP, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
    MC_PXI_Task_Config (ch, PXI_CAOP, 1, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
    MC_PXI_Task_Config (ch, PXI_CAOP, 2, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
    MC_PXI_Task_Config (ch, PXI_CAOP, 3, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
    MC_PXI_Task_Config (ch, PXI_CAOP, 4, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
	MC_PXI_Task_Config (ch, PXI_CAOP, 5, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
	MC_PXI_Task_Config (ch, PXI_CAOP, 6, 0x0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
	MC_PXI_Task_Config (ch, PXI_CAOP, 7, 0x0, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
	MC_PXI_Task_Config (ch, PXI_CAOP, 8, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);

    //(uint8 num, uint8 precalc_tstamp, uint8 precalc_opcode, uint8 phase, uint8 bl, uint8 cs, uint8 rnw_or_cke, uint8 op_type, uint8 ck_en)
    MC_PXI_Opcode_Config (ch, PXI_CAOP, 0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //OCR_WR_P0R0_BL1 //4 cycle cmd (act, write, read)
    MC_PXI_Opcode_Config (ch, PXI_CAOP, 1, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //OCR_WR_P0R1_BL1
    MC_PXI_Opcode_Config (ch, PXI_CAOP, 2, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CMD_CKE_ALL_OFF opcode
    MC_PXI_Opcode_Config (ch, PXI_CAOP, 3, 0x0, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CMD_CKE1_OFF_CKE0_ON opcode
    MC_PXI_Opcode_Config (ch, PXI_CAOP, 4, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CMD_CKE1_ON_CKE0_OFF opcode
	MC_PXI_Opcode_Config (ch, PXI_CAOP, 5, 0x0, 0xD, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CLK_ON opcode
	MC_PXI_Opcode_Config (ch, PXI_CAOP, 6, 0x0, 0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CLK_OFF opcode
	MC_PXI_Opcode_Config (ch, PXI_CAOP, 7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //OCR_WR_P0R0_BL0
	MC_PXI_Opcode_Config (ch, PXI_CAOP, 8, 0x0, 0xB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); //CMD_CKE_ALL_ON

    // (uint8 num, uint8 alu_cfg, uint32 data0_ptr, uint32 data1_ptr uint8 data0_sel, uint8 data1_sel, uint8 sel_spg, uint8 spg_mode, uint8 txvalid, uint8 muxing_mode ) 
    MC_PXI_Task_Config (ch, PXI_DQOP, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,0x0); //muxing_mode =0 . Do not send valid out
    MC_PXI_Task_Config (ch, PXI_DQOP, 1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);//BL32 write R0
    MC_PXI_Task_Config (ch, PXI_DQOP, 2, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);//BL32 read R0
	MC_PXI_Task_Config (ch, PXI_DQOP, 3, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);//BL32 write R1
    MC_PXI_Task_Config (ch, PXI_DQOP, 4, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);//BL32 read R1
  
    //(uint8 num, uint8 precalc_tstamp, uint8 precalc_opcode, uint8 phase, uint8 bl, uint8 cs, uint8 rnw_or_cke, uint8 op_type, uint8 ck_en)
    MC_PXI_Opcode_Config (ch, PXI_DQOP, 0, 0x0, 0x11, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); // OCR_WR_P1R0_BL1. Use phase 1 cmd for tdqss as WR latency is always even. Phy breaking OCR_WR_P0R0_BL1 i.e BL32 into BL16 and BL32 cmd??
    MC_PXI_Opcode_Config (ch, PXI_DQOP, 1, 0x0, 0x5, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0); // OCR_RD_P0R0_BL1 . RNW =1 for read command for proper mc timestamp that includes read_latency
	MC_PXI_Opcode_Config (ch, PXI_DQOP, 2, 0x0, 0x13, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0); // OCR_WR_P1R1_BL1. Use phase 1 cmd for tdqss as WR latency is always even. Phy breaking OCR_WR_P0R0_BL1 i.e BL32 into BL16 and BL32 cmd??
    MC_PXI_Opcode_Config (ch, PXI_DQOP, 3, 0x0, 0x7, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0); // OCR_RD_P0R1_BL1 . RNW =1 for read command for proper mc timestamp that includes read_latency
  
    // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr) 
	MC_PXI_Payload_Config (ch, PXI_CAWD, 0, 0x0, 0xf, 0x2, 0x0, 0x0); //Activate cmd. 
    
    // (uint8 num, uint8 alu_cfg, uint32 data0_ptr, uint32 data1_ptr uint8 data0_sel, uint8 data1_sel, uint8 sel_spg, uint8 spg_mode, uint8 txvalid, uint8 muxing_mode ) 
    MC_PXI_Task_Config (ch, PXI_CAWD, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1);  //activate
    
    if (napali_v1)
    {
        MC_PXI_Payload_Config (ch, PXI_CAWD,1, 0x0, 0x6 , 0x2, 0x1, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,2, 0x0, 0x6 , 0x2, 0x2, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,3, 0x0, 0x6 , 0x2, 0x3, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,4, 0x0, 0x6 , 0x2, 0x4, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,5, 0x0, 0x6 , 0x2, 0x5, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,6, 0x0, 0x6 , 0x2, 0x6, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,7, 0x0, 0x6 , 0x2, 0x7, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,8, 0x0, 0x6 , 0x2, 0x8, 0x0); //BL32 Write/Read cmd. 
        MC_PXI_Payload_Config (ch, PXI_CAWD,9, 0x0, 0x6 , 0x2, 0x9, 0x0); //BL32 Write/Read cmd. 
        MC_PXI_Payload_Config (ch, PXI_CAWD,10, 0x0, 0x6 , 0x2, 0xa, 0x0); //BL32 Write/Read cmd.
        MC_PXI_Payload_Config (ch, PXI_CAWD,11, 0x0, 0x6 , 0x2, 0xb, 0x0); //BL32 Write/Read cmd. 
        MC_PXI_Payload_Config (ch, PXI_CAWD,12, 0x0, 0x6 , 0x2, 0xc, 0x0); //BL32 Write/Read cmd. 
        MC_PXI_Payload_Config (ch, PXI_CAWD,13, 0x0, 0x6 , 0x2, 0xd, 0x0); //BL32 Write/Read cmd. 
        MC_PXI_Payload_Config (ch, PXI_CAWD,14, 0x0, 0x6 , 0x2, 0xe, 0x0); //BL32 Write/Read cmd.  
        MC_PXI_Payload_Config (ch, PXI_CAWD,15, 0x0, 0x6 , 0x2, 0xf, 0x0); //BL32 Write/Read cmd. 
        
        MC_PXI_Task_Config (ch, PXI_CAWD, 1, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 2, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 3, 0x0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 4, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 5, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 6, 0x0, 0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 7, 0x0, 0xE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 8, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 9, 0x0, 0x12, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 10, 0x0, 0x14, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 11, 0x0, 0x16, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 12, 0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 13, 0x0, 0x1A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 14, 0x0, 0x1C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
        MC_PXI_Task_Config (ch, PXI_CAWD, 15, 0x0, 0x1E, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,0x1);
    }
    else
    {
        MC_PXI_Payload_Config (ch, PXI_CAWD, 1, rpt_ctr, 0x6 , 0x2, 0x1, 0x0); //BL32 Write/Read cmd.    Since ca_payload nop is 0x7  , ca_wd payload_nop=0x6
        
        MC_PXI_Task_Config (ch, PXI_CAWD, 1, 0x1, 0x2, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x1); 
    }

    // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr)
    MC_PXI_Payload_Config (ch, PXI_DQRD, 0, 0x0, 0x0, 0x0, 0x0, 0x1); // clears error log
    MC_PXI_Payload_Config (ch, PXI_DQRD, 1, rpt_ctr, 0x0 , 0x8, 0x0, 0x0);//BL32 read 15
        
    // (uint8 num, uint8 alu_cfg, uint32 data0_ptr, uint32 data1_ptr uint8 data0_sel, uint8 data1_sel, uint8 sel_spg, uint8 spg_mode, uint8 txvalid, uint8 muxing_mode ) 
    MC_PXI_Task_Config (ch, PXI_DQRD, 0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x0, 0x0, 0x1, 0x1);  //prbs
    
    MC_PXI_Payload_Config (ch, PXI_DQWD, 1, rpt_ctr, 0x0 , 0x8, 0x1, 0x0);//BL32 write 15

    // (uint8 num, uint8 alu_cfg, uint32 data0_ptr, uint32 data1_ptr uint8 data0_sel, uint8 data1_sel, uint8 txvalid, uint8 muxing_mode) 
    MC_PXI_Task_Config (ch, PXI_DQWD, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0);//Dummy to delay dq_op vs dq_wd and to wait while ca is sending activate cmd
    MC_PXI_Task_Config (ch, PXI_DQWD, 1, 0x0, 0x0, 0x0, 0x2, 0x2, 0x0, 0x0, 0x1, 0x1);  //prbs
}

void Update_PXI_Tables(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 wr_rd)
{   
    uint32 REG_OFFSET_MC_PXI              =  0;
    uint32 REG_OFFSET_MC_PXI_SECURE       =  0;
    uint32 curr_clk_khz = 0;
	uint8 sw_status = 0;
	
    uint32 reg_cabo_osu_offset       = 0;
	
	uint8 napali_v1 = 0;
	uint32 rpt_ctr = 0;
    uint32 csp0 = 0;
    uint32 cap0 = 0;
	
	reg_cabo_osu_offset            = REG_OFFSET_MC_OSU(ch);
    REG_OFFSET_MC_PXI            = REG_OFFSET_MC_PXI(ch);
    REG_OFFSET_MC_PXI_SECURE     = REG_OFFSET_MC_PXI_SECURE(ch);
	
    curr_clk_khz = ddr->shrm_ddr_ptr->misc.current_clk_in_kHz;
	
	if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))
    {
        napali_v1 = 1;
        rpt_ctr = 0xe;
    }
    else
    {
        napali_v1 = 0;
        rpt_ctr = 0x1f;
    }

    if (cs)
        csp0 = 0x2;
    else
        csp0 = 0x1;

    if (wr_rd)
        cap0 = 0x22;
    else
        cap0 = 0x24;
	
	//Required as the o_ch*_cabo_caphy_qfi_clk gets enabled one cycle late so SSP stage doe snot propagate cmd in 1st clk cycle
      //early_cmd_valid, early_data_valid is 0 in case outbound_sel=1 and anyways mpe stuff is used and not the one after pxi_mux
      HWIO_OUTXF2 (reg_cabo_osu_offset,OSU_QFI_CLK_CTRL,QFI_CLK_SET,QFI_CLK_OVERRIDE_EN,0x1,0x1);
		 
      HWIO_OUTX (reg_cabo_osu_offset,OSU_LOAD_CONFIG, 0x1);
      sw_status = 0;
      do {
         sw_status = HWIO_INX (reg_cabo_osu_offset, OSU_LOAD_CONFIG);
      } while(sw_status);
      ///////////////////////////////////////////////

    MC_PXI_Latency_Config (ch);
		 
    //hard coded rrbc  
    //row  = rrbc_val[16:0]
    //col  = rrbc_val[26:17]
    //ba   = rrbc_val[29:27]
    //rank = rrbc_val[31:30]
        

    // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr)
    if (cs)
    {
        MC_PXI_GPR_Config (ch, PXI_CAWD, 4, 0x80000000);
        
        MC_PXI_Payload_Config (ch, PXI_CAOP, 1, 0x0, 0x6, 0x1, 0x4, 0x0);// CKE1 on
        MC_PXI_Payload_Config (ch, PXI_CAOP, 2, 0x0, 0x10, 0x1, 0x1, 0x0);// ACT
        MC_PXI_Payload_Config (ch, PXI_CAOP, 3, rpt_ctr, 0x7, 0x1, 0x1, 0x0);//tccd=16tck => 8 mc clk_cycles i.e. 7 nops. repeat =14 => 15 BL32 cmd 
    }
    else
    {
        MC_PXI_GPR_Config (ch, PXI_CAWD, 4, 0x40000000);
        
		MC_PXI_Payload_Config (ch, PXI_CAOP, 1, 0x0, 0x6, 0x1, 0x3, 0x0);// CKE0 on
        MC_PXI_Payload_Config (ch, PXI_CAOP, 2, 0x0, 0x10, 0x1, 0x0, 0x0);// ACT
        MC_PXI_Payload_Config (ch, PXI_CAOP, 3, rpt_ctr, 0x7, 0x1, 0x0, 0x0);//tccd=16tck => 8 mc clk_cycles i.e. 7 nops. repeat =14 => 15 BL32 cmd
    }

    if (wr_rd)
    {
        HWIO_OUTXFI (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_ALU_n_CTRL, 0, RD_WR, 0x1);
        // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr) 
		MC_PXI_Payload_Config (ch, PXI_DQOP, 0, 0x0, 0x1a, 0x1, 0x0, 0x0); //Dummy. 
        if (cs)
        {
            MC_PXI_Payload_Config (ch, PXI_DQOP, 1, rpt_ctr, 0x7, 0x1, 0x4, 0x0); //15 BL32 read opcodes. tccd apart
        }
        else
        {
            MC_PXI_Payload_Config (ch, PXI_DQOP, 1, rpt_ctr, 0x7, 0x1, 0x2, 0x0); //15 BL32 read opcodes. tccd apart
        }
    }
    else
    {
        //write
        HWIO_OUTXFI (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_ALU_n_CTRL, 0, RD_WR, 0x0);
	  
        // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr) 
        if (curr_clk_khz > 1600000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x1d , 0x1, 0x0, 0x0);//NOP
        else if (curr_clk_khz > 1333000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x1c , 0x1, 0x0, 0x0);//NOP
        else if (curr_clk_khz > 1066000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x1b , 0x1, 0x0, 0x0);//NOP
        else if (curr_clk_khz > 800000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x1a , 0x1, 0x0, 0x0);//NOP
        else if (curr_clk_khz > 533000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x19 , 0x1, 0x0, 0x0);//NOP
        else if (curr_clk_khz > 266000)
            MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x18 , 0x1, 0x0, 0x0);//NOP
        else
			MC_PXI_Payload_Config (ch, PXI_DQWD, 0, 0x0, 0x17 , 0x1, 0x0, 0x0);//NOP
        
        // (uint8 num, uint8 rpt_ctr, uint8 nop_ctr, uint8 task_ctr, uint8 task_ptr, uint8 error_clr) 
		MC_PXI_Payload_Config (ch, PXI_DQOP, 0, 0x0, 0x19, 0x1, 0x0, 0x0); //Dummy. 
        if (cs)
        {
            MC_PXI_Payload_Config (ch, PXI_DQOP, 1, rpt_ctr, 0x7, 0x1, 0x3, 0x0); //15 BL32 Write opcodes. tccd apart
        }
        else
        {
            MC_PXI_Payload_Config (ch, PXI_DQOP, 1, rpt_ctr, 0x7, 0x1, 0x1, 0x0); //15 BL32 Write opcodes. tccd apart

        }
    }

    // (uint8 num, uint8 cap0, uint8 cap1, uint8 csp0, uint8 csp1 )
    MC_PXI_CA_Data_Config (ch, PXI_CAWD, 0, 0x1 , 0x0, csp0, 0x0);// ACTIVATE 
    MC_PXI_CA_Data_Config (ch, PXI_CAWD, 1, 0x3 , 0x0, csp0, 0x0);// ACTIVATE
    MC_PXI_CA_Data_Config (ch, PXI_CAWD, 2, cap0, 0x0 , csp0, 0x0);// WRITE-1 onthe fly BL32
    MC_PXI_CA_Data_Config (ch, PXI_CAWD, 3, 0x12, 0x0, csp0, 0x0);// WRITE CAS-2

    if (napali_v1)
    {
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 4, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 5, 0x12, 0x8, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 6, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 7, 0x12, 0x10, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 8, cap0, 0x0 , csp0, 0x0);// READ-1   onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 9, 0x12, 0x18, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 10, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 11, 0x12, 0x20, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 12, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 13, 0x12, 0x28, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 14, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 15, 0x12, 0x30, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 16, cap0, 0x0 , csp0, 0x0);// READ-1   onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 17, 0x12, 0x38, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 18, cap0, 0x0 , csp0, 0x0);// READ-1 onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 19, 0x32, 0x0, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 20, cap0, 0x0 , csp0, 0x0);// READ-1 onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 21, 0x32, 0x8, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 22, cap0, 0x0 , csp0, 0x0);// READ-1 onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 23, 0x32, 0x10, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 24, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 25, 0x32, 0x18, csp0, 0x0);// READ CAS-2  
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 26, cap0, 0x0 , csp0, 0x0);// READ-1  onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 27, 0x32, 0x20, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 28, cap0, 0x0 , csp0, 0x0);// READ-1 onthe fly BL32
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 29, 0x32, 0x28, csp0, 0x0);// READ CAS-2
        MC_PXI_CA_Data_Config (ch, PXI_CAWD, 30, cap0, 0x0 , csp0, 0x0);//  READ-1  onthe fly BL32
		MC_PXI_CA_Data_Config (ch, PXI_CAWD, 31, 0x32, 0x30 , csp0, 0x0);// READ CAS-2
	}
}


