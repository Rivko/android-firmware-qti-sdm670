/*------------------------------------------------------------------------------
   SMP2P_INTERNAL.H  - SMP2P Internal Header
------------------------------------------------------------------------------*/
/*!
  @file
    smp2p_internal.h

  @brief
    This file contains the SMP2P internal data structure and APIs.
*/

/*------------------------------------------------------------------------------
     Copyright  2012-2016, 2017 Qualcomm Technologies Incorporated.
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/smp2p/inc/smp2p_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/17   rv      Added smp2p_destroy() and smp2p_deregister() internal 
                   function to support PDR.
01/26/17   jlg     Moved some structure definitions to settings interface
08/10/16   vr      Added CDSP smem host in SMP2P_GET_SMEM_ID macro
04/07/15   db      'SMP2P_GET_SMEM_ID' macro modified for SMP2P
07/24/14   bc      Adding sensor processor support
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
02/12/14   bt      Make smp2pi_process_new_rx_entries static.
09/25/13   pa      Removing unused log events from smp2p_log_event_type.
09/16/13   pa/bt   Add RESET_LOCAL state and reset handshake.
06/26/13   pa      Added SMP2P_LOG_EVENT_NEGOTIATE.
03/28/13   rs      Moved generic queue functions to SMEM 
10/25/12   pa      Initial version
===========================================================================*/
#ifndef SMP2P_INTERNAL_H_
#define SMP2P_INTERNAL_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "smp2p.h"
#include "smp2p_os_common.h"
#include "smp2p_settings.h"
#include "smem_list.h"
#if !defined(SMP2P_TZ_IMAGE)
#include "smem_log.h"
#endif

/*===========================================================================
                        DEFINITIONS
===========================================================================*/

#define SMP2P_COPY_FROM_SMEM(dst, dst_size, src, copy_size) \
  smp2p_mem_copy(dst, dst_size, src, copy_size)

#define SMP2P_COPY_TO_SMEM(dst, dst_size, src, copy_size)   \
  smp2p_mem_copy(dst, dst_size, src, copy_size)

#define SMP2P_MEMSET_SMEM(a,b,c)     memset(a,b,c)

#define SMP2P_READ_SMEM_4(addr)       (*(uint32*)(addr))
#define SMP2P_READ_SMEM_2(addr)       (*(uint16*)(addr))
#define SMP2P_WRITE_SMEM_4(addr, val) ((*(uint32*)(addr)) = (uint32)(val))
#define SMP2P_WRITE_SMEM_2(addr, val) ((*(uint16*)(addr)) = (uint16)(val))

#define SMP2P_ENTRIES_MAX     1024
#define SMP2P_IDENTIFIER      0x504D5324

#define SMP2P_GET_SMEM_ID(source, dest) \
               ((SMEM_APPS  == (source)) ? SMEM_ID_SMP2P_BASE_APPS  + (dest): \
                (SMEM_MODEM == (source)) ? SMEM_ID_SMP2P_BASE_MODEM + (dest): \
                (SMEM_ADSP  == (source)) ? SMEM_ID_SMP2P_BASE_ADSP  + (dest): \
                (SMEM_WCN   == (source)) ? SMEM_ID_SMP2P_BASE_WCN   + (dest): \
                (SMEM_RPM   == (source)) ? SMEM_ID_SMP2P_BASE_RPM   + (dest): \
                (SMEM_SSC   == (source)) ? SMEM_ID_SMP2P_BASE_SSC   + (dest): \
                (SMEM_TZ   == (source)) ? SMEM_ID_SMP2P_BASE_TZ     + (dest): \
                (SMEM_CDSP == (source)) ? SMEM_ID_SMP2P_BASE_CDSP   + (dest): \
                0)

#define SMP2P_SMEM_ITEM_SIZE(entries) (sizeof(smp2p_header_type) + \
                                      (entries)*sizeof(smp2p_entry_type))

