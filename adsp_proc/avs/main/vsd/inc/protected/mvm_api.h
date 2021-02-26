#ifndef __MVM_API_H__
#define __MVM_API_H__

/*
   Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/mvm_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "drv_api.h"

/**
  Entry command to any CVD based synchronous blocking APIs.
  The individual cmd_id payload needs to be type cast to void* 
  with size of payload specified in "size" argument.

  @par[in] cmd_id
  @par[in] params
  @par[in] size of params
  
  @return 
  APR_EOK on success.

  @dependencies
  None.
 
  @comments

*/
APR_EXTERNAL int32_t mvm_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /* __MVM_API_H__ */

