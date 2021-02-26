/******************************************************************************
  @file    perflock_native_stress_test.cpp
  @brief   mpctl stress test which stresses perflocks, hints, profiles

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include "mp-ctl.h"

#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#include <private/android_filesystem_config.h>
#include <cutils/properties.h>
#include <cutils/trace.h>

#ifdef SERVER
#include <dlfcn.h>
#endif

#define LOG_TAG           "ANDR-PERFLOCK-TESTER"
#include <cutils/log.h>
#if defined(ANDROID_JELLYBEAN)
#include "common_log.h"
#endif

#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000
#define PARAMS_PER_CALL 12
#define MAX_PARAMS_PER_CALL 128
#define CALLS_PER_THREAD 10000
#define NUM_THREADS 20
#define MAX_NUM_THREADS 100

#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)

static int (*perf_lock_acq)(unsigned long handle, int duration,
                            int list[], int numArgs) = NULL;
static int (*perf_lock_rel)(unsigned long handle) = NULL;
static int (*perf_hint)(int, const char *, int, int) = NULL;
static int (*perf_lock_use_profile)(int handle, int profile) = NULL;

static pthread_t perflock_thread[MAX_NUM_THREADS];

static int calls_per_thread = CALLS_PER_THREAD;
static int run_time = 10;

int handle = 0;
int duration = 0;
char arg_list[120] = {};
int resource_list[OPTIMIZATIONS_MAX] = {};
int release = 0;
int args_count = 0;
int argc_t;
char **argv_t;
static void *libhandle = NULL;

static void initialize(void) {
    const char *rc = NULL;
    char qcopt_lib_path[100] = {0};

    dlerror();

    if (property_get("ro.vendor.extension_library", qcopt_lib_path,
                NULL)) {
        QLOGE("NRP: lib name %s", qcopt_lib_path);
        libhandle = dlopen(qcopt_lib_path, RTLD_NOW);
        if (!libhandle) {
            QLOGE("NRP: Unable to open %s: %s\n", qcopt_lib_path,
                    dlerror());
        }
    }

    if (!libhandle) {
        QLOGE("NRP: Failed to get qcopt handle.\n");
    } else {
        /*
         * qc-opt handle obtained. Get the perflock acquire/release
         * function pointers.
         */
        *(void **) (&perf_lock_acq) = dlsym(libhandle, "perf_lock_acq");
        if ((rc = dlerror()) != NULL) {
             QLOGE("NRP: Unable to get perf_lock_acq function handle.\n");
             dlclose(libhandle);
             libhandle = NULL;
             return;
        }

        *(void **) (&perf_lock_rel) = dlsym(libhandle, "perf_lock_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE("NRP: Unable to get perf_lock_rel function handle.\n");
             dlclose(libhandle);
             libhandle = NULL;
             return;
        }

        *(void **) (&perf_hint) = dlsym(libhandle, "perf_hint");
        if ((rc = dlerror()) != NULL) {
             QLOGE("NRP: Unable to get perf_hint function handle.\n");
             dlclose(libhandle);
             libhandle = NULL;
             return;
        }

        *(void **) (&perf_lock_use_profile) = dlsym(libhandle, "perf_lock_use_profile");
        if ((rc = dlerror()) != NULL) {
             QLOGE("NRP: Unable to get perf_hint function handle.\n");
             dlclose(libhandle);
             libhandle = NULL;
             return;
        }
    }
}