#define SMP2P_SMEM_LOG_EVENT_ID       (SMEM_LOG_DEBUG_EVENT_BASE + 0x10)

/** smp2p_header_type.flags Bit Definitions */
/* Bits required for SSR Acknowledgement handshake feature. */
#define SMP2P_FLAG_RESTART_DONE         0x00000001
#define SMP2P_FLAG_RESTART_DONE_SHIFT            0
#define SMP2P_FLAG_RESTART_ACK          0x00000002
#define SMP2P_FLAG_RESTART_ACK_SHIFT             1

/* SMP2P Host, this macro defines the local processor in which SMP2P runs */
#define SMP2P_THIS_HOST         (smp2p_proc_info.host)
#define SMP2P_LPBK_HOST         SMP2P_THIS_HOST

#define SMP2P_FFLAGS_SUPPORTED  (SMP2P_FFLAGS_SSR_HANDSHAKE)
#define SMP2P_FFLAGS_REQUESTED  (smp2p_proc_info.fflags)

/* Max entries */
#define SMP2P_ENTRIES_TOTAL     (smp2p_proc_info.max_entries)

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

typedef enum
{
  SMP2P_DIR_INVALID = 0,
  SMP2P_DIR_TX,
  SMP2P_DIR_RX
} smp2p_context_dir_type;

/** SMP2P Context Structure.
 * Contains info on the SMP2P entry.
 * The typedef for this struct is forward declared in smp2p.h */
struct smp2p_context
{
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type    link;

  /* Processor ID. This refers to the remote endpoint of the entry */
  smem_host_type          remote_id;

  /* Direction of the context */
  smp2p_context_dir_type  dir;

  /* Name to use for this entry */
  char                    name[SMP2P_NAME_STRING_LEN];

  /* Index of the entry. Will be set to a negative number if invalid. */
  int32                   index;

  /* Previous value of the entry data.  Used to detect updates to entry data.
   * Used by Rx entries only. */
  uint32                  data_shadow;

  /* OS params used to notify clients of activity on the entry */
  smp2p_os_params_type    os_params;
};

/** Defines the SMP2P states */
typedef enum
{
  /* Closed is the initial state */
  SMP2P_STATE_CLOSED = 0,

  /* Init indicates the connection is negotiating */
  SMP2P_STATE_INIT,

  /* Open indicates the connection is ready to use */
  SMP2P_STATE_OPEN,

  /* Local endpoint has reset, perform handshake */
  SMP2P_STATE_RESET_LOCAL,

} smp2p_state_type;

/** Defines the SMP2P entry */
typedef struct
{
  /* Name of the entry.  16 byte null terminated string */
  char    name[SMP2P_NAME_STRING_LEN];

  /* Data is 4 bytes as defined by the client */
  uint32  data;
} smp2p_entry_type;

/** Defines the processors involved in this connection */
typedef struct
{
  uint16 proc_local;
  uint16 proc_remote;
} smp2p_header_procs_type;

/** Defines the version and flags information for this connection. */
typedef struct
{
  uint32 version : 8;
  uint32 flags   : 24;
} smp2p_header_version_type;

/** Defines the entry counts for this connection */
typedef struct
{
  /* Total number of entries possible in this connection as determined by
   * the size of the SMEM item. */
  uint16 entries_total;

  /* Total number of entries in use. */
  uint16 entries_valid;
} smp2p_header_entry_count_type;

/** Defines the SMP2P header.  This structure resides at the base of each
 *  SMP2P SMEM item. */
typedef struct
{
  /* Identifier is a constant for use in debugging and identifying this struct
   * in a binary capture. */
  uint32                        identifier;

  /* Version field holds the version number and flags. */
  smp2p_header_version_type     version;

  /* Processor field holds the local and remote endpoint processor IDs. */
  smp2p_header_procs_type       procs;

  /* Entry count holds the number of total entries and the entries valid. */
  smp2p_header_entry_count_type entry_count;

  /* Handshake flags and reserved fields for other miscellaneous uses. */
  uint32                        flags;

  /* Placeholder for SMP2P entries, which follow immediately after the header. */
  smp2p_entry_type              entry[];
} smp2p_header_type;

