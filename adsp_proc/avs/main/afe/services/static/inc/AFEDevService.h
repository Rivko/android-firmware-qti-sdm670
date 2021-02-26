/*========================================================================
  This file contains high level apis of AFE static service

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEDevService.h#1 $
 ====================================================================== */
#ifndef _AFE_DEVSERVICE_H_
#define _AFE_DEVSERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Function declarations
  ========================================================================== */
/**
  Starts the AFE static service.
  
  @param[in]  dummy            Unused.
  @param[out] ppHAfeDevService Pointer to the pointer of the created static 
                               service instance.
  
  @return 
  aDSP error code, if unsuccessful.
  
  @dependencies 
  None.
 */
ADSPResult  aud_create_stat_afe_svc(uint32_t dummy, void** ppHAfeDevService);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //#ifndef DEV_SERVICE_H
