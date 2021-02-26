/*============================================================================
  @file therm_target.c

  Target processor specific thermal code.

  Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/src/target/therm_target.c#2 $
$DateTime: 2017/08/07 11:42:50 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "therm_v.h"
#include "therm_npa_mitigate.h"
#include "therm_log.h"
#include "DALSys.h"
#include "npa.h"
#include "VCSDefs.h"
#include "voltage_level.h"

/*=======================================================================

                  STATIC MEMBER / FUNCTION DECLARATIONS / DEFINITIONS

========================================================================*/

/*=======================================================================

                  GLOBAL DEFINITIONS

========================================================================*/
/* List of all resources to be created for this target processor.
   Must defined for each target processor. */
const char *therm_monitor_res_avail[] =
{
  "/therm/mitigate/cpuv_restriction_cold",
};

static const char *vdd_restriction_res_avail[] =
{
  "/therm/mitigate/cpuv_restriction_cold",
  THERMAL_VCS_RES,
};

/* Array size of therm_monitor_res_avail[] to be used by common thermal
   SW. Must defined for each target processor. */
const unsigned int therm_monitor_res_count = ARR_SIZE(therm_monitor_res_avail);

/* Thermal task info struct */
extern qurt_thread_t thermal_tcb;

/* Allocate a event handle for your client. */
static npa_event_handle  vRestrictEventClient;
static npa_client_handle railClientHandle;

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/

/* Global define */

/* Set voltage restriction requests. */
static void handle_v_restrict_state(unsigned int state)
{
  switch (state)
  {
    case 0:
      /*Handle Normal*/
      therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "No %s voltage restriction.", THERMAL_VCS_RES);
      npa_cancel_request(railClientHandle);
      break;
    default:
    case 1:
      /* Disable Restrict to Nominal */
      therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "Restrict %s to nominal.", THERMAL_VCS_RES);
      npa_issue_scalar_request(railClientHandle, RAIL_VOLTAGE_LEVEL_NOM);
      break;
  }
}

/* Handle "/therm/mitigate/cpuv_restriction_cold" change events. */
void change_event_callback_handler(void         *context,
                                   unsigned int  event_type,
                                   void         *data,
                                   unsigned int  data_size )
{
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  do
  {
    if (data == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "Invalid data for cpuv_restriction_cold cb.");
      break;
    }

    event_data_ptr = (npa_event_data*)data;

    handle_v_restrict_state((unsigned int)event_data_ptr->state);
  } while (0);
}

/* Resource is available, so proceed to register for change events. */
static void init_npa_event_callback(void         *context,
                                    unsigned int  event_type,
                                    void         *data,
                                    unsigned int  data_size )
{
  npa_query_type query_result;

  railClientHandle = npa_create_sync_client(THERMAL_VCS_RES,
                                           "QDSP Restrict",
                                           VCS_NPA_CLIENT_ACTIVE_FLOOR);
  if (railClientHandle == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Cannot create client.");
    return;
  }

  vRestrictEventClient = npa_create_change_event_cb(
                                       "/therm/mitigate/cpuv_restriction_cold",
                                       "Event Name",
                                       change_event_callback_handler, NULL);
  if (vRestrictEventClient == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Cannot create change event cb cpuv_restriction_cold.");
    return;
  }

  memset(&query_result, 0x0, sizeof(npa_query_type));
  if (npa_query_by_event(vRestrictEventClient, NPA_QUERY_CURRENT_STATE,
                         &query_result) == NPA_QUERY_SUCCESS)
  {
    /* Get current state and apply it. */
    handle_v_restrict_state((unsigned int)query_result.data.state);
  }
}

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_target_init

  Initializes target specific thermal SW.

*/
void therm_target_init(void)
{

  DALSYS_InitMod(NULL);

  therm_log_init();

  therm_log_set_level(THERM_LOG_LEVEL_MAX);

  therm_npa_mitigate_init();

  npa_resources_available_cb(ARR_SIZE(vdd_restriction_res_avail),
                             vdd_restriction_res_avail,
                             init_npa_event_callback,
                             NULL);
}

/**
  @brief therm_target_after_task_start_init

  Initialize target specific thermal SW after thermal
  task has started.
*/
void therm_target_after_task_start_init(void)
{
  /* Intentionally left empty */
}

/**
  @brief therm_target_process_sigs

  Process rex signals for target specific SW.

  @param rcvdSigs
*/
void therm_target_process_sigs(unsigned int rcvdSigs,
                               qurt_signal_t *qurt_signal)
{
  /* Intentionally left empty */
}
