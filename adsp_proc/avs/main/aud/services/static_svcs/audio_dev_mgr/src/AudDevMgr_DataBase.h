/**
@file AudDevMgr_DataBase.h
@brief This file declares some common data types used by AudDevMgr_DataBase.cpp
*/

/*========================================================================
Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_DataBase.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
========================================================================== */
#ifndef AUD_DEV_MGR_DATABASE_H
#define AUD_DEV_MGR_DATABASE_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "AudDevMgr.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct 
{
   Adm_PPCallbackInfoType        structPPCallbackInfo;
} Adm_PPNodeType;

ADSPResult Adm_DataBaseRegisterPPCb(uint16_t unCoppID, Adm_PPCallbackInfoType PPCallbackInfo,
                                    bool_t isCompreCopp);
ADSPResult Adm_DataBaseDeRegisterPPCb(uint16_t unCoppID, Adm_PPCallbackInfoType PPCallbackInfo,
                                    bool_t isCompreCopp);
ADSPResult Adm_CallPPCb(elite_apr_port_t port, elite_msg_any_t msg);
void adm_database_init();
void adm_database_teardown();
void adm_database_lock();
void adm_database_unlock();

#ifdef __cplusplus

}
#endif //__cplusplus

#endif //#ifndef AUD_DEV_MGR_DATABASE_H