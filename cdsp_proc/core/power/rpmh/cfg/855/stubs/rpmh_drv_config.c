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

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/*
 *  MPSS SW TCS CONFIG
 *
 *  NOTE: order must be preserved for now (amc, wake, sleep)
 */
typedef enum
{
  TCS_MPSS_SW_AMC = 0,   //AMC_START
  TCS_MPSS_SW_SLEEP = 2, //AMC_END, SLEEP_TCS_START
  TCS_MPSS_SW_WAKE = 4,  //WAKE_TCS_START
  TCS_MPSS_SW_MAX = 6    //WAKE_TCS_END
} tcs_mpss_sw_config_enum;

/*
 *  MPSS HW TCS CONFIG
 *
 *  NOTE: order must be preserved for now (amc, wake, sleep)
 */
typedef enum
{
  TCS_MPSS_HW_AMC = 0,   //AMC_START
  TCS_MPSS_HW_SLEEP = 2, //AMC_END, SLEEP_TCS_START
  TCS_MPSS_HW_WAKE = 3,  //WAKE_TCS_START
  TCS_MPSS_HW_MAX = 4    //WAKE_TCS_END
} tcs_mpss_hw_config_enum;


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
  .num_drvs = 2,	    
  .drvs = (drv_config_t[])
  {
    {
      .drv_id = RSC_DRV_MODEM_SW, 		
      .num_tcs = TCS_MPSS_SW_MAX,	    
      .num_cmds = IMAGE_TCS_SIZE,   
      .num_amcs = (TCS_MPSS_SW_SLEEP - TCS_MPSS_SW_AMC),    
      .wake_start = TCS_MPSS_SW_WAKE,
      .sleep_start = TCS_MPSS_SW_SLEEP,
      .in_irq_num = 7,
    },    
    {
      .drv_id = RSC_DRV_MODEM_HW, 	
      .num_tcs = TCS_MPSS_HW_MAX,		    
      .num_cmds = IMAGE_TCS_SIZE,	    
      .num_amcs = (TCS_MPSS_HW_SLEEP - TCS_MPSS_HW_AMC),		    
      .wake_start = TCS_MPSS_HW_WAKE,
      .sleep_start = TCS_MPSS_HW_SLEEP,
      .in_irq_num = 8,
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

