#ifndef UL_TREE_H
#define UL_TREE_H
/*=============================================================================

FILE:      ul_tree.h

DESCRIPTION: This defines a tree interface

        Copyright (c) 2011 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/icb/interface/ul_tree.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  $Change: 16759227 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/13/11   rng     Initial Version

=============================================================================*/
#include "DALSys.h"

/*============================================================================
                          DATA TYPES
============================================================================*/

/*============================================================================
                          DEFINITIONS
============================================================================*/
typedef struct ul_treeNode
{
  uint64                   key;
  void *                   value;
  struct ul_treeNode * left;
  struct ul_treeNode * right;
} ul_treeNodeType;

/*============================================================================
                          EXTERNAL DATA
============================================================================*/

/*============================================================================
                          EXTERNAL FUNCTIONS
============================================================================*/

/**
   @brief ul_TreeLookup - Get the value associated with a key (if any)

   The lookup function will return the void * associated with the key.  If
   the key does not exist in the tree the function will return NULL.
   
   @param key : key to be found in the tree
   
   @return void* : value stored at the key or NULL if not found
*/
extern void * ul_TreeLookup( ul_treeNodeType * root, uint64 key );

#endif /* UL_TREE_H */
