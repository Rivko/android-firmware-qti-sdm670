/*=========================================================================
                       QDSS INIT 

GENERAL DESCRIPTION 
   Provides APIs to initialize QDSS driver components and to also flush trace 
   data from QDSS trace buffers.


 
EXTERNALIZED FUNCTIONS
   QDSSInit
   QDSSisEnabled
   QDSSFlush

 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   QDSSInit is the first function that needs to be invoked in order to 
   initialized QDSS driver components. QDSSInit assumes that the DAL component 
   has been initialized prior to executing QDSSInit.

   

      Copyright (c) 2011-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.qdsp6/2.1/debugtrace/common/src/qdss_init.c#4 $
==========================================================================*/  

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/17   rs       added support to configure replicator in adsp
===========================================================================*/

#include "comdef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDISTMCfg.h"
#include "DDITFunnel.h"
#include "DDISTMTrace.h"
#include "DDITMC.h"
#include "ClockDefs.h"
#include "npa.h"
#include <err.h>
#include "qdss.h"
#include "qdss_utils.h"
#include "qdss_fuse_check.h"


typedef enum{
   QDSS_OFF=0,
   QDSS_READY,
   QDSS_IN_USE,
   QDSS_ENUM_LAST=0x7FFFFFFF
}QDSSState;


typedef struct {
	QDSSState eQDSSState;
   DALDEVICEHANDLE hTrace;
   uint32 stm_port;
   npa_client_handle npa_client;   
   DALDEVICEHANDLE hSTMCfg,hFunnel,hTMC;
} QDSSContext;

static int32 QDSSchangeState(QDSSState new_state);
static boolean QDSSisReady(void);



static QDSSContext qdss_ctxt;


void QDSSInit(void)
{
   if(qdss_ctxt.eQDSSState == QDSS_OFF)
   {
      /*Initialize QDSS drivers only once*/
      QDSSchangeState(QDSS_READY);
   }   

#ifdef QDSS_STANDALONE_MODE
   QDSSOn();
#endif   
   return;
}

int32 QDSSOn(void)
{
   if (QDSSisReady())
   {
      if(FALSE==qdss_fuse_trace_access()) {
            return -1;
      }
      return QDSSchangeState(QDSS_IN_USE);

   }
   else if(QDSSisInUse()==TRUE)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
   

int32 QDSSOff(void)
{
   if(QDSSisInUse()==TRUE)
   {
      return QDSSchangeState(QDSS_READY);
   } else if(QDSSisReady())
   {
      return 0;
   } else
   {
      return -1;
   }
}



static boolean QDSSisReady(void)
{
   return (qdss_ctxt.eQDSSState==QDSS_READY);
}

boolean QDSSisInUse(void)
{
   return (qdss_ctxt.eQDSSState==QDSS_IN_USE);
}

void QDSSErrInit(void)
{
   if(qdss_ctxt.eQDSSState == QDSS_READY)
   {
      err_crash_cb_register(QDSSFlush);
   }
}

void QDSSFlush(void)
{
/**pre-requisite is that STM should configure :
  HWIO_OUTF(QDSS_STMSPTRIGCSR,ATBTRIGEN_DIR,HIGH);
  HWIO_OUTF(QDSS_STMSPTRIGCSR,TRIGCTL,1/0);//single-shot or multi-shot mode
  ETB/ETF should configure FFCR=0x623=TrigOnFl|TrigOnTrigEvt|FOnTrigEvt|EnTI|EnFt
**/
/*pre-requisite for flush is QDSS is in IN_USE state*/   
   if((QDSSisInUse()==TRUE) && (qdss_ctxt.stm_port!=NULL) )
   {
      DalSTMTrace_Trigger(qdss_ctxt.hTrace,qdss_ctxt.stm_port,TRACE_TRIG_GUARANTEED|TRACE_TRIG_TIMESTAMPED);
   }
}

static int32 QDSSAttachDrivers(void)
{
   int nErr;
#ifdef QDSS_STANDALONE_MODE
   
   TRY(nErr,DAL_TMCDeviceAttach("DALDEVICEID_TMC",&qdss_ctxt.hTMC));
   TRY(nErr,DAL_STMCfgDeviceAttach("DALDEVICEID_STM_CONFIG",&qdss_ctxt.hSTMCfg));
   {
      TFunnelControlParam funnel_control;

      TRY(nErr,DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&qdss_ctxt.hFunnel));
      funnel_control.pPortStr = "port_stm";
      funnel_control.Val.bEnable  = TRUE;

      TRY(nErr,DalTFunnel_Control(qdss_ctxt.hFunnel,
                                  TFUNNEL_PORT_ENABLE,
                                  &funnel_control));
   }
#endif /*QDSS_STANDALONE_MODE*/

   TRY(nErr,DAL_STMTraceDeviceAttach("DALDEVICEID_STM_TRACE",&qdss_ctxt.hTrace));
   if (qdss_ctxt.hTrace==NULL) {
      THROW(nErr,-1);
   }
   TRY(nErr,DalSTMTrace_NewPort(qdss_ctxt.hTrace,&qdss_ctxt.stm_port));

   CATCH(nErr) {}
   return nErr;   
}

