/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics DiagID Routines

General Description
  Functions related to managing and reading DiagID


Copyright (c) 2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diag_diagID.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/20/17   gn      Notify the clients of existing diagID after PDR
06/19/17   ph      Tagging the data with dynamic diag IDs.
02/07/17   tbg     Added Apps support
11/17/16   tbg     File created

===========================================================================*/

#include "comdef.h"
#include "diagdiag_v.h"
#include "diag_diagIDi.h"
#include "servreg_locator.h"
#include "event.h"
#include "assert.h"
#include "stringl.h"
#include "diagcomm_io.h"
#include "diagcomm_ctrl.h"
#include "diagpkt.h"
#include "diagi_v.h"

/* list to get diagID from */
diag_diagID_pname_type *diag_diagID_pname_list = NULL;

static osal_mutex_arg_t     mutex_add_diagID;

/* To check feature mask */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

/* TODO: replace this with a better check */
extern unsigned char *diag_event_mask[DIAG_MAX_STREAM_ID];
#define EVENT_MASK_BIT_SET_TEST(id) \
  (diag_event_mask[0][(id)/8] & (1 << ((id) & 0x07)))

#ifdef DIAG_FORCE_DIAGID
/* Flag used to indicate if DiagID should be assigned locally.
   Only set if FORCE_DIAGID is defined AND apps feature mask does NOT support
   the feature.
 */

uint8 diag_diagID_use_local = DIAGID_LOCAL_ID_UNINIT;

/* Counter to assign local IDs with. Start at 2. */
uint8 diag_diagID_local = 2;
#endif

extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID][DIAG_SEND_BUF_CNT];
extern void diag_svc_change_notify(int Svc);

/*===========================================================================

FUNCTION DIAG_DIAGID_FIND_ELEMENT

DESCRIPTION
  Locates an element in the diagID list based on the process name.

PARAMETERS
 None

RETURN VALUE
  If found, pointer to the diagID element.
  If not found, NULL

===========================================================================*/

diag_diagID_pname_type *diag_diagID_find_element(char *name)
{
  diag_diagID_pname_type *found_ptr = NULL;
  diag_diagID_pname_type *diagid_ptr = diag_diagID_pname_list;

  if(diagid_ptr) /* Search only if list exists */
  {
    do
    {
      /* Compare only MAX_PNAME_LEN-1 characters (avoid comparing last character,
         which could differ due to NULL termination if the name was truncated) */
      if((strncmp(diagid_ptr->process_name,name,(MAX_PNAME_LEN-1)))==0)
      {
        found_ptr = diagid_ptr;
        break;
      }
      diagid_ptr=diagid_ptr->next;
    } while(diagid_ptr!=NULL);
  }

  return found_ptr;
}


/*===========================================================================

FUNCTION DIAG_SEND_DIAGID_EVENTS

DESCRIPTION
  Checks if events are enabled for EVENT_DIAG_PROCESS_NAME_ID or EVENT_DIAG_QSHRINK_ID,
  and generates the required events if needed.

  The functions tracks the previous state of the masks, checks the current state, and a flag
  for each DiagID indicating if the event was previously sent. Based on this information the
  following logic table indicates the required action:

  PrevMask  CurrMask  SentFlag     Action
     0         0         0           none
     0         0         1           none
     0         1         0         Send Event, set flag.
     0         1         1         Resend Event.
     1         0         0           none
     1         0         1           none
     1         1         0         Send Event, set flag
     1         1         1           none

   In case the resend_flag is set, the events will always be generated.

PARAMETERS
 resend_flag - Flag to force resending the events, even if previously sent.

RETURN VALUE
  None

===========================================================================*/
void diag_send_diagID_events(boolean resend_flag)
{
  /* Send event with diag and process name for all diagID which are not send */
  diag_diagID_pname_type * diagID_p = diag_diagID_pname_list;
  EVENT_DIAG_DIAGID_PNAME_type diagID_pname_info;
  EVENT_DIAG_DIAGID_QGUID_type diagID_Qguid_info;
  boolean pname_enabled = FALSE;
  boolean guid_enabled = FALSE;
  static boolean pname_enabled_last = FALSE;
  static boolean guid_enabled_last = FALSE;

#ifndef DIAG_FORCE_DIAGID
#ifndef DIAG_MP_MASTER
  /* Check feature mask - return if not enabled on slaves without sending any events */
  if (!(DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask,F_DIAG_DIAGID_SUPPORT)))
    return;
