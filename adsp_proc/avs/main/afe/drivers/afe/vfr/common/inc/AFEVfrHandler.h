/*========================================================================
  This file contains AFE Vocoder Frame Reference handler related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/common/inc/AFEVfrHandler.h#1 $
 ====================================================================== */
#ifndef _AFE_VFR_HANDLER_H_
#define _AFE_VFR_HANDLER_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "mmdefs.h"
#include "AFEAvtDrift.h"
#include "Elite.h"
#include "audio_divide_qx.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

#define AFE_ACC_VFR_DRIFT_US_SHIFT  		   32
#define AFE_VFR_DRIFT_FRAME_COUNTER_SHIFT    0

/** Internal VFR source */
#define AFE_VFR_SOURCE_INTERNAL  0

/** External VFR source */
#define AFE_VFR_SOURCE_EXTERNAL  1

/** IRQ mux control select */
typedef enum _vfr_irq_src_type
{
   VFR_IRQ_MUX_CTL_SEL_INTERNAL    = 0x0,
   VFR_IRQ_MUX_CTL_SEL_EXTERNAL    = 0x1,
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_0  = 0x2, 
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_1  = 0x3,
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_2  = 0x4,
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_3  = 0x5,
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_4  = 0x6,
   VFR_IRQ_MUX_CTL_SEL_DIR_CONN_5  = 0x7,  /* Internal VFR 2, as per broken connection to mux */
} vfr_irq_src_type;

#define VFR_IRQ_SRC_START_IDX     (VFR_IRQ_MUX_CTL_SEL_INTERNAL)
#define VFR_IRQ_SRC_END_IDX       (VFR_IRQ_MUX_CTL_SEL_DIR_CONN_5)

/** VFR IRQ mux control input size */
#define VFR_IRQ_SRC_SIZE   (VFR_IRQ_SRC_END_IDX - VFR_IRQ_SRC_START_IDX + 1)

#define VFR_IRQ_SRC_INVALID   (0xFFFFFFFF)

#define NUM_MAX_VFR_SRC    2


/** VFR client interface version for Non-VSID based VFR
 *  source ID's */
#define VFR_CLIENT_INFO_VER_1    0

/** VFR client interface version for VSID (Voice System ID)
 *  based VFR source ID's */
#define VFR_CLIENT_INFO_VER_2    1

/** VFR client interface max supported version */
#define VFR_CLIENT_INFO_MAX_SUPPORTED_VER    (VFR_CLIENT_INFO_VER_2)

/** Bit position for the VFR_ID in the VSID
 *  BIT: 21:18  */
#define VSID_VFR_ID_MASK_BIT_POS    (18) 

/** VFR_ID bit mask value in the VSID
 *  4-bits for VFR ID */
#define VSID_VFR_ID_MASK_VAL        (0xF)

/** VFR_ID bit mask in the VSID */ 
#define VSID_VFR_BIT_MASK           (VSID_VFR_ID_MASK_VAL << VSID_VFR_ID_MASK_BIT_POS)


extern uint32_t *vfr_irq_src_map;

/*==========================================================================
  Structure definitions
  ========================================================================== */

/** @brief Vocoder Frame Reference (VFR) information. */
typedef struct afe_dma_vfr_info
{
   uint8_t                 vfr_client_info_ver;
                           /**< VFR client interface version info 
                                Supported values:
                                VFR_CLIENT_INFO_VER_1 (V1)
                                VFR_CLIENT_INFO_VER_2 (V2) */
   uint32_t						vfr_source;
                           /**< VFR source that caused this VFR singaling to clients. */
   uint8_t                 resync_status;
                           /**< Quick resync status. */
   uint64_t                vfr_av_timestamp_us;
                           /**< AV timer time stamp captured at VFR interrupt */
} vfr_info_t;

/*
 * drift info that's obtained after reading the 64bit vfr_drift_info from AFE.
 */
typedef struct afe_client_vfr_drift_info
{
   int32_t     acc_vfr_drift_us;
               /**< vfr drift in us */
   uint32_t    frame_counter;
               /**< frame counter */
} afe_client_vfr_drift_info_t;

/** @brief VFR client configuration. */
typedef struct vfr_client_info
{
   uint8_t                 vfr_client_info_ver;
                           /**< VFR client interface version info 
                                Supported values:
                                VFR_CLIENT_INFO_VER_1 (V1)
                                VFR_CLIENT_INFO_VER_2 (V2)
                           */
   qurt_elite_queue_t      *vfr_clientq;
                           /**< Client queue for the VFR configuration. */
   uint32_t                client_msg_id;
                           /**< Client message ID for the VFR configuration
                                message. */
   uint32_t                 vfr_source;
                           /**< Supported values:
                               For V1, 0: Internal, 1: External
                               For V2, 32-bit VSID (Voice System ID) */
} vfr_client_info_t;

/**
 * Reads all 64 bits of afe_vrr_drift_info in one shot & gives 
 * the values in acc_vfr_drift & frame_counter 
 */


static inline void afe_port_read_vfr_drift(afe_drift_info_t *afe_drift_ptr, afe_client_vfr_drift_info_t *drift_info, uint8_t vfr_source)
{
   if (NULL != afe_drift_ptr)
	{
      /* Read all 64 bits in one shot, MEMD */
	   uint64_t vfr_drift_info  = afe_drift_ptr[vfr_source].vfr_drift_info; 

	   drift_info->acc_vfr_drift_us  = (int32_t) (vfr_drift_info >> AFE_ACC_VFR_DRIFT_US_SHIFT);
	   drift_info->frame_counter     = (uint32_t)(vfr_drift_info >> AFE_VFR_DRIFT_FRAME_COUNTER_SHIFT);
	}
}

static inline void vfr_drv_read_vfr_drift(afe_drift_info_t *afe_drift_ptr, afe_client_vfr_drift_info_t *drift_info, 
                                          uint8_t vfr_client_info_ver, uint32_t vfr_source)
{
   uint32_t vfr_src_idx = 0;
   uint32_t mux_ctl_sel = 0;

   switch (vfr_client_info_ver)
   {
      case VFR_CLIENT_INFO_VER_1:
         {
            vfr_src_idx = vfr_irq_src_map[vfr_source];
         }
         break;
      case VFR_CLIENT_INFO_VER_2:
         {
            mux_ctl_sel = ((vfr_source & VSID_VFR_BIT_MASK) >> VSID_VFR_ID_MASK_BIT_POS);
            if((mux_ctl_sel < VFR_IRQ_SRC_SIZE) && (NULL != vfr_irq_src_map))
            {
               vfr_src_idx = vfr_irq_src_map[mux_ctl_sel];
            }
         }
         break;
      default:
         break;
   }

	if (NULL != afe_drift_ptr)
	{
      /* Read all 64 bits in one shot, MEMD */
	   uint64_t vfr_drift_info  = afe_drift_ptr[vfr_src_idx].vfr_drift_info;

	   drift_info->acc_vfr_drift_us  = (int32_t) (vfr_drift_info >> AFE_ACC_VFR_DRIFT_US_SHIFT);
	   drift_info->frame_counter     = (uint32_t)(vfr_drift_info >> AFE_VFR_DRIFT_FRAME_COUNTER_SHIFT);
	}
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_VFR_HANDLER_H_ */
