/*============================================================================
@file npa_event.c

Routines to create, destroy and manage NPA events

Copyright (c) 2009-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * <!-- npa_event_type_name -->
 *
 * @brief Converts trigger_type enum to a string
 *
 * @param trigger_type : An npa_event_trigger_type enum
 *
 * @return A ptr to the corresponding string
 */
static const char *npa_event_type_name( npa_event_trigger_type trigger_type )
{ 
  switch (trigger_type) 
  {
  case NPA_TRIGGER_RESERVED_EVENT_0:  return "NPA_TRIGGER_RESERVED_EVENT_0";
  case NPA_TRIGGER_RESERVED_EVENT_1:  return "NPA_TRIGGER_RESERVED_EVENT_1";
  case NPA_TRIGGER_RESERVED_EVENT_2:  return "NPA_TRIGGER_RESERVED_EVENT_2";
  case NPA_TRIGGER_CHANGE_EVENT:      return "NPA_TRIGGER_CHANGE_EVENT";
  case NPA_TRIGGER_WATERMARK_EVENT:   return "NPA_TRIGGER_WATERMARK_EVENT";
  case NPA_TRIGGER_THRESHOLD_EVENT:   return "NPA_TRIGGER_THRESHOLD_EVENT";

  default:                            return "unknown event trigger type";
  }
}


/**
 * <!-- npa_trigger_callback_event -->
 *
 * @brief Call the callback with it's context, the event_type, data, 
 * and data_size as arguments. 
 *
 * @param cb_event : Pointer to a npa_callback_event
 * @param event_type : An npa_event_type value.
 * @param data : Pointer to data appropriate to be passed to the callback
 * @param data_size : Size of data in bytes.
 */
void npa_trigger_callback_event( npa_callback_event *cb_event, 
                                 unsigned int        event_type, 
                                 void               *data,       
                                 unsigned int        data_size )
{
  CORE_VERIFY_PTR( cb_event );
  cb_event->callback( cb_event->context, event_type, data, data_size );
}


/**
 * <!-- npa_event_action_trigger_system_event -->
 *
 * @brief Calls the cb of system events on a resource
 *
 * This function calls the event callback of all system events created for a
 * given resource.  These events are for when a change event, watermark event,
 * and threshold event are created/modified/destroyed on a given resource.
 *
 * @param action : The event action created by the trigger of the system event.
 * Contains info on the triggering event type.
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_event_action_trigger_system_event( npa_event_action *action )
{
  npa_event     *event, *sys_event;
  npa_resource  *resource;
  npa_event     *trigger_event;

  CORE_VERIFY_PTR( trigger_event = action->state.event );
  CORE_VERIFY_PTR( resource = trigger_event->resource );
  CORE_VERIFY_PTR( resource->events.event_header );
  CORE_VERIFY_PTR( sys_event = resource->events.event_header->sys_event );

  Core_MutexLock( resource->event_lock );   

  /* This is the initial trigger of a system event */
  if( NPA_TRIGGER_SYSTEM_EVENT == trigger_event->trigger_type )
  {
    /* Scan the event list for all change and threshold events created before
       this system event and trigger the callback once for each */
    event = resource->events.event_header->event;
    while ( NULL != event )
    {
      /* Trigger the system event callback only when the change or threshold 
         event is enabled and not to be destroyed */
      if( ( NPA_TRIGGER_CHANGE_EVENT == event->trigger_type ) || 
          ( NPA_TRIGGER_WATERMARK_EVENT == event->trigger_type ) || 
          ( NPA_TRIGGER_THRESHOLD_EVENT == event->trigger_type ) )
      {
        npa_trigger_callback_event( &trigger_event->callback,
                                    NPA_EVENT_SYSTEM,
                                    event, 
                                    sizeof( npa_event ) );
      }
      event = event->next;
    }
  }
  /* This trigger is for any modification to a change or threshold event:
     1) create or destroy a change event
     2) enable, disable or destroy a threshold event */
  else
  {
    npa_event_trigger_type trigger_type =
      (npa_event_trigger_type)
        (trigger_event->trigger_type & NPA_TRIGGER_TYPE_MASK);
    CORE_VERIFY( ( NPA_TRIGGER_CHANGE_EVENT == trigger_type ) || 
                 ( NPA_TRIGGER_WATERMARK_EVENT == trigger_type ) || 
                 ( NPA_TRIGGER_THRESHOLD_EVENT == trigger_type ) );
    /* Scan for all valid system events and trigger the associated callbacks */
    while ( NULL != sys_event )
    {
      /* No disable or destoryed flag set */
      if( NPA_TRIGGER_SYSTEM_EVENT == sys_event->trigger_type ) 
      {
        npa_trigger_callback_event( &sys_event->callback,
                                    NPA_EVENT_SYSTEM,
                                    trigger_event,
                                    sizeof( npa_event ) );
      }
      sys_event = sys_event->next;
    }
  }

  Core_MutexUnlock( resource->event_lock ); 

  return DAL_SUCCESS;
}


