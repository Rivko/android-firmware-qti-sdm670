/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss.c#8 $
$DateTime: 2019/08/01 06:25:20 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/04/14   arindamm     First edit history header. Add new entries at top.
================================================================================*/

#include "ddrss.h"
#include "ddrss_mccc.h"
#include "mc.h"
#include "mc_common.h"
#include "ddr_ss_seq_hwioreg.h"	
#include "llcc.h"
#include "target_config.h"
#include "shrm_load_ram.h"
#include "mem_noc_seq_hwiobase.h"
#include "mem_noc_seq_hwioreg.h"
#include "ddr_shrm_periodic_training.h"

/* Define a neat way to trigger a compile time error based on passed
 * expression evaluating being true or false */
#define COMPILE_TIME_ASSERTION(exp) ((void)sizeof(char[1 - 2*!!((exp)-1)]))

// Create a while loop to get stuck in without Klocwork complaints
#define INVALID_PLATFORM_ID while (1) { ; }

SHRM_PERIODIC_TRAINING_DATA_STRUCT *shrm_periodic_training_data_ptr = (SHRM_PERIODIC_TRAINING_DATA_STRUCT *) (0xC00 + SEQ_DDR_SS_SHRM_MEM_SHRM_OFFSET);

uint16 DDRSS_Find_Clk_Index(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz)
{
   uint16 clk_idx = 0;

    for (clk_idx = 0; clk_idx < MAX_NUM_CLOCK_PLAN; clk_idx++)
    {
	   if ( clk_freq_khz <= ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[clk_idx].clk_freq_in_khz )
           break;
    }

   // Make sure we don't return an index past the end of the table
   return MIN(clk_idx, (MAX_NUM_CLOCK_PLAN-1));
}

//================================================================================================//
// One-time settings function.
//================================================================================================//
void DDRSS_set_config
(
    BOOT_DDR_STRUCT *ddr,
    uint32 offset,
    uint32 (*config)[2]
)
{
    uint16 reg = 0;
    
    for (reg = 0; config[reg][0] != 0; reg++)
    {
        out_dword(config[reg][0] + offset, config[reg][1]);
    }
}

//================================================================================================//
// Save/restore register values function.
//================================================================================================//
void DDRSS_Register_Save_Restore
(
    SAVE_RESTORE save_restore,
    uint16 stack_size,
    uint32 offset,
    uint32 *address_list,
    uint32 *value_stack
)
{
    uint16 index = 0;

    // Save values
    for (index = 0; address_list[index] != 0; index++)
    {
        if(index >= stack_size)
        {
            // Register list exceeded size of stack for storing values
            ddr_printf(DDR_ERROR, "ERROR: Index exceeds stack size!\n");
            ddr_abort();
            return;
        }
        
        if(save_restore == SAVE)
        {
            value_stack[index] = in_dword(address_list[index] + offset);
        }
        else
        {
            out_dword(address_list[index] + offset, value_stack[index]);
        }
    }
}

//================================================================================================//
// DDR one-time settings function. 
//================================================================================================//
void DDRSS_Config(BOOT_DDR_STRUCT *ddr)
{
    DDRSS_set_config(ddr, 0,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[DDRSS_CFG_IDX][0]));
}

//================================================================================================//
// MEMNOC one-time settings function. 
//================================================================================================//
void MEMNOC_Config(BOOT_DDR_STRUCT *ddr)
{
    DDRSS_set_config(ddr, SEQ_DDR_SS_MEMNOC_MEM_NOC_HM_OFFSET,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[MEMNOC_CFG_IDX][0]));
}

//================================================================================================//
// Loads DCB to buffer
// Copies DDR runtime struct from DCB to SHRM_DDR_STRUCT
// Copies AOP parameters struct from DCB to BOOT_DDR_STRUCT
// Copies training save/restore register address lists from DCB to BOOT_DDR_STRUCT
//================================================================================================//
#define DCB_NAME_BUF_LEN_MAX 22
void DDRSS_DCB_Init(BOOT_DDR_STRUCT *ddr)
{
    uintnt dcb_handle = 0;
    uint32 dcb_size = 0;
    char dcb_name[DCB_NAME_BUF_LEN_MAX] = {0};

    // Generate DCB file name
    ddr_external_snprintf(dcb_name, DCB_NAME_BUF_LEN_MAX, "/%04X_%04X_%01X_dcb.bin",
        ddr->shrm_ddr_ptr->misc.platform_id,
        (ddr->shrm_ddr_ptr->misc.chip_version & 0xFF00), // Ignore minor chip version when selecting DCB binary
        ddr->shrm_ddr_ptr->misc.target_silicon);

    // Obtain handle to DCB in XBL config
    ddr_external_dsf_config_open(dcb_name, &dcb_handle, &dcb_size);
    if(dcb_size != DCB_MAX_SIZE)
    {
        ddr_printf(DDR_ERROR, "ERROR: DCB size does not match maximum!\n");
        ddr_abort();
    }

    // Obtain buffer for DCB
    ddr_external_get_dsf_config_buffer((void**)&(ddr->dcb_ptr), dcb_size);
    if(ddr->dcb_ptr == NULL)
    {
        ddr_printf(DDR_ERROR, "ERROR: Unable to obtain DCB buffer!\n");
        ddr_abort();
    }

    // Read DCB to buffer
    ddr_external_dsf_config_read(
        dcb_handle,
        0,
        dcb_size,
        ddr->dcb_ptr);

    // Close DCB in XBL config
    ddr_external_dsf_config_close(dcb_handle);
    
    // Check DCB was generated with this DSF version
    if(((uint32*)ddr->dcb_ptr)[DSF_VERSION_IDX] != TARGET_DDR_SYSTEM_FIRMWARE_VERSION)
    {
        ddr_printf(DDR_ERROR, "ERROR: DCB version does not match DSF version!\n");
        ddr_abort();
    }
    
    // Print DCB CRC
    ddr_printf(DDR_NORMAL, "DCB CRC = 0x%X\n", ((uint32*)ddr->dcb_ptr)[DCB_CRC_IDX]);

    // Store DCB CRC in SHRM DDR struct
    ddr->shrm_ddr_ptr->misc.dcb_crc = ((uint32*)ddr->dcb_ptr)[DCB_CRC_IDX];
    
    // Copy DDR runtime struct to SHRM DDR struct
    ddr_external_memscpy(&(ddr->shrm_ddr_ptr->ddr_runtime),
        sizeof(ddr->shrm_ddr_ptr->ddr_runtime),
        (void*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[DDR_RUNTIME_IDX][0]),
        (*ddr->dcb_ptr)[DDR_RUNTIME_IDX][1]);
        
    // Copy AOP parameters runtime struct to boot DDR struct
    ddr_external_memscpy(&(ddr->aop_params),
        sizeof(ddr->aop_params),
        (void*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[AOP_PARAMS_IDX][0]),
        (*ddr->dcb_ptr)[AOP_PARAMS_IDX][1]);
    ddr_external_memscpy(&(ddr->training_params),
        sizeof(ddr->training_params),
        (void*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[TRAINING_PARAMS_IDX][0]),
        (*ddr->dcb_ptr)[TRAINING_PARAMS_IDX][1]);
    
    // Copy register address lists for saving and restoring training results to boot DDR struct
    ddr_external_memscpy(&(ddr->dq_phy_regs),
        sizeof(ddr->dq_phy_regs),
        (void*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[DQ_PHY_REGS_IDX][0]),
        (*ddr->dcb_ptr)[DQ_PHY_REGS_IDX][1]);
    ddr_external_memscpy(&(ddr->ca_phy_regs),
        sizeof(ddr->ca_phy_regs),
        (void*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[CA_PHY_REGS_IDX][0]),
        (*ddr->dcb_ptr)[CA_PHY_REGS_IDX][1]);
}

