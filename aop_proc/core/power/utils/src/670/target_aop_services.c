/*===========================================================================
                              target_aop_services.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <comdef.h>

#include "aop_fsm.h"
#include "aop_services.h"
#include "aop_interrupt_table.h"
#include "unpa/unpa_resource.h"
#include "icb_aop.h"
#include "HALhwio.h"
#include "aop_hwio.h"
#include "target_aop_qdss.h"
#include "qmp_msg_driver.h"
#include "CoreVerify.h"
#include "time_service.h"
#include "msg_ram_layout.h"
#include "aop_sleep.h"
#include "aop_ulog.h"
#include "pdc_global_hwio.h"
#include "aop_ipc.h"
#include "ChipInfo.h"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
uint32_t do_pdc_change_done (void);
uint32_t do_bcm_workaround(void);
uint32_t do_bcm_change_done(void);
uint32_t do_cx_collapse_lock(void);
uint32_t do_cx_collapse_unlock(void);
uint32_t do_trace_lock(void);
uint32_t do_trace_unlock(void);
uint32_t do_test_command(void);
uint32_t do_watchdog_log(void);
uint32_t do_crash_dump(void);

uint32_t pending_resource_sets;     // Track which resource requests are pending
uint32_t resource_sets_on;          // Track which resource requests are available

static icb_handle_t cnoc_handle = NULL;
static icb_handle_t debug_handle = NULL;
uint32_t cnoc_request;
uint32_t debug_request;
uint32_t cnoc_request_in_cb;
uint32_t debug_request_in_cb;

#define TWENTY_FIVE_MS  (19200 * 25)             // Twenty-Five MS in ticks

#define CX_COLLAPSE_SERVICE
#define QDSS_SUPPORT_SERVICE
#define BCM_WA_SERVICE
#define RAM_DUMP_SERVICE
#define BCM_CHANGE_DONE_SERVICE
#define PDC_WA_SERVICE


static boolean ddr_voted_on = FALSE;
static boolean ddr_completed = FALSE;
static boolean ddr_callback_called = FALSE;
static boolean cnoc_callback_called = FALSE;

/**
 * @brief The following structures are the services provided on this chip.
 *        NOTE: This must be in the same order specified in "enum services"
 *              the code uses the fact that each signal is (1<<position) so it can map
 *              to/from rex signals and array entries efficiently
**/
SERVICE_DESCRIPTOR aop_services[] = {
  //Name        Resource to start,   Current Need, Function            //enum services ...
  { "QUP/MMNC", SERVICE_RESOURCE_CNOC,     0, do_bcm_workaround },     //  BCM_WORKAROUND,     // 0
  { "CXC_LOCK", 0,                         0, do_cx_collapse_lock },   //  CX_COLLAPSE_LOCK,   // 1
  { "CXC_UNLK", 0,                         0, do_cx_collapse_unlock }, //  CX_COLLAPSE_UNLOCK, // 2
  { "DBG_LOCK", 0,                         0, do_trace_lock },         //  DBG_SIG_LOCK,       // 3
  { "DBG_UNLK", 0,                         0, do_trace_unlock },       //  DBG_SIG_UNLOCK,     // 4
  { "TEST_CMD", 0,                         0, do_test_command },       //  QMP_MSG_AVAILABLE,  // 5
  { "WATCHDOG", 0,                         0, do_watchdog_log},        //  LOG_ABORT_ISR,      // 6
  { "CrashDMP", SERVICE_RESOURCE_DDR_W_TO, 0, do_crash_dump },         //  CRASH_DUMP_NEEDED,  // 7
  { "BCM_CHNG", 0,                         0, do_bcm_change_done },    //  BCM_CHANGE_DONE,    // 8
  { "PDC_WA",   0,                         0, do_pdc_change_done },    //  PDC_WA_DONE,        // 9
};                                                                     //  NUM_SERVICES,       // 10


