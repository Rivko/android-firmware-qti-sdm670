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
#include "qcril_cm_util.h"
#include <telephony/ril.h>

struct qcril_cell_info_chain
{
  char *name;
  unsigned prio;
  int (*process)(struct qcril_chain_data *fd);
  struct qcril_cell_info_chain *next;
};

int qcril_cell_location_to_ril_cell_info_v6(struct qcril_chain_data *data);

static struct qcril_cell_info_chain v6 =
{
  .name    = "CELL_LOCATION_INFO -> RIL_CellInfo v6",
  .prio    = 1,
  .process = qcril_cell_location_to_ril_cell_info_v6,
  .next    = NULL
};

static struct qcril_cell_info_chain v12 =
{
  .name    = "CELL_LOCATION_INFO -> RIL_CellInfo_v12",
  .prio    = 0,
#if QCRIL_RIL_VERSION < 12
  .process = NULL,
#else
  .process = qcril_cell_location_to_ril_cell_info_v12,
#endif
  .next    = &v6
};

static struct qcril_cell_info_chain *chain_start = &v12;

int qcril_cell_info_process(
  nas_get_cell_location_info_resp_msg_v01 *rsp,
  uint64_t *timestamp,
  void **ril_response,
  unsigned *resp_len,
  unsigned *ncells
)
{
  struct qcril_cell_info_chain *c;
  struct qcril_chain_data cd = {
      .parm1  = timestamp,
      .input  = rsp,
      .output = NULL,
      .ret1   = NULL,
      .ret2   = NULL};
  int success = FALSE;


  if (ril_response && resp_len && ncells)
  {
    QCRIL_LOG_DEBUG("rsp: %x, ts: %x, resp: %x, len: %x",
            rsp,
            timestamp,
            ril_response,
            resp_len
    );
    for (c = chain_start; c != NULL; c = c->next)
    {
      if(c->process)
      {
        if (c->process(&cd))
        {
          QCRIL_LOG_DEBUG(">>%s<< Successful", c->name);
          success = TRUE;
        }
      }
    }
    if (success)
    {
      *ril_response = cd.output;
      *resp_len = (unsigned) cd.ret1;
      *ncells   = (unsigned) cd.ret2;
    }
  }
  return success;
}

//===========================================================================
// qcril_qmi_nas_is_cell_info_equal_member
//===========================================================================
uint8_t qcril_qmi_nas_is_cell_info_equal_member( void *first, void *second, size_t size)
{
  return memcmp(first, second, size) == 0;
}

//===========================================================================
// qcril_qmi_nas_is_cell_info_equal
//===========================================================================
uint8_t qcril_qmi_nas_is_cell_info_equal(RIL_CELL_INFO_LIST_TYPE *first, RIL_CELL_INFO_LIST_TYPE *second)
{
  uint8_t ret = FALSE;
  uint8_t invalid_ptrs1 = FALSE;
  uint8_t invalid_ptrs2 = FALSE;
  uint8_t type = FALSE;
  uint8_t registered = FALSE;
  uint8_t tst = FALSE;
  uint8_t eql_mbr = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  ret = ( ( invalid_ptrs1 = ((!first == !second) && !first))) ||
        ( ( invalid_ptrs2 = ((!first == !second) && first )) &&
          ( type = (first->cellInfoType == second->cellInfoType ) ) &&
          ( registered = (first->registered == second->registered ) )&&
          ( tst = (first->timeStampType == second->timeStampType) ) &&
          ( eql_mbr = qcril_qmi_nas_is_cell_info_equal_member(&first->CellInfo, &second->CellInfo, sizeof(first->CellInfo))));

  QCRIL_LOG_DEBUG("first: %x, second: %x, invl_ptrs: %u, invl_ptr2: %u, type: %u, registered: %u, tst: %u, eql_mbr: %u",
                  first, second, (unsigned)invalid_ptrs1, (unsigned)invalid_ptrs2, (unsigned)type, (unsigned)registered, (unsigned)tst, (unsigned)eql_mbr);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
} // qcril_qmi_nas_is_cell_info_equal

