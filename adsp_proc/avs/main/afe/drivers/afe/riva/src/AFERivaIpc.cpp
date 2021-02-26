/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaIpc.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFERivaIpc.cpp

DESCRIPTION: handles all the IPC between RIVA (CCPU) and LPASS (QDSP)

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

 Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaIpc.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/11    rb      Created file

========================================================================== */

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "AFERivaDriver.h"
#include "AFERivaDriver_i.h"
#include "AFEInternal.h"
#include "AFEDevService.h"
#include "AFEInterface.h"
#include "qurt_elite.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "LPASS_ADDRESS_FILE.h"
#include "lpasshwio_devcfg.h"
#include "AfeTimeStampDriver.h"


#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "DDIChipInfo.h"
#include "hwd_devcfg.h"

#define AFE_INTERNAL_BT_THREAD_STACK_SIZE 1024
#define AFE_INTERNAL_FM_THREAD_STACK_SIZE 1024

#define AFE_INTERNAL_BTFM_CMD_IST_STACK_SIZE  2048
#define AFE_INTERNAL_BTFM_DATA_IST_STACK_SIZE 1024

static qurt_mem_cache_mode_t riva_cache_attribs = QURT_MEM_CACHE_NONE;
static qurt_perm_t riva_mem_perm = (qurt_perm_t) (QURT_PERM_READ | QURT_PERM_WRITE);


void afe_riva_circ_buf_protect_handler(afe_dev_port_t *dev_port_ptr);

/**
 * notes about MMPM request for riva:
 * 1. MMPM request may take some time as we register with ADSPPM,
 *    if AV timer interrupt is already occurring, some of those interrupts are missed.
 *    This may be ok, since BT or FM connection is still in process.
 * 2. Sampling rate & num of channel should be set during BW request &
 *    also check on WAIT state of riva is required. Hence, mmpm_config is under mutex for BT/FM.
 * 3. BW request is done from both dma start as well as BT or FM start. BW requested are:
 *       (a) EBI1 to Q6 -> DMA start: clients <-> port through AXI. BT/FM start: system fabric b/w CMEM to LPASS.
 *       (b) LPM to LPM -> not useful for riva
 *       (c) LPM to AUDIF -> AHB-I BW ->??
 * 4. BW requested based on default value of sampling rate, num chan during DMA start, just serve as rough values.
 */

/*=====================================================================
 Globals
 ======================================================================*/
/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_riva_device_cfg_init()
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);
  lpasshwio_prop_riva_struct_t  *AFERivaPropStructPtr = NULL;
  const char *pStringDevice="AFERiva";
  ADSPResult result = ADSP_EOK;

  if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(pStringDevice, pHandle))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DALSYS_GetDALPropertyHandleStr failed for Riva");
    return ADSP_EFAILED;
  }

  if(DAL_SUCCESS != DALSYS_GetPropertyValue(pHandle,"AFERivaPropStructPtr", 0, &propValue))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DALSYS_GetPropertyValue failed for Riva");
    return ADSP_EFAILED;
  }

  AFERivaPropStructPtr = (lpasshwio_prop_riva_struct_t *)(propValue.Val.pStruct);
  aud_stat_afe_riva_ptr->hw_revision     = AFERivaPropStructPtr->hw_revision;
  aud_stat_afe_riva_ptr->bt_fm_ipc_int   = AFERivaPropStructPtr->internal_bt_fm_ipc_int;
  aud_stat_afe_riva_ptr->bt_rx_int       = AFERivaPropStructPtr->internal_bt_rx_int;
  aud_stat_afe_riva_ptr->bt_tx_int       = AFERivaPropStructPtr->internal_bt_tx_int;
  aud_stat_afe_riva_ptr->fm_tx_rx_int    = AFERivaPropStructPtr->internal_fm_tx_rx_int;
  aud_stat_afe_riva_ptr->ipc_send_mail_box_offset = AFERivaPropStructPtr->ipc_send_mail_box_offset;
  aud_stat_afe_riva_ptr->ccu_mb0_ctrl_reg_phy_addr = AFERivaPropStructPtr->ccu_mb0_ctrl_reg_phy_addr;
  if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                      AFERivaPropStructPtr->ccu_mb0_ctrl_reg_phy_addr,
                                                                      &(aud_stat_afe_riva_ptr->ccu_mb0_ctrl_reg_virt_addr),
                                                                      riva_mem_perm)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RIVA Dev Config init, Failed to get VA for mailbox PA: 0x%lx",
          AFERivaPropStructPtr->ccu_mb0_ctrl_reg_phy_addr);

    return ADSP_EFAILED;
  }
  aud_stat_afe_riva_ptr->hw_timestamp_available = (bool_t)AFERivaPropStructPtr->hw_timestamp_available;
  aud_stat_afe_riva_ptr->q6ss_ipc_reg_phy_addr  = AFERivaPropStructPtr->q6ss_ipc_reg_phy_addr;
  if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                      AFERivaPropStructPtr->q6ss_ipc_reg_phy_addr,
                                                                      &(aud_stat_afe_riva_ptr->q6ss_ipc_reg_virt_addr),
                                                                      riva_mem_perm)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RIVA Dev Config init, Failed to get VA for IPC reg PA: 0x%lx",
          AFERivaPropStructPtr->q6ss_ipc_reg_phy_addr);

    return ADSP_EFAILED;
  }
  aud_stat_afe_riva_ptr->q6ss_ipc_reg_ack_bit_pos = AFERivaPropStructPtr->q6ss_ipc_reg_ack_bit_pos;

  MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT Tx Int: %d, BT Rx Int: %d, Fm Int: %d, Ipc Int: %d, HW TS Avail: %d",
        AFERivaPropStructPtr->internal_bt_tx_int,AFERivaPropStructPtr->internal_bt_rx_int,AFERivaPropStructPtr->internal_fm_tx_rx_int,
        AFERivaPropStructPtr->internal_bt_fm_ipc_int, AFERivaPropStructPtr->hw_timestamp_available);

  return ADSP_EOK;
}

ADSPResult afe_riva_device_cfg_deinit()
{
  aud_stat_afe_riva_ptr->hw_revision     = 0;
  return ADSP_EOK;
}

/**
 * Perform one time initialization of the AFE Riva IPC during start up
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_ipc_init(aud_stat_afe_riva_t *stat_afe_riva_ptr)
{
  ADSPResult result = ADSP_EFAILED;
  char_t thread_name[16];

  if (ADSP_EOK != (result = afe_riva_device_cfg_init()))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RIVA Device Cfg Fail");
    return result;
  }

  //initialize is_riva_active flag to true
  aud_stat_afe_riva_ptr->is_riva_active                       = TRUE;

  aud_stat_afe_riva_ptr->riva_port_bt_voc_tx.data_size        = 60;    //8kHz * mono * 2 bytesPerCh * 3.75
  aud_stat_afe_riva_ptr->riva_port_bt_voc_tx.riva_intr_period = 3750;  //3.75 ms
  aud_stat_afe_riva_ptr->riva_port_bt_voc_rx.data_size        = 60;    //8kHz * mono * 2 bytesPerCh * 3.75
  aud_stat_afe_riva_ptr->riva_port_bt_voc_rx.riva_intr_period = 3750;  //3.75 ms

  aud_stat_afe_riva_ptr->riva_port_bt_aud.data_size           = 192;   //48kHz * stereo * 2 bytesPerCh * 1ms

  aud_stat_afe_riva_ptr->riva_port_fm_tx.data_size            = 480;   //48kHz * stereo * 2 bytesPerCh * 2.5
  aud_stat_afe_riva_ptr->riva_port_fm_rx.data_size            = 480;   //48kHz * stereo * 2 bytesPerCh * 2.5

  //Register the lpass<->riva IPC interrupt.
  qurt_elite_strl_cpy(thread_name,"riva_ipc_isr", sizeof(thread_name));

  if(ADSP_EOK
      != (result = qurt_elite_interrupt_register(&(aud_stat_afe_riva_ptr->riva_intr_struct),
                                                 aud_stat_afe_riva_ptr->bt_fm_ipc_int,
                                                 afe_driver_riva_cmd_isr, (void*)0, thread_name,
                                                 AFE_INTERNAL_BTFM_CMD_IST_STACK_SIZE)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Riva: Failed to register riva interrupt");
    return result;
  }
  stat_afe_riva_ptr->is_riva_intr_registered = TRUE;

  if (ADSP_FAILED(result = afe_riva_init_aud_sync()))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Riva: Failed to init aud sync");
  }

  return result;
}

/**
 * Perform de-initialization of the AFE Riva driver during destroy
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_ipc_deinit(aud_stat_afe_riva_t *stat_afe_riva_ptr)
{
  ADSPResult result;

  result = afe_riva_deinit_aud_sync();

  //Deregister the lpass<->riva IPC interrupt.
  if(stat_afe_riva_ptr->is_riva_intr_registered)
  {
    result |= qurt_elite_interrupt_unregister(&(stat_afe_riva_ptr->riva_intr_struct));
    stat_afe_riva_ptr->is_riva_intr_registered = FALSE;
  }

  result |= afe_riva_device_cfg_deinit();

  return result;
}

/**
 * initializes sec tasks.
 */
ADSPResult afe_riva_ipc_secondary_task_init (afe_dev_port_t *pDevPort)
{
  ADSPResult                 result = ADSP_EFAILED;
  afe_dynamic_thread_task_t  task;
  afe_dev_riva_port_t        *riva_port_ptr = NULL;
  uint32_t                   thread_priority = 0;
  uint32_t                   stack_size = 0;

  if (NULL == pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid device port pointer.");
    return result;
  }

  riva_port_ptr = (afe_dev_riva_port_t *)pDevPort->afe_drv;

  switch (pDevPort->intf)
  {
    case AFE_PORT_ID_INTERNAL_BT_SCO_RX:
      task.pfn_task_func = afe_driver_bt_voc_cmem_write;
      thread_priority = elite_get_thrd_prio(ELITETHREAD_DYNA_AFE_INTERNAL_BT_PRIO_ID);
      stack_size = AFE_INTERNAL_BT_THREAD_STACK_SIZE;
      break;

    case AFE_PORT_ID_INTERNAL_BT_SCO_TX:
      task.pfn_task_func = afe_driver_bt_voc_cmem_read;
      thread_priority = elite_get_thrd_prio(ELITETHREAD_DYNA_AFE_INTERNAL_BT_PRIO_ID);
      stack_size = AFE_INTERNAL_BT_THREAD_STACK_SIZE;
      break;

    case AFE_PORT_ID_INTERNAL_FM_RX:
      task.pfn_task_func = afe_driver_fm_aud_cmem_write;
      thread_priority = elite_get_thrd_prio(ELITETHREAD_DYNA_AFE_INTERNAL_FM_PRIO_ID);
      stack_size = AFE_INTERNAL_FM_THREAD_STACK_SIZE;
      break;

    case AFE_PORT_ID_INTERNAL_FM_TX:
      task.pfn_task_func = afe_driver_fm_aud_cmem_read;
      thread_priority = elite_get_thrd_prio(ELITETHREAD_DYNA_AFE_INTERNAL_FM_PRIO_ID);
      stack_size = AFE_INTERNAL_FM_THREAD_STACK_SIZE;
      break;

    default:
      return result;
  }

  if (ADSP_FAILED(result = \
                  qurt_elite_signal_init(&(riva_port_ptr->riva_signal))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "ADSP_FAILED to init signal in port manager = %d!!\n", result);
    return result;
  }

  task.task_signal_ptr = &(riva_port_ptr->riva_signal);
  task.vp_task_arg     = (void*) pDevPort;

  result = afe_dynamic_thread_launch(&(riva_port_ptr->dynamic_thread_ptr),
                                     &task, thread_priority, stack_size, (uint32)pDevPort->intf);
  if (ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Fail to add task, port: %x", pDevPort->intf);
  }

  return result;
}