#define LOG_SERVICES
#ifdef LOG_SERVICES
#define NUM_AOP_LOG_ENTRIES  60
volatile uint32 aop_log_index = 0;
uint16 wa_counts[NUM_SERVICES];
struct aop_log_entry {
  uint32        timestamp_lo;
  uint32        message[2];
  uint32        data;
} *aop_log_data = NULL;
uint32 *log_counts;

/**
 * <!-- aop_log_msg32 -->
 *
 * @brief   Internal logging function for debugging
 *
 * @param   message - The 8 byte string to store
 * @param   data    - The 32 bit quantity unique to the message
 *
 * @return  None
 */
void aop_log_msg32(char *message, uint32 data)
{
  struct aop_log_entry *log_ptr;

  uint64_t now = time_service_now();
  uint32 index = aop_log_index++;
  int j;

  index = index % NUM_AOP_LOG_ENTRIES;
  log_ptr = &aop_log_data[index];

  memset(log_ptr, 0xff, sizeof(*log_ptr));
  log_ptr->timestamp_lo = (uint32) (now);
  log_ptr->data = data;
  for (j = 0; j < 2; j++)
  {
    log_ptr->message[j] = message[j*4]         | message[j*4+1] << 8 |
                          message[j*4+2] << 16 | message[j*4+3] << 24;
  }
}
#else
void aop_log_msg32(char *message, uint32 data) {}
#endif
/**
 * <!-- aop_services_waitmask -->
 *
 * @brief Informs the main loop which signals this target needs.
 *
**/
uint32_t aop_services_waitmask(void)
{
  return AOP_SERVICES_WAIT;
}

/**
 * <!-- aop_services_new_message -->
 *
 * @brief Callback from the QMP driver.   Indicates a message is pending.
 *
**/
void aop_services_new_message(void)
{
  rex_set_sigs(&aop_services_tcb, QMP_MESSAGE_RECVD_SIG);
}

/**
 * <!-- aop_services_new_message -->
 *
 * @brief Callback from the timer service.   Indicates a timer has expired.
 *
**/
void aop_servies_timer_expired(uint64 now)
{
  rex_set_sigs(&aop_services_tcb, SERVICES_TIMER_SIGNAL);
}

/**
 * <!-- update_debug_resource -->
 *
 * @brief This function is called when the DDR resource is actually available.
 *        It will update the resource_sets_on variable based on what was
 *        currently requested, and signal that the resource is done if needed.
 *
**/
void update_debug_resource(void)
{
  // Turn on the voted resources
  resource_sets_on |= ((SERVICE_RESOURCE_DDR | SERVICE_RESOURCE_DDR_W_TO) & pending_resource_sets);

  // If we were waiting on the DDR with timeout, we can cancel the timeout.
  if (SERVICE_RESOURCE_DDR_W_TO & pending_resource_sets)
  {
    aop_cancel_timer_cb(aop_servies_timer_expired);
  }
}

/**
 * <!-- aop_services_process -->
 *
 * @brief Called from the main thread to process target specific items.
 *
 * @param[in] rex_signals - The pending rex signals
 * 
**/
void aop_services_process(uint32_t rex_signals)
{
  aop_log_msg32("SIGNALS ", (uint32) rex_signals);
  // Timer signal indicates the DDR w/ TimeOut has timed out.
  if (rex_signals & SERVICES_TIMER_SIGNAL)
  {
    // If it is pending and NOT on, its now marked as on.
    if ((0 != (pending_resource_sets & SERVICE_RESOURCE_DDR_W_TO)) &&
        (0 == (resource_sets_on      & SERVICE_RESOURCE_DDR_W_TO)))
    {
      resource_sets_on |= SERVICE_RESOURCE_DDR_W_TO;
      signal_resource_done();
    }
  }
    if (rex_signals & RESOURCE_SIGNAL)
  {
    if (cnoc_callback_called)
    {
      cnoc_callback_called=FALSE;
      resource_sets_on |= SERVICE_RESOURCE_CNOC;
      aop_log_msg32("CNOC_ON ", (cnoc_request_in_cb << 16 | pending_resource_sets << 8 | resource_sets_on));
    }
    if (ddr_callback_called)
    {
      ddr_callback_called=FALSE;
      update_debug_resource();
      aop_log_msg32(" DDR_ON ", (debug_request_in_cb << 16 | pending_resource_sets << 8 | resource_sets_on));
    }
    // Update the resource
    signal_resource_done();
  }

  // The low 16 bits are signals for workaournds.
  if (rex_signals & 0xFFFF)
  {
    // Service requests and rex signals are 1:1 mapping
    notify_new_service(rex_signals);
  }
}

