/**========================================================================
 @file vpm_session_cal_utils.cpp
 
 @brief This file contains calibration utilites for session objects
 
 Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_cal_utils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"
#include "vpm_session_dyn_svc_cfg_utils.h"
#include "vpm_session_utils.h"
#include "mmstd.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Table used for volume interpolation. */
static const uint32_t vpm_vol_interpolation_table[] = {
   2684,       3012,       3379,       3792,        4254,
   4774,       5356,       6010,       6743,        7566,
   8489,       9524,      10687,      11991,       13454,
   15095,      16937,      19004,      21323,       23924,
   26844,      30119,      33794,      37918,       42544,
   47735,      53560,      60095,      67428,       75655,
   84887,      95244,     106866,     119906,      134536,
   150952,     169371,     190038,     213226,      239243,
   268435,     301190,     337940,     379175,      425442,
   477353,     535599,     600952,     674279,      756554,
   848867,     952445,    1068661,    1199057,     1345364,
   1509524,    1693713,    1900377,    2132259,     2392434,
   2684355,    3011895,    3379402,    3791752,     4254415,
   4773532,    5355991,    6009521,    6742794,     7565539,
   8488674,    9524449,   10686608,   11990571,    13453642,
   15095235,   16937132,   19003775,   21322586,    23924335,
   26843546,   30118954,   33794022,   37917516,    42544153,
   47735324,   53559915,   60095213,   67427938,    75655391,
   84886745,   95244494,  106866080,  119905714,   134536424,
   150952350,  169371322,  190037749,  213225862,   239243352,
   268435456,  301189535,  337940217,  379175160,   425441527,
   477353244,  535599149,  600952130,  674279380,   756553907,
   848867446,  952444939, 1068660799, 1199057137,  1345364236,
   1509523501, 1693713225, 1900377495, 2132258619, 2392433520u,
   2684354560u
};

/* -----------------------------------------------------------------------
** Function definitions
** ----------------------------------------------------------------------- */

/* Find volume table format from a cal entry. */
static ADSPResult vpm_find_format_from_cal_entry(cvd_cal_table_descriptor_t *table_descriptor,
                                                 void *data,
                                                 uint32_t data_len,
                                                 bool_t *is_v1_format)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   bool_t               is_vol_level_found = FALSE;
   uint32_t             consumed_len = 0;
   voice_param_data_t   *cur_param;
   uint8_t              *vol_data;

   if (!is_v1_format)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_format_from_cal_entry(): Invalid input parameters");

      return ADSP_EBADPARAM;
   }

   *is_v1_format = FALSE;
   vol_data = (uint8_t *)data;

   /* Initial validation */
   rc = cvd_cal_validate_entry(table_descriptor, data, data_len);
   if (rc != APR_EOK)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_format_from_cal_entry(): Invalid table");
      return rc;
   }

   while (consumed_len < data_len)
   {
      if ((data_len - consumed_len) < sizeof(voice_param_data_t))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_format_from_cal_entry(): Improperly formatted cal data, "\
                "not enough bytes to form voice_param_data_t");

         result = ADSP_EFAILED;
         break;
      }

      cur_param = ((voice_param_data_t *)(vol_data + consumed_len));

      /* Validate param size */
      rc = cvd_cal_validate_entry(table_descriptor, data,
                                  consumed_len + cur_param->param_size + sizeof(voice_param_data_t));
      if (rc != APR_EOK)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_format_from_cal_entry(): Invalid table");

         result = ADSP_EFAILED;
         break;
      }

      consumed_len += (sizeof(voice_param_data_t));

      if ((cur_param->module_id == VOICE_MODULE_RX_VOL) &&
          (cur_param->param_id == VOICE_PARAM_VOL))
      {
         is_vol_level_found = TRUE;
         *is_v1_format = TRUE;
         break;
      }
      else if ((cur_param->module_id == VSS_MODULE_RX_VOLUME) &&
               (cur_param->param_id == VSS_PARAM_VOLUME))
      {
         is_vol_level_found = TRUE;
         *is_v1_format = FALSE;
         break;
      }

      consumed_len += cur_param->param_size;
   }

   if (FALSE == is_vol_level_found)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_format_from_cal_entry(): No format info found");

      result = ADSP_EFAILED;
   }

   return result;
}

/* Find volume table format from matched calibration entries. */
static ADSPResult vpm_find_vol_table_format_from_matched_cal_entries(vpm_session_object_t *session_obj,
                                                                     bool_t *is_v1_format)
{
   ADSPResult        result = ADSP_EOK;
   int32_t           rc;
   uint32_t          idx;
   void              *data;
   uint32_t          data_len;
   cvd_cal_entry_t   *cal_entry;
   bool_t            is_found = FALSE;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_table_format_from_matched_cal_entries(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   if (!is_v1_format)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_table_format_from_matched_cal_entries(): Invalid input parameters");

      return ADSP_EBADPARAM;
   }

   cal_entry = session_obj->volume_cal.matching_entries;

   for (idx = 0; idx < session_obj->volume_cal.num_matching_entries; ++idx)
   {
      data = cal_entry->start_ptr;
      data_len = cal_entry->size;

      if (ADSP_EOK == (result = vpm_find_format_from_cal_entry(&session_obj->volume_cal.table_handle->table_descriptor,
                                                               data,
                                                               data_len,
                                                               is_v1_format)))
      {
         /* Found volume level, exit. */
         is_found = TRUE;
         break;
      }
      /* else: Not found, keep searching. */

      cal_entry++;
   }

   if (FALSE == is_found)
   {
      result = ADSP_EFAILED;
   }

   return result;
}

/* Caller:  vpm_find_vol_table_format(). */
static ADSPResult vpm_find_vol_default_entry(vpm_session_object_t *session_obj,
                                             uint32_t vol_index)
{
   ADSPResult        result = ADSP_EOK;
   int32_t           rc;
   cvd_cal_column_t  columns[VPM_VOLUME_NUM_CAL_COLUMNS];
   cvd_cal_key_t     cal_key;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_table_format_from_matched_cal_entries(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Form a query key. */
   memset(columns, 0, sizeof(columns));

   columns[0].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
   columns[1].id = VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX;
   columns[0].value = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
   columns[1].value = vol_index;

   switch (session_obj->direction)
   {
      case VSS_IVOCPROC_DIRECTION_RX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[2].value = VPM_DEFAULT_RX_PP_SR;
         cal_key.num_columns = 3;

         break;
      }
      case VSS_IVOCPROC_DIRECTION_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = VPM_DEFAULT_TX_PP_SR;
         cal_key.num_columns = 3;

         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[3].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = VPM_DEFAULT_RX_PP_SR;
         columns[3].value = VPM_DEFAULT_TX_PP_SR;
         cal_key.num_columns = 4;

         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_find_vol_default_entry(): Incorrect VPM direction: %lu", session_obj->direction);

         return ADSP_EFAILED;
      }
   } /* End of switch (session_obj->direction) */

   cal_key.columns = columns;

   /* Search volume level from matched entries. */
   /* Find matched entries for vol_index. */
   rc = cvd_cal_query_table(session_obj->volume_cal.table_handle,
                            &cal_key, sizeof(session_obj->volume_cal.matching_entries),
                            session_obj->volume_cal.matching_entries,
                            &session_obj->volume_cal.num_matching_entries);

   if ((rc != APR_EOK) ||
       (!session_obj->volume_cal.num_matching_entries))
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_find_vol_default_entry(): Cannot find entries for volume index %d, rc=0x%08X",
            vol_index, rc);

      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_find_vol_default_entry(): network 0x%08X, Rx SR %d, Tx SR %d",
            columns[0].value,  columns[2].value, columns[3].value);

      result = ADSP_EFAILED;
   }

   return result;
}

/* Find volume table format. */
ADSPResult vpm_find_vol_table_format(vpm_session_object_t *session_obj,
                                     bool_t *is_v1_format)
{
   ADSPResult result = ADSP_EOK;
   int32_t    rc;

   if (!is_v1_format)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_table_format(): Invalid input parameter");

      return ADSP_EBADPARAM;
   }

   *is_v1_format = FALSE;

   /* Query min volume index with defaultsampling rate. */
   /* This is to obatin matched entries for checking table format. */
   if (ADSP_EOK != (result = vpm_find_vol_default_entry(session_obj,
                                                        session_obj->volume_cal.min_vol_index)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_table_format(): Cannot find min volume level");

      return ADSP_EFAILED;

   }

   /* Search through the matched entries. */
   result = vpm_find_vol_table_format_from_matched_cal_entries(session_obj, is_v1_format);

   return result;
}

/* Find volume level from a cal entry. */
static ADSPResult vpm_find_vol_level_from_cal_entry(cvd_cal_table_descriptor_t *table_descriptor,
                                                    void *data,
                                                    uint32_t data_len,
                                                    uint32_t *ret_vol_level)
{
   ADSPResult           result = ADSP_EOK;
   int32_t               rc;
   bool_t                is_vol_level_found = FALSE;
   uint32_t              consumed_len = 0;
   voice_param_data_t    *cur_param;
   uint8_t               *vol_data;

   if (!ret_vol_level)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_level_from_cal_entry(): Invalid input parameters");
      return ADSP_EBADPARAM;
   }

   vol_data = (uint8_t *)data;

   /* Initial validation */
   rc = cvd_cal_validate_entry(table_descriptor, data, data_len);
   if (rc != APR_EOK)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_level_from_cal_entry(): Invalid table");
      return ADSP_EFAILED;
   }

   while (consumed_len < data_len)
   {
      if ((data_len - consumed_len) < sizeof(voice_param_data_t))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_level_from_cal_entry(): Improperly formatted cal data, "\
                "not enough bytes to form voice_param_data_t");
         result = ADSP_EFAILED;
         break;
      }

      cur_param = ((voice_param_data_t *)(vol_data + consumed_len));

      /* Validate param size */
      rc = cvd_cal_validate_entry(table_descriptor, data,
                                  consumed_len + cur_param->param_size + sizeof(voice_param_data_t));
      if (rc != APR_EOK)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_level_from_cal_entry(): Invalid table");

         result = ADSP_EFAILED;
         break;
      }

      consumed_len += (sizeof(voice_param_data_t));

      if (((cur_param->module_id == VSS_MODULE_RX_VOLUME) && (cur_param->param_id == VSS_PARAM_VOLUME)) ||
          ((cur_param->module_id == VOICE_MODULE_RX_VOL) && (cur_param->param_id == VOICE_PARAM_VOL)))
      {
         is_vol_level_found = TRUE;
         *ret_vol_level = *((int32_t *)(vol_data + consumed_len));
         result = ADSP_EOK;
         break;
      }

      consumed_len += cur_param->param_size;
   }

   if (FALSE == is_vol_level_found)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_level_from_cal_entry(): No volume level found in this cal entry");

      result = ADSP_EFAILED;
   }

   return result;
}

/* Find volume level from matched calibration entries. */
/* Caller: vpm_find_vol_level() and
           cvp_calibrate_network(). */