#endif /* DIAG_MP_MASTER */
#endif /* DIAG_FORCE_DIAGID */

  if(EVENT_MASK_BIT_SET_TEST(EVENT_DIAG_PROCESS_NAME_ID))
    pname_enabled = TRUE;
  if(EVENT_MASK_BIT_SET_TEST(EVENT_DIAG_QSHRINK_ID))
    guid_enabled = TRUE;

  if(pname_enabled || guid_enabled)
  {
    /* Search list if at least one event is enabled, and send events as needed */
    while(diagID_p!=NULL)
    {
      /* Skip sending DiagID events for nodes assigned to remote processors */
      if(!diagID_p->is_remote)
      {
        /* Do check for sending PNAME ID events */
        if(pname_enabled && (!diagID_p->is_event_pname_sent || !pname_enabled_last || resend_flag))  /* Event not yet sent, or mask reenabled, or resend flag set */
        {
          int size = sizeof(EVENT_DIAG_DIAGID_PNAME_type);

          memset(&diagID_pname_info,0,sizeof(diagID_pname_info));
          diagID_pname_info.diagID=diagID_p->diagID;
          strlcpy(diagID_pname_info.pname, diagID_p->process_name, MAX_PNAME_LEN);

          /* Calculate the actual size of the event (based on the pname string length) */
          size = FPOS(EVENT_DIAG_DIAGID_PNAME_type, pname) + (strlen(diagID_pname_info.pname) + 1);
          event_report_payload(EVENT_DIAG_PROCESS_NAME_ID,size,&diagID_pname_info);
          diagID_p->is_event_pname_sent=TRUE;
        }

        /* Do check for sending QSHRINK GUID ID events */
        if(guid_enabled && diagID_p->is_qshrink_supported &&
           (!diagID_p->is_event_qguid_sent || !guid_enabled_last || resend_flag)) /* Event not yet sent, or mask reenabled, or resend_flag set */
        {
          memset(&diagID_Qguid_info,0,sizeof(diagID_Qguid_info));
          diagID_Qguid_info.diagID=diagID_p->diagID;
          memcpy(&diagID_Qguid_info.QGuid, &diagID_p->Qguid,QSHRINK_4_DATABASE_GUID_LENGTH);
          event_report_payload(EVENT_DIAG_QSHRINK_ID,sizeof(EVENT_DIAG_DIAGID_QGUID_type),&diagID_Qguid_info);
          diagID_p->is_event_qguid_sent=TRUE;
        }
      }
      diagID_p=diagID_p->next;
    }
  }

  pname_enabled_last = pname_enabled;
  guid_enabled_last = guid_enabled;
  return;
}


