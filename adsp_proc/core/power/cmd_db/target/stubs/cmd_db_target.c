/*===========================================================================
                             cmd_db_target.c

DESCRIPTION:
  Target specific functions for command DB in client image.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//------------------- --------------------------------------------------------
#include "cmd_db_int.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Exeternal Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================
/**
 * <!-- cmd_db_pre_db_access -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_prep_access(void)
{
  return;
}

/**
 * <!-- cmd_db_access_done -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_access_done(void)
{
  return;
}

/**
 * <!-- cmd_db_target_mem_init -->
 *
 * @brief Return target address for Command DB data.
 *
 *  @return Command DB data address for success, NULL for
 *          failure.
 *
 */
char *cmd_db_target_mem_init(void)
{
  char* ret_addr = NULL;
  return ret_addr;
}