//================================================================================================//
// DDR Initialization
//================================================================================================//
boolean HAL_DDR_Init (BOOT_DDR_STRUCT *ddr,
                      DDR_CHANNEL channel, 
                      DDR_CHIPSELECT chip_select,
                      uint32 clk_freq_khz)
{
    // Print DSF version
    ddr_printf(DDR_NORMAL, "DSF V%u.%u\n", TARGET_DDR_SYSTEM_FIRMWARE_MAJOR_VERSION, TARGET_DDR_SYSTEM_FIRMWARE_MINOR_VERSION);
    
    // Do initialial actions with DCB
    DDRSS_DCB_Init(ddr);
    
    // Trigger compile time check to ensure DDR_STRUCT size does not exceed
    // SW allocation of same structure in limited OCIMEM space of ~1.2KBytes 
    //COMPILE_TIME_ASSERTION(sizeof(BOOT_DDR_STRUCT) < 7354);
    // Any other size limits to abide by???
    // COMPILE_TIME_ASSERTION(sizeof(ddr->cdt_params) < 512 /*Total cdt and eCDT = 1024*/ /*min cdt: 489*/);
    // COMPILE_TIME_ASSERTION(sizeof(ddr->ddr_runtime) < 512 /*Total cdt and eCDT = 1024*/ /*min eCDT: 385*/);
    // COMPILE_TIME_ASSERTION(sizeof(ddr->flash_params.training_data) < 5001 );

    // Trigger compile time check to ensure local_vars size does not exceed 64KB. 
    // COMPILE_TIME_ASSERTION(sizeof(ddrss_rdwr_dqdqs_local_vars) < 65536);
    // COMPILE_TIME_ASSERTION(sizeof(ddrss_ca_vref_local_vars) < 65536);

    // Fill in the current version numbers for the DDR System Firmware, the DDRSS Core and the various sub-cores..
    ddr->shrm_ddr_ptr->boot_dsf_version = TARGET_DDR_SYSTEM_FIRMWARE_VERSION;
    ddr->shrm_ddr_ptr->mc.version.arch = TARGET_MC_ARCH_VERSION;
    ddr->shrm_ddr_ptr->mc.version.major = TARGET_MC_CORE_MAJOR_VERSION;
    ddr->shrm_ddr_ptr->mc.version.minor = TARGET_MC_CORE_MINOR_VERSION;
    ddr->shrm_ddr_ptr->phy.version.major = TARGET_PHY_CORE_MAJOR_VERSION;
    ddr->shrm_ddr_ptr->phy.version.minor = TARGET_PHY_CORE_MINOR_VERSION;
    ddr->shrm_ddr_ptr->llcc.version.major = TARGET_LLCC_CORE_MAJOR_VERSION;
    ddr->shrm_ddr_ptr->llcc.version.minor = TARGET_LLCC_CORE_MAJOR_VERSION;
    
    switch(ddr->shrm_ddr_ptr->misc.platform_id)
    {
        case MSM_VERSION_NAPALI:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x4;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3F;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xFFF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0xFFFFFFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFFFFFF;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x00FFFFFF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x6;
        break;
        
        case MSM_VERSION_WARLOCK:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x2;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0x0000FFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFF0000;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x000000FF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x2;
        break;
       
		case MSM_VERSION_REDDRAGON:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x2;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0x0000FFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFF0000;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x000000FF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x2;
        break;

		case MSM_VERSION_REDDRAGONLC:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x2;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0x0000FFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFF0000;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x000000FF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x2;
        break;

		case MSM_VERSION_KINGSMAN:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x2;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0x0000FFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFF0000;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x000000FF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x2;
        break;

		case MSM_VERSION_KINGSMANLC:
            ddr->shrm_ddr_ptr->misc.num_channel = 0x2;
            ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_pcb_mask = 0x3;
            ddr->shrm_ddr_ptr->misc.all_way_mask = 0xF;
            ddr->shrm_ddr_ptr->misc.ram_status0_mask = 0x0000FFFF;
            ddr->shrm_ddr_ptr->misc.ram_status1_mask = 0x0FFF0000;
            ddr->shrm_ddr_ptr->misc.pcb_status_mask  = 0x000000FF;
            ddr->shrm_ddr_ptr->misc.num_pcb = 0x2;
        break;
		
        default:
            INVALID_PLATFORM_ID;
        break;
    }
    
    // Don't try to initialize more channels than we have
    channel &= ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask;

    // Bus width is always 16 on SDM845
    ddr->shrm_ddr_ptr->misc.bus_width_pch = 16;

    // Store our initial DDR frequency
    ddr->shrm_ddr_ptr->misc.current_clk_in_kHz = clk_freq_khz;
    ddr->shrm_ddr_ptr->misc.new_clk_in_kHz = clk_freq_khz;
    
    // Flag that we have not yet initialized or trained DDR
    ddr->shrm_ddr_ptr->misc.boot_init_completed = 0;
    ddr->shrm_ddr_ptr->misc.boot_training_status = TRAINING_NOT_STARTED;
    
    // Initialize periodic events flag to all disabled
    ddr->shrm_ddr_ptr->periodic_events_enable_flag = 0;

    // Static DDRSS configuration
    DDRSS_Config(ddr);

    // Static MEMNOC configuration
    MEMNOC_Config(ddr);

    // Static MCCC configuration
    MCCC_Config(ddr, channel);
    
    //Enable PHY XO Clock
    DDRSS_PHY_XO_Clk_Ctrl(1);
    // Static DDR PHY & CC configuration
    DDR_PHY_CC_Config(ddr);

    //Set PHY_XO_CLK gating according to one time settings
    DDRSS_PHY_XO_Clk_Ctrl(ddr->shrm_ddr_ptr->ddr_runtime.phy_xoclk);
    // Static MC configuration
    MC_Config(ddr, channel);

    // Memory device is not initialized so DRAM information is not known
    DDRSS_eCDT_Decode_Entry(ddr, channel,
        (uint16*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[ECDT_PRE_DEVICE_INIT_IDX][0]));

    //Initalizing populated_chipselcet to DDR_CS_BOTH for SHRM clockswith go thru
    ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect = DDR_CS_BOTH;

    //Load SHRM image
    SHRM_Full_Load_SRAM(); 

    //Run SHRM processor
    SHRM_proc_run_ctrl(ddr, RUN);
 
    // MC pre-init 1 (before frequency switch)
    MC_Pre_Init_Setup_1(ddr, channel);

    // Disable Downtime Request before boot time frequency switch
    // Downtime Request could be enabled after boot training for normal frequency switch
    DDRSS_Downtime_Request_Ctrl(0);

    // Request frequency switch from SHRM
    /* CLOCK Switch prework for 200MHz witch */
    HAL_DDR_Pre_Clock_Switch(ddr, channel, clk_freq_khz , clk_freq_khz);

    /* CLock Switch Work for 200MHz */
    HAL_DDR_Clock_Switch(ddr, channel, clk_freq_khz , clk_freq_khz);

    /* Clock witch Post Work */
    HAL_DDR_Post_Clock_Switch(ddr, channel, clk_freq_khz , clk_freq_khz);

    // Do second phase of MC init
    MC_Pre_Init_Setup_2(ddr, channel);

    // Unfreeze DDR IO
    DDRSS_Freeze_IO_Ctrl(0);

    // Deassert DDR reset
    HWIO_OUTXF (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_RESET_CMD, RESET_N, 1);

    // Initialize the memory controller and DDR device
    MC_Memory_Device_Init(ddr, channel);

    // Detect DDR Topology for all channels
    // ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect assignment
    // is handled by MC_DDR_Topology_Detection() internally
    MC_DDR_Topology_Detection(ddr, channel, chip_select);

    // Do DDR address & post-init setup for all channels
    MC_Post_Init_Setup(ddr, channel);

    //  ADD IF unction for V1 to keep the clock alway ON       
    if((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version == 0x100)) {
     ddr->shrm_ddr_ptr->ddr_runtime.phy_xoclk = 1;
     }
    //Enable PHY XO Clock
    DDRSS_PHY_XO_Clk_Ctrl(1);
    //PHY Post Initalization
    DDR_PHY_CC_init (ddr, channel);

    //Set PHY_XO_CLK gating according to one time settings
    DDRSS_PHY_XO_Clk_Ctrl(ddr->shrm_ddr_ptr->ddr_runtime.phy_xoclk);
    // Memory device is initialized and DRAM information is known
    DDRSS_eCDT_Decode_Entry(ddr, channel,
        (uint16*)((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[ECDT_POST_DEVICE_INIT_IDX][0]));
#if 0 // Not integrated with DDI, so it has no value
    // Read values of parameters which can be overriden by eCDT
    DDRSS_eCDT_Read_Parameter_Values(ddr);
#endif

    // Initialize LLCC for all channels
    LLCC_init(ddr, channel);

    // Initialize SHRM QOS for all channels
    SHRM_init(ddr);

	// Enable Downtime Request for normal frequency switch
#if 0
    DDRSS_Downtime_Request_Ctrl(1);
#endif
    
	return TRUE;
}

//================================================================================================//
// DDR Software Self Refresh Enter
//================================================================================================//
boolean HAL_DDR_Enter_Self_Refresh (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   MC_Enter_Self_Refresh(ddr->shrm_ddr_ptr, channel, chip_select);
   
   return TRUE;
}

//================================================================================================//
// DDR Software Self Refresh Exit
//================================================================================================//
boolean HAL_DDR_Exit_Self_Refresh (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   MC_Exit_Self_Refresh(ddr->shrm_ddr_ptr, channel, chip_select);
   
   return TRUE;
}

//================================================================================================//
// DDR Device Mode Register Read
//================================================================================================//
uint32 HAL_DDR_Read_Mode_Register (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select,
                                   uint32 MR_addr)
{
   return MC_MR_Read (channel, chip_select, MR_addr);
}

//================================================================================================//
// DDR Device Mode Register Write
//================================================================================================//
boolean HAL_DDR_Write_Mode_Register (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select,
                                     uint32 MR_addr, uint32 MR_data)
{
   MC_MR_Write (ddr, channel, chip_select, MR_addr, MR_data);

   return TRUE;
}

//================================================================================================//
// ZQ Calibration
//================================================================================================//
boolean HAL_DDR_ZQ_Calibration (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   MC_ZQ_Calibration (ddr, channel, chip_select);

   return TRUE;
}

//================================================================================================//
// Enter Power Collapse
//================================================================================================//
boolean HAL_DDR_Enter_Power_Collapse(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_POWER_CLPS_MODE collapse_mode, 
                                     uint32 clk_freq_khz)
{
    // Place the MC into power collapse
	// MC_Enter_Power_Collapse(ddr->shrm_ddr_ptr, channel);
	
	// What else needs to be collapsed?  Phy, LLCC, MCCC, etc?
	
	return TRUE;
}

//================================================================================================//
// Exit Power Collapse
//================================================================================================//
boolean HAL_DDR_Exit_Power_Collapse(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_POWER_CLPS_MODE collapse_mode, 
                                    uint32 clk_freq_khz)
{
	// Take the MC out of  power collapse
	// MC_Exit_Power_Collapse(ddr->shrm_ddr_ptr, channel);
	
	// What else needs to be restored?  Phy, LLCC, MCCC, etc?
	
	return TRUE;
}

//============================================================================
// HAL_DDR_Enter_Deep_Power_Down
//============================================================================
boolean HAL_DDR_Enter_Deep_Power_Down(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
  DDR_CHIPSELECT chip_select)
{
    return TRUE;
}

//============================================================================
// HAL_DDR_Exit_Deep_Power_Down
//============================================================================
boolean HAL_DDR_Exit_Deep_Power_Down(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
  DDR_CHIPSELECT chip_select, uint32 clkspeed)
{
    return TRUE;
}

//============================================================================
// HAL_DDR_IOCTL
//============================================================================
boolean HAL_DDR_IOCTL (BOOT_DDR_STRUCT *ddr, IOCTL_CMD ioctl_cmd, IOCTL_ARG *ioctl_arg)
{
    boolean return_value;
    uint32 bus_width_bits = 0;
    uint32 interleave_bits = 0;
    uint32 column_address_bits = 0;
    uint32 bank_address_bits = 0;
    uint32 i;

    switch (ioctl_cmd)
    {
        case IOCTL_CMD_GET_CORE_REGISTERS_FOR_CRASH_DEBUG:
            ioctl_arg->result_code = IOCTL_RESULT_COMMAND_NOT_IMPLEMENTED;
            return_value = FALSE;
            break;

        /* For the IOCTL_CMD_GET_HIGHEST_BANK_BITcommand, ioctl_arg->results
           is where this API will write the highest DRAM bank bit. */
        case IOCTL_CMD_GET_HIGHEST_BANK_BIT:
            // Calculate highest DRAM bank bit in the 32-bit system address here.
            // Starting with LSB traveling towards MSB in the system address, we have
            //
            // Byte Address: 1b for x16 device, 2b for x32 device.
            // Interleave:  log2(NUM_CH) bits (ie, 1 bit for 2 channels, 2 bits for 4 channels, etc.  Interleaving
            //    always enabled, the only variable is how many channels are interleaved.
            // Column address bits:  From detected_ddr_device[]
            // Bank address bits:  log2(bank_bits from detected_ddr_device[])
            // Row address bits (which are above the highest bank bit):  From detected_ddr_device[]

            // This code assumes total symmetry of geometry across all dies, across all channels.
            
            bus_width_bits = ddr->shrm_ddr_ptr->misc.bus_width_pch/16;
            column_address_bits = ddr->shrm_ddr_ptr->detected_ddr_device[0].num_cols[0];
            
            i = NUM_CH;
            interleave_bits = 0;
            while ((i & 1) != 1)
            {
                i >>= 1;
                interleave_bits++;
            }
            
            i = ddr->shrm_ddr_ptr->detected_ddr_device[0].num_banks[0];
            bank_address_bits = 0;
            while ((i & 1) != 1)
            {
                i >>= 1;
                bank_address_bits++;
            }
            
            ioctl_arg->results = bus_width_bits + column_address_bits + interleave_bits + bank_address_bits - 1;

            // No need to update ioctl_arg->result_code, since we are returning return_value as TRUE, caller software
            // should ignore result_code.
            return_value = TRUE;
            break;

        case IOCTL_CMD_TEMP_CTRL_MIN_FREQ:
            ioctl_arg->results = IOCTL_RESULT_COMMAND_NOT_IMPLEMENTED;

            return_value = FALSE;
            break;

        /* Bad command, return appropriate result code. */
        default:
            ioctl_arg->result_code = IOCTL_RESULT_ILLEGAL_COMMAND;
            return_value = FALSE;
    }

    return return_value;
}


//================================================================================================//
// DDR Post Boot Training Setup.
//================================================================================================//
boolean DDRSS_Post_Boot_Training(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8 ch = 0;
   uint32 reg_offset = 0;
   uint8 byte_lane = 0;
   //uint32 interrupt_enable = 0;

   // Enable Downtime Request after boot training for normal frequency switch
   DDRSS_Downtime_Request_Ctrl(1);	
   
  /* Needed for V2.0.1 */ 
   if (((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version > 0x0200)) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_WARLOCK)|| (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGON) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_REDDRAGONLC) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMAN) || (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_KINGSMANLC) )
   {
       ch = LLCC_CH_INX(channel);
       reg_offset = REG_OFFSET_LLCC_FEAC(ch);
       HWIO_OUTX (reg_offset, LLCC_FEAC_RD_GFO_OVERRIDE_CFG0, 0x0);
       HWIO_OUTX (reg_offset, LLCC_FEAC_WR_GFO_OVERRIDE_CFG0, 0x0);
   }

   if (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && ((ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))))
   {
      /* After Training/Restore Enable Clock Gating for DDRSS */	
      HAL_DDR_Clock_Gating_Ctrl(ddr,1); 
   }

