/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * @file sysmonmgr.h
 * @brief header files for sysmon interaction
 *
 *  Created on: 
 *      Author: 
 */

#ifndef SYSMONMGR_IMAGE_H_
#define SYSMONMGR_IMAGE_H_

#include "adsppm.h"
#include "clientmgr.h"


/**
 * @fn sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs - send clients final q6 floor votes to dcvs
 * @param newClientsFinalVote - final clients q6 floor vote
 */
void sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs(
    uint32 newClientsFinalVote);

/**
 * @fn sysMonMgr_SendNewBwClientsFinalVoteToDcvs - send clients final ib/ab votes to dcvs
 * @param pNewClientsFinalVote - final clients BW vote - ib/ab
 */
void sysMonMgr_SendNewBwClientsFinalVoteToDcvs(
    AdsppmBusBWDataIbAbType* pNewClientsFinalVote,
    Adsppm_DCVS_Monitored_Routes route);

/**
 * @fn sysMonMgr_adsppm_dcvs_state_change_request - sending dcvs enable/disable request to dcvs 
 * @param dcvsEnableDisable - enable/disable dcvs vote
 */	
void sysMonMgr_adsppm_dcvs_state_change_request(
	boolean dcvsEnableDisable)	;

void sysMonMgr_onActiveClientClassChange(
		AdsppmClientManagerEventId eventId,
        void *param);

#endif /* SYSMONMGR_IMAGE_H_ */