/**
  @file tms_utils_clock.c
  @brief DDR & Q6 clock voting api's for TMS uses.

  This file contains API's for DDR & Q6 clock vote/unvote.
*/
/*
  ===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtools/utils/src/tms_utils_clock.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "tms_utils_msg.h"
#include "tms_dll_api.h"
#include "icbarb.h"
#include "npa.h"

typedef struct
{
  npa_client_handle hNPA_CPU; // NPA client handle for CPU clock vote

  npa_client_handle hNPA_DDR; // NPA client handle for DDR clock vote
  
} tms_utils_vote_internal_t;

static tms_utils_vote_internal_t tms_utils_vote_internal;

/** tmsDDRMasterSlave
 * Master-Slave pair for DDR vote (defined in scons).
 */
ICBArb_MasterSlaveType tmsDDRMasterSlave[] =
{ 
  { TMS_UTILS_VOTE_DDR_MSTER, TMS_UTILS_VOTE_DDR_SLAVE }
};

/** tmsDDRRequestType
 * Type of arbitration
 * Instantaneous bandwidth (IB) in Bytes/second
 * Arbitrated bandwidth (AB) in Bytes/second
 * Latency in nanoseconds
 */
ICBArb_RequestType tmsDDRRequestType[] = 
{
  /* Type=3, IB=<from_scons>, AB=0, Latency=0 */
  { ICBARB_REQUEST_TYPE_3, { TMS_UTILS_VOTE_IB_VALUE, 0, 0 } }
};

/** tms_utils_vote_cpu_cb
 * CPU resource availability callback.
 */
static void tms_utils_vote_cpu_cb( void *context, unsigned int event_type,
                                            void *data, unsigned int data_size)
{
  tms_utils_vote_internal.hNPA_CPU = 
    npa_create_sync_client( (const char *)data, "tmsCPUVote", 
                            NPA_CLIENT_REQUIRED);
  if (NULL == tms_utils_vote_internal.hNPA_CPU)
    TMS_MSG_ERROR("NPA client create for CPU failed.");
  else
    TMS_MSG_HIGH("NPA client create for CPU succeed.");
}

/** tms_utils_vote_ddr_cb
 * DDR resource availability callback.
 */
static void tms_utils_vote_ddr_cb( void *context, unsigned int event_type,
                                            void *data, unsigned int data_size)
{
  tms_utils_vote_internal.hNPA_DDR = 
    npa_create_sync_client_ex( (const char *)data, "tmsDDRVote", 
                               NPA_CLIENT_VECTOR, 
                               sizeof(tmsDDRMasterSlave), &tmsDDRMasterSlave);
  if (NULL == tms_utils_vote_internal.hNPA_DDR)
    TMS_MSG_ERROR("NPA client create for DDR failed.");
  else
    TMS_MSG_HIGH("NPA client create for DDR succeed.");
}

/** tms_utils_vote_init
 * Init api for voting related initialization.
 */
DLL_API_GLOBAL void tms_utils_vote_init( void )
{
  npa_resource_available_cb("/clk/cpu", tms_utils_vote_cpu_cb, NULL);

  npa_resource_available_cb("/icb/arbiter", tms_utils_vote_ddr_cb, NULL);
}

/** tms_utils_vote_cpu
 * CPU vote.
 */
static void tms_utils_vote_cpu( void )
{
  if(tms_utils_vote_internal.hNPA_CPU)
  {
    npa_issue_scalar_request(tms_utils_vote_internal.hNPA_CPU, NPA_MAX_STATE);
    TMS_MSG_HIGH("CPU vote");
  }
  else
  {
    TMS_MSG_HIGH("CPU vote not done");
  }
}

/** tms_utils_unvote_cpu
 * CPU unvote.
 */
static void tms_utils_unvote_cpu( void )
{
  if(tms_utils_vote_internal.hNPA_CPU)
  {
    npa_complete_request(tms_utils_vote_internal.hNPA_CPU);
    TMS_MSG_HIGH("CPU unvote");
  }
  else
  {
    TMS_MSG_HIGH("CPU unvote not done");
  }
}

/** tms_utils_vote_ddr
 * DDR vote.
 */
static void tms_utils_vote_ddr( void )
{
  if(tms_utils_vote_internal.hNPA_DDR)
  {
    npa_issue_vector_request( tms_utils_vote_internal.hNPA_DDR,
                          sizeof(tmsDDRRequestType)/sizeof(npa_resource_state),
                          (npa_resource_state *)tmsDDRRequestType);
    TMS_MSG_HIGH("DDR vote");
  }
  else
  {
    TMS_MSG_HIGH("DDR vote not done");
  }
}

/** tms_utils_unvote_ddr
 * DDR unvote.
 */
static void tms_utils_unvote_ddr( void )
{
  if(tms_utils_vote_internal.hNPA_DDR)
  {
    npa_complete_request(tms_utils_vote_internal.hNPA_DDR);
    TMS_MSG_HIGH("DDR unvote");
  }
  else
  {
    TMS_MSG_HIGH("DDR unvote not done");
  }
}

/** tms_utils_vote_cpu_ddr
 * Single api for both cpu & ddr vote.
 */
void tms_utils_vote_cpu_ddr( void )
{
  tms_utils_vote_cpu();
  tms_utils_vote_ddr();
}

/** tms_utils_unvote_cpu_ddr
 * Single api for both cpu & ddr unvote.
 */
void tms_utils_unvote_cpu_ddr( void )
{
  tms_utils_unvote_cpu();
  tms_utils_unvote_ddr();
}