#ifdef QDSS_STANDALONE_MODE

/**
* QDSS_STANDALONE_MODE - typically used on Virtio
* NPA is not functional.
* The assumption is QDSS clocks will be enabled by platform of via JTAG
**/

static int32 QDSSChipClkOn(void) 
{
   return 0;
}

static int32 QDSSChipClkOff(void) 
{
   return 0;
}

#else

static int32 QDSSChipClkOn(void)
{
   if (NULL==qdss_ctxt.npa_client) {
      qdss_ctxt.npa_client = npa_create_sync_client("/clk/qdss", "qdss", NPA_CLIENT_REQUIRED);
   }

   if (NULL==qdss_ctxt.npa_client) {
      return 0;
   }

   npa_issue_required_request(qdss_ctxt.npa_client, CLOCK_QDSS_LEVEL_DEBUG);
   return 0;
}

static int32 QDSSChipClkOff(void)
{
   if(NULL==qdss_ctxt.npa_client) {
      return 0;
   }
   npa_issue_required_request(qdss_ctxt.npa_client, CLOCK_QDSS_LEVEL_OFF);
   return 0;
}

#endif


int QDSSLocalClkOn(void);
int QDSSLocalClkOff(void);



static int32 QDSSClkOn(void)
{
   int nErr1,nErr2;

   nErr1 = QDSSLocalClkOn();
   nErr2 = QDSSChipClkOn();

   return (nErr1|nErr2);

}

static int32 QDSSClkOff(void)
{
   int nErr1,nErr2;

   nErr1 = QDSSLocalClkOff();
   nErr2 = QDSSChipClkOff();

   return (nErr1|nErr2);
}


static int32 QDSSchangeState(QDSSState new_state) 
{
   int32 ret=-1;
   
   switch(qdss_ctxt.eQDSSState){
      case QDSS_OFF:
         if (new_state==QDSS_READY)
         {
            if(QDSSAttachDrivers()==0)
            {
/*TBD: efuse check before changing to READY*/    
/*TBD: in standalone mode skip efuse check */
               qdss_ctxt.eQDSSState=new_state;
               ret=0;
            }
         }
         break;
      case QDSS_READY:
         if(new_state==QDSS_IN_USE)
         {
            if(QDSSClkOn()==0)
            {
               qdss_ctxt.eQDSSState=new_state;
               ret=0;               
            }
         }
         break;
      case QDSS_IN_USE:
         if(new_state==QDSS_READY)
         {
            if(QDSSClkOff()==0)
            {
               qdss_ctxt.eQDSSState=new_state;
               ret=0;               
            }
         }
         break;
      default:
         break;
   }
   return ret;
}

