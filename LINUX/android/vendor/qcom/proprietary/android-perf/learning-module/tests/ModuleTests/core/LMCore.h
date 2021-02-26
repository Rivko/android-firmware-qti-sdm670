/******************************************************************************
 *   @file    LMCore.h
 *   @brief   Learning Module Core test
 *
 *   DESCRIPTION
 *      Provides simple module level LM Core test which uses perf-hal hints
 *   to send a sequence of hints to test LM Core.
 *      It also provides api to send debug idle trigger which helps in manually
 *   triggering algorithms.
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 ********************************************************************************/
#ifndef LMCORE_H_
#define LMCORE_H_

void lmSendIdle();
int LMCoreTest();

#endif /* LMCORE_H_ */
