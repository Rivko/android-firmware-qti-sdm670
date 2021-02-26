#ifndef __CVD_UTILS_COMMON_I_H__
#define __CVD_UTILS_COMMON_I_H__

/*
   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_utils_common_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
 * COMMON UTILITIES                                                         *
 ****************************************************************************/

/**
  This routine queries all CVD services for to retrieve the number of apr objects 
  used by them. 
  This returns TRUE apr objects of all CVD services are freed up.
  Returns FALSE if all apr objects are not freed.

  @par Payload
  None
  
  @return 
  True -  If apr objects of all CVD services are freed.
  False - If apr objects of any CVD services are not freed.

  @dependencies
  None.
 
  @comments
  None.
  
*/
APR_INTERNAL bool_t is_cvd_utils_obj_freed( void );

#endif /* __CVD_UTILS_COMMON_I_H__ */
