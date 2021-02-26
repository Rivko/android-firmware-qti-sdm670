/*-----------------------------------------------------------------------
   Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/
#ifndef SYSMON_MARKER_H
#define SYSMON_MARKER_H

#ifdef __cplusplus
extern "C" {
#endif

//----------------------Prototypes----------------------------------

/*************************************************************************//**
 * @fn HP_profile
 * @brief This feature provides a framework to allow profiling piece of code or
 *       algorithm instead of profiling the entire text section.
 * @param marker - Unique number to find the profiling data mapped to function.
 * @param enable - 1 - Used to Enable the marker profiling.
 *                 0 - Used to Disable the marker profiling.
 *****************************************************************************/

void HP_profile(unsigned int marker, unsigned char enable);

#ifdef __cplusplus
}
#endif

#endif /*SYSMON_MARKER_H*/
