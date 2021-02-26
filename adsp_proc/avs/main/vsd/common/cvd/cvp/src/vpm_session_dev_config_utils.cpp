/**========================================================================
 @file vpm_session_dev_config_utils.cpp
 
 @brief This file contains device configuration utilites for session object
 
 Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_dev_config_utils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"
#include "vpm_session_utils.h"
#include "vpm_session_cal_utils.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Function definitions
** ----------------------------------------------------------------------- */


ADSPResult vpm_parse_device_config_wv_mod(vpm_session_object_t *session_obj,
                                          vpm_device_config_entry_t *device_config_entry,
                                          uint32_t *temp_wv_sr)
{
   ADSPResult result = ADSP_EOK;
   vpm_device_config_wv_sr_t *device_config_wv_sr;

   switch (device_config_entry->param_id)
   {
      case VSS_PARAM_RX_SAMPLING_RATE:
      {
         device_config_wv_sr = ((vpm_device_config_wv_sr_t *)device_config_entry);

         if (vpm_sr_is_valid(device_config_wv_sr->rx_sr) == FALSE)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_wv_mod(): "
                  "Invalid widevoice Rx sampling rate, %d",
                  device_config_wv_sr->rx_sr);
            result = ADSP_EBADPARAM;
            break;
         }

         if (session_obj->static_cal.is_registered || session_obj->dynamic_cal.is_registered)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_wv_mod(): "
                "Unable to register Device Config with "
                "WideVoice Config with Static or Dynamic Calibration Tables");
            break;
         }

         *temp_wv_sr = device_config_wv_sr->rx_sr;

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_parse_device_config_wv_mod(): Rx sample rate: %lu", device_config_wv_sr->rx_sr);

         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_wv_mod(): "\
                  "Invalid param_id 0x%08X for mod_id 0x%08x",
               device_config_entry->param_id,
               device_config_entry->module_id);
         break;
      }
   }

   return result;
}

static ADSPResult vpm_verify_sys_config_columns(uint32_t num_columns,
                                                void *columns_head,
                                                uint32_t *column_size)
{
   ADSPResult result = ADSP_EOK;
   uint32_t  i;
   uint32_t  advance_in_bytes = 0;

   vss_param_cal_column_t *columns = ((vss_param_cal_column_t *)columns_head);

   *column_size = 0;

   for (i = 0; i < num_columns; i++)
   {
      advance_in_bytes = 0;

      switch (columns->type)
      {
         case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32:
         {
            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_verify_sys_config_columns(): Verified column type: 0x%08X "\
                     "Array position: %d",
                  columns->type, i);

            advance_in_bytes += sizeof(columns->type);
            advance_in_bytes += sizeof(uint32_t);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8:
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Verified Unexpected "\
                     "column type: 0x%08X Array position: %d",
                  columns->type, i);

            advance_in_bytes += sizeof(columns->type);
            advance_in_bytes += sizeof(uint8_t);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16:
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Verified Unexpected "\
                     "column type: 0x%08X Array position: %d",
                  columns->type, i);

            advance_in_bytes += sizeof(columns->type);
            advance_in_bytes += sizeof(uint16_t);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64:
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Verified Unexpected "\
                     "column type: 0x%08X Array position: %d",
                  columns->type, i);

            advance_in_bytes += sizeof(columns->type);
            advance_in_bytes += sizeof(uint64_t);
            break;
         }
         default:
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined Column type "\
                     "column type: 0x%08X Array position: %d",
                  columns->type, i);

            result = ADSP_EBADPARAM;
            break;
         }
      }

      switch (columns->id)
      {
         /** Expected Columns **/
         case VSS_ICOMMON_CAL_COLUMN_NETWORK:
         {
            if (columns->na_value.uint32_val != VSS_ICOMMON_CAL_NETWORK_ID_NONE)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_NETWORK: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_MEDIA_ID:
         {
            if (columns->na_value.uint32_val != VSS_MEDIA_ID_NONE)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_MEDIA_ID: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE:
         {
            if (columns->na_value.uint32_val != VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
            /** Unexpected Columns **/
         case VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE:
         {
            if ((columns->na_value.uint32_val != 0) ||
                (columns->na_value.uint8_val != 0) ||
                (columns->na_value.uint16_val != 0) ||
                (columns->na_value.uint64_val != 0))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE:
         {
            if ((columns->na_value.uint32_val != 0) ||
                (columns->na_value.uint8_val != 0) ||
                (columns->na_value.uint16_val != 0) ||
                (columns->na_value.uint64_val != 0))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE:
         {
            if (columns->na_value.uint32_val != VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         case VSS_ICOMMON_CAL_COLUMN_FEATURE:
         {
            if (columns->na_value.uint32_val != VSS_ICOMMON_CAL_FEATURE_NONE)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined NA Value "\
                        "for VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE: 0x%08X",
                     columns->na_value.uint32_val);
               result = ADSP_EBADPARAM;
            }
            advance_in_bytes += sizeof(columns->id);
            break;
         }
         default:
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config_columns(): Undefined Column ID "\
                     "column ID: 0x%08X Array position: %d",
                  columns->id, i);
            result = ADSP_EBADPARAM;
            break;
         }
      }

      if (result)
      {
         /* Invalid Column Definitions */
         break; /** From Loop **/
      }
      else
      {
         /* Advance column pointer */
         columns = (vss_param_cal_column_t *)(((uint8_t *)columns) + advance_in_bytes);
         *column_size += advance_in_bytes;
      }
   }

   return result;
}

static ADSPResult vpm_verify_hdvoice_config_data(uint32_t *data_head)
{
   ADSPResult result = ADSP_EOK;

   vss_param_hdvoice_config_data_t *hdvoice_config_data = ((vss_param_hdvoice_config_data_t *)data_head);

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->feature_id)
      {
         case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE:
         case VSS_ICOMMON_CAL_FEATURE_BEAMR:
         case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_hdvoice_config_data(): Invalid Feature ID: 0x%08X",
                  hdvoice_config_data->feature_id);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->enable_mode)
      {
         case VSS_PARAM_FEATURE_DEFAULT_OFF:
         case VSS_PARAM_FEATURE_DEFAULT_ON:
         case VSS_PARAM_FEATURE_FORCED_OFF:
         case VSS_PARAM_FEATURE_FORCED_ON:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_hdvoice_config_data(): Invalid Enable Mode: 0x%08X",
                  hdvoice_config_data->enable_mode);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->rx_sampling_rate)
      {
         case VSS_PARAM_SAMPLING_RATE_16K:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_hdvoice_config_data(): Invalid Sampling Rate: 0x%08X",
                  hdvoice_config_data->rx_sampling_rate);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   return result;
}

