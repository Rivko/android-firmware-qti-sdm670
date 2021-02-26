/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include "sensors_log.h"
#include <cutils/properties.h>
#include <string.h>
#include <string>

/* file format */
/*
<type>=<type_name>=<type_value>
file=soc_id=/sys/devices/soc0/soc_id
file=hw_platform_path=/sys/devices/soc0/hw_platform
property=persist.vendor.sensor.<propertyname>=/persist/sensors/registry/registry/<filename>
*/
/*persist.vendor.sensor.<propertyname>  - complete string needs to be added to .json file*/

static const char* registry_config_file = "/persist/sensors/registry/sns_reg_config";
static const char* registry_config_ctrl_file = "/persist/sensors/registry/sns_reg_ctrl";
static const char* registry_update_ctrl = "/persist/sensors/registry/registry/sns_reg_update";
static const char* registry_metadata_file = "/persist/sensors/registry/registry/sns_reg_config";

#define START_ENTRY "BEG:AP\n"
#define END_ENTRY   "END:AP\n"

/**
 * @brief class parses the sen_reg_config and creates the required output
            files for see on adsp to parse them and use for .json files parsing
 *
 */
static int parse_device_hw_info (char *str1) {

  char *token = NULL, *sub_token = NULL, *saveptr1 = NULL;
  char *saveptr2 = NULL, *str2 = NULL;
  int token_id = 0;
  size_t write_len = 0;

  sns_logi("##sen_reg_config file parsing start\n");
  for (token_id = 0 ;;str1 = NULL)
  {
    token = strtok_r(str1, "\n", &saveptr1);
    if(token == NULL) {
      break;
    }
    int subtoken_id = 0;
    char property_name[PROPERTY_VALUE_MAX];
    sns_logd("%s \n", token);
    for (str2 = token;;str2 = NULL)
    {
      sub_token = strtok_r(str2, "=", &saveptr2);
      if(sub_token == NULL) {
        sns_logd("parsing end for subtoken");
        break;
      }
      sns_logd("\t subtoken_id:%d entry#:%s \n",
            subtoken_id, sub_token);
      if(subtoken_id == 0) {
        if(strncmp(sub_token, "property", sizeof("property"))) {
            /*no need to parse other than property*/
            break;
        }
      }
      /*reached here means it is for property*/
      if(subtoken_id == 1) {
        strlcpy(property_name, sub_token, PROPERTY_VALUE_MAX);
      }
      if(subtoken_id == 2) {
        sns_logd( "fopen on %s for property_name %s",
                sub_token, property_name);
        FILE *t_fp = fopen(sub_token, "w+");
        if (t_fp) {
            char sns_prop[PROPERTY_VALUE_MAX];
            int len;
            len = property_get(property_name, sns_prop, "");
            if (PROPERTY_VALUE_MAX == len) {
                sns_loge("max length reached");
                break;
            }
            sns_prop[len]='\n';
            if(len) {
                write_len = fwrite(sns_prop,(len+1), 1, t_fp);
                if(0 != write_len)
                {
                    sns_logi("('%s'):%s to %s",
                        property_name, sns_prop, sub_token);
                    fclose(t_fp);
                } else {
                    sns_logi( "writing %s failed for %s", sns_prop, sub_token);
                    fclose(t_fp);
                }
            } else {
                sns_logi("property:'%s' not set\n", property_name);
            }
          } else {
          sns_loge("fopen(w) failed for %s", sub_token);
          }
      }
      subtoken_id++;
    }
    token_id++;
  }
  return token_id;
}

/**
**/
static void
load_device_hw_info() {
  int no_of_entries = 0;
  size_t read_len = 0;
  struct stat stat_buf;
  FILE *fp = NULL;
  FILE *fp1 = NULL;

  if (0 == stat(registry_update_ctrl, &stat_buf)) {
    sns_logi("%s present, deleting older registry metadata", registry_update_ctrl);
    if (0 == unlink(registry_metadata_file))
      sns_logi("deleted the older meta data file sns_reg_config");
    else
      sns_loge("failed to delete older meta data file sns_reg_config");

    if (0 ==unlink(registry_update_ctrl))
      sns_logi("deleted the ota upgrade ctrl file sns_reg_update");
    else
      sns_loge("failed to delete update ctrl file");
  }

  fp1 = fopen(registry_config_ctrl_file, "w");
  if (fp1 == NULL) {
     sns_loge("sns_reg_cttl open failed\n");
     return;
  }

  if(fwrite(START_ENTRY, strlen(START_ENTRY), 1, fp1))
  {
    if(0 == stat(registry_config_file, &stat_buf)) {
      fp = fopen(registry_config_file, "r");
      if (fp) {
        char *str1 = (char *)malloc(stat_buf.st_size);
        if (str1) {
          read_len = fread(str1, sizeof(uint8_t),
                                      stat_buf.st_size,
                                      fp);
          if(0 != read_len)
          {
            str1[read_len - 1] = '\0';
            sns_logd( "reading fine'%s", registry_config_file);
          }
          no_of_entries = parse_device_hw_info(str1);
          sns_logi("##sen_reg_config file parsing end, total entries %d\n",
             no_of_entries);
        } else {
          sns_loge("malloc(str1) failed");
        }
      } else {
        sns_loge("fopen failed for %s\n", registry_config_file);
      }
    } else {
      sns_loge("stat failed for %s", registry_config_file);
    }
  }
  else
  {
    sns_loge("fwrite failed for %s", registry_config_ctrl_file);
  }

  fseek(fp1, 0, SEEK_SET);
  if(!fwrite(END_ENTRY, strlen(END_ENTRY), 1, fp1))
  {
    sns_loge("fwrite:end failed %s", registry_config_ctrl_file);
  }

  if (fp1) fclose(fp1);
  if (fp) fclose(fp);
}

void handle_registry_config() {
    load_device_hw_info();
}
