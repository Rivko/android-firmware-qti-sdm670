/*
 * gcs_acdb.h
 *
 * This header file defines ACDB related functionality for GCS.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GCS_ACDB_H__
#define __GCS_ACDB_H__

#include <osal_types.h>

typedef bool (*acdb_is_initialized_t) (void);

/* Get size of sub-graph list for a graph */
typedef int32_t (*acdb_loader_get_subgraph_list_size_t)(uint32_t, uint32_t,
                                                        uint32_t *);
/* Get sub-graph list for a graph */
typedef int32_t (*acdb_loader_get_subgraph_list_t)(uint32_t, uint32_t,
                                                   uint8_t *, size_t);
/* Get size of module list in a sub-graph of a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_info_list_size_t)(uint32_t, uint32_t,
                                                                 uint32_t,
                                                                 uint32_t *);
/* Get module list in a sub-graph of a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_info_list_t)(uint32_t, uint32_t,
                                                            uint32_t, uint8_t *,
                                                            size_t);
/* Get size of module connection data of a sub-graph in a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_conn_size_t)(uint32_t, uint32_t,
                                                            uint32_t, uint32_t *);
/* Get module connection data of a sub-graph in a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_conn_t)(uint32_t, uint32_t,
                                                       uint32_t, uint8_t *, size_t);
/* Get size of sub-graph connection data for a graph */
typedef int32_t (*acdb_loader_get_subgraph_conn_size_t)(uint32_t, uint32_t,
                                                        uint32_t, uint32_t,
                                                        uint32_t *);
/* Get sub-graph connection data for a graph */
typedef int32_t (*acdb_loader_get_subgraph_conn_t)(uint32_t, uint32_t, uint32_t,
                                                   uint32_t, uint8_t *, size_t);
/* Get size of calibration data for modules in a sub-graph of a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_cal_size_t)(uint32_t, uint32_t,
                                                           uint32_t, uint32_t *);
/* Get calibration data for modules in a sub-graph of a graph */
typedef int32_t (*acdb_loader_get_subgraph_mod_cal_t)(uint32_t, uint32_t,
                                                      uint32_t, uint8_t *, size_t);
/* Get size of list of modules which are loaded at device boot time */
typedef int32_t (*acdb_loader_get_boot_time_mod_list_size_t)(uint32_t *);
/* Get list of modules which are loaded at device boot time */
typedef int32_t (*acdb_loader_get_boot_time_mod_list_t)(uint8_t *, size_t);
/*
 * Get size of loading meta info, for modules that are loaded at boot time
 * or dynamically at usecase boundary
 */
typedef int32_t (*acdb_loader_get_mod_loading_meta_info_size_t)(uint32_t, uint32_t,
                                                                uint8_t *,
                                                                uint32_t *);
/*
 * Get loading meta info, for modules that are loaded at boot time
 * or dynamically at usecase boundary
 */
typedef int32_t (*acdb_loader_get_mod_loading_meta_info_t)(uint32_t, uint32_t,
                                                           uint8_t *, uint8_t *,
                                                           size_t);
struct gcs_acdb {
    void *acdb_handle;
    acdb_is_initialized_t acdb_is_initialized;
    acdb_loader_get_subgraph_list_size_t acdb_loader_get_subgraph_list_size;
    acdb_loader_get_subgraph_list_t acdb_loader_get_subgraph_list;
    acdb_loader_get_subgraph_mod_info_list_size_t acdb_loader_get_subgraph_mod_info_list_size;
    acdb_loader_get_subgraph_mod_info_list_t acdb_loader_get_subgraph_mod_info_list;
    acdb_loader_get_subgraph_mod_conn_size_t acdb_loader_get_subgraph_mod_conn_size;
    acdb_loader_get_subgraph_mod_conn_t acdb_loader_get_subgraph_mod_conn;
    acdb_loader_get_subgraph_conn_size_t acdb_loader_get_subgraph_conn_size;
    acdb_loader_get_subgraph_conn_t acdb_loader_get_subgraph_conn;
    acdb_loader_get_subgraph_mod_cal_size_t acdb_loader_get_subgraph_mod_cal_size;
    acdb_loader_get_subgraph_mod_cal_t acdb_loader_get_subgraph_mod_cal;
    acdb_loader_get_boot_time_mod_list_size_t acdb_loader_get_boot_time_mod_list_size;
    acdb_loader_get_boot_time_mod_list_t acdb_loader_get_boot_time_mod_list;
    acdb_loader_get_mod_loading_meta_info_size_t acdb_loader_get_mod_loading_meta_info_size;
    acdb_loader_get_mod_loading_meta_info_t acdb_loader_get_mod_loading_meta_info;
};

#endif /* __GCS_ACDB_H__ */
