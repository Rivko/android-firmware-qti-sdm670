/******************************************************************************
  @file    MpctlUtils.h
  @brief   Implementation of mpctl utils functions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __MPCTL_UTILS__H_
#define __MPCTL_UTILS__H_

#define FAILED -1
#define SUCCESS 0

#define MIN_FREQ_REQ      0
#define MAX_FREQ_REQ      1

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

static int perf_debug_output = 0;

#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000

struct hint_associated_data;

void write_to_file(FILE *output, const char *input_node, const char *input_val);
int  write_to_node(FILE *read_file, const char *node_str, const char *node);
void signal_chk_poll_thread(char *node, int rc);
inline int get_online_cpus (int * status_array, int array_sz);
int get_online_core(int cpu, int max_num_cpu_clustr);
int get_core_status(int cpu);
int vid_create_timer(hint_associated_data *data);
void rearm_slvp_timer(hint_associated_data *data);
void cancel_slvp_timer(hint_associated_data *data);
void send_irq_balance(int *msg, int **ret);
int update_freq_node(int start, int end, unsigned int val, char *fmtStr, int len);
char* get_devfreq_new_val(unsigned int, unsigned int *, int, char* );
int save_node_val(const char*, char* );
int change_node_val(const char *, const char *, int);
bool minBoundCheck(char* max, char* min, char* reqval);
bool maxBoundCheck(char* max, char* min, char* reqval);

#endif /*__MPCTL_UTILS__H_*/
