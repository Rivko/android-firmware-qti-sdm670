/*=============================================================================

    netmgr_sm_ext.h

Description:
  This file contains the machine generated header file for the state machine
  specified in the file:
  ./netmgr_sm.stm

=============================================================================*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


#ifndef NETMGR_SM_EXT_H
#define NETMGR_SM_EXT_H

#ifdef __cplusplus
/* If compiled into a C++ file, ensure symbols names are not mangled */
extern "C"
{
#endif

/* Include STM framework header */
#include <stm2.h>

/* Begin machine generated code for state machine array: NETMGR_SM[] */

/* Define a macro for the number of SM instances */
#define NETMGR_SM_NUM_INSTANCES 40

/* External reference to state machine structure */
extern stm_state_machine_t NETMGR_SM[ NETMGR_SM_NUM_INSTANCES ];

/* External enumeration representing state machine's states */
enum
{
  NETMGR_SM__NETMGR_STATE_DOWN,
  NETMGR_SM__NETMGR_STATE_INITED,
  NETMGR_SM__NETMGR_STATE_COMING_UP,
  NETMGR_SM__NETMGR_STATE_CONFIGURING,
  NETMGR_SM__NETMGR_STATE_RECONFIGURING,
  NETMGR_SM__NETMGR_STATE_UP,
  NETMGR_SM__NETMGR_STATE_GOING_DOWN,
  NETMGR_SM__NETMGR_STATE_GOING_DOWN_TO_COME_UP,
};

#ifndef STM_DATA_STRUCTURES_ONLY
/* User called 'reset' routine.  Should never be needed, but can be used to
   effect a complete reset of all a given state machine's instances. */
extern void NETMGR_SM_reset(void);
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated code for state machine array: NETMGR_SM[] */


#ifdef __cplusplus
} /* extern "C" {...} */
#endif

#endif /* NETMGR_SM_EXT_H */
