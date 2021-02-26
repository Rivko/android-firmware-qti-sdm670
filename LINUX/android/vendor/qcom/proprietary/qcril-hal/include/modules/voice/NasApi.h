/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "telephony/ril.h"
#include "modules/nas/qcril_qmi_nas.h"

typedef struct {
  bool                      stateValid;
  int                       state;
  bool                      radioTechValid;
  RIL_RadioTechnology       radioTech;
  bool                      statusOverviewValid;
  uint32_t                  statusOverview;
} voiceRegStatusStruct;

typedef struct {
  bool                      voiceRteValid;
  qmi_ril_nw_reg_rte_type   voiceRte;
  bool                      dataRteValid;
  qmi_ril_nw_reg_rte_type   dataRte;
  bool                      imsRteValid;
  qmi_ril_nw_reg_rte_type   imsRte;
} networkRteStruct;

void qcril_qmi_voice_update_voice_registration_status
(
  voiceRegStatusStruct& voiceReg
);

void qcril_qmi_voice_update_service_rte_info
(
  networkRteStruct& networkRte
);
