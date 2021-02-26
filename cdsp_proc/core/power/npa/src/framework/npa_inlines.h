/*============================================================================
@file npa_inlines.h

Common internal inline routines.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/framework/npa_inlines.h#1 $
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
  if ( !resource )
  {
    return NPA_UNDEFINED_RESOURCE;
  }
  if ( resource->definition )
  {
    return NPA_INSTANTIATED_RESOURCE;
  }
  if ( npa_aliased_node == resource->node )
  {
    return NPA_ALIAS_DEFINITION;
  }
  if ( resource->node )
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
  return ( link && ( NPA_INSTANTIATED_RESOURCE == 
                       npa_get_resource_type( link->resource ) ) );
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
 * <!-- npa_dispatch_event_callback -->
 *
 * @brief Call the callback with its context, the event_type, 
 * event data, and data_size as arguments. 
 *
 * @param event_callback : Pointer to a npa_event_callback
 * @param event_type : An npa_event_type value.
 * @param data : Pointer to data appropriate to this invocation of
 * the callback
 * @param data_size : Size of data in bytes.
 */
static NPA_INLINE void
npa_dispatch_event_callback( npa_event_callback *event_callback, 
                             unsigned int        event_type, 
                             void               *data,
                             unsigned int        data_size )
{
  CORE_VERIFY_PTR( event_callback );
  event_callback->callback( event_callback->context, event_type, 
                            data, data_size );
}


/**
 * <!-- _npa_resource_has_attribute -->
 *
 * @brief Return TRUE if the resource has the given attribute. If more than
 * one attribute (ORd together) is provided, returns TRUE if the resource
 * has any of those attributes.
 */
static NPA_INLINE unsigned int
_npa_resource_has_attribute( npa_resource *resource, unsigned int attribute )
{
  return ( resource->definition->attributes & attribute );
}

/**
 * <!-- _npa_set_resource_attribute -->
 *
 * @brief Sets the given attribute (or attributes if more than one is
 * ORd together) for the resource
 */
static NPA_INLINE void
_npa_set_resource_attribute( npa_resource *resource, unsigned int attribute )
{
  resource->definition->attributes |= attribute;
}

/**
 * <!-- _npa_unset_resource_attribute -->
 *
 * @brief Unsets (removes) the given attribute (or attributes if more than
 * one is ORd together) from resource
 */
static NPA_INLINE void
_npa_unset_resource_attribute( npa_resource *resource, unsigned int attribute )
{
  resource->definition->attributes &= ~attribute;
}

/**
 * <!-- npa_resource_has_internal_attribute -->
 *
 * @brief Return TRUE if the resource has the given internal attribute
 * in _internal->attributes. If more than one attribute (ORd together) is
 * provided, returns TRUE if the resource has any of those attributes.
 */
static NPA_INLINE unsigned int
npa_resource_has_internal_attribute( npa_resource *resource, 
                                     unsigned int attribute )
{
  return ( resource->_internal->attributes & attribute );
}


/**
 * <!-- _npa_request_has_attribute -->
 *
 * @brief Returns TRUE if the client has the given request attribute (or one
 * of the request attributes, if multiple are ORd together) set.
 * Inline version of the public routine, intended for use within the framework
 */
static NPA_INLINE unsigned int
_npa_request_has_attribute( npa_client_handle client, unsigned int attr )
{
  return client->request_attr & attr;
}

/**
 * <!-- npa_node_has_attribute -->
 *
 * @brief AND the attribute with the node's attributes and return TRUE or FALSE
 */
static NPA_INLINE unsigned int
npa_node_has_attribute( npa_node_definition *node,
                        npa_node_attribute   attribute )
{
  return ( node->attributes & attribute );
}


/**
 * <!-- npa_compare_times -->
 *
 * @brief Returns the signed time difference between time_a and time_b.
 */
static NPA_INLINE int64 npa_compare_times( uint64 time_a, uint64 time_b )
{
  return (int64)(time_a - time_b);
}

/** 
 * <!-- npa_calc_duration -->
 *
 * @brief Calculate the diff between two times. Optionally, update stats.
 *
 * @param start, end : The two time stamps
 * @param dur_ptr : Pointer to a npa_duration structure, can be NULL
 *
 * @return: end time - start time
 */
static NPA_INLINE
uint64 npa_calc_duration( uint64 start, uint64 end, npa_duration *dur_ptr )
{
  uint64 diff = end - start;

  if ( dur_ptr )
  {
    if ( diff > dur_ptr->max )
    { /* record the max */
      dur_ptr->max = diff;
    }
    /* update the count */
    ++dur_ptr->count;
  }
  return diff;
}

/**
 * <!-- npa_lock_resource -->
 */
static NPA_INLINE void npa_lock_resource( npa_resource *resource )
{
  Core_MutexLock( resource->node_lock );
}

/**
 * <!-- npa_unlock_resource -->
 */
static NPA_INLINE void npa_unlock_resource( npa_resource *resource )
{
  Core_MutexUnlock( resource->node_lock );
}

/**
 * <!-- npa_resource_get_current_state -->
 */
static NPA_INLINE npa_resource_state
npa_resource_get_current_state( npa_resource *resource )
{
  return resource->active_state;
}

#ifdef __cplusplus
}
#endif

#endif /* NPA_INLINES_H */
