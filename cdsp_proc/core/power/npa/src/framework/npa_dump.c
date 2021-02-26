/*============================================================================
@file npa_dump.c

Functions to dump states and structures to a log. 

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/framework/npa_dump.c#1 $
============================================================================*/

#include <stdio.h>
#include "npa_internal.h"
#include "npa_inlines.h"
#include "ULogFront.h"
#include "npa_dump.h"
#include "DALStdErr.h"

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
#include "npa_scheduler_internal.h"
#endif

#ifdef NPA_SELF_TEST
/** run npa self tests by calling Diag plugin */
//extern int npa_test(void);
extern void npa_scheduler_profiling_test(ULogHandle);
#endif

/**
 * <!-- npa_dump_alias -->
 *
 * @brief Dump the alias's name and definition name.
 *
 * @param log : ULog handle to write the results into.
 * @param link : Pointer to a npa_link structure. 
 */
static void npa_dump_alias( ULogHandle log, npa_link *link )
{
  npa_resource_definition *definition;
  definition = link->resource->definition;

  ULOG_RT_PRINTF_2( log, "npa_alias (alias_name: %s) (resource_name: %s)",
                    link->name, definition->name );
}


/**
 * <!-- npa_dump_resource_state -->
 *
 * @brief Dump a resource's useful names, states, and status.
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource.
 */
static void npa_dump_resource_state( ULogHandle log, npa_resource *resource )
{
  npa_resource_definition *definition;
  npa_duration driver_stats;

  definition = resource->definition;
  driver_stats = resource->_internal->request_duration;

  ULOG_RT_PRINTF_10( log, 
                    "npa_resource (name: %s) (handle: 0x%08x) "
                    "(sequence: 0x%08x) (units: %s) "
                    "(resource max: %u) (active max: %u) "
                    "(active state %u) (active headroom: %d) "
                    "(request state: %u) (required state: %u)",
                    definition->name, resource,
                    resource->sequence, definition->units,
                    definition->max, resource->active_max,
                    resource->active_state, resource->active_headroom,
                    resource->request_state, definition->attributes );

  ULOG_RT_PRINTF_5( log,
                    "(resource attributes: 0x%08x) "
                    "(node_lock: 0x%08x) (event_lock: 0x%08x) "
                    "(driver call count: %u) (driver duration max: %u)",
                    definition->attributes,
                    resource->node_lock, resource->event_lock,
                    driver_stats.count, driver_stats.max );
}


/**
 * <!-- npa_dump_resource_clients -->
 *
 * @brief Walk the client list, and dump them to the log.
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource. 
 */
static void
npa_dump_resource_clients( ULogHandle log, npa_resource *resource )
{
  npa_client *client;
  npa_resource_definition *definition;

  client = resource->clients;
  definition = resource->definition;

  while ( client )
  {
    ULOG_RT_PRINTF_6( log, 
                      "\tnpa_client (name: %s) (handle: 0x%08x) "
                      "(resource: %s) (type: %u) "
                      "(sequence: 0x%08x) (request: %u)", 
                      client->name, client,
                      definition->name, client->type,
                      client->sequence, client->work[client->index].state );

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
    if ( client->request_ptr )
    { /* have scheduler information too */
      npa_scheduler_data *request_ptr = client->request_ptr;

      ULOG_RT_PRINTF_9( log, "\trequest_ptr "
                        "(handle: 0x%08x) (prev: 0x%08x) (next: 0x%08x) "
                        "(request_time: 0x%08x) (start_time: 0x%08x) "
                        "(notification: 0x%08x) "
                        "(on_time: %lu) (late: %lu) "
                        "(forked: %lu)",
                        request_ptr, request_ptr->prev, request_ptr->next,
                        request_ptr->request_time, request_ptr->start_time,
                        request_ptr->notification,
                        request_ptr->stats.on_time, request_ptr->stats.late,
                        request_ptr->stats.forked );
    }
#endif
    client = client->next;
  }
}


/**
 * <!-- npa_dump_reserved_event -->
 *
 * @brief Dump reserved events registered against a resource
 *
 * @param log : ULog handle to write the results into.
 * @param event : Pointer to a npa_event. 
 */
