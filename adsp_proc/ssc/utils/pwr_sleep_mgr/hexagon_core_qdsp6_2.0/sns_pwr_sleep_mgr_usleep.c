/**
 * sns_pwr_sleep_mgr_usleep.c
 *
 * Usleep control for SDM845
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/utils/pwr_sleep_mgr/hexagon_core_qdsp6_2.0/sns_pwr_sleep_mgr_usleep.c#2 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
 *
 **/

#include "sns_assert.h"
#include "sns_island.h"
#include "sns_types.h"
#include "uSleep_npa.h"
#include "unpa_remote.h"


SNS_SECTION(".text.sns") void 
sns_pwr_sleep_mgr_disable_lpm_plus(int32_t unpa_client)
{
  unpaRemote_issueRequest(unpa_client, USLEEP_CPUVDD_LPR_DISABLE_LPM_PLUS);
}

SNS_SECTION(".text.sns") void 
sns_pwr_sleep_mgr_enable_lpm_plus(int32_t unpa_client)
{
  unpaRemote_issueRequest(unpa_client, USLEEP_CPUVDD_LPR_ENABLE_ALL);
}

int32_t
sns_pwr_sleep_mgr_create_unpa_handle(void)
{
  unpa_remote_client unpa_client = NULL;

  unpa_client = unpaRemote_createClient("SNS_PM_USLEEPVDD",
                                       UNPA_CLIENT_REQUIRED,
                                       USLEEP_CPUVDD_LPR_NODE_NAME);
  SNS_ASSERT(NULL != unpa_client);
  
  return unpa_client;
}
