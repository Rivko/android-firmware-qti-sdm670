/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/legacy.h>
#include "qcrili.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "modules/nas/qcril_qmi_prov.h"
#include "modules/nas/qcril_qmi_nas2.h"
#include "qcril_qmi_voice.h"
#include "qcril_qmi_pdc.h"
#include "qcril_pbm.h"
#include "qcril_other.h"
#include "modules/uim/UimOemHook.h"
#include "modules/data/DataCommon.h"

#define QCRIL_LEGACY_EVENT QCRIL_DECLARE_LEGACY_EVENT
#include "qcril_qmi_oem_eventlist.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void             qcril_qmi_oem_events_init();
qcril_evt_e_type qcril_qmi_oem_get_event(int oemhook_msg);
int              qcril_qmi_oem_get_oemhook_msg(qcril_evt_e_type event);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