/**
 * deinitializes sec task
 */
ADSPResult afe_riva_ipc_secondary_task_deinit (afe_dev_port_t *pDevPort)
{
  ADSPResult  result = ADSP_EFAILED;

  if (NULL == pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid device port pointer.");
  }
  else
  {
    if(pDevPort->afe_drv)
    {
      if(((afe_dev_riva_port_t *)(pDevPort->afe_drv))->dynamic_thread_ptr)
      {
        if(ADSP_FAILED(result = afe_dynamic_thread_destroy(
            &(((afe_dev_riva_port_t *)(pDevPort->afe_drv))->dynamic_thread_ptr))))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "FAILED, cannot destroy thread for port: 0x%x",
                pDevPort->intf);
        }
      }
    }
  }

  return result;
}

/**
 * This is a ISR routine to process interrupt from riva to lpass for IPC command.
 *
 * @param[in] none
 */
void afe_driver_riva_cmd_isr(void* null_ptr)
{
  qurt_elite_bufmgr_node_t       buf_mgr_node;
  ADSPResult                     result;
  elite_msg_any_t                msg;
  int                            actual_size;
  elite_msg_custom_riva_ipc_t    *ipc_msg_payload_ptr;
  qurt_elite_queue_t             *cmdq_ptr;

  if (NULL == qurt_elite_globalstate.pAfeStatSvcCmdQ)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDriverRiva: AFE service cmdQ handle is NULL");
    return;
  }

  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_riva_ipc_t), &buf_mgr_node, &actual_size)))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva: Failed to malloc");
    return;
  }
  ipc_msg_payload_ptr = (elite_msg_custom_riva_ipc_t*) buf_mgr_node.pBuffer;

  ipc_msg_payload_ptr->buffer_returnq_ptr  = buf_mgr_node.pReturnQ;
  ipc_msg_payload_ptr->responseq_ptr       = NULL;
  ipc_msg_payload_ptr->client_token        = 0;
  ipc_msg_payload_ptr->sec_op_code         = ELITEMSG_CUSTOM_RIVA_IPC_COMMAND;

  msg.unOpCode = ELITE_CUSTOM_MSG;
  msg.pPayload = (void*) ipc_msg_payload_ptr;

  cmdq_ptr = qurt_elite_globalstate.pAfeStatSvcCmdQ;
  if (ADSP_EOK != (result = qurt_elite_queue_push_back(cmdq_ptr, (uint64_t*)&msg)))
  {
    /* Return the msg buffer */
    elite_msg_release_msg(&msg);
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDriverRiva: FAILED to send Riva IPC msg to AfeSvc");
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDriverRiva: Riva IPC msg sent to AfeSvc");
  }

  return;
}

/**
 * This is a ISR routine to process interrupt from riva to lpass for data movement.
 *
 * @param[in] dev_port_ptr pointer to port instance
 */
void AfeDriver_RivaDataIsr(void* dev_port_ptr)
{
  afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *) dev_port_ptr;
  afe_dev_riva_port_t *riva_drv_ptr = NULL;

  if(NULL != afe_port_ptr)
  {
    riva_drv_ptr = (afe_dev_riva_port_t *)afe_port_ptr->afe_drv;
    if(AFE_RIVA_IPC_CONNECT == riva_drv_ptr->riva_connect)
    {
      //this will be exercised for SW Latching only. Currently by default we will use HW latching.
      //Adding this check not to break if SW latching comes in future.
      //For HW latching This variable is updating with HW time stamp value in the respective
      //secondary thread functions
      if(!(riva_drv_ptr->hw_timestamp_available) )
      {
        riva_drv_ptr->curr_avtimer_timestamp_us = avtimer_drv_get_time((avtimer_drv_handle_t)riva_drv_ptr->avt_drv);
      }
      riva_drv_ptr->dynamic_thread_ptr->isr_context_counter++;
      qurt_elite_signal_send(&riva_drv_ptr->riva_signal);
    }
  }
}

/**
 * This is a ISR routine to process interrupt from riva to read
 * samples from cmem.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 *
 *
 * AUD_SYNC notes: Only BT RX has aud_sync support. BT TX copies the same values of drift.
 * Hence during the below function no special action is necessary.
 */
ADSPResult afe_driver_bt_voc_cmem_read(void *dev_port_void_ptr)
{
  ADSPResult           result = ADSP_EOK;
  uint32_t             temp;
  afe_circ_buff_t      *circ_buff_ptr;
  afe_dev_riva_port_t  *riva_port_ptr;
  afe_dev_port_t       *dev_port_ptr;

  dev_port_ptr   = (afe_dev_port_t *)dev_port_void_ptr;
  riva_port_ptr  = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);
  circ_buff_ptr  = &(riva_port_ptr->circ_buff);

  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  // 3. application processor <-> lpass command process.

  //acquire riva driver mutex as the riva_connect variable can change in STOP command in AFES context
  //which disables AUD SYNC block and to update circ buffer pointers synchronously

  qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));

  //If lpass <-> riva was disconnected, or application processor does not start the port,
  //do nothing.

  if ( (TRUE == aud_stat_afe_riva_ptr->is_riva_active) && \
      (AFE_RIVA_IPC_CONNECT == riva_port_ptr->riva_connect) && \
      (AFE_RIVA_LPASS_DATA_PROCESSING_RUN == riva_port_ptr->riva_lpass_data_proc_state) )
  {
    /* Do aud_sync only on RX side, not on TX side. On TX side, use the same data */
    if (0 == riva_port_ptr->is_mute)
    {
      qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);
      riva_port_ptr->riva_lpass_drift_us         += aud_stat_afe_riva_ptr->drift_correction_us;
      aud_stat_afe_riva_ptr->drift_correction_us = 0;

      /* If HW timestamping available, update s/w latched TS value with h/w latched value */
      if( riva_port_ptr->hw_timestamp_available )
      {
        /* Copy the h/w latched TS value from BT Rx to BT Tx */
        riva_port_ptr->curr_avtimer_timestamp_us = aud_stat_afe_riva_ptr->bt_rx_timestamp;
      }
      qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);

      /* Update the drift structure for each of the VFR sources connected. This is required only for
	         new VFR drift tracking mechanism which works with hardware timestamping */
      if(VFR_HW_LATCH_V2 == vfr_drv_global_stat.hw_latch_ver)
      {
        afe_device_update_port_vfr_drift_info(dev_port_ptr, riva_port_ptr->curr_avtimer_timestamp_us);
      }
    }

    // detect if there is any un-handled jitter in RIVA interrupts and reset the buffer, drift handling
    afe_riva_circ_buf_protect_handler(dev_port_ptr);

    if (riva_port_ptr->is_adjust)
    {
      memset(circ_buff_ptr->top_addr, 0, circ_buff_ptr->buffer_size);
      circ_buff_ptr->head_addr = circ_buff_ptr->top_addr;
      circ_buff_ptr->tail_addr = circ_buff_ptr->top_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
      riva_port_ptr->is_adjust = 0;
    }
    // The head pointer adjustment should be done to the word aligned location in the case of circular buffer underrun
    temp = (dev_port_ptr->channels * dev_port_ptr->bytes_per_channel) << 1;

    //Read samples from cmem and stored in riva_port_ptr->circ_buff.
    afe_driver_linear_to_circ(dev_port_ptr->intf, circ_buff_ptr, (uint8_t*)(riva_port_ptr->cmem_buff_ptr), riva_port_ptr->data_size, temp, TRUE);
  }
  qurt_elite_mutex_unlock(&(riva_port_ptr->riva_drv_mutex));

  return result;
}

/**
 * This is a ISR routine to process interrupt from riva to write
 * samples to cmem.
 *
 * @param[in/out] void
 * @return the ADSP_EOK error code
 *
 * AUD_SYNC notes: Only if port is not mute, adjust the aud_sync block. Store the
 * drift for AV timer correction.
 */
ADSPResult afe_driver_bt_voc_cmem_write(void *dev_port_void_ptr)
{
  ADSPResult           result = ADSP_EOK;
  afe_circ_buff_t      *circ_buff_ptr;
  afe_dev_riva_port_t  *riva_port_ptr;
  afe_dev_port_t       *dev_port_ptr;
  int64_t              drift_correction_us = 0;

  dev_port_ptr  = (afe_dev_port_t *)dev_port_void_ptr;
  riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);
  circ_buff_ptr = &(riva_port_ptr->circ_buff);

  //if following process are in the thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  // 3. application processor <-> lpass command process.

  //acquire riva driver mutex as the riva_connect variable can change in STOP command in AFES context
  //which disables AUD SYNC block
  qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));

  //if lpass <-> riva was disconnected, do nothing.
  if ( (TRUE == aud_stat_afe_riva_ptr->is_riva_active) && \
      (AFE_RIVA_IPC_CONNECT == riva_port_ptr->riva_connect) )
  {
    //do aud_sync only on RX side, not on TX side. this happens even if RX port is not started from app side.
    //riva side opens both together.
    //read aud sync counters & adjust the hardware phase tracker only if it's not mute

    if (0 == riva_port_ptr->is_mute)
    {
      /* If HW timestamping available, update s/w latched TS value with h/w latched value */
      if( riva_port_ptr->hw_timestamp_available )
      {
        if ((AFE_TS_INVALID_MUX_ID != riva_port_ptr->hw_ts_mux_id))
        {
          /* Read hardware register to get timestamp */
          riva_port_ptr->curr_avtimer_timestamp_us = afe_device_read_timestamp_register(riva_port_ptr->hw_ts_mux_id);
        }
      }

      afe_riva_aud_sync(dev_port_ptr->intf, HAL_AUD_SYNC_INTF_BT_1, \
                        &riva_port_ptr->riva_lpass_drift_us, riva_port_ptr->us_per_sample,\
                        &riva_port_ptr->is_first_riva_lpass_sync, &drift_correction_us);

      /* Store for TX port's use */
      qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);
      aud_stat_afe_riva_ptr->drift_correction_us = drift_correction_us;
      aud_stat_afe_riva_ptr->bt_rx_timestamp = riva_port_ptr->curr_avtimer_timestamp_us;
      qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);

      /* Now update the VFR drift variables for all connected VFRs. This is required only in the new VFR
            drift tracking method which works when the hardwaretimestamp feature is enabled */
      if(VFR_HW_LATCH_V2 == vfr_drv_global_stat.hw_latch_ver)
      {
        afe_device_update_port_vfr_drift_info(dev_port_ptr, riva_port_ptr->curr_avtimer_timestamp_us);
      }
    }

    // detect if there is any un-handled jitter in RIVA interrupts and reset the bufer, drift handling
    afe_riva_circ_buf_protect_handler(dev_port_ptr);

    if ((riva_port_ptr->is_mute) || (AFE_RIVA_LPASS_DATA_PROCESSING_STOP == riva_port_ptr->riva_lpass_data_proc_state))
    {
      //if it's mute or port stopped, send zeros
      memset(riva_port_ptr->cmem_buff_ptr, 0, riva_port_ptr->data_size);
    }
    else
    {
      if (riva_port_ptr->is_adjust)
      {
        memset(circ_buff_ptr->top_addr, 0, circ_buff_ptr->buffer_size);
        circ_buff_ptr->tail_addr = circ_buff_ptr->top_addr;
        circ_buff_ptr->head_addr = circ_buff_ptr->top_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
        riva_port_ptr->is_adjust = 0;
      }
      //Write samples to cmem from riva_port_ptr->circ_buff.
      afe_driver_circ_to_linear(dev_port_ptr->intf, (uint8_t*) (riva_port_ptr->cmem_buff_ptr), circ_buff_ptr, riva_port_ptr->data_size, TRUE);
    }
  }
  //No need to clear interrupts, since interrupts are automatically cleared
  //and disabled when it is taken by QDSP6 core
  qurt_elite_mutex_unlock(&(riva_port_ptr->riva_drv_mutex));

  return result;
}

