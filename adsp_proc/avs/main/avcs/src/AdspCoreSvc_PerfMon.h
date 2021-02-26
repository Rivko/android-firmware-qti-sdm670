/**
@file AVSCoreSvc_PerfMon.h*****************************
@brief This file declares AVSCoreSvc_PerfMon.***********************

This file contains the utilities that measure run-time performance of
the AVS. Such as MIPS, memory consumption, thread stack levels, etc.
********************
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/avcs/src/AdspCoreSvc_PerfMon.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/08/10   MWC       Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#ifndef AVSCORESVC_PERFMON_H
#define AVSCORESVC_PERFMON_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "AdspCoreSvc.h"
#include "AdspCoreSvc_InstanceStruct.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
* This function handles the request for periodic profiling events.
*
* @param[in] pMe This is the object pointer of the core service.
* @param[in] pPacket This is the message packet of the request.
* @param[in] profiling_duration performance monitor duration set via QXDM
* @param[in] interface performance monitor interface
* @return none.
*/
void AdspCoreSvc_SetPeriodicProfilingEventsCmdHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pPacket,
                                         uint16_t profiling_duration);

void AdspCoreSvc_GetCommonProfInfoCmdHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pPacket);

void AVS_AdsppmVoteDone(AvsCoreSvc_t *pMe, elite_apr_packet_t* pPacket);
/**
* This function generates a periodic profiling event.
*
* @param[in] pMe This is the object pointer of the core service.
* @return none.
*/
void AdspCoreSvc_ProcessPerfMon(AvsCoreSvc_t *pMe);


ADSPResult AdspCoreSvc_AdsppmVoteDone(AvsCoreSvc_t *pMe, elite_msg_any_t *pMsg);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AVSCORESVC_PERFMON_H