/**
 * <!-- aop_services_isr -->
 *
 * @brief This is the entry point for interrupts from AOSS to AOP. This
 *        function posts a REX message to the services task for further
 *        processing.
 *
 * @param irq - the interrupt number
 *
 * @return none
 */
void aop_services_isr (int irq)
{
  rex_sigs_type sig = 0;

  // Disable this interrupt so that it's not triggered continuously
  rex_disable_interrupt (irq);

  switch (irq)
  {
    // BCM_SEQ_GEN1 IRQ
    case BCM_IRQ_4:
      sig = BCM_WORKAROUND_SEQ_SIG;
      break;

    case DBG_RSRC_REL_PDC_AOP_LOCK_REQ:
      sig = DBG_SIG_PDC_LOCK_SIG;
      break;

    case DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ:
      sig = DBG_SIG_PDC_UNLOCK_SIG;
      break;

    case CX_COLLAPSE_PDC_AOP_LOCK_REQ:
      sig = CX_COLLAPSE_SIG_LOCK_SIG;
      break;

    case CX_COLLAPSE_PDC_AOP_UNLOCK_REQ:
      sig = CX_COLLAPSE_SIG_UNLOCK_SIG;
      break;

    case AOP_PDC_IRQ_OUT_1:
      watchdog_interrupt_occurred = TRUE;
      sig = CRASH_DUMP_REQUIRED_SIG;
      break;

    // BCM_CP_DONE IRQ
    case BCM_IRQ_1:
      sig = BCM_CHANGE_DONE_SIG;
      break;

    case PDC_AOP_SUMMARY_IRQ:
      sig = PDC_AOP_SUMMARY_SIG;
      break;
      
    default:
      // Probably log and ignore
      break;
  }

  aop_log_msg32("  ISR   ", (irq << 24) | sig);

  rex_set_sigs (&aop_services_tcb, sig);
}

void signal_abort_isr(void)
{
  aop_log_msg32(" ABORT  ", (0xFF << 24) | LOG_WATCHDOG_BITE_SIG);

  watchdog_interrupt_occurred = TRUE;
  rex_set_sigs (&aop_services_tcb, LOG_WATCHDOG_BITE_SIG);
}

/**
 * <!-- do_bcm_workaround -->
 *
 * @brief Performs the BCM workaround for the IRQ
 *
 * @return 0 as no resources are requried.
 * 
**/
uint32_t do_bcm_workaround(void)
{
  uint32_t ret = 0;

#ifdef LOG_SERVICES
  log_counts[BCM_WORKAROUND] = ++wa_counts[BCM_WORKAROUND];
#endif
  if( !icb_handle_aop_service_req (BCM_IRQ_4) )
  {
    /* Abort if we failed internally. */
    ret = SERVICE_RESOURCE_ABORT;
  }
  rex_clear_interrupt (BCM_IRQ_4);
  rex_enable_interrupt (BCM_IRQ_4);

  return ret; 
}

/**
 * <!-- do_bcm_change_done -->
 *
 * @brief Performs the BCM change done for the IRQ
 *
 * @return 0 as no resources are requried.
 * 
**/
uint32_t do_bcm_change_done(void)
{
#ifdef LOG_SERVICES
  log_counts[BCM_CHANGE_DONE] = ++wa_counts[BCM_CHANGE_DONE];
#endif
  icb_handle_aop_service_req (BCM_IRQ_1);
  rex_clear_interrupt (BCM_IRQ_1);
  rex_enable_interrupt (BCM_IRQ_1);

  return 0; // No resource needed afterwards.
}