/** Defines debug and profiling info for this connection.
 *  The data stored here is not necessary for operation of SMP2P. */
typedef struct
{
  /* Size of SMEM allocations for this connection */
  uint32  size_smem;

  /* Size of local allocations for this connection */
  uint32  size_local;

  /* The number of calls to write() or modify() for this connection */
  uint32  count_tx;

  /* The number of received data changes for this connection */
  uint32  count_rx;

  /* The number of received data changes for this connection for which
   * there was no registered client */
  uint32  count_rx_unregistered;

  /* The number of interrupts received from the endpoint and nothing appears
   * to have changed.  No new entries, and no new data. */
  uint32  count_rx_no_update;

  /* The number of times that this side of the connection had to advertise
   * a new version number or feature flags */
  uint32  count_neg_advertisements;
} smp2p_debug_info_type;

/** Defines the top-level info struct for one SMP2P connection. */
typedef struct
{
  /* Points to the outbound header in SMEM for this connection. */
  smp2p_header_type                   * header_tx;

  /* Points to the inbound header in SMEM for this connection. */
  smp2p_header_type                   * header_rx;

  /* Previous value of entries valid of the remote processor. */
  uint16                                remote_entries_valid_shadow;

  /* Total entries available in the remote endpoint's SMEM item as
   * agreed upon during negotiation. */
  uint16                                remote_entries_total;

  /* State of the connection/ */
  smp2p_state_type                      state;

  /* Profiling and debug info */
  smp2p_debug_info_type                 debug_info;

  /* Pointer to critical section structure */
  void                                * cs;
} smp2p_info_type;

/** Defines the SMP2P log event IDs */
typedef enum
{
  SMP2P_LOG_EVENT_NONE = 0,
  SMP2P_LOG_EVENT_ISR,
  SMP2P_LOG_EVENT_CREATE,
  SMP2P_LOG_EVENT_REGISTER,
  SMP2P_LOG_EVENT_MODIFY,
  SMP2P_LOG_EVENT_READ,
  SMP2P_LOG_EVENT_LB_SERVER_ENTRIES_CREATED,
  SMP2P_LOG_EVENT_LB_SERVER_TX,
  SMP2P_LOG_EVENT_LB_SERVER_RX,
  SMP2P_LOG_EVENT_LB_CLIENT_TX,
  SMP2P_LOG_EVENT_LB_CLIENT_RX,
  SMP2P_LOG_EVENT_RESERVED0,
  SMP2P_LOG_EVENT_RESERVED1,
  SMP2P_LOG_EVENT_RESERVED2,
  SMP2P_LOG_EVENT_NEGOTIATE,
} smp2p_log_event_type;

/** Negotiation events for use with SMP2P event log */
enum 
{
  SMP2P_NEGOTIATION_EVENT_INVALID,
  SMP2P_NEGOTIATION_EVENT_1,
  SMP2P_NEGOTIATION_EVENT_2,
  SMP2P_NEGOTIATION_EVENT_3,
  SMP2P_NEGOTIATION_EVENT_4,
  SMP2P_NEGOTIATION_EVENT_5,
  SMP2P_NEGOTIATION_EVENT_6,
  SMP2P_NEGOTIATION_EVENT_7
};

/*===========================================================================
                        EXTERNAL DATA
===========================================================================*/
extern smp2p_proc_info_type smp2p_proc_info;

/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smp2pi_isr
===========================================================================*/
/**
  Interrupt service routine for SMP2P.  Handles interrupts due to negotiation,
  new entries and received data.

  Negotiation interrupts will not trigger events to the client.
  New entries and new received data will trigger events to a client who is
  registered for that entry.

  @param [in]   source    Source processor ID who triggered the interrupt.

  @return
  None.

  @dependencies
  None.
*/
/*=========================================================================*/
void  smp2pi_isr(smem_host_type source);

