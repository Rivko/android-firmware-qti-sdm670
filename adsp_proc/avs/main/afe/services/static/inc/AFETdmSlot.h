/*========================================================================


  This file contains functions to handle tdm modules.

  Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  06/04/14   david    Created file.

  ========================================================================== */
/**
@file AFETdmSlot.h

@brief contains functions to handle TDM module.

*/

#ifndef AFETDMSLOT_H
#define AFETDMSLOT_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*==========================================================================
  Function Prototypes
========================================================================== */

/**
  This function set the params for the tdm module
  AFE_MODULE_TDM. It supports the following Params:
  AFE_PARAM_ID_PORT_TDM_SLOT_CONFIG

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration data to fetch.
  @param[in] param_size  Actual Param size

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */

ADSPResult afe_port_set_tdm_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
                                                          int8_t *param_buffer_ptr, uint16_t param_size);



/**
  This function handles custom header

  @param[in] pDevPort Pointer to the instance of the port.

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_tdm_custom_header_handle(void *p_dev_port);

/**
  This function handles slot mapping offset convert

  @param[in] p_slot_offset_info Pointer to the instance of tdm_slot_offset_info_t
  @param[in] width_per_slot - width of slot.

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_tdm_slot_mapping_offset_convert(tdm_slot_offset_info_t *p_slot_offset_info, int16_t width_per_slot);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AFETDMSLOT_H