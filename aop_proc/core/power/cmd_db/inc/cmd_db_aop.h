#ifndef CMD_DB_AOP_H
#define CMD_DB_AOP_H
/*===========================================================================
                             cmd_db_aop.h

DESCRIPTION:
  Special Public API to command DB in AOP image.
  Provides special API functionality into command DB to add entries.
  These API's assume single threaded operation and are not multithread safe.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
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
#include "cmd_db_int.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct {
  char     res_id[CMD_DB_MAX_RES_ID_LEN+1];     /* resource identifer */
  uint32_t addr;                                /* Offset address */
  uint32_t priority[CMD_DB_DRV_ID_PRIORITY_SZ]; /* 2-bits per DRV ID variant 0-31 */
  uint32_t len;                                 /* Aux data len */
} cmd_db_add_entry_type;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- cmd_db_aop_update_msg_ram_info -->
 *
 * @brief Set CMD DB info in MSG RAM INFO
 *
 * This function should be called after all static and dynamic entries
 * been added.
 *
 */
void cmd_db_aop_update_msg_ram_info(void);

/**
 * <!-- cmd_db_add_static_data -->
 *
 * @brief Add static Command DB entries for SLV ID
 *
 *  This is used to prepare command DB to recieve entries and provides
 *  version info for all entries of the provided slv_id.
 *
 *  @param slv_id : Slave Id type.
 *  @param version : MSB is major version and LSB is minor version.
 *  @param entry : Entry header data addr.
 *  @param entry_cnt: Entry header cnt.
 *  @param data : Entry data addr.
 *  @param data_size: Entry data size.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_static_data(cmd_db_slv_id_type slv_id, uint16_t version,
                                       const cmd_db_entry_header *entry, uint16_t entry_cnt,
                                       const uint8_t *data, uint16_t data_size);

/**
 * <!-- cmd_db_add_entry_init -->
 *
 * @brief Prepare command DB to recieve entries for specified
 *        SLV ID
 *
 *  This is used to prepare command DB to recieve entries and provides
 *  version info for all entries of the provided slv_id.
 *
 *  @param slv_id : Only CMD_DB_SLV_ID_VRM is valid at this time.
 *  @param version : MSB is major version and LSB is minor version.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_entry_init(cmd_db_slv_id_type slv_id, uint16_t version);

/**
 * <!-- cmd_db_add_entry -->
 *
 * @brief Add entry to command DB.
 *
 *  This is used to an add entry to command DB run-time.
 *
 *  @param slv_id : Only CMD_DB_SLV_ID_VRM is valid at this time.
 *  @param entry : Entry to be added.
 *  @param data : Entry data to be added. NULL okay if
 *              entry->len == 0.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_entry(cmd_db_slv_id_type     slv_id,
                                 cmd_db_add_entry_type *entry,
                                 uint8_t               *data);

/**
 * <!-- cmd_db_add_entry_complete -->
 *
 * @brief Inform command DB that all entires of a certain
 *        slv_id have been added.
 *
 *  This call will flush added entries to SMEM. No entries will
 *  be accepted after this is called.
 *
 *  @param slv_id : Only CMD_DB_SLV_ID_VRM is valid at this time.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_entry_complete(cmd_db_slv_id_type slv_id);

/**
 * <!-- cmd_db_aop_init -->
 *
 * @brief Init command DB from AOP.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_aop_init(void);
#endif // CMD_DB_AOP_H
