/******************************************************************************

                        N E T M G R _ C B . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_cb.c
  @brief   Network Callback Framewoek

  DESCRIPTION
  Mechanisms for Netmgr Feature plugins to integrate

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "netmgr.h"
#include "netmgr_util.h"


/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define NETMGR_CB_ROOT_TABLE_NAME "ROOT"
#define NETMGE_CB_STR_LEN         32

/* TODO: Make somthing generic like this available for the rest of netmgr */
#define NETMGR_CB_NULL_CHECK(X) \
  if (!X) \
  { \
    netmgr_log_err("%s(): NULL parameter [%s] !!\n", __func__, #X); \
    return NETMGR_FAILURE; \
  }


struct netmgr_cb_node_s
{
  struct netmgr_cb_node_s *next;

  union
  {
    char owner_name[NETMGE_CB_STR_LEN];
    char chain_name[NETMGE_CB_STR_LEN];
    char table_name[NETMGE_CB_STR_LEN];
  };

  union
  {
    void * (*fp) (void *);
    struct netmgr_cb_node_s *cb_entry;
    struct netmgr_cb_node_s *cb_chain;
  };
};

LOCAL pthread_mutex_t netmgr_cb_table_lock = PTHREAD_MUTEX_INITIALIZER;
LOCAL struct netmgr_cb_node_s *netmgr_cb_root_table = 0;

#define netmgr_cb_lock() \
  pthread_mutex_lock(&netmgr_cb_table_lock);

#define netmgr_cb_unlock() \
  pthread_mutex_unlock(&netmgr_cb_table_lock);

/*===========================================================================
  FUNCTION  netmgr_cb_find_table
===========================================================================*/
/*!
@brief
  Gets the node of the table with the name specified

@pre
  netmgr_cb_table_lock must be held

@return
  Pointer to the netmgr_cb_node of the table entry. Null if no matching
  entry found

*/
/*=========================================================================*/
LOCAL struct netmgr_cb_node_s *
netmgr_cb_find_table
(
  const char *name
)
{
  struct netmgr_cb_node_s *next_table;
  next_table = netmgr_cb_root_table;
  while (next_table)
  {
    if (!strcmp(next_table->table_name, name))
      return next_table;
    next_table = next_table->next;
  }
  return 0;
}

/*===========================================================================
  FUNCTION  netmgr_cb_find_chain
===========================================================================*/
/*!
@brief
  Gets the node of the function entry chain with the name specified

@pre
  netmgr_cb_table_lock must be held

@return
  Pointer to the netmgr_cb_node of the chain. Null if no matching chain found

*/
/*=========================================================================*/
LOCAL struct netmgr_cb_node_s *
netmgr_cb_find_chain
(
  struct netmgr_cb_node_s *table,
  const char *name
)
{
  struct netmgr_cb_node_s *next_chain;
  next_chain = table->cb_chain;
  while (next_chain)
  {
    if (!strcmp(next_chain->chain_name, name))
      return next_chain;
    next_chain = next_chain->next;
  }
  return 0;
}

/*===========================================================================
  FUNCTION  netmgr_cb_insert_table
===========================================================================*/
/*!
@brief
  Appends the passed table to the end of the table list starting with the
  root node

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_cb_insert_table
(
  struct netmgr_cb_node_s *table
)
{
  struct netmgr_cb_node_s *next_table;
  next_table = netmgr_cb_root_table;
  if (!next_table)
  {
    netmgr_log_err("%s(): Root table not initialized\n", __func__);
    return;
  }

  while (next_table->next)
  {
    next_table = next_table->next;
  }
  next_table->next = table;
}

/*===========================================================================
  FUNCTION  netmgr_cb_insert_chain
===========================================================================*/
/*!
@brief
  Inserts a chain node into a table. The chain node is appended to end of
  the chain list for the given table.

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_cb_insert_chain
(
  struct netmgr_cb_node_s *table,
  struct netmgr_cb_node_s *chain
)
{
  struct netmgr_cb_node_s *next_chain;

  if (!table->cb_chain)
  {
    table->cb_chain = chain;
    return;
  }

  next_chain = table->cb_chain;
  while (next_chain->next)
    next_chain = next_chain->next;
  next_chain->next = chain;
}

/*===========================================================================
  FUNCTION  netmgr_cb_insert_cbentry
===========================================================================*/
/*!
@brief
  Inserts a call-back entry node into a chain. The node is appended to end of
  the call-back entry list for the given chain.

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_cb_insert_cbentry
(
  struct netmgr_cb_node_s *chain,
  struct netmgr_cb_node_s *entry
)
{
  struct netmgr_cb_node_s *next_entry;

  if (!chain->cb_entry)
  {
    chain->cb_entry = entry;
    return;
  }

  next_entry = chain->cb_entry;
  while (next_entry->next)
    next_entry = next_entry->next;
  next_entry->next = entry;
}

/*===========================================================================
  FUNCTION  netmgr_cb_free_cbentries_in_chain
===========================================================================*/
/*!
@brief
  Cleans up and frees all function call-back entries within a given chain.

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_cb_free_cbentries_in_chain
(
  struct netmgr_cb_node_s *chain
)
{
  struct netmgr_cb_node_s *next_entry, *current_entry;

  if (!chain->cb_entry)
  {
    return;
  }

  current_entry = chain->cb_entry;
  while (current_entry)
  {
    next_entry = current_entry->next;
    free(current_entry);
    current_entry = next_entry;
  }
}

/*===========================================================================
  FUNCTION  netmgr_cb_free_chains_in_table
===========================================================================*/
/*!
@brief
  Cleans up and frees all CB chains within a given table.

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_cb_free_chains_in_table
(
  struct netmgr_cb_node_s *table
)
{
  struct netmgr_cb_node_s *next_chain, *current_chain;

  if (!table->cb_chain)
  {
    return;
  }

  current_chain = table->cb_chain;
  while (current_chain)
  {
    netmgr_cb_free_cbentries_in_chain(current_chain);
    next_chain = current_chain->next;
    free(current_chain);
    current_chain = next_chain;
  }
}

/*===========================================================================
  FUNCTION  netmgr_print_cb_table
===========================================================================*/
/*!
@brief
  Prints a pretty picture of some pizza

@pre
  netmgr_cb_table_lock must be held

*/
/*=========================================================================*/
LOCAL void
netmgr_print_cb_table
(
  struct netmgr_cb_node_s *table
)
{
  struct netmgr_cb_node_s *next_chain;
  struct netmgr_cb_node_s *next_entry;
  netmgr_log_med("%s(): Function Table: %s\n", __func__, table->table_name);

  next_chain = table->cb_chain;
  while (next_chain)
  {
    netmgr_log_med("%s(): - Function Chain: %s\n", __func__,
                   next_chain->chain_name);
    next_entry = next_chain->cb_entry;
    while (next_entry)
    {
      netmgr_log_med("%s(): - - Function Entry: %s @ %p\n", __func__,
                     next_entry->owner_name, next_entry->fp);
      next_entry = next_entry->next;
    }
    next_chain = next_chain->next;
  }
}

/*===========================================================================
                     PUBLIC APIs
===========================================================================*/


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
)
{
  struct netmgr_cb_node_s *next_table;
  netmgr_cb_lock();
  next_table = netmgr_cb_root_table;
  while (next_table)
  {
    netmgr_print_cb_table(next_table);
    next_table = next_table->next;
  }
  netmgr_cb_unlock();
}

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
)
{
  netmgr_cb_lock();

  if (netmgr_cb_root_table)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): NETMGR CB already initialized\n", __func__);
    return NETMGR_FAILURE;
  }

  netmgr_cb_root_table = (struct netmgr_cb_node_s*)
                         malloc(sizeof(struct netmgr_cb_node_s));

  if (!netmgr_cb_root_table)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Failed to allocate root table node\n", __func__);
    return NETMGR_FAILURE;
  }

  memset(netmgr_cb_root_table, 0, sizeof(struct netmgr_cb_node_s));
  strlcpy(netmgr_cb_root_table->table_name,
          NETMGR_CB_ROOT_TABLE_NAME,
          sizeof(netmgr_cb_root_table->table_name));

  netmgr_cb_unlock();
  return NETMGR_SUCCESS;
}

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
)
{
  struct netmgr_cb_node_s *next_table, *current_table;

  netmgr_cb_lock();
  current_table = netmgr_cb_root_table;
  netmgr_cb_root_table = 0;

  while (current_table != 0)
  {
    netmgr_cb_free_chains_in_table(current_table);
    next_table = current_table->next;
    free(current_table);
    current_table = next_table;
  }
  netmgr_cb_unlock();
}

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
)
{
  struct netmgr_cb_node_s *next_table;
  NETMGR_CB_NULL_CHECK(name);
  netmgr_cb_lock();

  next_table = netmgr_cb_find_table(name);
  if (next_table)
  {
    netmgr_log_med("%s(): Table \"%s\" already exists\n", __func__, name);
    netmgr_cb_unlock();
    return NETMGR_FAILURE;
  }

  next_table = malloc(sizeof(struct netmgr_cb_node_s));
  if (!next_table)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Failed to allocate table node for \"%s\"\n",
                   __func__, name);
    return NETMGR_FAILURE;
  }

  memset(next_table, 0, sizeof(struct netmgr_cb_node_s));
  strlcpy(next_table->table_name, name, sizeof(next_table->table_name));;
  netmgr_cb_insert_table(next_table);

  netmgr_cb_unlock();
  return NETMGR_SUCCESS;
}

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
)
{
  struct netmgr_cb_node_s *table;
  struct netmgr_cb_node_s *next_chain;

  NETMGR_CB_NULL_CHECK(table_name);
  NETMGR_CB_NULL_CHECK(chain_name);

  netmgr_cb_lock();

  table = netmgr_cb_find_table(table_name);
  if (!table)
  {
    netmgr_log_med("%s(): Table \"%s\" does not exist\n", __func__, table_name);
    netmgr_cb_unlock();
    return NETMGR_FAILURE;
  }

  next_chain = netmgr_cb_find_chain(table, chain_name);
  if (next_chain)
  {
    netmgr_log_med("%s(): Chain \"%s\" already exists in table \"%s\"\n",
                   __func__, chain_name, table_name);
    netmgr_cb_unlock();
    return NETMGR_FAILURE;
  }

  next_chain = malloc(sizeof(struct netmgr_cb_node_s));
  if (!next_chain)
  {
    netmgr_log_med("%s(): Alloc failed for chain: %s\n", __func__, chain_name);
    netmgr_cb_unlock();
    return NETMGR_FAILURE;
  }

  memset(next_chain, 0, sizeof(struct netmgr_cb_node_s));
  strlcpy(next_chain->chain_name, chain_name, sizeof(next_chain->chain_name));
  netmgr_cb_insert_chain(table, next_chain);

  netmgr_cb_unlock();
  return NETMGR_SUCCESS;
}

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
)
{
  struct netmgr_cb_node_s *table;
  struct netmgr_cb_node_s *chain;
  struct netmgr_cb_node_s *entry;

  NETMGR_CB_NULL_CHECK(table_name);
  NETMGR_CB_NULL_CHECK(chain_name);
  NETMGR_CB_NULL_CHECK(owner_name);

  netmgr_cb_lock();

  table = netmgr_cb_find_table(table_name);
  if (!table)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Table \"%s\" does not exist\n", __func__, table_name);
    return NETMGR_FAILURE;
  }

  chain = netmgr_cb_find_chain(table, chain_name);
  if (!chain)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Chain \"%s\" does not exist in table \"%s\"\n",
                   __func__, chain_name, table_name);
    return NETMGR_FAILURE;
  }

  entry = malloc(sizeof(struct netmgr_cb_node_s));
  if (!entry)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Malloc failed\n", __func__);
    return NETMGR_FAILURE;
  }

  memset(entry, 0, sizeof(struct netmgr_cb_node_s));
  strlcpy(entry->owner_name, owner_name, sizeof(entry->owner_name));
  entry->fp = fp;
  netmgr_cb_insert_cbentry(chain, entry);

  netmgr_cb_unlock();
  return NETMGR_SUCCESS;
}

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
)
{
  struct netmgr_cb_node_s *table;
  struct netmgr_cb_node_s *chain;
  struct netmgr_cb_node_s *next_entry;

  NETMGR_CB_NULL_CHECK(table_name);
  NETMGR_CB_NULL_CHECK(chain_name);

  netmgr_cb_lock();

  table = netmgr_cb_find_table(table_name);
  if (!table)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Table \"%s\" does not exist\n", __func__, table_name);
    return NETMGR_FAILURE;
  }

  chain = netmgr_cb_find_chain(table, chain_name);
  if (!chain)
  {
    netmgr_cb_unlock();
    netmgr_log_med("%s(): Chain \"%s\" does not exist in table \"%s\"\n",
                   __func__, chain_name, table_name);
    return NETMGR_FAILURE;
  }

  next_entry = chain->cb_entry;
  while (next_entry != 0)
  {
    if (next_entry->fp)
    {
      netmgr_log_med("%s(): Running [%s:%s]-%s\n", __func__, table_name,
                     chain_name, next_entry->owner_name);
      next_entry->fp(data);
    }
    next_entry = next_entry->next;
  }

  netmgr_cb_unlock();
  return NETMGR_SUCCESS;
}
