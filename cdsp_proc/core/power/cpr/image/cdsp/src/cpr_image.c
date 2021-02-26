/*===========================================================================

Copyright (c) 2014-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/
#include <stdlib.h>
#include <stdbool.h>
#include <stringl/stringl.h>

#include "cpr_image.h"
#include "cpr_image_target.h"
#include "cpr_cfg.h"
#include "cpr_data.h"
#include "cpr_utils.h"
#include "cpr.h"
#include "cpr_isr.h"
#include "cpr_mpss_hwio.h" /* for MSS_CLAMP_MEM register */

#include "smem.h"

#include "CoreVerify.h"
#include "CoreMutex.h"
#include "comdef.h"
#include "VCSDefs.h" /* Enums for the VCS power modes */
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DDIClock.h"
#include "DDIVCS.h"
#include "DDIInterruptController.h"
#include "ClockDefs.h"
#include "DDIChipInfo.h"
#include "busywait.h"
//#include "pmapp_pwr.h" //Revisit


#define CPR_NPA_RESOURCE_THERMAL        "/therm/mitigate/cpr_cold"
#define CPR_NPA_RESOURCE_VDD_MSS        "/vcs/vdd_mss"
#define CPR_MSS_NPA_RESOURCE            "/cpr/mss/uv"
#define CPR_TURING_LDO_NPA_RESOURCE     "/cpr/turingldo/uv"

#define CPR_VDD_MSS_CLIENT_NAME         "CPR_client"
#define CPR_THERMAL_EVT_HDL_NAME        "CPR_thermal_change_cb"

typedef struct
{
  npa_resource_definition resource;
  npa_node_definition     node;
} cpr_npa_mss_resource_type;

static npa_resource_state cpr_npa_driver_func(npa_resource *, npa_client_handle, npa_resource_state);
//******************************************************************************
//******************************************************************************
//
// Global Data
//
//******************************************************************************
//******************************************************************************

static cpr_therm_cb       therm_event_handler;
static DalDeviceHandle   *cpr_dal_hdl_clk;
//static DalDeviceHandle   *cpr_dal_hdl_vcs; //Revisit
static CoreMutex         *cpr_lock_hdl;
static npa_client_handle  cpr_npa_hdl_vddmss; /* used for rail mode request */
static DalDeviceHandle   *cpr_dal_hdl_int_ctrl;

static cpr_thermal_region cpr_current_therm_state = CPR_THERMAL_REGION_COOL;

static cpr_voltage_mode next_mode;

static cpr_npa_mss_resource_type cpr_npa_mss_resource =
{
    /* Resource */
    {
    CPR_MSS_NPA_RESOURCE,                 /* Name          */
    "uV",                                 /* Units         */
    NPA_MAX_STATE,                        /* Max Value     */
    &npa_no_client_plugin,                /* Plugin        */
    NPA_RESOURCE_DEFAULT,                 /* Attributes    */
    NULL,                                 /* User data     */
    NULL,                                 /* Query fn      */
    NULL                                  /* Query lnk fn  */
  },

  /* Node */
  {
    "/node" CPR_MSS_NPA_RESOURCE,            /* Name           */
    &cpr_npa_driver_func,                    /* Driver fn      */
    NPA_NODE_DEFAULT,                        /* Attributes     */
    NULL,                                    /* User data      */
    0,                                       /* Num Dependency */
    NULL,                                    /* Dependency     */
    1,                                       /* Num resources  */
    &cpr_npa_mss_resource.resource           /* Resource       */
  }
};

static cpr_npa_mss_resource_type cpr_npa_mss_ldo_resource =
{
    /* Resource */
    {
    CPR_TURING_LDO_NPA_RESOURCE,             /* Name          */
    "uV",                                 /* Units         */
    NPA_MAX_STATE,                        /* Max Value     */
    &npa_no_client_plugin,                /* Plugin        */
    NPA_RESOURCE_DEFAULT,                 /* Attributes    */
    NULL,                                 /* User data     */
    NULL,                                 /* Query fn      */
    NULL                                  /* Query lnk fn  */
  },

  /* Node */
  {
    "/node" CPR_TURING_LDO_NPA_RESOURCE,        /* Name           */
    &cpr_npa_driver_func,                    /* Driver fn      */
    NPA_NODE_DEFAULT,                        /* Attributes     */
    NULL,                                    /* User data      */
    0,                                       /* Num Dependency */
    NULL,                                    /* Dependency     */
    1,                                       /* Num resources  */
    &cpr_npa_mss_ldo_resource.resource       /* Resource       */
  }
};

