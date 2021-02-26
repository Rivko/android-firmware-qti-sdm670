
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service for uimage mode.
  These APIs will be called by clients when they want to generate a new event. 
  The APIs will check to see if uImage mode is active or not when they are called.
  If active, the API will write the event packet into the uImage diag_buffer using 
  the diag_buffer interface. A dedicated diag_buffer will be set aside for uImage logging.
  This buffer will act as a circular buffer and will not be drained while in uImage mode.
  Upon exiting uImage mode, Diag task will drain out the uImage diag_buffer as part of 
  it’s regular draining process.




Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2015, 2017-2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                              Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_event_api.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/18   is      Add support for pd threshold buffering mode in island mode
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
08/12/15   xy      Verified DIAG uimage APIs in island mode 
06/15/15   xy      Added new DIAG APIs for uimage mode 
05/12/15   xy      Added support for DIAG uImage APIs 
05/12/15   xy      Created file.
===========================================================================*/
#include "eventi.h" /*for EVENT_MASK_SIZE*/
#include "diag_v.h" /*for DIAGBUF_VERSION_1*/
#include "diagdiag_v.h" /*for MAX_VALUE_UINT32 */
#include "msgcfg.h"
#include "micro_msg_diag_service.h"
#include "micro_diagbuffer.h"

#if defined(DIAG_UIMAGE_MODE)
#include "micro_diagbuf_mpd_type.h"
#include "diag_micro_i.h"               /* For micro_diagbuffer_drain_check() */

extern micro_diagbuf_mpd_type* micro_diagmpd_buf;
#endif

uint32 micro_alloc_count_event = 0;

extern unsigned char diag_event_composite_mask[EVENT_MASK_SIZE];

extern void event_report_core(event_id_enum_type event_id);
extern void event_report_payload_core(event_id_enum_type event_id, uint8 length, void *data);
boolean event_report_ext(event_id_enum_type event_id, unsigned int version, void *parameter);
boolean event_report_payload_ext(event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter);



/*===========================================================================
FUNCTION EVENT_REPORT

DESCRIPTION
   Report an event without payload data . This function can take additional
   parameters - version and void * to specify information regarding where
   the event is being generated.
 
PARAMETERS
   event_id       ID of the event to be reported.Event IDs are defined in event_defs.h
 
RETURN VALUE 
  TRUE: The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
void event_report(event_id_enum_type event_id)
{
  event_report_ext(event_id,0,NULL);
}


/*===========================================================================
FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
   Report an event with payload data .Payload structure format must be published
 
PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.
 
RETURN VALUE 
  TRUE:  The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
void
event_report_payload(event_id_enum_type event_id, uint8 length, void *payload)
{
   event_report_payload_ext(event_id,length,payload,0,NULL);
}

/*===========================================================================
FUNCTION EVENT_REPORT_EXT

DESCRIPTION
   Report an event without payload data . This function can take additional
   parameters - version and void * to specify information regarding where
   the event is being generated.
 
PARAMETERS
   event_id       ID of the event to be reported.Event IDs are defined in event_defs.h
   version        Specifies the version - currently, there are two versions 
                  0 - same as event_report()if not running in uImage mode. In uImage mode,
                      the buffering/queueing behavior is slightly different
                  1 - The outgoing event report header will have support to
                      include ID field to differentiate the events coming
                      from different instances of the processor.
                      same as event_report_ext()if not running in uImage mode. In uImage mode,
                      the buffering/queueing behavior is slightly different
  *parameter      Based on the version passed in, this pointer is interpreted
                  as follows-
                  version       pointer
                  0             NULL
                  1             uint32 * . The uint32 value this pointer points to,
                                is the ID of the instance generating this event.
 
RETURN VALUE 
  TRUE: The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean event_report_ext(event_id_enum_type event_id, unsigned int version, void *parameter)
{
#if defined(DIAG_UIMAGE_MODE)
  micro_diagbuffer_result ret_value = MICRO_DIAGBUFFER_ERROR;
  boolean status = FALSE;
#endif
 
  boolean ret = FALSE;
  
#if defined(DIAG_UIMAGE_MODE)
  if (((version == DIAGBUF_VERSION_1) && (parameter == NULL)) || (version > DIAGBUF_VERSION_1))
  {
    return FALSE;
  }

  /*check if it is in micro image mode*/
  if (diag_island_mode_check())
  {
    /*check if the event is enabled or not*/
    status = event_mask_enabled(diag_event_composite_mask, event_id);   
    if (status) 
    {
      /* allocate the event in diag_buffer*/
	  if(micro_diagmpd_buf)
	  {
		ret_value = MICRO_DIAGBUFFER_EVENT( micro_diagmpd_buf->handle, event_id, version, (void*)parameter );  
	  }
	  else
	  {
		ret_value = MICRO_DIAGBUFFER_ERR_INITINCOMPLETE;
	  }
	  ret = MICRO_DIAGBUFFER_GET_RESULT(ret_value);
      if( ret == TRUE )
	  {
	     if ( micro_alloc_count_event == MAX_VALUE_UINT32 )
         {
           MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_event has reached maximum value.");
         }
         else
         {
           micro_alloc_count_event++;
         }
         
         /* After successful allocation, check if we've reached threshold for draining. */
         micro_diagbuffer_drain_check();
      }
    }
  }
  else
