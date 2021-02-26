/*============================================================================
@file npa_dump.c

Functions to dump states and structures to a log. 

Copyright (c) 2012-2014  Qualcomm Technologies, Inc.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

============================================================================*/
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

  ULOG_RT_PRINTF_2(log, "npa_alias (alias_name: %s) (resource_name: %s)", 
                   link->name , 
                   definition->name );
}


/**
 * <!-- npa_dump_resource_state -->
 *
 * @brief Dump a resource's useful names, states, and status.
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource.
 */
static void npa_dump_resource_state( ULogHandle log, 
                                      npa_resource *resource )
{
  npa_resource_definition *definition;
  npa_resource_internal_data *internal_data;
  npa_duration *duration_data;
  npa_client *current_client_ptr;


  definition = resource->definition;

  ULOG_RT_PRINTF_10(log, 
                   "npa_resource (name: %s) "
                   "(handle: 0x%08X) "
                   "(sequence: 0x%08X) "
                   "(units: %s) "
                   "(resource max: %u) "
                   "(active max: %u) "
                   "(active state %u) "
                   "(active headroom: %d) "
                   "(request state: %u) "
                   "(resource_attributes: 0x%08X) ",
                   definition->name,
                   (UINTN)resource,
                   resource->sequence,
                   definition->units,
                   definition->max,
                   resource->active_max,
                   resource->active_state,
                   resource->active_headroom,
                   resource->request_state,
                   definition->attributes);

  current_client_ptr = resource->current_client;

  /**
   * If there is a current client add it to the dump.  Not expecting this 
   * to be printed out very often because both the driver and npa_dump lock
   * operate on a locked resource. This is more valuable for the JTAG npa dump
   * where the system may be locked or crashed while in the driver. 
   */
  if (current_client_ptr != NULL)
  {
    ULOG_RT_PRINTF_2(log, 
                   "current client (name: %s) "
                   "(handle: 0x%08X) ",
                   current_client_ptr->name,
                   current_client_ptr
                   );
  }

  internal_data = resource->_internal;
  if(NULL == internal_data)
  {
    ULOG_RT_PRINTF_3(log,
                     "\t(node_lock: %08X) "
                     "(event_lock: %08X) "
                     "(_internal: %08X) ",
                     (UINTN)resource->node_lock,
                     (UINTN)resource->event_lock,
                     (UINTN)internal_data );
  }
  else
  {
    duration_data = &(internal_data->stats.driver_duration);
    ULogFront_RealTimePrintf(log, 3,
                              "\t(node_lock: %08X) "
                              "(event_lock: %08X) "
                              "(_internal: %08X) ",
                              (UINTN)resource->node_lock,
                              (UINTN)resource->event_lock,
                              (UINTN)internal_data);

    ULogFront_RealTimePrintf(log, 10,
                             "\t(driver_dur: (max: %lu) "
                             "(count: %llu) "
                             "(total: %llu))",
                             ULOG64_LOWWORD( duration_data->max ),
                             ULOG64_DATA( duration_data->count ),
                             ULOG64_DATA( duration_data->total ) );
  }
}


/**
 * <!-- npa_dump_resource_clients -->
 *
 * @brief Walk the client list, and dump them to the log.
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource. 
 */
