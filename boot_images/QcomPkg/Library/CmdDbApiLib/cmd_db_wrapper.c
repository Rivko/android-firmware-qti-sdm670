/** @file npawrapper.c

  Map NPA api in common code to NPA DXE interface

  Copyright (c) 2016, Qualcomm Technologies Inc. All rights
  reserved.

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFICmdDb.h>

/*----------------------------------------------------------------------------
 * Cmd Db Client API
 * -------------------------------------------------------------------------*/

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
                             uint8_t                  *data)
{
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  cmd_db_err_type err = CMD_DB_ERR_FAIL;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->Query( cmd_db_protocol,
                              query_info,
                              query_result,
                              data,
                              &err);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
  }
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  uint32_t addr = 0;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryAddr( cmd_db_protocol,
                                  res_id,
                                  &addr);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  uint32_t priority = 0;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryPriority( cmd_db_protocol,
                                      addr,
                                      drv_id,
                                      &priority);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
  }
  return priority;
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  uint32_t priority = 0;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryPriorityResId( cmd_db_protocol,
                                           res_id,
                                           drv_id,
                                           &priority);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
  }
  return priority;
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  uint16_t version = 0;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryVersion( cmd_db_protocol,
                                     res_id,
                                     &version);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
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
 *  @return 0 for failure, non-zero for length address
 */
uint32_t cmd_db_query_len(const char *res_id)
{
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  uint32_t len = 0;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryLen( cmd_db_protocol,
                                 res_id,
                                 &len);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  cmd_db_err_type err = CMD_DB_ERR_FAIL;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QueryAuxData( cmd_db_protocol,
                                     res_id,
                                     len,
                                     data,
                                     &err);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
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
  EFI_CMD_DB_PROTOCOL * cmd_db_protocol;
  cmd_db_slv_id_type slv_id = CMD_DB_SLV_ID_INVALID;
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiCmdDbProtocolGuid, NULL, (void**)&cmd_db_protocol);
  if(status == EFI_SUCCESS)
  {
      cmd_db_protocol->QuerySlvId( cmd_db_protocol,
                                   res_id,
                                   &slv_id);
  }
  else
  {
      DEBUG ((EFI_D_ERROR, "Locate cmd_db_protocol failed:%r\n", status));
  }
  return slv_id;
}

