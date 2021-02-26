/*===========================================================================
                              wlan_workaround.c

DESCRIPTION:

Copyright (c) 2018 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
#define WLAN_CR        // Workaround for HWCR  QCTDD04455948  
#define WLAN_WA_ABORT  // Force AOP to ABORT if this workaround isn't working

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddr_mgmt.h"
#include "aop_hwio.h"
#include "wcssaon_hwio.h"
#include "time_service.h"
#include "CoreVerify.h"

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
#define WLAN_CONFIG_CHANGE_DELAY   100
#define WLAN_STATUS_POLL_INTERVAL  25
#define WLAN_READ_STATUS_DELAY     2
#define MAX_SLEEP_TRIES  20
#define MAX_WAKE_TRIES   4

#define WLAN_MASK (HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_VAL_BMSK | \
                   HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_BMSK     | \
                   HWIO_MPM_WCSSAON_CONFIG_TEST_BUS_SEL_BMSK)                      
#define WLAN_VAL_CLMP (HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_VAL_BMSK | \
                   HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_BMSK)
#define WLAN_VAL_SM_MODE (1 << HWIO_MPM_WCSSAON_CONFIG_TEST_BUS_SEL_SHFT)                      
#define WLAN_VAL_TSTBUS (HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_VAL_BMSK | \
                   HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_BMSK     | \
                   (2 << HWIO_MPM_WCSSAON_CONFIG_TEST_BUS_SEL_SHFT))                      

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
void *msg_ram_get_cprf_area (void);

// WA Info is saved for each Retention Sleep or Wake
// (two entries per cycle).   The top 8 bites of timestamp
// encode an error value - 0 means normal sleep/wake, not
// zero will be an error in the processing of the sleep/wake
typedef struct wlan_wa_info 
{
  uint64_t timestamp;
  // uint32_t flag1;
  uint32_t flag2;
  uint32_t flag3;
} WLAN_ENTRY;

typedef struct wlan_wa_stats {
  uint32_t signature;  // Expect "WLAN"
  uint32_t stats[7];   // 7 Stats Counters 0-2 are the sleep stats, 3-4 are wake stats, 5 is wake errors, 6 is sleep errors
} WLAN_STATS;

#define NUM_WLAN_LOGS (64-2)  // 32 bytes taken for stats
#define WLAN_SIGNATURE 0x4e414c57   // "WLAN" in hex.

uint32_t wlan_log_index = 0;
WLAN_STATS *wlan_stats = NULL;
WLAN_ENTRY *wlan_logs = NULL;

uint32_t wlan_time_stats[2][12];
uint32_t wlan_time_totals[2];

/**
 * <!-- log_wlan_info -->
 *
 * @brief   Stores a set of log data into our buffer
 *
 * @param   flag1 - 0 Normal, non zero for error 
 * @param   flag2 - Typically the config register
 * @param   flag3 - Typically the status register
 *
 * @return  None
 */
void log_wlan_info(uint32_t flag1, uint32_t flag2, uint32_t flag3)
{
  if (wlan_logs) {
    wlan_logs[wlan_log_index].timestamp = time_service_now();
    wlan_logs[wlan_log_index].timestamp |= (uint64_t) (flag1 & 0xFF) << 56;
    wlan_logs[wlan_log_index].flag2 = flag2;
    wlan_logs[wlan_log_index].flag3 = flag3;

    wlan_log_index = ((wlan_log_index+1) % NUM_WLAN_LOGS);
  }
}

/**
 * <!-- fill_histogram -->
 *
 * @brief   Updates the timing statistics structures
 *
 * @param   stats - The stats buffers to use.
 * @param   hist  - Where to store the percentages
 * @param   total - How many cycles did we have.
 * @param   fields - How many 32 bit entries is the histogram
 *
 * @return  None
 */
void fill_histogram(uint32_t *stats, uint32_t *hist, int start_index, uint32_t total, uint32_t fields)
{
  int i;
  uint8_t percent;
  CORE_VERIFY(fields < (7 - start_index));
  memset(hist, 0, 4*fields);
  for (i = 0; i < (fields*4); i++)
  {
    percent = (stats[i] * 250) / total;
    if (stats[i]) {
      percent++;
    }
    hist[i/4] |= ((percent & 0xff) << 8*(i%4));
  }
}

