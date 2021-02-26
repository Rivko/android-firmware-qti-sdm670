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
#define ADSP_WAKE_SET_LATENCY 0x7080 //1.5mS in 19.2Mhz 
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/*
 *  ADSP SW TCS CONFIG
 *
 *  NOTE: order must be preserved for now (amc, wake, sleep)
 */
typedef enum
{
  TCS_ADSP_SW_AMC   = 0,   //AMC_START
  TCS_ADSP_SW_SLEEP = 2,   //AMC_END, SLEEP_TCS_START
  TCS_ADSP_SW_WAKE  = 3,   //WAKE_TCS_START
  TCS_ADSP_SW_MAX   = 4    //WAKE_TCS_END
} tcs_adsp_sw_config_enum;

static const tcs_config_t tcs_config_adsp =
{
  .num_amcs = (TCS_ADSP_SW_SLEEP - TCS_ADSP_SW_AMC),    
  .sleep_start = TCS_ADSP_SW_SLEEP,
  .wake_start = TCS_ADSP_SW_WAKE,
};


//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

/*
 *  DRV CONFIG
 */
static const drv_config_data_t temp_config_data =
{
  .num_drvs = 1,	    
  .num_rsc_init_clks = 0,
  .num_rsc_sleep_clks = 0,
  .drvs = (drv_config_t[])
  {
    {
      //drv info	    
      .drv_id = RSC_DRV_AUDIO, 		
      .hw_drv = RSC_DRV_AUDIO, 		
      .global_drv = 4,
      //interrupt config
      .in_irq_num = 41,
      .epcb_timeout_irq_num = 21,

      //wake set latency
      .wake_set_latency = ADSP_WAKE_SET_LATENCY,
	  
      //tcs const details
      .tcs_offset = 0,
      .num_tcs = TCS_ADSP_SW_MAX,	    
      .num_cmds = IMAGE_TCS_SIZE,   

      //tcs config modes
      .num_modes = 1,
      .modes = (const tcs_config_t *[])
      {
        &tcs_config_adsp,
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

