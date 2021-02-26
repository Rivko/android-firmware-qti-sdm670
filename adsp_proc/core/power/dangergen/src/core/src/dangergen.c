/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include <stdlib.h>
#include "dangergen.h"
#include "dangergen_internal.h"
#include "dangergen_inlines.h"
#include "dangergen_hwio.h"
#include "dangergen_qdi.h"
#include "ULogFront.h"
#include "DALStdErr.h"
#include "CoreTime.h"
#include "DDIClock.h"
#include "interrupt_config.h"
#include "sysmon_qos_clock.h"
#include "dangergen_config.h"


// Forward function declarations
static dangergen_generator_type* dangergen_allocate(dangergen_driver_type* s);
static dangergen_result_type dangergen_deallocate(dangergen_driver_type* s,
    dangergen_generator_type* gen);
static void dangergen_qos_enable_vote(dangergen_driver_type* s, bool addVote);

static dangergen_result_type dangergern_get_interruptid (dangergen_watchdog_portid port_id, uint32 *interrupt_id);

// Static context structure for this driver
static dangergen_driver_type gDangerDriver;

dangergen_result_type dangergen_driver_init(void)
{

    dangergen_driver_type* self = &gDangerDriver;
    uint32 i;

    memset((void*)self, 0, sizeof(dangergen_driver_type));

    // Create lock
    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock,
        NULL))
    {
        return DANGERGEN_ERROR;
    }

    // Create log
    if(DAL_SUCCESS != ULogFront_RealTimeInit(
        &self->log,
        "Danger Driver Log",
        DANGERGEN_ULOG_SIZE,
        ULOG_MEMORY_LOCAL,
        ULOG_LOCK_NONE))
    {
        return DANGERGEN_ERROR;
    }

    // Open clock driver handle
    if(DAL_SUCCESS != DAL_ClockDeviceAttach(DALDEVICEID_CLOCK,
        &self->clock_data.driver_handle))
    {
        dangergen_log0(self->log, "failed to open clock driver handle");
        return DANGERGEN_ERROR;
    }

    if(DAL_SUCCESS !=
        DalClock_GetClockId(self->clock_data.driver_handle,
        DANGERGEN_CLOCK_NAME, &self->clock_data.clock_id))
    {
        dangergen_log1(self->log,
            "failed to get clock id for \"%s\" from clock driver",
            DANGERGEN_CLOCK_NAME);
        return DANGERGEN_ERROR;
    }

    // Initialize client handle pointers
    for(i = 0; i < DANGERGEN_ARRAY_SIZE(self->clients); i++)
        self->clients[i] = NULL;

    // Initialize state of generators
    for(i = 0; i < DANGERGEN_ARRAY_SIZE(self->generators); i++)
    {
        dangergen_generator_type* gen = &self->generators[i];
        gen->index = i;
        gen->allocated = false;
        gen->period_ns = DANGERGEN_UNSPECIFIED_TIME;
        gen->threshold1_ns = DANGERGEN_UNSPECIFIED_TIME;
        gen->threshold2_ns = DANGERGEN_UNSPECIFIED_TIME;
        gen->threshold3_ns = DANGERGEN_UNSPECIFIED_TIME;
        gen->window_ns = DANGERGEN_UNSPECIFIED_TIME;
    }

    // Initialize HWIO layer
    dangergen_hwio_initialize_module();

    // Initialize state of HW
    dangergen_qos_enable_vote(self, TRUE);
    for(i = 0; i < DANGERGEN_ARRAY_SIZE(self->generators); i++)
    {
        dangergen_hwio_initialize(&self->generators[i]);
    }
    self->interrupt_selection =  dangergen_hwio_interrupt_select_mode_status (MAJOR_VERSION_INTERRUPT_SELECTION);
    dangergen_qos_enable_vote(self, FALSE);

    // Initialize QDI module
    if(!dangergen_is_successful(dangergen_qdi_init(self)))
    {
        dangergen_log0(self->log, "danger generator QDI initialization failed");
        return DANGERGEN_ERROR;
    }

    dangergen_log0(self->log, "danger generator driver initialized");

    self->initialized = TRUE;

    

    return DANGERGEN_SUCCESS;

}