/**
 * <!-- save_wlan_stats -->
 *
 * @brief   Entry to update the timing statistics structures
 *
 * @param   index - 0 = sleep, 1 = wake
 * @param   elapsed - how many cycles did it take.
 *
 * @return  None
 */
void save_wlan_stats(uint32_t index, uint64_t elapsed)
{
  int bucket;
  int start_index;

  if (index > 2)
    return;

  // The bucket is how many 50 usecond periods it took.
  bucket = elapsed / 960;

  // Index 0 is not offset.   Sleep Stats
  if (index == 0)
  {
    if (bucket >= 12) {
      bucket = 11;
    }
    wlan_time_stats[0][bucket]++;
    wlan_time_totals[0]++;
    if (wlan_stats) {
      start_index = 0; // For klockwork
      fill_histogram(wlan_time_stats[0], &wlan_stats->stats[start_index], start_index, wlan_time_totals[0], 3);
    }
  }
  else if (index == 1) 
  {
    // Index 1 is offset by 50us.   0-99.9us is the first bucket.
    if (bucket <= 1) {
      bucket = 0;
    } else {
      bucket--;
      if (bucket >= 8) {
        bucket=7;
      }
    }
    wlan_time_stats[1][bucket]++;
    wlan_time_totals[1]++;
    if (wlan_stats) {
      start_index = 3; // For klockwork
      fill_histogram(wlan_time_stats[1], &wlan_stats->stats[start_index], start_index, wlan_time_totals[1], 2);
    }
  }
}

/**
 * <!-- wlan_status_read -->
 *
 * @brief   Read the status register and wait for it to be stable.
 *
 * @param   error_val - what to put if we get an error.
 * @param   last_read - Where to put the last read value.
 *
 * @return  None
 */
#define STATUS_TRIES 5
#define STATUS_MASK ((1 << 13) - 1)
uint32_t wcssaon_status[STATUS_TRIES];
bool wlan_status_read(uint32 error_val, uint32_t *last_read)
{
  int i;
  uint32_t first_status = HWIO_IN(WCSSAON_MPM_STATUS);
  uint32_t last_status = first_status;

  memset(wcssaon_status, 0xa5, sizeof(wcssaon_status));

  for (i = 0; i < STATUS_TRIES; i++)
  {
    busywait (WLAN_READ_STATUS_DELAY);
    // 1. On CX collapse/retention entry, read WCSSAON_MPM_STATUS
    //    (while it is in AON_SM mode; mode = 4’d1) until the same
    //     value is read two consecutive times
    //     (Only consider bits 12 downto 0),
    //    in first five iterations of read
    wcssaon_status[i] = HWIO_IN(WCSSAON_MPM_STATUS);
    if ((wcssaon_status[i] & STATUS_MASK) == (last_status & STATUS_MASK))
    {
      // Two consequtive reads!
      break;
    }
    last_status = wcssaon_status[i];
  }

  if (i >= STATUS_TRIES)
  {
    // 2. If no two consecutive reads match in first five data read,
    //    terminate the loop, and assert an error flag. 
    log_wlan_info(error_val, first_status, wcssaon_status[0]);
    log_wlan_info(error_val, wcssaon_status[1], wcssaon_status[2]);
    log_wlan_info(error_val, wcssaon_status[3], wcssaon_status[4]);
    if (wlan_stats) {
      wlan_stats->stats[6]++;
    }
    *last_read = wcssaon_status[4];
    return FALSE;
  }

  *last_read = last_status;
  return TRUE;
}

/**
 * <!-- wlan_workaround_init -->
 *
 * @brief   Called during init.  Sets up the structures.
 *
 * @return  None
 */
void wlan_workaround_init(void) 
{
  wlan_stats = msg_ram_get_cprf_area();
  memset(wlan_stats, 0, 1024);
  memset(wlan_time_stats, 0, sizeof(wlan_time_stats));
  wlan_logs = (WLAN_ENTRY  *) (wlan_stats+1);
  wlan_stats->signature = WLAN_SIGNATURE;

  //From EMAIL:  Init time configuration:
  //mpm_wcssaon_config[18:15] = 4’d1;  // so that pmu_aon_sm_testbus[31:0] is selected
  //Mpm_wcssaon_config = mpm_wcssaon_config | 0x00008000;  // Program so that WCSSAON_MPM_STATUS[12:0] gives WCSS AON FSM state
  HWIO_OUTM (MPM_WCSSAON_CONFIG, WLAN_MASK, WLAN_VAL_SM_MODE);
}