int perf_locks[] = {
    0x40400000,

    0x40800000,
    0x40800010,
    0x40800020,
    0x40800030,
    0x40800100,
    0x40800110,
    0x40800120,
    0x40800130,

    0x40804000,
    0x40804010,
    0x40804020,
    0x40804030,
    0x40804100,
    0x40804110,
    0x40804120,
    0x40804130,

    0x40C00000,
    0x40C04000,
    0x40C08000,
    0x40C0C000,
    0x40C10000,
    0x40C14000,
    0x40C18000,
    0x40C1C000,
    0x40C20000,
    0x40C24000,
    0x40C28000,
    0x40C2C000,
    0x40C30000,
    0x40C34000,
    0x40C38000,
    0x40C3C000,
    0x40C40000,
    0x40C44000,
    0x40C48000,
    0x40C4C000,
    0x40C50000,
    0x40C54000,
    0x40C58000,
    0x40C5C000,

    0x41000000,
    0x41000100,
    0x41004000,
    0x41004100,

    0x41400000,
    0x41404000,
    0x41408000,
    0x4140C000,
    0x41410000,
    0x41414000,
    0x41418000,
    0x4141C000,
    0x41420000,
    0x41424000,
    0x41428000,
    0x4142C000,
    0x41430000,
    0x41434000,
    0x41438000,

    0x41400100,
    0x41404100,
    0x41408100,
    0x4140C100,
    0x41410100,
    0x41414100,
    0x41418100,
    0x4141C100,
    0x41420100,
    0x41424100,
    0x41428100,
    0x4142C100,
    0x41430100,
    0x41434100,
    0x41438100,

    0x41800000,
    0x41804000,
    0x41808000,
    0x4180C000,
    0x41810000,
    0x41814000,
    0x41818000,
    0x4181C000,
    0x41820000,
    0x41824000,

    0x41C00000,
    0x41C04000,
    0x41C08000,

    0x42000000,
    0x42004000,

    0x42400000,
    0x42404000,
    0x42408000,
    0x4240C000,
    0x42410000,
    0x42414000,
    0x42418000,
    0x4241C000,

    0X42800000,
    0X42804000,
    0X42808000,
    0X4280C000,
    0X42810000,
    0X42814000,

    0X42C04000,
    0X42C00000,
    0x42C08000,
    0x42C0C000,
    0x42C10000,
    0x42C14000,
};

