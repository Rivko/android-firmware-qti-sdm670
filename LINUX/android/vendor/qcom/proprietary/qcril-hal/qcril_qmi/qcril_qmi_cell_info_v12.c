/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_cell_info.c
  @brief   qcril qmi - CELL_INFO

  DESCRIPTION
    Handles CELL_INFO_LIST responses

******************************************************************************/

#include "utils_standard.h"
#include "qcril_qmi_cell_info.h"
#include "qcril_qmi_cell_info_v12.h"
#include "qcril_log.h"
#include "modules/nas/qcril_qmi_nas.h"


void qcril_qmi_nas_init_gsm_cell_identity_v12
(
  RIL_CELL_INFO_LIST_GSM_ID_TYPE *cell,
  char *plmn,
  uint8_t  lac_valid,
  uint16_t lac,
  uint8_t  cid_valid,
  uint32_t cid,
  uint8_t  arfcn_valid,
  uint16_t arfcn,
  uint8_t bsic_valid,
  uint8_t bsic
)
{
  qcril_qmi_nas_init_gsm_cell_identity_v6(
    cell,
    plmn,
    lac_valid,
    lac,
    cid_valid,
    cid
  );
  if (cell)
  {
    cell->arfcn = INT_MAX;
    cell->bsic  = 0xff;

    if (arfcn_valid)
    {
        cell->arfcn = arfcn;
    }

    if (bsic_valid)
    {
        cell->bsic = bsic;
    }
  }
}

void qcril_qmi_nas_init_gsm_cell_ss_v12
(
  RIL_CELL_INFO_LIST_GSM_SS_TYPE *cell,
  int radio_if,
  uint8_t  rx_lev_valid,
  uint16_t rx_lev,
  uint8_t  ber_valid,
  int      bitErrorRate,
  uint8_t  ta_valid,
  uint32_t ta
)
{
  qcril_qmi_nas_init_gsm_cell_ss_v6(
    cell,
    radio_if,
    rx_lev_valid,
    rx_lev,
    ber_valid,
    bitErrorRate
  );

  cell->timingAdvance = (!ta_valid || ta == 0xffffffff) ? INT_MAX : ta;
}

void qcril_qmi_nas_init_wcdma_cell_identity_v12
(
  RIL_CELL_INFO_LIST_WCDMA_ID_TYPE *cell,
  char *plmn,
  uint8_t  lac_valid,
  uint16_t lac,
  uint8_t  cid_valid,
  uint32_t cid,
  uint8_t  psc_valid,
  uint16_t psc,
  uint8_t  uarfcn_valid,
  uint16_t uarfcn
)
{
  qcril_qmi_nas_init_wcdma_cell_identity_v6(
          cell,
          plmn,
          lac_valid,
          lac,
          cid_valid,
          cid,
          psc_valid,
          psc
  );
  if (cell)
  {
    cell->uarfcn = INT_MAX;

    if (uarfcn_valid)
    {
        cell->uarfcn = uarfcn;
    }
  }
}

void qcril_qmi_nas_init_lte_cell_identity_v12
(
  RIL_CELL_INFO_LIST_LTE_ID_TYPE *cell,
  uint8_t *plmn,
  uint8_t  tac_valid,
  uint16_t tac,
  uint8_t  cid_valid,
  uint32_t cid,
  uint8_t  pci_valid,
  uint16_t pci,
  uint8_t  earfcn_valid,
  uint32_t earfcn
)
{
  qcril_qmi_nas_init_lte_cell_identity_v6(
    cell,
    plmn,
    tac_valid,
    tac,
    cid_valid,
    cid,
    pci_valid,
    pci
  );

  if (cell)
  {
    cell->earfcn = INT_MAX;

    if (earfcn_valid)
    {
        cell->earfcn = earfcn;
    }
  }
}