/**
 * This is an ISR routine to process interrupt from riva to write
 * samples to cmem for FM Rx.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 *
 * AUD_SYNC notes: only one of FM read or write is called. find the drifts between sample counter-reference counter & correct
 */
ADSPResult afe_driver_fm_aud_cmem_write(void *dev_port_void_ptr)
{
  ADSPResult           result = 0;
  uint32_t             temp;
  int32_t              tail_head_gap;
  afe_circ_buff_t      *circ_buff_ptr;
  afe_circ_buff_t      *cmem_circ_buff_ptr;
  afe_dev_riva_port_t  *riva_port_ptr;
  afe_dev_port_t       *dev_port_ptr;
  int64_t              drift_correction_us; //useful for bt only, not for fm.

  dev_port_ptr       = (afe_dev_port_t *)dev_port_void_ptr;
  riva_port_ptr      = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);
  circ_buff_ptr      = &(riva_port_ptr->circ_buff);
  cmem_circ_buff_ptr = &(riva_port_ptr->cmem_circ_buff);


  //if following process are in the thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  // 3. application processor <-> lpass command process.

  //acquire riva driver mutex as the riva_connect variable can change in STOP command in AFES context
  //which disables AUD SYNC block and to update circ buffer pointers synchronously

  qurt_elite_mutex_lock( &(riva_port_ptr->riva_drv_mutex));

  if ( (TRUE == aud_stat_afe_riva_ptr->is_riva_active) && \
      (AFE_RIVA_IPC_CONNECT == riva_port_ptr->riva_connect) )
  {
    afe_riva_aud_sync(dev_port_ptr->intf, HAL_AUD_SYNC_INTF_FM, \
                      &riva_port_ptr->riva_lpass_drift_us, riva_port_ptr->us_per_sample, \
                      &riva_port_ptr->is_first_riva_lpass_sync, &drift_correction_us);

    if(riva_port_ptr->hw_timestamp_available)
    {
      /* Read hardware register to get timestamp */
      if ((AFE_TS_INVALID_MUX_ID != riva_port_ptr->hw_ts_mux_id))
      {
        /* Read hardware register to get timestamp */
        riva_port_ptr->curr_avtimer_timestamp_us = afe_device_read_timestamp_register(riva_port_ptr->hw_ts_mux_id);
      }
    }

    if(VFR_HW_LATCH_V2 == vfr_drv_global_stat.hw_latch_ver)
    {
      //now update the VFR drift variables for all connected VFRs
      afe_device_update_port_vfr_drift_info(dev_port_ptr, riva_port_ptr->curr_avtimer_timestamp_us);
    }

    //Check the tail/head circular distance, if
    // 1. there is enough space in cmem circular buffer, write the samples.
    // 2. otherwise, write zeros.
    // The head pointer adjustment should be done to the word aligned location in the case of circular buffer underrun
    temp = (dev_port_ptr->bytes_per_channel * dev_port_ptr->channels) << 1;
    cmem_circ_buff_ptr->head_addr =  cmem_circ_buff_ptr->top_addr + ((*(riva_port_ptr->head_reg_ptr)) <<2) + 4;

    if ((cmem_circ_buff_ptr->head_addr) >= (cmem_circ_buff_ptr->bottom_addr) )
    {
      cmem_circ_buff_ptr->head_addr = cmem_circ_buff_ptr->top_addr;
    }

    // RESET\Initialize the producer pointer w.r.t consumer ptr for the first time
    if(TRUE == riva_port_ptr->reset_cmem_ptrs)
    {
      // detect the head pointer position in double buffer and use other buffer for producer
      if ((cmem_circ_buff_ptr->head_addr) >= (cmem_circ_buff_ptr->top_addr+riva_port_ptr->data_size))
      {
        cmem_circ_buff_ptr->tail_addr    = cmem_circ_buff_ptr->top_addr;
      }
      else
      {
        cmem_circ_buff_ptr->tail_addr    = cmem_circ_buff_ptr->top_addr + riva_port_ptr->data_size;
      }
      *(riva_port_ptr->tail_reg_ptr) = (uint32_t)(cmem_circ_buff_ptr->tail_addr - cmem_circ_buff_ptr->top_addr)>>2;
      riva_port_ptr->reset_cmem_ptrs = FALSE;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM CMEM ptrs RESET, H: %lx, T: %lx", (uint32_t)cmem_circ_buff_ptr->head_addr, (uint32_t)cmem_circ_buff_ptr->tail_addr);
    }

    tail_head_gap = (int32_t)(cmem_circ_buff_ptr->tail_addr - cmem_circ_buff_ptr->head_addr);

    if (tail_head_gap < 0)
    {
      tail_head_gap += cmem_circ_buff_ptr->buffer_size;
    }

    // MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM CMEM Write: Head: %x, Tail: %x", cmem_circ_buff_ptr->head_addr, cmem_circ_buff_ptr->tail_addr);

    //Check the port state, if
    // 1. port is not in run state, copy zeros to cmem.
    // 2. otherwise, write the port circular buffer to cmem.

    if (AFE_RIVA_LPASS_DATA_PROCESSING_STOP == riva_port_ptr->riva_lpass_data_proc_state)
    {
      if (tail_head_gap <= (int32_t) (cmem_circ_buff_ptr->buffer_size - riva_port_ptr->data_size))
      {
        // If there is another space to store in cmem circular buffer,
        // write samples to circular cmem from riva_port_ptr->circ_buff.
        afe_driver_linear_to_circ(dev_port_ptr->intf, cmem_circ_buff_ptr, NULL, riva_port_ptr->data_size, temp, TRUE);
        *(riva_port_ptr->tail_reg_ptr) = (uint32_t)(cmem_circ_buff_ptr->tail_addr - cmem_circ_buff_ptr->top_addr)>>2;
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No sufficient space to fill, port_id: %d", dev_port_ptr->intf);
      }
    }
    else
    {
      if (riva_port_ptr->is_adjust)
      {
        // If it's first copy, then resync the read/write pointers of port circular buffer
        memset(circ_buff_ptr->top_addr, 0, circ_buff_ptr->buffer_size);
        circ_buff_ptr->tail_addr = circ_buff_ptr->top_addr;
        circ_buff_ptr->head_addr = circ_buff_ptr->top_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
        riva_port_ptr->is_adjust = 0;
      }
      if (tail_head_gap <= (int32_t) (cmem_circ_buff_ptr->buffer_size - riva_port_ptr->data_size))
      {
        // If there is another space to store in cmem circular buffer,
        // write samples to circular cmem from riva_port_ptr->circ_buff.
        afe_driver_circ_to_circ(dev_port_ptr->intf, cmem_circ_buff_ptr, circ_buff_ptr, riva_port_ptr->data_size, temp, TRUE);
        *(riva_port_ptr->tail_reg_ptr) = (uint32_t)(cmem_circ_buff_ptr->tail_addr - cmem_circ_buff_ptr->top_addr)>>2;
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No sufficient space to fill, port_id: %d", dev_port_ptr->intf);
      }
    }
  }

  qurt_elite_mutex_unlock( &(riva_port_ptr->riva_drv_mutex));
  return result;
}

/**
 * This is a ISR routine to process interrupt from riva to read
 * samples from cmem for FM Tx.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 *
 * AUD_SYNC notes: only one of FM read or write is called. find the drifts between sample counter-reference counter & correct
 */
ADSPResult afe_driver_fm_aud_cmem_read(void *dev_port_void_ptr)
{
  ADSPResult           result = ADSP_EOK;
  uint32_t             temp;
  afe_circ_buff_t      *circ_buff_ptr;
  afe_circ_buff_t      *cmem_circ_buff_ptr;
  afe_dev_riva_port_t  *riva_port_ptr;
  afe_dev_port_t       *dev_port_ptr;
  int64_t              drift_correction_us; //useful for bt only, not for fm.

  dev_port_ptr         = (afe_dev_port_t *)dev_port_void_ptr;
  riva_port_ptr        = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);
  circ_buff_ptr        = &(riva_port_ptr->circ_buff);
  cmem_circ_buff_ptr   = &(riva_port_ptr->cmem_circ_buff);


  //if following process are in the thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  // 3. application processor <-> lpass command process.

  // this mutex is acquiring becasue this riva_connect can be changed from AFES thread
  // while executing STOP commands and can disable aud sync block.
  qurt_elite_mutex_lock( &(riva_port_ptr->riva_drv_mutex));

  if((TRUE == aud_stat_afe_riva_ptr->is_riva_active) && \
      (AFE_RIVA_IPC_CONNECT == riva_port_ptr->riva_connect) )
  {
    afe_riva_aud_sync(dev_port_ptr->intf, HAL_AUD_SYNC_INTF_FM, \
                      &riva_port_ptr->riva_lpass_drift_us, riva_port_ptr->us_per_sample, \
                      &riva_port_ptr->is_first_riva_lpass_sync, &drift_correction_us);

    if(riva_port_ptr->hw_timestamp_available)
    {
      /* Read hardware register to get timestamp */
      if ((AFE_TS_INVALID_MUX_ID != riva_port_ptr->hw_ts_mux_id))
      {
        /* Read hardware register to get timestamp */
        riva_port_ptr->curr_avtimer_timestamp_us = afe_device_read_timestamp_register(riva_port_ptr->hw_ts_mux_id);
      }

      if(VFR_HW_LATCH_V2 == vfr_drv_global_stat.hw_latch_ver)
      {
        /* Now update the VFR drift variables for all connected VFRs */
        afe_device_update_port_vfr_drift_info(dev_port_ptr, riva_port_ptr->curr_avtimer_timestamp_us);
      }
    }

    // The head pointer adjustment should be done to the word aligned location in the case of circular buffer underrun
    temp = (dev_port_ptr->bytes_per_channel * dev_port_ptr->channels) << 1;

    cmem_circ_buff_ptr->tail_addr =  cmem_circ_buff_ptr->top_addr + (*(riva_port_ptr->tail_reg_ptr)<<2) + 4; //TODO: multiply by channel and bytes per channel

    //For sanity check, to see if the offset is valid one.
    if ((cmem_circ_buff_ptr->tail_addr) >= (cmem_circ_buff_ptr->bottom_addr) )
    {
      cmem_circ_buff_ptr->tail_addr = cmem_circ_buff_ptr->top_addr;
    }

    // RESET\Initialize the consumer pointer w.r.t producer ptr for the first time
    if(TRUE == riva_port_ptr->reset_cmem_ptrs)
    {
      // detect the head pointer position in double buffer and use other buffer for producer
      if ( (cmem_circ_buff_ptr->tail_addr) >= (cmem_circ_buff_ptr->top_addr + riva_port_ptr->data_size))
      {
        cmem_circ_buff_ptr->head_addr    = cmem_circ_buff_ptr->top_addr;
      }
      else
      {
        cmem_circ_buff_ptr->head_addr    = cmem_circ_buff_ptr->top_addr + riva_port_ptr->data_size;
      }
      *(riva_port_ptr->head_reg_ptr) = (uint32_t)(cmem_circ_buff_ptr->head_addr - cmem_circ_buff_ptr->top_addr)>>2;
      riva_port_ptr->reset_cmem_ptrs = FALSE;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM CMEM ptrs RESET, H: %lx, T: %lx", (uint32_t) cmem_circ_buff_ptr->head_addr,(uint32_t)cmem_circ_buff_ptr->tail_addr);
    }

    //MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM CMEM READ: Head: %x, Tail: %x", cmem_circ_buff_ptr->head_addr, cmem_circ_buff_ptr->tail_addr);

    //Check the port state, if
    // 1. port is not in run state, drop samples and update the cmem head address register
    // 2. otherwise, read circular buffer from cmem


    if (AFE_RIVA_LPASS_DATA_PROCESSING_STOP == riva_port_ptr->riva_lpass_data_proc_state)
    {
      // update the head ptr
      cmem_circ_buff_ptr->head_addr += riva_port_ptr->data_size;
      if ((cmem_circ_buff_ptr->head_addr) >= (cmem_circ_buff_ptr->bottom_addr))
      {
        cmem_circ_buff_ptr->head_addr = cmem_circ_buff_ptr->top_addr;
      }
    }
    else
    {
      //Check the tail/head circular distance, if distance
      // 1. less or equal half of buffer size, write the samples.
      // 2. larger than half of buffer size, drop the samples.
      if (riva_port_ptr->is_adjust)
      {
        memset(circ_buff_ptr->top_addr, 0, circ_buff_ptr->buffer_size);
        circ_buff_ptr->head_addr = circ_buff_ptr->top_addr;
        circ_buff_ptr->tail_addr = circ_buff_ptr->top_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
        riva_port_ptr->is_adjust = 0;
      }

      //Read samples from circular cmem to cmem_circ_buff_ptr.
      afe_driver_circ_to_circ(dev_port_ptr->intf, circ_buff_ptr, cmem_circ_buff_ptr, riva_port_ptr->data_size, temp, TRUE);
    }

    //Update the head address register value.
    *(riva_port_ptr->head_reg_ptr) = (uint32_t)(cmem_circ_buff_ptr->head_addr - cmem_circ_buff_ptr->top_addr)>>2;
  }

  qurt_elite_mutex_unlock( &(riva_port_ptr->riva_drv_mutex));
  return result;
}

