/*===========================================================================
                              aop_sleep.c

DESCRIPTION:

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rex.h"
#include "aop_interrupt_table.h"
#include "msmhwio.h"
#include "aop_hwio.h"
#include "time_service.h"
#include "pdc_seq.h"
#include "CoreVerify.h"
#include "rpmh_common.h"
#include "arc_rm_hwio.h"
#include "aoss_apm.h"
#include "ssc_apm.h"
#include "rsc.h"
#include "dog.h"
#include "clock.h"
#include "aop_sleep_stats.h"
#ifdef SDM855_TARGET_CONFIG
  #include "../../systemdrivers/clock/hw/sdm855/inc/ClockHWIO.h"
#else
  #include "../../systemdrivers/clock/hw/sdm845/inc/ClockHWIO.h"
#endif  
#include "aoss_misc_hwio.h"
#include "qmp_msg_driver.h"
#include "arc_hal_hwio.h"
#include "hwio_arc_vt.h"
#include "rpmh_common.h"
#include "rpmh_arc.h"
#include "aop_ulog.h"
#include "aop_sleep.h"
#include "drv_lookup.h"
#include "unpa/unpa_resource.h"
#include "pwr_utils_lvl.h"
#include "pdcHal_hwio.h"
#include "ChipInfo.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

#define OPERATIONAL_LVL     3
#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  uint64 now;
  uint64 scheduled;
  bool rude;
} sleep_history_t;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
uint32 g_num_halt = 0;
uint32 g_temp_wakeup_time = 10 * 19200000;
uint32 g_pdc_backoff_time = 9600; // 500us  - Time to exit AOSS sleep
uint32 g_pdc_threshold_time = 28800; // 1.5ms - Time it would be efficient to enter AOSS Sleep
uint32 g_allow_aoss_sleep = 1;

bool g_rude_wakeup = false;

sleep_history_t sleep_history[16];
uint32 sleep_history_index;
bool g_aoss_sleep_locked = FALSE;  // Global set to True during AOSS Sleep Lock state

#if 0
extern void update_pad(uint32);
extern uint32 get_pad(void);
#endif

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static aop_sleep_state_enum_type aop_sleep_state = AOP_SLEEP_IDLE;
static pdc_seq_handle aop_pdc_handle;
static pdc_seq_handle dbg_pdc_handle;
static rsc_handle aop_rsc_handle;
static rex_crit_sect_type timer_crit_sect;

static uint32 aosspwrupreq = 0;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern rex_tcb_type main_idle_tcb;
extern uint32 glob_wd_bite;
void rex_interrupt_soft_trigger(rex_vect_type v);
extern void bcm_service_workaround(int irq);
extern bool is_arc_operational (rpmh_arc_enum_t rm);
extern bool is_arc_operational2 (rpmh_arc_enum_t rm);
extern bool rex_is_interrupt_pending (rex_vect_type v);
extern void zero_degree_c_update(void);

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- pmux_init -->
 *
 * @brief Initializes the power mux for AOSS sleep
 *
 * @return none
 */
__attribute__((section("sleep_code_sec")))
static void pmux_init (void)
{
  uint32 rdata;
  
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);
  HWIO_OUT (APM_SW_OVERRIDE, 0);

  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT,TILE_ACT_BR,0x1);
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_CONFIG_0, APM_CONFIG, 0xC010);

  HWIO_OUT (VDD_SSCMX_APM_CONTROLLER_CONFIG_1, 0x05052805);
  HWIO_OUT (VDD_SSCMX_APM_CONTROLLER_CONFIG_2, 0x05050528);
  
  HWIO_OUT (VDD_SSCMX_APM_CONTROLLER_CONFIG_5, 0x3);
  HWIO_OUT (VDD_SSCMX_APM_CONTROLLER_CONFIG_8, 0x3);
  
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT,ENABLE_NORMAL_OP,0x1);
 
 
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT,TILE_ACT_BR,0x1);

  //Below settings are in sync with napali v2
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_5, APM2TILE_DOM2AUX_FDBK_CONFIG_SEL, 0x0);
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_8, APM2TILE_AUX2DOM_FDBK_CONFIG_SEL, 0x0);
  
  rdata = HWIO_INF (VDD_AOSS_APM_CONTROLLER_CONFIG_0,APM_CONFIG);
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_0, APM_CONFIG, (rdata | 0xC010));
 
  HWIO_OUT (VDD_AOSS_APM_CONTROLLER_CONFIG_1, 0x05052805);
  HWIO_OUT (VDD_AOSS_APM_CONTROLLER_CONFIG_2, 0x05050528);
 
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT, ENABLE_NORMAL_OP, 0x1);
 
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
  HWIO_OUT (APM_SW_OVERRIDE, 7);
}

