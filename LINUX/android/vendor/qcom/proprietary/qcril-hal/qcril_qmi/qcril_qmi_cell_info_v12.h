/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_cell_info.h
  @brief   qcril qmi - CELL_INFO

  DESCRIPTION
    Handles CELL_INFO_LIST responses

******************************************************************************/
#ifndef QCRIL_QMI_CELL_INFO_V12_H_INCLUDED
#define QCRIL_QMI_CELL_INFO_V12_H_INCLUDED

#include <network_access_service_v01.h>

int qcril_cell_location_to_ril_cell_info_v12(struct qcril_chain_data *data);

#endif /* QCRIL_QMI_CELL_INFO_V12_H_INCLUDED */
