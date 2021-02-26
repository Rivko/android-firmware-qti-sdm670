/**

@file   icb_aop.c

@brief  Implementation file for the ICB AOP library.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/common/icb_aop.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2017/09/01   sds     Call rpmh_barrier_all only. Do not call churn.
2017/06/05   sds     Add corner matching framework
2017/04/04   sds     Fix completion guard
2016/12/22   sds     Add post init
2016/11/23   sds     Add support for async completion callbacks
2016/09/20   sds     Created 

===========================================================================
        Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.
                          All Rights Reserved.
                          QUALCOMM Proprietary
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "icb_aopi.h"
#include "CoreVerify.h"
#include "cmd_db.h"
#include "cmd_db_bcm.h"
#include "rpmh_client.h"
#include <string.h>
#include <stdlib.h>
#include "rinit.h"
#include "rex.h"
#include "clock.h"
#include "bcm_hwio.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define VLVL_HISTORY_CNT         5
#define INITIAL_COMPLETION_COUNT 10

/* RPM message id update macro
 * Ignore new message ids if zero.
 */
#define RPM_MSG_ID_UPDATE(msg_id, new_msg_id) (msg_id) = (new_msg_id)? (new_msg_id): (msg_id)

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static icb_info_t *icb_info = NULL;
static icb_bcm_t * commit_queue = NULL;
static rpmh_client_handle rpmh_handle = NULL;

/* Completion queue. */
static rex_crit_sect_type completion_lock;
static icb_completion_t *completion_queue = NULL;
static icb_completion_t *completion_pool = NULL;
static icb_completion_t initial_completion_pool[INITIAL_COMPLETION_COUNT];

/* Match VCD data. */
rex_crit_sect_type match_lock;
static volatile int vlvl_hist[VLVL_HISTORY_CNT];
static uint32_t hist_idx;

/* -----------------------------------------------------------------------
** Internal Functions - Callbacks
** ----------------------------------------------------------------------- */
static void queue_completion
(
  icb_completion_t *completion
)
{
  rex_enter_crit_sect(&completion_lock);
  completion->next = completion_queue;
  completion_queue = completion;
  rex_leave_crit_sect(&completion_lock);
}

static icb_completion_t *get_completion( void )
{
  icb_completion_t *completion;

  /* Check to see if we have an unused completion first. */
  if( NULL != completion_pool )
  {
    /* Pop the head of the queue. */
    rex_enter_crit_sect(&completion_lock);
    completion = completion_pool;
    completion_pool = completion->next;
    rex_leave_crit_sect(&completion_lock);
  }
  /* All completions in use, malloc a new one. */
  else
  {
    completion = (icb_completion_t *)malloc(sizeof(icb_completion_t));
  }

  /* Clear the completion struct before we pass it back. */
  if( NULL != completion )
  {
    memset(completion, 0, sizeof(icb_completion_t));
  }

  return completion;
}

static void free_completion
(
  icb_completion_t * completion
)
{
  /* Push this unused completion item on the head of the queue. */
  rex_enter_crit_sect(&completion_lock);
  completion->next = completion_pool;
  completion_pool = completion;
  rex_leave_crit_sect(&completion_lock);
}

static void initialize_completion_queue( void )
{
  /* Create critical section for access to completion queue. */
  rex_init_crit_sect(&completion_lock);

  /* Add our statically allocated items to the completion pool. */
  memset(initial_completion_pool, 0, sizeof(initial_completion_pool));

  rex_enter_crit_sect(&completion_lock);
  for(uint32_t i = 0; i < INITIAL_COMPLETION_COUNT; i++)
  {
    initial_completion_pool[i].next = completion_pool;
    completion_pool = &initial_completion_pool[i];
  }
  rex_leave_crit_sect(&completion_lock);
}

void icb_rpmh_completion_callback
(
   uint32 req_id
)
{
  icb_completion_t *completion = NULL, *prev = NULL;

  /* Search the completion queue for a match. */
  rex_enter_crit_sect(&completion_lock);
  completion = completion_queue;
  while( NULL != completion )
  {
    /* If our request ids match, pop from the queue. */
    if( req_id == completion->req_id )
    {
      /* Check to see if we're popping the head. */
      if( completion == completion_queue )
      {
        completion_queue = completion->next;
      }
      /* Popping somewhere else. */
      else
      {
        prev->next = completion->next;
      }
      break;
    }
    prev = completion;
    completion = completion->next;
  }
  rex_leave_crit_sect(&completion_lock);
  
  /* If we found a matching client, call it's callback */
  if( NULL != completion )
  {
    icb_client_t *client = completion->client;
    client->callback(client->callback_data, completion->req_id);

    /* Return the completion to our pool. */
    free_completion(completion);
  }
}

