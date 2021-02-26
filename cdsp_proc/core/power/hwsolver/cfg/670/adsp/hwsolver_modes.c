/*============================================================================
@file hwsolver_modes.c

HW Solver modes, timeslots and priority tables.

Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
#include "hwsolver.h"
#include "pdc_config.h"
#include "rsc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
 
hwsolver_sleep_mode hwsolver_sleep_modes[] = 
{
  { "sdc_clkgate",
    SSC_LPASS_RSCP_EXIT_SLEEP,            /* RSC sequence start addr */
    0x0,  /* PDC sequence start addr; only meaningful if next field is 1 */
    0,    /* PDC sequence valid */
    0,    /* PDC timer valid */
    0x50, /* RSC sequence backoff */
    0x70, /* PDC sequence backoff */
    10    /* Enter+exit latency */
  },
  { "cx_collapse_w_timer",
    SSC_LPASS_RSCP_LONG_SEQ,              /* RSC sequence start addr */
    SSC_PDC_SSC_GDSC_AOSS,          /* PDC sequence start addr; only meaningful if next field is 1 */
    1,    /* PDC sequence valid */
    1,    /* PDC timer valid */
    250,  /* RSC sequence backoff */
    300,  /* PDC sequence backoff */
    9900  /* Enter+exit latency */
  },
  { "cx_collapse_wo_timer",
    SSC_LPASS_RSCP_LONG_SEQ,              /* RSC sequence start addr */
    SSC_PDC_SSC_GDSC_AOSS_NOTIMER,  /* PDC sequence start addr; only meaningful if next field is 1 */
    1,    /* PDC sequence valid */
    0,    /* PDC timer valid */
    250,  /* RSC sequence backoff */
    300,  /* PDC sequence backoff */
    9900  /* Enter+exit latency */
  }    
};

unsigned int hwsolver_num_sleep_modes = 
  sizeof(hwsolver_sleep_modes)/sizeof(hwsolver_sleep_mode);

hwsolver_timeslot_table hwsolver_timeslot_tables[] =
{
  { 0,
    { 0, 0, 0, 0 },
    4
  },
  { 19200, // TODO: don't need this timeslot at all
    { 0, 0, 0, 0 },
    4
  },
  { 179791,
    {0, 0, 0, 1 },
    4
  },
  { 0xfffffffe,
    {0, 0, 0, 2 },
    4
  },
};

unsigned int hwsolver_num_timeslot_tables = 
  sizeof(hwsolver_timeslot_tables)/sizeof(hwsolver_timeslot_table);
  
#ifdef __cplusplus
}
#endif
