#ifndef __AFE_USBA_DRIVER_I_H__
#define __AFE_USBA_DRIVER_I_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/usba/src/AFEUSBAudioDriver_i.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEUSBAudioDriver_i.h

DESCRIPTION: This file should declaration of internal structures used by USB Audio driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/dev/avs.adsp/2.8/mangeshk.AVS.ADSP.2.8.USBAudio/afe/drivers/afe/usba/src/AFEUSBAudioDriver_i.h#1 
$ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
03-16-2016  mk  Initial Draft

============================================================================*/
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "qurt_elite.h"
#include "AFEUSBAudioDriver.h"
#include "adsp_afe_service_commands.h"
#include "usb_afe.h"
#include "avtimer_drv_api.h"
#include "AFEDeviceDriver.h"
/*=====================================================================
 Macros
 ======================================================================*/

typedef struct usba_driver_state
{
  bool_t                  is_initialized;
  /* is usb audio driver initialized */
} usba_driver_state_t;

typedef struct afe_usba_driver_t
{
  uac_hnd      usba_driver_hdl;     /**< Handle to usb audio driver */
  int32_t  acc_avt_drift_us;
  int64_t avtimer_qtimer_offset;
  uac_session_data_path_delay_t data_path_delay;
}afe_usba_driver_t;

typedef struct afe_usba_dev_cfg_info
{
  uint32_t cfg_version;
  uint32_t dev_params_version;
  uint32_t lpcm_fmt_version;
  uint32_t sample_rate;
  uint32_t usb_device_token;
  uint16_t bit_width;
  uint16_t bytes_per_channel;
  uint16_t num_channels;
  uint16_t data_format;
  uac_session_lpcm_med_fmt_t lpcm_med_format;
} afe_usba_dev_cfg_info_t;


typedef struct afe_usba_dev_state_struct_t
{
  // high level driver specific handles
  afe_dev_port_t *afe_port_ptr;

  // configuration passed from apps
  afe_usba_dev_cfg_info_t afe_usba_dev_cfg;

  afe_usba_driver_t usb_drv;

  bool_t is_usb_device_token_rcvd;

} afe_usba_dev_state_struct_t;

/*=====================================================================
 Typedef's
 ======================================================================*/
/*=====================================================================
 Functions
 ======================================================================*/



#endif /*#ifndef __AFE_USBA_DRIVER_I_H__*/