//===========================================================================
// qcril_qmi_nas_count_cell_location_info_cells
//===========================================================================
unsigned qcril_qmi_nas_count_cell_location_info_cells
(
  nas_get_cell_location_info_resp_msg_v01 *cell_location_info
)
{
    nas_get_cell_location_info_resp_msg_v01 *li = cell_location_info;
    unsigned ncells = 0;
    unsigned ratcells = 0;
    unsigned i = 0;

    if (li)
    {
      // GERAN
      if (li->geran_info_valid)
      {
        ratcells = 0;
        if (li->geran_info.cell_id != NAS_CELL_ID_NOT_PRESENT)
        {
          /* If cell id is present (not NAS_CELL_ID_NOT_PRESENT) we have a serving cell on gsm */
          ratcells ++;
        }
        else
        {
          QCRIL_LOG_DEBUG("GERAN Cell id invalid. Neighbor len: %d", li->geran_info.nmr_cell_info_len);
        }
        ratcells += li->geran_info.nmr_cell_info_len;
        QCRIL_LOG_DEBUG("GERAN Cells found %d", ratcells);
        ncells += ratcells;
      }

      // UMTS
      if (li->umts_info_valid)
      {
        ratcells = 0;
        if (li->umts_info.cell_id != NAS_CELL_ID_NOT_PRESENT)
        {
          ratcells ++;
        }
        else
        {
          QCRIL_LOG_DEBUG("UMTS Cell id invalid. Monitored len: %d. Neighbor len: %d",
                           li->umts_info.umts_monitored_cell_len,
                           li->umts_info.umts_geran_nbr_cell_len);
        }
        ratcells += li->umts_info.umts_monitored_cell_len;
        QCRIL_LOG_DEBUG("UMTS Cells found: %d", ratcells);
        ncells += ratcells;
        ratcells = 0;
        /* Do we need to account for duplicates if multiple rats are available? */
        ratcells += li->umts_info.umts_geran_nbr_cell_len;
        QCRIL_LOG_DEBUG("UMTS - GERAN neighbor Cells found: %d", ratcells);
        ncells += ratcells;
      }

      // CDMA
      if (li->cdma_info_valid)
      {
        ncells ++;
        QCRIL_LOG_DEBUG("CDMA Cell found");
      }

      // LTE Intra Frequency
      if (li->lte_intra_valid)
      {
        ratcells = 1; // Serving cell information
        for( i = 0; i < li->lte_intra.cells_len; i++ )
        {
            if( li->lte_intra.cells[i].pci != li->lte_intra.serving_cell_id )
            {
                ratcells += 1; // Neighbouring Cell information
            }
        }

        QCRIL_LOG_DEBUG("LTE Intra-freq Cells found %d", ratcells);
        ncells += ratcells;
      }

      // LTE - Inter Frequency Neighbors
      if (li->lte_inter_valid)
      {
        ratcells = 0;
        for (i = 0; i < li->lte_inter.freqs_len; i++)
        {
          ratcells += li->lte_inter.freqs[i].cells_len;
        }
        QCRIL_LOG_DEBUG("LTE Inter-freq cells found: %d", ratcells);
        ncells += ratcells;
      }

      // LTE - GSM Neighbors
      if (li->lte_gsm_valid)
      {
        ratcells = 0;
        for (i = 0; i < li->lte_gsm.freqs_len; i++)
        {
          ratcells += li->lte_gsm.freqs[i].cells_len;
        }

        QCRIL_LOG_DEBUG("LTE - GSM Neighbor Cells found: %d", ratcells);
        ncells += ratcells;
      }

      // LTE - WCDMA Neighbors
      if (li->lte_wcdma_valid)
      {
        ratcells = 0;
        for (i = 0; i < li->lte_wcdma.freqs_len; i++)
        {
          ratcells += li->lte_wcdma.freqs[i].cells_len;
        }

        QCRIL_LOG_DEBUG("LTE - WCDMA Neighbor Cells found: %d", ratcells);
        ncells += ratcells;
      }

      if (li->umts_cell_id_valid)
      {
        QCRIL_LOG_DEBUG("Umts Cell Id valid");
      }

      if (li->wcdma_lte_valid)
      {
        ratcells = 0;
        ratcells += li->wcdma_lte.umts_lte_nbr_cell_len;
        QCRIL_LOG_DEBUG("WCDMA - LTE Neighbor Cells found: %d", ratcells);
        ncells += ratcells;
      }

      QCRIL_LOG_DEBUG("Total Cells found: %d", ncells);
    }

    return ncells;
} // qcril_qmi_nas_count_cell_location_info_cells

void qcril_qmi_nas_init_gsm_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
)
{
  if (cell)
  {
    cell->cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    cell->registered = FALSE;
    if (is_srv)
    {
      cell->registered = qcril_qmi_nas_is_rte_registered(QMI_RIL_RTE_GSM);
    }
    cell->timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    cell->timeStamp = *timestamp;
  }
}

void qcril_qmi_nas_init_gsm_cell_identity_v6
(
  RIL_CELL_INFO_LIST_GSM_ID_TYPE *cell,
  char * plmn,
  uint8_t  lac_valid,
  uint16_t lac,
  uint8_t  cid_valid,
  uint32_t cid
)
{
  if (cell)
  {
    cell->mcc = INT_MAX;
    cell->mnc = INT_MAX;
    cell->lac  = INT_MAX;
    cell->cid = INT_MAX;


    /* According to the QMI spec, lac and plmn
       should be ignored if cell_id is 0xffffffff */
    if (cid_valid && cid != 0xffffffff)
    {
        cell->cid = cid;
        if (lac_valid)
        {
            cell->lac = lac;
        }
        if (plmn != NULL)
        {
          cell->mcc = QCRIL_MCC_FROM_BCD_PLMN(plmn);
          cell->mnc = QCRIL_MNC_FROM_BCD_PLMN(plmn);
        }
    }
  }
}

