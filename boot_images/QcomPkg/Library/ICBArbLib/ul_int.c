/*=============================================================================

FILE:     ul_int.c 

DESCRIPTION: This file implements the NPA ICB Arbiter Node

    Copyright (c) 2010-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/ul_int.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2017/03/08   sds     Change BCM-as-clock to BCM-as-standalone generic framework
2017/03/01   sds     Update BW to BCM calculation
2017/02/24   sds     Add support for BCM-as-clock implementation
2016/11/08   sds     Add BCM query functions
2016/10/06   sds     BCM init state and AB aggregation restructure
2016/08/18   sds     Add support for vector type BCMs
2016/03/29   sds     Statically allocate hw request buffers.
2016/01/21   sds     Branch for Hoya from 2.0.
=============================================================================*/

#include "ul_i.h"
#include "ddiaxicfgint.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "ULogFront.h"
#include "ul_tree.h"
#include "string.h"
#include "CoreVerify.h"
#include "LoaderUtils.h"

/*============================================================================
                          DEFINES
============================================================================*/

/* Divide and round up to the nearest integer */
#define CEIL_DIV(dividend, divisor) (((dividend)/(divisor))+(((dividend)%(divisor))?1:0))
#define BW_SCALE(bw, dividend, divisor) CEIL_DIV((bw)*(dividend),(divisor))

#define REQUEST_LIST_SIZE    10

/* extern the Arbiter Internal Log */
extern ULogHandle icb_log;

typedef enum
{
  ICB_SW_STATE_UNINITIALIZED = 0, /**< Not yet initialized. */
  ICB_SW_STATE_INITIALIZING,      /**< Initializing, but not ready for requests. */
  ICB_SW_STATE_HOLD,              /**< Initialized, but holding requests. */
  ICB_SW_STATE_FLUSH,             /**< Flushing held requests. */
  ICB_SW_STATE_RUN                /**< All requests processed as received. */
} icb_sw_state_type;

/*============================================================================
                          STATIC INTERNAL DATA
============================================================================*/
static icb_sw_state_type sw_state = ICB_SW_STATE_UNINITIALIZED;
static icb_info_type *icb_info    = NULL;

/* Queue Head for the commit queues */
static icb_request_type * sw_node_queue = NULL;
static icb_request_type * hw_node_queue = NULL;

/*============================================================================
                     STATIC FUNCTIONS - NOTIFICATIONS
============================================================================*/
/** ul_notify_evaluate
 *
 * @brief Evaluate and update the current notification status for the passed notification.
 *        May trigger a callback.
 *
 */
void ul_notify_evaluate
(
  icb_notify_type *   notify,
  icb_node_state_type *state
)
{
  uint64_t state_val[ICB_NOTIFY_MAX_STATES], prev_val[ICB_NOTIFY_MAX_STATES];
  uint32_t i, states;
  bool callback_required = false;

  /* Calculate new state values.
   * IB is the max of all active requests.
   * AB is the sum of all active requests.
   * Total is the per-channel total calculated by compute_bw().
   *   This does have any util/vrail_comp factors applied. */
  /* Save and update state. */
  notify->status.ib    = MAX(state->ib, state->supp_ib);
  notify->status.ab    = state->ab + state->supp_ab;
  notify->status.total = state->total_bw;
  notify->status_valid = true;

  /* Collect our pre/post type that this notification cares about. */
  switch(notify->definition.threshold_type)
  {
    case ICB_NOTIFY_AB:
      state_val[0] = notify->status.ab;
      prev_val[0]  = notify->prev_state.ab;
      states = 1;
      break;
    case ICB_NOTIFY_IB:
      state_val[0] = notify->status.ib;
      prev_val[0]  = notify->prev_state.ib;
      states = 1;
      break;
    case ICB_NOTIFY_TOTAL:
      state_val[0] = notify->status.total;
      prev_val[0]  = notify->prev_state.total;
      states = 1;
      break;
    case ICB_NOTIFY_IB_AB:
      state_val[0] = notify->status.ib;
      state_val[1] = notify->status.ab;
      prev_val[0]  = notify->prev_state.ib;
      prev_val[1]  = notify->prev_state.ab;
      states = 2;
      break;
    default:
      /* Something is seriously broken if we get here. */
      return;
  }

  /* Check for monitored status value. */
  for( i = 0; i < states; i++ )
  {
    /* Check to see if the high threshold was crossed.
    * Only check if the threshold wasn't already crossed. */
    if(false == notify->status.high_threshold)
    {
      /* Crossing up through high threshold. */
      if(prev_val[i]  <  notify->definition.high_threshold[i] &&
         state_val[i] >= notify->definition.high_threshold[i])
      {
        notify->status.high_threshold = true;
        callback_required = true;
      }

      /* Crossing down through high threshold. */
      if(notify->definition.crossing_type == ICB_NOTIFY_CROSSING_BOTH &&
         prev_val[i]  >= notify->definition.high_threshold[i] &&
         state_val[i] <  notify->definition.high_threshold[i])
      {
        notify->status.high_threshold = true;
        callback_required = true;
      }
    }

    /* Check to see if the low threshold was crossed.
    * Only check if the threshold wasn't already crossed. */
    if(false == notify->status.low_threshold)
    {
      /* Crossing down through low threshold */
      if(prev_val[i]  >  notify->definition.low_threshold[i] &&
         state_val[i] <= notify->definition.low_threshold[i])
      {
        notify->status.low_threshold = true;
        callback_required = true;
      }

      /* Crossing up through low threshold */
      if(notify->definition.crossing_type == ICB_NOTIFY_CROSSING_BOTH &&
         prev_val[i]  <= notify->definition.low_threshold[i] &&
         state_val[i] >  notify->definition.low_threshold[i])
      {
        notify->status.low_threshold = true;
        callback_required = true;
      }
    }
  }

  /* Update previous state. */
  notify->prev_state.ib =    notify->status.ib;
  notify->prev_state.ab =    notify->status.ab;
  notify->prev_state.total = notify->status.total;

  /* Issue the callback if necessary. */
  if(callback_required)
  {
    /* Pass a copy of the state data.
     * We don't want to pass a pointer to the actual storage location. */
    icb_notify_status_type status = notify->status;
    notify->callback(notify, &status, notify->callback_data);
  }
}

