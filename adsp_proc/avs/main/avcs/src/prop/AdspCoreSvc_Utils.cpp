/**
@file AdspCoreSvc_Utils.cpp***********************

@brief This file contains the utility implementation for AdspCoreSvc.

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/23/16   HB		Created file for proprietary information.

==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.
--------------------------------------------------------------------------*/


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
/// ADSPCoreSvc header
#include "AdspCoreSvc.h"
#include "AdspCoreSvc_InstanceStruct.h"

// License database
#include "adsp_license_db.h"

// DS1 security header
//#include "ds1_license_handler.h"
// License database
#include "adsp_license_db.h"

#include "adsp_core_api_prop.h"



/**  Internal API. Opcode for deleting all the stored licenses. */
#define AVCS_CMD_DELETE_LICENSES            0x00012911

/*
//temp fix to remove dependency on third party algos. long
//term fix would be to use set-param or deprecate if not used
#ifdef __cplusplus
extern "C"{
#endif

int32_t get_dts_hpx_library_description_str_length(void);

int32_t get_dts_hpx_library_version_str_length(void);

int32_t DTS_HPX_Eagle_DSEC_GetParam(void* theInstance, int32_t nParameterType, void* pValue);

#ifdef __cplusplus
}
#endif
*/
// Set a license in the license database
static ADSPResult AVS_SetLicense(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

// Gets the license validation result from the license database
static ADSPResult AVS_GetLicenseValidationResult(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

//Delete all stored licenses
static ADSPResult AdspCoreSvc_DeleteLicenses(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);


static ADSPResult AdspCoreSvc_SetDMID(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);


static uint32_t round_up_to_4(uint32_t val)
{
	return ((val + 3) / 4) * 4;
}

ADSPResult AdspCoreSvc_Internal_Init()
{
	ADSPResult result = ADSP_EOK;
	result = adsp_license_db_init();
	if (ADSP_FAILED(result))
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to initialize license database!");
		return result;
	}
	return result;
}

void AdspCoreSvc_Internal_DeInit()
{
	adsp_license_db_deinit();
}

int AdspCoreSvcPrivateApr(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   QURT_ELITE_ASSERT(pMsg);

   elite_apr_packet_t *pAprPacket = (elite_apr_packet_t *) pMsg->pPayload;

   AvsCoreSvc_t *pMe = (AvsCoreSvc_t*)pInstance;

   uint32_t cmd = elite_apr_if_get_opcode(pAprPacket);
   switch( cmd )
   {
	   case AVCS_CMD_SET_LICENSE:
	   {
		   result = AVS_SetLicense(pMe, pAprPacket);
		   break;
	   }

	   case AVCS_CMD_DELETE_LICENSES:
	   {
		   result = AdspCoreSvc_DeleteLicenses(pMe, pAprPacket);
		   break;
	   }

	   case AVCS_CMD_GET_LICENSE_VALIDATION_RESULT:
	   {
		   result = AVS_GetLicenseValidationResult(pMe, pAprPacket);
		   break;
	   }

       case AVCS_CMD_SET_DMID:
       {
           result = AdspCoreSvc_SetDMID(pMe, pAprPacket);
           break;
       }



       default:
       {
		   /* Handle error. */
		   if ( elite_apr_if_msg_type_is_cmd( pAprPacket) )
		   {  /* Complete unsupported commands. */
			   (void)elite_apr_if_end_cmd( pMe->aprHandle, pAprPacket, APR_EUNSUPPORTED );
		   }
		   else
		   {  /* Drop unsupported events. */
			   (void)elite_apr_if_free( pMe->aprHandle, pAprPacket );
		   }
    	   result = ADSP_EUNSUPPORTED;
    	   break;
       }
   }
   return result;
}

static ADSPResult AVS_SetLicense(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;

   if(!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_cmd_set_license_t))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for set license.");
      (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_ENEEDMORE);
      return ADSP_ENEEDMORE;
   }

   avcs_cmd_set_license_t *pCmd = (avcs_cmd_set_license_t*)(elite_apr_if_get_payload_ptr(pAprPacket));
   uint32_t required_size = round_up_to_4(sizeof(avcs_cmd_set_license_t) + pCmd->size);
   if(elite_apr_if_get_payload_size(pAprPacket) < required_size)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for set license.");
      (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_ENEEDMORE);
      return ADSP_ENEEDMORE;
   }

   if ((0 != pCmd->overwrite) && (1 != pCmd->overwrite))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid value %lu of the overwrite flag in set license command.", pCmd->overwrite);
      (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EUNEXPECTED);
      return ADSP_EUNEXPECTED;
   }

   if (0 == pCmd->overwrite)
   {
	   // Check if there is already a license with this id.
	   uint32_t license_size = 0;
	   if (ADSP_EOK == adsp_license_db_get_size(pCmd->id, &license_size))
	   {
		   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "License with id %lu already exists.", pCmd->id);
		   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EALREADY);
		   return ADSP_EALREADY;
	   }
   }

   const void *pLicenseData = pCmd + 1; // Pointer to memory immediately after the header.
   result = adsp_license_db_add_license(pCmd->id, pLicenseData, pCmd->size);

   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
   return result;
}

