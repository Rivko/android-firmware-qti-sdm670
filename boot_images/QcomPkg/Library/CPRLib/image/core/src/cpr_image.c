/**
 * @file:  cpr_image.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/12/19 03:28:31 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/core/src/cpr_image.c#2 $
 * $Change: 15088669 $
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
#include "rpmh_client.h"
#include "cmd_db.h"
#include "voltage_level.h"


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIClock.h>
#include "gpu_clk_hwio.h"
//******************************************************************************
// Global Data
//******************************************************************************
static EFI_CLOCK_PROTOCOL *gpuClockProtocol = NULL;
static UINTN  gpuGfxClkPwrDomainId;
static rpmh_client_handle gfx_aging_handle;

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
    //return 0;       // Should never get here.
}
uint32 cpr_image_query_rail_hlvl(const char *res_name, rail_voltage_level vlvl)
{
	uint32 data_len;
	uint8 aux_len;
	uint16 *res_vlvls;
	size_t res_count;
	uint32 i;
	
	data_len = cmd_db_query_len (res_name);
	aux_len = (uint8)data_len;
	
	res_vlvls = (uint16 *) Core_Malloc(data_len);
    CORE_VERIFY (res_vlvls != NULL);
    res_count = data_len / sizeof(uint16);
	
	cmd_db_query_aux_data (res_name, &aux_len, (uint8 *)res_vlvls);
    CORE_VERIFY (data_len == (uint32)aux_len);
	
	//checking if vlvl is a valid for this rail.
	if ( ((int)vlvl < 0) )
	{
		return -1;
	}

	for (i = 0; i < res_count; i++)
	{
		if (res_vlvls[i] >= vlvl)
		{
		return i;
		}
		else if ((res_vlvls[i]) == 0 && (i != 0))
		{
		break;
		}
	}

  /* Input vlvl is higher than highest vlvl supported by this resource -
   * return the hlvl corresponding highest vlvl */
  return i-1;
}

boolean cpr_image_set_rail_mode(cpr_domain_id railId, cpr_domain_info* info)
{
	char *res_name = NULL;
	cpr_image_target_railway_rail_to_arc_domain(railId, &res_name );
	CORE_VERIFY_PTR(res_name);
	
	rail_voltage_level vlvl = cpr_image_target_cpr_mode_to_vlvl(info->u.mode);
	
	uint32 hlvl      = cpr_image_query_rail_hlvl (res_name,  vlvl);
	
	//checking if hlvl is valid
	if(hlvl < 0 || hlvl > 15)
	{
		CPR_LOG_ERROR("Invalid HLVL");
		CPR_ASSERT(0);
	}
	
	uint32 arc_gfx_addr = cmd_db_query_addr(res_name);
	
	if (gfx_aging_handle == NULL)
	{
		gfx_aging_handle = rpmh_create_handle(RSC_DRV_HLOS, "GPU_AGING");
		CORE_VERIFY_PTR(gfx_aging_handle);
	}
		
	if (info->u.mode == CPR_VOLTAGE_MODE_OFF)
	{
		//disable gpu power domain first and turn off rail
		if (gpuClockProtocol->DisableClockPowerDomain(gpuClockProtocol, gpuGfxClkPwrDomainId) != EFI_SUCCESS) 
		{
			CPR_LOG_ERROR("Failed to Disable GPU ClockPowerDomain");
			CPR_ASSERT(0);
		}
			HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_CLAMP_IO ,0x1);
			
		uint32 gfx_v_req_id = rpmh_issue_command(gfx_aging_handle, RPMH_SET_ACTIVE, 1, arc_gfx_addr, hlvl);
		rpmh_barrier_single (gfx_aging_handle, gfx_v_req_id);
		//done with using aging handle
		rpmh_delete_handle(gfx_aging_handle);
	}
	else
	{
		HWIO_OUTF(GPUCC_GPU_CC_GX_BCR, BLK_ARES, 0x1);
		gBS->Stall(2);
		HWIO_OUTF(GPUCC_GPU_CC_GX_BCR, BLK_ARES, 0x0);
		gBS->Stall(2);
		HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_RESET,0x1);
		gBS->Stall(2);
		HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_RESET,0x0);
		HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_CLAMP_IO ,0x0);
		gBS->Stall(2);
		
		//turn on rail first and enable gpu power domain 
		uint32 gfx_v_req_id = rpmh_issue_command(gfx_aging_handle, RPMH_SET_ACTIVE, 1, arc_gfx_addr, hlvl);
		rpmh_barrier_single (gfx_aging_handle, gfx_v_req_id);
		
		if (gpuClockProtocol->EnableClockPowerDomain(gpuClockProtocol, gpuGfxClkPwrDomainId) != EFI_SUCCESS) 
		{
			CPR_LOG_ERROR("Failed to Enable GPU ClockPowerDomain");
			CPR_ASSERT(0);
		}
	}
	
    return true;
}

boolean cpr_image_rail_transition_voltage(cpr_domain_id railId)
{
    //Not needed for Core.
    return true;
}

void cpr_image_open_remote_cfg(void** cfg, uint32* size)
{
    *cfg = smem_get_addr( SMEM_CPR_CONFIG, size );

    if(!(*cfg))
    {
        /*
         * KW does not like CPR_LOG_FATAL() 
         */
        CPR_LOG_ERROR("Failed to get smem addr");
        CPR_ASSERT(0);
    }
}

void cpr_image_close_remote_cfg()
{
    //Not needed for Core.
}

void cpr_image_wait(uint32 us)
{
    DALSYS_BusyWait( us );
}

//Function to be called after SMEM is initialized to push out the CPR settings
//to SMEM. 
void cpr_externalize_state()
{
    //Not needed for Core.
}

uint32 cpr_image_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    //Not needed for Core.
    return 0;
}

void cpr_image_enter_sleep(void)
{
    //Not needed for Core.
}

void cpr_image_exit_sleep(void)
{
    //Not needed for Core.
}

boolean cpr_image_can_resume_control(cpr_domain_id railId)
{
    //Not needed for Core.
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
    //Not yet supported for core.
    return 0;
}

void cpr_railway_get_supported_corners(int railwayRailId, railway_corner *corners, uint32 *cornerCount )
{
    //Not yet supported for core.
}

void cpr_get_gpu_power_domain_info(void)
{
	if (gpuClockProtocol==NULL)
  {
	//get global clock protocol info
	gBS->LocateProtocol(&gEfiClockProtocolGuid, 
						NULL,
						(VOID **)&gpuClockProtocol );
  }

  CORE_VERIFY_PTR(gpuClockProtocol);

  //Get power domain info for GPU gx_gdsc
  if (gpuClockProtocol->GetClockPowerDomainID(	gpuClockProtocol, 
												"gx_gdsc", 
												&gpuGfxClkPwrDomainId) != EFI_SUCCESS) 
	{
		CPR_LOG_ERROR("Failed to get GPU ClockPowerDomainID");
		CPR_ASSERT(0);
	}

}