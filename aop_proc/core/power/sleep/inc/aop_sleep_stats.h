#ifndef AOP_SLEEP_STATS_H
#define AOP_SLEEP_STATS_H
/*===========================================================================
                             aop_sleep_stats.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "msg_ram_layout.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
typedef enum aop_sleep_stats_enum_t
{
  AOP_SLEEP_STATS_AOSD = 0, // AOSS Sleep
  AOP_SLEEP_STATS_CXSD = 1, // CX Collapse
  AOP_SLEEP_STATS_LAST = AOP_SLEEP_STATS_CXSD,
  
  AOP_SLEEP_STATS_NUM_STATS = AOP_SLEEP_STATS_LAST + 1
} aop_sleep_stats_enum_t;

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================
/**
 * <!-- aop_sleep_stats_init -->
 *
 * @brief Initializes the sleep stats area
 *
 * @return N/A
 */
void aop_sleep_stats_init (void);

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
void aop_sleep_stats_enter (aop_sleep_stats_enum_t stat);

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
void aop_sleep_stats_exit (aop_sleep_stats_enum_t stat);

#endif // AOP_SLEEP_STATS_H