static ADSPResult vpm_find_vol_level_from_matched_cal_entries(vpm_session_object_t *session_obj,
                                                              uint32_t *ret_vol_level)
{
   ADSPResult        result = ADSP_EOK;
   int32_t           rc;
   uint32_t          idx;
   uint32_t          vol_level;
   void              *data;
   uint32_t          data_len;
   cvd_cal_entry_t   *cal_entry;
   bool_t            is_found = FALSE;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_level_from_matched_cal_entries(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   if (!ret_vol_level)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_level_from_matched_cal_entries(): Invalid input parameters");

      return ADSP_EBADPARAM;
   }

   cal_entry = session_obj->volume_cal.matching_entries;

   for (idx = 0; idx < session_obj->volume_cal.num_matching_entries; ++idx)
   {
      data = cal_entry->start_ptr;
      data_len = cal_entry->size;

      if (ADSP_EOK == (result = vpm_find_vol_level_from_cal_entry(&session_obj->volume_cal.table_handle->table_descriptor,
                                                                  data, data_len,
                                                                  &vol_level)))
      {
         /* Found volume level, exit. */
         is_found = TRUE;
         *ret_vol_level = vol_level;
         break;
      }
      /* else: not found, keep searching. */

      cal_entry++;
   }

   if (FALSE == is_found)
   {
      result = ADSP_EFAILED;
   }

   return result;
}

static uint32_t vpm_convert_mB_to_linearQ28(int32_t  volume_level_mB)
{
   uint32_t linear_gain = 0;
   /* Computed linear gain in Q28 format. */
   int32_t vol_diff_one;
   /* Intermediates needed for computation. */
   uint32_t vol_diff_two;
   /* Intermediate needed for computation. */
   int32_t closest_mB;
   /* Closest known mB to given mB. */
   int32_t index;
   /* Index into volume interpolation table. */
   int64_t vol_diff_product;
   /* Intermediates needed for computation. */

   /* Find the closest (lower) adjacent mB from the known values.*/
   {
      closest_mB = volume_level_mB % VPM_VOLUME_INTERPOLATION_STEPS;

      if (closest_mB < 0)
      {
         closest_mB = volume_level_mB - closest_mB - VPM_VOLUME_INTERPOLATION_STEPS;
      }
      else
      {
         closest_mB = volume_level_mB - closest_mB;
      }

      vol_diff_one = volume_level_mB - closest_mB;

      index = volume_level_mB - VPM_MIN_VOLUME_MB_SUPPORTED;
      index = index / VPM_VOLUME_INTERPOLATION_STEPS;

      linear_gain = vpm_vol_interpolation_table[index];

      if (vol_diff_one != 0)
      {
         /* For all other values compute value by interpolation. */
         vol_diff_two = (int32_t)(vpm_vol_interpolation_table[index + 1] - linear_gain);
         vol_diff_product = ((int64_t)vol_diff_two) * ((int64_t)vol_diff_one);
         linear_gain = linear_gain + (uint32_t)((vol_diff_product / VPM_VOLUME_INTERPOLATION_STEPS));
      }
   }

   return linear_gain;
}

/* Applies volume calibration data to VPM for linear volume mapping use case,
 * using the rsp_fn provided by the caller for handling the individual
 * set_param response.
 */
static ADSPResult vpm_calibrate_volume_linear_mapping(vpm_session_object_t *session_obj,
                                                      uint32_t vol_ramp_duration)
{
   ADSPResult              result = ADSP_EFAILED;
   voice_set_param_v2_t    set_param;
   cvd_cal_column_t        columns[VPM_VOLUME_NUM_CAL_COLUMNS];
   cvd_cal_key_t           cal_key;
   uint32_t                set_param_cnt;
   uint32_t                vol_level;
   vpm_vol_param_t         *p_vol_set_param_payload;
   uint32_t                vol_level_linearQ28;
   void                    *payload_address;
   void                    *ib_payload_address = NULL;
   uint32_t                payload_size;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_volume_linear_mapping(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   session_obj->volume_cal.num_matching_entries = 0;

   memset(columns, 0, sizeof(columns));

   columns[0].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
   columns[1].id = VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX;
   columns[0].value = session_obj->target_set.system_config.network_id;
   columns[1].value = session_obj->target_set.vol_step;

   switch (session_obj->direction)
   {
      case VSS_IVOCPROC_DIRECTION_RX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
         cal_key.num_columns = 3;
         break;
      }
      case VSS_IVOCPROC_DIRECTION_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.tx_pp_sr;
         cal_key.num_columns = 3;
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[3].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
         columns[3].value = session_obj->target_set.system_config.tx_pp_sr;
         cal_key.num_columns = 4;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_volume_linear_mapping(): Incorrect vpm direction: %lu",
               session_obj->direction);

         return ADSP_EFAILED;
      }
   }

   cal_key.columns = columns;

   /* Set the command response status to pending */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   for (;;)
   {
      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_calibrate_volume_linear_mapping(): Network 0x%08X, vol_index %d",
            columns[0].value, columns[1].value);

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_linear_mapping(): Rx SR %d, Tx SR %d",
            columns[2].value, columns[3].value);

      result = cvd_cal_query_table(session_obj->volume_cal.table_handle,
                                   &cal_key,
                                   sizeof(session_obj->volume_cal.matching_entries),
                                   session_obj->volume_cal.matching_entries,
                                   &session_obj->volume_cal.num_matching_entries);
      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_calibrate_volume_linear_mapping(): cvd_cal_query_table "\
                  "failed, result: 0x%08X. Not calibrating.", result);
         break;
      }

      if (0 == session_obj->volume_cal.num_matching_entries)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_calibrate_volume_linear_mapping(): Cannot find a matching "\
                "entry. Not calibrating");

         result = ADSP_ECONTINUE;
         break;
      }

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_linear_mapping(): Found %d matched entries",
            session_obj->volume_cal.num_matching_entries);
      
      /* Apply the calibration data. */
      for (set_param_cnt = 0; set_param_cnt < session_obj->volume_cal.num_matching_entries; ++set_param_cnt)
      {
         payload_address = (void *)session_obj->volume_cal.matching_entries[set_param_cnt].start_ptr;
         payload_size = session_obj->volume_cal.matching_entries[set_param_cnt].size;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   payload_address,
                                                                   payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_volume_linear_mapping(): "
                  "Failed to send set param for matched entry cnt: %lu, result: 0x%lx",
                  set_param_cnt, result);
         }
         else
         {
            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_calibrate_volume_linear_mapping(): payload_address: 0x%08X, "\
                     "payload_size: %lu, memory handle 0x%lx",
                  payload_address, payload_size, session_obj->volume_cal.vpm_mem_handle);
         }

      } /* End of for (set_param_cnt = 0) */

      /* Find the volume level. */
      if (ADSP_EOK != (result = vpm_find_vol_level_from_matched_cal_entries(session_obj, &vol_level)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_calibrate_volume_linear_mapping(): Cannot find volume "\
                  "level from matched entries, rc=0x%08X", result);

         break;
      }

      /* Apply the volume level. */
      /* Calculate volume level in linear Q28 format and set volume level to FW. */
      if (TRUE == session_obj->volume_cal.is_v1_format)
      {
         /* Convert from linearQ13 (32 bit) to linearQ28. */
         vol_level_linearQ28 = (vol_level << 15);
      }
      else
      {
         /* Convert from mb to linearQ28. */
         vol_level_linearQ28 = vpm_convert_mB_to_linearQ28(vol_level);
      }

      /* Allocate payload for in-band set param */
      if (NULL == (ib_payload_address = qurt_elite_memory_malloc(sizeof(vpm_vol_param_t),
                                                                 QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_volume_interpolation(): Failed to allocate set param payload for Rx"
             "soft volume calib");

         result = ADSP_ENOMEMORY;
         break;
      }

      /* Update the payload pointer in response handler */
      session_obj->session_rsp_ctrl.p_ack_payload = ib_payload_address;

      /* Populate the set param payload */
      p_vol_set_param_payload = (vpm_vol_param_t *)ib_payload_address;

      p_vol_set_param_payload->module_id = VOICE_MODULE_RX_VOL;
      p_vol_set_param_payload->param_id = VOICE_PARAM_SOFT_VOL;
      p_vol_set_param_payload->param_size = sizeof(vpm_vol_param_data_t);
      p_vol_set_param_payload->reserved = 0;
      p_vol_set_param_payload->param_data.volume = vol_level_linearQ28;
      p_vol_set_param_payload->param_data.ramp_duration_ms = (uint16_t)vol_ramp_duration;
      p_vol_set_param_payload->param_data.reserved = 0;

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_calibrate_volume_linear_mapping(): vol_level = %d, "
            "vol_level_linearQ28 = %d, vol_ramp_duration = %d",
            vol_level, vol_level_linearQ28, vol_ramp_duration);

      if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                ib_payload_address,
                                                                sizeof(vpm_vol_param_t),
                                                                ELITE_CMD_SET_PARAM,
                                                                CVD_CAL_PARAM_MINOR_VERSION_0)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_volume_linear_mapping(): Failed to send set param for applying vol level"
               "result: 0x%lx", result);
      }

      break;
   } /* End of for (;;)*/

   /* Check if no set param commands issued */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      if (ADSP_EOK == result)
      {
         result = ADSP_EFAILED;
      }

      /* Remove the command response pending status, if set */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      if (ib_payload_address)
      {
         /* Free-up the memory allocated for in-band set param
            Note that if at least 1 set param succeed, allocated memory is
            freed in response handler*/
         qurt_elite_memory_free(ib_payload_address);
      }
   }

   return result;
}

/* Find volume level corresponding to a volume index from cal table. */
/* Caller: vpm_find_closest_vol_index(), and cvp_find_vol_table_format()
           vpm_calibrate_volume_interpolation(). */
static ADSPResult vpm_find_vol_level(vpm_session_object_t *session_obj,
                                     uint32_t vol_index,
                                     uint32_t *ret_vol_level)
{
   ADSPResult        result = ADSP_EOK;
   int32_t            rc;
   uint32_t           vol_level;
   cvd_cal_column_t   columns[VPM_VOLUME_NUM_CAL_COLUMNS];
   cvd_cal_key_t      cal_key;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_vol_level(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   if (!ret_vol_level)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_vol_level(): Invalid input parameters");
      return ADSP_EFAILED;
   }

   /* Form a query key. */
   memset(columns, 0, sizeof(columns));

   columns[0].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
   columns[1].id = VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX;
   columns[0].value = session_obj->target_set.system_config.network_id;
   columns[1].value = vol_index;

   switch (session_obj->direction)
   {
      case VSS_IVOCPROC_DIRECTION_RX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
         cal_key.num_columns = 3;
         break;
      }
      case VSS_IVOCPROC_DIRECTION_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.tx_pp_sr;
         cal_key.num_columns = 3;
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX_TX:
      {
         columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
         columns[3].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
         columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
         columns[3].value = session_obj->target_set.system_config.tx_pp_sr;
         cal_key.num_columns = 4;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_find_vol_level(): Incorrect vpm direction: %lu",
               session_obj->direction);

         return ADSP_EFAILED;
      }
   }

   cal_key.columns = columns;

   /* Search volume level from matched entries. */
   for (;;)
   {
      /* Find matched entries for vol_index. */
      rc = cvd_cal_query_table(session_obj->volume_cal.table_handle,
                               &cal_key, sizeof(session_obj->volume_cal.matching_entries),
                               session_obj->volume_cal.matching_entries,
                               &session_obj->volume_cal.num_matching_entries);

      if ((rc != APR_EOK) || (session_obj->volume_cal.num_matching_entries == 0))
      {
         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_find_vol_level(): Cannot find entries for volume index %d, rc=0x%08X",
               vol_index, rc);

         MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_find_vol_level(): network 0x%08X, Rx SR %d, Tx SR %d",
               columns[0].value,  columns[2].value, columns[3].value);

         result = ADSP_EFAILED;
         break;
      }

      /* Find volume level from matched entries. */
      if (ADSP_EOK != (result = vpm_find_vol_level_from_matched_cal_entries(session_obj,
                                                                            &vol_level)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_find_vol_level(): Cannot find volume level from entries, rc=0x%08X",
               result);

         result = ADSP_EFAILED;
         break;
      }

      *ret_vol_level = vol_level;

      result = ADSP_EOK;
      break;
   }

   return result;
}