/**
 * <!-- npa_event_action_trigger_sync_event -->
 *
 * @brief Trigger a sync event
 *
 * Useful for proxy resources for keeping both resources in sync.
 *
 * @param action : An npa_event_action
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_event_action_trigger_sync_event( struct npa_event_action *action )
{
  npa_resource           *resource;
  npa_resource_state     *data = NULL;
  unsigned int            data_size = 0;
  npa_event              *sync_event;
  npa_sync_event_trigger *trigger;

  CORE_VERIFY( trigger = &action->state.sync_trigger );
  CORE_VERIFY_PTR( sync_event = trigger->event );
  CORE_VERIFY_PTR( resource = sync_event->resource );
  
  if ( trigger->valid )
  {
    data = &trigger->state;
    data_size = sizeof( npa_resource_state );
  }
    
  Core_MutexLock( resource->event_lock ); 
  npa_trigger_callback_event( &sync_event->callback,
                              NPA_EVENT_SYNC,
                              data,
                              data_size );
  Core_MutexUnlock( resource->event_lock ); 

  return DAL_SUCCESS;
}


/**
 * <!-- npa_event_action_new_event -->
 *
 * @brief Adds new event to a resource event list
 *
 * This function is called after a new event is created and links the event
 * to the resource event list.
 *
 * @param action : An npa_event_action
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_event_action_new_event( npa_event_action *action )
{
  npa_resource      *resource;
  npa_event_header  *event_header;
  npa_event        **head_event_pptr;
  npa_event         *event;

  CORE_VERIFY_PTR( event = action->state.event );
  CORE_VERIFY_PTR( resource = event->resource );
  CORE_VERIFY_PTR( event_header = resource->events.event_header );

  if( NPA_TRIGGER_SYSTEM_EVENT != event->trigger_type )
    head_event_pptr = &event_header->event;
  else
    head_event_pptr = &event_header->sys_event;
  
  Core_MutexLock( resource->event_lock );
  event->next = *head_event_pptr;
  if( NULL != *head_event_pptr )
  {
    ( *head_event_pptr )->prev = event;
  }
  ( *head_event_pptr ) = event;
  Core_MutexUnlock( resource->event_lock );
  
  return DAL_SUCCESS;
}


/**
 * <!-- npa_event_action_set_watermark -->
 *
 * @brief Set watermarks for a resource event.
 *
 * This is an internal function called by npa_set_event_watermarks that
 * actually applies the watermarks for a given resource.  It is processed
 * by the event thread.
 *
 * @param action : Data structure that contains a pointer to the event,
 * the watermark bounds, and the resource state and headroom.
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_event_action_set_watermark( struct npa_event_action *action )
{
  npa_resource      *resource;
  npa_event         *event;
  npa_set_watermark *watermark;

  CORE_VERIFY_PTR( watermark = &action->state.watermark );
  CORE_VERIFY_PTR( event = watermark->event );
  CORE_VERIFY_PTR( resource = event->resource );

  if ( watermark->lo <= watermark->hi ) 
  {
    npa_event_data event_data;

    /* Initialize npa_event_data structure */
    event_data.resource_name = resource->definition->name;
    event_data.state         = watermark->state;
    event_data.headroom      = watermark->headroom;

    /* Trigger the event if it's active */
    Core_MutexLock( resource->event_lock );

    if ( event->trigger_type & (unsigned)NPA_TRIGGER_DESTROY_FLAG ) 
    {
      Core_MutexUnlock( resource->event_lock );
      return DAL_SUCCESS;
    }
    
    CORE_VERIFY( NPA_TRIGGER_WATERMARK_EVENT == 
                 (~NPA_TRIGGER_DISABLED_FLAG & event->trigger_type ) );

    event->hi.watermark = watermark->hi;
    event->lo.watermark = watermark->lo;
    event->trigger_type &= ~NPA_TRIGGER_DISABLED_FLAG;

    if ( event_data.headroom < event->lo.watermark ) 
    {
      npa_trigger_callback_event( &event->callback,
                                  NPA_EVENT_LO_WATERMARK,
                                  &event_data, 
                                  sizeof( npa_event_data ) );
    }
    else if ( event_data.headroom > event->hi.watermark )
    {
      npa_trigger_callback_event( &event->callback,
                                  NPA_EVENT_HI_WATERMARK,
                                  &event_data, 
                                  sizeof( npa_event_data ) );
    }
    Core_MutexUnlock( resource->event_lock );
  }
  else
  {
    Core_MutexLock( resource->event_lock );
    event->hi.watermark = watermark->hi;
    event->lo.watermark = watermark->lo;
    event->trigger_type |= NPA_TRIGGER_DISABLED_FLAG;
    Core_MutexUnlock( resource->event_lock );
  }
  return DAL_SUCCESS;
}


