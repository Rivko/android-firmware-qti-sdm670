#ifndef EVENTI_H
#define EVENTI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Event Services internal header file

General Description
  Internal declarations to support diag event service.

Initializing and Sequencing Requirements 
  None

Copyright (c) 2007-2014,2016-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
06/05/17   nk      Added debug support for uimage in root PD
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
04/01/14   rs      Added support for build mask centralization. 
11/22/13   sr      Removed PACK from pointer references 
02/11/13   sr      Mainling Diag central routing  
11/09/12   is      Support for preset masks
08/31/12   sr      Support for mask retrieval command  
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
02/01/10   sg      Dual Mask Changes
09/28/10   mad     Added event_ctrl_stale_timer and event_ctrl_report_size
08/10/10   sg      Added function event_set_all_rt_masks
08/25/10   sg      Fixed compiler warnings
02/20/10   sg      Moved event_q_alloc and event_q_put here
01/10/08   mad     Added copyright and file description.
12/5/07    as      Created

===========================================================================*/

#include "comdef.h"
#include "event_defs.h"
#include "queue.h"
#include "diagdiag_v.h"    /* for event_id_type */
#include "diagcomm_io.h"   /* For diagcomm_port_type */
#include "memheap.h"
#include "diagbuf_mpd_type.h"


/* Size of the event mask array, which is derived from the maximum number
 of events.
*/
#define EVENT_MASK_SIZE (EVENT_LAST_ID/8 + 1)


/* Dual Mask Defines */
#define DIAG_EXT_EVENT_REPORT_GET_MASK 1
#define DIAG_EXT_EVENT_REPORT_SET_MASK 2
#define DIAG_EXT_EVENT_REPORT_SET_ALL 3


/* NOTE: diag_event_type and event_store_type purposely use the same
   format, except that event_store type is preceeded by a Q link
   and the event ID field has a union for internal packet formatting.
   If either types are changed, the service will not function properly. */
typedef struct
{
  q_link_type qlink;            /* To be accessed by event_q_xxx() only */

  union
  {
    unsigned int id;
    event_id_type event_id_field;
    uint16 sized_field;         /* Used to copy the event ID structure */
  }
  event_id;

  qword ts;                     /* always store full time stamp */

  event_payload_type payload;
}
event_store_type;

typedef struct 
{
  /* The length field indicates the approximate packet size of the event
     report packet  It is calculated based on the assumption of truncated
     time stamps, since most are truncated (except the first). */
  unsigned int length;
  byte version; /* version of the extended event pkt; currently we support 0 and 1 */

  q_type q;
}event_q_type;

/*This is the structure which contains all the variables needed to alloc and drain events 
**Userpd and Rootpd have their own structure.*/
typedef struct 
{
  uint32 drop_cnt;
  qword drop_ts;
  char *diag_event_heap_buf;
  mem_heap_type diag_event_heap;
  event_store_type* event_rpt_last;
  event_q_type     event_q;
  uint32 *event_alloc_count_p;
  uint32 *event_drop_count_p;
  #ifdef FEATURE_DEBUG_DIAG_HEAP
  uint32 diag_event_malloc_success_cnt;
  uint32 diag_event_free_cnt;
  uint32 event_drop_handler_malloc_cnt;
  uint32 event_drop_handler_free_cnt;
  uint32 event_q_alloc_malloc_cnt;
  uint32 event_drain_free_cnt;
  uint32 event_q_get_cnt;
  #endif
}event_struct;

/*This structure is used to encapsulate the diagmpd_buf and event_struct ptrs.
**Can be later expanded to add more pointers. DIAG_MPD_Q contains element of this type*/
typedef struct
{
  q_link_type  link;
  diagbuf_mpd_type *mpd_buf_ptr;
  event_struct * event_mpd_struct_ptr;
}diag_mpd_ptr_struct;


/*===========================================================================
FUNCTION    find_event_pd

DESCRIPTION
  This function is used to find an element in diag_main_q. 
  This function is compare_func for q_linear_search which is used in EVENT_Q_SEARCH
  If this compare function returns "1", q_linear_search returns a pointer for the element.

DEPENDENCIES
  None.

RETURN VALUE
  1 - PD found
  0 - PD not found
  
SIDE EFFECTS
  None

===========================================================================*/
int find_event_pd(void* ptr, void * val);

/*===========================================================================
MACRO    EVENT_Q_SEARCH

DESCRIPTION
  This function is used to find an element in diag_main_q. 
  It uses q_linear_seach to find the element.

DEPENDENCIES
  fin_event_pd fn is used as a compare function.

RETURN VALUE
  Returns ptr to the element in the queue.
  
SIDE EFFECTS
  None

===========================================================================*/

#define EVENT_Q_SEARCH(pid) (\
{ \
  diag_mpd_ptr_struct * rv=NULL;\
  rv= (diag_mpd_ptr_struct *)q_linear_search(&diag_main_q,find_event_pd,&pid);\
  (rv);\
})

/* ==================================================================
FUNCTION DIAG_EVENT_HEAP_INIT

DESCRIPTION
  Initializes the heap and heap structure which is used later to allocate events for the PD.

PARAMETERS
  ptr - void ptr of type event_struct which contains heap variables.
  heap_size - size of heap to be used.
  
RETURN
  None
===================================================================== */

void
diag_event_heap_init (event_struct * ptr, int heap_size);


/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes from the heap specified.
    
PARAMETERS
   num_bytes - Number of bytes to be malloced
   ptr       - event_struct type pointer for the PD. 
               This should contain informationm about the diag event heap to be used.

DEPENDENCIES
   diag_event_heap structure should be setup by calling diag_event_heap_init.


RETURN
   Returns a void pointer to chunk of memory malloced.
===================================================================== */
  void *diag_event_malloc (unsigned long num_bytes,event_struct *ptr);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE

DESCRIPTION
   Frees a chunk of memory allocated using diag_event_malloc.


PARAMETERS
   mem_ptr - ptr to the block to be freed
   ptr     - event_struct type pointer for the PD. 

DEPENDENCIES
   diag_event_heap structure should be setup by calling diag_event_heap_init.
   

RETURN
   None

===================================================================== */
  void diag_event_free (void *mem_ptr, event_struct * ptr);


void event_q_put(event_store_type * event,byte stream_id, event_struct * eventq);

event_store_type * event_q_alloc (event_id_enum_type id, uint8 payload_length,byte stream_id, diag_mpd_ptr_struct* ptr);

void event_control (uint8 config, uint8 config_mask);

void event_control_sec (uint8 config, uint8 config_mask);

void event_set_all_rt_masks( uint8 config, byte stream_id);

void * event_pkt_get_local_masks (void * req_pkt, uint16 pkt_len);

void event_ctrl_stale_timer(unsigned int timer_len);
void event_ctrl_report_size(unsigned int report_size, unsigned int send_max_num, unsigned int rpt_pkt_len_siz);

event_store_type *event_q_get (event_struct *ptr);
void diag_switch_event_preset_mask( void );

/* Sends last event id supported to Master Diag */
void diag_ctrl_send_event_id_ctrl_pkt(void);

boolean event_mask_enabled ( const unsigned char *mask_ptr, event_id_enum_type event_id);



#endif /* EVENTI_H */
