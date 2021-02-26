/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************

  @file    qcril_mbn_ota_file_list.c
  @brief   Provides interface to read MBN file list for MBN OTA with modem
           centric solution

  DESCRIPTION

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <utils/Log.h>
#include "mbn_utils.h"
#include "qcril_mbn_ota_file_list.h"
#ifdef QMI_RIL_UTF
#include "log/log.h"
#endif

#define DATA_PARTITION_ROOT "/data/vendor/"
#define LOCAL_MBN_BASE_DIR DATA_PARTITION_ROOT "radio/modem_config/"
#define LOCAL_MBN_LIST_FILE     "mbn_ota.txt"
#define MAX_MBN_FILE_COUNT      200

static inline void strip_append_newlines(char* file_name)
{
    int idx;

    idx = strlen(file_name) - 1;
    for (; idx >= 0; idx--) {
        if ((file_name[idx] == '\n') || (file_name[idx] == '\r'))
            file_name[idx] = '\0';
        else
            break;
    }
}

static inline int is_mbn_file_name(char* file_name)
{
#define MBN_FILE_SUFFIX_STR     ".mbn"
#define MBN_FILE_SUFFIX_LEN     4
    size_t length;

    length = strlen(file_name);
    if (length <= MBN_FILE_SUFFIX_LEN)
        return 0;

    return (!strncasecmp(file_name + length - MBN_FILE_SUFFIX_LEN,
                MBN_FILE_SUFFIX_STR, MBN_FILE_SUFFIX_LEN));
}

static uint32_t qcril_mbn_get_version(char* file_name)
{
    char* oem_version;
    int i;
    uint32_t len, version = 0;

    if (mcfg_get_oem_version(file_name, &oem_version, &len) != MBN_PARSE_SUCCESS)
        return 0;
    if (len != 4) {
        RLOGE("oem version is not 4 bytes");
        free(oem_version);
        return 0;
    }
    for (i = 3; i >= 0; i--)
        version = (version << 8) + oem_version[i];

    return version;
}

static int calculate_file_lines(char* file_name)
{
    FILE* fp = NULL;
    char buf[MAX_MBN_NAME_LENGTH];
    int count = 0;

    if ((fp = fopen(file_name, "r")) == NULL) {
        RLOGE("Failed to open file with read only permission: %s", file_name);
        return 0;
    }

    while ((fgets(buf, sizeof(buf), fp) != NULL) && (++count < MAX_MBN_FILE_COUNT));

    if (count >= MAX_MBN_NAME_LENGTH) {
        RLOGE("MBN file count reach the maximum: %d", MAX_MBN_FILE_COUNT);
    }
    fclose(fp);
    return count;
}

int qcril_get_mbn_file_list(struct local_modem_config_info** p_cfg_list)
{
    struct local_modem_config_info* cfg_list = NULL;
    char str_list_name[MAX_MBN_NAME_LENGTH];
    char buf[MAX_MBN_NAME_LENGTH];
    FILE* fp = NULL;
    int n_lines = 0, index = 0;

    do {
        if (strlcpy(str_list_name, LOCAL_MBN_BASE_DIR, MAX_MBN_NAME_LENGTH)
                    >= MAX_MBN_NAME_LENGTH) {
            RLOGE("MBN file PATH is too long");
            break;
        }
        if (strlcat(str_list_name, LOCAL_MBN_LIST_FILE, MAX_MBN_NAME_LENGTH)
                    >= MAX_MBN_NAME_LENGTH) {
            RLOGE("MBN full file name is too long");
            break;
        }

        n_lines = calculate_file_lines(str_list_name);
        RLOGI("%d lines for file %s", n_lines, str_list_name);
        if (n_lines == 0)
            break;

        cfg_list = malloc(sizeof(struct local_modem_config_info) * n_lines);
        if (cfg_list == NULL) {
            RLOGE("Failed to allocate memory for local MBN list: %s", strerror(errno));
            break;
        }
        memset(cfg_list, 0, sizeof(struct local_modem_config_info) * n_lines);

        if ((fp = fopen(str_list_name, "r")) == NULL) {
            RLOGE("Failed to open file with read only permission: %s", str_list_name);
            free(cfg_list);
            cfg_list = NULL;
            break;
        }
        while ((fgets(buf, sizeof(buf), fp) != NULL) && (index < n_lines)) {
            if (strlcpy(cfg_list[index].file_name, LOCAL_MBN_BASE_DIR, MAX_MBN_NAME_LENGTH)
                            >= MAX_MBN_NAME_LENGTH) {
                RLOGE("MBN file PATH is too long");
                break;
            }
            if (strlcat(cfg_list[index].file_name, buf, MAX_MBN_NAME_LENGTH)
                            >= MAX_MBN_NAME_LENGTH) {
                RLOGE("MBN full file name is too long: %s", cfg_list[index].file_name);
                break;
            }

            //remove common path in config_id
            strlcpy(cfg_list[index].config_id, buf, MAX_MBN_NAME_LENGTH);
            strip_append_newlines(cfg_list[index].config_id);

            strip_append_newlines(cfg_list[index].file_name);
            if (is_mbn_file_name(cfg_list[index].file_name))
            {
                cfg_list[index].version = qcril_mbn_get_version(cfg_list[index].file_name);
                index++;
            }
        }
        if (index < n_lines)
            RLOGE("Warning: Not get full file list");
    } while (0);

    if (fp)
        fclose(fp);

    if (index > 0) {
        *p_cfg_list = cfg_list;
    } else {
        if (cfg_list)
            free(cfg_list);
    }

    return index;
}
