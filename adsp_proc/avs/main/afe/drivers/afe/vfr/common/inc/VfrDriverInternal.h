/*========================================================================
  This file contains AFE Vocoder Frame Reference handler related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/common/inc/VfrDriverInternal.h#1 $
 ====================================================================== */
#ifndef _VFR_DRV_INTERNAL_H_
#define _VFR_DRV_INTERNAL_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "mmdefs.h"
#include "AFEAvtDrift.h"
#include "Elite.h"
#include "AFEVfrHandler.h"
#include "hwd_devcfg.h"
#include "qurt_elite.h"
#include "AFEDevCommon.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

#define VFR_FRAME_SIZE_IN_US 				      20000
#define VFR_FRAME_SIZE_IN_MS 				      20
#define AFE_MIN_DRIFT_ADJUST_US				   -2
#define AFE_MAX_DRIFT_ADJUST_US      		   2


/** Number of microseconds per second */
#define NUM_MICROSEC_PER_SEC     		(1000000)

/** Number of millisec per second */
#define NUM_MILLISEC_PER_SEC     		(1000)

/** Positive drift threshold for sw latching in number of
 *  samples */
#define VFR_SW_LATCH_DRIFT_POS_THRESH     	(3)

/** Negative drift threshold for sw latching in number of
 *  samples */
#define VFR_SW_LATCH_DRIFT_NEG_THRESH     	(-VFR_SW_LATCH_DRIFT_POS_THRESH)

typedef enum vfr_hw_latch_ver
{
   VFR_HW_LATCH_V1 = 0,    /**< Counter based latching */
   VFR_HW_LATCH_V2 = 1,    /**< AV Timer based latching */
}vfr_hw_latch_ver_t;

#define VFR_HW_VERSION_1   0x10000000  /* This macro is internal h/w version; not hw regsiter value*/

/*==========================================================================
  Structure definitions
  ========================================================================== */

/** Function pointer for VFR ISR handler */
typedef void (*vfr_isr_handler_t)(void *);


typedef struct vfr_hw_info
{
   uint32               reg_base_addr;
                        /**< VFR reg base addr : LPASS_VFR_IRQ_MUX_CTL */
   uint32               reg_size;     
                        /**< VFR reg page size */
   uint32               vfr_ts_mux_offset1;  
                        /**<  LPASS_VFR_IRQ_MUX_CTL_1 */
   uint32               gp_ts_mux_offset0; 
                        /**<  LPASS_GP_IRQ_TS_MUX_CTL_0 */
   uint32               gp_ts_mux_offset1; 
                        /**<LPASS_GP_IRQ_TS_MUX_CTL_1  */                     
   uint32               hw_revision;
                        /**< VFR hw revision, 0: unsupported */
   uint32               reg_virt_base;
                        /**< VFR reg virtual addr base */
   qurt_mem_region_t    vfr_qmem_region;
                        /**< VFR register mapping region */
   uint32_t             num_vfr_ts_mux;
                        /**< Number of VFR TS Mux's available */
   uint32_t             num_gp_ts_mux;
                        /**< Number of GP TS Mux's available */
   uint32_t             available_l2vic_num[VFR_IRQ_SRC_SIZE];
                        /**< Available l2vic numbers for VFT TS mux */
   lpasshwio_irq_type_t vfr_irq_type;
                        /**< VFR interrupt type */
}vfr_hw_info_t;


typedef struct vfr_src_state
{
   bool_t               is_initialized;
                        /**< Flag for VFR init */
   vfr_src_prop_t       vfr_prop;
                        /**< VFR hw prop object */
   uint32               l2vic_num;
                        /**< L2VIC # for this object */
   vfr_isr_handler_t    vfr_isr_handler;
                        /**< ISR handler for the VFR */
   qurt_elite_interrupt_ist_t  vfr_isr_info;
                        /**< Structure that contains information for 
                             registering interrupt using QuRT API's */
   bool_t               is_vfr_enabled;
                        /**< Whether VFR is enabled. */
   bool_t               is_hw_latched;
                        /**< Whether VFR is hw or sw latched */
   uint32_t             mux_id;
                        /**< If HW latched, id of the corresponding Mux */
   uint64_t			      cur_av_time_stamp;
                        /**< AV timer time stamp for current VFR */
   uint64_t			      prev_av_time_stamp;
                        /**< AV timer time stamp for previous VFR */
   bool_t 			      is_first_vfr;
                        /**< Is it first VFR */
   uint8_t 			      vfr_src_index;
                        /**< VFR source index */
   qurt_elite_signal_t  vfr_signal;
                        /**< VFR signal. */
   qurt_elite_mutex_t   vfr_mutex;
                        /**< VFR mutex. */
   vfr_client_info_t    vfr_client_info;
                        /**< VFR client information. */
   uint32_t             hw_latch_ver;
                        /**< VFR hw latching version, counter or timer type */
   uint32_t             vfr_period;
                        /**< Time period for this VFR */
   void*                avt_drv_hdl;
                        /**< Handle of the AV Timer driver. */
   afe_dev_intr_prop_t  intr_prop;
                        /**< Interrupt prop struct */
} vfr_src_state_t;


typedef struct vfr_drv_state
{
   qurt_elite_thread_t     vfr_thread_id;
									/**< VFR Svc thread ID */
   qurt_elite_queue_t      *vfr_cmdq_ptr;
									/**< cmdQ for handling VFR */
   vfr_hw_info_t           hw_info;
								   /**< VFR hw info */
   uint32_t                vfr_irq_src_map[VFR_IRQ_SRC_SIZE];
								   /**< Mux irq ctl to VFR srx idx map */
   uint32_t                hw_latch_ver;
                           /**< VFR hw latching version, counter or timer type */
   vfr_src_state_t         vfr_state_info[NUM_MAX_VFR_SRC];
								   /**< VFR source state object */
   uint32_t                hw_latched_vfr_counter;
								   /**< HW latched VFR counter */
}vfr_drv_state_t;


extern vfr_drv_state_t vfr_drv_global_stat;

/*==========================================================================
  Function definitions
  ========================================================================== */
/**
  Creates the VFR driver thread which will be used for VFR
   drift feedback calculations

  @param[in]  None

  @result
  None

  @dependencies
  None.
*/
ADSPResult vfr_handler_init(void);

/**
  Destory the VFR driver thread 

  @param[in]  None

  @result
  None

  @dependencies
  None.
*/
void vfr_handler_deinit(void);

/**
  Enable a VFR source to by synced for a particular port

  @param[in]  p_drift_info: pointer to port drift info object
        corresponding to a particular VFR source
  @param[in]  sample_rate: AFE Port sampling rate
  @param[in]  port_id: AFE Port ID

  @result
  ADSP_EOK on success

  @dependencies
  None.
*/
ADSPResult afe_drv_add_to_vfr_sync(afe_drift_info_t *p_drift_info, uint32_t sample_rate, uint16_t port_id);

/**
  Disable a VFR source to by synced for a particular port

  @param[in]  p_drift_info: pointer to port drift info object
        corresponding to a particular VFR source

  @result
  ADSP_EOK on success

  @dependencies
  None.
*/
ADSPResult afe_drv_remove_from_vfr_sync(afe_drift_info_t *p_drift_info);

/**
  Resync the port's drift info when VFR source is disabled and 
  renabled while the port is still in RUN state 

  @param[in]  p_drift_info: pointer to port drift info object
        corresponding to a particular VFR source
  @result
  None

  @dependencies
  None.
*/
void afe_drv_resync_port_vfr_drift_info(afe_drift_info_t *p_drift_info);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _VFR_DRV_INTERNAL_H_ */
