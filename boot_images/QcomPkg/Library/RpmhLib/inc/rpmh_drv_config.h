#ifndef RPMH_DRV_CONFIG_H
#define RPMH_DRV_CONFIG_H
/*===========================================================================
                             rpmh_drv_config.h

DESCRIPTION:
  Provides structures needed for image specific DRV config. Additionally 
  provides access to the corresponding images DRV config.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rpmh_common.h"
#include "rpmh_image_config.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

#ifdef RPMH_SENSORS_ISLAND
typedef enum
{
  SLPI_VIRTUAL_TCS_CONFIG_ACTIVE,
  SLPI_VIRTUAL_TCS_CONFIG_SLEEP,
} tcs_virtual_config_enum;
#endif //RPMH_SENSORS_ISLAND

typedef struct
{
  uint32            num_amcs;  //# AMCs supported by this RSCSW_DRV    
  uint32            wake_start;  //start of wake TCSs    
  uint32            sleep_start;  //start of sleep TCSs    
} tcs_config_t;

typedef struct
{
  RSCSW_DRV_MAPPING     drv_id;     //includes virtual mapping	
  RSCSW_DRV_MAPPING     hw_drv;	//only HW DRVs 
  uint32                in_irq_num;
  uint32                epcb_timeout_irq_num;
  uint32                wake_set_latency;   //static worst case wake set latency value
#ifdef PDC_READ_WORKAROUND_V1
  uint32                num_cmds_allowed;  //V1 PDC READ bug requires we limit the # of cmds being processed     
#endif //PDC_READ_WORKAROUND_V1      
  uint32                tcs_offset;  //for virtual DRV support. The actual TCSs don't always start from 0
  uint32                num_tcs;   //# TCSs supported by this RSCSW_DRV	    
  uint32                num_cmds;  //# CMDs per TCS supported by this RSCSW_DRV	    
  uint32                num_modes; //number of supported TCS configs
  const tcs_config_t**  modes;    //TCS configuration. These are the static configs we can switch 
} drv_config_t;

typedef struct
{
    uint32       num_drvs;           //# RCSW_DRVs supported by this image
    uint32       num_rsc_init_clks;  //# of clocks we need to turn on at init for RSC access/operation
    char**       rsc_init_clks;      //list of RSC clock names we need to turn on at init for RSC access/operation
    uint32       num_rsc_sleep_clks; //# of clocks we need to turn on at init, 
                                     // and turn off in sleep for RSC access/operation
    char**       rsc_sleep_clks;     //list of RSC clock names we need to turn on at init,
                                     // and turn off in sleep for RSC access/operation

    drv_config_t *drvs;    //DRV list
} drv_config_data_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
extern const drv_config_data_t *const DRV_CONFIG_DATA;

//===========================================================================
//                             Function Definitions
//===========================================================================


#endif /* RPMH_DRV_CONFIG_H */
