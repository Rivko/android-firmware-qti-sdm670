/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: sysmonmgr_adsp.c
@brief: QDSP_PM adsp specific sysmon manager functions.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/target/845/adsp/sysmonmgr_adsp.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "sysmon_dcvs_adsppm.h"


void sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs(
    uint32 newClientsFinalVote)
{

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Send new clients final vote to DCVS: clk=%u",
        newClientsFinalVote);

    // Call SysMon DCVS API to update client's ADSP Clock vote to DCVS
    sd_adsppm_vote_t dspClk;
    dspClk.coreVoteinHz = newClientsFinalVote;

    sysmon_adsppm_static_vote_update(
        SYSMON_DCVS_RSC_ID_ADSP_CLK, &dspClk);

}


void sysMonMgr_SendNewBwClientsFinalVoteToDcvs(
    AdsppmBusBWDataIbAbType* pNewClientsFinalVote,
    Adsppm_DCVS_Monitored_Routes route)
{

    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
        "Send new clients final vote to DCVS: ib=%llu, ab=%llu",
        (uint32) pNewClientsFinalVote->Ib,
        (uint32) (pNewClientsFinalVote->Ib >> 32),
        (uint32) pNewClientsFinalVote->Ab,
        (uint32) (pNewClientsFinalVote->Ab >> 32));

    // Call SysMon DCVS API to update client's DDR BW vote to DCVS
    sd_adsppm_vote_t dsp2DdrBw;
    dsp2DdrBw.busVoteinAbIb.uIb = pNewClientsFinalVote->Ib;
    dsp2DdrBw.busVoteinAbIb.uAb = pNewClientsFinalVote->Ab;

    sysmon_adsppm_static_vote_update(
        SYSMON_DCVS_RSC_ID_ADSP_DDR_BW, &dsp2DdrBw);

}


void sysMonMgr_adsppm_dcvs_state_change_request(boolean dcvsEnableDisable)
{
    // Call SysMon DCVS API to enable/disable DCVS
    sysmon_adsppm_dcvs_state_change_request((dcvsEnableDisable?
			SYSMON_DCVS_STATE_ENABLE:SYSMON_DCVS_STATE_DISABLE));
}