static ADSPResult vpm_verify_custom_hdvoice_config_data(uint32_t *data_head)
{
   ADSPResult result = ADSP_EOK;

   vss_param_custom_bwe_config_data_t *hdvoice_config_data = ((vss_param_custom_bwe_config_data_t *)data_head);

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->feature_id)
      {
         case VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE:
         case VSS_ICOMMON_CAL_FEATURE_NONE:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_custom_hdvoice_config_data(): Invalid Feature ID: 0x%08X",
                  hdvoice_config_data->feature_id);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->enable_mode)
      {
         case VSS_PARAM_FEATURE_DEFAULT_OFF:
         case VSS_PARAM_FEATURE_DEFAULT_ON:
         case VSS_PARAM_FEATURE_FORCED_OFF:
         case VSS_PARAM_FEATURE_FORCED_ON:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_custom_hdvoice_config_data(): Invalid Enable Mode: 0x%08X",
                  hdvoice_config_data->enable_mode);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->rx_sampling_rate)
      {
         case VSS_PARAM_SAMPLING_RATE_8K:
         case VSS_PARAM_SAMPLING_RATE_16K:
         case VSS_PARAM_SAMPLING_RATE_32K:
         case VSS_PARAM_SAMPLING_RATE_48K:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_custom_hdvoice_config_data(): Invalid Rx Sampling Rate: 0x%08X",
                  hdvoice_config_data->rx_sampling_rate);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   if (result == ADSP_EOK)
   {
      switch (hdvoice_config_data->tx_sampling_rate)
      {
         case VSS_PARAM_SAMPLING_RATE_8K:
         case VSS_PARAM_SAMPLING_RATE_16K:
         case VSS_PARAM_SAMPLING_RATE_32K:
         case VSS_PARAM_SAMPLING_RATE_48K:
            break;

         default:
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_custom_hdvoice_config_data(): Invalid Tx Sampling Rate: 0x%08X",
                  hdvoice_config_data->tx_sampling_rate);
            result = ADSP_EBADPARAM;
            break;
      }
   }

   return result;
}

