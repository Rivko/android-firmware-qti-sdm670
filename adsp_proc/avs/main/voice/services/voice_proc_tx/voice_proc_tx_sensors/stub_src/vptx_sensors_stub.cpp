/*============================================================================
  FILE:          vptx_sensors_stub.cpp

  OVERVIEW:      stub file for sensors interface for fluence pro v2

  DEPENDENCIES:  None

Copyright (c) 2014, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/voice_proc_tx_sensors/stub_src/vptx_sensors_stub.cpp#2 $

  when       who        what, where, why
  ---------- -------    ---------------------------------------------------------
  2014-07-14 rishabht   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "vptx_sensors_api.h"



ADSPResult vptx_sns_qmi_init(void *pQmi_object, qurt_elite_channel_t  *pChannel, uint32_t qmi_reg_mask, uint32_t qmi_ind_mask, uint32_t session_num)
{
   return ADSP_EOK;
}


ADSPResult vptx_sns_qmi_register(void *pQmi_object)
{
   return ADSP_EOK;
}



void vptx_sns_qmi_destroy(void *pQmi_object)
{
   return;
}


void vptx_sns_qmi_deregister(void *pQmi_object)
{
   return;
}


uint32_t vptx_sns_qmi_get_mem_req(void)
{
   return 0;
}


/***************************************************************************************************************************/
   // Copy over the decoded message contents (basically the holding pattern).
   // this state(holding pattern) can take following values
   //typedef enum FPv2_HoldingPattern_Enum_Type
   //{
      //SNS_SAM_FACING_EVENT_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
      //FACING_UP_V01 = 1, /**<  Phone has just moved to a facing-up phone posture, which is defined as screen up  */
      //FACING_DOWN_V01 = 2, /**<  Phone has just moved to a facing-down phone posture, which is defined as screen down  */
      //FACING_NEUTRAL_V01 = 3, /**<  Phone has just left either the facing-up or the facing-down phone posture  */
      //SNS_SAM_FACING_EVENT_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
   //}FPv2_HoldingPattern_Enum_Type;
/***************************************************************************************************************************/

int32_t  vptx_sns_get_holding_pattern(void *pQmi_object)
{
   return 1;           // returning FACING_UP_V01 pattern, although this function should never be called if accelerometer is stubbed
}


void vptx_sns_clear_qmi_signal(void *pQmi_object, reg_ind_signal_t sig_flag)
{
   return;
}