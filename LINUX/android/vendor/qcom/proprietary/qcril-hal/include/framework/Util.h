/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
template<class T> void ignore( const T& ) { }
void setThreadName(const char *threadName);
void threadSleep(int seconds);
