

 /****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================*/
#include "HAL_SNS_DDR.h"
#include "shrm_csr_seq_hwiobase.h"
#include "ddr_ss_seq_hwiobase.h"
#include "shrm_csr_seq_hwioreg.h"
#include "ddrss.h"

//TODO: Remove this Macro once QDSS-SHRM HWIO header file is generated 
#define HWIO_QDSS_DDRSS_DDR_SHRM_PWRCTRL_ADDR  (0x69EB020)
#define GCC_DDR_I_HCLK_CBCR_ADDR     0x001442A4 
#define GCC_DNOC_CFG_CBCR_ADDR       0x001442A0  
#define GCC_DDRSS_CFG_AHB_CBCR_ADDR  0x00144020   
#define DPC_REG_DPCC_SHRM_CBCR_ADDR  0x014480D8
//CLK_ARES is bit 2
#define CLK_ARES_SET 0x04
#define CLK_ARES_CLEAR 0xFFFFFFFB

#define ASSERT_CORE_RESET  1<<16
#define DE_ASSERT_CORE_RESET  0x0

//================================================================================================//
// usage: 
// 1. call SHRM_rpmh_port_override_control(uint8 en) with en = 1
// 2. call SHRM_rsi_cmd_inject with task(cmd_payload), like ddr_clk_switch.
//    work function call be called multiple times here
// 3. call SHRM_rpmh_port_override_control(uint8 en) with en = 0
//================================================================================================//
void SHRM_rpmh_port_override_control(uint8 en)
{
	// change Override RPMh option in MUX sel to CSR, SHRM_RSI_PATH[SEL] PoR value = 0(enable)
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RSI_PATH, SEL, !en);

	// Override RPMh in MUX sel
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN, EN, en);
}

void SHRM_rsi_cmd_inject(uint32 cmd_payload)
{
	// specify payload before issuing trigger cmd
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD, CMD_PAYLOAD, cmd_payload);

	// issuing cmd trigger
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER, TRIGGER, 1);
	
	// Poll SHRM_CSR_SHRM_SPROC_STATUS to ensure task completed and processor in WAITI state
	while (HWIO_INXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_SPROC_STATUS, STATE) != 1);
	
	// clearing cmd trigger
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER, TRIGGER, 0);
}

//================================================================================================//
// usage: 
// After loading imem/dmem image to SHRM processor
// 1. release processor from reset state and fetch instruction
// 2. enable CGC
//================================================================================================//
//void SHRM_release_proc_out_of_reset() // TODO: remove after using function SHRM_proc_run_ctrl
//{
//	//Put SHRM into RUN state
//    HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_SPROC_CTRL, RUN_STALL, 0);
//	
//	//Disable GLOBAL_CGC override
//    HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL, GLOBAL_CGC, 0);
//}

//================================================================================================//
// usage: 
// when run_en = RUN (0)
// 1. release processor from stall mode
// 2. enable CGC
// when run_en = STALL (1), do the reverse
//================================================================================================//
void SHRM_proc_run_ctrl(BOOT_DDR_STRUCT *ddr, SHRM_RUN_CTRL_OPCODE run_en)
{
		//Put SHRM into RUN state
		HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_SPROC_CTRL, RUN_STALL, run_en);
		
#if TARGET_DSF_DV_SIMULATION
        // Required only in DV simulation environment
	    // Poll SHRM_CSR_SHRM_SPROC_STATUS to ensure task completed and processor in WAITI state
	    while (HWIO_INXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_SPROC_STATUS, STATE) != 1);
#endif
		if (! ((ddr->shrm_ddr_ptr->misc.chip_version == 0x0100) && (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI))) //nagarwal
		{
		//Disable GLOBAL_CGC override, this step is not applicable in Napali V1 because of HW bug, always enable clk as free running clk in V1
		HWIO_OUTX (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL, run_en);
		}

}

//================================================================================================//
// usage: 
// Reset the SHRM core via QDSS_DDRSS_DDR_SHRM_PWRCTRL 
//================================================================================================//

