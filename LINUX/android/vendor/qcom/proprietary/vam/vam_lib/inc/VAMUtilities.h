/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAMUTILITIES_H_
#define _VAMUTILITIES_H_

#include <stdint.h>
#include <utils/Log.h>
#include <string>
#include <ctime>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

#include "vaapi.h"

#ifdef DEBUG_LOG
#define VAM_LOGE(...) ALOGE("VAM: " __VA_ARGS__)
#define VAM_LOGD(...) ALOGD("VAM: " __VA_ARGS__)
#define VAM_LOGI(...) ALOGI("VAM: " __VA_ARGS__)
#define VAM_LOGW(...) ALOGW("VAM: " __VA_ARGS__)
#else
#define VAM_LOGE(...) do { } while (0)
#define VAM_LOGD(...) do { } while (0)
#define VAM_LOGI(...) do { } while (0)
#define VAM_LOGW(...) do { } while (0)
#endif

uint64_t getTimeNow();
void CreateUUID(char *uuid_str, size_t uuid_str_size);
bool IsValidUUID(const char *vaapi_id);

std::string getStrFromMetadataFrame(const vaapi_metadata_frame *pMetadata);
std::string getStrFromEvent(const vaapi_event *pEvent);
std::string getStrFromRule(const vaapi_rule *pR);
std::string getStrFromEnrollInfo(const vaapi_enrollment_info *pEnroll);

#define UNUSED(x) (void)(x)

#define LOG_LEVEL_KPI

#ifdef LOG_LEVEL_KPI
#include <cutils/properties.h>
#include <cutils/trace.h>

#define BASE_KPI_FLAG 1
#define DETAIL_KPI_FLAG 2

extern volatile uint32_t kpi_debug_level;

#define VAM_KPI_GET_MASK() ({\
char prop[PROPERTY_VALUE_MAX];\
property_get("persist.vam.kpi.debug", prop, std::to_string(BASE_KPI_FLAG).c_str()); \
kpi_debug_level = atoi (prop);})

class BaseKpiObject {
public:
    BaseKpiObject(const char* str) {
        if (kpi_debug_level >= BASE_KPI_FLAG) {
            atrace_begin(ATRACE_TAG_ALWAYS, str);
        }
    }
    ~BaseKpiObject() {
        if (kpi_debug_level >= BASE_KPI_FLAG) {
            atrace_end(ATRACE_TAG_ALWAYS);
        }
    }
};

#define VAM_KPI_BASE(name) ({\
BaseKpiObject a(name);\
})

class DetailKpiObject {
public:
    DetailKpiObject(const char* str) {
        if (kpi_debug_level >= DETAIL_KPI_FLAG) {
            atrace_begin(ATRACE_TAG_ALWAYS, str);
        }
    }
    ~DetailKpiObject() {
        if (kpi_debug_level >= DETAIL_KPI_FLAG) {
            atrace_end(ATRACE_TAG_ALWAYS);
        }
    }
};

#define VAM_KPI_DETAIL(name) ({\
DetailKpiObject a(name);\
})

#else
#define VAM_KPI_GET_MASK ()     do {} while (0)
#define VAM_KPI_BASE     (name) do {} while (0)
#define VAM_KPI_DETAIL   (name) do {} while (0)
#endif

#endif  // _VAMUTILITIES_H_
