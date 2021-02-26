/*============================================================================
@file npa_inlines.h

Common internal inline routines.

Copyright (c) 2009-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

============================================================================*/
#ifndef NPA_INLINES_H
#define NPA_INLINES_H

#include "npa_internal.h"
#include "CoreThread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <!-- npa_lock_graph -->
 *
 */
static NPA_INLINE void npa_lock_graph( void )
{
  Core_MutexLock( npa.graph_lock );
}


/**
 * <!-- npa_unlock_graph -->
 *
 */
static NPA_INLINE void npa_unlock_graph( void )
{
  Core_MutexUnlock( npa.graph_lock );
}


/**
 * <!-- npa_get_resources -->
 *
 * @brief Get the head of the global npa resources linked list. 
 */
static NPA_INLINE npa_link *npa_get_resources( void )
{
  return npa.resources;
}


/**
 * <!-- npa_get_resource_type -->
 *
 * @brief Returns the npa_resource_type based on several tests of the resource struct. 
 */
static NPA_INLINE
npa_resource_type npa_get_resource_type( npa_resource *resource )
{
  if ( NULL == resource )
  {
    return NPA_UNDEFINED_RESOURCE;
  }
  if ( NULL != resource->definition )
  {
    return NPA_INSTANTIATED_RESOURCE;
  }
  if ( npa_aliased_node == resource->node )
  {
    return NPA_ALIAS_DEFINITION;
  }
  if ( NULL != resource->node )
  {
    return NPA_DEFINED_RESOURCE;
  }

  return NPA_PLACEHOLDER_RESOURCE;
}


/**
 * <!-- npa_is_resource_type -->
 *
 * @brief Compare the type with the resource's type value.
 */
static NPA_INLINE unsigned int npa_is_resource_type( npa_resource     *resource, 
                                                     npa_resource_type type )
{
  return ( type == npa_get_resource_type( resource ) );
}


/**
 * <!-- npa_is_link_valid -->
 *
 * @brief Test whether a npa_link pointer is non-NULL and points to an
 * instantiated resource. 
 */
static NPA_INLINE unsigned int npa_is_link_valid( npa_link *link )
{
  return ( ( NULL != link ) && 
           ( NPA_INSTANTIATED_RESOURCE == npa_get_resource_type( link->resource ) ) );
}


/**
 * <!-- npa_is_link_alias -->
 *
 * @brief Return TRUE if it's an alias.
 *
 * If the link's name pointer is not the same as the resource's name pointer it's an alias
 */
static NPA_INLINE unsigned int npa_is_link_alias( npa_link *link )
{
  return ( link->name != link->resource->definition->name );
}


/**
 * <!-- npa_resource_has_attribute -->
 *
 * @brief AND the attribute with the resource's attributes and return TRUE or FALSE
 */
static NPA_INLINE unsigned int
npa_resource_has_attribute( npa_resource           *resource,
                            npa_resource_attribute  attribute )
{
  return ( ( resource->_internal->active_attr & attribute ) == 
                                                (unsigned) attribute );
}


/**
 * <!-- npa_node_has_attribute -->
 *
 * @brief AND the attribute with the node's attributes and return TRUE or FALSE
 */
static NPA_INLINE unsigned int
npa_node_has_attribute( npa_node_definition    *node,
                        npa_node_attribute  attribute )
{
  return ( node->attributes & attribute );
}


/**
 * <!-- npa_compare_times -->
 *
 * @brief Returns the signed time difference between time_a and time_b.
 */
static NPA_INLINE int64 npa_compare_times( uint64 time_a,
                                           uint64 time_b )
{
  return (int64)(time_a - time_b);
}


/**
 * <!-- npa_get_current_transaction -->
 *
 * @brief Returns the current active transaction in the calling thread, or NULL
 */
extern unsigned int npa_transaction_key;
static NPA_INLINE npa_transaction_handle npa_get_current_transaction( void )
{
  return (npa_transaction_handle) Core_TlsGet( npa_transaction_key );
}

#ifdef __cplusplus
}
#endif

#endif /* NPA_INLINES_H */