#if DSF_PERIODIC_TRAINING_EN	
   HAL_DDR_Periodic_Training(ddr, channel, chip_select);
#endif
   
#if 0 
   if (((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version >= 0x0200)))
   {
      /* Enable error interrupts */
      ch = MC_CH_INX(channel);
      reg_offset = REG_OFFSET_MC_INTERRUPT(ch);
      interrupt_enable = HWIO_INXI (reg_offset, INTERRUPT_INTERRUPT_ENABLE_n, 0);
      interrupt_enable |= HWIO_INTERRUPT_INTERRUPT_ENABLE_n_ADDR_DECERR_IRQ_EN_BMSK;
//      interrupt_enable |= HWIO_INTERRUPT_INTERRUPT_ENABLE_n_MEM_BUSHANG_ERR_IRQ_EN_BMSK;
//      interrupt_enable |= HWIO_INTERRUPT_INTERRUPT_ENABLE_n_RDQS_ERR_IRQ_EN_BMSK;
//      interrupt_enable |= HWIO_INTERRUPT_INTERRUPT_ENABLE_n_REFRESH_ERR_IRQ_EN_BMSK;
      HWIO_OUTXI (reg_offset, INTERRUPT_INTERRUPT_ENABLE_n, 0, interrupt_enable);
   }
