/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides pdr voting infrastructure for user PD modules.

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_policy_pdr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/15   din     File created to support Error on User PD.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "queue.h"
#include "qurt.h"
#include "err_policy_pdr.h"
#include "rcesn_qurt.h"

#define ERR_POLICY_PDR_RCESN_NAME   "err_policy_pdr_event"


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/


/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void err_qdi_client_send_pdr_info( boolean user_pdr_allowed );

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
/* MACRO for locking/unlocking the qmutex*/
#define ERR_POLICY_PDR_MUTEX_LOCK()  \
   qurt_pimutex_lock(&(err_policy_pdr.mutex)); 
  
#define ERR_POLICY_PDR_MUTEX_FREE()  \
   qurt_pimutex_unlock(&(err_policy_pdr.mutex)); 

#define ERR_POLICY_PDR_MUTEX_INIT() \
   qurt_pimutex_init (&(err_policy_pdr.mutex ));

#define ERR_POLICY_PDR_STRING_SIZE       16
#define ERR_POLICY_PDR_STRING_MIN_SIZE    3
typedef struct 
{
  /* Must be the first element */
  q_link_type    link;
  int            count;
  char           string[ERR_POLICY_PDR_STRING_SIZE];
} err_policy_pdr_veto_s;

typedef struct 
{
  q_type                       queue;
  qurt_mutex_t                 mutex;
  /* For RCESN notification about change in PDR policy */
  RCESN_HANDLE                 rcesn_handle;
  boolean                      initialized;
  err_policy_pdr_state_e       is_pdr_allowed;
} err_policy_pdr_s;

static err_policy_pdr_s err_policy_pdr;

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_POLICY_PDR_INIT

DESCRIPTION
  Initialize err policy pdr module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL void err_policy_pdr_init (void)
{
  if ( err_policy_pdr.initialized == TRUE )
    return;

  ERR_POLICY_PDR_MUTEX_INIT();

  ERR_POLICY_PDR_MUTEX_LOCK();

  /* Initialize Queue */
  q_init(&(err_policy_pdr.queue));

  /* Create rcesn node */
  err_policy_pdr.rcesn_handle = rcesn_create_name(ERR_POLICY_PDR_RCESN_NAME);

  err_policy_pdr.is_pdr_allowed = ERR_POLICY_PDR_ALLOWED;

  if ( err_policy_pdr.rcesn_handle != RCESN_NULL )
    (void)rcesn_setstatecurr_handle(err_policy_pdr.rcesn_handle,  
                                   err_policy_pdr.is_pdr_allowed);

  err_policy_pdr.initialized = TRUE;

  ERR_POLICY_PDR_MUTEX_FREE();

} /* err_policy_pdr_init */