//******************************************************************************
//******************************************************************************
//
// Local Helper Functions
//
//******************************************************************************
//******************************************************************************

/**
*
* <!-- cpr npa driver func -->
*
* @brief Driver function for CPR NPA resource.
*
*/
static npa_resource_state cpr_npa_driver_func(npa_resource *resource,
                                                  npa_client_handle client,
                                                  npa_resource_state state)
{
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    return state;
  }
  else
  {
    CPR_LOG_ERROR(" Received invalid vote on CPR NPA resource.");

    return (npa_resource_state)0;
  }
}


/**
*
* <!-- therm_change_event_cb -->
*
* @brief Listener for thermal events
*
*/
static void therm_change_event_cb(void *context,
                                  unsigned int event_type,
                                  void *data,
                                  unsigned int data_size)
{
    if(data != NULL)
    {
        cpr_thermal_region new_state = (cpr_thermal_region)((npa_event_data*)data)->state;

        CPR_LOG_TRACE("Therm change event: (OldState=%u) -> (NewState=%u)",
            cpr_current_therm_state,
            new_state);

        if(cpr_current_therm_state != new_state)
        {
            cpr_current_therm_state = new_state;

            therm_event_handler(new_state);
        }
    }
}

/**
*
* <!-- therm_resource_available_cb -->
*
* @brief Listener for thermal event availability
*
*/
static void therm_resource_available_cb(void *context,
                                        unsigned int event_type,
                                        void *data,
                                        unsigned int data_size)
{
    npa_event_handle  hdl;
    npa_query_type    query_result;

    CPR_LOG_INFO("Registering for thermal change cb");

    hdl = npa_create_change_event_cb(CPR_NPA_RESOURCE_THERMAL,
                                     CPR_THERMAL_EVT_HDL_NAME,
                                     therm_change_event_cb,
                                     NULL );

    if(NPA_QUERY_SUCCESS == npa_query_by_event(hdl, NPA_QUERY_CURRENT_STATE, &query_result))
    {
        therm_event_handler( (cpr_thermal_region)query_result.data.state );
    }
}

static boolean cpr_image_get_active_vcs_mode(cpr_voltage_mode *current_mode)
{
    npa_query_type query_result;
    boolean ret = false;

    if(!cpr_npa_hdl_vddmss)
    {
        /* Create vdd_mss client handle. */
        cpr_npa_hdl_vddmss = npa_create_sync_client(CPR_NPA_RESOURCE_VDD_MSS,
                                                    CPR_VDD_MSS_CLIENT_NAME,
                                                    NPA_CLIENT_REQUIRED);
    }

    if(NPA_QUERY_SUCCESS == npa_query_by_client(cpr_npa_hdl_vddmss, NPA_QUERY_CURRENT_STATE, &query_result))
    {
        ret = true;
        *current_mode = cpr_image_target_vlvl_corner_to_cpr_mode(query_result.data.state);
    }
    else
    {
        CPR_LOG_ERROR(" Failed to query %s", CPR_NPA_RESOURCE_VDD_MSS);
    }

    return ret;
}

//******************************************************************************
//******************************************************************************
//
// Image API Functions
//
//******************************************************************************
//******************************************************************************

/**
*
* <!-- cpr_image_register_thermal_cb -->
*
* NOTE: should be enabled after settling completed
*/
void cpr_image_register_thermal_cb(cpr_therm_cb cb)
{
    therm_event_handler = cb;

    CPR_LOG_INFO(" Registering for thermal resource available cb");

    npa_resource_available_cb("/therm/mitigate/cpr_cold",
                              therm_resource_available_cb,
                              NULL);
}

/**
*
* <!-- cpr_image_register_isr -->
*
* NOTE: should be enabled after settling completed.
*
*/
void cpr_image_register_isr(cpr_domain_id railId, uint32 interrupt, cpr_image_isr isr, void* ctx)
{
    if(!cpr_dal_hdl_int_ctrl)
    {
        // Get an interrupt controller handle
        CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &cpr_dal_hdl_int_ctrl));
    }

    // Register the CPR interrupt.
    CORE_DAL_VERIFY(DalInterruptController_RegisterISR(cpr_dal_hdl_int_ctrl,
                                                       interrupt,
                                                       (DALIRQ)isr,
                                                       (DALISRCtx)ctx,
                                                       DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER));
}

