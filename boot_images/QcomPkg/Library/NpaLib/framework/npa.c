/*============================================================================
@file npa.c

Init routines and some common functions used across other files in NPA.

Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

============================================================================*/
#include "npa_init.h"
#include "npa_internal.h"
#include "npa_log.h"
#include "CoreWorkContainer.h"

#ifdef NPA_USES_CORE_PROPERTY
#include "CoreProperty.h"
#endif /* NPA_USES_CORE_PROPERTY */

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
#include "npa_scheduler_internal.h"
#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */

#ifdef WINSIM
#include "npa_offline.h"
#include "npa_offline_dal.h"
#endif /* WINSIM */

#ifdef NPA_RUNTIME_DUMP_LOG
#include <npa_dump.h>
#endif /* NPA_RUNTIME_DUMP_LOG*/

#ifdef __cplusplus
extern "C" {
#endif
#include "CoreString.h"


/* node, resource definition, and driver used for "in_stm" resource */
static
npa_resource_state npa_in_stm_toggle( npa_resource      *resource,
                                      npa_client        *client,
                                      npa_resource_state state )
{
  if ( client->type != NPA_CLIENT_INITIALIZE && 0 == state )
  {
    /* signal the event workloop */
    Core_WorkQueueSignalWorkLoop( npa.npa_event_queue_handle );
  }
  /* no else, do nothing upon entering this state.
     When the event workloop is normally signaled, the state of
     this resource will be queried.  If it is 0, then will signal
     workloop 
 */
  return state;
}

/* The in stm toggle resource */
static npa_resource_definition npa_in_stm_resource_def[] =
{
  {
    "/npa/in_stm",
    "toggle",
    1,
    &npa_identity_plugin,
    NPA_RESOURCE_SINGLE_CLIENT,
    NULL,   /* User data field  */
    NULL,   /* Query Fcn */
    NULL,   /* Query Link Fcn*/    
    NULL,	/* Handle */    
  }
};

/* The in stm toggle node */
npa_node_definition npa_in_stm_node = 
{ 
  "/npa/in_stm",
  npa_in_stm_toggle, 
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(npa_in_stm_resource_def)
};

/*
  Configuration initialization interface
*/

#ifndef NPA_USES_CORE_PROPERTY
/* DEVCFG not defined so need to access config data directly */
extern const npa_config npa_config_data;

#else
/* Define DEVCFG structure info for npa_config_data */
CorePropertyStructure npa_config_data_property_struct[] =
{
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, log_buffer_size),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, resource_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, resource_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, client_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, client_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, event_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, event_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, link_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, link_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, event_queue_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, event_queue_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, async_request_workloop.num_events),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, async_request_workloop.priority),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, async_event_workloop.num_events),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_config, async_event_workloop.priority),
  {NULL} /* last entry */
};
#endif /* !NPA_USES_CORE_PROPERTY */


/**
 * <!-- npa_target_config -->
 *
 * @brief Get NPA Configuration table used by NPA for initialization.
 *  
 * If NPA_USES_CORE_PROPERTY is defined, then use Core's
 * GetProperty routines to get the data from the DEVCFG interface.
 * If NPA_USES_CORE_PROPERTY is not defined, then the SCONSCRIPT
 * will have linked in the data directly into the NPA image so return
 * the data directly.
 *
 * @return : Pointer to an npa_config table
 */