/**
 * Process the IPC command from Riva, and update the riva port, BT Voc/Aud, FM
 * instance.
 *
 * @param[in] afe_svc_ptr pointer to AfeSvc instance
 */

ADSPResult afe_driver_ipc_cmd_proc(aud_stat_afe_svc_t *afe_svc_ptr)
{
  ADSPResult result = 0;
  uint32_t cmd_id, ipc_mutex_bit, virt_addr;

  // IPC msg
  afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr;
  afe_ipc_send_msg_t *ipc_send_msg_ptr;

  // Port type in the msg contents
  afe_dev_port_t *afe_bt_aud_ptr;
  afe_dev_port_t *afe_bt_voc_tx_ptr;
  afe_dev_port_t *afe_bt_voc_rx_ptr;
  afe_dev_port_t *afe_fm_tx_ptr;
  afe_dev_port_t *afe_fm_rx_ptr;

  afe_bt_aud_ptr    = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_A2DP_RX)];
  afe_bt_voc_tx_ptr = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_SCO_TX)];
  afe_bt_voc_rx_ptr = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_SCO_RX)];
  afe_fm_tx_ptr     = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_FM_TX)];
  afe_fm_rx_ptr     = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_FM_RX)];

  aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr = *(uint32 *)aud_stat_afe_riva_ptr->ccu_mb0_ctrl_reg_virt_addr;

  /* At this point, we have the physical address of rcv_mailbox_addr.
   * Now query for its virtual address*/
  if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                      aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr,
                                                                      &virt_addr,
                                                                      riva_mem_perm)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE RivaIPC: Failed to get VA for Rcv mailbox address PA: 0x%lx",
          aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr);

    return ADSP_EFAILED;
  }

  aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr = virt_addr;

  aud_stat_afe_riva_ptr->riva_mailbox.send_mailbox_addr = (uint32_t)((aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr) + aud_stat_afe_riva_ptr->ipc_send_mail_box_offset);
  ipc_rcv_msg_ptr   = (afe_ipc_rcv_msg_t *)(aud_stat_afe_riva_ptr->riva_mailbox.rcv_mailbox_addr);
  ipc_send_msg_ptr  = (afe_ipc_send_msg_t *)(aud_stat_afe_riva_ptr->riva_mailbox.send_mailbox_addr);

  ipc_mutex_bit = (ipc_rcv_msg_ptr->header & (~AFE_IPC_MSG_CMD_ID));

  /* Check if IPC msg mutex bit is to 1 */
  if (0 == ipc_mutex_bit)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva IPC: IPC mutex bit is not set, ipc_header: 0x%lx",
          ipc_rcv_msg_ptr->header);
    return ADSP_EFAILED;
  }

  // Read the command ID from msg.
  cmd_id = ipc_rcv_msg_ptr->header & AFE_IPC_MSG_CMD_ID;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva: IPC: cmd_id=%lx, ipc_rcv_msg_ptr=%lx",cmd_id, (uint32_t)ipc_rcv_msg_ptr);

  switch (cmd_id)
  {

    case RIVA_BT_VOICE_START:
      afe_riva_bt_voice_start(ipc_rcv_msg_ptr, afe_bt_voc_tx_ptr, afe_bt_voc_rx_ptr);
      break;

    case RIVA_BT_VOICE_STOP:
      afe_riva_bt_voice_stop(afe_bt_voc_tx_ptr, afe_bt_voc_rx_ptr);
      break;

    case RIVA_BT_VOICE_MUTE:
      afe_riva_bt_voice_mute(afe_bt_voc_tx_ptr, afe_bt_voc_rx_ptr);
      break;

    case RIVA_BT_VOICE_UNMUTE:
      afe_riva_bt_voice_unmute(ipc_rcv_msg_ptr, afe_bt_voc_tx_ptr, afe_bt_voc_rx_ptr);
      break;

    case RIVA_BT_AUDIO_START:
      afe_riva_bt_audio_start(ipc_rcv_msg_ptr, afe_bt_aud_ptr);
      break;

    case RIVA_BT_AUDIO_STOP:
      afe_riva_bt_audio_stop(afe_bt_aud_ptr);
      break;

    case RIVA_FM_TX_AUDIO_START:
      afe_riva_fm_start(ipc_rcv_msg_ptr, afe_fm_tx_ptr);
      break;

    case RIVA_FM_RX_AUDIO_START:
      afe_riva_fm_start(ipc_rcv_msg_ptr, afe_fm_rx_ptr);
      break;

    case RIVA_FM_TX_AUDIO_STOP:
      afe_riva_fm_stop(afe_fm_tx_ptr);
      break;
    case RIVA_FM_RX_AUDIO_STOP:
      afe_riva_fm_stop(afe_fm_rx_ptr);
      break;

    default:
      result = 1;
      break;
  }

  ipc_send_msg_ptr->result = result;

  // send the ack back riva.
  afe_driver_ipc_ack_riva(ipc_rcv_msg_ptr, ipc_send_msg_ptr);
  return result;
}

/* Restarts the riva port
 * */