/** ul_notify_node_update
 *
 * @brief A node has been updated.
 *        This function is called to process any notifications this node has.
 */
void ul_notify_node_update
(
  icb_notify_list_type *notify_list,
  icb_node_state_type *  state
)
{
  icb_notify_type *iter = notify_list->head;
  uint32_t i;

  for( i = 0;
       i < notify_list->count && NULL != iter;
       i++ )
  {
    /* Evaluate this notification. */
    ul_notify_evaluate(iter, state);

    /* Get the next in the list. */
    iter = iter->next;
  }
}

/*============================================================================
                          STATIC FUNCTIONS - Helper
============================================================================*/
/**=============================================================================

  compute_bw

  @brief Computes total bandwidth for notifications

  @param state - a pointer to the node state to work with

  @return The total bandwidth on the node.

==============================================================================*/
uint64_t compute_bw
(
  icb_node_state_type *state,
  uint32_t            num_ports
)
{
  uint64_t ab, supp_ab, total_bw;
  uint32_t ports = MAX(1, num_ports);

  /* First, calculate the total bandwidth per port on this node. */
  if(state->supp_ab == 0)
  {
    supp_ab = 0;
  }
  else
  {
    supp_ab = state->ab + state->supp_ab;
  }
  ab = state->ab;
  total_bw = MAX(MAX(state->ib, state->supp_ib),
                 CEIL_DIV(MAX(ab, supp_ab), ports));

  return total_bw;
}

/*============================================================================
                     STATIC FUNCTIONS - Queue processing
============================================================================*/
/**
 * request_enqueue
 *
 * @brief Queue a request to the appropriate request queue.
 *
 * @param A pointer to the head of the queue
 * @param A pointer to the request to queue
*/
static void request_enqueue
(
  icb_request_type **queue_head,
  icb_request_type * req
)
{
  /* Only add requests once */
  if( req->is_queued )
  {
    return;
  }

  // Add new requests to the front
  req->next = *queue_head;
  *queue_head = req;
  req->is_queued = true;
}

void update_sw_latency_state
(
  icb_node_state_type *     node_state,
  icb_bw_request_list_type *request_list
)
{
  uint32_t latency = 0, supp_latency = 0;
  uint32_t req_idx;
  icb_bw_request_type **requests = request_list->requests;

  /* Search the request list for the appropriate latencies */
  for( latency = 0, supp_latency = 0, req_idx = 0;
       req_idx < request_list->num_entries; req_idx++ )
  {
    /* Required latencies only care about required requests. */
    if( NPA_CLIENT_VECTOR == requests[req_idx]->client->type )
    {
      if( 0 == latency ||
          (0 != requests[req_idx]->latency_ns &&
          requests[req_idx]->latency_ns < latency) )
      {
        latency = requests[req_idx]->latency_ns;
      }
    }
    else
    {
      /* Suppressible is the only other client type we accept. */
      if( 0 == supp_latency ||
          (0 != requests[req_idx]->latency_ns &&
            requests[req_idx]->latency_ns < supp_latency) )
      {
        supp_latency = requests[req_idx]->latency_ns;
      }
    }
  }

  /* Store back to the state variable. */
  node_state->latency_ns      = latency;
  node_state->supp_latency_ns = supp_latency;
}

void process_master_latency_request
(
  icb_master_def_type *master
)
{
  uint32_t req_idx;

  update_sw_latency_state( &master->state, &master->request_list );

  /* Request to the associated BCM */
  for( req_idx = 0; req_idx < master->num_latency_nodes; req_idx++ )
  {
    request_enqueue( &hw_node_queue, &master->latency_nodes[req_idx]->request );
  }
}

void update_sw_bandwidth_state
(
  icb_node_state_type *     node_state,
  icb_bw_request_list_type *request_list,
  uint32_t                  num_agg_ports
)
{
  icb_bw_request_type **requests = request_list->requests;
  uint64_t ib = 0, supp_ib = 0;
  uint64_t ab = 0, supp_ab = 0;
  uint32_t req_idx;
  icb_node_state_type state;

  /* Search the request list to get the MAX Ib while summing AB. */
  for( req_idx = 0; req_idx < request_list->num_entries; req_idx++ )
  {
    if( NPA_CLIENT_SUPPRESSIBLE_VECTOR == requests[req_idx]->client->type )
    {
      if(requests[req_idx]->ib > supp_ib)
      {
        supp_ib = requests[req_idx]->ib;
      }
      supp_ab += requests[req_idx]->ab;
    }
    else
    {
      if(requests[req_idx]->ib > ib)
      {
        ib = requests[req_idx]->ib;
      }
      ab += requests[req_idx]->ab;
    }
  }

  node_state->supp_ib = supp_ib;
  node_state->supp_ab = supp_ab;
  node_state->ib = ib;
  node_state->ab = ab;

  /* Compute effective bandwidth */
  state = *node_state;
  node_state->total_bw = compute_bw(&state, num_agg_ports);
}

void process_master_bw_request
(
  icb_master_def_type *master
)
{
  uint32_t req_idx;

  /* Compute new hw node state */
  update_sw_bandwidth_state(&master->state, &master->request_list, master->num_agg_ports);

  /* Update QoS HW */
  AxiCfgInt_QoSSetMasterBW( master );

  /* Request to the associated BCMs */
  for( req_idx = 0; req_idx < master->num_bw_nodes; req_idx++ )
  {
    request_enqueue( &hw_node_queue, &master->bw_nodes[req_idx]->request );
  }

  /* Process pending notifications (if any) */
  ul_notify_node_update(&master->notifications, &master->state);
}