#endif
   
   if (ddr->shrm_ddr_ptr->ddr_runtime.phy_extended_logging_enable)
   {
      for(ch = 0; ch < ddr->shrm_ddr_ptr->misc.num_channel; ch++)
      {
         reg_offset = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
         
         for(byte_lane = 0; byte_lane < NUM_DQ_PCH; byte_lane++)
         {
            // Enable RD FIFO underflow detection
            HWIO_OUTXF(reg_offset + (byte_lane * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_DM_CFG0, UNDERFLOW_DEBUG_EN, 1);
            
            // Enable RCW beat counter
            HWIO_OUTXF(reg_offset + (byte_lane * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, RCW_DQS_BEAT_CNT_EN, 1);
         }
      }
   }
   
   return TRUE;
}

boolean HAL_DDR_Boot_Training_Init(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{ 
  static   uint8   *local_vars;
  boolean bResult;
  uint32 local_var_size = LOCAL_VARS_AREA_SIZE;

  // Set a flag to indicate training has been started
  ddr->shrm_ddr_ptr->misc.boot_training_status = TRAINING_IN_PROGRESS;

  // Disable Downtime Request before boot training
  DDRSS_Downtime_Request_Ctrl(0);	
   
  bResult = ddr_external_get_buffer((void **)&local_vars, &local_var_size);
      
  if (bResult != TRUE)
  {
       return FALSE;
  }  
  
  // Training params structure pointer
    
  // Set all training parameters to recommended values
	
	// Do the training sequences identified in the training pointer array
  frwk_do_trainings
  (
     ddr,
     pre_training,
     do_training,
     post_training,
     &(ddr->training_params),
     local_vars
  );
  
  // Note which DSF version we trained with.
  ddr->training_results.version = TARGET_DDR_SYSTEM_FIRMWARE_VERSION;
  
  // Set a flag to indicate training has been done
  ddr->shrm_ddr_ptr->misc.boot_training_status = TRAINING_COMPLETED;
	
  return TRUE;
}

boolean HAL_DDR_Boot_Training_Restore(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    DDR_CHIPSELECT qualified_cs = DDR_CS_NONE;
    
    qualified_cs = (DDR_CHIPSELECT) (chip_select & (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect);
    //Enable PHY XO Clock
    DDRSS_PHY_XO_Clk_Ctrl(1);
    
    // Set a flag to indicate training has been started
    ddr->shrm_ddr_ptr->misc.boot_training_status = TRAINING_IN_PROGRESS;
  
    // Restore only if DSF version number matches, which indicates training has already been done 
    // for this version of the DSF at least once before.
    if (ddr->training_results.version == TARGET_DDR_SYSTEM_FIRMWARE_VERSION)
    {
        // Make sure DRAM mode registers are initialized as they were pre-training
        pre_training_mode_reg_writes(ddr, channel, qualified_cs);
        
        // Restore the training data
        DDRSS_Training_Save_Restore(ddr, channel, RESTORE);
        DDRSS_DRAM_Vref_Restore    (ddr, channel, qualified_cs);
        ddr_external_memscpy(ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset,
               sizeof(ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset),
               training_data_ptr->results.rcw.rd_opcode_offset,
               sizeof(training_data_ptr->results.rcw.rd_opcode_offset));
    }
    // If the version number does not match, return FALSE, so that caller of DSF can 
    // call HAL_DDR_Boot_Training() with INIT in order to re-train.
    else
    {
        ddr_printf(DDR_NORMAL, "\nDSF version changed, triggering re-training..\n");
        ddr_printf(DDR_NORMAL, " Previous DSF version : %u\n", ddr->training_results.version);
        ddr_printf(DDR_NORMAL, " Current  DSF version : %u\n\n", TARGET_DDR_SYSTEM_FIRMWARE_VERSION);
        return FALSE;
    }
    
    // Post Boot Training setup.
    DDRSS_Post_Boot_Training(ddr, channel, qualified_cs);
    
    // Set a flag to indicate training has been done
    ddr->shrm_ddr_ptr->misc.boot_training_status = TRAINING_COMPLETED;
    //Set PHY XO clk based on Run Time Flag
    DDRSS_PHY_XO_Clk_Ctrl(ddr->shrm_ddr_ptr->ddr_runtime.phy_xoclk);
    
  return TRUE;
}

boolean DDRSS_Periodic_Training_load_dit_struct_to_shrm(BOOT_DDR_STRUCT *ddr, uint8 ch, DDR_CHIPSELECT qualified_cs)
{
    uint8  cs = 0;
    uint8 freq_idx = 0;
    uint8 slab_ndx = 0;
    uint8 die = 0;
    uint32 clk_freq  = 0;
    uint16 dit_slope[NUM_DIT_COUNT] = {0};
    uint8 device_width = 0;
    uint32 reg_offset_ddr_phy = REG_OFFSET_DDR_PHY_CH(ch);
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    for (cs = 0; cs < 2; cs++)
    {
        if ((qualified_cs>> cs) & 0x1)
        {
            device_width = ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs];
            for (freq_idx = 0; freq_idx < NUM_DIT_FREQS;freq_idx++)
            {
                clk_freq = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[ddr->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels-NUM_DIT_FREQS+freq_idx].clk_freq_in_khz;

                for (die=0; die <= device_width; die++)
                {
                    DDRSS_DIT_Slope(ddr, ch, cs, die, clk_freq, dit_slope);
                    
                    for (slab_ndx = 0;slab_ndx < NUM_SLABS;slab_ndx++)
                    {
                        //write to shrm struct 
                        shrm_periodic_training_data_ptr->osc_slab[ch][freq_idx][cs][die][slab_ndx] = dit_slope[slab_ndx];
                    }
                    shrm_periodic_training_data_ptr->boot_dit.dit_count[ch][freq_idx][cs][die] = training_data_ptr->results.dit.dit_count[ch][freq_idx][cs][die];
                }
            }
        }
    }

    for (cs = 0; cs < 2; cs++)
    {
        if ((qualified_cs >> cs) & 0x1)
        {	
            switch (training_data_ptr->results.dit.dit_trac_prfs_ndx)
            {
                case 6:
		            if (cs)
		            {
		                shrm_periodic_training_data_ptr->boot_cdc_fine[ch][1][0] = HWIO_INXF (reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_6_CTL_CFG, HP_FINE_DELAY_RANK_1);
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][1][1] = HWIO_INXF (reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_6_CTL_CFG, HP_FINE_DELAY_RANK_1);
                    }
                    else
                    {
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][0][0] = HWIO_INXF (reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_6_CTL_CFG, HP_FINE_DELAY_RANK_0);
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][0][1] = HWIO_INXF (reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_6_CTL_CFG, HP_FINE_DELAY_RANK_0);
                    }
                    break;

                case 7:
                default:
                    if (cs)
                    {
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][1][0] = HWIO_INXF (reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_7_CTL_CFG, HP_FINE_DELAY_RANK_1);
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][1][1] = HWIO_INXF (reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_7_CTL_CFG, HP_FINE_DELAY_RANK_1);
                    }
                    else
                    {
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][0][0] = HWIO_INXF (reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_7_CTL_CFG, HP_FINE_DELAY_RANK_0);
                        shrm_periodic_training_data_ptr->boot_cdc_fine[ch][0][1] = HWIO_INXF (reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_CDCEXT_WR_7_CTL_CFG, HP_FINE_DELAY_RANK_0);
                    }
                    break;
            }
        }
    }
   return TRUE;
}



