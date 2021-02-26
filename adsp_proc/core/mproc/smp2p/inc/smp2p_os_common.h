/*
 * @file smp2p_os.h
 *
 * Common OS specific routine declarations for SMP2P. 
 */

/*==============================================================================
     Copyright (c) 2012-2016 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/smp2p/inc/smp2p_os_common.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/15   rv      Added smp2p_os_timetick_init() prototype.
08/19/13   pa      Added params flag to smp2p_os_log
10/10/12   pa      Initial revision.
===========================================================================*/
#ifndef SMP2P_OS_COMMON_H_
#define SMP2P_OS_COMMON_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem.h"
#include "smp2p_os.h"

/*===========================================================================
                       TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smp2p_mem_copy
===========================================================================*/
/**
  Copies bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond the size of 
  the destination buffer.

  The result of calling this on overlapping source and destination
  buffers is undefined.
  
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.

  @return
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smp2p_mem_copy
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
);

/*===========================================================================
  FUNCTION  smp2p_mem_move
===========================================================================*/
/**
  Moves bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond 
  the size of the destination buffer. 

  This function should be used in preference to memscpy() if there
  is the possiblity of source and destination buffers overlapping.
  The result of the operation is defined to be as if the copy were from
  the source to a temporary buffer that overlaps neither source nor
  destination, followed by a copy from that temporary buffer to the
  destination.
   
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.
   
  @return 
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smp2p_mem_move
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
);

/*===========================================================================
  FUNCTION  smp2p_os_init_event
===========================================================================*/
/**
  Initializes the OS specific event object.

  @param [in]   os_params   Pointer to the OS specific params struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_init_event(smp2p_os_params_type * os_params);

/*===========================================================================
  FUNCTION  smp2p_os_deinit_event
===========================================================================*/
/**
  De-initialize the OS specific event object.

  @param [in]   os_params   Pointer to the OS specific params struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_deinit_event(smp2p_os_params_type * os_params);

/*===========================================================================
  FUNCTION  smp2p_os_send_event
===========================================================================*/
/**
  Triggers the OS specific event object.

  @param [in]   os_params   Pointer to the OS specific params struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_send_event(smp2p_os_params_type * os_params);

/*===========================================================================
  FUNCTION  smp2p_os_wait
===========================================================================*/
/**
  Blocks on the OS specific event object found in the context.

  @param [in]   ctx   Pointer to the SMP2P context struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_wait(smp2p_context_type * ctx);

/*===========================================================================
  FUNCTION  smp2p_os_wait_multiple
===========================================================================*/
/**
  Blocks on the OS specific events object found in an array of contexts.

  @param [in]   ctx_arr     Array of one or more pointers to SMP2P contexts
                            to wait on.
  @param [in]   ctx_count   Number of valid contexts in ctx array
  @param [out]  trigger_idx Index of context which was triggered

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_wait_multiple
(
  smp2p_context_type ** ctx_arr,
  uint32                ctx_count,
  uint32              * trigger_idx
);

/*===========================================================================
  FUNCTION  smp2p_os_register_interrupt
===========================================================================*/
/**
  This function registers SMP2P interrupts.

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_register_interrupt(void);

/*===========================================================================
  FUNCTION  smp2p_os_send_interrupt
===========================================================================*/
/**
  This function sends out the interprocessor interrupts.

  @param[in] dest Destination processor to interrupt

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_send_interrupt(smem_host_type dest);

/*===========================================================================
  FUNCTION  smp2p_os_cs_create
===========================================================================*/
/**
  Creates the critical section used to acquire access to SMP2P from different
  tasks.

  @return
  Pointer to critical section object

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void * smp2p_os_cs_create(void);

/*===========================================================================
  FUNCTION  smp2p_os_cs_lock
===========================================================================*/
/**
  Locks the critical section used by SMP2P.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks can lock access to SMP2P.

  @param[in] cs Pointer to the critical section object

  @return
  None

  @dependencies
  None.

  @sideeffects
  Prevents context switch.
*/
/*==========================================================================*/
void smp2p_os_cs_lock(void * cs);

/*===========================================================================
  FUNCTION  smp2p_os_cs_unlock
===========================================================================*/
/**
  Releases the critical section used by SMP2P.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks can unlock access to SMP2P.

  @param[in] cs Pointer to the critical section object

  @return
  None

  @dependencies
  None.

  @sideeffects
  Allows context switch.
*/
/*==========================================================================*/
void smp2p_os_cs_unlock(void * cs);

/*===========================================================================
  FUNCTION  smp2p_os_timetick_init
===========================================================================*/
/**
  Creates the handle for the OS-specific timetick handle, if necessary.

  @return
  SMEM_ERROR on failure
  SMEM_SUCCESS on success
*/
/*=========================================================================*/
int32 smp2p_os_timetick_init(void);

/*===========================================================================
  FUNCTION  smp2p_os_timetick_get
===========================================================================*/
/**
  Returns the current timetick value.

  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*=========================================================================*/
uint32 smp2p_os_timetick_get(void);

/*===========================================================================
  FUNCTION  smp2p_os_log
===========================================================================*/
/**
  This function writes an event entry into the SMP2P log.

  If SMP2P_LOG_LOCAL is defined, this function writes entries to a local
  circular buffer.  If SMP2P_LOG_SMEM is defined, this function writes
  entries to the SMEM log.  These options are not mutually exclusive.

  @param[in]  event_id  An event identifier for this event
                        For example, an event may be receiving data, or
                        creating a new entry.
  @param[in]  remote_id Identifier of the remote endpoint.  If not applicable,
                        set to -1.
  @param[in]  data1     First data word.  Usually specific to the event_id.
  @param[in]  data2     Second data word.  Usually specific to the event_id.
  @param[in]  flags     Flags param. 
                        SMP2P_FLAGS_STM prevents this function from locking as 
                        well as from logging to SMEM log (which required 
                        interprocessor lock).

  @return
  None.
*/
/*=========================================================================*/
void smp2p_os_log
(
  uint32 event_id,
  int32  remote_id,
  uint32 data1,
  uint32 data2,
  uint32 flags
);

/*===========================================================================
  FUNCTION  smp2p_os_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c smp2p_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void * smp2p_os_malloc(uint32 size);

/*===========================================================================
  FUNCTION  smp2p_os_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c smp2p_os_malloc.

  @param[in] pMem    A reference to the region of memory to be freed.

  @return    0 if succesful.
             Negative value if an error occurred.
*/
/*=========================================================================*/
int32  smp2p_os_free(void * pMem);

/*===========================================================================
  FUNCTION  smp2p_os_mem_check
===========================================================================*/
/**
  Check memory canaries of dynamic memory.  Only used in test builds.

  @param[in] pMem    A reference to the region of memory to be freed.

  @return
  None
*/
/*=========================================================================*/
void   smp2p_os_mem_check(void * pMem);

/*===========================================================================
  FUNCTION  smp2p_os_start_loopback_tasks
===========================================================================*/
/**
  This function initializes and starts the loopback tasks for server and client.

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_start_loopback_tasks(void);

#endif /* SMP2P_OS_COMMON_H_ */
