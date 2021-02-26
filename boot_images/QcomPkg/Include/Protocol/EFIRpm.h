/** 
  @file  EFIRpm.h
  @brief RPM API.
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
#ifndef __EFIRPM_H__
#define __EFIRPM_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/rpmclient.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_rpm_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_RPM_PROTOCOL_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_rpm_constants */

/*  Protocol GUID definition */
/** @ingroup efi_rpm_protocol */
#define EFI_RPM_PROTOCOL_GUID \
  { 0x1bbb8f8e, 0x4a4e, 0x44a0, { 0xac, 0xf7, 0x35, 0xfd, 0x84, 0x9f, 0xd9, 0x0 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the RPM API Protocol GUID.
 */
extern EFI_GUID gEfiRpmProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

/* EFI_KVP_DESTROY */
/** @ingroup efi_kvp_destroy
  @par Summary
  Release kvp memory.

  @param[in]  self           Pointer to kvp

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_KVP_DESTROY)(kvp_t *self);

/* EFI_KVP_PUT */
/** @ingroup efi_kvp_put
  @par Summary
  Add new value to kvp.

  @param[in]  self           Pointer to kvp
  @param[in]  type           Value type
  @param[in]  length         Value length
  @param[in]  value          Pointer to value

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_KVP_PUT)
(
  kvp_t *self, 
  unsigned type, 
  unsigned length, 
  const void *value
 );

/* EFI_KVP_CLEAR */
/** @ingroup efi_kvp_clear
  @par Summary
  Zeroing kvp buffer and reset indexes

  @param[in]  self           Pointer to kvp

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_KVP_CLEAR)(kvp_t *self);

/* EFI_KVP_DESTROY */
/** @ingroup efi_kvp_destroy
  @par Summary
  Release kvp memory.

  @param[in]  self           Pointer to kvp

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */

/* EFI_KVP_CREATE */
/** @ingroup efi_kvp_create
  @par Summary
  Create kvp.

  @param[in]  buffer_size    Buffer size
  @param[out] retval         Pointer to kvp

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_KVP_CREATE)
(
  unsigned buffer_size,
  kvp_t **retval
 );

/* EFI_RPM_POST_REQUEST */
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
typedef EFI_STATUS (EFIAPI* EFI_RPM_POST_REQUEST)
(
  rpm_set_type set,
  rpm_resource_type resource,
  unsigned id,
  kvp_t *kvps,
  unsigned *pMsg_id
 );

/* EFI_RPM_CHURN_QUEUE */
/** @ingroup efi_rpm_churn_queue
  @par Summary
  Polls the RPM message queue until ACK for specified message received.
  @param[in]  message_id

  @return
  EFI_SUCCESS -- Function completed successfully. \n
 */
typedef EFI_STATUS (EFIAPI* EFI_RPM_CHURN_QUEUE)
(
  unsigned message_id
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_rpm_protocol
  @par Summary
  Rpm Api Protocol interface.

  @par Parameters
*/
typedef struct _EFI_RPM_PROTOCOL {
  UINT64                Revision;
  EFI_KVP_DESTROY       KvpDestroy;
  EFI_KVP_PUT           KvpPut;
  EFI_KVP_CLEAR         KvpClear;
  EFI_KVP_CREATE        KvpCreate;
  EFI_RPM_POST_REQUEST  RpmPostRequest;
  EFI_RPM_CHURN_QUEUE   RpmChurnQueue;
  
 }EFI_RPM_PROTOCOL;

#endif /* __EFIRPM_H__ */
