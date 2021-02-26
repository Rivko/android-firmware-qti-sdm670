#ifndef ICB_SOC_DATA_H
#define ICB_SOC_DATA_H
/**

@file   icb_soc_data.h

@brief  This module provides the type definitions for ICB SoC specifc data.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/icb_soc_data.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/04/13   sds     Add request type skipped by UL for standalone
2017/04/07   sds     Add RPMh hardware base address
2017/03/08   sds     Change BCM-as-clock to BCM-as-standalone generic framework
2017/02/24   sds     Add support for BCM-as-clock implementation
2016/11/08   sds     Add BCM query functions
2016/10/31   sds     Add support for multiple RSCs
2016/08/18   sds     Add support for vector type BCMs
2016/05/03   sds     Add support for power domains during configuration
2016/01/21   sds     Branch for Hoya restructure.
2012/01/06   sds     Created 

===========================================================================
            Copyright (c) 2012-2017 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "HALnoc.h"
#include "icbid.h"
#include "npa.h"
#include "npa_resource.h"
#include "ClockDefs.h"
#include "icb_notify.h"
#include "ul_tree.h"
#include "rpmh_client.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define ICB_ARRAY(arr) ARRAY_SIZE(arr), (arr)
#define ICB_EMPTY_ARRAY 0, NULL

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef enum
{
  /* Request queue types for master and slave nodes. */
  ICB_MASTER_BW_REQUEST = 0,
  ICB_MASTER_LATENCY_REQUEST,
  ICB_SLAVE_BW_REQUEST,

  /* Request queue types for hardware nodes. */
  ICB_HW_BANDWIDTH_REQUEST,
  ICB_HW_LATENCY_REQUEST,
  ICB_HW_VECTOR_REQUEST,
  ICB_HW_NO_REQUEST,        /**< Externally managed */

  /* Request types for master and slave nodes on hardware nodes */
  ICB_MASTER_HW_REQUEST,
  ICB_SLAVE_HW_REQUEST,

  ICB_REQUEST_KIND_COUNT,
} icb_request_kind_type;

/* Request structure for node process queues */
typedef struct icb_request_queue_node icb_request_type;
typedef struct icb_request_queue_node
{
  icb_request_type *    next;
  icb_request_kind_type type;
  void *                node;
  bool                  is_queued;  /**< Has this request been queued */
} icb_request_type;

/* Request structure for rpmh process queues */
typedef struct icb_hw_node_type icb_hw_node_type;
typedef struct icb_rpmh_request_queue_node icb_rpmh_request_type;
typedef struct icb_rpmh_request_queue_node
{
  icb_rpmh_request_type *next;
  icb_hw_node_type *     node;
  uint32_t               val;
  bool                   is_queued;  /**< Has this request been queued */
} icb_rpmh_request_type;

typedef struct
{
  uint64_t          ib;           /**< Instantaneous BW request in Bytes/Sec */
  uint64_t          ab;           /**< Arbitrated BW request in Bytes/Sec */
  npa_client_handle client;
  uint32_t          latency_ns; 
} icb_bw_request_type;

/* BW Request List */
typedef struct
{
  uint32_t              num_entries;
  uint32_t              list_size;
  icb_bw_request_type **requests;
} icb_bw_request_list_type;

typedef struct
{
  icb_request_kind_type type;           /**< Request from master or slave? */
  void *                node;           /**< Pointer to the requesting node. */
} icb_hw_request_type;

/* HW Request List */
typedef struct
{
  uint32_t              num_entries;
  uint32_t              list_size;
  icb_hw_request_type **requests;
} icb_hw_request_list_type;

/** Node State */
typedef struct
{
  uint64_t total_bw;        /**< Total BW as computed by compute_bw() */
  uint64_t ib;              /**< Required Ib value */
  uint64_t supp_ib;         /**< Suppressible Ib value */
  uint64_t ab;              /**< Required Ab value */
  uint64_t supp_ab;         /**< Suppressible Ab value */
  uint32_t latency_ns;      /**< Required latency in ns */
  uint32_t supp_latency_ns; /**< Suppressible latency in ns */
} icb_node_state_type;

/** Notification types */
typedef struct
{
  uint64_t ib;
  uint64_t ab;
  uint64_t total;
} icb_notify_state;