static ADSPResult vpm_verify_sys_config(uint32_t num_columns,
                                        void *columns_head,
                                        uint32_t num_sys_configs,
                                        void *sys_config_head,
                                        bool_t is_custom_feature)
{
   ADSPResult result = ADSP_EOK;
   uint32_t i, j = 0;
   uint32_t *sys_config_column_value = ((uint32_t *)sys_config_head);
   vss_param_cal_column_t *columns = ((vss_param_cal_column_t *)columns_head);
   uint32_t column_id = 0;
   uint32_t column_type = 0;
   uint32_t column_size = 0;
   uint64_t typed_column_value = 0;

   for (i = 0; i < num_sys_configs; i++)
   {
      for (j = 0; j < num_columns; j++)
      {
         column_id = columns->id;
         column_type = columns->type;
         column_size = 0;

         switch (column_type)
         {
            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64:
               typed_column_value = (*(uint64_t *)sys_config_column_value);
               column_size = sizeof(uint64_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32:
               typed_column_value = (*(uint32_t *)sys_config_column_value);
               column_size = sizeof(uint32_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16:
               typed_column_value = (*(uint16_t *)sys_config_column_value);
               column_size = sizeof(uint16_t);
               break;

            case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8:
               typed_column_value = (*(uint8_t *)sys_config_column_value);
               column_size = sizeof(uint8_t);
               break;

            default:
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Invalid Column Type: 0x%08X",
                     column_type);
               result = ADSP_EBADPARAM;
               break;
         }

         switch (typed_column_value)
         {
            case VSS_ICOMMON_CAL_NETWORK_ID_NONE:
            case VSS_ICOMMON_CAL_NETWORK_ID_CDMA:
            case VSS_ICOMMON_CAL_NETWORK_ID_GSM:
            case VSS_ICOMMON_CAL_NETWORK_ID_WCDMA:
            case VSS_ICOMMON_CAL_NETWORK_ID_VOIP:
            case VSS_ICOMMON_CAL_NETWORK_ID_LTE:
               if (column_id != VSS_ICOMMON_CAL_COLUMN_NETWORK)
               {
                  MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%016x "\
                           "with Column ID: 0x%08X",
                        typed_column_value, column_id);
                  result = ADSP_EBADPARAM;
               }
               break;

            case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE:
            case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB:
            case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB:
            case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB:
            case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB:
               if ((column_id != VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE) &&
                   (column_id != VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE))
               {
                  MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%016x "\
                           "with Column ID: 0x%08X",
                        typed_column_value, column_id);
                  result = ADSP_EBADPARAM;
               }
               break;

            case VSS_MEDIA_ID_NONE:
            case VSS_MEDIA_ID_13K:
            case VSS_MEDIA_ID_EVRC:
            case VSS_MEDIA_ID_4GV_NB:
            case VSS_MEDIA_ID_4GV_WB:
            case VSS_MEDIA_ID_4GV_NW:
            case VSS_MEDIA_ID_4GV_NW2K:
            case VSS_MEDIA_ID_AMR_NB:
            case VSS_MEDIA_ID_AMR_WB:
            case VSS_MEDIA_ID_EAMR:
            case VSS_MEDIA_ID_EFR:
            case VSS_MEDIA_ID_FR:
            case VSS_MEDIA_ID_HR:
            case VSS_MEDIA_ID_PCM_8_KHZ:
            case VSS_MEDIA_ID_PCM_16_KHZ:
            case VSS_MEDIA_ID_PCM_32_KHZ:
            case VSS_MEDIA_ID_PCM_48_KHZ:
            case VSS_MEDIA_ID_G711_ALAW:
            case VSS_MEDIA_ID_G711_MULAW:
            case VSS_MEDIA_ID_G711_ALAW_V2:
            case VSS_MEDIA_ID_G711_MULAW_V2:
            case VSS_MEDIA_ID_G711_LINEAR:
            case VSS_MEDIA_ID_G729:
            case VSS_MEDIA_ID_G722:
            case VSS_MEDIA_ID_EVS:
               if (column_id != VSS_ICOMMON_CAL_COLUMN_MEDIA_ID)
               {
                  MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%016x "\
                           "with Column ID: 0x%08X",
                        typed_column_value, column_id);
                  result = ADSP_EBADPARAM;
               }
               break;

            case VSS_ICOMMON_CAL_FEATURE_NONE:
            case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE:
            case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2:
            case VSS_ICOMMON_CAL_FEATURE_BEAMR:
               if( column_id != VSS_ICOMMON_CAL_COLUMN_FEATURE || FALSE!= is_custom_feature )
               {
                  MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%016x "\
                        "with Column ID: 0x%08X, is_custom_feature(%d)", typed_column_value, column_id, is_custom_feature);
                  result = ADSP_EBADPARAM;
               }
               break;

            case VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE:
               if( column_id != VSS_ICOMMON_CAL_COLUMN_FEATURE || TRUE != is_custom_feature )
               {
                  MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%016x "\
                        "with Column ID: 0x%08X, is_custom_feature(%d)", typed_column_value, column_id, is_custom_feature);
                  result = ADSP_EBADPARAM;
               }
               break;

            case 0:
            case VSS_PARAM_SAMPLING_RATE_8K:
            case VSS_PARAM_SAMPLING_RATE_16K:
            case VSS_PARAM_SAMPLING_RATE_32K:
            case VSS_PARAM_SAMPLING_RATE_48K:
            case 41000:
               if ((column_id != VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE) &&
                   (column_id != VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE))
               {
                  MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Unmatching Column Value: 0x%08X "\
                           "with Column ID: 0x%08X",
                        typed_column_value, column_id);
                  result = ADSP_EBADPARAM;
               }
               break;

            default:
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_sys_config(): Undefined Column Value: 0x%08X \
                                                  with Column ID: 0x%08X",
                     typed_column_value, column_id);
               result = ADSP_EBADPARAM;
               break;
         }

         if (result)
         {
            /*  Invalid Sysem Configuration List  */
            break;  /*  Break from Inner For Loop  */
         }
         else
         {
            /* Advance Column Pointer to find next ID */
            columns = (vss_param_cal_column_t *)(((uint8_t *)columns) + sizeof(columns->id) + sizeof(columns->type) + column_size);

            /* Advance to next Column Value in Sys Config List */
            sys_config_column_value = (uint32_t *)(((uint8_t *)sys_config_column_value) + column_size);
         }
      }
      if (result)
      {
         /*  Invalid Sysem Configuration List  */
         break;  /*  Break from Outer For Loop  */
      }
      else
      {
        if( FALSE == is_custom_feature )
        {
         /* Do Data Parse */
         result = vpm_verify_hdvoice_config_data(sys_config_column_value);

         if (result)
         {
            /** Invalid Config Data **/
            break;
         }
         else
         {
            sys_config_column_value = (uint32_t *)(((uint8_t *)sys_config_column_value) + sizeof(vss_param_hdvoice_config_data_t));
            /* Return Column Pointer to the first Column */
            columns = ((vss_param_cal_column_t *)columns_head);
         }
      }
         else
         {
           /* Do Data Parse */
           result = vpm_verify_custom_hdvoice_config_data(sys_config_column_value);

           if (result)
           {
              /** Invalid Config Data **/
              break;
           }
           else
           {
              sys_config_column_value = (uint32_t *)(((uint8_t *)sys_config_column_value) + sizeof(vss_param_custom_bwe_config_data_t));
              /* Return Column Pointer to the first Column */
              columns = ((vss_param_cal_column_t *)columns_head);
           }
         }
      }
   }

   return result;
}

