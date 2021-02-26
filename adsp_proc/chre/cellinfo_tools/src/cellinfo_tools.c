/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "cellinfo_tools.h"
#include <chre_api/chre/wwan.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

const char *cellinfo_get_type_name(enum chreWwanCellInfoType type)
{
    switch (type)
    {
        case CHRE_WWAN_CELL_INFO_TYPE_GSM:
            return "GSM";
        case CHRE_WWAN_CELL_INFO_TYPE_CDMA:
            return "CDMA";
        case CHRE_WWAN_CELL_INFO_TYPE_LTE:
            return "LTE";
        case CHRE_WWAN_CELL_INFO_TYPE_WCDMA:
            return "WCDMA";
        case CHRE_WWAN_CELL_INFO_TYPE_TD_SCDMA:
            return "TD-SCDMA";
        default:
            return "UNKNOWN";
    }
}

const char *cellinfo_get_timestamp_type_name(enum chreWwanCellTimeStampType ts_type)
{
    switch (ts_type)
    {
        default:
        case CHRE_WWAN_CELL_TIMESTAMP_TYPE_UNKNOWN:
            return "UNKNOWN";
        case CHRE_WWAN_CELL_TIMESTAMP_TYPE_ANTENNA:
            return "ANTENNA";
        case CHRE_WWAN_CELL_TIMESTAMP_TYPE_MODEM:
            return "MODEM";
        case CHRE_WWAN_CELL_TIMESTAMP_TYPE_OEM_RIL:
            return "OEM_RIL";
        case CHRE_WWAN_CELL_TIMESTAMP_TYPE_JAVA_RIL:
            return "JAVA_RIL";
    }
}

void cellinfo_dump_gsm_cell(const struct chreWwanCellInfoGsm *cell, cellinfo_dumpfn dumpfn)
{
    if  (!cell || !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("    Null Gsm Cell Pointer\n");
        }

        return;
    }

    dumpfn("    ID Gsm ");

    if (cell->cellIdentityGsm.mcc != INT32_MAX && cell->cellIdentityGsm.mnc != INT32_MAX)
    {
        dumpfn("plmn: %03d-%02d ", cell->cellIdentityGsm.mcc, cell->cellIdentityGsm.mnc);
    }

    if (cell->cellIdentityGsm.lac != INT32_MAX)
    {
        dumpfn("lac: %d ", cell->cellIdentityGsm.lac);
    }

    if (cell->cellIdentityGsm.cid != INT32_MAX)
    {
        dumpfn("cid: %d ", cell->cellIdentityGsm.cid);
    }

    if (cell->cellIdentityGsm.arfcn != INT32_MAX)
    {
        dumpfn("arfcn: %d ", cell->cellIdentityGsm.arfcn);
    }

    if (cell->cellIdentityGsm.bsic != UINT8_MAX)
    {
        dumpfn("bsic: %x ", (unsigned)cell->cellIdentityGsm.bsic);
    }

    dumpfn("\n");

    dumpfn("    SS GSM ");
    if ((cell->signalStrengthGsm.signalStrength >= 0 &&
            cell->signalStrengthGsm.signalStrength <= 31) ||
            cell->signalStrengthGsm.signalStrength == 99)
    {
        dumpfn("ss: %d ", cell->signalStrengthGsm.signalStrength);
    }
    else
    {
        dumpfn("ss: <invalid> ");
    }

    if ((cell->signalStrengthGsm.bitErrorRate >= 0 && cell->signalStrengthGsm.bitErrorRate <= 7) ||
            cell->signalStrengthGsm.bitErrorRate == 99)
    {
        dumpfn("ber: %d ", cell->signalStrengthGsm.bitErrorRate);
    }
    else
    {
        dumpfn("ber: <invalid> ");
    }

    if (cell->signalStrengthGsm.timingAdvance != INT32_MAX)
    {
        dumpfn("ta: %d ", cell->signalStrengthGsm.timingAdvance);
    }

    dumpfn("\n");
}

