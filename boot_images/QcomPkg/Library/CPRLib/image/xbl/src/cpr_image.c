/**
 * @file:  cpr_image.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/xbl/src/cpr_image.c#1 $
 * $Change: 14402555 $
 */
#include <stdlib.h>
#include "cpr_image.h"
#include "cpr_image_defs.h"
#include "cpr_image_target.h"
#include "cpr_cfg.h"
#include "cpr_data.h"
#include "cpr.h"
#include "cpr_smem.h"
#include "cpr_utils.h"

#include "railway.h"
#include "CoreHeap.h"
#include "CoreVerify.h"
#include "CoreMsgRam.h"
#include "DALSys.h"
#include "smem.h"
#include "LoaderUtils.h"

//******************************************************************************
// Global Data
//******************************************************************************

typedef struct
{
    cpr_domain_id   cpr_rail_id;
    const char*     railway_name;
} rail_name_map;

const static rail_name_map railway_name_map[] =
{
    { CPR_RAIL_MX,       "vddmx"      },
    { CPR_RAIL_CX,       "vddcx"      },
    { CPR_RAIL_VDDA,     "vdda_ebi"   },
    { CPR_RAIL_SSC_MX,   "vdd_ssc_mx" },
    { CPR_RAIL_SSC_CX,   "vdd_ssc_cx" },
    { CPR_RAIL_LPI_MX,   "vdd_lpi_mx" },
    { CPR_RAIL_LPI_CX,   "vdd_lpi_cx" },
    { CPR_RAIL_WLAN,     "vdd_wlan" },
};

typedef struct
{
    boolean         init;
    int32           railway_rail_id;
    railway_voter_t voter;
} railway_handle;

static railway_handle *handles;


//******************************************************************************
// Local Helper Functions
//******************************************************************************

static const char* cpr_rpm_railway_rail_name(cpr_domain_id rail_id)
{
    for(int i = 0; i < (sizeof(railway_name_map) / sizeof(rail_name_map)); i++) {
        if(rail_id == railway_name_map[i].cpr_rail_id) {
            return railway_name_map[i].railway_name;
        }
    }

    CPR_ASSERT( 0 );
    return NULL;
}

static railway_handle* get_railway_handle(cpr_domain_id railId)
{
    uint32 railIdx = cpr_utils_get_rail_idx( railId );

    if(!handles)
    {
        handles = cpr_image_malloc(sizeof(railway_handle) * cpr_info.settings->railsCount);
    }

    if(!handles[railIdx].init)
    {
        handles[railIdx].railway_rail_id = rail_id( cpr_rpm_railway_rail_name( railId ) );
        CPR_ASSERT( handles[railIdx].railway_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY );

        handles[railIdx].init = true;
    }

    return &handles[railIdx];
}

/*static void cpr_image_pre_switch(const railway_settings *settings, void* ctx)
{
    cpr_domain_id railId = (cpr_domain_id)(uintptr_t)ctx;
    cpr_pre_state_switch( railId );
}

static void cpr_image_post_switch(const railway_settings *settings, void* ctx)
{
    cpr_domain_id railId = (cpr_domain_id)(uintptr_t)ctx;
    cpr_post_state_switch( railId, (cpr_voltage_mode)settings->mode, settings->microvolts, CPR_UNKNOWN_FREQ );
}*/

//******************************************************************************
// Public API Functions
//******************************************************************************

void* cpr_image_malloc(uint32 size)
{
    void* buf = NULL;
    buf = Core_Malloc(size);
    if (buf == NULL)
        CPR_ASSERT(0);

    memset( buf, 0, size );
    return buf;
}

void cpr_image_free(void* buf)
{
    Core_Free( buf );
}

void cpr_image_memscpy(void* dst, const void* src, uint32 size)
{
    memscpy(dst, size, src, size);
}

void cpr_image_register_thermal_cb(cpr_therm_cb cb)
{

}

void cpr_image_register_isr(cpr_domain_id railId, uint32 interrupt, cpr_image_isr isr, void* ctx)
{

}

void cpr_image_enable_clock(const char* clkId)
{

}

uint32 cpr_image_get_chip_version()
{
    return DalChipInfo_ChipVersion();
}

cpr_foundry_id cpr_image_get_foundry()
{
    //TODO fix this when we have support
    /*DalChipInfoFoundryIdType foundryID = DalChipInfo_FoundryId();

    switch(foundryID)
    {
        case(DALCHIPINFO_FOUNDRYID_TSMC):
            return CPR_FOUNDRY_TSMC;
        case(DALCHIPINFO_FOUNDRYID_GF):
            return CPR_FOUNDRY_GF;
        case(DALCHIPINFO_FOUNDRYID_SS):
            return CPR_FOUNDRY_SS;
        case(DALCHIPINFO_FOUNDRYID_IBM):
            return CPR_FOUNDRY_IBM;
        case(DALCHIPINFO_FOUNDRYID_UMC):
            return CPR_FOUNDRY_UMC;
        default:
            break;
    }*/
    return CPR_FOUNDRY_TSMC;
    //CORE_VERIFY(0); // Chip foundry is not valid
    //return 0;       // Should never get here.
}

