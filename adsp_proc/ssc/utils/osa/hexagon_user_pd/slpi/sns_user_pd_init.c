/*=============================================================================
  @file sns_user_pd_init.c

  @brief
  PD init for Sensors Root PD.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  INCLUDES
  ===========================================================================*/

#include "Diag_LSM.h"
#include "diag_uimage.h"
#include "msg_diag_service.h"
#define DBG_MED_PRIO    2

#include "sns_assert.h"
#include "sns_fw_sensor.h"
#include "sns_fw_diag_service.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_timer.h"
#include "sns_types.h"

#ifdef SSC_TARGET_PRAM_AVAILABLE
#include "pram_mgr.h"
#endif /* SSC_TARGET_PRAM_AVAILABLE */

/*=============================================================================
  Forward Declarations
  ===========================================================================*/
extern void sns_fw_init(void);
extern void sns_timer_init(void);
extern void sns_sdc_init(void);

/*=============================================================================
  Static Data
  ===========================================================================*/
static bool sns_init_done = false;

volatile uint64_t sns_assert_line;
volatile const char* sns_assert_file;

/*=============================================================================
  Public Functions
  ===========================================================================*/

sns_rc sns_user_pd_init()
{
  if(false == sns_init_done)
  {
#ifdef SSC_TARGET_PRAM_AVAILABLE
    // Map in PRAM memory
    sns_pram_addr = pram_acquire_partition(SNS_HEAP_PRAM_NAME, &sns_pram_size);
    // Initialize our Heaps
    sns_heap_init(sns_pram_addr);

    // Adjust the pram_addr and size to take into account the amount used
    // by the heap
    SNS_ASSERT(sns_pram_size > SNS_PRAM_HEAP_SIZE);
    sns_pram_addr = (uint8_t*)sns_pram_addr + SNS_PRAM_HEAP_SIZE;
    sns_pram_size -= SNS_PRAM_HEAP_SIZE;
#else
    sns_pram_addr = NULL;
    sns_pram_size = 0;
    sns_heap_init( NULL );
#endif

/* If enabled, this will delay the framework initialization by 10 seconds.
   This is to easily capture init messages when SSC boots up */
#if defined(SNS_DELAY_INIT)
    const sns_time one_second_in_ticks = 19200000ULL;
    for(int i = 10; i > 0; i--)
    {
      MSG_1(MSG_SSID_SNS, DBG_MED_PRIO, "init countdown %d ", i);
      /* sns_busy_wait is implemented as a sleep() */
      sns_busy_wait(one_second_in_ticks);
    }
#endif

    sns_diag_fw_preinit();

    //Init the F3 trace buffer
    void *sns_f3_trace_buffer = sns_malloc(SNS_HEAP_MAIN, SNS_F3_TRACE_SIZE);
    if(NULL != sns_f3_trace_buffer)
    {
      diag_lsm_f3_trace_init( sns_f3_trace_buffer, SNS_F3_TRACE_SIZE );
    }
    // Set diag to exit island mode when island buffer is full
    diag_set_uimage_tx_mode(DIAG_TX_MODE_BUFFERED_THRESH);

    //Map SDC TCM region
    sns_sdc_init();

    sns_osa_init();

    sns_timer_init();
    sns_fw_init();
    sns_heap_init_delayed();
    sns_init_done = true;
    return SNS_RC_SUCCESS;
  }
  return SNS_RC_FAILED;
}