void process_slave_bw_request
(
  icb_slave_def_type *slave
)
{
  uint32_t req_idx;

  /* Compute new hw node state */
  update_sw_bandwidth_state(&slave->state, &slave->request_list, slave->num_agg_ports);

  /* Request to the associated BCM */
  for( req_idx = 0; req_idx < slave->num_bw_nodes; req_idx++ )
  {
    request_enqueue( &hw_node_queue, &slave->bw_nodes[req_idx]->request );
  }

  /* Process pending notifications (if any) */
  ul_notify_node_update(&slave->notifications, &slave->state);
}

/**
 * update_hw_bandwidth_state
 *
 * @brief Calculate hw node bandwidth state
 *
 * @param A pointer to the hw node to evaluate
 *
*/
void update_hw_bandwidth_state
(
  icb_hw_node_type *hw_node
)
{
  uint64_t ib = 0, supp_ib = 0, ab = 0, supp_ab = 0;
  uint32_t req_idx, node_width, node_ports, bcm_width = hw_node->width;
  icb_node_state_type *state;

  /* Only aggregate if our state is not FLUSH.
   * If it is FLUSH, we know we've already aggregated.
   * No need to do it again. */
  if( ICB_SW_STATE_FLUSH != sw_state )
  {
    /* Find the max for each bandwidth category. */
    for( req_idx = 0; req_idx < hw_node->request_list.num_entries; req_idx++ )
    {
      icb_hw_request_type *request = hw_node->request_list.requests[req_idx];

      /* Fetch the right state object and node width. */
      switch(request->type)
      {
        case ICB_MASTER_HW_REQUEST:
        {
          icb_master_def_type *master = (icb_master_def_type *)request->node;
          state      = &master->state;
          node_width = master->width;
          node_ports = MAX(1, master->num_agg_ports);
          break;
        }
        case ICB_SLAVE_HW_REQUEST:
        {
          icb_slave_def_type *slave = (icb_slave_def_type *)request->node;
          state      = &slave->state;
          node_width = slave->width;
          node_ports = MAX(1, slave->num_agg_ports);
          break;
        }
        default:
          /* Ignore any type we don't expect. */
          continue;
      }
      
      /* MAX aggregation over our requests, scaled by:
      * 1.) Ratio of bcm_width to node width
      * 2.) AB requests to be scaled to per channel */
      if(ICB_HW_NODE_KIND_BANDWIDTH == hw_node->type)
      {
        ib      = MAX(ib,      BW_SCALE(state->ib,bcm_width,node_width));
        supp_ib = MAX(supp_ib, BW_SCALE(state->supp_ib,bcm_width,node_width));
        ab      = MAX(ab,      BW_SCALE(state->ab,bcm_width,node_width*node_ports));
        supp_ab = MAX(supp_ab, BW_SCALE(state->supp_ab,bcm_width,node_width*node_ports));
      }
      else
      {
        ib      = MAX(ib,      state->ib);
        supp_ib = MAX(supp_ib, state->supp_ib);
        ab      = MAX(ab,      state->ab);
        supp_ab = MAX(supp_ab, state->supp_ab);
      }
    }

    /* Save back to our state. */
    hw_node->state.total_bw = MAX(MAX(ib, supp_ib), ab + supp_ab);
    hw_node->state.ib       = ib;
    hw_node->state.supp_ib  = supp_ib;
    hw_node->state.ab       = ab;
    hw_node->state.supp_ab  = supp_ab;
  }
}

void process_hw_bw_request
(
  icb_hw_node_type *hw_node
)
{
  /* Compute new hw node state */
  update_hw_bandwidth_state(hw_node);

  /* Commit to hardware.
   * This gets a little complicated.
   * 
   * Always use the aggregated state if either of the following are true:
   * 1.) ICB is in the RUN state
   * 2.) ICB is in the FLUSH state
   *
   * If ICB is in the HOLD state:
   *   Only use the aggregated state if non-zero, do nothing otherwise.
   */
  if( ICB_SW_STATE_RUN   == sw_state ||
      ICB_SW_STATE_FLUSH == sw_state ||
      (ICB_SW_STATE_HOLD == sw_state && hw_node->state.total_bw != 0) )
  {
    AxiCfgInt_SetBandwidth(hw_node, &hw_node->state);
  }
}

void process_hw_vector_request
(
  icb_hw_node_type *hw_node
)
{
  //icb_node_state_type *state;

  /* Compute new hw node state */
  update_hw_bandwidth_state(hw_node);

  /* Commit to hardware.
   * This gets a little complicated.
   * 
   * Always use the aggregated state if either of the following are true:
   * 1.) ICB is in the RUN state
   * 2.) ICB is in the FLUSH state
   *
   * If ICB is in the HOLD state:
   *   Only use the aggregated state if non-zero, do nothing otherwise.
   */
  if( ICB_SW_STATE_RUN   == sw_state ||
      ICB_SW_STATE_FLUSH == sw_state ||
      (ICB_SW_STATE_HOLD == sw_state && hw_node->state.total_bw != 0) )
  {
    AxiCfgInt_SetVector(hw_node, &hw_node->state);
  }
}

