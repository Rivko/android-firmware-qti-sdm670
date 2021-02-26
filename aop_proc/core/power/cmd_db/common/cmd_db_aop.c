/*===========================================================================
                             cmd_db_aop.c

DESCRIPTION:
  Special Public API to command DB in AOP image.
  Provides special API functionality into command DB to add entries.

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
#include "cmd_db_int.h"
#include "cmd_db_aop.h"
#include "cmd_db_aop_int.h"
#include "msg_ram_layout.h"
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define CMD_DB_DATA_INFO_SIZE(_data_) ((uint16_t)(_data_.end - _data_.start))
#define CMD_DB_AOP_ALIGN8(_addr_) (((_addr_) + 0x7) & (~0x7))

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
static cmd_db_data_info_type dyn_hw_id_hdr_cache = {0};
static cmd_db_data_info_type dyn_hw_id_data = {0};
static cmd_db_slv_id_info_type active_slv_id_info = {0};

cmd_db_header_type *smem_db = NULL;
cmd_db_data_info_type smem_data = {0};

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- cmd_db_aop_update_msg_ram_info -->
 *
 * @brief Set CMD DB info in MSG RAM INFO
 *
 * This function should be called after all static and dynamic entries
 * have been added.
 *
 */
void cmd_db_aop_update_msg_ram_info(void)
{
  uint32 data[MSG_RAM_CMD_DB_DATA_SIZE];

  data[0] = (uint32_t)cmd_db_addr;
  data[1] = (uint32_t)smem_data.end - (uint32_t)smem_db;
  msg_ram_cmd_db_set_init(data);
}

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
                                       const uint8_t *data, uint16_t data_size)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_slv_id_info_type *slv_id_hdr = NULL;
  uint8_t i;
  uint16_t total_data_size = 0;
  uint16_t header_data_size = 0;

  do
  {
    if ((entry == NULL) || (data == NULL))
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    header_data_size = (uint16_t)(entry_cnt * sizeof(cmd_db_entry_header));
    total_data_size = CMD_DB_AOP_ALIGN8(data_size + header_data_size);

    /* Validate additional data will fit*/
    if (((uint32_t)(CMD_DB_DATA_INFO_SIZE(smem_data) + total_data_size)) >=
        (CMD_DB_SIZE - (uint32_t)CMD_DB_AOP_ALIGN8(sizeof(cmd_db_header_type))))
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    /* Accessing CMD DB region */
    cmd_db_prep_access();

    /* Find a free entry */
    for (i = 0; i < CMD_DB_MAX_SLV_ID; i++)
    {
      if (smem_db->slv_id_info[i].slv_id == 0)
      {
        slv_id_hdr = &(smem_db->slv_id_info[i]);
        break;
      }
    }

    if (i >= CMD_DB_MAX_SLV_ID)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    slv_id_hdr->slv_id = slv_id;
    slv_id_hdr->cnt = entry_cnt;
    slv_id_hdr->version = version;
    slv_id_hdr->header_offset = (uint16_t)CMD_DB_AOP_ALIGN8(CMD_DB_DATA_INFO_SIZE(smem_data));
    slv_id_hdr->data_offset = slv_id_hdr->header_offset + header_data_size;
    smem_data.end += total_data_size;

    /* Copy entry headers */
    memcpy((smem_data.start + slv_id_hdr->header_offset), entry, header_data_size);
    /* Copy data */
    memcpy((smem_data.start + slv_id_hdr->data_offset), data, data_size);
  } while (0);
  /* Done accessing CMD DB region */
  cmd_db_access_done();

  return err;
}

