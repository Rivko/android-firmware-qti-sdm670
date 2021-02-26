/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Legacy Service Mapping layer implementation for Events

GENERAL DESCRIPTION
  Contains main implementation of Legacy Service Mapping layer for Diagnostic
  Event Services.

EXTERNALIZED FUNCTIONS
  event_report
  event_report_payload

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2012, 2014-2015, 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM_Event.c#2 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
09/29/17   sa     Send IOCTL for mask update after diagID is set.
06/08/17   gn     Added support for diag services to uImage
05/03/17   sp     Resolved KW errors
04/24/17   sp     Added support for extended logs in userPD
06/18/15   sa     Added support for time sync feature.
07/06/14   ph     Added support for Diag over STM
07/05/12   sg     Changes to bringup Diag MultiPD
08/25/10   sg     Fixed compiler warnings
04/12/10   sg     Merging both DCM rtos and WM files
06/22/09   mad    Removed inclusion of diagdiag.h and diagpkti.h
01/19/09   mad    Removed inclusion of diagtune.h, updated copyright
09/30/08   mad    Changes to take out Component services, and use
                  filesystem calls to access windiag driver.
05/01/08   JV     Added support to update the copy of event_mask in this process
                  during initialization and also on mask change
11/12/07   mad    Created

===========================================================================*/


/* ==========================================================================
   Include Files
========================================================================== */

#include "comdef.h"
#include "event.h"
#include "Diag_LSMi.h" /* for declaration of windiag Handle */
#include "DiagSvc_Malloc.h"
#include "event_defs.h"
#include "queue.h"
#include "eventi.h"
#include "Diag_LSM_Event_i.h"
#include "diag_shared_i.h" /* for definition of diag_data struct. */
#include "diagstub.h"
#include "Diag_LSM_Qdi_wrapper.h"
#include "Diag_LSM_stm.h"
#include "stringl.h"
#include "diag_cfg.h"

/*this keeps track of number of failures to WriteFile().
This will currently be used only internally.*/
static unsigned int gEvent_commit_fail = 0;

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get_LSM)(qword time);

/*Local Function declarations*/
static byte *event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len);

/* The event mask. */
static unsigned char event_mask[EVENT_MASK_SIZE];

#define EVENT_MASK_BIT_SET(id) \
  (event_mask[(id)/8] & (1 << ((id) & 0x07)))

extern unsigned char diag_event_composite_mask[EVENT_MASK_SIZE];

