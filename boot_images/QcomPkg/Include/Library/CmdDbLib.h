#ifndef __CMD_DB_LIB_H__
#define __CMD_DB_LIB_H__
/*============================================================================
  @file CmdDbLib.h

  CmdDb library API.

  This file contains init api for CmdDb.

               Copyright (c) 2016 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include <Library/cmd_db.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
 * <!-- cmd_db_pre_db_access -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_prep_access(void);

/**
 * <!-- cmd_db_access_done -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_access_done(void);

/**
 * <!-- cmd_db_target_mem_init -->
 *
 * @brief Return target address for Command DB data.
 *
 *  @return Command DB data address for success, NULL for
 *          failure.
 *
*/
char *cmd_db_target_mem_init(void);

/**
 * <!-- cmd_db_init -->
 *
 * @brief Init command DB.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_init(void);

#endif /* #ifndef __CMD_DB_LIB_H__ */

