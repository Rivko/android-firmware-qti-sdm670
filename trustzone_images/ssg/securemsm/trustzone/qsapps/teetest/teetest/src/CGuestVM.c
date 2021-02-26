/*===========================================================================
Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stringl.h>

#include "object.h"
#include "teetest.h"
#include "IGuestVM.h"
#include "IGuestVM_invoke.h"
#include "IPVC.h"
#include "IEnv.h"
#include "IEnv_invoke.h"
#include "CPVC.h"
#include "CGuestVM.h"
#include "CGuestVM_open.h"
#include "CEnv_open.h"
#include "CUptime.h"
#include "IUptime.h"
#include "teetest_hal_qgic.h"

//------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------
extern int tee_test_secdisp_cmd_hdl(const void *req_ptr, size_t req_len);
static uint32_t FIQ_count = 0;

// Panic codes are TEE specific

#define PANIC_CODE_0 INT32_C(0)
#define PANIC_CODE_1 INT32_C(1)
#define PANIC_CODE_2 INT32_C(2)

typedef enum
{
  TEE_TEST_FIQ_SI_TZ,         //Raise a sw interrupt due to TZ side
  TEE_TEST_FIQ_SI_TEE,        //Raise a sw interrupt due to TEE side
  TEE_TEST_FIQ_TIMER_TEE,     //Raise a timer interrupt due to TEE side
  TEE_TEST_FIQ_COUNT_TEE,     //Get FIQ count result from  TEE side
  TEE_TEST_FIQ_SI_TEE_INIT,   //Fake schedule execution, register FIQ
  TEE_TEST_FIQ_SI_TEE_EXIT,   //Deregister FIQ
  TEE_TEST_EXEC_SD_TEST,      //Execute Secure Display test
  TEE_TEST_PANIC              //Schedule a panic
} GuestVM_schedule_t;


#define CGuestVM_release(ctx)   Object_OK
#define CGuestVM_retain(ctx)    Object_OK

static int32_t CGuestVM_handleCommand(void *me, const void *req_ptr, size_t req_len, const void *rsp_ptr, size_t rsp_len, size_t *rsp_lenout)
{

  int      ret = Object_OK;
  uint32_t registeredIntnum;

  GuestVM_schedule_t *command = (GuestVM_schedule_t *)req_ptr;
  static Object       TEE_Outbound_Object;

  switch (*command)
  {
    case TEE_TEST_FIQ_SI_TZ:
    {
      FIQ_count = 0;

      /* Kick off the interrupt */
      HAL_qgic_InterruptType intnum = TEE_INT_SGI(9);
      HAL_qgic_Generate(intnum, HAL_QGIC_SECURE, HAL_QGIC_TARGET_SELF);
      while (!interrupt_exit) {
        ;
      }
      ret = Object_OK;
      break;
    }

    case TEE_TEST_FIQ_TIMER_TEE:
    {
      if(!Object_isNull(TEE_Outbound_Object))
      {
         FIQ_count = 0;

         /* Kick off the interrupt */
         volatile uint32_t timeout = 10;
         TEST_OK(IPVC_startTimer(TEE_Outbound_Object, timeout, 0, &registeredIntnum))
         while (!interrupt_exit) {
           ;
         }
         ret = Object_OK;
      }
      else
      {
        ret = Object_ERROR;
      }
      break;
    }

    case TEE_TEST_FIQ_SI_TEE_INIT:
    {
      const unsigned char tee_smc_string[] = "tee_smc() reply";

      memscpy((void*)rsp_ptr, rsp_len, tee_smc_string, sizeof(tee_smc_string));
      *rsp_lenout = sizeof(tee_smc_string);

      FIQ_count   = 0;

      /* Prepare an opener object */
      Object TEE_RemoteEnv_Object = CEnv_create(ENV_REMOTE);

      /* IEnv_open Test */
      TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CPVC_UID, &TEE_Outbound_Object))

      /* IPVC_registerInterrupt Test */
      TEST_OK(IPVC_registerInterrupt(TEE_Outbound_Object, TEE_INT_SGI(10), IPVC_INTF_TRIGGER_EDGE, TRUE, &registeredIntnum))
      break;
    }

    case TEE_TEST_FIQ_SI_TEE_EXIT:
    {
      /* Install an interrupt handler */
      if(!Object_isNull(TEE_Outbound_Object))
      {
        /* IPVC_deregisterInterrupt Test */
        TEST_OK(IPVC_deregisterInterrupt(TEE_Outbound_Object, TEE_INT_SGI(10)))

        /* Release the outbound object Test */
        TEST_OK(IEnv_release(TEE_Outbound_Object))
      }
      else
      {
        ret = Object_ERROR;
      }
      break;
    }

    case TEE_TEST_FIQ_SI_TEE:
    {
      /* Kick off the interrupt */
      HAL_qgic_InterruptType intnum = TEE_INT_SGI(10);
      HAL_qgic_Generate(intnum, HAL_QGIC_SECURE, HAL_QGIC_TARGET_SELF);
      while (!interrupt_exit) {
        ;
      }
      break;
    }

    case TEE_TEST_EXEC_SD_TEST:
    {
      TEST_OK(tee_test_secdisp_cmd_hdl(req_ptr, req_len));
      break;
    }

    case TEE_TEST_PANIC:
    {
      /* Prepare an opener object */
      Object TEE_Module_Object = CEnv_create(ENV_REMOTE);
      /* Call the exit function which exists in the IEnv object*/
      TEST_OK(IEnv_exit(TEE_Module_Object, PANIC_CODE_2))

      break;
    }

    case TEE_TEST_FIQ_COUNT_TEE:
    {
      *((uint32_t *)rsp_ptr) = FIQ_count;
      break;
    }

    default:
      ret = Object_ERROR;
      //Unrecognised request;
  }
  return ret;
}

static int32_t CGuestVM_handleInterrupt(void *me, int32_t bpos_val)
{
  FIQ_count++;
  
  Object TEE_Outbound_Object;
  Object TEE_RemoteEnv_Object = CEnv_create(ENV_REMOTE);

  uint64_t upTime;
  int      ret = Object_OK;

  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CUptime_UID, &TEE_Outbound_Object))
  TEST_OK(IUptime_getUptime(TEE_Outbound_Object, &upTime))
  TEST_OK(IUptime_release(TEE_Outbound_Object))

  return Object_OK;
}

static IGuestVM_DEFINE_INVOKE(CGuestVM_invoke, CGuestVM_, void *)


//----------------------------------------------------------------
// Exported functions
//----------------------------------------------------------------
Object CGuestVM_create()
{
  return (Object) { CGuestVM_invoke, NULL };
}


