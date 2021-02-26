#ifndef QDSS_CONTROL_PRIV_H
#define QDSS_CONTROL_PRIV_H

/*=============================================================================

FILE:         qdss_control_priv.h

DESCRIPTION:  Private data structures used by the qdss control subsystem

================================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/
#include "DDISTMCfg.h"
#include "DDITFunnel.h"
#include "DDITMC.h"
#include "DDISTMTrace.h"
#include "DDIHWEvent.h"
#include "DDITimer.h"
#include "DDITPDM.h"
#include "DDITPDM.h"
#include "DALSys.h"
#include "DDITimetick.h"
#include "DDIClock.h"
#include "tracer.h"
#include "qdss_control_config.h"
#include "qdss_utils.h"
#include "qdss_diag.h"
#include "uSleep_mode_trans.h"
#include "ClockDefs.h"
#include "npa.h"


struct qdss_clock{
   DALDEVICEHANDLE hClock;
   ClockIdType scc_at_clk_id;
   ClockIdType scc_q6_atbm_clk_id;
   ClockIdType scc_pclkdbg_clk_id;
   ClockIdType scc_dbg_tsctr_clk_id;
   ClockIdType scc_noc_atb_clk_id;
};


typedef struct {

   DALDEVICEHANDLE hTrace;
   uint32 local_stm_port;

   DALDEVICEHANDLE hTraceChip;
   uint32 chip_stm_port;

   DALDEVICEHANDLE hTimetick;
   struct qdss_clock clk;
   uint16 sync_tag;
} QDSSContext;



struct qdss_ts {
   DALDEVICEHANDLE    hTimer;
   DALSYSEventHandle  hTimerEvent;
   DALSYSEventObj     hTimerObject;

   uint32  timer_us;
   boolean bStarted;
   uint64  ssc_dbgctr; //SSC debug counter on island entry
   //statistics
   uint32 timer_callbacks;
};

struct qdss_control {
   DALSYSSyncHandle hSync;
   DALDEVICEHANDLE hSTMCfg;
   DALDEVICEHANDLE hFunnel;
   DALDEVICEHANDLE hTMC;
   DALDEVICEHANDLE hInterruptCtl;
   DALDEVICEHANDLE hHWEvent;
   DALDEVICEHANDLE hTPDA;
   DALDEVICEHANDLE hTPDM;   
   uint8 current_trace_sink;
   boolean bTraceSinkEnabled;
   tracer_client_handle_t hTracer;
   uSleep_notification_cb_handle huSleep;
   uint32 test_data_count;
   struct qdss_ts ts;
   QDSSContext ctxt;
};

extern struct qdss_control qdss;

int qdss_ensure_hw_ready(void);
int qdss_trace_sink_enable(void);
int qdss_trace_sink_disable(void);
void QDSSLogSyncEvents(tracer_event_id_t id);
void QDSSEndMarker(tracer_event_id_t id);
void QDSSModeSwitchMarker(tracer_event_id_t id);
void ETBSwitchToCBUF(void);
void ETBSwitchToHWFIFO(void);
uint64 qdss_ssc_Get_QDSS_Timeval(void);
void QDSS_OpenSSCFunnel(void);
void QDSS_CloseSSCFunnel(void);
int qdss_stm_async_gen(void);
int qdss_csr_init(void);
void QDSS_IslandEnter(void);
void QDSS_IslandExit(void);
extern void (*pfn_qdss_retention_enter)(void);
extern void (*pfn_qdss_retention_exit)(void);
void qdss_csr_timestamp_sync(void);
void qdss_csr_timestamp_sync_renable(void);
void qdss_ssc_ts_lpi_request(uint8 request);





#endif //QDSS_CONTROL_PRIV_H
