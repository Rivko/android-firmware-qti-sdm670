/*!
  @file
  qcril_qmi_lte_direct_disc_misc.h

  @brief
  Utility functions for lte direct discovery

*/

/*===========================================================================

 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_LTE_DIRECT_DISC_MISC_H
#define QCRIL_QMI_LTE_DIRECT_DISC_MISC_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "ril.h"
#include "qcrili.h"
#include "qcril_qmi_client.h"
#include "LteDirectDiscovery.pb.h"
#include "lte_v01.h"
#include "qcril_qmi_lte_direct_disc_dispatcher.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/

RIL_Token qcril_qmi_lte_direct_disc_convert_lted_token_to_ril_token
(
 uint32_t lted_token
);
uint32_t qcril_qmi_lte_direct_disc_free_and_convert_ril_token_to_lted_token
(
 RIL_Token ril_token
);
LteDirectDiscovery_Error qcril_qmi_lte_direct_disc_convert_ril_error_to_lted_error
(
 RIL_Errno err
);
LteDirectDiscovery_Error qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error
(
 qmi_response_type_v01* qmi_service_response
);
boolean qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril
(
 qmi_lte_disc_result_type_enum_v01   qmi_result,
 LteDirectDiscovery_Result *result
);
LteDirectDiscovery_MsgId qcril_qmi_lte_direct_disc_map_event_to_request
(
 int event
);
boolean qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi
(
 LteDirectDiscovery_DiscoveryType discovery_type,
 qmi_lte_discovery_type_enum_v01          *qmi_discovery_type
);

boolean qcril_qmi_lte_direct_disc_map_ril_category_to_qmi
(
 uint32_t ril_category,
 qmi_lte_disc_category_type_enum_v01 *qmi_category
);

boolean qcril_qmi_lte_direct_disc_map_qmi_category_to_ril
(
 qmi_lte_disc_category_type_enum_v01 qmi_category,
 uint32_t *ril_category
);

boolean qcril_qmi_lte_direct_disc_map_ril_range_to_qmi
(
 uint32_t ril_range,
 qmi_lte_disc_announcing_policy_range_enum_v01 *qmi_range
);

boolean qcril_qmi_lte_direct_disc_map_qmi_range_to_ril
(
 qmi_lte_disc_announcing_policy_range_enum_v01 qmi_range,
 uint32_t *ril_range
);

boolean qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi
(
 com_qualcomm_qti_radioconfiginterface_Plmn *ril_plmn,
 qmi_lte_disc_plmn_id_s_type_v01            *qmi_plmn
);

boolean qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril
(
 qmi_lte_disc_plmn_id_s_type_v01            *qmi_plmn,
 com_qualcomm_qti_radioconfiginterface_Plmn *ril_plmn
);

#endif /* QCRIL_QMI_LTE_DIRECT_DISC_MISC_H */