static ADSPResult AVS_GetLicenseValidationResult(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   uint32_t size = 0;
   uint32_t id = 0;

   if(!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_cmd_get_license_validation_result_t))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for get license validation result command.");
      result = ADSP_EUNEXPECTED;
   }
   else
   {
	   bool_t is_validated = FALSE;
	   avcs_cmd_get_license_validation_result_t *pCmd = (avcs_cmd_get_license_validation_result_t*)(elite_apr_if_get_payload_ptr(pAprPacket));
	   id = pCmd->id;
	   result = adsp_license_db_is_validated(id, &is_validated);
	   if (ADSP_FAILED(result))
	   {
		   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No license exists with id %lu", id);
	   }
	   else
	   {
		   if (!is_validated)
		   {
			   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "License with id %lu has not been validated", id);
			   result = ADSP_ENOTIMPL;
		   }
		   else
		   {
			   result = adsp_license_db_get_size(id, &size);
			   if (ADSP_FAILED(result))
			   {
				   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get size for license with id %lu", id);
			   }
		   }
	   }
   }

    /* Allocate the return packet */
   int32_t rc;
   elite_apr_packet_t *pPacket = NULL;
   rc = elite_apr_if_alloc_cmd_rsp(
		   pMe->aprHandle,
		   elite_apr_if_get_dst_addr( pAprPacket),
		   elite_apr_if_get_dst_port( pAprPacket),
		   elite_apr_if_get_src_addr( pAprPacket),
		   elite_apr_if_get_src_port( pAprPacket),
		   elite_apr_if_get_client_token( pAprPacket),
		   AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT,
		   round_up_to_4(sizeof(avcs_cmdrsp_get_license_validation_result_t) + size),
		   &pPacket);
   if (APR_EOK != rc)
   {
	   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Get License Validation Result Response packet allocation failed  = %ld!", rc);
	   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_ENOMEMORY);
	   return ADSP_ENOMEMORY;
   }
   if (NULL == pPacket)
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Get License Validation Result Response packet allocation failed. pPacket is NULL");
	   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_ENOMEMORY);
	   return ADSP_ENOMEMORY;
   }


   avcs_cmdrsp_get_license_validation_result_t *pHeader = (avcs_cmdrsp_get_license_validation_result_t*)elite_apr_if_get_payload_ptr(pPacket);
   uint8_t *pBuf = (uint8_t*)(pHeader + 1);
   uint32_t buf_size = elite_apr_if_get_payload_size(pPacket) - sizeof(avcs_cmdrsp_get_license_validation_result_t);
   uint32_t license_size = 0;

   if (ADSP_SUCCEEDED(result))
   {
	   result = adsp_license_db_get_license(id, pBuf, buf_size, &license_size);
	   if (ADSP_FAILED(result))
	   {
		   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get license with id %lu", id);
		   size = 0;
	   }
	   else if(license_size != size)
	   {
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Mismatch in license size. Expected %lu Got %lu", size, license_size);
		   result = ADSP_EUNEXPECTED;
		   size = 0;
	   }
   }

   pHeader->result = result;
   pHeader->size = size;

   // Fill in the padding, if any
   pBuf += size;
   uint32_t padding_size = buf_size - size;
   memset(pBuf, 0, padding_size);

   /* send response */
   if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
   {
	   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Get License Validation Result Response send failed  = %ld!", rc);
	   (void) elite_apr_if_free(pMe->aprHandle, pPacket);
	   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EFAILED);
	   return ADSP_EFAILED;
   }

    /* free the original packet */
    (void)elite_apr_if_free( pMe->aprHandle, pAprPacket);

   return result;
}

static ADSPResult AdspCoreSvc_DeleteLicenses(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK, tmpResult = ADSP_EOK;
   if(!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   adsp_license_db_deinit();
   tmpResult = adsp_license_db_init();
   if (ADSP_FAILED(tmpResult))
   {
	   result = tmpResult;
   }

   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
   return result;
}

static ADSPResult AdspCoreSvc_SetDMID(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   if(!pAprPacket)
   {
	   return ADSP_EBADPARAM;
   }

   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_service_cmd_set_dmid_t))
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for setting the Dolby Manufacturer ID.");
	   result = ADSP_EUNEXPECTED;
	   goto bailout;
   }

   {
	   avcs_service_cmd_set_dmid_t *pCmd = (avcs_service_cmd_set_dmid_t*)(elite_apr_if_get_payload_ptr(pAprPacket));

	   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_service_cmd_set_dmid_t))
	   {
		   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for setting the Dolby Manufacturer ID.");
		   result = ADSP_EUNEXPECTED;
		   goto bailout;
	   }

	   uint32_t ds1_license_dmid = AVCS_CMD_SET_DMID;
	   uint32_t ds1_license_dmid_size = 0;
	   uint32_t lic_result = ADSP_EOK;

	   //Check if the dmid license is already stored
	   lic_result = adsp_license_db_get_size(ds1_license_dmid, &ds1_license_dmid_size);

	   if(lic_result != ADSP_EOK)
	   {
		   //Add the dmid license
		   lic_result = adsp_license_db_add_license(ds1_license_dmid, &(pCmd->dm_id), sizeof(pCmd->dm_id));
		   if(lic_result != ADSP_EOK)
		   {
			   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure in setting ds1 manufacturer id with result %lu", lic_result);
		   }
	   }
	   else
	   {
		   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DMID already set, ignoring this ID.");
	   }
   }


   bailout:
   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
   return result;
}