static const npa_config *npa_target_config( void )
{
#ifdef NPA_USES_CORE_PROPERTY
  const npa_config *config_data = NULL;
  static npa_config final_data;
  int count;

  /* check to see if there is Target Specific config data */
  if ( CORE_PROPERTY_SUCCESS ==
        CoreProperty_GetPtr("/dev/power/npa/target/config",
                            "config_data",
                            (const void **) &config_data))
  { /* have target specific data and it is valid  This trump all else */
    /* make sure we have a valid pointer */
    CORE_VERIFY_PTR(config_data);

    /* last entry should point to its self */
    CORE_VERIFY(config_data == config_data->table_end);
    ULOG_RT_PRINTF_1( npa.log,
          "npa_target_config:"
          " General NPA Target Config structure found: 0x%08x",
          config_data );
    /* copy across to a writable location */
    core_memcpy(&final_data, sizeof(final_data), config_data, sizeof(config_data));
  }
  else
  { /* Don't have Target specific config so get general config data */
    /* check to see if there is a config table and
     * get the pointer if it exists.  base_config will have 
     * its old value if there is not a match.
     */
    CoreProperty_GetPtr("/dev/power/npa/config",
                        "config_data",
                        (const void **) &config_data);

    /* make sure we have a valid pointer */
    CORE_VERIFY_PTR(config_data);

    /* last entry should point to its self */
    CORE_VERIFY_PTR(config_data == config_data->table_end);

    ULOG_RT_PRINTF_1( npa.log,
          "npa_target_config:"
          " General NPA Target Individual Config structure found: 0x%08x",
          config_data );

    /* copy across to a writable location */
    core_memcpy(&final_data,sizeof(final_data), config_data, sizeof(config_data));

    /* check to see if there are any extra overwrites to this data
     * dictated by the Target code
     */
    count = CoreProperty_UpdateStructure("/dev/power/npa/target/config",
                                         npa_config_data_property_struct,
                                         (void *) &final_data);
    if (count > 0 && count <= CORE_PROPERTY_NO_ERROR)
    { /* updated a property */
      ULOG_RT_PRINTF_0( npa.log,
          "npa_target_config: Individual NPA Target Config found" );
    }
  }

  final_data.table_end = &final_data; /* proper table termination */

  return &final_data;
#else /* !NPA_USES_CORE_PROPERTY */
      /* last entry should point to its self */
  CORE_VERIFY(&npa_config_data == npa_config_data.table_end);
  return &npa_config_data;
#endif /* NPA_USES_CORE_PROPERTY */
}


/**
 * <!-- npa_init_pool -->
 *
 * @brief Used to setup pools for things like resources, clients, internal data, events, and links
 *
 * @param pool: Pointer where the completed pool will be placed.
 * @param npa_pool_config: Initial and subsequent number of elements in pool.
 * @param element_size: Size in bytes of the elements. 
 */
void npa_init_pool( CorePoolType          *pool, 
                    const npa_pool_config *config,
                    unsigned int           element_size )
{
  CorePoolConfigType  pool_config;

  CORE_VERIFY_PTR( pool );
  CORE_VERIFY_PTR( config );

  pool_config.size       = element_size;
  pool_config.initial    = config->initial;
  pool_config.subsequent = config->subsequent;

  Core_PoolConfig( pool, &pool_config );
}


/**
 * <!-- npa_async_event_handler -->
 *
 * @param pData: Pointer to a npa_event_action.
 *
 */
DALResult npa_async_event_handler( void *pData )
{
  npa_event_action  *action = (npa_event_action *)pData;

  CORE_VERIFY_PTR( action );
  CORE_VERIFY_PTR( action->function );
  return action->function( action );
}


#ifndef FEATURE_WINCE_OAL
/**
 * <!-- npa_init_workloops -->
 *
 * @param config_data: Pointer to a npa_event_action.
 *
 */
static void npa_init_workloops( const npa_config *config_data )
{
  if ( config_data->async_event_workloop.num_events )
  {
    CORE_DAL_VERIFY( 
      Core_RegisterContainerWorkLoopEx( "NPA_ASYNC_EVENT_WL",
                                        NPA_DEFAULT_WL_STACKSIZE,
                                        config_data->async_event_workloop.priority, 
                                        &npa.npa_event_handle ) );

    CORE_DAL_VERIFY( 
      Core_WorkQueueCreate( &npa.npa_event_queue_handle, 
                            npa_async_event_handler,
                            config_data->event_queue_allocation.initial,
                            config_data->event_queue_allocation.subsequent, 
                            sizeof(npa_event_action) ) );
      
    CORE_DAL_VERIFY( 
      Core_AddContainerToWorkLoop( npa.npa_event_queue_handle, npa.npa_event_handle ) );
  }

  
  if ( config_data->async_request_workloop.num_events )
  {
    /* Create async request Workloop */
    CORE_DAL_VERIFY(  
      DALSYS_RegisterWorkLoopEx( "NPA_ASYNC_REQUEST_WL", /* workloop name */
                                 NPA_DEFAULT_WL_STACKSIZE,
                                 config_data->async_request_workloop.priority, 
                                 config_data->async_request_workloop.num_events,
                                 &npa.async_request_handle,
                                 NULL ) );
  }
}
#endif /* !FEATURE_WINCE_OAL */

