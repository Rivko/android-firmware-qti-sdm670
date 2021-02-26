#ifndef _AFE_TDM_DRIVER_H_
#define _AFE_TDM_DRIVER_H_
/**
@file AFETdmDriver.h **
This file contains function declarations of TDM interface Driver

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/inc/AFETdmDriver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
14/03/11    mspk      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "AFEInternal.h"
#include "mmdefs.h"
#include "VfrDriverInternal.h"
#include "AFEDeviceDriver.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/**
  @brief Perform TDM Driver Initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_driver_init(void);

/**
  @brief Perform TDM Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_driver_deinit(void);


/**
  @brief Perform TDM port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_port_init(aud_stat_afe_svc_t* afe_svc_ptr);


/**
  @brief Perform TDM port deinitialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);


/**
  @brief Gets TDM port port

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
void afe_tdm_get_dev_port(uint16 unPortId, afe_dev_port_t **pDevPort);



#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_TDM_DRIVER_H_)
