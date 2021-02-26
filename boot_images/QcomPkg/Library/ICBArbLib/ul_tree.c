/*=============================================================================

FILE:      ul_tree.c

DESCRIPTION: This defines an AVL tree implementation

        Copyright (c) 2011 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/ul_tree.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/13/11   rng     Initial Version

=============================================================================*/
#include "ul_tree.h"

/*============================================================================
                          DATA TYPES
============================================================================*/


/*============================================================================
                          DEFINITIONS
============================================================================*/
#define TREECOMPARE(key1,key2)  (int64)((key2) - (key1))

/*============================================================================
                          EXTERNAL DATA
============================================================================*/
/*============================================================================
                          STATIC DATA
============================================================================*/

/*============================================================================
                          STATIC FUNCTIONS
============================================================================*/

/* treeLookup */
static ul_treeNodeType * treeLookup( ul_treeNodeType * root, uint64 key )
{
  int64                  compResult;
  ul_treeNodeType  * node = root;

  while( NULL != node )
  {
    compResult = TREECOMPARE(key, node->key);

    if( compResult == 0 )
    {
      /* Short Circuit return when we find it */
      return node;
    }
    else if( compResult > 0 )
    {
      node = node->left;
    }
    else
    {
      node = node->right;
    }
  }

  return NULL;  // NOTE: node must be NULL if we got here
}


/*============================================================================
                          EXTERNAL FUNCTIONS
============================================================================*/

/** ul_TreeLookup
 * 
 * @author ngibson (11/17/2011)
 * 
 * @param root 
 * @param key 
 * 
 * @return void* 
 */
void * ul_TreeLookup( ul_treeNodeType * root, uint64 key )
{
  ul_treeNodeType * node;

  node = treeLookup(root, key);

  return (( NULL != node ) ? node->value : NULL);
}
