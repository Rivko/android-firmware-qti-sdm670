#ifndef BOOT_EXTERN_MONACO_INTERFACE_H
#define BOOT_EXTERN_MONACO_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN MONACO DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external monaco driver
  To be used along with boot_extern_ddr_interface

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
 
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/15   as      Added boot_monaco_get_spd_smbios_info
04/20/15   ck      Added boot_monaco_get_single_instance_info
03/03/15   ck      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DDRTargetLib.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*=============================================================================
    Function : boot_monaco_initialize_device
=============================================================================*/

/**
  @param[in]
    monaco_config_t * monaco_config

  @return
  None.

  @dependencies
  None.
*/
void boot_monaco_initialize_device(monaco_config_t * monaco_config);


/**
  @param[in] 
    ddr_shared_params * ddr_single_instance

  @return
  None.

  @dependencies
  None.
*/

void
boot_monaco_get_single_instance_info(ddr_shared_params * ddr_single_instance);


/**
  @param[in]
    spd_info_per_dimm * ddr_spd_smbios

  @return
  None.

  @dependencies
  None.
*/

void
boot_monaco_get_spd_smbios_info(spd_info_per_dimm * ddr_spd_smbios, size_t array_size);


#endif /* BOOT_EXTERN_MONACO_INTERFACE_H */