ADSPResult afe_riva_restart_the_port(afe_dev_port_t *pDevPort)
{
  ADSPResult  result = ADSP_EFAILED;

  if (NULL == pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE RIVA PORT RESTART FAILED, AFE port NULL pointer access");
    return result;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RIVA port restart started, port_id: 0x%lx", pDevPort->intf);

  afe_port_mutex_lock(pDevPort);
  pDevPort->dev_proc_seq = AFE_DEV_RESTART;

  /* Stop the device interface */
  if(ADSP_EOK != (result = afe_port_modules_deinit_and_dev_stop(pDevPort)))
  {
    afe_port_mutex_unlock(pDevPort);
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE RIVA PORT RESTART FAILED, failed to stop the device, port 0x%X", pDevPort->intf);
    goto _riva_restart_bail_out;
  }
  afe_port_mutex_unlock(pDevPort);

  /* Start the device interface */
  /*No need to acquire mutex to perform modules init and dev start, since now port is in STOP state. */
  /* ADSP_ECONTINUE will be for h/w mad or s/w duty enabled case */
  if(ADSP_ECONTINUE == (result = afe_port_modules_init_and_dev_start(pDevPort)))
  {
    result = ADSP_EOK;
  }
  else if(ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE RIVA PORT RESTART FAILED, failed to start the device, port 0x%X", pDevPort->intf);

    /* Set the port state to CONFIG.
     * This is needed to execute stop cmd sequence completely. */
    pDevPort->dev_proc_seq = AFE_NORMAL_SEQ;

    pDevPort->port_state = AFE_PORT_STATE_CONFIG;

    /* execute stop cmd sequence */
    afe_port_apr_dma_stop(pDevPort, NULL);

    goto _riva_restart_bail_out;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE RIVA PORT RESTART succesfully completed, port_id: 0x%lx", pDevPort->intf);

  _riva_restart_bail_out:
  afe_port_mutex_lock(pDevPort);
  pDevPort->dev_proc_seq = AFE_NORMAL_SEQ;
  afe_port_mutex_unlock(pDevPort);
  return result;
}

/**
 * AUD_SYNC notes: aud sync block is started
 * if afe port open happens after riva is opened, then av timer won't correct
   if afe port open happens before riva is opened, then initially AV timer uses 0 drift correction.
   note: hardware supports only BT RX side correction. RX and TX sides are inherently sync'ed at riva
 */
void afe_riva_bt_voice_start(afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr, afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr)
{
  char_t thread_name[16];
  ADSPResult result = 0;
  uint32 virt_addr = 0;
  // Get the payload of the msg.
  afe_ipc_rcv_bt_voc_msg_t *bt_voc_msg_ptr = (afe_ipc_rcv_bt_voc_msg_t *) ipc_rcv_msg_ptr;
  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  afe_dev_riva_port_t *tx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_tx_ptr->afe_drv);
  afe_dev_riva_port_t *rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_rx_ptr->afe_drv);

  if ((NULL == bt_voc_msg_ptr->tx_cmem_buff_ptr) || (NULL == bt_voc_msg_ptr->rx_cmem_buff_ptr))
  {
    MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"bt_voice_start: Tx or Rx CMEM buffer ptr is NULL, Tx buffer ptr:0x%x, Rx buffer ptr:0x%x", \
          bt_voc_msg_ptr->tx_cmem_buff_ptr, bt_voc_msg_ptr->rx_cmem_buff_ptr);
    return;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bt_voice_start:port_state_rx=%d,port_state_tx=%d ", afe_bt_voc_rx_ptr->port_state, afe_bt_voc_tx_ptr->port_state);

  if ((TRUE == aud_stat_afe_riva_ptr->is_riva_active) &&      //since some reg are read.
      (AFE_RIVA_IPC_WAIT == tx_riva_port_ptr->riva_connect))
  {
    // Get the CMEM buffer address
    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(bt_voc_msg_ptr->tx_cmem_buff_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT_VOC_START: Failed to get VA for Tx CMEM buf PA: 0x%lx",
            bt_voc_msg_ptr->tx_cmem_buff_ptr);

      goto voc_start_bail_out;
    }
    tx_riva_port_ptr->cmem_buff_ptr = (int16_t *)virt_addr;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(bt_voc_msg_ptr->rx_cmem_buff_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT_VOC_START: Failed to get VA for Rx CMEM buf PA: 0x%lx",
            bt_voc_msg_ptr->rx_cmem_buff_ptr);

      goto voc_start_bail_out;
    }
    rx_riva_port_ptr->cmem_buff_ptr = (int16_t *)virt_addr;

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bt_voice_start: Rx port virt addr = 0x%lx , phys addr = 0x%lx",
          (uint32)(rx_riva_port_ptr->cmem_buff_ptr), (uint32)(bt_voc_msg_ptr->rx_cmem_buff_ptr));
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bt_voice_start: Tx port virt addr = 0x%lx , phys addr = 0x%lx",
          (uint32)(tx_riva_port_ptr->cmem_buff_ptr), (uint32)(bt_voc_msg_ptr->tx_cmem_buff_ptr));

    // Get the CMEM buffer size
    switch(bt_voc_msg_ptr->period)
    {
      case 1250:
      {
        tx_riva_port_ptr->data_size = 20; //1.25*2*8 = 20;
        // threshold to detect unhandled jitters in RIVA inteerupts.
        // 0.35ms is choosen, typically it won't go beyond few micro sec.
        tx_riva_port_ptr->riva_ipc_int_reset_thrh_us = 1600;
        break;
      }
      case 2500:
      {
        tx_riva_port_ptr->data_size = 40; //2.5*2*8 = 40
        // threshold to detect unhandled jitters in RIVA inteerupts.
        // 0.35ms is choosen, typically it won't go beyond few micro sec
        tx_riva_port_ptr->riva_ipc_int_reset_thrh_us = 2850;
        break;
      }
      case 3750:
      {
        tx_riva_port_ptr->data_size = 60; //3.75*2*8 = 60
        // threshold to detect unhandled jitters in RIVA inteerupts.
        // 0.35ms is choosen, typically it won't go beyond few micro sec
        tx_riva_port_ptr->riva_ipc_int_reset_thrh_us = 4100;
        break;
      }
      case 7500:
      {
        tx_riva_port_ptr->data_size = 120; //7.5*2*8 = 120
        // threshold to detect unhandled jitters in RIVA inteerupts.
        // 0.35ms is choosen, typically it won't go beyond few micro sec
        tx_riva_port_ptr->riva_ipc_int_reset_thrh_us = 7850;
        break;
      }
      default:
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR : BT_VOC_START, Invalid RIVA interrupt period : %lu", bt_voc_msg_ptr->period);
        goto voc_start_bail_out;
      }
    }

    if (16000 == bt_voc_msg_ptr->sample_rate)
    {
      tx_riva_port_ptr->data_size *= 2;
    }
    rx_riva_port_ptr->data_size = tx_riva_port_ptr->data_size;

    // initialize the threshold value for Rx RIVA port
    rx_riva_port_ptr->riva_ipc_int_reset_thrh_us = tx_riva_port_ptr->riva_ipc_int_reset_thrh_us;

    //If port stat is in run, check the sample rate match.
    if (AFE_PORT_STATE_RUN == afe_bt_voc_tx_ptr->port_state)
    {
      if (afe_bt_voc_tx_ptr->sample_rate     != bt_voc_msg_ptr->sample_rate ||
          tx_riva_port_ptr->riva_intr_period  != bt_voc_msg_ptr->period)
      {
        afe_bt_voc_tx_ptr->sample_rate      = bt_voc_msg_ptr->sample_rate;
        afe_bt_voc_tx_ptr->dev_sample_rate  = bt_voc_msg_ptr->sample_rate;
        tx_riva_port_ptr->riva_intr_period  = bt_voc_msg_ptr->period;

        /* Since sample rate of the running port is going to change,
         * so, need to reallocate the working buffers as per new sampling rate.
         * By performing device restart, all the intended operations will be taken care*/
        if(ADSP_EOK != afe_riva_restart_the_port(afe_bt_voc_tx_ptr))
        {
          return;
        }
      }
    }
    else
    {
      afe_bt_voc_tx_ptr->sample_rate = bt_voc_msg_ptr->sample_rate;
      afe_bt_voc_tx_ptr->dev_sample_rate = bt_voc_msg_ptr->sample_rate;
      tx_riva_port_ptr->riva_intr_period = bt_voc_msg_ptr->period;
      afe_riva_set_sample_rate_params(tx_riva_port_ptr, afe_bt_voc_tx_ptr);
    }

    //If port state is in run, check the sample rate match.
    if (AFE_PORT_STATE_RUN == afe_bt_voc_rx_ptr->port_state)
    {
      if (afe_bt_voc_rx_ptr->sample_rate     != bt_voc_msg_ptr->sample_rate ||
          rx_riva_port_ptr->riva_intr_period != bt_voc_msg_ptr->period)
      {
        afe_bt_voc_rx_ptr->sample_rate      = bt_voc_msg_ptr->sample_rate;
        afe_bt_voc_rx_ptr->dev_sample_rate      = bt_voc_msg_ptr->sample_rate;
        rx_riva_port_ptr->riva_intr_period  = bt_voc_msg_ptr->period;

        /* Since sample rate of the running port is going to change,
         * so, need to reallocate the working buffers as per new sampling rate.
         * By performing device restart, all the intended operations will be taken care*/
        if(ADSP_EOK != afe_riva_restart_the_port(afe_bt_voc_rx_ptr))
        {
          return;
        }
      }
    }
    else
    {
      afe_bt_voc_rx_ptr->sample_rate       = bt_voc_msg_ptr->sample_rate;
      afe_bt_voc_rx_ptr->dev_sample_rate      = bt_voc_msg_ptr->sample_rate;
      rx_riva_port_ptr->riva_intr_period   = bt_voc_msg_ptr->period;
      afe_riva_set_sample_rate_params(rx_riva_port_ptr, afe_bt_voc_rx_ptr);
    }

    //no need to acquire mutex here as the MMPM config will be done from same thread AFeS
    result |= afe_mmpm_voting(afe_bt_voc_rx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                              AFE_MMPM_VOTE, TRUE);
    result |= afe_mmpm_voting(afe_bt_voc_tx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                              AFE_MMPM_VOTE, TRUE);
    if (ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM request failed and port is deregistering with MMPM, port-id: 0x%x", afe_bt_voc_rx_ptr->intf);
      if(ADSP_EOK != afe_mmpm_voting(afe_bt_voc_rx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                                     AFE_MMPM_DEVOTE, TRUE))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM port deregistering with MMPM is failed, port-id: 0x%x", afe_bt_voc_rx_ptr->intf);
      }

      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM request failed and port is deregistering with MMPM, port-id: 0x%x", afe_bt_voc_tx_ptr->intf);

      if(ADSP_EOK != afe_mmpm_voting(afe_bt_voc_tx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                                     AFE_MMPM_DEVOTE, TRUE))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM port deregistering with MMPM is failed, port-id: 0x%x", afe_bt_voc_tx_ptr->intf);
      }
      goto voc_start_bail_out;
    }

    /* If Timestamp hardware is available, configure the TS mux */
    if(aud_stat_afe_riva_ptr->hw_timestamp_available)
    {
      /* Configure TS block for internal BT Rx port */
      afe_ts_drv_config_vfr_ts_mux(HW_TS_INTR_TYPE_BT_RX,
                                   0,
                                   &rx_riva_port_ptr->hw_ts_mux_id);

      tx_riva_port_ptr->hw_ts_mux_id = rx_riva_port_ptr->hw_ts_mux_id;

      /* Initialize the Rx timestamp value to 0 */
      aud_stat_afe_riva_ptr->bt_rx_timestamp = 0;

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HW TS Block configured, BT_RX mux_id: 0x%lx", rx_riva_port_ptr->hw_ts_mux_id);
    }

    //start audio synchronization between riva & lpass
    if (ADSP_EOK != (result = afe_riva_start_bt_aud_sync(afe_bt_voc_rx_ptr->sample_rate, tx_riva_port_ptr, rx_riva_port_ptr)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT AudSync config fails");
      goto voc_start_bail_out;
    }

    afe_riva_add_to_vfr_sync(afe_bt_voc_rx_ptr);
    afe_riva_add_to_vfr_sync(afe_bt_voc_tx_ptr);

    /* Initialize previous time stamps, current time stamps will be collected in first RIVA interrupt */
    tx_riva_port_ptr->prev_avtimer_timestamp_us = avtimer_drv_get_time((avtimer_drv_handle_t)tx_riva_port_ptr->avt_drv);
    rx_riva_port_ptr->prev_avtimer_timestamp_us = tx_riva_port_ptr->prev_avtimer_timestamp_us;



    // Secondary task on the threadpool:- RIVA triggered
    if (ADSP_EOK != (result = afe_riva_ipc_secondary_task_init(afe_bt_voc_rx_ptr)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RIVA Voc Rx port fails to init secondary task");
      goto voc_start_bail_out;
    }
    if (ADSP_EOK != (result = afe_riva_ipc_secondary_task_init(afe_bt_voc_tx_ptr)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RIVA Voc Tx port fails to init secondary task");
      goto voc_start_bail_out;
    }

    // update the ipc connection status
    tx_riva_port_ptr->riva_connect =  AFE_RIVA_IPC_CONNECT;
    rx_riva_port_ptr->riva_connect =  AFE_RIVA_IPC_CONNECT;

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_START: added port port_id=%x & %x to vfr sync & to aud_sync",afe_bt_voc_rx_ptr->intf, afe_bt_voc_tx_ptr->intf);
    MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_START: %x.riva_intr_period=%ld,data_size=%ld,int_samples_per_period=%ld,samples_per_ms=%d,sample_rate=%ld,us_per_sample=%lu ",\
          afe_bt_voc_rx_ptr->intf, rx_riva_port_ptr->riva_intr_period, \
          rx_riva_port_ptr->data_size,afe_bt_voc_rx_ptr->int_samples_per_period,rx_riva_port_ptr->samples_per_ms,afe_bt_voc_rx_ptr->sample_rate, rx_riva_port_ptr->us_per_sample);

    // To enable interrupt in the last step to escape race condition.
    qurt_elite_strl_cpy(thread_name,"AfeBtVocRxIsr", sizeof(thread_name));
    if(ADSP_EOK
        != (result = qurt_elite_interrupt_register(&(rx_riva_port_ptr->intr_struct),
                                                   aud_stat_afe_riva_ptr->bt_rx_int,
                                                   AfeDriver_RivaDataIsr,
                                                   (void*)afe_bt_voc_rx_ptr, thread_name,
                                                   AFE_INTERNAL_BTFM_DATA_IST_STACK_SIZE)))
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva: Failed to register afe_bt_voc_rx interrupt");
      goto voc_start_bail_out;
    }

    qurt_elite_strl_cpy(thread_name,"AfeBtVocTxIsr", sizeof(thread_name));
    if(ADSP_EOK
        != (result = qurt_elite_interrupt_register(&(tx_riva_port_ptr->intr_struct),
                                                   aud_stat_afe_riva_ptr->bt_tx_int,
                                                   AfeDriver_RivaDataIsr,
                                                   (void*)afe_bt_voc_tx_ptr, thread_name,
                                                   AFE_INTERNAL_BTFM_DATA_IST_STACK_SIZE)))
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva: Failed to register afe_bt_voc_tx interrupt");
    }
  }

  voc_start_bail_out:
  return;
}