dangergen_result_type dangergen_open(dangergen_open_params_type* params,
    dangergen_handle_type* h)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_result_type result = DANGERGEN_ERROR;
    dangergen_generator_type* generator;

    if(params == NULL || h == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    // Only counter-based danger generation is supported
    if(params->method != DANGERGEN_METHOD_WATCHDOG)
    {
        dangergen_log1(self->log, "unsupported danger generator method: %d",
            params->method);
        return DANGERGEN_ERROR_UNSUPPORTED;
    }

    dangergen_lock(self->lock);

    generator = dangergen_allocate(self);
    if(generator != NULL)
    {
        dangergen_client_handle_type* client =
            malloc(sizeof(dangergen_client_handle_type));
        if(client != NULL)
        {
            // Set up the new client
            client->username[0] = '\n';
            if(params->username != NULL)
            {
                strlcpy(client->username, params->username,
                    DANGERGEN_MAX_USERNAME_LEN);
            }
            client->state = DANGERGEN_STATE_UNCONFIGURED;
            client->hw = generator;
            client->refresh_count = 0;
            client->last_refresh_timestamp = 0;
            client->mode = DANGERGEN_ENABLE_MODE_SW;

            // Enable required clock(s)
            if(DAL_SUCCESS !=
                DalClock_EnableClock(self->clock_data.driver_handle,
                self->clock_data.clock_id))
            {
                free(client);
                dangergen_log1(self->log, "cannot enable clock (id %d)",
                    self->clock_data.clock_id);
            }
            else
            {
                // Initialize HW configuration
                dangergen_qos_enable_vote(self, true);
                dangergen_hwio_initialize(generator);

                self->active_clients += 1;
                self->clients[client->hw->index] = client;
                *h = (dangergen_handle_type)client; // set user's handle
                dangergen_log4(self->log,
                    "open successful: handle = 0x%x, username = \"%s\", "
                    "generator index = %d, active clients = %d",
                    client, client->username, client->hw->index,
                    self->active_clients);
                result = DANGERGEN_SUCCESS;
            }
        }
    }

    dangergen_unlock(self->lock);

    return result;

}

dangergen_result_type dangergen_close(dangergen_handle_type h)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;
    dangergen_result_type result = DANGERGEN_ERROR;
    DALResult dal_result;

    if(client == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    dangergen_lock(self->lock);

    // Disable clock(s).  The below API should implement reference counting
    // so the clock will not be disabled until there are no references.
    dal_result = DalClock_DisableClock(self->clock_data.driver_handle,
        self->clock_data.clock_id);
    if(DAL_SUCCESS != dal_result)
    {
        dangergen_log1(self->log, "cannot disable clock (id %d)",
            self->clock_data.clock_id);
    }

    if((DAL_SUCCESS == dal_result) &&
        dangergen_is_successful(dangergen_deallocate(self, client->hw)))
    {
        self->active_clients -= 1;
        self->clients[client->hw->index] = NULL;
        dangergen_log2(self->log,
            "close successful: handle = 0x%x, username = \"%s\"",
            client, client->username);
        free(client);
        dangergen_qos_enable_vote(self, false);
        result = DANGERGEN_SUCCESS;
    }
    else
    {
        // Consider recovery here if dangergen_deallocate fails
        // (re-enable clocks?)
    }

    dangergen_unlock(self->lock);

    return result;

}

