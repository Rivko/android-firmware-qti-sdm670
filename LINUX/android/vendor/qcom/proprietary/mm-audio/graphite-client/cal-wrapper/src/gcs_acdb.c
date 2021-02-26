/*
 * gcs_acdb.c
 *
 * This file contains functions used by GCS to access graph information from
 * ACDB data files. These graph related informations are used for setup, start,
 * stop and tear-down a use case for Codec DSP using Graphite (Graph-Lite)
 * architecture.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <dlfcn.h>
#include <gcs_acdb.h>
#include <osal_errorcodes.h>
#include <osal_log.h>
#include <osal_mem.h>

#define LIB_ACDB_LOADER "libacdbloader.so"

/* Struct to hold ACDB infomation for GCS use */
static struct gcs_acdb gcs_acdb;

int32_t gcs_cal_wrapper_init(void)
{
    bool is_acdb_init = false;
    int32_t ret = 0;

    osal_mem_set(&gcs_acdb, 0, sizeof(struct gcs_acdb));
    /*
     * Dynamically load ACDB library to get handle
     */
    gcs_acdb.acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (gcs_acdb.acdb_handle == NULL) {
        OSAL_LOGE("%s: DLOPEN failed for %s", __func__, LIB_ACDB_LOADER);
        ret = -ENODEV;
        goto exit;
    }
    gcs_acdb.acdb_is_initialized = (acdb_is_initialized_t)
                    dlsym(gcs_acdb.acdb_handle,
                    "acdb_loader_is_initialized");
    if (!gcs_acdb.acdb_is_initialized) {
        OSAL_LOGE("%s: No symbol acdb_loader_is_initialized in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    /*
     * It is expected that, before calling this init functions, the caller
     * would have already initialized ACDB loader and hence, GCS Calibration
     * wrapper does not have to re-initialize it.
     * In case ACDB loader is not initialized by the time this init is called,
     * GCS will fail to initialize and would return error.
     */
    if (gcs_acdb.acdb_is_initialized != NULL)
        is_acdb_init = gcs_acdb.acdb_is_initialized();

    if (is_acdb_init != true) {
        OSAL_LOGE("%s: ACDB Loader in %s not initialized. Exisiting.\n",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_list_size =
        (acdb_loader_get_subgraph_list_size_t) dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_list_size");
    if (!gcs_acdb.acdb_loader_get_subgraph_list_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_list_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_list =
        (acdb_loader_get_subgraph_list_t) dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_list");
    if (!gcs_acdb.acdb_loader_get_subgraph_list) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_list in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_info_list_size =
        (acdb_loader_get_subgraph_mod_info_list_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_info_list_size");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_info_list_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_info_list_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_info_list =
        (acdb_loader_get_subgraph_mod_info_list_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_info_list");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_info_list) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_info_list in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_conn_size =
        (acdb_loader_get_subgraph_mod_conn_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_conn_size");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_conn_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_conn_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_conn =
        (acdb_loader_get_subgraph_mod_conn_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_conn");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_conn) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_conn in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_conn_size =
        (acdb_loader_get_subgraph_conn_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_conn_size");
    if (!gcs_acdb.acdb_loader_get_subgraph_conn_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_conn_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_conn =
        (acdb_loader_get_subgraph_conn_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_conn");
    if (!gcs_acdb.acdb_loader_get_subgraph_conn) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_conn in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_cal_size =
        (acdb_loader_get_subgraph_mod_cal_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_cal_size");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_cal_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_cal_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_subgraph_mod_cal =
        (acdb_loader_get_subgraph_mod_cal_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_subgraph_mod_cal");
    if (!gcs_acdb.acdb_loader_get_subgraph_mod_cal) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_subgraph_mod_cal in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_boot_time_mod_list_size =
        (acdb_loader_get_boot_time_mod_list_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_boot_time_mod_list_size");
    if (!gcs_acdb.acdb_loader_get_boot_time_mod_list_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_boot_time_mod_list_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_boot_time_mod_list =
        (acdb_loader_get_boot_time_mod_list_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_boot_time_mod_list");
    if (!gcs_acdb.acdb_loader_get_boot_time_mod_list) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_boot_time_mod_list in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_mod_loading_meta_info_size =
        (acdb_loader_get_mod_loading_meta_info_size_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_mod_loading_meta_info_size");
    if (!gcs_acdb.acdb_loader_get_mod_loading_meta_info_size) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_mod_loading_meta_info_size in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    gcs_acdb.acdb_loader_get_mod_loading_meta_info =
        (acdb_loader_get_mod_loading_meta_info_t)
         dlsym(gcs_acdb.acdb_handle,
            "acdb_loader_get_mod_loading_meta_info");
    if (!gcs_acdb.acdb_loader_get_mod_loading_meta_info) {
        OSAL_LOGE("%s: No symbol acdb_loader_get_mod_loading_meta_info in %s",
                __func__, LIB_ACDB_LOADER);
        ret = -ENOSYS;
        goto fail;
    }

    goto exit;

fail:
    dlclose(gcs_acdb.acdb_handle);
    osal_mem_set(&gcs_acdb, 0, sizeof(struct gcs_acdb));
exit:
    return ret;
}

int32_t gcs_cal_wrapper_deinit(void)
{
    dlclose(gcs_acdb.acdb_handle);
    osal_mem_set(&gcs_acdb, 0, sizeof(struct gcs_acdb));
    return 0;
}

int32_t gcs_cal_get_subgraph_list_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_list_size(u_id, d_id, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG list size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_list(uint32_t u_id, uint32_t d_id,
                                  uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_list(u_id, d_id,
                                                  cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG list\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_info_list_size(uint32_t u_id, uint32_t d_id,
                                                uint32_t sg_id,
                                                uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_info_list_size(u_id, d_id,
                                                               sg_id,
                                                               cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG mod list size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_info_list(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_info_list(u_id, d_id, sg_id,
                                                          cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG mod list\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_conn_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_conn_size(u_id, d_id, sg_id,
                                                          cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG mod conn size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_conn(uint32_t u_id, uint32_t d_id,
                                      uint32_t sg_id,
                                      uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_conn(u_id, d_id, sg_id,
                                                     cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG mod conn\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_conn_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t src_sg_id,
                                       uint32_t dst_sg_id,
                                       uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_conn_size(u_id, d_id,
                                                      src_sg_id, dst_sg_id,
                                                      cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG conn size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_conn(uint32_t u_id, uint32_t d_id,
                                  uint32_t src_sg_id, uint32_t dst_sg_id,
                                  uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_conn(u_id, d_id,
                                                 src_sg_id, dst_sg_id,
                                                 cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG conn\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_cal_size(uint32_t u_id, uint32_t d_id,
                                          uint32_t sg_id,
                                          uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_cal_size(u_id, d_id,
                                                         sg_id, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG cal size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_subgraph_mod_cal(uint32_t u_id, uint32_t d_id,
                                     uint32_t sg_id,
                                     uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_subgraph_mod_cal(u_id, d_id, sg_id,
                                                    cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get SG mod cal\n", __func__);

    return ret;
}

int32_t gcs_cal_get_boot_time_mod_list_size(uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_boot_time_mod_list_size(cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get boot_time mod list size\n", __func__);

    return ret;
}

int32_t gcs_cal_get_boot_time_mod_list(uint8_t *cal_buff, size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_boot_time_mod_list(cal_buff, cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get boot_time mod list\n", __func__);

    return ret;
}

int32_t gcs_cal_get_mod_loading_meta_info_size(uint32_t mod_type,
                                               uint32_t mod_count,
                                               uint8_t *mod_list,
                                               uint32_t *cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_mod_loading_meta_info_size(mod_type,
                                                              mod_count,
                                                              mod_list,
                                                              cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get dynamic load mod meta info size\n",
                    __func__);

    return ret;
}

int32_t gcs_cal_get_mod_loading_meta_info(uint32_t mod_type,
                                          uint32_t mod_count,
                                          uint8_t *mod_list,
                                          uint8_t *cal_buff,
                                          size_t cal_size)
{
    int ret;

    ret = gcs_acdb.acdb_loader_get_mod_loading_meta_info(mod_type,
                                                         mod_count,
                                                         mod_list,
                                                         cal_buff,
                                                         cal_size);
    if (ret)
        OSAL_LOGE("%s: failed to get dynamic load mod meta info\n",
                    __func__);

    return ret;
}
