/******************************************************************************
 *   @file    Debug.h
 *   @brief   Implementation of Android Framework Learning Module Test
 *
 *   DESCRIPTION
 *      Provides LM Test related Debug API
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 ********************************************************************************/
#ifndef DEBUG_H_
#define DEBUG_H_
#include "DebugLog.h"
#define LOG_TAG_TEST "LmTest"

#define DEBUGTEST(log...)    DEBUGE(LOG_TAG_TEST,log); \
                             printf(log);

#endif /* DEBUG_H_ */