/**
*
* <!-- cpr_image_get_chip_version -->
*
* @brief Calls DalChipInfo_ChipVersion on the modem version of the CPR driver.
*
*/
uint32 cpr_image_get_chip_version(void)
{
    return (uint32)DalChipInfo_ChipVersion();
}

/**
*
* <!-- cpr_image_get_foundry -->
*
* @brief Get the foundry and return a foundry enum that our CPR driver understands.
*
*/
cpr_foundry_id cpr_image_get_foundry(void)
{
    return cpr_image_target_get_foundry();
}

/**
*
* <!-- cpr_image_enable_clock -->
*
* @brief Turn the CPR clock on.
*
*/
void cpr_image_enable_clock(const char* clkId)
{
    /*
     * TODO: Remove clock driver dependency to make settling work.
     */
    ClockIdType clockId;

    CPR_LOG_INFO(" Enable clock: %s", clkId);

    if(!cpr_dal_hdl_clk)
    {
        // Attach to Clock Driver
        CORE_DAL_VERIFY( DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &cpr_dal_hdl_clk));
    }

    // Enable the rbcpr bus clock.
    DalClock_GetClockId(cpr_dal_hdl_clk, clkId, &clockId);
    DalClock_EnableClock(cpr_dal_hdl_clk, clockId);

    // Take the clock out of reset state.
    DalClock_ResetClock(cpr_dal_hdl_clk, clockId, CLOCK_RESET_DEASSERT);
}

/**
*
* <!-- cpr_image_set_rail_mode -->
*
* @brief set the rail's current voltage mode to the specified mode and change the physical voltage on the rail
*
*
*/
boolean cpr_image_set_rail_mode(cpr_domain_id railId, cpr_domain_info* info)
{
    boolean ret = false;

    //only supporting MSS
    if (railId != CPR_RAIL_MSS)
    {
        return ret;
    }

    if(!cpr_npa_hdl_vddmss)
    {
        /* Create vdd_mss client handle. */
        cpr_npa_hdl_vddmss = npa_create_sync_client(CPR_NPA_RESOURCE_VDD_MSS,
                                                    CPR_VDD_MSS_CLIENT_NAME,
                                                    NPA_CLIENT_REQUIRED);
    }

    CPR_LOG_INFO(" Requesting rail mode switch to %u", info->u.mode);

    next_mode = info->u.mode;

    npa_issue_required_request(cpr_npa_hdl_vddmss, cpr_image_target_cpr_mode_to_vlvl_corner(info->u.mode));

    if(cpr_image_get_active_vcs_mode(&next_mode))
    {
        if(next_mode == info->u.mode)
        {
            CPR_LOG_INFO(" Rail mode switched to %u", next_mode);
            ret = true;
        }
        else
        {
            CPR_LOG_ERROR(" Failed to switch rail mode to %u (%u)", info->u.mode, next_mode);
        }
    }

    return ret;
}

/**
*
* <!-- cpr_image_rail_transition_voltage -->
*
* @brief change the physical voltage on the rail.
*
*/
boolean cpr_image_rail_transition_voltage(cpr_domain_id railId)
{
    boolean transition_completed = false;
    cpr_npa_rail_event_data_type cpr_npa_rail_event_data;

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {0}};

    //Change the physical voltage on the rail
    cpr_mode_settings* mode_setting = cpr_utils_get_active_mode_setting(railId);

    if(mode_setting)
    {
        info.u.mode = mode_setting->mode;
    }
    else
    {
        CPR_ASSERT( cpr_image_get_active_vcs_mode( &(info.u.mode) ) );
    }

    /*
     * Make sure current VCS corner is supported by CPR.
     */
    if(info.u.mode > CPR_VOLTAGE_MODE_RETENTION)
    {
        CPR_LOG_INFO(" Transition rail voltage: Start (mode: %u)", info.u.mode);

        cpr_npa_rail_event_data.voltage_uv = cpr_get_voltage(railId, &info);
        cpr_npa_rail_event_data.corner = cpr_image_target_cpr_mode_to_vlvl_corner(info.u.mode);
        cpr_npa_rail_event_data.ack = false;

        // Broadcast intra-corner voltage update recommendation event
        if (railId == CPR_RAIL_MSS)
        {
            npa_dispatch_custom_events(
              cpr_npa_mss_resource.resource.handle,
              (npa_event_type)CPR_NPA_RAIL_EVENT_INTRACORNER_VOLTAGE_UPDATE,
              &cpr_npa_rail_event_data);
        }
        else
        {
            npa_dispatch_custom_events(
              cpr_npa_mss_ldo_resource.resource.handle,
              (npa_event_type)CPR_NPA_RAIL_EVENT_INTRACORNER_VOLTAGE_UPDATE,
              &cpr_npa_rail_event_data);
        }

        if(cpr_npa_rail_event_data.ack == true)
        {
            transition_completed = true;

            CPR_LOG_INFO(" Transition rail voltage: Completed");
        }
        else
        {
            /*
             * Should not abort as VCS driver could be just in the process of corner/voltage transition
             */
            CPR_LOG_WARNING(" Transition rail voltage: Failed(No ACK from Client)");
        }
    }

    return transition_completed;
}