void qcril_qmi_nas_init_gsm_cell_ss_v6
(
  RIL_CELL_INFO_LIST_GSM_SS_TYPE *cell,
  int radio_if,
  uint8_t ss_valid,
  int16_t ss,
  uint8_t ber_valid,
  int     bitErrorRate
)
{
  if (cell)
  {
    cell->signalStrength = 99;
    cell->bitErrorRate   = 99;
    if (ss_valid)
    {
        switch (radio_if)
        {
            case NAS_RADIO_IF_GSM:
                cell->signalStrength =
                    qcril_qmi_nas_util_calculate_neighbor_cell_rssi_no_cache(
                            ss, radio_if
                    );
                break;
            case NAS_RADIO_IF_UMTS:
                qcril_cm_util_rssi_to_gw_signal_strength(-ss, &cell->signalStrength);
                break;
            case NAS_RADIO_IF_LTE:
                qcril_cm_util_rssi_to_gw_signal_strength(-ss/10, &cell->signalStrength);
                break;
        }
    }

    if (ber_valid)
    {
        cell->bitErrorRate = bitErrorRate;
    }
  }
}

//===========================================================================
// qcril_qmi_nas_convert_geran_cells_to_ril_cellinfo
//===========================================================================
unsigned qcril_qmi_nas_convert_geran_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
)
{
  nas_get_cell_location_info_resp_msg_v01 *li = nas_location_info;
  RIL_CELL_INFO_LIST_TYPE *curr_cel = NULL;
  unsigned i;
  unsigned ret = 0;

  if (li && li->geran_info_valid && ril_info && *ril_info && timestamp && ril_info_items > 0)
  {
    curr_cel = *ril_info;
    if ( li->geran_info.cell_id != NAS_CELL_ID_NOT_PRESENT )
    {
      qcril_qmi_nas_init_gsm_cell_v6(
              curr_cel,
              TRUE,
              timestamp
      );
      qcril_qmi_nas_init_gsm_cell_identity_v6(
              &curr_cel->CellInfo.gsm.cellIdentityGsm,
              li->geran_info.plmn,
              TRUE,
              li->geran_info.lac,
              TRUE,
              li->geran_info.cell_id
      );
      qcril_qmi_nas_init_gsm_cell_ss_v6(
              &curr_cel->CellInfo.gsm.signalStrengthGsm,
              NAS_RADIO_IF_GSM,
              TRUE,
              li->geran_info.rx_lev,
              FALSE,
              99
      );
      QCRIL_LOG_DEBUG("PLMN From Geran:");
      qcril_qmi_print_hex((unsigned char*)li->geran_info.plmn, NAS_PLMN_LEN_V01);
      ++(*ril_info);
      curr_cel = *ril_info;
      ret ++;
    }

    for (i = 0; i < li->geran_info.nmr_cell_info_len && ret < ril_info_items; i++)
    {
      qcril_qmi_nas_init_gsm_cell_v6(
              curr_cel,
              FALSE,
              timestamp
      );
      qcril_qmi_nas_init_gsm_cell_identity_v6(
              &curr_cel->CellInfo.gsm.cellIdentityGsm,
              li->geran_info.nmr_cell_info[i].nmr_plmn,
              TRUE,
              li->geran_info.nmr_cell_info[i].nmr_lac,
              TRUE,
              li->geran_info.nmr_cell_info[i].nmr_cell_id
      );
      qcril_qmi_nas_init_gsm_cell_ss_v6(
              &curr_cel->CellInfo.gsm.signalStrengthGsm,
              NAS_RADIO_IF_GSM,
              TRUE,
              li->geran_info.nmr_cell_info[i].nmr_rx_lev,
              FALSE,
              99
      );
      ++(*ril_info);
      curr_cel = *ril_info;
      ret ++;
    }
    if (i < li->geran_info.nmr_cell_info_len)
    {
      QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for GERAN. "
                      "Slots: %d. Want %d more",
                      ril_info_items,
                      li->geran_info.nmr_cell_info_len - i);
    }
  }
  return ret;
} // qcril_qmi_nas_convert_geran_cells_to_ril_cellinfo