/**
 * <!-- pmux_configure_sleep -->
 *
 * @brief Configures the power mux for entry into AOSS sleep
 *
 * @return none
 */
 __attribute__((section("sleep_code_sec")))
static void pmux_configure_sleep (void)
{
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);
  // HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_DOM, 1);
  // HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_DOM, 1);
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS_VAL, 1);   
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS, 1);   
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS_VAL, 1);   
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS, 1);
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT, EN_COMP, 0);
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT, EN_COMP, 0);
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
}

/**
 * <!-- pmux_configure_wake -->
 *
 * @brief Configures the power mux for exit from AOSS sleep
 *
 * @return none
 */
__attribute__((section("sleep_code_sec")))
static void pmux_configure_wake (void)
{
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);
  // The pmux clock is turned on as part of Clock_ProcessorRestore
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT, EN_COMP, 1);
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT, EN_COMP, 1);
  // HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_DOM, 0);
  // HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_DOM, 0);
  HWIO_OUTF (VDD_SSCMX_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS, 0);   
  HWIO_OUTF (VDD_AOSS_APM_CONTROLLER_CONFIG_7, CTRL2TILE_COMP_OUT_BYPASS, 0);
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
}

/**
 * <!-- aop_sleep_isr -->
 *
 * @brief This is the entry point for interrupts from AOSS to AOP. This
 *        function posts a REX message to the main idle task for further
 *        processing.
 *
 * @param irq - the interrupt number
 *
 * @return none
 */
void aop_sleep_isr (int irq)
{
  rex_sigs_type sig = 0;

  // Disable this interrupt so that it's not triggered continuously
  rex_disable_interrupt (irq);
  
  switch (irq)
  {
    case SOC_SLEEP_PDC_AOP_LOCK_REQ:
      sig = AOP_SLEEP_SIG_PDC_LOCK_REQ;
      break;

    case RPMH_IDLE:
      sig = AOP_SLEEP_SIG_RPMH_IDLE;
      break;

    case SOC_SLEEP_PDC_AOP_UNLOCK_REQ:
      sig = AOP_SLEEP_SIG_PDC_UNLOCK_REQ;
      break;

    default:
      // Probably log and ignore
      break;
  }

  rex_set_sigs (&main_idle_tcb, sig);
}

/**
 * <!-- halt -->
 *
 * @brief Low level assembly function to issue WFI
 *
 * @return none
 */
__asm static void halt(void)
{
  /* Lock interrupts via primask instead of basepri during sleep, or else we never wake up. */
  asm("mrs r0, basepri");
  asm("cpsid i");
  asm("mov r1, #0");
  asm("msr basepri, r1");

  /* Do the halt, using ISB to clear the M3 store buffer */
  asm("dsb");
  asm("isb");
  asm("wfi");

  /* Revert interrupt lock to basepri-based. */
  asm("msr basepri, r0");
  asm("cpsie i");
  asm("bx r14");
}

void aop_timer_init(void)
{
  rex_init_crit_sect(&timer_crit_sect);  
}

/**
 * <!-- aop_sleep_init -->
 *
 * @brief Initialization routine for AOSS sleep
 *
 * @return none
 */