void process_hw_latency_request
(
  icb_hw_node_type *hw_node
)
{
  uint32_t latency_ns = 0, supp_latency_ns = 0;
  uint32_t req_idx;
  icb_node_state_type *state;

  /* Only aggregate if our state is not FLUSH.
   * If it is FLUSH, we know we've already aggregated.
   * No need to do it again. */
  if( ICB_SW_STATE_FLUSH != sw_state )
  {
    /* Find the min non-zero for each latency category. */
    for( req_idx = 0; req_idx < hw_node->request_list.num_entries; req_idx++ )
    {
      icb_hw_request_type *request = hw_node->request_list.requests[req_idx];

      /* Fetch the right state object. */
      switch(request->type)
      {
        case ICB_MASTER_HW_REQUEST:
          state = &(((icb_master_def_type *)request->node)->state);
          break;
        default:
          /* Ignore any type we don't expect. */
          continue;
      }
      
      /* Min non-zero aggregation, because zero means "don't care". */
      if( 0 == latency_ns ||
          (0 != state->latency_ns &&
          state->latency_ns < latency_ns) )
      {
        latency_ns = state->latency_ns;
      }
      if( 0 == supp_latency_ns ||
          (0 != state->supp_latency_ns &&
          state->supp_latency_ns < supp_latency_ns) )
      {
        supp_latency_ns = state->supp_latency_ns;
      }
    }

    /* Save back to our state. */
    hw_node->state.latency_ns      = latency_ns;
    hw_node->state.supp_latency_ns = supp_latency_ns;
  }

  /* Commit to hardware.
   * This gets a little complicated.
   * 
   * Always use the aggregated state if either of the following are true:
   * 1.) ICB is in the RUN state
   * 2.) ICB is in the FLUSH state
   *
   * If ICB is in the HOLD state:
   *   Only use the aggregated state if non-zero, do nothing otherwise.
   */
  if( ICB_SW_STATE_RUN   == sw_state ||
      ICB_SW_STATE_FLUSH == sw_state ||
      (ICB_SW_STATE_HOLD == sw_state && hw_node->state.total_bw != 0) )
  {
    AxiCfgInt_SetLatency(hw_node, &hw_node->state);
  }
}

/*============================================================================
                          EXTERNAL FUNCTIONS
============================================================================*/
/**
 * @copydoc
 */
void ul_issue_pair_request
(
  npa_client_handle    client,
  icb_bw_request_type *request,
  icb_bw_request_type *old_request,
  icb_pair_type       *pair
)
{
  icb_master_def_type *master = pair->master;
  icb_slave_def_type  *slave  = pair->slave;
  bool is_type_changed, is_latency_changed, is_bw_changed;

  /* Log the request */
  ULOG_RT_PRINTF_4(icb_log,
                   "Issue Pair Request (MID: %d, SID: %d) (request: Ib: 0x%08x Ab: 0x%08x)",
                   master->id,
                   slave->id,
                   (uint32_t)request->ib,
                   (uint32_t)request->ab );

  /* Set the client type change flag */
  is_type_changed = (client->request_attr & NPA_REQUEST_CHANGED_TYPE);

  /* Set the changed type flags.
   * If the client type was changed, they all are. */
  is_latency_changed = (0 < master->num_latency_nodes) &&
                       (request->latency_ns != old_request->latency_ns || is_type_changed );
  is_bw_changed = ((request->ab != old_request->ab) || (request->ib != old_request->ib) || is_type_changed );

  /* If the latency request needs updating, queue it.
   * We'll service that later once we've processed all nodes initially. */
  if( is_latency_changed )
  {
    request_enqueue( &sw_node_queue, &master->latency_request );
  }

  /* If the bandwidth changed, compute new AB and queue. */
  if( is_bw_changed || is_type_changed )
  {
    /* Queue requests for consolidated processing later. */
    request_enqueue( &sw_node_queue, &master->bw_request );
    request_enqueue( &sw_node_queue, &slave->bw_request );
  }
}

/** ul_add_bw_request
 * 
 * @brief Adds a BW request to the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param request 
 * @param request_list
 */
void ul_add_bw_request
(
  icb_bw_request_type      * request,
  icb_bw_request_list_type * request_list
)
{
  void * old_list;

  if( NULL == request_list || NULL == request )
  {
    return;
  }

  /* Increase the size of the list if necessary */
  if( request_list->num_entries + 1 >= request_list->list_size )
  {
    /* Save the old list */
    old_list = request_list->requests;

    if(DAL_SUCCESS !=
       DALSYS_Malloc( (request_list->list_size + REQUEST_LIST_SIZE) * sizeof(void*),
                      (void **)&request_list->requests ))
    {
      request_list->requests = old_list;
      return;
    }

    if( NULL != old_list )
    {
      /* Copy and free old buffer. */
      memscpy(
        (void*)request_list->requests,
        (request_list->list_size + REQUEST_LIST_SIZE)* sizeof(void*),
        old_list,
        request_list->list_size * sizeof(void*));
      DALSYS_Free( old_list );
      old_list = NULL;
    }
    request_list->list_size += REQUEST_LIST_SIZE;
  }

  request_list->requests[request_list->num_entries++] = request;
}

/** ul_remove_bw_request
 * 
 * @brief Removes a BW request from the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param request 
 * @param request_list
 */
void ul_remove_bw_request
(
  icb_bw_request_type      * request,
  icb_bw_request_list_type * request_list
)
{
  uint32 i;

  if( NULL == request_list || NULL == request )
  {
    return;
  }

  for( i = 0; i < request_list->num_entries; i++ )
  {
    if( request_list->requests[i] == request )
    {
      /* We found the request to remove */
      break;
    }
  }

  if( i < request_list->num_entries )
  {
    memsmove(&request_list->requests[i],
             (request_list->num_entries - i)*sizeof(void*),
             &request_list->requests[i+1],
             (request_list->num_entries - i - 1)*sizeof(void*));
    request_list->num_entries--;
  }
}

/** ul_add_hw_request
 * 
 * @brief Adds a hw request to the passed request list
 * 
 * @author seansw
 * 
 * @param request 
 * @param request_list
 */
void ul_add_hw_request
(
  icb_hw_request_type      * request,
  icb_hw_request_list_type * request_list
)
{
  if( NULL == request_list || NULL == request )
  {
    return;
  }

  /* Request list is statically allocated.
   * Make sure we don't attempt to exceed our allocated size. */
  CORE_VERIFY(request_list->num_entries < request_list->list_size);

  request_list->requests[request_list->num_entries++] = request;
}

