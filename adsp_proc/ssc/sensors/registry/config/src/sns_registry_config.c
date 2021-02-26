/*=============================================================================
  @file sns_registry_config.c

  Configuration options for the Sensors Registry parser.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_registry_parser.h"
#include "sns_types.h"

/*=============================================================================
  Config Variable Definitions
  ===========================================================================*/

static char const hw_platform_name[] = "hw_platform";
static char const platform_subtype_name[] = "platform_subtype";
static char const platform_subtype_id_name[] = "platform_subtype_id";
static char const platform_version_name[] = "platform_version";
static char const soc_id_name[] = "soc_id";
static char const revision_name[] = "revision";

#if defined(SSC_TARGET_HEXAGON)
char const persist_mnt_name[] = "/mnt/vendor";
char const data_dir_name[] = "/data";
char registry_dir[SNS_REGISTRY_DIR_LEN_MAX] = "/persist/sensors/registry/registry";
char config_dir[SNS_REGISTRY_DIR_LEN_MAX] = "/persist/sensors/registry/config";
char const config_dir_new[] = "/vendor/etc/sensors/config";
char const config_dir_le[] = "/system/etc/sensors/config";

static char const hw_platform_path[] = "/sys/devices/soc0/hw_platform";
static char const platform_subtype_path[] = "/sys/devices/soc0/platform_subtype";
static char const platform_subtype_id_path[] = "/sys/devices/soc0/platform_subtype_id";
static char const platform_version_path[] = "/sys/devices/soc0/platform_version";
static char const soc_id_path[] = "/sys/devices/soc0/soc_id";
static char const revision_path[] = "/sys/devices/soc0/revision";
#elif defined(SSC_TARGET_X86)
char const persist_mnt_name[] = "/xxxxxxxxxxxxx";
char const registry_dir[SNS_REGISTRY_DIR_LEN_MAX] = "registry/registry";
char config_dir[SNS_REGISTRY_DIR_LEN_MAX] = "registry/config";
char const config_dir_new[] = "registry/config";
char const config_dir_le[] = "sensors/config";

static char const hw_platform_path[] = "./registry/hw_platform";
static char const platform_subtype_path[] = "./registry/platform_subtype";
static char const platform_subtype_id_path[] = "./registry/platform_subtype_id";
static char const platform_version_path[] = "./registry/platform_version";
static char const soc_id_path[] = "./registry/soc_id";
static char const revision_path[] = "./registry/revision";
#endif

/**
 * Device properties
 *
 * Additional options can be added using the same pattern.
 */
sns_registry_hw sns_registry_hw_info[] =
{
  {hw_platform_name, hw_platform_path, NULL},
  {platform_subtype_name, platform_subtype_path, NULL},
  {platform_subtype_id_name, platform_subtype_id_path, NULL},
  {platform_version_name, platform_version_path, NULL},
  {soc_id_name, soc_id_path, NULL},
  {revision_name, revision_path, NULL},
};
const uint32_t sns_registry_hw_info_len = ARR_SIZE(sns_registry_hw_info);