/*===========================================================================

FUNCTION diag_add_diagID_element

DESCRIPTION
  Create and add a new diag_diagID_pname_type element to diag_diagID_pname_list

PARAMETERS
  name - Name of the process
  guid - Qshrink4 GUID array (16 byte array if present).
         If Qshrink4 not supported, pass in NULL
  sent - flag indicating if the control packet requesting the ID has been sent
  remote - flag indicating if the node is assigned to a remote processor

RETURN VALUE
  None

===========================================================================*/
void diag_add_diagID_element(char * name, uint8 *guid, boolean sent, boolean remote)
{
  diag_diagID_pname_type *new_diagID_elem;
  diag_diagID_pname_type *temp_ptr;
  static diag_diagID_pname_type *diag_diagID_tail_ptr=NULL;
  uint8 stream_index =0;

  new_diagID_elem = (diag_diagID_pname_type*)malloc(sizeof(diag_diagID_pname_type));

  if(new_diagID_elem)
  {
    memset(new_diagID_elem,0,sizeof(diag_diagID_pname_type));
    new_diagID_elem->is_ctrl_sent=sent;
    new_diagID_elem->is_remote = remote;
    new_diagID_elem->diagID = DIAGID_UNKNOWN_ID;

    for(stream_index=0;stream_index<DIAG_MAX_STREAM_ID;stream_index++)
    {
      new_diagID_elem->tx_mode_ptr[stream_index] = NULL;
    }

    if(guid != NULL)
    {
      new_diagID_elem->is_qshrink_supported=TRUE;
      memcpy(new_diagID_elem->Qguid, guid, QSHRINK_4_DATABASE_GUID_LENGTH);
    }
    else
      new_diagID_elem->is_qshrink_supported = FALSE;

    /* Note that name stored will be truncated/NULL terminated at MAX_PNAME_LEN
       if necessary. This needs to be taken into account when doing comparisons
       in other functions */
    strlcpy(new_diagID_elem->process_name, name, MAX_PNAME_LEN);
    new_diagID_elem->next=NULL;
  }
  else
  {
    MSG(MSG_SSID_DIAG,MSG_LEGACY_ERROR,"Failed to allocate memory for diagID element");
  return;
  }
  if(diag_diagID_pname_list==NULL)
  {
    /*This is the 1st element so add it at the head */
    diag_diagID_pname_list=new_diagID_elem;
    diag_diagID_tail_ptr=new_diagID_elem;
  }
  else
  {
    /* Itterate through the end of the list and add element.
     Also check that the element with same diagID is not present in the list since we will not cleanup when PD goes down */
    osal_lock_mutex(&mutex_add_diagID);
    temp_ptr = diag_diagID_find_element(name);
    if(temp_ptr != NULL)
    {
      /* Item already in list */
      free(new_diagID_elem);
      /* Notify the clients of existing diagID in case of PD restart
      (Node for PD will not be deleted from pname list when PD goes down)*/
      diag_svc_change_notify(DIAG_DIAGID_CHANGE);
    }
    else
    {
      /* item not in list, so add it */
      diag_diagID_tail_ptr->next = new_diagID_elem;
      diag_diagID_tail_ptr = diag_diagID_tail_ptr->next;
    }
    osal_unlock_mutex(&mutex_add_diagID);
  }
}

/*===========================================================================

FUNCTION diag_diagID_set_id

DESCRIPTION
  Set the ID value for the diagID element with NAME
  DiagID value.

PARAMETERS
  name - pname of the element to set
  id   - diagID to be set

DEPENDENCIES
  None

RETURN VALUE
  TRUE if ID is set
  FALSE if name not found

===========================================================================*/

