#ifndef CMD_DB_H
#define CMD_DB_H
/*===========================================================================
                             cmd_db.h

DESCRIPTION:
  Public API to command DB.  Provides query functionality into command DB.

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
#include <stdint.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define CMD_DB_MAX_RES_ID_LEN 8
#define CMD_DB_DRV_ID_PRIORITY_SZ  2

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef enum {
  CMD_DB_SUCCESS = 0,
  CMD_DB_ERR_FAIL,
  CMD_DB_ERR_INVALID_PARAM,
  CMD_DB_ERR_NOT_FOUND,
} cmd_db_err_type;

typedef enum {
  CMD_DB_SLV_ID_INVALID    = 0,
  CMD_DB_SLV_ID_VALID_LOW  = 3,
  CMD_DB_SLV_ID_ARC        = CMD_DB_SLV_ID_VALID_LOW,
  CMD_DB_SLV_ID_VRM        = 4,
  CMD_DB_SLV_ID_BCM        = 5,
  CMD_DB_SLV_ID_VALID_HIGH = CMD_DB_SLV_ID_BCM,
  CMD_DB_SLV_ID_MAX        = 0x7ffffff,
} cmd_db_slv_id_type;

typedef struct {
  char     res_id[CMD_DB_MAX_RES_ID_LEN+1];     /* Resource Identifer */
  uint32_t addr;                                /* TCS Addr Slave ID + Offset address */
  uint32_t priority[CMD_DB_DRV_ID_PRIORITY_SZ]; /* 2-bits per DRV ID variant 0-31 */
  uint32_t len;                                 /* Aux data len */
  uint16_t version;                             /* MSB is Major Version and LSB is Minor version
                                                   Identifies the slv id type of aux data version */
} cmd_db_query_result_type;

/* CMD DB QUERY TYPES */
typedef enum {
  CMD_DB_QUERY_RES_ID = 0,
  CMD_DB_QUERY_ADDRESS,
  CMD_DB_QUERY_INVALID,
  CMD_DB_QUERY_MAX = 0x7ffffff,
} cmd_db_query_type;

typedef struct {
  char               res_id[CMD_DB_MAX_RES_ID_LEN+1]; /* Resource identifer */
  uint32_t           addr;                            /* Look up via TCS address */
  cmd_db_query_type  type;
  cmd_db_slv_id_type slv_id;                          /* Accelerate look up */
} cmd_db_query_info;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- cmd_db_query -->
 *
 * @brief Query command db API.
 *
 *  This is used to retrieve a command DB entry based on query info.
 *
 *  @param query_info : Provide information to be used for query.
 *  @param query_result : Query entry result info.
 *  @param data : Client address to return entry data.  NULL
 *              okay if entry->len == 0.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_query(cmd_db_query_info        *query_info,
                             cmd_db_query_result_type *query_result,
                             uint8_t                  *data);

/**
 * <!-- cmd_db_query_addr -->
 *
 * @brief Query command db for resource id address.
 *
 *  This is used to retrieve resource address based on resource
 *  id.
 *
 *  @param res_id : resource id to query for address
 *
 *  @return 0 for failure, non-zero for offset address
 */
uint32_t cmd_db_query_addr(const char *res_id);

/**
 * <!-- cmd_db_query_priority -->
 *
 * @brief Query command db for resource address priority from
 *        command DB.
 *
 *  This is used to retrieve a command DB entry based resource address.
 *
 *  @param addr : resource addr to query for priority.
 *  @param drv_id : DRV ID to query resource for priority on.
 *
 *  @return priority value.
 */
uint32_t cmd_db_query_priority(uint32_t addr, uint8_t drv_id);

/**
 * <!-- cmd_db_query_priority_res_id -->
 *
 * @brief Query command db for resource id priority from
 *        command DB.
 *
 *  This is used to retrieve a command DB entry based resource
 *  id.
 *
 *  @param res_id : resource id to query for priority.
 *  @param drv_id : DRV ID to query resource for priority on.
 *
 *  @return priority value.
 */
uint32_t cmd_db_query_priority_res_id(const char *res_id, uint8_t drv_id);

/**
 * <!-- cmd_db_query_version -->
 *
 * @brief Query command db for resource id address.aux data
 *        version.
 *
 *  This is used to retrieve associated resource id aux data
 *  version. Version will be common for all resources of same
 *  cmd_db_slv_id_type. This is used to retrieve associated
 *  resource id auxillary data len.
 *
 *  @param res_id : resource id to query for address
 *
 *  @return 0 for failure, non-zero for version
 */
uint16_t cmd_db_query_version(const char *res_id);

/**
 * <!-- cmd_db_query_len -->
 *
 * @brief Query command db for resource id address.aux data len
 *
 *  This is used to retrieve associated resource id auxillary
 *  data len.
 *
 *  @param res_id : resource id to query for address
 *
 *  @return 0 for failure, non-zero for length address
 */
uint32_t cmd_db_query_len(const char *res_id);

/**
 * <!-- cmd_db_query_aux_data -->
 *
 * @brief Query command db for aux data.
 *
 *  This is used to retrieve a command DB entry based resource address.
 *
 *  @param res_id : Resource to retrive AUX Data on.
 *  @param len : Caller provides size of data buffer passed in.
 *             Returns size of data copied to data buffer.
 *  @param data : Data buffer to copy returned aux data to.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_query_aux_data (const char *res_id,
                                       uint8_t *len, uint8_t *data);

/**
 * <!-- cmd_db_query_slv_id -->
 *
 * @brief Query command db for resource id address.slv_id type
 *
 *  This is used to retrieve associated resource id slv_id type.
 *
 *  @param res_id : resource id to query for address
 *
 *  @return CMD_DB_SLV_ID_INVALID for failure, otherwise slv_id
 *          type.
 */
cmd_db_slv_id_type cmd_db_query_slv_id(const char *res_id);

#endif // CMD_DB_H
