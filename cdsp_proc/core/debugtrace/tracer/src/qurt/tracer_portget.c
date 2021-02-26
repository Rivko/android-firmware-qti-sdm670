/*===========================================================================
  FILE: tracer_portget.c

  OVERVIEW: Get a STM port from the port pool. Extension of tracer_portmgr.c

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_atomic.h"
#include "tracer_portmgr.h"
#include "qurt_island.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

//=========================================================================//
//Caller can check that port manager is initialized before calling this
//function.
STMTracePortType tracer_port_get_impl(struct portMgr_s *portMgr,int *key)
{
   int myCount = 0;  //Escape route.
   int myKey;
   int newKey;

   if (TRUE == portMgr->bInitialized) // Additional safeguard. Needed?
   {
      do
      {
         myCount++;
         if (myCount > TRACER_PORT_POOL_SIZE)
         {  // Get out if can't find a port
            *key = ~0;
            return (portMgr->trashPort);
         }

         // Get next port from pool.
         do
         {
            myKey = portMgr->nextKey;
            newKey = ((myKey + 1) >= TRACER_PORT_POOL_SIZE) ?
               0 : (myKey + 1);

         } while (!TRACER_ATOMIC_CAS(&portMgr->nextKey, myKey, newKey));

      // Verify the port is not already in use.
      } while (!TRACER_ATOMIC_CAS(&(portMgr->pool[myKey].inUseFlag), 0, 1));

      // Track usage
      TRACER_ATOMIC_INC (&(portMgr->inUseCount));
      if (portMgr->inUseHiMark < portMgr->inUseCount)
      {
         TRACER_ATOMIC_SET (&(portMgr->inUseHiMark),portMgr->inUseCount);
      }

      *key = myKey;  // Return key value
      return (portMgr->pool[*key].port); // Return port number.
   }
   else
   {
      *key = ~0;
      return 0;
   }
}

STMTracePortType tracer_port_get(int *key)
{

   if (tracer_log_all_to_ssc_qdss) {
     return tracer_port_get_impl(&gPortMgrMicro,key);
   }
   else {

      if (0==qurt_island_get_status()) {
         /*Not in island mode */
         return tracer_port_get_impl(&gPortMgr,key);
      }
      else {
         return tracer_port_get_impl(&gPortMgrMicro,key);
      }
  }
}

//=========================================================================//
void tracer_port_free_impl(struct portMgr_s *portMgr,int key)
{
   if (~0 != key)
   {
      TRACER_ATOMIC_SET (&(portMgr->pool[key].inUseFlag), 0);
      TRACER_ATOMIC_DEC (&(portMgr->inUseCount));
   }
   return;
}

void tracer_port_free(int key)
{
 if (tracer_log_all_to_ssc_qdss) {
     return tracer_port_free_impl(&gPortMgrMicro,key);
 }
 else {
    if (0==qurt_island_get_status()) {
       /*Not in island mode */
       return tracer_port_free_impl(&gPortMgr,key);
    }
    else {
       return tracer_port_free_impl(&gPortMgrMicro,key);
    }
 }
}


