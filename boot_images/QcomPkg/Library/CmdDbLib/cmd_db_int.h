#ifndef CMD_DB_INT_H
#define CMD_DB_INT_H
/*===========================================================================
                             cmd_db_int.h

DESCRIPTION:
  Internal structure of the command DB data.

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
#include <Library/cmd_db.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define CMD_DB_VER   0x00000001
#define CMD_DB_MAGIC_NUM 0x0c0330db

#define CMD_DB_SLV_ID_ACTUAL 3
#define CMD_DB_SLV_ID_RESERVE 5
#define CMD_DB_MAX_SLV_ID (CMD_DB_SLV_ID_ACTUAL+CMD_DB_SLV_ID_RESERVE)

#define CMD_DB_MAX_HW_ID_ENTRIES   100
#define CMD_DB_SIZE                8192
#define CMD_DB_MAX_DATA_ENTRIES    64
#define CMD_DB_MAX_DATA_ENTRY_SIZE 32
#define CMD_DB_MAX_DATA_SIZE (CMD_DB_MAX_DATA_ENTRIES * CMD_DB_MAX_DATA_ENTRY_SIZE)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/* CMD DB SMEM entries Do not change ordering without thinking about alignment issues
   uint64_t should be kept at an 8 byte alignment address */
typedef struct {
  uint64_t res_id;
  uint32_t priority[CMD_DB_DRV_ID_PRIORITY_SZ];
  uint32_t addr;
  uint16_t len;
  uint16_t offset;
} cmd_db_entry_header;

/* Internal DB per SLV_ID Type Header */
typedef struct {
  uint16_t        slv_id;
  uint16_t        header_offset;  /* Entry header offset from data  */
  uint16_t        data_offset;    /* Entry offset for data location */
  uint16_t        cnt;            /* Number of entries for HW type */
  uint16_t        version;        /* MSB is Major Version and LSB is Minor version
                                     Identifies the HW type of Aux Data */
  uint16_t        reserved[3];
} cmd_db_slv_id_info_type;

/* Internal DB Header */
typedef struct
{
  uint32_t                version;
  uint32_t                magic_num;
  cmd_db_slv_id_info_type slv_id_info[CMD_DB_MAX_SLV_ID];
  uint32_t                check_sum;
  uint32_t                reserved;
  uint8_t data[];
} cmd_db_header_type;

typedef struct {
  uint8_t *start;
  uint8_t *end;
} cmd_db_data_info_type;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
extern char* cmd_db_addr;

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- cmd_db_conv_str_to_unit64 -->
 *
 * @brief Copy res_id string to uint64_t internal entry
 *
 *  @param res_id : resource id string.
 *
 *  @return uint64_t version of string.
 *
 */
uint64_t cmd_db_conv_str_to_unit64(const char *res_id);

#endif // CMD_DB_INT_H
