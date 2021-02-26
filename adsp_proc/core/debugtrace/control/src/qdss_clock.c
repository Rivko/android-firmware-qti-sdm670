/*=============================================================================

FILE:         qdss_clock.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qdss_clock_priv.h"
#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "stdlib.h"
#include "ssc_qdss_csr_regs.h"

extern boolean qdss_lpm_enabled;
extern void qdss_ssc_lpi_request(uint32 reg_addr, uint32 request);
extern uint32 qdss_ssc_csr_base;
	
#define QDSS_ENABLE_CLOCK(clk_name,clk_id)               \
   TRY(nErr,DalClock_GetClockId(qdss.ctxt.clk.hClock,    \
                                clk_name,                \
                                &clk_id));               \
   TRY(nErr,DalClock_EnableClock(qdss.ctxt.clk.hClock,   \
                                 clk_id))   



int QDSSLocalClkOn(void)
{
   int nErr=0;

   if (NULL==qdss.ctxt.clk.hClock) {
      TRY(nErr,DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, 
                                     &qdss.ctxt.clk.hClock));

      QDSS_ENABLE_CLOCK("scc_at_clk",qdss.ctxt.clk.scc_at_clk_id);
      QDSS_ENABLE_CLOCK("scc_q6_atbm_clk",qdss.ctxt.clk.scc_q6_atbm_clk_id);
      QDSS_ENABLE_CLOCK("scc_pclkdbg_clk",qdss.ctxt.clk.scc_pclkdbg_clk_id);
      QDSS_ENABLE_CLOCK("scc_dbg_tsctr_clk",qdss.ctxt.clk.scc_dbg_tsctr_clk_id);
      QDSS_ENABLE_CLOCK("scc_noc_atb_clk",qdss.ctxt.clk.scc_noc_atb_clk_id);
   }

   if (0==nErr) {
      //Enable LPM entry exit sequences
      qdss_qurt_lpm_enable(); //This sets a flag in qurt
      qdss_lpm_enable(); //This sets a flag in root PD
      
	  if ((qdss_lpm_enabled) && (0 != qdss_ssc_csr_base)) {
	      qdss_ssc_lpi_request(SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,1);
	      qdss_ssc_lpi_request(SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,1);
	      qdss_ssc_lpi_request(SSC_APB_LOW_POWER_HANDSHAKE_ADDR,1);
	      qdss_ssc_lpi_request(SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,1);		
	      qdss_ssc_lpi_request(SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE,1);				  		  
	  }      
  }

   CATCH(nErr) {}

   return nErr;
}

int QDSSLocalClkOff(void)
{
   //SSC qdss clocks are off by default due to save power 
   //However, if they are turned on for debugging/tracing
   //the clocks will remain on.
   return 0;
}

int _QDSSLocalClkOff(void)
{
   int nErr;

   if (NULL == qdss.ctxt.clk.hClock) {
      return -1;
   }

   TRY(nErr,DalClock_DisableClock(qdss.ctxt.clk.hClock,qdss.ctxt.clk.scc_at_clk_id));
   TRY(nErr,DalClock_DisableClock(qdss.ctxt.clk.hClock,qdss.ctxt.clk.scc_q6_atbm_clk_id));
   TRY(nErr,DalClock_DisableClock(qdss.ctxt.clk.hClock,qdss.ctxt.clk.scc_pclkdbg_clk_id));
   TRY(nErr,DalClock_DisableClock(qdss.ctxt.clk.hClock,qdss.ctxt.clk.scc_dbg_tsctr_clk_id));
   TRY(nErr,DalClock_DisableClock(qdss.ctxt.clk.hClock,qdss.ctxt.clk.scc_noc_atb_clk_id));

   CATCH(nErr) {}
   return nErr;
}

int QDSSLocalClkSet(int bEnable) 
{
   if (bEnable) {
      return QDSSLocalClkOn();
   }
   else {
      return QDSSLocalClkOff();
   }
}


struct _qdss_clock qdss_clock;

const qurt_qdi_obj_t qdss_clockQDIOpener = {
    qdss_clockQDI_Invoke,
    QDI_REFCNT_PERM,
    qdss_clockQDI_Release    
};


int qdss_clockQDI_Open
(
  int hClientHandle,
  qurt_qdi_obj_t *objptr,
  const char *devname,
  int mode
)
{
  /*
   * Initialize the QDI object function pointers and create the handle.
   */
  qdss_clock.qdss_clockQDIDrv.invoke = qdss_clockQDI_Invoke;
  qdss_clock.qdss_clockQDIDrv.refcnt = QDI_REFCNT_INIT;
  qdss_clock.qdss_clockQDIDrv.release = qdss_clockQDI_Release;

  return qurt_qdi_handle_create_from_obj_t(hClientHandle,&qdss_clock.qdss_clockQDIDrv);

}

int qdss_clockQDI_Invoke(int hClientHandle,
			  qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
  switch (method) 
  {
    case QDI_OPEN:
      return qdss_clockQDI_Open(hClientHandle, (void *)pobj, a1.ptr, a2.num);

  case QDSS_CLOCK_QDI_SET:
     return QDSSLocalClkSet((int)a1.ptr);

    case QDI_ISLAND_CHECK_ALLOC:
      return 1;
	  
    default:
      return qurt_qdi_method_default(hClientHandle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);	
  }

  return -1;
}


void qdss_clockQDI_Release ( qurt_qdi_obj_t *qdiobj )
{
  free(qdiobj);
}

int qdss_clock_init(void) 
{
   int nErr=QDSS_CONTROL_ERROR;

   if (qdss_clock.is_initialized)  {
      return QDSS_CONTROL_SUCCESS;
   }

   qurt_mutex_init(&qdss_clock.Mutex);

   TRY(nErr,qurt_qdi_register_devname(qdss_clockQDIName, &qdss_clockQDIOpener));
   nErr = QDSS_CONTROL_SUCCESS;
   qdss_clock.is_initialized = TRUE;
   CATCH(nErr) {}
   return nErr;
}

