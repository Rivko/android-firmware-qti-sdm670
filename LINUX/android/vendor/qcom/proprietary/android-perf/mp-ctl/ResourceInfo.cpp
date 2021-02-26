/******************************************************************************
  @file    ResourceInfo.cpp
  @brief   Implementation of performance resources

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "ResourceInfo.h"

#define LOG_TAG "ANDR-PERF-RESOURCE"
#include <cutils/log.h>
#include "Target.h"
#include "MpctlUtils.h"

ResourceInfo::ResourceInfo() {
    mResource.major = MISC_MAJOR_OPCODE;
    mResource.minor = UNSUPPORTED_OPCODE;
    mResource.mapping = 0;
    mResource.cluster = -1;
    mResource.core = -1;
    mResource.qindex = UNSUPPORTED_Q_INDEX;
}

ResourceInfo::ResourceInfo(ResourceInfo const& resObj) {
    mResource = resObj.mResource;
}

ResourceInfo& ResourceInfo::operator=(ResourceInfo const& resObj) {
    mResource = resObj.mResource;

    return *this;
}

bool ResourceInfo::operator==(const ResourceInfo& resObj) {
  return ((mResource.major == resObj.mResource.major) &&
          (mResource.minor == resObj.mResource.minor) &&
          (mResource.mapping == resObj.mResource.mapping) &&
          (mResource.cluster == resObj.mResource.cluster) &&
          (mResource.value == resObj.mResource.value) &&
          (mResource.core == resObj.mResource.core));
}

bool ResourceInfo::operator!=(const ResourceInfo& resObj) {
  return !(*this == resObj);
}

unsigned int minorIdxMaxSize[MAX_MAJOR_RESOURCES] = {
    MAX_DISPLAY_MINOR_OPCODE,
    MAX_PC_MINOR_OPCODE,
    MAX_CPUFREQ_MINOR_OPCODE,
    MAX_SCHED_MINOR_OPCODE,
    MAX_CORE_HOTPLUG_MINOR_OPCODE,
    MAX_INTERACTIVE_MINOR_OPCODE,
    MAX_CPUBW_HWMON_MINOR_OPCODE,
    MAX_VIDEO_MINOR_OPCODE,
    MAX_KSM_MINOR_OPCODE,
    MAX_OND_MINOR_OPCODE,
    MAX_GPU_MINOR_OPCODE,
    MAX_MISC_MINOR_OPCODE,
    MAX_LLCBW_HWMON_MINOR_OPCODE,
    MAX_MEMLAT_MINOR_OPCODE
};

unsigned short ResourceInfo::DiscoverQueueIndex() {
    int idx = 0;
    QLOGI("QIndex being sought for major %d minor %d",
          mResource.major, mResource.minor);

    //make sure we get the unsupported q index for all invalid major/minor types
    if (mResource.major >= MAX_MAJOR_RESOURCES) {
        mResource.qindex = UNSUPPORTED_Q_INDEX;
        return mResource.qindex;
    }

    switch(mResource.major) {
        case MEMLAT_MAJOR_OPCODE:
             idx += MAX_LLCBW_HWMON_MINOR_OPCODE;
        case LLCBW_HWMON_MAJOR_OPCODE:
             idx += MAX_MISC_MINOR_OPCODE;
        case MISC_MAJOR_OPCODE:
            idx += MAX_GPU_MINOR_OPCODE;
            //intentional fall through
        case GPU_MAJOR_OPCODE:
            idx += MAX_OND_MINOR_OPCODE;
            //intentional fall through
        case ONDEMAND_MAJOR_OPCODE:
            idx += MAX_KSM_MINOR_OPCODE;
            //intentional fall through
        case KSM_MAJOR_OPCODE:
            idx += MAX_VIDEO_MINOR_OPCODE;
            //intentional fall through
        case VIDEO_MAJOR_OPCODE:
            idx += MAX_CPUBW_HWMON_MINOR_OPCODE;
            //intentional fall through
        case CPUBW_HWMON_MAJOR_OPCODE:
            idx += MAX_INTERACTIVE_MINOR_OPCODE;
            //intentional fall through
        case INTERACTIVE_MAJOR_OPCODE:
            idx += MAX_CORE_HOTPLUG_MINOR_OPCODE;
            //intentional fall through
        case CORE_HOTPLUG_MAJOR_OPCODE:
            idx += MAX_SCHED_MINOR_OPCODE;
            //intentional fall through
        case SCHED_MAJOR_OPCODE:
            idx += MAX_CPUFREQ_MINOR_OPCODE;
            //intentional fall through
        case CPUFREQ_MAJOR_OPCODE:
            idx += MAX_PC_MINOR_OPCODE;
            //intentional fall through
        case POWER_COLLAPSE_MAJOR_OPCODE:
            idx += MAX_DISPLAY_MINOR_OPCODE;
            //intentional fall through
        case DISPLAY_OFF_MAJOR_OPCODE:
            break;
    }

    mResource.qindex = idx + mResource.minor;

    if ((mResource.minor >= minorIdxMaxSize[mResource.major]) ||
        (mResource.qindex >= MAX_MINOR_RESOURCES)) {
        mResource.qindex = UNSUPPORTED_Q_INDEX;
    }

    return mResource.qindex;
}

void ResourceInfo::Dump() {
    QLOGI("Resource version=%d, major=%d, minor=%d, map=%d ",
           mResource.version, mResource.major, mResource.minor, mResource.mapping);
    QLOGI("resource cluster=%d, core=%d, value=%d, queue-index=%d",
          mResource.cluster, mResource.core, mResource.value, mResource.qindex);
}

int ResourceInfo::GetMinorIndex(int major) {
    if (major >= 0 && major < MAX_MAJOR_RESOURCES) {
       return minorIdxMaxSize[major];
    }
    else {
       QLOGE("Major type 0x%x not supported", major);
    }
    return -1;
}

void ResourceInfo::ParseNewRequest(unsigned int opcode) {
    mResource.version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;;
    mResource.mapping = (opcode & EXTRACT_MAP_TYPE) >> SHIFT_BIT_MAP;
    mResource.major = (opcode & EXTRACT_MAJOR_TYPE) >> SHIFT_BIT_MAJOR_TYPE;
    mResource.minor = (opcode & EXTRACT_MINOR_TYPE) >> SHIFT_BIT_MINOR_TYPE;
    mResource.cluster = (opcode & EXTRACT_CLUSTER) >> SHIFT_BIT_CLUSTER;
    mResource.core = (opcode & EXTRACT_CORE) >> SHIFT_BIT_CORE;
    DiscoverQueueIndex();
    QLOGI("Resource bitmap %x, qindex %d, cluster %d, core %d, major %x, minor %x map %d", opcode,
          mResource.qindex, mResource.cluster, mResource.core, mResource.major, mResource.minor, mResource.mapping);
    return;
}

/****************************************************************************
 **Physical transaltion layer                                              **
 ***************************************************************************/
