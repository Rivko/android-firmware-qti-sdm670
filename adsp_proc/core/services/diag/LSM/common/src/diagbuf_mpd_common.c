/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Subsystem Buffer Allocation Manager

General Description
  This file contains routines that manage the diag output buffer.
  This allocation scheme is modeled as a circular buffer. Currently, 
  diag output buffer is used for streamed data (LOG, MSG).

Initialization and Sequencing Requirements diagbuf_mpd_init() must be 
called before using this service.

Copyright (c) 2014-2017 by QUALCOMM Technologies, Incorporated. 
All Rights Reserved. Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
04/24/17   sp      Added support for extended logs in userPD
06/21/16   as      Made diagbuf_mpd_init() mutex name generic
05/12/16   sa      Circular Mode for UserPD buffer
11/11/15   as      Changes for PD restart 
12/12/14   vk      Created file. To fix 8996 compilation issue. 
===========================================================================*/


#include "customer.h"

#include "comdef.h"
#include "diag_v.h" /* not needed */
#include "diagbuf_v.h" /* not needed */
#include "diagbuf_mpd.h"
#include "diagcomm_v.h"
#include "diagdiag_v.h" // For diag_cmd_ext_v1_type
#include "diagi_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "diag_qdi.h"
#include "Diag_LSM.h"

#include "err.h"
#include "stringl.h"

#include <stdio.h>
#include "diag_diagIDi.h"
#include "osal.h"
#include "diag_cfg.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"

