/*
 * @file smp2p.h
 *
 * Public API and data structures for use with SMP2P.
 */

/*==============================================================================
     Copyright (c) 2012-2016 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/mproc/smp2p.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/16   rv      Added smp2p_read_ex, to support single threaded mode.
08/06/13   pa      Added smp2p_modify_ex and SMP2P_FLAGS_*.
10/10/12   pa      Initial revision.
===========================================================================*/

#ifndef SMP2P_H_
#define SMP2P_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "smem.h"

/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/

/** SMP2P status/return codes */
#define SMP2P_STATUS_SUCCESS            0
#define SMP2P_STATUS_FAILURE           -1
#define SMP2P_STATUS_INVALID_PARAM     -2
#define SMP2P_STATUS_NOT_INIT          -3
#define SMP2P_STATUS_OUT_OF_RESOURCES  -4
#define SMP2P_STATUS_DUPLICATE         -5
#define SMP2P_STATUS_NOT_FOUND         -6

/** SMP2P name string length in bytes */
#define SMP2P_NAME_STRING_LEN           16

/** SMP2P flags */
#define SMP2P_FLAGS_NONE      0
#define SMP2P_FLAGS_NO_INTR   1 /**< Do not send an interrupt */
#define SMP2P_FLAGS_STM       2 /**< Single threaded mode, no locks are taken */

/** SMP2P Context Structure.
 * Contains info on the SMP2P entry.
 * This is a forward declaration. Struct definition is in internal header file.
 */
typedef struct smp2p_context smp2p_context_type;

/*===========================================================================
FUNCTION      smp2p_init
===========================================================================*/
/**
  Initializes the SMP2P internal structures.

  @return
  None.

  @dependencies
  SMEM is initialized.
*/
/*=========================================================================*/
void smp2p_init( void );

/*===========================================================================
  FUNCTION  smp2p_create
===========================================================================*/
/**
  Creates one SMP2P 32 bit entry on behalf of the client to the specified
  destination.

  Clients may call this routine multiple times to create multiple entries.

  If the SMP2P connection has not been opened, that process will be initiated
  and this function will return a status code indicating that initialization
  is in progress.

  The client may call smp2p_write when either of the following two has occurred:
    - This function returns success.
    - The client has called the wait API.

  @param [out]  ctx       Pointer to pointer of SMP2P context.
                          Context will be created and this pointer will be made
                          to point to it.
  @param [in]   dest      Destination processor ID.
  @param [in]   name      16 byte name string.
                          Must be unique for outbound entries to the specified
                          endpoint.

  @return
  SMP2P_STATUS_SUCCESS       - Entry is created and ready to accept write data.
  SMP2P_STATUS_NOT_INIT      - Connection is opening. Client must call wait API
                               before writing data.
  SMP2P_STATUS_DUPLICATE     - Duplicate name found.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.
  Other non-success status   - Error occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_create
(
  smp2p_context_type ** ctx,
  smem_host_type        dest,
  char *                name
);

/*===========================================================================
  FUNCTION  smp2p_write
===========================================================================*/
/**
  Writes data to the SMP2P entry and notifies remote endpoint. Entry location is
  determined by contents of context parameter.

  @param [in]   ctx       Pointer to SMP2P context.
  @param [in]   data      Data to be written.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_write
(
  smp2p_context_type * ctx,
  uint32               data
);

/*===========================================================================
  FUNCTION  smp2p_modify
===========================================================================*/
/**
  Performs read-modify-write bits in the entry specified, then notifies remote
  endpoint.  Clear mask is applied before set mask.

  @param[in]  context   Pointer to SMP2P context.
  @param[in]  clr_mask  Bit mask to use when clearing bits.
  @param[in]  set_mask  Bit mask to use when setting bits.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_modify
(
  smp2p_context_type * ctx,
  uint32               clr_mask,
  uint32               set_mask
);

/*===========================================================================
  FUNCTION  smp2p_modify_ex
===========================================================================*/
/**
  Performs read-modify-write bits in the entry specified, then notifies remote
  endpoint.  Clear mask is applied before set mask.

  @param[in]  context   Pointer to SMP2P context.
  @param[in]  clr_mask  Bit mask to use when clearing bits.
  @param[in]  set_mask  Bit mask to use when setting bits.
  @param[in]  flags     Flags parameter.
                        SMP2P_FLAGS_NO_INTR disables sending an interrupt 
                          following the SMP2P entry update.
                        SMP2P_FLAGS_NONE is normal behavior.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_modify_ex
(
  smp2p_context_type * ctx,
  uint32               clr_mask,
  uint32               set_mask,
  uint32               flags
);

/*===========================================================================
  FUNCTION  smp2p_register
===========================================================================*/
/**
  Registers an OS event to be triggered when data arrives on the specified
  entry.

  If the specified entry is not yet created by the remote endpoint,
  SMP2P will save the params and trigger the specified OS event upon
  creation of the entry.

  If the specified entry is already created by the remote endpoint,
  SMP2P will trigger the event immediately to inform the client to call
  smp2p_read() to read the data there.

  Clients should call the wait API to wait for an event.

  @param [out]  ctx       Pointer to pointer of SMP2P context.
                          Context will be created and this pointer will be made
                          to point to it.
  @param [in]   source    Source processor ID.
  @param [in]   name      16 byte name string of the entry.

  @return
  SMP2P_STATUS_SUCCESS          - Operation successful.
  SMP2P_STATUS_NOT_FOUND        - Entry not found in Rx data.
                                  Client may call wait API to be notified when
                                  the Tx side to creates the entry.
  SMP2P_STATUS_INVALID_PARAM    - Invalid parameter.
  SMP2P_STATUS_DUPLICATE        - Duplicate name already present.
  SMP2P_STATUS_OUT_OF_RESOURCES - Out of room in pending list.

  @dependencies
  none.
*/
/*=========================================================================*/
int32 smp2p_register
(
  smp2p_context_type ** ctx,
  smem_host_type        source,
  char *                name
);

