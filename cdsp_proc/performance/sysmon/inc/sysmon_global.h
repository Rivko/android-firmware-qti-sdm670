/*-----------------------------------------------------------------------------
   Copyright (c) 2014 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/
/** @file sysmon_global.h
 *  @brief Sysmon global header file
 */
#ifndef SYSMON_GLOBAL_H_
#define SYSMON_GLOBAL_H_

/** @brief Global return used by sysmon */
typedef enum
{
    SYSMON_SUCCESS = 0,
    /**< Indicates success */

    SYSMON_FAILURE = 1
    /**< Indicates failure */
}SYSMON_RETURN;

#ifndef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF
#endif

#endif /* SYSMON_GLOBAL_H_ */