/* -----------------------------------------------------------------------
** Internal Functions - BCM processing
** ----------------------------------------------------------------------- */
/**
@brief Insert a bcm to the commit queue.

@param[in] bcm - The bcm handle to queue
*/
static void request_enqueue
(
  icb_bcm_t * bcm
)
{
  icb_bcm_t *iter, *prev;

  /* Insert into the queue, sorted by clock id. */
  for( iter = commit_queue, prev = NULL;
       iter != NULL;
       prev = iter, iter = iter->next )
  {
    /* Only break out if the clock id of the node to be inserted
     * is less than the current node to be compared.
     * We may want to also compare node names at some point. */
    if( bcm->clk_id < iter->clk_id )
    {
      break;
    }
  }

  /* Find out where we're inserting, as it changes what we do.
   * If we're inserting before the head, we need to replace it. */
  if( iter == commit_queue )
  {
    bcm->next = commit_queue;
    commit_queue = bcm;
  }
  /* Insertion somewhere else in the list.
   * Note, this works even if we're inserting at the end, as iter
   * ends up being NULL. */
  else
  {
    prev->next = bcm;
    bcm->next  = iter;
  }
}

/**
@brief Commit any pending requests.
*/
static uint32_t bcm_commit
(
  icb_client_t *client,
  bool          completion
)
{
  icb_bcm_t *bcm;
  rpmh_command_set_t command_set;
  uint32_t num_cmds = 0, barrier_id = 0;
  rpmh_client_handle rpmh;

  /* Don't commit empty queues. */
  if( NULL == commit_queue )
  {
    return 0;
  }
  
  /* First, initialize our command set structure for first VCD. */
  memset(&command_set, 0, sizeof(rpmh_command_set_t));

  /* Go through our list of bcms. */
  while( NULL != commit_queue )
  {
    /* Pop the head of the queue. */
    bcm = commit_queue;
    commit_queue = bcm->next;
    bcm->next = NULL;

    /* Store this command. */
    num_cmds++;
    command_set.commands[num_cmds-1].address = bcm->hw_id;

    /* Use BCM's static request value if our ref_cnt is non-zero.
     * Otherwise, leave it as zero. */
    if( bcm->ref_cnt > 0 )
    {
      command_set.commands[num_cmds-1].data  = bcm->vote;
    }

    /* Issue the current command set if:
     * 1.) We're at the end of the queue.
     * 2.) The next bcm in the queue is on a different VCD
     * 3.) We're about to exceed the size of the command array
     */
    if( NULL == commit_queue ||
        bcm->clk_id != commit_queue->clk_id ||
        num_cmds == IMAGE_TCS_SIZE )
    {
      uint32_t msg_id;

      /* Set the commit bit since we're at the end of a VCD.
       * Set the completion flag if we need to wait for a response.
       * Update bookkeeping in the command set. */
      command_set.commands[num_cmds-1].data       |= BCM_VOTETABLE_COMMIT_BMSK;
      command_set.commands[num_cmds-1].completion  = completion;
      command_set.set                              = RPMH_SET_ACTIVE;
      command_set.num_commands                     = num_cmds;

      msg_id = rpmh_issue_command_set(rpmh_handle, &command_set);
      RPM_MSG_ID_UPDATE(barrier_id, msg_id);
      memset(&command_set, 0, sizeof(rpmh_command_set_t));
      num_cmds = 0;
    }
  }

  /* If the client asks for completion and we have a request to wait for
   * call the correct completion API from RPMh driver. */
  if( completion && barrier_id != 0 )
  {
    /* Blocking call.
     * Call RPMh barrier. */
    if( NULL == client->callback )
    {
      rpmh_barrier_all(rpmh_handle, barrier_id);
    }
    /* Async request.
     * Queue a completion to our completion queue and
     * tell RPMh driver that we're waiting for it. */
    else
    {
      icb_completion_t *completion;

      CORE_VERIFY_PTR(completion = get_completion());
      completion->client = client;
      completion->req_id = barrier_id;
      queue_completion(completion);

      rpmh_notify_completion_all(rpmh_handle, barrier_id);
    }
  }

  return barrier_id;
}

