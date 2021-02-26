/**
@file AudDevMgr_mmpm.h
@brief This file declares some common data types used by AudDevMgr_mmpm.cpp.
*/
/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
==========================================================================*/
/*-----------------------------------------------------------------------
Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#ifndef AUD_DEV_MGR_MMPM_H
#define AUD_DEV_MGR_MMPM_H

#include "AudDevMgr.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

   /*---------------------------------------------------------------------------
   Function Declarations and Documentation
   ----------------------------------------------------------------------------*/
/*
   This function calls MMPM request API with the required MIPS and BW values.

   @param pAudStatAdmSvc[in/out] This points to the instance of AdmStatSvc_InfoType.
   @return Success or Failure                        
*/

   ADSPResult AudDevMgr_RequestHwResources(AdmStatSvc_InfoType *pAudStatAdmSvc);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AUD_DEV_MGR_MMPM_H