/**
 * <!-- do_cx_collapse_lock -->
 *
 * @brief  A CX Lock signal has been received.  Ack it immediately
 *
**/
uint32_t do_cx_collapse_lock (void)
{
#ifdef LOG_SERVICES
  log_counts[CX_COLLAPSE_LOCK] = ++wa_counts[CX_COLLAPSE_LOCK];
#endif
  // DeAssert the unlock ack
  HWIO_OUT (AOP_GPO_WDCLR, (1 << 1));
  // Assert the lock ack
  HWIO_OUT (AOP_GPO_WDSET, (1 << 0));

  rex_clear_interrupt (CX_COLLAPSE_PDC_AOP_LOCK_REQ);
  rex_enable_interrupt(CX_COLLAPSE_PDC_AOP_LOCK_REQ);
  return 0;
}

/**
 * <!-- do_cx_collapse_unlock -->
 *
 * @brief A CX Collapse Unlock signal was received.  Ack it immediately
 *
**/
uint32_t do_cx_collapse_unlock(void)
{
#ifdef LOG_SERVICES
  log_counts[CX_COLLAPSE_UNLOCK] = ++wa_counts[CX_COLLAPSE_UNLOCK];
#endif
  // Deassert the Lock Ack
  HWIO_OUT (AOP_GPO_WDCLR, (1 << 0));
  // Assert the unlock ack
  HWIO_OUT (AOP_GPO_WDSET, (1 << 1));
  rex_clear_interrupt (CX_COLLAPSE_PDC_AOP_UNLOCK_REQ);
  rex_enable_interrupt(CX_COLLAPSE_PDC_AOP_UNLOCK_REQ);
  return 0;
}

/**
 * <!-- do_trace_lock -->
 *
 * @brief A Debug Track Lock has been received.  Ask QDSS what to do.
 *        This funcion has a sister of DBG_SIG_UNLOCK.   Keep their resource
 *        needs the same as they require the same resources.
 *
 * @return The services that are required.   
 * 
**/
uint32_t do_trace_lock(void)
{
  uint32_t ret_val = 0;

#ifdef LOG_SERVICES
  log_counts[DBG_SIG_LOCK] = ++wa_counts[DBG_SIG_LOCK];
#endif
  ret_val = aop_qdss_main (DBG_SIG_PDC_LOCK_SIG);
  aop_services[DBG_SIG_UNLOCK].current_resource_sets_needed = ret_val;
  return ret_val;
}


/**
 * <!-- do_trace_unlock -->
 *
 * @brief A Debug Track Lock has been received.  Ask QDSS what to do.
 *        This funcion has a sister of DBG_SIG_UNLOCK.   Keep their resource
 *        needs the same as they require the same resources.
 *
 * @return The services that are required.   
 *
**/
uint32_t do_trace_unlock(void)
{
  uint32_t ret_val = 0;
#ifdef LOG_SERVICES
  log_counts[DBG_SIG_UNLOCK] = ++wa_counts[DBG_SIG_UNLOCK];
#endif
  ret_val = aop_qdss_main (DBG_SIG_PDC_UNLOCK_SIG);

  // Update the sister service's needs too
  aop_services[DBG_SIG_LOCK].current_resource_sets_needed = ret_val;
  return ret_val;
}

/**
 * <!-- do_test_command -->
 *
 * @brief Interpret a message from the QMP driver
 *
**/
uint32_t do_test_command(void)
{
#ifdef LOG_SERVICES
  log_counts[QMP_MSG_AVAILABLE] = ++wa_counts[QMP_MSG_AVAILABLE];
#endif
  service_msg_queue();
  return 0;
}

extern uint32 glob_wd_bite;
extern void log_watchdog_bite();
uint32_t do_watchdog_log(void)
{
#ifdef LOG_SERVICES
  log_counts[LOG_ABORT_ISR] = ++wa_counts[LOG_ABORT_ISR];
#endif
  aop_log_msg32("WDOGBITE", glob_wd_bite);
  log_watchdog_bite();
  return SERVICE_RESOURCE_DDR;   // Keep DDR ON (Forever)
}

