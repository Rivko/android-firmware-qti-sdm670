/*============================================================================
@file npa_remote_rpm_protocol.c

NPA Remote RPM Protocol

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
 
$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/remote/npa_remote_rpm_protocol.c#1 $
============================================================================*/

#include "npa_remote_resource.h"
#include "npa_remote_rpm_protocol.h"
#include "npa_internal.h"
#include "rpmclient.h"
#include "npa_inlines.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Local client context for remote resources */
typedef struct npa_rpm_client_context
{
  /* Identity of remote resources on the RPM */
  struct
  {
    rpm_resource_type type;
    unsigned          id;
  } remote_name;

  unsigned            units;
  kvp_t              *new_kvps;
  unsigned            fnf_msg_id;
} npa_rpm_client_context;

typedef struct npa_rpm_client_context *npa_rpm_client_handle;

static struct
{
  CorePoolConfigType pool_config;
} npa_rpm_config =
{
  { sizeof(npa_rpm_client_context), 30, 10 }
};

static CorePoolType npa_rpm_client_handle_pool;

/* Variables used in deferring some NAS updates to wakeup, in cases where
   writing them to NAS will cause an overflow */
#define NPA_RPM_NAS_MAX_BYTES 800
static npa_list_header npa_rpm_deferred_nas_updates;
static unsigned npa_rpm_num_nas_bytes;

/* Initialized to RPM_ACTIVE_SET. Modified to RPM_SEMI_ACTIVE_SET on targets
   where the RPM driver supports a semi-active set. */
static rpm_set_type NPA_RPM_SEMI_ACTIVE_SET = RPM_ACTIVE_SET;

/* Used by the scheduler */
unsigned npa_rpm_num_deferred_nas_updates;

/* Pointer to the function used to flush msgs to the RPM. Initialized to 
 * rpm_barrier, this may be switched to rpm_churn_queue by issuing a 
 * non-zero request to the "/protocol/rpm/polling_mode" resource.
 */
static void (*npa_rpm_sync)( unsigned int ) = rpm_barrier;

static
npa_query_status npa_rpm_query_function( npa_resource    *resource,
                                         unsigned int     query_id,
                                         npa_query_type  *result )
{
  switch ( query_id )
  {
  case NPA_QUERY_REMOTE_RESOURCE_AVAILABLE:
  {
    /* The RPM boots first, so all of its resources are always available.
     * Call npa_remote_resource_available with the given context
     */
    npa_remote_resource_available( result->data.reference ); 
    break;
  }
  
  /* If the RPM supported remote queries and the REMOTE_PROXY attribute
     was set, we would do something like this -
  case NPA_QUERY_CURRENT_STATE:
    if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_REMOTE_PROXY ) )
    {
      // query the RPM for remote state and return
    }
    else, fall-through to default for resources that don't have that attribute
   */

  default:
    /* Let npa_query handle the query */
    return NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  return NPA_QUERY_SUCCESS;
}

static NPA_INLINE void npa_rpm_init( void )
{
  Core_PoolConfig( &npa_rpm_client_handle_pool, &npa_rpm_config.pool_config );
  
  if ( RPM_NUM_SETS == 4 )
  { /* The RPM driver supports a semi-active set. Fragile check, but we don't
       have a more robust one, atm */
    NPA_RPM_SEMI_ACTIVE_SET = (rpm_set_type)3;
  }
}

static void npa_rpm_create_client_context( npa_resource *resource )
{
  npa_rpm_client_handle handle;
  const char *remote_name = resource->node->name;

  CORE_VERIFY_PTR( handle = (npa_rpm_client_handle)
                            Core_PoolAlloc( &npa_rpm_client_handle_pool ) );
  memset( handle, 0, sizeof(npa_rpm_client_context) );

  /* Interpret resource->definition->units as an unsigned int */
  {
    const char *units = resource->definition->units;
    unsigned int i, len = strlen( units );
    
    if ( len > 4 ) len = 4;
    for ( i = 0; i < len; ++i )
    {
      handle->units |= units[i] << 8*i;
    }
  }

  /* Interpret remote_resource_name as (4 bytes of type & 4 bytes of id)
   * Resource authors must pad both type and id with NULLs, if either is
   * less than 4 bytes in length. The type is a 4-byte character array and
   * the id is an unsigned int encoded little-endian.
   *
   * Correct names look like - "clk1\x01\0\0\0", "reg\0\x03\0\0\0",
   * "ldo\0\0\0\0\0" or "vreg\x01\x01\0\0"
   *
   * Assumes 8 bytes of data and will crash and burn if not the case!
   * TODO: Don't require the id part to be NULL-padded?
   */
  handle->remote_name.type 
    = (rpm_resource_type) (remote_name[0] | remote_name[1] << 8 |
                           remote_name[2] << 16 | remote_name[3] << 24);

  handle->remote_name.id = remote_name[4] | remote_name[5] << 8 |
                           remote_name[6] << 16 | remote_name[7] << 24;

  if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    /* Vector resources manage KVP buffers on their own. At each request, we
     * are given a KVP buffer in resource->state_vector. We don't need to
     * pre-allocate a buffer, but still need to create a kvp_t object.
     */
    handle->new_kvps = kvp_create( 0 );
  }
  else
  {
    /* We build a KVP buffer for scalar resources on the fly. Preallocate
     * the memory used for that now to prevent malloc'ing from an
     * uncomfortable location later (e.g., an IST). The 24 bytes will hold
     * 2 KVPs (type, len, value triples); the first is the state and the
     * second the active_max.
     */
    handle->new_kvps = kvp_create( 24 );
  }

  /* Associate client context with resource */
  resource->node->data = handle;
}

