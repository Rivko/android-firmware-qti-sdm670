#ifndef _AFE_RIVA_DRIVER_H_
#define _AFE_RIVA_DRIVER_H_
/*========================================================================
  ELite

  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/inc/AFERivaDriver.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  12/10/10   jq      Created file.

  ========================================================================== */
/**
@file AFERivaDriver.h

@brief has high level APIs for RIVA driver
*/

#include "AFEInternal.h"
#include "mmdefs.h"
#include "AFEDeviceDriver.h"
#include "VfrDriverInternal.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*=====================================================================
 Macros
 ======================================================================*/
/** lpass<->Riva ipc max playload size. */
#define IPC_PAYLOAD_SIZE 					7

/** BT Voice Start IPC cmd. */
#define RIVA_BT_VOICE_START   	  		0x0001UL

/** BT Voice Stop IPC cmd. */
#define RIVA_BT_VOICE_STOP         		0x0002UL

/** BT Voice mute IPC cmd. */
#define RIVA_BT_VOICE_MUTE          	0x0003UL

/** BT Voice un-mute IPC cmd. */
#define RIVA_BT_VOICE_UNMUTE        	0x0004UL

/** BT Audio Start IPC cmd. */
#define RIVA_BT_AUDIO_START         	0x0005UL

/** BT Audio Stop IPC cmd. */
#define RIVA_BT_AUDIO_STOP          	0x0006UL

/** BT FM Tx Start IPC cmd. */
#define RIVA_FM_TX_AUDIO_START      	0x0100UL

/** BT FM Tx Stop IPC cmd. */
#define RIVA_FM_TX_AUDIO_STOP       	0x0200UL

/** BT FM Rx Start IPC cmd. */
#define RIVA_FM_RX_AUDIO_START      	0x0300UL

/** BT FM Rx Stop IPC cmd. */
#define RIVA_FM_RX_AUDIO_STOP       	0x0400UL

/** lpass<->Riva ipc command header cmdID bits. */
#define AFE_IPC_MSG_CMD_ID 				0x7FFFFFFFUL

/**
 * FM Circular buffer size: 48kHz * stereo * 2bytesPerCh * 5ms *
 * 2 = 1920.  5 ms extra buffer size was considered:
 * circular buffer needs 1ms tolerance for under-run case, and
 * 1ms tolerance for over-run case. (Assume av-timer is 1ms per
 * interrupt)
 */
#define AFE_FM_CIRC_BUF_SIZE 1920

/**
 * BT Circular buffer size: 16kHz * 2bytesPerCh * 10ms + 8 = 328
 * 4 ms extra buffer size was considered: circular buffer needs
 * 2ms tolerance for under-run case, and 2ms tolerance for
 * over-run case.(Assume av-timer is 1ms per interrupt)
 */
#define AFE_BTVOC_CIRC_BUF_SIZE 328

/**
 * Number of milliseconds in a second
 */
#define NUM_MS_PER_SEC (1000)


/*=====================================================================
 Typedef's
 ======================================================================*/

/** @brief structure of received fm port message from
 *         riva */
typedef struct
{
	uint32_t  header;
								 /**< header of message structure, including command ID */
	int16_t*  cmem_buff_ptr;
								 /**< cmem circular buffer start address */
	uint32_t  cmem_buff_size;
								 /**< cmem circular buffer size */
	uint32_t* head_reg_ptr;
								 /**< register for cmem circular buffer head address */
	uint32_t* tail_reg_ptr;
								 /**< register for cmem circular buffer tail address */
	uint32_t  sample_isr_cnt;
								 /**< number of sample in WORD (32-bit) per riva interrupt */
} afe_ipc_rcv_fm_msg_t;

/** @brief structure of received bt_aud port message from
 *         riva */
typedef struct
{
	uint32_t  header;
								 /**< header of message structure, including command ID */
	int16_t*  cmem_buff_ptr;
								 /**< cmem circular buffer start address */
	uint32_t  cmem_buff_size;
								 /**< cmem circular buffer size */
	uint32_t* head_reg_ptr;
								 /**< register for cmem circular buffer head address */
	uint32_t* tail_reg_ptr;
								 /**< register for cmem circular buffer tail address */
} afe_ipc_rcv_bt_aud_msg_t;

/** @brief structure of received bt_voc port message from
 *         riva */
