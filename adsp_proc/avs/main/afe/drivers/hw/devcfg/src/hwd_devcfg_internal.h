/*========================================================================
hwddevcfg_internal.h

This file contains the implementation of avtimer server.

Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
Qualcomm Technologies, Inc. (QTI) Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */
#ifndef HWDDEVCFG_INTERNAL_H
#define HWDDEVCFG_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "hwd_devcfg.h"

  static ADSPResult HwdDevCfg_ReadAudioIfConfigData(lpasshwio_prop_lpaif_struct_t *pDev);
  static ADSPResult HwdDevCfg_ReadGenericclkConfigData(HwdGenericClkPropertyType *pDev);
  static ADSPResult HwdDevCfg_ReadAvtimerConfigData(HwdAvtimerPropertyType *pDev);
  static ADSPResult HwdDevCfg_ReadResamplerConfigData(HwdResamplerPropertyType *pDev);
  static ADSPResult HwdDevCfg_ReadSpdifConfigData(lpasshwio_prop_spdif_struct_t *pDev);
  static ADSPResult HwdDevCfg_ReadHdmiOutputConfigData(lpasshwio_prop_hdmi_output_struct_t *pDev);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef HWDDEVCFG_INTERNAL_H