void qcril_qmi_nas_init_wcdma_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
)
{
  if (cell)
  {
    cell->cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    cell->registered = FALSE;
    if (is_srv)
    {
      cell->registered = qcril_qmi_nas_is_rte_registered(QMI_RIL_RTE_WCDMA);
    }
    cell->timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    cell->timeStamp = *timestamp;
  }
}

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
)
{
  if (cell)
  {
    cell->mcc = INT_MAX;
    cell->mnc = INT_MAX;
    cell->lac = INT_MAX;
    cell->cid = INT_MAX;
    cell->psc = INT_MAX;

    if (plmn != NULL)
    {
      cell->mcc = QCRIL_MCC_FROM_BCD_PLMN(plmn);
      cell->mnc = QCRIL_MNC_FROM_BCD_PLMN(plmn);
    }
    if (lac_valid)
    {
        cell->lac = lac;
    }

    if (cid_valid)
    {
        cell->cid = cid;
    }

    if (psc_valid)
    {
        cell->psc = psc;
    }
  }
}

void qcril_qmi_nas_init_wcdma_cell_ss_v6
(
  RIL_SignalStrengthWcdma *cell,
  int      radio_if,
  uint8_t  ss_valid,
  int16_t  ss,
  uint8_t  ber_valid,
  int      bitErrorRate
)
{
  if (cell)
  {
    cell->signalStrength = 99;
    cell->bitErrorRate   = 99;
    if (ss_valid)
    {
        switch(radio_if)
        {
          case NAS_RADIO_IF_UMTS:
            qcril_cm_util_rssi_to_gw_signal_strength( -ss, &cell->signalStrength);
            break;
          case NAS_RADIO_IF_LTE:
            qcril_cm_util_rssi_to_gw_signal_strength( -ss / 10, &cell->signalStrength);
            break;
        }
    }

    if (ber_valid)
    {
        cell->bitErrorRate = bitErrorRate;
    }
  }
}

//===========================================================================
// qcril_qmi_nas_convert_umts_cells_to_ril_cellinfo
//===========================================================================
unsigned qcril_qmi_nas_convert_umts_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
)
{
  nas_get_cell_location_info_resp_msg_v01 *li = nas_location_info;
  RIL_CELL_INFO_LIST_TYPE *curr_cel = NULL;
  unsigned ret = 0;
  unsigned i;

  if (li && li->umts_info_valid && li->umts_cell_id_valid && ril_info && *ril_info && timestamp &&
      ril_info_items > 0)
  {
    curr_cel = *ril_info;
    if ( li->umts_info.cell_id != NAS_CELL_ID_NOT_PRESENT )
    {
      qcril_qmi_nas_init_wcdma_cell_v6(
              curr_cel,
              TRUE,
              timestamp
      );
      qcril_qmi_nas_init_wcdma_cell_identity_v6(
              &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
              li->umts_info.plmn,
              TRUE,
              li->umts_info.lac,
              TRUE,
              li->umts_cell_id,
              TRUE,
              li->umts_info.psc
      );
      qcril_qmi_nas_init_wcdma_cell_ss_v6(
              &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
              NAS_RADIO_IF_UMTS,
              TRUE,
              li->umts_info.rscp,
              FALSE,
              99
      );
    }

    // UMTS Neighbors
    for (i = 0; i < li->umts_info.umts_monitored_cell_len && ret < ril_info_items; i++)
    {
      qcril_qmi_nas_init_wcdma_cell_v6(
              curr_cel,
              FALSE,
              timestamp
      );
      qcril_qmi_nas_init_wcdma_cell_identity_v6(
              &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
              NULL,
              FALSE,
              0xffff,
              FALSE,
              0xffff,
              TRUE,
              li->umts_info.umts_monitored_cell[i].umts_psc
      );
      qcril_qmi_nas_init_wcdma_cell_ss_v6(
              &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
              NAS_RADIO_IF_UMTS,
              TRUE,
              li->umts_info.umts_monitored_cell[i].umts_rscp,
              FALSE,
              99
      );

      curr_cel = ++(*ril_info);
      ret ++;
    }

    if (i < li->umts_info.umts_monitored_cell_len)
    {
      QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for UMTS Monitored Cells. "
                      "Slots: %d. Need: %d more",
                      ril_info_items,
                      li->umts_info.umts_monitored_cell_len - i);
    }

    // GERAN Neighbors
    for (i = 0; i < li->umts_info.umts_geran_nbr_cell_len && ret < ril_info_items; i++)
    {
      qcril_qmi_nas_init_gsm_cell_v6(
              curr_cel,
              FALSE,
              timestamp
      );
      qcril_qmi_nas_init_gsm_cell_identity_v6(
              &curr_cel->CellInfo.gsm.cellIdentityGsm,
              NULL,
              FALSE,
              0xffff,
              FALSE,
              0xffffffff
      );
      qcril_qmi_nas_init_gsm_cell_ss_v6(
              &curr_cel->CellInfo.gsm.signalStrengthGsm,
              NAS_RADIO_IF_UMTS,
              TRUE,
              li->umts_info.umts_geran_nbr_cell[i].geran_rssi,
              FALSE,
              99
      );
      ++(*ril_info);
      curr_cel = (*ril_info);
      ret ++;
    }

    if (i < li->umts_info.umts_geran_nbr_cell_len)
    {
      QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for Geran Neighbors. "
                      "Slots: %d. Need: %d more",
                      ril_info_items,
                      li->umts_info.umts_geran_nbr_cell_len - i);
    }
  }
  return ret;
} // qcril_qmi_nas_convert_umts_cells_to_ril_cellinfo

