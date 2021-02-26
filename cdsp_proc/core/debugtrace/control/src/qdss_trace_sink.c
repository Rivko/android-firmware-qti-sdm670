/*=============================================================================

FILE:         qdss_trace_sink.c

DESCRIPTION:  Implementation for the trace sink switch command

================================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/

#include "qdss_control.h"
#include "qdss_control_priv.h"


int qdss_stm_async_gen(void) 
{
   int nErr=0;

   TRY(nErr,DalSTMCfg_STMControl(qdss.hSTMCfg,STM_TIMESTAMP_FREQ_PKTGEN,1));

   CATCH(nErr) {}
   return nErr;
}



/*-------------------------------------------------------------------------*/

/**
  @brief Re-enable trace after a trace sink switch.

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_enable(void)
{
   int nErr = 0;
   uint8 stm_enabled = 0;

   qdss.bTraceSinkEnabled = TRUE;
   TRY(nErr,  DalTMC_EnableTrace(qdss.hTMC));

   TRY(nErr, qdss_control_get_stm(&stm_enabled));

   if (stm_enabled) {
      //re-enable STM so the trace sink receives a
      //sync channel packets
      TRY(nErr,DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,0));
      TRY(nErr,DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,1));
   }


   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Disable  trace before a trace sink switch

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_disable(void)
{
   int nErr = 0;

   qdss.bTraceSinkEnabled = FALSE;
   TRY(nErr,DalTMC_DisableTrace(qdss.hTMC));

   CATCH(nErr) {}
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief Switch to CBUF and start trace

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_cbuf_start(void)
{
   int nErr;

   QDSS_CloseSSCFunnel();
   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_MODE_CIRCULAR_BUFFER));
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Stop tracing to CBUF.

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_cbuf_stop(void)
{
   int nErr;
   TRY(nErr,qdss_trace_sink_disable());
   QDSS_CloseSSCFunnel();

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Switch trace sink to HWFIFO and start tracing

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_hwfifo_start(void)
{
   int nErr;


   QDSS_OpenSSCFunnel();
   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_MODE_HW_FIFO));
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stop tracing to HWFIFO

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_hwfifo_stop(void)
{
   int nErr;
   TRY(nErr,qdss_trace_sink_disable());
   QDSS_CloseSSCFunnel();
   
   CATCH(nErr) {}
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief  Start tracing to a new trace sink

  @param           trace_sink [in ] :   trace sink to start

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_PARAM = Invalid parameter.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_trace_sink_start(uint8 trace_sink)
{
   int nErr = QDSS_CONTROL_SUCCESS;

   switch(trace_sink) {
   case QDSS_TRACESINK_CBUF:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_cbuf_start(),
               QDSS_CONTROL_ERROR);
      break;

   case QDSS_TRACESINK_HWFIFO:
   case QDSS_TRACESINK_AUTO:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_hwfifo_start(),
               QDSS_CONTROL_ERROR);

      break;


   default:
      THROW(nErr,QDSS_CONTROL_UNSUPPORTED);
   }

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stop tracing to a given trace sink

  @param           trace_sink [in ] :  trace sink to stop

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_PARAM = Invalid parameter.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_trace_sink_stop(uint8 trace_sink)
{
   int nErr = QDSS_CONTROL_SUCCESS;

   switch(trace_sink) {
   case QDSS_TRACESINK_CBUF:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_cbuf_stop(),
               QDSS_CONTROL_ERROR);
      break;

   case QDSS_TRACESINK_HWFIFO:
   case QDSS_TRACESINK_AUTO:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_hwfifo_stop(),
               QDSS_CONTROL_ERROR);
      break;


   default:
      THROW(nErr,QDSS_CONTROL_UNSUPPORTED);
   }

   CATCH(nErr) {}
   return nErr;
}



/*-------------------------------------------------------------------------*/

/**
  @brief  Trace sink switch diag command handler

  @param                 pReq [in ] :  request packet
  @param              req_len [in ] :  request packet length
  @param                 pRsp [in ] :  response packet
  @param              rsp_len [in ] :  response packet length

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_handler(qdss_trace_sink_req  *pReq,
                            int req_len,
                            qdss_trace_sink_rsp *pRsp,
                            int rsp_len)
{
   int nErr;

   TRY(nErr,qdss_control_set_sink(pReq->trace_sink));

   CATCH(nErr) {}
   pRsp->result = nErr;
   return nErr;
}

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/

int qdss_control_get_sink(uint8 *sinkid)
{
   int nErr;

   if (0==qdss.hSync) {
      nErr=QDSS_CONTROL_BAD_STATE;
      return nErr;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   TRY(nErr,DalTMC_GetMode(qdss.hTMC,sinkid));

   CATCH(nErr) {}

   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_BAD_PARAM = Parameter invalid.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_set_sink(uint8 sinkid)
{
   int nErr;
   uint8 new_trace_sink= (uint8) sinkid;
   uint8 current_trace_sink =  qdss.current_trace_sink;

   if (0==qdss.hSync) {
      nErr=QDSS_CONTROL_BAD_STATE;
      return nErr;
   }

   DALSYS_SyncEnter(qdss.hSync);

   TRY(nErr,qdss_ensure_hw_ready());

   current_trace_sink =  qdss.current_trace_sink;
   
   TRY(nErr,qdss_trace_sink_stop(qdss.current_trace_sink));
   qdss.current_trace_sink = new_trace_sink;
   TRY(nErr,qdss_trace_sink_start(new_trace_sink));

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr){
      //had an error - keep the current
      qdss.current_trace_sink = current_trace_sink;
   }

   DALSYS_SyncLeave(qdss.hSync);

   return nErr;
}


/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
*/
int qdss_control_get_sink_list(uint8 *sinkid_list, int list_size, int *num_sinks)
{
#define QDSS_NUM_TRACE_SINKS 3

   if (0!=num_sinks) {
      *num_sinks = QDSS_NUM_TRACE_SINKS;
   }

   if (list_size < QDSS_NUM_TRACE_SINKS) {
      return 1; //error
   }

   sinkid_list[0]=QDSS_TRACESINK_CBUF;
   sinkid_list[1]=QDSS_TRACESINK_HWFIFO;
   sinkid_list[2]=QDSS_TRACESINK_AUTO;

   return QDSS_CONTROL_SUCCESS; //success
}