static inline int32_t vpm_convert_volume_step_to_mB(int32_t min_vol_mB,
                                                    int32_t max_vol_mB,
                                                    uint32_t total_vol_steps,
                                                    uint32_t vol_step)
{
   int32_t mB_per_step;
   int32_t vol_level;

   mB_per_step = (max_vol_mB - min_vol_mB) / total_vol_steps;
   vol_level = min_vol_mB + mB_per_step * vol_step;

   return vol_level;
}

static inline uint32_t vpm_abs(int32_t value)
{
   return (value  < 0) ? -(value) : value;
}

/* Find the volume index that indexes to the closest volume level
   (to the target volume level) in calibration table.
   Caller: vpm_calibrate_volume_interpolation(). */
static ADSPResult vpm_find_closest_vol_index(vpm_session_object_t *session_obj,
                                             uint32_t in_vol_level,
                                             uint32_t *ret_vol_index)
{
   ADSPResult result = ADSP_EOK;
   uint32_t vol_index;
   uint32_t vol_level = 0xFFFFFFFF;
   uint32_t distance = 0XFFFFFFFF;
   uint32_t cur_distance;
   bool_t is_found = FALSE;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_closest_vol_index(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   if (NULL == ret_vol_index)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_closest_vol_index(): Invalid input parameters");
      return ADSP_EBADPARAM;
   }

   /* Note: Double closed region ([min, max]). */
   for (vol_index = session_obj->volume_cal.min_vol_index;
        vol_index <= session_obj->volume_cal.max_vol_index;
        ++vol_index)
   {
      if (ADSP_EOK != (result = vpm_find_vol_level(session_obj, vol_index, &vol_level)))
      {
         /* Not found, keep searching. */
         continue;
      }

      cur_distance = vpm_abs(in_vol_level - vol_level);

      if (distance > cur_distance)
      {
         /* Current distance is decreasing, keep searching. */
         distance = cur_distance;
      }
      else
      {
         /* Current distance is increasing, exit. */
         /* Assumption: volume level increases as volume index increases. */
         is_found = TRUE;
         *ret_vol_index = (vol_index - 1); /* Assumption: vol index is incremented by 1. */
         result = ADSP_EOK;
         break;
      }
   }

   if (FALSE == is_found)
   {
      /* No volume level has been found. */
      result = ADSP_EFAILED;
   }

   return result;
}

/* Applies volume calibration data to VPM for non-linear volume mapping use
 * case (interpolation is required), using the rsp_fn provided by the caller
 * for handling the individual set_param response.
 */
static ADSPResult vpm_calibrate_volume_interpolation(vpm_session_object_t *session_obj,
                                                     uint32_t vol_ramp_duration)
{
   ADSPResult              result = ADSP_EFAILED;
   uint32_t                min_vol_level;
   uint32_t                max_vol_level;
   uint32_t                interpolated_vol_level;
   uint32_t                set_param_cnt;
   voice_set_param_v2_t    set_param;
   uint32_t                vol_index = 0;
   uint32_t                vol_level_linearQ28;
   cvd_cal_column_t        columns[VPM_VOLUME_NUM_CAL_COLUMNS];
   cvd_cal_key_t           cal_key;
   void                    *payload_address;
   void                    *ib_payload_address = NULL;
   vpm_vol_param_t         *p_vol_set_param_payload;
   uint32_t                payload_size;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_volume_interpolation(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Set the command response status to pending */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   for (;;)
   {
      session_obj->volume_cal.num_matching_entries = 0;

      /* Find min and max volume level for a specific network ID. */
      /* Input: network id, volume table, min_vol_index, and max_vol_index. */
      /* Output: min and max volume level (corresponding to the min_vol_index and max_vol_index). */
      /* Assumption: volume level increases as volume index increase. */

      if (ADSP_EOK != (result = vpm_find_vol_level(session_obj,
                                                   session_obj->volume_cal.min_vol_index,
                                                   &min_vol_level)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_volume_interpolation(): Cannot find min volume "\
                "level. Not calibrating");
         break;
      }

      if (ADSP_EOK != (result = vpm_find_vol_level(session_obj,
                                                   session_obj->volume_cal.max_vol_index,
                                                   &max_vol_level)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_volume_interpolation(): Cannot find max volume level. "\
                "Not calibrating");
         break;
      }

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_calibrate_volume_interpolation(): min_vol_level = %d, max_vol_level = %d",
            min_vol_level, max_vol_level);

      /* Interpolate target volume step into target volume level. */
      /* Input: client specified volume step, number of total volume steps, min and max volume level. */
      /* Output: interpolated volume level in mB. */

      interpolated_vol_level = vpm_convert_volume_step_to_mB(min_vol_level,
                                                             max_vol_level,
                                                             session_obj->target_set.client_num_vol_steps,
                                                             session_obj->target_set.vol_step);

      /* Find the closest volume level in table, as well as the corresponding volume index. */
      /* Input: interpolated volume level in mB. */
      /* Output: closest volume level in table, and the corresponding volume index. */
      /* Assumption: volume level increases as volume index increase. */

      result = vpm_find_closest_vol_index(session_obj, interpolated_vol_level, &vol_index);

      if (result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_volume_interpolation(): Cannot find closest volume level to "\
                  "target volume level %d. Not calibrating",
               interpolated_vol_level);
         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_interpolation(): interpolated_vol_level = %d, "\
                  "vol_index = %d",
               interpolated_vol_level,
               vol_index);
      }

      /* Find the calibration data of the closest volume level. */

      memset(columns, 0, sizeof(columns));
      columns[0].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
      columns[1].id = VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX;
      columns[0].value = session_obj->target_set.system_config.network_id;
      columns[1].value = vol_index;

      switch (session_obj->direction)
      {
         case VSS_IVOCPROC_DIRECTION_RX:
         {
            columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
            cal_key.num_columns = 3;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_TX:
         {
            columns[2].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            columns[2].value = session_obj->target_set.system_config.tx_pp_sr;
            cal_key.num_columns = 3;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_RX_TX:
         {
            columns[2].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            columns[3].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            columns[2].value = session_obj->target_set.system_config.rx_pp_sr;
            columns[3].value = session_obj->target_set.system_config.tx_pp_sr;
            cal_key.num_columns = 4;
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_volume_interpolation(): Incorrect vpm direction: %lu",
                  session_obj->direction);

            return ADSP_EFAILED;
         }
      }

      cal_key.columns = columns;

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_interpolation(): Network 0x%08X, vol_index %d",
            columns[0].value,
            columns[1].value);

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_interpolation(): Rx SR %d, Tx SR %d",
            columns[2].value,
            columns[3].value);

      result = cvd_cal_query_table(session_obj->volume_cal.table_handle,
                                   &cal_key,
                                   sizeof(session_obj->volume_cal.matching_entries),
                                   session_obj->volume_cal.matching_entries,
                                   &session_obj->volume_cal.num_matching_entries);

      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_calibrate_volume_interpolation(): cvd_cal_query_table "\
                  "failed, result: 0x%08X. Not calibrating.", result);

         break;
      }

      if (0 == session_obj->volume_cal.num_matching_entries)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_calibrate_volume_interpolation(): Cannot find a matching "\
                "entry. Not calibrating");

         result = ADSP_ECONTINUE;
         break;
      }

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_volume_interpolation(): Find %d matched entries",
            session_obj->volume_cal.num_matching_entries);

      /* Apply calibration data. */
      for (set_param_cnt = 0; set_param_cnt < session_obj->volume_cal.num_matching_entries; ++set_param_cnt)
      {
         payload_address = (void *)session_obj->volume_cal.matching_entries[set_param_cnt].start_ptr;
         payload_size = session_obj->volume_cal.matching_entries[set_param_cnt].size;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   payload_address,
                                                                   payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_volume_interpolation(): "
                  "Failed to send set param for matched entry cnt: %lu, result: 0x%lx",
                  set_param_cnt, result);
         }
         else
         {
            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_calibrate_volume_interpolation(): payload_address: 0x%08X, "\
                     "payload_size: %lu, memory handle 0x%lx",
                  payload_address, payload_size,
                  session_obj->volume_cal.vpm_mem_handle);
         }

      } /* End of for(set_param_cnt)*/

      /* Apply volume level. */
      /* Convert interpolated volume level in mB to Q28 format. */
      vol_level_linearQ28 = vpm_convert_mB_to_linearQ28(interpolated_vol_level);

      /* Allocate payload for in-band set param */
      if (NULL == (ib_payload_address = qurt_elite_memory_malloc(sizeof(vpm_vol_param_t),
                                                                 QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_volume_interpolation(): Failed to allocate set param payload for Rx"
             "soft volume calib");

         result = ADSP_ENOMEMORY;
         break;
      }

      /* Update the payload pointer in response handler */
      session_obj->session_rsp_ctrl.p_ack_payload = ib_payload_address;

      /* Populate the set param payload */
      p_vol_set_param_payload = (vpm_vol_param_t *)ib_payload_address;

      p_vol_set_param_payload->module_id = VOICE_MODULE_RX_VOL;
      p_vol_set_param_payload->param_id = VOICE_PARAM_SOFT_VOL;
      p_vol_set_param_payload->param_size = sizeof(vpm_vol_param_data_t);
      p_vol_set_param_payload->reserved = 0;
      p_vol_set_param_payload->param_data.volume = vol_level_linearQ28;
      p_vol_set_param_payload->param_data.ramp_duration_ms = (uint16_t)vol_ramp_duration;
      p_vol_set_param_payload->param_data.reserved = 0;

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_calibrate_volume_interpolation(): interpolated_vol_level = %d, "
            "vol_level_linearQ28 = %d, vol_ramp_duration = %d",
            interpolated_vol_level, vol_level_linearQ28, vol_ramp_duration);

      if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                ib_payload_address,
                                                                sizeof(vpm_vol_param_t),
                                                                ELITE_CMD_SET_PARAM,
                                                                CVD_CAL_PARAM_MINOR_VERSION_0)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_volume_interpolation(): Failed to send set param for applying vol level"
               "result: 0x%lx", result);
      }

      break;
   } /* End of for (;;) */

   /* Check if no set param commands issued */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      /* Remove the command response pending status, if set */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      if (ib_payload_address)
      {
         /* Free-up the memory allocated for in-band set param.
            Note that if at least 1 set param succeed, allocated memory is
            freed in response handler */
         qurt_elite_memory_free(ib_payload_address);
      }

      if (ADSP_EOK == result)
      {
         result = ADSP_EFAILED;
      }
   }

   return result;
}