//===========================================================================
// qcril_qmi_nas_initialize_cdma_ril_cellinfo
//===========================================================================
void qcril_qmi_nas_initialize_cdma_ril_cellinfo
(
  RIL_CELL_INFO_LIST_TYPE *cell_info,
  boolean srv,
  RIL_TimeStampType timestampType,
  uint64_t *timestamp
)
{
  if (cell_info)
  {
      memset(cell_info, 0, sizeof(*cell_info));
      cell_info->cellInfoType = RIL_CELL_INFO_TYPE_CDMA;
      cell_info->registered = FALSE;

      if (srv)
      {
          cell_info->registered = qcril_qmi_nas_is_rte_registered(QMI_RIL_RTE_1x);
      }
      if (timestamp)
      {
          cell_info->timeStamp = *timestamp;
      }
      cell_info->timeStampType = timestampType;

      cell_info->CellInfo.cdma.cellIdentityCdma.networkId = INT_MAX;
      cell_info->CellInfo.cdma.cellIdentityCdma.systemId = INT_MAX;
      cell_info->CellInfo.cdma.cellIdentityCdma.basestationId = INT_MAX;
      cell_info->CellInfo.cdma.cellIdentityCdma.longitude = INT_MAX;
      cell_info->CellInfo.cdma.cellIdentityCdma.latitude = INT_MAX;

      cell_info->CellInfo.cdma.signalStrengthCdma.dbm = INT_MAX;
      cell_info->CellInfo.cdma.signalStrengthCdma.ecio = INT_MAX;

      cell_info->CellInfo.cdma.signalStrengthEvdo.dbm = INT_MAX;
      cell_info->CellInfo.cdma.signalStrengthEvdo.ecio = INT_MAX;
      cell_info->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
  }
} // qcril_qmi_nas_initialize_cdma_ril_cellinfo

//===========================================================================
// qcril_qmi_nas_convert_cdma_cells_to_ril_cellinfo
//===========================================================================
unsigned qcril_qmi_nas_convert_cdma_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
)
{
  nas_get_cell_location_info_resp_msg_v01 *li = nas_location_info;
  RIL_CELL_INFO_LIST_TYPE *curr_cel = NULL;
  RIL_SignalStrength ss;
  unsigned ret = 0;

  if (li && li->cdma_info_valid && ril_info && *ril_info && timestamp && ril_info_items > 0)
  {
    curr_cel = *ril_info;

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

    ++(*ril_info);
    curr_cel = (*ril_info);
    ret ++;
  }
  return ret;
} // qcril_qmi_nas_convert_cdma_cells_to_ril_cellinfo

int qcril_qmi_nas_qmi_ta_to_ril_ta(uint8_t ta_valid, uint32_t ta)
{
  int ret = INT_MAX;

  if ( ta_valid )
  {
    if (ta < UINT32_MAX && ta < INT_MAX)
    {
      ret = (int) ta;
    }
  }
  return ret;
}

void qcril_qmi_nas_init_lte_cell_v6
(
  RIL_CELL_INFO_LIST_TYPE *cell,
  uint8_t is_srv,
  uint64_t *timestamp
)
{
  if (cell)
  {
    cell->cellInfoType = RIL_CELL_INFO_TYPE_LTE;
    cell->registered = FALSE;
    if (is_srv)
    {
      cell->registered = qcril_qmi_nas_is_rte_registered(QMI_RIL_RTE_SUB_LTE);
    }
    cell->timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    cell->timeStamp = *timestamp;
  }
}

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
)
{
  if (cell)
  {

    cell->mcc = INT_MAX;
    cell->mnc = INT_MAX;
    cell->tac = INT_MAX;
    cell->ci = INT_MAX;
    cell->pci = INT_MAX;

    if (plmn != NULL)
    {
      cell->mcc = QCRIL_MCC_FROM_BCD_PLMN(plmn);
      cell->mnc = QCRIL_MNC_FROM_BCD_PLMN(plmn);
    }

    if (tac_valid)
    {
        cell->tac = tac;
    }

    if (cid_valid)
    {
        cell->ci = cid;
    }

    if (pci_valid)
    {
        cell->pci = pci;
    }
  }
}

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
)
{
  if (cell)
  {
    memset(cell, 0, sizeof(*cell));
    cell->signalStrength = INT_MAX;
    cell->rsrp           = INT_MAX;
    cell->rsrq           = INT_MAX;
    cell->rssnr          = INT_MAX;
    cell->cqi            = INT_MAX;

    if (rsrq_valid)
    {
        cell->rsrq = - rsrq / 10;
    }

    if (rsrp_valid)
    {
        cell->rsrp = - rsrp / 10;
    }

    if (ss_valid)
    {
        qcril_cm_util_rssi_to_gw_signal_strength(
                qcril_qmi_nas_util_calculate_neighbor_cell_rssi_no_cache(
                        ss, radio_if),
                &cell->signalStrength
        );
    }

    if (rssnr_valid)
    {
        cell->rssnr = rssnr;
    }

    if (cqi_valid)
    {
        cell->cqi = cqi;
    }
    cell->timingAdvance = qcril_qmi_nas_qmi_ta_to_ril_ta(ta_valid,ta);

  }
}

