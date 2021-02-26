/*========================================================================
  This file contains timestamp register programming APIs

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/inc/AfeTimeStampDriver.h#1 $
 ====================================================================== */
#ifndef _AFE_TIMESTAMP_DRIVER_H_
#define _AFE_TIMESTAMP_DRIVER_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "qurt_elite.h"
#include "VfrDriverInternal.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */
#define AFE_TS_SOURCE_QTIMER               0
#define AFE_TS_SOURCE_AVTIMER              1


enum
{
   AFE_TS_VFR0_MUX_ID = 0, //Mux ID for first mux controls whihc of several interupts to pass through LPASS.
   AFE_TS_VFR1_MUX_ID,     //Mux ID for second mux controls whihc of several interupts to pass through LPASS.
   AFE_TS_SENSORS_MUX_ID,  // Not exists yet.
   AFE_TS_GP0_MUX_ID,      // Mux ID for SECOND VFR mux which includes the original 32-input plus the entire QDSP6SS L2VIC vector (174 inputs) used only for timestamping
   AFE_TS_GP1_MUX_ID,      //Mux ID for THIRD VFR mux which includes the original 32-input plus the entire QDSP6SS L2VIC vector (174 inputs) used only for timestamping.
   AFE_TS_INVALID_MUX_ID = 0xFFFFFFFF
};

typedef enum hw_ts_intr_type
{
   HW_TS_INTR_TYPE_VFR = 0, // VFR interrupt types
   HW_TS_INTR_TYPE_BT_TX,   // BT TX interrupt type
   HW_TS_INTR_TYPE_BT_RX,   // BT RX interrupt type
   HW_TS_INTR_TYPE_FM,      // FM interrupt type
}hw_ts_intr_type_t;


/*==========================================================================
  Function prototypes
  ========================================================================== */

/**
  Function for initializing time stamp driver

  @param[in]  vfr_hw_info_ptr: vfr hw info

  @result
  None

  @dependencies
  None.
*/
ADSPResult afe_ts_drv_init(vfr_hw_info_t *vfr_hw_info_ptr);

/**
  Function for releasing the mux
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_release_ts_mux(uint32_t mux_id);

/**
  Function for configuring the mux for interrupt source selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  mux_ctl_sel: Mux control input index

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_set_source_type(uint32_t mux_id, uint32 mux_ctl_sel);


/**
  Function for configuring the mux for timer source selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  timer_source: Qtimer/AV-timer

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_set_timer_source(uint32_t mux_id, uint32 timer_source);


/**
  Function for reading the timestamp (us) from latched STC counter

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block
  @param[out] timestamp: latched timestamp (us)

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_get_timestamp(uint32_t mux_id, uint64_t *timestamp);


/**
  Function for reading the latched STC counter

  @param[in]  interupt type: VFR, BT_TX, BT_RX, FM
  @param[in]  mux_ctl_sel_in: VFR mux input index
  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block 


  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_config_vfr_ts_mux(uint32_t intr_type, uint32_t mux_ctl_sel_in, uint32_t *mux_id);

#endif /*#if _AFE_TIMESTAMP_DRIVER_H_*/