/*===========================================================================

FUNCTION ERR_POLICY_PDR_REGISTER_STATE_CHANGE_EVENT

DESCRIPTION
  Registration function for state change event

DEPENDENCIES
  err_policy_pdr_init ( called in RCINIT group 0 ) must be called before this
  function.

RETURN VALUE
  E_POLICY_PDR_SUCCESS - if registration sucessful
  E_POLICY_PDR_FAIL - Otherwise

SIDE EFFECTS 
  None

===========================================================================*/
e_policy_pdr_err_type err_policy_pdr_register_state_change_event 
(
  qurt_anysignal_t * const signal,
  unsigned int             mask
)
{
  /* Although we initialize here, if not initialized, it expects the clients
  to maintain rcinit dependency to err_policy_pdr_init() */
  if ( err_policy_pdr.initialized != TRUE )
  {
    err_policy_pdr_init();
  }

  if ( err_policy_pdr.rcesn_handle != RCESN_NULL &&
       rcesn_register_handle_qurt( err_policy_pdr.rcesn_handle, 
		              (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, 
			      signal, mask) )
  {
    return E_POLICY_PDR_SUCCESS;
  }

  return E_POLICY_PDR_FAIL;
      
} /* err_policy_pdr_register_state_change_event */

/*===========================================================================

FUNCTION ERR_POLICY_PDR_DEREGISTER_STATE_CHANGE_EVENT

DESCRIPTION
  De-registration function for state change event

DEPENDENCIES
  err_policy_pdr_init ( called in RCINIT group 0 ) must be called before this
  function.

RETURN VALUE
  E_POLICY_PDR_SUCCESS - if deregistration sucessful
  E_POLICY_PDR_FAIL - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL e_policy_pdr_err_type err_policy_pdr_deregister_state_change_event 
(
  qurt_anysignal_t * const signal,
  unsigned int             mask
)
{
  /* Although we initialize here, if not initialized, it expects the clients
  to maintain rcinit dependency to err_policy_pdr_init() */
  if ( err_policy_pdr.initialized != TRUE )
  {
    err_policy_pdr_init();
  }

  if ( err_policy_pdr.rcesn_handle != RCESN_NULL &&
       rcesn_unregister_handle_qurt( err_policy_pdr.rcesn_handle, 
		              (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, 
			      signal, mask) )
  {
    return E_POLICY_PDR_SUCCESS;
  }

  return E_POLICY_PDR_FAIL;
      
} /* err_policy_pdr_deregister_state_change_event */

/*===========================================================================

FUNCTION ERR_POLICY_PDR_IS_ALLOWED

DESCRIPTION
  Based on vetos , determines if PDR is allowed.

DEPENDENCIES
  err_policy_pdr_init ( called in RCINIT group 0 ) must be called before this
  function.

RETURN VALUE
  ERR_POLICY_PDR_ALLOWED if allowed
  ERR_POLICY_PDR_NOT_ALLOWED if not allowed

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL err_policy_pdr_state_e err_policy_pdr_is_pdr_allowed(void)
{
  /* Although we initialize here, if not initialized, it expects the clients
  to maintain rcinit dependency to err_policy_pdr_init() */
  if ( err_policy_pdr.initialized != TRUE )
  {
    err_policy_pdr_init();
  }

  /* Mutex is not needed since this is an atomic operation.
     Add it if needed in future */
  //ERR_POLICY_PDR_MUTEX_LOCK();
  
  if ( err_policy_pdr.is_pdr_allowed == ERR_POLICY_PDR_ALLOWED )
  {
    //ERR_POLICY_PDR_MUTEX_FREE();
    return ERR_POLICY_PDR_ALLOWED;
  } 

  //ERR_POLICY_PDR_MUTEX_FREE();

  return ERR_POLICY_PDR_NOT_ALLOWED;

} /* err_policy_pdr_is_allowed */


/*===========================================================================

FUNCTION ERR_POLICY_PDR_UPDATE_ERR_ROOTPD

DESCRIPTION
  Based on vetos, Updates root PD through QDI Call. The QDI call is invoked
  only pdr policy changes i.e. when first veto is cast or last veto is removed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void err_policy_pdr_update_err_rootpd(void)
{
  err_policy_pdr_state_e is_pdr_allowed = ERR_POLICY_PDR_NOT_ALLOWED;
   
  if ( err_policy_pdr.initialized != TRUE )
    return;
	    
  /* Determines is pdr allowed after current operation */
  if ( 0 == q_cnt (&(err_policy_pdr.queue)) )
  {
    is_pdr_allowed = ERR_POLICY_PDR_ALLOWED;
  } 
    
  /* Check if pdr policy has changed */
  if ( is_pdr_allowed == err_policy_pdr.is_pdr_allowed )
  {
    /* No Need to update clients */
    return;
  } 

  /* There was a change in PDR policy, update global PDR policy */
  err_policy_pdr.is_pdr_allowed = is_pdr_allowed;

  /* Update err edi client module on PDR policy */
  err_qdi_client_send_pdr_info( err_policy_pdr.is_pdr_allowed );

  /* Broadcast RCESN event */
  if ( err_policy_pdr.rcesn_handle != RCESN_NULL )
    (void)rcesn_signal_handle( err_policy_pdr.rcesn_handle, is_pdr_allowed );

} /* err_policy_pdr_update_err_rootpd */