boolean diag_diagID_set_id(char *name, uint8 id)
{
  diag_diagID_pname_type *ptr = diag_diagID_find_element(name);
  boolean rval = FALSE;

  if(ptr != NULL)
  {
    ptr->diagID = id;
    /* Set the TX pointer for the diagID received */
    diag_diagID_set_tx_ptr(name);

    /* Notify the clients of diagID update */
    diag_svc_change_notify(DIAG_DIAGID_CHANGE);

    rval = TRUE;
  }
  return rval;
}
/*===========================================================================

FUNCTION diag_diagID_set_tx_ptr

DESCRIPTION
  Set the TX pointer for the diagID element for the given NAME
  and DiagID value.

PARAMETERS
  name - pname of the element

DEPENDENCIES
  None

RETURN VALUE
  TRUE if TX pointer is set
  FALSE if name not found or no buffers available

===========================================================================*/
boolean diag_diagID_set_tx_ptr(char* name)
{
  boolean rval = FALSE;
  uint8 stream_index = 0;
  diag_pd_type diag_pd = DIAG_PD_NONE;
  diag_diagID_pname_type *ptr = NULL;
  uint8 buf_index;

  diag_pd = diag_get_pd_name(name);

  if(diag_pd == DIAG_PD_NONE)
  {
    rval = FALSE;
    return rval;
  }
  ptr = diag_diagID_find_element(name);

  if(ptr != NULL)
  {
    if(diag_pd == DIAG_PD_AUDIO )
    {
      buf_index = DIAG_DIAGID_AUDIO_USER_IDX;
    }
    else if (diag_pd == DIAG_PD_SENSOR)
    {
      buf_index = DIAG_DIAGID_SENSOR_USER_IDX;
    }
    else
    {
      /* If there is a new diagID and if no dedicated buffer is available,
      the data from that diagID goes to the root PD send buffer.(Same as Legacy behavior )*/
      buf_index = DIAG_LOCAL_SEND_BUF_IDX;
    }

    if(buf_index >= DIAG_SEND_BUF_CNT )
    {
      rval = FALSE;
      return rval;
    }

    for (stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index++)
    {
      /* Now update the TX structure to the diagID pointer */
      ptr->tx_mode_ptr[stream_index] = &diag_tx_mode[stream_index][buf_index];
    }
    rval = TRUE;
  }

  return rval;
}
/*===========================================================================

FUNCTION diag_get_diagID_from_GUID

DESCRIPTION
  Searches diag_diagID_pname_list for a GUID match and returns the corresponding
  DiagID value.

PARAMETERS
  guid - Byte array of Qshrink GUID to search for,
         of fixed length QSHRINK_4_DATABASE_GUID_LENGTH

DEPENDENCIES
  None

RETURN VALUE
  DiagID of the matching entry, or DIAGID_UNKNOWN_ID if not found.

===========================================================================*/

uint8 diag_get_diagID_from_GUID(uint8 *guid)
{
  diag_diagID_pname_type * temp_ptr=diag_diagID_pname_list;
  uint8 id = DIAGID_UNKNOWN_ID;

  while(temp_ptr!=NULL)
  {
    if((memcmp(temp_ptr->Qguid,guid,QSHRINK_4_DATABASE_GUID_LENGTH))==0)
    {
    id=temp_ptr->diagID;
      break;
    }
    temp_ptr=temp_ptr->next;
  }
  return id;

}


/*===========================================================================
PUBLIC API

FUNCTION diag_get_diagID

DESCRIPTION
  Returns the diagID value of the current process space.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  DiagID of the current process. DIAGID_UNKNOWN_ID indicates the value has
  not been assigned.

===========================================================================*/

uint8 diag_get_diagID()
{
#if defined(DIAG_MP_MASTER)
  /* For Apps, return APPS ID */
  /* (no service registry support currently) */
  return DIAGID_APPS_ID;
#else
  diag_diagID_pname_type *diagid_ptr = NULL;
  /* Retain value for future look-ups */
  static boolean valid_id_found = FALSE;
  static uint8 id = DIAGID_UNKNOWN_ID;


  /*Get the process and and then look for diagID in the list */
  if(!valid_id_found)
  {
    diagid_ptr = diag_diagID_find_element(servreg_get_local_domain());
    if(diagid_ptr)
    {
      id = diagid_ptr->diagID;
      if(id != DIAGID_UNKNOWN_ID)
      {
        valid_id_found = TRUE;
    }
    }
  }

  return id;
#endif
}


/*===========================================================================

FUNCTION DIAG_CTRL_REQUEST_DIAGID

DESCRIPTION
  Request a DiagID assignment for the provided process name. The DiagID value
  is assigned by the apps processor. It is to remain constant so long as the
  entire device is not reset (in the event of an SSR, apps will reassign the
  same value provided earlier)

PARAMETERS
  name - Process name which needs a DiagID assigned

RETURN VALUE
  None

===========================================================================*/

