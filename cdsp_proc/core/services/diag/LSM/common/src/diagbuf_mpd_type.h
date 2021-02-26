#ifndef DIAGBUF_MPD_TYPE_H
#define DIAGBUF_MPD_TYPE_H
/*==========================================================================

               Diagnostics Buffer

Description
  Type definition for the diagmpd buffer.

Copyright (c) 2014-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved. Qualcomm Confidential and Proprietary 
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header:
      //components/dev/core.adsp/2.6/vinayk.CORE.ADSP.2.6.multiPD_optimization_Sep19/services/diag/LSM/qurt/src/diagbuf_mpd_type.h#1
      $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out. 
11/03/16   sa      Implement seperate queue for UserPD events and enabled buffering
05/12/16   sa      Circular Mode for UserPD buffer
12/11/15   as      Changes for PD restart 
12/04/14   vk      Created file 
===========================================================================*/

#include "queue.h"
#include "osal.h"

typedef struct
{
  q_link_type        link;                           // Link used so that the root PD can place this item in a linked list

  /* PD identifier group */                                        
  int32              pid;                            // The process ID associated with the diag buf 

  /* Variable directly affecting mpd buffer group */
  uint8             *diagbuf_mpd_buf;                // The allocated memory for the buffer. 
  int32              diagbuf_mpd_head;               // The head of the ring buffer
  int32              diagbuf_mpd_tail;               // The tail of the ring buffer_used
  uint32             diagbuf_size;                   // Size of diagbuf pointed by diagbuf_mpd_buf
  osal_mutex_arg_t   diagbuf_mpd_buf_cs;             // The mutex associated with the diagbuf.  Used for updating the head
  volatile boolean   is_in_use;                      // Indicates whether the buffer can be freed, is_in_use == TRUE means 
                                                     // it cannot be freed, is_in_use == FALSE means it can be freed
  volatile boolean   is_pd_killed;                   // Indicates that the PD is being killed/restarting, 
                                                     // is_pd_dying == FALSE means PD is running ok, no kill or 
                                                     // restart in progress

  /* mpd buffer drain algorithm driver group */
  uint32             diagbuf_mpd_commit_size;        // Number of bytes currently in the buffer
  uint32             diagbuf_mpd_commit_threshold;   // Commit threshold in bytes. This is a % of diagbuf_size member
  uint32             diagbuf_mpd_drain_threshold;    // Drain threshold in bytes. This is a  % of diagbuf_size member
  osal_timer_t       diagbuf_mpd_drain_timer;        // PD Drain timer
  osal_timer_cb_type diagbuf_mpd_timer_cb;           // Timer callback function 

  /* mpd buffer Statistic group */
  uint32             msg_alloc_count;                // Msg allocation counter
  uint32             msg_drop_count;                 // Number of msgs that could not be allocated because the buffer is full
  uint32             log_alloc_count;                // Log allocation counter
  uint32             log_drop_count;                 // Number of logs that could not be allocated because the buffer is full                                        
  uint32             mpd_head_tail_reset_count;      //Number of times the head and tail was reset when the buffer was in circular mode
  uint32		         event_alloc_count;				//Moved the event alloc and drop variable here since this way userpd can reply to health command.
  uint32		         event_drop_count;               // These event alloc/drop variables will be updated from rootpd while reset from userpd.
  uint8              diagbuf_mpd_mode;               //Variable to check if buffer is in circular or streaming mode
  uint8              diagID;                         // Variable to store diagID.

} diagbuf_mpd_type;                                 // Type to encapsulate the diagbuf used for multi-PD

/*=========================================================================== 
FUNCTION DIAGBUF_PD_DELETE

DESCRIPTION 
  This function deletes a PD from PD linked list. It calls
  DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST()
 
PARAMETERS 
 diagbuf_mpd_type *mpd_buf_to_remove - the UserPD diagbuf
                                       pointer.
 
RETURN VALUE 
  TRUE  - If the PD has  been successfully deleted the PD node
  FALSE - If PD is a core PD, it cannot be deleted. Or if
          q_delete_ext returned FALSE i.e. it could not find the
          item in queue Or the PD is not being deleted.
===========================================================================*/
boolean diagbuf_pd_delete( diagbuf_mpd_type *mpd_buf_to_remove );


/*=========================================================================== 
FUNCTION EVENT_PD_DELETE

DESCRIPTION 
  This function deletes a PD from diag_main_q PD link list. It calls
  q_delete to remove the PD from the diag_main_q
 
PARAMETERS 
 diagbuf_mpd_type *mpd_buf_to_remove - the UserPD diagbuf pointer.
 
RETURN VALUE 
  TRUE  - PD has  been successfully deleted the PD node
  FALSE - PD was not deleted.
===========================================================================*/
boolean event_pd_delete( diagbuf_mpd_type *mpd_buf_to_remove );


#endif  /* DIAGBUF_MPD_TYPE_H */