/**
 * <!-- npa_event_action_set_threshold -->
 *
 * @brief Set a threshold for a resource event.
 *
 * This is an internal function called by npa_set_event_thresholds that
 * actually applies the thresholds for a given resource.  It is processed
 * by the event thread.
 *
 * @param action : Data structure that contains a pointer to the event,
 * the threshold bounds, and the resource state and headroom.
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_event_action_set_threshold( struct npa_event_action *action )
{
  npa_event         *event;
  npa_resource      *resource;
  npa_set_threshold *threshold;

  CORE_VERIFY_PTR( threshold = &action->state.threshold );
  CORE_VERIFY_PTR( event = threshold->event );
  CORE_VERIFY_PTR( resource = event->resource );

  if ( threshold->lo <= threshold->hi )
  {
    npa_event_data  event_data;
    npa_event_type  event_type;

    /* Initialize npa_event_data structure */
    event_data.resource_name = resource->definition->name;
    event_data.state         = threshold->state;
    event_data.headroom      = threshold->headroom;

    /* Trigger the event if it's active */
    Core_MutexLock( resource->event_lock );
    
    if ( event->trigger_type & (unsigned)NPA_TRIGGER_DESTROY_FLAG ) 
    {
      Core_MutexUnlock( resource->event_lock );
      return DAL_SUCCESS;
    }
    
    CORE_VERIFY( NPA_TRIGGER_THRESHOLD_EVENT == 
                 (~NPA_TRIGGER_DISABLED_FLAG & event->trigger_type ) );

    event->hi.threshold = threshold->hi;
    event->lo.threshold = threshold->lo;
    event->trigger_type &= ~NPA_TRIGGER_DISABLED_FLAG;

    if ( event_data.state < event->lo.threshold )
    {
      event_type = NPA_EVENT_THRESHOLD_LO;
    }
    else if ( event->hi.threshold < event_data.state  )
    {
      event_type = NPA_EVENT_THRESHOLD_HI;
    }
    else
    {
      event_type = NPA_EVENT_THRESHOLD_NOMINAL;
    }

    npa_trigger_callback_event( &event->callback,
                                event_type,
                                &event_data, 
                                sizeof( npa_event_data ) );
    Core_MutexUnlock( resource->event_lock );
  }
  else
  {
    Core_MutexLock( resource->event_lock );
    event->hi.threshold = threshold->hi;
    event->lo.threshold = threshold->lo;
    event->trigger_type |= NPA_TRIGGER_DISABLED_FLAG;
    Core_MutexUnlock( resource->event_lock );
  }

  return DAL_SUCCESS;
}


/**
 * <!-- npa_event_action_destroy -->
 *
 * @brief Destroy an event.
 *
 * This is an internal function called by npa_destroy_event_handle to unlink
 * and free an event.  It is processed by the event thread.
 *
 * @param action : Data structure containing the event to destroy.
 *
 * @return DAL_SUCCESS if the destroy was successfully completed.
 */
static DALResult 
npa_event_action_destroy( struct npa_event_action *action )
{
  npa_event    *event, *event_ptr, **head_event_pptr;
  npa_resource *resource;

  CORE_VERIFY_PTR( event = action->state.event );
  CORE_VERIFY_PTR( resource = event->resource );

  if( NULL == resource->events.event_header ) return DAL_ERROR;

  if( NPA_TRIGGER_SYSTEM_EVENT != event->trigger_type )
    head_event_pptr = &resource->events.event_header->event;
  else
    head_event_pptr = &resource->events.event_header->sys_event;
  event_ptr = *head_event_pptr;

  while( ( event_ptr != NULL ) && ( event_ptr != event) )
    event_ptr = event_ptr->next;
  /* Event does not exist or is already destroyed */
  if( NULL == event_ptr ) return DAL_ERROR;

  CORE_VERIFY( event->trigger_type & (unsigned)NPA_TRIGGER_DESTROY_FLAG );

  Core_MutexLock( resource->event_lock );
  /* Remove from element from the list */
  if ( NULL != event->prev )
  {
    event->prev->next = event->next;
  }
  if ( NULL != event->next )
  {
    event->next->prev = event->prev;
  }
  /* Check to see if it's the head of either event queue - if so, remove it
   *
   * If we kept track of which kind of event it was, we'd only have to
   * look at that queue, but that state is not currently being cached
   * by the event. */
  if ( event == *head_event_pptr )
  {
    ( *head_event_pptr ) = event->next;
  }

  Core_MutexUnlock( resource->event_lock );

  Core_PoolFree( &npa.event_pool, event );
  return DAL_SUCCESS;
}


/**
 * <!-- npa_new_event -->
 *
 * @brief Creates a new event.
 *
 * This is an internal function used to create and link an event for a 
 * resource.  If the event type is NPA_TRIGGER_RESOURCE_AVAILABLE it 
 * will create a placeholder to that resource.
 *
 * @param resource_name : Resource to create the event for.
 * @param event_name : Name of event to create.
 * @param trigger_type : The type of event that triggers the event.
 * @param callback : Function to call when the event is triggered.
 * @param context : Data pointer to pass to the function when it is called.
 *
 * @return The new event, else NULL
 */