/*==============================================================================

  FUNCTION   ul_int_init

  DESCRIPTION 
    Initialize and define the bus arbiter internal data structures

  PARAMETERS
    None

  RETURN VALUE    
    A pointer to the ICB info structure.
 
==============================================================================*/
icb_info_type *ul_int_init( void )
{
  uint32_t index, i, j;

  /* Only initialize if we haven't tried before. */
  if( ICB_SW_STATE_UNINITIALIZED != sw_state )
  {
    return NULL;
  }

  /* Set state to initializing. */
  sw_state = ICB_SW_STATE_INITIALIZING;

  /* Retrieve from DAL properties, bail if not found. */
  icb_info = (icb_info_type *)AxiCfgInt_GetProperty( "icb_info" );
  if( NULL == icb_info )
  {
    return NULL;
  }

  /* Initialize each topology populated. */
  for( index = 0; index < icb_info->num_topologies; index++ )
  {
    icb_topology_type *topology = icb_info->topologies[index];

    /* Initialize hw request structures. */
    for( i = 0; i < topology->num_hw_nodes; i++ )
    {
      icb_hw_node_type *hw_node = topology->hw_nodes[i];

      hw_node->request.node = hw_node;
      memset(hw_node->request_list.requests, 0,
            sizeof(icb_hw_request_type *) * hw_node->request_list.list_size);
    }

    /* Initialize master request structures. */
    for( i = 0; i < topology->num_masters; i++ )
    {
      icb_master_def_type *master = topology->masters[i];

      master->bw_request.node      = master;
      master->latency_request.node = master;
      master->hw_request.node      = master;

      /* Add this master to the request list of any hw
      * node it requires. */
      for( j = 0; j < master->num_bw_nodes; j++ )
      {
        ul_add_hw_request(&master->hw_request, &master->bw_nodes[j]->request_list);
      }
      for( j = 0; j < master->num_latency_nodes; j++ )
      {
        ul_add_hw_request(&master->hw_request, &master->latency_nodes[j]->request_list);
      }
    }

    /* Initialize slave request structures. */
    for( i = 0; i < topology->num_slaves; i++ )
    {
      icb_slave_def_type *slave = topology->slaves[i];

      slave->bw_request.node = slave;
      slave->hw_request.node = slave;

      /* Add this master to the request list of any hw
      * node it requires. */
      for( j = 0; j < slave->num_bw_nodes; j++ )
      {
        ul_add_hw_request(&slave->hw_request, &slave->bw_nodes[j]->request_list);
      }
    }
  }

  /* First state init complete. Set to HOLD state. */
  sw_state = ICB_SW_STATE_HOLD;

  return icb_info;
}

/*==============================================================================

  FUNCTION   ul_int_post_init

  DESCRIPTION 
    Flush hardware nodes and set to run mode.

  PARAMETERS
    None

  RETURN VALUE    
    None

==============================================================================*/
void ul_int_post_init( void )
{
  uint32_t index, i;

  /* Set our state to flush, so that we short cut some aggregation. */
  sw_state = ICB_SW_STATE_FLUSH;

  /* Flush all known topologies. */
  for( index = 0; index < icb_info->num_topologies; index++ )
  {
    icb_topology_type *topology = icb_info->topologies[index];

    /* Flush all hardware nodes in preparation for transition to RUN state. */
    for( i = 0; i < topology->num_hw_nodes; i++ )
    {
      icb_hw_node_type *node = topology->hw_nodes[i];

      /* Place requests in hardware node queue.
      * we'll want the flush to do the same. */
      request_enqueue(&hw_node_queue, &node->request);
    }

    /* Flush out the queue using the default RSC (index 0).
    * No need to wait for it to finish. */
    ul_request_commit(topology, NPA_REQUEST_FIRE_AND_FORGET);
  }

  /* Flush complete, transition to RUN state. */
  sw_state = ICB_SW_STATE_RUN;
}

/** @copydoc ul_request_commit */
void ul_request_commit
(
  icb_topology_type *topology,
  unsigned int       request_attr
)
{
  icb_request_type *req;

  /* Loop through the entire sw request queue */
  req  = sw_node_queue;
  while( NULL != req )
  {
    /* Dequeue the request */
    req->is_queued = false;
    sw_node_queue = req->next;
    req->next = NULL;

    /* Issue the request, based on the type. */
    switch( req->type )
    {
      case ICB_MASTER_BW_REQUEST:
        process_master_bw_request((icb_master_def_type *)req->node);
        break;
      case ICB_MASTER_LATENCY_REQUEST:
        process_master_latency_request((icb_master_def_type *)req->node);
        break;
      case ICB_SLAVE_BW_REQUEST:
        process_slave_bw_request((icb_slave_def_type *)req->node);
        break;
      default:
        /* Unrecognized type, skip it. */
        break;
    }

    /* Next item in list */
    req = sw_node_queue;
  }

  /* Now that we've processed the sw node requests,
   * we should have some hardware requests to handle. */
  req = hw_node_queue;
  while( NULL != req )
  {
    /* Dequeue the request */
    req->is_queued = false;
    hw_node_queue = req->next;
    req->next = NULL;

    /* Issue the request. */
    switch( req->type )
    {
      case ICB_HW_BANDWIDTH_REQUEST:
        process_hw_bw_request((icb_hw_node_type *)req->node);
        break;
      case ICB_HW_LATENCY_REQUEST:
        process_hw_latency_request((icb_hw_node_type *)req->node);
        break;
      case ICB_HW_VECTOR_REQUEST:
        process_hw_vector_request((icb_hw_node_type *)req->node);
        break;
      default:
        /* Unrecognized type, skip it. */
        break;
    }

    /* Next item in list */
    req = hw_node_queue;
  }

  /* Final commit to hardware. */
  AxiCfgInt_Commit( topology, request_attr );
}