/**
 * <!-- wlan_below_mol_workaround -->
 *
 * @brief   Called before CX goes below MOL.
 *
 * @return  None
 */
void wlan_below_mol_workaround(void)
{
  uint32 wcssaon = HWIO_IN (MPM_WCSSAON_CONFIG);

  if (wcssaon & HWIO_MPM_WCSSAON_CONFIG_ARES_N_BMSK)
  {
    int    i;
    uint32_t wlan_status1;
    uint32_t wlan_status2;
    uint64_t start = time_service_now();  // Stats for how long we are here.
    uint64_t end = 0;  // Stats for how long we are here.

    if (! wlan_status_read(1, &wlan_status1))
    {
      // ERROR
    }
    log_wlan_info(0, wcssaon, wlan_status1);

    switch (wlan_status1 & STATUS_MASK)
    {
      // 3. Safe state match condition :
      //     On finding the first pair of matching AON_SM values in step #1,
      //     if state matches any of the five safe states listed above, program
      //     MSM clamp ovrd and ovrd_val bits with 1’b1, using the
      //     expression 3a) below.   
      //       assign     mpm_wcssaon_msm_clamp_en_ovrd     = mpm_wcssaon_config_slp[6] ;
      //       assign     mpm_wcssaon_msm_clamp_en_ovrd_val = mpm_wcssaon_config_slp[7] ;
      //       a) MPM_WCSSAON_CONFIG = MPM_WCSSAON_CONFIG | 0x0000_00C0;
      //       b) Allow ARC to continue with h/w CX collapse/retention sequence immediately
      case 0x0:   // ALL_ZERO
      case 0x1:   // RESET
      case 0x80:  // POWER_OFF
      case 0x200: // PBS_WAIT
      case 0x400: // WLAN_DISABLE
        HWIO_OUTM (MPM_WCSSAON_CONFIG, WLAN_MASK, WLAN_VAL_CLMP);
        break;
      default:
        // 4. Unsafe state match condition :
        //    In step #1, if the first pair of matching AON_SM values does not
        //    fall in safe state category (five safe states listed above)
        //    a) i. Change mux select of WCSSAON_MPM_STATUS mux to 2, which is
        //          referred to as MUX_SEL_2_CLAMP_EN_ST
        //      ii. Along with this also update MSM clamp ovrd and ovrd_val bits with 1’b1
        //          mpm_wcssaon_config[18:15] = 4’d2;  // so that pmu_aon_sm_testbus[63:32] is selected
        //          mpm_wcssaon_config[6] = mpm_wcssaon_config[7] = 1
        //          MPM_WCSSAON_CONFIG = MPM_WCSSAON_CONFIG | 0x0001_00C0;
        HWIO_OUTM (MPM_WCSSAON_CONFIG, WLAN_MASK, WLAN_VAL_TSTBUS);

        //    b) WAIT for 3 sleep clocks (3 cycles of 32Khz), for the updated
        //       MPM_WCSSAON_CONFIG to be reflected inside WCSSAON
        // AOP can go and do its other tasks in this time.
        busywait (WLAN_CONFIG_CHANGE_DELAY);

        for (i = 0; i < MAX_SLEEP_TRIES; i++)
        {
          //    c) Read the WCSSAON_MPM_STATUS register up to 5 times to
          //       check that bits 0 and 1 are both ‘1’ at the same time,
          //       in two consecutive reads. (Due to clamp override or other wise)
          //       The key here is, WCSSAON_MPM_STATUS having both 0 and 1 bits 1,
          //       indicates the test_mux switched to MUX_SEL_2_CLAMP_EN_ST, as
          //       MUX_SEL_1_AON_FSM_ST, the previous state, is one-hot in LSB 13 bits.
          //       This should also cause clamps to get asserted for func_clk_disable signal inside WLAN,
          //       as clamp overrides, which were also asserted simultaneously with mux select change,
          //       should take only one extra cycle at the maximum, due to the expectation of a common
          //       path from AOSS to WCSS.
          //    d) If we do not see a match in step c) above, read WCSSAON_MPM_STATUS after another 50us WAIT
          //        (Again, up to 5 times, to check that bits 0 and 1 are both ‘1’ at the same time,
          //       in two consecutive reads.)
          //    e) If we read bits 0 and 1, as ‘1’, it implies the change in clamp overrides has taken effect,
          //       and we can give the control to ARC sequencer for CX collapse sequence initiation.
          //    f) If step d) is still not reading data, we can repeat steps d) and e) till WCSSAON_MPM_STATUS
          //       reflects new test bus mode (MUX_SEL_2_CLAMP_EN_ST).
          if (wlan_status_read(2, &wlan_status2))
          {
            if ((wlan_status2 & 0x03) == 0x03)
            {
              // Step C above has succeeded, break out of the For loop.
              break;
            }
          }
          // Either we got an error from wlan_status, or its not what we want.
          busywait(WLAN_STATUS_POLL_INTERVAL);
        }
        if (i >= MAX_SLEEP_TRIES) {
          if (wlan_stats) {
            log_wlan_info(4, wlan_status1, wlan_status2);
            wlan_stats->stats[6]++;
#ifdef WLAN_WA_ABORT
            wlan_stats->stats[6] |= 0x80000000;
            abort();
#endif
          }
        }
        break;
    }

    end = time_service_now();
    save_wlan_stats(0, end - start);
  }
  else {
    log_wlan_info(0, wcssaon, ~0);
  }
}

