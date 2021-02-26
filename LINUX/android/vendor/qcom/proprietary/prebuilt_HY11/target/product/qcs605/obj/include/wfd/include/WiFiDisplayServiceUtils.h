#ifndef _WFD_WIFIDISPLAYSERVICEUTILS_H_
#define _WFD_WIFIDISPLAYSERVICEUTILS_H_

/*==============================================================================
*  WiFiDisplayServiceUtils.h
*
*  DESCRIPTION:
*  Placeholder for macros to be used for native service
*
*
*  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
11/06/2014                    InitialDraft
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================
*/

#ifndef FORBID_CONSTRUCTORS
#define FORBID_CONSTRUCTORS(name) \
    name(const name &) = delete; \
    name &operator=(const name &) = delete
#endif

#define CHECK_TRANSACTION(res) do {\
    if(res != 0)\
    {\
        ALOGE("Transaction failed with %x!!!",res);\
        return res;\
    }\
}while(0)

#define CHECK_TRANSACTION_PTR(res) do {\
    if(res != 0)\
    {\
        ALOGE("Transaction failed with %x!!!",res);\
        return NULL;\
    }\
}while(0)
#endif // _WFD_WIFIDISPLAYSERVICEUTILS_H_