/**
*
* <!-- cpr_image_malloc -->
*
*/
void* cpr_image_malloc(uint32 size)
{
    void* buf = calloc(1, size);

    CPR_ASSERT(buf);

    return buf;
}

/**
*
* <!-- cpr_image_free -->
*
*/
void cpr_image_free(void* buf)
{
    free(buf);
}

/**
*
* <!-- cpr_image_memscpy -->
*
*/
void cpr_image_memscpy(void* dst, void* src, uint32 size)
{
    memscpy(dst, size, (const void  *)src, size);
}

/**
*
* <!-- cpr_image_open_remote_cfg -->
*
*/
void cpr_image_open_remote_cfg(void** cfg, uint32* size)
{
    *cfg = smem_get_addr( SMEM_CPR_CONFIG, size );
}

/**
*
* <!-- cpr_image_close_remote_cfg -->
*
*/
void cpr_image_close_remote_cfg(void)
{
}

/**
*
* <!-- cpr_image_wait -->
*
*/
void cpr_image_wait(uint32 us)
{
    busywait(us);
}

/**
*
* <!-- cpr_image_mutex_lock -->
*
*/
void cpr_image_mutex_lock(void)
{
    if(!cpr_lock_hdl)
    {
        cpr_lock_hdl = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);
        CPR_ASSERT(cpr_lock_hdl);
    }

    Core_MutexLock(cpr_lock_hdl);
}

/**
*
* <!-- cpr_image_mutex_unlock -->
*
*/
void cpr_image_mutex_unlock(void)
{
    if(cpr_lock_hdl)
    {
        Core_MutexUnlock(cpr_lock_hdl);
    }
}

/**
*
* <!-- cpr_image_get_eldo_voltage -->
*
*/
uint32 cpr_image_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    if(railId != CPR_RAIL_TURING_LDO)
    {
        CPR_LOG_FATAL("Unsupported rail for calculating eLDO voltage: 0x%x", railId);
    }

    return cpr_image_target_get_ldo_voltage(info);
}

/**
*
* <!-- cpr_image_mask_sensors -->
*
*/
void cpr_image_enter_sleep(void)
{
    CPR_LOG_INFO("Enter sleep --- NOT SUPPORTED!");
}

/**
*
* <!-- cpr_image_unmask_sensors -->
*
*/
void cpr_image_exit_sleep(void)
{
    CPR_LOG_INFO("Exit sleep: (mode:%u) --- NOT SUPPORTED!", next_mode);
}

boolean cpr_image_can_resume_control(cpr_domain_id railId)
{
    cpr_rail_state* rail_state = cpr_utils_get_rail_state(railId);

    return (rail_state->disableVotes == 0);
}

void cpr_image_enable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
#if 0
    /*
     * need to unclamp memory in order to enable sensors
     */
    HWIO_OUTF(MSS_CLAMP_MEM, UNCLAMP_ALL, 0x1);
    HWIO_OUTF(MSS_CLAMP_IO, UNCLAMP_ALL, 0x1);

    /*
     * Set the MSS rail to PWM.
     */
    if((type == CPR_MEASUREMENT_TYPE_AGING) && (rail->id == CPR_RAIL_MSS))
    {
        CPR_LOG_INFO("Set MSS rail to NPM mode");

      //  pmapp_pwr_vdd_mss_set_npm(TRUE); // will set mss to NPM mode //Revisit
    }
