#ifdef DIAG_UIMAGE_MODE
#ifndef MICRO_DIAGBUF_MPD_TYPE_H
#define MICRO_DIAGBUF_MPD_TYPE_H
/*==========================================================================

               Diagnostics Micro DiagBuffer Types

Description
  Type definition for the diagmpd buffer.

Copyright (c) 2017-2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved. Qualcomm Confidential and Proprietary 
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header:
      //components/rel/core.qdsp6/2.0/services/diag/LSM/qurt/src/diagbuf_mpd_type.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/19/18   sa      Changed UIMAGE_UPD_DIAGBUFFER_SIZE to 4k.
01/08/18   is      Add support for pd threshold buffering mode in island mode
10/31/17   nk      Qshrink 2 support for uimage diag messages
09/27/17   ph      Reduced the micro diagbuffer size for userPDs.
06/08/17   gn      Created file 
===========================================================================*/

#include "queue.h"
#include "osal.h"
#include "micro_diagbuffer.h"

#define DIAG_SYSTEM  0
#define DIAG_DIAGBUFFER  1
#define DIAG_ROOTPD  0
#define DIAG_USERPD  1

/*This buffer should always be power of 2*/
#ifndef UIMAGE_UPD_DIAGBUFFER_SIZE
#define UIMAGE_UPD_DIAGBUFFER_SIZE  4096
#endif

#define UIMAGE_UPD_DIAGBUFFER_SIZE_FINAL MICRO_ULOG_MEM_NEEDED(UIMAGE_UPD_DIAGBUFFER_SIZE)

#define DIAG_MAX_ISLAND_REGIONS 2

typedef struct
{
  uint8 mode;                       /* Stores the current mode (streaming, cir buffering, threshold buffering */
  boolean drain;                    /* True if draining; False otherwise */
  
  uint32 buffered_many_bytes_mark;  /* # of bytes used before we start draining */
  uint8 buffered_many_per_mark;     /* % of buffer used before we start draining */
  
  uint8 prev_mode;                  /* Stores the previous mode after a mode switch */
  
} diagbuffer_tx_mode_type;

typedef struct
{
  q_link_type link;                           // Link used so that the root PD can place this item in a linked list
  uint32 buffer_type;                         // Indicates system or diagbuffer
  uint32 buffer_location;                     // Indicates root or user pd
  micro_diagbuffer_handle handle;
  diagbuffer_tx_mode_type tx_mode;            // Stores streaming/buffering mode info for the buffer
  
  volatile boolean   is_in_use;                      // Indicates whether the buffer can be freed, is_in_use == TRUE means 
                                                     // it cannot be freed, is_in_use == FALSE means it can be freed
  volatile boolean   is_pd_killed;                   // Indicates that the PD is being killed/restarting, 
                                                     // is_pd_dying == FALSE means PD is running ok, no kill or 
                                                     // restart in progress												 
  osal_mutex_arg_t micro_diagbuf_mpd_buf_cs;
} micro_diagbuf_mpd_type;                                 // Type to encapsulate the micro_diagbuf used for multi-PD

/* Strucutre type to hold island_region and its virtual address for the memory allocated from QURTOS_ISLAND_POOL */
typedef struct 
{
	qurt_mem_region_t island_region;
	void *island_shared_mem;
	int client_handle;
	unsigned size;
}
diag_island_mem_type;

#endif
#endif  /* DIAGBUF_MPD_TYPE_H */