static void npa_dump_resource_clients( ULogHandle log, 
                                       npa_resource *resource )
{
  npa_client *client;
  npa_resource_definition *definition;

  client = resource->clients;
  definition = resource->definition;

  while ( NULL != client )
  {
    ULOG_RT_PRINTF_6(log, 
                     "\tnpa_client (name: %s) "
                     "(handle: 0x%08x) "
                     "(resource: %s) "
                     "(type: %u) "
                     "(sequence: 0x%08X) "
                     "(request: %u)", 
                     client->name,
                     (UINTN)client,
                     definition->name,
                     client->type,
                     client->sequence,
                     client->work[client->index].state);

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
    if (client->request_ptr)
    { /* have Scheduler information too */
      ULOG_RT_PRINTF_9(log, 
                       "\t\trequest "
                       "(handle: 0x%08x) "
                       "(prev: 0x%08x) "
                       "(next: 0x%08x) "
                       "(request_time: 0x%08x) "
                       "(request_time: 0x%08x) "
                       "(notification: 0x%08x) "
                       "(on_time: %lu) "
                       "(late: %lu) "
                       "(forked: %lu)",
                       client->request_ptr,
                       client->request_ptr->prev,
                       client->request_ptr->next,
                       client->request_ptr->request_time,
                       client->request_ptr->start_time,
                       client->request_ptr->notification,
                       client->request_ptr->stats.on_time,
                       client->request_ptr->stats.late,
                       client->request_ptr->stats.forked
                       );
    }
#endif
    client = client->next;
  }
}


/**
 * <!-- npa_dump_reserved_event -->
 *
 * @brief Compare the event's callback fcn ptr vs the contiuation functions to determine/dump the event type.
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
    ULOG_RT_PRINTF_1(log, "\tnode dependency (node: %s)",
                     continuation->data.node.definition->name );
  }
  else if ( npa_alias_resource_continuation == event->callback.callback )
  {
    continuation = (npa_continuation *)event->callback.context;
    ULOG_RT_PRINTF_1(log, "\talias dependency (alias: %s)",
                     continuation->data.alias.alias_name );

  }
  else if ( npa_resources_available_continuation == event->callback.callback  )
  {
    continuation = (npa_continuation *)event->callback.context;
    ULOG_RT_PRINTF_2(log, "\tresources available callback (callback: %p)(context: %p)",
                      continuation->callback.callback,
                      continuation->callback.context );
  }
  else 
  {
    ULOG_RT_PRINTF_2(log, "\tresource available callback (callback: %p)(context: %p)",
                      event->callback.callback,
                      event->callback.context );
  }
}


/**
 * <!-- npa_dump_resource_events -->
 *
 * @brief Walk the resource's event linked list, dumping each element's info to the log. 
 *
 * @param log : ULog handle to write the results into.
 * @param resource : Pointer to a npa_resource. 
 */
static void npa_dump_resource_events( ULogHandle log, 
                                      npa_resource *resource )
{
  npa_event *event;

  event = (resource->definition) ? resource->events.event_header->event:
                                   resource->events.event;

  while (NULL != event)
  {
    switch( event->trigger_type )
    {
      case NPA_TRIGGER_RESERVED_EVENT_0:
        npa_dump_reserved_event( log, event );
        break;
      case NPA_TRIGGER_CHANGE_EVENT:
        ULOG_RT_PRINTF_3(log, 
                         "\tnpa_change_event (name: %s) "
                         "(handle: 0x%08x) (resource: %s)", 
                         event->name,
                         event,
                         event->resource_name );
        break;
      case NPA_TRIGGER_WATERMARK_EVENT:
        ULOG_RT_PRINTF_5(log, 
                         "\tnpa_watermark_event (name: %s) (handle: 0x%08X) "
                         "(resource: %s) (lo_watermark: %u) (hi_watermark: %u)", 
                         event->name,
                         event,
                         event->resource_name, 
                         event->lo.watermark,
                         event->hi.watermark );
        break;
      case NPA_TRIGGER_THRESHOLD_EVENT:
        ULOG_RT_PRINTF_5(log, 
                         "\tnpa_threshold_event (name: %s) (handle: 0x%08X) "
                         "(resource: %s) (lo_threshold: %u) (hi_threshold: %u)", 
                         event->name,
                         event,
                         event->resource_name, 
                         event->lo.threshold,
                         event->hi.threshold );
        break;
      default:
        // TODO: there's new event types in development (system event,
        // etc) that should eventually be handled
        ULOG_RT_PRINTF_4(log, 
                         "\tnpa_event (name: %s) (handle: 0x%08X) "
                         "(resource: %s) (type: %d)", 
                         event->name,
                         event,
                         event->resource_name, 
                         event->trigger_type );
        break;
    }

    event = event->next;
  }
}