__attribute__((section("sleep_code_sec")))
void aop_sleep_init (void)
{
  aop_timer_init();
  rex_set_interrupt_vector (SOC_SLEEP_PDC_AOP_LOCK_REQ, aop_sleep_isr);
  rex_set_interrupt_vector (RPMH_IDLE, aop_sleep_isr);
  rex_set_interrupt_vector (SOC_SLEEP_PDC_AOP_UNLOCK_REQ, aop_sleep_isr);

  rex_clear_interrupt (SOC_SLEEP_PDC_AOP_LOCK_REQ);
  rex_clear_interrupt (RPMH_IDLE);
  rex_clear_interrupt (SOC_SLEEP_PDC_AOP_UNLOCK_REQ);

  // Turn the proc clock off, but not the bus clock
  HWIO_OUTF (AOP_WFI_CONFIG, PROC_CLK_HALT, 1);
  HWIO_OUTF (AOP_WFI_CONFIG, BUS_CLK_HALT, 0);
  HWIO_OUTF (AOP_WFI_CONFIG, CHIP_SLEEP_UPON_WFI, 0);

  // Enable the QTimer interrupt
  rex_clear_interrupt (AOP_QTIMER_INTR_0);
  // rex_enable_interrupt (AOP_QTIMER_INTR_0);

  // Initialize the RSC sequences
  rsc_driver_init ();

  CORE_VERIFY (rsc_get_handles ("aop", 1, &aop_rsc_handle)  == 1);
  CORE_VERIFY (rsc_instance_init (aop_rsc_handle) == RSC_SUCCESS);
  CORE_VERIFY (rsc_set_low_power_mode (aop_rsc_handle, 0) == RSC_SUCCESS);

#if !defined(SDX24_STUBS)
  pmux_init ();
#endif

  // Disable clock gating on V1
  // Enable the PDC and configure the wakeup interrupt
  // As this is 845 V1 specifc, not needed for warlock
  if ((ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SDM845) &&
      (ChipInfo_GetChipVersion() < CHIPINFO_VERSION(2,0)))
  {
    HWIO_OUTF (RPMH_PDC_AOP_ENABLE_PDC, DISABLE_CLK_GATE, 1);
  }
   
  // Enable bit 8 for soc_sleep_unlock to level high.
  HWIO_OUTI2 (RPMH_PDC_AOP_IRQ_i_CFG_DRVd, 0, 8, 0x4);
  
  // Enable bit 1 for PMIC Reset to level high.
  HWIO_OUTI2 (RPMH_PDC_AOP_IRQ_i_CFG_DRVd, 0, 1, 0x4);
  
  // Enable rude (0x100) and PMIC Reset (0x02) interrupts
  HWIO_OUTI2 (RPMH_PDC_AOP_IRQ_ENABLE_BANKb_DRVd, 0, 0, 0x102);

  // Configure the PDC sequences
  pdc_seq_handles ("aop", 1, &aop_pdc_handle);

  CORE_VERIFY (pdc_seq_handle_init (aop_pdc_handle) == PDC_SEQ_SUCCESS);
  CORE_VERIFY (pdc_enable (aop_pdc_handle, TRUE)  == PDC_SEQ_SUCCESS);
  CORE_VERIFY (pdc_seq_set_lpm (aop_pdc_handle, 1) == PDC_SEQ_SUCCESS);

#ifdef SDM670_TARGET_CONFIG
  //configure debug PDC sequence 
  pdc_seq_handles ("debug", 1, &dbg_pdc_handle);

  CORE_VERIFY (pdc_seq_handle_init (dbg_pdc_handle) == PDC_SEQ_SUCCESS);
  CORE_VERIFY (pdc_enable (dbg_pdc_handle, TRUE)  == PDC_SEQ_SUCCESS);
  CORE_VERIFY (pdc_seq_set_lpm (dbg_pdc_handle, 1) == PDC_SEQ_SUCCESS);

  CORE_VERIFY (rex_get_interrupt_polarity(RPMH_IDLE) == LEVEL_HIGH);
#endif
}

// A bit of priority inversion with respect to the timer services.  But, from the idle thread
// we will implement a simple timer service. 

#define NUM_TIMER_CALLBACKS 4  // DDR_MGR test, OPSCALING POLL, AOP_SERVICES are known users.
struct idle_timer_service
{
  uint32   enabled;
  uint64_t wakeup_time;
  PFV_W64  callback_fn;
} idle_task_timers[NUM_TIMER_CALLBACKS];


uint64_t aop_timer_min = ~0;