npa_event *npa_new_event( const char      *resource_name,
                          const char      *event_name,
                          npa_event_trigger_type trigger_type,
                          npa_callback     callback,
                          void            *context )
{
  npa_event_header *event_header = NULL;
  npa_event        *event;
  npa_resource     *resource;
  npa_link         *link = npa_find_resource_link( resource_name );
  npa_event_action *action;
  unsigned          trigger_system_event = 0;

  ULOG_RT_PRINTF_3( npa.log,
                    "npa_create_event "
                    "(resource: \"%s\") (event: \"%s\") (type: \"%s\")",
                    resource_name, event_name,
                    npa_event_type_name( trigger_type ) );
  
  if ( ( NULL == link ) ||
       ( !npa_is_resource_type( link->resource, NPA_INSTANTIATED_RESOURCE ) && 
         NPA_TRIGGER_RESOURCE_AVAILABLE != trigger_type ) )
  {
    /* TODO: need to create a reference copy of the resource_name here */
    ULOG_RT_PRINTF_2( npa.log, 
                      "\tFAILED npa_new_event \"%s\": resource \"%s\" does not exist", 
                      event_name, 
                      resource_name );
    return NULL; 
  }

  if ( NULL == npa.npa_event_queue_handle )
  {
    ULOG_RT_PRINTF_1( npa.log, 
                      "\tFAILED npa_new_event \"%s\": "
                      "NPA target configuration does not support async events",
                      event_name );
//Fix to allow RPM available callback with workloops disabled (FEATURE_WINCE_OAL)
//	return NULL; 

  }

  resource      = link->resource;
  resource_name = link->name;

  /* Can't create events against markers */
  if( NPA_IS_RESOURCE_MARKER(resource) )
  { 
    ULOG_RT_PRINTF_2( npa.log,
                      " Can't create event \"%s\" against marker resource \"%s\"",
                      event_name, resource_name );
    return NULL; 
  }

  /* How should the error be handled? Currently it's fatal... */
  CORE_VERIFY_PTR( resource );
  CORE_VERIFY( strlen( event_name ) <= NPA_MAX_STR_LEN );
  CORE_VERIFY_PTR( callback );
  
  if ( NPA_TRIGGER_RESOURCE_AVAILABLE != trigger_type )
  {
    /* Allocate event list header */
    Core_MutexLock( resource->node_lock );
    if( NULL == resource->events.event_header )
    {
      resource->events.event_header = (npa_event_header *)Core_PoolAlloc( &npa.event_pool );
      CORE_VERIFY_PTR( resource->events.event_header );
      memset( resource->events.event_header, 0, sizeof( npa_event ) );
    }
    Core_MutexUnlock( resource->node_lock );

    event_header = resource->events.event_header;
  }
  
  event = (npa_event *)Core_PoolAlloc( &npa.event_pool );
  CORE_VERIFY_PTR( event );

  memset( event, 0, sizeof( npa_event ) );

  event->name          = event_name;
  event->resource_name = resource_name;
  event->resource      = resource;
  event->trigger_type  = trigger_type;

  event->callback.callback = callback;
  event->callback.context  = context;
  
  switch( trigger_type )
  {
  case NPA_TRIGGER_RESOURCE_AVAILABLE:
    {
      Core_MutexLock( resource->event_lock );
      event->next = resource->events.event;
      if ( NULL != resource->events.event )
      {
        resource->events.event->prev = event;
      }
      resource->events.event = event;
      Core_MutexUnlock( resource->event_lock );

      return event;
    }
  case NPA_TRIGGER_SYSTEM_EVENT:
    {
      CORE_VERIFY_PTR( event_header );
      /* Support only one system event per resource right now */
      if ( 0 < event_header->sys_count ) 
      {
        Core_PoolFree( &npa.event_pool, event );
        return NULL;
      }
      event_header->sys_count++;
      trigger_system_event = 1;
  
      break;
    }
  case NPA_TRIGGER_SYNC_EVENT:
    {
      /* Support only one sync event per resource right now */
      CORE_VERIFY( 0 == event_header->sync_count );
      event_header->sync_count = 1;
  
      break;
    }
  case NPA_TRIGGER_CHANGE_EVENT:
    {
      Core_AtomicInc((volatile uint32_t *)( &event_header->reg_count ));

      /* If there is a system event, trigger it */
      if ( event_header->sys_count ) trigger_system_event = 1;

      break;
    }
  case NPA_TRIGGER_WATERMARK_EVENT:
  case NPA_TRIGGER_THRESHOLD_EVENT:
    {
      Core_AtomicInc((volatile uint32_t *)(&event_header->reg_count ));
      
      event->trigger_type |= NPA_TRIGGER_DISABLED_FLAG;
      
      break;
    }
  default:
    CORE_VERIFY( 0 );
  }

  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  if ( action ) //KW fix
  {
	  action->function = npa_event_action_new_event;
	  action->state.event = event;
	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );

	  if ( trigger_system_event )
	  {
		CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
		if ( action ) //KW fix
		{
			action->function = npa_event_action_trigger_system_event;
			action->state.event = event;
			CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
		}
	  }
  }
  return event;
}


/** 
 * <!-- npa_create_event_cb -->
 * 
 * @brief Create an NPA event handler with the given resource
 *
 * This will return a NULL handle if the resource does not exist or
 * the event handler could not be created.
 *
 * The event_name must be < 64 characters in length.
 *
 * The event_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * The callback pointer must be non-NULL.  When invoked, the callback
 * arguments will be ( context, event_type, npa_event_data *, sizeof(
 * npa_event_data) )
 *
 * NOTE (For Resource Authors): This function should not be called
 * from within a driver or update function unless you know that there
 * will be no other events that might also be attempting to lock the
 * same resource, otherwise the possibility for deadlock exists.  If
 * you are not using the functions from within a resource, there are
 * no usage restrictions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param event_name : Name of the event handler that's being created 
 * @param trigger_type : Type of trigger for event handler to be created 
 * @param callback : Callback to be invoked when event is triggered
 * @param context : Context to be passed back in callback invocation
 *
 * @return New event handle, NULL if the event handle could not be created
 */
npa_event_handle npa_create_event_cb( const char   *resource_name, 
                                      const char   *event_name,
                                      npa_event_trigger_type trigger_type,
                                      npa_callback  callback,
                                      void         *context )
{
  return npa_new_event( resource_name, event_name, 
                        trigger_type,
                        callback, context ); 
}


