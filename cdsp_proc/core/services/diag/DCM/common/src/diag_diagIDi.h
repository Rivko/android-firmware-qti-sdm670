#ifndef DIAGDIAGIDI_H
#define DIAGDIAGIDI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics DiagID Routines

General Description
  Header file for items related to managing and reading DiagID


Copyright (c) 2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/diag_diagIDi.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/19/17   ph      Tagging the data with dynamic diag IDs.
02/07/17   tbg     Added is_remote flag
11/17/16   tbg     File created

===========================================================================*/

#include "comdef.h"
#include "diagdiag_v.h" 
#include "diagcomm_io.h" 
#include "diagbuf_v.h"
#include "diag_mode_v.h"


/* Predefined values for DiagID (all other dynamically assigned) */
#define DIAGID_UNKNOWN_ID 0
#define DIAGID_APPS_ID    1

/*Linklist to store diagID and processname*/
typedef struct diag_diagID_pname_type
{
  struct diag_diagID_pname_type *next;
  boolean is_event_pname_sent;     /*Check if the table has sent event to APPS.*/
  boolean is_event_qguid_sent;     /*Check if the table has sent event to APPS.*/
  boolean is_ctrl_sent;            /*Check if the ctrl packet for requesing diagID is send to APPS*/
  boolean is_qshrink_supported;    /*Check if this PD supports qshrink*/
  boolean is_remote;               /* flag indicating if node is associated with a remote processor */
  uint8 diagID;                    /* diagID for the PD */
  diag_tx_mode_type* tx_mode_ptr[DIAG_MAX_STREAM_ID]; /* TX mode pointer for diagID */
  char process_name[MAX_PNAME_LEN];/* PROCESS_NAME returned by TMS. NULL terminated. */
  uint8 Qguid[QSHRINK_4_DATABASE_GUID_LENGTH]; /* GUID for the Qshirnk database */
} diag_diagID_pname_type;

extern diag_diagID_pname_type *diag_diagID_pname_list;

#ifdef DIAG_FORCE_DIAGID
#define DIAGID_LOCAL_ID_UNINIT 0xff
#define DIAGID_LOCAL_ID_ENABLED 0x01
#define DIAGID_LOCAL_ID_DISABLED 0x00
#endif 


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
void diag_ctrl_request_diagID(char * name);

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
  DiagID of the matching entry, or 0 if not found.
  
===========================================================================*/
uint8 diag_get_diagID_from_GUID(uint8 *guid);



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
boolean diag_diagID_set_id(char *name, uint8 id);


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
void diag_add_diagID_element(char *name, uint8 *guid, boolean sent, boolean remote);


/*===========================================================================

FUNCTION DIAG_SEND_DIAGID_EVENTS

DESCRIPTION
  Checks if events are enabled for EVENT_DIAG_PROCESS_NAME_ID or EVENT_DIAG_QSHRINK_ID,
  and generates the required events if needed.
  
PARAMETERS
 resend_flag - Flag indicating if the events should be resent even if
               previously sent.
  
RETURN VALUE
  None

===========================================================================*/
void diag_send_diagID_events(boolean resend_flag);


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
void diag_diagID_init(void);

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
void diag_ctrl_generate_diagID(char *name, diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel);

/*===========================================================================

FUNCTION diag_diagID_set_tx_ptr

DESCRIPTION
  Set the TX pointer for the diagID element for the given NAME
  and DiagID value.
  
PARAMETERS
  name - pname of the element
  id   - diagID
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if TX pointer is set
  FALSE if name not found or no buffers available
  
===========================================================================*/
boolean diag_diagID_set_tx_ptr(char *name);

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
diag_diagID_pname_type *diag_diagID_find_element(char *name);

/*===========================================================================

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
uint8 diag_get_diagID();
#endif