dangergen_result_type dangergen_configure_watchdog(dangergen_handle_type h,
    dangergen_watchdog_params_type* params)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;
    dangergen_generator_type* generator = NULL;
    uint32 interrupt_id = 0;

    if(params == NULL || h == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    if(client->state == DANGERGEN_STATE_ENABLED)
    {
        // Can't reconfigure when already enabled
        dangergen_log2(self->log,
            "can't configure watchdog: invalid state, current state = %d, "
            "client = 0x%x", client->state, client);
        return DANGERGEN_ERROR_INVALID_STATE;
    }

    if((params->threshold1_ns == DANGERGEN_UNSPECIFIED_TIME) ||
       (params->period_ns == DANGERGEN_UNSPECIFIED_TIME))
    {
        // User must specify the period and first threshold at least
        dangergen_log0(self->log,
            "can't configure watchdog: insufficient configuration parameters");
        return DANGERGEN_ERROR_BADPARAM;
    }

    // Copy HW configuration parameters
    generator = client->hw;
    generator->period_ns = params->period_ns;
    generator->threshold1_ns = params->threshold1_ns;
    generator->threshold2_ns = params->threshold2_ns;
    generator->threshold3_ns = params->threshold3_ns;
    generator->window_ns = params->window_ns;
    generator->port_id = params->port_id;

    // TODO: automatically set threshold2, threshold3, and window
    // if they were not set by the user

    // Update HW configuration
    dangergen_hwio_configure(generator);

    if( (self->interrupt_selection == TRUE) && 
        (generator->port_id > DANGERGEN_PORTID_UNKNOWN) &&
        (generator->port_id < DANGERGEN_PORTID_MAX))
      {
         if(DANGERGEN_SUCCESS == dangergern_get_interruptid (generator->port_id, &interrupt_id))
            {
                dangergen_log2(self->log,
                               "Configuring interrupt: port id= %d, interrupt id = %d",
                               generator->port_id, interrupt_id);
                generator->interrupt_id = interrupt_id;
                dangergen_hwio_interrupt_configure(generator);
            }
         else
            {
               dangergen_log0(self->log,
                 "can't configure interrupt id: incorrect port id");
               return DANGERGEN_ERROR_BADPARAM;

            }
      }

    client->state = DANGERGEN_STATE_DISABLED;

    return DANGERGEN_SUCCESS;

}

dangergen_result_type dangergen_switch_mode(dangergen_handle_type h, dangergen_generator_enable_mode mode)
{
  dangergen_driver_type* self = &gDangerDriver;
  dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;
  
  if(client == NULL)
  {
      return DANGERGEN_ERROR_BADPARAM;
  }
  
  if((client->state != DANGERGEN_STATE_DISABLED) || 
     (dangergen_hwio_hw_enable_mode_status (MAJOR_VERSION_HW_ENABLE_MODE) == FALSE))
  {
      dangergen_log2(self->log,
          "switch failed: invalid state, current state = %d, client = 0x%x",
          client->state, client);
      return DANGERGEN_ERROR_INVALID_STATE;
  }

  if (mode >= DANGERGEN_ENABLE_MODE_MAX)
  {
      dangergen_log1(self->log,
                     "Invalid Mode passed = %d; setting SW mode", mode);
      mode = DANGERGEN_ENABLE_MODE_SW;
  }
  
  dangergen_log0(self->log,
                 "Programming HW mode");

  dangergen_hwio_switch_mode(client->hw,mode);
  dangergen_hwio_enable(client->hw);
 
  client->state = DANGERGEN_STATE_ENABLED;
  client->mode  = mode;
  
  return DANGERGEN_SUCCESS;


}

dangergen_result_type dangergen_enable(dangergen_handle_type h)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;

    if(client == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    if((client->state != DANGERGEN_STATE_DISABLED) ||
       (client->mode  == DANGERGEN_ENABLE_MODE_HW))
    {
        dangergen_log2(self->log,
            "enable failed: invalid state, current state = %d, client = 0x%x",
            client->state, client);
        return DANGERGEN_ERROR_INVALID_STATE;
    }

        dangergen_hwio_enable(client->hw);

    client->state = DANGERGEN_STATE_ENABLED;

    return DANGERGEN_SUCCESS;

}

dangergen_result_type dangergen_disable(dangergen_handle_type h)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;

    if(client == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    if(client->state != DANGERGEN_STATE_ENABLED)
    {
        dangergen_log2(self->log,
            "disable failed: invalid state, current state = %d, client = 0x%x",
            client->state, client);
        return DANGERGEN_ERROR_INVALID_STATE;
    }

    if (dangergen_hwio_hw_enable_mode_status (MAJOR_VERSION_HW_ENABLE_MODE) == TRUE)
    {
        dangergen_hwio_switch_mode(client->hw,DANGERGEN_ENABLE_MODE_SW);
        client->mode  = DANGERGEN_ENABLE_MODE_SW;
    }

    dangergen_hwio_disable(client->hw);
    client->state = DANGERGEN_STATE_DISABLED;

    return DANGERGEN_SUCCESS;

}