/** 
 * <!-- npa_destroy_event_handle -->
 *
 * @brief Destroy the event handler
 *   
 * After invocation, the event handle is invalidated and must not be
 * reused
 *
 * NOTE (For Resource Authors): This function should not be called from
 * within a driver or update function unless you know that there will
 * be no other events that might also be attempting to lock the same
 * resource, otherwise the possibility for deadlock exists.  If you are
 * not using the functions from within a resource, there are no usage
 * restrictions.
 *
 * @param event : Handle of event being destroyed
 */
void npa_destroy_event_handle( npa_event_handle event )
{
  npa_resource          *resource;
  npa_event_action      *action;
  npa_event_trigger_type trigger_type;
  
  if( NULL == event ) { return; }

  resource = event->resource;
  CORE_VERIFY_PTR( resource );

  ULOG_RT_PRINTF_2( npa.log,
                    "npa_destroy_event_handle "
                    "(event: \"%s\") (resource: \"%s\")",
                    event->name, resource->definition->name );

  /* Change the type */
  Core_MutexLock( resource->event_lock );
  event->trigger_type |= (unsigned)NPA_TRIGGER_DESTROY_FLAG;
  Core_MutexUnlock( resource->event_lock );

  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  if ( action ) // KW fix
  {
	  action->function = npa_event_action_destroy;
	  action->state.event = event;
	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );

	  trigger_type = ( npa_event_trigger_type )( event->trigger_type & NPA_TRIGGER_TYPE_MASK );
	  if( ( NPA_TRIGGER_CHANGE_EVENT == trigger_type ) ||
		  ( NPA_TRIGGER_WATERMARK_EVENT == trigger_type ) ||
		  ( NPA_TRIGGER_THRESHOLD_EVENT == trigger_type ) )
	  {
		npa_event_header *event_header = resource->events.event_header;
		CORE_VERIFY_PTR( event_header );
		Core_AtomicDec( (uint32_t *) &event_header->reg_count );

		/* If there is a system event, trigger it */
		if( resource->events.event_header->sys_count )
		{
		  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
		  if ( action )
		  {
			  action->function = npa_event_action_trigger_system_event;
			  action->state.event = event;
			  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
		  }
		}
	  }
  }
}

/* TODO: revisit */
#define npa_custom_event_pool npa.event_pool
CORE_STATIC_VERIFY( sizeof(npa_custom_event) < sizeof(npa_event) );


/**
 * <!-- npa_create_custom_event -->
 *
 * @brief  Create a custom event, a (callback, context) tuple that may be
 * arbitrarily posted by resource authors and dispatched in the event workloop
 *
 * @param name: Name of the custom event
 * @param callback: Callback to be invoked on dispatch, with context supplied
 * during post
 * @param data : Data to associate with event at creation; will be 
 * accessible via event->data in callback
 *
 * @return The new event
 */
npa_custom_event* npa_create_custom_event( const char   *name,
                                           npa_callback  callback,
                                           void         *data )
{
  npa_custom_event *event = NULL;
  CORE_VERIFY( strlen(name) <= NPA_MAX_STR_LEN );

  CORE_VERIFY_PTR( 
       event = (npa_custom_event *)Core_PoolAlloc( &npa_custom_event_pool ) );
  
  event->name = name;
  event->callback = callback;
  event->data = data;

  ULOG_RT_PRINTF_1( npa.log,
                    "npa_create_custom_event (event: \"%s\")",
                    name );
  return event;
}


/**
 * <!-- npa_custom_event_action_trigger -->
 *
 * @brief Trigger a custom event.
 *
 * Calls the callback for a custom event.
 *
 * @param action : The npa_event_action to call
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_custom_event_action_trigger( struct npa_event_action *action )
{
  struct npa_custom_event_data *custom;
  CORE_VERIFY_PTR( action );
  
  CORE_VERIFY_PTR( custom = &action->state.custom );

  custom->event->callback( custom->context, NPA_EVENT_CUSTOM,
                           (void *)custom->event->data,
                           sizeof(custom->event->data) );
  return DAL_SUCCESS;
}


/**
 * <!-- npa_post_custom_event -->
 *
 * @brief Post a custom event to npa's npa_event_queue_handle
 *
 * @param event: The custom event
 * @param context: The context to post the event with
 */
void npa_post_custom_event( npa_custom_event *event, void *context )
{
  npa_event_action *action;
  npa_query_type    in_stm_query;
  
  CORE_DAL_VERIFY( 
    Core_WorkQueueAlloc( npa.npa_event_queue_handle, (void **)&action ) );
  CORE_VERIFY_PTR( action ); /* keep klocworks happy */
  if ( action )
  {
	action->function = npa_custom_event_action_trigger;
	action->state.custom.event = event;
	action->state.custom.context = context;

	 npa_resource_lock( npa.in_stm_resource );
	  CORE_VERIFY( 
	       NPA_QUERY_SUCCESS == npa_query_resource( npa.in_stm_resource, 
	                                                NPA_QUERY_CURRENT_STATE, 
	                                                &in_stm_query ) );
	  CORE_DAL_VERIFY( 
	       Core_WorkQueueInsert( npa.npa_event_queue_handle, action, 
	                             CORE_WORKQUEUE_TAIL, FALSE, 
	                             in_stm_query.data.state ? TRUE : FALSE ) );
	  npa_resource_unlock( npa.in_stm_resource );
  }
}


