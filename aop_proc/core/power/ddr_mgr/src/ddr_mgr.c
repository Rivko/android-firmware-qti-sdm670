/*===========================================================================
                              ddr_mgr.c

DESCRIPTION:

Copyright (c) 2016,2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
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
#include "aop_hwio.h"
#include "ddr_mgmt.h"
#include "ddr_mgmt_target.h"
#include "aop_fsm.h"
#include "CoreVerify.h"
#include "hwio_ddraux_seq.h"
#include "hwio_ddraux_tcs.h"
#include "hwio_ddraux_ddraux.h"
#include "hwio_arc_rm.h"
#include "hwio_arc_irq.h"
#include "hwio_arc_vt.h"
#include "hwio_arc_sw_override.h"
#include "hwio_bcm.h"
#include "hwio_bcm_int.h"
#include "hwio_bcm_sw_override.h"
#include "hwio_bcm_vt.h"
#include "hwio_cprf.h"
#include "hwio_shrm_regs.h"
#include "rex.h"
#include "rinit.h"
#include "aop_interrupt_table.h"
#include "icb_aop.h"
#include "rpmh_common.h"
#include "rpmh_client.h"
#include "cmd_db.h"
#include "aop_cx_collapse.h"
#include "pwr_utils_lvl.h"
#include "unpa/unpa_resource.h"
#include "arc_runtime.h"
#include "pmapp_npa.h"
#include "aop_ulog.h"
#include "cprf.h"
#include "ddr_smem.h"
#include "qmp_msg_driver.h"
#include "msg_ram_layout.h"
#include "ChipInfo.h"
#include "aop_sleep.h"
#include "wcssaon_hwio.h"
#include "time_service.h"
#include "drv_mapping.h"

#include "pdc_apps_hwio.h"
#include "pdc_modem_hwio.h"

#ifndef SDX24_STUBS
  #include "pdc_compute_hwio.h"
  #include "pdc_lpass_hwio.h"
#endif

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define MIN_SIZE        // Define this to remove code that is not currently used.
#define PARALLEL_SEQUENCING (g_parallel_sequence)  // Can SHRM handle CX Changes during a frequency change.
#define GCC_WORKING FALSE
#define DCC_WORKING FALSE
#define RES_WORD_SIZE 10

#if !defined(SDM670_TARGET_CONFIG)  && !defined(SDX24_TARGET_CONFIG)
#define RUMI
#endif

#ifdef RUMI
#define ONE_SECOND (1920000) // actually, 100ms; speed things up on a RUMI
#else
#define ONE_SECOND (19200000)
#endif

#define ONE_MILLISECOND (19200)
#define DEFAULT_SEQUENCE_TIMEOUT  (ONE_MILLISECOND * 5)

#define SIG_ARC_CX_RM_INTERRUPT        0x00000001
#define SIG_ARC_MX_RM_INTERRUPT        0x00000002
#define SIG_ARC_VDDA_RM_INTERRUPT      0x00000004
#define SIG_ARC_DDR_RM_INTERRUPT       0x00000008

#define SIG_BCM_INTERRUPT              0x00000010
#define SIG_DRIVER_DCC_BANDWIDTH       0x00000020
#define SIG_DRIVER_XO_VOTE_COMP        0x00000040
#define SIG_UPDATE_FREQUENCY_PLAN      0x00000080

#define SIG_DDR_AUX_SEQ0_INTERRUPT     0x00000100
#define SIG_DDR_AUX_SEQ1_INTERRUPT     0x00000200
#define SIG_DDR_AUX_TCS_DONE_INT       0x00000400
#define SIG_VOTE_DONE_INT              0x00000800

#define SIG_ARC_CX_SEQ_PRE_INTERRUPT   0x00001000
#define SIG_ARC_MX_SEQ_PRE_INTERRUPT   0x00002000
#define SIG_ARC_VDDA_SEQ_INTERRUPT     0x00004000
#define SIG_FAL_INTERRUPT              0x00008000

#define SIG_ARC_CX_SEQ_POST_INTERRUPT  0x00010000
#define SIG_ARC_MX_SEQ_POST_INTERRUPT  0x00020000
#define SIG_VOLTAGE_COMP_DONE          0x00040000
#define SIG_ZERO_TEMP_EVENT            0x00080000

#define SIG_POLL_TIMER                 0x00100000
#define SIG_VOLTAGE_SEQ                0x00200000

#define DDR_AUX_SIGNAL_MASK            0x003FFFFF  // OR of the signals above


#define INTR_AUX_STAT_SEQ0_DONE 0x01
#define INTR_AUX_STAT_SEQ1_DONE 0x02
#define INTR_AUX_STAT_TCSn_DONE 0x04
#define INTR_AUX_STAT_SEQ_INT   0x08
#define INTR_AUX_STAT_SHRM_GPW  0x10
#define INTR_AUX_STAT_BIT5_RSVD 0x20
#define INTR_AUX_STAT_BIT6_RSVD 0x40
#define INTR_AUX_STAT_SUMRY_INT 0x80

#define INTR_RM_AGGR_DONE      0x01
#define INTR_RM_SOLVING_NEEDED 0x02
#define INTR_RM_TIMER_EXPIRE   0x04
#define INTR_RM_SEQ_START      0x08
#define INTR_RM_SEQ_DONE       0x10
#define INTR_RM_SEQ_PREWORK    0x20
#define INTR_RM_SEQ_POSTWORK   0x40
#define INTR_RM_DRC_VOTE_ERROR 0x80

// ARC Interrupt bit positions
#define ARC_AGGR_DONE 0x01
#define IRQ_TIMER_EXPIRED_BITMASK 0x4
#define ARC_SEQUENCE_PREWORK 0x20
#define ARC_SEQUENCE_POSTWORK 0x40


//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct ddr_npa_client_t
{
  const char *name;
  unpa_client *client_handle;
  unpa_query_handle client_query_handle;
} ddr_npa_client_t;
typedef struct ddr_npa_resource_t
{
  const char *name;
  unpa_query_handle query_handle;
  ddr_npa_client_t clients[NUM_CLIENTS];
} ddr_npa_resource_t;

typedef struct ddr_npa_t
{
  ddr_npa_resource_t resources[NUM_NPA_RESOURCES];
} ddr_npa_t;

typedef struct ddr_rail_t
{
  arc_runtime_client_t *rc;
} ddr_rail_t;

typedef struct ddr_rails_t
{
  ddr_rail_t rails[NUM_VOLTAGE_RESOURCES];
} ddr_rails_t;

typedef struct ddr_aux_t
{
  ddr_rails_t *rails;
  ddr_npa_t *npa;
} ddr_aux_t;

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

static ddr_npa_t ddr_npa =
{
  .resources[0] =
  {
    .name = "mx.lvl",
    .clients[0] =
    {
      .name = "ddr_freq"
    },
    .clients[1] =
    {
      .name = "ddr_dep"
    },
    .clients[2] =
    {
      .name = "ddr_temp"
    },
  },
  .resources[1] =
  {
    .name = "cx.lvl",
    .clients[0] =
    {
      .name = "ddr_freq"
    },
    .clients[1] =
    {
      .name = "ddr_dep"
    },
    .clients[2] =
    {
      .name = "ddr_temp"
    }
  },
  .resources[2] =
  {
    .name = "ebi.lvl",
    .clients[0] =
    {
      .name = "ddr_freq"
    },
    .clients[1] =
    {
      .name = "ddr_dep"
    },
    .clients[2] =
    {
      .name = "ddr_temp"
    },
  },
  .resources[3] =
  {
    .name = PMIC_NPA_GROUP_ID_DDR,
    .clients[0] =
    {
      .name = "ddr_freq"
    },
    .clients[1] =
    {
      .name = "ddr_dep"
    },
  }
};

static rex_tcb_type ddr_fsm_tcb;

static ddr_rails_t ddr_rails;

unsigned long config_updates = 0;
unsigned long sequencer_count = 0;
unsigned long frequency_count = 0;
unsigned long interrupt_count = 0;
uint32 ddr_mgr_poll_time = (ONE_SECOND/100000);  // 10 us

uint32  g_ddrss_on_vote = D0_On;    // What vote to send to turn Powerstate ON.

// Test variables for bringup
boolean g_cpr_disabled = FALSE;     // True when CPR is disabled
#define g_alc_override TRUE      // True if ACV Hack is enabled.

uint32  g_xo_seq_req_id = 0; //req_id of rpmh call to bringup XO.
uint32  g_volt_seq_req_id = 0; //req_id of rpmh call to bringup Voltages.
uint64_t g_xo_on_time = 0; //timeout variable to store XO on times.

#define g_opscale_bcm TRUE
#define g_opscale_shub TRUE
#define g_use_local_table FALSE
#define g_retain_llc_only FALSE
#define g_copy_enable_only FALSE
//boolean g_use_local_table =  FALSE;  // Do we trust the SMEM table?
//boolean g_opscale_shub = TRUE;
//boolean g_opscale_bcm = TRUE;
//boolean g_retain_llc_only =  FALSE;  // Set to true when we are on RUMI
//boolean g_copy_enable_only = FALSE;  // Do we want to set enable/disable only

boolean g_zero_deg_assert = FALSE;    // set to True when APPS has asserted a zero degree event
uint32  g_temp_nom_volt = NOMINAL;    // NOMINAL vote for zero temp event

uint32  g_command_timeout = DEFAULT_SEQUENCE_TIMEOUT; // Default value for checking errors.
uint32  g_log_max_time    = ~0;     // Default when to stop the ddr logging.

#ifndef MIN_SIZE
boolean g_do_memtest = FALSE;       // Do we even want to do memtest
boolean g_ddr_vote_to_icb = FALSE;  // Do we vote for resources for memtest
boolean g_cmd_db_mem_test = FALSE;  // Query command db after a change as quick MEMTEST
boolean g_parallel_sequence = TRUE;  // Can CX/MX change while freq change?
#endif

unsigned int arc_to_rm[] = {RPMH_ARC_VDD_MX, RPMH_ARC_VDD_CX, RPMH_ARC_VDD_EBI};
unsigned int rm_to_arc[] = {RESOURCE_CX, RESOURCE_MX, RESOURCE_VDDA};
unsigned int cx_to_mx_map[16];

#ifndef SDX24_STUBS
extern void wlan_above_mol_workaround(void);
extern void wlan_below_mol_workaround(void);
extern void wlan_workaround_init(void);
#endif

extern uint64_t time_service_now(void);
extern void cx_collapse_voltage_comp_done (void);
extern bool g_aoss_sleep_locked;
void aop_ddr_client_callback(uint32 req_id);

boolean g_log_enabled = TRUE;
#define DDR_LOG_MSGRAM
#undef  DDR_SWEVENTS

#ifndef DDR_SWEVENTS
#define NUM_LOG_ENTRIES  64
volatile uint32 log_index = 0;
struct log_entry {
  uint32 timestamp;
#ifdef DDR_LOG_MSGRAM
  uint32 message[2];
#else
  char   message[24];
#endif
  uint32 data;
};

#ifdef DDR_LOG_MSGRAM
struct log_entry *log_data = NULL;
#else
struct log_entry log_data[NUM_LOG_ENTRIES+1];
#endif //DDR_LOG_MSGRAM
#endif //DDR_SWEVENTS

typedef enum
{
  CAPPED,
  FIXED
} ddr_debug_mode;

/**
 * <!-- ddr_log_msg32 -->
 *
 * @brief   Stores a log message in our internal buffer
 *
 * @param   message - The 24 byte string to store
 * @param   data    - The 32 bit quantity unique to the message
 *
 * @return  None
 */
#ifndef DDR_SWEVENTS
void ddr_log_msg32(char *message, uint32 data)
{
#ifdef DDR_LOG_MSGRAM
  struct log_entry *log_ptr = log_data;
  int j;
#else
  struct log_entry *log_ptr = (struct log_entry *)(((uint32) &log_data[1]) & 0xFFFFFFE0);
#endif
  uint64_t now = time_service_now();
  uint32 index = log_index++;
  if (!g_log_enabled)
    return;

  index = index % NUM_LOG_ENTRIES;
  log_ptr = log_ptr + index;

  memset(log_ptr, 0, sizeof(log_data[0]));
  log_ptr->timestamp = (uint32) now;
#ifdef DDR_LOG_MSGRAM
  for (j = 0; j < (sizeof(log_ptr->message)/sizeof(uint32)); j++)
  {
    log_ptr->message[j] = message[j*4]         | message[j*4+1] << 8 |
                          message[j*4+2] << 16 | message[j*4+3] << 24;
  }
#else
  strlcpy(log_ptr->message, message, sizeof(log_data[0].message));
#endif
  log_ptr->data = data;
}

/**
 * <!-- ddr_log_msg8 -->
 *
 * @brief   Used to log a message when we have 4 8-bit values instead of 1 32 bit value
 *
 * @param   message - The 24 byte string to store
 * @param   data[0-3] - The 8 bit quantities unique to the message
 *
 * @return  None
 */
void ddr_log_msg8(char *message, unsigned char data0, unsigned char data1, unsigned char data2, unsigned char data3)
{
  ddr_log_msg32(message, ((0xff & data0) << 24) | ((0xff & data1) << 16) |
                         ((0xff & data2) << 8 ) | ((0xFF & data3)      ));
}
#endif //DDR_SWEVENTS

enum debug_timing {
  WAITING_FOR_TCS0,
  WAITING_FOR_SEQ0,
  WAITING_FOR_SEQ1,
  NUM_DEBUG_TIMINGS
};
// Global variables that store the debug status ni the event of an abort
static uint32 debug_tcs_stat[HWIO_RSI_TCSm_CMDt_MAXm+1];
static uint32 debug_seq_stat[HWIO_SEQn_STAT_MAXn+1][2];
uint64_t elapsed_stats[NUM_DEBUG_TIMINGS];

// Counter of the number of sequencers in progress.
uint32_t num_in_progress = 0;

#define DDR_ISSUES_SEQUENCER_RUNNING    0x01
#define DDR_ISSUES_SHRM_DID_NOT_RESPOND 0x02

// debug_ddraux
//
// This function is called to diagnose potential problems we see when
// communicating with DDR_AUX, SHRM, GCC
//
// Paramters:
//   None
//
// Return:
//   A BITMASK of the issues seen.
//
int debug_ddraux(void)
{
  int m, t, n;
  uint32 issues_seen = 0;
  uint64_t now = time_service_now();
  uint32 seq_elapsed[HWIO_SEQn_STAT_MAXn+1];
  uint32 shrm_dump_regs[2];

  // CLKPLAN and SEQ_BRANCH_EVENT indicate what the sequences should be doing
  uint32 gcc_clkplan = HWIO_IN(SEQ_GCC_CLKPLAN);
  uint32 seq_branch_event = HWIO_IN(SEQ_BRANCH_EVENT);
  DDR_MGR_SWEVENT(DDRAUX_CLKPLAN_BRANCH, gcc_clkplan, seq_branch_event);

  for (n = 0; n <= HWIO_SEQn_STAT_MAXn; n++)
  {
    uint32 addr;
    uint32 instruction;
    seq_elapsed[n] = 0;
    debug_seq_stat[n][0] = HWIO_INI(SEQn_STAT, n);
    debug_seq_stat[n][1] = HWIO_INI(SEQn_CFG, n);
    addr = debug_seq_stat[n][0] & HWIO_SEQn_STAT_SEQ_PC_BMSK;
    if (debug_seq_stat[n][0] & HWIO_SEQn_STAT_SEQ_BUSY_BMSK)
    {
      seq_elapsed[n] = now - elapsed_stats[WAITING_FOR_SEQ0+n];
      addr-=2;

      if (seq_elapsed[n] >= g_command_timeout)
      {
        issues_seen |= DDR_ISSUES_SEQUENCER_RUNNING;
      }

      //ToDo: if n=1 call cpr_debug_dump && arc_debug_dump
    }
    instruction = HWIO_INI(SEQ_MEMq, addr >> 2);
    //printf("Sequencer Status: Busy|Address:%08.8X.  Start Start|Address:%08.8X. Instruction:%08.8X Seq Elapsed:%08.8X");
    DDR_MGR_SWEVENT(DDRAUX_SEQUENCER_STATUS, debug_seq_stat[n][0], debug_seq_stat[n][1],
                    instruction, seq_elapsed[n]);
  }
  for (m = 0; m <= HWIO_RSI_TCSm_CMDt_MAXm; m++)
  {
    debug_tcs_stat[m] = HWIO_INI(RSI_TCSm_STAT, m);     // Grab the status
    if (debug_tcs_stat[m] & HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK)
    {
      uint32 tcs_cmd_data[1+HWIO_RSI_TCSm_CMDt_MAXt];
      for (t = 0; t <= HWIO_RSI_TCSm_CMDt_MAXt; t++)
      {
        tcs_cmd_data[t] = HWIO_INI2(RSI_TCSm_CMDt, m, t);
      }
      //printf("TCS %d Busy Error.  Status %08.8X Cmds: %08.8X %08.8X");
      DDR_MGR_SWEVENT(TCS_BUSY_AT_ERROR, m, debug_tcs_stat[m], tcs_cmd_data[0], tcs_cmd_data[1]);
    }
  }

  if (debug_seq_stat[0][0] & HWIO_SEQn_STAT_SEQ_BUSY_BMSK)
  {
    // TCS's 6-13 are used during Frequency Change, Collapse, or Restore 
    // Sequences.  If any of these are busy, and the Frequency DDRAUX Sequencer
    // is busy.   Point blame at SHRM.
    for (m = 6; m <= HWIO_RSI_TCSm_CMDt_MAXm; m++)
    {
      if (debug_tcs_stat[m] & HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK)
      {
        if (seq_elapsed[0] >= g_command_timeout)
        {
          issues_seen |= DDR_ISSUES_SHRM_DID_NOT_RESPOND;
        }
        //printf("SHRM did not respond: TCS %d Status %08.8X Elapsed: %08.8d");
        DDR_MGR_SWEVENT(INVESTIGATE_SHRM_FREQUENCY, m, debug_tcs_stat[m], seq_elapsed[0]);
      }
    }
  }

  if (debug_seq_stat[1][0] & HWIO_SEQn_STAT_SEQ_BUSY_BMSK)
  {
    // TCS's 3-5 are used during Voltage Change Sequences.
    // If any of these TCS's are busy, and the Voltage DDRAUX Sequencer
    // is busy.   Point blame at SHRM.
    for (m = 3; m <= 5; m++)
    {
      if (debug_tcs_stat[m] & HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK)
      {
        if (seq_elapsed[1] >= g_command_timeout)
        {
          issues_seen |= DDR_ISSUES_SHRM_DID_NOT_RESPOND;
        }
        //printf("SHRM did not respond: TCS %d Status %08.8X Elapsed: %08.8d");
        DDR_MGR_SWEVENT(INVESTIGATE_SHRM_VOLTAGE, m, debug_tcs_stat[m], seq_elapsed[1]);
      }
    }
  }
  
  // If we are in Flush Flow, TCS0 is in use.
  if (debug_tcs_stat[0] & HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK)
  {
    uint32 elapsed = now - elapsed_stats[WAITING_FOR_TCS0];
    if (elapsed >= g_command_timeout)
    {
      issues_seen |= DDR_ISSUES_SHRM_DID_NOT_RESPOND;
    }
    //printf("SHRM did not respond: TCS %d Status %08.8X Elapsed: %08.8d");
    DDR_MGR_SWEVENT(INVESTIGATE_SHRM_FLUSH, debug_tcs_stat[0], elapsed);
  }
  if (issues_seen & DDR_ISSUES_SHRM_DID_NOT_RESPOND)
  {
    shrm_dump_regs[0] = HWIO_IN(SHRM_CSR_SHRM_SPROC_STATUS);
    shrm_dump_regs[1] = HWIO_IN(SHRM_CSR_SHRM_APB_DEBUG_STATUS);
    DDR_MGR_SWEVENT(SHRM_STATUS_AT_TIMEOUT, shrm_dump_regs[0], shrm_dump_regs[1]);
  }
  ddr_log_msg32("DEBUG   ", issues_seen);
  
  return issues_seen;
}

/**
 * <!-- ddraux_did_not_respond -->
 *
 * @brief   This is the entry routine when a timer fires indicating a sequence may be taking
 *          too long.  It aborts the AOP if the debug_ddraux routine thinks there is an error.
 *
 * @param   lateness - Not used.
 *
 * @return  None
 */
void ddraux_did_not_respond(uint64_t lateness)
{
  // Debug DDR_AUX registers.  If the routine indicates there is likely an error, 
  // ABORT the system so it can be diagnosed.
  if (debug_ddraux() > 0)
  {
    abort();
  }
  num_in_progress = 0;
}

/**
 * <!-- start_debug_timer -->
 *
 * @brief   When a sequence begins, this routine will start a timer and log the start time.
 *
 * @param   event: Indicates which event from "enum debug_timing"
 *
 * @return  None
 */
void start_debug_timer(uint32 event)
{
  elapsed_stats[event] = time_service_now();
  num_in_progress++;
  aop_register_timer_cb(g_command_timeout, ddraux_did_not_respond);
}

/**
 * <!-- stop_debug_timer -->
 *
 * @brief   When a sequence completes, this routine is used to cancel the timer 
 *
 * @param   event: Indicates which event from "enum debug_timing"
 *
 * @return  None
 */
void stop_debug_timer(uint32 event)
{
  if (num_in_progress > 0)
  {
    num_in_progress--;
    if (num_in_progress == 0)
    {
      aop_cancel_timer_cb(ddraux_did_not_respond);
    }
  }
}

// The following is the structure which describes the steps DDR is allowed
// to make.   For each state, there is a frequency which it represents, and
// there is also the voltage needs for CX/MX and VDDA.  The clock parameter
// is not currently used, but often deliniates where frequency jumps can
// happen.  The last two fields (up/down) denote from this state, what is
// the furthest down you can go.   Note: below the collapse/restore frequency
// DDR must go up to the collapse/restore frequency first.
//
// Do a double if double_freq_change is TRUE
// - Need to make sure we can go from 3 to 1 with 2 as an intermediate on the way down.
DDRCfgType smem_ddr_cfg;
DDRCfgType *global_ddr_cfg = NULL;
LOCAL_DDRCfgType *ddr_cfg;
LOCAL_FREQ_STATE local_mc_states[16] = {
  //   cp DDRFREQ, period, enable,  clk,      cx,      mx,    vdda,                               PMIC,   up,down dbl_chg, ALC, copied 
  {    0,    0,0xFFFFFFFF, TRUE,    GCC, VDD_OFF, VDD_OFF, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0,   2, 0,    FALSE, 0,  FALSE },    //0
  {    1,  100,  10000,    TRUE,    GCC, MIN_SVS, NOMINAL, MIN_SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_1,  10, 2,    FALSE, 12, FALSE },    //1
  {    2,  200,   5000,    TRUE,    GCC, MIN_SVS, NOMINAL, MIN_SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_1,  10, 0,    FALSE, 12, FALSE },    //2 Collapse/Restore
  {    3,  300,   3333,    TRUE,    GCC, MIN_SVS, NOMINAL, MIN_SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_1,  10, 1,    FALSE, 12, FALSE },    //3
  {    4,  451,   2216,    TRUE,    GCC, LOW_SVS, NOMINAL, LOW_SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_2,  10, 1,    FALSE, 12, FALSE },    //4
  {    5,  547,   1827,    TRUE,    GCC,     SVS, NOMINAL,     SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_2,  10, 1,    FALSE, 12, FALSE },    //5
  {    6,  681,   1467,    TRUE,  DDRCC,     SVS, NOMINAL,     SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  10, 1,    FALSE, 12, FALSE },    //6
  {    7,  768,   1302,    TRUE,  DDRCC,     SVS, NOMINAL,     SVS, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  10, 1,    FALSE, 12, FALSE },    //7
  {    8, 1018,    982,    TRUE,  DDRCC,  SVS_L1, NOMINAL,  SVS_L1, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_10, 10, 1,    FALSE, 12, FALSE },    //8
  {    9, 1355,    738,    TRUE,  DDRCC, NOMINAL, NOMINAL, NOMINAL, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_10, 10, 1,    FALSE, 12, FALSE },    //9
  {   10, 1556,    642,    TRUE,  DDRCC,   TURBO,   TURBO, NOMINAL, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_10, 11, 1,    FALSE, 12, FALSE },    //10
  {   11, 1804,    554,    TRUE,  DDRCC,TURBO_L1,   TURBO,   TURBO, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_10, 11, 1,    FALSE, 15, FALSE },    //11
  {   12,    0,    554,   FALSE,  DDRCC,   TURBO,   TURBO,   TURBO, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  11, 1,    FALSE, 0,  FALSE },    //12
  {   13,    0,    554,   FALSE,  DDRCC,   TURBO,   TURBO,   TURBO, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  11, 1,    FALSE, 0,  FALSE },    //13
  {   14,    0,    554,   FALSE,  DDRCC,   TURBO,   TURBO,   TURBO, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  11, 1,    FALSE, 0,  FALSE },    //14
  {   15,    0,    554,   FALSE,  DDRCC,   TURBO,   TURBO,   TURBO, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_9,  11, 1,    FALSE, 0,  FALSE }     //15
  };
#define NUM_LOCAL_MC 12
uint32 mc_collapse_cp=2;  // The Clock Plan at which to collapse/restore.