/* Applies volume calibration data to VPM , using the rsp_fn provided by the
 * caller for handling the individual set_param response.
 */
ADSPResult vpm_calibrate_volume(vpm_session_object_t *session_obj,
                                uint32_t vol_ramp_duration)
{
   ADSPResult  result = ADSP_EOK;
   int32_t     rc;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_volume(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   for (;;)
   {
      if (FALSE == session_obj->volume_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_calibrate_volume(): Calibration data is not registered,"
             " Not calibrating");

         result = ADSP_EFAILED;
         break;
      }

      if (session_obj->target_set.client_num_vol_steps ==
             session_obj->volume_cal.num_vol_indices)
      {
         /* Linear volume mapping. Push volume calibration data and volume level
          * in Q28 format to VPM.
          */
         result = vpm_calibrate_volume_linear_mapping(session_obj,
                                                      vol_ramp_duration);
      }
      else if (session_obj->target_set.client_num_vol_steps >
                  session_obj->volume_cal.num_vol_indices)
      {
         if (TRUE == session_obj->volume_cal.is_v1_format)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_calibrate_volume(): Interpolation is not supported "\
                   "for format V1");

            result = ADSP_EBADPARAM;
            break;
         }

         /* Interpolation is required. Push volume calibration data and volume
          * level in Q28 format to VPM.
          */
         result = vpm_calibrate_volume_interpolation(session_obj,
                                                     vol_ramp_duration);
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_volume(): Unexpected volume steps, client "\
                  "volume steps %d, cal table volume steps %d",
               session_obj->target_set.client_num_vol_steps,
               session_obj->volume_cal.num_vol_indices);

         result = ADSP_EUNEXPECTED;
      }
      break;
   } /* end of for(;;)*/

   if (ADSP_EOK == result)
   {
      /* Commands have been sent to FW. Update the active set. */
      session_obj->active_set.vol_step = session_obj->target_set.vol_step;
      session_obj->active_set.client_num_vol_steps = session_obj->target_set.client_num_vol_steps;
   }

   return result;
}

/* Applies common calibration data to VPM using the rsp_fn provided by the
 * caller for handling the individual set_param response.
 */
ADSPResult vpm_calibrate_common(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EFAILED;
   ADSPResult           set_param_result = ADSP_EFAILED;
   cvd_cal_column_t     columns[VPM_COMMON_NUM_CAL_COLUMNS];
   cvd_cal_key_t        cal_key;
   uint32_t             set_param_cnt;
   uint32_t             payload_size;
   void                 *payload_address;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_common(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   for (;;)
   {
      session_obj->common_cal.num_matching_entries = 0;

      if (FALSE == session_obj->common_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_calibrate_common(): Calibration data is not registered, Not calibrating");

         result = ADSP_EFAILED;
         break;
      }

      memset(columns, 0, sizeof(columns));

      columns[0].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
      columns[0].value = session_obj->target_set.system_config.network_id;

      switch (session_obj->direction)
      {
         case VSS_IVOCPROC_DIRECTION_RX:
         {
            columns[1].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            columns[1].value = session_obj->target_set.system_config.rx_pp_sr;
            cal_key.num_columns = 2;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_TX:
         {
            columns[1].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            columns[1].value = session_obj->target_set.system_config.tx_pp_sr;
            cal_key.num_columns = 2;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_RX_TX:
         {
            columns[1].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            columns[2].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            columns[1].value = session_obj->target_set.system_config.rx_pp_sr;
            columns[2].value = session_obj->target_set.system_config.tx_pp_sr;
            cal_key.num_columns = 3;
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_common(): Incorrect vpm direction: %lu",
                  session_obj->direction);

            result = ADSP_EBADPARAM;
            break;
         }
      } /* End of switch (session_obj->direction) */

      cal_key.columns = columns;

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_common(): Network 0x%08X, Rx SR %d, Tx SR %d",
            columns[0].value, columns[1].value, columns[2].value);

      result = cvd_cal_query_table(session_obj->common_cal.table_handle, &cal_key,
                                   sizeof(session_obj->common_cal.matching_entries),
                                   session_obj->common_cal.matching_entries,
                                   &session_obj->common_cal.num_matching_entries);
      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_common(): cvd_cal_query_table failed, "
               "result: 0x%08X. Not calibrating.", result);

         break;
      }

      if (0 == session_obj->common_cal.num_matching_entries)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_calibrate_common(): Cannot find a matching entry. "\
                "Not calibrating.");

         result = ADSP_ECONTINUE;
         break;
      }

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_calibrate_common(): Found %d matched entries",
            session_obj->common_cal.num_matching_entries);

      /* Set the command response status to pending */
      vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

      /* Apply calibration data. */
      for (set_param_cnt = 0; set_param_cnt < session_obj->common_cal.num_matching_entries; ++set_param_cnt)
      {
         payload_address = (void *)session_obj->common_cal.matching_entries[set_param_cnt].start_ptr;
         payload_size = session_obj->common_cal.matching_entries[set_param_cnt].size;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   payload_address,
                                                                   payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_common(): "
                  "Failed to send set param for matched entry cnt: %lu, result: 0x%lx",
                  set_param_cnt, result);
         }
         else
         {
            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_calibrate_common(): payload_address: 0x%08X, payload_size: %lu, memory handle: 0x%lx",
                  payload_address, payload_size, session_obj->common_cal.vpm_mem_handle);
         }

      } /* End of for (set_param_cnt) */

      /* If none of the set params passed */
      if (!session_obj->session_rsp_ctrl.num_cmd_issued)
      {
         if (ADSP_EOK == result)
         {
            result = ADSP_EFAILED;
         }

         /* Remove command response status to pending */
         vpm_set_cmd_rsp_pending_status(session_obj, FALSE);
      }

      break;
   } /* End of for (;:)*/

   return result;
}

static ADSPResult vpm_calibrate_dynamic_init_query(
  vpm_session_object_t *session_obj,
  uint32_t vol_index,
  uint32_t* query_handle
)
{
  cvd_cal_key_t    cal_key;
  cvd_cal_column_t *query_key_columns;
  uint32_t         column_index = 0;
  ADSPResult result = ADSP_EOK;

  if ( !session_obj )
  {
    MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
        "vpm_calibrate_dynamic_init_query(): Session obj pointer is NULL" );

    return ADSP_EFAILED;
  }

  query_key_columns = session_obj->dynamic_cal.query_key_columns;

  memset( query_key_columns, 0, sizeof( session_obj->dynamic_cal.query_key_columns ) );

  query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
  query_key_columns[ column_index ].value = session_obj->target_set.system_config.network_id;
  column_index += 1;

  query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX;
  query_key_columns[ column_index ].value = vol_index;
  column_index += 1;

  switch ( session_obj->direction )
  {
    case VSS_IVOCPROC_DIRECTION_TX:
    {
      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.tx_pp_sr;
      column_index += 1;

      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.tx_voc_op_mode;
      column_index += 1;
      break;
    }

    case VSS_IVOCPROC_DIRECTION_RX:
    {
      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.rx_pp_sr;
      column_index += 1;

      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.rx_voc_op_mode;
      column_index += 1;
      break;
    }

    case VSS_IVOCPROC_DIRECTION_RX_TX:
    {
      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.tx_pp_sr;
      column_index += 1;

      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.rx_pp_sr;
      column_index += 1;

      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.tx_voc_op_mode;
      column_index += 1;

      query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
      query_key_columns[ column_index ].value = session_obj->target_set.system_config.rx_voc_op_mode;
      column_index += 1;
      break;
    }

    default:
    {
      MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_dynamic_init_query(): Incorrect vpm direction: %lu",
             session_obj->direction );

      return ADSP_EFAILED;
    }
  }

  query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_MEDIA_ID;
  query_key_columns[ column_index ].value = session_obj->target_set.system_config.media_id;
  column_index += 1;

  query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_FEATURE;
  query_key_columns[ column_index ].value = session_obj->target_set.system_config.feature;

  cal_key.columns = query_key_columns;
  cal_key.num_columns = ( column_index + 1 );

  MSG_3( MSG_SSID_DFLT, DBG_MED_PRIO,
        "CVD_CAL_MSG: VPM Dynamic configured with the following settings."
        " vpm_calibrate_dynamic_init_query(): Network 0x%08X, "\
        "Tx PP SR %d, Rx PP SR %d",
        session_obj->target_set.system_config.network_id,
        session_obj->target_set.system_config.tx_pp_sr,
        session_obj->target_set.system_config.rx_pp_sr );

  MSG_3( MSG_SSID_DFLT, DBG_MED_PRIO,
        "CVD_CAL_MSG: VPM Dynamic configured with the following settings"
        " vpm_calibrate_dynamic_init_query(): Volume index %d, "\
        "Tx voc op mode 0x%08X, Rx voc op mode 0x%08X",
        session_obj->target_set.vol_step,
        session_obj->target_set.system_config.tx_voc_op_mode,
        session_obj->target_set.system_config.rx_voc_op_mode );

  MSG_2( MSG_SSID_DFLT, DBG_MED_PRIO,
        "CVD_CAL_MSG: VPM Dynamic configured with the following settings."
        " vpm_calibrate_dynamic_init_query(): Media ID 0x%08X, Feature 0x%08X.",
        session_obj->target_set.system_config.media_id,
        session_obj->target_set.system_config.feature );

  result = cvd_cal_query_init( session_obj->dynamic_cal.table_handle, &cal_key,
                               session_obj->dynamic_cal.matching_entries,
                               sizeof( session_obj->dynamic_cal.matching_entries ),
                               query_handle );

  if ( ADSP_EOK != result )
  {
    MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
           "vpm_calibrate_dynamic_init_query(): cvd_cal_query_table_init failed, "\
           "result: 0x%08X. Not calibrating.", result );
  }

  return result;
}