/**
 * <!-- npa_in_stm_node_created -->
 *
 * Callback invoked when the "/npa/in_stm" node is created
 * @param: ignored
 *
 */
void npa_in_stm_node_created ( void *context, unsigned event_type, 
                               void *data, unsigned data_size )
{
  npa.in_stm_resource = npa_in_stm_node.resources[0].handle;
}

/* External target-specific function  */
extern void npa_target_init( void );

/* The /dev/null node */
extern npa_node_definition null_node;

/* Node to control CoreMutex lockless mode */
extern npa_node_definition coremutex_lockless_node;

/** Vector logging limit */
unsigned int npa_vector_logging_limit = 0;

/** The main NPA data structure */
npa_struct npa;


/**
 * <!-- npa_init -->
 *
 * @brief Initialize config, dalsys, locks, logging, memory pools, and the graph.
 */
void npa_init( void )
{
  ULogResult ulog_result;
  /* If the log doesn't exist, we're not initialized. */

  if ( NULL == npa.log )           
  {
    DALSYSConfig syscfg;

    memset(&syscfg, 0, sizeof(syscfg));
    const npa_config *npa_config_data = npa_target_config();

    CORE_VERIFY_PTR( npa_config_data );

    /* Initialize DALSYS */
    DALSYS_InitMod(&syscfg);

    /* Create NPA locks */
    CORE_VERIFY_PTR( npa.graph_lock = 
        Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
    CORE_VERIFY_PTR( npa.event_lock =
        Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
    
    /* Initialize logging system */
    ulog_result = ULogFront_RealTimeInit(&npa.log,
                                         NPA_DEFAULT_LOG_NAME,
                                         npa_config_data->log_buffer_size,
                                         ULOG_MEMORY_LOCAL,
                                         ULOG_LOCK_OS );

    CORE_VERIFY(DAL_SUCCESS == ulog_result
                || (ULOG_ERR_INITINCOMPLETE == ulog_result
                    && 0 == npa_config_data->log_buffer_size));

    /* Initialize memory pools */
    npa_init_pool( &npa.resource_pool, 
                   &npa_config_data->resource_allocation, 
                   sizeof(npa_resource) );

    /* every resource has an interal data area so use same counts */
    npa_init_pool( &npa.internal_pool, 
                   &npa_config_data->resource_allocation,
                   sizeof(npa_resource_internal_data) );

    npa_init_pool( &npa.client_pool, 
                   &npa_config_data->client_allocation,
                   sizeof(npa_client) );

    npa_init_pool( &npa.event_pool,
                   &npa_config_data->event_allocation,
                   sizeof(npa_event) );

    npa_init_pool( &npa.link_pool,
                   &npa_config_data->link_allocation,
                   sizeof(npa_link) );

    npa_list_Init(&npa_config_data->list_allocation);

    npa_transaction_init();

    /* TODO: Perhaps this should be featurized separately  */
#ifndef FEATURE_WINCE_OAL
    npa_init_workloops( npa_config_data );
#endif /* !FEATURE_WINCE_OAL */

    /* Add /dev/null */
    npa_define_node( &null_node, NULL, NULL );

    /* Add node for lockless mode control */
    npa_define_node( &coremutex_lockless_node, NULL, NULL );
    /* Remove logging for the lockless mode resource so that locks will
       not be applied when logging a request to the resource. The resource
       itself logs something equivalent from within its driver fcn */
    CORE_DAL_VERIFY( npa_remove_resource_log_by_handle( "NPA Log", 
          coremutex_lockless_node.resources[0].handle ) );
    /* Add node for controlling event processing signaling */
    npa_define_node_cb( &npa_in_stm_node, NULL, 
                        npa_in_stm_node_created, NULL );
    /* Initialize target nodes */
    npa_target_init();

#ifdef NPA_RUNTIME_DUMP_LOG
    ULogDiagAddPlugin( npa_ulog_diag_plugin, (uint32) ULOG_DIAG_PLUGIN_ID_NPA );
#endif
  }

  return;
}

/*
  Utility and common routines
*/

/** 
 * <!-- npa_calc_duration -->
 *
 * @brief Calculate the difference between two times.
 *
 * Optionally, update stats.
 *
 * @param start, end: The two time stamps
 * @param dur_ptr: Pointer to a npa_duration structure, can be NULL
 *
 * @return: end time - start time
 */
uint64 npa_calc_duration( uint64 start, uint64 end, npa_duration *dur_ptr )
{
  uint64 diff = end - start;

  if ( dur_ptr )
  {
    if (diff >= dur_ptr->max)
    { /* record the max */
      dur_ptr->max = diff;
    }
    ++dur_ptr->count;
    dur_ptr->total += diff;
  }
  return diff;
}


/**
 * <!-- npa_new_continuation -->
 *
 * @brief Get an new continuation from the pool, and initialize it. 
 *
 * @param callback: Function to call when the continuation is processed.
 * @param context: Data passed to the callback. 
 */
npa_continuation *npa_new_continuation( npa_callback  callback, 
                                        npa_user_data context )
{
  npa_continuation *continuation = 
      (npa_continuation *)Core_PoolAlloc( &npa.continuation_pool );
  CORE_VERIFY_PTR( continuation );

  memset( continuation, 0, sizeof( npa_continuation ) );
  continuation->callback.callback = callback;
  continuation->callback.context  = context;

  return continuation;
}


/**
 * <!-- npa_destroy_continuation -->
 *
 * @brief return the passed in continuation to the pool.
 *
 * @param continuation: Pointer to the continuation that is no longer in use. 
 */
void npa_destroy_continuation( npa_continuation *continuation )
{
  if ( NULL != continuation )
  {
    Core_PoolFree( &npa.continuation_pool, continuation );
  }
}


/**
 * <!-- npa_find_extension -->
 *
 * @brief Traverse the exension linked list looking for the extension id. 
 *
 * @param extension: The starting point of the list to traverse.
 * @param extension_id: The id to look for. 
 *
 * @return The found extension or NULL if not found. 
 */
npa_user_data npa_find_extension( npa_extension_header *extension, 
                                  unsigned int          extension_id  )
{
  while ( ( NULL != extension ) && 
          ( extension_id != extension->extension_id ) )
  {
    extension = (npa_extension_header *)extension->next_extension;
  }
  return extension;
}


/**
 * <!-- npa_dal_event_callback -->
 *
 * @brief Callback function that translates npa callbacks into dal_event triggers.
 *
 * This function can be registered as the NPA callback, with the
 * DALSYSEventHandle as the context argument, and when invoked it will
 * trigger the dal event.  The other input arguments will be passed on to
 * the dal event without translation.
 *
 * @param dal_event_handle: dal_event_handle (passed in as context)
 * @param event_type: int
 * @param data: Pointer to callback data
 * @param data_size: Size of void *
 */
void npa_dal_event_callback( void         *context,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size )
{
  DALSYSEventHandle dal_event = (DALSYSEventHandle)context;
  CORE_VERIFY_PTR( dal_event );
  CORE_DAL_VERIFY( DALSYS_EventCtrlEx( dal_event,
                                       DALSYS_EVENT_CTRL_TRIGGER,
                                       event_type,
                                       (int *)data, 
                                       data_size ) );
  return;
}


/**
 * <!-- npa_timer_init -->
 *
 * @brief Sets up a marker to indicate to NPA that the timer subsystem is up. 
 * 
 * Set up in NPA's scons and invoked from rcinit.
 */
void npa_timer_init( void )
{ 
  ULOG_RT_PRINTF_0( npa.log, "npa_timer_init" );

  /* Let all of NPA know that timer is up */
  npa_define_marker("/init/timer");
}


/**
 * <!-- npa_sclk_to_qclk -->
 *
 * @brief Convert time from sclks to qtimer ticks
 */
uint64 npa_sclk_to_qclk( npa_resource_time time )
{
#ifdef WINSIM
  return time;
#else
  /* B family */
  uint64 time_qclk = (uint64)time;
  /* The factor to convert 32kHz clock to 19.2MHz is 586 */
  return ( time_qclk * 586 );
#endif /* WINSIM */
}

#ifdef __cplusplus
}
#endif
