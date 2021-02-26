/** 
  @file  EFIRpmh.h
  @brief RPMH API.
*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
07/23/15   rli     Initial revision.

=============================================================================*/
#ifndef __EFIRPMH_H__
#define __EFIRPMH_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/rpmh_client.h>
#include <Library/rpmh_common.h>
#include <Library/rpmh_image_config.h>
#include <Library/rpmh_resource_commands.h>
#include <Library/rpmh_uefi.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_rpm_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_RPMH_PROTOCOL_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_rpmh_constants */

/*  Protocol GUID definition */
/** @ingroup efi_rpm_protocol */
#define EFI_RPMH_PROTOCOL_GUID \
  { 0xe1d0b573, 0xf29f, 0x495d, { 0x93, 0x52, 0xca, 0x0d, 0xd4, 0x2a, 0x0a, 0x74 } }

  


/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the RPMH API Protocol GUID.
 */
extern EFI_GUID gEfiRpmhProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

/* EFI_RPMH_INIT */
/** @ingroup efi_rpmh_init
  @par Summary
  Initializes the RPMH subsystem.
  to this function.

  @return
  EFI_SUCCESS -- Function completed successfully.
 */
typedef EFI_STATUS (EFIAPI* EFI_RPMH_INIT)(void);


/* EFI_RPMH_POST_REQUEST */
/** @ingroup efi_rpm_post_request
  @par Summary
  RPM Request Service API
  Post a request to the *local* request state.  This will eventually synchronize with the RPM copy.
  @param[in]  set        The RPM "set" which receives this request, affecting what scenarios it applies to.
  @param[in]  resource   The class of resource being requested (different resource classes accept different data).
  @param[in]  id         The particular resource being addressed (i.e., if 'resource' tells us we're talking to a clock, this identifies *which* clock).
  @param[in]  kvps       The data being sent in this request; the data is overlaid on previous kvps, so incremental updates are acceptable.
  @param[out] pMsg_id    Pointer to unique identifier by which this request can be identified later.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
 */
typedef EFI_STATUS (EFIAPI* EFI_RPMH_IS_DRV_ID_VALID)
(
  RSCSW_DRV_MAPPING drv_id,
  boolean *drv_id_valid
 );

/* EFI_RPMH_CHURN_QUEUE */
/** @ingroup efi_rpm_churn_queue
  @par Summary
  Polls the RPM message queue until ACK for specified message received.
  @param[in]  message_id

  @return
  EFI_SUCCESS -- Function completed successfully. \n
 */
typedef EFI_STATUS (EFIAPI* EFI_RPMH_TCS_NUM_CMDS)
(
  RSCSW_DRV_MAPPING drv_id,
  uint32 *drv_id_size
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CREATE_HANDLE_INTERNAL)
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  boolean explicit_client,
  rpmh_client_handle *client_handle
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CREATE_HANDLE_EXPLICIT)
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  rpmh_client_handle *client_handle
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CREATE_HANDLE)
(
  RSCSW_DRV_MAPPING drv_id,
  const char *client_name,
  rpmh_client_handle *client_handle
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_INVALIDATE_EXPLICIT_CMDS)
(
  rpmh_client_handle client_handle
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CLIENT_REGISTER_COMPLETION_CALLBACK)
(
  rpmh_client_handle client_handle,
  rpmh_completion_cb callback
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_DELETE_HANDLE)
(
 rpmh_client_handle client_handle
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_FIND_RESOURCE_COMMAND)
(
  uint32 address,
  rpmh_resource_command_t **resource_command
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CHURN_ALL)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_CHURN_SINGLE)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_BARRIER_ALL)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_BARRIER_SINGLE)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_WAIT_FOR_READ)
(
  struct rpmh_client_t *handle,
  uint32 req_id,
  boolean polling,
  uint32 *data
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_FINISH_AMC)
(
  RSCSW_DRV_MAPPING drv_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_FINISH_SLEEP_WAKE_AMC)