/**
 * <!-- npa_post_custom_event_nodups -->
 *
 * @brief Post a custom event or if one is queued for dispatch already, simply
 * update the event in the dispatch queue
 *
 * @param event: The custom event to either post afresh or update in the
 * dispatch queue
 * @param context: The context to post or update the event with.
 * @param p_action: An opaque handle, representing an event action, that
 * is created when this function is first called and used in all subsequent
 * calls. Must be used as below.
 *
 * npa_event_action *action = NULL; // init to NULL is important
 * npa_post_custom_event_nodups( event, &action );
 * ...
 * npa_post_custom_event_nodups( event2, &action );
 */
void npa_post_custom_event_nodups( npa_custom_event  *event,
                                   void              *context,  
                                   npa_event_action **p_action )
{
  npa_event_action *action;
  npa_query_type    in_stm_query;

  CORE_VERIFY_PTR( p_action );

  if ( *p_action == NULL )
  {
    CORE_DAL_VERIFY(
      Core_WorkQueueAlloc( npa.npa_event_queue_handle, (void **)p_action ) );
    CORE_VERIFY_PTR( *p_action ); /* keep klocworks happy */
  }
  action = *p_action;
  if ( action )
  {
 
	  npa_resource_lock( npa.in_stm_resource );
	  CORE_VERIFY( 
	       NPA_QUERY_SUCCESS == npa_query_resource( npa.in_stm_resource, 
	                                                NPA_QUERY_CURRENT_STATE,
	                                                &in_stm_query ) );

  Core_WorkQueueLock( npa.npa_event_queue_handle );
  
  action->function = npa_custom_event_action_trigger;
  action->state.custom.event = event;
  action->state.custom.context = context;

	  CORE_DAL_VERIFY( 
	       Core_WorkQueueInsert( npa.npa_event_queue_handle, action, 
	                             CORE_WORKQUEUE_TAIL, TRUE, 
	                             in_stm_query.data.state ? TRUE : FALSE ) );

	  Core_WorkQueueUnlock( npa.npa_event_queue_handle );
          npa_resource_unlock( npa.in_stm_resource );
  }
}


/**
 * <!-- npa_custom_event_action_destroy -->
 *
 * @brief Free the npa_event_action to the npa_custom_event_pool
 *
 * @param action : Pointer to a npa_event_action containing an npa_custom_event
 * that is no longer needed. 
 *
 * @return DAL_SUCCESS
 */
static DALResult 
npa_custom_event_action_destroy( struct npa_event_action *action )
{
  CORE_VERIFY_PTR( action );
  CORE_VERIFY_PTR( action->state.custom.event );

  Core_PoolFree( &npa_custom_event_pool, action->state.custom.event );
  return DAL_SUCCESS;
}



/**
 * <!-- npa_destroy_custom_event -->
 *
 * @brief Put a npa_custom_event_action_destroy on the npa event queue to destroy
 * the event asyncronously.
 *
 * npa_custom_event_action_destroy will free the event when it processes the
 * action.  The action allocated by Core_WorkQueueAlloc will be freed by the
 * Core_WorkElementProcess function. 
 *
 * @param event : The npa_custom_event that is no longer needed. 
 */
void npa_destroy_custom_event( npa_custom_event *event )
{
  npa_event_action *action;

  if ( event == NULL ) return;

  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  CORE_VERIFY_PTR( action ); /* keep klocworks happy */
  if ( action )
  {
	  action->function = npa_custom_event_action_destroy;
	  action->state.custom.event = event;
	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
  }
}


/** 
 * <!-- npa_set_event_watermarks -->
 *
 * @brief Set the watermarks for triggering the event handler.
 *
 * Watermarks are defined as the difference between the aggregated
 * request value and the active resource maximum.  Headroom > 0 means the
 * resource is oversubscribed, as the request value is greater than
 * the resource can provide.
 *
 * For example: If the caller wanted to be notified when the resource
 * state rose past qthe hi watermark of (max-20) units or dropped below
 * the low watermark of (max-40) units, the hi and lo watermarks would
 * be set to -20 and -40, respectively
 *
 * If the user wishes to recieve notification when the resource is
 * oversubscribed, set both the hi and lo watermark to zero.  Then the
 * user will receive a NPA_EVENT_HI_WATERMARK notification when the
 * resource is oversubscribed and NPA_EVENT_LO_WATERMARK when the
 * resource is operating normally.
 *  
 * No watermark event will be triggered until the watermarks have been
 * set.  If the lo watermark > hi watermark, no watermark events will
 * be triggered.
 *
 * The event may be triggered during this function if the resource is
 * below the lo watermark or above the hi watermark when this function
 * is invoked. It is only valid to set watermarks on watermark events.
 *  
 * @param event : Handle of event being manipulated
 * @param hi_watermark : Hi watermark setting
 * @param lo_watermark : Lo watermark setting
 */
