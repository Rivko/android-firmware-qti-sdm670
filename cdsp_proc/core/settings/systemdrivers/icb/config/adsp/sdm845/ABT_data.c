/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History


2017/3/29  sds   update with rumi testing
2017/2/22  aks   created 
 
        Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABT_cfg.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "/dev/ABTimeout"                */
/*---------------------------------------------------------------------------*/

/* ABT HWIO map array */
ABT_platform_hwio_info_type hwio_map_info[] =
{
  {"CORE_TOP_CSR" , 0x0,},
  {"LPASS" , 0x0,},
};

/* ABT Configuration Data*/
ABT_slave_info_type ABT_cfgdata[] = 
{
  [0] = {
    .name              = "LPASS_AHBE",
    .slave_id          = HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT0_SHFT,
    .base_addr         = (void*)LPASS_AHBE_TIME_REG_BASE,
    .clk_name_array    = (char *[]){ "lpass_audio_wrapper_bus_timeout_aon_clk" },
    .clk_disable_array = (uint8_t[]){ FALSE },
    .num_of_clk        = 1,
    .slave_enable      = TRUE,
    .intr_enable       = TRUE,
    .timeout_val       = 0xFF,
    .is_noc = FALSE,
    .wakeup_id = 0xFF,  // No wakeup needed
  },
  [1] = {
    .name              = "LPASS_AHBI",
    .slave_id          = HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT1_SHFT,
    .base_addr         = (void*)LPASS_AHBI_TIME_REG_BASE,
    .clk_name_array    = (char *[]){ "lpass_audio_core_bus_timeout_core_clk" },
    .clk_disable_array = (uint8_t[]){ FALSE },
    .num_of_clk        = 1,
    .slave_enable      = TRUE,
    .intr_enable       = TRUE,
    .timeout_val       = 0xFF,
    .is_noc = FALSE,
    .wakeup_id = 0xFF,  // No wakeup needed
  },
  [2] = {
    .name              = "LPASS_AHBL",
    .slave_id          = HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT2_SHFT,
    .base_addr         = (void*)LPASS_AHBL_TIME_REG_BASE,
    .clk_name_array    = (char *[]){ "lpass_audio_core_bus_timeout_core_clk" },
    .clk_disable_array = (uint8_t[]){ FALSE },
    .num_of_clk        = 1,
    .slave_enable      = TRUE,
    .intr_enable       = TRUE,
    .timeout_val       = 0xFF,
    .is_noc = FALSE,
    .wakeup_id = 0xFF,  // No wakeup needed
  },
};

static char* aPowerDomains[]=
{
   "audio_core_gdsc",
};


/* ABT Platform Data type */
ABT_platform_info_type ABT_platform_info =
{
    .name =             "LPASS",                                      /* Image name */
    .intr_map_addr =    (void*[]) /* Interrupt Enable Addresses */
                          { (void*) HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR },
    .intr_status_addr = (void*[]) /* Interrupt Status Addresses */
                          { (void*) HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR },
    .num_status_reg =   1, /* Number of register in intr_map_addr */
    .globa_en_addr =    /* ABT Slave global en address */
                          (void*)HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, 
    .intr_vector =      90,   /* ABT TCSR Summary interrupt vector */
    .intr_priority =    1,     /* ABT LPASS Interrupt priority */
    .hwio_map_info =    hwio_map_info,                              /* HWIO map info array  */
    .num_hwio_regions = sizeof(hwio_map_info)/sizeof(hwio_map_info[0]), /* Number of hwio region mappings */    
    .wakeup_addr =      NULL,
    .num_wakeup_reg =   0,
    .power_domain_info = aPowerDomains,
    .num_power_domains = sizeof(aPowerDomains) / sizeof(aPowerDomains[0]),
};

/* ABT Configuration Property Data*/
ABT_propdata_type ABT_propdata = 
{
    /* Length of the config  data array */
    .len = sizeof(ABT_cfgdata)/sizeof(ABT_cfgdata[0]), 
    /* Pointer to config data array */ 
    .slave_info = ABT_cfgdata,
    /* Pointer to platform info data */ 
    .platform_info = &ABT_platform_info                                    
};

