/*============================================================================
  @file sns_err_handler.c

  @brief
  Error handler functions for Q6 and SDC crashes.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#ifdef SSC_TARGET_SDC_AVAILABLE
#include "sns_hwio_sdc.h"
#include "sns_osa_target.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_sdc.h"

#include "DALDeviceId.h"
#include "DDIIPCInt.h"
#include "DDIInterruptController.h"
#include "err.h"
#include "uIPCInt.h"
#include "uInterruptController.h"

#include "ClockDefs.h"
#include "DDIClock.h"
#include "busywait.h"

/*============================================================================
  Constants
  ============================================================================*/
#define SDC_WDOG_EXPIRED_IRQ "sdc_wdog_expired_irq"

/*============================================================================
  Forward declarations
  ============================================================================*/
extern void *ssc_sdc_vaddr;

/*============================================================================
  Data
  ============================================================================*/
uint8_t sdc_code_dump[SDC_CODE_RAM_SIZE];
uint8_t sdc_data_dump[SDC_DATA_RAM_SIZE];

volatile bool sdc_crash = false;

static DALInterruptID   sdc_wdog_irq;
static ClockIdType      sleep_clk_id;
static DalDeviceHandle *clock_handle;

/*============================================================================
  Helper Functions
  ============================================================================*/
/*
 * sns_err_handler_cb()
 * cb registered with error handling services
 *
 * Stops SDC and saves the status of SDC TCM into DDR.
 *
 */
static void sns_err_handler_cb(void)
{
  void *code_ram_addr, *data_ram_addr;

  code_ram_addr = (void *)((intptr_t)ssc_sdc_vaddr + SDC_CODE_RAM_OFFSET);
  data_ram_addr = (void *)((intptr_t)ssc_sdc_vaddr + SDC_DATA_RAM_OFFSET);

  if (sdc_crash == false)
  {
    int32 uipc_rc;

    uipc_rc = uIPCInt_IsSupported(DALIPCINT_PROC_SPS, DALIPCINT_GP_3);
    if (uipc_rc == UIPCINT_SUCCESS)
    {
      uipc_rc = uIPCInt_Trigger(DALIPCINT_PROC_SPS, DALIPCINT_GP_3);
      if (uipc_rc == UIPCINT_SUCCESS)
      {
        uint32   sdc_wdog_status = 0; 
        uint32_t retry = 0;
        do
        {
          uipc_rc = uInterruptController_IsInterruptPending(sdc_wdog_irq,
                                                            &sdc_wdog_status);
      
          if (sdc_wdog_status)
          {
             break;
          }
          retry++;
        } while ((uipc_rc == UINTERRUPT_SUCCESS) && (retry < 2000));
      }
    }
    else
    {
      //do nothing;
    }
  }

  //Turn SDC TCM on
  sns_sdc_tcm_clk_config(true);

  //save SDC TCM
  memcpy((void *)sdc_code_dump, code_ram_addr, sizeof(sdc_code_dump));
  memcpy((void *)sdc_data_dump, data_ram_addr, sizeof(sdc_data_dump));
}

static void sdc_wdog_expired_isr(void)
{
  sdc_crash = true;

  DalClock_ResetClock(clock_handle, sleep_clk_id, CLOCK_RESET_ASSERT);
  busywait(1000);
  DalClock_ResetClock(clock_handle, sleep_clk_id, CLOCK_RESET_DEASSERT);

  ERR_FATAL("Crash on SDC", 0, 0, 0);
}

/*============================================================================
  Public   Functions
  ============================================================================*/
/**
 * check sns_err.h
 */
void sns_sdc_wdog_enable(void)
{
  DALResult        dal_rc_clk, dal_rc_int;
  DalDeviceHandle *dal_int_handle = NULL;

  sdc_crash = false;

  //Enable sleep clock needed for SDC wdog operation
  dal_rc_clk = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
  if (DAL_SUCCESS == dal_rc_clk)
  {
    dal_rc_clk = DalClock_GetClockId(clock_handle,
                                 "scc_sleep_clk",
                                 &sleep_clk_id);
    if (DAL_SUCCESS == dal_rc_clk)
    {
      DalClock_ResetClock(clock_handle, sleep_clk_id, CLOCK_RESET_ASSERT);
      busywait(1000);
      DalClock_ResetClock(clock_handle, sleep_clk_id, CLOCK_RESET_DEASSERT);
    }
  }

  dal_rc_int = DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                                               &dal_int_handle);
  if (DAL_SUCCESS == dal_rc_int)
  {
    uint32_t irq_config;

    irq_config = DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,
                                              0,  /*< Priority -- autoassign*/
                                              0); /*< non-island mode */

    dal_rc_int = DalInterruptController_GetInterruptID(dal_int_handle,
                                                   SDC_WDOG_EXPIRED_IRQ,
                                                   &sdc_wdog_irq);
    if (DAL_SUCCESS == dal_rc_int)
    {
      dal_rc_int = DalInterruptController_RegisterISR(dal_int_handle,
                                                  sdc_wdog_irq,
                                                  (DALISR)sdc_wdog_expired_isr,
                                                  NULL,
                                                  irq_config);
    }
  }

  if (DAL_SUCCESS != dal_rc_int)
  {
    ERR_FATAL("Failed to register SDC wdog ISR, result %d", dal_rc_int, 0, 0);
  }

  if (DAL_SUCCESS == dal_rc_clk)
  {
    dal_rc_clk = DalClock_EnableClock(clock_handle, sleep_clk_id);
  }

  if (DAL_SUCCESS != dal_rc_clk)
  {
    ERR_FATAL("Failed to Enable sleep clk, result %d", dal_rc_clk, 0, 0);
  }
}

/**
 * check sns_err.h
 */
void sns_err_handler_enable(void)
{
   err_crash_cb_register(sns_err_handler_cb);
}

#endif // SSC_TARGET_SDC_AVAILABLE