static ADSPResult vpm_verify_feature_config(vpm_session_object_t *session_obj,
                                            vpm_device_config_entry_t *device_config_entry,
                                            bool_t is_custom_feature)
{
   ADSPResult result = ADSP_EOK;
   uint32_t   column_size = 0;
   uint8_t  *param_data_rdr;

   if ((NULL == session_obj) || (NULL == device_config_entry))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_feature_config(): Invalid input parameter!");
      return ADSP_EBADPARAM;
   }

   param_data_rdr = ((uint8_t *)device_config_entry + sizeof(voice_param_data_t));

   if (param_data_rdr != NULL)
   {
      session_obj->hdvoice_config_info.hdvoice_config_hdr.minor_version = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->hdvoice_config_info.hdvoice_config_hdr.size = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->hdvoice_config_info.hdvoice_config_hdr.columns = (void *)param_data_rdr;
   }

   for (;;)
   {
      /** Verify Feature Config headers **/
      if (session_obj->hdvoice_config_info.hdvoice_config_hdr.minor_version != 0)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_feature_config(): Invalid Version %d",
               session_obj->hdvoice_config_info.hdvoice_config_hdr.minor_version);

         result = ADSP_EBADPARAM;
         break;
      }
      else if ((session_obj->hdvoice_config_info.hdvoice_config_hdr.size + (2 * sizeof(uint32_t))) !=
                  device_config_entry->param_size)
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_feature_config(): Invalid HD voice data size 0x%08X with param size 0x%08X",
               session_obj->hdvoice_config_info.hdvoice_config_hdr.size,
               device_config_entry->param_size);

         result = ADSP_EBADPARAM;
         break;
      }
      else if (0 == session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_feature_config(): Invalid number of columns %d",
               session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns);

         result = ADSP_EBADPARAM;
         break;
      }
      else if (NULL == session_obj->hdvoice_config_info.hdvoice_config_hdr.columns)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_verify_feature_config(): Column definition in header is NULL %d");

         result = ADSP_EBADPARAM;
         break;
      }
      else
      {
         /** Verify Sys Config Column data **/
         if (ADSP_EOK == (result = vpm_verify_sys_config_columns(session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns,
                                                                 session_obj->hdvoice_config_info.hdvoice_config_hdr.columns,
                                                                 &column_size)))
         {
            param_data_rdr += column_size;

            if (param_data_rdr != NULL)
            {
               session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries = (*(uint32_t *)param_data_rdr);
            }
            param_data_rdr += sizeof(uint32_t);

            session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head = ((void *)param_data_rdr);

            if (session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries == 0)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_feature_config(): Invalid number of entries %d",
                     session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries);
               result = ADSP_EBADPARAM;
               break;
            }
            else if (session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head == NULL)
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_feature_config(): Sys Config Entries is NULL");
               result = ADSP_EBADPARAM;
               break;
            }
            else
            {
               /** Verify Sys Config Data **/
               result = vpm_verify_sys_config(session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns,
                                              session_obj->hdvoice_config_info.hdvoice_config_hdr.columns,
                                              session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries,
                                              session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head,
                                              is_custom_feature);

               if (result)
               {
                  /** Invalid Sys Config Data **/
                  break;
               }
            }
         }
         else
         {
            /** Invalid Column Defintitions **/
            break;
         }
      }

      if (result == ADSP_EOK)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_verify_feature_config(): HD Voice Parameter Verified!");
         break;
      }
   }

   return result;
}

ADSPResult vpm_clear_feature_config(vpm_session_object_t *session_obj)
{
   session_obj->hdvoice_config_info.hdvoice_config_hdr.columns = NULL;
   session_obj->hdvoice_config_info.hdvoice_config_hdr.minor_version = 0;
   session_obj->hdvoice_config_info.hdvoice_config_hdr.num_columns = 0;
   session_obj->hdvoice_config_info.hdvoice_config_hdr.num_sys_config_entries = 0;
   session_obj->hdvoice_config_info.hdvoice_config_hdr.size = 0;
   session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head = NULL;
   session_obj->hdvoice_config_info.is_custom_bwe_config_registered = FALSE;

   return ADSP_EOK;
}

ADSPResult vpm_parse_device_config_feature_mod(vpm_session_object_t *session_obj,
                                               vpm_device_config_entry_t *device_config_entry)
{
   int32_t result = ADSP_EOK;

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_parse_device_config_feature_mod(): Feature Mod Parser");

   switch (device_config_entry->param_id)
   {
      case VSS_PARAM_HDVOICE_CONFIG:
      {
         if (session_obj->common_cal.is_registered || session_obj->volume_cal.is_registered)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_feature_mod(): "\
                   "Feature Config Module with HD Voice Incompatible "\
                   "with Common or Volume Calibration Tables");
            break;
         }
         else
         {
            result = vpm_verify_feature_config(session_obj, device_config_entry, FALSE);
         }

         if (result)
         {
            /* Invalid parameter, clear feature config info */
            (void)vpm_clear_feature_config(session_obj);
         }
         else
         {
           session_obj->hdvoice_config_info.is_custom_bwe_config_registered = FALSE;
         }
         break;
      }
      case VSS_PARAM_CUSTOM_BWE_CONFIG:
      {
        if (session_obj->common_cal.is_registered || session_obj->volume_cal.is_registered)
        {
           MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_feature_mod(): "\
                  "Feature Config Module with HD Voice Incompatible "\
                  "with Common or Volume Calibration Tables");
           break;
        }
        else
        {
           result = vpm_verify_feature_config(session_obj, device_config_entry, TRUE);
        }
        
        if (result)
        {
           /* Invalid parameter, clear feature config info */
           (void)vpm_clear_feature_config(session_obj);
        }
        else
        {
          session_obj->hdvoice_config_info.is_custom_bwe_config_registered = TRUE;
        }
         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_device_config_feature_mod(): "\
                  "Invalid param_id 0x%08X for mod_id 0x%08x",
               device_config_entry->param_id,
               device_config_entry->module_id);
         break;
      }
   }

   return result;
}

ADSPResult vpm_clear_sound_device_info(vpm_session_object_t *session_obj)
{
   if (session_obj == NULL)
   {
      return ADSP_EBADPARAM;
   }

   session_obj->sound_device_info.is_available = FALSE;
   session_obj->sound_device_info.device_pair.direction = VPM_SOUND_DEVICE_DIRECTION_INVALID;
   session_obj->sound_device_info.device_pair.rx_device_id = 0;
   session_obj->sound_device_info.device_pair.tx_device_id = 0;

   return ADSP_EOK;
}

