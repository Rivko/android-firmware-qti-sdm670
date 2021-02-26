/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    qcril_qmi_ota_daemon.c
  @brief   a seperate thread for upgrading MBN in EFS

  DESCRIPTION
    Provide a seperate thread for updgrading MBN in modem EFS, and trigger
    modem SSR if needed

******************************************************************************/

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>
#include "pdc_utils.h"
#include "mbn_utils.h"
#include "modules/nas/qcril_db.h"
#include "qcril_mbn_db.h"
#include "qcril_mbn_sw_update.h"
#include "qcril_mbn_ota_file_list.h"
#include "qcrili.h"
#ifdef QMI_RIL_UTF
#include "log/log.h"
#endif

#define MAX_SUBS                    3
#define OEM_VERSION_LENGTH          4

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MBN_OTA"

/* MBN info in EFS */
struct efs_modem_config_info {
    struct pdc_config_pdc_info  config_id;
    struct pdc_config_item_info config_info;
    uint32_t                    is_current:1;
    uint32_t                    sub:3;
    uint32_t                    is_matched:1;
};

static void dump_mbn_list_info(struct efs_modem_config_info* p_modem_list, int n_list)
{
    int idx, subs;
    uint32_t len;
    char *tmp_str;

    if (!p_modem_list || !n_list) {
        RLOGI("no MBN in modem EFS.");
        return;
    }

    len = (PDC_CONFIG_DESC_SIZE_MAX_V01 > PDC_CONFIG_ID_SIZE_MAX_V01) ?
        PDC_CONFIG_DESC_SIZE_MAX_V01 : PDC_CONFIG_ID_SIZE_MAX_V01;
    len += 1;
    if ((tmp_str = malloc(len)) == NULL) {
        RLOGE("Failed to alloc memory to hold config_id/config_desc for dump purpose");
        return;
    }

    RLOGI("current MBNs in modem EFS:");
    for (idx = 0; idx < n_list; idx++) {
        /* show config_id */
        len = (p_modem_list[idx].config_id.config_id_len < PDC_CONFIG_ID_SIZE_MAX_V01) ?
            p_modem_list[idx].config_id.config_id_len : PDC_CONFIG_ID_SIZE_MAX_V01;
        memcpy(tmp_str, p_modem_list[idx].config_id.config_id, len);
        tmp_str[len] = '\0';
        RLOGI("%2d: config_id: %s", idx, tmp_str);
        /* show config_desc */
        if (p_modem_list[idx].config_info.config_desc_valid) {
            len = (p_modem_list[idx].config_info.config_desc_len < PDC_CONFIG_DESC_SIZE_MAX_V01) ?
                p_modem_list[idx].config_info.config_desc_len : PDC_CONFIG_DESC_SIZE_MAX_V01;
            memcpy(tmp_str, p_modem_list[idx].config_info.config_desc, len);
            tmp_str[len] = '\0';
            RLOGI("    config_desc: %s", tmp_str);
        }
        if (p_modem_list[idx].config_info.config_version_valid) {
            RLOGI("    config_version: 0x%x", p_modem_list[idx].config_info.config_version);
        }
        if (p_modem_list[idx].is_current) {
            RLOGI("    This config is activated with SUBs:");
            for (subs = 0; subs < MAX_SUBS; subs++) {
                if (p_modem_list[idx].sub & (1 << subs))
                    RLOGI("    SUB%d", subs);
            }
        }
    }

    free(tmp_str);
}

static void dump_local_file_list(struct local_modem_config_info* p_list, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        RLOGI("file_name: %s, config_name: %s, version: 0x%08x, is_matched: %d",
            p_list[i].file_name, p_list[i].config_id, p_list[i].version, p_list[i].is_matched);
    }

}

static inline int is_config_activated(struct pdc_config_pdc_info *config_id,
                    struct pdc_config_pdc_info *config_ids_per_sub, uint32_t *p_sub_mask)
{
    uint32_t i, sub_idx, sub_mask = 0;

    if (!config_id || !config_ids_per_sub)
        return 0;
    for (sub_idx = 0; sub_idx < MAX_SUBS; sub_idx++) {
        if (config_id->config_id_len != config_ids_per_sub[sub_idx].config_id_len)
            continue;
        for (i = 0; i < config_id->config_id_len; i++) {
            if (config_id->config_id[i] != config_ids_per_sub[sub_idx].config_id[i])
                break;
        }
        if (i == config_id->config_id_len)
            sub_mask |= (1 << sub_idx);
    }