#if defined FEATURE_DIAG_STM
/*===========================================================================

FUNCTION EVENT_GENERATE_STM

DESCRIPTION
  Generate STM event report for event being generated, and send immediately

PARAMETERS
  event_id      ID of the event to be generated
  length        Length of the payload pointed to by 'payload'
  *payload      Pointer to the event payload

=============================================================================*/
static void event_generate_stm (event_id_enum_type event_id, uint8 length, void *payload)
{
  event_rpt_type stm_event_hdr;
  uint8 *event_pkt, *ptr;
  event_id_type stm_event_id;
  uint32 pkt_length;
  uint32 copy_bytes;
  uint32 hdr_size = sizeof(event_rpt_type)-1;
  qword diag_ts;

/* Verify that the event id is in the right range and that the
    corresponding bit is turned on in the event mask. */
    if ((event_id > EVENT_LAST_ID) || !EVENT_MASK_BIT_SET (event_id))
    {
       return;
    }

  /* Calculate packet length for event report (single event) */
  pkt_length = sizeof(event_rpt_type) - 1;
  pkt_length += sizeof(event_type);
  pkt_length += length;
  pkt_length +=DIAG_REST_OF_DATA_POS;
  if (length > EVENT_PAY_TWO_BYTE)
  {
    /* include extra byte for payload length field */
    pkt_length += 1;
  }

  /* Allocate packet for event report */
  event_pkt = diag_stm_event_alloc(pkt_length);

  if (event_pkt)
  {
    diag_data* pdiag_data = (diag_data*) event_pkt;

    /* Fill in the fact that this is an event to be sent over STM */
    pdiag_data->diag_data_type = DIAG_DATA_TYPE_STM_EVENT;

    /* Advance the pointer to point to the event_rpt_type part */
    ptr = event_pkt + DIAG_REST_OF_DATA_POS;
    /*  Note: Not accounting for dropped STM events here if alloc fails.
        Should be added?
     */

    /* Set fields of event_report */
    stm_event_hdr.cmd_code = DIAG_EVENT_REPORT_F;
    stm_event_hdr.length = pkt_length - hdr_size;

    copy_bytes = memscpy(ptr, (pkt_length-(ptr-event_pkt)),&stm_event_hdr,hdr_size);
    ptr += copy_bytes;

    stm_event_id.id = event_id;
    if (length <= EVENT_PAY_TWO_BYTE)
    {
      stm_event_id.payload_len = length;
    }
    else
    {
      stm_event_id.payload_len = EVENT_PAY_LENGTH;
    }
    stm_event_id.time_trunc_flag = FALSE;
    stm_event_id.reserved = 0; 

    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),
                          &stm_event_id,sizeof(event_id_type));
    ptr += copy_bytes;

    diag_time_get_LSM(diag_ts);
    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),diag_ts,sizeof(qword));
    ptr += copy_bytes;

    if (payload)
    {
      if (length > EVENT_PAY_TWO_BYTE)
      {
        *ptr++ = length;
      }
      /* Copy payload */
      copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),payload,length);
    }

    /* Send event report to STM */
    diag_stm_write_pkt((void*) (event_pkt + DIAG_REST_OF_DATA_POS), pkt_length-DIAG_REST_OF_DATA_POS );

    /* Free event report */
    diag_stm_event_free((void*)event_pkt);
  }
}
#endif /*/ FEATURE_DIAG_STM */


/* Externalized functions */
/*===========================================================================

FUNCTION EVENT_REPORT_CORE

DESCRIPTION
  Report an event. Published Diag API.

DEPENDENCIES
   Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void event_report_core (event_id_enum_type Event_Id)
{
   if(diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pEvent;
      int Alloc_Len = 0;

#if defined FEATURE_DIAG_STM
	  /* Is STM enabled, allocate entire event report and send event immediately. */
	  if( diag_stm_enabled )
      {
          event_generate_stm(Event_Id,0,NULL);
      }
      else
#endif /* FEATURE_DIAG_STM */
      {

        pEvent = event_alloc (Event_Id, 0, &Alloc_Len);
        if(pEvent)
        {
           if(diag_lsm_write_data(pEvent, Alloc_Len) < Alloc_Len )
           {
              gEvent_commit_fail++;
           }
           DiagSvc_Free(pEvent, GEN_SVC_ID);
        }
      }
   }
   return;
}/* event_report */

/*===========================================================================

FUNCTION EVENT_REPORT_PAYLOAD_CORE

DESCRIPTION
  Report an event with payload data.

PARAMETERS
   Event_Id       - ID of the event to be reported, as defined in "event_defs.h"
   Length         - Length of the associated payload
   *pPayload      - Pointer to the payload structure. Payload should be published.

DEPENDENCIES
  Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
event_report_payload_core (event_id_enum_type Event_Id, uint8 Length, void *pPayload)
{
   if( diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pEvent = NULL;
      int Alloc_Len = 0;

      if (Length > 0 && pPayload)
      {
#if defined FEATURE_DIAG_STM
        /* Is STM enabled, allocate entire event report and send event immediately. */
        if( diag_stm_enabled )
        {
            event_generate_stm(Event_Id,Length,pPayload);
        }
        else