LOCAL_FREQ_STATE local_shub_states[16] = {
  //   cp freq, period, enable, clk,       cx,      mx,    vdda,                                PMIC, up,down,dbl, ALC, copied
  {    0,    0,0xFFFFFFFF, TRUE,  NA, VDD_OFF, VDD_OFF, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 1, 0,   FALSE, 0, FALSE },      //0
  {    1,  150,    6666,   TRUE,  NA, MIN_SVS, NOMINAL, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 0,   FALSE, 0, FALSE },      //1 Collapse/Restore
  {    2,  300,    3333,   TRUE,  NA, LOW_SVS, NOMINAL, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //2
  {    3,  426,    2345,   TRUE,  NA,     SVS, NOMINAL, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //3
  {    4,  533,    1876,   TRUE,  NA,  SVS_L1, NOMINAL, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //4
  {    5,  600,    1666,   TRUE,  NA, NOMINAL, NOMINAL, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //5
  {    6,  700,    1428,   TRUE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //6
  {    7,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //7
  {    8,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //8
  {    9,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //9
  {   10,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //10
  {   11,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //11
  {   12,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //12
  {   13,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //13
  {   14,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //14
  {   15,    0,    1240,  FALSE,  NA,   TURBO,   TURBO, VDD_OFF, (uint32) PMIC_NPA_MODE_ID_DDR_CFG_0, 6, 1,   FALSE, 0, FALSE },      //15
};
#define NUM_LOCAL_SHUB 7
uint32 shub_collapse_cp = 1;  // The Clock Plan at which to collapse/restore.

LOCAL_DDRCfgType local_ddr_cfg = {
  NUM_LOCAL_MC,
  NUM_LOCAL_SHUB,
  local_mc_states,
  local_shub_states
};


void mx_callback  (uint32_t req_id);
void cx_callback  (uint32_t req_id);
void vdda_callback(uint32_t req_id);

// STARK TABLE
// The start of the STARK table implementing in this code could be to
// A) reduce the above mc_states table (or what's passed in) to as few states as possible.
// B) Only vote the state to the DDR_SS Powerstate ARC (D0_On+1 = LOW, DO_On+2 = MED, etc)
// AOP Software will choose a SHUB state with the same voltage needs as the chosen MC_STATE.
//
// What would need to be implemented would be
// 1-1 mapping for SHUB.
// SHUB states -> List of allowed MC states, List of allowed MMNOC states, List of allowed SNOC states
// For SHUB states VDDA
// shub_clk >= ddr_clk/4

// voltage_arc_status: This is an AOP wide status - it indicates the votes
// to CX/MX/VDDA for each of the AOP clients.  The AOP software must
// maintain and aggregate its vote to RPMh locally.
static struct resource_states voltage_arc_status[NUM_VOLTAGE_RESOURCES] = {
  { 0, 0, 0, 0, ARC_PENDING_NONE, FALSE, FALSE, 0, mx_callback},     // MX
  { 0, 0, 0, 0, ARC_PENDING_NONE, FALSE, FALSE, 0, cx_callback},     // CX
  { 0, 0, 0, 0, ARC_PENDING_NONE, FALSE, FALSE, 0, vdda_callback}    // VDDA
};
struct resource_states *voltage_arc_status_ptr;

// ddr_status maintains the state of the DDR.  This structure is the decoder
// as we transition DDR, and maintains the current, next, and desired DDR states
// as well as information on LLC cache flush and DDR's CX/VDDA votes.
static struct ddr_state ddr_status = {
  FALSE,     // True if DDR_RM sequence is waiting on AOP.
  FALSE,     // True if BCM sequence is waiting on AOP for MC.
  FALSE,     // True if BCM sequence is waiting on AOP for SHUB.
  FALSE,     // True if BCM sequence is waiting on AOP for ALC.
  FALSE,     // True if BCM sequence is waiting on AOP for ACV.
  TRUE,      // Our vote to the DDR_RM ARC
  FALSE,     // There is no FAL in progress.

  2,         // What CP are we going to for MC
  2,         // What CP are we going to for SHUB
  2,         // Intermediate frequency for MC
  2,         // Intermediate frequency for SHUB
  FALSE,     // Set to true if we have to redo MC transition

  2,         // The current CP of MC
  2,         // The current CP of SHUB

  D0_On,     // Current RM  Desired OL of DDRM RM ARC.
  0,         // Current BCM Desired Clock Plan for MC
  0,         // Current BCM Desired Clock Plan for SHUB
  0,         // Current BCM Desired clock plan for ACV
  0,         // Current BCM Desired clock plan for ALC

  0,         // last read value from BCM
  0,         // last read value from BCM

  FALSE,     // Indicates the flush is complete.
  0,         // What we will flush
  0,         // What is active in LLC.

  XO_ON,     // last vote sent for XO
  TRUE,      // Have we seen a completion for XO?

  FALSE,     // last vote sent for path to dcc to DDR
  FALSE,     // Have we seen a completion for that path?
  0,         // Request_ID of the last DCC Path ON vote.
  FALSE,     // TRUE if a memory test is required.

  D0_On,     // last vote sent for DDR Resource Mgr
  0,         // last vote sent for CX for Frequency
  0,         // last vote sent for MX for Frequency
  0,         // last vote sent for VDDA for Frequency
  0xFF,      // PMIC mode last sent.
  SHRM_LEVEL_HIGH, // The SHRM frequency level

  TRUE,      // True if SHRM is on.

  FALSE,     // TCS 0 Not pending
  FALSE,     // TCS 1 Not pending
  FALSE,     // TCS 2 Not pending

  0,         // unsigned int ddr_mc_floor;
};

static DDR_STATS *ddr_stats = NULL;
static DDR_MGR_CONFIG *ddr_mgr_config = NULL;

// The AOP code is primarily 2 finite state machines.  The Sequencer FSM ensures
// we are only running one voltage or frequency sequence at a time.  The frequency
// FSM performs the main loop for frequency switches.  It determines the next work
// item, votes for the voltage, waits for voltage, runs the frequency sequence, waits
// for that, and then determines what to do next.
static AOP_FSM sequencer_fsm;
static AOP_FSM frequency_fsm;
static boolean freq_fsm_pending_vote = FALSE;       // True if the frequency FSM has votes
static boolean freq_fsm_voting_in_progress = FALSE; // True if voting is in progress, allows batching

static uint32 xo_arc_address;
static uint32 ddr_arc_address;

#ifndef MIN_SIZE
static icb_handle_t icb_handle;  // Holds the route from DCC to MEMNOC
#endif
static rpmh_client_handle aop_ddr_client;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void seq_fsm_signal_start_voltage(void);
void freq_fsm_signal_new_frequency(void);
void arc_unblock_rm_prework(unsigned int arc_rm);
unsigned int get_arc_current_level(VOLTAGE_RESOURCES arc);
void freq_fsm_signal_dcc_bandwidth_complete(void);
boolean voltage_dependency_ready(void);
void zero_degree_c_update(void);

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

uint32 freq_switch_log_index = 0;
FREQ_SWITCH_TABLE *freq_switch_table_ptr = NULL;

#ifdef LOCAL_FREQ_ARRAY   // define this to only allow SMEM written if
                          // DDR is operational.
uint32 local_switch_log_index = 0;
FREQ_SWITCH_TABLE local_freq_switch_table[3];
#else
FREQ_SWITCH_TABLE local_freq_switch_entry;
#endif


/**
 * <!-- ddr_log_Freq -->
 *
 * @brief   This function is called to log a DDR frequency change event in SMEM.
 *
 * @param   event: The event which occurred (CHANGE, FLUSH, or DONE)
 *
 * @return  None
 */
void ddr_log_freq(enum event event)
{
  static boolean freq_started = FALSE;
  //static int     last_index = 0;
  uint64_t now = time_service_now();
#ifdef LOCAL_FREQ_ARRAY
  uint32 index = local_switch_log_index++;

  CORE_VERIFY(index < NUM_ENTRIES(local_freq_switch_table));
  FREQ_SWITCH_TABLE *freq_stats = &local_freq_switch_table[index];
#else
  FREQ_SWITCH_TABLE *freq_stats = &local_freq_switch_entry;
#endif

  // Only log done changes if we actually did work.
  if (event != FREQ_DONE)
  {
    freq_started = TRUE;        // Frequency change started.;
  }
  else if (freq_started == FALSE)
  {
    // If AOP receives a request that results in no work, do not log it.
    return;
  }
  freq_stats->timestamp = time_service_now();
  //freq_switch_table_ptr[index].delta = (1000 * (freq_switch_table_ptr[index].timestamp - freq_switch_table_ptr[last_index].timestamp) / 19200);

  freq_stats->event              = (uint8) event;
  freq_stats->powerstate_request = ddr_status.ddr_arc_level;
  freq_stats->bcm_acv            = ddr_status.ddr_bcm_acv_level;
  freq_stats->bcm_alc            = ddr_status.ddr_bcm_alc_level;

  freq_stats->bcm_mc_request     = (ddr_status.ddr_bcm_mc_level << 4)   | ddr_status.ddr_reg_mc_level;
  freq_stats->bcm_shub_request   = (ddr_status.ddr_bcm_shub_level << 4) | ddr_status.ddr_reg_shub_level;
  freq_stats->current_mc_cp      = (ddr_status.ddr_next_mc_lvl << 4)    | ddr_status.current_mc_cp;
  freq_stats->current_shub_cp    = (ddr_status.ddr_next_shub_lvl << 4)  | ddr_status.current_shub_cp;

#ifdef LOCAL_FREQ_ARRAY
  // If DDR is ON, we copy the local log data out.  If not, we wait.
  if (ddr_status.current_mc_cp > 0)
  {
    for (index = 0; index < local_switch_log_index; index++)
    {
      if (local_switch_log_index != 1)
      {
        #ifndef DDR_SWEVENTS
        ddr_log_msg8("SwtchIdx", local_switch_log_index, 0, index, freq_switch_log_index);
        #endif
        //DDR_MGR_SWEVENT(ddr_LOCAL_SWITCH_LOG_INDEX, local_switch_log_index, 0, index, freq_switch_log_index);
      }
      memcpy (freq_switch_table_ptr + freq_switch_log_index,
              &local_freq_switch_table[index],
              sizeof(*freq_switch_table_ptr));
      ++freq_switch_log_index;
      freq_switch_log_index = freq_switch_log_index % FREQ_SWITCH_COUNT;
    }
    local_switch_log_index = 0;
  }
#else
  // Copy the local log data entry out to smem.
  memcpy (freq_switch_table_ptr + freq_switch_log_index,
          &local_freq_switch_entry,
          sizeof(*freq_switch_table_ptr));
  ++freq_switch_log_index;
  freq_switch_log_index = freq_switch_log_index % FREQ_SWITCH_COUNT;
#endif
}



/**
 * <!-- update_alc_values -->
 *
 * @brief If the flag changed in the debug build, this updates the
 *        configuration based on the provided mask.
 *
 * @param new_mask - 4 bits per clock plan, the ALC override to use.
 *
 * @return None
 */
void update_alc_values(uint64_t new_mask)
{
  int i;

  for (i = 0; i < 16; i++)
  {
    local_mc_states[i].alc_override = new_mask & 0xF; // Set the ALC override to the low 4 bits of the mask
    new_mask>>=4;                                  // Shift the low bits of the mask out.
  }
}

/**
 * <!-- build_alc_mask -->
 *
 * @brief This function creates the mask (4 bits per CP)
 *        of what the ALC override values would be (if enabled).
 *
 * @param new_mask - 4 bits per clock plan, the ALC override to use.
 *
 * @return None
 */
uint64_t build_alc_mask(void)
{
  int i;
  uint64_t ret_val = 0;

  // Start with themax CP, and go down to 0.
  for (i = 15; i >= 0; i--)
  {
    ret_val = ((ret_val << 4) | (local_mc_states[i].alc_override & 0x0F));
  }

  return ret_val;
}

// ddr_rm_vote
//
// This function sends our vote to the DDR Powerstate ARC.
//
// Paramters:
//   state:  The desired state of the DDR Resource Manager.
//
// Return:
//   None.
//
void ddr_rm_vote(enum DDR_OLS state)
{
  if (state != ddr_status.ddr_rm_vote) {
    HWIO_OUTI(RMm_IRQ_CLR, RPMH_ARC_DDR_SS, IRQ_TIMER_EXPIRED_BITMASK);
    (void) arc_send_vote(RESOURCE_DDR, CLIENT_DDR_FREQUENCY, state, FireForget);
    ddr_status.ddr_rm_vote = state;
  }
}

bool g_voltages_pending = FALSE;
bool pmic_vote_pending = FALSE;

/**
 * <!-- pmic_vote_callback -->
 *
 * @brief   This function handles the PMIC vote being completed.
 *
 * @param   request_id which has completed.   Note, the call to cast the vote
 *          may still be in progress when this callback is invoked.   Thus, the
 *          request_id may or may not yet have been given to the caller.
 *
 * @return  N/A
 */
void pmic_vote_callback (uint32_t req_id)
{
  #ifndef DDR_SWEVENTS
  ddr_log_msg8("PMIC CB ", req_id >> 8, req_id, pmic_vote_pending, ddr_status.ddr_pmic_mode_vote);
  #endif
  pmic_vote_pending = FALSE;
  rex_set_sigs(&ddr_fsm_tcb, SIG_VOTE_DONE_INT);
}

/**
 * <!-- common_arc_callback -->
 *
 * @brief   This function handles the Voltage vote being completed.
 *
 * @param   arc: Which arc has completed
 *          log: What message to put in the logging
 *          req_id: which request has completed.
 *          Note, the call to cast the vote may still be in progress when this
 *          callback is invoked.   Thus, the request_id may or may not yet have
 *          been given to the caller.
 *
 * @return  N/A
 */
void common_arc_callback (VOLTAGE_RESOURCES arc, char *log, uint32_t req_id)
{
  #ifndef DDR_SWEVENTS
  ddr_log_msg8(log, req_id >> 8, req_id, voltage_arc_status[arc].votes_pending, 0);
  #endif
  voltage_arc_status[arc].votes_pending = FALSE;
  rex_set_sigs(&ddr_fsm_tcb, SIG_VOTE_DONE_INT);
}

/**
 * <!-- cx_callback -->
 *
 * @brief   This function handles the CX vote being completed.
 *
 * @param   request_id which has completed.
 *
 * @return  N/A
 */
void cx_callback (uint32_t req_id)
{
  common_arc_callback(RESOURCE_CX, "CXVoteCB", req_id);
}

/**
 * <!-- mx_callback -->
 *
 * @brief   This function handles the MX vote being completed.
 *
 * @param   request_id which has completed.
 *
 * @return  N/A
 */
void mx_callback (uint32_t req_id)
{
  common_arc_callback(RESOURCE_MX, "MXVoteCB", req_id);
}

/**
 * <!-- vdda_callback -->
 *
 * @brief   This function handles the VDDA vote being completed.
 *
 * @param   request_id which has completed.
 *
 * @return  N/A
 */
void vdda_callback (uint32_t req_id)
{
  common_arc_callback(RESOURCE_VDDA, "VAVoteCB", req_id);
}

/**
 * <!-- is_cx_pc_path -->
 *
 * @brief   This function is invoked by the uNPA flow for the MX rail.  It
 *          handles the weird MX dependencies we have which require MX to be
 *          moving to TURBO when CX is going to/from collapse.
 *
 * @param   cx_state: Current CX vote from uNPA
 *          mx_state: Current MX vote from uNPA
 *
 * @return  TRUE: Apply the TURBO vote.  False, no CX collapse requirements.
 */
extern bool g_cx_is_collapsed;
bool is_cx_pc_path (unpa_resource_state cx_state, unpa_resource_state mx_state)
{
  unsigned int current_cx_level = get_arc_current_level(RESOURCE_CX);

  ddr_log_msg8("CX_REST ", current_cx_level, cx_state >> 4, voltage_arc_status[RESOURCE_CX].sequence_level, g_cx_is_collapsed);

  // CX Restore from collapse case.
  if ((g_cx_is_collapsed) &&
      ((cx_state >= MIN_SVS) || (voltage_arc_status[RESOURCE_CX].sequence_level > 0)))
  {
    return TRUE;
  }

  // CX collapse case.
  if ((!g_cx_is_collapsed) &&
      (voltage_arc_status[RESOURCE_CX].sequence_level == 0) &&
      (current_cx_level != 0))
  {
    return TRUE;
}

  return FALSE;
}

/**
 * <!-- ddr_mgr_poll -->
 *
 * @brief When we need to poll for something, this function 
 *        the DDR_TEST is resumed.
 *
 * @param None
 *
 * @return None
 */
void ddr_mgr_poll(uint64_t lateness)
{
  rex_set_sigs(&ddr_fsm_tcb, SIG_POLL_TIMER);
}

#ifndef MIN_SIZE
// This function is called when we have the callback from the
// bandwidth request.
void icb_callback( void *callback_data, uint32_t req_data )
{
  // Each callback means we are now on.  So, if I get the callback, assume DCC path is on.
  rex_set_sigs(&ddr_fsm_tcb, SIG_DRIVER_DCC_BANDWIDTH);
}

// ddr_vote_dcc_path
//
// This function votes the BCM path for DCC to talk to DDR.
//
// Paramters:
//   on_off:  TRUE: vote the path on (with completion)  FALSE: vote it off (F&F)
//
// Return:
//   None
//
void ddr_vote_dcc_path(boolean on_off)
{
  if (on_off != ddr_status.ddr_dcc_vote)
  {
    ddr_status.ddr_dcc_vote = on_off;

    if (on_off)
    {
      ddr_status.dcc_on_request = 0;
      if (g_ddr_vote_to_icb)
      {
        ddr_status.dcc_on_request = icb_enable_route(icb_handle, TRUE);
        ddr_log_msg32("BusVt On", ddr_status.dcc_on_request);
      }
      if (ddr_status.dcc_on_request == 0)
      {
        ddr_status.ddr_dcc_on = TRUE;
        freq_fsm_signal_dcc_bandwidth_complete();
      }
    }
    else
    {
      if (g_ddr_vote_to_icb)
      {
        ddr_log_msg8 ("BusVtOff", ddr_status.ddr_dcc_on, 0, 0, 0);
        (void) icb_disable_route(icb_handle, FALSE);
      }
      ddr_status.ddr_dcc_on = FALSE;
    }
  }
}
#endif

// arc_xo_rm_vote
//
// Vote the XO ARC ON/OFF.
//
// Paramters:
//   state: XO State
//   CompletionRequired:  Whether completion is required or vote can go fire and forget
//
// Return:
//   None
//
void arc_xo_rm_vote(unsigned int state, boolean CompletionRequired)
{
  if (state != ddr_status.ddr_xo_vote) {
    ddr_status.ddr_xo_vote = state;
    (void) arc_send_vote(RESOURCE_XO, CLIENT_DEPENDENCY, state, CompletionRequired);
    //Update the xo_vote.
    //xo_on state will be updated in the callback for XO_ON case.
    if (state == XO_BUFFER_OFF)
    {
      ddr_log_msg32("XO OFF  ", 0);
      ddr_status.ddr_xo_on = FALSE;
    } 
  }
}

/**
 * <!-- vlvl_to_hlvl -->
 *
 * @brief   Converts a VLVL to an HLVL
 *
 * @param   arc: is the resource being queried
 * @param   vlvl:  The VLVL to convert
 *
 * @return  The HLVL
 */
static uint32 vlvl_to_hlvl (VOLTAGE_RESOURCES arc, uint32 vlvl)
{
  static int res_idx[NUM_VOLTAGE_RESOURCES] = {-1, -1, -1};
  int hlvl = 0;

  if (res_idx[arc] < 0)
  {
    res_idx[arc] = pwr_utils_lvl_resource_idx (ddr_npa.resources[arc].name);
    CORE_VERIFY (res_idx[arc] >= 0);
  }

  hlvl = pwr_utils_hlvl (res_idx[arc], vlvl);

  CORE_VERIFY (hlvl >= 0);

  return hlvl;
}

/**
 * <!-- hlvl_to_vlvl -->
 *
 * @brief Convert the ARC operating level to the chip independent voltage levels.
 *
 * @param arc: Which ARC Resource is being queried.
 * @param hlvl: The hardware operating level
 *
 * @return The target independent voltage level.
 */
static uint32 hlvl_to_vlvl (VOLTAGE_RESOURCES arc, uint32 hlvl)
{
  static int res_idx[NUM_VOLTAGE_RESOURCES] = {-1, -1, -1};
  int vlvl = RAIL_VOLTAGE_LEVEL_OFF;

  if (res_idx[arc] < 0)
  {
    res_idx[arc] = pwr_utils_lvl_resource_idx (ddr_npa.resources[arc].name);
    CORE_VERIFY (res_idx[arc] >= 0);
  }

  vlvl = pwr_utils_vlvl (res_idx[arc], hlvl);

  CORE_VERIFY (vlvl >= 0);

  return vlvl;
}

// AOP's aggregated vote.
// get_aop_needs
//
// This function returns the AOP's needs (value it will vote) from the driver.
// This function is necessary as AOP will have multiple "clients" voting each
// regulator, and we need to get the MAX vote out.
//
// Paramters:
//   arc: Which ARC resource are we interested in
//
// Return:
//   The value AOP should vote to that ARC, as a HLVL
unsigned int get_aop_needs(VOLTAGE_RESOURCES arc)
{
  uint32_t vote;

  switch (arc) 
  {
    case RESOURCE_MX:
      vote = ddr_status.ddr_mx_vote;
      break;
    case RESOURCE_CX:
      vote = ddr_status.ddr_cx_vote;
      break;
    case RESOURCE_VDDA:
      vote = ddr_status.ddr_vdda_vote;
      break;
    default:
      return 0;
  }

  return vlvl_to_hlvl (arc, vote);
}

/**
 * <!-- arc_send_vote -->
 *
 * @brief This routine sends a vote to the associated RESOURCE.   Voltage ARCs are
 *        converted to vlvls to be consistent.   PMIC resource votes are sent as is,
 *        XO and DDR Powerstate ARC, the levels are sent as is.
 *
 * @param arc:  Which ARC gets the vote.
 * @param vote: Votes value
 *
 * @return Any request ID
 */
uint32 arc_send_vote (VOLTAGE_RESOURCES arc, AOP_INTERNAL_CLIENT client, unsigned int level,
                    enum PCBCompletionType completion)
{
  uint32 request_id = 0;
  #ifndef DDR_SWEVENTS
  ddr_log_msg8("SendVote", arc, client, level>>8, level);
  #endif
  //DDR_MGR_SWEVENT(ddr_ARC_SEND_VOTE, arc, client, level);
  if ((arc != RESOURCE_DDR) && (arc != RESOURCE_XO))
  {
    ddr_npa_client_t *ddr_npa_client = &ddr_npa.resources[arc].clients[client];
    if (ddr_npa_client->client_handle == NULL)
    {
      ddr_npa_client->client_handle = unpa_create_client (ddr_npa_client->name, UNPA_CLIENT_REQUIRED,
                                                          ddr_npa.resources[arc].name);
      CORE_VERIFY (ddr_npa_client->client_handle != NULL);
    }

    if (completion == FireForget)
    {
      unpa_set_request_attribute (ddr_npa_client->client_handle, UNPA_REQUEST_FIRE_AND_FORGET);
    }

    if (arc == RESOURCE_PMIC) 
    {
#ifndef SDX24_STUBS
      pmic_vote_pending = TRUE;
      unpa_issue_request_async (ddr_npa_client->client_handle, level, pmic_vote_callback);
#else
      unpa_issue_request (ddr_npa_client->client_handle, level);
#endif
    }
    else 
    {
      if( completion == FireForget)
      {
        unpa_issue_request (ddr_npa_client->client_handle, level);
      }
      else
      {
        // A Vote is pending until its not.
        g_voltages_pending = TRUE;
        voltage_arc_status[arc].votes_pending = TRUE;
        request_id = unpa_issue_request_async (ddr_npa_client->client_handle, level,
                                                      voltage_arc_status[arc].voltage_vote_callback);
        voltage_arc_status[arc].vote_req_id = request_id;
        if (request_id == 0)
        {
          voltage_arc_status[arc].votes_pending = FALSE;
        }
      }
    }
  }
  else
  {
    // TODO: evaluate if we should add a npa resource for xo and ddr as well?
    uint32 resource = 0;

    switch (arc)
    {
      case RESOURCE_DDR:
        resource = ddr_arc_address;
        break;
      case RESOURCE_XO:
        resource = xo_arc_address;
        break;
      default:
        CORE_VERIFY(0);
    }
    request_id = rpmh_issue_command (aop_ddr_client, RPMH_SET_ACTIVE,
                               completion==CompletionRequired?TRUE:FALSE,
                               resource, level);

    //Request notification on completion for XO Up requests,
    //of type CompletionRequired,
    if ((arc == RESOURCE_XO) && (completion))
    {
      g_xo_seq_req_id = request_id;
      ddr_log_msg32("XO start", request_id);
      if (request_id)
      {
        g_xo_on_time = time_service_now();
        rpmh_notify_completion_single(aop_ddr_client, request_id);
      }
      else
      {
        aop_ddr_client_callback(0);
      }
    }
  }

  return request_id;
}

// signal_bcm_alc_sequence_complete
//
// This function provides a mechanism to indicate to BCM that the ALC sequence is complete.
//
// Paramters:
//   None.
//
// Return:
//   none.
void signal_bcm_alc_sequence_complete()
{
   HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_0_CLEAR, 1 << BCM_ALC_CLOCK_DOMAIN);
   HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_0_EN, 1 << BCM_ALC_CLOCK_DOMAIN, 1 << BCM_ALC_CLOCK_DOMAIN);
   HWIO_OUTI(SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm, BCM_ALC_CLOCK_DOMAIN, 1);
}

// signal_bcm_acv_sequence_complete
//
// This function provides a mechanism to indicate to BCM that the ACV sequence is complete.
//
// Paramters:
//   None.
//
// Return:
//   none.
void signal_bcm_acv_sequence_complete()
{
   HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_0_CLEAR, 1 << BCM_ACV_CLOCK_DOMAIN);
   HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_0_EN, 1 << BCM_ACV_CLOCK_DOMAIN, 1 << BCM_ACV_CLOCK_DOMAIN);
   HWIO_OUTI(SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm, BCM_ACV_CLOCK_DOMAIN, 1);
}

// freq_fsm_signal_frequency_sequence_complete
//
// Signals the frequency_fsm that the frequency sequence has completed.
// This signal occurs when we are in the "Wait for Frequency" state, and
// signifies that we can leave that state.
//
// It is called as a result of the sequencer_fsm completing the frequency
// sequence assigned.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_frequency_sequence_complete(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_FREQUENCY_SEQUENCE_COMPLETE);
}

#ifndef MIN_SIZE
// freq_fsm_signal_dcc_bandwidth_complete
//
// Signals the frequency_fsm that the frequency sequence has completed.
// This signal occurs when we are in the "Wait for Frequency" state, and
// signifies that we can leave that state.
//
// It is called as a result of the sequencer_fsm completing the frequency
// sequence assigned.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_dcc_bandwidth_complete(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_DCC_COMPLETE);
}
#endif

// freq_fsm_signal_amc_sequence_complete
//
// Signals the frequency_fsm that the TCS issued in AMC mode has completed.
// AMC is currently used when performing the read of flush data, a cache flush itself,
// and Active/Idle FAL indication.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_amc_sequence_complete(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_AMC_COMPLETE);
}

// service_bcm_interrupt
//
// This function is called when BCM interrupts the AOP indicating a new CP is
// ready.
//
// Paramters:
//   None.
//
// Return:
//   None.
void service_bcm_interrupt(void)
{
  freq_fsm_signal_new_frequency();   // Signal the Frequency State Machine
}

// service_fal_interrupt
//
// This function is called when BCM interrupts the AOP indicating a new CP is
// ready for the ALC/ACV.   If we don't have one in progress, it programs TCS #2
// and sends it on its way.
//
// Paramters:
//   None.
//
// Return:
//   None.
void service_fal_interrupt(void) {
  if (ddr_status.fal_in_progress == FALSE) {
    unsigned long tcs_cmd;
    unsigned long tcs_index = 0;
    unsigned long cmd_to_bitmask[] = { 1, 0x33, 0x77, 0xFF };

    // If we are overriding around BCM's ACV request, ack
    if (g_alc_override)
    {
      // The override excludes BCM voting.
      if (ddr_status.bcm_alc_pending == FAL_PENDING)
      {
        ddr_status.bcm_alc_pending = FAL_IDLE;
        signal_bcm_alc_sequence_complete();
      }
      // Even though we overrode ALC, we cannot process ACV as SHRM
      // cannot handle both.   If we need to handle ACV, it will have 
      // to be serialized with the frequency_fsm.
      if (ddr_status.bcm_acv_pending == FAL_PENDING)
      {
        ddr_status.bcm_acv_pending = FAL_IDLE;
        signal_bcm_acv_sequence_complete();
      }
      return;
    }

    if (ddr_status.tcs_2_pending == TRUE)
    {
      uint32 tcs_stat = HWIO_INI(RSI_TCSm_STAT, 2); // Status of TCS2
      #ifndef DDR_SWEVENTS
      ddr_log_msg32("ERR:TCS2", tcs_stat);
      #endif
      DDR_MGR_SWEVENT(ddr_SERVICE_FAL_TCS2, tcs_stat);
      return;
    }
    // program TCS 2 in AMC mode
    if (ddr_status.bcm_acv_pending == FAL_PENDING)
    {
      ddr_status.bcm_acv_pending = FAL_SEQUENCING;
      tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_FAL_ACV,
                               0, ddr_status.ddr_bcm_acv_level);
      HWIO_OUTI2(RSI_TCSm_CMDt, FAL_TCS, tcs_index, tcs_cmd);
      tcs_index++;
    }
    if (ddr_status.bcm_alc_pending == FAL_PENDING)
    {
      ddr_status.bcm_alc_pending = FAL_SEQUENCING;
      tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_FAL_ALC,
                               0, ddr_status.ddr_bcm_alc_level);
      HWIO_OUTI2(RSI_TCSm_CMDt, FAL_TCS, tcs_index, tcs_cmd);
      tcs_index++;
    }

    if (tcs_index > 0)
    {
      ddr_status.fal_in_progress = TRUE;

      HWIO_OUTI(RSI_TCSm_CFG, FAL_TCS, cmd_to_bitmask[tcs_index - 1]);
      // Put the TCS into AMC mode.
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FAL_TCS, AMC_ENABLE, 1);
      // Trigger the TCS in AMC mode.
#ifdef ALLOW_SHRM
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FAL_TCS, TCS_START, 0);
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FAL_TCS, TCS_START, 1);
      ddr_status.tcs_2_pending = TRUE;
#else
      service_fal_complete();
#endif
    }
  }
}

// service_fal_complete
//
// This function is called when TCS2 is completed.  It informs BCM that any FAL
// commands that were sequencing are now done, and then checks if there are any
// further pending FAL requests.
//
// Paramters:
//   None.
//
// Return:
//   None.
void service_fal_complete()
{
  if (ddr_status.bcm_acv_pending == FAL_SEQUENCING) {
    ddr_status.bcm_acv_pending = FAL_IDLE;
    signal_bcm_acv_sequence_complete();
  }
  if (ddr_status.bcm_alc_pending == FAL_SEQUENCING) {
    ddr_status.bcm_alc_pending = FAL_IDLE;
    signal_bcm_alc_sequence_complete();
  }

  ddr_status.fal_in_progress = FALSE;
  // Service any additional FAL commands now.
  service_fal_interrupt();
}

// service_ddr_rm_interrupt
//
// This function is called when DDR_RM interrupts the AOP indicating a new DDR
// State is desired.
//
// Paramters:
//   None.
//
// Return:
//   None.
void service_ddr_rm_interrupt(void)
{
  unsigned int rm_state = HWIO_INFI(RMm_OPERATIONAL_LEVEL, RPMH_ARC_DDR_SS, SEQUENCE_OL);
  unsigned int timer_expired = HWIO_INFI (RMm_IRQ_STATUS, RPMH_ARC_DDR_SS, RM_IRQ_STATUS) & IRQ_TIMER_EXPIRED_BITMASK;
  //Workaround for Timer wakeups
  if (timer_expired != 0)
  {
    HWIO_OUTI(RMm_IRQ_CLR, RPMH_ARC_DDR_SS, IRQ_TIMER_EXPIRED_BITMASK);
    if (rm_state < D0_On)
    {
      // Check and set mol, clear the IRQ
      HWIO_OUTFI(RMm_OPERATIONAL_LEVEL, RPMH_ARC_DDR_SS, SEQUENCE_OL, D0_On);
      rm_state = D0_On;
    }
  }

  ddr_status.ddr_rm_vote_on = (rm_state >= D0_On);
  ddr_status.ddr_arc_level = rm_state;
  ddr_status.ddr_rm_pending = TRUE;

  freq_fsm_signal_new_frequency();   // Signal the Frequency State Machine
}

// seq_fsm_signal_voltage_sequence_complete
//
// Signals the sequencer_fsm that the voltage sequence has completed.  This causes the
// sequencer_fsm to signal the voltage management software and return to the idle state.
//
// Paramters:
//   None.
//
// Return:
//   None.
void seq_fsm_signal_voltage_sequence_complete(void)
{
  aop_fsm_process_event(&sequencer_fsm, FSM_VOLTAGE_SEQUENCE_COMPLETE);
}

// seq_fsm_signal_frequency_sequence_complete
//
// Signals the sequencer_fsm that the frequency sequence has completed.  This causes the
// sequencer_fsm to signal the frequency_fsm and return to the idle state.
//
// Paramters:
//   None.
//
// Return:
//   None.
void seq_fsm_signal_frequency_sequence_complete(void)
{
  aop_fsm_process_event(&sequencer_fsm, FSM_FREQUENCY_SEQUENCE_COMPLETE);
}

/**
 * <!-- rail_change_handler -->
 *
 * @brief  Handle events (Aggregation, Prework, Postwork) coming from the ARC
 *
 * @param  c - indicates which ARC has the event
 * @param  e - indicates the event
 * @param  ed - Event data indicates the sequence level
 * @param  cd - Client data (not used)
 *
 * @return c->blocked if we want to keep the ARC blocked.
 * @return value the desired sequence level
 */
static uint32 rail_change_handler (arc_runtime_client_t *c, arc_runtime_event_t e,
                                   uint32 ed, uint32 cd)
{
  uint32 arc = rm_to_arc[c->arc];
  unsigned int aop_needs = get_aop_needs(arc);
  uint32 desired_sequence_level = ed;

  c->blocked = true;

  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("RailChng", arc, e, aop_needs, desired_sequence_level);
  #endif
  DDR_MGR_SWEVENT(ddr_rail_change_handler, arc, e, aop_needs, desired_sequence_level);

  // Special case, we started the next sequence before handling the sequence done.
  if ((voltage_arc_status[arc].sequencing) && (e == ARC_RUNTIME_EVENT_PREWORK))
  {
    voltage_arc_status[arc].pending = ARC_PENDING_DEFERRED_SEQUENCE;
    voltage_arc_status[arc].sequence_level = desired_sequence_level;
    return desired_sequence_level;
  }

  if ((arc == RESOURCE_CX) && 
      (desired_sequence_level >= voltage_arc_status[arc].mol) &&
      (voltage_arc_status[arc].current < voltage_arc_status[arc].mol))
  {
    //Turn XO back on, when bringing CX above MOL.   This is required
    //for the CPR Workaround
    arc_xo_rm_vote(XO_ON, TRUE);
  }

//in sync with NapaliV2

  if ((arc == RESOURCE_CX) && (g_cpr_disabled == TRUE) &&
      (desired_sequence_level >= voltage_arc_status[arc].mol))
  {
    // Turn ON CX, MX CPRF CTRL_INTERACTION_EN bit
    // at CX pre work, when CX is about to go above MOL
    cpr_enable_controller(RPMH_ARC_VDD_CX, TRUE);
    cpr_enable_controller(RPMH_ARC_VDD_MX, TRUE);
    g_cpr_disabled = FALSE;
  }

  // CX Collapse doesn't use the DDR_AUX Sequencer
  if ((arc == RESOURCE_CX) && (desired_sequence_level == 0) && (e == ARC_RUNTIME_EVENT_PREWORK))
  {
    voltage_arc_status[arc].in_sequence_level = voltage_arc_status[arc].sequence_level = desired_sequence_level;
    voltage_arc_status[arc].pending = ARC_PENDING_NONE;
    voltage_arc_status[arc].current = 0;
    c->blocked = false;
    return voltage_arc_status[arc].sequence_level;
  }

  if (arc == RESOURCE_CX)
  {
      (void) arc_send_vote(RESOURCE_MX, CLIENT_DEPENDENCY,
                           cx_to_mx_map[MAX(aop_needs, desired_sequence_level)], FireForget);
  }

      voltage_arc_status[arc].pending = ARC_PENDING_SEQUENCE;
  voltage_arc_status[arc].sequence_level = desired_sequence_level;

  if (voltage_dependency_ready())
  {
    seq_fsm_signal_start_voltage();   // Signal the Sequence State Machine to
                                      // perform voltage processing
  }

  return desired_sequence_level;
}

// freq_fsm_signal_new_frequency
//
// Signals the frequency_fsm that either BCM or the DDR RM ARC has
// indicated that something may be changing.  This function only sends
// the approprate event to the Frequency FSM.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_new_frequency(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_FREQUENCY_CHANGE_REQUEST);
}

