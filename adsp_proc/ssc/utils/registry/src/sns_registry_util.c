/*=============================================================================
  @file sns_registry_util.c

  Utility functions pertinent to registry for use by Sensor Developers.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <string.h>
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_registry.pb.h"
#include "sns_registry_util.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

#define SNS_REGISTRY_MAX_FLT_ARR_LEN 20

// Maximum number of nested registry subgroups in decode utility
#define STREAM_STACK_LEN 5

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Check whether the given subgroup name matches the query subgroup name.
 *
 * @return Index into arg->parse_info if found; -1 otherwise
 */
static int
check_name_match(sns_registry_decode_arg const *arg, pb_buffer_arg const *name)
{
  for(int i = 0; i < arg->parse_info_len; i++)
  {
    char *found;
    size_t grp_len = strlen(arg->parse_info[i].group_name);

    found = strstr((char*)name->buf, arg->parse_info[i].group_name);
    if(NULL != found && (strlen(found) == grp_len || 0 == grp_len))
      return i;
  }
  return -1;
}

/**
 * Parse a registry item.  If name_match is not -1, pass each item back through
 * the appropriate callback function.  If this item is of type group,
 * return it to the caller.
 *
 * @param[i] stream PB encoded stream of one registry item
 * @param[i] arg User-provided callback/search arguments
 * @param[o] stream_out If item is of type subgroup, return here
 * @param[i] parse_idx For each non-group item, call cb func for this index
 *
 * @return Index associated with returned subgroup; -2 if item is not subgroup
 */
static int
parse_reg_item(pb_istream_t *stream, sns_registry_decode_arg *arg,
    pb_istream_t *stream_out, int parse_idx)
{
  pb_wire_type_t wire_type;
  bool eof = false;
  uint32_t tag;
  pb_buffer_arg item_name, item_str;
  bool type_grp = false;
  sns_registry_data_item reg_item;
  int rv = -2;

  while(pb_decode_tag(stream, &wire_type, &tag, &eof) && !eof)
  {
    if(PB_WT_STRING == wire_type && sns_registry_data_item_str_tag == tag)
    {
      pb_istream_t substream;
      if(pb_make_string_substream(stream, &substream))
      {
        item_str.buf_len = substream.bytes_left;
        item_str.buf = substream.state;
        pb_read(&substream, NULL, substream.bytes_left);
        pb_close_string_substream(stream, &substream);
      }
    }
    else if(PB_WT_STRING == wire_type && sns_registry_data_item_name_tag == tag)
    {
      pb_istream_t substream;
      if(pb_make_string_substream(stream, &substream))
      {
        item_name.buf_len = substream.bytes_left;
        item_name.buf = substream.state;
        pb_read(&substream, NULL, substream.bytes_left);
        pb_close_string_substream(stream, &substream);
      }
    }
    else if(PB_WT_64BIT == wire_type && sns_registry_data_item_sint_tag == tag)
    {
      reg_item.has_sint = true;
      pb_decode_fixed64(stream, &reg_item.sint);
    }
    else if(PB_WT_32BIT == wire_type && sns_registry_data_item_flt_tag == tag)
    {
      reg_item.has_flt = true;
      pb_decode_fixed32(stream, &reg_item.flt);
    }
    else if(PB_WT_32BIT == wire_type && sns_registry_data_item_version_tag == tag)
    {
      reg_item.has_version = true;
      pb_decode_fixed32(stream, &reg_item.version);
    }
    else if(PB_WT_STRING == wire_type && sns_registry_data_item_subgroup_tag == tag)
    {
      pb_make_string_substream(stream, stream_out);
      pb_istream_t temp = *stream_out;
      pb_read(&temp, NULL, temp.bytes_left);
      pb_close_string_substream(stream, &temp);
      type_grp = true;
    }
    else
    {
      pb_skip_field(stream, wire_type);
    }
  }

  if(-1 != parse_idx && !type_grp)
    arg->parse_info[parse_idx].parse_func(&reg_item, &item_name,
        &item_str, arg->parse_info[parse_idx].parsed_buffer);
  if(type_grp)
    rv = check_name_match(arg, &item_name);

  return rv;
}


/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
 * Add a <stream, match> tuple to an open slot in <stream_stack, match_stack>.
 */
static void
insert_stream(pb_istream_t stream_stack[], int match_stack[],
    pb_istream_t *stream, int match)
{
  for(int i = 0; -2 != match && i < STREAM_STACK_LEN; i++)
  {
    if(-2 == match_stack[i])
    {
      match_stack[i] = match;
      stream_stack[i] = *stream;
      break;
    }
  }
}

/**
 * Find and remove an unprocessed subgroup within the stream_stack.
 */