#endif /* FEATURE_DIAG_STM */
        {
         pEvent = event_alloc (Event_Id, Length, &Alloc_Len);
         if (pEvent)
         {
            event_store_type* temp = (event_store_type*) (pEvent + FPOS(diag_data, rest_of_data));
            memcpy (temp->payload.payload, pPayload, Length);
           if(diag_lsm_write_data(pEvent, Alloc_Len) < Alloc_Len )
            {
               gEvent_commit_fail++;
            }
            DiagSvc_Free(pEvent, GEN_SVC_ID);
         }
        }
      }
      else
      {
         event_report (Event_Id);
      }
   }
   return;
}/* event_report_payload */


/*===========================================================================
FUNCTION   Diag_LSM_Event_Init

DESCRIPTION
  Initializes the event service. 
  Mask will be updated later after diagID has been set.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_Event_Init(void)
{
   return TRUE;
} /* Diag_LSM_Event_Init */

/*===========================================================================

FUNCTION    Diag_LSM_Event_DeInit

DESCRIPTION
  De-Initialize the Diag Event service.

DEPENDENCIES
  None.

RETURN VALUE
  boolean: returns TRUE; currently does nothing.

SIDE EFFECTS
  None

===========================================================================*/
boolean Diag_LSM_Event_DeInit(void)
{
  return TRUE;
} /* Diag_LSM_Event_DeInit */

/*===========================================================================

FUNCTION    event_update_mask

DESCRIPTION
  Updates the event mask for this user process

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean event_update_mask(void)
{
     boolean status = TRUE;
   if(diag_qdi_handle>=DIAG_FD_MIN_VALUE)
   {
      dword maskLen_Recvd = 0;
      if(!qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_IOCTL,DIAG_IOCTL_GETEVENTMASK, NULL,0, event_mask, sizeof(event_mask), &maskLen_Recvd)
        || maskLen_Recvd != EVENT_MASK_SIZE)
      {
         status = FALSE;
      }
   }
   else
   {
      status = FALSE;
   }
   /* Update diag_event_composite_mask if GETEVENTMASK IOCTL is successful*/
   if(status == TRUE)
   {
   	 memscpy(diag_event_composite_mask,sizeof(diag_event_composite_mask),event_mask,sizeof(event_mask));
   }
    return status;
} /* event_update_mask */


/*==========================================================================

FUNCTION event_alloc

DESCRIPTION
  This routine allocates an event item from the pre-mallocated memory pool
   and fills in the following information:

  Event ID
  Time stamp
  Payload length field

  //TODO :This routine also detects dropped events and handles the reporting of
  //dropped events.

RETURN VALUE
  A pointer to the allocated  event is returned.
  NULL if the event cannot be allocated.
  The memory should be freed by the calling function, using DiagSvc_Free().
  pAlloc_Len is an output value, indicating the number of bytes allocated.

===========================================================================*/
static byte *
event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len)
{
   byte *pEvent = NULL;
   int alloc_len = 0;

   /* Verify that the event id is in the right range and that the
    corresponding bit is turned on in the event mask. */
    if ((id > EVENT_LAST_ID) || !EVENT_MASK_BIT_SET (id))
    {
       return NULL;
    }

   alloc_len =  FPOS(diag_data, rest_of_data) + FPOS (event_store_type, payload.payload) + payload_length ;
   pEvent = (byte *) DiagSvc_Malloc(alloc_len, GEN_SVC_ID);
   if (pEvent)
   {
      event_store_type* temp = NULL;
      diag_data* pdiag_data = (diag_data*) pEvent;
      /* Fill in the fact that this is an event. */
      pdiag_data->diag_data_type = DIAG_DATA_TYPE_EVENT;
      /* Advance the pointer to point to the event_store_type part */
      temp = (event_store_type*) (pEvent + FPOS(diag_data, rest_of_data));
      if(pAlloc_Len)
      {
         *pAlloc_Len = alloc_len;
      }
      time_get (temp->ts);
      temp->event_id.id = id;
      temp->payload.length = payload_length;
   }
   return pEvent;
} /* event_alloc */