int perf_locks_res_val_pair[] = {
    0x40400000,0x1,
    0x40800000,0x5fe,
    0x40800010,0x4d6,
    0x40800020,0x4d6,
    0x40800030,0x4d6,
    0x40804100,0x3e6,
    0x40800110,0x4d7,
    0x40800120,0x4d8,
    0x40800130,0x4d9,
    0x40804000,0x3e6,
    0x40804010,0x3e7,
    0x40804020,0x3e8,
    0x40804030,0x3e9,
    0x40804100,0x3e6,
    0x40804110,0x3e7,
    0x40804120,0x3e8,
    0x40804130,0x3e9,
    0x40C00000,0x1,
    0x40C04000,0x1f,
    0x40C08000,0xff,
    0x40C0C000,0x5,
    0x40C10000,0x23,
    0x40C14000,0xfff,
    0x40C18000,0x1f,
    0x40C1C000,0x60,
    0x40C20000,0x50,
    0x40C24000,0x4fe,
    0x41000000,0x4,
    0x41004000,0x0,
    0x41000100,0x4,
    0x41004100,0x0,
    0x41400000,0xfe,
    0x41400100,0xfd,
    0x41404000,0x1f,
    0x41404100,0x1,
    0x41408000,0x1f,
    0x41408100,0x1,
    0x4140C000,0x1f,
    0x4140C100,0x1f,
    0x41410000,0x4f,
    0x41410100,0xa,
    0x41414000,0x5fe,
    0x41414100,0x4de,
    0x41418000,0x1,
    0x41418100,0xf,
    0x4141C000,0xff,
    0x4141C100,0xff,
    0x41420000,0xfe,
    0x41420100,0xfd,
    0x41424000,0xf,
    0x41424100,0xf,
    0x41428000,0xd,
    0x41428100,0x12,
    0x4142C000,0x2f,
    0x4142C100,0x2c,
    0x41430000,0xf,
    0x41430100,0xf,
    0x41434000,0xff,
    0x41434100,0x1,
    0x41800000,0xff,
    0x41804000,0xa,
    0x41808000,0xf,
    0x41C00000,0x1,
    0x41C04000,0x1,
    0x41C08000,0x1,
    0x42000000,0x1,
    0x42004000,0x1,
    0x42400000,0x1,
    0x42404000,0x1,
    0x42408000,0x1,
    0x4240C000,0x1,
    0x42410000,0x1,
    0x42414000,0x1,
    0x42418000,0x1,
    0x4241C000,0x1,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x41414100,0x4de,
    0x41418000,0x1,
    0x41418100,0xf,
    0x4141C000,0xff,
    0x4141C100,0xff,
    0x41420000,0xfe,
    0x41420100,0xfd,
    0x41424000,0xf,
    0x41424100,0xf,
    0x41428000,0xd,
    0x40804020,0x3e8,
    0x40804030,0x3e9,
    0x40804100,0x3e6,
    0x40804110,0x3e7,
    0x40804120,0x3e8,
    0x40804130,0x3e9,
    0x40C00000,0x1,
    0x40C04000,0x1f,
    0x40C08000,0xff,
    0x40C0C000,0x5,
    0x40800000,0x5fe,
    0x40800010,0x4d6,
    0x40800020,0x4d6,
    0x40800030,0x4d6,
    0x40800100,0x4d6,
    0x40800110,0x4d7,
    0x40800120,0x4d8,
    0x40800130,0x4d9,
    0x40804000,0x3e6,
    0x40804010,0x3e7,
    0x40400000,0x1,
    0x40800000,0x5fe,
    0x40800010,0x4d6,
    0x40800020,0x4d6,
    0x40804110,0x3e7,
    0x40804120,0x3e8,
    0x40804130,0x3e9,
    0x40C00000,0x1,
    0x40C04000,0x1f,
    0x40C08000,0xff,
    0x40C0C000,0x5,
    0x40C10000,0x23,
    0x40C14000,0xfff,
    0x40C18000,0x1f,
    0x40C1C000,0x60,
    0x40C20000,0x50,
    0x40C24000,0x4fe,
    0x41000000,0x4,
    0x41004000,0x0,
    0x41000100,0x4,
    0x41004100,0x0,
    0x41400000,0xfe,
    0x41400100,0xfd,
    0x41404000,0x1f,
    0x41404100,0x1,
    0x41408000,0x1f,
    0x41408100,0x1,
    0x4140C000,0x1f,
    0x4140C100,0x1f,
    0x41410000,0x4f,
    0x41410100,0xa,
    0x41414000,0x5fe,
    0x41414100,0x4de,
    0x41418000,0x1,
    0x41418100,0xf,
    0x4141C000,0xff,
    0x4141C100,0xff,
    0x41420000,0xfe,
    0x41420100,0xfd,
    0x41424000,0xf,
    0x41424100,0xf,
    0x41428000,0xd,
    0x41428100,0x12,
    0x4142C000,0x2f,
    0x4142C100,0x2c,
    0x41430000,0xf,
    0x41430100,0xf,
    0x41434000,0xff,
    0x42C04000,0x1,
    0x41414100,0x4de,
    0x41418000,0x1,
    0x41418100,0xf,
    0x4141C000,0xff,
    0x4141C100,0xff,
    0x41420000,0xfe,
    0x41420100,0xfd,
    0x41424000,0xf,
    0x41424100,0xf,
    0x40800100,0x4d6,
    0x40800030,0x4d6,
    0x40800100,0x4d6,
    0x40800110,0x4d7,
    0x40800120,0x4d8,
    0x40800130,0x4d9,
    0x40804000,0x3e6,
    0x40804010,0x3e7,
    0x40804020,0x3e8,
    0x40804030,0x3e9,
    0x41434100,0x1,
    0x41800000,0xff,
    0x41804000,0xa,
    0x41808000,0xf,
    0x41C00000,0x1,
    0x41C04000,0x1,
    0x41C08000,0x1,
    0x42000000,0x1,
    0x42004000,0x1,
    0x42400000,0x1,
    0x42404000,0x1,
    0x42408000,0x1,
    0x4240C000,0x1,
    0x42410000,0x1,
    0x42414000,0x1,
    0x42418000,0x1,
    0x4241C000,0x1,
    0x42800000,0x1,
    0x41428000,0xd,
    0x40804020,0x3e8,
    0x40804030,0x3e9,
    0x40804100,0x3e6,
    0x40804110,0x3e7,
    0x40804120,0x3e8,
    0x40804130,0x3e9,
    0x40C00000,0x1,
    0x40C04000,0x1f,
    0x40C08000,0xff,
    0x40C0C000,0x5,
    0x40800000,0x5fe,
    0x40800010,0x4d6,
    0x40800020,0x4d6,
    0x40C00000,0x1f,
    0x40C04000,0x1f,
    0x40C14000,0x1f,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0x1f,
    0x42408000,0xff,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41404000,0x1f,
    0x41434100,0xff,
    0x40804100,0x3e6,
    0x4141C000,0x1f,
    0x40800000,0x4d6,
    0x41000100,0x4,
    0x40C20000,0x52,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x41424100,0xfe,
    0x40804000,0x516,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x40C00000,0x0,
    0x40C04000,0x1f,
    0x40C14000,0xfe,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0xf,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41404000,0x1f,
    0x41434100,0xff,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42408000,0xff,
    0x41804000,0x5,
    0x40C00000,0x1f,
    0x40C04000,0x1f,
    0x40C14000,0x1f,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0x1f,
    0x42408000,0xff,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x40804100,0x3e6,
    0x4141C000,0x1f,
    0x40800000,0x4d6,
    0x41000100,0x4,
    0x40C20000,0x52,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x41424100,0xfe,
    0x40804000,0x516,
    0x41804000,0xa,
    0x41428100,0xf,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x40C00000,0x1f,
    0x40C04000,0x1f,
    0x40C14000,0x1f,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0x1f,
    0x42408000,0xff,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x40C00000,0x0,
    0x40C04000,0x1f,
    0x40C14000,0xfe,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0xf,
    0x42408000,0xff,
    0x41804000,0x5,
    0x40C00000,0x1f,
    0x40C04000,0x1f,
    0x40C14000,0x1f,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0x1f,
    0x42408000,0xff,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41404000,0x1f,
    0x41434100,0xff,
    0x40804100,0x3e6,
    0x4141C000,0x1f,
    0x40800000,0x4d6,
    0x41000100,0x4,
    0x40C20000,0x52,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x41424100,0xfe,
    0x40804000,0x516,
    0x41804000,0xa,
    0x41428100,0xf,
    0x41418100,0x1f,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
    0x4141C000,0x1f,
    0x41400100,0xf,
    0x41410100,0x2f,
    0x42408000,0xfff,
    0x41804000,0x30,
    0x4141C000,0xfe,
    0x41404000,0x2f,
    0x41434100,0xf,
    0x40804100,0x4e6,
    0x4141C000,0xfdf,
    0x4141C000,0x1f,
    0x40C00000,0x0,
    0x40C04000,0x1f,
    0x40C14000,0xfe,
    0x40C20000,0x50,
    0x41400100,0xfe,
    0x41410100,0xf,
    0x42408000,0xff,
    0x41804000,0x5,
    0x40C24000,0x4fe,
    0x40C14000,0x1f,
    0x41004000,0x0,
    0x42800000,0x1,
    0x42C04000,0x1,
    0x40C08000,0x1f,
};

