/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_cell_info.h
  @brief   qcril qmi - CELL_INFO

  DESCRIPTION
    Handles CELL_INFO_LIST responses

******************************************************************************/
#ifndef QCRIL_QMI_CELL_INFO_H_INCLUDED
#define QCRIL_QMI_CELL_INFO_H_INCLUDED


#include <network_access_service_v01.h>
#include "qcrili.h"

struct qcril_chain_data
{
  void *parm1;
  void *parm2;

  void *input;
  void *output;

  void *ret1;
  void *ret2;
};

#define QCRIL_NCC_FROM_BSIC(bsic) ({ \
  uint8_t b = (bsic);                \
  uint8_t ncc = 0xff;                \
  if (b != 0xff)                     \
  {                                  \
    ncc = (b >> 3) & 0x07;           \
  }                                  \
  ncc;                               \
})

#define QCRIL_BCC_FROM_BSIC(bsic) ({ \
  uint8_t b = (bsic);                \
  uint8_t bcc = 0xff;                \
  if (b != 0xff)                     \
  {                                  \
    bcc = b & 0x07;                  \
  }                                  \
})

#define QCRIL_MAKE_BSIC(ncc,bcc) ({ \
  uint8_t n = (ncc);         \
  uint8_t b = (bcc);         \
  uint8_t bsic = 0xff;              \
  if (n != 0xff && b != 0xff)       \
  {                                 \
    n = n & 0x07;                   \
    b = b & 0x07;                   \
    bsic = ( n << 3 ) | b;          \
  }                                 \
  bsic;                             \
})

int qcril_cell_info_process(
  nas_get_cell_location_info_resp_msg_v01 *rsp,
  uint64_t *timestamp,
  void **response,
  unsigned *resp_len,
  unsigned *ncells
);

uint8_t qcril_qmi_nas_is_cell_info_equal_member
(
  void *first,
  void *second,
  size_t size
);

uint8_t qcril_qmi_nas_is_cell_info_equal
(
  RIL_CELL_INFO_LIST_TYPE *first,
  RIL_CELL_INFO_LIST_TYPE *second
);

unsigned qcril_qmi_nas_count_cell_location_info_cells
(
  nas_get_cell_location_info_resp_msg_v01 *cell_location_info
);

unsigned qcril_qmi_nas_count_cell_location_info_cells
(
  nas_get_cell_location_info_resp_msg_v01 *cell_location_info
);

void qcril_qmi_nas_init_gsm_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
);

void qcril_qmi_nas_init_gsm_cell_identity_v6
(
  RIL_CELL_INFO_LIST_GSM_ID_TYPE *cell,
  char * plmn,
  uint8_t  lac_valid,
  uint16_t lac,
  uint8_t  cid_valid,
  uint32_t cid
);

void qcril_qmi_nas_init_gsm_cell_ss_v6
(
  RIL_CELL_INFO_LIST_GSM_SS_TYPE *cell,
  int radio_if,
  uint8_t ss_valid,
  int16_t ss,
  uint8_t ber_valid,
  int     bitErrorRate
);

unsigned qcril_qmi_nas_convert_geran_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
);

void qcril_qmi_nas_init_wcdma_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
);

void qcril_qmi_nas_init_wcdma_cell_identity_v6
(
  RIL_CELL_INFO_LIST_WCDMA_ID_TYPE *cell,
  char * plmn,
  uint8_t  lac_valid,
  uint16_t lac,
  uint8_t  cid_valid,
  uint32_t cid,
  uint8_t  psc_valid,
  uint16_t psc
);

void qcril_qmi_nas_init_wcdma_cell_ss_v6
(
  RIL_SignalStrengthWcdma *cell,
  int      radio_if,
  uint8_t  ss_valid,
  int16_t  ss,
  uint8_t  ber_valid,
  int      bitErrorRate
);

unsigned qcril_qmi_nas_convert_umts_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
);

void qcril_qmi_nas_initialize_cdma_ril_cellinfo
(
  RIL_CELL_INFO_LIST_TYPE *cell_info,
  boolean srv,
  RIL_TimeStampType timestampType,
  uint64_t *timestamp
);

unsigned qcril_qmi_nas_convert_cdma_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
);

int qcril_qmi_nas_qmi_ta_to_ril_ta(uint8_t ta_valid, uint32_t ta);

void qcril_qmi_nas_init_lte_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
);

void qcril_qmi_nas_init_lte_cell_identity_v6
(
  RIL_CELL_INFO_LIST_LTE_ID_TYPE *cell,
  uint8_t *plmn,
  uint8_t  tac_valid,
  uint16_t tac,
  uint8_t  cid_valid,
  uint32_t cid,
  uint8_t  pci_valid,
  uint16_t pci
);

void qcril_qmi_nas_init_lte_cell_ss_v8
(
  RIL_LTE_SignalStrength_v8 *cell,
  int      radio_if,
  uint8_t  ss_valid,
  int16_t  ss,
  uint8_t  rsrp_valid,
  int16_t  rsrp,
  uint8_t  rsrq_valid,
  int16_t  rsrq,
  uint8_t  rssnr_valid,
  int      rssnr,
  uint8_t  cqi_valid,
  int      cqi,
  uint8_t  ta_valid,
  uint32_t ta
);

unsigned qcril_qmi_nas_convert_lte_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
);

void qcril_ril_cellinfo_free
(
  RIL_CELL_INFO_LIST_TYPE *cellinfo
);

void qcril_qmi_nas_convert_cell_location_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  RIL_CELL_INFO_LIST_TYPE **cellinfo,
  uint64_t *time,
  unsigned *number_of_cells
);

struct qcril_chain_data;

int qcril_cell_location_to_ril_cell_info_v6(struct qcril_chain_data *data);

#endif /* QCRIL_QMI_CELL_INFO_H_INCLUDED */