/* -----------------------------------------------------------------------
** Internal Functions - Initialization
** ----------------------------------------------------------------------- */
static void initialize_match_vcds( void )
{
  int CX_OFF = pwr_utils_hlvl_named_resource("cx.lvl", RAIL_VOLTAGE_LEVEL_OFF);

  memset((void *)vlvl_hist, 0, sizeof(vlvl_hist));
  hist_idx = 0;

  rex_init_crit_sect(&match_lock);

  for( uint32_t cd = 0;
       cd < icb_info->num_match_vcds;
       cd++ )
  {
    uint32_t cp;
    icb_match_vcd_t *match_vcd = &icb_info->match_vcds[cd];

    /* Enable SW override, but start at CP 0. */
    HWIO_OUTI(SW_CP_CDm, match_vcd->vcd, 1 << HWIO_SHFT(SW_CP_CDm, VALID));

    /* Cache the hlvl that has been programmed. */
    for( cp = 0; cp < MAX_CPS; cp++ )
    {
      match_vcd->hlvls[cp] = 
        (uint8_t)HWIO_INFI2(TCS_CFG_VOTE_DATA_CDm_CPn, match_vcd->vcd, cp, CMD0_DATA);

      /* If we see off after the first level, we're done reading. */
      if( CX_OFF == match_vcd->hlvls[cp] && 0 < cp )
      {
        break;
      }
    }

    match_vcd->num_hlvls = cp;
  }
}

/* -----------------------------------------------------------------------
** External interface
** ----------------------------------------------------------------------- */
/**
@copydoc icb_init()
*/
void icb_init( void )
{
  /* Get a pointer to the system resources. */
  CORE_VERIFY_PTR( icb_info = icb_get_target_data() );

  /* Fetch BCM data. */
  for( uint32_t i = 0; i < icb_info->num_bcms; i++ )
  {
    icb_bcm_t *bcm = icb_info->bcms[i];
    struct bcm_db bcm_aux;
    uint8_t len = sizeof(bcm_aux);

    /* Get BCM information for all BCMs.
     * Use aux fetch to check if the BCM exists. */
    CORE_VERIFY( CMD_DB_SUCCESS == cmd_db_query_aux_data(bcm->name, &len, (uint8_t *)&bcm_aux) );
    bcm->hw_id   = cmd_db_query_addr( bcm->name );
    bcm->clk_id  = bcm_aux.clk_id;
  }

  /* Create our RPMh client handle.
   * Register our completion callback function. */
  CORE_VERIFY_PTR( rpmh_handle = rpmh_create_handle( RSC_DRV_AOP, "ICB" ) );
  rpmh_client_register_completion_callback(rpmh_handle, icb_rpmh_completion_callback);

  /* Initialize items related to our callback completion queue. */
  initialize_completion_queue();

  /* Configure BCM hardware */
  CORE_VERIFY(RINIT_SUCCESS == rinit_module_init("bcm"));

  /* Run target specific initialization and workarounds. */
  icb_target_init(icb_info);

  /* Corner matched VCD initialization */
  initialize_match_vcds();
}

/**
@copydoc icb_post_init()
*/
void icb_post_init( void )
{
  /* Enable BCM and call target post init. */
  HWIO_OUT(BCM_CFG_BCM_EN, 0x1);
  icb_target_post_init();

  /* Wait for BCM to idle. */
  while( 0 == HWIO_IN(STATUS_BCM) );

  /* Now that BCM HW is idle, we can switch to HW controlled mode. */
  Clock_SwitchToHWCtrlMode();
}

/**
@copydoc icb_get_handle()
*/
icb_handle_t icb_get_handle
(
  ICBId_MasterType master,
  ICBId_SlaveType  slave
)
{
  return icb_get_handle_ex(master, slave, NULL, NULL);
}

/**
@copydoc icb_get_handle_ex()
*/
icb_handle_t icb_get_handle_ex
(
  ICBId_MasterType master, 
  ICBId_SlaveType  slave,
  icb_callback_t   callback,
  void *           callback_data
)
{
  icb_route_t *route = NULL;
  icb_handle_t handle = NULL;

  /* Try to find the route specified. */
  for( uint32_t i = 0; i < icb_info->num_routes; i++ )
  {

    if( master == icb_info->routes[i]->master && slave == icb_info->routes[i]->slave)
    {
      route = icb_info->routes[i];
      break;
    }
  }
  
  /* Allocate and fill out a client handle. */
  if( NULL != route )
  {
    handle = (icb_handle_t)malloc(sizeof(icb_client_t));
    if( handle )
    {
      handle->route         = route;
      handle->callback      = callback;
      handle->callback_data = callback_data;
    }
  }
  
  return handle;
}