(
  RSCSW_DRV_MAPPING drv_id,
  rpmh_set_enum set
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_ISSUE_COMMAND_SET)
(
  rpmh_client_handle client_handle,
  rpmh_command_set_t *cmd_set,
  uint32 *req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_INVALIDATE_SLEEP_COMMAND)
(
  rpmh_client_handle client_handle,
  uint32 address
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_INVALIDATE_WAKE_COMMAND)
(
  rpmh_client_handle client_handle,
  uint32 address
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_ISSUE_READ_COMMAND)
(
  rpmh_client_handle client_handle,
  uint32 address,
  boolean polling,
  uint32 *data
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_ISSUE_COMMAND)
(
  rpmh_client_handle client_handle,
  rpmh_set_enum set,
  boolean completion,
  uint32 address,
  uint32 data,
  uint32 *req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_ENTER_SLEEP)
(
  RSCSW_DRV_MAPPING drv_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_EXIT_SLEEP)
(
  RSCSW_DRV_MAPPING drv_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_BARRIER_ALL)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

typedef EFI_STATUS (EFIAPI* EFI_RPMH_BARRIER_SINGLE)
(
  rpmh_client_handle client_handle,
  uint32 req_id
);

/* EFI_RPMH_DEINIT */
/** @ingroup efi_rpmh_deinit
  @par Summary
  RPMH destructor.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_RPMH_DEINIT)(void);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_rpm_protocol
  @par Summary
  Rpm Api Protocol interface.

  @par Parameters
*/
typedef struct _EFI_RPMH_PROTOCOL {
  UINT64                           Revision;
  EFI_RPMH_INIT                    RpmhInit;
  EFI_RPMH_IS_DRV_ID_VALID             rpmh_is_drv_id_valid;
  EFI_RPMH_TCS_NUM_CMDS                rpmh_tcs_num_cmds;
  EFI_RPMH_CREATE_HANDLE_INTERNAL      rpmh_create_handle_internal;
  EFI_RPMH_CREATE_HANDLE_EXPLICIT      rpmh_create_handle_explicit;
  EFI_RPMH_CREATE_HANDLE               rpmh_create_handle;
  EFI_RPMH_INVALIDATE_EXPLICIT_CMDS    rpmh_invalidate_explicit_cmds;
  EFI_RPMH_CLIENT_REGISTER_COMPLETION_CALLBACK        rpmh_client_register_completion_callback;
  EFI_RPMH_DELETE_HANDLE               rpmh_delete_handle;
  EFI_RPMH_FIND_RESOURCE_COMMAND       rpmh_find_resource_command;
  EFI_RPMH_CHURN_ALL                   rpmh_churn_all;
  EFI_RPMH_CHURN_SINGLE                rpmh_churn_single;
  EFI_RPMH_FINISH_AMC                  rpmh_finish_amc;
  EFI_RPMH_FINISH_SLEEP_WAKE_AMC       rpmh_finish_sleep_wake_amc;
  EFI_RPMH_ISSUE_COMMAND_SET           rpmh_issue_command_set;
  EFI_RPMH_INVALIDATE_SLEEP_COMMAND    rpmh_invalidate_sleep_command;
  EFI_RPMH_INVALIDATE_WAKE_COMMAND     rpmh_invalidate_wake_command;
  EFI_RPMH_ISSUE_READ_COMMAND          rpmh_issue_read_command;
  EFI_RPMH_ISSUE_COMMAND               rpmh_issue_command;
  EFI_RPMH_ENTER_SLEEP                 rpmh_enter_sleep;
  EFI_RPMH_EXIT_SLEEP                  rpmh_exit_sleep;
  EFI_RPMH_DEINIT                      RpmhDeInit;
  EFI_RPMH_BARRIER_ALL                 rpmh_barrier_all;
  EFI_RPMH_BARRIER_SINGLE              rpmh_barrier_single;
  
 }EFI_RPMH_PROTOCOL;

#endif /* __EFIRPMH_H__ */