typedef struct
{
	uint32_t header;
								 /**< header of message structure, including command ID */
	int16_t* tx_cmem_buff_ptr;
								 /**< cmem linear buffer start address for bt_voc tx port */
	int16_t* rx_cmem_buff_ptr;
								 /**< cmem linear buffer start address for bt_voc rx port */
	uint32_t cmem_buff_size;
								 /**< cmem linear buffer size */
	uint32_t sample_rate;
								 /**< sample rate of riva bt_voc port */
	uint32_t period;
								 /**< number of us per riva interrupt */
} afe_ipc_rcv_bt_voc_msg_t;

/** @brief structure of received message from riva */
typedef struct
{
  uint32_t header;
                   /**< header of message structure, including command ID */
  uint32_t payload[IPC_PAYLOAD_SIZE];
                  /**< payload of receive msg */
} afe_ipc_rcv_msg_t;

/** @brief message structure to send to riva */
typedef struct
{
  uint32_t result;
                  /**< result of IPC command process */
} afe_ipc_send_msg_t;

/**
 * Struct for Riva<->Lpass IPC mailbox message address,
 * including send mailbox address and receive mailbox address.
 */
typedef struct {
   uint32_t                  send_mailbox_addr;
                             /**< Lpass <-> Riva ipc send mail box address */
   uint32_t                  rcv_mailbox_addr;
                             /**< Lpass <-> Riva ipc rcv mail box address */
} riva_mailbox_t;

typedef enum
 {
    AFE_RIVA_LPASS_DATA_PROCESSING_STOP = 0,   /**< AFE Port is in the Stop state, no data processing drop/send zeros for Tx/Rx  */
    AFE_RIVA_LPASS_DATA_PROCESSING_RUN,        /**< AFE Port is in the Stop state, data processing */
  } afe_riva_lpass_data_proc_state;

/**
 * Struct for riva ports specific properties
 */
typedef struct afe_dev_riva_port {
   uint64_t                riva_clock;
                           /**< for riva port, it reads the current time from av-timer*/
   qurt_elite_interrupt_ist_t intr_struct;
                           /**< Interrupt structure for qurt_elite interrupt register */
   qurt_elite_signal_t          riva_signal;
                           /**< The port-corresponding RIVA signal instance */
   uint32_t                data_size;
                           /**< Data size for one riva data interrupt of Bt/FM ports */
   int16_t                 *cmem_buff_ptr;
                           /**< Cmem buffer address for lpass read/write */
   afe_circ_buff_t         circ_buff;
                           /**< BtVocTx/BtVocRx/BtAud/FmTx/FmRx port circular buffer */
   afe_circ_buff_t         cmem_circ_buff;
                           /**< BtAud/FmTx/FmRx port cmem circular buffer */
   uint32_t                *head_reg_ptr;
                           /**< BtAud/FmTx/FmRx port cmem circular buffer Head Register addess */
   uint32_t                *tail_reg_ptr;
                           /**< BtAud/FmTx/FmRx port cmem circular buffer Tail Register addess */
   uint32_t                riva_connect;
                           /**< Flag the riva is connected with lpass */
   uint32_t                is_mute;
                           /**< flag the BtVoc is on mute or not */
   uint32_t                is_adjust;
                           /**< flag the BtVoc/Fm needs to adjust the tail/head of port circular buffer */
   uint32_t                riva_intr_period;
                           /**< for BT Voc only, riva interrupt period in us, 3750us or 7500us */
   uint8_t                 is_first_riva_lpass_sync;
                           /**< if it's first then ref cnt is loaded to sample counter. TODO: effect on VFR */
   int64_t                 riva_lpass_drift_us;
                           /**< drift between lpass and riva in microseconds that's yet to be corrected on avtimer instance.
                            * This value is updated everytime AV timer adj is made. Note: port mutex protects this (b/w RIVA side & AV timer side).*/
   int32_t                 acc_avt_drift_us;
                           /**< accumulated avt drift = riva_lpass drift that's already corrected on avtimer. reset at port start (& not at stop).*/
   uint32_t                last_vfr_timestamp;
                           /**< snapshot of hardware VFR timestamp during last VFR interrupt. samples */
   uint32_t 		       riva_ipc_int_reset_thrh_us;
						   /**< threshold value to detect the missing interrupt from RIVA */
   uint64_t                curr_avtimer_timestamp_us;
						   /**< snapshot of AV timer timestamp during current RIVA interrupt.*/
   uint64_t                prev_avtimer_timestamp_us;
						   /**< snapshot of AV timer timestamp during previous RIVA interrupt.*/
   uint16_t                false_alarm_frame_cntr;
                           /**< false alarm counter : after this cntr reaches a thresh drifts are sent out */
   int32_t                 drift_mem;
                           /**< drift memory in samples*/
   int64_t                 acc_vfr_drift;
                           /**< acc drift in us */
   uint8_t                 is_first_vfr;
                           /**< flag to designate the first VFR */
   uint8_t                 is_vfr_enabled;
                           /** whether VFR is enabled for this port or not */
   uint16_t                samples_per_ms;
                           /** number of samples per ms */
   uint32_t                us_per_sample;
                           /** number of microseconds per sample : 1/sample_rate in Q25*/
   uint16_t				      reset_cmem_ptrs;
						         /** flag	to synchronize CMEM	head\tail ptrs between RIVA	and
						          *  LPASS in	case of	FM	  */
   void*                   avt_drv;
			   			      /**< Handle of the AV Timer driver. */
   uint16_t 			      bit_width;
                           /**< bits per sample */
   uint32_t 			      dev_cfg_minor_ver;
                           /**< Device config minor version */
   bool_t                  hw_timestamp_available;
                           /**< Hardware timestamping block available for RIVA */
   afe_dynamic_thread_t    *dynamic_thread_ptr;
                           /**< Dynamic worker thread for seconday tasks.*/
   uint32_t                hw_ts_mux_id;
                           /**< Hardware timestamping mux ID, if available */
   qurt_elite_mutex_t      riva_drv_mutex;
                           /**< Mutex for updating variables across cmd and data processing threads */
   afe_riva_lpass_data_proc_state  riva_lpass_data_proc_state;
                          /** Device state which indicates RiVA need to process or
                              drop/send zeros */
   uint32_t				   block_transfer_time;
   	   	   	   	   	   	   /** time in ms equivalent to one riva frame */
   uint64_t				   block_transfer_time_us;
   	   	   	   	   	   	   /** time in us equivalent to one riva frame */
} afe_dev_riva_port_t;

