/*===========================================================================
                              rpmh_client.c

DESCRIPTION: 
  Main client facing functionality. Initialization, sending 
  commands, sending command sets, completing commands/comand sets, invalidating
  commands, entering/exiting sleep, checking if a drv is valid. 

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "Library/rpmh_client.h"
#include "Library/rpmh_resource_commands.h"
//#include "Library/rpmh_client_internal.h"
//#include "Library/rpmh_drv_config.h"
#include <malloc.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIRpmh.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/**
 * <!-- rpmh_is_drv_id_valid -->
 *
 * @breif Checks whether or not the given RSC+DRV is valid for the context
 *
 * @param drv_id : The RSC+DRV mapping to check
 *
 * @return whether or not the given ID is supported in the given context
 */
boolean rpmh_is_drv_id_valid (RSCSW_DRV_MAPPING drv_id)
{
    boolean drv_id_valid =0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_is_drv_id_valid(drv_id, &drv_id_valid);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return drv_id_valid;
}


/**
 * <!-- rpmh_tcs_num_cmds -->
 *
 * @brief Return the # commands per TCS for this DRV
 *
 * @param drv_id : RSC+DRV id for num cmds check
 *
 * @return number of commands in the TCSs for this DRV
 */
uint32 rpmh_tcs_num_cmds (RSCSW_DRV_MAPPING drv_id)
{
    uint32 drv_id_size =0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_tcs_num_cmds(drv_id, &drv_id_size);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return drv_id_size;
}


/**
 * <!-- rpmh_create_handle_internal -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 * @param explicit_client : Tells us if this client is explicit or not
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle_internal (RSCSW_DRV_MAPPING drv_id, const char *client_name, boolean explicit_client)
{ 
    rpmh_client_handle client_handle = 0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_create_handle_internal(drv_id, client_name, explicit_client, &client_handle);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return client_handle;
}


/**
 * <!-- rpmh_create_handle_explicit -->
 *
 * @brief Create an explicit client handle. 
 *
 *  Explicit client means for sleep/wake requests, the client will take
 *  care of telling the RPMh driver exactly what is required via rpmh_issue_command_set. The RPMh
 *  driver will not fill in any wake/sleep values based on active requests.
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands.
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle_explicit (RSCSW_DRV_MAPPING drv_id, const char *client_name)
{
    rpmh_client_handle client_handle = 0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_create_handle_explicit(drv_id, client_name, &client_handle);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return client_handle;
}


/**
 * <!-- rpmh_create_handle -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 * @param explicit_client : Tells us if this client is explicit or not
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle (RSCSW_DRV_MAPPING drv_id, const char *client_name)
{
    rpmh_client_handle client_handle = 0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_create_handle(drv_id, client_name, &client_handle);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return client_handle;
}

/**
 * <!-- rpmh_explicit_client_invalidate -->
 *
 * @brief Invalidate requests for the given explicit client 
 *
 * @param drv_id : The explicit client on which to invalidate votes
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
void rpmh_invalidate_explicit_cmds(rpmh_client_handle client_handle)
{  
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_invalidate_explicit_cmds(client_handle);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return;
}

/**
 * <!-- rpmh_client_completion_callback -->
 *
 * @brief Each client is allowed a single callback for completion requests. 
 *        This API registers that callback 
 *
 * @param client_handle : The client associated with the callback being registered
 * @param callback      : The callback function
 */
void rpmh_client_register_completion_callback(rpmh_client_handle client_handle, rpmh_completion_cb callback)
{
  
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_client_register_completion_callback(client_handle, callback);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return;
}

/**
 * <!-- rpmh_delete_handle -->
 *
 * @breif Delete a client handle.
 *
 * @param handle : The handle to delete
 */ 
void rpmh_delete_handle (rpmh_client_handle client_handle)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_delete_handle(client_handle);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return; 
}

/**
 * <!-- rpmh_find_resource_command -->
 *
 * @brief Lookup the resource command for the corresponding address
 *
 * @param address : The address of the corresponding resource command to find
 *
 * @return a pointer to the resource command
 */ 
rpmh_resource_command_t *rpmh_find_resource_command (uint32 address)
{
    rpmh_resource_command_t *resource_command =NULL;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_find_resource_command(address, &resource_command );
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return resource_command;
}
 
/**
 * <!-- rpmh_churn_all -->
 *
 * @brief Poll for all commands up to and including the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_churn_all (rpmh_client_handle client_handle, uint32 req_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_churn_all(client_handle, req_id );
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return;
}


/**
 * <!-- rpmh_churn_single -->
 *
 * @brief Poll for the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_churn_single (rpmh_client_handle client_handle, uint32 req_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_churn_single(client_handle, req_id );
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }

    return;
}

//TODO register this with interrupt handler
/**
 * <!-- rpmh_finish_amc -->
 *
 * @brief Handle a finished AMC
 *
 * @param drv_id : The RSC+DRV mapping for the AMC
 */ 