/* Find a module id /parameter id pair from cal entries. */
static ADSPResult vpm_find_mid_pid_pair_from_cal_entry(cvd_cal_table_descriptor_t *table_descriptor,
                                                       void *data, uint32_t data_len,
                                                       uint32_t mid, uint32_t pid,
                                                       void **ret_data_addr,
                                                       uint32_t *ret_data_len)
{
   ADSPResult           result = ADSP_EFAILED;
   int32_t              rc;
   bool_t               is_found = FALSE;
   uint32_t             consumed_len = 0;
   voice_param_data_t   *cur_param;
   uint8_t              *cal_data;

   if ((!ret_data_addr) || (!ret_data_len))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry(): Invalid input parameters");
      return APR_EBADPARAM;
   }

   /* Initial validation */
   rc = cvd_cal_validate_entry(table_descriptor, data, data_len);

   if (rc != APR_EOK)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry(): Invalid table");

      return ADSP_EFAILED;
   }

   cal_data = (uint8_t *)data;
   while (consumed_len < data_len)
   {
      if ((data_len - consumed_len) < sizeof(voice_param_data_t))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry(): Improperly formatted cal data, "\
                "not enough bytes to form voice_param_data_t");
         rc = APR_EFAILED;
         break;
      }

      cur_param = ((voice_param_data_t *)(cal_data + consumed_len));

      /* Validate param size */
      rc = cvd_cal_validate_entry(table_descriptor, data,
                                  consumed_len + cur_param->param_size + sizeof(voice_param_data_t));
      if (rc != APR_EOK)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry(): Invalid table");
         break;
      }

      consumed_len += (sizeof(voice_param_data_t));

      if ((cur_param->module_id == mid) && (cur_param->param_id == pid))
      {
         is_found = TRUE;
         *ret_data_addr = (void *)cur_param;
         *ret_data_len = cur_param->param_size + sizeof(voice_param_data_t);
         result = ADSP_EOK;

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_find_mid_pid_pair_from_cal_entry(): MID/PID "\
                  "0x%08X / 0x%08X pair found in calibration entry.",
               mid, pid);

         break;
      }

      consumed_len += cur_param->param_size;
   } /* End of while (consumed_len < data_len) */

   if (is_found == FALSE)
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_find_mid_pid_pair_from_cal_entry(): No module id/parameter id "\
               "0X%08X/0X%08X pair found in calibration entry.",
            mid, pid);

      result = ADSP_EFAILED;
   }

   return result;
}

static ADSPResult vpm_find_mid_pid_pair_from_cal_entry_v3(cvd_cal_table_descriptor_t *table_descriptor,
                                                          void *data, uint32_t data_len,
                                                          uint32_t mid, uint32_t pid,
                                                          void **ret_data_addr,
                                                          uint32_t *ret_data_len)
{
   ADSPResult            result = ADSP_EFAILED;
   int32_t               rc;
   bool_t                is_found = FALSE;
   uint32_t              consumed_len = 0;
   voice_param_data_v2_t *cur_param;
   uint8_t               *cal_data;


   if ((!ret_data_addr) || (!ret_data_len))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry_v3(): Invalid input parameters");
      return APR_EBADPARAM;
   }
   
   /* Initial validation */
   rc = cvd_cal_validate_entry(table_descriptor, data, data_len);

   if (rc != APR_EOK)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry_v3(): Invalid table");

      return ADSP_EFAILED;
   }

   cal_data = (uint8_t *)data;
   while (consumed_len < data_len)
   {
      if ((data_len - consumed_len) < sizeof(voice_param_data_v2_t))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry_v3(): Improperly formatted cal data, "\
                "not enough bytes to form voice_param_data_v2_t");
         rc = APR_EFAILED;
         break;
      }
      cur_param = ((voice_param_data_v2_t*)(cal_data + consumed_len));
      
      /* Validate param size */
      rc = cvd_cal_validate_entry(table_descriptor, data,
                                  consumed_len + cur_param->param_size + sizeof(voice_param_data_v2_t));
      if (rc != APR_EOK)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_find_mid_pid_pair_from_cal_entry(): Invalid table");
         break;
      }

      consumed_len += sizeof(voice_param_data_v2_t);

      if ((cur_param->module_id == mid) && (cur_param->param_id == pid))
      {
         is_found = TRUE;
         *ret_data_addr = (void *)cur_param;
         *ret_data_len = cur_param->param_size + sizeof(voice_param_data_v2_t);
         result = ADSP_EOK;

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_find_mid_pid_pair_from_cal_entry_v3(): MID/PID "\
                  "0x%08X / 0x%08X pair found in calibration entry.",
               mid, pid);

         break;
      }

      consumed_len += cur_param->param_size;
   } /* End of while (consumed_len < data_len) */

   if (is_found == FALSE)
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_find_mid_pid_pair_from_cal_entry_v3(): No module id/parameter id "\
               "0X%08X/0X%08X pair found in calibration entry.",
            mid, pid);

      result = ADSP_EFAILED;
   }

   return result;
}

/* Find module id and paramter id pair from common calibration data. */
static ADSPResult vpm_find_common_mid_pid_pair(vpm_session_object_t *session_obj,
                                               uint32_t mid, uint32_t pid,
                                               void **ret_data_addr,
                                               uint32_t *ret_data_len)
{
   ADSPResult  result = ADSP_EFAILED;
   uint32_t    idx;
   void        *data;
   uint32_t    data_len;
   bool_t      is_found = FALSE;

   cvd_cal_entry_t *cal_entry;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_common_mid_pid_pair(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Assumption: matched entries have been found in network calibration procedure. */
   cal_entry = session_obj->common_cal.matching_entries;

   for (idx = 0; idx < session_obj->common_cal.num_matching_entries; ++idx)
   {
      data = cal_entry->start_ptr;
      data_len = cal_entry->size;

      if (ADSP_EOK == (result = vpm_find_mid_pid_pair_from_cal_entry(&session_obj->common_cal.table_handle->table_descriptor,
                                                                     data, data_len,
                                                                     mid, pid,
                                                                     &data, &data_len)))
      {
         /* Found pair, exit. */
         is_found = TRUE;
         *ret_data_addr = data;
         *ret_data_len = data_len;

         result = ADSP_EOK;

         break;
      }

      cal_entry++;
   }

   return result;
}


void vpm_volume_level_query_cb_fn(void *cal_param_payload,
                                  void *cb_data)
{
   vpm_vol_level_info_t *vol_level_info;
   uint16_t instance_id = 0;
   cvd_cal_param_v1_t *cal_param;
        
   cal_param = (cvd_cal_param_v1_t *)( (int8_t*)cal_param_payload );
   if ( CVD_CAL_PARAM_MINOR_VERSION_0 == *( uint32_t *)cal_param_payload )
    {
     instance_id = 0;
    }
    else
    {
     instance_id = cal_param->instance_id;
    }
   for (;;)
   {
      if ((NULL == cal_param) || (NULL == cb_data))
      {
         break;
      }

      vol_level_info = ((vpm_vol_level_info_t *)cb_data);

      if (((cal_param->module_id == VSS_MODULE_RX_VOLUME) &&
           (cal_param->param_id == VSS_PARAM_VOLUME)) ||
          ((cal_param->module_id == VOICE_MODULE_RX_VOL) &&
           (cal_param->param_id == VOICE_PARAM_VOL)))
      {
         vol_level_info->is_vol_level_found = TRUE;
         vol_level_info->vol_level = *((uint32_t *)(cal_param->param_data));
         vol_level_info->instance_id = instance_id;
      }
      break;
   }
}

static ADSPResult vpm_calibrate_dynamic_cal_handle_usecase(
  vpm_session_object_t *session_obj,
  vpm_dynamic_cal_usecase_enum_t usecase
)
{

  if ( !session_obj )
  {
    MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
         "vpm_calibrate_dynamic_cal_handle_usecase(): Session obj pointer is NULL" );

    return ADSP_EFAILED;
  }

  switch ( usecase )
  {
    case VPM_DYNAMIC_CAL_USECASE_GAIN:
    {
      ( void )cvd_cal_utility_remove_na_matching_entries(
                session_obj->dynamic_cal.query_handle, VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX );
    }
    break;

    case VPM_DYNAMIC_CAL_USECASE_GENERIC:
    case VPM_DYNAMIC_CAL_USECASE_VOCODER:
    {
      /* At present this is a place holder for other usecases.
         No handling is supported for other use cases as of now.
      */
    }
    break;
  }
  return ADSP_EOK;
}

/* Applies dynamic calibration data to VPM with linear volume mapping use case,
 * using the rsp_fn provided by the caller for handling the individual
 * set_param response.
 */