boolean cpr_image_set_rail_mode(cpr_domain_id railId, cpr_domain_info* info)
{
    railway_handle*  hdl    = get_railway_handle( railId );
    railway_corner   corner = cpr_image_target_cpr_mode_to_railway_corner(info->u.mode);
    railway_settings setting;

    if(!hdl->voter) {
        hdl->voter = railway_create_voter( hdl->railway_rail_id, RAILWAY_CPR_SETTLING_VOTER );
    }

    railway_corner_vote( hdl->voter, corner );
    railway_transition_rails();

    /*
     * Check if rail was transitioned to the requested corner
     */
    railway_get_current_settings(hdl->railway_rail_id, &setting);

    return (setting.mode == corner) ? true : false;
}

boolean cpr_image_rail_transition_voltage(cpr_domain_id railId)
{
    railway_transition_rails();

    return true;
}

void cpr_image_open_remote_cfg(void** cfg, uint32* size)
{
    *cfg = smem_get_addr( SMEM_CPR_CONFIG, size );

    if(!(*cfg))
    {
        *size = 0x3000;     //Reserve 12K for now
        *cfg = smem_alloc(SMEM_CPR_CONFIG, *size);
        
        if(!(*cfg))
        {
            /*
             * KW does not like CPR_LOG_FATAL() 
             */
            CPR_LOG_ERROR("Failed to get smem addr");
            CPR_ASSERT(0);
        }
    }

    /* Store smem address in msg_ram since there is no smem driver in AOP */
    msg_ram_set_smem_address((uint32)SMEM_CPR_CONFIG, (uint32)*cfg);
    CPR_LOG_TRACE("smem addr: %p (size: 0x%x)", *cfg, *size);
}

void cpr_image_close_remote_cfg()
{

}

void cpr_image_wait(uint32 us)
{
    DALSYS_BusyWait( us );
}

//Function to be called after SMEM is initialized to push out the CPR settings
//to SMEM. These settings are to be picked up by the RPM CPR driver during boot.
//Must be done before the RPM FW execution begins.
void cpr_externalize_state()
{
    CPR_LOG_INFO("Exporting CPR state");

    cpr_smem_serialize_config( cpr_utils_get_rail_state( CPR_RAIL_CX ),
            false );
    cpr_smem_serialize_config( cpr_utils_get_rail_state( CPR_RAIL_MX ),
            true );
    cpr_smem_serialize_config( cpr_utils_get_rail_state( CPR_RAIL_VDDA ),
            true );
    cpr_smem_serialize_settings( cpr_utils_get_rail_state( CPR_RAIL_MSS),
            true );
    cpr_smem_serialize_settings( cpr_utils_get_rail_state( CPR_RAIL_GFX),
            true );
    cpr_smem_serialize_config( cpr_utils_get_rail_state( CPR_RAIL_LPI_CX),
            true );
    cpr_smem_serialize_config( cpr_utils_get_rail_state( CPR_RAIL_LPI_MX),
            true );

    CPR_LOG_INFO("--- Done with CPR state export ---");
}

uint32 cpr_image_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    /* SBL/XBL does not need this */
    return 0;
}

void cpr_image_enter_sleep(void)
{
    /* SBL/XBL does not need this */
}

void cpr_image_exit_sleep(void)
{
    /* SBL/XBL does not need this */
}

boolean cpr_image_can_resume_control(cpr_domain_id railId)
{
    /* SBL/XBL does not need this */
    return true;
}

void cpr_image_enable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    cpr_image_target_enable_measurements(rail, type);
}

void cpr_image_disable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    cpr_image_target_disable_measurements(rail, type);
}

const cpr_settings* cpr_image_get_settings(void)
{
    return cpr_image_target_get_settings();
}

/*
 * Wrapper functions for Railway declared in cpr_image_api.h
 */
uint32 cpr_railway_get_voltage(int railwayRailId, railway_corner railwayCorner)
{
    cpr_domain_id domain = cpr_image_target_railway_rail_to_cpr_domain( railwayRailId );
    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {cpr_image_target_railway_corner_to_cpr_mode(railwayCorner)}};

    return cpr_get_voltage( domain, &info );
}

void cpr_railway_get_supported_corners(int railwayRailId, railway_corner *corners, uint32 *cornerCount )
{
    const cpr_voltage_mode* modes = NULL;

    cpr_get_supported_modes( cpr_image_target_railway_rail_to_cpr_domain(railwayRailId), &modes, cornerCount );

    for(uint32 i = 0; i < *cornerCount; i++)
    {
        corners[i] = cpr_image_target_cpr_mode_to_railway_corner( modes[i] );
    }
}