ADSPResult vpm_parse_sound_device_mod(vpm_session_object_t *session_obj,
                                      vpm_device_config_entry_t *sound_device_mod)
{
   ADSPResult result = ADSP_EOK;

   if ((session_obj == NULL) || (sound_device_mod == NULL))
   {
      return ADSP_EBADPARAM;
   }

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_parse_sound_device_mod(): Sound device Mod Parser");

   switch (sound_device_mod->param_id)
   {
      case VSS_PARAM_RX_SOUND_DEVICE_ID:
      {
         if (sound_device_mod->param_size != sizeof(uint32_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Invalid sound device param size %d for param 0x%08X",
                  sound_device_mod->param_size, sound_device_mod->param_id);

            result = ADSP_EBADPARAM;
            break;
         }

         if (VPM_SOUND_DEVICE_DIRECTION_TX ==\
                session_obj->sound_device_info.device_pair.direction)
         {
            session_obj->sound_device_info.device_pair.direction =\
               VPM_SOUND_DEVICE_DIRECTION_RX_TX;
         }
         else if (VPM_SOUND_DEVICE_DIRECTION_INVALID ==\
                     session_obj->sound_device_info.device_pair.direction)
         {
            session_obj->sound_device_info.device_pair.direction =\
               VPM_SOUND_DEVICE_DIRECTION_RX;
         }

         session_obj->sound_device_info.device_pair.rx_device_id\
            = *((uint32_t *)((uint8_t *)sound_device_mod + sizeof(voice_param_data_t)));

         session_obj->sound_device_info.is_available = TRUE;

         break;
      }
      case VSS_PARAM_TX_SOUND_DEVICE_ID:
      {
         if (sound_device_mod->param_size != sizeof(uint32_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Invalid sound device param size %d for param 0x%08X",
                  sound_device_mod->param_size, sound_device_mod->param_id);

            result = ADSP_EBADPARAM;
            break;
         }

         if (VPM_SOUND_DEVICE_DIRECTION_RX ==\
                session_obj->sound_device_info.device_pair.direction)
         {
            session_obj->sound_device_info.device_pair.direction =\
               VPM_SOUND_DEVICE_DIRECTION_RX_TX;
         }
         else if (VPM_SOUND_DEVICE_DIRECTION_INVALID ==\
                     session_obj->sound_device_info.device_pair.direction)
         {
            session_obj->sound_device_info.device_pair.direction =\
               VPM_SOUND_DEVICE_DIRECTION_TX;
         }

         session_obj->sound_device_info.device_pair.tx_device_id
            = *((uint32_t *)((uint8_t *)sound_device_mod + sizeof(voice_param_data_t)));

         session_obj->sound_device_info.is_available = TRUE;
         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO, "Unrecognized param ID 0x%08X for mod ID 0x%08X",
               sound_device_mod->param_id, sound_device_mod->module_id);
         break;
      }
   } /* End of switch (sound_device_mod->param_id)*/

   if (result != ADSP_EOK)
   {
      (void)vpm_clear_sound_device_info(session_obj);
   }

   return result;
}


/* Verify custom topology clock control parameters.
 */
static ADSPResult vpm_verify_clk_ctrl_columns(uint32_t num_columns,
                                              void *columns_head,
                                              uint32_t *column_size)
{
   ADSPResult result = ADSP_EOK;
   uint32_t  i;
   uint32_t  advance_in_bytes = 0;

   vss_param_clock_control_params_t *columns = ((vss_param_clock_control_params_t *)columns_head);
   advance_in_bytes = sizeof(vss_param_clock_control_params_t);
   *column_size = 0;

   for (i = 0; i < num_columns; i++)
   {
      if (columns->direction != VSS_IVOCPROC_DIRECTION_RX &&
          columns->direction != VSS_IVOCPROC_DIRECTION_TX &&
          columns->direction != VSS_IVOCPROC_DIRECTION_RX_TX)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_clk_ctrl_columns(): Undefined direction 0x%08X", columns->direction);
         result = ADSP_EBADPARAM;
         break;
      }
      if (columns->sampling_rate != VSS_PARAM_SAMPLING_RATE_ANY &&
          columns->sampling_rate != VSS_PARAM_SAMPLING_RATE_8K &&
          columns->sampling_rate != VSS_PARAM_SAMPLING_RATE_16K &&
          columns->sampling_rate != VSS_PARAM_SAMPLING_RATE_32K &&
          columns->sampling_rate != VSS_PARAM_SAMPLING_RATE_48K)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_clk_ctrl_columns(): Undefined sampling_rate 0x%08X", columns->sampling_rate);
         result = ADSP_EBADPARAM;
         break;
      }
      if (columns->mpps_scale_factor < VPM_MIN_MPPS_SCALE_FACTOR ||
          columns->mpps_scale_factor > VPM_MAX_MPPS_SCALE_FACTOR)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_clk_ctrl_columns(): Invalid mpps_scale_factor 0x%08X", columns->mpps_scale_factor);
         result = ADSP_EBADPARAM;
         break;
      }
      if (columns->bus_bw_scale_factor < VPM_MIN_BW_SCALE_FACTOR ||
          columns->bus_bw_scale_factor > VPM_MAX_BW_SCALE_FACTOR)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_clk_ctrl_columns(): Invalid bus_bw_scale_factor 0x%08X", columns->bus_bw_scale_factor);
         result = ADSP_EBADPARAM;
         break;
      }
      {
         /* Advance column pointer */
         columns = (vss_param_clock_control_params_t *)(((uint8_t *)columns) + advance_in_bytes);
         *column_size += advance_in_bytes;
      }
   }
   return result;
}

/* Verify custom topology clock control table.
 */
static ADSPResult vpm_verify_clk_ctrl_params(vpm_session_object_t *session_obj,
                                             vpm_device_config_entry_t *device_config_entry)
{
   ADSPResult result = ADSP_EOK;
   uint32_t column_size = 0;
   uint8_t *param_data_rdr;

   if (session_obj == NULL || device_config_entry == NULL)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_clk_ctrl_params(): Invalid input parameter!");
      return ADSP_EBADPARAM;
   }

   param_data_rdr = ((uint8_t *)device_config_entry + sizeof(voice_param_data_t));

   if (param_data_rdr != NULL)
   {
      session_obj->clk_ctrl_table.minor_version = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->clk_ctrl_table.size = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->clk_ctrl_table.num_columns = (*(uint32_t *)param_data_rdr);
   }
   param_data_rdr += sizeof(uint32_t);

   if (param_data_rdr != NULL)
   {
      session_obj->clk_ctrl_table.columns = (void *)param_data_rdr;
   }

   for (;;)
   {
      /* Verify clock control headers */
      if (session_obj->clk_ctrl_table.minor_version != 0)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_clk_ctrl_params(): Invalid Version %d",
               session_obj->clk_ctrl_table.minor_version);
         result = ADSP_EBADPARAM;
         break;
      }
      else if ((session_obj->clk_ctrl_table.size + (2 * sizeof(uint32_t))) != device_config_entry->param_size)
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_verify_clk_ctrl_params(): Invalid data size 0x%08X with param size 0x%08X",
               session_obj->clk_ctrl_table.size,
               device_config_entry->param_size);
         result = ADSP_EBADPARAM;
         break;
      }
      else if (0 == session_obj->clk_ctrl_table.num_columns)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_clk_ctrl_params(): Invalid number of columns %d",
               session_obj->clk_ctrl_table.num_columns);
         result = ADSP_EBADPARAM;
         break;
      }
      else if (NULL == session_obj->clk_ctrl_table.columns)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_verify_clk_ctrl_params(): Column definition is NULL %d");
         result = ADSP_EBADPARAM;
         break;
      }
      else
      {
         /* Verify clk_ctrl_table column data */
         result = vpm_verify_clk_ctrl_columns(session_obj->clk_ctrl_table.num_columns,
                                              session_obj->clk_ctrl_table.columns,
                                              &column_size);
         if (result != ADSP_EOK)
         {
            /* Invalid Column Defintitions */
            break;
         }
      }
      break;
   }

   return result;
}