//===========================================================================
// qcril_qmi_nas_convert_lte_cells_to_ril_cellinfo
//===========================================================================
unsigned qcril_qmi_nas_convert_lte_cells_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  uint64_t *timestamp,
  RIL_CELL_INFO_LIST_TYPE **ril_info,
  unsigned ril_info_items
)
{
  nas_get_cell_location_info_resp_msg_v01 *li = nas_location_info;
  RIL_CELL_INFO_LIST_TYPE *curr_cel = NULL, *srv_cell = NULL;
  unsigned ret =0;
  unsigned i = 0, j;
  int ss_found = FALSE;

  if (li && ril_info && *ril_info && timestamp && ril_info_items > 0)
  {
    curr_cel = *ril_info;

    if ( li->lte_intra_valid )
    {

      srv_cell = curr_cel;
      ++(*ril_info);
      curr_cel = (*ril_info);
      ret ++;

      // Neighbors
      for (i = 0; i < li->lte_intra.cells_len && ril_info_items > 0; i++)
      {
        if (li->lte_intra.cells[i].pci == li->lte_intra.serving_cell_id)
        {
          /* This is for the Serving Cell */
          ss_found = TRUE;
          qcril_qmi_nas_init_lte_cell_v6(
                  curr_cel,
                  TRUE,
                  timestamp
          );
          qcril_qmi_nas_init_lte_cell_identity_v6(
                  &curr_cel->CellInfo.lte.cellIdentityLte,
                  li->lte_intra.plmn,
                  TRUE,
                  li->lte_intra.tac,
                  TRUE,
                  li->lte_intra.global_cell_id,
                  TRUE,
                  li->lte_intra.serving_cell_id
          );
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
                  li->timing_advance_valid,
                  li->timing_advance
          );

          QCRIL_LOG_DEBUG("Serving cell rsrp: %d. rssi: %d. srxlev: %d. output signal Strength: %d",
                  -(li->lte_intra.cells[i].rsrp / 10),
                  li->lte_intra.cells[i].rssi,
                  li->lte_intra.cells[i].srxlev,
                  srv_cell->CellInfo.lte.signalStrengthLte.signalStrength);

        }
        else
        {
          /* This is for a neighbor Cell */
          qcril_qmi_nas_init_lte_cell_v6(
                  curr_cel,
                  TRUE,
                  timestamp
          );
          qcril_qmi_nas_init_lte_cell_identity_v6(
                  &curr_cel->CellInfo.lte.cellIdentityLte,
                  NULL,
                  FALSE,
                  0xffff,
                  FALSE,
                  INT_MAX,
                  TRUE,
                  li->lte_intra.cells[i].pci
          );
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

          ++(*ril_info);
          curr_cel = (*ril_info);
          ret ++;
        }
      }

      if (!ss_found)
      {
        qcril_qmi_nas_init_lte_cell_v6(
          srv_cell,
          TRUE,
          timestamp
        );
        qcril_qmi_nas_init_lte_cell_identity_v6(
          &srv_cell->CellInfo.lte.cellIdentityLte,
          li->lte_intra.plmn,
          TRUE,
          li->lte_intra.tac,
          TRUE,
          li->lte_intra.global_cell_id,
          TRUE,
          li->lte_intra.serving_cell_id
        );
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
          0xffff,
          FALSE,
          INT_MAX,
          li->timing_advance_valid,
          li->timing_advance
        );

      }

      if (i < li->lte_intra.cells_len)
      {
        QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - Intra. "
                        "Slots: %d. Want %d more",
                        ril_info_items,
                        li->lte_intra.cells_len - i);
      }
    }

    if ( li->lte_inter_valid )
    {
      for (i = 0; i < li->lte_inter.freqs_len; i++)
      {
        for (j = 0; j < li->lte_inter.freqs[i].cells_len && ret < ril_info_items; j++)
        {
          qcril_qmi_nas_init_lte_cell_v6(
                  curr_cel,
                  FALSE,
                  timestamp
          );
          qcril_qmi_nas_init_lte_cell_identity_v6(
                  &curr_cel->CellInfo.lte.cellIdentityLte,
                  NULL,
                  FALSE,
                  0xffff,
                  FALSE,
                  INT_MAX,
                  TRUE,
                  li->lte_inter.freqs[i].cells[j].pci
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

          ++(*ril_info);
          curr_cel = (*ril_info);
          ret ++;
        }
        if ( j < li->lte_inter.freqs[i].cells_len )
        {
          QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - Inter, freq %d. "
                          "Slots: %d. Want %d more",
                          i,
                          ril_info_items,
                          li->lte_inter.freqs[i].cells_len - j);
        }
      }
    }

    if ( li->lte_gsm_valid )
    {
      for (i = 0; i < li->lte_gsm.freqs_len; i++)
      {
        for (j = 0; j < li->lte_gsm.freqs[i].cells_len && ret < ril_info_items; j++)
        {
          qcril_qmi_nas_init_gsm_cell_v6(
                  curr_cel,
                  FALSE,
                  timestamp
          );
          qcril_qmi_nas_init_gsm_cell_identity_v6(
                  &curr_cel->CellInfo.gsm.cellIdentityGsm,
                  NULL,
                  FALSE,
                  0xffff,
                  FALSE,
                  0xffffffff
          );
          qcril_qmi_nas_init_gsm_cell_ss_v6(
                  &curr_cel->CellInfo.gsm.signalStrengthGsm,
                  NAS_RADIO_IF_LTE,
                  TRUE,
                  li->lte_gsm.freqs[i].cells[j].rssi,
                  FALSE,
                  99
          );

          ++(*ril_info);
          curr_cel = (*ril_info);
          ret ++;
        }
        if ( j < li->lte_gsm.freqs[i].cells_len )
        {
          QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - GSM Neighbors, freq %d. "
                          "Slots: %d. Want %d more",
                          i,
                          ril_info_items,
                          li->lte_gsm.freqs[i].cells_len - j);
        }
      }
    }

    if ( li->lte_wcdma_valid )
    {
      for (i = 0; i < li->lte_wcdma.freqs_len; i++)
      {
        for (j = 0; j < li->lte_wcdma.freqs[i].cells_len && ret < ril_info_items; j++)
        {
          qcril_qmi_nas_init_wcdma_cell_v6(
                  curr_cel,
                  FALSE,
                  timestamp
          );
          qcril_qmi_nas_init_wcdma_cell_identity_v6(
                  &curr_cel->CellInfo.wcdma.cellIdentityWcdma,
                  NULL,
                  FALSE,
                  0xffff,
                  FALSE,
                  0xffff,
                  TRUE,
                  li->lte_wcdma.freqs[i].cells[j].psc
          );
          qcril_qmi_nas_init_wcdma_cell_ss_v6(
                  &curr_cel->CellInfo.wcdma.signalStrengthWcdma,
                  NAS_RADIO_IF_LTE,
                  TRUE,
                  li->lte_wcdma.freqs[i].cells[j].cpich_rscp,
                  FALSE,
                  99
          );

          ++(*ril_info);
          curr_cel = (*ril_info);
          ret ++;
        }
        if ( j < li->lte_wcdma.freqs[i].cells_len )
        {
          QCRIL_LOG_ERROR("Not enough RIL_CellInfo slots for LTE - WCDMA Neighbors, freq %d. "
                          "Slots: %d. Want %d more",
                          i,
                          ril_info_items,
                          li->lte_wcdma.freqs[i].cells_len - j);
        }
      }
    }
  }
  return ret;
} // qcril_qmi_nas_convert_lte_cells_to_ril_cellinfo