    *p_sub_mask = sub_mask;
    return (!!sub_mask);
}

/* This function will generate all MBN list and their infos, querying from modem side
 * Note: this function will allocate memory for modem_list, need to free it after use
 */

static int get_modem_list(struct efs_modem_config_info **p_modem_list)
{
    uint32_t i, sub_mask, index = 0;
    struct efs_modem_config_info *modem_list;
    pdc_config_list_info *config_list = NULL;
    struct pdc_config_item_info config_info;
    struct pdc_config_pdc_info config_ids_per_sub[MAX_SUBS];
    int result, is_activated;
    unsigned int num_of_configs = 0;

    /* set NULL as default */
    *p_modem_list = NULL;

    do {
        /* get config_id lists from modem side */
        if ((result = pdc_get_config_lists(&config_list, &num_of_configs)) != PDC_SUCCESS) {
            RLOGE("Failed to get config list from modem, error code: %d", result);
            index = result;
            break;
        }

        /* allocate memory to hold configuration info */
        modem_list = (struct efs_modem_config_info*)malloc(
                            sizeof(struct efs_modem_config_info) * num_of_configs);
        if (modem_list == NULL) {
            RLOGE("Failed to allocate memeory to hold modem list");
            index = PDC_ERROR;
            break;
        }
        memset(modem_list, 0, sizeof(struct efs_modem_config_info) * num_of_configs);

        /* get current config for each sub */
        for (i = 0; i < MAX_SUBS; i++) {
            result = pdc_get_current_config(config_ids_per_sub + i,  PDC_CONFIG_TYPE_MODEM_SW_V01, i);
            if (result == PDC_NOT_AVAIL) {
                /* no selected MBN on this SUB */
                config_ids_per_sub[i].config_id_len = 0;
            } else if (result != PDC_SUCCESS) {
                RLOGE("failed to get current config for SUB %d", i);
                config_ids_per_sub[i].config_id_len = 0;
            }
        }

        /* go over the list, fill the config_info in the table and mark current activated config */
        index = 0;
        pdc_config_list_info *config_index = config_list;
        while (config_index) {
            result = pdc_get_config_info(&(config_index->config_item), &config_info, PDC_CONFIG_TYPE_MODEM_SW_V01);
            if (result == PDC_SUCCESS) {
                /* check if it is activated */
                is_activated = (is_config_activated(&(config_index->config_item), config_ids_per_sub, &sub_mask));
                modem_list[index].config_id = config_index->config_item;
                modem_list[index].config_info = config_info;
                modem_list[index].is_current = is_activated;
                modem_list[index].sub = sub_mask;
                index++;
            } else {
                RLOGE("Failed to get config info");
                free(modem_list);
                modem_list = NULL;
                index = PDC_ERROR;
                break;
            }
            config_index = config_index->next;
        }

        *p_modem_list = modem_list;
    } while(0);

    //free the modem config list
    while (config_list) {
        pdc_config_list_info *config = config_list->next;
        free(config_list);
        config_list = config;
    }

    return index;
}

/* Generate an unique config_id
 * config_id: using OTA_"filepath"_date as a temporary solution
 * for long term: Using an algorithm to generate (like QPST??)
 */
static int generate_config_id(char* mbn_file, char *config_name, struct pdc_config_pdc_info* config_id)
{
    size_t ret;
    struct timeval tv;
#define MAX_TIME_STRING_LENGTH     64
    char time_str[MAX_TIME_STRING_LENGTH];

    QCRIL_NOTUSED(mbn_file);

    strlcpy((char*)config_id->config_id, "OTA_", PDC_CONFIG_ID_SIZE_MAX_V01);
    ret = strlcat((char*)config_id->config_id, config_name, PDC_CONFIG_ID_SIZE_MAX_V01);
    if (ret >= PDC_CONFIG_ID_SIZE_MAX_V01) {
        RLOGW("boundary exceeds when generate config_id");
        config_id->config_id_len = strlen((char*)config_id->config_id);
        return PDC_SUCCESS;
    }
    gettimeofday(&tv, NULL);
    snprintf(time_str, MAX_TIME_STRING_LENGTH, "%ld:%ld", tv.tv_sec, tv.tv_usec);
    ret = strlcat((char*)config_id->config_id, time_str, PDC_CONFIG_ID_SIZE_MAX_V01);
    if (ret >= PDC_CONFIG_ID_SIZE_MAX_V01)
        RLOGW("boundary exceeds when generate config_id");

    config_id->config_id_len = strlen((char*)config_id->config_id);
    return PDC_SUCCESS;
}