// encode_tcs_cmd
//
// This function encodes the TCS value for RSI Commands.  RSI Commands have
// a specific format.  This function understands that format and encodes the long word.
//
// Paramters:
//   response_req = RSI_FF (Fire/Forget) or RSI_RESP (RESPonse Required)
//   last = RSI_NOTLAST (More commands in TCS are coming) or RSI_LAST (Last Cmd in group)
//   opcode = RSI_OPCODE_*  Specifies the opcode.
//   id = RSI Resource ID DDR_SS/LLC/SHUB/MC  - see command specifics
//   data = data field for the command (typically the powerstate/operating level)
//
// Return:
//   The 32-bit encoded value.
unsigned long encode_tcs_cmd(unsigned int response_req,
                             unsigned int last, unsigned int opcode,
                             unsigned int id, unsigned int data)
{
  unsigned long tcs_cmd;

  tcs_cmd =  (RSI_REQ      & 0x0003) << 26;  // Bits 26:27
  tcs_cmd |= (response_req & 0x0001) << 25;  // Bits 25:25
  tcs_cmd |= (last         & 0x0001) << 24;  // Bits 24:24
  tcs_cmd |= (opcode       & 0x003F) << 19;  // Bits 19:23
  tcs_cmd |= (id           & 0x0007) << 16;  // Bits 16:18
  tcs_cmd |= (data         & 0xFFFF);        // Bits 0:15

  return tcs_cmd;
}

// encode_tcs_gpr
//
// This function encodes the TCS value for General Purpose Read Commands.  RSI Commands have
// a specific format.  This function understands that format and encodes the long word.
// Note: This function is currently not used.
//
// Paramters:
//   last = RSI_NOTLAST (More commands in TCS are coming) or RSI_LAST (Last Cmd in group)
//   addr = address for the read command (24 bit quantity).
//
// Return:
//   The 32-bit encoded value.
unsigned long encode_tcs_gpr(unsigned int last, unsigned int addr)
{
  unsigned long tcs_cmd;
  tcs_cmd =  (RSI_GPR  & 0x0003) << 26;   // Bits 26:27  - General Purpose Read
  tcs_cmd |= (RSI_RESP & 0x0001) << 25;   // Bits 25:25  - Always need a response to read.
  tcs_cmd |= (last     & 0x0001) << 24;   // Bits 24:24  - is this the last read?
  tcs_cmd |= (addr     & 0xFFFFFF);       // Address being read.

  return tcs_cmd;
}

/**
 * <!-- arc_get_client_vote -->
 *
 * @brief Return the vote for a given client
 *
 * @param arc - which resource is being queried
 * @param client - which client vote is being requested.
 *
 * @return  The client's vote as an HLVL
 */
unsigned int arc_get_client_vote (VOLTAGE_RESOURCES arc, AOP_INTERNAL_CLIENT client)
{
  ddr_npa_client_t *ddr_npa_client = &ddr_npa.resources[arc].clients[client];

  if (ddr_npa_client->client_query_handle == NULL)
  {
    ddr_npa_client->client_query_handle = unpa_get_client_query_handle (ddr_npa_client->name,
                                                                        ddr_npa.resources[arc].name);
    CORE_VERIFY (ddr_npa_client->client_query_handle != NULL);
  }

  unpa_query_result result;
  unpa_query_status status = unpa_query (ddr_npa_client->client_query_handle, UNPA_QUERY_ACTIVE_STATE,
                                         &result);

  CORE_VERIFY (status == UNPA_QUERY_SUCCESS);

  return vlvl_to_hlvl (arc, result.val);
}

/**
 * <!-- min_current_arc_status -->
 *
 * @brief Determines the lowest level we expect the ARC based on current information.
 *        If the ARC is stable, the current level is returned.  If a aggregation or sequence is
 *        pending/in_progress, the lowest level we'll see is returned.
 *
 * @param arc - the desired ARC instance. Only applicable to CX, MX, VDDA.
 *
 * @return Returns a Hardware Level (0-15)
 */
uint32 min_current_arc_status (uint32 arc)
{
  unsigned voltage_state = MIN(voltage_arc_status[arc].current,
                               voltage_arc_status[arc].sequence_level);

  if (voltage_arc_status[arc].sequencing) {
    voltage_state = MIN(voltage_state, voltage_arc_status[arc].in_sequence_level);
  }

  return voltage_state;
}

// frequency_needs_met
//
// Determine if the frequency needs have been met.  This function is called if
// to determine if DDR's needs are met by the current voltage values.
//
// Paramters:
//   None.
//
// Return:
//   TRUE: DDR doesn't need different than the current values.
//
boolean frequency_needs_met(void)
{
  int i;

  // We can signal if there are votes pending and voting is finished.
  boolean signal_freq_fsm = !freq_fsm_voting_in_progress;
  uint32 min_arc_status[3];
  min_arc_status[0] = min_current_arc_status(RESOURCE_MX);
  min_arc_status[1] = min_current_arc_status(RESOURCE_CX);
  min_arc_status[2] = min_current_arc_status(RESOURCE_VDDA);
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("FreqNeed", pmic_vote_pending, min_arc_status[0], min_arc_status[1], min_arc_status[2]);
  #endif
  //DDR_MGR_SWEVENT(ddr_freq_needs, pmic_vote_pending, min_arc_status[0], min_arc_status[1], min_arc_status[2]);

  if (ddr_status.ddr_xo_on == FALSE)
  {
    return FALSE;
  }

  if (pmic_vote_pending == TRUE)
  {
    return FALSE;
}

  // Determine if there are still votes outstanding.
  if (g_voltages_pending) 
    {
    g_voltages_pending = FALSE;
    for (int i = RESOURCE_MX; i < NUM_VOLTAGE_RESOURCES; i++)
      {
      if (voltage_arc_status[i].votes_pending)
      {
        g_voltages_pending = TRUE;
    }
  }

    if (g_voltages_pending)
    {
      ddr_log_msg32 ("VoltPend", g_volt_seq_req_id);
      return FALSE;
    }
  }

  // Voting is complete, Now check if anything was outstanding.
  return signal_freq_fsm;
}

// freq_fsm_signal_process_complete
//
// Signals the frequency_fsm that we have completed processing the
// current requests.  This is an internal signal only.
// The "process" state is always a transient state in the FSM.  We
// come here whenever we need to see if there is work to do, and then
// exit immediatley to a state which reflects the work to be done.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_process_complete(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_PROCESS_COMPLETE);
}

// freq_fsm_signal_voltage_requirements_met
//
// Signals the frequency_fsm that the voltage requirements have been met.
// This signal occurs when we are in the "Wait for Voltage" state, and
// signifies that we can leave that state.
//
// It is called from 1 of 2 locations.  The first is when the sequencer
// completes a voltage sequence, if the needs are met, then this signal is
// sent.  The second is when we are finished determining the needs for the
// frequency FSM.  If the needs are met by our previous vote, then this is
// also sent.
//
// Paramters:
//   None.
//
// Return:
//   None.
void freq_fsm_signal_voltage_requirements_met(void)
{
  aop_fsm_process_event(&frequency_fsm, FREQ_FSM_VOLTAGE_REQUIREMENTS_MET);
}

// seq_fsm_signal_start_voltage
//
// Signals the sequencer_fsm that the voltages pending are ready to be sequenced.   This is
// called after the voltage manager has determined we have reach the correct grouping of
// regulators to sequence.  When the sequencer fsm is at idle and receives, it gathers all
// the pending ARCs and begins their sequence.
//
// Paramters:
//   None.
//
// Return:
//   None.
void seq_fsm_signal_start_voltage(void)
{
  rex_set_sigs(&ddr_fsm_tcb, SIG_VOLTAGE_SEQ);
}

// seq_fsm_signal_start_frequency
//
// Signals the sequencer_fsm that the frequency sequence is ready to be run.  This is
// called when the frequence_fsm has determined we have reach the state to run the sequence.
// When the sequencer fsm is at idle and receives this signal, the frequency FSM is started.
//
// Paramters:
//   None.
//
// Return:
//   None.
void seq_fsm_signal_start_frequency(void)
{
  aop_fsm_process_event(&sequencer_fsm, FSM_PERFORM_FREQUENCY_CHANGE);
}

// ddr_aux_frequency_sequence_complete
//
// Called when the DDR AUX indicates the frequncy sequence has completed.
//
// Paramters:
//   None.
//
// Return:
//   None.
void ddr_aux_frequency_sequence_complete(void)
{
#ifndef MIN_SIZE
  if (PARALLEL_SEQUENCING) {
    freq_fsm_signal_frequency_sequence_complete();
  } else {
    seq_fsm_signal_frequency_sequence_complete();
  }
#else
  freq_fsm_signal_frequency_sequence_complete();
#endif
}

// freq_mgmt_votes_inprogress
//
// This function provides a mechanism to indicate the frequency FSM has votes
// coming.
//
// When the voting completes, this function will check if the ARCs were all
// already in the AGGREGATED state.
//
// Paramters:
//   voting:  TRUE if voting is going to start from the frequency_fsm.
//
// Return:
//   none.
void freq_mgmt_votes_inprogress(boolean voting)
{
  freq_fsm_voting_in_progress = voting;

  // When voting finishes, signal the state machine to start if it had work to do.
  if (freq_fsm_voting_in_progress == FALSE)
  {
    int arc;
    for (arc = 0; arc < NUM_VOLTAGE_RESOURCES; arc++)
    {
      // If pending is PENDING SEQUENCE, Allow the FSM to continue.
      if (voltage_arc_status[arc].pending == ARC_PENDING_SEQUENCE)
      {
        if (voltage_dependency_ready()) {
          seq_fsm_signal_start_voltage();   // Signal the Sequence State Machine to
                                            // perform voltage processing
        }
        break;
      }
    }
  }
}

// signal_bcm_mc_sequence_complete
//
// This function provides a mechanism to indicate to BCM that the MC sequence is complete.
//
// Paramters:
//   None.
//
// Return:
//   none.
void signal_bcm_mc_sequence_complete()
{
   HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_0_CLEAR, 1 << BCM_MC_CLOCK_DOMAIN);
   HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_0_EN, 1 << BCM_MC_CLOCK_DOMAIN, 1 << BCM_MC_CLOCK_DOMAIN);
   HWIO_OUTI(SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm, BCM_MC_CLOCK_DOMAIN, 1);
}

// signal_bcm_shub_sequence_complete
//
// This function provides a mechanism to indicate to BCM that the SHUB sequence is complete.
//
// Paramters:
//   None.
//
// Return:
//   none.
void signal_bcm_shub_sequence_complete()
{
   HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_0_CLEAR, 1 << BCM_SHUB_CLOCK_DOMAIN);
   HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_0_EN, 1 << BCM_SHUB_CLOCK_DOMAIN, 1 << BCM_SHUB_CLOCK_DOMAIN);
   HWIO_OUTI(SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm, BCM_SHUB_CLOCK_DOMAIN, 1);
}

// arc_unblock_rm_prework
//
// This function writes the CSR to allow the ARC past the prework AOP interrupt.
//
// Paramters:
//   arc_rm:  Which ARC we are writing to.
//
// Return:
//   None.
void arc_unblock_rm_prework(unsigned int arc_rm)
{
  if (arc_rm == DDRSS_POWERSTATE_RM)
  {
    #ifndef DDR_SWEVENTS
    ddr_log_msg8("PS_PreWk", arc_rm, 0, 0, ddr_status.ddr_arc_level);
    #endif
    //DDR_MGR_SWEVENT(ddr_arc_unblock_powerstate_arc, arc_rm);
    HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm, arc_rm, UNBLOCK_SEQ_PRE, 1);
    HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm, arc_rm, UNBLOCK_SEQ_PRE, 0);
  }
  else
  {
    #ifndef DDR_SWEVENTS
    ddr_log_msg8 ("RM_PreWk", arc_rm, voltage_arc_status[rm_to_arc[arc_rm]].current,
                  voltage_arc_status[rm_to_arc[arc_rm]].sequence_level, voltage_arc_status[rm_to_arc[arc_rm]].in_sequence_level);
    #endif
    //DDR_MGR_SWEVENT(ddr_arc_unblock_rm_prework, rm_to_arc[arc_rm], voltage_arc_status[rm_to_arc[arc_rm]].sequence_level);
    arc_runtime_client_event_dispatch (ddr_rails.rails[rm_to_arc[arc_rm]].rc, ARC_RUNTIME_EVENT_PREWORK_UNBLOCK, 0);
  }
}

// get_arc_current_level
//
// This function determines what is the current level for the given ARC.
//
// Paramters:
//   Which ARC resource are we looking at.
//
// Return:
//   The current OL for that ARC.
unsigned int get_arc_current_level(VOLTAGE_RESOURCES arc)
{
  unsigned int hlvl = 0;

  hlvl = HWIO_INFI(RMm_OPERATIONAL_LEVEL, arc_to_rm[arc], CURRENT_OL);

  return hlvl;
}

/**
 * <!-- is_arc_operational -->
 *
 * @brief Determines if the ARC is operational (i.e. >= MOL)
 *
 * @param arc - the desired ARC instance. Only applicable to CX, MX, VDDA.
 *
 * @return true - ARC is >= MOL
 *         false - ARC is < MOL
 */
bool is_arc_operational (rpmh_arc_enum_t rm)
{
  CORE_VERIFY (rm < NUM_VOLTAGE_RESOURCES);
  unsigned int arc = rm_to_arc[rm];
  return (get_arc_current_level (arc) >= voltage_arc_status[arc].mol);
}

/**
 * <!-- is_arc_operational2 -->
 *
 * @brief Determines if the ARC is operational by looking at all
 *        levels(i.e. >= MOL)
 *
 * @param arc - the desired ARC instance. Only applicable to CX, MX, VDDA.
 *
 * @return true - ARC is >= MOL
 *         false - ARC is < MOL
 */
bool is_arc_operational2 (rpmh_arc_enum_t rm)
{
  CORE_VERIFY (rm < NUM_VOLTAGE_RESOURCES);
  unsigned int arc = rm_to_arc[rm];
  uint32 operational_level = HWIO_INI(RMm_OPERATIONAL_LEVEL, arc_to_rm[arc]);

  uint32 dest_ol = (operational_level & HWIO_RMm_OPERATIONAL_LEVEL_DESTINATION_OL_BMSK) >> HWIO_RMm_OPERATIONAL_LEVEL_DESTINATION_OL_SHFT;
  uint32 seq_ol =  (operational_level & HWIO_RMm_OPERATIONAL_LEVEL_SEQUENCE_OL_BMSK) >> HWIO_RMm_OPERATIONAL_LEVEL_SEQUENCE_OL_SHFT;
  uint32 agg_ol =  (operational_level & HWIO_RMm_OPERATIONAL_LEVEL_AGGREGATED_OL_BMSK) >> HWIO_RMm_OPERATIONAL_LEVEL_AGGREGATED_OL_SHFT;
  uint32 sol_ol =  (operational_level & HWIO_RMm_OPERATIONAL_LEVEL_SOLVED_OL_BMSK) >> HWIO_RMm_OPERATIONAL_LEVEL_SOLVED_OL_SHFT ;
  uint32 cur_ol =  (operational_level & HWIO_RMm_OPERATIONAL_LEVEL_CURRENT_OL_BMSK) >> HWIO_RMm_OPERATIONAL_LEVEL_CURRENT_OL_SHFT ;

  return ((dest_ol >= voltage_arc_status[arc].mol) || 
          (seq_ol >= voltage_arc_status[arc].mol)  || 
          (agg_ol >= voltage_arc_status[arc].mol)  || 
          (sol_ol >= voltage_arc_status[arc].mol)  || 
          (cur_ol >= voltage_arc_status[arc].mol));
}

// clear_shrm_frequency
//
// This function clears the bits that set SHRM frequencies.  If CX is not changing, these should
// be cleared.
//
// Paramters:
//   None.
//
// Return:
//   None.
void clear_shrm_frequency(void)
{
  HWIO_OUTM(SEQ_BRANCH_EVENT,
            HWIO_SEQ_BRANCH_EVENT_SHRM_CLK_EN_BMSK | HWIO_SEQ_BRANCH_EVENT_SHRM_BMSK,
            0);
}

// update_shrm_frequency
//
// This function updates the SHRM frequencies necessary.  As CX changes, so does the SHRM frequency.
//
// Paramters:
//   None.  Looks at the voltage_arc_status.
//
// Return:
//   None.
void update_shrm_frequency(void)
{
  unsigned int desired_level;
  unsigned long sequencer_branch_register = 0;

  // TODO: Read the levels from BCM
  if (voltage_arc_status[RESOURCE_CX].sequence_level >= vlvl_to_hlvl(RESOURCE_CX, SVS_L1)) {
    desired_level = SHRM_LEVEL_HIGH;
  } else if (voltage_arc_status[RESOURCE_CX].sequence_level >= vlvl_to_hlvl(RESOURCE_CX, LOW_SVS)) {
    desired_level = SHRM_LEVEL_LOW;
  } else if (voltage_arc_status[RESOURCE_CX].sequence_level >= vlvl_to_hlvl(RESOURCE_CX, MIN_SVS)) {
    desired_level = SHRM_LEVEL_MIN;
  } else {
    desired_level = SHRM_LEVEL_OFF;
  }

  if (!ddr_status.ddr_shrm_on && (desired_level > SHRM_LEVEL_OFF))
  {
    sequencer_branch_register |= (1 << SEQ_BRANCH_TURN_SHRM_ON);
  }
  else if (ddr_status.ddr_shrm_on && (desired_level == SHRM_LEVEL_OFF))
  {
    sequencer_branch_register |= (1 << SEQ_BRANCH_TURN_SHRM_OFF);
  }

  if (ddr_status.ddr_shrm_level != desired_level)
  {
    if (desired_level > ddr_status.ddr_shrm_level)
    {
      HWIO_OUTF(SEQ_GCC_CLKPLAN, SHRM_CP, desired_level);
      // Tell the sequence to change the SHRM Frequency
      sequencer_branch_register |= (1 << SEQ_BRANCH_SHRM_Freq_Up);
    }
    else if (desired_level < ddr_status.ddr_shrm_level)
    {
      HWIO_OUTF(SEQ_GCC_CLKPLAN, SHRM_CP, desired_level);
      sequencer_branch_register |= (1 << SEQ_BRANCH_SHRM_Freq_Down);
    }
#ifdef ALLOW_SHRM
    // If the current and next state are ON, SHRM is operational.
    if ((ddr_status.ddr_shrm_level != SHRM_LEVEL_OFF) &&
        (desired_level != SHRM_LEVEL_OFF))
    {
      sequencer_branch_register |= (1 << SEQ_BRANCH_SHRM_Operational);
    }
#endif
  }

  HWIO_OUTM(SEQ_BRANCH_EVENT,
            HWIO_SEQ_BRANCH_EVENT_SHRM_CLK_EN_BMSK | HWIO_SEQ_BRANCH_EVENT_SHRM_BMSK,
            sequencer_branch_register);
  ddr_status.ddr_shrm_level = desired_level;
  ddr_status.ddr_shrm_on = (desired_level != SHRM_LEVEL_OFF);
}