static void npa_dump_reserved_event( ULogHandle log, 
                                     npa_event *event )
{
  npa_continuation *continuation;

  CORE_VERIFY( NPA_TRIGGER_RESERVED_EVENT_0 == event->trigger_type );

  if ( npa_define_node_continuation == event->callback.callback )
  {
    continuation = (npa_continuation *)event->callback.context;
    ULOG_RT_PRINTF_1( log, "\tnode dependency (node: %s)",
                      continuation->data.node.definition->name );
  }
  else if ( npa_alias_resource_continuation == event->callback.callback )
  {
    continuation = (npa_continuation *)event->callback.context;
    ULOG_RT_PRINTF_1( log, "\talias dependency (alias: %s)",
                      continuation->data.alias.alias_name );

  }
  else if ( npa_resources_available_continuation == event->callback.callback  )
  {
    continuation = (npa_continuation *)event->callback.context;
    ULOG_RT_PRINTF_2( log, "\tresources available callback "
                      "(callback: %p) (context: %p)",
                      continuation->callback.callback,
                      continuation->callback.context );
  }
  else 
  {
    ULOG_RT_PRINTF_2( log, "\tresource available callback "
                      "(callback: %p) (context: %p)",
                      event->callback.callback, event->callback.context );
  }
}


/**
 * <!-- npa_dump_resource_events -->
 *
 * @brief Walk the list of events registered against the resource
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource. 
 */
static void npa_dump_resource_events( ULogHandle log, npa_resource *resource )
{
  npa_event *event;
  const char *resource_name;

  if ( resource->definition )
  {
    event = resource->events.list->reg_events;
    resource_name = resource->definition->name;
  }
  else
  {
    event = resource->events.creation_events;
    resource_name = "";
  }

  while ( event )
  {
    switch ( event->trigger_type )
    {
      case NPA_TRIGGER_RESERVED_EVENT_0:
        npa_dump_reserved_event( log, event );
        break;
      case NPA_TRIGGER_CHANGE_EVENT:
        ULOG_RT_PRINTF_3( log, 
                          "\tnpa_change_event (name: %s) (handle: 0x%08x) "
                          "(resource: %s)", 
                          event->name, event,
                          resource_name );
        break;
      case NPA_TRIGGER_WATERMARK_EVENT:
        ULOG_RT_PRINTF_5( log, 
                          "\tnpa_watermark_event (name: %s) (handle: 0x%08x) "
                          "(resource: %s) (lo_watermark: %u) (hi_watermark: %u)", 
                          event->name, event,
                          resource_name,
                          event->lo.watermark, event->hi.watermark );
        break;
      case NPA_TRIGGER_THRESHOLD_EVENT:
        ULOG_RT_PRINTF_5( log, 
                          "\tnpa_threshold_event (name: %s) (handle: 0x%08x) "
                          "(resource: %s) (lo_threshold: %u) (hi_threshold: %u)", 
                          event->name, event,
                          resource_name,
                          event->lo.threshold, event->hi.threshold );
        break;
      case NPA_TRIGGER_CUSTOM_EVENT1:
      case NPA_TRIGGER_CUSTOM_EVENT2:
      case NPA_TRIGGER_CUSTOM_EVENT3:
      case NPA_TRIGGER_CUSTOM_EVENT4:
        ULOG_RT_PRINTF_4( log, 
                          "\tnpa_custom_event (name: %s) (handle: 0x%08x) "
                          "(resource: %s) (trigger_type: %d)", 
                          event->name, event,
                          resource_name, event->trigger_type );
        break;
      default:
        // TODO: there's new event types in development (system event,
        // etc) that should eventually be handled
        ULOG_RT_PRINTF_4( log, 
                          "\tnpa_event (name: %s) (handle: 0x%08x) "
                          "(resource: %s) (type: %d)", 
                          event->name, event,
                          resource_name, event->trigger_type );
    }
    event = event->next;
  }
}


/**
 * <!-- npa_dump_placeholder -->
 *
 * @brief If a resource is only a placeholder still (i.e. it has not yet 
 * been created, this function is invoked to dump it. 
 *
 * @param log : ULog handle to write the results into.
 * @param link : Pointer to a npa_link structure
 */
static void npa_dump_placeholder( ULogHandle log, npa_link *link )
{
  npa_resource *resource;

  resource = link->resource;

  ULOG_RT_PRINTF_1( log, "missing resource (name: %s)", link->name );

  if ( resource->events.creation_events )
  {
    npa_dump_resource_events( log, resource );
  }
}


/**
 * <!-- npa_dump_resource -->
 *
 * @brief Lock the resource and call the dump functions on it's components. 
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource. 
 */