/**
 * <!-- do_crash_dump -->
 *
 * @brief Servcie the crash dump
 *
**/
extern void  detect_apps_hang(void);
uint32_t do_crash_dump(void)
{
#ifdef LOG_SERVICES
  log_counts[CRASH_DUMP_NEEDED] = ++wa_counts[CRASH_DUMP_NEEDED];
#endif
  aop_log_msg32("CRASH!!!", ddr_completed);
  
  // Clear the interrupt
  rex_clear_interrupt (AOP_PDC_IRQ_OUT_1);

  SWEVENT(RAMDUMP_FATAL_ERROR, ddr_completed);
  
  // assert aop_gpo[31]
  HWIO_OUT (AOP_GPO_WDSET, (1 << 31));

  // detect_apps_hang is suspected of causing AOSS hang if the
  // registers are not available.  If this occurs, AOP will 
  // not properly record its own state.   Remove detect_apps_hang
  // for now.
  // detect_apps_hang();

  abort();

  // Not reached.
  return SERVICE_RESOURCE_DDR;   // Keep DDR ON (Forever)
}

/**
 * <!-- cnoc_callback -->
 *
 * @brief This function is called when we enable the CNOC interface.
 *
 * @param[in] callback_data - Client data passed as a part of client registration
 * @param[in] req_data      - RPMh request id associated with this callback
 * 
**/
void cnoc_callback(void *callback_data, uint32_t req_data)
{
  cnoc_callback_called = TRUE;
  cnoc_request_in_cb = req_data;
  rex_set_sigs (&aop_services_tcb, RESOURCE_SIGNAL);
}


/**
 * <!-- debug_callback -->
 *
 * @brief This function is called when the DDR resource is available.
 *
 * @param[in] callback_data - Client data passed as a part of client registration
 * @param[in] req_data      - RPMh request id associated with this callback
 * 
**/
void debug_callback(void *callback_data, uint32_t req_data)
{
  ddr_completed = TRUE;
  ddr_callback_called = TRUE;
  debug_request_in_cb = req_data;

  rex_set_sigs (&aop_services_tcb, RESOURCE_SIGNAL);
}

/**
 * <!-- disable_ddr_resource -->
 *
 * @brief This function is called when we want to Disable the DDR resource.
 *        It first makes sure all DDR resources are disabled, and then does so
 *
**/
void disable_ddr_resource()
{
  if (0 == (pending_resource_sets & (SERVICE_RESOURCE_DDR|SERVICE_RESOURCE_DDR_W_TO)))
  {
    ddr_voted_on = FALSE;
    ddr_completed = FALSE;
    aop_log_msg32("DDR  OFF", (pending_resource_sets<< 16 | resource_sets_on));
    debug_request = icb_disable_route(debug_handle, FALSE);
  }
}

/**
 * <!-- enable_ddr_resource -->
 *
 * @brief This function is called when we want to Enable the DDR resource.
 *
**/
void enable_ddr_resource(uint32 request)
{
  if (!ddr_voted_on)
  {
    ddr_voted_on = TRUE;
    debug_request = icb_enable_route(debug_handle, TRUE);
    aop_log_msg32("Enab DDR", debug_request);
    if (debug_request == 0)
    {
      // Zero returned means its ON and there is nothing to do
      debug_callback(NULL, debug_request);
    }
  }
  else
  {
    // DDR is already voted on, if it is actually ON, then we are done, just notify
    // otherwise, wait for the callback or Timeout if applicable
    if (ddr_completed)
    {
      debug_request = 0;
      update_debug_resource();
      signal_resource_done();
    }
  }
}

/**
 * <!-- do_pdc_change_done -->
 *
 * @brief This function is called subsytems are exiting PDC low power mode
 *
**/
uint32 do_pdc_change_num_irq = 0;

