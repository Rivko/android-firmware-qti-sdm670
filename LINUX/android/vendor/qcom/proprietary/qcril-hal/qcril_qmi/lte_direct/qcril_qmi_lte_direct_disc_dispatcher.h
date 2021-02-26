/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_lte_direct_disc_dispatcher.h

  @brief
  Dispatcher for LTE-D provisioning requests

*/

#ifndef QCRIL_QMI_LTE_DIRECT_DISC_DISPATCHER_H
#define QCRIL_QMI_LTE_DIRECT_DISC_DISPATCHER_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include <telephony/ril.h>
#include "qcrili.h"
#include "qcril_qmi_client.h"


#include "radio_config_interface.pb.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
uint8_t qcril_qmi_lte_direct_disc_dispatcher_req_handler
(
 com_qualcomm_qti_radioconfiginterface_ConfigMsg *config_msg_ptr,
 com_qualcomm_qti_radioconfiginterface_ConfigMsg *resp_ptr /* Out parameter */
);

#endif /* QCRIL_QMI_LTE_DIRECT_DISC_DISPATCHER_H */