void diag_ctrl_request_diagID(char * name)
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  uint32 userpkt_len = 0;
  char process_name[MAX_PNAME_LEN];

#ifdef DIAG_FORCE_DIAGID
  /* If not supported on apps, abort control packet and set local ID */
  if(diag_diagID_use_local == DIAGID_LOCAL_ID_ENABLED)
  {
    diag_diagID_pname_type* diagID_node = diag_diagID_find_element(name);
    if(diagID_node)
    {
      diagID_node->diagID = diag_diagID_local++;
      /* Set the TX pointer and send notification here if using
      local diagID assignments*/
      diag_diagID_set_tx_ptr(name);
      diag_svc_change_notify(DIAG_DIAGID_CHANGE);
    }
    return;
  }
#endif

  /* Copy string to ensure it's NULL terminated at or before MAX_PNAME_LEN */
  strlcpy(process_name, name, MAX_PNAME_LEN);
  userpkt_len = strlen(process_name) + 1;

  /* Compose DIAG_CTRL_MSG_REG packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_DIAGID;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_diagID_type) + userpkt_len;
  ctrl_msg.data.ctrl_msg_diagID.version = DIAG_CTRL_MSG_DIAGID_VER;

  /* Send msg immediately */
  (void) diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg,
                                      DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_diagID_type),
                                      process_name, userpkt_len, DIAG_CTRL_USER_PKT_ID_0,
                                      DIAGCOMM_PORT_REMOTE_PROC,
                                      DIAGCOMM_PORT_1, TRUE, &is_sent );
}

/*===========================================================================

FUNCTION DIAG_CTRL_GENERATE_DIAGID

DESCRIPTION
  Assigns a DiagID assignment for the provided process name, and transmits
  the corresponding control packet back to the requestor. If there is already
  an ID assigned to the process name, the same ID will be reused.

PARAMETERS
  process_name - Process name which needs a DiagID assigned
  port_type    - Port type to send the control packet to
  ctrl_channel - Control channel to send the control packet to

RETURN VALUE
  None

===========================================================================*/

#if defined(DIAG_MP_MASTER)
void diag_ctrl_generate_diagID(char *name, diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel)
{
  uint8 id;
  uint32 userpkt_len = 0;
  char process_name[MAX_PNAME_LEN];
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  diag_diagID_pname_type *diagID_ptr;
  static uint8 diagID_counter = (DIAGID_APPS_ID + 1);

  /*** Create new node ***/

  /* Check if node already exists */
  diagID_ptr = diag_diagID_find_element(name);
  if(diagID_ptr == NULL)
  {
    /* Create node on list for remote ID */
    diag_add_diagID_element(name, NULL, TRUE, TRUE);
    diagID_ptr = diag_diagID_find_element(name);
    diagID_ptr->diagID = diagID_counter++;
  }

  id = diagID_ptr->diagID;

  /*** Send control packet ***/

  /* Copy string to ensure it's NULL terminated at or before MAX_PNAME_LEN */
  strlcpy(process_name, name, MAX_PNAME_LEN);
  userpkt_len = strlen(process_name) + 1;

  /* Compose DIAG_CTRL_MSG_REG packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_DIAGID;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_diagID_type) + userpkt_len;
  ctrl_msg.data.ctrl_msg_diagID.version = DIAG_CTRL_MSG_DIAGID_VER;
  ctrl_msg.data.ctrl_msg_diagID.diagID = id;

  /* Send msg immediately */
  (void) diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg,
                                      DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_diagID_type),
                                      process_name, userpkt_len,
                                      port_type,
                                      ctrl_channel, TRUE, &is_sent );
}
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_DIAGID_HANDLER