/*===========================================================================

FUNCTION ERR_POLICY_PDR_COMPARATOR_FUNC

DESCRIPTION
  Used by queue logic to find out the node with a given value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful
  FALSE if unsuccessful

SIDE EFFECTS
  None

===========================================================================*/
static int err_policy_pdr_comparator_func
(
  void * link,
  void * compare_val
)
{
  err_policy_pdr_veto_s * temp_node = (err_policy_pdr_veto_s*)link;
  
  if ( temp_node == NULL || compare_val == NULL )
    return 0;

  if ( strncmp((char *)compare_val, temp_node->string, 
             ERR_POLICY_PDR_STRING_SIZE ) == 0 )
  {
    return 1;
  }

  return 0;

} /* err_policy_pdr_comparator_func */

/*===========================================================================

FUNCTION ERR_POLICY_PDR_VALIDATE_RETURN_USER_STRING

DESCRIPTION
  Validates and returns user string.

DEPENDENCIES
  None.

RETURN VALUE
  E_POLICY_PDR_SUCCESS if successful

SIDE EFFECTS
  None

===========================================================================*/

e_policy_pdr_err_type err_policy_pdr_validate_return_string
(
  char * user_string,
  char * ret_string

)
{
  uint8 str_len;
  
  /* Check if string is not NULL */
  if ( user_string == NULL )
  {
   return E_POLICY_PDR_INVALID_ARGS;
  }
  
  /* Ensures NULL terminated string */
  str_len = strlcpy(ret_string, user_string, ERR_POLICY_PDR_STRING_SIZE );
  
  /* Check if string is of the required size */
  if ( str_len < ERR_POLICY_PDR_STRING_MIN_SIZE )
  {
    return E_POLICY_PDR_INVALID_ARGS;
  }

  return E_POLICY_PDR_SUCCESS;

} /* err_policy_pdr_validate_return_string */

/*===========================================================================

FUNCTION ERR_POLICY_PDR_VETO

DESCRIPTION
  Temporairly disables PDR. Veto ownership is tracked using argument 
  "user_string" for uniqueness and to facilitate debugging based on 
  the argument "user_string". To re-enable PDR,
  remove veto using the same string. If there are multiple vetos, 
  same number of remove vetos would be required with same user_string 
  names.

  *NOTE* user_string must be atleast 3 characters in length and cannot be 
  longer than 15 characters. The length specified is excluding NULL terminated 
  character. Typically, thread name is a good choice.

  If the err policy driver ( initialized in RCINIT Group 0) is not 
  initialized, it would be initialized with this call, but the root PD will be 
  updated only after qdi client is initialized.

DEPENDENCIES
  err_policy_pdr_init ( called in RCINIT group 0 ) must be called before this
  function.

RETURN VALUE
  E_POLICY_PDR_SUCCESS if successful

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL e_policy_pdr_err_type err_policy_pdr_veto(char * user_string)
{
  /*  This function can be called before the module is fully 
  initialized.  DO NOT BREAK THIS */
  err_policy_pdr_veto_s * temp_node;
  char ret_string[ERR_POLICY_PDR_STRING_SIZE];
  e_policy_pdr_err_type ret;
	  
  /* Although we initialize here, if not initialized, it expects the clients
  to maintain rcinit dependency to err_policy_pdr_init() */
  if ( err_policy_pdr.initialized != TRUE )
  {
    err_policy_pdr_init();
  }	 

  ERR_POLICY_PDR_MUTEX_LOCK();

  ret = err_policy_pdr_validate_return_string( user_string, ret_string);

  if( ret != E_POLICY_PDR_SUCCESS )
    return ret;

  /* Search for the item in the queue */
  temp_node = (err_policy_pdr_veto_s *) q_linear_search(
		  &(err_policy_pdr.queue),
		  err_policy_pdr_comparator_func,
		  ret_string);

  if ( temp_node != NULL )
  {
    temp_node->count++;
    ERR_POLICY_PDR_MUTEX_FREE();
    return E_POLICY_PDR_SUCCESS;
  }

  temp_node = calloc ( 1, sizeof(err_policy_pdr_veto_s ));

  if ( temp_node == NULL )
  {
    ERR_POLICY_PDR_MUTEX_FREE();
    return E_POLICY_PDR_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Populate temp_node */
  strlcpy(temp_node->string, ret_string, ERR_POLICY_PDR_STRING_SIZE );
  temp_node->count= 1;

  /* Insert in the queue */
  q_link( temp_node, &(temp_node->link) );
  q_put(&(err_policy_pdr.queue), &(temp_node->link) );

  /* Update to err on root pd, only if PDR policy is updated */
  err_policy_pdr_update_err_rootpd();

  ERR_POLICY_PDR_MUTEX_FREE();

  return E_POLICY_PDR_SUCCESS;

} /* err_policy_pdr_veto */