ADSPResult vpm_clear_clock_control_info(vpm_session_object_t *session_obj)
{
   if (session_obj == NULL)
   {
      return ADSP_EBADPARAM;
   }

   session_obj->clk_ctrl_table.columns = NULL;
   session_obj->clk_ctrl_table.minor_version = 0;
   session_obj->clk_ctrl_table.num_columns = 0;
   session_obj->clk_ctrl_table.size = 0;

   return ADSP_EOK;
}

/* Parse custom topology clock control table.
 */
ADSPResult vpm_parse_clk_ctrl_mod(vpm_session_object_t *session_obj,
                                  vpm_device_config_entry_t *clk_ctrl_mod)
{
   ADSPResult result = ADSP_EOK;

   if ((session_obj == NULL) || (clk_ctrl_mod == NULL))
   {
      return ADSP_EBADPARAM;
   }

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_parse_clk_ctrl_mod(): Clock control mod Parser");

   switch (clk_ctrl_mod->param_id)
   {
      case VSS_PARAM_CLOCK_CONTROL:
      {
         result = vpm_verify_clk_ctrl_params(session_obj, clk_ctrl_mod);

         if (result)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_parse_clk_ctrl_mod(): param 0x%08X, vpm_verify_clk_ctrl_params result %d",
                  clk_ctrl_mod->param_id, result);

            /* Invalid parameters, clear clock control info. */
            (void)vpm_clear_clock_control_info(session_obj);
         }
         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_parse_clk_ctrl_mod(): Unrecognized param ID 0x%08X for mod ID 0x%08X",
               clk_ctrl_mod->param_id, clk_ctrl_mod->module_id);
         break;
      }
   }
   return result;
}

ADSPResult vpm_post_parse_device_config(vpm_session_object_t *session_obj,
                                        uint32_t temp_wv_sr)
{
   if ((temp_wv_sr != 0) &&
       (NULL == session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head))
   {
      /* Pick higher Rx sampling rate between widevoice and system_config settings,
       * if client has not set it.  */
      session_obj->widevoice_rx_sr = temp_wv_sr;

      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_post_parse_device_config(): "\
             "Device Config Registered with WideVoice Module");

      if ((FALSE == session_obj->is_sr_set_by_client) &&
          (session_obj->widevoice_rx_sr > session_obj->target_set.system_config.rx_pp_sr))
      {
         session_obj->target_set.system_config.rx_pp_sr = session_obj->widevoice_rx_sr;
         session_obj->is_vocproc_config_changed = TRUE;
      }
   }

   return ADSP_EOK;
}

/* Search clock voting scale factors for the custom topology use cases.
 */