void cellinfo_dump_wcdma_cell(const struct chreWwanCellInfoWcdma *cell, cellinfo_dumpfn dumpfn)
{
    if  (!cell || !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("    Null Gsm Cell Pointer\n");
        }

        return;
    }

    dumpfn("    ID Wcdma ");

    if (cell->cellIdentityWcdma.mcc != INT32_MAX && cell->cellIdentityWcdma.mnc != INT32_MAX)
    {
        dumpfn("plmn: %03d-%02d ", cell->cellIdentityWcdma.mcc, cell->cellIdentityWcdma.mnc);
    }

    if (cell->cellIdentityWcdma.lac != INT32_MAX)
    {
        dumpfn("lac: %d ", cell->cellIdentityWcdma.lac);
    }

    if (cell->cellIdentityWcdma.cid != INT32_MAX)
    {
        dumpfn("cid: %d ", cell->cellIdentityWcdma.cid);
    }

    if (cell->cellIdentityWcdma.psc != INT32_MAX)
    {
        dumpfn("psc: %x ", (unsigned)cell->cellIdentityWcdma.psc);
    }

    if (cell->cellIdentityWcdma.uarfcn != INT32_MAX)
    {
        dumpfn("uarfcn: %d ", cell->cellIdentityWcdma.uarfcn);
    }

    dumpfn("\n");

    dumpfn("    SS WCDMA ");
    if ((cell->signalStrengthWcdma.signalStrength >= 0 &&
            cell->signalStrengthWcdma.signalStrength <= 31) ||
            cell->signalStrengthWcdma.signalStrength == 99)
    {
        dumpfn("ss: %d ", cell->signalStrengthWcdma.signalStrength);
    }
    else
    {
        dumpfn("ss: <invalid> ");
    }

    if ((cell->signalStrengthWcdma.bitErrorRate >= 0 && cell->signalStrengthWcdma.bitErrorRate <= 7) ||
            cell->signalStrengthWcdma.bitErrorRate == 99)
    {
        dumpfn("ber: %d ", cell->signalStrengthWcdma.bitErrorRate);
    }
    else
    {
        dumpfn("ber: <invalid> ");
    }

    dumpfn("\n");
}

void cellinfo_dump_lte_cell(const struct chreWwanCellInfoLte *cell, cellinfo_dumpfn dumpfn)
{
    if (!cell | !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("    Null LTE Cell Pointer\n");
        }
        return;
    }

    dumpfn("    ID LTE ");

    if (cell->cellIdentityLte.mcc != INT32_MAX &&
            cell->cellIdentityLte.mnc != INT32_MAX )
    {
        dumpfn("plmn: %03d-%02d ",
                cell->cellIdentityLte.mcc,
                cell->cellIdentityLte.mnc);
    }

    if (cell->cellIdentityLte.ci != INT32_MAX)
    {
        dumpfn("ci: %d ", cell->cellIdentityLte.ci);
    }

    if (cell->cellIdentityLte.pci != INT32_MAX)
    {
        dumpfn("pci: %d ", cell->cellIdentityLte.pci);
    }

    if (cell->cellIdentityLte.tac != INT32_MAX)
    {
        dumpfn("tac: %d ", cell->cellIdentityLte.tac);
    }

    if (cell->cellIdentityLte.earfcn != INT32_MAX)
    {
        dumpfn("earfcn: %d ", cell->cellIdentityLte.earfcn);
    }

    dumpfn("\n");

    dumpfn("    SS LTE ");

    if( (cell->signalStrengthLte.signalStrength >= 0 &&
                cell->signalStrengthLte.signalStrength <= 31 ) ||
            cell->signalStrengthLte.signalStrength == 99)
    {
        dumpfn("ss: %d ", cell->signalStrengthLte.signalStrength);
    }
    else
    {
        dumpfn("ss: <invalid> ");
    }

    if ( cell->signalStrengthLte.rsrp >= 44 &&
            cell->signalStrengthLte.rsrp <= 140 )
    {
        dumpfn("rsrp: %d ", cell->signalStrengthLte.rsrp);
    }
    else if (cell->signalStrengthLte.rsrp != INT32_MAX)
    {
        dumpfn("rsrp: <invalid> ");
    }

    if (cell->signalStrengthLte.rsrq >= 3 &&
            cell->signalStrengthLte.rsrq <= 20)
    {
        dumpfn("rsrq: %d ", cell->signalStrengthLte.rsrq);
    }
    else if (cell->signalStrengthLte.rsrq != INT32_MAX)
    {
        dumpfn("rsrq: <invalid> ");
    }

    if (cell->signalStrengthLte.rssnr >= -200 &&
            cell->signalStrengthLte.rssnr <= 300)
    {
        dumpfn("rssnr: %d ", cell->signalStrengthLte.rssnr);
    }
    else if (cell->signalStrengthLte.rssnr != INT32_MAX )
    {
        dumpfn("rssnr: <invalid> ");
    }

    if (cell->signalStrengthLte.cqi >= 0 &&
            cell->signalStrengthLte.cqi <= 15)
    {
        dumpfn("cqi: %d ", cell->signalStrengthLte.cqi);
    }
    else if (cell->signalStrengthLte.cqi != INT32_MAX)
    {
        dumpfn("cqi: <invalid> ");
    }

    if (cell->signalStrengthLte.timingAdvance != INT32_MAX)
    {
        dumpfn("timingAdvance: %d", cell->signalStrengthLte.timingAdvance);
    }

}