boolean HAL_DDR_Periodic_Training(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
    uint8  ch = 0;
    DDR_CHIPSELECT qualified_cs = DDR_CS_NONE;
    uint16 dtts_wakeup_interval = 0;
    uint16 dit_read_cnt = 0;
    uint8 acq_ndx = 0;
    uint8 trac_ndx = 0;
    uint8 trac_prfs_index = 0;
    uint8 max_num_samples = 4;
    training_data *training_data_ptr;
    uint32 dit_runtime_count = 0;
    uint32 acq_freq     = 0;
    uint32 track_freq   = 0;
    uint32 threshold = 0;
    uint8 MR23_value      = 0;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    trac_ndx    = training_data_ptr->results.dit.dit_trac_ndx;
    track_freq   = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[trac_ndx].clk_freq_in_khz;

    if ((track_freq >= DSF_MIN_PERIODIC_TRAINING_FREQ) && (ddr->shrm_ddr_ptr->ddr_runtime.periodic_training_enable))
    {
        acq_ndx    = training_data_ptr->results.dit.dit_acq_ndx;
        acq_freq   = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[acq_ndx].clk_freq_in_khz;
        trac_prfs_index = DDRSS_Get_Freq_Index (ddr, track_freq);
        training_data_ptr->results.dit.dit_trac_prfs_ndx = trac_prfs_index;

        //calculate SHKE_PDT_TRAC_THRESHOLD_CFG threshold
        threshold = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[trac_ndx].clk_period_in_psec;

        max_num_samples =training_data_ptr->results.dit.dit_loop_count;
        dtts_wakeup_interval  = DIT_CNT_1SEC/ ((max_num_samples+1)* training_data_ptr->results.dit.dit_interval_per_sec) ;
        dit_runtime_count = training_data_ptr->results.dit.dit_runtime_count;
        
        // Decode the runtime count for the DRAM MR23 register
        if (dit_runtime_count < 2048)
        {
            MR23_value = dit_runtime_count / 16;
        }
        else
        {
            if (dit_runtime_count == 2048)
                MR23_value = 0x40;
            else if (dit_runtime_count == 4096)
                MR23_value = 0x80;
            else if (dit_runtime_count == 8192)
                MR23_value = 0xC0;
        }
        
        dit_read_cnt = dit_runtime_count/2 + (acq_freq/50000);

        for (ch = 0; ch < NUM_CH; ch++)
        {
            if ((channel >> ch) & 0x1)
            {
                qualified_cs = (DDR_CHIPSELECT) (chip_select & (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect);
                
                // rewrite MR23 in case of restore
                MC_MR_Write (ddr, CH_1HOT(ch), qualified_cs, JEDEC_MR_23, MR23_value);

                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_TIMER_CTRL, TRAC_INTERVAL, dtts_wakeup_interval);
                //HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_TIMER_CTRL, ACQ_INTERVAL, 0x00000014);  //currently done in one time settings
                HWIO_OUTXF2 (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_THRESHOLD_CFG, TRAC_FREQ_THRESHOLD, TRAC_FREQ_THRESHOLD_EN, threshold, 1);
                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_MRR_TIMER_CTRL, MPC_START_TO_MRR, dit_read_cnt);
				HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, NUM_SAMPLES, max_num_samples);
                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, NUM_SAMPLES, max_num_samples);
                HWIO_OUTX (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_DIT_FILTER_THRESHOLD_CFG, 0xFFFF0040);

                DDRSS_Periodic_Training_load_dit_struct_to_shrm(ddr,ch,qualified_cs);

#if DSF_PERIODIC_TRAINING_ALGO_DEBUG_EN
                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, 0);
                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, 0);
