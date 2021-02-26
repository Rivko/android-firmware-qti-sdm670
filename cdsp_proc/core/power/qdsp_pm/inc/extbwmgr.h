/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: extbwmgr.h
@brief: Header for ADSPPM External BW Manager.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/extbwmgr.h#1 $
*/

#ifndef _EXTBWMGR_H_
#define _EXTBWMGR_H_

#include "adsppm.h"

/**
 * @fn ExtBwMgr_Init
 * @brief Initialize the External BW Manager
 *        Requires that the DCVS Manager is already initialized
 */
Adsppm_Status ExtBwMgr_Init(void);

/**
 * @fn ExtBwMgr_IssueExtBwRequest
 * @brief Aggregates current external BW values with requests from SysMon DCVS
 */
Adsppm_Status ExtBwMgr_IssueExtBwRequest(uint32 numUpdatedExtBwVotes,
    AdsppmBusBWRequestValueType* pUpdatedExtBusBwValues);

/**
 * @fn ExtBwMgr_GetInfo
 * @brief Get DCVS ADSP to DDR BW aggregation information for testing purposes
 */
Adsppm_Status ExtBwMgr_GetInfo(AdsppmInfoDcvsAdspDdrBwType* pDcvsAdspDdrBwInfo,
                               Adsppm_DCVS_Monitored_Routes route );
/**
 * @fn ExtBwMgr_UpdateAdsptoDDrClientFloorVotes
 * @brief Update clients DDR floor votes when floor aggregation changes
 */    

void ExtBwMgr_UpdateAdsptoDDrClientFloorVotes( 
    AdsppmBusBWDataIbAbType* pNewClientsFloorVote,
    Adsppm_DCVS_Monitored_Routes Route);


#endif /* _EXTBWMGR_H_ */