DESCRIPTION
  This handles the Diag subsystem command for reporting DiagIDs. It generates
  a response packet cotnaining a table of all currently assigned diagIDs and
  the associated process names.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
#if defined(DIAG_MP_MASTER)
PACK(void*) diag_diagID_handler(PACK(void *)req_pkt, uint16 pkt_len)
{
  diag_diagid_table_req_type *req = (diag_diagid_table_req_type*)req_pkt;
  diag_diagid_table_rsp_v1_type *rsp = NULL;
  uint8 name_entry_count = 0;
  diag_diagID_pname_type *diagid_ptr = diag_diagID_pname_list;
  int payload_len = 0;

  /* Check for valid packet size */
  if(pkt_len != sizeof(diag_diagid_table_req_type))
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /* Check for valid version */
  if(req->version != DIAG_DIAGID_CMD_VERSION_1)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  /* Build reponse packet with diagID entries */

  /* Calculate size */
  while(diagid_ptr != NULL)
  {
    name_entry_count++;
    payload_len += sizeof(diag_diagid_name_entry_type) + strlen(diagid_ptr->process_name) + 1;
    diagid_ptr = diagid_ptr->next;
  }

  rsp = (diag_diagid_table_rsp_v1_type*)
    diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, DIAGDIAG_GET_DIAG_ID,
                         sizeof(diag_diagid_table_rsp_v1_type) + payload_len);
  if(rsp)
  {
    int i,j;
    uint8 *payload_ptr = (uint8*)rsp->name_payload;
    uint8 *src_ptr;
    diag_diagid_name_entry_type name_entry;

    rsp->version = DIAG_DIAGID_CMD_VERSION_1;
    rsp->num_names = name_entry_count;

    diagid_ptr = diag_diagID_pname_list;
    for(i=0; i<name_entry_count; i++)
    {
      name_entry.diagID = diagid_ptr->diagID;
      name_entry.name_length = strlen(diagid_ptr->process_name)+1;

      src_ptr = (uint8*)&name_entry;
      for(j=0; j<sizeof(name_entry); j++)
        *payload_ptr++ = *src_ptr++;

      src_ptr = (uint8*)diagid_ptr->process_name;
      for(j=0; j<name_entry.name_length; j++)
        *payload_ptr++ = *src_ptr++;

      diagid_ptr = diagid_ptr->next;
    }
  }

  return (void*)rsp;
}
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP_MASTER)
/* DiagID table Command */
static const diagpkt_user_table_entry_type diag_subsys_tbl_diagid[] =
{
  {DIAGDIAG_GET_DIAG_ID, DIAGDIAG_GET_DIAG_ID, diag_diagID_handler}
};
#endif

/*===========================================================================

FUNCTION DIAG_DIAGID_INIT

DESCRIPTION
  Initialization required for DiagID. Will initialize mutex and register and
  command responses. Should be called before feature mask exchange occurs.

PARAMETERS
 None

RETURN VALUE
  None

===========================================================================*/
void diag_diagID_init(void)
{
  int return_val;
#if defined(DIAG_MP_MASTER)
  diag_diagID_pname_type *diagID_ptr;
  char *apps_name = "Apps"; /* Hard code apps name until available dynamically */
#endif

  mutex_add_diagID.name = "MUTEX_DIAGID_CS";
  return_val = osal_init_mutex(&mutex_add_diagID);
  ASSERT(OSAL_SUCCESS == return_val);

#if defined(DIAG_MP_MASTER)
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, DIAG_SUBSYS_DIAG_SERV, diag_subsys_tbl_diagid);

  /* Create initial entry for Apps processor */
  diag_add_diagID_element(apps_name, NULL, TRUE, FALSE);
  diagID_ptr = diag_diagID_find_element(apps_name);
  if(diagID_ptr != NULL)
    diagID_ptr->diagID = DIAGID_APPS_ID;
#endif
}


