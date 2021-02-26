/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/cdsp/sdm670/ABT_data.c#3 $
$DateTime: 2018/04/06 07:36:35 $
$Author: pwbldsvc $
$Change: 15866471 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2016/05/20  bc     Removing duplicate headers, and reducing public API
2015/11/17  pm     Port to MSM8998 
2015/01/15  tb     Port for MSM8996
2014/12/22  tb     Added optional disable for clocks
2014/07/17  pm     Changed MSS_TOP region name to MODEM_TOP
2014/06/24  tb     Port for MDM9x45
2013/11/14  tb     Added support for multiple enable/status registers
2013/11/14  pm     Port to MSM8916
2013/04/16  pm     Added interrupt priority
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2012 - 2016 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABT_cfg.h"
#include "ABTimeout_HWIO_CDSP.h"

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
  {"TURING_AHBE_TIME" , 0x0,},
};

/* ABT Configuration Data*/
ABT_slave_info_type ABT_cfgdata[] = 
{
  [0] = {
    .name              = "CDSP_AHBE",
    .slave_id          = HWIO_TCSR_TIMEOUT_INTR_STATUS_TURING_IRQ_OUT_AHB_TIMEOUT0_SHFT,
    .base_addr         = (void*)TURING_AHBE_TIME_REG_BASE,
    .clk_name_array    = (char *[]){ "turing_turing_wrapper_bus_timeout_aon_clk" },
    .clk_id_array      = (ClockIdType[]){0},
    .clk_disable_array = (uint8_t[]){ FALSE },
    .num_of_clk        = 1,
    .slave_enable      = true,
    .intr_enable       = true,
    .timeout_val       = 0xFF,
    .is_noc = false,
    .wakeup_id = 0xFF,  // No wakeup needed
    .deferral_pref     = ABT_DEFER_NONE,
    .power_domain      = NULL,
    .power_domain_id   = 0,
    .timebase          = NULL,
  },
};

/* ABT Platform Data type */
ABT_platform_info_type ABT_platform_info =
{
    .name =             "CDSP", /* Image name */
    .intr_map_addr =    (void*[]) /* Interrupt Enable Addresses */
                          { (void*) HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR },
    .intr_status_addr = (void*[]) /* Interrupt Status Addresses */
                          { (void*) HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR },
    .num_status_reg =   1, /* Number of register in intr_map_addr */
    .globa_en_addr =    /* ABT Slave global en address */
                          (void*)HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, 
    .intr_vector =      116,   /* ABT TCSR Summary interrupt vector */
    .intr_priority =    1,     /* ABT Modem Interrupt priority */
    .hwio_map_info =    hwio_map_info,                              /* HWIO map info array  */
    .num_hwio_regions = sizeof(hwio_map_info)/sizeof(hwio_map_info[0]), /* Number of hwio region mappings */    
    .wakeup_addr =      NULL,
    .num_wakeup_reg =   0
};

/* ABT Configuration Property Data*/
ABT_propdata_type ABT_propdata = 
{
    /* Length of the config  data array */
    .len = sizeof(ABT_cfgdata)/sizeof(ABT_slave_info_type), 
    /* Pointer to config data array */ 
    .slave_info = ABT_cfgdata,
    /* Pointer to platform info data */ 
    .platform_info = &ABT_platform_info                                    
};

