/** @file RpmhDxe.c

  This file implements RPMH EFI protocol interface.

  Copyright (c) 2016-2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  reserved. 

**/

/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIRpmh.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Library/rpmh_client.h>
#include <Library/rpmh_common.h>
#include <Library/rpmh_image_config.h>
#include <Library/rpmh_resource_commands.h>
#include <Library/rpmh_uefi.h>

/*=========================================================================
      Globals and prototypes
==========================================================================*/

STATIC EFI_HANDLE  gRpmhProtocolHandle = NULL;
STATIC EFI_STATUS EFI_rpmh_deinit(void);

/* //Exit Boot Services vars */
EFI_EVENT EfiExitBootServicesEvent = (EFI_EVENT)NULL;
extern EFI_GUID gEfiEventExitBootServicesGuid;

/*----------------------------------------------------------------------------
 * RPMH API
 * -------------------------------------------------------------------------*/

/**
 * <!-- rpmh_init -->
 *
  @brief rpmh_init - Initializes the RPMH subsystem.
* @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_init( VOID)
{
  rpmh_client_init();
  
  return EFI_SUCCESS;
}
/**
 * <!-- EFI_rpm_post_request -->
 *
 * @brief RPM Request Service API
 *
 * @param[in]  set        The RPM "set" which receives this request, affecting what scenarios it applies to.
 * @param[in]  resource   The class of resource being requested.
 * @param[in]  id         The particular resource being addressed.
 * @param[in]  kvps       The data being sent in this request.
 * @param[out] pMsg_id Pointer to unique identifier for this request.
 *
 * @return       EFI_SUCCESS
 *
 */