typedef struct icb_notify_type
{
  struct icb_notify_type *     next;
  struct icb_notify_list_type *list;
  icb_notify_definition_type   definition;
  icb_notify_cb_type           callback;
  void *                       callback_data;
  bool                         status_valid;
  icb_notify_status_type       status;
  icb_notify_state             prev_state;
} icb_notify_type;

typedef struct icb_notify_list_type
{
  icb_notify_type *head;
  uint32_t         count;
} icb_notify_list_type;

typedef enum
{
  ICB_BUS_NOC = 0,
  ICB_BUS_VIRT,
  ICB_BUS_TYPE_SIZE = 0x7FFFFFFF
} icb_bus_type;

/* Bus definition structure */
typedef struct
{
  const char *            base_name;
  uint8_t *               base_addr;
  uint32_t                region_size;
  icb_bus_type            type;
  HAL_noc_InfoType *      info;             /**< Bus info type, from HAL. */
  uint32_t                num_en_cfg_clks;  /**< Number of enable/disable config clocks  */
  const char * const *    en_cfg_clk_names; /**< Array enable/disable config clock names */
  ClockIdType *           en_cfg_clk_ids;   /**< Array enable/disable config clock ids */
  uint32_t                num_pwr_domains;  /**< Number of power domain */
  const char * const *    pwr_domain_names; /**< Array of power domain names */
  ClockPowerDomainIdType* pwr_domain_ids;   /**< Array of power domains ids */
} icb_bus_def_type;

/* QoS policy definitions */
typedef struct
{
  uint32_t                  default_priority;
  HAL_noc_LimiterType       limiter;
  bool                      limiter_enabled;
  HAL_noc_RegulatorType     regulator;
  HAL_noc_RegulatorModeType regulator_mode;
  bool                      urgency_forwarding_enabled;
  bool                      is_applied;
} icb_noc_master_policy_type;

typedef struct
{
  icb_noc_master_policy_type *noc;
} icb_qos_policy_type;

/* Hardware node definition */
typedef enum
{
  ICB_HW_NODE_KIND_BANDWIDTH = 0,
  ICB_HW_NODE_KIND_LATENCY,
  ICB_HW_NODE_KIND_VECTOR,
  ICB_HW_NODE_KIND_RAW,
  ICB_HW_NODE_KIND_COUNT,
  ICB_HW_NODE_KIND_SIZE = 0x7FFFFFF
} icb_hw_node_kind_type;

typedef struct icb_hw_node_type
{
  const char *             id;            /**< SW identifier */
  icb_hw_node_kind_type    type;          /**< HW node type */
  uint32_t                 hw_id;         /**< HW identifier */
  uint32_t                 width;         /**< Effective width */
  uint32_t                 bw_unit;       /**< Minimum bandwidth unit */
  uint32_t                 clk_id;        /**< Associated clock domain id */
  icb_node_state_type      state;         /**< Node state */
  uint32_t                 output;        /**< Output state */
  icb_request_type         request;       /**< HW request structure */
  icb_rpmh_request_type    active;        /**< Active/Wake set commit request stucture */
  icb_rpmh_request_type    sleep;         /**< Sleep set commit request stucture */
  icb_hw_request_list_type request_list;  /**< List of current requests for this node */
  icb_hw_request_list_type query_list;    /**< List of query requests for this node */
  icb_node_state_type      query_state;   /**< Node state for query requests */
} icb_hw_node_type;

typedef struct
{
  const char *             name;                /**< String identifier */
  ICBId_MasterType         id;                  /**< Enum identifier */
  icb_bus_def_type *       bus;                 /**< Definition of associated bus */
  uint32_t                 width;               /**< Effective width in bytes */
  uint32_t                 num_qos_ports;       /**< Number of QoS ports */
  uint32_t *               qos_port_ids;        /**< QoS port numbers */
  uint32_t                 num_agg_ports;       /**< Number of ports for aggregation purposes */
  bool                     is_locally_owned;    /**< Is this port owned by the local processor? */
  icb_qos_policy_type *    qos_policy;          /**< Default QoS Policy */
  icb_qos_policy_type *    qos_policy_override; /**< QoS policy override */
  icb_node_state_type      state;               /**< Node state */
  icb_request_type         bw_request;          /**< Bandwidth queue request. */
  icb_request_type         latency_request;     /**< Latency queue request. */
  icb_hw_request_type      hw_request;          /**< Hardware request structure. */
  icb_bw_request_list_type request_list;        /**< List of current requests for this master */
  uint32_t                 num_bw_nodes;        /**< Number of hw bandwidth nodes associated with this master */
  icb_hw_node_type **      bw_nodes;            /**< HW bandwidth nodes associated with this master */
  uint32_t                 num_latency_nodes;   /**< Number of hw latency nodes associated with this master */
  icb_hw_node_type **      latency_nodes;       /**< HW latency nodes associated with this master */
  icb_notify_list_type     notifications;       /**< List of current notification requests */
  icb_bw_request_list_type query_list;          /**< List of query requests for this master */
  icb_node_state_type      query_state;         /**< Node state for query requests */
} icb_master_def_type;