/*===========================================================================

FUNCTION DIAGBUF_MPD_INIT

DESCRIPTION
   This function allocates memory of type diagbuf_mpd_type, 
   initializes it and returns pointer to this location to the
   UserPD. UserPD uses this pointer to access it's diagbuf which
   is allocated in diagbuf_mpd_buf_init() and pointer to it, is
   saved in diagbuf_mpd_type->diagbuf_mpd_buf. It is assumed
   that this function is operating at the privileged level to
   make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle - that is passed to diag_qdi_invoke
   int pid           - PID obtained in diag_lsm_init using qurt_getpid fn.
   
RETURN VALUE
   diagbuf_mpd_type if qurt_qdi_user_malloc is successful
   NULL otherwise
   
===========================================================================*/
diagbuf_mpd_type* diagbuf_mpd_init( int client_handle, int pid)
{
  diagbuf_mpd_type *rv = NULL;
  int return_val = 0;

  //Initialize the shared memory between root PD and user PD
  rv = (diagbuf_mpd_type *)qurt_qdi_user_malloc(client_handle, sizeof(diagbuf_mpd_type));

  if ( rv == NULL )
  {
    return (rv);
  }
  
  //initialize all members of diagbuf_mpd_type
  rv->diagbuf_mpd_head             = 0;
  rv->diagbuf_mpd_tail             = 0; 
  rv->diagbuf_size                 = 0; 
  rv->msg_alloc_count              = 0; 
  rv->msg_drop_count               = 0;
  rv->log_alloc_count              = 0; 
  rv->log_drop_count               = 0;
  rv->mpd_head_tail_reset_count    = 0;
  rv->event_alloc_count            = 0;
  rv->event_drop_count             = 0;
  
  //Initialize all thresholds to 0
  rv->diagbuf_mpd_commit_threshold = 0; 
  rv->diagbuf_mpd_drain_threshold  = 0; 
  rv->diagbuf_mpd_commit_size      = 0;
  
  //Set mode to streaming
  rv->diagbuf_mpd_mode             = DIAG_PD_BUF_MODE_STREAMING;

  
  //Set diagID to unknown
  rv->diagID= DIAGID_UNKNOWN_ID;

  //Set the process ID of the diagmpd buf
  rv->pid = pid;

  //Initialize diagbuf mutex
  rv->diagbuf_mpd_buf_cs.name = "MUTEX_DIAGBUF_MPD_CS";
  return_val = osal_init_mutex(&rv->diagbuf_mpd_buf_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  return (rv);
} /* diagbuf_mpd_init */

/*===========================================================================

FUNCTION DIAGBUF_MPD_BUF_INIT

DESCRIPTION
   This function allocates and initializes a diagbuf to be used
   by UserPD. It copies the allocated pointer to the UserPD's
   object passed in param *ptr. This allows UserPD to access
   this pointer for writing diag traffic to this buffer which is
   "it's diagbuf" with shared access to Guest OS. This function
   returns allocated pointer to the caller.
   It is assumed that this function is operating at the
   privileged level to make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   diagbuf_mpd_type *ptr - pointer to UserPD object
   int buf_len           - length of the diagbuf requested by
                           a UserPD
   
RETURN VALUE
   uint8* pointer to buffer allocated
   NULL otherwise
   
===========================================================================*/
uint8* diagbuf_mpd_buf_init( int client_handle, diagbuf_mpd_type *ptr, int buf_len )
{
  uint8 *rv = NULL;

  if ( ptr )
  {
    /*Allocate the data for the diagbuf portion of the structure */
    rv = qurt_qdi_user_malloc(client_handle, buf_len);


    if ( rv != NULL )
    {
      /* Copy the pointer value to the user PD object passed in */
      if ( qurt_qdi_copy_to_user(client_handle, &(ptr->diagbuf_mpd_buf), &rv, sizeof(rv)) < 0 )
      {
        qurt_qdi_user_free(client_handle, rv);
        ptr->diagbuf_size = 0;
        rv = NULL;
      }
      else
      {
        diagbuf_header_type *pkt_hdr; /* pointer to the first header */

        /* 
           Setting the first status field to DIAGBUF_UNCOMMIT_S is all
           the initizalization needed
        */
        pkt_hdr = ((diagbuf_header_type *)&rv[0]);
        pkt_hdr->status = DIAGBUF_UNUSED_S;
        pkt_hdr->length = buf_len;
        ptr->diagbuf_size                 = buf_len;
        ptr->diagbuf_mpd_commit_threshold = (ptr->diagbuf_size * DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER) / 100;
        ptr->diagbuf_mpd_drain_threshold  = (ptr->diagbuf_size * DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER) / 100; 
        ptr->diagbuf_mpd_drain_timer      = DIAG_MULTIPD_DRAIN_TIMER; 
        ptr->is_in_use                    = FALSE;  /* Marking that the buffer can be freeded if needed */
        ptr->is_pd_killed                 = FALSE;  /* Indicating that PD is up and running, no kill or 
                                                       restart in progress */ 
      }
    }
  }
  return (rv);
} /* diagbuf_mpd_buf_init */

/*===========================================================================

FUNCTION DIAGBUF_MPD_DEINIT

DESCRIPTION
   This function cleans up the diag allocation buffer at
   startup.
 
PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   diagbuf_mpd_type *ptr - pointer to UserPD object
   
RETURN VALUE
   None
 
===========================================================================*/
void diagbuf_mpd_deinit( int client_handle, diagbuf_mpd_type *buf )
{
  int return_val;

  //Delete the mutex that was previously used
  if ( buf )
  {
    return_val = osal_delete_mutex(&buf->diagbuf_mpd_buf_cs);
    ASSERT(OSAL_SUCCESS == return_val);

    if ( buf->diagbuf_mpd_buf )
    {
      qurt_qdi_user_free(client_handle, buf->diagbuf_mpd_buf);
    }
    qurt_qdi_user_free(client_handle, buf);
  }
} /* diagbuf_mpd_deinit */

extern osal_mutex_arg_t diagmpd_buf_list_state_mutex;
/*===========================================================================
FUNCTION DIAGBUF_MPD_CLEANUP

DESCRIPTION
  Updates in_use field in the client's diagbuf, indicating that
  the buffer is not in use. 
 
PARAMETERS 
   diag_qdi_opener *diag_qdi_obj_ptr  - pointer to diag qdi structure
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_cleanup(diag_qdi_opener *diag_qdi_obj_ptr)
{
  if ( diag_qdi_obj_ptr )
  {
    diagbuf_mpd_type *mpd_buf = diag_qdi_obj_ptr->user_pd_node_ptr;
    if ( mpd_buf ) 
    {
      boolean buf_in_use;
      /* Mark that PD is going down */
      mpd_buf->is_pd_killed = TRUE; 

      /* Now we need to wait until the PD's diagbuf is no longer in use,
         in case the diag task is in middle of draining a packet.
      */ 
      do
      {
        //TODO: What we really need here is a condition variable.
        osal_lock_mutex(&diagmpd_buf_list_state_mutex);
        buf_in_use = mpd_buf->is_in_use;
        osal_unlock_mutex(&diagmpd_buf_list_state_mutex);
  }
      while ( buf_in_use ); 

	  // Code to free event struct memory and also remove it from queue.
	  event_pd_delete(mpd_buf);
	  /* Code to free diagmpd_buf struct and remove it from queue.
	  ** This should be freed after freeing event_struct for thr PD since we use the PID field from diagmpd_buf*/
      diagbuf_pd_delete(mpd_buf); 

      /* if we have come here, drain task has drain the PD diagbuf
         to a packet boundary, and have removed it from its linked
         list. Now it is safe to free the memory. 
  */
      diagbuf_mpd_deinit(diag_qdi_obj_ptr->qdi_handle, diag_qdi_obj_ptr->user_pd_node_ptr); 
      diag_qdi_obj_ptr->user_pd_node_ptr = NULL;
    }
  }
} /* diagbuf_mpd_cleanup */