static bool
get_stream(pb_istream_t stream_stack[], int match_stack[],
    pb_istream_t *stream, int *match)
{
  for(int i = 0; i < STREAM_STACK_LEN; i++)
  {
    if(-2 != match_stack[i])
    {
      *match = match_stack[i];
      match_stack[i] = -2;
      *stream = stream_stack[i];
      return true;
    }
  }

  return false;
}

/**
 * Callback function used to decode sns_registry_read_event.  Function will
 * extract each item within the read event including items within subgroups
 * and will optionally run a parsing function on the item passed.
 *
 * @param[i] stream, Refer pb_callback_s::decode
 * @param[i] field, Refer pb_callback_s::decode
 * @param[i] arg of type sns_registry_decode_arg
 */
SNS_SECTION(".text.sns") bool
sns_registry_item_decode_cb(pb_istream_t *stream_in, const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  sns_registry_decode_arg *decode_arg = (sns_registry_decode_arg*)*arg;
  pb_istream_t temp, stream = *stream_in;
  pb_istream_t stream_stack[STREAM_STACK_LEN];
  int match_stack[STREAM_STACK_LEN];
  int match_curr, match_result;

  for(int i = 0; i < STREAM_STACK_LEN; i++)
    match_stack[i] = -2;

  match_curr = check_name_match(decode_arg, decode_arg->item_group_name);
  match_result = parse_reg_item(&stream, decode_arg, &temp, match_curr);
  insert_stream(stream_stack, match_stack, &temp, match_result);

  while(get_stream(stream_stack, match_stack, &stream, &match_curr))
  {
    pb_wire_type_t wire_type;
    bool eof = false;
    uint32_t tag;

    while(pb_decode_tag(&stream, &wire_type, &tag, &eof) && !eof)
    {
      if(PB_WT_STRING == wire_type && sns_registry_data_items_tag == tag)
      {
        pb_istream_t substream;
        if(pb_make_string_substream(&stream, &substream))
        {
          match_result = parse_reg_item(&substream, decode_arg, &temp, match_curr);
          insert_stream(stream_stack, match_stack, &temp, match_result);
          pb_read(&substream, NULL, substream.bytes_left);
          pb_close_string_substream(&stream, &substream);
        }
      }
      else
        pb_skip_field(&stream, wire_type);
    }
  }

  pb_read(stream_in, NULL, stream_in->bytes_left);
  return true;
}

