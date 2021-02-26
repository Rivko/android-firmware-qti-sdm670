/**
 * @file:  cpr_smem.h
 * @brief:
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/10/31 00:28:19 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_smem.h#2 $
 * $Change: 14733083 $
 */
#ifndef CPR_SMEM_H
#define CPR_SMEM_H

#include "cpr_cfg.h"
#include "cpr_data.h"

//******************************************************************************
// V2 Defines
//******************************************************************************
typedef struct __attribute__ ((__packed__))
{
    uint32 mode;  // cpr_voltage_mode
    int16  decodedFuseSteps;
    int16  decodedFuseOffset;
    uint16 enableCount;
    uint16 targetsCount;
    uint16 submodesCount;
    int16  totalMarginMV;
} cpr_smem_mode_state;

typedef struct __attribute__ ((__packed__))
{
    uint16  size;              // in bytes
    uint32  id;                // cpr_rail_id
    uint32  controlMode;       // cpr_control_mode
    int32   marginAdjustments[CPR_MARGIN_ADJUSTMENT_MAX];
    int16   decodedFuseAging;
    int8    hasVPdata;         // Flag to indicate prsence of complete VP for rail.
    uint32  activeVoltageMode; // cpr_voltage_mode
    uint8   modeStateCount;
} cpr_smem_rail_hdr;

typedef struct __attribute__ ((__packed__))
{
    uint8   version;
    uint8   railCount;
    uint16  dataSize;       // Data size written to SMEM to get offset.
    uint16  smemSize;       // SMEM buffer size
} cpr_smem_hdr;

//------------------------------------------------------------------------------------------//

typedef struct __attribute__ ((__packed__))
{
    uint32 foundry;
    uint32 min;
    uint32 max;
} cpr_smem_version;

typedef struct __attribute__ ((__packed__))
{
    uint8  ro;
    uint16 kv;
} cpr_smem_kv;

typedef struct __attribute__ ((__packed__))
{
    boolean kvs; //cpr_smem_kv
    uint8 count;
} cpr_smem_kv_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint8  tempSensorStartId;
    uint8  tempSensorEndId;
    uint8  tempPoints[3];
} cpr_smem_temp_adj_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint8 cprRevMin;
    uint8 cprRevMax;
    int32 openLoop;
    int32 closedLoop;
    int32 maxFloorToCeil;
} cpr_smem_margin_data;

typedef struct __attribute__ ((__packed__))
{
    uint8 count;
    boolean data; //cpr_smem_margin_data
} cpr_smem_margin_cfg;

typedef struct __attribute__ ((__packed__))
{
    boolean volt; //cpr_smem_fuse
    boolean quot; //cpr_smem_fuse
    boolean rosel; //cpr_smem_fuse
    boolean quotOffset; //cpr_smem_fuse
    boolean voltOffset; //cpr_smem_fuse
} cpr_smem_fuse_data;

typedef struct __attribute__ ((__packed__))
{
    uint8  ro;
    uint16 quotient;
} cpr_smem_quotient;

typedef struct __attribute__ ((__packed__))
{
    boolean quots; //cpr_smem_quotient
    uint8 count;
} cpr_smem_quotient_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint32 interplEnabled;
    uint32 freq;
    uint32 ceiling;
    uint32 floor;
} cpr_smem_freq_data;

typedef struct __attribute__ ((__packed__))
{
    uint32 fref;
    boolean margins; //cpr_smem_margin_cfg
    boolean fuses; //cpr_smem_fuse_data
    uint8 marginTempBands[4];
    uint32 freqDelta;
    boolean quotients; //cpr_smem_quotient_cfg
    uint8 subModesCount;
    boolean subModes; //cpr_smem_freq_data
} cpr_smem_voltage_data;

typedef struct __attribute__ ((__packed__))
{
    uint8   modesCount;
    boolean supportedModes; //cpr_voltage_mode

    boolean tempAdjCfg; //cpr_smem_temp_adj_cfg
    boolean kvCfg; //cpr_smem_kv_cfg

    boolean modes; //cpr_smem_voltage_data
} cpr_smem_voltage_plan;