/*===========================================================================
  FUNCTION  smp2pi_add_tx_entry
===========================================================================*/
/**
  This function initializes a new entry in the outbound SMEM item for the
  specified destination.  The name is copied to the entry and the top level
  entry count (entries_valid) is incremented.

  Very little param validation is done here.  Name should point to a valid 16
  byte null terminated string.  Ctx should be valid as well.

  @param [in]   ctx   Context for the entry to add.

  @return
  SMP2P_STATUS_DUPLICATE        - Name already exists in an entry
  SMP2P_STATUS_OUT_OF_RESOURCES - No more free entries
  SMP2P_STATUS_SUCCESS          - Entry added.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_add_tx_entry(smp2p_context_type * ctx);

/*===========================================================================
  FUNCTION  smp2pi_negotiate
===========================================================================*/
/**
  Performs the negotiation process of the SMP2P connection.

  If negotiation has not started, this function first looks for the SMP2P SMEM
  item for the specified destination processor. If the SMEM item exists, this
  function uses the remote processor version number and feature flags as the
  starting point for negotiation. If the SMEM item does not exist, this
  function advertises the local processor's version number and feature flags.

  If negotiation is in progress, this function will attempt to match the remote
  processor's version number and feature flags, or gear down to a version or
  feature set that both processors support.

  At each step, the remote processor is notified of the change with an SMP2P
  interrupt being triggered.

  When negotiation is complete, this function returns success. If any entries
  are in the Tx pending list, their clients are notified and the Tx pending
  list is emptied.

  @param [in]   dest   Destination processor ID.

  @return
  SMP2P_STATUS_SUCCESS  - Negotiation complete and connection is open.
  SMP2P_STATUS_NOT_INIT - Negotiation is in progress and is waiting for
                          response from remote endpoint.
  SMP2P_STATUS_FAILURE  - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_negotiate(smem_host_type dest);

/*===========================================================================
  FUNCTION  smp2pi_validate_context
===========================================================================*/
/**
  Validates that the context structure contents are within range of the
  valid SMP2P destination and entry counts.

  Note that this routine only performs bounds checking and cannot catch
  corruption or attacks which cause the context data to fall within valid range.

  @param [in]   ctx    Pointer to context to check.

  @return
  SMP2P_STATUS_INVALID_PARAM - Context contains invalid data
  SMP2P_STATUS_NOT_INIT      - Context contains valid port_id, but the SMP2P
                               connection is not yet complete.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_validate_context(smp2p_context_type * ctx);

/*===========================================================================
  FUNCTION  smp2pi_find_entry_index
===========================================================================*/
/**
  Scans the SMP2P entries for the specified name.

  @param [in]   header  Pointer to SMP2P header to scan.
  @param [in]   name    Name of the entry to search for.

  @return
  Returns the index of the entry with the given name.
  If not found, -1 is returned.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_find_entry_index(smp2p_header_type * header, char * name);

/*===========================================================================
  FUNCTION  smp2p_target_init
===========================================================================*/
/**
@brief
  This function initialize the processor info and interprocessor interrupts 
  for this target.

@param  None

@return None

@note   None
*/
/*=========================================================================*/
void smp2p_target_init(void);

/*===========================================================================
  FUNCTION  smp2p_destroy
===========================================================================*/
/**
  Remove the TX context from smp2p_tx_active_list/ smp2p_tx_pending_list and 
  reset the entry data to zero.

  @param [in]   ctx       Pointer to SMP2P context.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_destroy
(
  smp2p_context_type *ctx
);

/*===========================================================================
  FUNCTION  smp2p_deregister
===========================================================================*/
/**
  Remove the RX context from smp2p_rx_active_list/ smp2p_rx_pending_list.

  @param [in]   ctx       Pointer to SMP2P context.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_deregister
(
  smp2p_context_type *ctx
);

#endif /* SMP2P_INTERNAL_H_ */
