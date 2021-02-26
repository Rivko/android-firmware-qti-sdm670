/*============================================================================
@file npa_config.h

Target Configuration Data Structures 

Copyright (c) 2010-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

============================================================================*/
#ifndef NPA_CONFIG_H
#define NPA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Custom latency values for resource, -1 means framework default */
typedef struct npa_resource_latency_data
{
  int req_latency;     /* time to synchronously process a request
                        * Does not include notification time
                        */
  int fork_latency;    /* time to fork a request */ 
  int notify_latency;  /* time to notify completion of a scheduled request */
} npa_resource_latency_data;

/* Structure to specify custom overrides per resource */
/* Currently used to override default latency values */
typedef struct npa_resource_config
{
  const char                *name;      /* resource name */
  npa_resource_latency_data  latency;   /* latency data */
} npa_resource_config;

/* NPA target configuration data
 *
 * The config data structures defines the target-specific configuration data 
 * (pool sizes, workloop priorities, etc).
 * */

/* Helper structures for config data */
typedef struct npa_pool_config
{
  unsigned int initial;         /* Initial allocation */
  unsigned int subsequent;      /* Subsequent allocations */
} npa_pool_config;

typedef struct npa_workloop_config
{
  unsigned int priority;        /* Priorities */
  unsigned int num_events;      /* Number of events */
} npa_workloop_config;

/* Data structure for NPA configuration data
 * WARNING - If you change this structure, you must also update
 * npa_config_data_property_struct[] in npa.c
 */
typedef struct npa_config
{
  unsigned int  log_buffer_size; /* Size in bytes */

  /* Allocations */
  npa_pool_config resource_allocation;
  npa_pool_config client_allocation;
  npa_pool_config event_allocation;
  npa_pool_config link_allocation;
  npa_pool_config event_queue_allocation;
  npa_pool_config list_allocation;

  /* Thread config data */
  npa_workloop_config async_request_workloop;
  npa_workloop_config async_event_workloop;

  /* last entry in the structure.
   * Used to verify that the data given and the structure used 
   * match. 
   * Should be set to the address of self.
   * If it does not equal "self", then the data and structure may 
   * be out of sync and should not be used. 
   */
  const struct npa_config *table_end;

} npa_config;

#ifdef __cplusplus
}
#endif

#endif /* NPA_CONFIG_H */