/* The MAX key */
#define STR_MAX_AS_KEY 0x78616d

static NPA_INLINE
unsigned int npa_rpm_post_request( npa_resource *resource, rpm_set_type set, 
                                   npa_resource_state state )
{
  unsigned int msg_id;
  npa_rpm_client_handle handle = (npa_rpm_client_handle)resource->node->data;
  npa_resource_state max = resource->active_max;

  if ( set == RPM_NEXT_ACTIVE_SET )
  {
    /* If we've already written a NAS request for this resource in the current
       sleep enter, discount the previous request size from the limit */
    npa_rpm_num_nas_bytes -= rpm_get_request_size( RPM_NEXT_ACTIVE_SET,
                                                   handle->remote_name.type, 
                                                   handle->remote_name.id );
    /* The 40 is the (~) size of the RPM message header for this request +
       space for the state KVP */
    if ( npa_rpm_num_nas_bytes + 40 > NPA_RPM_NAS_MAX_BYTES )
    {
      npa_list_PushBack( &npa_rpm_deferred_nas_updates, (void *)resource );
      npa_rpm_num_deferred_nas_updates++;
      return 0;
    }
  }

  kvp_clear( handle->new_kvps );
  
  kvp_put( handle->new_kvps, handle->units, 4, (const char *)&state );
  if ( set == RPM_ACTIVE_SET || set == NPA_RPM_SEMI_ACTIVE_SET )
  {
    kvp_put( handle->new_kvps, STR_MAX_AS_KEY, 4, (const char *)&max );
  }

  msg_id = rpm_post_request( set,
                             handle->remote_name.type, handle->remote_name.id,
                             handle->new_kvps );
                             
  if ( set == RPM_NEXT_ACTIVE_SET )
  {
    npa_rpm_num_nas_bytes += rpm_get_request_size( RPM_NEXT_ACTIVE_SET,
                                                   handle->remote_name.type, 
                                                   handle->remote_name.id );
    ULOG_RT_PRINTF_1( npa.log, "Bytes in NAS payload: %d",
                      npa_rpm_num_nas_bytes );
  }
  
  ULOG_RT_PRINTF_6( npa.log, "npa_rpm_post_request (set: %d) "
                    "(resource: \"%s\") (handle: 0x%08x) "
                    "(state: %u) (max: %u) (msg_id: 0x%08x)", set,
                    resource->definition->name, resource, state, max, msg_id );
  return msg_id;
}

