/*=============================================================================

FILE:         qdss_control.c

DESCRIPTION:  Implements handlers for diag comamnds to configure and control
              SSC QDSS.

================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "qdss_diag.h"
#include "DALDeviceId.h"
#include "qdss.h"
#include "qdss_tfunnel.h"
#include "qdss_ts.h"
#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_micro.h"
#include "qdss_clock_priv.h"
#include "DDITPDA.h"
#include "qdss_fuse_check.h"

struct qdss_control qdss;

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_QUERY_STATUS diag command

  Memory for request and response packets is already allocated
  before calling this function. This applies for all diag command handlers below

  @param                 pReq [in ] :  request packet
  @param              req_len [in ] :  request packet length
  @param                 pRsp [in ] :  response packet
  @param              rsp_len [in ] :  response packet length

  @return 0 if successful, error code otherwise
 */
int qdss_query_status_handler(qdss_query_status_req *pReq,
                              int req_len,
                              qdss_query_status_rsp *pRsp,
                              int rsp_len)
{

   int nErr;

   TRY(nErr,qdss_control_get_sink(&pRsp->trace_sink));
   TRY(nErr,qdss_control_get_stm(&pRsp->stm_enabled));
   TRY(nErr,qdss_control_get_hwe(&pRsp->hw_events_enabled));

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_STM diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_stm_handler(qdss_filter_stm_req *pReq,
                          int req_len,
                          qdss_filter_stm_rsp *pRsp,
                          int rsp_len)
{
   int nErr;

   if (pReq->state) {
      TRY(nErr,qdss_stm_trace_enable());
   }
   else {
      TRY(nErr,qdss_stm_trace_disable());
   }

   CATCH(nErr){}
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_HWEVENTS_ENABLE diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_hwevents_handler(qdss_filter_hwevents_req *pReq,
                          int req_len,
                          qdss_filter_hwevents_rsp *pRsp,
                          int rsp_len)

{
   int nErr;

   TRY(nErr,qdss_control_set_hwe(pReq->state));

   CATCH(nErr){}

   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_HWEVENTS_CONFIGURE diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_hwevents_configure_handler(qdss_filter_hwevents_configure_req *pReq,
                          int req_len,
                          qdss_filter_hwevents_configure_rsp *pRsp,
                          int rsp_len)

{
   int nErr;

   TRY(nErr,qdss_control_set_hwe_reg(pReq->register_addr,
                                     pReq->register_value));

   CATCH(nErr) {}
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_filter_hwevents_set_detect_mask_handler(qdss_filter_hwevents_set_detect_mask_req *pReq,
                        int req_len,
                        qdss_filter_hwevents_set_detect_mask_rsp *pRsp,
                        int rsp_len)
{
   int nErr = QDSS_HWEVENT_UNKNOWN_ERR;
   TRY(nErr,qdss_control_set_hwe_detect_mask(pReq->hwe_mask));
   CATCH(nErr) {}
   pRsp->result=nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_filter_hwevents_set_trigger_mask_handler(qdss_filter_hwevents_set_trigger_mask_req *pReq,
                        int req_len,
                        qdss_filter_hwevents_set_trigger_mask_rsp *pRsp,
                        int rsp_len)
{
   int nErr = QDSS_HWEVENT_UNKNOWN_ERR;
   TRY(nErr,qdss_control_set_hwe_trigger_mask(pReq->hwe_mask));
   CATCH(nErr) {}
   pRsp->result=nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
void qdss_usleep_notification(uSleep_state_notification state)
{
   switch(state) {
   case USLEEP_STATE_ENTER:
      QDSS_IslandEnter();
      break;
   case USLEEP_STATE_EXIT:
      QDSS_IslandExit();
      break;
   default:
      //do nothing
      break;
   }
}


/*-------------------------------------------------------------------------*/
void qdss_register_usleep(void) 
{
   //TODO: Need to time enter exit latencies.
   qdss.huSleep=uSleep_registerNotificationCallback(300,600,qdss_usleep_notification);
}


/*-------------------------------------------------------------------------*/
int qdss_enable_local_etb(void) 
{
   int nErr;
   int tmc_mode = TMC_MODE_CIRCULAR_BUFFER;
   
   if ( (qdss.current_trace_sink == QDSS_TRACESINK_CBUF) ||
        (qdss.current_trace_sink == QDSS_TRACESINK_HWFIFO)) {
      tmc_mode = qdss.current_trace_sink;
   }
   else {
      qdss.current_trace_sink = QDSS_TRACESINK_AUTO;
      tmc_mode = TMC_MODE_CIRCULAR_BUFFER;
   }
   TRY(nErr,DalTMC_SetMode(qdss.hTMC,tmc_mode));
   TRY(nErr,DalTMC_EnableTrace(qdss.hTMC));

   CATCH(nErr){}

   return nErr;
}

int  QDSSAttachTraceDrivers(void);

/*-------------------------------------------------------------------------*/

/**
  @brief  Ensure QDSS HW is ready to be configure

 This function votes QDSS clocks on, attach to drivers and init hardware
 If this is already done this function returns success

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
 */
int qdss_ensure_hw_ready(void)
{
   int nErr = DAL_ERROR;

   THROW_IF(nErr,(FALSE==qdss_fuse_trace_access()),QDSS_CONTROL_FUSE_BLOWN);

   if ((NULL == qdss.hTMC)||(NULL == qdss.hFunnel)||(NULL == qdss.hSTMCfg)||
       (NULL == qdss.hHWEvent) || (NULL==qdss.huSleep)) {
      //Make sure clocks are turned on.
      TRY(nErr,QDSSOn());

      if (NULL == qdss.hTMC) {
         TRY(nErr, DAL_TMCDeviceAttach("DALDEVICEID_TMC_SLPI",&qdss.hTMC));
         TRY(nErr, qdss_enable_local_etb());
      }

      if (NULL == qdss.hFunnel) {
         TRY(nErr,DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&qdss.hFunnel));
      }

      if (NULL == qdss.hSTMCfg) {
         TRY(nErr,DAL_STMCfgDeviceAttach("DALDEVICEID_STM_CONFIG_SLPI",&qdss.hSTMCfg));
      }

      if (NULL == qdss.hHWEvent) {
         TRY(nErr,DAL_HWEventDeviceAttach("DALDEVICEID_HWEVENT",&qdss.hHWEvent));
      }

      if (NULL == qdss.hTPDM) {
         TRY(nErr, DAL_TPDMDeviceAttach("DALDEVICEID_LPA_TPDM",&qdss.hTPDM));
      }
      if (NULL == qdss.hTPDA) {
         TRY(nErr, DAL_TPDMDeviceAttach("DALDEVICEID_LPA_TPDA",&qdss.hTPDA));
      }



	if (NULL== qdss.huSleep) {
	   qdss_register_usleep();
	}

      TRY(nErr,QDSSAttachTraceDrivers());

      TRY(nErr,qdss_csr_init());
      
   }

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr){
      if (DAL_ERROR==nErr) {
         nErr = QDSS_CONTROL_BAD_STATE;
      }
      else if (DAL_ERROR_DEVICE_ACCESS_DENIED==nErr)
      {
         nErr = QDSS_CONTROL_FUSE_BLOWN;
      }
   }

   return nErr;
}


int qdss_control_on_bootup(void);

/*-------------------------------------------------------------------------*/

#define QDSS_INVALID_TRACE_SINK  0xFF

/**
  @brief Initializes the QDSS control subsystem.

  This is called from RC init. QDSS HW is not touched or clocked.
  That happens when explicitly turned on by DIAG commands

  @return None
 */
void qdss_control_init(void)
{
   int nErr;
   qdss_diag_init();
   qdss.hTracer = 0;
   qdss.hTMC = 0;
   qdss.hFunnel = 0;
   qdss.hSTMCfg = 0;
   qdss.hSync = 0;
   qdss.current_trace_sink = QDSS_TRACESINK_HWFIFO;
   qdss.bTraceSinkEnabled = FALSE;
   TRY(nErr,DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &qdss.hSync, NULL));
   qdss.ts.timer_us = 0;
   TRY(nErr,qdss_clock_init());
   TRY(nErr,qdss_csr_init());
   
   TRY(nErr,qdss_control_on_bootup());


   CATCH(nErr) {
   }
   IGNORE(nErr);
}

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_get_stm(uint8 *state)
{
   uint32 uStatus = 0;
   int nErr;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMStatus(qdss.hSTMCfg,STM_EN,&uStatus),
            QDSS_CONTROL_DEVICE_ERROR);

   *state =  (uStatus)?1:0;

   CATCH(nErr){}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_set_stm(uint8 state)
{
   int nErr;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   //disable first, so enable would actually be a re-enable
   //This is to ensure ASYNC gets generated anytime this is called.
   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,0),
            QDSS_CONTROL_DEVICE_ERROR);

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,state),
            QDSS_CONTROL_DEVICE_ERROR);

   CATCH(nErr){}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_get_hwe(uint8 *state)
{
   uint32 uStatus = 0;
   int nErr;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMStatus(qdss.hSTMCfg,STM_HW_EVT_EN,&uStatus),
            QDSS_CONTROL_DEVICE_ERROR);

   *state =  (uStatus)?1:0;

   CATCH(nErr){}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}