/*===========================================================================

FUNCTION ERR_POLICY_PDR_REMOVE_VETO

DESCRIPTION
  Removes Veto and reenables PDR if all vetoes are removed. 
  Veto ownership is tracked using argument 
  "user_string" for uniqueness and to facilitate debugging based on 
  the argument "user_string".To re-enable PDR,
  remove veto using the same string. If there are multiple vetoes, 
  same number of remove vetos would be required with same user_string 
  names.

  *NOTE* user_string must match the string while vetoing.

  If the err policy driver ( initialized in RCINIT Group 0) is not 
  initialized, it would be initialized with this call, but the root PD will be 
  updated only after qdi client is initialized.

DEPENDENCIES
  Veto is required to remove veto.
  err_policy_pdr_init ( called in RCINIT group 0 ) must be called before this
  function

RETURN VALUE
  E_POLICY_PDR_SUCCESS if successful

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL e_policy_pdr_err_type err_policy_pdr_remove_veto(char * user_string)
{
  err_policy_pdr_veto_s * temp_node;
  char ret_string[ERR_POLICY_PDR_STRING_SIZE];
  e_policy_pdr_err_type ret;
	  
  /* Although we initialize here, if not initialized, it expects the clients
  to maintain rcinit dependency to err_policy_pdr_init() */
  if ( err_policy_pdr.initialized != TRUE )
  {
    err_policy_pdr_init();
  }

  ERR_POLICY_PDR_MUTEX_LOCK();
  
  ret = err_policy_pdr_validate_return_string( user_string, ret_string);

  if( ret != E_POLICY_PDR_SUCCESS )
    return ret;

  /* Search for the item in the queue */
  temp_node = (err_policy_pdr_veto_s *) q_linear_search(
		  &(err_policy_pdr.queue),
		  err_policy_pdr_comparator_func,
		  ret_string);

  if ( temp_node == NULL )
  {
    ERR_POLICY_PDR_MUTEX_FREE();
    return E_POLICY_PDR_NO_VETO;
  }

  /* If there are multiple vetos for same user_string, decrement count */
  if ( temp_node->count > 1 )
  {
    temp_node->count--;
    ERR_POLICY_PDR_MUTEX_FREE();
    return E_POLICY_PDR_SUCCESS;
  }

  /* Remove the item in the queue */
  q_delete ( &(err_policy_pdr.queue), &(temp_node->link) );

  free(temp_node);

  /* Update to err on root pd, only if PDR policy is updated */
  err_policy_pdr_update_err_rootpd();

  ERR_POLICY_PDR_MUTEX_FREE();

  return E_POLICY_PDR_SUCCESS;

} /* err_policy_pdr_remove_veto */

