/*===========================================================================

           S Y S _ M _SMSM _ MPSS. C

DESCRIPTION

Copyright (c) 2011-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/sys_m_smsm_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m_smsm.h"
#include "sys_m.h"
#include "rcinit.h"
#include "err.h"
#include "smp2p.h"
#include "DALSys.h"
#include "npa_resource.h"


#define SYS_M_HOSTS 2
#define SYS_M_HOST_APSS 0
#define SYS_M_HOST_TZ 1

uint32 sys_m_smp2p_cached_init_state = 0;
 
smp2p_context_type *sys_m_smsm_tx_context = 0;
smp2p_context_type *sys_m_smsm_rx_context[SYS_M_HOSTS];
uint32 sys_m_rx_state = 0;
uint32 sys_m_tx_state = 0;

static qurt_mutex_t sys_m_smsm_lock_mutex;
static int sys_m_smsm_lock_inited = 0;

extern int sys_m_forced_shutdown;

static inline void SYS_M_SMSM_LOCK_INIT(void)
{
   if (!sys_m_smsm_lock_inited)
   {
      qurt_pimutex_init(&sys_m_smsm_lock_mutex);

      sys_m_smsm_lock_inited = 1;
   }
}

static inline void SYS_M_SMSM_LOCK(void)
{
   if (sys_m_smsm_lock_inited)
   {
      qurt_pimutex_lock(&sys_m_smsm_lock_mutex);
   }
}

static inline void SYS_M_SMSM_UNLOCK(void)
{
   if (sys_m_smsm_lock_inited)
   {
      qurt_pimutex_unlock(&sys_m_smsm_lock_mutex);
   }
}

/**
  Sets bit in sys_m point to point array to apps

  @return
  Error codes

  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set(uint32 bits)
{
   int32 smp2p_return_val = 0, sys_m_smp2p_return_val = 0;

   if (sys_m_smsm_tx_context == 0)
      return SYS_M_SMP2P_NOT_READY;

   //write update to array
   sys_m_tx_state |= bits;
   smp2p_return_val = smp2p_write(sys_m_smsm_tx_context, sys_m_tx_state);
   switch(smp2p_return_val)
   {
      case SMP2P_STATUS_SUCCESS:
         sys_m_smp2p_return_val = SYS_M_SMP2P_SUCCESS;
         break;
      case SMP2P_STATUS_NOT_INIT:
         sys_m_smp2p_return_val = SYS_M_SMP2P_NOT_READY;
         break;
      case SMP2P_STATUS_INVALID_PARAM:
         sys_m_smp2p_return_val = SYS_M_SMP2P_INVALID_PARAM;
         break;
      default:
         ERR_FATAL("smp2p_write returning invalid code", 0, 0, 0);
         break;
   }

   return sys_m_smp2p_return_val;
}

/**
  Single threaded mode version of above (no mutex operations) 
  Sets bit in sys_m point to point array to apps 
                        
  @return
  Error codes
  
  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set_stm(uint32 bits)
{
    int32 smp2p_return_val = 0, sys_m_smp2p_return_val = 0;
    
    if (sys_m_smsm_tx_context == 0)
        return SYS_M_SMP2P_NOT_READY;

    //write update to array (not needed, but preserves state for debugging)
    sys_m_tx_state |= bits;

    smp2p_return_val = smp2p_modify_ex(sys_m_smsm_tx_context, 0, bits, SMP2P_FLAGS_STM);

    switch(smp2p_return_val)
    {
        case SMP2P_STATUS_SUCCESS:
            sys_m_smp2p_return_val = SYS_M_SMP2P_SUCCESS;
            break;
        case SMP2P_STATUS_NOT_INIT:
            sys_m_smp2p_return_val = SYS_M_SMP2P_NOT_READY;
            break;
        case SMP2P_STATUS_INVALID_PARAM:
            sys_m_smp2p_return_val = SYS_M_SMP2P_INVALID_PARAM;
            break;
        default:
            ERR_FATAL("smp2p_modify_ex returning invalid code", 0, 0, 0);
            break;
    }
    
    return sys_m_smp2p_return_val;
}

/**
  Sets bit in sys_m point to point array to apps
  If sys_m_smsm_tx_context is not ready, then set sys_m_smp2p_cached_init_state flag

  @return
   SYS_M_SMP2P_SUCCESS : successfully set or cached the bit

  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set_cached(SYS_M_SUBSYS2AP_SMSM_BITS bits)
{
   int32 sys_m_smp2p_return_val = 0;

   SYS_M_SMSM_LOCK();
   sys_m_smp2p_return_val = sys_m_smsm_apps_set(bits);

   if (sys_m_smp2p_return_val == SYS_M_SMP2P_NOT_READY)
   {
      // sys_m_smsm_tx_context is not yet ready to set the bits.
      // Set the bits in sys_m_smsm_task task once sys_m_smsm_tx_context is ready/

      sys_m_smp2p_cached_init_state |= bits;

   }
   else if(sys_m_smp2p_return_val == SYS_M_SMP2P_INVALID_PARAM)
   {
     ERR_FATAL("Sys_m cannot set SMP2P bit", 0, 0, 0);
   }
   SYS_M_SMSM_UNLOCK();
   return SYS_M_SMP2P_SUCCESS;
}


/**
 *
 * @brief sys_m_smsm_init
 *
 * System monitor SMSM initialization function
 *
*/
void sys_m_smsm_init(void)
{
   SYS_M_SMSM_LOCK_INIT();
}