// apply_cx_above_mol
//
// All handling for stuff when CX goes above MOL
//
// Paramters:
//   None
//
// Return:
//   None.
static void
apply_cx_above_mol (void)
{
//in sync with NapaliV2

      // Toggle CPRF_INTERFACE_EN for MX, GFX
      // after seq is complete and CX has transitioned to MOL.
    // CX Interface need not be toggled since CX is already
    // above MOL.
      // CPRF_INTERFACE_EN for MSS is toggled in MPSS when MSS.
      // exits sleep.

      cpr_toggle_cprf_interface_en(RPMH_ARC_VDD_MX, FALSE);
      cpr_toggle_cprf_interface_en(RPMH_ARC_VDD_MX, TRUE);
#ifndef SDX24_STUBS
      cpr_toggle_cprf_interface_en(RPMH_ARC_VDD_GFX, FALSE);
      cpr_toggle_cprf_interface_en(RPMH_ARC_VDD_GFX, TRUE);
#endif
  // PDC timer WA
  // Apps PDC
  uint32 in = HWIO_IN (RPMH_PDC_APPS_SEQ_PWR_CTRL_OVERRIDE_MASK);
  uint32 out = in &= ~0x1;
  HWIO_OUT (RPMH_PDC_APPS_SEQ_PWR_CTRL_OVERRIDE_MASK, out);

  // Modem PDC
  in = HWIO_IN (RPMH_PDC_MODEM_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in &= ~0x1;
  HWIO_OUT (RPMH_PDC_MODEM_SEQ_PWR_CTRL_OVERRIDE_MASK, out);

#ifndef SDX24_STUBS
  // Audio
#ifndef SDM670_TARGET_CONFIG 
  in = HWIO_IN (RPMH_PDC_AUDIO_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in &= ~0x1;
  HWIO_OUT (RPMH_PDC_AUDIO_SEQ_PWR_CTRL_OVERRIDE_MASK, out);
#endif

  // Compute
  in = HWIO_IN (RPMH_PDC_COMPUTE_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in &= ~0x1;
  HWIO_OUT (RPMH_PDC_COMPUTE_SEQ_PWR_CTRL_OVERRIDE_MASK, out);
  
  wlan_above_mol_workaround();

#endif
}

// voltage_sequence_complete
//
// Informs the main voltage manager about the voltage sequence completing.
//
// Paramters:
//   None
//
// Return:
//   None.
void voltage_sequence_complete(void)
{
  int i;

  HWIO_OUTFI(SEQn_CFG, VOLTAGE_SEQUENCER, SEQ_START, 0);
  clear_shrm_frequency();
  for (i = 0; i < NUM_VOLTAGE_RESOURCES; i++) 
  {
    if (voltage_arc_status[i].sequencing) 
	{
      if ((VOLTAGE_RESOURCES)i == RESOURCE_CX)
      {
        if ((voltage_arc_status[i].current < voltage_arc_status[i].mol) &&
            (voltage_arc_status[i].in_sequence_level >= voltage_arc_status[i].mol))
        {
          apply_cx_above_mol ();
        }
      }
      #ifndef DDR_SWEVENTS
      ddr_log_msg8 ("VoltDone", i, voltage_arc_status[i].current,
                    voltage_arc_status[i].in_sequence_level, 0);
      #endif
      DDR_MGR_SWEVENT(ddr_voltage_sequence_done, i,
              voltage_arc_status[i].current,
              voltage_arc_status[i].in_sequence_level);

      voltage_arc_status[i].current = voltage_arc_status[i].in_sequence_level;
      voltage_arc_status[i].sequencing = FALSE;

      // We had received the aggrigation interrupt before the sequence was complete.
      // In this case, we deferred processing until we were complete.  Now that
      // we processed the completion, this code calls rail_change_handler again
      // to process the request.
      if (voltage_arc_status[i].pending == ARC_PENDING_DEFERRED_SEQUENCE)
      {
        uint32 ret_val;
        arc_runtime_client_t client_stub;

        memset(&client_stub, 0, sizeof(client_stub));
        client_stub.arc = arc_to_rm[i];
        client_stub.blocked = FALSE;

        ret_val = rail_change_handler(&client_stub, ARC_RUNTIME_EVENT_PREWORK,
                                      voltage_arc_status[i].sequence_level, 0);

        if (client_stub.blocked == FALSE)
        {
          voltage_arc_status[i].in_sequence_level = ret_val;
          arc_unblock_rm_prework(arc_to_rm[i]);
        }
      }
    }
  }

  // At the completion of a voltage sequence, check if the frequency sequence
  // is pending, and if all it's needs are met, trigger that FSM.
  if (freq_fsm_pending_vote && frequency_needs_met())
  {
    freq_fsm_pending_vote = FALSE;
    freq_fsm_signal_voltage_requirements_met();
  }

  // Make sure at the conclusion that if anything is pending, we allow it
  // to go (if appropriate)
  if ((voltage_arc_status[RESOURCE_CX].pending == ARC_PENDING_SEQUENCE) ||
      (voltage_arc_status[RESOURCE_MX].pending == ARC_PENDING_SEQUENCE) ||
      (voltage_arc_status[RESOURCE_VDDA].pending == ARC_PENDING_SEQUENCE))
  {
    if (voltage_dependency_ready())
    {
      seq_fsm_signal_start_voltage();   // Signal the Sequence State Machine to
                                        // perform voltage processing
    }
  }
}

// xo_command_complete
//
// Informs the main voltage manager about the XO ARC completion.
//
// Paramters:
//   None
//
// Return:
//   None.
void xo_command_complete(void)
{
  if (ddr_status.ddr_xo_vote == XO_ON && ddr_status.ddr_xo_on == FALSE)
  {
    ddr_status.ddr_xo_on = TRUE;

    //Recheck the voltage dependecy and start the sequencer
    //Prior attemps would have failed due to XO dependency.
    if (voltage_dependency_ready())
    {
      seq_fsm_signal_start_voltage();   // Signal the Sequence State Machine to
                                        // perform voltage processing
    }
  }
}

/**
 * <!-- check_cx_mx_dependency -->
 *
 * @brief Determine if the CX/MX Arcs are in a state which would allow a sequence to continue
 *
 * @param None
 *
 * @return True if MX will be in an allowable state (typically MX>=CX or MX@TURBO) if the sequence would run
 *         FALSE if we need to see an MX aggregated event for a new vote.
 */
boolean check_cx_mx_dependency(void)
{
  boolean ret_val = TRUE;
  unsigned int cx_level;
  unsigned int mx_level;

  // If MX is at PENDING_SEQUENCE, we need to run a sequence to ensure we are not stuck.
  if (voltage_arc_status[RESOURCE_MX].pending == ARC_PENDING_SEQUENCE)
  {
    return TRUE;
  }

  // If CX is changing, use the sequence_level, if not, use the current level
  if ((voltage_arc_status[RESOURCE_CX].pending == ARC_PENDING_SEQUENCE) ||
      (voltage_arc_status[RESOURCE_CX].sequencing))
  {
    cx_level = voltage_arc_status[RESOURCE_CX].sequence_level;
  }
  else
  {
    cx_level = voltage_arc_status[RESOURCE_CX].current;
  }

  //If cx is going above mol and XO sequence is not complete; return.
  if ((cx_level >= voltage_arc_status[RESOURCE_CX].mol) &&
          (ddr_status.ddr_xo_on == FALSE))
  {
    ddr_log_msg32("Block CX", g_xo_seq_req_id);
    ret_val = FALSE;
  }

  return ret_val;
}

// voltage_dependency_ready
//
// This function checks if the Voltage ARCs are ready for their sequencer to
// be run.
//
//
// Paramters:
//   None.
//
// Return:
//   True if all pending ARCs are at aggregation.
boolean voltage_dependency_ready(void)
{
  int i;
  boolean ret_val = TRUE;

  if (freq_fsm_voting_in_progress)
  {
    return FALSE;               // This is a momentary state, wait for the rest.
  }

  if (!check_cx_mx_dependency())
    {
      ret_val = FALSE;
    }

  for (i = 0; ret_val && (i < NUM_VOLTAGE_RESOURCES); i++)
        {
    if (voltage_arc_status[i].pending == ARC_PENDING_DEFERRED_SEQUENCE)
        {
          ret_val = FALSE;
        }
  }

  return ret_val;
}


// sequencer_fsm_idle_entry
//
// Sequencer Idle entry has no work to do.   Both parameters are unused.
//
// Paramters:
//   fsm: The Sequencer FSM data structure
//   event: The event which caused us to exit the previous state.
//
// Return:
//   none.
void sequencer_fsm_idle_entry(AOP_FSM *fsm, unsigned int event) {
  aop_busy(VOLTAGE_FSM_MASK, FALSE);
  ++sequencer_count;
}


// program_ddr_aux_voltage_sequence
//
// This function looks at the change mask (see sequence_choice below for mapping).
// From there, it determines the sequence to run, and program the voltage sequence start address.
//
// Paramters:
//   change_mask:  The mask indicating which are changing.
//
// Return:
//   none.
void program_ddr_aux_voltage_sequence(unsigned int change_mask)
{
  // Determine which sequence based on which regulator is changing
  enum voltage_sequence_names sequence_choice[] =
  {
    ERROR_SEQUENCE,      // No regulators are changing.
    VOLTAGE_SWITCH,      // CX Only
    VOLTAGE_SWITCH,      // MX Only
    VOLTAGE_SWITCH,      // CX & MX Only
    VDDA_ONLY,           // VDDA only
    VOLTAGE_SWITCH,      // VDDA & CX Only
    VOLTAGE_SWITCH,      // VDDA & MX Only
    VOLTAGE_SWITCH,      // VDDA & CX & MX Only
  };

  if (change_mask >= NUM_ENTRIES(sequence_choice)) {
    CORE_VERIFY(0);
  }

  // Update the branch events for voltage changes.
  HWIO_OUTF(SEQ_BRANCH_EVENT, VOLTAGE, change_mask);

  //Program the start address.  (You must set the address before you set the start bit)
  HWIO_OUTFI(SEQn_CFG, VOLTAGE_SEQUENCER, SEQ_START_ADDR, sequence_choice[change_mask]);

  //Start the sequence now.
  HWIO_OUTFI(SEQn_CFG, VOLTAGE_SEQUENCER, SEQ_START, 0);
  start_debug_timer(WAITING_FOR_SEQ1);
  HWIO_OUTFI(SEQn_CFG, VOLTAGE_SEQUENCER, SEQ_START, 1);
}


// disable_frequency_commands
//
// Make sure all the commands in the TCSs are disabled
// for SHUB/MC frequency commands.  This is used when
// calling COLLAPSE/RESTORE to update GCC, but not SHRM.
//
// Paramters:
//   None
//
// Return:
//   None
//
void disable_frequency_commands()
{
  unsigned int tcs_index;
  for (tcs_index = 0; tcs_index < 3; tcs_index++) {
    HWIO_OUTI(RSI_TCSm_CFG, SHUB_CMD_START + tcs_index, 0);
    HWIO_OUTI(RSI_TCSm_CFG, MC_CMD_START + tcs_index, 0);
  }
}

// program_frequency_cmds
//
// This function programs the PREWORK/WORK/POSTWORK commands in a TCS.
// SHUB commamds are in TCS 6,7 and 8 for PREWORK/WORK/POST.
// MC commamds are in TCS 10,11 and 12 for PREWORK/WORK/POST.
//
// Paramters:
//   tcs_offset.  Where to store the PREWORK (6 for SHUB, 10 for MC).
//   resource: Which resource are we changing (SHUB or MC).
//   sequence_level: What level are we going to.
//
// Return:
//   None.
//
void program_frequency_cmds(unsigned int tcs_offset, unsigned int resource,
                            unsigned int sequence_level)
{
  unsigned int tcs_index;
  unsigned long tcs_cmd;
  unsigned int opcode_val[3] = {RSI_OPCODE_CLOCK_PREWORK,
                                RSI_OPCODE_CLOCK_WORK,
                                RSI_OPCODE_CLOCK_POSTWORK};

  for (tcs_index = 0; tcs_index < 3; tcs_index++) {
    tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, opcode_val[tcs_index],
                             resource, sequence_level);
    // Write the first cmd of each TCS
    HWIO_OUTI2(RSI_TCSm_CMDt, tcs_offset + tcs_index, 0, tcs_cmd);
    // Enable 1 command
    HWIO_OUTI(RSI_TCSm_CFG, tcs_offset + tcs_index, 1);
    // Put the TCS into TCS (Non-AMC) mode.
    HWIO_OUTFI(RSI_TCSm_AMC_MODE, tcs_offset + tcs_index, AMC_ENABLE, 0);
  }
  if (g_alc_override && (resource == RSI_RESOURCE_MC))
  {
    if (ddr_status.ddr_bcm_alc_level != local_mc_states[sequence_level].alc_override)
    {
      ddr_status.ddr_bcm_alc_level = local_mc_states[sequence_level].alc_override;

      tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_FAL_ALC,
                               0, local_mc_states[sequence_level].alc_override);
      // Write the first cmd of each TCS
      HWIO_OUTI2(RSI_TCSm_CMDt, tcs_offset + 2, 1, tcs_cmd);
      // Enable 2 commands
      HWIO_OUTI(RSI_TCSm_CFG, tcs_offset + 2, 0x33);
    }
  }
}

// program_voltage_cmds
//
// This function programs the VOLTAGE commands in the VOLTAGE TCS's.
// TCS 3 stores the PREWORK, TCS 4 stores the CHANGE, TCS 5 stores the Postwork.
//
// Paramters:
//   resource: Which resource we are setting.
//   cmd_index: Which command in the TCS will store the CMD.
//   sequence_level: What level are we going to.
//
// Return:
//   None
//
void program_voltage_cmds(VOLTAGE_RESOURCES resource, unsigned int cmd_index,
                          unsigned int sequence_level)
{
  unsigned int tcs_index = 0;
  unsigned long tcs_cmd;
  unsigned int opcode_val[3] = {RSI_OPCODE_VOLTSLEW_PREWORK,
                                RSI_OPCODE_VOLTSLEW_WORK,
                                RSI_OPCODE_VOLTSLEW_POSTWORK};

  for (tcs_index = 0; tcs_index < 3; tcs_index++) {
    tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_NOTLAST, opcode_val[tcs_index],
                             resource, sequence_level);
    HWIO_OUTI2(RSI_TCSm_CMDt, VOLTAGE_CMD_START + tcs_index, cmd_index, tcs_cmd);
  }
}

// set_voltage_last
//
// This function updates the TCS to ensure the last voltage change is marked as such.
// It also sets the CFG and AMC registers appropriately as well.
//
// Paramters:
//   The index of the last command in the TCS.
//
// Return:
//   None.
//
void set_voltage_last(int cmd_index) {
  unsigned int tcs_index = 0;
  unsigned long cmd_to_bitmask[] = { 1, 0x33, 0x77, 0xFF };
  for (tcs_index = 0; tcs_index < 3; tcs_index++) {
    HWIO_OUTMI2(RSI_TCSm_CMDt, VOLTAGE_CMD_START + tcs_index, cmd_index, 1 << 24, RSI_LAST << 24);

    // Enable cmd_index command(s)
    HWIO_OUTI(RSI_TCSm_CFG, VOLTAGE_CMD_START + tcs_index, cmd_to_bitmask[cmd_index]);

    // Put the TCS into TCS (Non-AMC) mode.
    HWIO_OUTFI(RSI_TCSm_AMC_MODE, VOLTAGE_CMD_START + tcs_index, AMC_ENABLE, 0);
  }
}

// sequencer_fsm_idle_exit
//
// This function is called when we exit the idle state for the sequencer FSM.  We will either
// run a FREQUENCY Sequence or VOLTAGE Sequence when this occurs.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//
// Return:
//   return values.
//
unsigned int sequencer_fsm_idle_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  // Determine which reglators are going to sequence
  aop_busy(VOLTAGE_FSM_MASK, TRUE);
  switch (event)
  {
    // If the next event is a VOLTAGE_CHANGE_READY, run the voltage FSM if anything is pending.
    case FSM_VOLTAGE_CHANGE_READY:
    {
      int i;
      for (i = 0; i < NUM_VOLTAGE_RESOURCES; i++)
      {
        if (voltage_arc_status[i].pending == ARC_PENDING_SEQUENCE)
        {
          return next_state;
        }
      }
      // If no ARCs were pending, we were able to process an ARC that became pending as we entered the
      // sequence state.
      return SEQUENCER_IDLE;
    }
    case FSM_PERFORM_FREQUENCY_CHANGE:
      // If the next event is a PERFORM_FREQUENCY_CHANGE, That FSM already setup the sequencer to go,
      // we just transition to that state and start the sequencer.
      return next_state;

    default:
      CORE_VERIFY(0);
  }
}

// apply_cx_below_mol
//
// This function applies any workarounds/specific handling when CX goes below MOL
// 
// Paramters: None
//
// Return:
//   None
//
static void
apply_cx_below_mol (void)
{
//in sync with Napaliv2
	
  // Turn OFF CX, MX CPRF CTRL_INTERACTION_EN bit
  // when CX is about to go below MOL

    cpr_enable_controller(RPMH_ARC_VDD_CX, FALSE);
    cpr_enable_controller(RPMH_ARC_VDD_MX, FALSE);
  
    // Turn OFF MSS CPRF CTRL_INTERACTION_EN bit
    // when CX is about to go below MOL.
    // MSS CPRF CTRL_INTERACTION_EN needs to be set by MPSS
    // at MSS Sleep Exit. 
    
      cpr_enable_controller_force(RPMH_ARC_VDD_MSS, FALSE);
    g_cpr_disabled = TRUE;

  // PDC timer WA
  // Apps PDC
  uint32 in = HWIO_IN (RPMH_PDC_APPS_SEQ_PWR_CTRL_OVERRIDE_MASK);
  uint32 out = in |= 0x1;
  HWIO_OUT (RPMH_PDC_APPS_SEQ_PWR_CTRL_OVERRIDE_MASK, out);

  // Modem PDC
  in = HWIO_IN (RPMH_PDC_MODEM_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in |= 0x1;
  HWIO_OUT (RPMH_PDC_MODEM_SEQ_PWR_CTRL_OVERRIDE_MASK, out);

#ifndef SDX24_STUBS
  // Audio
#ifndef SDM670_TARGET_CONFIG 
  in = HWIO_IN (RPMH_PDC_AUDIO_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in |= 0x1;
  HWIO_OUT (RPMH_PDC_AUDIO_SEQ_PWR_CTRL_OVERRIDE_MASK, out);
#endif

  // Compute
  in = HWIO_IN (RPMH_PDC_COMPUTE_SEQ_PWR_CTRL_OVERRIDE_MASK);
  out = in |= 0x1;
  HWIO_OUT (RPMH_PDC_COMPUTE_SEQ_PWR_CTRL_OVERRIDE_MASK, out);

  // WLAN workaround,
  wlan_below_mol_workaround();

#endif
}

/**
 * <!-- cx_allowed_to_sequence -->
 *
 * @brief   This function looks at the state of the system and determines if we
 *          can run the CX sequence now.   
 *
 * @param   None
 *
 * @return  True if CX can be sequenced now.  False if there is a dependency.
 */
bool cx_allowed_to_sequence(void)
{
#ifdef SDM855_TARGET_CONFIG
  uint32_t mx_max_vlvl = TURBO_L1;
#else
  uint32_t mx_max_vlvl = TURBO;
#endif

  // Do not run CX sequence if XO is not on.   CPR workaround.
  if ((voltage_arc_status[RESOURCE_CX].sequence_level > voltage_arc_status[RESOURCE_CX].mol) &&
      (ddr_status.ddr_xo_on == FALSE))
  {
    ddr_log_msg32("Block CX", g_xo_seq_req_id);
    return FALSE;
  }

  if (voltage_arc_status[RESOURCE_CX].current < voltage_arc_status[RESOURCE_CX].mol)
  {
    int mx_level;

    // Do not run CX sequence if MX is not on.   CPR workaround.
    if (min_current_arc_status (RESOURCE_MX) < voltage_arc_status[RESOURCE_MX].mol)
    {
      ddr_log_msg32("MXRet CX", g_volt_seq_req_id);
      return FALSE;
    }

    // Do not run CX sequence if MX is sequencing and CX is below MOL.  CPR Workaround
    if ((voltage_arc_status[RESOURCE_MX].pending == ARC_PENDING_SEQUENCE) ||
        (voltage_arc_status[RESOURCE_MX].sequencing == TRUE))
    {
      ddr_log_msg32("MXSeqReq", g_xo_seq_req_id);
      return FALSE;
    }

    // Do not run CX sequence if restoring CX and MX is not at Turbo(_L1)
    mx_level = get_arc_current_level(RESOURCE_MX);
    if ((get_arc_current_level(RESOURCE_CX) == 0) &&
        (mx_level != vlvl_to_hlvl(RESOURCE_MX, mx_max_vlvl)))
    {
      ddr_log_msg32("MXDepCXC", mx_level);
      return FALSE;
    }
  }

  return TRUE;
}
// sequencer_fsm_voltage_entry
//
// This function looks at any voltage ARC waiting and starts the sequence
// for that.  It updates the TCS's as necessary and chooses the correct
// voltage sequence to run.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   None
//
void sequencer_fsm_voltage_entry(AOP_FSM *fsm, unsigned int event)
{
  unsigned int change_mask = 0;
  unsigned int tcs_index = 0;

  // Determine which reglators are going to sequence
  int i;

  if (voltage_dependency_ready())
  {
  for (i = 0; i < NUM_VOLTAGE_RESOURCES; i++)
  {
    // If the arc is not pending a change, then don't program the TCS.
      if (voltage_arc_status[i].pending == ARC_PENDING_SEQUENCE)
    {
        // Make sure CX is allowed to move based on the MX state.
        if ((i == RESOURCE_CX) && (voltage_arc_status[i].sequence_level >= voltage_arc_status[i].mol))
        {
          if (!cx_allowed_to_sequence())
          {
            continue;
          }
        }

      voltage_arc_status[i].sequencing = TRUE;
      voltage_arc_status[i].in_sequence_level = voltage_arc_status[i].sequence_level;

        // Do the below MOL CX Workarounds
        if (((VOLTAGE_RESOURCES) i == RESOURCE_CX) &&
            (voltage_arc_status[i].current >= voltage_arc_status[i].mol) &&
            (voltage_arc_status[i].sequence_level < voltage_arc_status[i].mol))
        {
            apply_cx_below_mol ();
        }

        // allow ARC to continue
        arc_unblock_rm_prework(arc_to_rm[i]);

        // if cx is changing, we tell SHRM
      if (i == RESOURCE_CX) 
	  {
        update_shrm_frequency();
      }

      voltage_arc_status[i].pending = ARC_PENDING_NONE;
      change_mask |= (1 << i);
      program_voltage_cmds((VOLTAGE_RESOURCES) i, tcs_index,
                           voltage_arc_status[i].in_sequence_level);
      tcs_index++;
    }
  }
  }

  if (tcs_index > 0)
  {
    set_voltage_last(--tcs_index);

    program_ddr_aux_voltage_sequence(change_mask);
    #ifndef DDR_SWEVENTS
    ddr_log_msg8 ("Run VSeq", tcs_index, change_mask, 0, 0);
    #endif
    //DDR_MGR_SWEVENT(ddr_Running_Voltage_Seq, tcs_index, change_mask);
  }
  else
  {
    #ifndef DDR_SWEVENTS
    ddr_log_msg8 ("NO VSEQ!",
                  voltage_arc_status[0].pending,
                  voltage_arc_status[1].pending,
                  voltage_arc_status[2].pending, 0);
    #endif
    //DDR_MGR_SWEVENT(ddr_NOT_Running_Voltage_Seq,
    //        voltage_arc_status[0].pending,
    //        voltage_arc_status[1].pending,
    //        voltage_arc_status[2].pending);
    seq_fsm_signal_voltage_sequence_complete();
  }
}

// sequencer_fsm_voltage_exit
//
// This function is called when the voltage sequence is done.   It calls
// voltage_sequence_complete to make sure anyone who cares is informed and
// then returns the next state (IDLE).
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//   next_state: The default next_state.
//
// Return:
//   What state to go to: IDLE
//
unsigned int sequencer_fsm_voltage_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  voltage_sequence_complete();
  return next_state;
}

// sequencer_fsm_freq_entry
//
// This function starts running the frequency sequencer (Sequencer 0).
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   None
//
void sequencer_fsm_freq_entry(AOP_FSM *fsm, unsigned int event) {
#ifdef ALLOW_SHRM
  start_debug_timer(WAITING_FOR_SEQ0);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 1);
#else
  ddr_aux_frequency_sequence_complete();
#endif
}

// sequencer_fsm_freq_exit
//
// This function is called when the frequency sequence completes.  It just needs to
// inform the frequency FSM that the sequence it wanted is complete, and then goes back
// to idle.
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//   next_state: The default next_state.
//
// Return:
//   The default next state - Idle.
unsigned int sequencer_fsm_freq_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  freq_fsm_signal_frequency_sequence_complete();
  return next_state;
}

// setup_sequencer_fsm
//
// This function initializes the sequencer FSM.  This FSM ensures that only a voltage or frequency
// sequence is running at once.  Also, when we get a VOLTAGE sequence, it runs the sequencer for each
// Voltage ARC waiting on DDR.
//
// Paramters:
//   None
//
// Return:
//   None
//
void setup_sequencer_fsm(void)
{
  aop_fsm_init(&sequencer_fsm, NUM_SEQUENCER_FSM_STATES, "Sequencer FSM");
  aop_fsm_init_state(&sequencer_fsm, SEQUENCER_IDLE,
                 &sequencer_fsm_idle_entry,
                 &sequencer_fsm_idle_exit, "Idle");
  aop_fsm_init_state(&sequencer_fsm, CHANGE_VOLTAGE,
                 &sequencer_fsm_voltage_entry,
                 &sequencer_fsm_voltage_exit, "Change Voltage");
  aop_fsm_init_state(&sequencer_fsm, CHANGE_FREQUENCY,
                 &sequencer_fsm_freq_entry,
                 &sequencer_fsm_freq_exit, "Change Frequency");
  // voltage change ready can be performed at idle, but can be received
  // always.
  aop_fsm_add_event(&sequencer_fsm, SEQUENCER_IDLE, FSM_VOLTAGE_CHANGE_READY, FALSE,
                CHANGE_VOLTAGE);  // change state to CHANGE_VOLTAGE
  aop_fsm_add_event(&sequencer_fsm, CHANGE_VOLTAGE, FSM_VOLTAGE_CHANGE_READY, TRUE,
                CHANGE_VOLTAGE);  // remain at state CHANGE_VOLTAGE
  aop_fsm_add_event(&sequencer_fsm, CHANGE_FREQUENCY, FSM_VOLTAGE_CHANGE_READY, TRUE,
                CHANGE_FREQUENCY);  // remain at state CHANGE_FREQUENCY

  // frequency change ready can be performed at idle, but can be received
  // when waiting for voltage or immediately if we asked to just acked a
  // Frequency change.
  aop_fsm_add_event(&sequencer_fsm, SEQUENCER_IDLE, FSM_PERFORM_FREQUENCY_CHANGE,
                FALSE, CHANGE_FREQUENCY);  // change state to CHANGE_FREQUENCY
  aop_fsm_add_event(&sequencer_fsm, CHANGE_VOLTAGE, FSM_PERFORM_FREQUENCY_CHANGE,
                TRUE, CHANGE_VOLTAGE);  // remain at state CHANGE_VOLTAGE
  aop_fsm_add_event(&sequencer_fsm, CHANGE_FREQUENCY, FSM_PERFORM_FREQUENCY_CHANGE,
                TRUE, CHANGE_FREQUENCY);  // remain at state CHANGE_FREQUENCY

  // voltage sequence_complete is only expected when running a voltage
  // sequence
  aop_fsm_add_event(&sequencer_fsm, CHANGE_VOLTAGE, FSM_VOLTAGE_SEQUENCE_COMPLETE,
                FALSE, SEQUENCER_IDLE);

  // frequency sequence_complete is only expected when running a frequency
  // sequence
  aop_fsm_add_event(&sequencer_fsm, CHANGE_FREQUENCY, FSM_FREQUENCY_SEQUENCE_COMPLETE,
                    FALSE, SEQUENCER_IDLE);
}

// Structure of what votes we send each time, and where to get the current vote.
struct vote_info {
  VOLTAGE_RESOURCES arc;
  unsigned int *vote_ptr;
  uint32 last_vote;
} vote_list[NUM_VOLTAGE_RESOURCES] = {
  { RESOURCE_MX,   &ddr_status.ddr_mx_vote, 0 },
  { RESOURCE_CX,   &ddr_status.ddr_cx_vote, 0 },
  { RESOURCE_VDDA, &ddr_status.ddr_vdda_vote, 0 }
};

/**
 * <!-- send_voltage_cmds -->
 *
 * @brief   This function sends the votes for CX/MX/VDDA
 *
 * @param   resp:  Send the votes completion required if TRUE, False otherwise.
 *
 * @return  None
 */
void send_voltage_cmds(bool resp) 
{
  uint32 num_commands = 0;
  uint32_t log_value = 0;
  int resp_count = 0;
  int vote_order[NUM_VOLTAGE_RESOURCES] = { 1, 0, 2 };

  // For each of the ARCs, send a vote if needed.
  for (int i = 0; i < NUM_VOLTAGE_RESOURCES; i++)
  {
    int arc = vote_order[i];
    // For logging, determine the last and current OL's being voted.
    uint32 last_vote_hlvl = vlvl_to_hlvl(vote_list[arc].arc, vote_list[arc].last_vote);
    uint32 vote_hlvl      = vlvl_to_hlvl(vote_list[arc].arc, *(vote_list[arc].vote_ptr));

    log_value |= (((last_vote_hlvl << 4) | vote_hlvl) << (8*arc));

    // Is our vote different?
    if (*(vote_list[arc].vote_ptr) != vote_list[arc].last_vote)
    {
      uint32 request_id;

      // Send the vote.
      request_id = arc_send_vote (vote_list[arc].arc, CLIENT_DDR_FREQUENCY,
                                  *(vote_list[arc].vote_ptr),
                                  resp?CompletionRequired:FireForget);
      vote_list[arc].last_vote = *(vote_list[arc].vote_ptr);

      // FOr logging - save what happened.
      num_commands++;
      if (request_id != 0)
      {
        resp_count++;
        g_volt_seq_req_id = request_id;
      }
      ddr_log_msg32("DDR Vote", arc << 28 | (request_id & 0x0fffffff));
    }
  }

  if (num_commands)
  {
    log_value |= ((num_commands << 28) | (resp_count << 24));
    ddr_log_msg32("VotesOut", log_value);
  }
}

/**
 * <!-- send_voltage_cmds -->
 *
 * @brief   Send the voltage votes as required by DDR.   Really this just saves it in the
 *          data structure and makes send_voltage_cmds do the work.  The Opscale code goes
 *          here to update ICB of the new CX vote
 *
 * @param   cx_vote:  What value to send for CX
 * @param   mx_vote:  What value to send for MX
 * @param   vdda_vote:  What value to send for VDDA
 * @param   resp:  Send the votes completion required if TRUE, False otherwise.
 *
 * @return  True if CX can be sequenced now.  False if there is a dependency.
 */
void frequency_fsm_send_voltages(uint32_t cx_vote, uint32_t mx_vote, uint32_t vdda_vote, bool resp) 
{
  freq_mgmt_votes_inprogress(TRUE);

  ddr_status.ddr_vdda_vote = vdda_vote;
  ddr_status.ddr_cx_vote = cx_vote;
  ddr_status.ddr_mx_vote = mx_vote;

  send_voltage_cmds(resp) ;

  freq_mgmt_votes_inprogress(FALSE);

  if (g_opscale_bcm)
  {
    icb_update_ddr_cx_state (cx_vote);   // Let BCM know.
  }
}

// frequency_fsm_idle_entry
//
// This function is called whenever we are done processing any change and determined
// there is nothing left to do.   When we enter this state, we do ensure our voltage
// votes are for the current state.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   return values.
//
void frequency_fsm_idle_entry(AOP_FSM *fsm, unsigned int event)
{
  unsigned int vdda_vote = ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vdda;
  unsigned int pmic_mode = ddr_cfg->pMCCfg[ddr_status.current_mc_cp].ddr_automode_cfg;
  unsigned int cx_vote = MAX(ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vddcx,
                             ddr_cfg->pSHUBCfg[ddr_status.current_shub_cp].vddcx);
  unsigned int mx_vote = MAX(ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vddmx,
                             ddr_cfg->pSHUBCfg[ddr_status.current_shub_cp].vddmx);

  ddr_log_freq(FREQ_DONE);

  frequency_fsm_send_voltages(cx_vote, mx_vote, vdda_vote, FALSE);
  if (pmic_mode != ddr_status.ddr_pmic_mode_vote) {
    ddr_status.ddr_pmic_mode_vote = pmic_mode;
    (void) arc_send_vote (RESOURCE_PMIC, CLIENT_DDR_FREQUENCY, pmic_mode, CompletionRequired);
  }
  aop_busy(FREQUENCY_FSM_MASK, FALSE);
}


// find_enabled_plan
//
// This function looks at the frequency plan and gets the best enabled plan.
//
// Paramters:
//     states: Pointer to the states structure.
//     state_count: the maximum number of entries in that table
//     desired_plan: What clock plan is being requested.
//
// Return:
//   The closest plan that would satisfy the desired_plan
unsigned int find_enabled_plan(LOCAL_FREQ_STATE *states, unsigned int state_count,
                               unsigned int desired_plan)
{
  unsigned int i = 1;
  unsigned int ret_val = 0;

  if (desired_plan == 0)
  {
    return 0; // 0 cannot be disabled.
  }

  // Find the lowest enabled plan that is greater than our needs
  // or return the highest enabled plan.
  for (; i < state_count; i++)
  {
    if (states[i].freq_enable)
    {
      ret_val = i;
      // If its enabled and satisfies our needs, done.
      if (i >= desired_plan)
      {
        break;
      }
    }
  }

  return ret_val;
}

// min_mc_lvl_for_ddr_rm_arc
//
// This function returns the minimum MC level to satisfy the DDR_SS Powerstate ARC.
// It needs to know what BCM wants, and allows for the Powerstate ARC to also choose higher
// than typical entries.
//
// Paramters:
//     None
//
// Return:
//   The MC clock plan that satisfies the DDR_SS Powerstate ARC
unsigned int min_mc_lvl_for_ddr_rm_arc(void)
{
  // Start the ret_val at 0
  unsigned int ret_val = 0;
  unsigned int i = 0;

  if (ddr_status.ddr_arc_level > D0_On) // Test mode, ARC wants > On, set the level at # past D0_On
  {
    ret_val = ddr_status.ddr_arc_level - D0_On;
  }
  else if (ddr_status.ddr_arc_level >= D0p5_PLL_Off) // Typical, operational or PLL Off
  {
    if (ddr_status.ddr_bcm_mc_level == 0)
    {
      ret_val = mc_collapse_cp;
    }
    else
    {
      ret_val = 1;
    }
  }
  // else ret_val = 0  // ARC wants off, and BCM wants off, return off.
  ret_val = find_enabled_plan(ddr_cfg->pMCCfg, ddr_cfg->nNumMCStates, ret_val);

  return ret_val;
}