#endif
  {
    /*if not in micro image mode, call regular API*/
	/*To Do: Map to extended api once extended support is ported */
    event_report_core(event_id);
	ret=TRUE;
  }
  return ret ;
  
} /* event_report_ext */

/*===========================================================================
FUNCTION MICRO_EVENT_REPORT_PAYLOAD_EXT

DESCRIPTION
   Report an event with payload data .Payload structure format must be published
 
PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.
   version    Specifies the version - currently, there are two versions 
              0 - same as event_report_payload()if not running in uImage mode. In uImage mode,
                  the buffering/queueing behavior is slightly different
              1 - The outgoing event report header will have support to
                  include ID field to differentiate the events coming
                  from different instances of the processor.
                  same as event_report_payload_ext()if not running in uImage mode. In uImage mode,
                  the buffering/queueing behavior is slightly different
   
   *parameter   Based on the version passed in, this pointer is interpreted
                as follows-
                version       pointer
                 0             NULL
                 1             uint32 * . The uint32 value this pointer points to,
                               is the ID of the instance generating this event.
 
RETURN VALUE 
  TRUE:  The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean event_report_payload_ext(event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter)
{
#if defined(DIAG_UIMAGE_MODE)
  micro_diagbuffer_result ret_value = MICRO_DIAGBUFFER_ERROR;
  boolean status = FALSE;
#endif
  boolean ret = FALSE;

  if (((version == DIAGBUF_VERSION_1) && (parameter == NULL)) || (version > DIAGBUF_VERSION_1))
  {
    return FALSE;
  }
  if (length <= 0 || payload == NULL)
  {
    return FALSE;
  }

#if defined(DIAG_UIMAGE_MODE)
  /*check if it is in micro image mode*/
  if (diag_island_mode_check())
  {
    /*check if the event is enabled or not*/
    status = event_mask_enabled(diag_event_composite_mask, event_id);   
    if (status) 
    {
      /* allocate the event in diag_buffer */
	  if(micro_diagmpd_buf)
	  {
		ret_value =  MICRO_DIAGBUFFER_EVENT_PAYLOAD( micro_diagmpd_buf->handle, event_id, 0, NULL, length, payload);
	  }
	  else
	  {
		ret_value = MICRO_DIAGBUFFER_ERR_INITINCOMPLETE;
	  }	  
	  ret = MICRO_DIAGBUFFER_GET_RESULT(ret_value);

      if( ret == TRUE )
	  {
	     if ( micro_alloc_count_event == MAX_VALUE_UINT32 )
         {
           MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_event has reached maximum value.");
         }
         else
         {
           micro_alloc_count_event++;
         }
         
         /* After successful allocation, check if we've reached threshold for draining. */
         micro_diagbuffer_drain_check();
      }
    }
  }
  else
#endif
  {
    /*if not in micro image mode, call regular API*/
	/*To Do: Map to extended api once extended support is ported */
    event_report_payload_core(event_id, length, payload);
	ret=TRUE;
  }

  return ret;
  
} /* event_report_payload_ext */

