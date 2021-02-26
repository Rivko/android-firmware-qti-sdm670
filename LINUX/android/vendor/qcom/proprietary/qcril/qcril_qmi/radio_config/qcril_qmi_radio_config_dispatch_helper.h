/*!
  @file
  qcril_qmi_radio_config_dispatch_helper.h

  @brief
  Function handlers for set/get config requests
  Analyzes and processes the request data.
  Handles sending response/error response.
*/
/*===========================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_RADIO_CONFIG_DISPATCH_HELPER_H
#define QCRIL_QMI_RADIO_CONFIG_DISPATCH_HELPER_H

#include "qcrili.h"
/**
 * Analyzes and processes the set_config request data.
 */
void qcril_qmi_radio_config_dispatch_set_request
(
	const qcril_request_params_type *const params_ptr,
	qcril_request_return_type *const ret_ptr /*!< Output parameter */
);


/**
 * Analyzes and processes the get_config request data.
 */
void qcril_qmi_radio_config_dispatch_get_request
(
	const qcril_request_params_type *const params_ptr,
	qcril_request_return_type *const ret_ptr /*!< Output parameter */
);


#endif /* QCRIL_QMI_RADIO_CONFIG_DISPATCH_HELPER_H */
