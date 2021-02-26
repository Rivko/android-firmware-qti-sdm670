/**
@file adsp_lsm_service_modules_params.h

@brief This file contains LSM service related module ID, param ID
and topology defintions which are proprietary to Qualcomm Technologies, Inc.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      LSM_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
======================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/prop/adsp_lsm_service_modules_params.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  08/24/2016 sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
  08/03/2016 sj      API header file split for HY22 package
==============================================================================*/

#ifndef ADSP_LSM_SERVICE_MODULES_PARAMS_H_
#define ADSP_LSM_SERVICE_MODULES_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "mmdefs.h"

/*
 * Static Topologies are topologies pre-defined in the LSM service and
 * available built-in the ADSP. They can be directly used in
 * LSM_SESSION_CMD_OPEN_TX_V2 wihtout using LSM_CMD_ADD_TOPOLOGIES *
 */

/** @addtogroup lsmtopo_ids
@{ */

/** ID of the LSM Tx topology for the Detection Engine Version 2 module
    (LSM_MODULE_VOICE_WAKEUP_V2)
*/
#define LSM_TX_TOPOLOGY_ID_VOICE_WAKEUP_V2    0x00012C0B

/** ID of the LSM Tx topology for the Detection Engine Version 3 module
    (LSM_MODULE_VOICE_WAKEUP_V3)
*/
#define LSM_TX_TOPOLOGY_ID_VOICE_WAKEUP_V3    0x00012C1D

/** ID of the LSM Tx topology for the Audio Context Detection module
    (LSM_MODULE_ACD).
*/
#define LSM_TX_TOPOLOGY_ID_ACD    0x00012C1F

    /** @} *//* end_addtogroup lsmtopo_ids */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_LSM_SERVICE_MODULES_PARAMS_H_ */
