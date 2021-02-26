/**
  @file  EFICmdDb.h
  @brief Cmd DB Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __EFICMD_DB_H__
#define __EFICMD_DB_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/cmd_db.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_cmd_db_constants
  Protocol version.
*/
#define EFI_CMD_DB_PROTOCOL_REVISION 0x0000000000010000
/** @} */ /* end_addtogroup efi_cmd_db_constants */

/*  Protocol GUID definition */
/** @ingroup efi_cmd_db_protocol */
#define EFI_CMD_DB_PROTOCOL_GUID \
  { 0x1C34F691, 0xD33D, 0x4E14, { 0xB8, 0xF8, 0x32, 0xC6, 0xC0, 0x29, 0xE9, 0x5B } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the EFI CMD DB Protocol GUID.
 */
extern EFI_GUID gEfiCmdDbProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** @cond */
typedef struct _EFI_CMD_DB_PROTOCOL EFI_CMD_DB_PROTOCOL;
/** @endcond */

/* EFI_CMD_DB_QUERY */
/** @ingroup efi_cmd_db_query
  @par Summary
  Query command db API.

  @param[in]     This         : Pointer to the
                                EFI_CMD_DB_PROTOCOL instance.
  @param[in]     query_info   : Provide information to be used
                                for query.
  @param[in/out] query_result : Query entry result info.
  @param[out]    data         : Client address to return entry
                                data.  NULL okay if
                                entry->len==0.
  @param[out]    err          : CMD_DB_SUCCESS for success, otherwise
                                cmd_db_err_type.

  @par Description
  This is used to retrieve a command DB entry based on query
  info.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY)(
    IN EFI_CMD_DB_PROTOCOL          *This,
    IN cmd_db_query_info            *query_info,
    IN OUT cmd_db_query_result_type *query_result,
    OUT uint8_t                     *data,
    OUT cmd_db_err_type             *err
);


/* EFI_CMD_DB_QUERY_ADDR */
/** @ingroup efi_cmd_db_query_addr
  @par Summary
  Query command db for resource id address.

  @param[in]  This   : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id : resource id to query for address
  @param[out] addr   : 0 for failure, non-zero for offset address

  @par Description
  This is used to retrieve resource address based on resource id.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_ADDR)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    OUT uint32_t           *addr
);

/* EFI_CMD_DB_QUERY_PRIORITY */
/** @ingroup efi_cmd_db_query_priority
  @par Summary
  Query command db for resource address priority from command DB

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  addr     : resource addr to query for priority.
  @param[in]  drv_id   : DRV ID to query resource for priority on.
  @param[out] priority : priority value

  @par Description
  This is used to retrieve a command DB entry based resource address priority.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_PRIORITY)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN uint32_t             addr,
    IN uint8_t              drv_id,
    OUT uint32_t           *priority
);

/* EFI_CMD_DB_QUERY_PRIORITY_RES_ID */
/** @ingroup efi_cmd_db_query_priority_res_id
  @par Summary
  Query command db for resource id priority from command DB.

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id   : resource id to query for priority
  @param[in]  drv_id   : DRV ID to query resource for priority on.
  @param[out] priority : priority value

  @par Description
  This is used to retrieve a command DB entry based resource id priority.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_PRIORITY_RES_ID)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    IN uint8_t              drv_id,
    OUT uint32_t           *priority
);

/* EFI_CMD_DB_QUERY_VERSION */
/** @ingroup efi_cmd_db_query_version
  @par Summary
  Query command db for resource id address.aux data version.

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id   : resource id to query version
  @param[out] version  : 0 for failure, non-zero for version

  @par Description
  This is used to retrieve associated resource id aux data
  version. Version will be common for all resources of same
  cmd_db_slv_id_type. This is used to retrieve associated
  resource id auxillary data len.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_VERSION)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    OUT uint16_t           *version
);

/* EFI_CMD_DB_QUERY_LEN */
/** @ingroup efi_cmd_db_query_len
  @par Summary
  Query command db for resource id address.aux data len

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id   : resource id to query for aux data len
  @param[out] len      : 0 for failure, non-zero for length address

  @par Description
  This is used to retrieve associated resource id auxillary
  data len.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_LEN)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    OUT uint32_t           *len
);

/* EFI_CMD_DB_QUERY_AUX_DATA */
/** @ingroup efi_cmd_db_query_aux_data
  @par Summary
  Query command db for aux data.

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id   : Resource to retrive AUX Data on.
  @param[out] len      : Caller provides size of data buffer passed in.
                         Returns size of data copied to data buffer.
  @param[out] data     : Data buffer to copy returned aux data to.
  @param[out] err      : CMD_DB_SUCCESS for success, otherwise cmd_db_err_type.

  @par Description
  This is used to retrieve a command DB entry aux data based on resource address.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_AUX_DATA)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    IN OUT uint8_t         *len,
    OUT uint8_t            *data,
    OUT cmd_db_err_type    *err
);

/* EFI_CMD_DB_QUERY_SLV_ID */
/** @ingroup efi_cmd_db_query_slv_id
  @par Summary
  Query command db for resource id address.slv_id type

  @param[in]  This     : Pointer to the EFI_CMD_DB_PROTOCOL instance.
  @param[in]  res_id   : resource id to query for slv id type.
  @param[out] slv_id   : CMD_DB_SLV_ID_INVALID for failure, otherwise slv_id type.

  @par Description
  This is used to retrieve associated resource id slv_id type.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

 */
typedef EFI_STATUS (EFIAPI* EFI_CMD_DB_QUERY_SLV_ID)(
    IN EFI_CMD_DB_PROTOCOL *This,
    IN const char          *res_id,
    OUT cmd_db_slv_id_type *slv_id
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_cmd_db_protocol
  @par Summary
  Cmd Db Driver Protocol interface.

  @par Parameters
  @inputprotoparams{cmd_db_proto_params.tex}
*/
typedef struct _EFI_CMD_DB_PROTOCOL {
  UINT64                           Revision;
  EFI_CMD_DB_QUERY                 Query;
  EFI_CMD_DB_QUERY_ADDR            QueryAddr;
  EFI_CMD_DB_QUERY_PRIORITY        QueryPriority;
  EFI_CMD_DB_QUERY_PRIORITY_RES_ID QueryPriorityResId;
  EFI_CMD_DB_QUERY_VERSION         QueryVersion;
  EFI_CMD_DB_QUERY_LEN             QueryLen;
  EFI_CMD_DB_QUERY_AUX_DATA        QueryAuxData;
  EFI_CMD_DB_QUERY_SLV_ID          QuerySlvId;
}EFI_CMD_DB_PROTOCOL;

#endif /* EFICMD_DB_H */