#else
                // SW workaround for single-rank issue for Napali V1
                if (ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI && ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)
                {
                    HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, 0);
                    HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, 0);
                }
                else
                {
                    // Other than Napali V1 or Napali V1 with dual-rank                  
                    HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, qualified_cs);
                    HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, qualified_cs);
                    ddr->shrm_ddr_ptr->periodic_events_enable_flag |= (1<<SHKE_PDT_TRAC_EVENT_CTRL_EN);
                }

                HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_DIT_EVENT_CFG, TRAC_EVENT_EN, 1);
#endif
            }
        }

        //copy data to SHRM data ptr
        shrm_periodic_training_data_ptr->boot_dit.dit_training_freq = training_data_ptr->results.dit.dit_training_freq;
        shrm_periodic_training_data_ptr->boot_dit.dit_runtime_count = training_data_ptr->results.dit.dit_runtime_count;
        shrm_periodic_training_data_ptr->boot_dit.dit_fine_step_limit = training_data_ptr->results.dit.dit_fine_step_limit;
        shrm_periodic_training_data_ptr->boot_dit.dit_interval_per_sec = training_data_ptr->results.dit.dit_interval_per_sec;
        shrm_periodic_training_data_ptr->boot_dit.dit_loop_count = training_data_ptr->results.dit.dit_loop_count;
        shrm_periodic_training_data_ptr->boot_dit.dit_acq_ndx = training_data_ptr->results.dit.dit_acq_ndx;
        shrm_periodic_training_data_ptr->boot_dit.dit_trac_ndx = training_data_ptr->results.dit.dit_trac_ndx;
        shrm_periodic_training_data_ptr->boot_dit.dit_trac_prfs_ndx = training_data_ptr->results.dit.dit_trac_prfs_ndx;

    } // if (track_ndx != 0xFF)
   
    return TRUE;
}