bool ResourceInfo::TranslateToPhysical() {
    bool ret = true;
    short tmp = 0;

    //logical cluster/core now translated to physical core/cluster based on target
    tmp = Target::getCurTarget().getPhysicalCluster(mResource.cluster);
    QLOGI("physical cluster %d",tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.cluster = tmp;
    }

    tmp = Target::getCurTarget().getPhysicalCore(mResource.cluster, mResource.core);
    QLOGI("physical core %d",tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.core = tmp;
    }

    QLOGI("returned %d", ret);
    return ret;
}

bool ResourceInfo::TranslateToPhysicalOld() {
    bool ret = true;
    short tmp = 0;
    QLOGI("Physical cluster is %d", mResource.cluster);

    /* As some of the resources are core independent
     * we map them to the perf cluster.
     * */
    if ((mResource.major != CPUFREQ_MAJOR_OPCODE) &&
        (mResource.major != INTERACTIVE_MAJOR_OPCODE) &&
        (mResource.major != CORE_HOTPLUG_MAJOR_OPCODE)) {
        tmp = Target::getCurTarget().getPhysicalCluster(mResource.cluster);
        QLOGI("physical cluster %d",tmp);
        if (tmp < 0) {
            ret = false;
        } else {
            mResource.cluster = tmp;
       }
    }

    tmp = Target::getCurTarget().getPhysicalCore(mResource.cluster, mResource.core);
    QLOGI("physical core %d", tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.core = tmp;
    }
    QLOGI("returned %d", ret);
    return ret;
}

bool ResourceInfo::TranslateValueMapToPhysical() {
    bool ret = true;
    int val = 0;

    if (mResource.mapping == VALUE_MAPPED) {
        val = Target::getCurTarget().getMappedValue(mResource.qindex, mResource.value);
        QLOGI("Mapped Value is 0x%x", val);
        if (val != FAILED) {
            mResource.value = val;
        } else {
            QLOGE("Value could not be mapped for the resource 0x%x", mResource.qindex);
            ret = false;
        }
    }

    return ret;
}



