/******************************************************************************
  @file    perflock_native_test.cpp
  @brief   mpctl test which tests perflocks, hints, profiles

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include "mp-ctl.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <cutils/properties.h>

#define LOG_TAG           "ANDR-PERFLOCK-TESTER"
#include <cutils/log.h>

#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000

#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)

static int (*perf_lock_acq)(unsigned long handle, int duration,
                            int list[], int numArgs) = NULL;
static int (*perf_lock_rel)(unsigned long handle) = NULL;
static int (*perf_hint)(int, const char *, int, int) = NULL;
static int (*perf_lock_use_profile)(int handle, int profile) = NULL;

char arg_list[120] = {};
int resource_list[OPTIMIZATIONS_MAX] = {};
int args_count = 0;
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

void show_usage() {
    printf("usage: perflock_native_test <options> <option-args>\n"
           "\t--acq <handle> <duration> <resource, value, resource, value,...>\n"
           "\t--hint <hint_id> [<duration>] [<pkg-name>] [<hint-type>]\n"
           "\t--profile <handle> <profile_id> [<duration>]\n"
           "\t--rel <handle>\n"
           "\tperflock acq, rel examles\n"
           "\tperflock_native_test --acq 0 5000 0x40C10000,0x1f\n"
           "\tperflock_native_test --rel 5"
           "\tperflock hint examles\n"
           "\tperflock_native_test --hint 0x00001203\n"
           "\tperflock_native_test --hint 0x00001203 20\n"
           "\tperflock_native_test --hint 0x00001203 10 \"helloApp\"\n"
           "\tperflock profile acq, rel examples"
           "\tperflock_native_test --profile 0 2\n"
           "\tperflock_native_test --profile 3 -1\n"
           "\tperflock_native_test --profile 0 5 10000\n");

    if (libhandle) {
        dlclose(libhandle);
        libhandle = NULL;
    }

    return;
}

int main(int argc, char *argv[]) {
    int handle = -1, profile = 0;
    int duration = 0;
    char *argstr;
    int hint_id = -1, hint_type = -1;
    char *pkg = NULL;
    int idx = 1;
    char *ptr = NULL;

    QLOGE("NRP: %s %d", __func__, argc);

    if (argc < 2) {
        show_usage();
        return 0;
    }

    initialize();

    std::string arg = argv[idx];
    idx++;
    if ((arg == "-h") || (arg == "--help")) {
        show_usage();
        return 0;
    } else if (arg == "--acq") {
        if (perf_lock_acq) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                sscanf(argv[idx], "%s", arg_list);
                argstr = strtok_r(arg_list, ",", &ptr);
                while (argstr != NULL) {
                    sscanf(argstr, "%X", &resource_list[args_count]);
                    QLOGE("NRP: resource_list[%d] = %x", args_count, resource_list[args_count]);
                    args_count++;
                    argstr = strtok_r(NULL, ",", &ptr);
                }
                idx++;
            } else {
                show_usage();
                return 0;
            }

            handle = perf_lock_acq(handle, duration, resource_list, args_count);
            printf("handle=%d\n", handle);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE("NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
	} else if (arg == "--hint") {
        if (perf_hint) {
            duration = -1;
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            handle = perf_hint(hint_id, pkg, duration, hint_type);
            printf("handle=%d\n", handle);
            QLOGE("Client: perf hint hint_id=0x%x", hint_id);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE("NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
	} else if (arg == "--profile") {
        duration = -1;
        if (perf_lock_use_profile) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                profile = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }

            handle = perf_lock_use_profile(handle, profile);
            printf("handle=%d\n", handle);
            QLOGE("Client: lock profile handle=%d profile=%d", handle, profile);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE("NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
                QLOGE("NRP:releasing profile=%d", profile);
                printf("Releasing profile=%d\n", profile);
                perf_lock_use_profile(handle, -1);
            }
        }
    } else if (arg == "--rel") {
        if (perf_lock_rel) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            QLOGE("Client: Release handle=%d", handle);
            perf_lock_rel(handle);
        }
    } else {
        show_usage();
        return 0;
    }

    if (libhandle) {
        dlclose(libhandle);
        libhandle = NULL;
    }

    return handle;
}
