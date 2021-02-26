/**
 * @file:  cpr_voltage_plan.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/inc/cpr_voltage_plan.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_VOLTAGE_PLAN_H
#define CPR_VOLTAGE_PLAN_H

#include "cpr_types.h"

#define CPR_FUSE_MAPPING(base_register, field) { \
    .address = HWIO_##base_register##_ADDR, \
    .offset  = HWIO_##base_register##_##field##_SHFT, \
    .mask    = HWIO_##base_register##_##field##_BMSK }


typedef uint32(*cpr_idx_func)(cpr_voltage_mode mode);

typedef struct
{
    /*
     * NOTE:
     * MSB bit(s) must be the first entry.
     * LSB bit(s) (bit0) must be the last entry.
     */
    const struct raw_fuse_data {
        uintptr_t address;
        uint32 offset;
        uint32 mask;
    } *data;
    uint16 count;
} cpr_fuse;

typedef struct
{
    const cpr_fuse* volt;
    const cpr_fuse* quot;
    const cpr_fuse* rosel;
    const cpr_fuse* quotOffset;
    const cpr_fuse* voltOffset;
}cpr_fuse_data;

typedef struct
{
    cpr_foundry_id foundry;
    uint32 min;
    uint32 max;
} cpr_version;

typedef struct
{
    const cpr_version** versions;
    uint8 count;
} cpr_version_range;

typedef struct
{
    const cpr_kv *kvs;
    uint8 count;
} cpr_kv_cfg;

typedef struct
{
    const cpr_quotient *quots;
    uint8 count;
} cpr_quotient_cfg;

typedef struct
{
    uint8 cprRevMin;
    uint8 cprRevMax;
    int32 openLoop;
    int32 closedLoop;
    int32 maxFloorToCeil;
} cpr_margin_data;

typedef struct
{
    uint8 count;
    const cpr_margin_data *data;
} cpr_margin_cfg;

// Added for temperature margin adjustment feature
typedef struct 
{
    uint8  tempSensorStartId; // Used for temp adj feature [startid:endid]
    uint8  tempSensorEndId;   // Used for temp adj feature [startid:endid]

    uint8  tempPoints[3];     // Used for the temperature thresholds. in degree C

#if 0 /* Disable debug feature to save memory */
    uint8  singleStepQuot;  // Used for debugging purposes only, read HPG for more info
    uint8  perRoKvMarginEn; // Used for debugging purposes only, read HPG for more info
#endif
} cpr_temp_adj_cfg;

typedef struct
{
    uint8                    modesCount;
    const cpr_voltage_mode*  supportedModes;
    const cpr_temp_adj_cfg*  tempAdjCfg; // Used for temp margin adjustment 

    const cpr_kv_cfg*        kvCfg; // KV values are same among all modes

    struct cpr_voltage_data
    {
        uint32                fref; // EFuse Open Loop Reference Voltage
        const cpr_margin_cfg* margins;
        const cpr_fuse_data*  fuses;
        uint8                 marginTempBands[4]; /* Used for temp margin adjustment.
                                                    The amount of margin in number of PMIC steps
                                                    NOTE: non-APC rails has only one core.  */
        uint32                   freqDelta;
        const cpr_quotient_cfg*  quotients;

        uint8                    subModesCount;

        const struct cpr_freq_data {
            boolean interplEnabled;
            uint32 freq;
            uint32 ceiling;
            uint32 floor;
        } *subModes;
    } *modes;
} cpr_voltage_plan;

typedef struct cpr_voltage_data cpr_voltage_data;
typedef struct cpr_freq_data cpr_freq_data;

typedef struct cpr_aging_cfg
{
    uint8              cprRevMinToRun; /* run de-aging if CPR Rev is >= this CPR Rev */
    cpr_voltage_mode   modeToRun;
    uint8              sensorID;
    uint8              bypassSensorIDsCount;
    uint8              fusePackingFactor; /* used to multiply the final fuse value by this value */
    uint32             kv; /* x100 */
    uint32             scalingFactor; /* x10 */
    int32              marginLimit;
    uint8              *bypassSensorIDs;
    cpr_fuse           fuse;
} cpr_aging_cfg;

typedef struct
{
    cpr_domain_id rail;
    uint8 count;
    const struct voltage_plan_list {
        const cpr_version *version;
        const cpr_voltage_plan* cfg;
    } *list;
    const cpr_aging_cfg *agingCfg;
} cpr_versioned_voltage_plan;

#endif