/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_set_hwe(uint8 state)
{
  int nErr;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());
   if (state) {
      //sync timestamps before enabling HW events
      qdss_csr_timestamp_sync();
   }
   else {
      //make sure we resync for next session
      qdss_csr_timestamp_sync_renable();
   }

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_EN,state),
            QDSS_CONTROL_ERROR);

   /*Enable default pre-compiled HW events if state==1*/
   if (state==0x1)
   {

      THROW_IF(nErr, DAL_SUCCESS !=
               DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_ENABLE_MASK,0xFFFFFFFF),
            QDSS_CONTROL_ERROR);

      THROW_IF(nErr,DAL_SUCCESS !=
               DAL_HWEvent_ConfigurePreset(qdss.hHWEvent),
               QDSS_CONTROL_DEVICE_ERROR);
   }

   CATCH(nErr){}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}


/*-------------------------------------------------------------------------*/
int qdss_control_set_hwe_detect_mask(uint32 bit_mask)
{
   int nErr;
   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr, DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_ENABLE_MASK,bit_mask),
            QDSS_CONTROL_ERROR);
   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr) {}
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_control_set_hwe_trigger_mask(uint32 bit_mask)
{
   int nErr;
   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr, DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_TRIGGER_MASK,bit_mask),
            QDSS_CONTROL_ERROR);
   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr) {}
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}
/*-------------------------------------------------------------------------*/

