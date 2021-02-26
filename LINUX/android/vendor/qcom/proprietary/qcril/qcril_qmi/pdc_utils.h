
/******************************************************************************
  @file    pdc_utils.h
  @brief   interfaces for communication with QMI PDC

  DESCRIPTION
    Provide interface for MBN operations

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PDC_UTILS_H
#define __PDC_UTILS_H

#include "persistent_device_configuration_v01.h"

/* error code */
#define PDC_SUCCESS                 0
#define PDC_ERROR                   -1
#define PDC_BUSY                    -2
#define PDC_FRAME_RESET             -3
#define PDC_INVALID_ID              -4
#define PDC_NO_MEMORY               -5
#define PDC_NOT_AVAIL               -6

struct pdc_config_pdc_info {
    uint32_t config_id_len;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
};
typedef struct pdc_config_list_info {
    struct pdc_config_pdc_info config_item;
    struct pdc_config_list_info *next;
}pdc_config_list_info;

struct pdc_config_item_info {
    uint8_t config_desc_valid;
    uint32_t config_desc_len;
    uint8_t config_desc[PDC_CONFIG_DESC_SIZE_MAX_V01];
    uint8_t config_version_valid;
    uint32_t config_version;
    uint8_t base_version_valid;
    uint32_t base_version;
};

int pdc_init(void);
int pdc_get_config_lists(pdc_config_list_info **config_list, unsigned int *num_of_configs);
int pdc_load_config(char* file_name, struct pdc_config_pdc_info* config_id, pdc_config_type_enum_v01 config_type);
int pdc_delete_config(struct pdc_config_pdc_info* config_id, pdc_config_type_enum_v01 config_type);
int pdc_deactivate_config(uint32_t sub_id, pdc_config_type_enum_v01 config_type);
int pdc_get_config_info(struct pdc_config_pdc_info* config_id, struct pdc_config_item_info *config_info, pdc_config_type_enum_v01 config_type);
int pdc_get_current_config(struct pdc_config_pdc_info *conf_id, pdc_config_type_enum_v01 config_type, uint32_t sub_id);
int pdc_select_config(struct pdc_config_pdc_info *conf_id, pdc_config_type_enum_v01 config_type, uint32_t sub_id);
int pdc_activate_config(pdc_config_type_enum_v01 config_type, uint32_t sub_id);
int pdc_enable_auto_selection(int enable, int slot);
void pdc_clean();

#endif
