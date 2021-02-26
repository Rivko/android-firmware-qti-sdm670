/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides pdr voting infrastructure for user PD modules.

Copyright (c) 2015 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/err_policy_pdr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/15   din     File created to support Error on User PD.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                     DECLARATIONS FOR MODULE

===========================================================================*/
typedef enum
{
  E_POLICY_PDR_SUCCESS,
  E_POLICY_PDR_FAIL,
  E_POLICY_PDR_INVALID_ARGS,
  E_POLICY_PDR_NO_VETO,
  E_POLICY_PDR_DYNAMIC_ALLOCATION_FAILED
} e_policy_pdr_err_type;

typedef enum
{
  ERR_POLICY_PDR_NOT_ALLOWED,
  ERR_POLICY_PDR_ALLOWED,
} err_policy_pdr_state_e;

/*===========================================================================

                              Function definitions

===========================================================================*/

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
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
e_policy_pdr_err_type err_policy_pdr_register_state_change_event 
(
  qurt_anysignal_t * const signal,
  unsigned int             mask
);

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
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
e_policy_pdr_err_type err_policy_pdr_deregister_state_change_event 
(
  qurt_anysignal_t * const signal,
  unsigned int             mask
);

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
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
err_policy_pdr_state_e err_policy_pdr_is_pdr_allowed(void);

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
  function

RETURN VALUE
  E_POLICY_PDR_SUCCESS if successful

SIDE EFFECTS
  None

===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
e_policy_pdr_err_type err_policy_pdr_veto(char * user_string);

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
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
e_policy_pdr_err_type err_policy_pdr_remove_veto(char * user_string);