/**
 * <!-- wlan_below_mol_workaround -->
 *
 * @brief   Called after CX goes above MOL from below.
 *
 * @return  None
 */
void wlan_above_mol_workaround(void)
{
  // WLAN workaround
  uint32 wcssaon = HWIO_IN (MPM_WCSSAON_CONFIG);
  log_wlan_info(0, 0x80000000 | wcssaon, 0);
  if (wcssaon & HWIO_MPM_WCSSAON_CONFIG_ARES_N_BMSK)
  {
    int i;
    uint64_t start = time_service_now();  // Stats for how long we are here.
    uint64_t end = 0;  // Stats for how long we are here.
    uint32_t wlan_status;

    //5. On CX collapse/retention exit,
    //     change mpm_wcssaon_msm_clamp_en_ovrd and mpm_wcssaon_msm_clamp_en_ovrd_val to ‘0’. Also change  mpm_wcssaon_config[18:15] = 4’d1 (MUX_SEL_1_AON_FSM_ST)
    //     MPM_WCSSAON_CONFIG = (MPM_WCSSAON_CONFIG & 0xFFFE_ FF3F) | 0x0000_8000;
    //        Write MPM_WCSSAON_CONFIG = (MPM_WCSSAON_CONFIG & 0xFFF8_ 7F3F) | 0x0000_0000;
    //                                                                  ~0x78C0
    HWIO_OUTM (MPM_WCSSAON_CONFIG, WLAN_MASK, 0x0000);

    // 6. WAIT for 3 sleep clocks for the changes for deasserting MSM clamp overrides and going back to MUX_SEL_1_AON_FSM_ST to get affected.  (100us)
    busywait (WLAN_CONFIG_CHANGE_DELAY);

    for (i = 0; i < MAX_WAKE_TRIES; i++)
    {
      //7. Read the WCSSAON_MPM_STATUS register up to 5 times to check that bits 1:0 are one-hot. This indicates mux select has changed
      //   Low Bits become 00
      if (wlan_status_read(1, &wlan_status))
      {
        if ((wlan_status & 0x03) == 0)
        {
          break;
        }
      }
      // 8. If a 00 is not found (Step 7) in bits WCSSAON_MPM_STATUS[1:0], repeat step #7,
      //     for another 3 times at 50us interval, and declare error, if not still matching
      //     (AOSS can choose to poll continuously)
      busywait (WLAN_STATUS_POLL_INTERVAL);
    }
    if (i >= MAX_WAKE_TRIES)
    {
      wlan_stats->stats[5]++;
      log_wlan_info(5, wlan_status, 0);
#ifdef WLAN_WA_ABORT
      wlan_stats->stats[5] |= 0x80000000;
      abort();
#endif
    }

    HWIO_OUTM (MPM_WCSSAON_CONFIG, WLAN_MASK, WLAN_VAL_SM_MODE);

    // 9. Once a 00 is seen in step #7, in another 50us, we assume msm_clamp_en_ovrd/ovrd_val to get deasserted. 
    // busywait (WLAN_STATUS_POLL_INTERVAL);

    end = time_service_now();
    save_wlan_stats(1, end - start);
  }
}