static NPA_INLINE
unsigned int npa_rpm_post_vector_request( npa_resource *resource,
                                          rpm_set_type  set,
                                          npa_resource_state *vector )
{
  unsigned int msg_id;
  npa_rpm_client_handle handle = (npa_rpm_client_handle)resource->node->data;

  if ( set == RPM_NEXT_ACTIVE_SET )
  {
    /* If we've already written a NAS request for this resource in the current
       sleep enter, discount the previous request size from the limit */
    npa_rpm_num_nas_bytes -= rpm_get_request_size( RPM_NEXT_ACTIVE_SET,
                                                   handle->remote_name.type, 
                                                   handle->remote_name.id );
    /* The 28 is the (~) size of the message header for this request */
    if ( ( npa_rpm_num_nas_bytes + 28 +
          resource->active_max * sizeof(npa_resource_state ) ) 
         > NPA_RPM_NAS_MAX_BYTES )
    {
      npa_list_PushBack( &npa_rpm_deferred_nas_updates, (void *)resource );
      npa_rpm_num_deferred_nas_updates++;
      return 0;
    }
  }

  kvp_swapbuffer( handle->new_kvps, (const char *)vector,
                  resource->active_max * sizeof(npa_resource_state) );

  msg_id = rpm_post_request( set, handle->remote_name.type,
                             handle->remote_name.id, handle->new_kvps );
                             
  if ( set == RPM_NEXT_ACTIVE_SET )
  {
    npa_rpm_num_nas_bytes += rpm_get_request_size( RPM_NEXT_ACTIVE_SET,
                                                   handle->remote_name.type, 
                                                   handle->remote_name.id );
    ULOG_RT_PRINTF_1( npa.log, "Bytes in NAS payload: %d",
                      npa_rpm_num_nas_bytes );
  }
                                    
  ULOG_RT_PRINTF_6( npa.log, "npa_rpm_post_request (set: %d) "
                    "(resource: \"%s\") (handle: 0x%08x) (vector: 0x%08x) "
                    "(size: %d) (msg_id: 0x%08x)",
                    set, resource->definition->name, resource,
                    vector, resource->active_max, msg_id );
  return msg_id;
}

/* When the scheduler LPR enter_fcn issues requests intended for Next Wakeup, 
 * they are aggregated alongside current requests and the result (overall_agg)
 * is written into the Next Active Set, rather than the current active set.
 * Concurrently, the sleep set may need to be updated. We cannot do this inline
 * with the NAS write, as that would mean updating the "current" sleep set
 * sleep set per the "next" active set. Instead, we defer the updates to
 * a function that is invoked during scheduler LPR exit (i.e. on next wakeup).
 */
static npa_list_header npa_rpm_deferred_ss_updates;

void npa_rpm_do_deferred_updates( void )
{
  unsigned msg_id = 0;
  
  /* Process deferred NAS updates */
  npa_resource *resource = 
      (npa_resource *)npa_list_PopFront( &npa_rpm_deferred_nas_updates );

  while ( resource )
  {
    if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
    {
      msg_id = npa_rpm_post_vector_request( resource, RPM_ACTIVE_SET, 
                                            resource->state_vector );
    }
    else
    {
      msg_id = npa_rpm_post_request( resource, RPM_ACTIVE_SET, 
                                     resource->active_state );
    }
    resource = 
      (npa_resource *) npa_list_PopFront( &npa_rpm_deferred_nas_updates );
  }
  
  if ( msg_id )
  {
    rpm_churn_queue( msg_id );
  }
  
  /* Reset */
  npa_rpm_num_deferred_nas_updates = 0;
  npa_rpm_num_nas_bytes = 0;

  /* Process deferred Sleep Set updates */
  resource = (npa_resource *) npa_list_PopFront( &npa_rpm_deferred_ss_updates );
  while ( resource )
  {
    if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
    {
      npa_rpm_post_vector_request( resource, RPM_SLEEP_SET,
                                   resource->required_state_vector );
    }
    else /* scalar resource */
    {
      npa_rpm_post_request( resource, RPM_SLEEP_SET, 
                            NPA_RESOURCE_REQUIRED_REQUEST(resource) );
    }
    resource = (npa_resource *) npa_list_PopFront( 
                                         &npa_rpm_deferred_ss_updates );
  }
}


