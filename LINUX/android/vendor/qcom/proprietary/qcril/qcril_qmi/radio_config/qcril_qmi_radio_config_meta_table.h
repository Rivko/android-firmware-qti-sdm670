/******************************************************************************
  @file    qcril_qmi_radio_config_meta_table.h

  DESCRIPTION
    Handles imss related radio config req and response handlers

  ---------------------------------------------------------------------------

  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_QMI_CONFIG_META_TABLE_H
#define QCRIL_QMI_CONFIG_META_TABLE_H

extern const qcril_qmi_radio_config_meta_type qcril_qmi_radio_config_meta_data_v01[];

size_t get_length_meta_table_v01(void);
#endif