void cellinfo_dump_cdma_cell(const struct chreWwanCellInfoCdma *cell, cellinfo_dumpfn dumpfn)
{
    if (!cell || !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("    Null Cdma Cell Pointer\n");
        }

        return;
    }

    dumpfn("    ID CDMA ");

    if (cell->cellIdentityCdma.networkId != INT32_MAX)
    {
        dumpfn("nwid: %d ", cell->cellIdentityCdma.networkId);
    }

    if (cell->cellIdentityCdma.systemId != INT32_MAX)
    {
        dumpfn("sysid: %d ", cell->cellIdentityCdma.systemId);
    }

    if (cell->cellIdentityCdma.basestationId != INT32_MAX)
    {
        dumpfn("bsid: %d ", cell->cellIdentityCdma.basestationId);
    }

    if (cell->cellIdentityCdma.latitude != INT32_MAX)
    {
        dumpfn("lat: %d ", cell->cellIdentityCdma.latitude);
    }

    if (cell->cellIdentityCdma.longitude != INT32_MAX)
    {
        dumpfn("long: %d ", cell->cellIdentityCdma.longitude);
    }

    dumpfn("\n");

    dumpfn("    SS CDMA ");

    if (cell->signalStrengthCdma.dbm > 0)
    {
        dumpfn("dbm: %d ", cell->signalStrengthCdma.dbm);
    }

    if (cell->signalStrengthCdma.ecio > 0)
    {
        dumpfn("ecio: %d ", cell->signalStrengthCdma.ecio);
    }

    dumpfn("\n");

    dumpfn("    SS EvDo ");

    if (cell->signalStrengthEvdo.dbm > 0)
    {
        dumpfn("dbm: %d ", cell->signalStrengthEvdo.dbm);
    }

    if (cell->signalStrengthEvdo.ecio > 0)
    {
        dumpfn("ecio: %d ", cell->signalStrengthEvdo.ecio);
    }

    if (cell->signalStrengthEvdo.signalNoiseRatio >= 0 &&
            cell->signalStrengthEvdo.signalNoiseRatio <= 8)
    {
        dumpfn("snr: %d ", cell->signalStrengthEvdo.signalNoiseRatio);
    }
    dumpfn("\n");
}

