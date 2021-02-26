/******************************************************************************
  @file    ResourceInfo.h
  @brief   Implementation of performance resources

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __RESOURCE_INFO__H_
#define __RESOURCE_INFO__H_

#include "mp-ctl.h"

#define INVALID_OPCDE_TYPE -1
#define EXTRACT_VERSION     0xC0000000
#define EXTRACT_MAJOR_TYPE  0x1FC00000
#define EXTRACT_MINOR_TYPE  0x000FC000
#define EXTRACT_MAP_TYPE    0x00002000
#define EXTRACT_CLUSTER     0x00000F00
#define EXTRACT_CORE        0x00000070

#define SHIFT_BIT_VERSION     30
#define SHIFT_BIT_MAJOR_TYPE  22
#define SHIFT_BIT_MINOR_TYPE  14
#define SHIFT_BIT_CLUSTER      8
#define SHIFT_BIT_CORE         4
#define SHIFT_BIT_MAP         13

enum {
    CPU0,
    CPU1,
    CPU2,
    CPU3,
    CPU4,
    CPU5,
    CPU6,
    CPU7
};

enum {
    CLUSTER0,
    CLUSTER1
};

enum {
    SYNC_CORE,
    ASYNC_CORE,
    CORE_INDEPENDENT
};

//Needed for translation layer
#define CONVERT_IN_MHZ       100
#define CONVERT_IN_MSEC      10
#define UNSUPPORTED_Q_INDEX (MAX_DISPLAY_MINOR_OPCODE + MAX_PC_MINOR_OPCODE + \
                          MAX_CPUFREQ_MINOR_OPCODE + MAX_SCHED_MINOR_OPCODE + \
                          MAX_CORE_HOTPLUG_MINOR_OPCODE + MAX_INTERACTIVE_MINOR_OPCODE + \
                          MAX_CPUBW_HWMON_MINOR_OPCODE + MAX_VIDEO_MINOR_OPCODE + \
                          MAX_KSM_MINOR_OPCODE + MAX_OND_MINOR_OPCODE + MAX_GPU_MINOR_OPCODE + UNSUPPORTED_OPCODE)

#define DISPLAY_OFF_INDEX DISPLAY_OFF_MAJOR_OPCODE

typedef struct resource {
    unsigned char version;
    unsigned char major;
    unsigned char minor;
    unsigned char mapping;
    signed char cluster;
    signed char core;
    unsigned short qindex;
    union {
        unsigned int value;
        unsigned int level;
    };
}Resource;

class ResourceInfo {
    private:
        Resource mResource;

    public:
        ResourceInfo();
        ~ResourceInfo() {}
        ResourceInfo(ResourceInfo const& obj);
        ResourceInfo& operator=(ResourceInfo const& obj);
        bool operator==(ResourceInfo const& obj);
        bool operator!=(ResourceInfo const& obj);

        void ParseNewRequest(unsigned int opcode);
        unsigned short DiscoverQueueIndex();
        static int GetMinorIndex(int major);
        void Dump();

        //get and set functions
        unsigned short GetQIndex() {
            return mResource.qindex;
        }
        char GetCluster() {
            return mResource.cluster;
        }
        char GetCore() {
            return mResource.core;
        }
        unsigned char GetMapping() {
            return mResource.mapping;
        }
        unsigned char GetMajor() {
            return mResource.major;
        }
        unsigned char GetMinor() {
            return mResource.minor;
        }
        unsigned int GetValue() {
            return mResource.value;
        }
        Resource& GetResourceObject() {
            return mResource;
        }

        void SetMajor(unsigned char major) {
            mResource.major = major;
        }
        void SetMinor(unsigned char minor) {
            mResource.minor = minor;
        }
        void SetValue(unsigned int value) {
            mResource.value = value;
        }

    public:
        //pyhical translation layer
        bool TranslateToPhysical();
        bool TranslateValueMapToPhysical();
        bool TranslateToPhysicalOld();

};

#endif /*__RESOURCE_INFO__H_*/