/**
 * AUD_SYNC notes: aud_sync block is stopped.
 * if afe port close happens after riva is closed, then av timer uses 0 drift correction
   if afe port close happens before riva is opened, then av timer is closed first & drift correction doesn't matter.
   note: hardware supports only BT RX side correction. RX and TX sides are inherently sync'ed at riva
 */
void afe_riva_bt_voice_stop(afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr)
{
  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  ADSPResult result = ADSP_EOK;

  afe_dev_riva_port_t *tx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_tx_ptr->afe_drv);
  afe_dev_riva_port_t *rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_rx_ptr->afe_drv);

  if(AFE_RIVA_IPC_WAIT == tx_riva_port_ptr->riva_connect)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_STOP: Not in IPC connect state");
    return;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "bt_voice_stop:port_state_rx=%d,port_state_tx=%d ",
        afe_bt_voc_rx_ptr->port_state, afe_bt_voc_tx_ptr->port_state);

  qurt_elite_mutex_lock( &(tx_riva_port_ptr->riva_drv_mutex));
  // update the ipc connection status
  tx_riva_port_ptr->riva_connect = AFE_RIVA_IPC_WAIT;
  qurt_elite_mutex_unlock( &(tx_riva_port_ptr->riva_drv_mutex));

  qurt_elite_mutex_lock( &(rx_riva_port_ptr->riva_drv_mutex));
  rx_riva_port_ptr->riva_connect = AFE_RIVA_IPC_WAIT;
  qurt_elite_mutex_unlock( &(rx_riva_port_ptr->riva_drv_mutex));

  /* disable the 3.75/7.5 ms periodical bt interrupt
   * disable interrupt in the first step to decrease race condition.
   */
  qurt_elite_interrupt_unregister(&(rx_riva_port_ptr->intr_struct));
  qurt_elite_interrupt_unregister(&(tx_riva_port_ptr->intr_struct));

  /* Release BT Tx and Rx timestamp muxes */
  if(aud_stat_afe_riva_ptr->hw_timestamp_available)
  {
    afe_ts_drv_release_ts_mux(rx_riva_port_ptr->hw_ts_mux_id);
    afe_ts_drv_release_ts_mux(tx_riva_port_ptr->hw_ts_mux_id);
  }

  afe_riva_remove_from_vfr_sync(afe_bt_voc_tx_ptr);
  afe_riva_remove_from_vfr_sync(afe_bt_voc_rx_ptr);

  //stop aud synchronization between riva & lpass
  if(TRUE == aud_stat_afe_riva_ptr->is_bt_aud_sync_enable)
  {
    afe_riva_stop_bt_aud_sync(tx_riva_port_ptr, rx_riva_port_ptr);
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "BT_VOC_STOP: removed port port_id=%x & %x from vfr sync & from aud_sync",
        afe_bt_voc_rx_ptr->intf, afe_bt_voc_tx_ptr->intf);

  result |= afe_mmpm_voting(afe_bt_voc_rx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                            AFE_MMPM_DEVOTE, TRUE);
  result |= afe_mmpm_voting(afe_bt_voc_tx_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                            AFE_MMPM_DEVOTE, TRUE);


  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFE MMPM request failed, port_id_rx: 0x%x, port_id_tx: 0x%x",
          afe_bt_voc_rx_ptr->intf, afe_bt_voc_tx_ptr->intf);
  }

  afe_riva_ipc_secondary_task_deinit(afe_bt_voc_tx_ptr);
  afe_riva_ipc_secondary_task_deinit(afe_bt_voc_rx_ptr);

  return;
}

/**
 * AUD_SYNC notes: aud_sync block is stopped during mute.
 * TODO: what'll the VFR counts be if latched?
 */
void afe_riva_bt_voice_mute(afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr)
{
  afe_dev_riva_port_t *tx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_tx_ptr->afe_drv);
  afe_dev_riva_port_t *rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_rx_ptr->afe_drv);


  qurt_elite_mutex_lock( &(tx_riva_port_ptr->riva_drv_mutex));
  qurt_elite_mutex_lock( &(rx_riva_port_ptr->riva_drv_mutex));

  if (AFE_RIVA_IPC_WAIT != rx_riva_port_ptr->riva_connect)
  {
    //set the mute flag.
    tx_riva_port_ptr->is_mute = 1;
    rx_riva_port_ptr->is_mute = 1;

    afe_riva_remove_from_vfr_sync(afe_bt_voc_tx_ptr);
    afe_riva_remove_from_vfr_sync(afe_bt_voc_rx_ptr);

    //stop aud synchronization between riva & lpass
    if (TRUE == aud_stat_afe_riva_ptr->is_bt_aud_sync_enable)
    {
      afe_riva_stop_bt_aud_sync(tx_riva_port_ptr, rx_riva_port_ptr);
    }

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_MUTE: removed port port_id=%x & %x from vfr sync & from aud_sync",afe_bt_voc_rx_ptr->intf, afe_bt_voc_tx_ptr->intf);
  }
  qurt_elite_mutex_unlock( &(tx_riva_port_ptr->riva_drv_mutex));
  qurt_elite_mutex_unlock( &(rx_riva_port_ptr->riva_drv_mutex));
}

/**
 * AUD_SYNC notes: aud_sync block is initialized again.
 * todo: since mute/unmute is typically used for BT hand-over, during this time,
 * complete reset of aud_sync block is not necessary. However, in this implementation,
 * that's we have done it.
 */
void afe_riva_bt_voice_unmute(afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr, afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr)
{
  ADSPResult result;
  uint32 virt_addr = 0;

  // Get the payload of the msg.
  afe_ipc_rcv_bt_voc_msg_t *bt_voc_msg_ptr = (afe_ipc_rcv_bt_voc_msg_t *) ipc_rcv_msg_ptr;
  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  afe_dev_riva_port_t *tx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_tx_ptr->afe_drv);
  afe_dev_riva_port_t *rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_voc_rx_ptr->afe_drv);

  if ((NULL == bt_voc_msg_ptr->tx_cmem_buff_ptr) || (NULL == bt_voc_msg_ptr->rx_cmem_buff_ptr))
  {
    MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"bt_voice_unmute: Tx or Rx CMEM buffer ptr is NULL, Tx buffer ptr:0x%x, Rx buffer ptr:0x%x", \
          bt_voc_msg_ptr->tx_cmem_buff_ptr, bt_voc_msg_ptr->rx_cmem_buff_ptr);
    return;
  }

  if ((TRUE == aud_stat_afe_riva_ptr->is_riva_active))
  {
    if (tx_riva_port_ptr->is_mute)
    {
      // Get the CMEM buffer address
      if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                          (uint32)(bt_voc_msg_ptr->tx_cmem_buff_ptr),
                                                                          &virt_addr,
                                                                          riva_mem_perm)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT_VOC_UNMUTE: Failed to get VA for Tx cmem buf ptr PA: 0x%lx",
              bt_voc_msg_ptr->tx_cmem_buff_ptr);

        goto voc_unmute_bail_out;
      }
      tx_riva_port_ptr->cmem_buff_ptr = (int16_t *)virt_addr;

      if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                          (uint32)(bt_voc_msg_ptr->rx_cmem_buff_ptr),
                                                                          &virt_addr,
                                                                          riva_mem_perm)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BT_VOC_UNMUTE: Failed to get VA for Rx cmem buf ptr PA: 0x%lx",
              bt_voc_msg_ptr->rx_cmem_buff_ptr);

        goto voc_unmute_bail_out;
      }

      rx_riva_port_ptr->cmem_buff_ptr = (int16_t *)virt_addr;

      // Get the CMEM buffer size
      switch(bt_voc_msg_ptr->period)
      {
        case 1250:
        {
          tx_riva_port_ptr->data_size = 20; //1.25*2*8 = 20
        }
        break;
        case 2500:
        {
          tx_riva_port_ptr->data_size = 40; //2.5*2*8 = 40
        }
        break;
        case 3750:
        {
          tx_riva_port_ptr->data_size = 60; //3.75*2*8 = 60
        }
        break;
        case 7500:
        {
          tx_riva_port_ptr->data_size = 120; //7.5*2*8 = 120
        }
        break;
        default:
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR : BT_VOC_UNMUTE, Invalid RIVA interrupt period : %lu", bt_voc_msg_ptr->period);
          goto voc_unmute_bail_out;
        }
      }

      if (16000 == bt_voc_msg_ptr->sample_rate)
      {
        tx_riva_port_ptr->data_size *= 2;
      }
      rx_riva_port_ptr->data_size = tx_riva_port_ptr->data_size;

      qurt_elite_mutex_lock( &(afe_bt_voc_tx_ptr->port_mutex));
      //If port stat is in run, check the sample rate match.
      if (AFE_PORT_STATE_RUN == afe_bt_voc_tx_ptr->port_state)
      {
        if (afe_bt_voc_tx_ptr->sample_rate != bt_voc_msg_ptr->sample_rate ||
            tx_riva_port_ptr->riva_intr_period != bt_voc_msg_ptr->period)
        {
          afe_bt_voc_tx_ptr->sample_rate      = bt_voc_msg_ptr->sample_rate;
          afe_bt_voc_tx_ptr->dev_sample_rate      = bt_voc_msg_ptr->sample_rate;
          tx_riva_port_ptr->riva_intr_period  = bt_voc_msg_ptr->period;

          afe_port_de_init_sample_rate_dependent_modules(afe_bt_voc_tx_ptr);
          afe_riva_set_sample_rate_params(tx_riva_port_ptr, afe_bt_voc_tx_ptr);
          afe_driver_set_circ_buff(afe_bt_voc_tx_ptr);
          afe_port_re_init_sample_rate_dependent_modules(afe_bt_voc_tx_ptr);
        }
      }
      else
      {
        afe_bt_voc_tx_ptr->sample_rate       = bt_voc_msg_ptr->sample_rate;
        afe_bt_voc_tx_ptr->dev_sample_rate       = bt_voc_msg_ptr->sample_rate;
        tx_riva_port_ptr->riva_intr_period   = bt_voc_msg_ptr->period;
        afe_riva_set_sample_rate_params(tx_riva_port_ptr, afe_bt_voc_tx_ptr);
      }
      qurt_elite_mutex_unlock( &(afe_bt_voc_tx_ptr->port_mutex));

      qurt_elite_mutex_lock( &(afe_bt_voc_rx_ptr->port_mutex));
      //If port stat is in run, check the sample rate match.
      if (AFE_PORT_STATE_RUN == afe_bt_voc_rx_ptr->port_state)
      {
        if (afe_bt_voc_rx_ptr->sample_rate != bt_voc_msg_ptr->sample_rate ||
            rx_riva_port_ptr->riva_intr_period != bt_voc_msg_ptr->period)
        {
          afe_bt_voc_rx_ptr->sample_rate = bt_voc_msg_ptr->sample_rate;
          afe_bt_voc_rx_ptr->dev_sample_rate = bt_voc_msg_ptr->sample_rate;
          rx_riva_port_ptr->riva_intr_period = bt_voc_msg_ptr->period;

          afe_port_de_init_sample_rate_dependent_modules(afe_bt_voc_rx_ptr);
          afe_riva_set_sample_rate_params(rx_riva_port_ptr, afe_bt_voc_rx_ptr);
          afe_driver_set_circ_buff(afe_bt_voc_rx_ptr);
          afe_port_re_init_sample_rate_dependent_modules(afe_bt_voc_rx_ptr);
        }
      }
      else
      {
        afe_bt_voc_rx_ptr->sample_rate       = bt_voc_msg_ptr->sample_rate;
        afe_bt_voc_rx_ptr->dev_sample_rate       = bt_voc_msg_ptr->sample_rate;
        rx_riva_port_ptr->riva_intr_period   = bt_voc_msg_ptr->period;
        afe_riva_set_sample_rate_params(rx_riva_port_ptr, afe_bt_voc_rx_ptr);
      }
      qurt_elite_mutex_unlock( &(afe_bt_voc_rx_ptr->port_mutex));

      //start aud synchronization between riva & lpass
      if (ADSP_EOK != (result = afe_riva_start_bt_aud_sync(afe_bt_voc_rx_ptr->sample_rate, tx_riva_port_ptr, rx_riva_port_ptr)))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to config Aud Sync hw block");
        goto voc_unmute_bail_out;
      }
      afe_riva_add_to_vfr_sync(afe_bt_voc_rx_ptr);
      afe_riva_add_to_vfr_sync(afe_bt_voc_tx_ptr);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_UNMUTE: added port port_id=%x & %x to vfr sync & to aud_sync",afe_bt_voc_rx_ptr->intf, afe_bt_voc_tx_ptr->intf);
      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_VOC_UNMUTE: %x.riva_intr_period=%ld,data_size=%ld,int_samples_per_period=%ld,samples_per_ms=%d,sample_rate=%ld,us_per_sample=%lu ",\
            afe_bt_voc_rx_ptr->intf, rx_riva_port_ptr->riva_intr_period, \
            rx_riva_port_ptr->data_size,afe_bt_voc_rx_ptr->int_samples_per_period,\
            rx_riva_port_ptr->samples_per_ms,afe_bt_voc_rx_ptr->sample_rate, rx_riva_port_ptr->us_per_sample);
    }

    qurt_elite_mutex_lock( &(tx_riva_port_ptr->riva_drv_mutex));
    tx_riva_port_ptr->is_mute = 0;
    qurt_elite_mutex_unlock( &(tx_riva_port_ptr->riva_drv_mutex));

    qurt_elite_mutex_lock( &(rx_riva_port_ptr->riva_drv_mutex));
    //reset the mute flag.
    rx_riva_port_ptr->is_mute = 0;
    qurt_elite_mutex_unlock( &(rx_riva_port_ptr->riva_drv_mutex));

  }

  voc_unmute_bail_out:
  return;

}

