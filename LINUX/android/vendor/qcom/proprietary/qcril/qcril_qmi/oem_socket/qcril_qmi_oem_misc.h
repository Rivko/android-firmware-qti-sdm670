/*!
  @file
  qcril_qmi_oem_misc.h

  @brief

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

#ifndef QCRIL_QMI_OEM_MISC_H
#define QCRIL_QMI_OEM_MISC_H


#include "ril.h"
#include "qcrili.h"
#include "qtiTelephonyService.pb.h"

QtiTelephonyService_RILOEMMessageId qcril_qmi_oem_proto_map_event_to_request(qcril_evt_e_type event);
RIL_Token qcril_qmi_oem_convert_proto_token_to_ril_token(uint32_t oem_token);
uint32_t qcril_qmi_oem_free_and_convert_ril_token_to_proto_token(RIL_Token ril_token);

#endif /* QCRIL_QMI_OEM_MISC_H */