// get_valid_freq
//
// This function returns the minimum MC level to satisfy the DDR_SS Powerstate ARC.
// It needs to know what BCM wants, and allows for the Powerstate ARC to also choose higher
// than typical entries.
//
// Paramters:
//     None
//
// Return:
//   The MC clock plan that satisfies the DDR_SS Powerstate ARC
void get_valid_freq(unsigned int *mc_level, unsigned int *shub_level)
{
  int i;
  unsigned int bcm_mc_val =   ddr_status.ddr_bcm_mc_level;
  unsigned int bcm_shub_val = ddr_status.ddr_bcm_shub_level;
  unsigned int ret_mc_val = 0;
  unsigned int ret_shub_val = 0;

  // First ensure MC is on if SHUB is on.
  if ((bcm_shub_val > 0) && (bcm_mc_val == 0))
  {
    bcm_mc_val = mc_collapse_cp;
  }

  // Now find the maximum of either bcm's value or ARCs value.
  ret_mc_val = MAX(bcm_mc_val, min_mc_lvl_for_ddr_rm_arc());

  if ((ret_mc_val > 0) && (ddr_status.ddr_mc_floor > 0))
  {
    bcm_mc_val = MAX(ret_mc_val, ddr_status.ddr_mc_floor);
  }

  ret_mc_val = find_enabled_plan(ddr_cfg->pMCCfg, ddr_cfg->nNumMCStates, ret_mc_val);

  if (ret_mc_val > 0)
  {
    unsigned int tmp_shub_val = shub_collapse_cp;
    unsigned int max_shub_period = ddr_cfg->pMCCfg[ret_mc_val].clk_period * 4;

    if (bcm_shub_val == 0) {
      ret_shub_val = shub_collapse_cp; // Start SHUB @ restore frequency if BCM doesn't care.
      }
    else {
      ret_shub_val = bcm_shub_val;
    }

    for (i = 0; i < ddr_cfg->nNumSHUBStates; i++)
    {
      if (ddr_cfg->pSHUBCfg[i].freq_enable) {
        tmp_shub_val = i;
        // If it is enabled and satisfies the MC/SHUB dependency and is high enough,
        // we are done.
        if ((i >= ret_shub_val) && (ddr_cfg->pSHUBCfg[i].clk_period <= max_shub_period))
        {
          break;
        }
      }
    }

    // we've found an enabled SHUB frequency that satisfies constraints.
    // now go as high as we can in the same CX corner
    for (; g_opscale_shub && (i < ddr_cfg->nNumSHUBStates); i++)
  {
      if (ddr_cfg->pSHUBCfg[i].vddcx > ddr_cfg->pMCCfg[ret_mc_val].vddcx) break;

      if (ddr_cfg->pSHUBCfg[i].freq_enable) tmp_shub_val = i;
    }

    ret_shub_val = tmp_shub_val;
  }

  *mc_level = ret_mc_val;
  *shub_level = ret_shub_val;
}

// frequency_fsm_idle_exit
//
// Called when we exit the idle state.   Nothing to really do here, just enter the
// process_entry state.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   The next state.
//
unsigned int frequency_fsm_idle_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  aop_busy(FREQUENCY_FSM_MASK, TRUE);
  return next_state;
}

// frequency_fsm_process_entry
//
// This function is called when we want to determine what to do next.   This is called
// whenever we leave idle as well as whenever we return from a frequency change or cache
// flush.  Basically, anytime we want to see what to do, we enter this state.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   return values.
//
void frequency_fsm_process_entry(AOP_FSM *fsm, unsigned int event)
{
  // unsigned int new_state;
  frequency_count++;

  // Tell the FSM that FREQ_FSM_FREQUENCY_CHANGE_REQUEST are handled.
  aop_fsm_clear_pending(fsm, FREQ_FSM_FREQUENCY_CHANGE_REQUEST);

  // Check BCM MC data
  if (ddr_status.bcm_mc_pending == TRUE)
  {
    ddr_status.ddr_bcm_mc_level = ddr_status.ddr_reg_mc_level;
    if (ddr_status.current_mc_cp >= ddr_status.ddr_bcm_mc_level)
    {
      #ifndef DDR_SWEVENTS
      ddr_log_msg8("Unblk MC", ddr_status.current_mc_cp, ddr_status.ddr_bcm_mc_level, ddr_status.ddr_reg_mc_level, 0);
      #endif
      DDR_MGR_SWEVENT(ddr_UNBLOCK_BCM_MC,ddr_status.current_mc_cp, ddr_status.ddr_bcm_mc_level, ddr_status.ddr_reg_mc_level );
      ddr_status.bcm_mc_pending = FALSE;
      signal_bcm_mc_sequence_complete();
    }
  }

  // Check BCM SHUB data
  if (ddr_status.bcm_shub_pending == TRUE)
  {
    ddr_status.ddr_bcm_shub_level = ddr_status.ddr_reg_shub_level;
    if (ddr_status.current_shub_cp >= ddr_status.ddr_bcm_shub_level)
    {
      #ifndef DDR_SWEVENTS
      ddr_log_msg8("UnblkSHB", ddr_status.current_shub_cp, ddr_status.ddr_bcm_shub_level, ddr_status.ddr_reg_shub_level, 0);
      #endif
      DDR_MGR_SWEVENT(ddr_UNBLOCK_BCM_SHUB,ddr_status.current_shub_cp, ddr_status.ddr_bcm_shub_level, ddr_status.ddr_reg_shub_level);
      ddr_status.bcm_shub_pending = FALSE;
      signal_bcm_shub_sequence_complete();
    }
  }

  // If BCM wants us ON, Prepare for that.
  if (MAX(ddr_status.ddr_bcm_mc_level, ddr_status.ddr_bcm_shub_level) > 0)
  {
    // Bandwidth may make cache's dirty.
    ddr_status.ddr_flush_complete = FALSE;

    // If BCM wants us ON, make sure the DDR RM is voted as On.
    if (ddr_status.ddr_rm_vote != g_ddrss_on_vote)
    {
      ddr_rm_vote(g_ddrss_on_vote);
    }
  }
  else
  {
    // If BCM wants us OFF, make sure the DDR RM is voted as OFF.
    if ((ddr_status.ddr_rm_vote !=
         Dx_PLL_Off_MC_Collapse_PHY_Collapse_SHUB_Collapse_LLC_Collapse))
    {
      ddr_rm_vote(Dx_PLL_Off_MC_Collapse_PHY_Collapse_SHUB_Collapse_LLC_Collapse);
    }
  }


  // Allow the ARC to transition state if needed.
  if (ddr_status.ddr_rm_pending == TRUE)
  {
    //ddr_status.ddr_arc_level is the desired OL from the ARC.
    //If ddr_arc_level is == D0_On, then the ARC just wants MC on.
    //If ddr_arc_level > D0_On, the ARC wants us at least at CP ddr_arc_level-DO_On
    unsigned int ddr_rm_ack_lvl = min_mc_lvl_for_ddr_rm_arc();

    // Check the DDRSS_Sequence
    if (ddr_status.current_mc_cp >= ddr_rm_ack_lvl)
    {
      #ifndef DDR_SWEVENTS
      ddr_log_msg8("Unblk_PS", ddr_status.current_mc_cp, ddr_rm_ack_lvl, ddr_status.ddr_arc_level, 0);
      #endif
      DDR_MGR_SWEVENT(ddr_UNBLOCK_POWERSTATE_ARC,ddr_status.current_mc_cp, ddr_rm_ack_lvl, ddr_status.ddr_arc_level );
      ddr_status.ddr_rm_pending = FALSE;
      arc_unblock_rm_prework(DDRSS_POWERSTATE_RM);
    }
  }

  get_valid_freq(&ddr_status.ddr_dest_mc_lvl, &ddr_status.ddr_dest_shub_lvl) ;

  // Lookup in table next state from current state.
  // Get next step
  freq_fsm_signal_process_complete();
}

// next_cp
//
// This function returns the next cp for the frequency plan (bus_states).  It looks at the
// current_cp and final_cp, and determines what would be the first step in that journey.
// This function is likely called many times as we transition.
//
// Paramters:
//   bus_states:  Either the MC or SHUB bus states structure.
//   current_cp:  The "from" cp to look at the transition.
//   final_cp:    The "final" destination for the clock plan we want.
//
// Return:
//   The clock plan which is next in the process.  It may be the final one, or an intermediate one.
unsigned int next_cp(LOCAL_FREQ_STATE *bus_states, unsigned int current_cp, unsigned int final_cp, unsigned int default_cp)
{
  unsigned int next_cp;

  if (final_cp < current_cp) {
    next_cp = MAX(final_cp, bus_states[current_cp].min_down_idx);
    if ((final_cp == 0) && (next_cp < default_cp) && (current_cp != default_cp))
    {
      next_cp = MIN(default_cp, bus_states[current_cp].max_up_idx);
    }
  } else {
    if (current_cp == 0 && final_cp > 0) {
      next_cp = default_cp;
    } else {
      next_cp = MIN(final_cp, bus_states[current_cp].max_up_idx);
    }
  }

  return next_cp;
}

// frequency_fsm_process_exit
//
// This function is called when we are done processing what to do next.  Generally speaking,
// the PROCESS state is a transient state and requires no INTERRUPT to move to the next state.
// It is created as a convenience to ensure we check what to do next after every action.
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//   next_state: The default next_state.
//
// Return:
//   return values.
//
unsigned int frequency_fsm_process_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state)
{
  uint64_t now = time_service_now();
  // If there is no frequency change, go back to idle.
  if ((ddr_status.ddr_dest_mc_lvl == ddr_status.current_mc_cp) &&
      (ddr_status.ddr_dest_shub_lvl == ddr_status.current_shub_cp) &&
      (ddr_status.ddr_mc_lvl_double == FALSE))
  {
#ifndef MIN_SIZE
    if (ddr_status.memtest_needed && g_do_memtest) {
      next_state = FREQUENCY_WAIT_FOR_DCC;
    } else {
      next_state = FREQUENCY_IDLE;
    }
#else
    next_state = FREQUENCY_IDLE;
#endif
    return next_state;
  }

  // If we are heading to off, and the ddr_arc votes LLC Collapse, start
  // flush.
  if ((ddr_status.ddr_dest_mc_lvl == 0) &&
      (ddr_status.ddr_dest_shub_lvl == 0) &&
      (ddr_status.current_mc_cp >= mc_collapse_cp))
  {
    if ((ddr_status.ddr_arc_level ==
         Dx_PLL_Off_MC_Collapse_PHY_Collapse_SHUB_Collapse_LLC_Collapse) &&
        (ddr_status.ddr_flush_complete == FALSE))
    {
      // We can do a flush, move to that state.
      next_state = FREQUENCY_READING_FLUSH_DATA;
    }
    // No flush required, move towards Collapse.
  }

  if (!ddr_status.ddr_mc_lvl_double) {
    ddr_status.ddr_next_mc_lvl = next_cp(ddr_cfg->pMCCfg,
                                         ddr_status.current_mc_cp,
                                         ddr_status.ddr_dest_mc_lvl,
                                         mc_collapse_cp);
    ddr_status.ddr_mc_lvl_double = ddr_cfg->pMCCfg[ddr_status.ddr_next_mc_lvl].double_freq_change;
  }

  // If MC is going up, bring shub up to the max of MC next and SHUB last.
  // The expectation is that SHUB can always go higher, so there isn't a case
  // where the SHUB cannot go to the next MC level (or higher).
  // Also, if MC is going down, only move SHUB once MC is done going lower.
  if ((ddr_status.ddr_next_mc_lvl >= ddr_status.current_mc_cp ) ||
      (ddr_status.ddr_next_mc_lvl == 0 ))
  {
    ddr_status.ddr_next_shub_lvl = next_cp(ddr_cfg->pSHUBCfg,
                                           ddr_status.current_shub_cp,
                                           ddr_status.ddr_dest_shub_lvl,
                                           shub_collapse_cp);
  }

  // Finally, if MC is going to collapse, make sure SHUB is at the collapse
  // frequency at this point too.  Similarly, if SHUB is going to collapse,
  // make sure MC is going to also.
  if ((ddr_status.ddr_next_mc_lvl == 0) && (ddr_status.ddr_next_shub_lvl != 0))
  {
    ddr_status.ddr_next_mc_lvl = ddr_status.current_mc_cp;
  }
  if ((ddr_status.ddr_next_shub_lvl == 0) && (ddr_status.ddr_next_mc_lvl != 0))
  {
    ddr_status.ddr_next_shub_lvl = ddr_status.current_shub_cp;
  }

  if (ddr_status.ddr_next_mc_lvl != ddr_status.current_mc_cp)
  {
    ddr_stats->mc_cp_history = (ddr_stats->mc_cp_history << 4) | (ddr_status.ddr_next_mc_lvl & 0x0F);
    if (ddr_status.current_mc_cp == 0)
    {
      // Restore time
      ddr_stats->powerstate_restore_begin = now;
    } else if (ddr_status.ddr_next_mc_lvl == 0)
    {
      // Collapse time
      ddr_stats->powerstate_collapse_begin = now;
    } else
    {
      // Frequency change time
      ddr_stats->previous_mc_begin = ddr_stats->mc_begin;
      ddr_stats->mc_begin = now;
    }
  }
  if (ddr_status.ddr_next_shub_lvl != ddr_status.current_shub_cp)
  {
    ddr_stats->shub_cp_history = (ddr_stats->shub_cp_history << 4) | (ddr_status.ddr_next_shub_lvl & 0x0F);
    ddr_stats->previous_shub_begin = ddr_stats->shub_begin;
    ddr_stats->shub_begin = now;
  }
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("Procssed",
                ddr_status.ddr_next_mc_lvl,
                ddr_status.ddr_next_shub_lvl,
                0,
                next_state);
  #endif
  DDR_MGR_SWEVENT(ddr_FREQ_FSM_PROCESS_DONE, ddr_status.ddr_next_mc_lvl,
          ddr_status.ddr_next_shub_lvl, next_state);

  return next_state;
}

// get_needs
//
// This function will get the AOP Frequency needs for the change
// which is in progress.   CX's needs are the maximum of the current
// state to the destination state.  We will look through any intermediate
// frequencies as well to ensure the maximum voltage is met.    For VDDA
// the voltage is the higher of the current or next state's needs.
//
// Paramters:
//   cx_vote: Pointer to where to store the CX needs.
//   vdda_vote: Pointer to where to store the VDDA needs.
//
// Return:
//   return values.
//
void get_needs(unsigned int *cx_vote, unsigned int *mx_vote, unsigned int *vdda_vote, unsigned int *pmic_mode)
{
  unsigned int temp_cp;
  unsigned int steps = 0;

  temp_cp = ddr_status.current_mc_cp;
  *cx_vote =   ddr_status.ddr_cx_vote;
  *mx_vote =   ddr_status.ddr_mx_vote;
  *vdda_vote = ddr_cfg->pMCCfg[temp_cp].vdda;
  *pmic_mode = ddr_cfg->pMCCfg[temp_cp].ddr_automode_cfg;

  // Start by walking the MC changes to get CX and VDDA
  while (temp_cp != ddr_status.ddr_dest_mc_lvl) {
    steps++;
    temp_cp = next_cp(ddr_cfg->pMCCfg, temp_cp, ddr_status.ddr_dest_mc_lvl, mc_collapse_cp);
    *cx_vote = MAX(*cx_vote, ddr_cfg->pMCCfg[temp_cp].vddcx);
    *mx_vote = MAX(*mx_vote, ddr_cfg->pMCCfg[temp_cp].vddmx);

    // Limit CX to Turbo if we currently have a 0 vote.
    if ((ddr_status.ddr_cx_vote == 0) && (*cx_vote > TURBO)) {
      *cx_vote = TURBO;
    }
    // If its the first step, and we are planning on changing MC.
    if ((ddr_status.ddr_next_mc_lvl != ddr_status.current_mc_cp) && (steps == 1))
    {
      // VDDA and Mode are only updated on the first step.
      *vdda_vote = MAX(*vdda_vote, ddr_cfg->pMCCfg[temp_cp].vdda);
      *pmic_mode = MAX(*pmic_mode, ddr_cfg->pMCCfg[temp_cp].ddr_automode_cfg);
    }
  }

  // Now walking the SHUB changes to get CX
  temp_cp = ddr_status.current_shub_cp;
  steps=0;
  while (temp_cp != ddr_status.ddr_dest_shub_lvl) {
    steps++;
    temp_cp = next_cp(ddr_cfg->pSHUBCfg, temp_cp, ddr_status.ddr_dest_shub_lvl, shub_collapse_cp );
    *cx_vote = MAX(*cx_vote, ddr_cfg->pSHUBCfg[temp_cp].vddcx);
    *mx_vote = MAX(*mx_vote, ddr_cfg->pSHUBCfg[temp_cp].vddmx);
  }
}

// frequency_fsm_voltage_entry
//
// This function is called as we enter the voltage state.  It is responsible
// for ensuring the CX/MX/XO/and DCC bandwidth votes are all made.   If we know
// our needs are already met, this function triggers the FSM even to move on, otherwise
// we wait for other parts of the FSM to trigger that event.
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//
// Return:
//   None
//
uint64_t cycles_to_bcd(uint64_t elapsed);
void frequency_fsm_voltage_entry(AOP_FSM *fsm, unsigned int event) {
  unsigned int cx_vote;
  unsigned int mx_vote;
  unsigned int vdda_vote;
  unsigned int pmic_mode;

  int needs = 0;

  ddr_log_freq(FREQ_CHANGE);

  get_needs(&cx_vote, &mx_vote, &vdda_vote, &pmic_mode);

  frequency_fsm_send_voltages(cx_vote, mx_vote, vdda_vote, TRUE);

  if (pmic_mode != ddr_status.ddr_pmic_mode_vote) {
    ddr_status.ddr_pmic_mode_vote = pmic_mode;
    (void) arc_send_vote (RESOURCE_PMIC, CLIENT_DDR_FREQUENCY, pmic_mode, CompletionRequired);
  }

  if ((freq_fsm_pending_vote == FALSE) && frequency_needs_met())
  {
    freq_fsm_pending_vote = FALSE;
    freq_fsm_signal_voltage_requirements_met();
  }
  else
  {
    freq_fsm_pending_vote = TRUE;
  }
}

// frequency_fsm_voltage_exit
//
// This function is used once we know we have the proper voltage for the state change
// we want to do.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//   next_state: The default next_state.
//
// Return:
//   The next_state to go for.   Basically, always now run the Frequency change sequence
unsigned int frequency_fsm_voltage_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  return next_state;
}

#ifndef MIN_SIZE
// frequency_fsm_memtest_entry
//
// When we enter this state, we are to perform the DCC DDR based memory test.
//
// Paramters:
//   fsm:  The frequency fsm data structure.
//   event:  The event received.
//
// Return:
//   return values.
//
void frequency_fsm_memtest_entry(AOP_FSM *fsm, unsigned int event)
{
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("StrtMemT",
                ddr_status.ddr_next_mc_lvl,
                ddr_status.ddr_next_shub_lvl,
                START_ADDR_DCC_MEMTEST,
                g_cmd_db_mem_test);
  #endif

  ddr_status.memtest_needed = FALSE;

  if (g_cmd_db_mem_test)
  {          // Query command db after a change.
    #ifndef DDR_SWEVENTS
    ddr_log_msg8("CmdDBTst", ddr_status.current_mc_cp, ddr_status.current_shub_cp, 0, 0);
    #endif
    DDR_MGR_SWEVENT(ddr_CMD_DB_MEM_TEST,ddr_status.current_mc_cp, ddr_status.current_shub_cp );
    if ((ddr_arc_address != cmd_db_query_addr("ddr.lvl")) ||
        (xo_arc_address  != cmd_db_query_addr("xo.lvl")))
    {
    #ifndef DDR_SWEVENTS
      ddr_log_msg8("DBTstBAD", ddr_status.current_mc_cp, ddr_status.current_shub_cp, 0, 0);
    #endif
    DDR_MGR_SWEVENT(ddr_CMD_DB_MEM_TEST_FAIL, ddr_status.current_mc_cp, ddr_status.current_shub_cp);
    } else {
      #ifndef DDR_SWEVENTS
      ddr_log_msg8("DBTst OK", ddr_status.current_mc_cp, ddr_status.current_shub_cp, 0, 0);
      #endif
      DDR_MGR_SWEVENT(ddr_CMD_DB_MEM_TEST_PASS, ddr_status.current_mc_cp, ddr_status.current_shub_cp);
    }
    ddr_aux_frequency_sequence_complete();
    return;
  }

  HWIO_OUTM(SEQ_BRANCH_EVENT, HWIO_SEQ_BRANCH_EVENT_DCC_BMSK,
            (1 << SEQ_BRANCH_Perform_Memory_Test));
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 0);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START_ADDR, START_ADDR_DCC_MEMTEST);
  if (PARALLEL_SEQUENCING)
  {
#ifdef ALLOW_SHRM
    start_debug_timer(WAITING_FOR_SEQ0);
    HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 1);
#else
    ddr_aux_frequency_sequence_complete();
#endif // ALLOW_SHRM
  }
  else
  {
    seq_fsm_signal_start_frequency();
  }
  return;
}

// frequency_fsm_memtest_exit
//
// This function is called when we finally receive the completion for the memory test sequence
//
// Paramters:
//   fsm:  The frequency fsm data structure.
//   event: The event received.   This should only be FREQ_FSM_DCC_COMPLETE
//   next_state: Recommendation for next state from the FSM.
//
// Return:
//   The next state:  Expected is IDLE
//
unsigned int frequency_fsm_memtest_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  ddr_vote_dcc_path(FALSE);
  return next_state;
}

// frequency_fsm_dcc_entry
//
// When we enter this state, we are in the last transition of frequency and
// must run the DCC portion of the sequence and we have not yet seen the DCC
// bandwidth requests acknoledged.   This state waits for the last transition.
//
// Paramters:
//   fsm:  The frequency fsm data structure.
//   event:  The event received.
//
// Return:
//   return values.
//
void frequency_fsm_dcc_entry(AOP_FSM *fsm, unsigned int event)
{
  ddr_log_msg8 ("DCCEnter", ddr_status.ddr_dcc_on, 0, 0, 0);
  if (ddr_status.ddr_dcc_on)
  {
    freq_fsm_signal_dcc_bandwidth_complete();
  }
  else
  {
    ddr_vote_dcc_path(TRUE);
  }
  return;
}

// frequency_fsm_dcc_exit
//
// This function is called when we finally receive the completion for the DCC
// votes.
//
// Paramters:
//   fsm:  The frequency fsm data structure.
//   event: The event received.   This should only be FREQ_FSM_DCC_COMPLETE
//   next_state: Recommendation for next state from the FSM.
//
// Return:
//   The next state:  Expected is FREQUENCY_WAIT_FOR_FREQUENCY
//
unsigned int frequency_fsm_dcc_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state) {
  ddr_log_msg8 ("DCC Exit", ddr_status.ddr_dcc_on, 0, 0, 0);
  ddr_status.ddr_dcc_on = TRUE;
  return next_state;
}
#endif

// frequency_fsm_freq_entry
//
// On Entry we determine what sequence to run, program DDR_AUX to run that
// sequence, and trigger the appropriate sequences.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   None
//
void frequency_fsm_freq_entry(AOP_FSM *fsm, unsigned int event) {
  int sequence_choice = 0;
  enum frequency_start_addresses start_addr;  // Pretend start address.
  unsigned long tcs_cmd;
  unsigned long cmd_to_bitmask[] = { 1, 0x33, 0x77, 0xFF };

  // Determine which sequences to run.
#define SHUB_FREQ 1
#define MC_FREQ 2
  if (ddr_status.ddr_next_shub_lvl != ddr_status.current_shub_cp)
  {
    sequence_choice |= SHUB_FREQ;
  }
  // Determine which sequences to run.
  if ((ddr_status.ddr_next_mc_lvl != ddr_status.current_mc_cp) ||
      (ddr_status.ddr_mc_lvl_double))
  {
    sequence_choice |= MC_FREQ;
    if (ddr_status.ddr_next_mc_lvl == ddr_status.current_mc_cp)
    {
      ddr_status.ddr_mc_lvl_double = FALSE;
    }
  }

#ifndef MIN_SIZE
  if (ddr_status.ddr_next_mc_lvl > 0 && g_do_memtest) {
    ddr_status.memtest_needed = TRUE;
  } else {
    ddr_status.memtest_needed = FALSE;
  }
#endif

  HWIO_OUTF(SEQ_GCC_CLKPLAN, SHUB_CP, ddr_status.ddr_next_shub_lvl);
  HWIO_OUTF(SEQ_GCC_CLKPLAN, MC_CP, ddr_status.ddr_next_mc_lvl);

  switch (sequence_choice) {
    case SHUB_FREQ:  // SHUB Frequency only changing
      if (ddr_status.current_shub_cp == 0) {
        CORE_VERIFY(0);  // Calling a SHUB only restore an error.
      } else if (ddr_status.ddr_next_shub_lvl == 0) {
        CORE_VERIFY(0);  // Calling a SHUB only collapse an error.
      } else {
        program_frequency_cmds(SHUB_CMD_START, RSI_RESOURCE_SHUB,
                               ddr_status.ddr_next_shub_lvl);
        start_addr = START_ADDR_SHUB_ONLY_CHANGE;
      }
      break;
    case MC_FREQ:  // MC Only changing.
      if (ddr_status.current_mc_cp == 0) {
        CORE_VERIFY(0);  // Calling a MC only restore an error.
      } else if (ddr_status.ddr_next_mc_lvl == 0) {
        CORE_VERIFY(0);  // Calling a MC only collapse an error.
      } else {
        program_frequency_cmds(MC_CMD_START, RSI_RESOURCE_MC,
                               ddr_status.ddr_next_mc_lvl);
        start_addr = START_ADDR_MC_ONLY_CHANGE;
      }
      break;
    case SHUB_FREQ | MC_FREQ:  // Both SHUB and MC need to change.
      // If we were collapsed, run restore.
      if ((ddr_status.current_shub_cp == 0) ||
           (ddr_status.current_mc_cp == 0))
      {
        unsigned int tcs_cmd_index = 0;
        disable_frequency_commands();
        tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_POWERSTATE,
                                 RSI_RESOURCE_DDRSS, 0);
        HWIO_OUTI2(RSI_TCSm_CMDt, RESTORE_PS_CMD, tcs_cmd_index++, tcs_cmd);
        tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_POWERSTATE,
                                 RSI_RESOURCE_SYSTEM_CACHE, 0);
        HWIO_OUTI2(RSI_TCSm_CMDt, RESTORE_PS_CMD, tcs_cmd_index++, tcs_cmd);

        // Put the TCS into TCS (Non-AMC) mode.
        HWIO_OUTFI(RSI_TCSm_AMC_MODE, RESTORE_PS_CMD, AMC_ENABLE, 0);
        
        if ((g_alc_override) && 
            (ddr_status.ddr_bcm_alc_level != local_mc_states[mc_collapse_cp].alc_override))
        {
          ddr_status.ddr_bcm_alc_level = local_mc_states[mc_collapse_cp].alc_override;
          tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_FAL_ALC,
                                   0, local_mc_states[mc_collapse_cp].alc_override);
          // Write the third command in the TCS as the ACV command.
          HWIO_OUTI2(RSI_TCSm_CMDt, RESTORE_PS_CMD, tcs_cmd_index++, tcs_cmd);
        }

        HWIO_OUTI(RSI_TCSm_CFG, RESTORE_PS_CMD, cmd_to_bitmask[tcs_cmd_index-1]);
        start_addr = START_ADDR_RESTORE;
      }
      else if ((ddr_status.ddr_next_shub_lvl == 0) ||
               (ddr_status.ddr_next_mc_lvl == 0))
      {
        unsigned int tcs_cmd_index = 0;
        // We need to collapse.  Right now, SHUB MC collapse together.
         unsigned short lb_powerstate_data = 0x5555;
         // If the flush indicates complete, send the low powerstate for LLC
         if (ddr_status.ddr_flush_complete && !g_retain_llc_only)
         {
           static char nibble_to_mask[] = { // 0 (Not active) bits become 10 (NRET),
             0xAA, 0xA9, 0xA6, 0xA5,        // 1 (Active)     bits become 01 (RET)
             0x9A, 0x99, 0x96, 0x95,
             0x6A, 0x69, 0x66, 0x65,
             0x5A, 0x59, 0x56, 0x55
             };
           // Build the powerstate command for LLC - active lines becomes 0x10, Retain,
           // and not active ones become No retain (0x01).
           lb_powerstate_data = nibble_to_mask[ddr_status.ddr_lb_activations & 0x0F] |
                                nibble_to_mask[(ddr_status.ddr_lb_activations >> 4) & 0x0F] << 8;
         }

         disable_frequency_commands();
         if ((g_alc_override) && 
             (ddr_status.ddr_bcm_alc_level != local_mc_states[0].alc_override))
         {
           ddr_status.ddr_bcm_alc_level = local_mc_states[0].alc_override;

           // Write the first command as the ACV override to bring us down to 0's ACV value
           tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_FAL_ALC,
                                    0, local_mc_states[0].alc_override);
           HWIO_OUTI2(RSI_TCSm_CMDt, COLLAPSE_PS_CMD, tcs_cmd_index++, tcs_cmd);
         }
         tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_POWERSTATE,
                                  RSI_RESOURCE_SYSTEM_CACHE, lb_powerstate_data);
         HWIO_OUTI2(RSI_TCSm_CMDt, COLLAPSE_PS_CMD, tcs_cmd_index++, tcs_cmd);

         tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_POWERSTATE,
                                  RSI_RESOURCE_DDRSS, 3);
         HWIO_OUTI2(RSI_TCSm_CMDt, COLLAPSE_PS_CMD, tcs_cmd_index++, tcs_cmd);

         HWIO_OUTI(RSI_TCSm_CFG, COLLAPSE_PS_CMD, cmd_to_bitmask[tcs_cmd_index-1]);
         
         // Put the TCS into TCS (Non-AMC) mode.
         HWIO_OUTFI(RSI_TCSm_AMC_MODE, COLLAPSE_PS_CMD, AMC_ENABLE, 0);
         start_addr = START_ADDR_COLLAPSE;
      }
      else
      {
        program_frequency_cmds(SHUB_CMD_START, RSI_RESOURCE_SHUB,
                               ddr_status.ddr_next_shub_lvl);
        program_frequency_cmds(MC_CMD_START, RSI_RESOURCE_MC,
                               ddr_status.ddr_next_mc_lvl);
        start_addr = START_ADDR_SHUB_MC_COMBINATION;
      }
      break;
    default:
      CORE_VERIFY(0);
      break;
  }
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("FreqStrt",
                ddr_status.ddr_next_mc_lvl,
                ddr_status.ddr_next_shub_lvl,
                start_addr,
                0);
  #endif
  DDR_MGR_SWEVENT(ddr_Starting_Frequency_Seq,
          ddr_status.ddr_next_mc_lvl,
          ddr_status.ddr_next_shub_lvl,
          start_addr);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 0);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START_ADDR, start_addr);
