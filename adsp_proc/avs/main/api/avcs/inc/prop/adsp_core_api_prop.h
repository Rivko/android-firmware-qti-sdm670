#ifndef _ADSP_CORE_API_PROP_H_
#define _ADSP_CORE_API_PROP_H_

/**
@file adsp_core_api_prop.h

@brief This file contains the structure definitions for the AVCS
commands used in client-AVCS communications.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      AV_Core_Service_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/20/16    HB      Created file for proprietary information.
========================================================================== */
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup core_cmd_set_license
  Sets the license information. Currently, this command is used for both DTS
  Headphone:X (DTS-HPX) and Dolby.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_SET_LICENSE

  @apr_msgpayload{avcs_cmd_set_license}
    @table{weak__avcs__cmd__set__license}

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_CMD_SET_LICENSE                               0x00012919

typedef struct avcs_cmd_set_license avcs_cmd_set_license_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_set_license
@{ */
/* Payload header of the AVCS_CMD_SET_LICENSE command.
*/
struct avcs_cmd_set_license
{
   uint32_t id;
   /**< Unique ID of the license.

        @values Any valid license ID */

   uint32_t overwrite;
   /**< Specifies whether to overwrite an existing license ID.

        @values
        - 0 -- Do not overwrite an existing license with this ID
        - 1 -- Overwrite an existing license with this ID @tablebulletend */

   uint32_t size;
   /**< Size in bytes of the license data following this header. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_set_license */

/** @ingroup core_cmd_get_license
  Queries for license validation results.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_GET_LICENSE_VALIDATION_RESULT

  @apr_msgpayload{avcs_cmd_get_license_validation_result}
    @table{weak__avcs__cmd__get__license__validation__result}

  @return
    None.

  @dependencies
    A license with the given ID must have been set (#AVCS_CMD_SET_LICENSE).
    The aDSP must have a function that validates this license.
*/
#define AVCS_CMD_GET_LICENSE_VALIDATION_RESULT        0x0001291A

typedef struct avcs_cmd_get_license_validation_result avcs_cmd_get_license_validation_result_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_get_license_validation_result
@{ */
/* Payload of the AVCS_CMD_GET_LICENSE_VALIDATION_RESULT command.
*/
struct avcs_cmd_get_license_validation_result
{
   uint32_t id;
   /**< Unique ID of the license.

        @values Any valid license ID */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_get_license_validation_result */

/** @ingroup core_resp_get_license
  Returns the license validation results to the client in response to an
  #AVCS_CMD_GET_LICENSE_VALIDATION_RESULT request.

  @apr_hdr_fields
   Opcode -- AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT

  @apr_msgpayload{avcs_cmdrsp_get_license_validation_result}
    @table{weak__avcs__cmdrsp__get__license__validation__result}

  @return
    None.

  @dependencies
    #AVCS_CMD_GET_LICENSE_VALIDATION_RESULT must have been sent.
*/
#define AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT        0x0001291B

typedef struct avcs_cmdrsp_get_license_validation_result avcs_cmdrsp_get_license_validation_result_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmdrsp_get_license_validation_result
@{ */
/* Payload header of the AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT command.
*/
struct avcs_cmdrsp_get_license_validation_result
{
   uint32_t result;
   /**< License validation result:
        - #ADSP_EOK -- The license validation result was successfully
          retrieved.
        - #ADSP_ENOTEXIST -- There is no license with the given ID.
        - #ADSP_ENOTIMPL -- There is no validation function for a license with
          this ID. @tablebulletend */

   uint32_t size;
   /**< Length in bytes of the validation result that follows this structure. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmdrsp_get_license_validation_result */

/** @ingroup core_cmd_dolby_set_mfg_id
  Sets the manufacturer ID information for Dolby modules.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_SET_DMID

  @apr_msgpayload{avcs_service_cmd_set_dmid_t}
    @table{weak__avcs__service__cmd__set__dmid__t}

  @description
    To enable the modules provided by Dolby (like the Dolby Digital Plus
    decoder and the Dolby Audio Processing), a valid manufacturer ID must be
    sent to the aDSP.
    @par
    This command allows the client to send the manufacturer ID to the
    aDSP before opening any sessions that use the Dolby modules. This
    manufacturer ID is validated against the actual license present in the
    aDSP image (the OEM licensee modifies the aDSP image with the actual
    license file provided by Dolby).
    @par
    A success result from the aDSP indicates that the manufacturer ID was
    properly communicated to aDSP. It does not indicate
    whether the aDSP found the manufacturer ID to be valid.
    @par
    This command can be called multiple times, but the aDSP accepts only
    the first call. Subsequent calls before aDSP shutdown are ignored.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AVCS_CMD_SET_DMID               0x00012918

typedef struct avcs_service_cmd_set_dmid_t avcs_service_cmd_set_dmid_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_service_cmd_set_dmid_t
@{ */
/* Payload of the AVCS_CMD_SET_DMID command, which sends
  Dolby mfg id information to the AVS.
*/
struct avcs_service_cmd_set_dmid_t
{
    uint32_t                  dm_id;  /**< Manufacturer ID. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_service_cmd_set_dmid_t */

/** @ingroup core_cmd_set_license
  ID of the DTS-HPX license. */
#define DTS_HPX_LICENSE_ID                                 0x00028346


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_CORE_API_PROP_H_ */
