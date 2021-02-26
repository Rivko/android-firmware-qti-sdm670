/*===========================================================================
                              aop_sleep_stats.c

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
#include "msg_ram_layout.h"
#include "time_service.h"
#include "CoreVerify.h"
#include <string.h>
#include "aop_sleep_stats.h"
#include "comdef.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static sleep_stats_t *sleep_stats = NULL;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- clear_4 -->
 *
 * @brief Clears out the memory region. Accesses the memory as a 4 byte integer.
 *
 * @param m - the memory area
 * @param len - the length in bytes
 *
 * @return N/A
 */
static void clear_4 (void *m, uint32 len)
{
  len = (len + 3) >> 2;
  uint32 *im = (uint32 *)m;

  for (; len > 0; len --, im ++)
  {
    *im = 0;
  }
}

/**
 * <!-- aop_sleep_stats_init -->
 *
 * @brief Initializes the sleep stats area
 *
 * @return N/A
 */
void aop_sleep_stats_init (void)
{
  sleep_stats = msg_ram_get_sleep_stats_area ();
  clear_4 (sleep_stats, sizeof (*sleep_stats));
  clear_4 (sleep_stats + 1, sizeof (*sleep_stats));
  sleep_stats[0].stat_type = 'dsoa';
  sleep_stats[1].stat_type = 'dsxc';
}

/**
 * <!-- aop_sleep_stats_enter -->
 *
 * @brief Call this function when entering a particular sleep mode. Updates
 *        the 'last_entered' field.
 *
 * @param stat : which stat to update
 *
 * @return N/A
 */
void aop_sleep_stats_enter (aop_sleep_stats_enum_t stat)
{
  CORE_VERIFY (stat < AOP_SLEEP_STATS_NUM_STATS);
  
  sleep_stats[stat].last_entered = time_service_now ();
}

/**
 * <!-- aop_sleep_stats_exit -->
 *
 * @brief Call this function when exiting a particular sleep mode. Updates
 *        the 'last_exited',count, and total_duration fields.
 *
 * @param stat : which stat to update
 *
 * @return N/A
 */
void aop_sleep_stats_exit (aop_sleep_stats_enum_t stat)
{
  CORE_VERIFY (stat < AOP_SLEEP_STATS_NUM_STATS);
  
  sleep_stats[stat].count ++;
  uint64 now = time_service_now ();

  CORE_VERIFY (now >= sleep_stats[stat].last_entered);
  sleep_stats[stat].total_duration += (now - sleep_stats[stat].last_entered);
  sleep_stats[stat].last_exited = now;
}