#ifndef MIN_SIZE
  if (PARALLEL_SEQUENCING)
#endif
  {
#ifdef ALLOW_SHRM
    start_debug_timer(WAITING_FOR_SEQ0);
    HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 1);
#else
    ddr_aux_frequency_sequence_complete();
#endif
  }
#ifndef MIN_SIZE
  else
  {
    seq_fsm_signal_start_frequency();
  }
#endif
}

// cycles_to_bcd
//
// This function is used as a convenience when reading data dumps in the
// debugger.   When viewing the max time in our ddr_stats data structure,
// the debugger typically displays the number in hex.
// This function will first convert the elapsed parameter into microseconds.
// It then converts each HEX nibble into the decimal equivalent.   For example,
// if the elapsed time is 100us, instead of 100 being stored as 0x64, it will
// be stored as 0x100.   Similary, 1.234ms will be stored as 0x1234
//
// Paramters:
//   elapsed: The 56 bit time in XO cycles to convert.
//
// Return:
//   The formatted number in hex nibbles.
//
uint64_t cycles_to_bcd(uint64_t elapsed)
{
  uint64_t val = (10 * elapsed) / 192; // convert ticks to us.
  uint64_t ret_val = 0;
  uint32_t shift = 0;

  while (val > 0)
  {
    ret_val |= (val % 10) << shift;
    shift += 4;
    val /= 10;
  }

  return ret_val;
}

// frequency_fsm_freq_exit
//
// This function is called when the frequency FSM exits the frequency change
// state.   This occurs after we get the sequence complete interrupt.
//
// Paramters:
//   fsm: The FSM data structure.
//   event: The event which caused us to exit (FREQUENCY SEQUENCE COMPLETE from DDR-AUX)
//   next_state: FSM's default next state - (Process Request)
//
// Return:
//   The next state to go to.
//
unsigned int frequency_fsm_freq_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state)
{
  uint64_t now = time_service_now();
  uint64_t elapsed = 0;
  if (ddr_status.ddr_next_mc_lvl != ddr_status.current_mc_cp)
  {
    uint64_t *begin = NULL;
    uint64_t *max = NULL;
    uint64_t *max_time = NULL;
    if (ddr_status.current_mc_cp == 0) {
      // Restore time
      begin = &ddr_stats->powerstate_restore_begin;
      max = &ddr_stats->powerstate_restore_max;
      max_time = &ddr_stats->powerstate_restore_max_time;
      ddr_stats->restore_count++;
      //allow_driver_logging = false;
    } else if (ddr_status.ddr_next_mc_lvl == 0) {
      // Collapse time
      begin = &ddr_stats->powerstate_collapse_begin;
      max = &ddr_stats->powerstate_collapse_max;
      max_time = &ddr_stats->powerstate_collapse_max_time;
      ddr_stats->collapse_count++;
      //allow_driver_logging = true;
    } else if (ddr_status.ddr_next_mc_lvl != ddr_status.current_mc_cp) {
      // Frequency change time
      begin = &ddr_stats->mc_begin;
      max = &ddr_stats->mc_max;
      max_time = &ddr_stats->mc_max_time;
      ddr_stats->mc_count++;
    }
    if (begin != NULL)
    {
    elapsed = now - *begin;
      if ((uint32) elapsed > g_log_max_time)
      {
        ddr_log_msg32("Log Stop", elapsed);
        g_log_enabled = FALSE;
      }
      if (elapsed > *max)
      {
        if (elapsed < ONE_SECOND)  // Max out times at 1 second.
        {
          *max = elapsed;
          *max_time = cycles_to_bcd(elapsed); // convert ticks to us.
        }
      }
    }
  }
  if ((ddr_status.ddr_next_shub_lvl != ddr_status.current_shub_cp) &&
      (ddr_status.ddr_next_shub_lvl > 0) &&
      (ddr_status.current_shub_cp > 0))
  {
    ddr_stats->shub_count++;
    elapsed = now - ddr_stats->shub_begin;
    if (elapsed > ddr_stats->shub_max)
    {
      if (elapsed < ONE_SECOND)  // Max out times at 1 second.
      {
        ddr_stats->shub_max = elapsed;
        ddr_stats->shub_max_time = cycles_to_bcd(elapsed); // convert ticks to us.
      }
    }
  }
  ddr_status.current_mc_cp = ddr_status.ddr_next_mc_lvl;
  ddr_status.current_shub_cp = ddr_status.ddr_next_shub_lvl;
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("FreqDone",
                ddr_status.current_mc_cp, ddr_status.current_shub_cp,
                (elapsed >> 8) & 0xff, elapsed & 0xff);
  #endif
  DDR_MGR_SWEVENT(ddr_Frequency_Seq_complete, ddr_status.current_mc_cp, ddr_status.current_shub_cp);

  if (((ddr_status.current_mc_cp == 0) || (ddr_status.current_shub_cp == 0)) &&
      ((ddr_status.current_mc_cp != 0) || (ddr_status.current_shub_cp != 0)))
  {
    CORE_VERIFY(0);
  }

  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 0);
  return next_state;
}


// frequency_fsm_readdata_entry
//
// This function is used when we want to read what is active and dirty prior to a cache flush.
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//   next_state: The default next_state.
//
// Return:
//   None
//
void frequency_fsm_readdata_entry(AOP_FSM *fsm, unsigned int event)
{
  unsigned long tcs_cmd;
  unsigned int vdda_vote = ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vdda;
  unsigned int pmic_mode = ddr_cfg->pMCCfg[ddr_status.current_mc_cp].ddr_automode_cfg;
  unsigned int cx_vote = MAX(ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vddcx,
                             ddr_cfg->pSHUBCfg[ddr_status.current_shub_cp].vddcx);
  unsigned int mx_vote = MAX(ddr_cfg->pMCCfg[ddr_status.current_mc_cp].vddmx,
                             ddr_cfg->pSHUBCfg[ddr_status.current_shub_cp].vddmx);

  ddr_log_freq(FREQ_FLUSH);
  // Vote the current voltage votes.   Might as well reduce voltages during flush.
  if (pmic_mode != ddr_status.ddr_pmic_mode_vote) {
    ddr_status.ddr_pmic_mode_vote = pmic_mode;
    (void) arc_send_vote (RESOURCE_PMIC, CLIENT_DDR_FREQUENCY, pmic_mode, CompletionRequired);
  }

  frequency_fsm_send_voltages(cx_vote, mx_vote, vdda_vote, TRUE);

  if (ddr_status.tcs_0_pending == TRUE)
  {
    uint32 tcs_stat = HWIO_INI(RSI_TCSm_STAT, 0); // Status of TCS0
    #ifndef DDR_SWEVENTS
    ddr_log_msg32("ERR:TCS0", tcs_stat);
    #endif
    DDR_MGR_SWEVENT(ddr_READDATA_TCS0_INUSE, tcs_stat);
    return;
  }
  // Program TCS0 to read Activations and Dirty bits.
  tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_CACHE_STAT_QUERY,
                           SHRM_ACTIVE_LINES, 0);
  HWIO_OUTI2(RSI_TCSm_CMDt, READ_TCS, 0, tcs_cmd);

  tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_CACHE_STAT_QUERY,
                           SHRM_DIRTY_LINES, 0);
  HWIO_OUTI2(RSI_TCSm_CMDt, READ_TCS, 1, tcs_cmd);

  // Enable the two commands and make them blocking
  HWIO_OUTI(RSI_TCSm_CFG, READ_TCS, 0x33);

  // Put the TCS into AMC mode.
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, READ_TCS, AMC_ENABLE, 1);

#ifdef ALLOW_SHRM
  // Trigger the TCS in AMC mode.
  start_debug_timer(WAITING_FOR_TCS0);
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, READ_TCS, TCS_START, 0);
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, READ_TCS, TCS_START, 1);
  ddr_status.tcs_0_pending = TRUE;
#else
  freq_fsm_signal_amc_sequence_complete();
#endif
}

// frequency_fsm_readdata_exit
//
// This function is used once we get the completion to the AMC Read of the flush related data.
// This function will look at the data and determine which state to go to next.  We go thru the
// activations and dirty, and build the mask of ~active & dirty>0.   If a cache line is not dirty
// we do not flush it, and if it IS active, we do not flush it.  Only lines that are not active AND
// ARE dirty are flushed.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   return values.
//
unsigned int frequency_fsm_readdata_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state)
{
  // 1 bit per activation 2 bits per dirty  Up to 8 PCBs
  unsigned long activations;
  unsigned long dirty;
  unsigned long dirty_to_activation_mask = 0X80;
  unsigned long dirty_mask = 0;
  unsigned long dirty_sum = 0;

  activations = HWIO_INI(RSI_TCS0_RESP_DATAm, 0);
  dirty       = HWIO_INI(RSI_TCS0_RESP_DATAm, 1) & 0x0000FFFF;

  if (activations & 0xFFFFFF00) {
    CORE_VERIFY(0);
  }

  ddr_status.ddr_lb_activations = activations;  // Assume flush completes, abort sets this to false.
  ddr_status.ddr_flush_complete = TRUE;  // Assume flush completes, abort sets this to false.

  // I care about what's not active.  Invert activations
  activations = ~activations;

  // While something is still dirty.
  while (dirty & 0xFFFF) {
    // If its "not active" and "dirty > 0"
    if ((dirty_to_activation_mask & activations) && (dirty & 0xC000)) {
      // Keep the sum for statistics sake
      dirty_sum += ((dirty & 0xC000) >> 14);
      // Update the what to flush mask.
      dirty_mask |= dirty_to_activation_mask;
    }
    dirty <<= 2;
    dirty_to_activation_mask >>= 1;
  }

  // Set the ddr_pcb_mask to what was ~active AND dirty.
  ddr_status.ddr_pcb_mask = dirty_mask;

  // If something needs flushing, we'll do that, otherwise, shortcut to
  // PROCESS_REQUEST and determine what to do next.
  if (dirty_mask == 0) {
    next_state = FREQUENCY_PROCESS_REQUEST;
  }
  #ifndef DDR_SWEVENTS
  ddr_log_msg32("ReadData", ddr_status.ddr_pcb_mask);
  #endif
  DDR_MGR_SWEVENT(ddr_Flush_readdata_complete, ddr_status.ddr_pcb_mask);
  return next_state;
}

// frequency_fsm_flush_entry
//
// This function is called when we are entering the flush state.  This function
// sends the flush command to SHRM in TCS0.  We then wait for the flush to complete.
//
// Paramters:
//   fsm: The frequency fsm pointer.
//   event: The event that caused us to enter this state.
//     Neither of these are used in this function
//
// Return:
//   None
//
void frequency_fsm_flush_entry(AOP_FSM *fsm, unsigned int event)
{
  unsigned long tcs_cmd;
  uint64_t now = time_service_now();

  if (ddr_status.tcs_0_pending == TRUE)
  {
    uint32 tcs_stat = HWIO_INI(RSI_TCSm_STAT, 0); // Status of TCS1

    #ifndef DDR_SWEVENTS
    ddr_log_msg32("ERR:TCS0", tcs_stat);
    #endif
    DDR_MGR_SWEVENT(ddr_FLUSHENTRY_TCS0_INUSE, tcs_stat);
    return;
  }
  // Program TCS0 to perform the flush
  tcs_cmd = encode_tcs_cmd(RSI_RESP, RSI_LAST, RSI_OPCODE_CACHEFLUSH,
                           RSI_RESOURCE_SYSTEM_CACHE, ddr_status.ddr_pcb_mask);

  ddr_stats->flush_begin = now;
  HWIO_OUTI2(RSI_TCSm_CMDt, FLUSH_TCS, 0, tcs_cmd);
  HWIO_OUTI(RSI_TCSm_CFG, FLUSH_TCS, 0x01);
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_TCS, AMC_ENABLE, 1);
#ifdef ALLOW_SHRM
  start_debug_timer(WAITING_FOR_TCS0);
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_TCS, TCS_START, 0);
  HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_TCS, TCS_START, 1);
  ddr_status.tcs_0_pending = TRUE;
#else
  freq_fsm_signal_amc_sequence_complete();
#endif
}

// frequency_fsm_flush_exit
//
// This function is called while we are in the flush state
//
// Paramters:
//   fsm: The frequency fsm.
//   event:  What event triggered us to look at exiting.
//   next_state: The default next_state.
//
// Return:
//   The next_state to use.
//
unsigned int frequency_fsm_flush_exit(AOP_FSM *fsm, unsigned int event, unsigned int next_state)
{
  unsigned long tcs_cmd = 0;
  unsigned long flush_status = 0;
  uint64_t now = time_service_now();
  uint64_t elapsed;

  switch (event) {
    case FREQ_FSM_FREQUENCY_CHANGE_REQUEST:
      // If we get a frequency change, we need to abort the cache flush.
      if (ddr_status.tcs_1_pending == TRUE)
      {
        uint32 tcs_stat = HWIO_INI(RSI_TCSm_STAT, 1); // Status of TCS1
        #ifndef DDR_SWEVENTS
        ddr_log_msg32("ERR:TCS1", tcs_stat);
        #endif
        DDR_MGR_SWEVENT(ddr_FLUSHEXIT_TCS1_INUSE, tcs_stat);
        break;
      }
      ddr_status.ddr_flush_complete = FALSE;
      // Program TCS1 to abort the flush
      tcs_cmd = encode_tcs_cmd(RSI_FF, RSI_LAST, RSI_OPCODE_CACHEFLUSH_ABORT,
                               RSI_RESOURCE_SYSTEM_CACHE, 0);

      HWIO_OUTI2(RSI_TCSm_CMDt, FLUSH_ABORT_TCS, 0, tcs_cmd);
      HWIO_OUTI(RSI_TCSm_CFG, FLUSH_ABORT_TCS, 0x01);
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_ABORT_TCS, AMC_ENABLE, 1);
#ifdef ALLOW_SHRM
      // Since we don't care about TCS1 completing (yet), no need to
      // enable the TCS_DONE interrupt here.
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_ABORT_TCS, TCS_START, 0);
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, FLUSH_ABORT_TCS, TCS_START, 1);
      ddr_status.tcs_1_pending = TRUE;
#else
      // Currently we don't care about the FLUSH ABORT completing.
#endif

      // Wait in the waiting for flush complete until we get the AMC completion.
      next_state = FREQUENCY_WAIT_FLUSH_COMPLETE;
      break;
    case FREQ_FSM_AMC_COMPLETE:
      // When we get the AMC complete, it means that either we aborted it and the flush finished,
      // or the flush finished on its own.  Either way, move on to the default next_state.
      ddr_stats->flush_count++;
      flush_status = HWIO_INI(RSI_TCS0_RESP_DATAm, 0);
      elapsed = now - ddr_stats->flush_begin;
      if (elapsed > ddr_stats->flush_max)
      {
        ddr_stats->flush_max = elapsed;
        ddr_stats->flush_max_time = cycles_to_bcd(elapsed);  // time in us
      }

      break;
    default:
      CORE_VERIFY(0);
      break;
  }
  #ifndef DDR_SWEVENTS
  ddr_log_msg8 ("LLC_Evnt", event, next_state,
                flush_status, ddr_status.ddr_pcb_mask);
  #endif
  DDR_MGR_SWEVENT(ddr_Event_during_LLC_Flush, event, next_state,
                flush_status, ddr_status.ddr_pcb_mask);
  return next_state;
}

// setup_frequency_fsm
//
// This function initializes the FSM structure for the frequency side FSM.
//
// Paramters:
//   None
//
// Return:
//   None
//
void setup_frequency_fsm(void)
{
  aop_fsm_init (&frequency_fsm, NUM_FREQUENCY_FSM_STATES, "Frequency FSM");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_IDLE,
                 &frequency_fsm_idle_entry,
                 &frequency_fsm_idle_exit, "Idle");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_PROCESS_REQUEST,
                 &frequency_fsm_process_entry,
                 &frequency_fsm_process_exit, "Processing Request");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_WAIT_FOR_VOLTAGE,
                 &frequency_fsm_voltage_entry,
                 &frequency_fsm_voltage_exit, "Waiting for Voltage");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_WAIT_FOR_FREQUENCY,
                 &frequency_fsm_freq_entry,
                 &frequency_fsm_freq_exit, "Waiting for Frequency Sequence");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_READING_FLUSH_DATA,
                 &frequency_fsm_readdata_entry,
                 &frequency_fsm_readdata_exit, "Reading Flush Data");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_WAIT_FLUSH_COMPLETE,
                 &frequency_fsm_flush_entry,
                 &frequency_fsm_flush_exit, "Waiting for Flush completion");
#ifndef MIN_SIZE
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_WAIT_FOR_DCC,
                 &frequency_fsm_dcc_entry,
                 &frequency_fsm_dcc_exit, "Waiting for Bandwidth to DCC");
  aop_fsm_init_state(&frequency_fsm, FREQUENCY_WAIT_FOR_MEMTEST,
                 &frequency_fsm_memtest_entry,
                 &frequency_fsm_memtest_exit, "Waiting for DDR Memory Test");
#endif


  // FREQ_FSM_FREQUENCY_CHANGE_REQUEST can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_IDLE, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                FALSE, FREQUENCY_PROCESS_REQUEST); // Expect FREQUENCY_PROCESS_REQUEST state next
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_PROCESS_REQUEST, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_PROCESS_REQUEST); // Save CHANGE_REQUEST pending if Idle needs it.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_VOLTAGE, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_WAIT_FOR_VOLTAGE); // Save CHANGE_REQUEST for later
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_FREQUENCY, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_WAIT_FOR_FREQUENCY); // Save CHANGE_REQUEST for later
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_READING_FLUSH_DATA, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_READING_FLUSH_DATA); // Save this for the flush.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FLUSH_COMPLETE, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                FALSE, FREQUENCY_WAIT_FLUSH_COMPLETE); // Send Abort flush, stay in this state
#ifndef MIN_SIZE
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_DCC, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_WAIT_FOR_DCC); // Save CHANGE_REQUEST for later
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_MEMTEST, FREQ_FSM_FREQUENCY_CHANGE_REQUEST,
                TRUE, FREQUENCY_WAIT_FOR_MEMTEST); // Save CHANGE_REQUEST for later
#endif

  // FREQ_FSM_VOLTAGE_REQUIREMENTS_MET can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_VOLTAGE, FREQ_FSM_VOLTAGE_REQUIREMENTS_MET,
                FALSE, FREQUENCY_WAIT_FOR_FREQUENCY); // Expect FREQUENCY_WAIT_FOR_FREQUENCY state next

#ifndef MIN_SIZE
  // FREQ_FSM_DCC_COMPLETE can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_PROCESS_REQUEST, FREQ_FSM_DCC_COMPLETE,
                TRUE, FREQUENCY_PROCESS_REQUEST); // Save this for later
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_VOLTAGE, FREQ_FSM_DCC_COMPLETE,
                TRUE, FREQUENCY_WAIT_FOR_VOLTAGE); // Save this for later
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_DCC, FREQ_FSM_DCC_COMPLETE,
                FALSE, FREQUENCY_WAIT_FOR_MEMTEST); // Handled DCC Completion now
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_FREQUENCY, FREQ_FSM_DCC_COMPLETE,
                TRUE, FREQUENCY_WAIT_FOR_FREQUENCY); // Save this for later.
#endif

  // FREQ_FSM_FREQUENCY_SEQUENCE_COMPLETE can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_FREQUENCY, FREQ_FSM_FREQUENCY_SEQUENCE_COMPLETE,
                FALSE, FREQUENCY_PROCESS_REQUEST); // Expect FREQUENCY_PROCESS_REQUEST state next
#ifndef MIN_SIZE
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FOR_MEMTEST, FREQ_FSM_FREQUENCY_SEQUENCE_COMPLETE,
                FALSE, FREQUENCY_PROCESS_REQUEST); // When MEMTEST is done, process any new requests
#endif

  // FREQ_FSM_AMC_COMPLETE can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_READING_FLUSH_DATA, FREQ_FSM_AMC_COMPLETE,
                FALSE, FREQUENCY_WAIT_FLUSH_COMPLETE); // Expect FREQUENCY_WAIT_FLUSH_COMPLETE state next
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_WAIT_FLUSH_COMPLETE, FREQ_FSM_AMC_COMPLETE,
                FALSE, FREQUENCY_PROCESS_REQUEST); // Expect FREQUENCY_PROCESS_REQUEST state next
  // FREQ_FSM_PROCESS_COMPLETE can be received in the following states.
  aop_fsm_add_event(&frequency_fsm, FREQUENCY_PROCESS_REQUEST, FREQ_FSM_PROCESS_COMPLETE,
                FALSE, FREQUENCY_WAIT_FOR_VOLTAGE); // Typically FREQUENCY_WAIT_FOR_VOLTAGE state next

}

typedef struct {
  int irq;                   /* IRQ to register for. */
  rex_sigs_type rex_signal;  /* Rex Signal to send. */
  rex_sigs_type (*chk_int_func)(int); /* Function to check if we should set the signal. */
  void (*clr_int_func)(int); /* Function to clear interrupt. */
} IRQ_SIG_MAP;

void clear_arc_irq(int irq);
void clear_ddraux_irq(int irq);
rex_sigs_type check_amc_func(int irq);
rex_sigs_type check_arc_func(int irq);
rex_sigs_type check_bcm_func(int irq);

IRQ_SIG_MAP irq_mapping[] = {
  { ARC_CX_INTERRUPT,     SIG_ARC_CX_RM_INTERRUPT,   check_arc_func, NULL},
  { ARC_MX_INTERRUPT,     SIG_ARC_MX_RM_INTERRUPT,   check_arc_func, NULL },
  { ARC_DDRPHY_INTERRUPT, SIG_ARC_VDDA_RM_INTERRUPT, check_arc_func, NULL },
  { ARC_DDR_INTERRUPT,    SIG_ARC_DDR_RM_INTERRUPT,  NULL, clear_arc_irq },
  { BCM_IRQ_3,            SIG_BCM_INTERRUPT,         check_bcm_func, NULL },
  { DDR_AUX_IRQ_0,        SIG_DDR_AUX_SEQ0_INTERRUPT,NULL, clear_ddraux_irq},
  { DDR_AUX_IRQ_1,        SIG_DDR_AUX_SEQ1_INTERRUPT,NULL, clear_ddraux_irq},
  { DDR_AUX_IRQ_2,        SIG_DDR_AUX_TCS_DONE_INT,  NULL, clear_ddraux_irq},
  { VOLTAGE_COMP_DONE,    SIG_VOLTAGE_COMP_DONE, NULL, NULL}
};


void register_isr_func(void);

volatile boolean updated_frequency_table = FALSE;
volatile rex_sigs_type ddr_aux_signal_mask  = DDR_AUX_SIGNAL_MASK;

#ifdef ARC_TEST
#define TEST_DRV_ID 1

volatile int g_run_ddrmgr_test = FALSE;
volatile int g_test_cx_mx = FALSE;
volatile int g_test_bcm = TRUE;
volatile int g_test_ddr = FALSE;
#if 0
volatile int g_test_vrm = FALSE;
volatile int g_test_zero_temp = FALSE;
#endif
volatile int g_clear_votes = FALSE;
volatile int g_min_votes = TRUE;
volatile int g_min_test_delay = 190;              // Min delay time for the test is 10 us
volatile int g_max_test_delay = (ONE_SECOND/100); // Max delay time for the test is 10 ms
volatile uint32 iterations = 0;
volatile uint32 total_iterations = 0;

static int g_save_bcm_mc;
static int g_save_bcm_shub;
uint32     g_save_ddrss_voted = 0;
uint32     g_save_ddrss_timers = 0;

static rex_tcb_type ddr_test_tcb;

unsigned int ddr_test_delay_time = (ONE_SECOND);

/**
 * <!-- ddr_mgr_test_resume -->
 *
 * @brief If ARC_TEST is defined, checks the current time and if its >= the wakeup time,
 *        the DDR_TEST is resumed.
 *
 * @param None
 *
 * @return None
 */
int ddrtest_timer_assigned = -1;
void ddr_mgr_test_resume(uint64_t lateness)
{
  ddrtest_timer_assigned = -1;
#ifdef ARC_TEST
  rex_resume_task(&ddr_test_tcb);
#endif
}

void ddr_test_delay(unsigned int delay_time)
{
  ddrtest_timer_assigned = aop_register_timer_cb(delay_time, ddr_mgr_test_resume);
  rex_suspend_task(&ddr_test_tcb);
}

#if 0
struct temp_vote {
  uint32 assert_state;
  uint32 cx_vote;
  uint32 vdda_vote;

  uint32 cx_1ms_state;
  uint32 vdda_1ms_state;
};

static uint32 vrm_votes[] = {400, 0, 6, 0,       // Voltage, Enable, Mode, Headroom
                             500, 1, 6, 100,     // Voltage, Enable, Mode, Headroom
                             700, 1, 6, 200,     // Voltage, Enable, Mode, Headroom
                             600, 1, 6, 200,     // Voltage, Enable, Mode, Headroom
                             600, 1, 6, 100,     // Voltage, Enable, Mode, Headroom
                            };
static uint32 cx_mx_xo_votes[] = {3, 3, 3, // CX/MX
                                  //0, 1, 1, // CX/MX
                                  2, 3, 3,
                                  //0, 1, 1,
                                  7, 4, 3,
                                  //0, 1, 1,
                                  2, 3, 3,
                                  //0, 1, 1,
                                  3, 3, 3,
                                 };
static uint32 ddr_ss_ols[] = {0x03,    // OL=3=BCM or Restore Freq (CP2),
                              0x07,    // OL=4-15 = MAX(BCM CP,(OL-3) as CP)  Set MC CP to 4
                              0x04,    // OL=4-15 = MAX(BCM CP,(OL-3) as CP)  Set MC CP to 1
                              0x0f,    // OL=4-15 = MAX(BCM CP,(OL-3) as CP)  Set MC CP to 12 (or MAX)
                              0x04,    // OL=4-15 = MAX(BCM CP,(OL-3) as CP)  Set MC CP to 1
                              //0x00,    // OL=0 means Flush then OFF (Assuming no BCM CP)
                              0x0f,    // OL=4-15 = MAX(BCM CP,(OL-3) as CP)  Set MC CP to 12 (or MAX)
                              //0x00,    // OL=0 means Flush then OFF (Assuming no BCM CP)
                             };

static struct temp_vote votes[] = {
                           {0, 0x40, 0x40, 3, 3},
                           {1, 0x40, 0x40, 6, 6},
                           {1, 0xc0, 0xc0, 6, 6},
                           {1, 0x1a0, 0x40, 8, 6},
                           {1, 0x30, 0x30, 6, 6},
                           {0, 0x40, 0x40, 3, 3},
                           {0, 0xc0, 0xc0, 5, 5},
                           {0, 0x1a0, 0x40, 8, 3},
                           {0, 0x40, 0x30, 3, 2},
                           {1, 0x100, 0x30, 6, 6},
                           {0, 0x100, 0x30, 6, 2},
                           {1, 0x40, 0x30, 6, 6},
                           {0, 0x40, 0x100, 3, 6},
                           {1, 0x40, 0x30, 6, 6},
};
#endif

// The following is the list of BCM nodes we clear/minimize during a BCM test
static uint32 clear_bcm_nodes[] = {
  0, 56, 57, // MC
  10, 11, 12, 13, 14, 15, 58, 59,  // SHUB
  3, // ALC
  4, // ACV
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, // SNOC
#if defined(SDM855_TARGET_CONFIG) || defined(SDM1000_TARGET_CONFIG)
  1, 2, // compute NOC
  32, // QUP
  42, // config NOC
#endif
};


unsigned int mc_votes[] = { 0x0, 0x200, 0x400, 0x600, 0x800, 0xA00, 0xC00, 0x1000, 0x1400, 0x1800, 0x2000, 0x2400 };
unsigned int shub_votes[] = { 0x0, 0x200, 0x400, 0x800, 0xA00, 0xC00, 0x1000 };
unsigned int mc_cp = 0;
unsigned int shub_cp = 0;

void ddr_test_thread(dword param)
{
  rpmh_client_handle aop_client = NULL;

  aop_client = rpmh_create_handle (RSC_DRV_AOP, "AOP_TEST");
  uint32 req_id, ddr_idx, cx_mx_idx, vrm_idx;
  uint32 temp_idx = 0;
  uint32 vrm_test_reg_addr;
  int i;
  vrm_test_reg_addr = cmd_db_query_addr("ldoa10");

  ddr_idx = 0;
  cx_mx_idx = 0;
  vrm_idx = 0;

  ddr_test_delay(120 * ONE_SECOND);
  /* Once this was a Simple test */
  while (1)
  {
    if (g_run_ddrmgr_test == FALSE) {
      if (iterations > 0)
      {
        // clean up after the test if they turn it off.
#if 0
        if (g_test_ddr)
        {
          int i;
          ddr_status.ddr_bcm_mc_level = g_save_bcm_mc;
          ddr_status.ddr_bcm_shub_level = g_save_bcm_shub ;
          for (i = 0; i <= HWIO_DRVd_RMm_VOTE_MAXd; i++) {
            if (g_save_ddrss_voted & (1 << i))
            {
              HWIO_OUTI2(DRVd_RMm_VOTE, i, 7, D0_On);
            }
            if (g_save_ddrss_timers & (1 << i))
            {
              HWIO_OUTI2(DRVd_RMm_VOTE, i, 7, 1);
            }
          }
        }
        if (g_test_vrm)
        {
          req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE, FALSE, vrm_test_reg_addr, 0);
          req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE, FALSE, vrm_test_reg_addr+4, 0);
          req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE, FALSE, vrm_test_reg_addr+8, 0);
          req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE, FALSE, vrm_test_reg_addr+12, 0);
          rpmh_churn_all (aop_client, req_id);
        }
        if (g_test_cx_mx)
        {
          HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 0, 0);
          HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 1, 0);
        }