/**
 * AUD_SYNC notes: BT audio doesn't need any aud_sync support since it's only AV timer interrupt (no riva interrupt)
 */
void afe_riva_bt_audio_start(afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr, afe_dev_port_t *afe_bt_aud_ptr)
{
  afe_dev_riva_port_t *rx_riva_port_ptr;
  afe_ipc_rcv_bt_aud_msg_t *bt_aud_msg_ptr;
  uint32_t                   virt_addr;
  uint32_t                   num_bytes_all_channels;
  ADSPResult result = ADSP_EOK;

  // Get the payload of the msg.
  bt_aud_msg_ptr = (afe_ipc_rcv_bt_aud_msg_t *) ipc_rcv_msg_ptr;

  // payload validation
  if ((NULL == bt_aud_msg_ptr->cmem_buff_ptr) || (NULL == bt_aud_msg_ptr->head_reg_ptr) || (NULL == bt_aud_msg_ptr->tail_reg_ptr) )
  {
    MSG_3(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"bt_audio_start: Received Null address, cmem buffer ptr:0x%x, head reg ptr:0x%x tail reg ptr:0x%x", \
          bt_aud_msg_ptr->cmem_buff_ptr, bt_aud_msg_ptr->head_reg_ptr,bt_aud_msg_ptr->tail_reg_ptr);
    return;
  }

  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_aud_ptr->afe_drv);

  if ((TRUE == aud_stat_afe_riva_ptr->is_riva_active) &&     //since some riva registers are read
      (AFE_RIVA_IPC_WAIT == rx_riva_port_ptr->riva_connect) )
  {
    // Get the CMEM buffer address, head/tail address.
    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(bt_aud_msg_ptr->tail_reg_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: Failed to get for CMEM tail reg ptr PA: 0x%lx",
            bt_aud_msg_ptr->tail_reg_ptr);

      goto _bail_out_bt_aud_start;
    }

    rx_riva_port_ptr->head_reg_ptr = (uint32 *)virt_addr;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(bt_aud_msg_ptr->head_reg_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: Failed to get for CMEM head reg ptr PA: 0x%lx",
            bt_aud_msg_ptr->head_reg_ptr);

      goto _bail_out_bt_aud_start;
    }
    rx_riva_port_ptr->tail_reg_ptr = (uint32 *)virt_addr;


    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(bt_aud_msg_ptr->cmem_buff_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: Failed to get for CMEM buf ptr PA: 0x%lx",
            bt_aud_msg_ptr->cmem_buff_ptr);

      goto _bail_out_bt_aud_start;
    }

    rx_riva_port_ptr->cmem_circ_buff.top_addr = (uint8_t *)virt_addr;

    rx_riva_port_ptr->cmem_circ_buff.bottom_addr  = rx_riva_port_ptr->cmem_circ_buff.top_addr + bt_aud_msg_ptr->cmem_buff_size;

    rx_riva_port_ptr->cmem_circ_buff.buffer_size  = bt_aud_msg_ptr->cmem_buff_size;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(*(rx_riva_port_ptr->head_reg_ptr)),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: Failed to get for CMEM head ptr PA: 0x%lx",
            *(rx_riva_port_ptr->head_reg_ptr));

      goto _bail_out_bt_aud_start;
    }

    rx_riva_port_ptr->cmem_circ_buff.head_addr = (uint8_t *)virt_addr;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(*(rx_riva_port_ptr->tail_reg_ptr)),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: Failed to get for CMEM tail ptr PA: 0x%lx",
            *(rx_riva_port_ptr->tail_reg_ptr));

      goto _bail_out_bt_aud_start;
    }

    rx_riva_port_ptr->cmem_circ_buff.tail_addr = (uint8_t *)virt_addr;

    num_bytes_all_channels = afe_bt_aud_ptr->bytes_per_channel* afe_bt_aud_ptr->channels;

    rx_riva_port_ptr->data_size = num_bytes_all_channels * afe_bt_aud_ptr->int_samples_per_period;

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_START: port_id=%x ",afe_bt_aud_ptr->intf);

    qurt_elite_mutex_lock( &(rx_riva_port_ptr->riva_drv_mutex));
    // update the ipc connection status
    rx_riva_port_ptr->riva_connect =  AFE_RIVA_IPC_CONNECT;
    //unlock the mutex
    qurt_elite_mutex_unlock( &(rx_riva_port_ptr->riva_drv_mutex));

    if (ADSP_EOK != (result = afe_mmpm_voting(afe_bt_aud_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                                              AFE_MMPM_VOTE, TRUE)))
    {
      afe_mmpm_voting(afe_bt_aud_ptr, AFE_MMPM_BT_FM_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM request failed");
    }
  }

  _bail_out_bt_aud_start:

  return;
}

/**
 * AUD_SYNC notes: BT audio doesn't need any aud_sync support since it's only AV timer interrupt (no riva interrupt)
 */
void afe_riva_bt_audio_stop(afe_dev_port_t *afe_bt_aud_ptr)
{
  afe_dev_riva_port_t *rx_riva_port_ptr;
  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  rx_riva_port_ptr = (afe_dev_riva_port_t *)(afe_bt_aud_ptr->afe_drv);

  if (AFE_RIVA_IPC_CONNECT == rx_riva_port_ptr->riva_connect)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BT_AUD_STOP: port_id=%x ",afe_bt_aud_ptr->intf);
    qurt_elite_mutex_lock( &(rx_riva_port_ptr->riva_drv_mutex));
    // update the ipc connection status
    rx_riva_port_ptr->riva_connect =  AFE_RIVA_IPC_WAIT;
    qurt_elite_mutex_unlock( &(rx_riva_port_ptr->riva_drv_mutex));

    afe_mmpm_voting(afe_bt_aud_ptr, AFE_MMPM_BT_FM_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
  }
}

/**
 * AUD_SYNC notes: Only one of FM RX or TX exist at a time & aud_sync is used for that.
 */