static void npa_dump_resource( ULogHandle log, 
                               npa_resource *resource )
{
  if ( resource->node_lock )
  { /* Resource has a node_lock; so acquire and dump state and clients */
    Core_MutexLock( resource->node_lock );
    npa_dump_resource_state( log, resource );
    npa_dump_resource_clients( log, resource );
    Core_MutexUnlock( resource->node_lock );
  }

  if ( resource->event_lock )
  { /* Resource has an event lock; so dump events */
    Core_MutexLock( resource->event_lock );
    if ( resource->events.list )
    {
      npa_dump_resource_events( log, resource );
    }
    Core_MutexUnlock( resource->event_lock );
  }

  ULOG_RT_PRINTF_1( log, "\tend npa_resource (handle: 0x%08x)", resource );
}


/**
* <!-- npa_dump_link -->
*
* @brief Identify the link type and call appropriate dump routines. 
*
* @param log : ULog handle to write the results into.
* @param link : Pointer to a npa_link structure
*/
static void npa_dump_link( ULogHandle log, npa_link *link )
{
  npa_resource *resource;
  
  resource = link->resource;

  if ( resource->definition == NULL )
  {
    npa_dump_placeholder(log, link );
  }
  else
  {  
    if ( 0 == strncmp( link->name, 
                       resource->definition->name,
                       NPA_MAX_STR_LEN+1 ) )
    {
      npa_dump_resource(log, resource);
    }
    else 
    {
      npa_dump_alias(log, link );
    }
  }
}


#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
/**
 * <!-- npa_dump_scheduler_control_stats -->
 *
 * @brief Write scheduler statistics to the provided log handle.
 *
 * @param log : ULog handle to write the results into.
 */
static void npa_dump_scheduler_control_stats( ULogHandle log )
{
  ULOG_RT_PRINTF_5( log, 
                    "npa_scheduler stats"
                    "(num_run_scheduler_requests: %lu) "
                    "(sched_requests_handled : %lu) "
                    "(lpr_enter : %lu) "
                    "(lpr_exit : %lu) "
                    "(forked : %lu) ",
                    npa_scheduler.stats.num_run_scheduler_requests,
                    npa_scheduler.stats.sched_requests_handled,
                    npa_scheduler.stats.lpr_enter,
                    npa_scheduler.stats.lpr_exit,
                    npa_scheduler.stats.forked );
}
#endif


/**
 * <!-- npa_dump -->
 * 
 * @brief Walk the npa resources list for "resource_name" and dump details.
 * Also dump scheduler stats (why?)
 *
 * @param dump_log : ULog handle to write the results into.
 * @param resource_name : The name of the resource to dump
 */
void npa_dump( ULogHandle dump_log, const char *resource_name ) 
{
  npa_link *link;

  npa_lock_graph();
  ULOG_RT_PRINTF_0( dump_log, "npa_dump" );

  for ( link = npa_get_resources(); link; link = link->next )
  {
    if ( !resource_name || 
         ( 0 == strncmp( resource_name, link->name, NPA_MAX_STR_LEN + 1 ) ) )
    {
      npa_dump_link( dump_log, link );
    }
  }

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
  npa_dump_scheduler_control_stats(dump_log);
#endif

  ULOG_RT_PRINTF_0( dump_log, "end npa_dump" );
  npa_unlock_graph();
}


/**
 * <!-- npa_ulog_diag_plugin -->
 * 
 * @brief Used with the ULog Diag Plugin function, this creates a dump log.
 *
 * Once installed this function generates a "NPA Dump Log" ulog when the ULog 
 * Diag handler plugin command is called with the ULOG_DIAG_PLUGIN_ID_NPADUMP
 * plugin id. 
 *
 * @param command : ULOG_DIAG_PLUGIN_ID_NPADUMP is the only recognized command
 * currently. 
 *
 * @return DAL_SUCCESS if the command is supported
 */
int npa_ulog_diag_plugin( uint32 command )
{
  int ret = DAL_ERROR;

  ULogHandle dump_log;
  ULogFront_RealTimeInit( &dump_log, "NPA Plugin Log",
                          0x10000, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS );

  switch ( command )
  {
    case NPA_GENERATE_DUMP_LOG:
      if ( dump_log )
      {
        npa_dump( dump_log, NULL );
        ret = DAL_SUCCESS;
      }
      break;

#ifdef NPA_SELF_TEST
    case NPA_RUN_SELF_TEST:
      if ( dump_log )
      {
        npa_test();
        npa_scheduler_profile_test( dump_log );
        ret_val = DAL_SUCCESS;
      }
      break;
#endif

    default:
      /* Unknown command */
      break;
  }

  return ret;
}
