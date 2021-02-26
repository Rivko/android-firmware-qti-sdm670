/**
@file AudDevMgr_GetSetBits.h
@brief This file declares functions that are used by AudioDevMgr to communicate with APR.

*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_device" group
      description in the elite_audio_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/inc/AudDevMgr_GetSetBits.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger. 
==========================================================================*/

#ifndef AUD_DEV_MGR_GETSETBITS_H
#define AUD_DEV_MGR_GETSETBITS_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include adsp_adm_api.h


/** @addtogroup audio_device
@{ */

/**
  Gets the Common Object Pre/Postprocessing (COPP) live input flag from a COPP
  open command payload.

  @param[in] pPayload   Pointer to the payload.

  @return
  0 -- COPP input is not live.\n
  1 -- COPP input is live.

  @dependencies
  None.
 */
static __inline uint16_t adm_get_copp_live_input_flag
        (const adm_cmd_copp_open_t* pPayload)
{
    uint16_t usBitMask = ADM_BIT_MASK_COPP_LIVE_INPUT_FLAG;
    uint16_t shift = ADM_SHIFT_COPP_LIVE_INPUT_FLAG;

    if (!pPayload) return 0;

    return ((pPayload->flags & usBitMask) >> shift);
}

/**
  Sets the COPP live input flag in a COPP open command payload.

  @param[in,out] pPayload   Pointer to the payload.
  @param[in]     usFlag     Input flag:\n
                            - 0 -- COPP input is not live.
                            - 1 -- COPP input is live.

  @return
  None.

  @dependencies
  None.
 */
static __inline void adm_set_copp_live_input_flag
        (adm_cmd_copp_open_t* pPayload, uint16_t usFlag)
{
    uint16_t usBitMask = ADM_BIT_MASK_COPP_LIVE_INPUT_FLAG;
    uint16_t shift = ADM_SHIFT_COPP_LIVE_INPUT_FLAG;
    uint16_t usTemp = 0;

    if (!pPayload) return;

    usTemp = (usFlag << shift) & usBitMask;
    pPayload->flags = (pPayload->flags & ~usBitMask) | usTemp;
}

/**
Gets the COPP ID from a COPP handle.

  @param[in] copp_handle   COPP handle.

  @return
  COPP ID associated with the COPP handle,

  @dependencies
  None.
 */
static __inline uint16_t adm_get_copp_id(uint32_t copp_handle)
{
    uint32_t bit_mask = ADM_BIT_MASK_COPP_ID;
    uint16_t shift = ADM_SHIFT_COPP_ID;

    return (uint16_t)((copp_handle & bit_mask) >> shift);
}

/**
  Sets the COPP ID in a COPP handle.

  @param[in,out] pCoppHandle   Pointer to the COPP handle.
  @param[in]     usCoppId      COPP ID associated with the COPP handle.

  @return
  None.

  @dependencies
  None.
 */
static __inline void adm_set_copp_id(uint32_t* pCoppHandle, uint16_t usCoppId)
{
    uint32_t bit_mask = ADM_BIT_MASK_COPP_ID;
    uint16_t shift = ADM_SHIFT_COPP_ID;
    uint32_t temp = 0;

    if (!pCoppHandle) return;

    temp = ( (uint32_t)usCoppId << shift ) & bit_mask;
    *pCoppHandle = (*pCoppHandle & ~bit_mask) | temp;
}

/**
  Gets the service ID from a COPP handle.

  @param[in] ulCoppHandle   COPP handle.

  @return
  Service ID associated with the COPP handle.

  @dependencies
  None.
 */
static __inline uint8_t adm_get_service_id(uint32_t ulCoppHandle)
{
    uint32_t bit_mask = ADM_BIT_MASK_SERVICE_ID;
    uint16_t shift = ADM_SHIFT_SERVICE_ID;

    return (uint8_t)((ulCoppHandle & bit_mask) >> shift);
}

/**
  Sets the service ID in a COPP handle.

  @param[in,out] pCoppHandle   Pointer to the COPP handle.
  @param[in]     svc_id        Service ID associated with the COPP handle.

  @return
  None.

  @dependencies
  None.
 */
static __inline void adm_set_service_id(uint32_t* pCoppHandle, uint8_t svc_id)
{
    uint32_t bit_mask = ADM_BIT_MASK_SERVICE_ID;
    uint16_t shift = ADM_SHIFT_SERVICE_ID;
    uint32_t temp = 0;

    if (!pCoppHandle) return;

    temp = ( (uint32_t)svc_id << shift ) & bit_mask;
    *pCoppHandle = (*pCoppHandle & ~bit_mask) | temp;
}

/**
  Gets the domain ID from a COPP handle.

  @param[in] ulCoppHandle   COPP handle.

  @return
  Domain ID associated with the COPP handle.

  @dependencies
  None.
 */
static __inline uint8_t adm_get_domain_id(uint32_t ulCoppHandle)
{
    uint32_t bit_mask = ADM_BIT_MASK_DOMAIN_ID;
    uint16_t shift = ADM_SHIFT_DOMAIN_ID;

    return (uint8_t)((ulCoppHandle & bit_mask) >> shift);
}

/**
  Sets the domain ID in a COPP handle.

  @param[in,out] pCoppHandle   Pointer to the COPP handle.
  @param[in]     domain_id     Domain ID associated with the COPP handle.

  @return
  None.

  @dependencies
  None.
 */
static __inline void adm_set_domain_id(uint32_t* pCoppHandle, uint8_t domain_id)
{
    uint32_t bit_mask = ADM_BIT_MASK_DOMAIN_ID;
    uint16_t shift = ADM_SHIFT_DOMAIN_ID;
    uint32_t temp = 0;

    if (!pCoppHandle) return;

    temp = ( (uint32_t)domain_id << shift ) & bit_mask;
    *pCoppHandle = (*pCoppHandle & ~bit_mask) | temp;
}

/** @} */ /* end_addtogroup audio_device */

#endif // #ifndef AUD_DEV_MGR_GETSETBITS_H