#endif
}

void cpr_image_disable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
#if 0
    /*
     * Set the MSS rail back to AUTO.
     */
    if((type == CPR_MEASUREMENT_TYPE_AGING) && (rail->id == CPR_RAIL_MSS))
    {
        CPR_LOG_INFO("Set MSS rail back to Default mode");

      //  pmapp_pwr_vdd_mss_set_npm(FALSE); // will set mss back to Default mode  //Revisit
    }

    /*
     * reset the clamp
     */
    HWIO_OUTF(MSS_CLAMP_MEM, UNCLAMP_ALL, 0x0);
    HWIO_OUTF(MSS_CLAMP_IO, UNCLAMP_ALL, 0x0);
#endif
}

const cpr_settings* cpr_image_get_settings(void)
{
    return cpr_image_target_get_settings();
}

/*==============================================================================
**
** Wrapper for OLD CPR APIs
** TODO: remove old CPR APIs 
**
**==============================================================================
*/


DALResult CPR_SetNextModeCorner ( cpr_domain_id eRail, rail_voltage_level eCorner )
{
    if ((eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO) || eCorner > RAIL_VOLTAGE_LEVEL_MAX )
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    CPR_IMAGE_LOCK_CORE();
    next_mode = cpr_image_target_vlvl_corner_to_cpr_mode(eCorner);

    CPR_LOG_TRACE("CPR_SetNextModeCorner: (rail:%u) (corner:%u)", eRail, eCorner);
    CPR_IMAGE_UNLOCK_CORE();

    return DAL_SUCCESS;
}


DALResult CPR_GetSupportedCorners( cpr_domain_id eRail, rail_voltage_level **eCorners, uint32 *uCornerCount )
{
    npa_resource_state initial_state;
    if (eCorners == NULL || uCornerCount == NULL)
    {
        return DAL_ERROR;
    }
    
    if (eRail == CPR_RAIL_MSS)
    {
        if(!cpr_npa_mss_resource.resource.handle)
        {
           // Create CPR NPA node for broadcasting intra-corner voltage recommendation events.
           initial_state = 0;
           npa_define_node(&cpr_npa_mss_resource.node, &initial_state, NULL);
        }
    }
    else if (eRail == CPR_RAIL_TURING_LDO)
    {
        if(!cpr_npa_mss_ldo_resource.resource.handle)
        {
            // Create CPR NPA node for broadcasting intra-corner voltage recommendation events.
            initial_state = 0;
            npa_define_node(&cpr_npa_mss_ldo_resource.node, &initial_state, NULL);
        }
    }
    else
    {
        return DAL_ERROR;
    }	

    const cpr_voltage_mode* modes = NULL;

    cpr_get_supported_modes( eRail, &modes, uCornerCount );

    *eCorners = cpr_image_malloc( *uCornerCount * sizeof(rail_voltage_level) );

    for(int i = 0; i < *uCornerCount; i++)
    {
        (*eCorners)[i] = cpr_image_target_cpr_mode_to_vlvl_corner( modes[i] );
    }

    CPR_LOG_TRACE("CPR_GetSupportedCorners: (rail:%u) (#corners:%u)", eRail, *uCornerCount);

    return DAL_SUCCESS;
}

DALResult CPR_GetRailVoltageRecommendation( cpr_domain_id eRail, rail_voltage_level eCorner, uint32 *nVoltageUV )
{
    if ((eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO) || eCorner > RAIL_VOLTAGE_LEVEL_MAX || nVoltageUV == NULL)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {cpr_image_target_vlvl_corner_to_cpr_mode(eCorner)}};

    *nVoltageUV = cpr_get_voltage(eRail, &info);

    CPR_LOG_TRACE("CPR_GetRailVoltageRecommendation: rail:%d (mode:%u) (voltage:%u)", eRail, info.u.mode, *nVoltageUV);
    return DAL_SUCCESS;
}

