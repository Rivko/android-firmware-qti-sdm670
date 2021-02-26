/*============================================================================
  FILE:         CmdDbDxe.c

  OVERVIEW:     Implementation of the CmdDb EFI protocol interface

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFICmdDb.h>
#include <Library/CmdDbLib.h>
#include <Library/cmd_db.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
/**
 * <!-- EFI_cmd_db_query -->
 *
 * @brief Query command db API.
 *
 * This is used to retrieve a command DB entry based on query
 * info.
 *
 * @param[in]     This         : Pointer to the
 *                               EFI_CMD_DB_PROTOCOL instance.
 * @param[in]     query_info   : Provide information to be used
 *                               for query.
 * @param[in/out] query_result : Query entry result info.
 * @param[out]    data         : Client address to return
 *                               entry data.  NULL okay if
 *                               entry->len==0.
 * @param[out]    err          : CMD_DB_SUCCESS for success,
 *                               otherwise cmd_db_err_type.
 *
 *  @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query (EFI_CMD_DB_PROTOCOL      *This,
                                    cmd_db_query_info        *query_info,
                                    cmd_db_query_result_type *query_result,
                                    uint8_t                  *data,
                                    cmd_db_err_type          *err)
{
  if ((This == NULL) || (query_info == NULL) || (query_result == NULL) ||
      (err == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *err = cmd_db_query(query_info, query_result, data);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_addr -->
 *
 * @brief Query command db for resource id address.
 *
 * This is used to retrieve resource address based on resource
 * id.
 *
 * @param[in]  This   : Pointer to the EFI_CMD_DB_PROTOCOL
 *                      instance.
 * @param[in]  res_id : resource id to query for address
 * @param[out] addr   : 0 for failure, non-zero for offset
 *                      address
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_addr (EFI_CMD_DB_PROTOCOL *This,
                                         const char          *res_id,
                                         uint32_t            *addr)
{
  if ((This == NULL) || (res_id == NULL) || (addr == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *addr = cmd_db_query_addr(res_id);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_priority -->
 *
 * @brief Query command db for resource address priority from
 *        command DB
 *
 * This is used to retrieve a command DB entry based resource
 * address priority.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL
 *                        instance.
 * @param[in]  addr     : resource addr to query for priority.
 * @param[in]  drv_id   : DRV ID to query resource for priority
 *                        on.
 * @param[out] priority : priority value
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_priority (EFI_CMD_DB_PROTOCOL *This,
                                             uint32_t             addr,
                                             uint8_t              drv_id,
                                             uint32_t            *priority)
{
  if ((This == NULL) || (priority == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *priority = cmd_db_query_priority(addr, drv_id);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_priority_res_id -->
 *
 * @brief Query command db for resource id priority from command
 *        DB.
 *
 * This is used to retrieve a command DB entry based resource
 * id priority.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCO
 *                        instance.
 * @param[in]  res_id   : resource id to query for priority
 * @param[in]  drv_id   : DRV ID to query resource for priority
 *                        on.
 * @param[out] priority : priority value
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_priority_res_id (EFI_CMD_DB_PROTOCOL *This,
                                                    const char          *res_id,
                                                    uint8_t              drv_id,
                                                    uint32_t            *priority)
{
  if ((This == NULL) || (res_id == NULL) || (priority == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *priority = cmd_db_query_priority_res_id(res_id, drv_id);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_version -->
 *
 * @brief Query command db for resource id address.aux data
 *        version.
 *
 * This is used to retrieve associated resource id aux data
 * version. Version will be common for all resources of same
 * cmd_db_slv_id_type. This is used to retrieve associated
 * resource id auxillary data len.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL
 *                        instance.
 * @param[in]  res_id   : resource id to query version
 * @param[out] version  : 0 for failure, non-zero for version
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_version (EFI_CMD_DB_PROTOCOL *This,
                                            const char          *res_id,
                                            uint16_t            *version)
{
  if ((This == NULL) || (res_id == NULL) || (version == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *version = cmd_db_query_version(res_id);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_len -->
 *
 * @brief Query command db for resource id address.aux data len
 *
 * This is used to retrieve associated resource id auxillary
 * data len.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL
 *                        instance.
 * @param[in]  res_id   : resource id to query for aux data len
 * @param[out] len      : 0 for failure, non-zero for length
 *                        address
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_len (EFI_CMD_DB_PROTOCOL *This,
                                        const char          *res_id,
                                        uint32_t            *len)
{
  if ((This == NULL) || (res_id == NULL) || (len == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *len = cmd_db_query_len(res_id);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_aux_data -->
 *
 * @brief Query command db for aux data.
 *
 * This is used to retrieve a command DB entry aux data based on
 * resource address.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL
 *                        instance.
 * @param[in]  res_id   : Resource to retrive AUX Data on.
 * @param[out] len      : Caller provides size of data buffer
 *                        passed in. Returns size of data copied
 *                        to data buffer.
 * @param[out] data     : Data buffer to copy returned aux data
 *                        to.
 * @param[out] err      : CMD_DB_SUCCESS for success, otherwise
 *                        cmd_db_err_type.
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_aux_data (EFI_CMD_DB_PROTOCOL *This,
                                             const char          *res_id,
                                             uint8_t             *len,
                                             uint8_t             *data,
                                             cmd_db_err_type     *err)
{
  if ((This == NULL) || (res_id == NULL) || (len == NULL) || (data == NULL) ||
      (err == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *err = cmd_db_query_aux_data(res_id, len, data);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_cmd_db_query_slv_id -->
 *
 * @brief Query command db for resource id address.slv_id type
 *
 * This is used to retrieve associated resource id slv_id type.
 *
 * @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL
 *                        instance.
 * @param[in]  res_id   : resource id to query for slv id type.
 * @param[out] slv_id   : CMD_DB_SLV_ID_INVALID for failure,
 *                        otherwise slv_id type.
 *
 * @return       EFI_STATUS
 */
STATIC EFI_STATUS EFI_cmd_db_query_slv_id (EFI_CMD_DB_PROTOCOL *This,
                                           const char          *res_id,
                                           cmd_db_slv_id_type  *slv_id)
{
  if ((This == NULL) || (res_id == NULL) || (slv_id == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *slv_id = cmd_db_query_slv_id(res_id);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
STATIC EFI_HANDLE  gProtocolHandle = NULL;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static EFI_CMD_DB_PROTOCOL CmdDbProtocol =
{
  EFI_CMD_DB_PROTOCOL_REVISION,
  EFI_cmd_db_query,
  EFI_cmd_db_query_addr,
  EFI_cmd_db_query_priority,
  EFI_cmd_db_query_priority_res_id,
  EFI_cmd_db_query_version,
  EFI_cmd_db_query_len,
  EFI_cmd_db_query_aux_data,
  EFI_cmd_db_query_slv_id,
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
CmdDbEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
   EFI_STATUS efiStatus = EFI_SUCCESS;
   cmd_db_err_type err = CMD_DB_SUCCESS;

   err = cmd_db_init();
   if (err)
   {
      return EFI_UNSUPPORTED;
   }

   efiStatus = gBS->InstallMultipleProtocolInterfaces(
                    &gProtocolHandle,
                    &gEfiCmdDbProtocolGuid,(void **)&CmdDbProtocol,
                    NULL );

   return efiStatus;
}

