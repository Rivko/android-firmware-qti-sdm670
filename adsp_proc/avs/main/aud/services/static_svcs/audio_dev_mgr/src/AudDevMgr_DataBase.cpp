/**
@file AudDevMgr_DataBase.cpp
@brief This file defines variables and functions for managing  AudioDevMgr database.
*/

/*========================================================================
Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_DataBase.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
========================================================================== */
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AudDevMgr_DataBase.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

 /*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct 
{
   Adm_PPNodeType       structPPNode[ADM_MAX_COPPS];
   Adm_PPNodeType       structComprPPNode[ADM_COMPRESSED_MAX-ADM_COMPRESSED_MIN+1];   
   qurt_elite_mutex_t        AdmDataBaseLock;
} Adm_DataBase;

/*----------------------------------------------------------------------------
 * Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
void adm_database_init()
{
   qurt_elite_mutex_init( &(Adm_DataBase.AdmDataBaseLock) );
}

void adm_database_teardown()
{
   qurt_elite_mutex_destroy( &(Adm_DataBase.AdmDataBaseLock) );
}

void adm_database_lock()
{
   qurt_elite_mutex_lock( &(Adm_DataBase.AdmDataBaseLock) );
}

void adm_database_unlock()
{
   qurt_elite_mutex_unlock( &(Adm_DataBase.AdmDataBaseLock) );
}

ADSPResult Adm_DataBaseRegisterPPCb(uint16_t unCoppID, Adm_PPCallbackInfoType PPCallbackInfo, 
                                    bool_t isCompreCopp)
{
   if (FALSE == isCompreCopp)
   {
      if (ADM_MAX_COPPS <= unCoppID)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM: CoppID [%d] out of range, max permissible COPP ID = %d", unCoppID, ADM_MAX_COPPS - 1);
         return ADSP_EBADPARAM;
      }
      qurt_elite_mutex_lock( &(Adm_DataBase.AdmDataBaseLock) );
      Adm_DataBase.structPPNode[unCoppID].structPPCallbackInfo = PPCallbackInfo;
      qurt_elite_mutex_unlock( &(Adm_DataBase.AdmDataBaseLock) );

      return ADSP_EOK;
   }
   else
   {     
      if ( ((unCoppID >= ADM_COMPRESSED_MIN_RX) && (unCoppID <= ADM_COMPRESSED_MAX_RX)) ||
           ((unCoppID >= ADM_COMPRESSED_MIN_TX) && (unCoppID <= ADM_COMPRESSED_MAX_TX)) )  	  
      {
         qurt_elite_mutex_lock( &(Adm_DataBase.AdmDataBaseLock) );
         Adm_DataBase.structComprPPNode[unCoppID-ADM_COMPRESSED_MIN].structPPCallbackInfo = PPCallbackInfo;
         qurt_elite_mutex_unlock( &(Adm_DataBase.AdmDataBaseLock) );
         return ADSP_EOK;	  		 
      }		 
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM: Compressed DevID [%d] out of range", unCoppID);   
         return ADSP_EBADPARAM;
      }		 
   }
}

ADSPResult Adm_DataBaseDeRegisterPPCb(uint16_t unCoppID, Adm_PPCallbackInfoType PPCallbackInfo,
                                      bool_t isCompreCopp)
{
   if (FALSE == isCompreCopp)
   {
      if (ADM_MAX_COPPS <= unCoppID)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM: CoppID [%d] out of range", unCoppID);
         return ADSP_EBADPARAM;
      }
      qurt_elite_mutex_lock( &(Adm_DataBase.AdmDataBaseLock) );
      Adm_DataBase.structPPNode[unCoppID].structPPCallbackInfo.cbFuntion = NULL;
      Adm_DataBase.structPPNode[unCoppID].structPPCallbackInfo.pContext = NULL;
      qurt_elite_mutex_unlock( &(Adm_DataBase.AdmDataBaseLock) );

      return ADSP_EOK;
   }
   else
   {     
      if ( ((unCoppID >= ADM_COMPRESSED_MIN_RX) && (unCoppID <= ADM_COMPRESSED_MAX_RX)) ||
           ((unCoppID >= ADM_COMPRESSED_MIN_TX) && (unCoppID <= ADM_COMPRESSED_MAX_TX)) )  	  
      {
         qurt_elite_mutex_lock( &(Adm_DataBase.AdmDataBaseLock) );
         Adm_DataBase.structPPNode[unCoppID-ADM_COMPRESSED_MIN].structPPCallbackInfo.cbFuntion = NULL;
         Adm_DataBase.structPPNode[unCoppID-ADM_COMPRESSED_MIN].structPPCallbackInfo.pContext = NULL;
         qurt_elite_mutex_unlock( &(Adm_DataBase.AdmDataBaseLock) );   
	  
         return ADSP_EOK;	  	  
      }		 
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM: Compressed DevID [%d] out of range", unCoppID);   
         return ADSP_EBADPARAM;
      }
   }
}

ADSPResult Adm_CallPPCb(elite_apr_port_t port, elite_msg_any_t msg)
{
   uint16_t unCoppID = (uint16_t)port;
   if (ADM_MAX_COPPS <= unCoppID)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM: CoppID [%d] out of range", unCoppID);
      return ADSP_EBADPARAM;
   }
   Adm_PPNodeType *pPPNode = &(Adm_DataBase.structPPNode[unCoppID]);
   Adm_PPCallbackInfoType *pCallbackInfo = &(pPPNode->structPPCallbackInfo);
   if (NULL == pCallbackInfo->cbFuntion)
   {
      return ADSP_ENOTREADY;
   }

   return pCallbackInfo->cbFuntion(pCallbackInfo->pContext, msg);
}