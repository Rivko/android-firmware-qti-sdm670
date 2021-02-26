/*===========================================================================
                             cmd_db.c

DESCRIPTION:
  Initialize and query support for command DB.

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
#include <Library/cmd_db.h>
#include <Library/CmdDbLib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

#define ADDR_SLV_ID(id) ((cmd_db_slv_id_type)(((id) >> 16)&0xf))

#define CMD_DB_TEST_MODE

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
cmd_db_header_type *smem_query_db = NULL;

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
uint64_t cmd_db_conv_str_to_unit64(const char *res_id)
{
  uint64_t ret = 0;
  uint8_t  i;

  for (i = 0; i < 8; i++)
  {
    if (res_id[i] == '\0')
    {
      break;
    }
    ret |= ((uint64_t)res_id[i])<<(8*i);
  }
  return ret;
}

/**
 * <!-- cmd_db_validate_addr_slv_id -->
 *
 * @brief Quick validate of addr based on known bad values.
 *
 *  Validate address slv_id field is valid.
 *
 *  @param slv_id : Slave id to validate.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
static cmd_db_err_type cmd_db_validate_slv_id(uint32_t slv_id)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  if ((slv_id < CMD_DB_SLV_ID_VALID_LOW) ||
      (slv_id > CMD_DB_SLV_ID_VALID_HIGH))
  {
    err = CMD_DB_ERR_FAIL;
  }
  return err;
}

/**
 * <!-- cmd_db_query -->
 *
 * @brief Query command db API.
 *
 *  This is used to retrieve a command DB entry based on query info.
 *
 *  @param query_info : Provide information to be used for query.
 *  @param query_result : Query entry result info. Caller to set
 *                      entry->len > 0 if expecting entry data
 *                      returned.
 *  @param data : Client address to return entry data.  NULL
 *              okay if entry->len == 0.  If data == NULL,
 *              caller can expect entry->len to be > 0 to
 *              discover size of data stored.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_query(cmd_db_query_info        *query_info,
                             cmd_db_query_result_type *query_result,
                             uint8_t                  *data)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  uint64_t res_id;
  uint16_t slv_id_idx;
  uint16_t entry_idx;
  uint8_t found = 0;
  cmd_db_entry_header *entry = NULL;
  cmd_db_err_type valid_slv_id_hint;

  do
  {
    if (smem_query_db == NULL)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    if ((query_info == NULL) || (query_result == NULL))
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    if ((data == NULL) && (query_result->len > 0))
    {
      err = CMD_DB_ERR_INVALID_PARAM;
      break;
    }

    switch (query_info->type)
    {
      case CMD_DB_QUERY_RES_ID:
        res_id = cmd_db_conv_str_to_unit64(query_info->res_id);
        break;
      case CMD_DB_QUERY_ADDRESS:
        err = cmd_db_validate_slv_id(ADDR_SLV_ID(query_info->addr));
        if (err == CMD_DB_SUCCESS)
        {
          /* Use slv_id to speed up query */
          query_info->slv_id = ADDR_SLV_ID(query_info->addr);
        }
        break;
      default:
        err = CMD_DB_ERR_FAIL;
        break;
    }
    if (err != CMD_DB_SUCCESS)
    {
      break;
    }

    valid_slv_id_hint = cmd_db_validate_slv_id(query_info->slv_id);

    /* Accessing CMD DB region */
    cmd_db_prep_access();
    /* Iterate through entries until match found. */
    for (slv_id_idx = 0; slv_id_idx < CMD_DB_MAX_SLV_ID; slv_id_idx++)
    {
      /* Skip over other slv_id's if valid hint was given */
      if ((valid_slv_id_hint == CMD_DB_SUCCESS) &&
          (smem_query_db->slv_id_info[slv_id_idx].slv_id != query_info->slv_id))
      {
        continue;
      }

      for (entry_idx = 0; entry_idx < smem_query_db->slv_id_info[slv_id_idx].cnt; entry_idx++)
      {
        /* Look for a matching entry based on query type */
        entry =
          (cmd_db_entry_header *)(smem_query_db->data +
                                  smem_query_db->slv_id_info[slv_id_idx].header_offset +
                                  (sizeof(cmd_db_entry_header) * entry_idx));
        if ((query_info->type == CMD_DB_QUERY_RES_ID) &&
            (res_id == entry->res_id))
        {
          found = 1;
          break;
        }
        else if ((query_info->type == CMD_DB_QUERY_ADDRESS)&&
                 (query_info->addr == entry->addr))
        {
          found = 1;
          break;
        }
      }
      if (found == 1)
      {
        break;
      }
    }
    if (found == 0)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    /* Transfer data over to return */
    strlcpy(query_result->res_id, query_info->res_id,
            sizeof(query_result->res_id));
    query_result->addr = entry->addr;
    query_result->priority[0] = entry->priority[0];
    query_result->priority[1] = entry->priority[1];
    query_result->version = smem_query_db->slv_id_info[slv_id_idx].version;

    /* Figure out how much data to copy back. */
    if ((query_result->len > 0) && (entry->len > 0))
    {
      /* Copy back as much as client asks for this is to account for minor version updates
         where extra data might not be handled by all client images */
      uint32_t cpy_len = (query_result->len > entry->len)?(entry->len):(query_result->len);
      memcpy(data,
             (smem_query_db->data + smem_query_db->slv_id_info[slv_id_idx].data_offset + entry->offset),
             cpy_len);
      query_result->len = cpy_len;
    }
    else if (entry->len > 0)
    {
      /* Return length back but no data */
      query_result->len = entry->len;
    }
  } while (0);
  /* Done accessing CMD DB region */
  cmd_db_access_done();

  return err;
}

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
uint32_t cmd_db_query_addr(const char *res_id)
{
  uint32_t addr = 0;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};

  if (res_id == NULL)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));

  if (cmd_db_query(&query, &query_result, NULL) == CMD_DB_SUCCESS)
  {
    addr = query_result.addr;
  }

  return addr;
}

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
uint32_t cmd_db_query_priority(uint32_t addr, uint8_t drv_id)
{
  uint32_t ret_val = 0;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};

  query.type = CMD_DB_QUERY_ADDRESS;
  query.addr  = addr;

  if (cmd_db_query(&query, &query_result, NULL) == CMD_DB_SUCCESS)
  {
    ret_val = ((query_result.priority[0]>>drv_id) & 0x1) +
      ((((query_result.priority[1]>>drv_id) & 0x1))<<1);
  }

  return ret_val;
}

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
uint32_t cmd_db_query_priority_res_id(const char *res_id, uint8_t drv_id)
{
  uint32_t ret_val = 0;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};

  if (res_id == NULL)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));

  if (cmd_db_query(&query, &query_result, NULL) == CMD_DB_SUCCESS)
  {
    ret_val = ((query_result.priority[0]>>drv_id) & 0x1) +
      ((((query_result.priority[1]>>drv_id) & 0x1))<<1);
  }

  return ret_val;
}

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
uint16_t cmd_db_query_version(const char *res_id)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};
  uint16_t version = 0;

  if (res_id == NULL)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));

  err = cmd_db_query(&query, &query_result, NULL);
  if (err == CMD_DB_SUCCESS)
  {
    version = query_result.version;
  }

  return version;
}

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
 *  @return 0 for failure, non-zero for lenght address
 */
