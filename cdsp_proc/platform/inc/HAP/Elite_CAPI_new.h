/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S.
  Government. Diversion contrary to U.S. law prohibited. 
==============================================================================*/
#ifndef ELITE_CAPI_NEW_H
#define ELITE_CAPI_NEW_H

#include "mmdefs.h"
#include "adsp_error_codes.h"
#include "Elite_CAPI.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/**
  Gets the memory requirement of the library. Optional input parameters can be
  sent down to the CAPI wrapper to calculate worst case memory requirements.

  @param[in]  format    Format of the input buffer
  @param[in]  bps       bps
  @param[out] size_ptr  Number of bytes of memory being requested.

   @return
   Indication of success or failure.

   @dependencies
   None.
*/
typedef ADSPResult(* capi_getsize_f)(int32_t format, uint32_t bps,
                                     uint32_t* size_ptr);

/**
  An initialization function that must be called to instantiate the library
  to set up the virtual function table, and also to allocate any memory
  required by the library. States within the library must be initialized at
  the same time.

  If the algorithm does not support a particular data format, the appropriate
  error code is to be returned. 

  @newpage
  Before returning SUCCESS and once the function is complete, a debug message
  is to be printed:

  Input parameters are sent down to the APPI wrapper.

   @param[in]  _pif      Pointer to the library object.
   @param[in]  bps       bps
   @param[out] size_ptr  Number of bytes of memory being
         requested.

   @return
   Indication of success or failure.

   @dependencies
   The appi_getsize_f() function must have been executed and any
   memory requested must have been allocated. 
*/
typedef ADSPResult(* capi_ctor_f)(ICAPI* capi_ptr, uint32_t format,
                                  uint32_t bps);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ELITE_CAPI_NEW_H
