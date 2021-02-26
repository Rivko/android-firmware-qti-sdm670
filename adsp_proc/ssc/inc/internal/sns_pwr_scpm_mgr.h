#pragma once
/** ======================================================================================
  @file sns_pwr_scpm_mgr.h

  @brief Sensors Power SCPM Manager

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 ===================================================================================== **/


/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_isafe_list.h"
#include "sns_osa_thread.h"
#include "sns_rc.h"
#include "sns_time.h"

/**
*****************************************************************************************
                                  Constants & Macros
*****************************************************************************************
*/
/* Sampling period of 10 ms */
#define SNS_THREAD_UTIL_SAMPLING_PERIOD_NS 10000000ULL

/* High thread utilization threshold for bumping up MCPS request */
#define SNS_HIGH_THREAD_UTILIZATION_THRESHOLD 60

/* Low thread utilization threshold for bumping down MCPS request */
#define SNS_LOW_THREAD_UTILIZATION_THRESHOLD 10

/* Run length threshold for low thread utilization */
#define SNS_LOW_THREAD_UTILIZATION_RUN_LEN_THRESH 40

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_SVS 10

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_NOM 30

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_TURBO 40

/* MCPS vote time window of 1s */
#define SNS_MCPS_VOTE_TIME_WINDOW 1000000000ULL

/* Number of SSC Q6 HW threads */
#define NUM_HW_THREAD 2

#define SNS_Q6_MCPS_LOW_SVS 125

#define SNS_Q6_MCPS_SVS 250

#define SNS_Q6_MCPS_NOMINAL 550

#define SNS_Q6_MCPS_TURBO 700

#define SNS_Q6_MCPS_DEFAULT 0
/**
*****************************************************************************************
                                  Structures
*****************************************************************************************
*/
typedef struct
{
  sns_isafe_list_item list_entry;
  bool          require_speedup;
} sns_scpm_mgr_client;


/**
*****************************************************************************************
                                  Public Functions
*****************************************************************************************
*/

/**
 * Update the thread utilization for the provided thread
 * Check and update the SLPI MIPS vote based on the thread utilization update
 *
 * @param[i]    Thread pointer
 * @param[i]    Thread utilization in percentage
 *
 */
void
sns_pwr_scpm_mgr_update_thread_utilization(sns_osa_thread *thread, uint32_t thread_utilization);

/**
 * Initialize the sensors power scpm manager.
 * Create handles to the SCPM required to set SSC MIPS and BW requirements
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc
sns_pwr_scpm_mgr_init(void);

/**
 * Register scpm client
 */
sns_scpm_mgr_client * sns_pwr_scpm_mgr_create_client(void);

/**
 * Request an increase in MCPS
 */
sns_rc
sns_pwr_scpm_mgr_mcps_bump_up(sns_scpm_mgr_client *client);

/**
 * Request a decrease in MCPS
 */
sns_rc
sns_pwr_scpm_mgr_mcps_bump_down(sns_scpm_mgr_client *client);
/**
 * Request SCPM manager for absolute MCPS vote
 */
sns_rc
sns_pwr_scpm_mgr_absolute_mcps_vote(
  sns_scpm_mgr_client *client, uint32_t abs_mcps);
/**
 * Request to get Current active MCPS vote
 */
uint32_t
sns_pwr_scpm_mgr_get_current_mcps_vote();