/** @copydoc ul_get_route */
icb_route_list_type *ul_get_route
(
  uint32_t         index,
  ICBId_MasterType master,
  ICBId_SlaveType  slave
)
{
  uint64_t hash;

  /* Check for index. */
  if( index >= icb_info->num_topologies )
  {
    return NULL;
  }

  /* Find the route in the tree */
  hash =  ((uint64_t)master & 0xFFFFFFFF) << 32;
  hash |= ((uint64_t)slave & 0xFFFFFFFF);

  return (icb_route_list_type*)ul_TreeLookup( icb_info->topologies[index]->route_tree_root, hash );
}

/* @copydoc ul_target_get_master_list */
void ul_target_get_master_list
(
  ul_int_master_list_type *master_list
)
{
  /* TODO: figure out the right way to do this */
  master_list->num_masters = icb_info->topologies[0]->num_masters;
  master_list->masters     = icb_info->topologies[0]->masters;
}


/* @copydoc ul_target_get_slave_list */
void ul_target_get_slave_list
(
  ul_int_slave_list_type *slave_list
)
{
  /* TODO: figure out the right way to do this */
  slave_list->num_slaves = icb_info->topologies[0]->num_slaves;
  slave_list->slaves     = icb_info->topologies[0]->slaves;
}

/** @copydoc ul_get_master_state */
icb_node_state_type *ul_get_master_state
(
  ICBId_MasterType master
)
{
  icb_node_state_type *state = NULL;
  uint32_t i;

  /* TODO: figure out the right way to do this */
  for( i = 0; i < icb_info->topologies[0]->num_masters; i++ )
  {
    if( master == icb_info->topologies[0]->masters[i]->id )
    {
      state = &icb_info->topologies[0]->masters[i]->state;
      break;
    }
  }

  return state;
}

/** @copydoc ul_get_slave_state */
icb_node_state_type *ul_get_slave_state
(
  ICBId_SlaveType slave
)
{
  icb_node_state_type *state = NULL;
  uint32_t i;

  /* TODO: figure out the right way to do this */
  for( i = 0; i < icb_info->topologies[0]->num_slaves; i++ )
  {
    if( slave == icb_info->topologies[0]->slaves[i]->id )
    {
      state = &icb_info->topologies[0]->slaves[i]->state;
      break;
    }
  }

  return state;
}

/** @copydoc ul_find_master */
icb_master_def_type *ul_find_master( ICBId_MasterType id )
{
  uint32_t i;
  icb_master_def_type *master = NULL;

  /* TODO: figure out the right way to do this */
  for( i = 0;
       i < icb_info->topologies[0]->num_masters;
       i++ )
  {
    if( id == icb_info->topologies[0]->masters[i]->id )
    {
      master = icb_info->topologies[0]->masters[i];
    }
  }

  return master;
}

/** @copydoc ul_find_slave */
icb_slave_def_type *ul_find_slave( ICBId_SlaveType id )
{
  uint32_t i;
  icb_slave_def_type *slave = NULL;

  /* TODO: figure out the right way to do this */
  for( i = 0;
       i < icb_info->topologies[0]->num_slaves;
       i++ )
  {
    if( id == icb_info->topologies[0]->slaves[i]->id )
    {
      slave = icb_info->topologies[0]->slaves[i];
    }
  }

  return slave;
}

/*============================================================================
                       Internal functions - Query 
============================================================================*/
/** ul_add_hw_query
 * 
 * @brief Adds a hw query to the passed request list
 * 
 * @author seansw
 * 
 * @param request 
 * @param request_list
 */
void ul_add_hw_query
(
  icb_hw_request_type      * request,
  icb_hw_request_list_type * request_list
)
{
  uint32_t idx;

  if( NULL == request_list || NULL == request )
  {
    return;
  }

  /* Check the list to make sure we aren't adding a duplicate. */
  for( idx = 0; idx < request_list->num_entries; idx++ )
  {
    if( request_list->requests[idx] == request )
    {
      break;
    }
  }

  /* If we've reached the end, try to add a new entry. */
  if( idx == request_list->num_entries )
  {
    /* Request list is statically allocated.
    * Make sure we don't attempt to exceed our allocated size. */
    CORE_VERIFY(request_list->num_entries < request_list->list_size);

    request_list->requests[request_list->num_entries++] = request;
  }
}

void process_master_bw_query
(
  icb_master_def_type *master
)
{
  uint32_t req_idx;

  /* Do we have a request with this query? */
  bool is_request = 0 != master->query_list.num_entries;

  /* Is there a request along with this query? */
  if( is_request )
  {
    /* Compute query state using the query list. */
    update_sw_bandwidth_state(&master->query_state,
                              &master->query_list,
                              master->num_agg_ports);
  }

  /* Request to the associated BCMs */
  for( req_idx = 0; req_idx < master->num_bw_nodes; req_idx++ )
  {
    icb_hw_node_type *hw_node = master->bw_nodes[req_idx];

    request_enqueue( &hw_node_queue, &hw_node->request );
  }
}

void process_master_latency_query
(
  icb_master_def_type *master
)
{
  uint32_t req_idx;

  /* Do we have a request with this query? */
  bool is_request = 0 != master->query_list.num_entries;

  /* Is there a request along with this query? */
  if( is_request )
  {
    /* Compute query state using the query list. */
    update_sw_latency_state( &master->query_state,
                             &master->query_list );
  }
  /* Request to the associated BCM */
  for( req_idx = 0; req_idx < master->num_latency_nodes; req_idx++ )
  {
    icb_hw_node_type *hw_node = master->latency_nodes[req_idx];

    request_enqueue( &hw_node_queue, &hw_node->request );
  }
}