//===========================================================================
// qcril_ril_cellinfo_free
//===========================================================================
void qcril_ril_cellinfo_free
(
  RIL_CELL_INFO_LIST_TYPE *cellinfo
)
{
  if (cellinfo)
  {
    qcril_free(cellinfo);
  }
} // qcril_ril_cellinfo_free

//===========================================================================
// qcril_qmi_nas_convert_cell_location_to_ril_cellinfo
//
//  Convert a qmi nas get_cell_location_info response into the format used
//  by Android
//
//  nas_location_info: The get_cell_location_info response data from qmi nas
//  cellinfo: Output parameter in which the function will put a pointer to the
//            array of RIL_CellInfo
//  time: Timestamp at which the information was captured
//  number_of_cells:   Output parameter. Number of elements in the array pointed
//                     to by *cellinfo
//
//===========================================================================
void qcril_qmi_nas_convert_cell_location_to_ril_cellinfo
(
  nas_get_cell_location_info_resp_msg_v01 *nas_location_info,
  RIL_CELL_INFO_LIST_TYPE **cellinfo,
  uint64_t *time,
  unsigned *number_of_cells
)
{
  unsigned totcells = 0, ncells = 0;
  unsigned consumed_cells = 0;
  int buflen = 0;
  RIL_CELL_INFO_LIST_TYPE *cellinfobuf = NULL, *cellinfoptr = NULL;
  nas_get_cell_location_info_resp_msg_v01 *li = nas_location_info;

  if (nas_location_info && cellinfo && time)
  {
    *cellinfo = NULL;
    totcells = ncells = qcril_qmi_nas_count_cell_location_info_cells(li);
    buflen = ncells * sizeof(RIL_CELL_INFO_LIST_TYPE);
    QCRIL_LOG_DEBUG("CellInfo slots allocated: %d. Buflen: %d", ncells, buflen);
    cellinfobuf = qcril_malloc(buflen);
    cellinfoptr = cellinfobuf;
    if (cellinfobuf)
    {
      do
      {
        consumed_cells = qcril_qmi_nas_convert_geran_cells_to_ril_cellinfo(
                                     nas_location_info,
                                     time,
                                     &cellinfoptr,
                                     ncells);

        if (consumed_cells > ncells)
        {
          QCRIL_LOG_ERROR("GERAN consumed more cell slots than available: %d", consumed_cells);
          break;
        }
        else
        {
          QCRIL_LOG_DEBUG("GERAN consumed %d cell slots", consumed_cells);
        }
        ncells -= consumed_cells;

        consumed_cells = qcril_qmi_nas_convert_umts_cells_to_ril_cellinfo(
                                     nas_location_info,
                                     time,
                                     &cellinfoptr,
                                     ncells);
        if (consumed_cells > ncells)
        {
          QCRIL_LOG_ERROR("UMTS consumed more cell slots than available: %d", consumed_cells);
          break;
        }
        else
        {
          QCRIL_LOG_DEBUG("UMTS consumed %d cell slots", consumed_cells);
        }
        ncells -= consumed_cells;

        consumed_cells = qcril_qmi_nas_convert_cdma_cells_to_ril_cellinfo(
                                     nas_location_info,
                                     time,
                                     &cellinfoptr,
                                     ncells);
        if (consumed_cells > ncells)
        {
          QCRIL_LOG_ERROR("CDMA Consumed more cell slots than available: %d", consumed_cells);
          break;
        }
        else
        {
          QCRIL_LOG_DEBUG("CDMA consumed %d cell slots", consumed_cells);
        }
        ncells -= consumed_cells;

        consumed_cells = qcril_qmi_nas_convert_lte_cells_to_ril_cellinfo(
                                     nas_location_info,
                                     time,
                                     &cellinfoptr,
                                     ncells);
        if (consumed_cells > ncells)
        {
          QCRIL_LOG_ERROR("LTE Consumed more cell slots than available: %d", consumed_cells);
          break;
        }
        else
        {
          QCRIL_LOG_DEBUG("LTE consumed %d cell slots", consumed_cells);
        }
        ncells -= consumed_cells;

      } while(0);
    }

    (*cellinfo) = cellinfobuf;
  }
  else
  {
    QCRIL_LOG_DEBUG("NULL parameters passed");
  }

  if (number_of_cells)
  {
    QCRIL_LOG_DEBUG("Total Cells: %d. Used cells: %d", totcells, totcells - ncells);
    *number_of_cells = totcells - ncells;
  }
} // qcril_qmi_nas_convert_cell_location_to_ril_cellinfo

int qcril_cell_location_to_ril_cell_info_v6(struct qcril_chain_data *data)
{
  int ret = FALSE;
  unsigned ncells = 0;
  RIL_CELL_INFO_LIST_TYPE * output = NULL;

  if ( QCRIL_RIL_VERSION > 6 && data && data->input && data->parm1 && !data->output)
  {
    uint64_t *timestamp = (uint64_t *)data->parm1;
    nas_get_cell_location_info_resp_msg_v01 *input = (nas_get_cell_location_info_resp_msg_v01 *)data->input;
    QCRIL_LOG_DEBUG("Running v6 conversion");

    qcril_qmi_nas_convert_cell_location_to_ril_cellinfo(
            input,
            &output,
            timestamp,
            &ncells
    );
      data->ret1 = (void *)(ncells * sizeof(RIL_CELL_INFO_LIST_TYPE));
      data->ret2 = (void *)(unsigned long)ncells;
      data->output = output;
    ret = TRUE;
  }
  return ret;
}