/**
 *
 * @brief sys_m_smsm_task
 *
 * System Monitor SMSM handling task
 *
 * @param[in] hEvent: Task init paramter
 * @param[in] tid: task id
 *
*/
DALResult sys_m_smsm_task(DALSYSEventHandle hEvent, void* tid)
{
    int32 return_val;
    uint32 rx_state_received;
    uint32 rx_state_change;
    uint32 idx;

    //block for start signal
    rcinit_handshake_startup();
    //create outbound
    return_val = smp2p_create(&sys_m_smsm_tx_context, SMEM_APPS, SYS_M_SMP2P_PORT_OUT);
    if (return_val != SMP2P_STATUS_SUCCESS && return_val != SMP2P_STATUS_NOT_INIT)
        ERR_FATAL("Sys_m cannot create outbound SMP2P", return_val, 0, 0);
     
    //wait for outbound readiness 
    if (return_val == SMP2P_STATUS_NOT_INIT)
    {
        return_val = smp2p_wait(sys_m_smsm_tx_context);
        if (return_val == SMP2P_STATUS_FAILURE)
            ERR_FATAL("Sys_m cannot wait outbound SMP2P", return_val, 0, 0);
    }
    
    npa_define_marker("/ssr/smp2p/inited");
    
    //register to listen on inbound 
    return_val = smp2p_register(&sys_m_smsm_rx_context[SYS_M_HOST_APSS], SMEM_APPS, SYS_M_SMP2P_PORT_IN);
    if (return_val != SMP2P_STATUS_SUCCESS && return_val != SMP2P_STATUS_NOT_FOUND)
        ERR_FATAL("Sys_m cannot listen on SMP2P", return_val, 0, 0);



    return_val = smp2p_register(&sys_m_smsm_rx_context[SYS_M_HOST_TZ], SMEM_TZ, SYS_M_SMP2P_TZ_PORT_IN);
    if (return_val != SMP2P_STATUS_SUCCESS && return_val != SMP2P_STATUS_NOT_FOUND)
        ERR_FATAL("Sys_m cannot listen on SMP2P", return_val, 0, 0);

    SYS_M_SMSM_LOCK();
    // sys_m_tx_state will hold the already "set" smp2p bits done before this
    // sys_m_smp2p_cached_init_state will OR with sys_m_tx_state to set additional smp2p bits if any
    if(sys_m_smp2p_cached_init_state)
    {
        if (sys_m_smsm_apps_set(sys_m_smp2p_cached_init_state) != SYS_M_SMP2P_SUCCESS)
         {
             ERR_FATAL("Sys_m cannot set SMP2P bit", 0, 0, 0);
         }
    }
    SYS_M_SMSM_UNLOCK();

    //task forever loop
    for (;;)
    {
        return_val = smp2p_wait_multiple(sys_m_smsm_rx_context, SYS_M_HOSTS, &idx);
        if (return_val == SMP2P_STATUS_FAILURE)
            ERR_FATAL("Sys_m cannot wait for SMP2P", return_val, 0, 0);

        if(idx >= SYS_M_HOSTS){
            ERR_FATAL("idx is invalid = %d",idx,0,0);
        }
        //attempt to read data
        return_val = smp2p_read(sys_m_smsm_rx_context[idx], &rx_state_received);
        
        //if state has changed, act appropriately
        rx_state_change = rx_state_received ^ sys_m_rx_state;
        //update global state if it has changed
        if (rx_state_change != 0)
            sys_m_rx_state = rx_state_received;

        if((SYS_M_HOST_TZ == idx) && ((rx_state_change & SYS_M_AP2SUBSYS_SMSM_ERRFATAL)))
        {
            ERR_FATAL("err fatal notification received from TZ ", 0, 0, 0);
        }

        //error fatal
        if ((SYS_M_HOST_APSS == idx)&&(rx_state_change & SYS_M_AP2SUBSYS_SMSM_ERRFATAL))
        {
            sys_m_forced_shutdown = 1;
            ERR_FATAL("APPS force stop", 0, 0, 0);
        }
        //graceful shutdown
        if((SYS_M_HOST_APSS == idx)&& (rx_state_change & SYS_M_AP2SUBSYS_SMSM_SHUT_DWN))
        {
            sys_m_shutdown(SYS_M_SMSM_P2P);
        }
    }
}