void afe_riva_fm_start(afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr, afe_dev_port_t *afe_fm_ptr)
{
  afe_dev_riva_port_t *riva_port_ptr;
  char_t thread_name[16];
  ADSPResult result = 0;
  uint32_t virt_addr, num_bytes_all_channels;
  // Get the payload of the msg.
  afe_ipc_rcv_fm_msg_t *fm_msg_ptr = (afe_ipc_rcv_fm_msg_t *) ipc_rcv_msg_ptr;

  // payload validation
  if ((NULL == fm_msg_ptr->cmem_buff_ptr) || (NULL == fm_msg_ptr->head_reg_ptr) || (NULL == fm_msg_ptr->tail_reg_ptr) )
  {
    MSG_3(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"fm_start:Received Null address, cmem buffer ptr:0x%x, head reg ptr:0x%x tail reg ptr:0x%x", \
          fm_msg_ptr->cmem_buff_ptr, fm_msg_ptr->head_reg_ptr,fm_msg_ptr->tail_reg_ptr);
    return;
  }

  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement
  riva_port_ptr = (afe_dev_riva_port_t *)(afe_fm_ptr->afe_drv);
  if ((TRUE == aud_stat_afe_riva_ptr->is_riva_active) && //since some registers are read (like head_reg_ptr)
      (AFE_RIVA_IPC_WAIT == riva_port_ptr->riva_connect) )
  {
    // Get the CMEM buffer address, head/tail address.
    // Get the CMEM buffer address, head/tail address.
    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(fm_msg_ptr->tail_reg_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM_START: Failed to get for VA for CMEM tail ptr PA: 0x%lx",
            fm_msg_ptr->tail_reg_ptr);
      goto fm_start_bail_out;
    }

    riva_port_ptr->head_reg_ptr = (uint32 *)virt_addr;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(fm_msg_ptr->head_reg_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM_START: Failed to get for VA for CMEM head ptr PA: 0x%lx",
            fm_msg_ptr->head_reg_ptr);
      goto fm_start_bail_out;
    }

    riva_port_ptr->tail_reg_ptr = (uint32 *)virt_addr;

    if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress_Statically_Mapped(riva_cache_attribs,
                                                                        (uint32)(fm_msg_ptr->cmem_buff_ptr),
                                                                        &virt_addr,
                                                                        riva_mem_perm)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FM_START: Failed to get VA for CMEM buf ptr PA: 0x%lx",
            fm_msg_ptr->cmem_buff_ptr);
      goto fm_start_bail_out;
    }

    riva_port_ptr->cmem_circ_buff.top_addr = (uint8_t *)virt_addr;

    riva_port_ptr->cmem_circ_buff.buffer_size  = fm_msg_ptr->cmem_buff_size;
    riva_port_ptr->cmem_circ_buff.bottom_addr  = riva_port_ptr->cmem_circ_buff.top_addr + fm_msg_ptr->cmem_buff_size;

    MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FM_START:cmem_buff_ptr=%x,cmem_buff_size=%d,tail_reg_ptr=%x,*tail_reg_ptr=%x,head_reg_ptr=%x,*head_reg_ptr=%x",\
          (unsigned int)fm_msg_ptr->cmem_buff_ptr, (int)fm_msg_ptr->cmem_buff_size, (unsigned int) fm_msg_ptr->tail_reg_ptr, (unsigned int) *(riva_port_ptr->head_reg_ptr), (unsigned int)fm_msg_ptr->head_reg_ptr,(unsigned int) *(riva_port_ptr->tail_reg_ptr));

    num_bytes_all_channels = afe_fm_ptr->bytes_per_channel* afe_fm_ptr->channels;

    //acquire port mutex as port parameters are updating
    qurt_elite_mutex_lock(&afe_fm_ptr->port_mutex);

    //If port stat is in run, check the sample rate match.
    if (AFE_PORT_STATE_RUN == afe_fm_ptr->port_state)
    {
      if (riva_port_ptr->data_size != fm_msg_ptr->sample_isr_cnt * num_bytes_all_channels)
      {
        riva_port_ptr->data_size = fm_msg_ptr->sample_isr_cnt * num_bytes_all_channels;

        afe_riva_set_sample_rate_params(riva_port_ptr, afe_fm_ptr);

        afe_driver_set_circ_buff(afe_fm_ptr);
      }
    }
    else
    {
      riva_port_ptr->data_size = fm_msg_ptr->sample_isr_cnt * num_bytes_all_channels;
      afe_riva_set_sample_rate_params(riva_port_ptr, afe_fm_ptr);
    }
    qurt_elite_mutex_unlock(&afe_fm_ptr->port_mutex);

    riva_port_ptr->riva_intr_period = ((uint64_t)fm_msg_ptr->sample_isr_cnt * NUM_US_PER_SEC/afe_fm_ptr->sample_rate);

    if (ADSP_EOK != (result = afe_mmpm_voting(afe_fm_ptr, AFE_MMPM_BT_FM_TOPOLOGY,
                                              AFE_MMPM_VOTE, TRUE)))
    {
      afe_mmpm_voting(afe_fm_ptr, AFE_MMPM_BT_FM_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM request failed");
      goto fm_start_bail_out;
    }

    //start aud synchronization between riva & lpass
    if (ADSP_EOK != (result = afe_riva_start_fm_aud_sync(afe_fm_ptr->sample_rate, riva_port_ptr, fm_msg_ptr->sample_isr_cnt)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fails to config AUd Sync block in FM");
      goto fm_start_bail_out;
    }

    afe_riva_add_to_vfr_sync(afe_fm_ptr);

    // Secondary task on the threadpool:- RIVA triggered
    if (ADSP_EOK != (result = afe_riva_ipc_secondary_task_init(afe_fm_ptr)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RIVA port fails to init secondary task");
      goto fm_start_bail_out;
    }

    // Used for CMEM pointer RESET\Init in first RIVA interrupt
    riva_port_ptr->reset_cmem_ptrs = TRUE;

    // update the ipc connection status
    riva_port_ptr->riva_connect =  AFE_RIVA_IPC_CONNECT;

    //if Timestamp hardware is available need to configure the mux
    if(aud_stat_afe_riva_ptr->hw_timestamp_available)
    {
      afe_ts_drv_config_vfr_ts_mux(HW_TS_INTR_TYPE_FM,
                                   0,
                                   &riva_port_ptr->hw_ts_mux_id);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Hardware timestamp block configured for FM interrupt, mux_id: 0x%lx",
            riva_port_ptr->hw_ts_mux_id);
    }

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FM_START: added port port_id=%x to vfr sync & to aud_sync",afe_fm_ptr->intf);
    MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FM_START: port_id=%x,data_size=%d,sample_rate=%d, bytes_per_channel=%d,sample_per_riva_int=%d,int_per_period=%d,samples_per_ms=%d,us_per_sample=%d,channels=%d",\
          afe_fm_ptr->intf, (unsigned int)riva_port_ptr->data_size,(unsigned int)afe_fm_ptr->sample_rate,(unsigned int)afe_fm_ptr->bytes_per_channel,(unsigned int)fm_msg_ptr->sample_isr_cnt,\
          (unsigned int)afe_fm_ptr->int_samples_per_period,(unsigned int)riva_port_ptr->samples_per_ms,(unsigned int)riva_port_ptr->us_per_sample,(int)afe_fm_ptr->channels);

    // To enable interrupt in the last step to escape race condition.
    qurt_elite_strl_cpy(thread_name,"AfeFmIsr", sizeof(thread_name));
    if(ADSP_EOK
        != (result = qurt_elite_interrupt_register(&(riva_port_ptr->intr_struct),
                                                   aud_stat_afe_riva_ptr->fm_tx_rx_int,
                                                   AfeDriver_RivaDataIsr, (void*)afe_fm_ptr,
                                                   thread_name,
                                                   AFE_INTERNAL_BTFM_DATA_IST_STACK_SIZE)))
    {
      //print error msg...
    }

  }
  fm_start_bail_out:
  return;
}

/**
 * AUD_SYNC notes: Only one of FM RX or TX exist at a time & aud_sync is used for that.
 */
void afe_riva_fm_stop(afe_dev_port_t *afe_fm_ptr)
{
  afe_dev_riva_port_t *riva_port_ptr;

  riva_port_ptr = (afe_dev_riva_port_t *)(afe_fm_ptr->afe_drv);

  /* Check if the BT_FM chip is already in IPC wait state */
  if(AFE_RIVA_IPC_WAIT == riva_port_ptr->riva_connect)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "FM_STOP: Not in IPC connect state, port_id: 0x%x",
          afe_fm_ptr->intf);
    return;
  }

  //If following process are in one thread, no mutex needed:
  // 1. riva <-> lpass command process
  // 2. riva <-> lpass data movement

  //acquire riva driver mutex while updating riva_connect state. As the data processing happens on the dynamic thread
  //need to update synchronously.

  qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));
  // update the ipc connection status
  riva_port_ptr->riva_connect = AFE_RIVA_IPC_WAIT;
  qurt_elite_mutex_unlock( &(riva_port_ptr->riva_drv_mutex));

  /* disable the periodical RivaLpassFmIntr
   * disable interrupt in the first step to decrease race condition.
   */
  qurt_elite_interrupt_unregister(&(riva_port_ptr->intr_struct));

  /* Release FM timestamp mux */
  if(aud_stat_afe_riva_ptr->hw_timestamp_available)
  {
    afe_ts_drv_release_ts_mux(riva_port_ptr->hw_ts_mux_id);
  }

  /* Stop aud synchronization between riva & lpass */

  afe_riva_remove_from_vfr_sync(afe_fm_ptr);

  if(TRUE == aud_stat_afe_riva_ptr->is_fm_aud_sync_enable)
  {
    afe_riva_stop_fm_aud_sync(riva_port_ptr);
  }


  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "FM_STOP: removed port port_id=%x from vfr sync & from aud_sync",
        afe_fm_ptr->intf);

  afe_mmpm_voting(afe_fm_ptr, AFE_MMPM_BT_FM_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);

  /* Destroy the dynamic thread associated with FM Tx/Rx */
  afe_riva_ipc_secondary_task_deinit(afe_fm_ptr);

  return;
}

/**
 * This is a mailbox control routine
 *
 * @param[in] ipc_rcv_msg_ptr IPC received mail box address
 * @param[in] ipc_send_msg_ptr IPC send mail box address
 */

void afe_driver_ipc_ack_riva(afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr, afe_ipc_send_msg_t *ipc_send_msg_ptr)
{
  uint32_t temp;
  uint32_t *mail_box_ctrl_reg_ptr;

  mail_box_ctrl_reg_ptr = (uint32_t *)(aud_stat_afe_riva_ptr->q6ss_ipc_reg_virt_addr);;

  if ( NULL == ipc_rcv_msg_ptr || NULL == ipc_send_msg_ptr )
  {
    return;
  }

  // 1. reset the R/W bit in command header (bit 31),
  //    so that Riva know that IPC msg has already been read.
  temp = ipc_rcv_msg_ptr->header;
  temp = temp & AFE_IPC_MSG_CMD_ID;
  ipc_rcv_msg_ptr->header = temp;

  // 2. Set the 16th bit to 1 and write in the address LPASS_QDSP6SS_IPC which triggers
  // interrupt to RIVA from LPASS
  temp = (1UL << aud_stat_afe_riva_ptr->q6ss_ipc_reg_ack_bit_pos);
  *mail_box_ctrl_reg_ptr = temp;
  return;
}

/**
  @brief function called up on a signal from the LPASS subsystem monitor.
  SSR: SubSystem Restart

   . set a flag
   . find all active ports so that all future accessses to riva are stopped.
   . do as though riva disconnected (do all things that RIVA disconnect does accept ack)
   .
  @return  none
 */

void afe_riva_crash_handler(void)
{
  //riva crash
  if (TRUE == aud_stat_afe_riva_ptr->is_riva_active)
  {
    //set this global flag so that all riva ports stop accessing cmem.
    aud_stat_afe_riva_ptr->is_riva_active = FALSE;

    // Port type
    afe_dev_port_t *afe_bt_aud_ptr    = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_A2DP_RX)];
    afe_dev_port_t *afe_bt_voc_tx_ptr = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_SCO_TX)];
    afe_dev_port_t *afe_bt_voc_rx_ptr = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_BT_SCO_RX)];
    afe_dev_port_t *afe_fm_tx_ptr     = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_FM_TX)];
    afe_dev_port_t *afe_fm_rx_ptr     = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(AFE_PORT_ID_INTERNAL_FM_RX)];

    //stop each port. (check for whether port is started or not is made inside these funcs)
    afe_riva_bt_voice_stop(afe_bt_voc_tx_ptr, afe_bt_voc_rx_ptr);
    afe_riva_bt_audio_stop(afe_bt_aud_ptr);
    afe_riva_fm_stop(afe_fm_tx_ptr);
    afe_riva_fm_stop(afe_fm_rx_ptr);

    //now that riva ports are stopped, set the flag back to TRUE for future usecases to work.
    aud_stat_afe_riva_ptr->is_riva_active = TRUE;
  }

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RIVA SSR: handled riva crash!");
}

/**
 * called after riva is recovered from a crash to set the global flag back to start
 */
void afe_riva_recovery_handler (void)
{
  aud_stat_afe_riva_ptr->is_riva_active = TRUE;
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RIVA SSR: handled riva recovery!");
}

void afe_riva_circ_buf_protect_handler(afe_dev_port_t *dev_port_ptr)
{
  afe_dev_riva_port_t  *riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);

  // Protection logic if RIVA is not giving interrupts with in the acceptable time
  int32_t diff = (int32_t) (riva_port_ptr->curr_avtimer_timestamp_us - riva_port_ptr->prev_avtimer_timestamp_us);

  if (diff < 0)
  {
    diff += 0x7FFFFFFF;
  }

  if (diff > (int32_t)(riva_port_ptr->riva_ipc_int_reset_thrh_us))
  {
    MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MISSING INT: 0x%x, curr_TS - %lu, prev_TS %lu, diff - %ld, thresh - %lu", dev_port_ptr->intf, (uint32_t) riva_port_ptr->curr_avtimer_timestamp_us, (uint32_t) riva_port_ptr->prev_avtimer_timestamp_us, diff, riva_port_ptr->riva_ipc_int_reset_thrh_us);

    // RESET circ buffer for both Tx and Rx
    riva_port_ptr->is_adjust = TRUE;

    if (AFE_PORT_ID_INTERNAL_BT_SCO_RX == dev_port_ptr->intf )
    {
      // RESET drift logic if it is Rx port
      riva_port_ptr->is_first_riva_lpass_sync = TRUE;
    }

    riva_port_ptr->riva_lpass_drift_us  = 0;
  }

  riva_port_ptr->prev_avtimer_timestamp_us = riva_port_ptr->curr_avtimer_timestamp_us;

  return;
}