#endif
        iterations = 0;
      }
      ddr_test_delay(10 * ONE_SECOND);
      continue;
    } else {
      if (iterations == 0 )
      {
        int i;
        int j;

        if (g_clear_votes)
        {
          if (g_test_ddr || g_test_bcm)
          {
            g_save_bcm_mc = ddr_status.ddr_bcm_mc_level;
            g_save_bcm_shub = ddr_status.ddr_bcm_shub_level;

            for (i = 0; i <= HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_MAXr; i++) {
              for (j = 0; j < NUM_ENTRIES(clear_bcm_nodes); j++) {
                HWIO_OUTI2(RPMH_BCM_VOTETABLE_DRVr_NDd, i, clear_bcm_nodes[j],
                  (0 | HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_BMSK |
                   HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK));
              }
            }
            for (i = 0; i <= HWIO_DRVd_RMm_VOTE_MAXd; i++) {
              if (HWIO_INI2(DRVd_RMm_VOTE, i, 7) != 0)
              {
                g_save_ddrss_voted |= (1 << i);
              }
              if (HWIO_INI2(DRVd_RMm_TIMER, i, 7) != 0)
              {
                g_save_ddrss_timers |= (1 << i);
              }
#if 0
              if (g_test_ddr && (i == TEST_DRV_ID))
              {
                HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 7, D0_On);
              }
#endif
              else
              {
                HWIO_OUTI2(DRVd_RMm_VOTE, i, 7, 0);
              }
              HWIO_OUTI2(DRVd_RMm_TIMER, i, 7, 0);
              if (i != 6) {
                HWIO_OUTI2(DRVd_RMm_VOTE, i, 0, 0);
                HWIO_OUTI2(DRVd_RMm_TIMER, i, 0, 0);
                HWIO_OUTI2(DRVd_RMm_VOTE, i, 1, 0);
                HWIO_OUTI2(DRVd_RMm_TIMER, i, 1, 0);
              }
            }

            ddr_status.ddr_bcm_mc_level = 0;
            ddr_status.ddr_bcm_shub_level = 0;
          }
        }
      }
    }

    iterations++;
    total_iterations++;

#if 0
    if (g_test_vrm) {
      // We should put something in for VRM.
      req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE,
                                   FALSE, vrm_test_reg_addr, vrm_votes[vrm_idx++]);
      req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE,
                                   FALSE, vrm_test_reg_addr+4, vrm_votes[vrm_idx++]);
      req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE,
                                   FALSE, vrm_test_reg_addr+8, vrm_votes[vrm_idx++]);
      req_id = rpmh_issue_command (aop_client, RPMH_SET_ACTIVE,
                                   FALSE, vrm_test_reg_addr+12, vrm_votes[vrm_idx++]);
      rpmh_churn_all (aop_client, req_id);

      if (vrm_idx >= NUM_ENTRIES(vrm_votes)) {
        vrm_idx = 0;
      }
    }
#endif

#if 0
    if (g_test_zero_temp)
    {

      uint32 cx_state =  get_arc_current_level(RESOURCE_CX);
      uint32 state_vdda = get_arc_current_level(RESOURCE_VDDA);
      #ifndef DDR_SWEVENTS
      ddr_log_msg8 ("test_zero_temp          ", votes[temp_idx].cx_1ms_state,
                    votes[temp_idx].vdda_1ms_state, cx_state, state_vdda);
      #endif
      if (iterations > 1)
      {
        CORE_VERIFY(cx_state == votes[temp_idx].cx_1ms_state);
        CORE_VERIFY(state_vdda == votes[temp_idx].vdda_1ms_state);
      }


      temp_idx++;
      if (temp_idx >= NUM_ENTRIES(votes))
      {
        temp_idx = 0;
      }
      //update_arc_vote(RESOURCE_CX, CLIENT_DEPENDENCY, votes[temp_idx].cx_vote);
      //update_arc_vote(RESOURCE_VDDA, CLIENT_DEPENDENCY, votes[temp_idx].vdda_vote);

      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 0, vlvl_to_hlvl(RESOURCE_CX,votes[temp_idx].cx_vote));
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 2, vlvl_to_hlvl(RESOURCE_VDDA, votes[temp_idx].vdda_vote));
      g_zero_deg_assert = votes[temp_idx].assert_state;
      rex_set_sigs(&ddr_fsm_tcb, SIG_ZERO_TEMP_EVENT);
    }

    if (g_test_cx_mx) {
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 0, cx_mx_xo_votes[cx_mx_idx++]);
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 1, cx_mx_xo_votes[cx_mx_idx++]);
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 8, cx_mx_xo_votes[cx_mx_idx++]);
      if (cx_mx_idx >= NUM_ENTRIES(cx_mx_xo_votes)) {
        cx_mx_idx = 0;
      }

      ddr_test_delay(ddr_test_delay_time);
    }

    if (g_test_ddr) {
      HWIO_OUTI2(DRVd_RMm_VOTE, 17, 7, 0);
      ddr_status.ddr_flush_complete = FALSE;
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 7, ddr_ss_ols[ddr_idx++]);
      if (ddr_idx >= NUM_ENTRIES(ddr_ss_ols)) {
        ddr_idx = 0;
      }
    }
#endif

    if (g_test_bcm) {
      unsigned int mc_vote = 0;
      unsigned int shub_vote = 0;
      HWIO_OUTI2(DRVd_RMm_VOTE, 17, 7, 0);

      if ((iterations % 17) == 0)
      {
        mc_vote = 0x3fff;
        shub_vote = 0x3fff;
      }
      else if ((iterations % 31) == 0)
      {
        mc_vote = 0x3fff;
        shub_vote = 0x3fff;
      }
      else
      {
        mc_cp++;
        shub_cp++;

        // MC increasing
        if (((mc_cp / NUM_ENTRIES(mc_votes)) % 4) >= 2)
        {
          mc_vote = mc_votes[mc_cp % NUM_ENTRIES(mc_votes)];
        }
        else
        {
          mc_vote = mc_votes[NUM_ENTRIES(mc_votes) - (mc_cp % NUM_ENTRIES(mc_votes))];
        }
        // SHUB increasing
        if (((shub_cp / NUM_ENTRIES(shub_votes)) % 4) >= 2)
        {
          shub_vote = shub_votes[shub_cp % NUM_ENTRIES(shub_votes)];
        }
        else
        {
          shub_vote = shub_votes[NUM_ENTRIES(shub_votes) - (shub_cp % NUM_ENTRIES(shub_votes))];
        }
      }
      HWIO_OUTI2(RPMH_BCM_VOTETABLE_DRVr_NDd, TEST_DRV_ID, 0,
        (mc_vote | HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_BMSK |
                 HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK));
      HWIO_OUTI2(RPMH_BCM_VOTETABLE_DRVr_NDd, TEST_DRV_ID, 10,
        (shub_vote | HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_BMSK |
                 HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK));
    }

#if 0
    if (g_test_cx_mx) {
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 0, cx_mx_xo_votes[cx_mx_idx++]);
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 1, cx_mx_xo_votes[cx_mx_idx++]);
      HWIO_OUTI2(DRVd_RMm_VOTE, TEST_DRV_ID, 8, cx_mx_xo_votes[cx_mx_idx++]);
      if (cx_mx_idx >= NUM_ENTRIES(cx_mx_xo_votes)) {
        cx_mx_idx = 0;
      }
    }
#endif

    ddr_test_delay(ddr_test_delay_time);

    ddr_mgr_config->ddr_mgr_test_count = iterations;
    if ((iterations % 10) == 0)
    {
      if (g_min_votes && ((iterations % 100000) == 0))
      {
        int i, j;
        for (i = 0; i <= HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_MAXr; i++)
        {
          for (j = 0; j < NUM_ENTRIES(clear_bcm_nodes); j++)
          {
            uint32_t val;
            // If the system has a vote for this BCM node
            if ((val = HWIO_INI2(RPMH_BCM_VOTETABLE_DRVr_NDd, i,
                                         clear_bcm_nodes[j])) & 0x03FFFFFF)
            {
              // And the vote is not 1, set it to 1.
              if ((val & 0x3FFF) != 1)
              {
                HWIO_OUTI2(RPMH_BCM_VOTETABLE_DRVr_NDd, i, clear_bcm_nodes[j],
                  (1 | HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_BMSK |
                   HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK));
              }
            }
          }
        }
        for (i = 0; i <= HWIO_DRVd_RMm_VOTE_MAXd; i++)
        {
          if (i != 6)
          {
            if (HWIO_INI2(DRVd_RMm_VOTE, i, 0) > 2)
            {
              HWIO_OUTI2(DRVd_RMm_VOTE, i, 0, 2);
              HWIO_OUTI2(DRVd_RMm_VOTE, i, 1, 3);
            }
          }
        }
      }
      ddr_test_delay_time = ((3*ddr_test_delay_time) / 4);
      if (ddr_test_delay_time <= g_min_test_delay)
      {
        ddr_test_delay_time = g_max_test_delay ;
      }
    }
  }
}

#endif

/**

 * <!-- send_initial_votes -->
 *
 * @brief  This function sends the initial votes to the uNPA
 *
 * @param
 * @param
 *
 * @return
 */
static void send_initial_votes (void)
{
  uint32 cx_vlvl;
  uint32 mx_vlvl;
  uint32 vdda_vlvl;

  //TODO: Need to read the following from cmd_db?
  voltage_arc_status[RESOURCE_MX].mol = 3;
  voltage_arc_status[RESOURCE_CX].mol = 2;
  voltage_arc_status[RESOURCE_VDDA].mol = 2;

    voltage_arc_status[RESOURCE_MX].sequence_level =
    voltage_arc_status[RESOURCE_MX].in_sequence_level =
      voltage_arc_status[RESOURCE_MX].current = get_arc_current_level(RESOURCE_MX);

    voltage_arc_status[RESOURCE_CX].sequence_level =
    voltage_arc_status[RESOURCE_CX].in_sequence_level =
      voltage_arc_status[RESOURCE_CX].current = get_arc_current_level(RESOURCE_CX);

    voltage_arc_status[RESOURCE_VDDA].sequence_level =
    voltage_arc_status[RESOURCE_VDDA].in_sequence_level =
      voltage_arc_status[RESOURCE_VDDA].current = get_arc_current_level(RESOURCE_VDDA);

  cx_vlvl = hlvl_to_vlvl(RESOURCE_CX, voltage_arc_status[RESOURCE_CX].sequence_level);
  mx_vlvl = hlvl_to_vlvl(RESOURCE_MX, voltage_arc_status[RESOURCE_MX].sequence_level);
  vdda_vlvl = hlvl_to_vlvl(RESOURCE_VDDA, voltage_arc_status[RESOURCE_VDDA].sequence_level);

  // The sequencing here is important so that we don't end up with multiple NPA apply calls
  (void) arc_send_vote (RESOURCE_XO, CLIENT_DEPENDENCY, XO_ON, FireForget);
  (void) arc_send_vote (RESOURCE_CX, CLIENT_TEMP_EVENT, 0, FireForget);
  (void) arc_send_vote (RESOURCE_VDDA, CLIENT_TEMP_EVENT, 0, FireForget);
  (void) arc_send_vote (RESOURCE_MX, CLIENT_TEMP_EVENT, 0, FireForget);

  frequency_fsm_send_voltages(cx_vlvl, mx_vlvl, vdda_vlvl, FALSE);
  (void) arc_send_vote (RESOURCE_PMIC, CLIENT_DDR_FREQUENCY,
                 ddr_cfg->pMCCfg[ddr_status.current_mc_cp].ddr_automode_cfg,
                 CompletionRequired);

  ddr_status.ddr_pmic_mode_vote = ddr_cfg->pMCCfg[ddr_status.current_mc_cp].ddr_automode_cfg;

  ddr_status.ddr_mx_vote = mx_vlvl;
  ddr_status.ddr_cx_vote = cx_vlvl;
  ddr_status.ddr_vdda_vote = vdda_vlvl;

  (void) arc_send_vote (RESOURCE_MX, CLIENT_DEPENDENCY, cx_to_mx_map[voltage_arc_status[RESOURCE_CX].current], FireForget);
  ddr_rm_vote((enum DDR_OLS) g_ddrss_on_vote);
}

// process_amc_func
//
// This function is used to verify that a TCS Done interrupt is
// sent for a TCS which we use in AMC mode.
//
// Paramters:
//   irq: The interrupt that caused this.   Validate its what we
//        are looking for.
//
// Return:
//   The signal(s) to send to the thread.
//
void process_amc_func()
{
  uint32 tcs_stat;
  if (ddr_status.tcs_0_pending)
  {
    tcs_stat = HWIO_INMI(RSI_TCSm_STAT, 0, HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK); // Status of TCS0
    if (tcs_stat == 0) {
      stop_debug_timer(WAITING_FOR_TCS0);
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, 0, TCS_START, 0);
      ddr_status.tcs_0_pending = FALSE;
      freq_fsm_signal_amc_sequence_complete();
    }
  }
  if (ddr_status.tcs_1_pending)
  {
    tcs_stat = HWIO_INMI(RSI_TCSm_STAT, 1, HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK); // Status of TCS1
    if (tcs_stat == 0) {
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, 1, TCS_START, 0);
      ddr_status.tcs_1_pending = FALSE;
    }
  }
  if (ddr_status.tcs_2_pending)
  {
    tcs_stat = HWIO_INMI(RSI_TCSm_STAT, 2, HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK); // Status of TCS2
    if (tcs_stat == 0) {
      HWIO_OUTFI(RSI_TCSm_AMC_MODE, 2, TCS_START, 0);
      ddr_status.tcs_2_pending = FALSE;
      service_fal_complete();
    }
  }
}


// reinit_gcc
//
// GCC was unable to be initialized by the boot process.
// This needs to be fixed in future chips, for now now, run our mini GCC_INIT
// sequence to assign clock plans for SHRM, MC, and GCC
//
// Paramters:
//   param: unused.
//
// Return:
//   None
//
void reinit_gcc(void)
{
  // GCC is not getting initialized properly so we must run the BOOT_HACK
  // sequence to complete the initialization.
  HWIO_OUTF(SEQ_GCC_CLKPLAN, SHRM_CP, ddr_status.ddr_shrm_level);
  HWIO_OUTF(SEQ_GCC_CLKPLAN, SHUB_CP, ddr_status.current_shub_cp);
  HWIO_OUTF(SEQ_GCC_CLKPLAN, MC_CP, ddr_status.current_mc_cp);

  program_frequency_cmds(SHUB_CMD_START, RSI_RESOURCE_SHUB,
                         ddr_status.current_shub_cp);
  program_frequency_cmds(MC_CMD_START, RSI_RESOURCE_MC,
                         ddr_status.current_mc_cp);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 0);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START_ADDR, GCC_INIT);
  HWIO_OUTFI(SEQn_CFG, FREQUENCY_SEQUENCER, SEQ_START, 1);
  (void) rex_wait(SIG_DDR_AUX_SEQ0_INTERRUPT);
  rex_clr_sigs(&ddr_fsm_tcb, SIG_DDR_AUX_SEQ0_INTERRUPT);
}

/**
 * <!-- copy_clockplan -->
 *
 * @brief Copies the SMEM version of the clockplan into local memory.
 *
 * @param src - the SMEM copy of the clock plan
 * @param dest - the local version
 *
 * @return None
 */
void copy_clockplan(freq_state *src, LOCAL_FREQ_STATE *dest)
{
  // Last sanity checks
  if ((src->clk_idx != dest->clk_idx) ||
      (src->freq_khz > 10000000))
  {
    CORE_VERIFY(0);
  }

  dest->freq_mhz = src->freq_khz/1000;
  dest->clk_period = (src->clk_period==0)?((src->freq_khz==0)?0xFFFFFFFF:(1000000000/src->freq_khz)):src->clk_period;
  dest->freq_enable = src->freq_enable;
  dest->mode = src->mode;
  dest->vddcx = src->vddcx;
  if ((src->vddmx > VDD_RET) && (src->vddmx < NOMINAL))
  {
    dest->vddmx = NOMINAL;
  }
  else
  {
    dest->vddmx = src->vddmx;
  }
  dest->vdda =  src->vdda;
  dest->ddr_automode_cfg = (uint32) src->ddr_automode_cfg;
  dest->max_up_idx = src->max_up_idx;
  dest->min_down_idx = src->min_down_idx;
  dest->double_freq_change = src->double_freq_change;
  dest->copied = TRUE;
}

/**
 * <!-- validate_cfg_table -->
 *
 * @brief  Ensure the local frequency table does not have common errors.  Also, update
 *         the collapse frequency to be values as specified in the table.
 *
 * @param  local_cfg - the structure which describes the local config.
 *
 * @return None
 */
void validate_cfg_table(LOCAL_DDRCfgType *local_cfg)
{
  uint32 min_enabled_mc = 0;
  uint32 min_enabled_shub = 0;
  uint32 max_enabled_mc = 0;
  uint32 max_enabled_shub = 0;
  uint32 mc_table_collapse = 0;
  uint32 shub_table_collapse = 0;
  uint32 index;
  uint32 max_shub_period;

  for (index = 1; index < 16; index++)
  {
    if ((mc_table_collapse == 0) &&
        (local_cfg->pMCCfg[index].min_down_idx == 0))
    {
      mc_table_collapse = index;
    }
    if ((shub_table_collapse == 0) &&
        (local_cfg->pSHUBCfg[index].min_down_idx == 0))
    {
      shub_table_collapse = index;
    }

    if (local_cfg->pMCCfg[index].freq_enable)
    {
      max_enabled_mc = index;
      if (min_enabled_mc == 0)
      {
        min_enabled_mc = index;
      }
    }
    if (local_cfg->pSHUBCfg[index].freq_enable)
    {
      max_enabled_shub = index;
      if (min_enabled_shub == 0)
      {
        min_enabled_shub = index;
      }
    }
  }

  max_shub_period = ddr_cfg->pMCCfg[max_enabled_mc].clk_period * 4;
  while ((max_enabled_mc > 0) && (ddr_cfg->pSHUBCfg[max_enabled_shub].clk_period > max_shub_period))
  {
    ddr_cfg->pMCCfg[max_enabled_mc].freq_enable = FALSE;
    while ((max_enabled_mc > 0) && (ddr_cfg->pMCCfg[--max_enabled_mc].freq_enable == FALSE))
      ;
    max_shub_period = ddr_cfg->pMCCfg[max_enabled_mc].clk_period * 4;
  }

  if ((mc_table_collapse > 0) && (local_cfg->pMCCfg[mc_table_collapse].freq_enable))
  {
    mc_collapse_cp = mc_table_collapse;
  }
  else
  {
    mc_collapse_cp = min_enabled_mc;
  }
  if ((shub_table_collapse > 0) && (local_cfg->pSHUBCfg[shub_table_collapse].freq_enable))
  {
    shub_collapse_cp = shub_table_collapse;
  }
  else
  {
    shub_collapse_cp = min_enabled_shub;
  }

  CORE_VERIFY(mc_collapse_cp > 0);
  CORE_VERIFY(shub_collapse_cp > 0);

  for (index = 1; index < 16; index++)
  {
    if (local_cfg->pMCCfg[index].min_down_idx == index)
    {
      local_cfg->pMCCfg[index].min_down_idx = index-1;
    }
    if (local_cfg->pSHUBCfg[index].min_down_idx == index)
    {
      local_cfg->pSHUBCfg[index].min_down_idx = index-1;
    }
    if (local_cfg->pMCCfg[index].max_up_idx <= index)
    {
      local_cfg->pMCCfg[index].max_up_idx = index+1;
    }
    if (local_cfg->pSHUBCfg[index].max_up_idx <= index)
    {
      local_cfg->pSHUBCfg[index].max_up_idx = index+1;
    }
    if ((local_cfg->pMCCfg[index].min_down_idx == 0) && (index != mc_collapse_cp))
    {
      local_cfg->pMCCfg[index].min_down_idx = 1;
    }
    if ((local_cfg->pSHUBCfg[index].min_down_idx == 0) && (index != shub_collapse_cp))
    {
      local_cfg->pSHUBCfg[index].min_down_idx = 1;
    }
  }

  local_cfg->pMCCfg[0].min_down_idx = 0;
  local_cfg->pMCCfg[0].max_up_idx = mc_collapse_cp;
  local_cfg->pMCCfg[1].min_down_idx = mc_collapse_cp;
  local_cfg->pMCCfg[mc_collapse_cp].min_down_idx = 0;

  local_cfg->pSHUBCfg[0].min_down_idx = 0;
  local_cfg->pSHUBCfg[0].max_up_idx = shub_collapse_cp;
  local_cfg->pSHUBCfg[1].min_down_idx = shub_collapse_cp;
  local_cfg->pSHUBCfg[shub_collapse_cp].min_down_idx = 0;
}

/**
 * <!-- copy_global_cfg -->
 *
 * @brief  Validate and then copy the SMEM config structure to local memory
 *
 * @param global_cfg - the SMEM version of the table
 * @param local_cfg - the local version of the table
 *
 * @return
 */
void copy_global_cfg(DDRCfgType *global_cfg, LOCAL_DDRCfgType *local_cfg)
{
  int index;

  if ((g_use_local_table == TRUE) ||
      (global_cfg == NULL) ||
      (global_cfg->pMCCfg == NULL) ||
      (global_cfg->pSHUBCfg == NULL) ||
      (global_cfg->nNumMCStates == 0) ||
      (global_cfg->nNumMCStates > 16) ||
      (global_cfg->nNumSHUBStates == 0) ||
      (global_cfg->nNumSHUBStates > 16))
  {
    return;
  }

  // As we copy the table, we need to recompute the collapse/restore clock plans
  mc_collapse_cp = 0;
  shub_collapse_cp = 0;
  for (index = 0; index < global_cfg->nNumMCStates; index++) {
    if (g_copy_enable_only) {
      local_cfg->pMCCfg[index].freq_enable = global_cfg->pMCCfg[index].freq_enable;
    } else {
      copy_clockplan(&global_cfg->pMCCfg[index], &local_cfg->pMCCfg[index]);
    }
  }
  local_cfg->nNumMCStates = global_cfg->nNumMCStates;
  for (;index < 16; index++) {
    local_cfg->pMCCfg[index].freq_enable = FALSE;
  }

  for (index = 0; index < global_cfg->nNumSHUBStates; index++) {
    if (g_copy_enable_only) {
      local_cfg->pSHUBCfg[index].freq_enable = global_cfg->pSHUBCfg[index].freq_enable;
    } else {
      copy_clockplan(&global_cfg->pSHUBCfg[index], &local_cfg->pSHUBCfg[index]);
    }
  }
  local_cfg->nNumSHUBStates = global_cfg->nNumSHUBStates;
  for (;index < 16; index++) {
    local_cfg->pSHUBCfg[index].freq_enable = FALSE;
  }

  validate_cfg_table(local_cfg);

  return;
}

/**
 * <!-- clear_hardcode_ddr_cp -->
 *
 * @brief  If a test mode has disabled any clock plans, this function reenables those.
 *
 * @param None
 *
 * @return None
 */
void clear_hardcode_ddr_cp(void)
{
  uint32 index;

  // We will vote the powerstate arc to this value.   It will show that we
  // are in test mode by doing this.
  g_ddrss_on_vote = D0_On;
  for (index = 1; index < 16; index++)
  {
    if (local_mc_states[index].copied & FREQ_PLAN_TEST_DISABLED)
    {
      local_mc_states[index].copied &= ~FREQ_PLAN_TEST_DISABLED;
      local_mc_states[index].freq_enable = TRUE;
    }
    if (local_shub_states[index].copied & FREQ_PLAN_TEST_DISABLED)
    {
      local_shub_states[index].copied &= ~FREQ_PLAN_TEST_DISABLED;
      local_shub_states[index].freq_enable = TRUE;
    }
  }
}

// hardcode_ddr_cp
//
// This function is used for testing to set a clock plan to the maximum value.
//
// Paramters:
//   max_cp:
//     0 -  Restore previous setting
//     !0 - The highest clock plan frequency that would be allowed.
//
//   mode:
//     CAPPED - Allow frequency to scale up to CAP rate.
//     FIXED  - Set frequency to specific rate.
//
// Return:
//   None
//
void hardcode_ddr_cp(uint32 max_cp_freq, ddr_debug_mode mode)
{
  uint32 index;
  uint32 ddr_voltage;
  uint32 max_cp = 0;

  // Make sure any previous setting is cleared first.
  clear_hardcode_ddr_cp();

  for (index = 1; index < 16; index++)
  {
    if (local_mc_states[index].freq_mhz >= max_cp_freq)
    {
      break;
    }
  }
  max_cp = index;

  // For now, we cannot limit it below the collapse frequency, and still support collapse.
  if ((max_cp < mc_collapse_cp) || (max_cp >= 15))
  {
    ddr_status.ddr_bcm_mc_level = ddr_status.ddr_reg_mc_level = find_enabled_plan(ddr_cfg->pMCCfg, ddr_cfg->nNumMCStates,
                                                               ddr_status.ddr_reg_mc_level);
    rex_set_sigs(&ddr_fsm_tcb, SIG_UPDATE_FREQUENCY_PLAN);
    return;
  }

  // We will vote the powerstate arc to this value.   It will show that we
  // are in test mode by doing this.
  if ( FIXED == mode )
  {
    g_ddrss_on_vote = max_cp + D0_On;
  }

  for (index = 1+max_cp; index < 16; index++)
  {
    if (index <= mc_collapse_cp)
    {
      continue;
    }

    if (local_mc_states[index].freq_enable)
    {
      // Set the 0x2 bit if we are disabling.
      local_mc_states[index].copied |= FREQ_PLAN_TEST_DISABLED;
      local_mc_states[index].freq_enable = FALSE;
    }
  }
  ddr_voltage = local_mc_states[max_cp].vddcx;
  for (index = 1; index < 16; index++)
  {
    if (index <= shub_collapse_cp)
    {
      continue;
    }

    if (local_shub_states[index].vddcx > ddr_voltage)
    {
      if (local_shub_states[index].freq_enable)
      {
        // Set the 0x2 bit if we are disabling.
        local_shub_states[index].copied |= FREQ_PLAN_TEST_DISABLED;
        local_shub_states[index].freq_enable = FALSE;
      }
    }
  }
  ddr_status.ddr_bcm_mc_level = ddr_status.ddr_reg_mc_level = find_enabled_plan(ddr_cfg->pMCCfg, ddr_cfg->nNumMCStates,
                                                               ddr_status.ddr_reg_mc_level);
  rex_set_sigs(&ddr_fsm_tcb, SIG_UPDATE_FREQUENCY_PLAN);
}

// aop_ddr_client_callback
//
// This is the registered callback for all aop_ddr_client's rpmh requests.
// The callback notification needs to be requested for the callback to be
// called on completeion.
//
// Prarameters:
//  req_id: Used for logging.
//
void aop_ddr_client_callback(uint32 req_id)
{
   ddr_log_msg32("DDRclnCB", req_id);
   if (g_xo_seq_req_id == req_id)
   {
    uint64_t elapsed=time_service_now()-g_xo_on_time;
    uint32_t elapsed_time=(uint32_t)cycles_to_bcd(elapsed);

    if (elapsed_time > ddr_stats->pad)
    {
        ddr_stats->pad = elapsed_time;
    }

      if ((ddr_status.ddr_xo_on == FALSE) && (ddr_status.ddr_xo_vote == XO_ON))
      {
        ddr_log_msg32("XO done.", req_id);
        //Send SIG_DRIVER_XO_VOTE_COMP to update XO seq completion state.
        rex_set_sigs(&ddr_fsm_tcb, SIG_DRIVER_XO_VOTE_COMP);
        g_xo_seq_req_id = 0;
      }

      rex_set_sigs(&ddr_fsm_tcb, SIG_VOTE_DONE_INT);
    }
}


static bool g_zero_degree_voted = FALSE;
void zero_degree_c_update()
    {
  if (!g_aoss_sleep_locked && g_zero_deg_assert)
  {
    (void) arc_send_vote(RESOURCE_CX, CLIENT_TEMP_EVENT, g_temp_nom_volt, FireForget);
    (void) arc_send_vote(RESOURCE_VDDA, CLIENT_TEMP_EVENT, g_temp_nom_volt, FireForget);
    g_zero_degree_voted = TRUE;
  } 
  else if (g_aoss_sleep_locked && g_zero_degree_voted)
  {
    (void) arc_send_vote(RESOURCE_CX, CLIENT_TEMP_EVENT, 0, FireForget);
    (void) arc_send_vote(RESOURCE_VDDA, CLIENT_TEMP_EVENT, 0, FireForget);
    g_zero_degree_voted = FALSE;
    }
}

