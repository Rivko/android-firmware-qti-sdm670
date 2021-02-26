#ifndef CMD_DB_AOP_INT_H
#define CMD_DB_AOP_INT_H
/*===========================================================================
                             cmd_db_aop_int.h

DESCRIPTION:
  Internal structure of the command DB aop data.

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
#include "cmd_db.h"
#include "cmd_db_aop.h"
//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

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
 * <!-- cmd_db_target_aop_mem_init -->
 *
 * @brief Return target address for Command DB AOP data.
 *
 *  @return Command DB data address for success, NULL for
 *          failure.
 *
 */
char *cmd_db_target_aop_mem_init(void);

/**
 * <!-- cmd_db_aop_target_init -->
 *
 * @brief Init command DB aop target.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_aop_target_init(void);
#endif // CMD_DB_AOP_INT_H
