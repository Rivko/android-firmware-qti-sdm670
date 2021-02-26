#ifndef RPMH_UEFI_H
#define RPMH_UEFI_H

/*===========================================================================
                             rpmh_uefi.h

DESCRIPTION:
  Interface for interacting with TCSs. This includes managing TCS state, 
  configuring TCSs, and handling TCS completion.  

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

#include "rpmh_resource_commands.h"



rpmh_client_handle rpmh_create_handle_internal (RSCSW_DRV_MAPPING drv_id, const char *client_name, boolean explicit_client);
//uint32 rpmh_wait_for_read (rpmh_client_t *handle, uint32 req_id, boolean polling);

void rpmh_finish_sleep_wake_amc (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set);
uint32 rpmh_issue_read_command (rpmh_client_handle client_handle, uint32 address, boolean polling);

rpmh_resource_command_t *rpmh_find_resource_command (uint32 address);
rpmh_resource_command_t *rpmh_add_resource_command (uint32 address);
void rpmh_finish_amc (RSCSW_DRV_MAPPING drv_id);


#endif // #define RPMH_UEFI_H