ADSPResult vpm_get_clock_scale_factors(
  vpm_session_object_t *session_obj,
  vss_icommon_rsp_set_system_config_t *set_system_config_rsp )
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    use_case_idx;
   uint32_t    num_clk_columns;
   uint32_t    param_match_cnt;
   uint32_t    max_param_match_cnt;

   vss_param_clock_control_params_t *clk_column;

   vss_param_clock_control_params_t default_vals = { VSS_IVOCPROC_DIRECTION_RX_TX,\
      VSS_PARAM_SAMPLING_RATE_ANY,
      VSS_ICOMMON_CAL_NETWORK_ID_NONE,
      VSS_MEDIA_ID_NONE,
      0, 0 };

   vpm_clk_control_config_t *clk_table_ptr = &session_obj->clk_ctrl_table;
   vss_icommon_cmd_set_system_config_t *sys_config = &session_obj->active_set.system_config;

   /* Default values. */
   set_system_config_rsp->tx_mpps_scale_factor = 0;
   set_system_config_rsp->tx_bw_scale_factor = 0;
   set_system_config_rsp->rx_mpps_scale_factor = 0;
   set_system_config_rsp->rx_bw_scale_factor = 0;

   if (NULL == sys_config)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_get_clock_scale_factors(): Invalid params");
      return ADSP_EBADPARAM;
   }

   if (FALSE == session_obj->is_device_config_registered)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_get_clock_scale_factors(): Device config table not registered with CVP");
      return ADSP_EOK;
   }

   if ((NULL == clk_table_ptr) || (NULL == clk_table_ptr->columns))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_get_clock_scale_factors(): No clock table");
      return ADSP_EOK;
   }

   num_clk_columns = clk_table_ptr->num_columns;

   /* Find the matching clock column that can support the current voice use case. */
   MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_get_clock_scale_factors(): "\
            "network_id 0x%04X media_id 0x%04X",
         sys_config->network_id,
         sys_config->media_id);

   MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_get_clock_scale_factors(): "\
            "tx_sampl_rate 0x%08X rx_sampl_rate 0x%04X, num_clk_columns %d",
         sys_config->tx_pp_sr,
         sys_config->rx_pp_sr, num_clk_columns);

   /* Find match for Tx scale factors */
   max_param_match_cnt = 0;

   for (use_case_idx = 0; use_case_idx < num_clk_columns; use_case_idx++)
   {
      clk_column = (vss_param_clock_control_params_t *)
                   ((uint8_t *)clk_table_ptr->columns + (use_case_idx * sizeof(vss_param_clock_control_params_t)));

      param_match_cnt = 0;

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
               "direction 0x%08X network_id 0x%04X media_id 0x%04X",
            clk_column->direction,
            clk_column->network_id,
            clk_column->media_type_id);

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
               "mpps_scale_factor %d bus_bw_scale_factor %d sampling_rate %d",
            clk_column->mpps_scale_factor,
            clk_column->bus_bw_scale_factor,
            clk_column->sampling_rate);

      if ((VSS_IVOCPROC_DIRECTION_TX == clk_column->direction) &&
          (sys_config->tx_pp_sr == clk_column->sampling_rate) &&
          (sys_config->network_id == clk_column->network_id) &&
          (sys_config->media_id == clk_column->media_type_id))
      {
         set_system_config_rsp->tx_mpps_scale_factor = clk_column->mpps_scale_factor;
         set_system_config_rsp->tx_bw_scale_factor = clk_column->bus_bw_scale_factor;

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
                  "Tx Match found: direction %d mpps_scale_factor %d bus_bw_scale_factor %d",
               clk_column->direction,
               clk_column->mpps_scale_factor,
               clk_column->bus_bw_scale_factor);

         break;
      }

      if ((VSS_IVOCPROC_DIRECTION_TX == clk_column->direction) ||
          (default_vals.direction == clk_column->direction))
      {
         if (VSS_IVOCPROC_DIRECTION_TX == clk_column->direction)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->tx_pp_sr == clk_column->sampling_rate) ||
          (default_vals.sampling_rate == clk_column->sampling_rate))
      {
         if (sys_config->tx_pp_sr == clk_column->sampling_rate)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->network_id == clk_column->network_id) ||
          (default_vals.network_id == clk_column->network_id))
      {
         if (sys_config->network_id == clk_column->network_id)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->media_id == clk_column->media_type_id) ||
          (default_vals.media_type_id == clk_column->media_type_id))
      {
         if (sys_config->media_id == clk_column->media_type_id)
            param_match_cnt++;
      }
      else
         continue;

      if (param_match_cnt >= max_param_match_cnt)
      {
         max_param_match_cnt = param_match_cnt;
         set_system_config_rsp->tx_mpps_scale_factor = clk_column->mpps_scale_factor;
         set_system_config_rsp->tx_bw_scale_factor = clk_column->bus_bw_scale_factor;

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
                  "Tx Close Match: max_param_match_cnt %d mpps_scale_factor %d bus_bw_scale_factor %d",
               max_param_match_cnt,
               clk_column->mpps_scale_factor,
               clk_column->bus_bw_scale_factor);
      }
   } /*End of for (use_case_idx) */

   /* Find match for Rx scale factors */
   max_param_match_cnt = 0;

   for (use_case_idx = 0; use_case_idx < num_clk_columns; use_case_idx++)
   {
      clk_column = (vss_param_clock_control_params_t *)
                   ((uint8_t *)clk_table_ptr->columns + (use_case_idx * sizeof(vss_param_clock_control_params_t)));
      param_match_cnt = 0;

      if ((VSS_IVOCPROC_DIRECTION_RX == clk_column->direction) &&
          (sys_config->rx_pp_sr == clk_column->sampling_rate) &&
          (sys_config->network_id == clk_column->network_id) &&
          (sys_config->media_id == clk_column->media_type_id))
      {
         set_system_config_rsp->rx_mpps_scale_factor = clk_column->mpps_scale_factor;
         set_system_config_rsp->rx_bw_scale_factor = clk_column->bus_bw_scale_factor;

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
                  "Rx Match found: direction %d mpps_scale_factor %d bus_bw_scale_factor %d",
               clk_column->direction,
               clk_column->mpps_scale_factor,
               clk_column->bus_bw_scale_factor);
         break;
      }

      if ((VSS_IVOCPROC_DIRECTION_RX == clk_column->direction) ||
          (default_vals.direction == clk_column->direction))
      {
         if (VSS_IVOCPROC_DIRECTION_RX == clk_column->direction)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->rx_pp_sr == clk_column->sampling_rate) ||
          (default_vals.sampling_rate == clk_column->sampling_rate))
      {
         if (sys_config->rx_pp_sr == clk_column->sampling_rate)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->network_id == clk_column->network_id) ||
          (default_vals.network_id == clk_column->network_id))
      {
         if (sys_config->network_id == clk_column->network_id)
            param_match_cnt++;
      }
      else
         continue;

      if ((sys_config->media_id == clk_column->media_type_id) ||
          (default_vals.media_type_id == clk_column->media_type_id))
      {
         if (sys_config->media_id == clk_column->media_type_id)
            param_match_cnt++;
      }
      else
         continue;

      if (param_match_cnt >= max_param_match_cnt)
      {
         max_param_match_cnt = param_match_cnt;
         set_system_config_rsp->rx_mpps_scale_factor = clk_column->mpps_scale_factor;
         set_system_config_rsp->rx_bw_scale_factor = clk_column->bus_bw_scale_factor;

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_get_clock_scale_factors(): "\
                  "Rx Close Match: max_param_match_cnt %d mpps_scale_factor %d bus_bw_scale_factor %d",
               max_param_match_cnt,
               clk_column->mpps_scale_factor,
               clk_column->bus_bw_scale_factor);
      }
   }

   return ADSP_EOK;
}

