/**
@file AVSCoreSvc_PerfMon.h*****************************
@brief This file declares AVSCoreSvc_PerfMon.***********************

This file declares the AVS Core Service instance structure.
********************
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/avcs/src/AdspCoreSvc_InstanceStruct.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/08/10   MWC       Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#ifndef AVSCORESVC_INSTANCESTRUCT_H
#define AVSCORESVC_INSTANCESTRUCT_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "AdspCoreSvc.h"
#include "adsp_core_api.h"

/// AVTimer driver APIs
#include "avtimer_drv_api.h"


// Dynamic Loader
#include "AdspCoreSvc_DynamicLoader.h"
#include "npa.h"

#include "adsp_prv_avcs_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

#define AVCS_MAX_CMD_Q_ELEMENTS  8
#define AVCS_CMD_Q_SIZE_IN_BYTES (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AVCS_MAX_CMD_Q_ELEMENTS))

#ifdef SIM
//to enable dynamic loading on SIM define the flag SIM_DYNAMIC_LOADING
//#define SIM_DYNAMIC_LOADING
#endif
//define below flag for target and for SIM when SIM_DYNAMIC_LOADING is defined.
#if !defined(SIM) || defined(SIM_DYNAMIC_LOADING)
#define DYNAMIC_LOADING_THREAD_PROC
#endif

typedef struct AvsCoreSvc_Prof_t
{
   /** clock related */
   uint32_t npa_adsp_core_clk;
   uint32_t npa_snoc_clk;
   uint32_t npa_bimc_clk;

   npa_query_handle core_clk_handle;
   npa_query_handle snoc_clk_handle;
   npa_query_handle bimc_clk_handle;

   /** state*/
   uint32_t                                uProfilingLevel;            // 0=OFF, 1=overall stats, 2 = overall + per-thread
   uint32_t                                uPmuMode;                   // 0 = AXI BW; 1 = AHB BW ; 2 = L1 I$, D$ miss ratios, MPPS; 3 = L2 $ miss ratio
   uint32_t                                uPrevWallClock;
   qurt_elite_timer_t                      profileTimer;
   uint64_t                                ullPrevPCycles;
   uint32_t                                periodCounter;  //every period this counter is incremented.

   elite_apr_addr_t                        perfMonEvtSrcAddr;
   elite_apr_addr_t                        perfMonEvtDstAddr;
   elite_apr_port_t                        perfMonEvtDstPort;

   uint32_t                                perfMonEvtPingPhyAddrLsw;
   uint32_t                                perfMonEvtPingPhyAddrMsw;
   uint32_t                                perfMonEvtTotalSize;

   uint32_t                                perfMonEvtPingVirtAddr;
   uint32_t                                perfMonEvtPongVirtAddr;
   bool_t                                  adsppmEventNeeded;
   bool_t 										    adsppmEventPerThreadNeeded;
   bool_t                                  keep_sysmon_dcvs_enabled;
   uint16_t                                totNumProfSetsInBatch; //for group events
   uint16_t                                numProfSetInBatch; //variable
   uint32_t                                uMaxProfiledThreads;

   uint32_t                                unMemMapHandle;

   uint32_t                                total_pmu_event_sets;
   uint16_t                                num_pmu_counters;
   avcs_pmu_event_set_t                    *event_set_ptr;
   uint32_t                                cfg_count_index;
   uint32_t                                read_count_index;

   avcs_component_adsppm_voting_info_t     overall;
   avcs_component_adsppm_voting_info_t     VOC;

   uint32_t                                tot_num_hw_threads;
   uint32_t                                q6_arch_version;

   uint16_t                                queryClkInterval; //clk query interval in terms of num of periodCounters

   uint32_t                                prev_pmu_cfg;
   uint32_t                                prev_pmu_evt_cfg;
   uint32_t                                prev_pmu_evt_cfg1;

} AvsCoreSvc_Prof_t;

typedef struct AvsCoreSvc_CacheOp_t
{
    //Cache Line Locking related
    qurt_mem_region_t                  memRegion;
    qurt_addr_t                         virtAddr;
    uint32_t                           totalSize;
    uint32_t                           lockedSize;

}AvcCoreSvc_CacheOp_t;

#define AVS_STATE_STATIC_SVC_MOD_READY 0x1
#define AVS_STATE_ALL_MODULES_READY    0x5

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

typedef struct {
   elite_svc_handle_t                   avcsSvcHandle;
   qurt_elite_channel_t                     avcsChannel;
   qurt_elite_queue_t                      *avcsResponseQ;
   qurt_elite_signal_t                     avcsTimerSignal;
   uint32_t                                unAvcsChannelWaitMask;      //contains the current channel mask or signals to act upon
   uint32_t                                unAvcsChannelStatus;        //current signal received
   elite_apr_handle_t                   aprHandle;
   elite_apr_addr_t                     aprAddr;
   //Declaration for command and response Q for asp core service aligned to 8 bytes
   QURT_ELITE_ALIGN(char avcsCmdQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AVCS_CMD_Q_SIZE_IN_BYTES)], 8);
   QURT_ELITE_ALIGN(char avcsRespQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AVCS_CMD_Q_SIZE_IN_BYTES)], 8);
       //profiling related state
   AvsCoreSvc_Prof_t                       prof;
   AvcCoreSvc_CacheOp_t                    cacheOp;
   adsp_dynamic_loader_t              *pDynLoader;
   uint32_t                            avs_state; //0x1->  AVS services are up and ready. Static modules are also available
                                                   //0x5 -- AVS services are up and ready and all modules are available for use.

} AvsCoreSvc_t;

//Function to process private apr commands.
int AdspCoreSvcPrivateApr(void* pInstance, elite_msg_any_t* pMsg);
ADSPResult AdspCoreSvc_Internal_Init();
void AdspCoreSvc_Internal_DeInit();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ADSPCORESVC_INSTANCESTRUCT_H