STATIC EFI_STATUS EFI_rpmh_is_drv_id_valid
(
  RSCSW_DRV_MAPPING drv_id,
  boolean *drv_id_valid
)
{
  if (drv_id_valid == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *drv_id_valid = rpmh_is_drv_id_valid(drv_id);
  
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_rpm_churn_queue -->
 *
 * @brief Polls the RPM message queue until ACK for specified message received.
 *
 * @param[in] message_id
 *
 * @return       EFI_SUCCESS
 *
 */
STATIC EFI_STATUS EFI_rpmh_tcs_num_cmds
(
  RSCSW_DRV_MAPPING drv_id,
  uint32 *drv_id_size
)
{
  if (drv_id_size == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *drv_id_size = rpmh_tcs_num_cmds(drv_id);
  
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_rpmh_create_handle_internal -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param[in] drv_id          : The RSC+DRV mapping for the client
 * @param[in] client_name     : A unique string to identify the client
 * @param[in] explicit_client : Tells us if this client is explicit or not
 * @param[out] An opaque pointer to the handle, to be used for future RPMh driver APIs
 *
 * @return       EFI_SUCCESS
 * 
 */
STATIC EFI_STATUS EFI_rpmh_create_handle_internal
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  boolean explicit_client,
  rpmh_client_handle *client_handle
)
{
  if ((client_name == NULL) || client_handle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
 
  *client_handle = rpmh_create_handle_internal(drv_id, client_name, explicit_client);
  
  return EFI_SUCCESS;
}
/**
 * <!-- EFI_rpmh_create_handle_explicit -->
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
 * @param[in] drv_id          : The RSC+DRV mapping for the client
 * @param[in] client_name     : A unique string to identify the client
 * @param[out] An opaque pointer to the handle, to be used for future RPMh driver APIs
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_create_handle_explicit
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  rpmh_client_handle *client_handle
)
{
  if ((client_name == NULL) || client_handle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *client_handle = rpmh_create_handle_explicit(drv_id, client_name);
  
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_rpmh_create_handle -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param[in] drv_id          : The RSC+DRV mapping for the client
 * @param[in] client_name     : A unique string to identify the client
 * @param[in] explicit_client : Tells us if this client is explicit or not
 * @param[out] An opaque pointer to the handle, to be used for future RPMh driver APIs
 *
 * @return       EFI_SUCCESS
 * 
 */
STATIC EFI_STATUS EFI_rpmh_create_handle
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  rpmh_client_handle *client_handle
)
{
  if ((client_name == NULL) || client_handle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *client_handle = rpmh_create_handle(drv_id, client_name);
  
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_rpmh_invalidate_explicit_cmds -->
 *
 * @brief Invalidate requests for the given explicit client 
 *
 * @param[in] drv_id : The explicit client on which to invalidate votes
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_invalidate_explicit_cmds
(
  rpmh_client_handle client_handle
)
{
  rpmh_invalidate_explicit_cmds(client_handle);
  
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_rpmh_client_register_completion_callback -->
 *
 * @brief Each client is allowed a single callback for completion requests. 
 *        This API registers that callback 
 *
 * @param[in] client_handle : The client associated with the callback being registered
 * @param[in] callback      : The callback function
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_client_register_completion_callback
(
  rpmh_client_handle client_handle,
  rpmh_completion_cb callback
)
{
  rpmh_client_register_completion_callback(client_handle, callback);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_delete_handle -->
 *
 * @breif Delete a client handle.
 *
 * @param[in] handle : The handle to delete
 *
 * @return       EFI_SUCCESS
 */ 
STATIC EFI_STATUS EFI_rpmh_delete_handle
(
 rpmh_client_handle client_handle
)
{
  rpmh_delete_handle(client_handle);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_find_resource_command -->
 *
 * @brief Lookup the resource command for the corresponding address
 *
 * @param[in] address : The address of the corresponding resource command to find
 *
 * @param[out] a pointer to the resource command
 *
 * @return       EFI_SUCCESS
 */ 
STATIC EFI_STATUS EFI_rpmh_find_resource_command
(
  uint32 address,
  rpmh_resource_command_t **resource_command
)
{
  if (resource_command == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *resource_command = rpmh_find_resource_command(address);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_churn_all -->
 *
 * @brief Poll for all commands up to and including the given req_id to complete
 *
 * @param[in] handle : The handle the command is associated with
 * @param[in] req_id : The command ID to wait on
 *
 * @return       EFI_SUCCESS
 * 
 */ 
STATIC EFI_STATUS EFI_rpmh_churn_all
(
  rpmh_client_handle client_handle,
  uint32 req_id
)
{
  rpmh_churn_all(client_handle, req_id );
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_churn_single -->
 *
 * @brief Poll for the given req_id to complete
 *
 * @param[in] handle : The handle the command is associated with
 * @param[in] req_id : The command ID to wait on
 *
 * @return       EFI_SUCCESS
 */ 
STATIC EFI_STATUS EFI_rpmh_churn_single
(
  rpmh_client_handle client_handle,
  uint32 req_id
)
{
  rpmh_churn_single(client_handle, req_id );
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_finish_amc -->
 *
 * @brief Handle a finished AMC
 *
 * @param[in] drv_id : The RSC+DRV mapping for the AMC
 * 
 * @return       EFI_SUCCESS
 * 
 */ 

STATIC EFI_STATUS EFI_rpmh_finish_amc
(
  RSCSW_DRV_MAPPING drv_id
)
{
  rpmh_finish_amc(drv_id);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_finsh_sleep_wake_amc -->
 *
 * @brief Handle a finished sleep or wake AMC. 
 *
 * @param[in] drv_id : The RSC+DRV mapping for the AMC
 * @param[in] set    : The set associated with the finished AMC
 *
 * @return       EFI_SUCCESS
 */ 
STATIC EFI_STATUS EFI_rpmh_finish_sleep_wake_amc
(
  RSCSW_DRV_MAPPING drv_id,
  rpmh_set_enum set
)
{
  rpmh_finish_sleep_wake_amc(drv_id, set);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_issue_command_set -->
 *
 * @brief Issue a command set (multiple commands in parallel). If none of the resource commands
 *          are dirty, the command set is not sent. However if at least one resource command is
 *          dirty, the command set will be sent. The not dirty commands will finish quickly as
 *          the HW aggregation won't update, so the dirty commands will be the long pole.
 *
 * @param[in] handle      : The client handle associated with the command set
 * @param[in] command_set : The structure containing all the details of the command set request
 * @param[out] ID for command set on this handle. Used in barrier APIs to track completion
 *
 * @return       EFI_SUCCESS
 */ 
STATIC EFI_STATUS EFI_rpmh_issue_command_set
(
  rpmh_client_handle client_handle,
  rpmh_command_set_t *cmd_set,
  uint32 *req_id
)
{
  if ((cmd_set == NULL) || req_id == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *req_id = rpmh_issue_command_set(client_handle, cmd_set);
  
  return EFI_SUCCESS;
}
/**
 * <!-- rpmh_invalidate_sleep_command -->
 * @breif Remove the sleep command for a given address (resource command).
 *
 * @param[in] handle  : The client handle associated with the invalidate
 * @param[in] address : Concatenation of slaveID and offset
 *
 * @return       EFI_SUCCESS
 */

STATIC EFI_STATUS EFI_rpmh_invalidate_sleep_command
(
  rpmh_client_handle client_handle,
  uint32 address
)
{
  rpmh_invalidate_sleep_command(client_handle, address);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_invalidate_wake_command -->
 *
 * @brief Remove the wake command for a given address (resource command). 
 *
 * @param[in] handle  : The client handle associated with the invalidate
 * @param[in] address : Concatenation of slaveID and offset
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_invalidate_wake_command
(
  rpmh_client_handle client_handle,
  uint32 address
)
{
  rpmh_invalidate_wake_command(client_handle, address);
  
  return EFI_SUCCESS;
}
/**
 * <!-- rpmh_issue_read_command -->
 *
 * @brief Issue a read command and return the data. THIS IS A BLOCKING API! 
 *
 *  This will block the given SW thread until the data is read from HW.
 *
 * @param[in] handle  : The client handle associated with the read command
 * @param[in] address : Concatenation of slaveID and offset
 * @param[in] data    : Data to be sent
 * @param[in] polling : TRUE for polling mode, FALSE for event based handling
 * @param[out] ID for this command on this handle. Used in barrier APIs to track completion
 *
 * @return       EFI_SUCCESS
 */

STATIC EFI_STATUS EFI_rpmh_issue_read_command
(
  rpmh_client_handle client_handle,
  uint32 address,
  boolean polling,
  uint32 *data
)
{
    return EFI_UNSUPPORTED;
}

/**
 * <!-- rpmh_issue_command -->
 *
 * @brief Issue a single command
 *
 * @param[in] handle     : The client handle associated with the command
 * @param[in] set        : The set associated with the command
 * @param[in] completion : TRUE for completion requests, FALSE for fire and forget
 * @param[in] address    : Concatenation of slaveID and offset
 * @param[in] data       : Data to be sent
 * @param[out] ID for this command on this handle. Used in barrier APIs to track completion
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_issue_command
(
  rpmh_client_handle client_handle,
  rpmh_set_enum set,
  boolean completion,
  uint32 address,
  uint32 data,
  uint32 *req_id
)
{
  if (req_id == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *req_id = rpmh_issue_command(client_handle, set, completion, address, data);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_enter_sleep -->
 *
 * @breif API to configure RSCSW_DRV for entering sleep. 
 *        Configures RSC TCSs, sends sleep AMCs if needed  
 *
 * @param[in] drv_id : The RSC+DRV mapping to configure for sleep
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_enter_sleep
(
  RSCSW_DRV_MAPPING drv_id
)
{
  rpmh_enter_sleep(drv_id);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_exit_sleep -->
 *
 * @brief API to configure RSCSW_DRV for exiting sleep. 
 *        Sends wake AMCs if needed, cleans up driver context sleep 
 *
 * @param[in] drv_id : The RSC+DRV mapping to configure for sleep
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_exit_sleep
(
  RSCSW_DRV_MAPPING drv_id
)
{
  rpmh_exit_sleep(drv_id);
  
  return EFI_SUCCESS;
}

/**
 * <!-- rpmh_barrier_all -->
 * RPMH Barrier functionality support is not yet added.
 */
 
STATIC EFI_STATUS EFI_rpmh_barrier_all
(
  rpmh_client_handle client_handle,
  uint32 		     req_id
)
{ 
  return EFI_UNSUPPORTED;
}

/**
 * <!-- rpmh_barrier_single -->
 * RPMH Barrier functionality support is not yet added.
 */
 
STATIC EFI_STATUS EFI_rpmh_barrier_single
(
  rpmh_client_handle client_handle,
  uint32 		     req_id
)
{ 
  return EFI_UNSUPPORTED;
}

STATIC EFI_RPMH_PROTOCOL RpmhProtocol =
{
  EFI_RPMH_PROTOCOL_REVISION,
  EFI_rpmh_init,
  EFI_rpmh_is_drv_id_valid,
  EFI_rpmh_tcs_num_cmds,
  EFI_rpmh_create_handle_internal,
  EFI_rpmh_create_handle_explicit,
  EFI_rpmh_create_handle,
  EFI_rpmh_invalidate_explicit_cmds,
  EFI_rpmh_client_register_completion_callback,
  EFI_rpmh_delete_handle,
  EFI_rpmh_find_resource_command,
  EFI_rpmh_churn_all,
  EFI_rpmh_churn_single,
  EFI_rpmh_finish_amc,
  EFI_rpmh_finish_sleep_wake_amc,
  EFI_rpmh_issue_command_set,
  EFI_rpmh_invalidate_sleep_command,
  EFI_rpmh_invalidate_wake_command,
  EFI_rpmh_issue_read_command,
  EFI_rpmh_issue_command,
  EFI_rpmh_enter_sleep,
  EFI_rpmh_exit_sleep,
  EFI_rpmh_deinit,
  EFI_rpmh_barrier_all,
  EFI_rpmh_barrier_single,
};

/**
 * <!-- Exit Boot service CB -->
 *
 * @brief Call RPMH destructor
 *
 * @return       VOID
 */
VOID
EFIAPI
RpmhPlatformExitCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_rpmh_deinit();
  rpmh_exit();
}

/**
  Install Exit boot services callback
**/
static VOID RpmhRegisterEBSCallback(VOID)
{
  //Register to Exit Boot Service Event
  gBS->CreateEventEx (EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      RpmhPlatformExitCallback,
                      NULL,
                      &gEfiEventExitBootServicesGuid,
                      &EfiExitBootServicesEvent);
}

/**
 * <!-- EFI_rpmh_deinit -->
 *
 * @brief rpmh destructor 
 *
 * @return       EFI_SUCCESS
 */
STATIC EFI_STATUS EFI_rpmh_deinit(void)
{
  // Invoke RPMH client deinit function
  EFI_STATUS  Status = EFI_SUCCESS;
  //
  // Unpublish our protocols (Rpmhprotocol)
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                    gRpmhProtocolHandle,
                    &gEfiRpmhProtocolGuid,(void **)&RpmhProtocol,
                    NULL );
  return Status;
}

/**
 * Entry point for the Rpm DXE driver.
 */
EFI_STATUS EFIAPI RpmhDxeEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  status;

  /* Initialize DALSYS */
  DALSYS_InitMod(NULL);

  //Initialize RPM
  rpmh_client_init();
  //Install Exit Boot Service Callback
  RpmhRegisterEBSCallback();

  /* ********************************* */

   status = gBS->InstallMultipleProtocolInterfaces(
                    &gRpmhProtocolHandle,
                    &gEfiRpmhProtocolGuid,
                    (void **)&RpmhProtocol, NULL );

  return status;
}