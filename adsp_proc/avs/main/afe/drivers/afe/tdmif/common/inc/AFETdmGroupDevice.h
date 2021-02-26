/*========================================================================
   This file contains declarations

  Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/inc/AFETdmGroupDevice.h#1 $
 ====================================================================== */

#ifndef _AFE_TDM_GROUP_DEVICE_H
#define _AFE_TDM_GROUP_DEVICE_H


/**
  @brief Perform TDM group device configuration

  @param[in] param_id Id of the parameter
             param_buffer_ptr pointer to the parameter buffer
             param_size size of the parameter

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size);

#endif