/**
 * <!-- aop_register_timer_cb -->
 *
 * @brief Register a function to be called from the idle thread after a period of time
 *
 * @param[in] period   - The number of ticks in the future to set the timer
 * @param[in] callback - The function to call.
 * 
**/
__attribute__((section("sleep_code_sec")))
int aop_register_timer_cb(uint32 period, PFV_W64 callback)
{
  int i;
  int ret_val = -1;
  uint64_t wakeup = time_service_now() + (uint64_t) period;
  rex_enter_crit_sect(&timer_crit_sect);
  // Check if its already assigned.
  for (i = 0; i < NUM_TIMER_CALLBACKS; i++)
  {
    if ((idle_task_timers[i].enabled == TRUE) &&
        (idle_task_timers[i].callback_fn == callback))
    {
      ret_val = i;
      idle_task_timers[i].wakeup_time = wakeup;
    }
  }
  if (ret_val < 0) 
  {
  for (i = 0; i < NUM_TIMER_CALLBACKS; i++)
  {
    if (idle_task_timers[i].enabled == FALSE)
    {
      ret_val = i;
      idle_task_timers[i].callback_fn = callback;
      idle_task_timers[i].wakeup_time = wakeup;
      idle_task_timers[i].enabled = TRUE;
      break;
    }
  }
  }

  aop_timer_min = ~0;
  for (i = 0; i < NUM_TIMER_CALLBACKS; i++)
  {
    if (idle_task_timers[i].enabled == TRUE)
    {
      aop_timer_min = MIN(aop_timer_min, idle_task_timers[i].wakeup_time);
    }
  }
  rex_leave_crit_sect(&timer_crit_sect);
  return ret_val;
}

/**
 * <!-- aop_cancel_timer_cb -->
 *
 * @brief Cancel a previously registered callback function
 *
 * @param[in] callback - The function to have been called.
 * 
**/
__attribute__((section("sleep_code_sec")))
int aop_cancel_timer_cb(PFV_W64 callback)
{
  int i;
  aop_timer_min = ~0;
  int ret_val = -1;
  rex_enter_crit_sect(&timer_crit_sect);
  for (i = 0; i < NUM_TIMER_CALLBACKS; i++)
  {
    if (idle_task_timers[i].enabled == TRUE)
    {
      if (idle_task_timers[i].callback_fn == callback)
      {
        ret_val = i;
        idle_task_timers[i].enabled = FALSE;
      }
      else
      {
        aop_timer_min = MIN(aop_timer_min, idle_task_timers[i].wakeup_time);
      }
    }
  }
  rex_leave_crit_sect(&timer_crit_sect);
  return ret_val;
}

/**
 * <!-- aop_timer_check -->
 *
 * @brief Determine if any callbacks are necessary
 *
**/
void aop_timer_check(void)
{
  int i;
  aop_timer_min = ~0;

  uint64_t now = time_service_now();

  rex_enter_crit_sect(&timer_crit_sect);
  for (i = NUM_TIMER_CALLBACKS-1; i >= 0 ; i--)
  {
    if (idle_task_timers[i].enabled == TRUE)
    {
      if (idle_task_timers[i].wakeup_time <= now)
      {
        // Call the callback and mark it disabled.
        idle_task_timers[i].enabled = FALSE;
        (*idle_task_timers[i].callback_fn)(now - idle_task_timers[i].wakeup_time);
      }
      else
      {
        aop_timer_min = MIN(aop_timer_min, idle_task_timers[i].wakeup_time);
      }
    }
  }
  rex_leave_crit_sect(&timer_crit_sect);
}

/**
 * <!-- calculate_wakeup_time -->
 *
 * @brief Calculates the wakeup time for AOP SW based on the use case
 *
 * @return the wakeup time
 */
uint64_t calculate_and_set_wakeup_time (bool *aop_only)
{
  // Check to see if any ARC has a valid wakeup timer
  uint8 arc_index = 0;
  uint32 wakeup_time_lo = 0;
  uint32 wakeup_time_hi = 0;
  uint64_t wakeup_time = ~0;
 
  wakeup_time = aop_timer_min;

  if (! (*aop_only))
  {
    for (arc_index = 0; arc_index < RPMH_ARC_NUM; arc_index ++)
    {
      uint64 temp;
      bool wakeup_valid = HWIO_INFI (RPMH_ARC_RMm_WAKEUP_TIMER_STATUS_HIGH, arc_index, WAKEUP_TIMER_ENABLE);
      if (wakeup_valid)
      {
        wakeup_time_hi = HWIO_INFI (RPMH_ARC_RMm_WAKEUP_TIMER_STATUS_HIGH, arc_index, WAKEUP_TIMER_VALUE_HIGH);
        wakeup_time_lo = HWIO_INI (RPMH_ARC_RMm_WAKEUP_TIMER_STATUS_LOW, arc_index);
        temp = wakeup_time_hi;
        temp <<= 32;
        temp |= wakeup_time_lo;

        if (temp < wakeup_time)
        {
          wakeup_time = temp;
        }
      }
    }
  }
  
  // 1s for now, 19.2Mhz XO clock
  // TODO: Should not be necessary once AOP moves out of standalone mode
  if ((wakeup_time == ~0) && (g_temp_wakeup_time != 0))
  {
    wakeup_time = (time_service_now () + g_temp_wakeup_time);
  }

  if (! (*aop_only))
  {
    if (wakeup_time < (time_service_now() + g_pdc_threshold_time))
    {
      *aop_only = true;
    }
    else
  {
    // Program the PDC wakeup time
    // TODO: Need to apply a backoff to this value below to account for the AOSS sleep exit
    wakeup_time -= g_pdc_backoff_time;
    pdc_seq_set_wakeup_time (aop_pdc_handle, wakeup_time);
  }
  }

  // Set the AOP Qtimer
  wakeup_time_lo = (uint32) wakeup_time;
  wakeup_time_hi = wakeup_time >> 32;

  HWIO_OUT (AOP_F0_QTMR_V1_CNTP_CVAL_LO, wakeup_time_lo);
  HWIO_OUT (AOP_F0_QTMR_V1_CNTP_CVAL_HI, wakeup_time_hi);
  HWIO_OUTF (AOP_F0_QTMR_V1_CNTP_CTL, EN, 1);
  HWIO_OUTF (AOP_F0_QTMR_V1_CNTP_CTL, IMSK, 0);

  return wakeup_time;
}


