/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file defines vairables and functions that would be used in both normal
  mode and island mode.

Copyright (c) 2015,2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/diag_micro.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   gn      Added support for diag services to uImage
11/09/15   ph      Optimizations in rt maks for uimage.
08/13/15   xy      Created
===========================================================================*/
#include "diaglogi.h"           /*For LOG_MASK_SIZE*/ 
#include "event.h"             /*For EVENT_MASK_SIZE*/
#include "micro_diagbuffer.h"  /*For micro_diagbuffer_handle*/
#include "diagtune.h"          /*For LOG_EQUIP_ID_LAST*/
#include "eventi.h"
#include "msg_arrays_i.h"      /*For msg_mask_tbl etc */
#include "msg.h"               /*For Message typedefs and prototypes */
#include "msgi.h"              /*For DIAG_MSG_NONE*/
#include "msgcfg.h"            /*For MSG_MASK_TBL_CNT */
#include "micro_diagbuf_mpd_type.h"

boolean diag_services_initialized = FALSE;

byte log_composite_mask[LOG_MASK_SIZE];
byte log_listener_mask[LOG_MASK_SIZE];
log_state_enum_type log_state = LOG_INIT_STATE;
uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];

boolean log_mask_enabled (const byte *xx_mask_ptr, unsigned int xx_id,
                                 unsigned int xx_item);
extern const uint16 log_last_item_tbl[]; /* externed from log_arrays_i.c */

unsigned char diag_event_composite_mask[EVENT_MASK_SIZE]; 

uint8 msg_enabled_mask  = DIAG_MSG_NONE; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */
boolean msg_service_init = FALSE;

#ifdef DIAG_UIMAGE_MODE
micro_diagbuf_mpd_type *micro_diagmpd_buf = NULL;
#endif

#ifdef DIAG_UIMAGE_MODE_ROOT
char uimage_buffer[UIMAGE_DIAGBUFFER_SIZE_FINAL];
micro_diagbuf_mpd_type root_micro_diagbuf;
#endif

/*===========================================================================

FUNCTION LOG_MASK_ENABLED

DESCRIPTION
  This function returns a boolean indicating if the specified code is enabled.

  The equipment ID and item code are passed in to avoid duplicating the
  calculation within this call.  It has to be done for most routines that call
  this anyways.

===========================================================================*/
boolean log_mask_enabled (
  const byte *mask_ptr,
  unsigned int id,
  unsigned int item
)
{
  unsigned int offset, index, mask;
  boolean enabled = FALSE;

  if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id]) {

    offset = log_mask_offset_tbl[id];

    index = LOG_MASK_ARRAY_INDEX(item);

    mask = LOG_MASK_BIT_MASK(item);

    if (mask_ptr[offset + index] & mask) {
      enabled = TRUE;
    }
  } else {
    /* It is expected that this will be compiled out in a target build */
    //MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid code given 0x%x", (id << 12) | item);
  }
  return enabled;
} /* log_mask_enabled */

/*===========================================================================
FUNCTION EVENT_MASK_ENABLED

DESCRIPTION
  This function returns a boolean indicating if the specified event id is enabled.
  The event id is passed as an argument to the function for which the
  event masks needs to be checked.

===========================================================================*/
boolean event_mask_enabled (
  const unsigned char *mask_ptr,
  event_id_enum_type event_id
)
{
  unsigned int mask;
  boolean enabled = FALSE;
  if (event_id <= EVENT_LAST_ID)
  {
    mask = ( 1 << (event_id & 0x07) );
    if(mask_ptr[event_id/8] & mask)
    {
      enabled  = TRUE;
    }
  }
  else
  {
   // MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Invalid event id given %d", event_id);
  }
  return enabled;
} /* event_mask_enabled */


