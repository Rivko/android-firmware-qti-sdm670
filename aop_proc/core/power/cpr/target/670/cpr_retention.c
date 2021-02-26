/*=========================================================================
                                   cpr_retention.c

DESCRIPTION:
  This file initializes the retention voltages for the specified rails.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
=========================================================================*/

//=========================================================================
//                      Includes and Variable Definitions
//=========================================================================
//
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include "cprf.h"
#include "cpr_retention.h"
#include "arc_runtime.h"
#include "cpr_fuses_hwio.h"
#include "msmhwio.h"
#include "cpr_smem.h"
#include "cpr_data.h"
#include "ChipInfo.h"

// retention programmed in mV ( 600mV = 0.6V )

static const uint32 vddcx_retention_table[8] =
{
   /* 000 */ 560,
   /* 001 */ 520,
   /* 010 */ 480,
   /* 011 */ 440,
   /* 100 */ 400,
   /* 101 */ 350,
   /* 110 */ 350,
   /* 111 */ 560
};

static const uint32 vddmx_retention_data[8] =
{
   /* 000 */ 644,
   /* 001 */ 644,
   /* 010 */ 592,
   /* 011 */ 540,
   /* 100 */ 488,
   /* 101 */ 488,
   /* 110 */ 488,
   /* 111 */ 644
};

/* lpi_mx follows system mx, but its a 8mV stepper instead of 4mV
* which causes the floor to be different voltage levels, hence
* a seperate table is used */
static const uint32 vddlpimx_retention_data[8] =
{
   /* 000 */ 648,
   /* 001 */ 648,
   /* 010 */ 592,
   /* 011 */ 544,
   /* 100 */ 488,
   /* 101 */ 488,
   /* 110 */ 488,
   /* 111 */ 648
};

#ifdef QCS605_CPR_CONFIG
static const uint32 vddlcx_retention_table[8] =
{
   /* 000 */ 560+96,
   /* 001 */ 520+96,
   /* 010 */ 480+96,
   /* 011 */ 440+96,
   /* 100 */ 400+96,
   /* 101 */ 350+96,
   /* 110 */ 350+96,
   /* 111 */ 560+96
};
#else
static const uint32 vddlcx_retention_table[8] =
{
   /* 000 */ 560,
   /* 001 */ 520,
   /* 010 */ 480,
   /* 011 */ 440,
   /* 100 */ 400,
   /* 101 */ 350,
   /* 110 */ 350,
   /* 111 */ 560
};
#endif

static const pvs_data_t vddcx_pvs_data =
{
   /* .retention_table */ vddcx_retention_table,
   /* .fuse            */ { HWIO_ADDR(QFPROM_RAW_PTE_ROW0_MSB), 29, 0x7 }  //correct address 0x780134 bit[61:63]
};

static const pvs_data_t vddmx_pvs_data =
{
   /* .retention_table */ vddmx_retention_data,
   /* .fuse            */ { HWIO_ADDR(QFPROM_RAW_PTE_ROW3_LSB), 17, 0x7 }   //correct address 0x780148 bit[17:19]}
};

/* lpi_mx follows system mx, but its a 8mV stepper instead of 4mV
* which could causes the floor to be different voltage levels, hence
* a seperate table is used */
static const pvs_data_t vddlpimx_pvs_data =
{
   /* .retention_table */ vddlpimx_retention_data,
   /* .fuse            */ { HWIO_ADDR(QFPROM_RAW_PTE_ROW3_LSB), 17, 0x7 }   //correct address 0x780148 bit[17:19]}
};

static const pvs_data_t vddlcx_pvs_data =
{
   /* .retention_table */ vddlcx_retention_table,
   /* .fuse            */ { HWIO_ADDR(QFPROM_RAW_PTE_ROW0_MSB), 29, 0x7 }  //correct address 0x780134 bit[61:63]
};



static const aon_data_t aon_data_670 =
{
    /* voltage */ 572,
    /* fuse    */ { HWIO_ADDRI(QFPROM_RAW_QC_SPARE_REGn_LSB, 16), 0, 0x1f }
};

static uint32 read_fuse_value(const fuse_t *fuse)
{
    return ( __inpdw(fuse->address) >> fuse->offset ) & (uint32)fuse->mask;
}

static void set_sleep_voltage(uint8 rail_id, const pvs_data_t *pvs_data_ptr)
{
  uint32 lookup_val = read_fuse_value( &pvs_data_ptr->fuse );

  cpr_set_ret_voltage(rail_id, pvs_data_ptr->retention_table[lookup_val]);
}

static int32 decode_aon_targ_volt(void)
{
    uint32 aon_raw_fuse = read_fuse_value( &(aon_data_670.fuse) );
    uint8 aon_sign = (aon_raw_fuse & 0x10)>>4;
    uint16 aon_step = (aon_raw_fuse & 0xf);
    int32 voltage = 0;

    voltage = aon_sign ?
                (aon_data_670.voltage - (aon_step * 10)) :
                (aon_data_670.voltage + (aon_step * 10)) ;

    return voltage;
}

static int32 get_aon_aging_margin()
{
    cpr_rail mx_rail;
    cpr_rail_state mx_state;
    mx_rail.id = CPR_RAIL_MX;
    mx_state.modeSettings = NULL;
    mx_state.activeMode = NULL;
    mx_state.previousMode = NULL;
    int32 aging_margin = 0;

    if (ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SDM845 &&
            ChipInfo_GetChipVersion() < CHIPINFO_VERSION(2,0))
        return 0;

    cpr_smem_deserialize_config(&mx_rail, &mx_state);

    aging_margin = mx_state.marginAdjustments[CPR_MARGIN_ADJUSTMENT_TEST];

    return aging_margin ? (aging_margin/1000) : aging_margin;
}

void cpr_retention_init(void)
{
    set_sleep_voltage((uint8)RPMH_ARC_VDD_CX, &vddcx_pvs_data);
    set_sleep_voltage((uint8)RPMH_ARC_VDD_EBI, &vddcx_pvs_data);
    set_sleep_voltage((uint8)RPMH_ARC_SSC_CX, &vddlcx_pvs_data);
    set_sleep_voltage((uint8)RPMH_ARC_SSC_MX, &vddlpimx_pvs_data);

    /* MX RET Voltage =   Max ( MEM_RETENTION (decoded),
     *                          AON_TARG_VOLT (decoded) - 15mV +
     *                          AON_AGING (decoded) )
     */
    uint32 lookup_val = read_fuse_value( &(vddmx_pvs_data.fuse) );
    uint32 mx_ret_dec = vddmx_pvs_data.retention_table[lookup_val];
    int32 aon_targ_volt_dec = decode_aon_targ_volt();
    int32 aon_aging_margin = get_aon_aging_margin();
    uint32 mx_ret_vol = MAX (mx_ret_dec,
            (aon_targ_volt_dec - 15 + aon_aging_margin));
    cpr_set_ret_voltage(RPMH_ARC_VDD_MX, mx_ret_vol);
}
