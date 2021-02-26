/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    qcril_mbn_ota_file_list.h
  @brief   Provides interface to read MBN file list for MBN OTA with modem
           centric solution

  DESCRIPTION

******************************************************************************/

#ifndef __OTA_FILE_LIST_H
#define __OTA_FILE_LIST_H

#define MAX_MBN_NAME_LENGTH     256

/* local MBN info */
struct local_modem_config_info {
    char file_name[MAX_MBN_NAME_LENGTH];
    char config_id[MAX_MBN_NAME_LENGTH];
    uint32_t version;
    uint32_t is_matched:1;
};

int qcril_get_mbn_file_list(struct local_modem_config_info** p_cfg_list);

#endif