static ADSPResult vpm_calibrate_dynamic_with_linear_volume_mapping(vpm_session_object_t *session_obj,
                                                                   uint32_t vol_ramp_duration,
                                                                   vpm_dynamic_cal_usecase_enum_t usecase)
{
   ADSPResult                       result = ADSP_EOK;
   voice_cmd_set_param_v3_t         set_param;
   cvd_cal_key_t                    cal_key;
   cvd_cal_column_t                 *query_key_columns;
   uint32_t                         column_index = 0;
   vpm_vol_param_v2_t               *p_vol_set_param_payload;
   vpm_vol_level_info_t             vol_level_info;
   uint32_t                         vol_level_linearQ28 = 0;
   cvd_cal_log_commit_info_t        log_info;
   cvd_cal_log_cal_data_header_t    log_info_data;
   void                             *payload_address;
   void                             *ib_payload_address = NULL;
   uint32_t                         payload_size;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_dynamic_with_linear_volume_mapping(): "\
             "Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Set the command response status to pending */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   for (;;)
   {
      if (FALSE == session_obj->dynamic_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_calibrate_dynamic_with_linear_volume_mapping(): "\
                "Dynamic calibration data is not registered. Not calibrating.");

         result = ADSP_EFAILED;
         break;
      }

      result = vpm_calibrate_dynamic_init_query( session_obj,
                 session_obj->target_set.vol_step, &session_obj->dynamic_cal.query_handle );
      if (ADSP_EOK != result)
      {
        MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_linear_volume_mapping(): cvd_cal_query_table_init failed, "\
               "result: 0x%08X. Not calibrating.", result);

         break;
      }

      {
        /* Handle any usecase dependent calibration. */
        ( void )vpm_calibrate_dynamic_cal_handle_usecase( session_obj, usecase );
      }

      vol_level_info.is_vol_level_found = FALSE;

      /* Determine the volume level from matching calibration entries. */
      result = cvd_cal_query(session_obj->dynamic_cal.query_handle,
                             vpm_volume_level_query_cb_fn,
                             ((void *)&vol_level_info));

      if (ADSP_EOK != result)
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_calibrate_dynamic_with_linear_volume_mapping(): cvd_cal_query "
               "failed with result: 0x%08X for Handle: %d", 
               result,session_obj->dynamic_cal.query_handle);
         cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
         session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
         break;
      }

      if (TRUE == vol_level_info.is_vol_level_found)
      {
         /* Calculate volume level in linear Q28 format and set volume level to FW. */
         if (TRUE == session_obj->dynamic_cal.is_v1_vol_format)
         {
            /* Convert from linearQ13 (32 bit) to linearQ28. */
            vol_level_linearQ28 = (vol_level_info.vol_level << 15);
         }
         else
         {
            /* Convert from mb to linearQ28. */
            vol_level_linearQ28 = vpm_convert_mB_to_linearQ28(vol_level_info.vol_level);
         }
      }

      /* Send the calibration query handle to VPM for retrieving and applying
       * all matching entries. */
      if (ADSP_EOK != (result = vpm_set_param_v3_to_dyn_svc(session_obj,
                                                            session_obj->dynamic_cal.query_handle)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_linear_volume_mapping(): Failed to send set param v3 cmd to dyn svcs,"
               " result: 0x%lx", result);
      }

      if (TRUE == vol_level_info.is_vol_level_found)
      {
         /* Allocate payload for in-band set param */
         if (NULL == (ib_payload_address = qurt_elite_memory_malloc(sizeof(vpm_vol_param_v2_t),
                                                                    QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_calibrate_dynamic_with_linear_volume_mapping(): Failed to allocate set param payload for Rx"
                "soft volume calib");

            result = ADSP_ENOMEMORY;
            break;
         }

         /* Update the payload pointer in response handler */
         session_obj->session_rsp_ctrl.p_ack_payload = ib_payload_address;

         /* Populate the set param payload */
         p_vol_set_param_payload = (vpm_vol_param_v2_t *)ib_payload_address;

         p_vol_set_param_payload->module_id = VOICE_MODULE_RX_VOL;
         p_vol_set_param_payload->instance_id = vol_level_info.instance_id;
         p_vol_set_param_payload->param_id = VOICE_PARAM_SOFT_VOL;
         p_vol_set_param_payload->param_size = sizeof(vpm_vol_param_data_t);
         p_vol_set_param_payload->reserved = 0;
         p_vol_set_param_payload->param_data.volume = vol_level_linearQ28;
         p_vol_set_param_payload->param_data.ramp_duration_ms = (uint16_t)vol_ramp_duration;
         p_vol_set_param_payload->param_data.reserved = 0;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   ib_payload_address,
                                                                   sizeof(vpm_vol_param_v2_t),
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_1)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_dynamic_with_linear_volume_mapping(): Failed to send set param v2 to dynamic svcs, "
                  "result: 0x%lx", result);

            /* Free up the allocated payload for in-band set param */
            if (ib_payload_address)
            {
               qurt_elite_memory_free(payload_address);
            }
         }

      } /* end of if (TRUE == vol_level_info.is_vol_level_found) */


      /* Log cal data. */
      log_info_data.table_handle = session_obj->dynamic_cal.table_handle;
      log_info_data.cal_query_handle = session_obj->dynamic_cal.query_handle;
      log_info_data.data_seq_num = 0;

      log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                              (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof(log_info_data);
      log_info.data_container_header = &log_info_data;
      log_info.payload_size = 0;
      log_info.payload_buf = NULL;

      cvd_cal_log_data((log_code_type)LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT,
                       (void *)&log_info, sizeof(log_info));

      break;
   } /* End of for (;;) */

   /* Check if all the set param commands failed */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      /* Clear the command response pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      if (ADSP_EOK == result)
      {
         result = ADSP_EFAILED;
      }
   }

   return result;
}

/* Find volume level corresponding to a volume index from dynamic cal table. */
static ADSPResult vpm_find_vol_level_from_dynamic_cal(vpm_session_object_t *session_obj,
                                                      uint32_t vol_index,
                                                      uint32_t *ret_vol_level)
{
   ADSPResult                       result = ADSP_EOK;
   int32_t                          rc;
   cvd_cal_key_t                    cal_key;
   cvd_cal_column_t                 *query_key_columns;
   uint32_t                         column_index = 0;
   uint32_t                         cal_query_handle;
   vpm_vol_level_info_t             vol_level_info;
   cvd_cal_log_commit_info_t        log_info;
   cvd_cal_log_cal_data_header_t    log_info_data;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_find_vol_level_from_dynamic_cal(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   for (;;)
   {
      if (FALSE == session_obj->dynamic_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_find_vol_level_from_dynamic_cal(): "\
                "Dynamic calibration data is not registered.");

         return ADSP_EOK;
      }

      rc = vpm_calibrate_dynamic_init_query( session_obj,
             vol_index, &cal_query_handle );
      if (rc)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_find_vol_level_from_dynamic_cal(): cvd_cal_query_table_init failed, "\
                  "rc=0x%08X.", rc);
         break;
      }

      vol_level_info.is_vol_level_found = FALSE;

      /* Determine the volume level from matching calibration entries. */
      rc = cvd_cal_query(cal_query_handle,
                         vpm_volume_level_query_cb_fn,
                         ((void *)&vol_level_info));
      if (rc)
      {
         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_find_vol_level_from_dynamic_cal(): cvd_cal_query failed with rc = 0x%08X",
               rc);

         (void)cvd_cal_query_deinit(cal_query_handle);

         break;
      }

      if (TRUE == vol_level_info.is_vol_level_found)
      {
         *ret_vol_level = vol_level_info.vol_level;
      }

      /* Log cal data. */

      log_info_data.table_handle = session_obj->dynamic_cal.table_handle;
      log_info_data.cal_query_handle = cal_query_handle;
      log_info_data.data_seq_num = 0;

      log_info.instance =  ((session_obj->attached_mvm_handle << 16) |
                               (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof(log_info_data);
      log_info.data_container_header = &log_info_data;
      log_info.payload_buf = NULL;
      log_info.payload_size = 0;

      cvd_cal_log_data( ( log_code_type ) LOG_ADSP_CVD_CAL_DATA_C,
                            CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT,
                            (void *)&log_info,
                            sizeof(log_info));

      (void)cvd_cal_query_deinit(cal_query_handle);

      break;
   }

   return result;
}

/* Find the volume index that indexes to the closest volume level
   (to the target volume level) in dynamic calibration table.
   Caller: vpm_calibrate_dynamic_with_volume_interpolation(). */
static ADSPResult vpm_find_closest_vol_index_from_dynamic_cal(vpm_session_object_t *session_obj,
                                                              uint32_t in_vol_level,
                                                              uint32_t *ret_vol_index)
{
   ADSPResult  result = ADSP_EOK;
   int32_t     rc;
   uint32_t    vol_index;
   uint32_t    vol_level = 0xFFFFFFFF;
   uint32_t    distance = 0XFFFFFFFF;
   uint32_t    cur_distance;
   bool_t      is_found = FALSE;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_find_closest_vol_index_from_dynamic_cal(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   if (!ret_vol_index)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_find_closest_vol_index_from_dynamic_cal(): Invalid input parameters");

      return ADSP_EBADPARAM;
   }

   /* Note: Double closed region ([min, max]). */
   for (vol_index = session_obj->dynamic_cal.min_vol_index;\
           vol_index <= session_obj->dynamic_cal.max_vol_index;\
           ++vol_index)
   {
      if (ADSP_EOK != (result = vpm_find_vol_level_from_dynamic_cal(session_obj,
                                                                    vol_index,
                                                                    &vol_level)))
      {
         /* Not found, keep searching. */
         continue;
      }

      cur_distance = vpm_abs(in_vol_level - vol_level);

      if (distance > cur_distance)
      {
         /* Current distance is decreasing, keep searching. */
         distance = cur_distance;
      }
      else
      {
         /* Current distance is increasing, exit. */
         /* Assumption: volume level increases as volume index increases. */
         is_found = TRUE;
         *ret_vol_index = (vol_index - 1); /* Assumption: vol index is incremented by 1. */
         result = ADSP_EOK;
         break;
      }
   }

   if (FALSE == is_found)
   {
      /* No volume level has been found. */
      result = ADSP_EFAILED;
   }

   return result;
}

/* Applies dynamic calibration data to VPM for non-linear volume mapping use
 * case (interpolation is required), using the rsp_fn provided by the caller
 * for handling the individual set_param response.
 */
static ADSPResult vpm_calibrate_dynamic_with_volume_interpolation(vpm_session_object_t *session_obj,
                                                                  uint32_t vol_ramp_duration,
                                                                  vpm_dynamic_cal_usecase_enum_t usecase)
{
   ADSPResult                       result = ADSP_EOK;
   voice_cmd_set_param_v3_t         set_param;
   cvd_cal_key_t                    cal_key;
   cvd_cal_column_t                 *query_key_columns;
   uint32_t                         column_index = 0;
   vpm_vol_param_v2_t               *p_vol_set_param_payload;
   uint32_t                         vol_level_linearQ28 = 0;
   uint32_t                         min_vol_level = 0;
   uint32_t                         max_vol_level = 0;
   uint32_t                         vol_index;
   cvd_cal_log_commit_info_t        log_info;
   cvd_cal_log_cal_data_header_t    log_info_data;
   void                             *payload_address;
   void                             *ib_payload_address = NULL;
   uint32_t                         payload_size;
   uint32_t                         interpolated_vol_level;
   vpm_vol_level_info_t             vol_level_info;
   uint32_t                         minor_version;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_calibrate_dynamic_with_volume_interpolation(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Set the command response status to pending */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   for (;;)
   {

      /* Find min and max volume level for a specific network ID. */
      /* Assumption: volume level increases as volume index increase. */

      if (ADSP_EOK != (result = vpm_find_vol_level_from_dynamic_cal(session_obj,
                                                                    session_obj->dynamic_cal.min_vol_index,
                                                                    &min_vol_level)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_dynamic_with_volume_interpolation(): Cannot find min volume "\
                "level. Not calibrating");
         break;
      }

      if (ADSP_EOK != (result  = vpm_find_vol_level_from_dynamic_cal(session_obj,
                                                                     session_obj->dynamic_cal.max_vol_index,
                                                                     &max_vol_level)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_dynamic_with_volume_interpolation(): Cannot find max volume level. "\
                "Not calibrating");
         break;
      }

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_calibrate_dynamic_with_volume_interpolation(): min_vol_level = %d, max_vol_level = %d",
            min_vol_level, max_vol_level);

      /* Interpolate target volume step into target volume level. */
      /* Input: client specified volume step, number of total volume steps, min and max volume level. */
      /* Output: interpolated volume level in mB. */
      interpolated_vol_level = vpm_convert_volume_step_to_mB(min_vol_level,
                                                             max_vol_level,
                                                             session_obj->target_set.client_num_vol_steps,
                                                             session_obj->target_set.vol_step);

      /* Find the closest volume level in table, as well as the corresponding volume index. */
      /* Input: interpolated volume level in mB. */
      /* Output: closest volume level in table, and the corresponding volume index. */
      /* Assumption: volume level increases as volume index increase. */
      if (ADSP_EOK != (result = vpm_find_closest_vol_index_from_dynamic_cal(session_obj,
                                                                            interpolated_vol_level,
                                                                            &vol_index)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_volume_interpolation(): Cannot find closest volume level to "\
                  "target volume level %d. Not calibrating",
               interpolated_vol_level);
         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_calibrate_dynamic_with_volume_interpolation(): interpolated_vol_level = %d, "\
               "vol_index = %d", interpolated_vol_level, vol_index);
      }

      /* Find the calibration data of the closest volume level. */
      result = vpm_calibrate_dynamic_init_query( session_obj, vol_index,
                                                 &session_obj->dynamic_cal.query_handle );
      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_volume_interpolation(): cvd_cal_query_table_init failed, "\
               "result: 0x%08X. Not calibrating.", result);
         break;
      }

      {
        /* Handle any usecase dependent calibration. */
        ( void )vpm_calibrate_dynamic_cal_handle_usecase( session_obj, usecase );
      }

      /* Send the calibration query handle to VPM for retrieving and applying
       * all matching entries.
       */

      /* Send the calibration query handle to VPM for retrieving and applying
       * all matching entries. */
      if (ADSP_EOK != (result = (vpm_set_param_v3_to_dyn_svc(session_obj,
                                                             session_obj->dynamic_cal.query_handle))))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_volume_interpolation(): Failed to send set param v3 cmd"
               " to dyn svcs, result: 0x%lx", result);
      }

      /* Allocate payload for in-band set param */
      if (NULL == (ib_payload_address = qurt_elite_memory_malloc(sizeof(vpm_vol_param_t),
                                                                 QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_calibrate_dynamic_with_volume_interpolation(): Failed to allocate set param payload for Rx"
             "soft volume calib");

         result = ADSP_ENOMEMORY;
         break;
      }
      
      minor_version =0;
      mmstd_memset(&vol_level_info, 0, sizeof(vol_level_info));
      result = cvd_cal_get_minor_version ( session_obj->dynamic_cal.query_handle, &minor_version );
      if ( ( ADSP_EOK == result ) && ( CVD_CAL_PARAM_MINOR_VERSION_1 == minor_version ) )
      {
         result = cvd_cal_query(session_obj->dynamic_cal.query_handle,
                                vpm_volume_level_query_cb_fn,
                                ((void *)&vol_level_info));

         if (ADSP_EOK != result)
         {
            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_calibrate_dynamic_with_volume_interpolation(): cvd_cal_query "
                  "failed with result: 0x%08X for Handle: %d", 
                  result, session_obj->dynamic_cal.query_handle);

            cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
            session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;

            break;
         }
      }

      /* Update the payload pointer in response handler */
      session_obj->session_rsp_ctrl.p_ack_payload = ib_payload_address;

      /* Populate the set param payload */
      p_vol_set_param_payload = (vpm_vol_param_v2_t *)ib_payload_address;

      /* Convert interpolated volume level in mB to Q28 format. */
      vol_level_linearQ28 = vpm_convert_mB_to_linearQ28(interpolated_vol_level);

      p_vol_set_param_payload->module_id = VOICE_MODULE_RX_VOL;
      p_vol_set_param_payload->instance_id = vol_level_info.instance_id;
      p_vol_set_param_payload->reserved = 0;
      p_vol_set_param_payload->param_id = VOICE_PARAM_SOFT_VOL;
      p_vol_set_param_payload->param_size = sizeof(vpm_vol_param_data_t);
      p_vol_set_param_payload->param_data.volume = vol_level_linearQ28;
      p_vol_set_param_payload->param_data.ramp_duration_ms = (uint16_t)vol_ramp_duration;
      p_vol_set_param_payload->param_data.reserved = 0;

      if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                ib_payload_address,
                                                                sizeof(vpm_vol_param_t),
                                                                ELITE_CMD_SET_PARAM,
                                                                CVD_CAL_PARAM_MINOR_VERSION_1)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic_with_volume_interpolation(): Failed to send set param cmd"
               "to dynamic svcs, result: 0x%lx", result);
      }

      /* Log cal data. */
      log_info_data.table_handle = session_obj->dynamic_cal.table_handle;
      log_info_data.cal_query_handle = session_obj->dynamic_cal.query_handle;
      log_info_data.data_seq_num = 0;

      log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                              (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof(log_info_data);
      log_info.data_container_header = &log_info_data;
      log_info.payload_size = 0;
      log_info.payload_buf = NULL;

      cvd_cal_log_data((log_code_type)LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT,
                             (void *)&log_info, sizeof(log_info));


      break;
   } /* for (;;) */

   /* If all the set param commands failed */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      /* Clear the command response status to pending */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      /* Free-up the memory allocated for in-band set param.
         Note that if at least 1 set param succeed, allocated memory is
         freed in response handler */
      if (ib_payload_address)
      {
         qurt_elite_memory_free(ib_payload_address);
      }

      if (ADSP_EOK == result)
      {
         result = ADSP_EFAILED;
      }
   }

   return result;
}

