#ifndef _ADSP_AFE_SERVICE_MODULES_PARAMS_H_
#define _ADSP_AFE_SERVICE_MODULES_PARAMS_H_

/*========================================================================*/
/**
@file adsp_afe_service_modules_params.h

This file contains AFE service module ID and param ID definitions proprietary
to Qualcomm Technologies, Inc.
 */
/*===========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================ */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/public/adsp_afe_service_modules_params.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/20/16    sj     Separate header file for proprietary module and param ID's
========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#define AFE_TX_TOPOLOGY_ID_SWMAD_HWMAD                 0x00012E19
#define AFE_TX_TOPOLOGY_ID_SWMAD                       0x00012E18
#define AFE_TX_TOPOLOGY_ID_AANC                        0x0001025B
#define AFE_TX_TOPOLOGY_ID_SW_DUTY_CYCLE_MAD           0x00013093

#define AFE_MODULE_SIDETONE_IIR_FILTER                 0x00010202
#define AFE_MODULE_AANC                                0x00010214
#define AFE_MODULE_ID_SW_MAD                           0x0001022D

#define AFE_MODULE_SPEAKER_PROTECTION_EX_VI         0x0001026F
#define AFE_MODULE_SPEAKER_PROTECTION_TH_VI         0x0001026A

#define AFE_PORTS_CMD_DTMF_CTL                         0x00010102


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AFE_SERVICE_MODULES_PARAMS_H_ */