/** Slave Settings */
typedef struct
{
  const char *             name;          /**< String identifier */
  ICBId_SlaveType          id;            /**< External Slave ID */
  icb_bus_def_type *       bus;           /**< Definition of associated bus */
  uint32_t                 width;         /**< Width of slave bus in bytes */
  uint32_t                 num_agg_ports; /**< Number of ports for aggregation purposes */
  icb_node_state_type      state;         /**< Node State */
  icb_request_type         bw_request;    /**< Bandwidth queue request. */
  icb_hw_request_type      hw_request;    /**< Hardware request structure. */
  icb_bw_request_list_type request_list;  /**< List of current requests for this slave */
  uint32_t                 num_bw_nodes;  /**< Number of hw bandwidth nodes associated with this slave */
  icb_hw_node_type **      bw_nodes;      /**< HW bandwidth nodes associated with this slave */
  icb_notify_list_type     notifications; /**< List of current notification requests */
  icb_bw_request_list_type query_list;    /**< List of query requests for this slave */
  icb_node_state_type      query_state;   /**< Node state for query requests */
} icb_slave_def_type;    

/** Route Settings */
typedef struct
{
  icb_master_def_type *master;  /**< Pointer to master data */
  icb_slave_def_type  *slave;   /**< Pointer to slave data */
} icb_pair_type;

typedef struct
{
  uint32_t       num_pairs; /**< Number of Master/Slave Pairs in route */
  icb_pair_type *pairs;     /**< Array of internal master slave pairs */
} icb_route_list_type;

typedef struct
{
  ICBId_MasterType     master_id;  /**< External Master ID */
  ICBId_SlaveType      slave_id;   /**< External Slave ID */
  icb_route_list_type *route_list; /**< List of nodes in route */
} icb_route_type;

typedef struct
{
  RSCSW_DRV_MAPPING  rsc;
  rpmh_client_handle handle;
} icb_rpmh_handle_type;

typedef struct
{
  icb_rpmh_handle_type  rpmh_handle;
  ul_treeNodeType *     route_tree_root; /* Root of route tree */
  uint32_t              num_buses;
  icb_bus_def_type **   buses;
  uint32_t              num_masters;
  icb_master_def_type **masters;
  uint32_t              num_slaves;
  icb_slave_def_type ** slaves;
  uint32_t              num_hw_nodes;
  icb_hw_node_type **   hw_nodes;
} icb_topology_type;

/* Standalone definition structures */
typedef enum
{
  ICB_STANDALONE_INTERFACE_CLOCK = 0,
  ICB_STANDALONE_INTERFACE_FAL,
  ICB_STANDALONE_INTERFACE_COUNT,
  ICB_STANDALONE_INTERFACE_SIZE = 0x7FFFFFF
} icb_standalone_interface_type;

typedef struct
{
  npa_node_definition node_definition;
} icb_standalone_node_type;

typedef struct
{
  icb_standalone_interface_type type;
  icb_rpmh_handle_type          rpmh_handle;
  icb_hw_node_type *            hw_node;
} icb_standalone_data_type;

typedef struct
{
  const char *base_name;
  uint8_t *   base_addr;
  uint32_t    region_size;
  uint32_t    version;
} icb_hw_version_type;

/* Target definition structure */
typedef struct
{
  npa_node_definition *      node_definition;
  uint32_t                   num_topologies;
  icb_topology_type **       topologies;
  uint32_t                   num_standalone_nodes;
  icb_standalone_node_type **standalone_node_definitions;
  icb_hw_version_type        hw;
} icb_info_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ICB_SOC_DATA_H */