/* Applies dynamic calibration data to VPM, using the rsp_fn provided by the
 * caller for handling the individual set_param response. The dynamic
 * calibration data contains the volume calibration.
 */
ADSPResult vpm_calibrate_dynamic(vpm_session_object_t *session_obj,
                                 uint32_t vol_ramp_duration,
                                 vpm_dynamic_cal_usecase_enum_t usecase)
{
   ADSPResult result = ADSP_EOK;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_calibrate_dynamic(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   for (;;)
   {
      if (FALSE == session_obj->dynamic_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_calibrate_dynamic(): Calibration data is not registered. "\
                "Not calibrating");

         result = ADSP_EFAILED;
         break;
      }

      if (session_obj->target_set.client_num_vol_steps ==
             session_obj->dynamic_cal.num_vol_indices)
      {
         /* Linear volume mapping. Push dynamic calibration data and volume level
          * in Q28 format to VPM.
          */
         result = vpm_calibrate_dynamic_with_linear_volume_mapping(session_obj,
                                                                   vol_ramp_duration, usecase);
      }
      else if (session_obj->target_set.client_num_vol_steps >
                  session_obj->dynamic_cal.num_vol_indices)
      {
         if (TRUE == session_obj->dynamic_cal.is_v1_vol_format)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_calibrate_dynamic(): Interpolation is not supported "\
                   "for format V1");

            result = ADSP_EBADPARAM;
            break;
         }

         /* Interpolation is required. Push dynamic calibration data and volume
          * level in Q28 format to VPM.
          */
         result = vpm_calibrate_dynamic_with_volume_interpolation(session_obj,
                                                                  vol_ramp_duration, usecase);
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_dynamic(): Unexpected volume steps, client "\
                  "volume steps %d, cal table volume steps %d",
               session_obj->target_set.client_num_vol_steps,
               session_obj->volume_cal.num_vol_indices);

         result = ADSP_EUNEXPECTED;
      }

      break;
   } /* End of for (;;) */

   if (ADSP_EOK == result)
   {
      /* Commands have been sent to FW. Update the active set. */
      session_obj->active_set.vol_step = session_obj->target_set.vol_step;
      session_obj->active_set.client_num_vol_steps = session_obj->target_set.client_num_vol_steps;
   }

   return result;
}