uint32 do_pdc_change_done (void)
{
  // Check which subsystem needs to be handled
  uint32 in = HWIO_IN (RPMH_PDC_AOP_IRQ_STATUS_BANK0);
  do_pdc_change_num_irq ++;
  aop_log_msg32 ("PDC_IRQ", in);  
  
  while (in)
  {
    // This is for modem
    if (in & HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_1_BMSK)
    {
      AOP_IPC_SendInterrupt (AOP_IPC_MSS, AOP_IPC_SecondaryInterrupt);
      HWIO_OUTF (RPMH_PDC_AOP_IRQ_CLEAR_BANK0, MODEM_PDC_AOP_IRQ_CLEAR_1, 1);
    }

    // This is for Compute
    if (in & HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_1_BMSK)
    {
      AOP_IPC_SendInterrupt (AOP_IPC_CDSP, AOP_IPC_SecondaryInterrupt);
      HWIO_OUTF (RPMH_PDC_AOP_IRQ_CLEAR_BANK0, COMPUTE_PDC_AOP_IRQ_CLEAR_1, 1);
    }

    // This is for Apps
    if (in & HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_1_BMSK)
    {
      AOP_IPC_SendInterrupt (AOP_IPC_APPS_TZ, AOP_IPC_PrimaryInterrupt);
      HWIO_OUTF (RPMH_PDC_AOP_IRQ_CLEAR_BANK0, APPS_PDC_AOP_IRQ_CLEAR_1, 1);
    }
    
    in = HWIO_IN (RPMH_PDC_AOP_IRQ_STATUS_BANK0);
  }

  rex_enable_interrupt (PDC_AOP_SUMMARY_IRQ);
  return 0;
}

/**
 * <!-- vote_resource_needs -->
 *
 * @brief This function votes on the current needs.
 *
 * @param[in] current_needs - Bitmask requesting the target specific resources.
 * 
**/
void vote_resource_needs(uint32 current_needs)
{
  uint32_t resource_bitmask;
  aop_log_msg32("RSC_NEED", (current_needs << 16 | pending_resource_sets << 8 | resource_sets_on));

  // Foreach resource
  for (resource_bitmask = 1; resource_bitmask <= MAX_SERVICE_RESOURCE; resource_bitmask <<= 1)
  {
    boolean voted  = ((pending_resource_sets & resource_bitmask) != 0)?TRUE:FALSE;
    boolean needed = ((current_needs         & resource_bitmask) != 0)?TRUE:FALSE;

    if (voted && !needed)
    {
      // Turn it off
      pending_resource_sets &= ~resource_bitmask;
      resource_sets_on      &= ~resource_bitmask;
      if (resource_bitmask == SERVICE_RESOURCE_CNOC)
      {
        aop_log_msg32("CNOC OFF", (pending_resource_sets<< 16 | resource_sets_on));
        cnoc_request = icb_disable_route(cnoc_handle, FALSE);
      }
      else if ((resource_bitmask == SERVICE_RESOURCE_DDR) ||
               (resource_bitmask == SERVICE_RESOURCE_DDR_W_TO))
      {
        // This function ensures all resources who need DDR are off
        disable_ddr_resource();
      }
    }
    else if (!voted && needed)
    {
      // Turn it on
      pending_resource_sets |= resource_bitmask;
      if (resource_bitmask == SERVICE_RESOURCE_CNOC)
      {
        cnoc_request = icb_enable_route(cnoc_handle, TRUE);
        aop_log_msg32("EnabCNOC", cnoc_request);
        if (cnoc_request == 0)
        {
          cnoc_callback(NULL, cnoc_request);
        }
      }
      else if (resource_bitmask == SERVICE_RESOURCE_DDR)
      {
        // Only vote it ON if we didn't already vote it on for DDR w/TO
        enable_ddr_resource(resource_bitmask);
      }
      else if (resource_bitmask == SERVICE_RESOURCE_DDR_W_TO)
      {
        // Start a 25MS timer.  In the error case scenario, PMIC may not be
        // responding properly to VRM.  In this case, there is a 4ms timer
        // for responses to be sent back to the the ARC.  Thus, DDR in the
        // error case may require multiple responses from ARCs at 4ms each.
        // This allows 8ms for those responses, but 2ms addional to bring 
        // DDR up and then, times two.
        aop_register_timer_cb(TWENTY_FIVE_MS, aop_servies_timer_expired);
        enable_ddr_resource(resource_bitmask);
      }
      else if (resource_bitmask == SERVICE_RESOURCE_ABORT)
      {
        abort();
      }
    }
  }
}