void rpmh_finish_amc (RSCSW_DRV_MAPPING drv_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_finish_amc(drv_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }

    return;
}


/**
 * <!-- rpmh_finsh_sleep_wake_amc -->
 *
 * @brief Handle a finished sleep or wake AMC. 
 *
 * @param drv_id : The RSC+DRV mapping for the AMC
 * @param set    : The set associated with the finished AMC
 */ 
void rpmh_finish_sleep_wake_amc (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_finish_sleep_wake_amc(drv_id, set);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }

    return;
}


/**
 * <!-- rpmh_issue_command_set -->
 *
 * @brief Issue a command set (multiple commands in parallel). If none of the resource commands
 *          are dirty, the command set is not sent. However if at least one resource command is
 *          dirty, the command set will be sent. The not dirty commands will finish quickly as
 *          the HW aggregation won't update, so the dirty commands will be the long pole.
 *
 * @param handle      : The client handle associated with the command set
 * @param command_set : The structure containing all the details of the command set request
 *
 * @return ID for command set on this handle. Used in barrier APIs to track completion
 */ 
uint32 rpmh_issue_command_set (rpmh_client_handle client_handle, rpmh_command_set_t *cmd_set)
{
    uint32 req_id = 0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_issue_command_set(client_handle, cmd_set, &req_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }

    return req_id;
}


/**
 * <!-- rpmh_invalidate_sleep_command -->
 * @breif Remove the sleep command for a given address (resource command).
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_sleep_command (rpmh_client_handle client_handle, uint32 address)
{ 
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_invalidate_sleep_command(client_handle, address);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
}


/**
 * <!-- rpmh_invalidate_wake_command -->
 *
 * @brief Remove the wake command for a given address (resource command). 
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_wake_command (rpmh_client_handle client_handle, uint32 address)
{ 
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_invalidate_wake_command(client_handle, address);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
}


/**
 * <!-- rpmh_issue_read_command -->
 *
 * @brief Issue a read command and return the data. THIS IS A BLOCKING API! 
 *
 *  This will block the given SW thread until the data is read from HW.
 *
 * @param handle  : The client handle associated with the read command
 * @param address : Concatenation of slaveID and offset
 * @param data    : Data to be sent
 * @param polling : TRUE for polling mode, FALSE for event based handling
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
uint32 rpmh_issue_read_command (rpmh_client_handle client_handle, uint32 address, boolean polling)
{
    uint32 data = 0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_issue_read_command(client_handle, address, polling, &data);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
  
    return data;
}

/**
 * <!-- rpmh_issue_command -->
 *
 * @brief Issue a single command
 *
 * @param handle     : The client handle associated with the command
 * @param set        : The set associated with the command
 * @param completion : TRUE for completion requests, FALSE for fire and forget
 * @param address    : Concatenation of slaveID and offset
 * @param data       : Data to be sent
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
uint32 rpmh_issue_command(rpmh_client_handle client_handle, rpmh_set_enum set, boolean completion, 
                          uint32 address, uint32 data)
{
    uint32 req_id =0;
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_issue_command(client_handle, set, completion, address, data, &req_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
  
    return req_id;  
}


/**
 * <!-- rpmh_enter_sleep -->
 *
 * @breif API to configure RSCSW_DRV for entering sleep. 
 *        Configures RSC TCSs, sends sleep AMCs if needed  
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep
 */
void rpmh_enter_sleep (RSCSW_DRV_MAPPING drv_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_enter_sleep(drv_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }

    return ; 
}

/**
 * <!-- rpmh_exit_sleep -->
 *
 * @brief API to configure RSCSW_DRV for exiting sleep. 
 *        Sends wake AMCs if needed, cleans up driver context sleep 
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep
 */
void rpmh_exit_sleep (RSCSW_DRV_MAPPING drv_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_exit_sleep(drv_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return ; 
}

void rpmh_barrier_all (rpmh_client_handle client_handle, uint32 req_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_barrier_all(client_handle, req_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return ;
}

void rpmh_barrier_single (rpmh_client_handle client_handle, uint32 req_id)
{
    EFI_RPMH_PROTOCOL *gRpmhProtocol;
    EFI_STATUS status;
    status = gBS->LocateProtocol(&gEfiRpmhProtocolGuid, NULL, (void**)&gRpmhProtocol);

    if(status == EFI_SUCCESS)  
    {
       gRpmhProtocol->rpmh_barrier_single(client_handle, req_id);
    }
    else
    {
       DEBUG ((EFI_D_ERROR, "Rpmh protocol is not initialized:%r\n", status));
    }
    return ;
}