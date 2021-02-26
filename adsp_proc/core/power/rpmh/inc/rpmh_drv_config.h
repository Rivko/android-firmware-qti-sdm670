#ifndef RPMH_DRV_CONFIG_H
#define RPMH_DRV_CONFIG_H
/*===========================================================================
                             rpmh_drv_config.h

DESCRIPTION:
  Provides structures needed for image specific DRV config. Additionally 
  provides access to the corresponding images DRV config.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
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
#include "rpmh_os.h"

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
  uint32                global_drv; //global drv_id
  uint32                global_drv_2; //only used if the target supports different global IDs based on version
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
                               //   between for the given DRV
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

/**
 * <!-- rpmh_resource_command_init -->
 *
 * @brief Retrieve the global DRV ID mapping, used for priority lookup in cmd_db. Note
 *          this has to be in the target specific area as IDs can change from version to
 *          version for the same target. 
 *
 * @param drv_index : Local drv index for global lookup
 *
 * @return the global DRV ID
 */
uint8 get_global_drv_id(uint32 drv_index);

#endif /* RPMH_DRV_CONFIG_H */