boolean qdss_stm_trace_is_enabled (void) 
{
   uint8 stm_enabled = 0;

   if ((NULL == qdss.hTMC)||(NULL == qdss.hFunnel)||(NULL == qdss.hSTMCfg)||
       (DAL_SUCCESS!=qdss_control_get_stm(&stm_enabled)) ||
       (0==stm_enabled)) {
      return FALSE;
   }
   return TRUE;

}

extern void tracer_set_local_stm_enable(boolean enable);


/*-------------------------------------------------------------------------*/


int qdss_stm_trace_enable(void)
{
   int nErr = 0;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,QDSSOn());


   TRY(nErr,qdss_ensure_hw_ready());
   TRY(nErr,qdss_enable_local_etb());
   TRY(nErr, qdss_tfunnel_enable_port(TFUNNEL_SSC_STM_PORT));
   qdss_csr_timestamp_sync();
   TRY(nErr, qdss_control_set_stm(1)); //1 for enable

   



   if (QDSS_TRACESINK_AUTO == qdss.current_trace_sink )  {
      QDSS_OpenSSCFunnel();
      ETBSwitchToHWFIFO();
      QDSSLogSyncEvents(QDSS_SSC_TS_SYNC);
      QDSSEndMarker(QDSS_SSC_ETB_NORMAL_MODE_START);
      ETBSwitchToCBUF();
      QDSS_CloseSSCFunnel();
      QDSSModeSwitchMarker(QDSS_SSC_ETB_NORMAL_MODE_START);
      QDSSLogSyncEvents(QDSS_SSC_TS_SYNC);
   }




   CATCH(nErr) {}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_stm_trace_disable(void)
{
   int nErr = 0;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);

   if (qdss_stm_trace_is_enabled()) {
      if (QDSS_TRACESINK_AUTO == qdss.current_trace_sink ) {
         QDSS_OpenSSCFunnel();
         ETBSwitchToHWFIFO();
         QDSSLogSyncEvents(QDSS_SSC_TS_SYNC);
         QDSSEndMarker(QDSS_SSC_ETB_NORMAL_MODE_END);
         ETBSwitchToCBUF();
         QDSS_CloseSSCFunnel();
      }

      TRY(nErr, qdss_tfunnel_disable_port(TFUNNEL_SSC_STM_PORT));
      TRY(nErr, qdss_control_set_stm(0)); //0 for disable
   }
   CATCH(nErr) {}     

   qdss_csr_timestamp_sync_renable();
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}