int perf_hints[] = {
    0x00001203,
    0x00001300,
    0x00001206,
    0x00001207,
    0x00001301,
    0x00001302,
    0x00001303,
    0x00001304,
    0x00001305,
    0x00001306,
    0x00001307,
    0x00001081,
    0x00001081,
    0x00001083,
    0x00001080,
    0x00001084,
    0x00001085,
    0x00001086,
    0x00001087,
    0x00001088,
    0x00001089,
};

static struct timespec test_start;
static struct timespec test_end;

int getElapsedTime() {
    int time_delta = 0;
    clock_gettime(CLOCK_MONOTONIC, &test_end);
    time_delta = test_end.tv_sec - test_start.tv_sec + (test_start.tv_nsec - test_end.tv_nsec)/(1000*1000*1000);
    return time_delta;
}

int size = (sizeof(perf_locks)/sizeof(perf_locks[0]));
static void call(int params_per_call) {
    int i = 0, j;
    int resource_list[MAX_PARAMS_PER_CALL];
    for (i=0; i<params_per_call; i+=2) {
        j = rand()%size;
        resource_list[i] = perf_locks[j];
        resource_list[i+1] = rand()%1500;
        printf("requesting locks for Resource:0x%x value:%d \n", resource_list[i], resource_list[i+1]);
    }

    perf_lock_acq(handle, duration, resource_list, PARAMS_PER_CALL);
    if (duration) {
        struct timespec sleep_time;
        //QLOGE("NRP: Sleep");
        sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
        sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
        nanosleep(&sleep_time, NULL);
    }
}

static void call2(int params_per_call) {
    int i = 0, j = 0;
    int resource_list[MAX_PARAMS_PER_CALL];
    int size = (sizeof(perf_locks_res_val_pair)/sizeof(perf_locks_res_val_pair[0]));

    for (i=0; i<params_per_call; i+=2) {
        j = rand()%size;
        if (j%2 != 0) {
            j = j-1;
        }
        resource_list[i] = perf_locks_res_val_pair[j];
        resource_list[i+1] = perf_locks_res_val_pair[j+1];
        printf("requesting locks for Resource:0x%x value:%d \n", resource_list[i], resource_list[i+1]);
    }

    perf_lock_acq(handle, duration, resource_list, PARAMS_PER_CALL);
    if (duration) {
        struct timespec sleep_time;
        //QLOGE("NRP: Sleep");
        sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
        sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
        nanosleep(&sleep_time, NULL);
    }
}