/**
 * Function used to parse items belonging to physical sensor configuration
 * registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *           defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *           string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_phy_sensor_cfg
 *           where parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_phy_sensor_cfg(sns_registry_data_item *reg_item,
    pb_buffer_arg *item_name, pb_buffer_arg *item_str_val,
    void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_sint)
  {
    sns_registry_phy_sensor_cfg *cfg =
      (sns_registry_phy_sensor_cfg*)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "is_dri", item_name->buf_len))
    {
      cfg->is_dri = (reg_item->sint == 1) ? true : false;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "hw_id",
                         item_name->buf_len))
    {
      cfg->hw_id = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "res_idx",
                         item_name->buf_len))
    {
      cfg->res_idx = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sync_stream",
                         item_name->buf_len))
    {
      cfg->sync_stream = (reg_item->sint == 1) ?
        true : false;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**
 * Function used to parse items belonging to physical sensor
 * platform configuration registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as defined in
 *           sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item string value as
 *           defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_phy_sensor_pf_cfg
 *           where parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_phy_sensor_pf_cfg(sns_registry_data_item *reg_item,
    pb_buffer_arg *item_name, pb_buffer_arg *item_str_val, void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else
  {
    sns_registry_phy_sensor_pf_cfg *cfg =
      (sns_registry_phy_sensor_pf_cfg*)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "bus_type", item_name->buf_len) &&
       reg_item->has_sint)
    {
      cfg->bus_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "bus_instance", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->bus_instance = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "slave_config", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->slave_config = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "min_bus_speed_khz", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->min_bus_speed_khz = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "max_bus_speed_khz", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->max_bus_speed_khz = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "reg_addr_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->reg_addr_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "dri_irq_num", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->dri_irq_num = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_pull_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_pull_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_is_chip_pin", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_is_chip_pin = (1 == reg_item->sint);
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_drive_strength", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_drive_strength = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_trigger_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_trigger_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "num_rail", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->num_rail = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "rail_on_state", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->rail_on_state = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "vddio_rail", item_name->buf_len) &&
            item_str_val->buf_len > 0)
    {
      sns_strlcpy(cfg->vddio_rail,
                  item_str_val->buf,
                  sizeof(cfg->vddio_rail));
    }
    else if(0 == strncmp((char*)item_name->buf, "vdd_rail", item_name->buf_len) &&
            item_str_val->buf_len > 0)
    {
      sns_strlcpy(cfg->vdd_rail,
                  item_str_val->buf,
                  sizeof(cfg->vdd_rail));
    }
    else if(0 == strncmp((char*)item_name->buf, "rigid_body_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->rigid_body_type = reg_item->sint;
    }
  }

  return rv;
}

/**
 * Function used to parse items belonging to physical sensor platform
 * configuration registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as defined
 *           in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item string value as
 *           defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_phy_sensor_pf_cfg where
 *           parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_axis_orientation(sns_registry_data_item *reg_item,
                                    pb_buffer_arg  *item_name,
                                    pb_buffer_arg  *item_str_val,
                                    void *parsed_buffer)
{
  bool rv = true;

  if(NULL == parsed_buffer || NULL == reg_item || NULL == item_name ||
     NULL == item_str_val)
  {
    rv = false;
  }
  else
  {
    triaxis_conversion *axis_map = (triaxis_conversion*)parsed_buffer;

    if(('x' == ((char*)item_name->buf)[0] || 'y' == ((char*)item_name->buf)[0] ||
        'z' == ((char*)item_name->buf)[0]) && item_str_val->buf_len > 0)
    {
      triaxis axis_idx = TRIAXIS_X;

      if('y' == ((char*)item_name->buf)[0])
      {
        axis_idx = TRIAXIS_Y;
      }
      else if('z' == ((char*)item_name->buf)[0])
      {
        axis_idx = TRIAXIS_Z;
      }

      if(('-' == ((char*)item_str_val->buf)[0] || '+' == ((char*)item_str_val->buf)[0]) &&
          ('x' == ((char*)item_str_val->buf)[1] || 'y' == ((char*)item_str_val->buf)[1] ||
           'z' == ((char*)item_str_val->buf)[1]))
      {
        if('-' == ((char*)item_str_val->buf)[0])
        {
          axis_map[axis_idx].invert = true;
        }

        if('x' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_X;
        }
        else if('y' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_Y;
        }
        else if('z' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_Z;
        }
      }
    }
  }

  return rv;
}

/**
 * Function used to parse N dimensional float array.
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *           defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *           string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type pb_float_arr_arg where parsed
 *           items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_float_arr(sns_registry_data_item *reg_item,
                             pb_buffer_arg  *item_name,
                             pb_buffer_arg  *item_str_val,
                             void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt)
  {
    pb_float_arr_arg *bias_arr_arg = (pb_float_arr_arg*)parsed_buffer;

    if(*bias_arr_arg->arr_index <= SNS_REGISTRY_MAX_FLT_ARR_LEN)
    {
      bias_arr_arg->arr[*bias_arr_arg->arr_index] = reg_item->flt;
      (*bias_arr_arg->arr_index)++;
    }
    else
    {
      rv = false;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**
 * Function used to parse items belonging to factory calibration
 * 3 * 3 correction matrix registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *           defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *           string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type matrix3 where parsed items
 *           are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_corr_matrix_3(sns_registry_data_item *reg_item,
                                 pb_buffer_arg  *item_name,
                                 pb_buffer_arg  *item_str_val,
                                 void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt)
  {
    matrix3 *mat = (matrix3*)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "0_0", item_name->buf_len))
    {
      mat->e00 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "0_1", item_name->buf_len))
    {
      mat->e01 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "0_2", item_name->buf_len))
    {
      mat->e02 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_0", item_name->buf_len))
    {
      mat->e10 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_1", item_name->buf_len))
    {
      mat->e11 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_2", item_name->buf_len))
    {
      mat->e12 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_0", item_name->buf_len))
    {
      mat->e20 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_1", item_name->buf_len))
    {
      mat->e21 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_2", item_name->buf_len))
    {
      mat->e22 = reg_item->flt;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**
 * Function used to parse items in Motion Detect Config registry group
 *
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *           defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item
 *           string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_md_cfg where
 *           parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_md_cfg(sns_registry_data_item *reg_item,
                          pb_buffer_arg  *item_name,
                          pb_buffer_arg  *item_str_val,
                          void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else
  {
    sns_registry_md_cfg *md_cfg = (sns_registry_md_cfg*)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "disable", item_name->buf_len) &&
       reg_item->has_sint)
    {
      md_cfg->disable = (reg_item->sint == 1) ? true : false;
    }
    else if(0 == strncmp((char*)item_name->buf, "thresh", item_name->buf_len) &&
       reg_item->has_flt)
    {
      md_cfg->thresh = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "win", item_name->buf_len) &&
       reg_item->has_flt)
    {
      md_cfg->win = reg_item->flt;
    }
  }

  return rv;
}
