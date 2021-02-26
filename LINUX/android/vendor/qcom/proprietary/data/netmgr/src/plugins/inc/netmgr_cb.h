/******************************************************************************

                        N E T M G R _ C B . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_cb.h
  @brief   Network Callback Framework Headers

  DESCRIPTION
  Header file for Plugin Interface module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


#ifndef _NETMGR_CB_H_

/*===========================================================================
  FUNCTION  netmgr_print_cb_tables
===========================================================================*/
/*!
@brief
  Prints a pretty tree of all function trees, chains, and CB entries

@pre
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

*/
/*=========================================================================*/
void
netmgr_print_cb_tables
(
  void
);

/*===========================================================================
  FUNCTION  netmgr_cb_init
===========================================================================*/
/*!
@brief
  Initializes the CB root table node. Calling this when already initialized
  will result in an error, but the CB framework will still function

@pre
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

@return
  NETMGR_FAILURE or NETMGR_SUCCESS

*/
/*=========================================================================*/
int
netmgr_cb_init
(
  void
);

/*===========================================================================
  FUNCTION  netmgr_cb_reset
===========================================================================*/
/*!
@brief
  Frees up all CB framework resources and resets root node to (nil).
  netmgr_cb_init() must be invoked after netmgr_cb_reset() if the CB
  framework is to be used again

@pre
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

*/
/*=========================================================================*/
void
netmgr_cb_reset
(
  void
);

/*===========================================================================
  FUNCTION  netmgr_cb_create_table
===========================================================================*/
/*!
@brief
  Creates a new table with the specified name. This name should be unique.

@pre
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

@return
  NETMGR_FAILURE or NETMGR_SUCCESS

*/
/*=========================================================================*/
int netmgr_cb_create_table
(
  const char *name
);

/*===========================================================================
  FUNCTION  netmgr_cb_create_chain
===========================================================================*/
/*!
@brief
  Creates a new chain with the specified name. This name should be unique.

@pre
  Specified table must already exist
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

@return
  NETMGR_FAILURE or NETMGR_SUCCESS

*/
/*=========================================================================*/
int
netmgr_cb_create_chain
(
  const char *table_name,
  const char *chain_name
);

/*===========================================================================
  FUNCTION  netmgr_cb_insert_cb
===========================================================================*/
/*!
@brief
  Create a function call-back entry in the specified table->chain

@pre
  Specified table must already exist
  Specified chain must already exist
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

@return
  NETMGR_FAILURE or NETMGR_SUCCESS

*/
/*=========================================================================*/
int netmgr_cb_insert_cb
(
  const char *table_name,
  const char *chain_name,
  const char *owner_name,
  void * (*fp) (void *)
);

/*===========================================================================
  FUNCTION  netmgr_cb_runall
===========================================================================*/
/*!
@brief
  Runs all callbacks registered in the particular table->chain

@pre
  Specified table must already exist
  Specified chain must already exist
  netmgr_cb_table_lock must NOT be held otherwise we deadlock

@return
  NETMGR_FAILURE or NETMGR_SUCCESS. Note that NETMGR_SUCCESS will be returned
  even if one of the CBs errored out. It merely indicates that the tables
  and chains were successfully found and iterated.

*/
/*=========================================================================*/
int
netmgr_cb_runall
(
  const char *table_name,
  const char *chain_name,
  void *data
);
#endif /* _NETMGR_CB_H_ */