void SHRM_core_reset()
{
    uint32 i = 0 , delay = 20;

    //Asserting SHRM core reset via APB_PWRCTRL CSR
    out_dword(HWIO_QDSS_DDRSS_DDR_SHRM_PWRCTRL_ADDR, ASSERT_CORE_RESET);

    /*CORE_RESET bit remains high for at least 10 processor clock
     *cycles in order for reset to happen correctly
     *So introducing  delay of 20 clock cycles before de-asserting
     *CORE_RESET
     */
    for(i = 0 ; i < delay ; i++);
    
    //De-asserting SHRM core reset via APB_PWRCTRL CSR
    out_dword(HWIO_QDSS_DDRSS_DDR_SHRM_PWRCTRL_ADDR, DE_ASSERT_CORE_RESET);

   
}


void SHRM_CBC_reset()
{
    uint32 gcc_ddr_i_hclk_cbcr = 0;
    uint32 gcc_dnoc_cfg_cbcr = 0;
    uint32 gcc_ddrss_cfg_ahb_cbcr = 0;
    uint32 dpc_reg_dpcc_shrm_cbcr = 0;
    uint32 delay = 0;


    gcc_ddr_i_hclk_cbcr = in_dword (GCC_DDR_I_HCLK_CBCR_ADDR);
    dpc_reg_dpcc_shrm_cbcr = in_dword (DPC_REG_DPCC_SHRM_CBCR_ADDR);
    gcc_dnoc_cfg_cbcr = in_dword (GCC_DNOC_CFG_CBCR_ADDR);
    gcc_ddrss_cfg_ahb_cbcr = in_dword (GCC_DDRSS_CFG_AHB_CBCR_ADDR);

    gcc_ddr_i_hclk_cbcr = gcc_ddr_i_hclk_cbcr | CLK_ARES_SET;
    dpc_reg_dpcc_shrm_cbcr = dpc_reg_dpcc_shrm_cbcr | CLK_ARES_SET;
    gcc_dnoc_cfg_cbcr = gcc_dnoc_cfg_cbcr | CLK_ARES_SET;
    gcc_ddrss_cfg_ahb_cbcr = gcc_ddrss_cfg_ahb_cbcr | CLK_ARES_SET;


    //Assert CBC Reset
    out_dword (GCC_DDR_I_HCLK_CBCR_ADDR, gcc_ddr_i_hclk_cbcr );
    out_dword (DPC_REG_DPCC_SHRM_CBCR_ADDR, dpc_reg_dpcc_shrm_cbcr);
    out_dword (GCC_DNOC_CFG_CBCR_ADDR, gcc_dnoc_cfg_cbcr );
    out_dword (GCC_DDRSS_CFG_AHB_CBCR_ADDR, gcc_ddrss_cfg_ahb_cbcr);

    gcc_ddr_i_hclk_cbcr = gcc_ddr_i_hclk_cbcr & CLK_ARES_CLEAR;
    dpc_reg_dpcc_shrm_cbcr = dpc_reg_dpcc_shrm_cbcr & CLK_ARES_CLEAR;
    gcc_dnoc_cfg_cbcr = gcc_dnoc_cfg_cbcr & CLK_ARES_CLEAR;
    gcc_ddrss_cfg_ahb_cbcr = gcc_ddrss_cfg_ahb_cbcr & CLK_ARES_CLEAR;

     for (delay = 0 ; delay < 50 ; delay++);

    //Assert CBC clear 
    out_dword (GCC_DDRSS_CFG_AHB_CBCR_ADDR, gcc_ddrss_cfg_ahb_cbcr);
    out_dword (GCC_DNOC_CFG_CBCR_ADDR, gcc_dnoc_cfg_cbcr );
    out_dword (DPC_REG_DPCC_SHRM_CBCR_ADDR, dpc_reg_dpcc_shrm_cbcr);
    out_dword (GCC_DDR_I_HCLK_CBCR_ADDR, gcc_ddr_i_hclk_cbcr );


     for (delay = 0 ; delay < 50 ; delay++);

}


void SHRM_init(BOOT_DDR_STRUCT *ddr)
{
    // Load configuration including QoS
    DDRSS_set_config(ddr, SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[SHRM_CFG_IDX][0]));

    // Issuing QoS load trigger
    HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_QOS_CTRL, LOAD_PARAMETERS, 1);
}