/**
 * <!-- target_aop_service_init -->
 *
 * @brief Initialize the Target specific needs for the workarounds.   Basically, this function
 *        sets up the services structure for the common code, and registers all the interrupts.
 *
**/
void target_aop_service_init(void)
{
#ifdef LOG_SERVICES
  aop_log_data = (struct aop_log_entry *) msg_ram_get_aop_logging_area();
  log_counts = (uint32 *) &aop_log_data[NUM_AOP_LOG_ENTRIES];
  memset((void *) aop_log_data, 0, 1024);
#endif
  CORE_VERIFY(cnoc_handle = icb_get_handle_ex(ICBID_MASTER_AOSS,ICBID_SLAVE_DCC_CFG, cnoc_callback, NULL));
  CORE_VERIFY(debug_handle = icb_get_handle_ex(ICBID_MASTER_AOSS, ICBID_SLAVE_EBI1,  debug_callback, NULL));

  aop_services_ptr = aop_services;
  aop_services_count = sizeof(aop_services)/sizeof(aop_services[0]);

  aop_log_msg32("SERV_PTR", (uint32) aop_services_ptr);
  aop_log_msg32("SERV_CNT", (uint32) aop_services_count);
#ifdef CX_COLLAPSE_SERVICE
  rex_set_interrupt_vector (CX_COLLAPSE_PDC_AOP_LOCK_REQ, aop_services_isr);
  rex_clear_interrupt      (CX_COLLAPSE_PDC_AOP_LOCK_REQ);
  rex_enable_interrupt     (CX_COLLAPSE_PDC_AOP_LOCK_REQ);

  rex_set_interrupt_vector (CX_COLLAPSE_PDC_AOP_UNLOCK_REQ, aop_services_isr);
  rex_clear_interrupt      (CX_COLLAPSE_PDC_AOP_UNLOCK_REQ);
  rex_enable_interrupt     (CX_COLLAPSE_PDC_AOP_UNLOCK_REQ);
#endif
#ifdef QDSS_SUPPORT_SERVICE
  // Debug PDC wrapper
  rex_set_interrupt_vector (DBG_RSRC_REL_PDC_AOP_LOCK_REQ, aop_services_isr);
  rex_set_interrupt_vector (DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ, aop_services_isr);
  rex_clear_interrupt (DBG_RSRC_REL_PDC_AOP_LOCK_REQ);
  rex_clear_interrupt (DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ);

  rex_enable_interrupt (DBG_RSRC_REL_PDC_AOP_LOCK_REQ);
#endif
  // BCM workaround IRQ
#ifdef BCM_WA_SERVICE
  rex_set_interrupt_vector (BCM_IRQ_4, aop_services_isr);
  rex_clear_interrupt (BCM_IRQ_4);
  rex_enable_interrupt (BCM_IRQ_4);
#endif
#ifdef BCM_CHANGE_DONE_SERVICE
  rex_set_interrupt_vector (BCM_IRQ_1, aop_services_isr);
  rex_clear_interrupt (BCM_IRQ_1);
  rex_enable_interrupt (BCM_IRQ_1);
#endif
#ifdef RAM_DUMP_SERVICE
  rex_set_interrupt_vector(AOP_PDC_IRQ_OUT_1, (rex_isr_func_type)aop_services_isr);
  rex_clear_interrupt (AOP_PDC_IRQ_OUT_1);
  rex_enable_interrupt(AOP_PDC_IRQ_OUT_1);
#endif

  rex_set_interrupt_vector (PDC_AOP_SUMMARY_IRQ, aop_services_isr);
  rex_clear_interrupt (PDC_AOP_SUMMARY_IRQ);
  rex_enable_interrupt (PDC_AOP_SUMMARY_IRQ);
}