ADSPResult vpm_register_device_config_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult         result = ADSP_EOK;
   int32_t            rc;
   uint32_t           payload_size;
   cvd_virt_addr_t    device_config_base_virt_addr;
   uint32_t           parsed_size = 0;
   uint32_t           temp_wv_sr = 0;

   elite_apr_packet_t             *p_apr_pkt;
   cvd_cal_log_commit_info_t      log_info;
   cvd_cal_log_table_header_t     log_info_table;
   vpm_device_config_entry_t      *device_config_entry;

   vss_ivocproc_cmd_register_device_config_t  *payload;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         if (TRUE == session_obj->is_device_config_registered)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_register_device_config_cmd_ctrl(): Device config data already registered");

            result = ADSP_EALREADY;
            break;
         }

         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         if (payload_size != sizeof(vss_ivocproc_cmd_register_device_config_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_device_config_cmd_ctrl(): Unexpected payload size, %d != %d",
                  payload_size,
                  sizeof(vss_ivocproc_cmd_register_device_config_t));

            result = ADSP_EBADPARAM;
            break;
         }

         payload = (vss_ivocproc_cmd_register_device_config_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

         if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                              payload->mem_size,
                                                              payload->mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_device_config_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
                  result);

            /* End the APR command */
            break;
         }

         if (APR_EOK != (rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle,
                                                                 payload->mem_address,
                                                                 &device_config_base_virt_addr)))
         {
            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_device_config_cmd_ctrl(): Cannot get virtual address,"
                  " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                  (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

            result = ADSP_EFAILED;
            break;
         }

         if (payload->mem_size < sizeof(vpm_device_config_entry_t))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_device_config_cmd_ctrl(): Mem size is too small, %4d",
                  payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         (void)cvd_mem_mapper_cache_invalidate_v2(&device_config_base_virt_addr,
                                                  payload->mem_size);

         /* Log device config data. */
         log_info_table.table_handle = 0;

         log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                                 (session_obj->self_apr_port));
         log_info.call_num = session_obj->target_set.system_config.call_num;
         log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_TABLE;
         log_info.data_container_header_size = sizeof(log_info_table);
         log_info.data_container_header = &log_info_table;
         log_info.payload_size = payload->mem_size;
         log_info.payload_buf = device_config_base_virt_addr.ptr;

         cvd_cal_log_data( ( log_code_type ) LOG_ADSP_CVD_CAL_DATA_C,
                           CVD_CAL_LOG_DEVICE_CONFIG_TABLE,
                               (void *)&log_info, sizeof(log_info));

         do
         {
            device_config_entry =
                                  (vpm_device_config_entry_t *)(((uint8_t *)device_config_base_virt_addr.ptr) + parsed_size);

            if ((device_config_entry == NULL) ||
                (((uint8_t *)device_config_entry) + sizeof(voice_param_data_t) == NULL))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_device_config_cmd_ctrl(): "\
                      "Device Config Entry is NULL");

               result = ADSP_EFAILED;
               break;
            }

            if (((((uint8_t *)device_config_entry) + sizeof(vpm_device_config_entry_t)) > (((uint8_t *)device_config_base_virt_addr.ptr) + payload->mem_size)) ||
                ((((uint8_t *)device_config_entry) + device_config_entry->param_size) > (((uint8_t *)device_config_base_virt_addr.ptr) + payload->mem_size)))
            {
               MSG_4(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_device_config_cmd_ctrl(): "\
                        "Device Config Entry out Virt Mem Bounds. "\
                        "entry_addr=0x%08x entry_size=0x%08x, virt_addr=0x%08x, virt_mem_size=0x%08x",
                     device_config_entry,
                     device_config_entry->param_size,
                     device_config_base_virt_addr.ptr,
                     payload->mem_size);

               result = ADSP_EFAILED;
               break;
            }

            switch (device_config_entry->module_id)
            {
               case VSS_MODULE_WIDEVOICE:
               {
                  result = vpm_parse_device_config_wv_mod(session_obj, device_config_entry, &temp_wv_sr);
                  break;
               }
               case VSS_MODULE_FEATURE_CONFIG:
               case VSS_MODULE_CUSTOM_FEATURE_CONFIG:
               {
                  result = vpm_parse_device_config_feature_mod(session_obj, device_config_entry);
                  break;
               }
               case VSS_MODULE_SOUND_DEVICE:
               {
                  result = vpm_parse_sound_device_mod(session_obj, device_config_entry);
                  break;
               }
               case VSS_MODULE_CLOCK_CONTROL:
               {
                  result = vpm_parse_clk_ctrl_mod(session_obj, device_config_entry);
                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_device_config_cmd_ctrl(): "\
                           "Invalid mod_id 0x%08X",
                        device_config_entry->module_id);
                  break;
               }
            }

            parsed_size += (device_config_entry->param_size + sizeof(voice_param_data_t));

            if (result)
            {
               /* Error in Parsing. Break do-while loop */
               break;
            }
         }while (parsed_size < payload->mem_size);

         if (ADSP_EOK != result)
         {
            /* Propagate errors from Device Config Parsing */
            break;
         }

         session_obj->is_device_config_registered = TRUE;

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG: vpm_register_device_config_cmd_ctrl(): Successfully registered, Table size reported: %d",
               payload->mem_size);

         (void)vpm_post_parse_device_config(session_obj, temp_wv_sr);

         if (TRUE == vpm_is_reinit_required(session_obj))
         {
            vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_REINIT);
         }
         else
         {
            /* Nothing to do, end the APR command */
            break;
         }
      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      result = vpm_state_control(session_obj);

      /* Check the goal completion status. If completed, end the APR command,
       else keep the cmd in pending status until completion.
       Session cmdQ will not be popped until cmd completion */
      vpm_check_goal_completion_status(session_obj);

      return result;

   } /* End of for(;;)*/


   /* End APR command if any error or no action is required */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

ADSPResult vpm_deregister_device_config_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   if (FALSE == session_obj->is_device_config_registered)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_deregister_device_config_cmd_ctrl(): Device config data not registered");

      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_ENOTREADY);

      return ADSP_ENOTREADY;
   }

   session_obj->widevoice_rx_sr = VPM_DEFAULT_RX_PP_SR;
   session_obj->is_device_config_registered = FALSE;

   (void)vpm_clear_feature_config(session_obj);
   (void)vpm_clear_sound_device_info(session_obj);
   (void)vpm_clear_clock_control_info(session_obj);

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EOK);

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
       "vpm_deregister_device_config_cmd_ctrl(): Successfully deregistered dev cfg");

   return ADSP_EOK;
}

