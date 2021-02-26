#ifndef __UL_I_H__
#define __UL_I_H__
/*=============================================================================

FILE:      ul_i.h

DESCRIPTION: This file defines the internal UL interface.

    Copyright (c) 2011-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/ul_i.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when        who     what, where, why
 ---------   ---     -----------------------------------------------------------
2017/03/08   sds     Change BCM-as-clock to BCM-as-standalone generic framework
2017/02/24   sds     Add support for BCM-as-clock implementation
2016/11/08   sds     Add BCM query functions
2016/01/22   sds     Branch for Hoya from 2.0.
=============================================================================*/

/*============================================================================
                          INCLUDES
============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "icbid.h"
#include "npa_resource.h"
#include "icb_soc_data.h"

/*============================================================================
                          Defines
============================================================================*/
#ifndef MAX
  #define MAX(x,y) ((x)>(y)?(x):(y))
#endif /* MAX */
#ifndef MIN
  #define MIN(x,y) ((x)<(y)?(x):(y))
#endif /* MIN */

/* Divide and round up to the nearest integer */
#define CEIL_DIV(dividend, divisor) (((dividend)/(divisor))+(((dividend)%(divisor))?1:0))

/*============================================================================
                       Type definitions
============================================================================*/
typedef struct
{
  uint32_t              num_masters;
  icb_master_def_type **masters;
} ul_int_master_list_type;

typedef struct
{
  uint32_t            num_slaves;
  icb_slave_def_type **slaves;
} ul_int_slave_list_type;

/*============================================================================
                       External functions
============================================================================*/

/** ul_issue_pair_request
 * 
 * @brief Issue request for the given master slave pair 
 * 
 */
void ul_issue_pair_request
(
  npa_client_handle    client,
  icb_bw_request_type *request,
  icb_bw_request_type *old_request,
  icb_pair_type       *pair
);

/** ul_add_bw_request
 * 
 * @brief Adds a BW request to the passed request list
 * 
 */
void ul_add_bw_request
(
  icb_bw_request_type *     request,
  icb_bw_request_list_type *request_list
);

/** ul_remove_bw_request
 * 
 * @brief Removes a BW request from the passed request list
 * 
 */
void ul_remove_bw_request
(
  icb_bw_request_type *     request,
  icb_bw_request_list_type *request_list
);

/**
 * @brief    Initialize and define the bus arbitration subnodes
 *
 */
void ul_internal_init( void );

/**
   @brief ul_get_route - Get a route associated with 
          an external master/slave pair

   This function looks up the route based upon the passed in 
   External Master ID and External Slave ID.  If the route 
   exists it will return a pointer to the route structure. 
   Otherwise it will return NULL if the route is not found. 
   
   @param index  - The topology index to use
   @param master - External Master ID
   @param slave  - External Slave ID
   
   @return ul_route_list_type* 
*/
icb_route_list_type *ul_get_route
(
  uint32_t         index,
  ICBId_MasterType master,
  ICBId_SlaveType  slave
);

/** ul_int_init
 * @brief Initializes ICB route internal data structures.
 *
 * @returns A pointer to the ICB info structure.
 */
icb_info_type *ul_int_init( void );

/** ul_int_init
 * 
 */
void ul_int_post_init( void );

/**
 * ul_request_commit
 *
 * @brief Commits all requests in the request queues
 *
 * @param The topology to commit changes to
 * @param The requesting client attributes field
 */
void ul_request_commit
(
  icb_topology_type *topology,
  unsigned int       request_attr
);

/** ul_target_init
 *
 */
void ul_target_init( void );

/** ul_target_get_route_tree
 * 
 * @return ul_treeNodeType* 
 */
ul_treeNodeType * ul_target_get_route_tree( void );

/** ul_target_get_master_list
 * 
 * @brief Get the internal master list data.
 */
void ul_target_get_master_list( ul_int_master_list_type *master_list );

/** ul_target_get_slave_list
 * 
 * @brief Get the internal slave list data.
 */
void ul_target_get_slave_list( ul_int_slave_list_type *slave_list );

/** ul_get_master_state
 *
 * @brief Get the internal node aggregated state
 */
icb_node_state_type *ul_get_master_state( ICBId_MasterType master );

/** ul_get_slave_state
 *
 * @brief Get the internal node aggregated state
 */
icb_node_state_type *ul_get_slave_state( ICBId_SlaveType slave );

/** ul_find_master
 *
 * @brief Find a master definition structure by id.
 */
icb_master_def_type *ul_find_master( ICBId_MasterType id );

/** ul_find_master
 *
 * @brief Find a master definition structure by id.
 */
icb_slave_def_type *ul_find_slave( ICBId_SlaveType id );

/** ul_notify_evaluate
 *
 * @brief Evaluate and update the current notification status for the passed notification.
 *
 */
void ul_notify_evaluate
(
  icb_notify_type *    notify,
  icb_node_state_type *state
);

/*============================================================================
                       External functions - Query 
============================================================================*/
/** ul_issue_query_request
 * 
 * @brief Issue a query request for the given master slave pair 
 *
 * @param[in] request The request value for this pair. NULL to use current state.
 * @param[in] pair    The master slave pair to aggregate
 * 
 */
void ul_issue_query_request
(
  icb_bw_request_type *request,
  icb_pair_type       *pair
);

/**
 * ul_query_commit
 *
 * @brief Commits all requests in the request queues for queries
 */
void ul_query_commit( void );

/** ul_fetch_hw_queue
 * 
 * @brief Get the current bcm command set queue length
 * @param[in]    topology         The topology used to queue
 * 
 * @returns The current queue length
 */
uint32_t ul_fetch_hw_queue_length
(
  icb_topology_type *topology
);

/** ul_fetch_hw_queue
 * 
 * @brief Get the current bcm command set queue command list
 *
 * @param[in]    topology         The topology used to queue
 * @param[inout] command_sets     Location to store command sets
 * @param[in]    num_command_sets Maximum number of command sets to fetch
 *
 * @returns The actual length returned
 */
uint32_t ul_fetch_hw_queue
(
  icb_topology_type * topology,
  rpmh_command_set_t *command_sets,
  uint32_t            num_command_sets
);

/** ul_flush_query
 * 
 * @brief Flush all results from previous queries
 * 
 */
void ul_flush_query( icb_topology_type *topology );

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
);

#endif /* __UL_I_H__ */
