/*===========================================================================
  FILE: tracer_portmgr.c

  OVERVIEW: Tracer management of a pool of STM ports.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_portmgr.c#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "DALStdErr.h"

#include "tracer_portmgr.h"
#include "tracer_stp.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
// Must successfully obtain a trash and a d32 STM port before
// port manager is considered initialized.
boolean tracer_portmgr_init_impl(struct portMgr_s *portMgr,
                                 DALDEVICEHANDLE stm_handle)
{
   int ii;

   portMgr->nextKey = 0;
   portMgr->inUseCount = 0;
   portMgr->inUseHiMark = 0;

   if (DAL_SUCCESS != DalSTMTrace_NewPort(stm_handle,
                                          &(portMgr->trashPort)))
   {
      portMgr->bInitialized = FALSE;
      return portMgr->bInitialized;
   }
   if (DAL_SUCCESS != DalSTMTrace_NewPort(stm_handle,
                                          &(portMgr->d32Port)))
   {
      DalSTMTrace_FreePort(stm_handle, portMgr->trashPort);
      portMgr->bInitialized = FALSE;
      return portMgr->bInitialized;
   }
   for (ii = 0; ii < TRACER_PORT_POOL_SIZE; ii++)
   {
      if (DAL_SUCCESS != DalSTMTrace_NewPort(stm_handle,
                                             &(portMgr->pool[ii].port)))
      {
         portMgr->pool[ii].inUseFlag = TRUE;
         portMgr->inUseCount++;
      }
      else
      {
         portMgr->pool[ii].inUseFlag = FALSE;
      }
   }
   if (TRACER_PORT_POOL_SIZE >portMgr->inUseCount)
   {
      portMgr->bInitialized = TRUE;   // Consider initialized
   }
   else
   {
      portMgr->bInitialized = FALSE;  // Unable to obtain any ports.
   }
   return portMgr->bInitialized;
}

boolean tracer_portmgr_init(void)
{
   if (!tracer_portmgr_init_impl(&gPortMgr,tracer_stp_get_hStm())){
      return FALSE;
   }

   if (!tracer_portmgr_init_impl(&gPortMgrMicro,tracer_stp_get_hStmSLPI())){
      return FALSE;
   }
   return TRUE;
}


void tracer_portmgr_deinit_impl(struct portMgr_s *portMgr,
                                DALDEVICEHANDLE stm_handle)
{
   int ii;
   if (TRUE == portMgr->bInitialized)
   {
      portMgr->bInitialized = FALSE;

      for (ii = 0; ii < TRACER_PORT_POOL_SIZE; ii++)
      {
         portMgr->pool[ii].inUseFlag = FALSE;
         if (0 != portMgr->pool[ii].port)
         {
            DalSTMTrace_FreePort(stm_handle, portMgr->pool[ii].port);
         }
      }
      DalSTMTrace_FreePort(stm_handle, portMgr->trashPort);
      DalSTMTrace_FreePort(stm_handle, portMgr->d32Port);
   }

   return;
}

void tracer_portmgr_deinit(void)
{
   tracer_portmgr_deinit_impl(&gPortMgr,tracer_stp_get_hStm());
   tracer_portmgr_init_impl(&gPortMgrMicro,tracer_stp_get_hStmSLPI());
}


//=========================================================================//
int tracer_port_himark(void)
{
   return (gPortMgr.inUseHiMark > gPortMgrMicro.inUseHiMark) ? gPortMgr.inUseHiMark :  gPortMgrMicro.inUseHiMark;
}