/**
 * <!-- cmd_db_add_entry_init -->
 *
 * @brief Prepare command DB to recieve entries for specified HW ID
 *
 *  This is used to prepare command DB to recieve entries and provides
 *  version info for all entries of the provided slv_id.
 *
 *  @param slv_id : Only CMD_DB_SLV_ID_VRM is valid at this time.
 *  @param version : MSB is major version and LSB is minor version.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_entry_init(cmd_db_slv_id_type slv_id, uint16_t version)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;

  if (slv_id != CMD_DB_SLV_ID_VRM)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  memset(&dyn_hw_id_hdr_cache, 0x0, sizeof(cmd_db_data_info_type));
  memset(&dyn_hw_id_data, 0x0, sizeof(cmd_db_data_info_type));
  memset(&active_slv_id_info, 0x0, sizeof(cmd_db_slv_id_type));

  do
  {
    dyn_hw_id_hdr_cache.start = malloc(sizeof(cmd_db_entry_header) * CMD_DB_MAX_HW_ID_ENTRIES);

    if (dyn_hw_id_hdr_cache.start == NULL)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    dyn_hw_id_data.start = malloc(CMD_DB_MAX_DATA_SIZE);
    if (dyn_hw_id_data.start == NULL)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    dyn_hw_id_hdr_cache.end = dyn_hw_id_hdr_cache.start;
    dyn_hw_id_data.end = dyn_hw_id_data.start;

    active_slv_id_info.slv_id = (uint16_t)slv_id;
    active_slv_id_info.version = version;
  } while (0);

  if (err != CMD_DB_SUCCESS)
  {
    if (dyn_hw_id_hdr_cache.start != NULL)
    {
      free(dyn_hw_id_hdr_cache.start);
      memset(&dyn_hw_id_hdr_cache, 0x0, sizeof(cmd_db_data_info_type));
    }
    if (dyn_hw_id_data.start != NULL)
    {
      free(dyn_hw_id_data.start);
      memset(&dyn_hw_id_data, 0x0, sizeof(cmd_db_data_info_type));
    }
  }
  return err;
}

/**
 * <!-- cmd_db_add_entry -->
 *
 * @brief Add entry to command DB.
 *
 *  This is used to an entry to command DB run-time.
 *
 *  @param slv_id : Only CMD_DB_SLV_ID_VRM is valid at this time.
 *  @param entry : Entry to be added.
 *
 *  @return 0 for success, non-zero for error
 */
cmd_db_err_type cmd_db_add_entry(cmd_db_slv_id_type     slv_id,
                                 cmd_db_add_entry_type *entry,
                                 uint8_t               *data)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_entry_header *db_entry_header = NULL;

  if (slv_id != CMD_DB_SLV_ID_VRM)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  do
  {
    if ((uint16_t)slv_id != active_slv_id_info.slv_id)
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    if (entry->len > CMD_DB_MAX_DATA_ENTRY_SIZE)
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    /* Check if entry addition will exceed memory */
    if (active_slv_id_info.cnt >= CMD_DB_MAX_HW_ID_ENTRIES)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    if ((entry->len + dyn_hw_id_data.end) > (dyn_hw_id_data.start + CMD_DB_MAX_DATA_SIZE))
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    if ((entry->len > 0) && (data == NULL))
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    /* Copy over to internal entry */
    db_entry_header = (cmd_db_entry_header *)dyn_hw_id_hdr_cache.end;
    db_entry_header->len = entry->len;
    db_entry_header->addr = entry->addr;
    db_entry_header->priority[0] = entry->priority[0];
    db_entry_header->priority[1] = entry->priority[1];
    db_entry_header->addr = entry->addr;
    db_entry_header->res_id = cmd_db_conv_str_to_unit64(entry->res_id);
    dyn_hw_id_hdr_cache.end += sizeof(cmd_db_entry_header);
    active_slv_id_info.cnt++;

    /* Byte order copy data */
    if (entry->len > 0)
    {
      db_entry_header->offset = dyn_hw_id_data.end - dyn_hw_id_data.start;
    }

    memcpy(dyn_hw_id_data.end, data, entry->len);
    dyn_hw_id_data.end += entry->len;
  } while (0);

  return err;
}

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
cmd_db_err_type cmd_db_add_entry_complete(cmd_db_slv_id_type slv_id)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;

  if (slv_id != CMD_DB_SLV_ID_VRM)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  err  = cmd_db_add_static_data(slv_id, active_slv_id_info.version,
                                (const cmd_db_entry_header *)dyn_hw_id_hdr_cache.start,
                                active_slv_id_info.cnt, dyn_hw_id_data.start,
                                CMD_DB_DATA_INFO_SIZE(dyn_hw_id_data));

  free(dyn_hw_id_hdr_cache.start);
  free(dyn_hw_id_data.start);
  memset(&dyn_hw_id_hdr_cache, 0x0, sizeof(cmd_db_data_info_type));
  memset(&dyn_hw_id_data, 0x0, sizeof(cmd_db_data_info_type));

  return err;
}

/**
 * <!-- cmd_db_aop_init -->
 *
 * @brief Init command DB from AOP.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_aop_init(void)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  smem_db = (cmd_db_header_type *)cmd_db_target_aop_mem_init();

  do
  {
    if (smem_db == NULL)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    /* Accessing CMD DB region */
    cmd_db_prep_access();

    /* Initialize header */
    smem_db->version = CMD_DB_VER;
    smem_db->magic_num = CMD_DB_MAGIC_NUM;

    smem_data.start = (uint8_t *)CMD_DB_AOP_ALIGN8((uint32_t)smem_db->data);
    smem_data.end   = smem_data.start;
    /* Done accessing CMD DB region */
    cmd_db_access_done();

    /* Add static target data */
    err = cmd_db_aop_target_init();
  } while (0);

  return err;
}
