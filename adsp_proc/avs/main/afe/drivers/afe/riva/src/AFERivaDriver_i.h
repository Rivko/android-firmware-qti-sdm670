/*========================================================================
  ELite

  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaDriver_i.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  12/10/10   jq      Created file.

  ========================================================================== */
/**
@file AFEDriverRiva.h

@brief has structures of AFE port BT voice, BT audio, and FM
*/

#ifndef AFEDRIVERRIVA_H
#define AFEDRIVERRIVA_H

#include "AFEInterface.h"
#include "AFEInternal.h"
#include "adsp_afe_service_commands.h"
#include "aud_sync_hal.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * false alarm threshold in number of VFR frames before which alarm is not raised
 */
#define  FALSE_ALARM_MAX_FRAME_CNT 6

/**
 * number of ms between VFR frames
 */
#define  VFR_FRAME_SIZE            20

#if 0
/**
 * log2 of number of milliseconds taken to correct drift between riva and lpass
 * when 4, any amount of drift (small or big) is corrected within a span of next 4ms (2^2) to a value within 4us.
 * if AVT_CORRECTION_INTERVAL__S is too high then correction will be slower;
 * if too small, then correction could be aggressive
 * & might cause drift wrt VFR at voice layer.
 * Suppose 1 sample drift at 8k is detected then drift is 125us. If we correct in 4ms, each time we correct by 125/4 = ~31 us.
 * However the remainder (=1) us will be corrected at 1us rate.
 * We do not want to correct the last 16us at once, neither can we remember to correct at old rate of 8us.
 * Suppose while correcting the drift of 125us, another drift of 125 happens, then we correct 250us in 16ms => 16 us each time.
 *
 */
#define  AVT_CORRECTION_INTERVAL__S   2
#define  AVT_CORRECTION_INTERVAL__M   (1 << AVT_CORRECTION_INTERVAL__S)

#endif

/**
 * maximum number of corrections per RIVA interrupt, in samples
 *
 */
#define  MAX_SMP_CORR_PER_RIVA_INT   1
/**
 * maximum number of corrections per a ms of Av timer interrupt, in us.
 * This constant is decided based on the constraint that if drift is such that it
 * decreases the time available for av timer interrupt processing (1ms), then the decrease
 * shouldn't be drastic.
 *
 * this number should be high if MAX_SMP_CORR_PER_RIVA_INT is also high.
 */
#define  MAX_US_CORR_PER_MS_IN_AVT_INT     50

/*==========================================================================
  Structure Definitions
========================================================================== */
/** Riva port  states. */
typedef enum
{
   AFE_RIVA_IPC_WAIT    =0,
                  /**< Riva is not accessible in this state */
   AFE_RIVA_IPC_CONNECT =1
                  /**< Riva is accessible in this state */
} afe_port_riva_state;

/** @brief Enums for BT/FM interface cfg versions */
typedef enum
{
	BT_FM_CFG_V1 = 0x1
}afe_param_id_bt_fm_cfg_minor_version;

extern afe_dev_riva_port_t      *riva_ports_ptr_arr[AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE];
extern aud_stat_afe_riva_t      *aud_stat_afe_riva_ptr;

void afe_driver_linear_to_circ(uint16_t port_id,afe_circ_buff_t *dstCircBuf, uint8_t *srcLinearBuf, uint32_t length, uint32_t numBytes, bool_t aligned);
void afe_driver_circ_to_linear(uint16_t port_id, uint8_t *dstLinearBuf, afe_circ_buff_t *srcCircBuf, uint32_t length, bool_t aligned);

void afe_memcpy(uint8_t *dst_buff_ptr, uint32_t dst_size, 
				uint8_t *src_buff_ptr, uint32_t src_size, 
				bool_t aligned, uint16_t port_id);

void afe_dev_riva_get_transfer_block_length(afe_dev_port_t *dev_port_ptr,uint32_t sampleRate);
void afe_driver_ipc_ack_riva(afe_ipc_rcv_msg_t *pRcvIpcMsg, afe_ipc_send_msg_t *pSendIpcMsg);
void afe_driver_circ_to_circ(uint16_t port_id, afe_circ_buff_t *dst_circ_buff_ptr, afe_circ_buff_t *src_circ_buff_ptr, uint32_t length, uint32_t num_bytes, bool_t aligned);

/**
 * This is to set the riva port circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 */
void afe_driver_set_circ_buff(afe_dev_port_t *dev_port_ptr);

/**
 * This is a AV-Timer ISR routine to read samples from port circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @param[in] dst_ptr pointer to destination buffer
 * @param[in] num_samples number of samples to be read from port circular buffer
 */
ADSPResult afe_read_riva(afe_dev_port_t *dev_port_ptr, int16_t *dst_ptr, uint32_t num_samples);

/**
 * This is a AV-Timer ISR routine to write samples to port circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @param[in] src_ptr pointer to source buffer
 * @param[in] num_samples number of samples to write to port circular buffer
 */
ADSPResult afe_write_riva(afe_dev_port_t *dev_port_ptr, int16_t *src_ptr, uint32_t num_samples);

/**
 * This is a ISR routine to process interrupt from riva to read
 * samples from cmem.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 */

ADSPResult afe_driver_bt_voc_cmem_read(void *dev_port_ptr);

/**
 * This is a ISR routine to process interrupt from riva to write
 * samples to cmem.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 */