boolean HAL_DDR_Periodic_Training_Ctrl(BOOT_DDR_STRUCT *ddr, uint8 enable)
{
    uint8 ch = BROADCAST_CH_NUM;
    DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
    
    if (ddr->shrm_ddr_ptr->ddr_runtime.periodic_training_enable)
    {
        if (enable)
        {
            HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, qualified_cs);
            HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, qualified_cs);
            ddr->shrm_ddr_ptr->periodic_events_enable_flag |= (1<<SHKE_PDT_TRAC_EVENT_CTRL_EN);
            HWIO_OUTXFI (REG_OFFSET_MC_INTERRUPT(ch), INTERRUPT_INTERRUPT_ENABLE_n, 2, DIT_IRQ_EN, 0x1);
        }
        else
        {
            HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, 0);
            HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, 0);
            ddr->shrm_ddr_ptr->periodic_events_enable_flag &= (~(1<<SHKE_PDT_TRAC_EVENT_CTRL_EN));
            HWIO_OUTXFI (REG_OFFSET_MC_INTERRUPT(ch), INTERRUPT_INTERRUPT_ENABLE_n, 2, DIT_IRQ_EN, 0x0);
            HWIO_OUTXFI (REG_OFFSET_MC_INTERRUPT(ch), INTERRUPT_INTERRUPT_CLEAR_n, 2, DIT_IRQ_CLR, 0x1);
        }
        
        HWIO_OUTXF (REG_OFFSET_MC_SHKE(ch), SHKE_LOAD_CONFIG, LOAD_CONFIG, 1);
    }
    
    return TRUE;
}


boolean HAL_DDR_Pre_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz, uint32 new_clk_khz)
{
   uint16 clk_idx =0 ;

   // Update the DDR struct with the requested new clock frequency
   ddr->shrm_ddr_ptr->misc.new_clk_in_kHz = new_clk_khz;
   
   clk_idx =  DDRSS_Find_Clk_Index(ddr, new_clk_khz);

   /* Enabling RPMH port override control*/
   SHRM_rpmh_port_override_control(1);

   /* CLOCK Switch prework for new_clk_khz switch */
   SHRM_rsi_cmd_inject(RSI_CLOCKSWITCH_PREWORK_CMD |clk_idx);

   /* Disabling RPMH port override control*/
   SHRM_rpmh_port_override_control(0);


   return TRUE;
}

boolean HAL_DDR_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz, uint32 new_clk_khz)
{
   uint16 clk_idx =0 ;

   clk_idx =  DDRSS_Find_Clk_Index(ddr, new_clk_khz);

   /* Enabling RPMH port override control*/
   SHRM_rpmh_port_override_control(1);

   /* CLOCK Switch work for new_clk_khz switch */
   SHRM_rsi_cmd_inject(RSI_CLOCKSWITCH_WORK_CMD |clk_idx);

   /* Disabling RPMH port override control*/
   SHRM_rpmh_port_override_control(0);


  return TRUE;
}



boolean HAL_DDR_Post_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz, uint32 new_clk_khz)
{
   uint16 clk_idx =0 ;

   clk_idx =  DDRSS_Find_Clk_Index(ddr, new_clk_khz);

   /* Enabling RPMH port override control*/
   SHRM_rpmh_port_override_control(1);

   /* CLOCK Switch postwork for new_clk_khz switch */
   SHRM_rsi_cmd_inject(RSI_CLOCKSWITCH_POSTWORK_CMD |clk_idx);
 
   /* Disabling RPMH port override control*/
   SHRM_rpmh_port_override_control(0);

   // Save the new clock frequency as the current clock frequency
   ddr->shrm_ddr_ptr->misc.current_clk_in_kHz = ddr->shrm_ddr_ptr->misc.new_clk_in_kHz;

  return TRUE;
}

//================================================================================================//
// Enable or disable downtime requests in DDRSS DT Aggregator
//================================================================================================//
void DDRSS_Downtime_Request_Ctrl (uint8 enable)
{ 
   // enable = 1 -> DT_REQ_DISABLE = 0, enable DT aggregator
   // enable = 0 -> DT_REQ_DISABLE = 1, disable DT aggregator
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                0, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                1, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                2, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                3, DT_REQ_DISABLE, ~enable);				
   
   // Current workaround in MC block to enable/disable Task 5 (Downtime Request) for boot time freq switch
   // TODO: may be removed once downtime request could be disabled correctly in DDRSS level
   // Enable/disable MC freq switch task 5 (Downtime Request)
   HWIO_OUTXFI (REG_OFFSET_MC_SHKE(BROADCAST_CH_NUM), SHKE_FSW_TASK_CFG_n, 5, TASK_EN, enable);
}
   
void DDRSS_Training_Save_Restore (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, SAVE_RESTORE save_restore)
{
    uint8 ch = 0;
    uint8 dq = 0;
    uint8 ca = 0;
    uint8 num_ch = 0;
    training_data *training_data_ptr = NULL;
    uint32 offset = 0;
    
    num_ch = ddr->shrm_ddr_ptr->misc.num_channel;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    for (ch = 0; ch < num_ch; ch++) 
    {
        if ((channel >> ch) & 0x1)
        {
            // Save/restore DQ registers
            offset = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
            
            for (dq = 0; dq < NUM_DQ_PCH; dq++)
            {
                DDRSS_Register_Save_Restore(
                    save_restore,
                    NUM_DQ_TRAINING_STACK,
                    offset + (dq * DDR_PHY_OFFSET),
                    ddr->dq_phy_regs,
                    training_data_ptr->results.training_stack.dq_training_data_stack[ch][dq]);
            }
            
            // Save/restore CA registers
            offset = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
            
            for (ca = 0; ca < NUM_CA_PCH; ca++)
            {
                DDRSS_Register_Save_Restore(
                    save_restore,
                    NUM_CA_TRAINING_STACK,
                    offset + (ca * DDR_PHY_OFFSET),
                    ddr->ca_phy_regs,
                    training_data_ptr->results.training_stack.ca_training_data_stack[ch][ca]);
            }
        }
    }
}

//================================================================================================//
// Setting LLCC cache override
//================================================================================================// 
boolean HAL_DDR_LLCC_cacheable_mode_override_ctrl(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, CACHEABLE_MODE cachable_mode )
{
    LLCC_cacheable_mode_override_ctrl (channel, cachable_mode);

   return TRUE;
}