void npa_set_event_watermarks( npa_event_handle         event,
                               npa_resource_state_delta hi_watermark,
                               npa_resource_state_delta lo_watermark )
{
  npa_event_action *action;
  npa_resource     *resource;

  CORE_VERIFY_PTR( event );
  CORE_VERIFY_PTR( resource = event->resource );
  
  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  if ( action ) //KW
  {
	  action->function = npa_event_action_set_watermark;
	  action->state.watermark.event    = event;
	  action->state.watermark.hi       = hi_watermark;
	  action->state.watermark.lo       = lo_watermark;

	  Core_MutexLock( resource->node_lock );
	  action->state.watermark.state    = resource->active_state;
	  action->state.watermark.headroom = resource->active_headroom;
	  Core_MutexUnlock( resource->node_lock );

	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
  
	  /* If there is a system event, trigger it */
	  if( event->resource->events.event_header->sys_count )
	  {
		CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
		if ( action ) //KW
		{
			action->function = npa_event_action_trigger_system_event;
			action->state.event = event;
			CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
		}
	  }
  }
}


/** 
 * <!-- npa_set_event_thresholds -->
 *
 * @brief Set the absolute thresholds for triggering the event handler
 *
 * If the user wishes to receive notification that the resource's state
 * crosses certain thresholds.  The hi and lo thresholds defines three
 * regions.  It is only valid to set the threshold on threshold events.
 *
 *   Low Region     - state < lo_threshold
 *   Nominal Region - lo_threshold <= state <= hi_threshold
 *   Hi Region      - hi_threshold < state 
 *
 * On every resource state change, if the resource has changed the
 * region it is in, then the user event will be triggered.  Also, the
 * user event will be triggered when the thresholds are initially set
 * to notify which region the resource is in at that time.
 *
 * If lo_threshold > hi_threshold, the event is disabled. 
 *
 * @param event : Handle to event being set
 * @param hi_threshold : Hi threshold setting
 * @param lo_threshold : Lo threshold setting
 */
void npa_set_event_thresholds( npa_event_handle   event,
                               npa_resource_state lo_threshold,
                               npa_resource_state hi_threshold )
{
  npa_event_action *action;
  npa_resource     *resource;

  CORE_VERIFY_PTR( event );
  CORE_VERIFY_PTR( resource = event->resource );
 
  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  if ( action ) //KW
  {
	  action->function = npa_event_action_set_threshold;
	  action->state.threshold.event = event;
	  action->state.threshold.hi    = hi_threshold;
	  action->state.threshold.lo    = lo_threshold;

	  Core_MutexLock( resource->node_lock );
	  action->state.threshold.state    = resource->active_state;
	  action->state.threshold.headroom = resource->active_headroom;
	  Core_MutexUnlock( resource->node_lock );

	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );

	  /* If there is a system event, trigger it */
	  if( event->resource->events.event_header->sys_count )
	  {
		CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
		if ( action ) //KW
		{
			action->function = npa_event_action_trigger_system_event;
			action->state.event = event;
			CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
		}
	  }
  }
}


/** 
 * <!-- npa_post_sync_event -->
 *
 * @brief Post a sync event to event work queue  
 *
 * @param event: Handle to event 
 * @param valid: If the passed-in resource state is valid or not
 * @param state: New resource state
 */
void npa_post_sync_event( npa_event *event, int valid, npa_resource_state state )
{
  npa_event_action *action;
  
  CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );
  if ( action )
  {
	  action->function = npa_event_action_trigger_sync_event;
	  action->state.sync_trigger.event = event;
	  action->state.sync_trigger.valid = valid;
	  action->state.sync_trigger.state = state;
	  CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
  }
}


/**
 * <!-- npa_event_action_trigger -->
 *
 * @brief Trigger an event action  
 * 
 * Internal function called by _npa_update_resource_state whenever
 * there are events for a given resource and the overall aggregation
 * or headroom changes.  This function goes through all of the events
 * on a resource and will post the event's callback to the event WL if
 * the event should be triggered.
 *
 * @param action: Action to post
 *
 * @return DAL_SUCCESS if successful.
 */