/**
 * <!-- npa_dump_placeholder -->
 *
 * @brief If a resource pointer in a link has a definition of NULL this gets called. 
 *
 * @param log : ULog handle to write the results into.
 * @param link : Pointer to a npa_link structure
 */
static void npa_dump_placeholder( ULogHandle log, npa_link *link )
{
  npa_resource *resource;

  resource = link->resource;

  ULOG_RT_PRINTF_1(log, "missing resource (name: %s)", 
                   link->name );

  if( resource->events.event ) npa_dump_resource_events( log, resource );
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
  if (resource->node_lock)
  { /* Have a lock so dump state and clients */
    Core_MutexLock( resource->node_lock );
    npa_dump_resource_state(log, resource);
    npa_dump_resource_clients(log, resource);
    Core_MutexUnlock( resource->node_lock );
  }

  if (resource->event_lock)
  { /* Have a lock so dump events */
    Core_MutexLock( resource->event_lock );
    if( resource->events.event_header ) npa_dump_resource_events(log, resource);
    Core_MutexUnlock( resource->event_lock );
  }


  ULOG_RT_PRINTF_1(log, "\tend npa_resource (handle: 0x%08X)", 
                   resource );
}


/**
* <!-- npa_dump_link -->
*
* @brief Identify a link type and call a appropriate dump function. 
*
* @param log : ULog handle to write the results into.
* @param link : Pointer to a npa_link structure
*/
static void npa_dump_link( ULogHandle log, npa_link *link )
{
  npa_resource *resource;
  
  resource = link->resource;

  if ( NULL == resource->definition ) //&definition==0
  {
    npa_dump_placeholder(log, link );
  }
  else
  {  
    if (0 == strncmp(link->name, 
                   resource->definition->name,
                   NPA_MAX_STR_LEN+1 )) //"&resource_name"=="&link_name"
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
  ULOG_RT_PRINTF_5(log, 
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
                   npa_scheduler.stats.forked);
}
#endif


/**
 * <!-- npa_dump -->
 * 
 * @brief Walk the npa resources list, find a match, and dump the details. 
 *
 * Also dump scheduler info if the system uses this feature. 
 *
 * @param dump_log : ULog handle to write the results into.
 * @param resource_name : The ASCII name of the resource to dump
 */
void npa_dump( ULogHandle dump_log, const char *resource_name ) 
{
  npa_link *link;

  npa_lock_graph();
  ULOG_RT_PRINTF_0( dump_log, "npa_dump" );

  for ( link = npa_get_resources(); NULL != link; link = link->next )
  {
    if ( ( NULL == resource_name ) || 
         ( 0 == strncmp( resource_name, 
                         link->name,
                         NPA_MAX_STR_LEN + 1) ) )
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
 * plugin id number. 
 *
 * @param command : ULOG_DIAG_PLUGIN_ID_NPADUMP is the only recognized command
 * currently. 
 *
 * @return DAL_SUCCESS if the command is supported
 */
int npa_ulog_diag_plugin( uint32 command )
{
  int ret_val = DAL_ERROR;
  ULogHandle dump_log;
  switch( command )
  {
    case NPA_GENERATE_DUMP_LOG:
      ULogFront_RealTimeInit( &dump_log,
                              "NPA Dump Log",
                              0x8000,
                              ULOG_MEMORY_LOCAL,
                              ULOG_LOCK_OS );

      if( dump_log != NULL )
      {
        npa_dump( dump_log, NULL );
        ret_val = DAL_SUCCESS;
      }
      else
      {
        ret_val = DAL_ERROR;
      }
      break;
    default:
      //unrecognized command
      ret_val = DAL_ERROR;
      break;
  }
  return ret_val;
}