static void callhints(int params_per_call) {
    int j = 0;
    int size = (sizeof(perf_hints)/sizeof(perf_hints[0]));

    j = rand()%size;
    printf("Requesting perf hint :0x%x \n", perf_hints[j]);

    for (int i=0; i<params_per_call;i++) {
        perf_hint(perf_hints[j]+i, NULL, duration, -1);
    }

    if (duration) {
        struct timespec sleep_time;
        //QLOGE("NRP: Sleep");
        sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
        sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
        nanosleep(&sleep_time, NULL);
    }
}

static void callprofiles(int params_per_call) {
    int j = 0;
    int size = 10;
    int handle = -1;

    j = rand()%size;

    for (int i=0; i<params_per_call;i++) {
        printf("Requesting perf profile :%d \n", j);
        handle = perf_lock_use_profile(0, j+i);
    }

    if (duration) {
        struct timespec sleep_time;
        //QLOGE("NRP: Sleep");
        sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
        sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
        nanosleep(&sleep_time, NULL);
        perf_lock_use_profile(handle, -1);
    }
}

static void *perflock_function(void *i) {
    int j;
    int params_per_call = 2;

    int threadno = *((int *) i);
    printf("call from thread %d \n", threadno);

    for (int k=0; k<calls_per_thread; k++) {
        params_per_call = 2;
        for (j = 0; j < 2; j++) {
            duration = 3000 + rand()%5000;
            printf("thread:%d calling iter:%d\n",threadno, j);
            call(params_per_call);
            call2(params_per_call);
        }
        for (j = 0; j < 2; j++) {
            duration = 3000 + rand()%7000;
            printf("thread:%d calling hints:%d\n",threadno, j);
            callhints(params_per_call);
        }
        for (j = 0; j < 2; j++) {
            duration = 3000 + rand()%5000;
            printf("thread:%d calling profiles:%d\n",threadno, j);
            callprofiles(params_per_call);
        }

        if ((run_time - getElapsedTime()) < 30) {
            break;
        }
        params_per_call = (params_per_call+ rand())%(MAX_PARAMS_PER_CALL/2);
        if (0!=(params_per_call%2)) {
            params_per_call = params_per_call-1;
            printf("params_per_call=%d\n", params_per_call);
        }
    }

    free(i);
    return NULL;
}

void show_usage() {
    printf("usage: perflock_native_stress_test [<num_hrs_to_run> <num_threads>]\n"
           "run ~10hrs by default with 20 threads\n"
           "\tExamples: \n"
           "\tperflock_native_stress_test 10\n"
           "\tperflock_native_stress_test 10 100\n");

    return;
}

int main(int argc, char *argv[]) {
    int i;
    QLOGE("NRP: %s %d", __func__, argc);

    int time_delta = 0;
    float hrs = 0.0;
    int num_threads = NUM_THREADS;

    if (argc > 3) {
        show_usage();
        return 0;
    } else if (argc==2) {
        hrs = atof(argv[1]);
    } else if (argc==3) {
        hrs = atof(argv[1]);
        num_threads = atoi(argv[2]);
    }

    if (hrs > 0.0) {
        run_time = hrs * 60 * 60;
    }

    if (num_threads < 0) {
        num_threads = NUM_THREADS;
    }
    if (num_threads > MAX_NUM_THREADS) {
        num_threads = MAX_NUM_THREADS;
    }

    clock_gettime(CLOCK_MONOTONIC, &test_start);

    initialize();

    printf("array size is %d", size);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (i = 0; i < num_threads; i++) {
        int *arg = (int *) malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
        pthread_create(&perflock_thread[i], &attr, perflock_function, (void *)arg);
        //sleep(1);
    }
    pthread_attr_destroy(&attr);
    for (i = 0; i < num_threads; i++)
        pthread_join(perflock_thread[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &test_end);
    time_delta = test_end.tv_sec - test_start.tv_sec + (test_start.tv_nsec - test_end.tv_nsec)/(1000*1000*1000);
    int remainder = 0;
    if (time_delta > 60) {
        remainder = time_delta%60;
        time_delta = time_delta/60;
    } else {
        printf("completed: %d secs\n", time_delta);
        return 0;
    }
    if (time_delta > 60) {
        remainder = (time_delta%60);
        time_delta = time_delta/60;
    } else {
        printf("completed: %d mins %d secs\n", time_delta, remainder);
        return 0;
    }
    if (time_delta > 24) {
        remainder = (time_delta%24);
        time_delta = time_delta/24;
        printf("completed: %d days %d hrs\n", time_delta, remainder);
    } else {
        printf("completed: %d hrs %d min\n", time_delta, remainder);
    }

    return 0;
}