uint32_t cmd_db_query_len(const char *res_id)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};
  uint32_t len = 0;

  if (res_id == NULL)
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));

  err = cmd_db_query(&query, &query_result, NULL);
  if (err == CMD_DB_SUCCESS)
  {
    len = query_result.len;
  }

  return len;
}

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
                                       uint8_t *len, uint8_t *data)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};

  if ((res_id == NULL) || (data == NULL) || (len == NULL))
  {
    return CMD_DB_ERR_INVALID_PARAM;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));
  query_result.len = *len;

  err = cmd_db_query(&query, &query_result, data);
  if (err == CMD_DB_SUCCESS)
  {
    *len = query_result.len;
  }

  return err;
}

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
cmd_db_slv_id_type cmd_db_query_slv_id(const char *res_id)
{
  cmd_db_slv_id_type ret_slv_id = CMD_DB_SLV_ID_INVALID;
  cmd_db_err_type err = CMD_DB_SUCCESS;
  cmd_db_query_info query = {0};
  cmd_db_query_result_type query_result = {0};

  if (res_id == NULL)
  {
    return CMD_DB_SLV_ID_INVALID;
  }

  query.type = CMD_DB_QUERY_RES_ID;
  strlcpy(query.res_id, res_id, sizeof(query.res_id));

  err = cmd_db_query(&query, &query_result, NULL);
  if (err == CMD_DB_SUCCESS)
  {
    ret_slv_id = ADDR_SLV_ID(query_result.addr);
  }

  return ret_slv_id;
}

#define CMD_DB_TEST_MODE

#ifdef CMD_DB_TEST_MODE
uint32_t cmd_db_test_addr[2] = {0};
uint16_t cmd_db_test_data[16] = {0};
uint32_t cmd_db_test_priority[4] = {0};

static void cmd_db_test(void)
{
  uint8_t len = sizeof(cmd_db_test_data);

  cmd_db_test_addr[0] = cmd_db_query_addr("MC0");
  cmd_db_test_addr[1] = cmd_db_query_addr("mss.lvl");

  cmd_db_query_aux_data("mss.lvl", &len, (uint8_t*)cmd_db_test_data);

  cmd_db_test_priority[0] = cmd_db_query_priority(cmd_db_test_addr[0], 0);
  cmd_db_test_priority[1] = cmd_db_query_priority(cmd_db_test_addr[1], 1);
  cmd_db_test_priority[2] = cmd_db_query_priority_res_id("cx.lvl", 0);
  cmd_db_test_priority[3] = cmd_db_query_priority_res_id("cx.tmr", 1);
}

#endif

/**
 * <!-- cmd_db_init -->
 *
 * @brief Init command DB.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_init(void)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;

  smem_query_db = (cmd_db_header_type *)cmd_db_target_mem_init();

  do
  {
    if (smem_query_db == NULL)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    /* Accessing CMD DB region */
    cmd_db_prep_access();

    /* Validate header */
    if (smem_query_db->version != CMD_DB_VER)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }

    if (smem_query_db->magic_num != CMD_DB_MAGIC_NUM)
    {
      err = CMD_DB_ERR_FAIL;
      break;
    }
  } while (0);
  /* Done accessing CMD DB region */
  cmd_db_access_done();

  if (err != CMD_DB_SUCCESS)
  {
    smem_query_db = NULL;
  }

#ifdef CMD_DB_TEST_MODE
  cmd_db_test();
#endif

  return err;
}