/**
@copydoc icb_enable_route()
*/
uint32_t icb_enable_route
(
  icb_handle_t handle,
  bool         completion
)
{
  icb_route_t *route = handle->route;
  uint32_t req_id = 0;

  /* Make sure the incoming handle is valid. */
  if( NULL != route )
  {
    for( uint32_t i = 0; i < route->num_bcms; i++ )
    {
      icb_bcm_t *bcm = route->bcms[i];

      /* Increment ref cnt.
       * Issue request if the ref cnt is 1. */
      bcm->ref_cnt++;
      if( 1 == bcm->ref_cnt )
      {
        request_enqueue( bcm );
      }
    }

    req_id = bcm_commit(handle, completion);
  }

  return req_id;
}

/**
@copydoc icb_disable_route()
*/
uint32_t icb_disable_route
(
  icb_handle_t handle,
  bool         completion
)
{
  icb_route_t *route = handle->route;
  uint32_t req_id = 0;

  /* Make sure the incoming handle is valid. */
  if( NULL != route )
  {
    for( uint32_t i = 0; i < route->num_bcms; i++ )
    {
      icb_bcm_t *bcm = route->bcms[i];

      /* Decrement ref cnt, but guard.
       * Issue request if the ref cnt is 0. */
      if( bcm->ref_cnt > 0 )
      {
        bcm->ref_cnt--;
        if( 0 == bcm->ref_cnt )
        {
          request_enqueue( bcm );
        }
      }
    }

    req_id = bcm_commit(handle, completion);
  }

  return req_id;
}

/* -----------------------------------------------------------------------
** Notification interface
** ----------------------------------------------------------------------- */
/**
@copydoc icb_update_ddr_mgr_cx_state()
*/
void icb_update_ddr_cx_state( int vlvl )
{
  vlvl_hist[hist_idx++] = vlvl;
  hist_idx = hist_idx % VLVL_HISTORY_CNT;

  /* Process any corner matched VCDs we might have */
  if( 0 < icb_info->num_match_vcds )
  {
    int hlvl;

    /* If our voltage is greater than MIN_SVS, we match. */
    if( RAIL_VOLTAGE_LEVEL_MIN_SVS < vlvl )
    {
      hlvl = pwr_utils_hlvl_named_resource("cx.lvl", vlvl);
    }
    /* Otherwise, clear our match value. */
    else
    {
      hlvl = 0;
    }

    rex_enter_crit_sect(&match_lock);
    for( uint32_t cd = 0;
         cd < icb_info->num_match_vcds;
         cd++ )
    {
      int cp;
      icb_match_vcd_t *match_vcd = &icb_info->match_vcds[cd];

      /* If our hlvl is greater than 0, look up the corner matched CP */
      if( 0 < hlvl )
      {
        /* Search from the top down.
         * That way, the first level we find less than or equal to the level
         * we're searching for is our match. */
        for( cp = match_vcd->num_hlvls - 1;
             cp >= 0;
             cp-- )
        {
          /* Stop if we hit a value less than or equal to desired.
           * In the case that we hit less than, this must be the best we can do.
           * We're only trying to corner follow, not drive Cx. */
          if( hlvl >= match_vcd->hlvls[cp] )
          {
            break;
          }
        }
      }
      /* Otherwise, SW requests CP 0. */
      else
      {
        cp = 0;
      }

      /* Update SW override CP. */
      match_vcd->desired_cp = cp;

      /* If this VCD isn't currently busy, we need to issue a new SW_CP. */
      if( !match_vcd->busy )
      {
        match_vcd->busy = true;
        match_vcd->pending_cp = match_vcd->desired_cp;
        HWIO_OUT(INTERRUPT_CP_CHANGE_DONE_INT_CLEAR, 1 << match_vcd->vcd);
        HWIO_OUTM(INTERRUPT_CP_CHANGE_DONE_INT_EN, 1 << match_vcd->vcd, 1 << match_vcd->vcd);
        HWIO_OUTFI(SW_CP_CDm, match_vcd->vcd, SW_CP, match_vcd->pending_cp);
      }
    }
    rex_leave_crit_sect(&match_lock);
  }
}

/* -----------------------------------------------------------------------
** AOP services interface
** ----------------------------------------------------------------------- */
/**
@brief Interface for AOP services to notify ICB of any pending BCM services

@param[in] irq - The IRQ to handle
*/
bool icb_handle_aop_service_req
(
  int irq
)
{
  /* Forward to target layer. */
  return icb_target_service_ist(irq);
}