/** @brief AFE Riva service declarations. */
typedef struct aud_stat_afe_riva
{
   afe_dev_riva_port_t        riva_port_bt_voc_tx;
                           /**< Port BtVoc Tx instance */
   afe_dev_riva_port_t        riva_port_bt_voc_rx;
                           /**< Port BtVoc rx instance */
   afe_dev_riva_port_t        riva_port_bt_aud;
                           /**< Port BtAud rx instance */
   afe_dev_riva_port_t        riva_port_fm_tx;
                           /**< Port FM Tx instance */
   afe_dev_riva_port_t        riva_port_fm_rx;
                           /**< Port FM Rx instance */
   qurt_elite_interrupt_ist_t riva_intr_struct;
                           /**< ist structure for riva<->lpass ipc interrupt */
   bool_t                  is_riva_intr_registered;
                           /**< flag to check if RIVA interrupt is registered */
   riva_mailbox_t             riva_mailbox;
                           /**< riva<->lpass ipc mailbox, send/rcv message address */
   qurt_elite_mutex_t         bt_rx_tx_aud_sync_mutex;
                           /**< RIVA bt rx/tx mutex for aud_sync purposes. protects below drift. */
   qurt_elite_mutex_t         aud_sync_mutex;
						         /**< Mutex for writting AUD_SYNC_CTRL register */
   bool_t                  is_aud_sync_mutexes_init;
                           /**< flag to make sure aud sync mutexes are initialized*/
   int64_t                    drift_correction_us;
                           /**< Drift corrected on the hardware useful for correction at AV timer int.
                            * BT RX populates it, and BT TX uses it and makes it zero. both happen using above mutex.
                            */
   bool_t                     is_riva_active;
                           /**< set to false when RIVA crashes and riva subsystem is restarting. This flag then prevents subsequent riva access. */
   uint32_t                   hw_revision;
						         /**< Internal BT chip and ADSP interface revision. 0-not supported, 1-first version in B-family. */
   uint16_t                   bt_fm_ipc_int;
						         /**< Internal BT\FM IPC interrupt number. */
   uint16_t                   bt_tx_int;
						         /**< Internal BT Mic path interrupt number. */
   uint16_t                   bt_rx_int;
						         /**< Internal BT speaker path interrupt number. */
   uint16_t                   fm_tx_rx_int;
						         /**< Internal FM interrupt number. */
   uint16                     ipc_send_mail_box_offset;
   uint32_t                   ccu_mb0_ctrl_reg_phy_addr;
   bool_t                     hw_timestamp_available;
                           /**< indicates if HW timestamping block is available to timestamp RIVA interrupts */
   uint64_t                   bt_rx_timestamp;
                           /**< Timestamp for BT RX interrupt. BT rx updates this and BT tx uses it for VFR drift tracking */
   bool_t                     is_bt_aud_sync_enable;
                           /**< BT audio sync enable/disable status. */
   bool_t                     is_fm_aud_sync_enable;
                           /**< FM audio sync enable/disable status. */
   bool_t                  is_crash_handler_registered;
                           /**< Flag to indicate if the RIVA crash handler has been registered. */
   bool_t                  is_recovery_handler_registered;
                           /**< Flag to indicate if the RIVA crash recovery handler has been registered */
   uint32_t                q6ss_ipc_reg_phy_addr;
                           /**< Q6SS IPC register Physical address. */
   uint32_t                q6ss_ipc_reg_ack_bit_pos;
                           /**< Q6SS IPC register interrupt acknowledge bit. */
   uint32_t                ccu_mb0_ctrl_reg_virt_addr;
                           /**< Mail box register virtual address. */
   uint32_t                q6ss_ipc_reg_virt_addr;
                           /**< Q6SS IPC register virtual address. */
   
} aud_stat_afe_riva_t;