/*-------------------------------------------------------------------------*/
 
void QDSS_IslandEnter(void)
{
   if (0==qdss.hSync) {
      return;
   }
   
   if (QDSS_TRACESINK_AUTO != qdss.current_trace_sink )  {
      return;
   }


   DALSYS_SyncEnter(qdss.hSync);

   if (qdss_stm_trace_is_enabled()) {
   
      tracer_event_simple(QDSS_ISLAND_ENTER);
      QDSS_OpenSSCFunnel();
      ETBSwitchToHWFIFO();
      QDSSLogSyncEvents(QDSS_SSC_TS_SYNC);
      QDSSEndMarker(QDSS_SSC_ETB_NORMAL_MODE_END);
      ETBSwitchToCBUF();
      QDSS_CloseSSCFunnel();
      QDSSModeSwitchMarker(QDSS_SSC_ETB_ISLAND_MODE_START);
   }
   DALSYS_SyncLeave(qdss.hSync);

}

/*-------------------------------------------------------------------------*/
uint32  HAL_qdss_etfetb_GetWritePtr(void);

void QDSS_IslandExit(void)
{
   uint32 etb_write_ptr = 0;

   if (0==qdss.hSync) {
      return;
   }

   if (QDSS_TRACESINK_AUTO != qdss.current_trace_sink )  {
      return;
   }


   DALSYS_SyncEnter(qdss.hSync);

   if (qdss_stm_trace_is_enabled()) {
     
      etb_write_ptr = HAL_qdss_etfetb_GetWritePtr();
      QDSS_OpenSSCFunnel();
      ETBSwitchToHWFIFO();
      UTRACER_EVENT(QDSS_ISLAND_EXIT,etb_write_ptr);
      QDSSLogSyncEvents(QDSS_SSC_TS_SYNC);
      QDSSEndMarker(QDSS_SSC_ETB_ISLAND_MODE_END);
      ETBSwitchToCBUF();
      QDSS_CloseSSCFunnel();
      QDSSModeSwitchMarker(QDSS_SSC_ETB_NORMAL_MODE_START);
   }

   DALSYS_SyncLeave(qdss.hSync);
}

/*-------------------------------------------------------------------------*/

//Uncomment below line to enable sensor ETM TRACE on boot-up
//#define QDSS_ENABLE_SENSOR_ETM_ON_BOOTUP  1

//Uncomment below line to enable sensor ETM after a given delay
//#define QDSS_ENABLE_SENSOR_ETM_DEFERRED  1



int qdss_control_on_bootup(void)
{
   int nErr = QDSS_CONTROL_SUCCESS;

#ifdef QDSS_STANDALONE_MODE
   TRY(nErr,qdss_stm_trace_enable());
   CATCH(nErr) {}
#elif QDSS_ENABLE_SENSOR_ETM_ON_BOOTUP
   TRY(nErr,qdss_control_set_etm(1));
   CATCH(nErr) {}
#elif QDSS_ENABLE_SENSOR_ETM_DEFERRED
   qdss.ts.timer_us = 30000000;
   qdss_ts_start_timer();
#endif

   return nErr;
}


int  QDSSAttachTraceDrivers(void)
{

   /* Initializing nErr to DAL_SUCCESS, if handles/ports are already initiaized then its obvious success
      as we will not be initializing it again. And if handles/ports are not initialized or partially initialized 
      then based on return value from individual DAL API call, a return value will be decided 
      (either DAL_SUCCESS = 0 or DAL_FAILUER = -1)
   */ 
      
   int nErr = DAL_SUCCESS;

   if (NULL==qdss.ctxt.hTrace) {
      TRY(nErr,DAL_STMTraceDeviceAttach("DALDEVICEID_STM_TRACE_SLPI",&qdss.ctxt.hTrace));
   }
   if (0==qdss.ctxt.local_stm_port) {
      TRY(nErr,DalSTMTrace_NewPort(qdss.ctxt.hTrace,&qdss.ctxt.local_stm_port));
   }
   if (NULL == qdss.ctxt.hTraceChip) {
      TRY(nErr,DAL_STMTraceDeviceAttach("DALDEVICEID_STM_TRACE",&qdss.ctxt.hTraceChip));
   }
   if (0==qdss.ctxt.chip_stm_port) {
      TRY(nErr,DalSTMTrace_NewPort(qdss.ctxt.hTraceChip,&qdss.ctxt.chip_stm_port));
   }

   CATCH(nErr) {}
   return nErr;   
}



