#pragma once
/**
 * @file sns_registry_util.h
 *
 * Utilities used in conjunction with registry.
 *
 * @note: All utilities in this file are intended for physical sensor
 * dirvers and can be used in island mode.
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/inc/utils/sns_registry_util.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 */

#include <stdbool.h>
#include "sns_registry.pb.h"

#define SNS_REGISTRY_MAX_NAME_LEN 40
#define SNS_REGISTRY_MAX_PARSE_GROUP 3

/*=============================================================================
  Forward Declarations
  ===========================================================================*/
struct sns_registry_data_item;
struct pb_buffer_arg;

/**Function used to parse items belonging to a specific registry
 * group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer opaque buffer pointing to location
 *       where parsed items are to be stored
 */
typedef bool(*sns_registry_parse_group)(
  sns_registry_data_item *reg_item,
  struct pb_buffer_arg  *item_name,
  struct pb_buffer_arg  *item_str_val,
  void *parsed_buffer);

/**Registry Group Parsing information used in
 * sns_registry_decode_arg. Defines how to parse registry
 * items belonging to a given registry group */
typedef struct
{
  // name of the registry group
  char group_name[SNS_REGISTRY_MAX_NAME_LEN];
  // the function that must be called when items of the above
  // mentioned group are found
  sns_registry_parse_group parse_func;
  // the pointer to buffer that must be passed to the parse_func
  // where parsed output will be stored.
  void *parsed_buffer;
} sns_registry_group_parse_info;

/**PB Decode argument used with sns_registry_item_decode_cb*/
typedef struct
{
  // the length of the parse_info array
  uint8_t parse_info_len;
  // parsing information for each registry group present in the
  // registry read event
  sns_registry_group_parse_info parse_info[SNS_REGISTRY_MAX_PARSE_GROUP];
  // the registry group that currently decoded item belongs to
  struct pb_buffer_arg *item_group_name;
  // the registry item version number
  uint32_t version;
} sns_registry_decode_arg;

/**Triaxial sensor axis indexing convention */
typedef enum
{
  TRIAXIS_X = 0,
  TRIAXIS_Y = 1,
  TRIAXIS_Z = 2,
  TRIAXIS_NUM = 3
} triaxis;

/**Triaxial sensor input to output axis conversion */
typedef struct
{
  // input axis index
  triaxis ipaxis;
  // output axis index the above input axis maps to
  triaxis opaxis;
  // whether the input needs to be negated when
  // translating to the output axis
  bool invert;
} triaxis_conversion;

/**Registry items supported as part of physical sensor
 * configuration registry group
 */
typedef struct
{
  bool is_dri;
  int64_t hw_id;
  uint8_t res_idx;
  bool sync_stream;
}sns_registry_phy_sensor_cfg;

/**Registry items supported as part of physical sensor
 * platform configuration registry group
 */
typedef struct
{
  uint8_t bus_type;
  uint8_t bus_instance;
  uint32_t slave_config;
  uint32_t min_bus_speed_khz;
  uint32_t max_bus_speed_khz;
  uint8_t reg_addr_type;
  uint32_t dri_irq_num;
  uint8_t irq_pull_type;
  bool irq_is_chip_pin;
  uint8_t irq_drive_strength;
  uint8_t irq_trigger_type;
  uint8_t num_rail;
  uint8_t rail_on_state;
  char vddio_rail[SNS_REGISTRY_MAX_NAME_LEN];
  char vdd_rail[SNS_REGISTRY_MAX_NAME_LEN];
  uint8_t rigid_body_type;
}sns_registry_phy_sensor_pf_cfg;

/**Registry items supported as part of MD
 * configuration registry group
 */
typedef struct
{
  // threshold in m/s2
  float thresh;
  // window in sec
  float win;
  // disable
  bool disable;
} sns_registry_md_cfg;

/**Callback function used to decode sns_registry_read_event
 * Function will extract each item within the read event
 * including items within subgroups and will optionally run a
 * parsing function on the item passed.
 *
 * @param[i] stream, Refer pb_callback_s::decode
 * @param[i] field, Refer pb_callback_s::decode
 * @param[i] arg of type sns_registry_decode_arg
 */
bool
sns_registry_item_decode_cb(pb_istream_t *stream,
                            const pb_field_t *field,
                            void **arg);

/**Function used to parse items belonging to physical sensor
 * configuration registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_phy_sensor_cfg
 *       where parsed items are stored
 */
bool
sns_registry_parse_phy_sensor_cfg(sns_registry_data_item *reg_item,
                                  struct pb_buffer_arg  *item_name,
                                  struct pb_buffer_arg  *item_str_val,
                                  void *parsed_buffer);

/**Function used to parse items belonging to physical sensor
 * platform configuration registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type
 *       sns_registry_phy_sensor_pf_cfg where parsed items
 *       are stored
 */
bool
sns_registry_parse_phy_sensor_pf_cfg(sns_registry_data_item *reg_item,
                                     struct pb_buffer_arg  *item_name,
                                     struct pb_buffer_arg  *item_str_val,
                                     void *parsed_buffer);

/**Function used to parse items belonging to physical sensor
 * platform configuration registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type
 *       sns_registry_phy_sensor_pf_cfg where parsed items
 *       are stored
 */
bool
sns_registry_parse_axis_orientation(sns_registry_data_item *reg_item,
                                    struct pb_buffer_arg  *item_name,
                                    struct pb_buffer_arg  *item_str_val,
                                    void *parsed_buffer);

/**Function used to parse N dimensional float array.
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type vector3 where parsed items
 *       are stored
 */
bool
sns_registry_parse_float_arr(sns_registry_data_item *reg_item,
                             struct pb_buffer_arg  *item_name,
                             struct pb_buffer_arg  *item_str_val,
                             void *parsed_buffer);

/**Function used to parse items belonging to factory calibration
 * scale registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type matrix3 where parsed items
 *       are stored
 */
bool
sns_registry_parse_scale(sns_registry_data_item *reg_item,
                         struct pb_buffer_arg  *item_name,
                         struct pb_buffer_arg  *item_str_val,
                         void *parsed_buffer);

/**Function used to parse items belonging to factory calibration
 * 3 * 3 correction matrix registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type matrix3 where parsed items
 *       are stored
 */
bool
sns_registry_parse_corr_matrix_3(sns_registry_data_item *reg_item,
                                 struct pb_buffer_arg  *item_name,
                                 struct pb_buffer_arg  *item_str_val,
                                 void *parsed_buffer);

/**Function used to parse items belonging to Motion Detect
 * Config registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_md_cfg where
 *       parsed items are stored
 */
bool
sns_registry_parse_md_cfg(sns_registry_data_item *reg_item,
                          struct pb_buffer_arg  *item_name,
                          struct pb_buffer_arg  *item_str_val,
                          void *parsed_buffer);

