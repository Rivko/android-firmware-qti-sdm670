/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: sysmonmgr_cdsp.c
@brief: QDSP_PM cdsp specific sysmon manager functions.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/target/830/cdsp/sysmonmgr_cdsp.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"


void sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs(
    uint32 newClientsFinalVote)
{

	//DCVS not present as of now for cdsp core compilation
	
}


void sysMonMgr_SendNewBwClientsFinalVoteToDcvs(
    AdsppmBusBWDataIbAbType* pNewClientsFinalVote,
    Adsppm_DCVS_Monitored_Routes route)
{
  if (route == Adsppm_DCVS_Route_DSP_DDR)
{
    //DCVS not present as of now for cdsp core compilation
  }
  else if (route == Adsppm_DCVS_Route_HCP_DDR)
  {
    //DCVS not present as of now for cdsp core compilation
  }
  else if (route == Adsppm_DCVS_Route_DMA_DDR)
  {
    //DCVS not present as of now for cdsp core compilation
  }
}


void sysMonMgr_adsppm_dcvs_state_change_request(boolean dcvsEnableDisable)
{

    //DCVS not present as of now for cdsp core compilation

}

