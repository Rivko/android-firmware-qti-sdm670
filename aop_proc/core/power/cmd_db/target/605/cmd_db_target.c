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
#include "page_select.h"

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
#ifdef CMD_DB_IN_DDR
  set_ddr_access((uint64_t)cmd_db_addr);
#endif
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
#ifdef CMD_DB_IN_DDR
  set_page_select(0);
#endif
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
  if (cmd_db_addr != (uint32_t)NULL)
  {
#ifdef CMD_DB_IN_DDR
    ret_addr = (char *)set_ddr_access((uint64_t)cmd_db_addr);
    set_page_select(0);
#else
    ret_addr = cmd_db_addr;
#endif
  }
  return ret_addr;
}