DALResult CprGetFloorAndCeiling( cpr_domain_id eRail, rail_voltage_level eCorner, uint32* floorVoltageUV, uint32* ceilingVoltageUV )
{
    if ((eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO) || eCorner > RAIL_VOLTAGE_LEVEL_MAX || floorVoltageUV == NULL || ceilingVoltageUV == NULL)
    {
        return DAL_ERROR;
    }

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {cpr_image_target_vlvl_corner_to_cpr_mode(eCorner)}};

    *ceilingVoltageUV = cpr_get_ceiling_voltage(eRail, &info);
    *floorVoltageUV   = cpr_get_floor_voltage(eRail, &info);

    CPR_LOG_TRACE("CprGetFloorAndCeiling: rail:%d (mode:%u) (floor:%u) (ceiling:%u)", eRail, info.u.mode, *floorVoltageUV, *ceilingVoltageUV);
    return DAL_SUCCESS;
}

DALResult  CPR_GetSafeVoltage(cpr_domain_id  eRail, rail_voltage_level eCorner, uint32 *nSafeVoltageuV)
{
    if ((eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO) || eCorner > RAIL_VOLTAGE_LEVEL_MAX || nSafeVoltageuV == NULL)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {cpr_image_target_vlvl_corner_to_cpr_mode(eCorner)}};

    *nSafeVoltageuV = cpr_get_safe_voltage(eRail, &info);

    CPR_LOG_TRACE("CPR_GetSafeVoltage: rail:%d (mode:%u) (voltage:%u)", eRail, info.u.mode, *nSafeVoltageuV);
    return DAL_SUCCESS;
}

DALResult CPR_GetEldoVoltageRecommendation(cpr_domain_id eRail, rail_voltage_level eCorner, uint32 *nVoltageUV)
{
    if ((eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO) || eCorner > RAIL_VOLTAGE_LEVEL_MAX || nVoltageUV == NULL)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {cpr_image_target_vlvl_corner_to_cpr_mode(eCorner)}};

    *nVoltageUV = cpr_get_eldo_voltage(eRail, &info);

    CPR_LOG_TRACE("CPR_GetEldoVoltageRecommendation: rail: (mode:%u) (voltage:%u)", eRail, info.u.mode, *nVoltageUV);

    return DAL_SUCCESS;
}

DALResult CPR_Enable( cpr_domain_id eRail )
{
    if (eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    if(!cpr_utils_is_closed_loop_mode(eRail))
    {
        return DAL_SUCCESS;
    }

    CPR_LOG_TRACE("CPR_Enable rail:%d (mode:%u)", eRail, next_mode);

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {next_mode}};

    cpr_post_state_switch(eRail, &info, cpr_get_voltage(eRail, &info));
    return DAL_SUCCESS;
}

DALResult CPR_Disable( cpr_domain_id eRail )
{
    if (eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    if(!cpr_utils_is_closed_loop_mode(eRail))
    {
        return DAL_SUCCESS;
    }

    CPR_LOG_TRACE("CPR_Disable rail: %d", eRail);

    cpr_pre_state_switch(eRail);
    return DAL_SUCCESS;
}

DALResult CPR_RelinquishedControl_Enter(cpr_domain_id  eRail)
{
    if (eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    if(!cpr_utils_is_closed_loop_mode(eRail))
    {
        return DAL_SUCCESS;
    }

    CPR_LOG_TRACE("CPR_RelinquishedControl_Enter for rail: %d", eRail);

    cpr_relinquish_control(eRail);
    return DAL_SUCCESS;
}

DALResult CPR_RelinquishedControl_Exit(cpr_domain_id  eRail)
{
    if (eRail != CPR_RAIL_MSS && eRail != CPR_RAIL_TURING_LDO)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    if(!cpr_utils_is_closed_loop_mode(eRail))
    {
        return DAL_SUCCESS;
    }

    CPR_LOG_TRACE("CPR_RelinquishedControl_Exit rail: %d (mode:%u)", eRail, next_mode);

    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {next_mode}};

    cpr_resume_control(eRail, &info, cpr_get_safe_voltage(eRail, &info));
    return DAL_SUCCESS;
}

DALResult CPR_MaskSensors(uint32 mask)
{
    CPR_LOG_INFO("CPR_MaskSensors --- NOT SUPPORTED!");
    return DAL_SUCCESS;
}

DALResult CPR_UnmaskSensors(uint32 mask)
{
    CPR_LOG_INFO("CPR_UnmaskSensors --- NOT SUPPORTED!");
    return DAL_SUCCESS;
}