/**
 * <!-- lpi_mx_scaling -->
 *
 * @brief Function to bump lpi_mx to Turbo. since rail voltage
 *        is stuck at 0.93v post bootup, if no other votes are
 *        made to CPR to make cpr voltage updates.
 *
 * @return none
 */
__attribute__((section("sleep_code_sec")))
void lpi_mx_scaling(void)
{
  unpa_client *arc_lmx_npa_client = unpa_create_client ("lmx_scaling", UNPA_CLIENT_REQUIRED, "lmx.lvl");
  CORE_VERIFY (arc_lmx_npa_client != NULL);

  unpa_issue_request (arc_lmx_npa_client, RAIL_VOLTAGE_LEVEL_TUR_L1);

  unpa_set_request_attribute (arc_lmx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (arc_lmx_npa_client, RAIL_VOLTAGE_LEVEL_OFF);
  
  unpa_destroy_client(arc_lmx_npa_client);
}

/**
 * <!-- aop_aoss_sleep -->
 *
 * @brief This function is used to enter AOSS sleep (the deeper AOP sleep mode).
 *        It is called instead of "aop_only_sleep" and requires the PDC wakeup 
 *        time to have been called first.
 *
 * @return none
 */
void aop_aoss_sleep(uint64_t wakeup_time)
{

#if !defined(SDX24_STUBS)
  // set the pmux comparator
  pmux_configure_sleep ();
#endif  
  // Update stats
  aop_sleep_stats_enter (AOP_SLEEP_STATS_AOSD);
  
  Clock_ProcessorSleep();

  // Set up the WFI behavior
  HWIO_OUTF (AOP_WFI_CONFIG, PROC_CLK_HALT, 1);
  HWIO_OUTF (AOP_WFI_CONFIG, BUS_CLK_HALT, 1);
  HWIO_OUTF (AOP_WFI_CONFIG, CHIP_SLEEP_UPON_WFI, 1);

  // Enable the Qtimer interrupt 
  rex_clear_interrupt (AOP_QTIMER_INTR_0);
  rex_enable_interrupt (AOP_QTIMER_INTR_0);

  // In case any RPMh HW goes non-idle
  rex_configure_interrupt (RPMH_IDLE, LEVEL_LOW);
  rex_clear_interrupt (RPMH_IDLE);
  rex_enable_interrupt (RPMH_IDLE);
  
  aop_sleep_state = AOP_SLEEP_WFI_ENTER;
    
  halt ();

  aop_sleep_state = AOP_SLEEP_WFI_EXIT;

  rex_disable_interrupt (AOP_QTIMER_INTR_0);
  rex_disable_interrupt (RPMH_IDLE);
  
  // Clear out the WFI behavior
  if (!aosspwrupreq)
  {
    HWIO_OUTF (AOP_WFI_CONFIG, PROC_CLK_HALT, 1);
  }
  
  HWIO_OUTF (AOP_WFI_CONFIG, BUS_CLK_HALT, 0);
  HWIO_OUTF (AOP_WFI_CONFIG, CHIP_SLEEP_UPON_WFI, 0);

  // Disable QTimer
  HWIO_OUTF (AOP_F0_QTMR_V1_CNTP_CTL, EN, 0);
  HWIO_OUTF (AOP_F0_QTMR_V1_CNTP_CTL, IMSK, 1);

  Clock_ProcessorRestore();

  // Update stats
  aop_sleep_stats_exit (AOP_SLEEP_STATS_AOSD);

#if !defined(SDX24_STUBS)
  // Configure the pmux
  pmux_configure_wake ();
#endif

  // Did we wakeup due to a rude interrupt?
  g_rude_wakeup = rex_is_interrupt_pending (SOC_SLEEP_PDC_AOP_UNLOCK_REQ);

  sleep_history[sleep_history_index].scheduled = wakeup_time;
  sleep_history[sleep_history_index].now = time_service_now ();
  sleep_history[sleep_history_index].rude = g_rude_wakeup;
  sleep_history_index ++;
  if (sleep_history_index >= ARR_SIZE(sleep_history))
  {
    sleep_history_index = 0;
  }
}

/**
 * <!-- aop_only_sleep -->
 *
 * @brief This function is used to enter AOP sleep (the shallow AOP sleep mode).
 *        It is called instead of "aop_aoss_sleep" and does not require the PDC
 *        wakeup time to have been set first.
 *
 * @return none
 */
__attribute__((section("sleep_code_sec")))
void aop_only_sleep(uint64_t wakeup_time)
{
  rex_clear_interrupt (AOP_QTIMER_INTR_0);
  rex_enable_interrupt (AOP_QTIMER_INTR_0);

    
  //SWEVENT (HALT_ENTER, (wakeup_time >> 31), wakeup_time);
#ifdef SDM670_TARGET_CONFIG    
  Clock_ProcessorReduce( true );
#endif
  halt ();
#ifdef SDM670_TARGET_CONFIG
 Clock_ProcessorReduce( false );
#endif
  //SWEVENT (HALT_EXIT, (wakeup_time >> 31), wakeup_time);

  g_num_halt ++;
  rex_disable_interrupt (AOP_QTIMER_INTR_0);
}

uint32 aop_busy_mask = 0;
/**
 * <!-- aop_busy -->
 *
 * @brief This function is used to prevent AOSS sleep if AOP is likely busy.
 *
 * @return none
 */
void aop_busy(uint32 thread_mask, bool busy)
{
  if (busy)
  {
    aop_busy_mask |= thread_mask;
  }
  else
  {
    aop_busy_mask &= ~thread_mask;
  }
}

/**
 * <!-- aop_sleep_main -->
 *
 * @brief This is the core of the sleep handling code. This function is expected
 *        to be called from the top level idle loop.
 *
 * @return none
 */
void aop_sleep_main (void)
{
  rex_sigs_type sigs;

  dog_task_stop();
  aop_timer_check();

  if (aop_sleep_state == AOP_SLEEP_IDLE)
  {
    rex_clear_interrupt (SOC_SLEEP_PDC_AOP_LOCK_REQ);
    rex_enable_interrupt (SOC_SLEEP_PDC_AOP_LOCK_REQ);
  }
  
  if ((aop_sleep_state == AOP_SLEEP_LOCK) && (g_allow_aoss_sleep) &&
      (! is_arc_operational (RPMH_ARC_VDD_CX)))
  {
    rex_clear_interrupt (RPMH_IDLE);
	  rex_configure_interrupt (RPMH_IDLE, LEVEL_HIGH);
    rex_enable_interrupt (RPMH_IDLE);
  }

  // Place any workarounds which block ABOVE the following INTLOCK.
  REX_INTLOCK ();
  
  // Check if any signals set
  sigs = rex_get_sigs (rex_self ());
  // rex_sigs_type sigs = AOP_SLEEP_SIG_PDC_LOCK_REQ;
  //rex_wait (AOP_SLEEP_SIG_PDC_LOCK_REQ);

  // States:
  // AOP_SLEEP_IDLE
  // AOP_SLEEP_LOCK_RECEIVED
  // AOP_SLEEP_IDLE_RECEIVED
  // AOP_SLEEP_WFI_ENTER
  // AOP_SLEEP_WFI_EXIT
  // AOP_SLEEP_UNLOCK_RECEIVED
  //

  // If AOSS sleep scenario not triggered, kick off local clock gating mode
  if ((sigs == 0) &&
      ((aop_sleep_state == AOP_SLEEP_IDLE) || (aop_sleep_state == AOP_SLEEP_LOCK)))
  {
    bool aop_only = true;
    // Calculate wakeup time - read ARCs, set wakeup time
    uint64_t wakeup_time = calculate_and_set_wakeup_time (&aop_only);
    
    aop_only_sleep(wakeup_time);

    REX_INTFREE ();
    
    dog_task_run ();
    
    return;
  }
  
  // Possible AOSS sleep situation
  // Ack the lock interrupt
  if ((sigs & AOP_SLEEP_SIG_PDC_LOCK_REQ) && (aop_sleep_state == AOP_SLEEP_IDLE))
  {
    rex_clr_sigs (rex_self (), AOP_SLEEP_SIG_PDC_LOCK_REQ);

    aop_sleep_state = AOP_SLEEP_LOCK;

    g_aoss_sleep_locked = TRUE;
    zero_degree_c_update();

    // Enable the unlock interrupt
    rex_clear_interrupt (SOC_SLEEP_PDC_AOP_UNLOCK_REQ);
    rex_enable_interrupt (SOC_SLEEP_PDC_AOP_UNLOCK_REQ);
    
    // Deassert the unlock ack
    HWIO_OUT (AOP_GPO_WDCLR, (1 << 3));
    
    // Assert the lock ack aop_gpo[2]
    HWIO_OUT (AOP_GPO_WDSET, (1 << 2));

    rex_clear_interrupt (SOC_SLEEP_PDC_AOP_LOCK_REQ);
  }

  if ((sigs & AOP_SLEEP_SIG_PDC_UNLOCK_REQ) && (aop_sleep_state == AOP_SLEEP_LOCK))
  {
    // Unlock is an edge interrupt, therefore no need to explicitly clear the interrupt
    rex_clr_sigs (rex_self (), AOP_SLEEP_SIG_PDC_UNLOCK_REQ);

    aop_sleep_state = AOP_SLEEP_IDLE;

    g_aoss_sleep_locked = FALSE;
    zero_degree_c_update();

    // Deassert the lock ack
    HWIO_OUT (AOP_GPO_WDCLR, (1 << 2));
    
    // Assert the unlock ack
    HWIO_OUT (AOP_GPO_WDSET, (1 << 3));

    //Clear out the unlock status since we just handled it
    rex_clear_interrupt (SOC_SLEEP_PDC_AOP_UNLOCK_REQ);

    // Clear out any stale information regarding idle status
    rex_disable_interrupt (RPMH_IDLE);
    rex_clr_sigs (rex_self (), AOP_SLEEP_SIG_RPMH_IDLE);
  }
  
  if ((sigs & AOP_SLEEP_SIG_RPMH_IDLE) && (aop_sleep_state == AOP_SLEEP_LOCK))
  {
    uint64_t wakeup_time;

    // if arc is somehow operational and haven't recieved falling edge RPMH_IDLE signal
    bool aop_only = is_arc_operational2(RPMH_ARC_VDD_CX);

    rex_clr_sigs (rex_self (), AOP_SLEEP_SIG_RPMH_IDLE);

    aop_sleep_state = AOP_SLEEP_HW_IDLE;
  
    // Calculate wakeup time - read ARCs, set wakeup time
    wakeup_time = calculate_and_set_wakeup_time (&aop_only);
    if (aop_only)
    {
      aop_only_sleep (wakeup_time);
    }
    else
    {
      aop_aoss_sleep (wakeup_time);
    }

    // Allow the next round of sleep 
    aop_sleep_state = AOP_SLEEP_LOCK;
  }

  REX_INTFREE ();
  dog_task_run(); 
}

/**
 * <!-- aop_sleep_wfi_config -->
 *
 * @brief When the debugger is attached, we disable proc halt.
 *        This happens via aosspwrupreq. If the debugger deattaches, we re-enable
 *        to be called from the top level idle loop.
 *
 * @param debugger_attached:        
 *
 * @return none
 */
void aop_sleep_wfi_config (boolean debugger_attached)
{
  if (debugger_attached)
  {
    aosspwrupreq = 1;
    HWIO_OUTF (AOP_WFI_CONFIG, PROC_CLK_HALT, 0);
  }
  else
  {
    aosspwrupreq = 0;
    HWIO_OUTF (AOP_WFI_CONFIG, PROC_CLK_HALT, 1);
  }
}