static
npa_resource_state npa_rpm_resource_driver( npa_resource      *resource,
                                            npa_client_handle  client,
                                            npa_resource_state state )
{
  unsigned int msg_id = 0;
  npa_rpm_client_handle handle = (npa_rpm_client_handle)resource->node->data;
  
  rpm_set_type set = \
    _npa_request_has_attribute( client, NPA_REQUEST_NEXT_AWAKE ) ? 
    RPM_NEXT_ACTIVE_SET : RPM_ACTIVE_SET;
    
  /* A previous version of this code wrote the sleep set when the required
   * aggregation differed from overall AND invalidated the sleep set when they
   * were the same. Invalidating the sleep set causes the RPM driver to issue
   * an immediate request to the RPM. So, we now simply write the required
   * aggregation into the sleep set and let the RPM driver or the
   * firmware on the RPM short-circuit the request as appropriate. Weighing the
   * excess traffic invalidation generates against the price of processing
   * a redundant sleep set message, it was determined that this is a win.
   * The change also makes the below code that much simpler.
   */
  if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    msg_id = npa_rpm_post_vector_request( resource, set, 
                                          resource->state_vector );
    if ( _npa_resource_has_attribute( resource, 
                                      NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) && 
         client->type == NPA_CLIENT_VECTOR )
    {
      if ( set == RPM_NEXT_ACTIVE_SET )
      { /* If the active set is NAS, defer the sleep set update */
        npa_list_PushFront( &npa_rpm_deferred_ss_updates, resource );
      }
      else
      {
         npa_rpm_post_vector_request( resource, RPM_SLEEP_SET, 
                                      resource->required_state_vector );
      }
    }
    
    /* If not a NAS request and there is a semi active set, update it */
    if ( set == RPM_ACTIVE_SET && NPA_RPM_SEMI_ACTIVE_SET != RPM_ACTIVE_SET )
    {
      /* If a vector resource doesn't support SUPPRESSIBLE2 requests,
         semiactive_state_vector has already been set to state_vector */
      npa_rpm_post_vector_request( resource, NPA_RPM_SEMI_ACTIVE_SET, 
                                   resource->semiactive_state_vector );
    }
  }
  else
  {
    if ( state != resource->active_state )
    {
      msg_id = npa_rpm_post_request( resource, set, state );
    }
    
    if ( _npa_resource_has_attribute( resource, 
                                      NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) && 
         client->type == NPA_CLIENT_REQUIRED )
    {
      if ( set == RPM_NEXT_ACTIVE_SET )
      { /* If the active set is NAS, defer the sleep set update */
        npa_list_PushFront( &npa_rpm_deferred_ss_updates, resource );
      }
      else
      {
         npa_rpm_post_request( resource, RPM_SLEEP_SET, 
                               NPA_RESOURCE_REQUIRED_REQUEST(resource) );
      }
    }
    
    /* If not a NAS request and there is a semi active set, update it */
    if ( set == RPM_ACTIVE_SET && NPA_RPM_SEMI_ACTIVE_SET != RPM_ACTIVE_SET )
    {
      /* Initialize the semiactive_agg to overall. Override with the actual
         field if the resource supports SUPPRESSIBLE2 requests */
      npa_resource_state semiactive_agg = state;
      if ( npa_resource_has_internal_attribute( 
                            resource, NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE2 ) )
      {
        semiactive_agg = NPA_RESOURCE_SEMIACTIVE_REQUEST(resource);
      }
      npa_rpm_post_request( resource, NPA_RPM_SEMI_ACTIVE_SET, semiactive_agg );
    }
  }

  /* msg_id is 0 if the request posted is to the Sleep or NAS sets OR
   * if the request is "KVP short-circuited" by the RPM driver. 
   * In such a case, set msg_id to the msg_id of the last Fire-and-Forget
   * request on this resource.
   */
  if ( msg_id == 0 ) msg_id = handle->fnf_msg_id;

  /* msg_id may still be 0, if both the current "active" set post returned 0
     (see above for why) and fnf_msg_id on the current resource is 0 */
  if ( msg_id )
  {
    /* If a "complete/cancel_request" or a request with an explicit
       Fire-and-Forget attribute */
    if ( _npa_request_has_attribute( client, NPA_REQUEST_DROP_VOTE ) ||
         _npa_request_has_attribute( client, NPA_REQUEST_FIRE_AND_FORGET ) )
    {
      handle->fnf_msg_id = msg_id;
    }
    else
    {
      /* synchronous request */
      ULOG_RT_PRINTF_1( npa.log, "npa_rpm_resource_driver_barrier "
                        "(msg_id: 0x%08x)", msg_id );
      npa_rpm_sync( msg_id );
      /* Reset fnf_msg_id as we've barried-ed now */
      handle->fnf_msg_id = 0;
    }
  }

  return state;
}

/* All RPM remoted resources have their driver functions set to the below
 * routine at init. 
 * For resources without the NO_INIT attribute, this function handles
 * CLIENT_INITIALIZE and then switches the resource over to the regular driver.
 * For resources with the NO_INIT attribute, this function handles
 * CLIENT_INITIALIZE and the first subsequent request from any client. This is
 * to ensure that the sleep set is programmed with the required_agg. It then
 * switches the resource over to the regular driver - the routine above
 */
