/*===========================================================================
                           check_call.h

 DESCRIPTION: Helper macro for checking standard linux calls

 Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 =============================================================================*/
#ifndef __CHECK_CALL_H__
#define __CHECK_CALL_H__
/*----------------------------------------------------------------------------
  Include files
 ----------------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>
#include <log/log.h>

/*----------------------------------------------------------------------------
  Consts and macros
 ----------------------------------------------------------------------------*/

/*============================================================================
  Macro:  CHECK_CALL
============================================================================*/
/**
  Evaluates "stmt", and if the return value is <0, logs error message.
*/
#define CHECK_CALL(stmt) do {\
    int __retval = stmt; \
    if (__retval < 0) { \
        ALOGE("Error %d in %s at line %d: \"" #stmt "\", %s", \
                __retval, \
                __FILE__, \
                __LINE__, \
                strerror(errno)); \
    } \
} while(0)

#endif /* __CHECK_CALL_H__ */
