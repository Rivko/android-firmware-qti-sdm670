/**
 * @file:  cpr_defs.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2017/12/06 00:59:17 $
 * $Header: //components/rel/core.qdsp6/2.1/api/power/cpr_defs.h#1 $
 * $Change: 14992535 $
 */
#ifndef CPR_DEFS_H
#define CPR_DEFS_H

#include "comdef.h"
#include "npa_resource.h"
#include "voltage_level.h"

typedef enum
{
    CPR_VOLTAGE_MODE_OFF,
    CPR_VOLTAGE_MODE_RETENTION,
    CPR_VOLTAGE_MODE_MIN_SVS,
    CPR_VOLTAGE_MODE_LOW_SVS,
    CPR_VOLTAGE_MODE_SVS,
    CPR_VOLTAGE_MODE_SVS_L1,
    CPR_VOLTAGE_MODE_NOMINAL,
    CPR_VOLTAGE_MODE_NOMINAL_L1,
    CPR_VOLTAGE_MODE_TURBO,
    CPR_VOLTAGE_MODE_TURBO_L1,
    CPR_VOLTAGE_MODE_SUPER_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR,

} cpr_voltage_mode;

typedef enum
{
    // Core rails
    CPR_RAIL_MX     = 0x100,
    CPR_RAIL_CX,
    CPR_RAIL_MSS,
    CPR_RAIL_GFX,
    CPR_RAIL_VDDA,

    // APC rails
    CPR_RAIL_APC0   = 0x200,
    CPR_RAIL_APC1,
    CPR_RAIL_APC2,
    CPR_RAIL_APC3,
    CPR_RAIL_APC4,
    CPR_RAIL_APC5,
    CPR_RAIL_CBF,
    CPR_RAIL_APCC_LVT,
    CPR_RAIL_APCC_SLVT,
    CPR_RAIL_APCC_M4M,

    // SSC rails
    CPR_RAIL_SSC_MX = 0x300,
    CPR_RAIL_SSC_CX,

    // SSC rails
    CPR_RAIL_LPI_MX = 0x400,
    CPR_RAIL_LPI_CX,

    CPR_RAIL_MSS_LDO = 0x800,
    CPR_RAIL_TURING_LDO
} cpr_domain_id;

typedef enum {
    CPR_DOMAIN_TYPE_MODE_BASED,
    CPR_DOMAIN_TYPE_FREQ_BASED
} cpr_domain_type;

typedef struct
{
    cpr_domain_type type;
    union
    {
        cpr_voltage_mode mode;
        uint32 freq;
    } u;
} cpr_domain_info;

typedef cpr_domain_id cpr_rail_id_t;
typedef cpr_voltage_mode cpr_voltage_mode_t;

typedef enum
{
  CPR_NPA_RAIL_EVENT_INTRACORNER_VOLTAGE_UPDATE = NPA_TRIGGER_CUSTOM_EVENT1,
} CPRNPARailEventType;

typedef struct
{
  uint32 voltage_uv;
  rail_voltage_level corner;
  boolean ack;
} cpr_npa_rail_event_data_type;

#define CPR_NPA_RESOURCE_NAME_RAIL_MSS "/cpr/mss/uv"

#endif //CPR_DEFS_H
