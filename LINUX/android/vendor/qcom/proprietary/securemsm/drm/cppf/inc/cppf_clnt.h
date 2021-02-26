#ifndef CPPF_CLNT_H
#define CPPF_CLNT_H

/** @file libhdcpsrm.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces to the CPPF_CLNT shared library.
 */
/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/15   shl      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

   /**
     This function will load the CPPF tz app.

   */
   int CPPF_ServiceInitialize(void);

   /**
     This function will unload the CPPF tz app.

   */
   int CPPF_ServiceUninitialize(void);

   /**
      This function checks whether the minimum encryption level is changed after a new license
      parsing. It will be calling into TZ cppf app. This function should be called on HLOS side
      by DRM module which has knowledge when a new license is received.

      @param[in/out]    MinEncLevelChanged          the flag shows the change state, 0 means no change.

   */
   void CPPF_CheckMinEncLevelStatus(void);

   /**
      This function sets the min encryption level, this is only for test purpose.

      @param[in]    MinEncLevel         value to set

   */
   int CPPF_TestSetMinEncLevel(uint8_t minEncLevel);

   /**
      This function gets the min encryption level, this is only for test purpose.

      @param[in/out]    *MinEncLevel    address for returned value.

   */
   int CPPF_TestGetMinEncLevel(uint8_t *minEncLevel);

#ifdef __cplusplus
}
#endif

#endif //CPPF_CLNT_H
