/** @file afe_main.h

 @brief This file contains AFE service initialization and deinitialization functions

 */

/*========================================================================
  $Header: //components/rel/avs.adsp/2.8.5/afe/utils/misc/inc/afe_main.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 11/19/2014 rv   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _AFE_MAIN_H_
#define _AFE_MAIN_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus


#include "qurt_elite_types.h"


/**
  @brief Perform one time initialization of AFE.

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_main_init();


/**
  @brief Perform one time de-initialization of AFE.

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_main_deinit();



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _AFE_MAIN_H_