ADSPResult afe_driver_bt_voc_cmem_write(void *dev_port_ptr);

/**
 * This is a AV-Timer ISR routine to write samples to cmem circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @param[in] src_ptr pointer to source buffer
 * @param[in] num_samples number of samples to write to port circular buffer
 */
ADSPResult afe_write_bt_aud(afe_dev_port_t *dev_port_ptr, int16_t *src_ptr, uint32_t num_samples);

/**
 * This is a ISR routine to process interrupt from riva to write
 * samples to cmem for FM Rx.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 */

ADSPResult afe_driver_fm_aud_cmem_write(void *dev_port_ptr);

/**
 * This is a ISR routine to process interrupt from riva to read
 * samples from cmem for FM Tx.
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @return the ADSP_EOK error code
 */

ADSPResult afe_driver_fm_aud_cmem_read(void *dev_port_ptr);

/**
 * Perform one time initialization of the AFE Riva IPC
 *
 * @param[in] aud_stat_afe_riva_ptr pointer to riva static struct
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_ipc_init(aud_stat_afe_riva_t *aud_stat_afe_riva_ptr);

/**
 * Perform de-initialization of the AFE Riva driver
 *
 * @param[in] aud_stat_afe_riva_ptr pointer to riva static struct
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_ipc_deinit(aud_stat_afe_riva_t *aud_stat_afe_riva_ptr);

/**
 * Initializes secondary tasks and launches it using AFE dynamic threads.
 */
ADSPResult afe_riva_ipc_secondary_task_init (afe_dev_port_t *pDevPort);


/**
 * De-initializes secondary tasks and destroys AFE dynamic threads.
 */
ADSPResult afe_riva_ipc_secondary_task_deinit (afe_dev_port_t *pDevPort);


void afe_riva_bt_voice_start  (afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr,   afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr);
void afe_riva_bt_voice_stop   (afe_dev_port_t    *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr  );
void afe_riva_bt_voice_mute   (afe_dev_port_t    *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr  );
void afe_riva_bt_voice_unmute (afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr,   afe_dev_port_t *afe_bt_voc_tx_ptr, afe_dev_port_t *afe_bt_voc_rx_ptr);
void afe_riva_bt_audio_start  (afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr,   afe_dev_port_t *afe_bt_aud_ptr     );
void afe_riva_bt_audio_stop   (afe_dev_port_t    *afe_bt_aud_ptr);
void afe_riva_fm_start        (afe_ipc_rcv_msg_t *ipc_rcv_msg_ptr,   afe_dev_port_t *afe_fm_ptr         );
void afe_riva_fm_stop         (afe_dev_port_t    *afe_fm_ptr);
void afe_drv_riva_ssr_cb_register();
void afe_drv_riva_ssr_cb_deregister();
void afe_riva_crash_handler(void);
void afe_riva_recovery_handler (void);

/**
 * initializes aud sync memory & resets all registers
 */
ADSPResult afe_riva_init_aud_sync();

/**
 * deinitializes aud_sync
 */
ADSPResult afe_riva_deinit_aud_sync();

/**
 * starts the Aud sync for BT
 *
 * @param[in] sample_rate, sample rate of the BT port
 * @param[in] rx_riva_port_ptr, RX BT port
 * @param[in] tx_riva_port_ptr, TX BT port
 *
 */
ADSPResult afe_riva_start_bt_aud_sync(uint32_t sample_rate, afe_dev_riva_port_t *tx_riva_port_ptr, afe_dev_riva_port_t *rx_riva_port_ptr);

/**
 * stops the Aud sync for BT
 */
void afe_riva_stop_bt_aud_sync(afe_dev_riva_port_t *tx_riva_port_ptr, afe_dev_riva_port_t *rx_riva_port_ptr);

/**
 * starts the Aud sync for FM
 *
 * @param[in] sample_rate, sample rate of the BT port
 * @param[in] data_size, number of bytes per BT interrupt
 * @param[in] bytes_per_channel, number of bytes per sample of a channel
 */
ADSPResult afe_riva_start_fm_aud_sync(uint32_t sample_rate, afe_dev_riva_port_t *fm_port_ptr, uint32_t  samples_per_riva_int);

/**
 * stops the Aud sync for FM
 */
void afe_riva_stop_fm_aud_sync(afe_dev_riva_port_t *fm_port_ptr);

/**
 * maps the port id to aud sync ID.
 *
 * @param[in] port_id afe port id
 * @param[out] aud_sync_id aud sync for the given port_id
 *
 * @return ADSP error code.
 */
ADSPResult afe_riva_map_port_id_to_aud_sync(uint16_t port_id, hal_aud_sync_intf_t *aud_sync_id);

void afe_riva_aud_sync(uint16_t port_id, hal_aud_sync_intf_t intf, \
      int64_t *riva_lpass_drift_us, uint32_t us_per_sample, uint8_t *is_first_riva_lpass_sync, \
      int64_t *correction_us);

void afe_riva_calc_avt_adj(uint16_t port_id, int64_t *riva_lpass_drift_us, uint32_t block_transfer_time, int32_t *avt_adj);

ADSPResult afe_riva_set_sample_rate_params(afe_dev_riva_port_t  *riva_port_ptr, afe_dev_port_t *afe_dev_port_ptr);

uint64_t afe_riva_read_timestamp_register(uint32_t mux_id);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* AFEDRIVERRIVA_H */
