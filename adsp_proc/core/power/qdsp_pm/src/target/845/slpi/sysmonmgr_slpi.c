/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: sysmonmgr_slpi.c
@brief: QDSP_PM slpi specific sysmon manager functions.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/target/845/slpi/sysmonmgr_slpi.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"



void sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs(
    uint32 newClientsFinalVote)
{

	//DCVS not present on slpi

}


void sysMonMgr_SendNewBwClientsFinalVoteToDcvs(
    AdsppmBusBWDataIbAbType* pNewClientsFinalVote,
    Adsppm_DCVS_Monitored_Routes route)
{

    //DCVS not present on slpi

}



void sysMonMgr_adsppm_dcvs_state_change_request(boolean dcvsEnableDisable)
{

    //DCVS not present on slpi

}