// ddr_aux_main_loop
//
// This is the main loop for DDR_AUX functionality.  It first initializes the FSM
// structures and other variables, and then waits for REX events.  For each rex event,
// it calls the appropriate function to transition the FSM through the states.
//
// Paramters:
//   param: unused.
//
// Return:
//   Does not return.
//
void ddr_aux_main_loop(dword param)
{
  unsigned long rex_signals = 0;
  unsigned int  cx_hlvl;
  DDR_MGR_INFO_T *ddr_mgr_info_ptr = NULL;
  ClockCfgCPType* boot_cfg_cp;

  int ret_val = rinit_module_init("ddraux");  // Program initial DDR_AUX registers.

#ifdef DDR_LOG_MSGRAM
  log_data = msg_ram_get_aop_area();
  memset(log_data, 0, 1024);
#endif

  rex_clr_sigs(&ddr_fsm_tcb, 0xFFFFFFFF);
  setup_sequencer_fsm();
  setup_frequency_fsm();
  register_isr_func();

#ifndef SDX24_STUBS
  wlan_workaround_init();
#endif

  ddr_mgr_info_ptr = (DDR_MGR_INFO_T *) msg_ram_get_frequency_stats_area();
  memset(ddr_mgr_info_ptr, 0, sizeof(DDR_MGR_INFO_T));
  ddr_mgr_info_ptr->dictionary.ddr_event_offset = ((uint32) &(ddr_mgr_info_ptr->freq_switch_events)) - (uint32) ddr_mgr_info_ptr;
  ddr_mgr_info_ptr->dictionary.ddr_event_count  = FREQ_SWITCH_COUNT;
  ddr_mgr_info_ptr->dictionary.ddr_stats_offset = ((uint32) &(ddr_mgr_info_ptr->ddr_stats)) - (uint32) ddr_mgr_info_ptr;
  ddr_mgr_info_ptr->dictionary.ddr_mgr_config_offset = ((uint32) &(ddr_mgr_info_ptr->ddr_mgr_config)) - (uint32) ddr_mgr_info_ptr;
  msg_ram_ddrstats_set_init((void *) ddr_mgr_info_ptr);

  freq_switch_table_ptr = &ddr_mgr_info_ptr->freq_switch_events[0];
  ddr_stats = &ddr_mgr_info_ptr->ddr_stats;
  ddr_mgr_config = &ddr_mgr_info_ptr->ddr_mgr_config;

  ddr_mgr_config->ddr_mgr_operational_info |= g_opscale_shub ? DDRMGR_OPSCALE_SHUB_FLAG:0;
  ddr_mgr_config->ddr_mgr_operational_info |= g_alc_override ?DDRMGR_OVERRIDE_ALC_FLAG:0;
  ddr_mgr_config->ddr_mgr_operational_info |= g_opscale_bcm ? DDRMGR_OPSCALE_BCM_FLAG:0;

#ifdef ARC_TEST  
  ddr_mgr_config->ddr_mgr_test_info |= g_test_bcm ? DDRMGR_TEST_BCM:0;
  ddr_mgr_config->ddr_mgr_test_info |= g_test_ddr ? DDRMGR_TEST_DDR:0;
  ddr_mgr_config->ddr_mgr_test_info |= g_test_cx_mx ? DDRMGR_TEST_CX_MX:0;
#if 0
  ddr_mgr_config->ddr_mgr_test_info |= g_test_vrm ? DDRMGR_TEST_VRM:0;
  ddr_mgr_config->ddr_mgr_test_info |= g_test_zero_temp ? DDRMGR_TEST_ZERO_TEMP:0;
#endif
  ddr_mgr_config->ddr_mgr_test_info |= g_clear_votes ? DDRMGR_TEST_CLEAR_VOTES:0;
  ddr_mgr_config->ddr_mgr_test_info |= g_min_votes ? DDRMGR_TEST_MIN_VOTES:0;
  ddr_mgr_config->ddr_mgr_test_info |= g_run_ddrmgr_test ? DDRMGR_RUN_DDRMGR_TEST_CODE:0;
#endif

  ddr_mgr_config->alc_override_values = 0;
  ddr_mgr_config->ddraux_command_timeout = g_command_timeout;
  ddr_mgr_config->log_stop_time       = g_log_max_time;
  ddr_mgr_config->ddr_config_status   = DDRMGR_FLAG_DONE+config_updates;

  voltage_arc_status_ptr = &voltage_arc_status[0];

  ddr_rails.rails[RESOURCE_CX].rc = arc_runtime_register (RPMH_ARC_VDD_CX, rail_change_handler, 0);
  arc_runtime_event_register (ddr_rails.rails[RESOURCE_CX].rc, ARC_RUNTIME_EVENT_PREWORK);

  ddr_rails.rails[RESOURCE_MX].rc = arc_runtime_register (RPMH_ARC_VDD_MX, rail_change_handler, 0);
  arc_runtime_event_register (ddr_rails.rails[RESOURCE_MX].rc, ARC_RUNTIME_EVENT_PREWORK);

  ddr_rails.rails[RESOURCE_VDDA].rc = arc_runtime_register (RPMH_ARC_VDD_EBI, rail_change_handler, 0);
  arc_runtime_event_register (ddr_rails.rails[RESOURCE_VDDA].rc, ARC_RUNTIME_EVENT_PREWORK);

  ddr_arc_address = cmd_db_query_addr("ddr.lvl");
  xo_arc_address = cmd_db_query_addr("xo.lvl");

  // Create the path from AOSS to SHUB (so DCC can get from CNOC onto SHUB.
#ifndef MIN_SIZE
  icb_handle = icb_get_handle_ex(ICBID_MASTER_AOSS,ICBID_SLAVE_SNOC_MEM_NOC_SF, icb_callback, NULL);
  CORE_VERIFY(icb_handle);
#endif

  HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_0_EN, (1 << BCM_MC_CLOCK_DOMAIN)  | (1 << BCM_SHUB_CLOCK_DOMAIN) |
                                           (1 << BCM_ALC_CLOCK_DOMAIN) | (1 << BCM_ACV_CLOCK_DOMAIN) );

  aop_ddr_client = rpmh_create_handle (RSC_DRV_AOP, "DDR_MGR");
  rpmh_client_register_completion_callback(aop_ddr_client, aop_ddr_client_callback);

  global_ddr_cfg = Clock_GetBootConfigDDR();
  ddr_cfg = &local_ddr_cfg;
  boot_cfg_cp = Clock_GetBootConfigCP();

  // Try to read from DDR Team's SMEM first.
  if (((smem_ddr_cfg.pMCCfg = (freq_state *) get_ddr_table(MC_STATES, &smem_ddr_cfg.nNumMCStates)) != NULL) &&
      ((smem_ddr_cfg.pSHUBCfg = (freq_state *) get_ddr_table(SHUB_STATES, &smem_ddr_cfg.nNumSHUBStates)) != NULL))
  {
    // Copy the SMEM version to our version.
    copy_global_cfg(&smem_ddr_cfg, ddr_cfg);
  }
  else
  {
    // Next try to read from Clock Team's SMEM.
    global_ddr_cfg = Clock_GetBootConfigDDR();
    copy_global_cfg(global_ddr_cfg, ddr_cfg);
  }
  if (smem_ddr_cfg.pMCCfg) {
    free(smem_ddr_cfg.pMCCfg);
  }
  if (smem_ddr_cfg.pSHUBCfg) {
    free(smem_ddr_cfg.pSHUBCfg);
  }

  ddr_status.ddr_dest_mc_lvl    = boot_cfg_cp->nDDR;
  ddr_status.ddr_next_mc_lvl    = boot_cfg_cp->nDDR;
  ddr_status.current_mc_cp      = boot_cfg_cp->nDDR;
  ddr_status.ddr_bcm_mc_level   = boot_cfg_cp->nDDR;
  ddr_status.ddr_reg_mc_level   = boot_cfg_cp->nDDR;

  ddr_status.ddr_dest_shub_lvl  = boot_cfg_cp->nSHUB;
  ddr_status.ddr_next_shub_lvl  = boot_cfg_cp->nSHUB;
  ddr_status.current_shub_cp    = boot_cfg_cp->nSHUB;
  ddr_status.ddr_bcm_shub_level = boot_cfg_cp->nSHUB;
  ddr_status.ddr_reg_shub_level = boot_cfg_cp->nSHUB;

  // Build the dependency table
  for (cx_hlvl = 0; cx_hlvl < 16; cx_hlvl++)
  {
    uint32 cx_vlvl = hlvl_to_vlvl(RESOURCE_CX, cx_hlvl);
    uint32 mx_vlvl;
    if ((cx_vlvl > VDD_RET) && (cx_vlvl <= NOMINAL))
    {
      mx_vlvl = NOMINAL;
    }
    else
    {
      mx_vlvl = cx_vlvl;
    }
    cx_to_mx_map[cx_hlvl] = mx_vlvl;
  }

  ddr_mgr_config->alc_override_values = build_alc_mask();
  send_initial_votes ();

#ifdef ALLOW_SHRM  
  // Call reinit_gcc last before our main loop.   It
  // uses the interrupt and rex signals, and we need to
  // ensure everything is initialized first.
  reinit_gcc();
#endif

  while (TRUE)
  {
    rex_signals = rex_wait(ddr_aux_signal_mask);
    rex_clr_sigs(&ddr_fsm_tcb, rex_signals);
    if (updated_frequency_table) {
      validate_cfg_table(ddr_cfg);
      updated_frequency_table = FALSE;
    }
    if (rex_signals == 0) {
      continue;
    }

    // If the ddr_config_status variable is the special CALLCODE value, update our globals.
#if 0
    if (ddr_mgr_config->ddr_config_status == DDRMGR_FLAG_NEW)
    {
      uint32 test_info        = ddr_mgr_config->ddr_mgr_test_info;
      uint32 operational_info = ddr_mgr_config->ddr_mgr_operational_info;

      config_updates++;
      g_log_enabled = TRUE;

      g_command_timeout = ddr_mgr_config->ddraux_command_timeout;
      g_log_max_time    = ddr_mgr_config->log_stop_time;
      //g_opscale_shub = (boolean) ((operational_info & DDRMGR_OPSCALE_SHUB_FLAG) != 0)?TRUE:FALSE;
      //g_alc_override = (boolean) ((operational_info & DDRMGR_OVERRIDE_ALC_FLAG) != 0)?TRUE:FALSE;
      //g_opscale_bcm  = (boolean) ((operational_info & DDRMGR_OPSCALE_BCM_FLAG)  != 0)?TRUE:FALSE;
   
      g_test_bcm =     (int) ((test_info & DDRMGR_TEST_BCM)         != 0)?TRUE:FALSE;
      g_test_ddr =     (int) ((test_info & DDRMGR_TEST_DDR)         != 0)?TRUE:FALSE;
      g_test_cx_mx =   (int) ((test_info & DDRMGR_TEST_CX_MX)       != 0)?TRUE:FALSE;
#if 0
      g_test_vrm =     (int) ((test_info & DDRMGR_TEST_VRM)         != 0)?TRUE:FALSE;
      g_test_zero_temp=(int) ((test_info & DDRMGR_TEST_ZERO_TEMP)   != 0)?TRUE:FALSE;
#endif

      g_clear_votes =  (int) ((test_info & DDRMGR_TEST_CLEAR_VOTES) != 0)?TRUE:FALSE;
      g_min_votes =    (int) ((test_info & DDRMGR_TEST_MIN_VOTES)   != 0)?TRUE:FALSE;

      g_run_ddrmgr_test=(int)((test_info & DDRMGR_RUN_DDRMGR_TEST_CODE) != 0)?TRUE:FALSE;

      ddr_mgr_config->ddr_config_status = DDRMGR_FLAG_DONE+config_updates;
    }
#endif
    #ifndef DDR_SWEVENTS
    ddr_log_msg32("MainLoop", rex_signals);
    #endif
    //DDR_MGR_SWEVENT(ddr_MAIN_LOOP_PROCESSING, rex_signals);

    if (rex_signals & SIG_POLL_TIMER)
    {
      // Nothing to do now that AGG Blocking is removed.
    }

    if (rex_signals & SIG_DDR_AUX_TCS_DONE_INT)
    {
      // Clear the interrupt here as clearing it in the ISR is unreliable.
      // Since the DDR_AUX interrupt is edge triggered, its possible
      // that a second TCS triggered the interrupt in between the time when
      HWIO_OUT(AOP_INTR_CLR, 0x04);
      process_amc_func();
    }

    // After the workaround for Below MOL, handle the Pre/Postwork interrupts
    // to minimize the time they are not processed.   Basically, keep the ARC
    // moving if at all possible.
    if (rex_signals & SIG_ARC_CX_SEQ_PRE_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_CX, ARC_RUNTIME_EVENT_PREWORK);
    }
    if (rex_signals & SIG_ARC_CX_SEQ_POST_INTERRUPT)
      {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_CX, ARC_RUNTIME_EVENT_POSTWORK);
      }
    if (rex_signals & SIG_ARC_MX_SEQ_PRE_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_MX, ARC_RUNTIME_EVENT_PREWORK);
    }
    if (rex_signals & SIG_ARC_MX_SEQ_POST_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_MX, ARC_RUNTIME_EVENT_POSTWORK);
    }
    if (rex_signals & SIG_ARC_VDDA_SEQ_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_EBI, ARC_RUNTIME_EVENT_PREWORK);
    }

    // Handle BCM and Powerstate requests before the sequence is done.
    if (rex_signals & SIG_BCM_INTERRUPT)
    {
      service_bcm_interrupt();
    }
    if (rex_signals & SIG_ARC_DDR_RM_INTERRUPT)
    {
      service_ddr_rm_interrupt();
    }

    // Now handle items that may have a flurry of activity. Frequency sequences
    // completing may cause a new process step and a flury of vote.
    if (rex_signals & SIG_DDR_AUX_SEQ0_INTERRUPT)
    {
      // Clear the interrupt here as clearing it in the ISR is unreliable.
      HWIO_OUT(AOP_INTR_CLR, 0x01);
      stop_debug_timer(WAITING_FOR_SEQ0);
      ddr_aux_frequency_sequence_complete();
    }
    if (rex_signals & SIG_VOTE_DONE_INT)
    {
      if (freq_fsm_pending_vote && frequency_needs_met())
    {
        freq_fsm_pending_vote = FALSE;
        freq_fsm_signal_voltage_requirements_met();
    }
    }

    // Now check if any resources are pending aggregation.  It is possible we
    // have not processed the Sequence done yet, but as soon as we see the
    // voltage sequence complete, we'll want to know of any pending ARCs
    if (rex_signals & SIG_ARC_CX_RM_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_CX, ARC_RUNTIME_EVENT_AGGREGATION);
    }
    if (rex_signals & SIG_ARC_MX_RM_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_MX, ARC_RUNTIME_EVENT_AGGREGATION);
    }
    if (rex_signals & SIG_ARC_VDDA_RM_INTERRUPT)
    {
      arc_runtime_event_dispatch (RPMH_ARC_VDD_EBI, ARC_RUNTIME_EVENT_AGGREGATION);
    }
    if (rex_signals & SIG_DDR_AUX_SEQ1_INTERRUPT)
    {
      // Clear the interrupt here as clearing it in the ISR is unreliable.
      HWIO_OUT(AOP_INTR_CLR, 0x02);
      stop_debug_timer(WAITING_FOR_SEQ1);
      seq_fsm_signal_voltage_sequence_complete();
    }

#ifndef MIN_SIZE
    if (rex_signals & SIG_DRIVER_DCC_BANDWIDTH)
    {
      freq_fsm_signal_dcc_bandwidth_complete();
    }
#endif
    if (rex_signals & SIG_DRIVER_XO_VOTE_COMP)
    {
      xo_command_complete();  // Signal XO command is done.
    }
    if (rex_signals & SIG_VOLTAGE_SEQ)
    {
      aop_fsm_process_event(&sequencer_fsm, FSM_VOLTAGE_CHANGE_READY);
    }
    if (rex_signals & SIG_FAL_INTERRUPT)
    {
      service_fal_interrupt();
    }
    if (rex_signals & SIG_VOLTAGE_COMP_DONE)
    {
      cx_collapse_voltage_comp_done ();
    }
    if (rex_signals & SIG_UPDATE_FREQUENCY_PLAN)
    {
      freq_fsm_signal_new_frequency();
    }
    if (rex_signals & SIG_ZERO_TEMP_EVENT)
    {
      #ifndef DDR_SWEVENTS
      ddr_log_msg32("ZeroTemp", 0);
      #endif
      zero_degree_c_update();
    }
  }
}

/*
If the aggregated state is MOL or above.   There is almost no change necessary as solver would have no effect.  The sequence value chosen is not going to change.
        Only update is to disable the SW workaround interrupt from the ARC sequence.
If the aggregated state is below MOL, (and we want to allow that value) we enable to SW workaround in the ARC sequence.
Beyond enabling the interrupt; we basically ignore the fact it aggregated to a new value and it doesnӴ advance any of our state machines.
        When we get the interrupt from the ARC sequence, we read the sequence value now, and start the DDR_AUX sequence as required with that value.
    NOTE: 2 Behaviors are possible here:
1)      ARC decides to run a sequence to a value lower than its current state: (Either MOL or below MOL), and we run the sequence.
2)      ARC is at MOL and solves to stay at MOL
a.      No interrupt will occur in this case until we get an ԁRC AGGREGATED eventԠindicating itӳ above MOL.
b.      In this case, see ԁggregated state is MOL or aboveԠabove.
*/

// clear_arc_irq
//
// This function clears the ARC IRQ.
//
// Paramters:
//   irq: The interrupt which occurred.
//
// Return:
//   return values.
//
void clear_arc_irq(int irq)
{
  int value;
  int mask;
  int index = 0;  // Sets the RM that needs clearing.
  switch (irq) {
    case ARC_CX_INTERRUPT:
      index = RPMH_ARC_VDD_CX;
      break;
    case ARC_MX_INTERRUPT:
      index = RPMH_ARC_VDD_MX;
      break;
    case ARC_DDRPHY_INTERRUPT:
      index = RPMH_ARC_VDD_EBI;
      break;
    case ARC_DDR_INTERRUPT:
      index = RPMH_ARC_DDR_SS;
      break;
    default:
      CORE_VERIFY(0);
  }

  // Read the IRQ status - we will clear these bits.
  value = HWIO_INI(RMm_IRQ_STATUS, index);
  mask  = HWIO_INI(RMm_IRQ_EN, index);

  value = value & mask;

  // Clear the Interrupt from the ARC..
  HWIO_OUTI(RMm_IRQ_CLR, index, value);
}

// clear_ddraux_irq
//
// This function clears the interrupt status in the DDR_AUX.
//
// Paramters:
//   irq: The interrupt we receieved, this sets the mask to clear.
//
// Return:
//   None
//
void clear_ddraux_irq(int irq)
{
  unsigned long mask;
  switch (irq) {
    case DDR_AUX_IRQ_0:
      mask = DDR_AUX_SEQ0_INTCLR_MSK; // Sequencer 0 complete.
      break;
    case DDR_AUX_IRQ_1:
      mask = DDR_AUX_SEQ1_INTCLR_MSK; // Sequencer 1 complete.
      break;
    case DDR_AUX_IRQ_2:
      mask = DDR_AUX_TCS_DONE_INTCLR_MSK; // TCS_DONE
      break;
    default:
      CORE_VERIFY(0);
  }
  HWIO_OUT(AOP_INTR_CLR, mask);
}

// check_arc_func
//
// This function is used to determine which interrupt for the ARC fired.
// It then sends the correct signal to the FSM.
//
// Paramters:
//   irq: The interrupt that caused this.   Validate its what we
//        are looking for.
//
// Return:
//   The signal(s) to send to the thread.
//
rex_sigs_type check_arc_func(int irq)
{
  unsigned long aop_intr_value = 0;
  unsigned long aop_intr_mask = 0;
  int index = 0;  // Sets the RM that needs clearing.
  rex_sigs_type ret_val = 0;

  switch (irq) {
    case ARC_CX_INTERRUPT:
      index = 0;
      ret_val = SIG_ARC_CX_SEQ_PRE_INTERRUPT | SIG_ARC_CX_RM_INTERRUPT | SIG_ARC_CX_SEQ_POST_INTERRUPT;
      break;
    case ARC_MX_INTERRUPT:
      index = 1;
      ret_val = SIG_ARC_MX_SEQ_PRE_INTERRUPT | SIG_ARC_MX_RM_INTERRUPT | SIG_ARC_MX_SEQ_POST_INTERRUPT;
      break;
    case ARC_DDRPHY_INTERRUPT:
      index = 2;
      ret_val = SIG_ARC_VDDA_SEQ_INTERRUPT | SIG_ARC_VDDA_RM_INTERRUPT;
      break;
    default:
      CORE_VERIFY(0);
  }

  // Read the IRQ status - we will clear these bits.
  aop_intr_value = HWIO_INI(RMm_IRQ_STATUS, index);
  aop_intr_mask  = HWIO_INI(RMm_IRQ_EN, index);

  // Clear the Interrupt from the ARC..
  HWIO_OUTI(RMm_IRQ_CLR, index, aop_intr_value & aop_intr_mask);

  if (0 == (aop_intr_value & ARC_AGGR_DONE))
  {
    ret_val &= ~(SIG_ARC_CX_RM_INTERRUPT |
                 SIG_ARC_MX_RM_INTERRUPT |
                 SIG_ARC_VDDA_RM_INTERRUPT);
  }
  if (0 == (aop_intr_value & ARC_SEQUENCE_PREWORK))
  {
    ret_val &= ~(SIG_ARC_CX_SEQ_PRE_INTERRUPT |
                 SIG_ARC_MX_SEQ_PRE_INTERRUPT |
                 SIG_ARC_VDDA_SEQ_INTERRUPT);
  }

  if (0 == (aop_intr_value & ARC_SEQUENCE_POSTWORK))
  {
    ret_val &= ~(SIG_ARC_CX_SEQ_POST_INTERRUPT |
                 SIG_ARC_MX_SEQ_POST_INTERRUPT);
  }

  return ret_val;
}

// check_bcm_func
//
// This function is used to determine which Clock domain for BCM fired.
// It then returns the signal to send if appropriate.
//
// Paramters:
//   irq: The interrupt that caused this.   Validate its what we
//        are looking for.
//
// Return:
//   The signal(s) to send to the thread.
//

rex_sigs_type check_bcm_func(int irq)
{
  unsigned long aop_intr_value = 0;
  rex_sigs_type ret_val = 0;
  uint32  interrupt_status = HWIO_IN(INTERRUPT_SEQ_GENERIC_INT_0_STATUS);
  HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_0_EN, interrupt_status, ~interrupt_status);

  if (interrupt_status & (1 << BCM_MC_CLOCK_DOMAIN)) {
    // Make sure the reg variable is set before pending is set to TRUE
    ddr_status.ddr_reg_mc_level = HWIO_INFI(STATUS_BE_CDm, BCM_MC_CLOCK_DOMAIN, CLK_DEST_STATE);
    ddr_status.ddr_reg_mc_level = find_enabled_plan(ddr_cfg->pMCCfg, ddr_cfg->nNumMCStates,
                                                    ddr_status.ddr_reg_mc_level);
    ddr_status.bcm_mc_pending = TRUE;
    ret_val |= SIG_BCM_INTERRUPT;
  }
  if (interrupt_status & (1 << BCM_SHUB_CLOCK_DOMAIN)) {
    // Make sure the reg variable is set before pending is set to TRUE
    ddr_status.ddr_reg_shub_level = HWIO_INFI(STATUS_BE_CDm, BCM_SHUB_CLOCK_DOMAIN, CLK_DEST_STATE);
    ddr_status.ddr_reg_shub_level = find_enabled_plan(ddr_cfg->pSHUBCfg, ddr_cfg->nNumSHUBStates,
                                                      ddr_status.ddr_reg_shub_level);
    ddr_status.bcm_shub_pending = TRUE;
    ret_val |= SIG_BCM_INTERRUPT;
  }
  if (interrupt_status & (1 << BCM_ACV_CLOCK_DOMAIN)) {
    if (!g_alc_override)
    {
    ddr_status.ddr_bcm_acv_level = HWIO_INFI(STATUS_BE_CDm, BCM_ACV_CLOCK_DOMAIN, CLK_DEST_STATE);
    }
    ddr_status.bcm_acv_pending = FAL_PENDING;
    ret_val |= SIG_FAL_INTERRUPT;
  }
  if (interrupt_status & (1 << BCM_ALC_CLOCK_DOMAIN)) {
    if (!g_alc_override)
    {
    ddr_status.ddr_bcm_alc_level = HWIO_INFI(STATUS_BE_CDm, BCM_ALC_CLOCK_DOMAIN, CLK_DEST_STATE);
    }
    ddr_status.bcm_alc_pending = FAL_PENDING;
    ret_val |= SIG_FAL_INTERRUPT;
  }
  return ret_val;
}


// ddr_mgr_isr_func
//
// This is the function rex calls when an interrupt occurs.  Basically,
// it loops through the irq_mapping function and attempts to find the
// correction functions to call.
//
// Paramters:
//   irq: The interrupt which occurred.  See aop_interrupt_table.h for
//        the definition of all the interupts.
//
// Return:
//   return values.
//
void ddr_mgr_isr_func(int irq)
{
  int i;
  rex_sigs_type signal_to_send = 0;

  //ddr_mgr_test_resume();
  ++interrupt_count;
  for (i = 0; i < NUM_ENTRIES(irq_mapping); i++)
  {
      // Once we find the interrupt.
      if (irq_mapping[i].irq == irq) {
        // Set the default signal.
        signal_to_send = irq_mapping[i].rex_signal;
        if (irq_mapping[i].chk_int_func != NULL)
        {
          // If there is a chk_int_func function, then call that
          // to update the signal_to_send.  This function is used
          // when we need to read a status register to choose the
          // signal to send.
          signal_to_send = (*irq_mapping[i].chk_int_func)(irq);
        }
        // If there is still a signal to send, send it.
        if (signal_to_send) {
          rex_set_sigs(&ddr_fsm_tcb, signal_to_send);
        }
        // Call the clear function.
        if (irq_mapping[i].clr_int_func != NULL)
        {
          (*irq_mapping[i].clr_int_func)(irq); /* Function to clear interrupt. */
        }
      break;
    }
  }
  #ifndef DDR_SWEVENTS
  ddr_log_msg32("DDR ISR ", ((irq&0xFF) << 24) | (signal_to_send & 0xFFFFFF));
  #endif
  //DDR_MGR_SWEVENT(ddr_DDR_MGR_ISR_FUNC, irq, signal_to_send);
  rex_clear_interrupt(irq);
}

// register_isr_func
//
// This function will loop through the irq_mapping structure and register and
// enable each of the interrupts.
//
// Paramters:
//   None
//
// Return:
//   None
//
void register_isr_func(void)
{
  int i;

  for (i = 0; i < NUM_ENTRIES(irq_mapping); i++)
  {
    rex_set_interrupt_vector(irq_mapping[i].irq, &ddr_mgr_isr_func);
    rex_enable_interrupt(irq_mapping[i].irq);
  }
}

REX_DECLARE_STACK(ddr_task_stack, 224 * sizeof(rex_stack_word_type));
#ifdef ARC_TEST
REX_DECLARE_STACK(ddr_test_stack, 96 * sizeof(rex_stack_word_type));
#endif

#define DDR_AUX_PRIORITY 120
#define ARC_TEST_PRIORITY 101

static void ddr_process_msg( char* msg, EDEGES client )
{
  char str[RES_WORD_SIZE];
  uint32 val = 0;
  ddr_debug_mode mode;

  /* Get setting value */
  if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "val", strlen("val"), str, RES_WORD_SIZE) )
  {
    val = atoi(str);
  }

  if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "res", strlen("res"), str, RES_WORD_SIZE) )
  {
    if ( strcmp(str, "capped") == 0 )
    {
      hardcode_ddr_cp( val, CAPPED );
    }
    else if ( strcmp(str, "fixed") == 0 )
    {
      hardcode_ddr_cp( val, FIXED );
    }
  }
}

// zero_temp_event_cb
//
// {class: voltage_floor, event:zero_temp, res: cx/mx/ebi value: on/off}
// Paramters:
//   None
//
// Return:
//   None
//
void zero_temp_event_cb(char* msg, EDEGES sub )
{
  char val[RES_WORD_SIZE];
  /* QDSS message */
  if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "event", strlen("event"), val, RES_WORD_SIZE) )
  {
    if ( strcmp(val, "zero_temp") == 0 )
    {
      if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "value", strlen("value"), val, RES_WORD_SIZE) )
      {
        if ( strcmp(val, "on") == 0 )
        {
          g_zero_deg_assert = TRUE;
        }
        else if (strcmp(val, "off") == 0)
        {
          g_zero_deg_assert = FALSE;
        }
        rex_set_sigs(&ddr_fsm_tcb, SIG_ZERO_TEMP_EVENT);
      }
    }
  }
}

// ddr_mgr_init
//
// This is the function which is called by the idle thread to start the initialization
// of DDR_AUX.   This function must be called prior to any vote coming for any DDR related
// resource.
//
// Paramters:
//   None
//
// Return:
//   None
//
void ddr_mgr_init()
{
  memset(&ddr_fsm_tcb, 0, sizeof(ddr_fsm_tcb));
  rex_def_task(&ddr_fsm_tcb, ddr_task_stack,
               REX_STACK_SIZE(ddr_task_stack),
               DDR_AUX_PRIORITY, &ddr_aux_main_loop, 0);

  CORE_VERIFY ( MSG_SUCESS == qmp_msg_register_cb(ddr_process_msg, "ddr") );
  CORE_VERIFY ( MSG_SUCESS == qmp_msg_register_cb(&zero_temp_event_cb, "volt_flr"));

#ifdef ARC_TEST
  // If we are running the ARC test to validate DDR, start the arc_thread to
  // run the test.
  memset(&ddr_test_tcb, 0, sizeof(ddr_test_tcb));
  rex_def_task(&ddr_test_tcb, ddr_test_stack,
               REX_STACK_SIZE(ddr_test_stack),
               ARC_TEST_PRIORITY, &ddr_test_thread, 0);
#endif
}