static npa_resource_state
npa_rpm_resource_driver_initial( npa_resource      *resource,
                                 npa_client_handle  client,
                                 npa_resource_state state )
{
  unsigned msg_id = 0;
  
  if ( client->type == NPA_CLIENT_INITIALIZE )
  {
    ULOG_RT_PRINTF_3( npa.log,
      "npa_rpm_resource_driver_initial "
      "(resource: \"%s\") (handle: 0x%08x) (no_init: %d)",
      resource->definition->name, resource,
      _npa_resource_has_attribute( resource, NPA_RESOURCE_REMOTE_NO_INIT ) );

    /* This driver is shared between the protocol node and remote resources */
    if ( 0 == strncmp( resource->definition->name, RPM_PROTOCOL_RESOURCE_NAME,
                       NPA_MAX_STR_LEN + 1 ) )
    {
      npa_rpm_init();
      return state;
    }
    else
    {
      npa_rpm_create_client_context( resource );

      if ( _npa_resource_has_attribute( resource, 
                                        NPA_RESOURCE_REMOTE_NO_INIT ) )
      {
        /* Don't switch the driver function; this routine will handle the first
           client request as well */
        return state;
      }
    }
  }

  /* For resources without NO_INIT or on the first client request to resources
   * with NO_INIT, write the passed in state and the current required_agg into
   * the ACTIVE and SLEEP sets respectively.
   */
  if ( _npa_resource_has_attribute( resource, 
                                    NPA_RESOURCE_VECTOR_STATE ) )
  {
    msg_id = npa_rpm_post_vector_request( resource, RPM_ACTIVE_SET, 
                                          resource->state_vector );
    /* Initialize the semi-active set (if it exists) with the same state */
    if ( NPA_RPM_SEMI_ACTIVE_SET != RPM_ACTIVE_SET )
    {
      npa_rpm_post_vector_request( resource, NPA_RPM_SEMI_ACTIVE_SET,
                                   resource->state_vector );
    }

    npa_rpm_post_vector_request( resource, RPM_SLEEP_SET,
                                 resource->required_state_vector );
  }
  else /* scalar resource */
  {
    msg_id = npa_rpm_post_request( resource, RPM_ACTIVE_SET, state );
    /* Initialize the semi-active set (if it exists) with the same state */
    if ( NPA_RPM_SEMI_ACTIVE_SET != RPM_ACTIVE_SET )
    {
      npa_rpm_post_request( resource, NPA_RPM_SEMI_ACTIVE_SET, state );
    }

    npa_rpm_post_request( resource, RPM_SLEEP_SET,
                          NPA_RESOURCE_REQUIRED_REQUEST(resource) );
  }
  
  if ( msg_id )
  {
    npa_rpm_sync( msg_id );
  }

  /* Switch the resource over to the regular driver */
  resource->node->driver_fcn = npa_rpm_resource_driver;
  
  return state;
}

static
npa_resource_state npa_rpm_polling_mode_toggle( npa_resource      *resource,
                                                npa_client        *client,
                                                npa_resource_state state )
{
  /* We don't bother with an NPA_CLIENT_INITIALIZE section because we define
   * the resource with 0 as initial state and the logic below handles that */
  npa_rpm_sync = ( state == 0 ? rpm_barrier : rpm_churn_queue );
  return state;
}

/* The RPM polling mode toggle resource */
static npa_resource_definition npa_rpm_polling_mode_resource[] =
{
  {
    "/protocol/rpm/polling_mode",
    "toggle",
    1,
    &npa_identity_plugin,
    NPA_RESOURCE_DEFAULT,
  }
};

static npa_node_dependency npa_rpm_polling_mode_deps[] = 
{
  { RPM_PROTOCOL_RESOURCE_NAME, NPA_NO_CLIENT }
};

/* The RPM polling mode toggle node; lockless, because requests to this node
   MUST only be issued from sleep's context */
static npa_node_definition npa_rpm_polling_mode_node = 
{ 
  "/protocol/rpm/polling_mode",
  npa_rpm_polling_mode_toggle, 
  NPA_NODE_NO_LOCK,
  NULL,
  NPA_ARRAY(npa_rpm_polling_mode_deps),
  NPA_ARRAY(npa_rpm_polling_mode_resource)
};

/* The RPM protocol resource */
static npa_resource_definition npa_rpm_resource[] =
{
  {
    RPM_PROTOCOL_RESOURCE_NAME,
    "na",
    0,
    &npa_no_client_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    npa_rpm_query_function
  }
};

/* The RPM protocol node */
static npa_node_definition npa_rpm_node = 
{ 
  RPM_PROTOCOL_RESOURCE_NAME,
  npa_rpm_resource_driver_initial, 
  NPA_NODE_NO_LOCK,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(npa_rpm_resource)
};

void npa_rpm_available_cb( void *context, unsigned int event_type, 
                           void *data, unsigned int data_size )
{
  npa_resource_state dummy = 0;
  npa_define_node( &npa_rpm_node, &dummy, NULL );
  npa_define_node( &npa_rpm_polling_mode_node, &dummy, NULL );
} 

#ifdef __cplusplus
}
#endif
