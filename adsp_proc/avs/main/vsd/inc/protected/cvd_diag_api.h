#ifndef __CVD_DIAG_API_H__
#define __CVD_DIAG_API_H__

/*
  ============================================================================

   Copyright (C) 2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/cvd_diag_api.h#1 $
   $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/****************************************************************************
  Include file
****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
  DEFINITION
****************************************************************************/

typedef enum cvd_diag_callindex_enum_t
{
  /* public */ CVD_DIAG_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ CVD_DIAG_CALLINDEX_ENUM_INIT = 1,
  /* public */ CVD_DIAG_CALLINDEX_ENUM_DEINIT = 2,
  /* public */ CVD_DIAG_CALLINDEX_ENUM_INVALID
}
  cvd_diag_callindex_enum_t;

/****************************************************************************
 * CALL API                                                                 *
 ****************************************************************************/
/**
  Provides a single API entry point to the CVD DIAG utility.
 
  @param[in] index       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  APR_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/

APR_EXTERNAL int32_t cvd_diag_call (
  uint32_t index,
  void* params,
  uint32_t size
);

#endif  /* CVD_DIAG_API_H */