ADSPResult vpm_calibrate_static(vpm_session_object_t *session_obj)
{
   ADSPResult                       result = ADSP_EOK;
   int32_t                          rc;
   voice_cmd_set_param_v3_t         set_param;
   cvd_cal_key_t                    cal_key;
   cvd_cal_column_t                 *query_key_columns;
   uint32_t                         column_index = 0;
   cvd_cal_log_commit_info_t        log_info;
   cvd_cal_log_cal_data_header_t    log_info_data;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_calibrate_static(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   for (;;)
   {
      if (FALSE == session_obj->static_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_calibrate_static(): Static calibration data is not registered. "\
                "Not calibrating.");

         result = ADSP_EFAILED;
         break;
      }

      query_key_columns = session_obj->static_cal.query_key_columns;

      memset(query_key_columns, 0, sizeof(session_obj->static_cal.query_key_columns));

      query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
      query_key_columns[column_index].value = session_obj->target_set.system_config.network_id;
      column_index += 1;

      switch (session_obj->direction)
      {
         case VSS_IVOCPROC_DIRECTION_TX:
         {
            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.tx_pp_sr;
            column_index += 1;

            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.tx_voc_op_mode;
            column_index += 1;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_RX:
         {
            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.rx_pp_sr;
            column_index += 1;

            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.rx_voc_op_mode;
            column_index += 1;
            break;
         }
         case VSS_IVOCPROC_DIRECTION_RX_TX:
         {
            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.tx_pp_sr;
            column_index += 1;

            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.rx_pp_sr;
            column_index += 1;

            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.tx_voc_op_mode;
            column_index += 1;

            query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
            query_key_columns[column_index].value = session_obj->target_set.system_config.rx_voc_op_mode;
            column_index += 1;
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_calibrate_static(): Incorrect vpm direction: %lu",
                  session_obj->direction);

            return ADSP_EFAILED;
         }
      }

      query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_MEDIA_ID;
      query_key_columns[column_index].value = session_obj->target_set.system_config.media_id;
      column_index += 1;

      query_key_columns[column_index].id = VSS_ICOMMON_CAL_COLUMN_FEATURE;
      query_key_columns[column_index].value = session_obj->target_set.system_config.feature;

      cal_key.columns = query_key_columns;
      cal_key.num_columns = (column_index + 1);

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG: CVP Static configured with the following settings."
            " vpm_calibrate_static(): Network 0x%08X, Tx PP SR %d, Rx PP SR %d",
            session_obj->target_set.system_config.network_id,
            session_obj->target_set.system_config.tx_pp_sr,
            session_obj->target_set.system_config.rx_pp_sr);

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG: CVP Static configured with the following settings."
            " vpm_calibrate_static(): Tx voc op mode 0x%08X, Rx voc op mode 0x%08X, Media ID 0x%08X",
            session_obj->target_set.system_config.tx_voc_op_mode,
            session_obj->target_set.system_config.rx_voc_op_mode,
            session_obj->target_set.system_config.media_id);

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG: CVP Static configured with the following settings."
            " vpm_calibrate_static(): Feature 0x%08X.",
            session_obj->target_set.system_config.feature);

      result = cvd_cal_query_init(session_obj->static_cal.table_handle, &cal_key,
                                  session_obj->static_cal.matching_entries,
                                  sizeof(session_obj->static_cal.matching_entries),
                                  &session_obj->static_cal.query_handle);
      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_static(): cvd_cal_query_table_init failed, "\
                  "result: 0x%08X. Not calibrating.", result);

         break;
      }

      /* Set the command response pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

      /* Send the calibration query handle to VPM for walking through and
       * applying all the calibration parameters. */

      if (ADSP_EOK != (result = vpm_set_param_v3_to_dyn_svc(session_obj,
                                                            session_obj->static_cal.query_handle)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_calibrate_static(): Failed to send set param v3 to all dyn svcs, result: 0x%lx", result);

         /* Clear the command response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, FALSE);
         break;
      }

      /* Log cal data. */
      log_info_data.table_handle = session_obj->static_cal.table_handle;
      log_info_data.cal_query_handle = session_obj->static_cal.query_handle;
      log_info_data.data_seq_num = 0;

      log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                              (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof(log_info_data);
      log_info.data_container_header = &log_info_data;
      log_info.payload_size = 0;
      log_info.payload_buf = NULL;

      cvd_cal_log_data((log_code_type)LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_VOCPROC_STATIC_OUTPUT,
                             (void *)&log_info, sizeof(log_info));

      break;
   } /* for (;;) */

   return result;
}


/* Returns APR_EOK when completed.  If an entry was found, it is populated into ret_data_entry */
ADSPResult vpm_helper_search_hdvoice_config(vpm_session_object_t *session_obj,
                                            vss_ihdvoice_cmd_get_config_t *search_values,
                                            vss_ihdvoice_rsp_get_config_t *ret_data_entry)
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    i, j = 0;
   uint32_t    column_id = 0;
   uint32_t    column_type = 0;
   uint32_t    column_size = 0;
   uint32_t    matched_column_value = 0;
   uint64_t    typed_column_value = 0;

   uint32_t    *sys_config_column_value = (uint32_t *)session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head;
   vss_param_cal_column_t *columns = (vss_param_cal_column_t *)session_obj->hdvoice_config_info.hdvoice_config_hdr.columns;

   if ((FALSE == session_obj->is_device_config_registered) ||
       (NULL == sys_config_column_value))
   {
      return ADSP_EOK;
   }

   for (i = 0; i < session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries; i++)
   {
      matched_column_value = 0;

      for (j = 0; j < session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns; j++)
      {
         column_id = columns->id;
         column_type = columns->type;
         column_size = 0;

         switch (column_type)
         {
            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64:
               typed_column_value = *(uint64_t *)sys_config_column_value;
               column_size = sizeof(uint64_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32:
               typed_column_value = *(uint32_t *)sys_config_column_value;
               column_size = sizeof(uint32_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16:
               typed_column_value = *(uint16_t *)sys_config_column_value;
               column_size = sizeof(uint16_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8:
               typed_column_value = *(uint8_t *)sys_config_column_value;
               column_size = sizeof(uint8_t);
               break;

            default:
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_helper_search_hdvoice_config(): Invalid Column "
                     "Type: 0x%08X", column_type);

               result = ADSP_EBADPARAM;
               break;
         } /* switch (column_type) */

         switch (column_id)
         {
            case VSS_ICOMMON_CAL_COLUMN_NETWORK:
               if (search_values->network_id == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Network "
                        "ID=0x%08x", search_values->network_id);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE:
               if (search_values->rx_pp_sr == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Rx Sampling "
                        "Rate ID=0x%08x", search_values->rx_pp_sr);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE:
               if (search_values->tx_pp_sr == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Tx Sampling "
                        "Rate ID=0x%08x", search_values->tx_pp_sr);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_MEDIA_ID:
               if (search_values->media_id == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Media ID=0x%08x",
                        search_values->media_id);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE:
               if (search_values->tx_voc_op_mode == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Tx Voc "
                        "Operating Mode=0x%08x", search_values->tx_voc_op_mode);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE:
               if (search_values->rx_voc_op_mode == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Rx Voc "
                        "Operating Mode=0x%08x", search_values->rx_voc_op_mode);
                  matched_column_value++;
               }
               break;

            case VSS_ICOMMON_CAL_COLUMN_FEATURE:
               if (search_values->feature_id == typed_column_value)
               {
                  MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO,
                        "vpm_helper_search_hdvoice_config(): Matched Feature=0x%08x",
                        search_values->feature_id);
                  matched_column_value++;
               }
               break;

            default:
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_helper_search_hdvoice_config(): Undefined Column "
                     "Value: 0x%08X with Column ID: 0x%08X", typed_column_value,
                     column_id);

               result = ADSP_EBADPARAM;
               break;
         }

         /* Advance Column Pointer to find next ID */
         *(uint8_t **)&columns = (uint8_t *)columns + sizeof(columns->id) + sizeof(columns->type) + column_size;

         /* Advance to next Column Value in Sys Config List */
         *(uint8_t **)&sys_config_column_value = (uint8_t *)sys_config_column_value + column_size;
      }

      /** If a matched entry is found, return entry and exit **/
      if (matched_column_value == session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns)
      {
         if( FALSE == session_obj->hdvoice_config_info.is_custom_bwe_config_registered )
         {
           vss_param_hdvoice_config_data_t *matched_entry = 
                            (vss_param_hdvoice_config_data_t*)sys_config_column_value;
         ret_data_entry->enable_mode = matched_entry->enable_mode;
         ret_data_entry->feature_id = matched_entry->feature_id;
           ret_data_entry->rx_pp_sr = matched_entry->rx_sampling_rate;
           ret_data_entry->tx_pp_sr = VSS_PARAM_SAMPLING_RATE_ANY;
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_helper_search_hdvoice_config(): Matched Entry Found");
         }
         else
         {
           vss_param_custom_bwe_config_data_t *matched_entry = 
                           (vss_param_custom_bwe_config_data_t*)sys_config_column_value;
           ret_data_entry->enable_mode = matched_entry->enable_mode;
           ret_data_entry->feature_id = matched_entry->feature_id;
           ret_data_entry->rx_pp_sr = matched_entry->rx_sampling_rate;
           ret_data_entry->tx_pp_sr = matched_entry->tx_sampling_rate;
           MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_helper_search_hdvoice_config(): Matched Entry Found with custom BWE config");
         }

         break;
      }
      if( FALSE == session_obj->hdvoice_config_info.is_custom_bwe_config_registered )
      {
      *(uint8_t **)&sys_config_column_value += sizeof(vss_param_hdvoice_config_data_t);
      }
      else
      {
        *(uint8_t **)&sys_config_column_value += sizeof(vss_param_custom_bwe_config_data_t);
      }
      /* Return Column Pointer to the first Column */
      columns = (vss_param_cal_column_t *)session_obj->hdvoice_config_info.hdvoice_config_hdr.columns;
   }

   return result;
}


ADSPResult vpm_action_set_soundfocus_sectors(vpm_session_object_t *session_obj)
{
   ADSPResult  result = ADSP_EOK;
   int32_t     rc;

   vpm_soundfocus_param_t              *p_sound_focus_param = NULL;
   vss_isoundfocus_cmd_set_sectors_t   *set_sectors;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_set_soundfocus_sectors(): Session obj pointer is NULL");

      return ADSP_EBADPARAM;
   }

   set_sectors = &session_obj->target_set.soundfocus_sectors;

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_soundfocus_sectors(): enables[0-3] = "
         "[%d, %d, %d, %d]", set_sectors->enables[0], set_sectors->enables[1],
         set_sectors->enables[2], set_sectors->enables[3]);

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_soundfocus_sectors(): enables[4-7] = "
         "[%d, %d, %d, %d]", set_sectors->enables[4], set_sectors->enables[5],
         set_sectors->enables[6], set_sectors->enables[7]);

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_soundfocus_sectors(): start_angles[0-3] = "
         "[%d, %d, %d, %d]", set_sectors->start_angles[0], set_sectors->start_angles[1],
         set_sectors->start_angles[2], set_sectors->start_angles[3]);

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_soundfocus_sectors(): start_angles[4-7] = "
         "[%d, %d, %d, %d]", set_sectors->start_angles[4], set_sectors->start_angles[5],
         set_sectors->start_angles[6], set_sectors->start_angles[7]);

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_soundfocus_sectors(): gain_step = %d", set_sectors->gain_step);


   if (NULL == (p_sound_focus_param = (vpm_soundfocus_param_t *)qurt_elite_memory_malloc(sizeof(vpm_soundfocus_param_t),
                                                                                         QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_set_soundfocus_sectors(): Failed to allocate payload for inband set param");

      return ADSP_ENOMEMORY;
   }

   /* Poupulate the set param payload */
   p_sound_focus_param->module_id = VOICEPROC_MODULE_TX;
   p_sound_focus_param->param_id = VOICE_PARAM_FLUENCE_SOUNDFOCUS;
   p_sound_focus_param->param_size = sizeof(vpm_soundfocus_param_data_t);
   p_sound_focus_param->reserved = 0;

   memscpy(p_sound_focus_param->param_data.start_angles,
           sizeof(set_sectors->start_angles),
           set_sectors->start_angles,
           sizeof(set_sectors->start_angles));

   memscpy(p_sound_focus_param->param_data.enables,
           sizeof(set_sectors->enables),
           set_sectors->enables,
           sizeof(set_sectors->enables));

   p_sound_focus_param->param_data.gain_step = set_sectors->gain_step;
   p_sound_focus_param->param_data.reserved = 0;

   /* Set the command response pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   /* Set the ack pointer */
   session_obj->session_rsp_ctrl.p_ack_payload = (void *)p_sound_focus_param;

   /* Send set param command to dynamic services */
   if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                             p_sound_focus_param,
                                                             sizeof(vpm_soundfocus_param_t),
                                                             ELITE_CMD_SET_PARAM,
                                                             CVD_CAL_PARAM_MINOR_VERSION_0)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_action_set_soundfocus_sectors(): "
            "Failed to send set param cmd to dyn svcs, result: 0x%lx", result);

      /* Clear the command response pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      /* Free the allocated payload for ACK */
      if (p_sound_focus_param)
      {
         qurt_elite_memory_free(p_sound_focus_param);
      }
   }

   return result;
}

ADSPResult vpm_action_set_ui_properties(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;
   voice_set_param_v2_t set_param;

   if (0 == session_obj->shared_heap.ui_prop_cache.data_len)
   {
      MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
          "vpm_action_set_ui_properties(): No UI prop in cache, not setting");

      /* Return error code since no set params are issued */
      return ADSP_EFAILED;
   }
   /* Set the cmd rsp pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                             session_obj->shared_heap.ui_prop_cache.data,
                                                             session_obj->shared_heap.ui_prop_cache.data_len,
                                                             ELITE_CMD_SET_PARAM,
                                                             session_obj->shared_heap.ui_prop_cache.version )))
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_set_ui_properties(): Failed to send set param msg to dyn svcs, result: 0x%lx",
            result);

      /* Clear the cmd rsp pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      return result;
   }

   MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_ui_properties(): Sent set param to dyn svcs, addr: 0x%lx, payload_size: %d",
         session_obj->shared_heap.ui_prop_cache.data,
         session_obj->shared_heap.ui_prop_cache.data_len);

   /* return and wait for set param responses */
   return result;
}

ADSPResult vpm_validate_calib_payload(uint32_t mem_handle,
                                      uint32_t mem_size,
                                      uint64_t mem_address)
{
   ADSPResult  result = ADSP_EOK;
   int32_t     rc;

   rc = cvd_mem_mapper_validate_handle(mem_handle);
   if (rc)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_validate_calib_payload(): Invalid "
            "mem_handle: 0x%08X", mem_handle);

      return ADSP_EHANDLE;
   }

   rc = cvd_mem_mapper_validate_attributes_align(mem_handle, mem_address);
   if (rc)
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_validate_calib_payload(): Mis-aligned "
            "mem_address: lsw: 0x%08X msw: 0x%08X", (uint32_t)mem_address,
            (uint32_t)(mem_address >> 32));

      return ADSP_EBADPARAM;
   }

   rc = cvd_mem_mapper_validate_mem_is_in_region(mem_handle, mem_address,
                                                 mem_size);
   if (rc)
   {
      MSG_4(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_validate_calib_payload(): Memory is not within "
            "range, mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d",
            mem_handle, (uint32_t)mem_address,
            (uint32_t)(mem_address >> 32), mem_size);

      result = ADSP_EBADPARAM;
   }

   return result;
}