void process_slave_bw_query
(
  icb_slave_def_type *slave
)
{
  uint32_t req_idx;

  /* Do we have a request with this query? */
  bool is_request = 0 != slave->query_list.num_entries;

  /* Is there a request along with this query? */
  if( is_request )
  {
    /* Compute query state using the query list. */
    update_sw_bandwidth_state(&slave->query_state,
                              &slave->query_list,
                              slave->num_agg_ports);
  }

  /* Request to the associated BCM */
  for( req_idx = 0; req_idx < slave->num_bw_nodes; req_idx++ )
  {
    icb_hw_node_type *hw_node = slave->bw_nodes[req_idx];

    request_enqueue( &hw_node_queue, &hw_node->request );
  }
}

/**
 * update_hw_bandwidth_query
 *
 * @brief Calculate hw node bandwidth state for a query request
 *
 * @param A pointer to the hw node to evaluate
 *
*/
void update_hw_bandwidth_query
(
  icb_hw_node_type *hw_node
)
{
  uint64_t ib = 0, supp_ib = 0, ab = 0, supp_ab = 0;
  uint32_t req_idx, node_width, node_ports, bcm_width = hw_node->width;
  icb_node_state_type *state;

  /* Find the max for each bandwidth category. */
  for( req_idx = 0; req_idx < hw_node->query_list.num_entries; req_idx++ )
  {
    icb_hw_request_type *request = hw_node->query_list.requests[req_idx];

    /* Fetch the right state object and node width. */
    switch(request->type)
    {
      case ICB_MASTER_HW_REQUEST:
      {
        icb_master_def_type *master = (icb_master_def_type *)request->node;
        state      = &master->query_state;
        node_width = master->width;
        node_ports = MAX(1, master->num_agg_ports);
        break;
      }
      case ICB_SLAVE_HW_REQUEST:
      {
        icb_slave_def_type *slave = (icb_slave_def_type *)request->node;
        state      = &slave->query_state;
        node_width = slave->width;
        node_ports = MAX(1, slave->num_agg_ports);
        break;
      }
      default:
        /* Ignore any type we don't expect. */
        continue;
    }
    
    /* MAX aggregation over our requests, scaled by:
     * 1.) Ratio of bcm_width to node width
     * 2.) AB requests to be scaled to per channel */
    if(ICB_HW_NODE_KIND_BANDWIDTH == hw_node->type)
    {
      ib      = MAX(ib,      BW_SCALE(state->ib,bcm_width,node_width));
      supp_ib = MAX(supp_ib, BW_SCALE(state->supp_ib,bcm_width,node_width));
      ab      = MAX(ab,      BW_SCALE(state->ab,bcm_width,node_width*node_ports));
      supp_ab = MAX(supp_ab, BW_SCALE(state->supp_ab,bcm_width,node_width*node_ports));
    }
    else
    {
      ib      = MAX(ib,      state->ib);
      supp_ib = MAX(supp_ib, state->supp_ib);
      ab      = MAX(ab,      state->ab);
      supp_ab = MAX(supp_ab, state->supp_ab);
    }
  }

  /* Save back to our state. */
  hw_node->query_state.total_bw = MAX(MAX(ib, supp_ib), ab + supp_ab);
  hw_node->query_state.ib       = ib;
  hw_node->query_state.supp_ib  = supp_ib;
  hw_node->query_state.ab       = ab;
  hw_node->query_state.supp_ab  = supp_ab;
}

void process_hw_bw_query
(
  icb_hw_node_type *hw_node
)
{
  /* Do we have a request with this query? */
  bool is_request = 0 != hw_node->query_list.num_entries;

  /* Compute new hw node state if necessary. */
  if( is_request )
  {
    update_hw_bandwidth_query(hw_node);
  }
  /* Otherwise use current state. */
  else
  {
    hw_node->query_state = hw_node->state;
  }

  /* Commit to for query. */
  AxiCfgInt_SetBandwidth(hw_node, &hw_node->query_state);
}

void process_hw_vector_query
(
  icb_hw_node_type *hw_node
)
{
  /* Do we have a request with this query? */
  bool is_request = 0 != hw_node->query_list.num_entries;

  /* Compute new hw node state if necessary. */
  if( is_request )
  {
    update_hw_bandwidth_query(hw_node);
  }
  /* Otherwise use current state. */
  else
  {
    hw_node->query_state = hw_node->state;
  }

  /* Commit to for query. */
  AxiCfgInt_SetVector(hw_node, &hw_node->query_state);
}

void process_hw_latency_query
(
  icb_hw_node_type *hw_node
)
{
  uint32_t latency_ns = 0, supp_latency_ns = 0;
  uint32_t req_idx;
  icb_node_state_type *state;

  /* Do we have a request with this query? */
  bool is_request = 0 != hw_node->query_list.num_entries;

  /* Find the min non-zero for each latency category, but only if this is a request. */
  if( is_request )
  {
    for( req_idx = 0; req_idx < hw_node->query_list.num_entries; req_idx++ )
    {
      icb_hw_request_type *request = hw_node->query_list.requests[req_idx];

      /* Fetch the right state object. */
      switch(request->type)
      {
        case ICB_MASTER_HW_REQUEST:
          state = &(((icb_master_def_type *)request->node)->query_state);
          break;
        default:
          /* Ignore any type we don't expect. */
          continue;
      }
      
      /* Min non-zero aggregation, because zero means "don't care". */
      if( 0 == latency_ns ||
          (0 != state->latency_ns &&
          state->latency_ns < latency_ns) )
      {
        latency_ns = state->latency_ns;
      }
      if( 0 == supp_latency_ns ||
          (0 != state->supp_latency_ns &&
          state->supp_latency_ns < supp_latency_ns) )
      {
        supp_latency_ns = state->supp_latency_ns;
      }
    }

    /* Save back to our state. */
    hw_node->query_state.latency_ns      = latency_ns;
    hw_node->query_state.supp_latency_ns = supp_latency_ns;
  }
  else
  {
    hw_node->query_state = hw_node->state;
  }

  /* Commit to for query. */
  AxiCfgInt_SetLatency(hw_node, &hw_node->query_state);
}

