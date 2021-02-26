/*===========================================================================
                              arc_internal.c

DESCRIPTION:
  This file contains various variables, functions and utilities that are
  used internally within ARC driver.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include "arc.h"
#include "arc_internal.h"
#include "cmd_db.h"
#include "CoreVerify.h"

//===========================================================================
//                       Function Defintions
//===========================================================================

/**
 * <!-- arc_internal_init -->
 *
 * @brief This function performs any internal initialization ARC driver may
 *        have outside of rinit.
 */
void arc_internal_init (void)
{
  int8 i;
  uint32 addr;
  arc_rm_addr_map_t *entry;

  /* Populate the ARC resource to address mapping list by querying 
   * command db */
  for (i = 0; i < g_arc_rm_addr_map_size; i++)
  {
    entry = &g_arc_rm_addr_map[i];
    entry->addr = cmd_db_query_addr (entry->rm);
    CORE_VERIFY (entry->addr != 0);
  }

  return;
}