void cellinfo_dump_tdscdma_cell(const struct chreWwanCellInfoTdscdma *cell, cellinfo_dumpfn dumpfn)
{
    if (!cell || !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("    Null TD-SCDMA Cell Pointer\n");
        }

        return;
    }

    dumpfn("    ID TD-SCDMA ");

    if (cell->cellIdentityTdscdma.mcc != INT32_MAX &&
            cell->cellIdentityTdscdma.mnc != INT32_MAX)
    {
        dumpfn("plmn: %03d-%02d ",
                cell->cellIdentityTdscdma.mcc,
                cell->cellIdentityTdscdma.mnc);
    }

    if (cell->cellIdentityTdscdma.lac != INT32_MAX)
    {
        dumpfn("lac: %d ", cell->cellIdentityTdscdma.lac);
    }

    if (cell->cellIdentityTdscdma.cid != INT32_MAX)
    {
        dumpfn("cid: %d ", cell->cellIdentityTdscdma.cid);
    }

    if (cell->cellIdentityTdscdma.cpid != INT32_MAX)
    {
        dumpfn("cpid: %d ", cell->cellIdentityTdscdma.cpid);
    }

    dumpfn("\n");

    dumpfn("    SS TD-SCDMA ");

    if (cell->signalStrengthTdscdma.rscp >= 25 &&
            cell->signalStrengthTdscdma.rscp <= 120)
    {
        dumpfn("rscp: %d", cell->signalStrengthTdscdma.rscp);
    }
    else if (cell->signalStrengthTdscdma.rscp != INT32_MAX)
    {
        dumpfn("rscp: <invalid>");
    }

    dumpfn("\n");
}

void cellinfo_dump_cell(const struct chreWwanCellInfo *cell, cellinfo_dumpfn dumpfn)
{
    if (!cell || !dumpfn)
    {
        if (dumpfn)
        {
            dumpfn("  Null Cell Pointer\n");
        }
        return;
    }

    dumpfn("  [%10s_TS %20" PRIu64 "] %s %s {\n",
            cellinfo_get_timestamp_type_name(cell->timeStampType),
            cell->timeStamp,
            cellinfo_get_type_name(cell->cellInfoType),
            (cell->registered ? "(registered)" : ""));
    switch(cell->cellInfoType)
    {
        case CHRE_WWAN_CELL_INFO_TYPE_GSM:
            cellinfo_dump_gsm_cell(&cell->CellInfo.gsm, dumpfn);
            break;
        case CHRE_WWAN_CELL_INFO_TYPE_WCDMA:
            cellinfo_dump_wcdma_cell(&cell->CellInfo.wcdma, dumpfn);
            break;
        case CHRE_WWAN_CELL_INFO_TYPE_LTE:
            cellinfo_dump_lte_cell(&cell->CellInfo.lte, dumpfn);
            break;
        case CHRE_WWAN_CELL_INFO_TYPE_CDMA:
            cellinfo_dump_cdma_cell(&cell->CellInfo.cdma, dumpfn);
            break;
        case CHRE_WWAN_CELL_INFO_TYPE_TD_SCDMA:
            cellinfo_dump_tdscdma_cell(&cell->CellInfo.tdscdma, dumpfn);
            break;
    }

    dumpfn("  }\n");
}

void cellinfo_dump_event(const struct chreWwanCellInfoResult *evt, cellinfo_dumpfn dumpfn)
{
    unsigned i;
    if (!evt || !dumpfn) // TODO is this size check sufficient?
    {
        if (dumpfn)
        {
            dumpfn("Invalid chreWwanCellInfoResult\n");
        }
        return;
    }

    dumpfn("chreWwanCellInfoResult_v%d (%u cells) { \n",
            evt->version,
            evt->cellInfoCount);
    for (i = 0; i < evt->cellInfoCount; i++)
    {
        cellinfo_dump_cell(&evt->cells[i], dumpfn);
    }

    dumpfn("}\n");
}

struct chreWwanCellInfoResult * cellinfo_alloc_event(size_t ncells)
{
    struct chreWwanCellInfoResult *evt = NULL;

    evt = malloc(sizeof(*evt));
    if (evt)
    {
      memset(evt, 0, sizeof(*evt));
      if (ncells > 0) {
        evt->cells = malloc(ncells * sizeof(*(evt->cells)));
        if (evt->cells) {
          memset((void *) evt->cells, 0, ncells * sizeof(*(evt->cells)));
        }
      } else {
        evt->cells = NULL;
      }
      evt->version = CHRE_WWAN_CELL_INFO_RESULT_VERSION;
    }

    return evt;
}

void cellinfo_free_event(const struct chreWwanCellInfoResult *evt)
{
    if (evt){
        if (evt->cells) {
            free((void*) evt->cells);
        }
        free((void*) evt);
    }
}
