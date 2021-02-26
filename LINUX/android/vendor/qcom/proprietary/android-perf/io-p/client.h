/******************************************************************************
  @file    client.h
  @brief   Android performance iop library

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IOPCLIENT_H__
#define __IOPCLIENT_H__

#define QC_DEBUG_ERRORS 1
#ifdef QC_DEBUG_ERRORS
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#else
#define QLOGE(...)
#endif

#if QC_DEBUG
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#define QCLOGE(...)   ALOGE(__VA_ARGS__)
#else
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#define QCLOGE(...)
#endif

int perf_io_prefetch_start(int pid, const char *pkg_name,  const char *code_path);
int perf_io_prefetch_stop();
int perf_ux_engine_events(int opcode, int pid, const char *pkg_name, int lat);
char * perf_ux_engine_trigger(int opcode);
#endif //__IOPCLIENT_H__

#ifdef __cplusplus
}
#endif