//================================================================================================//
// Enable/Disable Periodic Control
//================================================================================================// 
boolean HAL_DDR_MC_All_Periodic_Ctrl(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   MC_All_Periodic_Ctrl(ddr, channel, chip_select, enable);
   return TRUE;
}   

//   OP[7]  OP[6] OP[5] OP[4]  OP[3] OP[2] OP[1] OP[0]
//  FSP-OP FSP-WR   DMD   RFU   VRCG   VRO   RPT   CBT
//    0       0      0      0      0    0      0     0
//    0       1      0      0      0    0      0     0

void DDRSS_DRAM_Vref_Restore     (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)

{
    uint8 ch;
    uint8 cs;
    uint8 vref_data;
    //uint8 fsp_op;
    //uint8 fsp_wr;
    uint8 num_ch    = ddr->shrm_ddr_ptr->misc.num_channel;
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    
	for (ch = 0; ch < num_ch; ch++)
    {
        if((channel >> ch) & 0x1)
        {
		  
            for(cs = 0; cs < NUM_CS; cs++)
            {
                if((chip_select >> cs) & 0x1)
                {
                    // TODO : BIMC wait timer
                    // Satisfy tVRCG_ENABLE=200ns. ROUNDUP(200ns/52ns)=0x4
                    seq_wait(200,NS);
                    MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (0x00 | MR13_BASE)); //enable RRO and disable DM
                    // Reset manual_1 timer
                    seq_wait(200,NS);
                    vref_data= training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][0/*die for x8 */][0/*fsp*/] ;
                    MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);
 
                    // TODO : BIMC wait timer
                    // Satisfy tVRCG_ENABLE=200ns. ROUNDUP(200ns/52ns)=0x4
                    //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, 0x04);
                    seq_wait(200,NS);
                  
                    MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (0x40 | MR13_BASE)); //enable RRO and disable DM
                    // Reset manual_1 timer
                    //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, 0x00);
                    seq_wait(200,NS);
                    vref_data= training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][0/*die for x8 */][1/*fsp*/] ;
                    MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);
			    }//if cs
		    }//for cs
	    }//if channel
    }//for channel
}		   

//================================================================================================//
// Enable/Disable Power Control
//================================================================================================// 
boolean HAL_DDR_MC_Power_Saving_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   MC_Power_Saving_Ctrl (ddr, channel, chip_select, enable);
   return TRUE;
}

//================================================================================================//
// Run RD DQDQS Coarse/Find Plot
//================================================================================================// 
boolean HAL_DDR_rd_dqdqs_coarse_fine_plot_pivot(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, uint64* output_iter)
{
   DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
   DDR_CHANNEL channel = (DDR_CHANNEL) (DDR_CH_ALL & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   ddr_external_set_clk_speed (200000);
   // pre_training_setup : pre_training_mode_reg_writes assumes 200MHz
   pre_training_setup(ddr, channel, qualified_cs, clk_freq_khz, 0, &(ddr->training_params), NULL);
   HAL_DDR_Periodic_Training_Ctrl(ddr, 0);

   DDRSS_rd_dqdqs_coarse_fine_plot_pivot(ddr, &(ddr->training_params), clk_freq_khz, output_iter);

   post_training_setup(ddr, channel, qualified_cs, clk_freq_khz, 0, &(ddr->training_params), NULL);

   return TRUE;
}

//================================================================================================//
// Run WR DQDQS Coarse/Find Plot
//================================================================================================// 
boolean HAL_DDR_wr_dqdqs_coarse_fine_plot_pivot(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, uint64* output_iter)
{
   DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
   DDR_CHANNEL channel = (DDR_CHANNEL) (DDR_CH_ALL & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
  
   ddr_external_set_clk_speed (200000);
   // pre_training_setup : pre_training_mode_reg_writes assumes 200MHz
   pre_training_setup(ddr, channel, qualified_cs, clk_freq_khz, 0, &(ddr->training_params), NULL);
   HAL_DDR_Periodic_Training_Ctrl(ddr, 0);

   ddr->training_params.wr_dqdqs.vref_step = 2; // work around for DDI space limitation
   DDRSS_wr_dqdqs_coarse_fine_plot_pivot(ddr, &(ddr->training_params), clk_freq_khz, output_iter);

   post_training_setup(ddr, channel, qualified_cs, clk_freq_khz, 0, &(ddr->training_params), NULL);

   return TRUE;
}



//================================================================================================//
// API to Handle Clock Gating Disable(enable=0)/Enable(enable =1).
//================================================================================================// 
void HAL_DDR_Clock_Gating_Ctrl(BOOT_DDR_STRUCT *ddr, uint8 enable)
{
   /* LLCC To Enable clock gating SET CLK_ENABLE = 0 */
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_LLCC0_SHUB_CBCR , CLK_ENABLE, !enable);
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_LLCC1_SHUB_CBCR , CLK_ENABLE, !enable);
   //HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_LLCC2_SHUB_CBCR , CLK_ENABLE, !enable);
   //HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_LLCC3_SHUB_CBCR , CLK_ENABLE, !enable);

   /*  MEMNOC To Enable clock gating SET CLK_ENABLE = 0 */
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MEMNOC_SHUB_CBCR , CLK_ENABLE, !enable);

   /* MC To Enable clock gating SET CLK_ENABLE = 0 */
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MC0_SHUB_CBCR , CLK_ENABLE, !enable);
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MC1_SHUB_CBCR , CLK_ENABLE, !enable);
   //HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MC2_SHUB_CBCR , CLK_ENABLE, !enable);
   //HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MC3_SHUB_CBCR , CLK_ENABLE, !enable);
   
   /*MCCC To Enable clock gating SET CLK_ENABLE = 0 */
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_MCCC_CBCR , CLK_ENABLE, !enable);

   /*SHRM To Enable clock gating SET CLK_ENABLE = 0 */
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET,DPCC_REG_SHRM_CBCR , CLK_ENABLE, !enable);

   /* Turn Off GCC clock to SHUB */
//   HWIO_OUTF (DDR_SS_REGS_GDSC_DISABLE_HS_REG, SHUB_CLK_UPTREE_GATING_EN,!enable);
   HWIO_OUTXF (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET,DDR_SS_REGS_GDSC_DISABLE_HS_REG, SHUB_CLK_UPTREE_GATING_EN, enable);


}