static int load_new_mbn(char *mbn_file, char *config_name)
{
    int ret_val = PDC_ERROR;
    struct pdc_config_pdc_info new_config_id;

    if (generate_config_id(mbn_file, config_name, &new_config_id) == PDC_SUCCESS) {
        ret_val = pdc_load_config(mbn_file, &new_config_id, PDC_CONFIG_TYPE_MODEM_SW_V01);
    }

    return ret_val;
}

void* qmi_ril_mbn_ota_thread_proc(void* empty_param)
{
    struct efs_modem_config_info *p_modem_list = NULL;
    struct local_modem_config_info *p_local_list = NULL;
    int n_modem_list, n_local_file;
    int is_all_in_local_matched;
    int idx, i, j;
    int ret_val;
    int res = PDC_SUCCESS;

    /* avoid the "unused" warning */
    QCRIL_NOTUSED(empty_param);

    do
    {
        /* initialize QMI PDC */
        if (pdc_init() != PDC_SUCCESS)
        {
            res = PDC_ERROR;
            break;
        }

        /* load MBN into database if needed */
        if (qcril_mbn_sw_load_to_db() != E_SUCCESS)
        {
            res = PDC_ERROR;
            break;
        }

        /* query modem configuration info */
        n_modem_list = get_modem_list(&p_modem_list);
        if (n_modem_list == PDC_NOT_AVAIL) {
            n_modem_list = 0;
        } else if (n_modem_list < 0) {
            res = PDC_ERROR;
            break;
        }

        /* query local file list info */
        n_local_file = qcril_get_mbn_file_list(&p_local_list);
        if (n_local_file <= 0) {
            n_local_file = 0;
            RLOGI("Failed to get local file list");
        }
        /*
         * find the matched MBN between local list and that in EFS
         * for these matched MBNs, keep it untouched
        */
        is_all_in_local_matched = 1;
        for (i = 0; i < n_local_file; i++) {
            for (j = 0; j < n_modem_list; j++) {
                if (p_local_list[i].version == p_modem_list[j].config_info.config_version) {
                    p_local_list[i].is_matched = 1;
                    p_modem_list[j].is_matched = 1;
                    break;
                }
            }
            if (j >= n_modem_list)
                is_all_in_local_matched = 0;
        }
        RLOGI("is_all_in_local_matched: %d", is_all_in_local_matched);

        /* for debug purpose */
        dump_mbn_list_info(p_modem_list, n_modem_list);
        dump_local_file_list(p_local_list, n_local_file);

        /* disable MBN auto selection */
        if (!is_all_in_local_matched) {
            ret_val = pdc_enable_auto_selection(0, 0);
            ret_val = pdc_enable_auto_selection(0, 1);
        }

        /* Delete the MBNs in EFS that are not matched */
        for (idx = 0; idx < n_modem_list; idx++) {
            if (!p_modem_list[idx].is_matched) {
                ret_val = pdc_delete_config(&p_modem_list[idx].config_id, PDC_CONFIG_TYPE_MODEM_SW_V01);
                if (ret_val != PDC_SUCCESS) {
                    RLOGE("Failed to delete the MBN with config_id: %s", p_modem_list[idx].config_id.config_id);
                }
            }
        }

        /* Upload local MBNs to EFS that are not matched */
        for (idx = 0; idx < n_local_file; idx++) {
            if (!p_local_list[idx].is_matched) {
                ret_val = load_new_mbn(p_local_list[idx].file_name, p_local_list[idx].config_id);
                if (ret_val != PDC_SUCCESS) {
                    RLOGE("Failed to load new MBN: %s, error code: %d", p_local_list[idx].file_name, ret_val);
                }
            }
        }

        /* Enable MBN auto selection. Ignore the errors */
        if (!is_all_in_local_matched) {
            ret_val = pdc_enable_auto_selection(1, 0);
            ret_val = pdc_enable_auto_selection(1, 1);
        }
    } while(0);

    /*release connection to PDC service*/
    pdc_clean();

    if (p_modem_list)
        free(p_modem_list);
    if (p_local_list)
        free(p_local_list);

    if(res != PDC_SUCCESS)
        return (void*)PDC_ERROR;

    return (void *)PDC_SUCCESS;
}