/*============================================================================
                       External functions - Query 
============================================================================*/
/** @copydoc ul_query_commit */
void ul_query_commit( void )
{
  icb_request_type *req;

  /* Loop through the entire sw request queue */
  req  = sw_node_queue;
  while( NULL != req )
  {
    /* Dequeue the request */
    req->is_queued = false;
    sw_node_queue = req->next;
    req->next = NULL;

    /* Issue the request, based on the type. */
    switch( req->type )
    {
      case ICB_MASTER_BW_REQUEST:
        process_master_bw_query((icb_master_def_type *)req->node);
        break;
      case ICB_MASTER_LATENCY_REQUEST:
        process_master_latency_query((icb_master_def_type *)req->node);
        break;
      case ICB_SLAVE_BW_REQUEST:
        process_slave_bw_query((icb_slave_def_type *)req->node);
        break;
      default:
        /* Unrecognized type, skip it. */
        break;
    }

    /* Next item in list */
    req = sw_node_queue;
  }

  /* Now that we've processed the sw node requests,
   * we should have some hardware requests to handle. */
  req = hw_node_queue;
  while( NULL != req )
  {
    /* Dequeue the request */
    req->is_queued = false;
    hw_node_queue = req->next;
    req->next = NULL;

    /* Issue the request. */
    switch( req->type )
    {
      case ICB_HW_BANDWIDTH_REQUEST:
        process_hw_bw_query((icb_hw_node_type *)req->node);
        break;
      case ICB_HW_LATENCY_REQUEST:
        process_hw_latency_query((icb_hw_node_type *)req->node);
        break;
      case ICB_HW_VECTOR_REQUEST:
        process_hw_vector_query((icb_hw_node_type *)req->node);
        break;
      default:
        /* Unrecognized type, skip it. */
        break;
    }

    /* Next item in list */
    req = hw_node_queue;
  }
}

/** ul_issue_query_request
 * 
 * @brief Issue a query request for the given master slave pair 
 * 
 */
void ul_issue_query_request
(
  icb_bw_request_type *request,
  icb_pair_type       *pair
)
{
  icb_master_def_type *master = pair->master;
  icb_slave_def_type  *slave  = pair->slave;

  /* Add query request to node request queues if
   * the request is not for current state. */
  if( NULL != request )
  {
    uint32_t i;
    ul_add_bw_request(request, &slave->query_list);
    for( i = 0; i < slave->num_bw_nodes; i++ )
    {
      ul_add_hw_query(&slave->hw_request, &slave->bw_nodes[i]->query_list);
    }

    ul_add_bw_request(request, &master->query_list);
    for( i = 0; i < master->num_bw_nodes; i++ )
    {
      ul_add_hw_query(&master->hw_request, &master->bw_nodes[i]->query_list);
    }
    for( i = 0; i < master->num_latency_nodes; i++ )
    {
      ul_add_hw_query(&master->hw_request, &master->latency_nodes[i]->query_list);
    }
  }

  /* If the latency request needs updating, queue it.
   * We'll service that later once we've processed all nodes initially. */
  if( 0 < master->num_latency_nodes )
  {
    request_enqueue( &sw_node_queue, &master->latency_request );
  }

  /* Queue requests for consolidated processing later. */
  request_enqueue( &sw_node_queue, &master->bw_request );
  request_enqueue( &sw_node_queue, &slave->bw_request );
}

/**
 * @copydoc ul_fetch_hw_queue_length
 */
uint32_t ul_fetch_hw_queue_length
(
  icb_topology_type *topology
)
{
  return AxiCfgInt_GetQueueLength(topology);
}

/**
 * @copydoc ul_fetch_hw_queue
 */
uint32_t ul_fetch_hw_queue
(
  icb_topology_type * topology,
  rpmh_command_set_t *command_sets,
  uint32_t            num_command_sets
)
{
  return AxiCfgInt_GetQueue(topology, command_sets, num_command_sets);
}


/** ul_flush_query
 * 
 * @brief Flush all results from previous queries
 * 
 */
void ul_flush_query
(
  icb_topology_type *topology
)
{
  uint32_t i;

  /* Clear query states for all nodes in this topology. */
  for( i = 0; i < topology->num_masters; i++ )
  {
    icb_master_def_type *master = topology->masters[i];
    memset(&master->query_state, 0, sizeof(icb_node_state_type));
    master->query_list.num_entries = 0;
  }
  for( i = 0; i < topology->num_slaves; i++ )
  {
    icb_slave_def_type *slave = topology->slaves[i];
    memset(&slave->query_state, 0, sizeof(icb_node_state_type));
    slave->query_list.num_entries = 0;
  }
  for( i = 0; i < topology->num_hw_nodes; i++ )
  {
    icb_hw_node_type *hw_node = topology->hw_nodes[i];
    memset(&hw_node->query_state, 0, sizeof(icb_node_state_type));
    hw_node->query_list.num_entries = 0;
  }

  /* Instruct hw layer to flush it's queues. */
  AxiCfgInt_Flush();
}

/*============================================================================
                   External functions - Standalone BCMs
============================================================================*/
/** ul_issue_resource_request
 * 
 * @brief Issue a request for a singleton resource
 * 
 */
void ul_issue_resource_request
(
  npa_client_handle client,
  npa_resource *    resource
)
{
  icb_standalone_data_type *    resource_data = (icb_standalone_data_type *)resource->definition->data;
  icb_rpmh_handle_type *        rpmh          = &resource_data->rpmh_handle;
  icb_hw_node_type *            hw_node       = resource_data->hw_node;

  /* Update hw node state. */
  hw_node->state.ib       = NPA_RESOURCE_REQUIRED_REQUEST(resource);
  hw_node->state.supp_ib  = NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource);
  hw_node->state.total_bw = MAX(hw_node->state.ib, hw_node->state.supp_ib);

  AxiCfgInt_CommitSingle(rpmh, hw_node, client->request_attr);
}