dangergen_result_type dangergen_refresh(dangergen_handle_type h)
{

    dangergen_driver_type* self = &gDangerDriver;
    dangergen_client_handle_type* client = (dangergen_client_handle_type*)h;

    if(client == NULL)
    {
        return DANGERGEN_ERROR_BADPARAM;
    }

    if((client->state != DANGERGEN_STATE_ENABLED) ||
       (client->mode  != DANGERGEN_ENABLE_MODE_SW))
    {
        // Refresh request is valid only in the enabled state
        dangergen_log2(self->log,
            "refresh failed: invalid state, current state = %d, client = 0x%x",
            client->state, client);
        return DANGERGEN_ERROR_INVALID_STATE;
    }

    dangergen_hwio_subtract(client->hw);
    client->refresh_count += 1;
    client->last_refresh_timestamp = CoreTimetick_Get64();

    return DANGERGEN_SUCCESS;

}

static dangergen_generator_type* dangergen_allocate(dangergen_driver_type* self)
{

    dangergen_generator_type* allocated_gen = NULL;
    uint32 i;

    if(self->active_clients < DANGERGEN_MAX_CLIENTS)
    {
        for(i = 0; i < DANGERGEN_ARRAY_SIZE(self->generators); i++)
        {
            dangergen_generator_type* gen = &self->generators[i];
            if(!gen->allocated)
            {
                gen->allocated = true;
                allocated_gen = gen;
                break;
            }
        }
    }

    if(allocated_gen)
    {
        dangergen_log1(self->log,
            "allocated danger generator %d", allocated_gen->index);
    }
    else
    {
        dangergen_log0(self->log, "failed to allocate danger generator");
    }

    return allocated_gen;

}

static dangergen_result_type dangergen_deallocate(dangergen_driver_type* self,
    dangergen_generator_type* gen)
{

    dangergen_result_type result = DANGERGEN_ERROR;

    if(gen->allocated)
    {
        gen->allocated = false;
        dangergen_log1(self->log,
            "de-allocated danger generator %d", gen->index);
        result = DANGERGEN_SUCCESS;
    }
    else
    {
        dangergen_log1(self->log,
            "failed to de-allocate danger generator %d - already de-allocated",
            gen->index);
    }

    return result;

}

// Vote for qos_enable is required before any registers are touched
static void dangergen_qos_enable_vote(dangergen_driver_type* self, bool addVote)
{
    if(addVote)
    {
        self->qos_enable_count += 1;
        if(self->qos_enable_count == 1)
        {
            dangergen_log0(self->log,
                "setting lpass_tcsr_qos_ctl.qos_enable and "
                "lpass_qos_noc_cgc.danger to 1");
            if(sysmon_qos_clock_enable_clientid(SYSMON_QOS_CLIENT_ID_QDSPPM, 1) != 0)
			   dangergen_hwio_set_qos_ctl(TRUE);
               dangergen_hwio_set_qos_noc(TRUE);
        }
    }
    else // remove vote
    {
        if(self->qos_enable_count > 0)
        {
            self->qos_enable_count -= 1;
            if(self->qos_enable_count == 0)
            {
                dangergen_log0(self->log,
                    "setting lpass_tcsr_qos_ctl.qos_enable and "
                    "lpass_qos_noc_cgc.danger to 0");
				dangergen_hwio_set_qos_noc(FALSE);

                if(sysmon_qos_clock_enable_clientid(SYSMON_QOS_CLIENT_ID_QDSPPM, 0) != 0)
                    dangergen_hwio_set_qos_ctl(FALSE);
            }
        }
    }
}


dangergen_result_type dangergern_get_interruptid (dangergen_watchdog_portid port_id, uint32 *interrupt_id)
{
   uint32 status = DANGERGEN_SUCCESS;

   if(port_id > DANGERGEN_PORTID_UNKNOWN && 
      port_id < DANGERGEN_PORTID_MAX &&
      interrupt_id != NULL)
    {
       *interrupt_id = danger_portid_interruptid_map_array[port_id].interrupt_id;
    }
   else
    {
       status = DANGERGEN_ERROR_BADPARAM;
    }

   return status;
}