/*===========================================================================
  FUNCTION  smp2p_read
===========================================================================*/
/**
  Reads data from an entry. Entry location is determined by contents of context
  parameter.

  Client should call this routine after SMP2P has triggered a receive event
  as registered by smp2p_register().

  Clients may call this routine at any time to read the current data value
  of the entry.

  @param [in]   ctx       Pointer to SMP2P context.
  @param [out]  data      Data read by SMP2P.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_read
(
  smp2p_context_type * ctx,
  uint32 *             data
);

/*===========================================================================
  FUNCTION  smp2p_read_ex
===========================================================================*/
/**
  Reads data from an entry. Entry location is determined by contents of context
  parameter.

  Client should call this routine after SMP2P has triggered a receive event
  as registered by smp2p_register().

  Clients may call this routine at any time to read the current data value
  of the entry.

  @param [in]   ctx     Pointer to SMP2P context.
  @param [out]  data    Data read by SMP2P.
  @param [in]   flags   Flags parameter.
                        SMP2P_FLAGS_STM single threaded mode, no locks are taken
                        SMP2P_FLAGS_NONE is normal behavior.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_read_ex
(
  smp2p_context_type * ctx,
  uint32 *             data,
  uint32               flags
);

/*===========================================================================
  FUNCTION  smp2p_wait
===========================================================================*/
/**
  Calls OS specific wait function to wait for an event specified in the context.

  For a transmit/outbound context, the following scenarios may trigger an event:
    - A call to smp2p_create() returned status indicating that the
      connection must be negotiated.  SMP2P will trigger the event when
      the connection is open and ready to use.

  For a receive/inbound context, the following scenarios may trigger an event:
    - An entry has new data available to read.
    - A call to smp2p_register() has found the requested entry.
      Client should read the data value after this event since data may be
      updated.
    - A call to smp2p_register() returned status indicating that the entry
      is not found.  SMP2P will trigger the event when the Tx side notifies
      the local side that the entry is created.
      Client should read the data value after this event since data may be
      updated.

  This function will sleep (ie. not return) until the event triggers.

  @param [in]   ctx       Pointer to SMP2P context.

  @return
  SMP2P_STATUS_SUCCESS  - Entry is created and ready to accept write data.
  SMP2P_STATUS_FAILURE  - Error occurred.

  @dependencies
  Context must initialized by a call to SMP2P.
*/
/*=========================================================================*/
int32 smp2p_wait
(
  smp2p_context_type * ctx
);

/*===========================================================================
  FUNCTION  smp2p_wait_multiple
===========================================================================*/
/**
  Calls OS specific wait function to wait for an event specified in the context.
  This function waits for multiple events and returns when any one of them
  has been triggered.

  This function will sleep (ie. not return) until the event triggers.

  @param [in]   ctx_arr     Array of one or more pointers to SMP2P contexts
                            to wait on.
  @param [in]   ctx_count   Number of valid contexts in ctx array
  @param [out]  trigger_idx Index of context which was triggered

  @return
  SMP2P_STATUS_SUCCESS  - Entry is created and ready to accept write data.
  SMP2P_STATUS_FAILURE  - Error occurred.

  @dependencies
  Context must initialized by a call to SMP2P.
*/
/*=========================================================================*/
int32 smp2p_wait_multiple
(
  smp2p_context_type ** ctx_arr,
  uint32                ctx_count,
  uint32              * trigger_idx
);

#endif /* SMP2P_H_ */
