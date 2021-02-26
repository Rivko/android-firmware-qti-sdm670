#ifndef __DDIAXICFGINT_H__
#define __DDIAXICFGINT_H__
/*=============================================================================

@file   ddiaxicfgint.h

@brief  Internal header file for bus configuration APIs.

when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/02/24   sds     Add single shot BCM requests
2016/11/08   sds     Add BCM query functions
2016/10/06   sds     Add vector request type
2016/02/22   sds     Branch for Hoya restructure.
2011/12/05   dj      Created file

===========================================================================
            Copyright (c) 2011-2017 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/
#include <stdint.h>
#include <stdbool.h>
#include "icbid.h"
#include "axicfgos.h"
#include "icb_soc_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  @brief  The first method called to initialize AxiCfg driver. 
          Will set up all the required resources including locks, memory maps

  @return bool
      @retval true  if successful
      @retval false if failed or not yet complete
 */
bool AxiCfgInt_Init( void );

/**
  @brief  The method called to reprogram the hardware to default/reset values.

  @return bool
      @retval true  if successful
      @retval false if failed
 */
bool AxiCfgInt_Reset( void );

/**
  @brief Configures the QoS Generator bandwidth for a master.
         Valid only when master QOS mode is either Limited, Regulator, or remote.

  @param[in] master   Master port structure
 */
void AxiCfgInt_QoSSetMasterBW
( 
  icb_master_def_type *master
);

/**
  @brief Configures the bandwidth request for a hardware node.

  @param[in]  hw_node Pointer to the hardware node
  @param[in]  state   Pointer to the hardware node state to use
 */
void AxiCfgInt_SetBandwidth
( 
  icb_hw_node_type *   hw_node,
  icb_node_state_type *state
);

/**
  @brief Configures the request for a hardware vector node.

  @param[in]  hw_node Pointer to the hardware node
  @param[in]  state   Pointer to the hardware node state to use
 */
void AxiCfgInt_SetVector
( 
  icb_hw_node_type *   hw_node,
  icb_node_state_type *state
);

/**
  @brief Configures the latency request for a hardware node.

  @param[in]  hw_node Pointer to the hardware node
  @param[in]  state   Pointer to the hardware node state to use
 */
void AxiCfgInt_SetLatency
(
  icb_hw_node_type *   hw_node,
  icb_node_state_type *state
);

/**
  @brief  Used to fetch a value from xml property file in target config lib
          Can only be called after AxiCfgOs_Init

  @param  pString   Name of the property string

  @return uint32_t    Value of the property string
  */
#define AxiCfgInt_GetProperty( pString )  AxiCfgOs_GetProperty( pString )


/**
 @brief Commit all changes to hardware.

 @param[in] topology     The topology to commit changes to
 @param[in] request_attr The NPA request attributes for this commit
 */
void AxiCfgInt_Commit
(
  icb_topology_type *topology,
  unsigned int       request_attr
);

/**
 @brief Commit a single change to hardware.

 @param[in] topology     The rpmh handle to commit changes to
 @param[in] hw_node      The hw node to commit changes for
 @param[in] request_attr The NPA request attributes for this commit

 @note To be used for singleton requests only.
 */
void AxiCfgInt_CommitSingle
(
  icb_rpmh_handle_type *rpmh,
  icb_hw_node_type *    hw_node,
  unsigned int          request_attr
);

/**
 @brief Get the number of currently pending command sets.

 @param[in] topology The topology used to queue

 @returns The number of command sets pending
 */
uint32_t AxiCfgInt_GetQueueLength
(
  icb_topology_type *topology
);

/**
 @brief Get the currently pending command sets.

 @param[in]    topology         The topology used to queue
 @param[inout] command_sets     The buffer to store the command sets in
 @param[in]    num_command_sets The maximum number of command sets to fetch

 @returns The number of command sets returned
 */
uint32_t AxiCfgInt_GetQueue
(
  icb_topology_type * topology,
  rpmh_command_set_t *command_sets,
  uint32_t            num_command_sets
);

/**
 @brief Discard all pending changes to hardware.
 */
void AxiCfgInt_Flush( void );

#ifdef __cplusplus
}
#endif

#endif /* __DDIAXICFGINT_H__ */
