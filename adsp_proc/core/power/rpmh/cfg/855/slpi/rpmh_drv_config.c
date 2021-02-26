/*===========================================================================
                              rpmh_drv_config.c

DESCRIPTION:
  Image specific data for DRV configuration. Multiple DRVs may be supported.
  (Example: HLOS and Display). Per DRV config includes DRVID, number of TCSs, 
  partition of TCSs for AMC/Sleep/Wake, 

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rpmh_common.h"
#include "rpmh_drv_config.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define SLPI_WAKE_SET_LATENCY 0x7080 //1.5mS in 19.2Mhz 
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

#define TCS_SLPI_SW_OFFSET 1
#define TCS_SLPI_VIRTUAL_OFFSET 4
/*
 *  SLPI SW TCS CONFIG
 *
 *  NOTE: order must be preserved for now (amc, wake, sleep)
 *
 *  TCS 0 -> AMC for SDC
 *  TCS 1 -> AMC for Q6
 *  TCS 2 -> Sleep TCS for Q6 (standard DRV)
 *  TCS 3 -> Wake TCS for Q6 (standard DRV)
 *  TCS 4 -> AMC for Subsystem (virtual DRV)
 *  TCS 5 -> Sleep TCS for Subsystem (virtual DRV)
 *  TCS 6 -> Wake TCS for Subsystem (virtual DRV)
 *
 */
typedef enum
{
  TCS_SLPI_SW_AMC   = 1 - TCS_SLPI_SW_OFFSET,  //AMC_START
  TCS_SLPI_SW_SLEEP = 2 - TCS_SLPI_SW_OFFSET,  //AMC_END, SLEEP_TCS_START
  TCS_SLPI_SW_WAKE  = 3 - TCS_SLPI_SW_OFFSET,  //WAKE_TCS_START           
  TCS_SLPI_SW_MAX   = 4 - TCS_SLPI_SW_OFFSET   //WAKE_TCS_END             
} tcs_slpi_sw_config_enum;

typedef enum
{
  TCS_SLPI_VIRTUAL_AMC   = 4 - TCS_SLPI_VIRTUAL_OFFSET,  //AMC_START 
  TCS_SLPI_VIRTUAL_SLEEP = 5 - TCS_SLPI_VIRTUAL_OFFSET,  //AMC_END, SLEEP_TCS_START 
  TCS_SLPI_VIRTUAL_WAKE  = 6 - TCS_SLPI_VIRTUAL_OFFSET,  //WAKE_TCS_START           
  TCS_SLPI_VIRTUAL_MAX   = 7 - TCS_SLPI_VIRTUAL_OFFSET   //WAKE_TCS_END             
} tcs_slpi_virtual_config_enum;


//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

static const tcs_config_t tcs_config_slpi =
{
  .num_amcs = (TCS_SLPI_SW_SLEEP - TCS_SLPI_SW_AMC),    
  .sleep_start = TCS_SLPI_SW_SLEEP,
  .wake_start = TCS_SLPI_SW_WAKE,
};

static const tcs_config_t tcs_config_slpi_virtual_active =
{
  .num_amcs = (TCS_SLPI_VIRTUAL_ACTIVE_SLEEP - TCS_SLPI_VIRTUAL_ACTIVE_AMC),    
  .sleep_start = TCS_SLPI_VIRTUAL_ACTIVE_SLEEP,
  .wake_start = TCS_SLPI_VIRTUAL_ACTIVE_WAKE,
};

static const tcs_config_t tcs_config_slpi_virtual =
{
  .num_amcs = (TCS_SLPI_VIRTUAL_SLEEP - TCS_SLPI_VIRTUAL_AMC),    
  .sleep_start = TCS_SLPI_VIRTUAL_SLEEP,
  .wake_start = TCS_SLPI_VIRTUAL_WAKE,
};


/*
 *  DRV CONFIG
 */
static const drv_config_data_t temp_config_data =
{
  .num_drvs = 2,    
  .num_rsc_init_clks = 1,
  .rsc_init_clks = (char*[]) {"scc_rsc_hclk_clk"},
  .num_rsc_sleep_clks = 0,
  .drvs = (drv_config_t[])
  {
    {
      //drv info	    
      .drv_id = RSC_DRV_SENSORS, 		
      .hw_drv = RSC_DRV_SENSORS,
      .global_drv = 5,
      //interrupt config
      .in_irq_num = 40,
      .epcb_timeout_irq_num = 0,
	  
      //wake set latency
      .wake_set_latency = SLPI_WAKE_SET_LATENCY,

#ifdef PDC_READ_WORKAROUND_V1
      .num_cmds_allowed = 3,
#endif //PDC_READ_WORKAROUND_V1      
	  
      //tcs const details
      .tcs_offset = TCS_SLPI_SW_OFFSET,
      .num_tcs = (TCS_SLPI_SW_MAX - TCS_SLPI_SW_AMC),    
      .num_cmds = IMAGE_TCS_SIZE,   

      //tcs config modes
      .num_modes = 1,
      .modes = (const tcs_config_t *[])
      {
        &tcs_config_slpi,
      }
    },
    {
      //drv info	    
      .drv_id = RSC_DRV_VIRTUAL_SENSORS, 	
      .hw_drv = RSC_DRV_SENSORS,
      .global_drv = 5,
      //interrupt config
      .epcb_timeout_irq_num = 0,

#ifdef PDC_READ_WORKAROUND_V1
      .num_cmds_allowed = 2,
#endif //PDC_READ_WORKAROUND_V1      


      //tcs const details
      .tcs_offset = TCS_SLPI_VIRTUAL_OFFSET,
      .num_tcs = (TCS_SLPI_VIRTUAL_MAX - TCS_SLPI_VIRTUAL_AMC),	    
      .num_cmds = IMAGE_TCS_SIZE,   

      //tcs config modes
      .num_modes = 1,
      .modes = (const tcs_config_t *[])
      {
        &tcs_config_slpi_virtual,
      }
    }
  }
};

/*
 *  pointer to make DRV CONFIG accessible
 */
const drv_config_data_t *const DRV_CONFIG_DATA = &temp_config_data;


//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
uint8 get_global_drv_id(uint32 drv_index)
{
  rpmh_core_verify(drv_index < DRV_CONFIG_DATA->num_drvs);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  return (uint8)drv->global_drv;
}