/*=====================================================================
 Global Variables
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
 * Perform one time initialization of the AFE Riva driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_init(void);

/**
 * Perform de-initialization of the AFE Riva driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_deinit(void);

/**
 * Perform RIVA compatible port's initialization
 *
 * @param[in] afe_svc_ptr pointer to AFE static service
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_port_init(aud_stat_afe_svc_t* afe_svc_ptr);


/**
 * Perform RIVA compatible port's de-initialization
 *
 * @param[in] afe_svc_ptr pointer to AFE static service
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

ADSPResult afe_set_int_bt_cfg(afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size);
ADSPResult afe_get_int_bt_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

/**
 * Perform RIVA compatible device open
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_open(afe_dev_port_t *pDevPort);

/**
 * Perform RIVA compatible device close
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_riva_close(afe_dev_port_t *pDevPort);

/**
 * Get the current time using Riva port data transfer
 * rate
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[out] curr_time - time stamp in sample ticks
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_get_time_for_riva_port(afe_dev_port_t *pDevPort,uint64 *curr_time);

/**
 * Process the IPC command from Riva, and update the riva port, BT Voc/Aud, FM
 * instance.
 *
 * @param[in] afe_svc_ptr pointer to AfeSvc instance
 */
ADSPResult afe_driver_ipc_cmd_proc(aud_stat_afe_svc_t *afe_svc_ptr);

/**
 * Riva cmd processing routine
 *
 * @param[in] none
 */
void afe_driver_riva_cmd_isr(void*);

/**
  @brief specified port is enabled for VFR drifts.

  @param[in] afe_port_ptr pointer to corresponding AFE port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_riva_add_to_vfr_sync(afe_dev_port_t *afe_port_ptr);

/**
  @brief Get VFR drift

  @param[in] afe_port_ptr, pointer to AFE port
  @param[in] vfr_state_ptr,  ptr vfr source 

  @return  None
*/
void afe_riva_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr);

/**
  @brief Get current AV timer timestamp for riva port

  @param[in] afe_port_ptr, pointer to AFE port
  @param[out] riva_av_timestamp,  AV timer timestamp

  @return  None
*/
void afe_riva_get_av_timestamp(afe_dev_port_t *afe_port_ptr, uint64_t *riva_av_timestamp);

/**
  @brief specified port is disabled for VFR drifts.

  @param[in] afe_port_ptr pointer to corresponding AFE port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_riva_remove_from_vfr_sync(afe_dev_port_t *afe_port_ptr);

/**
 * updates the accumulated drift for riva port
 */
void afe_update_avt_drift_for_riva_port(afe_dev_port_t *dev_port_ptr);

/**
 * For providing the delay introduced in RIVA port circular
 * buffer
 *
 * @param[in] port_id port id of the rt_proxy port
 *@param[out] circ_buf_delay_us is the delay introduced due to  
 *  	circular buffer
  
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_circ_buf_delay(afe_dev_port_t *dev_port_ptr, uint64_t* circ_buf_delay_us);

/**
  @brief Indicates whether RIVA connected to LPASS or not .

  @param[in] afe_port_ptr pointer to corresponding AFE port

  @return  TRUE if connected, FALSE if not connected
   *
 * @dependencies
 * None.
 *
 */
bool_t afe_is_riva_active(afe_dev_port_t* afe_port_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_RIVA_DRIVER_H_ */

