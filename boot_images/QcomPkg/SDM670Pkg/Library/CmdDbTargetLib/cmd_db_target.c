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
#include <Library/CmdDbLib.h>
//#include "msmhwiobase.h"

#define AOP_SS_MSG_RAM_START_ADDRESS_BASE                           0x0c300000
#define AOP_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00100000

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define AOP_MSG_RAM_END (AOP_SS_MSG_RAM_START_ADDRESS_BASE + \
                         AOP_SS_MSG_RAM_START_ADDRESS_BASE_SIZE)
/* MSG_RAM_END - 64K + cmd_db_data_offset field */
#define CMD_DB_AOP_DATA_ADDR (AOP_MSG_RAM_END - 0x10000 + 0xC)
#define CMD_DB_AOP_DATA_SZ (AOP_MSG_RAM_END - 0x10000 + 0x10)


//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Exeternal Declarations
//---------------------------------------------------------------------------
char *cmd_db_addr = (char *)NULL;

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
  cmd_db_addr = (char *)(uint64_t)(*((uint32_t *)CMD_DB_AOP_DATA_ADDR));
  return (char *)cmd_db_addr;
}