DALResult 
npa_event_action_trigger( struct npa_event_action *action )
{
  npa_event_trigger *trigger; 
  npa_event         *event;
  npa_event_data     event_data;
  npa_event_type     change_event    = NPA_EVENT_RESERVED;
  npa_event_type     threshold_event = NPA_EVENT_RESERVED;
  npa_event_type     watermark_event = NPA_EVENT_RESERVED;

  CORE_VERIFY_PTR( trigger = &action->state.trigger );
  CORE_VERIFY_PTR( trigger->resource );
  CORE_VERIFY_PTR( trigger->resource->events.event_header );

  Core_MutexLock( trigger->resource->event_lock ); 

  event = trigger->resource->events.event_header->event;

  /* Initialize npa_event_data structure */
  event_data.resource_name = trigger->resource->definition->name;
  event_data.state         = trigger->new_state;
  event_data.headroom      = trigger->new_headroom;

  if ( trigger->new_state != trigger->old_state )
  {
    change_event = NPA_EVENT_CHANGE;
  }

  /* To trigger watermark events */
  if ( trigger->new_headroom != trigger->old_headroom ) 
  {
    watermark_event =  
      ( trigger->old_headroom > trigger->new_headroom ) ?
      NPA_EVENT_LO_WATERMARK : NPA_EVENT_HI_WATERMARK;
  }

  while ( NULL != event )
  {
    if ( !( event->trigger_type & (unsigned)NPA_TRIGGER_DISABLED_FLAG ) &&
         !( event->trigger_type & (unsigned)NPA_TRIGGER_DESTROY_FLAG ) )
    {
      switch( event->trigger_type & NPA_TRIGGER_TYPE_MASK )
      {
      case NPA_TRIGGER_CHANGE_EVENT:
        /* To trigger change events */
        if ( NPA_EVENT_RESERVED != change_event ) 
        {
          npa_trigger_callback_event( &event->callback,
                                      NPA_EVENT_CHANGE, 
                                      (void *)&event_data, 
                                      sizeof( npa_event_data ) );
        }
        break;

      case NPA_TRIGGER_WATERMARK_EVENT:
        switch (watermark_event)
        {
        case NPA_EVENT_HI_WATERMARK:
          if ( trigger->old_headroom < event->hi.watermark &&
               event->hi.watermark <= trigger->new_headroom )
          {
            npa_trigger_callback_event( &event->callback,
                                        watermark_event,
                                        &event_data, 
                                        sizeof( npa_event_data ) );
          }
          break;
        case NPA_EVENT_LO_WATERMARK:
          if ( trigger->old_headroom > event->lo.watermark &&
               event->lo.watermark >= trigger->new_headroom )
          {
            npa_trigger_callback_event( &event->callback,
                                        watermark_event,
                                        &event_data, 
                                        sizeof( npa_event_data ) );
          }
          break;
          
        default: 
          break;
        }
        break;
    
      case NPA_TRIGGER_THRESHOLD_EVENT:
        if ( NPA_EVENT_RESERVED == change_event ) {break;}
        
        threshold_event = NPA_EVENT_RESERVED;
        if ( ( event_data.state < event->lo.threshold ) &&
             ( trigger->old_state >= event->lo.threshold ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_LO;
        }
        else if ( ( event_data.state > event->hi.threshold ) &&
                  ( trigger->old_state <= event->hi.threshold ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_HI;
        }
        else if ( ( ( event->lo.threshold <= event_data.state ) &&
                    ( event_data.state <= event->hi.threshold ) ) &&
                  ( ( trigger->old_state < event->lo.threshold ) ||
                    ( trigger->old_state > event->hi.threshold ) ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_NOMINAL;
        }
          
        if ( NPA_EVENT_RESERVED != threshold_event )
        {
          npa_trigger_callback_event( &event->callback,
                                      threshold_event,
                                      &event_data, 
                                      sizeof( npa_event_data ) );
        }
        break;
        
      case NPA_TRIGGER_SYNC_EVENT:
        break;
        
      default:
        CORE_VERIFY( 0 );
      }
    }
    event = event->next;
  } 

  Core_MutexUnlock( trigger->resource->event_lock ); 

  return DAL_SUCCESS;
}


/**
 * <!-- npa_resource_add_system_event_callback -->
 *
 * @brief Add a system event callback function to a resource
 *  
 * The callback function will be invoked on such event modifications as follows:
 * 1) A change event is created or destroyed;
 * 2) A watermark or threshold event is enabled, disabled or destroyed;
 * 3) Event watermarks or thresholds are changed.
 *
 * When invoked, the callback arguments will be 
 * (context, 0, npa_event *, sizeof(npa_event) ); 
 *
 * Note: a resource supports only one system event callback, so it is an error
 * to add a second. It is hoped that future versions will relax this 
 * restriction.
 *
 * @param resource_name : name of the resource that is to be used
 * @param callback : callback to be invoked
 * @param context : context to be passed back in callback invocation
 *
 * @return NPA_SUCCESS on success, NPA_ERROR otherwise.
 */
npa_status npa_resource_add_system_event_callback( const char   *resource_name,
                                                   npa_callback  callback,
                                                   void         *context )
{
  npa_event_handle handle = npa_create_event_cb( resource_name,
                                                 resource_name,
                                                 NPA_TRIGGER_SYSTEM_EVENT,
                                                 callback,
                                                 context );

  return ( handle ? NPA_SUCCESS : NPA_ERROR );
}


/**
 * <!-- npa_resource_remove_system_event_callback -->
 *
 * @brief Remove a system event callback function from a resource
 *  
 * The callback and context pointers should be the same as those when the 
 * callback is added.
 *
 * @param resource_name : name of the resource that is to be used
 * @param callback : callback to be invoked
 * @param context : context to be passed back in callback invocation
 *
 * @return NPA_SUCCESS on success, NPA_ERROR otherwise.
 */
npa_status npa_resource_remove_system_event_callback( const char   *resource_name,
                                                      npa_callback  callback,
                                                      void         *context )
{
  npa_event    *event;
  npa_resource *resource;

  if( ( NULL == resource_name ) || 
      ( NULL == ( resource = npa_find_resource( resource_name ) ) ) )
    return NPA_ERROR;

  Core_MutexLock( resource->event_lock );
  event = resource->events.event_header->sys_event;
  while( NULL != event )
  {
    if( ( event->callback.callback == callback ) &&
        ( event->callback.context == context ) )
    {
      CORE_VERIFY( NPA_TRIGGER_SYSTEM_EVENT == 
        ( event->trigger_type & NPA_TRIGGER_TYPE_MASK ) );
      break;
    }
    event = event->next;
  }
  Core_MutexUnlock( resource->event_lock );

  npa_destroy_event_handle( event );

  return NPA_SUCCESS;
}

#ifdef __cplusplus
}
#endif
