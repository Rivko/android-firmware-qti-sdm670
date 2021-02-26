#ifndef SMD_MAIN_H
#define SMD_MAIN_H

/*===========================================================================

                    Shared Memory Driver Main Routine Header File


 Copyright (c) 2005-2008, 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/12   bm      Replace smd_host_type with smem_host_type
01/05/12   hwu     Decouple SMD from SMEM.
07/25/11   rs      Added support for SSR Phase 2
01/06/11   bt      Removed SMD Block and Data Mover protocols.
09/27/10   tl/ih   Added SMD_CMD_PROCESS_PORT to process a specific port
06/08/10   tl      Split SMD task into multiple tasks with different priorities
04/21/10   tl      Implemented SMD Lite API
03/17/08   bfc     Implemented block port close.
02/21/08   bfc     First SMD Block port check in.
02/16/08   bfc     Added a new command for block ports.
10/08/07   hwu     Added is_loopback flag for loopback port.
09/12/07   bfc     smd_main now only processes one event, so the arguement
                   was removed, and the processing combined.
05/17/07   hwu     Added the SMD loopback feature.
12/14/06   hwu     Added new command type for remote processor reset.
                   Added new API smd_set_stream_closed() so startup can
                   forcefully close the SMD streams.
11/14/06   taw     Remove argument to smd_init().
11/03/06   ptm     Add events argument to smd_main.
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/05/06   ptm     Initial version - based on smd_task.h#3.
===========================================================================*/

#include "smd_internal.h"
#include "smem_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
extern boolean smd_sm_remote_reset;

/*-------------------------------------------------------------------------
 *        Define command data structures
 *-------------------------------------------------------------------------*/
typedef enum
{
  SMD_CMD_OPEN_MEMCPY,
  SMD_CMD_CLOSE,
  SMD_CMD_FLUSH,
  SMD_CMD_IOCTL,
  SMD_CMD_REMOTE_RESET,
  SMD_CMD_REMOTE_RESET_DONE,
  SMD_CMD_LOOPBACK,
  SMD_CMD_OPEN_SMDLITE,
  SMD_CMD_PROCESS_PORT,
} smd_cmd_e_type;

typedef struct
{
  smd_port_id_type port_id;
  boolean          is_loopback;
} smd_cmd_open_memcpy_type;

typedef struct
{
  smd_port_id_type      port_id;
  smd_close_callback_fn close_cb;
  void                  *cb_data;
  boolean               flush;
} smd_cmd_close_type;

typedef struct
{
  smd_port_id_type      port_id;
  smd_flush_callback_fn flush_cb;
  void                  *cb_data;
} smd_cmd_flush_type;

typedef struct
{
  smd_port_id_type     port_id;
  smd_ioctl_cmd_type   cmd;
  smd_ioctl_param_type param;
} smd_cmd_ioctl_type;

typedef struct
{
  smd_port_id_type      port_id;
} smd_cmd_process_port_type;

typedef struct smd_cmd_struct
{
  smd_cmd_e_type cmd;
  union
  {
    smd_cmd_open_memcpy_type open_memcpy;
    smd_cmd_close_type       close;
    smd_cmd_flush_type       flush;
    smd_cmd_ioctl_type       ioctl;
    smd_cmd_process_port_type process_port;
  } data;
} smd_cmd_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_init

DESCRIPTION   Initialize the SMD. Calls init functions for various subsystems,
              and initializes the command queue.

RETURN VALUE  None
===========================================================================*/
void smd_init( void );

/*===========================================================================
FUNCTION      smd_cmd

DESCRIPTION   This routine copies the given cmd buffer into a command queue
              structure, puts the command on the SMD command queue and sets
              the command queue signal for the SMD task.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd( smd_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION      smd_cmd_context

DESCRIPTION   Sends a command to the particular SMD task context.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

              context - pointer to the SMD context

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd_context( smd_cmd_type *cmd_ptr, smd_context_type *context);

/*===========================================================================
FUNCTION      smd_cmd_broadcast
===========================================================================*/
/**
@brief        Send an SMD command to all SMD tasks

              This function iterates through the list of SMD tasks and calls
              smd_cmd_context() to send the command to each task.

              This function is implemented by the OS-specific code, which
              alone has visibility into the true number of tasks and which
              ports they correspond to.

@param[in]    cmd_ptr    Pointer of the command buffer to send

@return       None
*/
/*=========================================================================*/
void smd_cmd_broadcast( smd_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION      smd_main

DESCRIPTION   Shared Memory Driver's main program.

              The task synchronizes itself with corresponding task on the
              other processor using SMSM.

              When a share memory port is opened, it synchronizes the opening
              with the other processor and is then responsible for sending
              and receiving data and control signals.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None
===========================================================================*/
void smd_main( smd_context_type * context );

/*===========================================================================
FUNCTION      smd_sm_enter_start

DESCRIPTION   State machine function for entering the START state. Clears
              both of the SMD SMSM state bits, registers the SMSM callback
              initializes the appropriate half of the shared data structures
              and sets the SMD WAIT state in SMSM.

RETURN VALUE  None

SIDE EFFECTS  Sets SMSM SMD WAIT state bit and interrupts the other processor.
===========================================================================*/
void smd_sm_enter_start( void );

/*===========================================================================
FUNCTION      smd_set_stream_closed

DESCRIPTION   This routine forces to transition all the open stream
              state to SMD_SS_CLOSED immediately. 

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS  provided for quick clean up upon errors.
===========================================================================*/
void smd_set_stream_closed( smd_context_type * context );

/*===========================================================================
FUNCTION      smd_handle_cmd

DESCRIPTION   Dispatch commands queued for the smd task to their handlers

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_handle_cmd( smd_context_type * context );

/*===========================================================================
  FUNCTION  smd_handle_ssr_close
===========================================================================*/
/**
  This function is in charge of resetting SMD ports for the specified host.
 
  As a response to the SSR reset command handler, this function takes as
  argument the specified procesor that is going into reset. It then locates
  the SMD ports (from the open port list, or directly from shared memory),
  and transitions the ports into CLOSED from the spefified host perspective.

  @param[in] host           SMD host that is going into reset.
 
  @return
  None.
 
  @dependencies
  The SMD task should not be allowed to operate when SSR in in progress
  (required for Phase 2, since SSR events is synchronous).
 
  @sideeffects
  Ports associated with the host are forceibly set to CLOSED.

*/
/*=========================================================================*/
void smd_handle_ssr_close( smem_host_type host );

#ifdef __cplusplus
}
#endif

#endif /* SMD_MAIN_H */