int qcril_cell_location_to_ril_cell_info_v12(struct qcril_chain_data *data)
{
  uint64_t *timestamp;
  unsigned totcells, ncells;
  int buflen;
  uint32_t i, j;
  int ret = FALSE;

  RIL_CELL_INFO_LIST_TYPE *cellinfobuf = NULL, *curr_cel, *srv_cell = NULL;
  nas_get_cell_location_info_resp_msg_v01 *li;
  QCRIL_LOG_DEBUG(
          "data: %x, data->parm1: %x,"
          " data->input: %x",
         data, data->parm1,
         data->input
  );
  if (QCRIL_RIL_VERSION >= 12 && data)
  {
    timestamp = (uint64_t *) data->parm1;
    li = (nas_get_cell_location_info_resp_msg_v01 *)data->input;
    do
    {
      if (!li || !timestamp || data->output)
      {
        break;
      }
      QCRIL_LOG_DEBUG("Running v12 conversion");
      totcells = ncells = qcril_qmi_nas_count_cell_location_info_cells(li);
      if (!ncells)
      {
        break;
      }
      buflen = ncells * sizeof(RIL_CELL_INFO_LIST_TYPE);
      cellinfobuf = qcril_malloc(buflen);
      QCRIL_LOG_DEBUG("CellInfo slots allocated: %d. Buflen: %d, buf: %x", ncells, buflen, cellinfobuf);
      curr_cel = cellinfobuf;
      if (cellinfobuf)
      {
        if ( li->geran_info_valid )
        {
          qcril_qmi_nas_init_gsm_cell_v6(
                  (RIL_CELL_INFO_LIST_TYPE *)curr_cel,
                  TRUE,
                  timestamp
          );
          qcril_qmi_nas_init_gsm_cell_identity_v12(
                  &curr_cel->CellInfo.gsm.cellIdentityGsm,
                  li->geran_info.plmn,
                  TRUE,
                  li->geran_info.lac,
                  TRUE,
                  li->geran_info.cell_id,
                  TRUE,
                  li->geran_info.arfcn,
                  TRUE,
                  li->geran_info.bsic
          );
          qcril_qmi_nas_init_gsm_cell_ss_v12(
                  &curr_cel->CellInfo.gsm.signalStrengthGsm,
                  NAS_RADIO_IF_GSM,
                  TRUE,
                  li->geran_info.rx_lev,
                  FALSE,
                  99,
                  TRUE,
                  li->geran_info.timing_advance
          );
          ++curr_cel;

          for (i = 0; i < li->geran_info.nmr_cell_info_len &&
                      (unsigned)(curr_cel - cellinfobuf) < ncells; i++)
          {
            qcril_qmi_nas_init_gsm_cell_v6(
                    (RIL_CELL_INFO_LIST_TYPE *)curr_cel,
                    FALSE,
                    timestamp
            );
            qcril_qmi_nas_init_gsm_cell_identity_v12(
                    &curr_cel->CellInfo.gsm.cellIdentityGsm,
                    li->geran_info.nmr_cell_info[i].nmr_plmn,
                    TRUE,
                    li->geran_info.nmr_cell_info[i].nmr_lac,
                    TRUE,
                    li->geran_info.nmr_cell_info[i].nmr_cell_id,
                    TRUE,
                    li->geran_info.nmr_cell_info[i].nmr_arfcn,
                    TRUE,
                    li->geran_info.nmr_cell_info[i].nmr_bsic
            );
            qcril_qmi_nas_init_gsm_cell_ss_v12(
                    &curr_cel->CellInfo.gsm.signalStrengthGsm,
                    NAS_RADIO_IF_GSM,
                    TRUE,
                    li->geran_info.nmr_cell_info[i].nmr_rx_lev,
                    FALSE,
                    99,
                    FALSE,
                    0xffffffff
            );
            ++curr_cel;
          }
        }

        if (li->cdma_info_valid)
        {
          RIL_SignalStrength ss;

          qcril_qmi_nas_initialize_cdma_ril_cellinfo(curr_cel,
                                                     TRUE,
                                                     RIL_TIMESTAMP_TYPE_OEM_RIL,
                                                     timestamp);
          curr_cel->CellInfo.cdma.cellIdentityCdma.networkId = li->cdma_info.nid;
          curr_cel->CellInfo.cdma.cellIdentityCdma.systemId = li->cdma_info.sid;
          curr_cel->CellInfo.cdma.cellIdentityCdma.basestationId = li->cdma_info.base_id;
          curr_cel->CellInfo.cdma.cellIdentityCdma.longitude = li->cdma_info.base_long;
          curr_cel->CellInfo.cdma.cellIdentityCdma.latitude = li->cdma_info.base_lat;

          // QMI_NAS_GET_CELL_LOCATION_INFO doesn't have SS information for CDMA.
          // Get from cache
          if (qcril_qmi_nas_signal_strength_con_conv_cache2ril(&ss) == E_SUCCESS)
          {
            if (ss.CDMA_SignalStrength.dbm != NAS_SIGNAL_STRENGTH_UNK)
            {
              curr_cel->CellInfo.cdma.signalStrengthCdma.dbm = ss.CDMA_SignalStrength.dbm;
            }
            if (ss.CDMA_SignalStrength.ecio != NAS_SIGNAL_STRENGTH_UNK)
            {
              curr_cel->CellInfo.cdma.signalStrengthCdma.ecio = ss.CDMA_SignalStrength.ecio;
            }

            if (ss.EVDO_SignalStrength.dbm != NAS_SIGNAL_STRENGTH_UNK)
            {
              curr_cel->CellInfo.cdma.signalStrengthEvdo.dbm = ss.EVDO_SignalStrength.dbm;
            }
            if (ss.EVDO_SignalStrength.ecio != NAS_SIGNAL_STRENGTH_UNK)
            {
              curr_cel->CellInfo.cdma.signalStrengthEvdo.ecio = ss.EVDO_SignalStrength.ecio;
            }
            if (ss.EVDO_SignalStrength.signalNoiseRatio != NAS_SIGNAL_STRENGTH_UNK)
            {
              curr_cel->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio = ss.EVDO_SignalStrength.signalNoiseRatio;
            }
          }
          else
          {
            QCRIL_LOG_INFO("qcril_qmi_nas_signal_strength_con_conv_cache2ril returns error."
                           " No SS available for cdma/evdo");
          }

          curr_cel ++;
        }

        if (li->umts_info_valid)
        {
          qcril_qmi_nas_init_wcdma_cell_v6(
                  curr_cel,
                  TRUE,
                  timestamp
          );
          qcril_qmi_nas_init_wcdma_cell_identity_v12(
                  &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
                  li->umts_info.plmn,
                  TRUE,
                  li->umts_info.lac,
                  li->umts_cell_id_valid,
                  li->umts_cell_id,
                  TRUE,
                  li->umts_info.psc,
                  TRUE,
                  li->umts_info.uarfcn
          );
          qcril_qmi_nas_init_wcdma_cell_ss_v6(
                  &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
                  NAS_RADIO_IF_UMTS,
                  TRUE,
                  li->umts_info.rscp,
                  FALSE,
                  99
          );
          curr_cel ++;

          // UMTS Neighbors
          for (i = 0; i < li->umts_info.umts_monitored_cell_len && (unsigned)(curr_cel - cellinfobuf) < ncells; i++)
          {
            qcril_qmi_nas_init_wcdma_cell_v6(
                    curr_cel,
                    FALSE,
                    timestamp
            );
            qcril_qmi_nas_init_wcdma_cell_identity_v12(
                    &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
                    NULL,
                    FALSE,
                    0xffff,
                    FALSE,
                    0xffff,
                    TRUE,
                    li->umts_info.umts_monitored_cell[i].umts_psc,
                    TRUE,
                    li->umts_info.umts_monitored_cell[i].umts_uarfcn
            );
            qcril_qmi_nas_init_wcdma_cell_ss_v6(
                    &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
                    NAS_RADIO_IF_UMTS,
                    TRUE,
                    li->umts_info.umts_monitored_cell[i].umts_rscp,
                    FALSE,
                    99
            );

            curr_cel++;
          }

          // GERAN Neighbors
          for (i = 0; i < li->umts_info.umts_geran_nbr_cell_len && (unsigned)(curr_cel - cellinfobuf) < ncells; i++)
          {
            qcril_qmi_nas_init_gsm_cell_v6(
                    curr_cel,
                    FALSE,
                    timestamp
            );
            qcril_qmi_nas_init_gsm_cell_identity_v12(
                    &curr_cel->CellInfo.gsm.cellIdentityGsm,
                    NULL,
                    FALSE,
                    0xffff,
                    FALSE,
                    0xffffffff,
                    TRUE,
                    li->umts_info.umts_geran_nbr_cell[i].geran_arfcn,
                    TRUE,
                    QCRIL_MAKE_BSIC(
                            li->umts_info.umts_geran_nbr_cell[i].geran_bsic_ncc,
                            li->umts_info.umts_geran_nbr_cell[i].geran_bsic_bcc
                    )
            );
            qcril_qmi_nas_init_gsm_cell_ss_v12(
                    &curr_cel->CellInfo.gsm.signalStrengthGsm,
                    NAS_RADIO_IF_UMTS,
                    TRUE,
                    li->umts_info.umts_geran_nbr_cell[i].geran_rssi,
                    FALSE,
                    99,
                    FALSE,
                    0xffffffff
            );
            curr_cel++;
          }

          if (i < li->umts_info.umts_geran_nbr_cell_len)
          {
            QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for Geran Neighbors. "
                            "Slots: %d. Need: %d more",
                            ncells,
                            li->umts_info.umts_geran_nbr_cell_len - i);
          }
        }

        if ( li->lte_intra_valid )
        {
          uint8_t srv_done = FALSE;

          srv_cell = curr_cel;
          curr_cel++;

          qcril_qmi_nas_init_lte_cell_v6(
            srv_cell,
            TRUE,
            timestamp
          );

          if(li->lte_intra_earfcn_valid)
          {
            qcril_qmi_nas_init_lte_cell_identity_v12(
              &srv_cell->CellInfo.lte.cellIdentityLte,
              li->lte_intra.plmn,
              TRUE,
              li->lte_intra.tac,
              TRUE,
              li->lte_intra.global_cell_id,
              TRUE,
              li->lte_intra.serving_cell_id,
              TRUE,
              li->lte_intra_earfcn
            );
          }
          else
          {
            qcril_qmi_nas_init_lte_cell_identity_v12(
              &srv_cell->CellInfo.lte.cellIdentityLte,
              li->lte_intra.plmn,
              TRUE,
              li->lte_intra.tac,
              TRUE,
              li->lte_intra.global_cell_id,
              TRUE,
              li->lte_intra.serving_cell_id,
              TRUE,
              li->lte_intra.earfcn
            );
          }

          qcril_qmi_nas_init_lte_cell_ss_v8(
            &srv_cell->CellInfo.lte.signalStrengthLte,
            NAS_RADIO_IF_LTE,
            FALSE,
            99,
            FALSE,
            0xffff,
            FALSE,
            0xffff,
            FALSE,
            INT_MAX,
            FALSE,
            INT_MAX,
            li->timing_advance_valid,
            li->timing_advance
          );

          // Neighbors
          for (i = 0; i < li->lte_intra.cells_len && ((unsigned)(curr_cel - cellinfobuf) < ncells || !srv_done); i++)
          {
            if (li->lte_intra.cells[i].pci == li->lte_intra.serving_cell_id)
            {
              /* This is for the Serving Cell */
              qcril_qmi_nas_init_lte_cell_ss_v8(
                      &srv_cell->CellInfo.lte.signalStrengthLte,
                      NAS_RADIO_IF_LTE,
                      TRUE,
                      li->lte_intra.cells[i].rssi,
                      TRUE,
                      li->lte_intra.cells[i].rsrp,
                      TRUE,
                      li->lte_intra.cells[i].rsrq,
                      FALSE,
                      INT_MAX,
                      FALSE,
                      INT_MAX,
                      li->timing_advance_valid,
                      li->timing_advance
              );

              QCRIL_LOG_DEBUG("Serving cell rsrp: %d. rsrq: %d. rssi: %d. srxlev: %d. output signal Strength: %d ta: %d",
                      li->lte_intra.cells[i].rsrp,
                      li->lte_intra.cells[i].rsrq,
                      li->lte_intra.cells[i].rssi,
                      li->lte_intra.cells[i].srxlev,
                      srv_cell->CellInfo.lte.signalStrengthLte.signalStrength,
                      li->timing_advance);

              QCRIL_LOG_DEBUG("Output rsrp: %d, rsrq: %d, rssnr: %d, cqi: %d, ta: %d",
                      srv_cell->CellInfo.lte.signalStrengthLte.rsrp,
                      srv_cell->CellInfo.lte.signalStrengthLte.rsrq,
                      srv_cell->CellInfo.lte.signalStrengthLte.rssnr,
                      srv_cell->CellInfo.lte.signalStrengthLte.cqi,
                      srv_cell->CellInfo.lte.signalStrengthLte.timingAdvance);
              qcril_qmi_print_hex( (uint8_t *)&srv_cell->CellInfo.lte.signalStrengthLte, sizeof(srv_cell->CellInfo.lte.signalStrengthLte));
              srv_done = TRUE;
            }
            else if ( (unsigned)(curr_cel - cellinfobuf) >= ncells && !srv_done)
            {
                continue;
            }
            else
            {
              /* This is for a neighbor Cell */
              qcril_qmi_nas_init_lte_cell_v6(
                      curr_cel,
                      FALSE,
                      timestamp
              );

              if(li->lte_intra_earfcn_valid)
              {
                qcril_qmi_nas_init_lte_cell_identity_v12(
                      &curr_cel->CellInfo.lte.cellIdentityLte,
                      NULL,
                      FALSE,
                      0xffff,
                      FALSE,
                      INT_MAX,
                      TRUE,
                      li->lte_intra.cells[i].pci,
                      TRUE,
                      li->lte_intra_earfcn
                );
              }
              else
              {
                qcril_qmi_nas_init_lte_cell_identity_v12(
                      &curr_cel->CellInfo.lte.cellIdentityLte,
                      NULL,
                      FALSE,
                      0xffff,
                      FALSE,
                      INT_MAX,
                      TRUE,
                      li->lte_intra.cells[i].pci,
                      TRUE,
                      li->lte_intra.earfcn
                );
              }

              qcril_qmi_nas_init_lte_cell_ss_v8(
                      &curr_cel->CellInfo.lte.signalStrengthLte,
                      NAS_RADIO_IF_LTE,
                      TRUE,
                      li->lte_intra.cells[i].rssi,
                      TRUE,
                      li->lte_intra.cells[i].rsrp,
                      TRUE,
                      li->lte_intra.cells[i].rsrq,
                      FALSE,
                      INT_MAX,
                      FALSE,
                      INT_MAX,
                      FALSE,
                      INT_MAX
              );

              QCRIL_LOG_DEBUG("Neighbor cell rsrp: %d. rssi: %d. srxlev: %d. output signal Strength: %d",
                      -(li->lte_intra.cells[i].rsrp / 10),
                      li->lte_intra.cells[i].rssi,
                      li->lte_intra.cells[i].srxlev,
                      curr_cel->CellInfo.lte.signalStrengthLte.signalStrength);

              curr_cel++;
            }
          }

          if (i < li->lte_intra.cells_len)
          {
            QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - Intra. "
                            "Slots: %d. Want %d more",
                            ncells,
                            li->lte_intra.cells_len - i);
          }
        }

        if ( li->lte_inter_valid )
        {
          for (i = 0; i < li->lte_inter.freqs_len; i++)
          {
            for (j = 0; j < li->lte_inter.freqs[i].cells_len && (unsigned)(curr_cel - cellinfobuf) < ncells; j++)
            {
              qcril_qmi_nas_init_lte_cell_v6(
                      curr_cel,
                      FALSE,
                      timestamp
              );
              qcril_qmi_nas_init_lte_cell_identity_v12(
                      &curr_cel->CellInfo.lte.cellIdentityLte,
                      NULL,
                      FALSE,
                      0xffff,
                      FALSE,
                      INT_MAX,
                      TRUE,
                      li->lte_inter.freqs[i].cells[j].pci,
                      TRUE,
                      li->lte_inter.freqs[i].earfcn
              );
              qcril_qmi_nas_init_lte_cell_ss_v8(
                      &curr_cel->CellInfo.lte.signalStrengthLte,
                      NAS_RADIO_IF_LTE,
                      TRUE,
                      li->lte_inter.freqs[i].cells[j].rssi,
                      TRUE,
                      li->lte_inter.freqs[i].cells[j].rsrp,
                      TRUE,
                      li->lte_inter.freqs[i].cells[j].rsrq,
                      FALSE,
                      INT_MAX,
                      FALSE,
                      INT_MAX,
                      FALSE,
                      INT_MAX
              );

              QCRIL_LOG_DEBUG("Neighbor cell (inter) rsrp: %d. rssi: %d. srxlev: %d. output signal Strength: %d",
                      -(li->lte_intra.cells[i].rsrp / 10),
                      li->lte_intra.cells[i].rssi,
                      li->lte_intra.cells[i].srxlev,
                      curr_cel->CellInfo.lte.signalStrengthLte.signalStrength);

              curr_cel++;
            }
            if ( j < li->lte_inter.freqs[i].cells_len )
            {
              QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - Inter, freq %d. "
                              "Slots: %d. Want %d more",
                              i,
                              ncells,
                              li->lte_inter.freqs[i].cells_len - j);
            }
          }
        }

        if ( li->lte_gsm_valid )
        {
          for (i = 0; i < li->lte_gsm.freqs_len; i++)
          {
            for (j = 0; j < li->lte_gsm.freqs[i].cells_len && (unsigned)(curr_cel - cellinfobuf) < ncells; j++)
            {
              qcril_qmi_nas_init_gsm_cell_v6(
                      curr_cel,
                      FALSE,
                      timestamp
              );
              qcril_qmi_nas_init_gsm_cell_identity_v12(
                      &curr_cel->CellInfo.gsm.cellIdentityGsm,
                      NULL,
                      FALSE,
                      0xffff,
                      FALSE,
                      0xffffffff,
                      TRUE,
                      li->lte_gsm.freqs[i].cells[j].arfcn,
                      li->lte_gsm.freqs[i].cells[j].cell_id_valid,
                      li->lte_gsm.freqs[i].cells[j].bsic_id
              );
              qcril_qmi_nas_init_gsm_cell_ss_v12(
                      &curr_cel->CellInfo.gsm.signalStrengthGsm,
                      NAS_RADIO_IF_LTE,
                      TRUE,
                      li->lte_gsm.freqs[i].cells[j].rssi,
                      FALSE,
                      99,
                      FALSE,
                      0xffffffff
              );

              curr_cel++;
            }
            if ( j < li->lte_gsm.freqs[i].cells_len )
            {
              QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - GSM Neighbors, freq %d. "
                              "Slots: %d. Want %d more",
                              i,
                              ncells,
                              li->lte_gsm.freqs[i].cells_len - j);
            }
          }
        }

        if ( li->lte_wcdma_valid )
        {
          for (i = 0; i < li->lte_wcdma.freqs_len; i++)
          {
            for (j = 0; j < li->lte_wcdma.freqs[i].cells_len && (unsigned)(curr_cel - cellinfobuf) < ncells; j++)
            {
              qcril_qmi_nas_init_wcdma_cell_v6(
                      (RIL_CELL_INFO_LIST_TYPE *)curr_cel,
                      FALSE,
                      timestamp
              );
              qcril_qmi_nas_init_wcdma_cell_identity_v12(
                      &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
                      NULL,
                      FALSE,
                      0xffff,
                      FALSE,
                      0xffff,
                      TRUE,
                      li->lte_wcdma.freqs[i].cells[j].psc,
                      TRUE,
                      li->lte_wcdma.freqs[i].uarfcn
              );
              qcril_qmi_nas_init_wcdma_cell_ss_v6(
                      &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
                      NAS_RADIO_IF_LTE,
                      TRUE,
                      li->lte_wcdma.freqs[i].cells[j].cpich_rscp,
                      FALSE,
                      99
              );

              curr_cel++;
            }
            if ( j < li->lte_wcdma.freqs[i].cells_len )
            {
              QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - WCDMA Neighbors, freq %d. "
                              "Slots: %d. Want %d more",
                              i,
                              ncells,
                              li->lte_wcdma.freqs[i].cells_len - j);
            }
          }
        }
      }
      data->output = cellinfobuf;

      data->ret1 = (void *)((unsigned)(curr_cel - cellinfobuf) * sizeof(RIL_CELL_INFO_LIST_TYPE));
      data->ret2 = (void *)(unsigned long)(curr_cel - cellinfobuf);

      ret = TRUE;
    }while(0);
  }
  return ret;
}