void tracer_log_sync_event(uint32 port, tracer_event_id_t id, uint64 tag);
void tracer_stp_send_event(uint32 port, tracer_event_id_t id);
int qdss_stm_async_gen(void);

//For Each D16 takes up 3 bytes, For 1K of padding we need this much
#define QDSS_END_MARKER_PADDING 342

void QDSSEndMarker(tracer_event_id_t id)
{
   int i;
   if (qdss.ctxt.local_stm_port!=NULL)  {
      tracer_stp_send_event(qdss.ctxt.local_stm_port,id);
      /* On 8996 ATB bridge that connects SSC ATB to main QDDS ATB
         lacks proper low power mode support. Sensor island trace data
         that is left in the ATB bridge buffers after it is drained and 
         flushed from SSC is lost or corrupted. 
         HW CR QCTDD02182478 tracks the ATB async bridge issue.
        
         This workaround pads the end of island mode trace with a series
         of end-markers, so that what is left on the ATB bridge when it enters
         retention is END markers. We pad sufficient number of END marker
         packets so that at least one of them will cross the ATB bridge and is
         available in the main QDSS intact. Parser on the PC side when it sees
         and END marker, will skip everything until the next ASYNC.
         This way loss/corruption of valid trace held in ASYNC brdige is avoided
      */
      for (i=0; i< QDSS_END_MARKER_PADDING; i++) {
         DalSTMTrace_Data16(qdss.ctxt.hTrace,
                            qdss.ctxt.local_stm_port,
                            TRACE_DATA_MARKED,
                            0x1111);
      }
   }
}


static uint64 QDSSGetTimeTicks(void)
{
   int nErr;
   uint64 now=0;

   if (0==qdss.ctxt.hTimetick) {
      TRY(nErr,DalTimetick_Attach("SystemTimer", &qdss.ctxt.hTimetick));
      TRY(nErr,DalTimetick_Enable(qdss.ctxt.hTimetick,1));
      TRY(nErr,DalTimetick_InitTimetick64(qdss.ctxt.hTimetick));
   }
   TRY(nErr,DalTimetick_GetTimetick64(qdss.ctxt.hTimetick, &now));

   CATCH(nErr) {};
   return now;
}




void QDSSSyncMarkers(void)
{
   DalSTMTrace_Data16
      (qdss.ctxt.hTraceChip,
       qdss.ctxt.chip_stm_port,
       TRACE_DATA_GUARANTEED | TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED,
       qdss.ctxt.sync_tag);

   DalSTMTrace_Data16
      (qdss.ctxt.hTrace,
       qdss.ctxt.local_stm_port,
       TRACE_DATA_GUARANTEED | TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED,
       qdss.ctxt.sync_tag);

   qdss.ctxt.sync_tag++;
}

void QDSSLogSyncEvents(tracer_event_id_t id)
{
   uint64 tag=0;

   if( (!QDSSisInUse()) ||
       (qdss.ctxt.local_stm_port==NULL) ||
       (qdss.ctxt.chip_stm_port == NULL)) {
      return;
   }

   QDSSSyncMarkers();

   tag=QDSSGetTimeTicks();
   tracer_log_sync_event(qdss.ctxt.chip_stm_port,id,tag);
   tracer_log_sync_event(qdss.ctxt.local_stm_port,id,tag);
}


void QDSSModeSwitchMarker(tracer_event_id_t id)
{  
   if (qdss.ctxt.local_stm_port!=NULL)  {
      qdss_stm_async_gen();
      tracer_stp_send_event(qdss.ctxt.local_stm_port,id);
   }
}


#define SSC_STM_TSCTR_FREQ  19200000 //AO timestamp frequency

uint32 qdss_get_ssc_tsctr_freq(void) 
{
   uint32 freq = 0;

   if (0==freq) {
      freq=SSC_STM_TSCTR_FREQ;
   }

   return freq;
}