typedef struct __attribute__ ((__packed__))
{
    boolean version; //cpr_smem_version
    boolean cfg;     //cpr_smem_voltage_plan
} cpr_smem_voltage_plan_list;

typedef struct __attribute__ ((__packed__))
{
    uint32 address;
    uint32 offset;
    uint32 mask;
} cpr_smem_raw_fuse_data;

typedef struct __attribute__ ((__packed__))
{
    uint16 count;
    boolean data; //cpr_smem_raw_fuse_data
} cpr_smem_fuse;

typedef struct __attribute__ ((__packed__))
{
    uint8              cprRevMinToRun;
    uint32             modeToRun; //cpr_voltage_mode
    uint8              sensorID;
    uint8              bypassSensorIDsCount;
    uint8              fusePackingFactor;
    uint32             kv;
    uint32             scalingFactor;
    int32              marginLimit;
    boolean            bypassSensorIDs; //uint8*
    cpr_smem_fuse      fuse;
} cpr_smem_aging_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint32 rail; //cpr_domain_id
    uint8 count;
    boolean list; //cpr_smem_voltage_plan_list
    boolean agingCfg; //cpr_smem_aging_cfg
} cpr_smem_versioned_voltage_plan;

typedef struct __attribute__ ((__packed__))
{
    unsigned hotTempCx10 :12;
    unsigned hotSensor   : 4;
    unsigned coldTempCx10:12;
    unsigned coldSensor  : 4;
} cpr_smem_hal_temp_data;

typedef struct __attribute__ ((__packed__))
{
    uint8  sensorsTotal;
    uint8  bypassSensorsCount;
    uint8  disableSensorsCount;
    
    uint8  stepQuotMax;        
    uint8  stepQuotMin;        

    boolean bypassSensors; //uint8*
    boolean disableSensors; //uint8*

    boolean refClk; //char [50]
    boolean ahbClk; //char [50]
} cpr_smem_hal_controller_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint8   upThresh;
    uint8   dnThresh;
    uint8   consecUp;
    uint8   consecDn;

    uint8   sensorsCount;
    boolean sensors; //uint8*
} cpr_smem_hal_thread_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint8   id;
    boolean init;
    boolean enabled;
    uint16  roMask4Thread;
    boolean cfg; //cpr_smem_hal_thread_cfg
} cpr_smem_hal_thread;

typedef struct __attribute__ ((__packed__))
{
    uint32 base; //uintptr_t
    uint32 type; //cpr_controller_type
    uint8 threadsCount;

    cpr_smem_hal_temp_data   tempSensorData;
    boolean cfg; //cpr_smem_hal_controller_cfg
    boolean threads; //cpr_smem_hal_thread
} cpr_smem_hal_controller;

typedef struct __attribute__ ((__packed__))
{
    uint32 id; //cpr_domain_id
    uint8 railIdx;
    uint8 settleModesCount;
    boolean name; //char [50]
    uint32 interruptId;
    boolean settleModes; //cpr_voltage_mode*
} cpr_smem_rail;

typedef struct __attribute__ ((__packed__))
{
    uint32 id; //cpr_domain_id
    uint32 funcId; //cpr_init_funcs_id
    boolean version; //cpr_smem_version
    cpr_smem_fuse railCPRRevFuse; //cpr_smem_fuse
    uint16 stepSize;
    uint32 fuseMultiplier;
    int32 thermalAdjustment[CPR_THERMAL_REGION_MAX];
} cpr_smem_enablement;

typedef struct __attribute__ ((__packed__))
{
    cpr_smem_fuse cprRev;
} cpr_smem_misc_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint8 railsCount;
    uint8 enablementsCount;
    uint8 controllersCount;
    boolean railVVPs; //cpr_smem_versioned_voltage_plan
    boolean controllers; //cpr_smem_hal_controller
    boolean rails; //cpr_smem_rail
    boolean enablements; //cpr_smem_enablement
    boolean miscCfg; //cpr_smem_misc_cfg
} cpr_smem_settings;

void cpr_smem_serialize_config(cpr_rail_state* state, boolean append);
void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state);

void cpr_smem_serialize_settings(cpr_rail_state* state, boolean append);
void cpr_smem_deserialize_settings(cpr_rail* rail, cpr_settings* settting);
#endif
