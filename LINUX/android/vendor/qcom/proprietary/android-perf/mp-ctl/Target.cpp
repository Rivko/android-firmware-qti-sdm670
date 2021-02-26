/******************************************************************************
  @file    Target.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET"
#include <cutils/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>

#include "Request.h"
#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"
#include "XmlParser.h"

Target Target::cur_target;

#define PROPERTY_PREDEFINED_CLUSTER_MAP "perflocks.predefined_clust_map"
#define PROPERTY_PREDEFINED_FREQUENCY_MAP "perflocks.predefined_freq_map"

//resolution nodes
#define RESOLUTION_DRM_NODE "/sys/class/drm/card0-DSI-1/modes"
#define RESOLUTION_GRAPHICS_NODE "/sys/class/graphics/fb0/modes"

#define PMQOSWFI_LATENCY_NODE "/sys/devices/system/cpu/cpu%d/cpuidle/state0/latency"

/*
 * Definition for various get functions
 * */
int Target::getCoresInCluster(int cluster) {
    if (mCorePerCluster) {
        if ((cluster >= 0) && (cluster < mNumCluster)) {
            int tmp = mCorePerCluster[cluster];
            if (tmp <= mTotalNumCores) {
                return tmp;
            }
        } else {
            QLOGE("Error: Invalid cluster id %d",cluster);
        }
    } else {
        QLOGE("Error: Target not initialized");
    }
    return -1;
}

int Target::getPhysicalCluster(unsigned short logicalCluster) {
    int cluster = -1;
    // If logicalCluster lies  within the range of the
    // physical clusters, then return the cluster from
    // the physicalCLusterMap.
    if (logicalCluster < mNumCluster) {
        if (mPhysicalClusterMap) {
            cluster = mPhysicalClusterMap[logicalCluster];
        }
    } else if ((logicalCluster >= START_PRE_DEFINE_CLUSTER) &&
               (logicalCluster < MAX_PRE_DEFINE_CLUSTER)) {
        //logical cluster lies in pre-defined cluster range
        if (mPredefineClusterMap) {
            cluster = mPredefineClusterMap[logicalCluster - START_PRE_DEFINE_CLUSTER];
        }
    } else {
        QLOGE("Error: Invalid logical cluster id %d", logicalCluster);
    }

    if (cluster > mNumCluster) {
        cluster = -1;
    }
    return cluster;
}

int Target::getFirstCoreOfPerfCluster() {
    int perfCluster = -1;
    int supportedCore = -1;

    if ((NULL == mPhysicalClusterMap) || (NULL == mLastCoreIndex)) {
        return supportedCore;
    }

    perfCluster = mPhysicalClusterMap[0];
    if ((perfCluster >= 0) && (perfCluster < mNumCluster)) {
        if (perfCluster == 0) {
            supportedCore = 0;
        } else {
            supportedCore = mLastCoreIndex[perfCluster-1] + 1;
        }
    }
    return supportedCore;
}

int Target::getPhysicalCore(int cluster, unsigned short logicalCore) {
    int physicalCore = -1;

    if(cluster < 0 || cluster >= mNumCluster)
        return physicalCore;

    if (cluster == 0) {
        physicalCore = logicalCore;
    } else {
        if (NULL != mLastCoreIndex) {
            physicalCore = logicalCore + mLastCoreIndex[cluster-1] + 1;
        }
    }
    if ((NULL != mLastCoreIndex) && (physicalCore > mLastCoreIndex[cluster])) {
        physicalCore = -1;
    }

    return physicalCore;
}

int Target::getLastCoreIndex(int cluster) {
    if (mLastCoreIndex) {
        if (cluster >=0 && cluster < mNumCluster) {
            return mLastCoreIndex[cluster];
        } else {
            QLOGE("Error: Invalid cluster id %d", cluster);
        }
    } else {
        QLOGE("Eror: Target not initialized");
    }
    return -1;
}

Target::Target() {
    QLOGI("Target constructor");
    mSyncCore = true;
    mNumCluster = 0;
    mTotalNumCores = 0;
    mCorePerCluster = NULL;
    mPhysicalClusterMap = NULL;
    mPredefineClusterMap = NULL;
    mLastCoreIndex = NULL;
    memset(mResourceSupported, 0x00, sizeof(mResourceSupported));
    mCoreCtlCpu = -1;
    mPmQosWfi = 0;
    mMinCoreOnline = 0;
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;
    mCpufreqGov = INTERACTIVE;
    memset(mValueMap, 0x00, sizeof(mValueMap));
    memset(mStorageNode, '\0', sizeof(mStorageNode));
    /* Setting reset value to UINT_MAX to avoid race condition with thermal engine.
     * UINT_MAX work as vote removed from perfd side */
    for (int i = 0; i < MAX_CLUSTER; i++)
         mCpuMaxFreqResetVal[i] = UINT_MAX;
    mResolution = PerfDataStore::MAP_RES_TYPE_ANY;
    mTargetName = NULL;
    mTotalGpuFreq = 0;
    mTotalGpuBusFreq = 0;
    mKernelVer = NULL;
}

Target::~Target() {
    QLOGI("Target destructor");

    if (mCorePerCluster) {
        delete [] mCorePerCluster;
        mCorePerCluster = NULL;
    }

    if (mPhysicalClusterMap){
        delete [] mPhysicalClusterMap;
        mPhysicalClusterMap = NULL;
    }

    if (mPredefineClusterMap){
        mPredefineClusterMap = NULL;
    }

    if (mLastCoreIndex) {
        delete [] mLastCoreIndex;
        mLastCoreIndex = NULL;
    }

    if (mTargetName) {
        delete [] mTargetName;
        mTargetName = NULL;
    }
}

/* Create a logical to physical cluster
 * mapping, in which Logical cluster "0"
 * maps to physical cluster "0". Useful
 * for the targets in which cluster0 is
 * the perf cluster.
 * */
void Target::mLogicalPerfMapPerfCluster() {
    int i;
    mPhysicalClusterMap = new int[mNumCluster];
    if(mPhysicalClusterMap) {
        for(i=0; i < mNumCluster; i++) {
            mPhysicalClusterMap[i] = i ;
        }
   } else {
     QLOGE("Error: Could not map Logical perf cluster to perf\n");
   }
}

/* Create a logical to physical cluster mapping
 * in which logical cluster "0" maps to power
 * cluster
 * */
void Target::mLogicalPerfMapPowerCluster() {
    int i, cluster = mNumCluster-1;
    mPhysicalClusterMap = new int[mNumCluster];
    if(mPhysicalClusterMap) {
        for(i=0; i < mNumCluster && cluster >= 0; i++,cluster--) {
            mPhysicalClusterMap[i] = cluster;
        }
    } else {
        QLOGE("Error: Could not map logical perf to power cluster\n");
    }
}

/* Calculate the number/index for last
 * core in this cluster
 * */
void Target::mCalculateCoreIdx() {
    int cumlative_core, i;
    mLastCoreIndex = new int[mNumCluster];
    if(mLastCoreIndex && mCorePerCluster) {
        cumlative_core = 0;
        for(i =0;i < mNumCluster; i++) {
            mLastCoreIndex[i] = cumlative_core + (mCorePerCluster[i] - 1);
            cumlative_core = mLastCoreIndex[i] + 1;
        }
    } else {
        QLOGE("Error: Initializing core index failed\n");
    }
}

/* Resource support functions. */

/* Return the bitmap value for the asked resource.
 * If Resource is supported it will return 1,
 * else 0.
 * */
bool Target::isResourceSupported(unsigned short major, unsigned short minor) {
    unsigned long tmp = 0;
    bool ret = false;

    if (major < MAX_MAJOR_RESOURCES) {
        tmp = mResourceSupported[major];
        ret = ((tmp >> minor) & 0x1);
    }

    return ret;
}

/* This function sets all the minor resources for a
 * major resource to be supported.
 * */
void Target::mSetAllResourceSupported() {
    unsigned int minorIdx,rshIdx;
    for (unsigned int i = 0; i < MAX_MAJOR_RESOURCES; i++) {
        minorIdx = ResourceInfo::GetMinorIndex(i);
        rshIdx = sizeof(unsigned long)*8 - minorIdx;
        mResourceSupported[i] = ((unsigned long)~0) >> rshIdx;
    }
}

/* This function resets the minor rsource in the specified
 * major resource to false (not supported).
 * */
void Target::mResetResourceSupported(unsigned short major, unsigned short minor) {
    unsigned long tmp;
    if (major < MAX_MAJOR_RESOURCES) {
        if (minor < ResourceInfo::GetMinorIndex(major)) {
            mResourceSupported[major] &= ~(1 << minor);
        }
    } /*if (major < MAX_MAJOR_RESOURCES)*/
}

/** This function returns the cluster number to which a
 * given cpu belongs. It also updates input parameters
 * with first cpu and last cpu in that cluster.
 * On error the return value is -1
 */
int Target::getClusterForCpu(int cpu, int &startcpu, int &endcpu) {
    int i, cluster = -1;
    bool success = false;

    startcpu = 0;
    endcpu = -1;

    if ((cpu < 0) || (cpu > mTotalNumCores) || (NULL == mLastCoreIndex)) {
        QLOGE("Invalid cpu number %d. Cannot find cluster.", cpu);
        return -1;
    }

    for (i = 0 ; i < mNumCluster && !success; i++) {
        endcpu = mLastCoreIndex[i];
        if (cpu >= startcpu && cpu <= endcpu) {
            cluster = i;
            success = true;
        }
        else {
            startcpu = mLastCoreIndex[i] + 1;
        }
    }

    if (!success){
        cluster = -1;
        startcpu = -1;
        endcpu = -1;
    }

    return cluster;
}


/* Functions to support value maps. */

/* This function initializes the valueMap for all
 * the resources. The default is "false" and
 * no map is specified.
 *
 * Whichever resource needs a map and has to be
 * specified should be done in specifc target
 * functions.
 * */
void Target::mInitAllResourceValueMap() {
    int i = 0;
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        mValueMap[i].mapSize = -1;
        mValueMap[i].map = NULL;
    }
}

int Target::readSocID() {
    int fd;
    int soc = -1;
    if (!access("/sys/devices/soc0/soc_id", F_OK)) {
        fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
    } else {
        fd = open("/sys/devices/system/soc/soc0/id", O_RDONLY);
    }
    if (fd != -1)
    {
        char raw_buf[5];
        read(fd, raw_buf,4);
        raw_buf[4] = 0;
        soc = atoi(raw_buf);
        close(fd);
    }
    else {
        close(fd);
    }
    mSocID = soc; /* set target socid */
    QLOGI("socid of the device: %d", soc);
    return soc;
}

int Target::getSocID() {
    return mSocID;
}

void Target::readPmQosWfiValue() {
    int fd = -1, i, n;
    char tmp[NODE_MAX];
    int latency = 0, max_latency = 0;
    const int MAX_BUF_SIZE = 16;
    char buf[MAX_BUF_SIZE];

    //read the PmQosWfi latency for all cpus to find the max value.
    for(i = 0; i < mTotalNumCores; i++) {
        snprintf(tmp, NODE_MAX, PMQOSWFI_LATENCY_NODE, i);
        fd = open(tmp, O_RDONLY);
        if (fd != -1) {
            memset(buf, 0x0, sizeof(buf));
            n = read(fd, buf, MAX_BUF_SIZE-1);
            if (n > 0) {
                latency = strtol(&buf[0], NULL, 0);
                max_latency = max(latency, max_latency);
            }
            close(fd);
        } else {
            QLOGW("Unable to read node = %s", tmp);
        }
    }

    if(max_latency == 0) {
        QLOGE("Unable to get mPmQosWfi latency, So initializing to default 0 value");
    } else {
        mPmQosWfi = max_latency + 1;
        QLOGI("mPmQosWfi latency caluculated as = %u", mPmQosWfi);
    }
}

void Target::readKernelVersion() {
    char tmp[NODE_MAX];
    int fd = -1, n;
    const int MAX_BUF_SIZE = 16;
    char buf[MAX_BUF_SIZE];


    fd = open(KERNEL_VERSION_NODE, O_RDONLY);
    if (fd != -1) {
        memset(buf, 0x0, sizeof(buf));
        n = read(fd, buf, MAX_BUF_SIZE-1);
        if (n > 0 && n < MAX_BUF_SIZE) {
            mKernelVer = new char[MAX_BUF_SIZE];
            if (mKernelVer != NULL)
                strlcpy(mKernelVer, buf, n);
            else
                QLOGE("Couldn't allocate memory for mKernelVer");
        } else
            QLOGE("Kernel Version node not present");
    }
    close(fd);
}
int Target::readResolution() {
    int fd = -1;
    int ret = PerfDataStore::MAP_RES_TYPE_ANY, n = 0;
    int w = 0, h = 0, t = 0;
    char *pch = NULL;
    const int MAX_BUF_SIZE = 16;

    //use the new node if present to get the resolution.
    if (!access(RESOLUTION_DRM_NODE, F_OK)) {
        fd = open(RESOLUTION_DRM_NODE, O_RDONLY);
    } else {
        fd = open(RESOLUTION_GRAPHICS_NODE, O_RDONLY);
    }

    if (fd != -1) {
        char buf[MAX_BUF_SIZE];
        memset(buf, 0x0, sizeof(buf));
        n = read(fd, buf, MAX_BUF_SIZE-1);
        if (n > 0) {
            //parse U:1080x1920p-60 (or) 1440x2560
            if (isdigit(buf[0])) {
                w = atoi(&buf[0]); //for new node
            } else {
                w = atoi(&buf[2]);//for old node
            }
            pch = strchr(buf, 'x');
            if (NULL != pch) {
                h = atoi(pch+1);
            }

            if (h && (w/h > 0)) {
                t = w;
                w = h;
                h = t;
            }

            //convert resolution to enum
            if (w <= 800 && h <= 1280) {
                ret = PerfDataStore::MAP_RES_TYPE_720P;
            }
            else if (w <= 1080 && h <= 1920) {
                ret = PerfDataStore::MAP_RES_TYPE_1080P;
            }
            else if (w <= 1440 && h <= 2560) {
                ret = PerfDataStore::MAP_RES_TYPE_2560;
            }
        }
        close(fd);
    }
    mResolution = ret; /* set the resolution*/
    QLOGI("Resolution of the device: %d", ret);
    return ret;
}

/* This function returns the mapped value for
 * a major resource and minor resource, specified
 * through the index.
 * */
int Target::getMappedValue(int resrcIndex, int val) {
    if ((resrcIndex >= 0) && (resrcIndex < MAX_MINOR_RESOURCES)
           && (resrcIndex != UNSUPPORTED_Q_INDEX)) {
        if(mValueMap[resrcIndex].map != NULL) {
            if (val >= 0 && val < mValueMap[resrcIndex].mapSize) {
                return mValueMap[resrcIndex].map[val];
            } else {
                QLOGE("Value Map not defined for this %d index for the %d value", resrcIndex, val);
            }
        } else {
            QLOGE("Value map not defined for this %d index", resrcIndex);
        }
    } else {
        QLOGE("Error: Cannot find mapped value for the resource with index %d", resrcIndex);
    }
    return -1;
}

/* Calculate bitmask of all CPUs in target.
 * For example, if total number of CPUs is 4,
 * then bitmask will be 0b1111 (or 0xF).
 * */
int Target::getAllCpusBitmask() {
    if(mTotalNumCores) {
        return (1 << mTotalNumCores) - 1;
    } else {
        QLOGE("Error: Initializing total cores failed\n");
        return -1;
    }
}

int Target::OverrideTables() {
    char property[PROPERTY_VALUE_MAX];
    int idx = -1, value = -1;
    char *ch = NULL;
    char *tmp_token = NULL;

    if (property_get(PROPERTY_PREDEFINED_CLUSTER_MAP, property, NULL) > 0) {
        QLOGI("  PROPERTY_PREDEFINED_CLUSTER_MAP_INDEX to %s", property);
        //ex. expect input "0,1,1,0,2,1,3,1"

        ch = strtok_r(property, ",", &tmp_token);
        while (NULL != ch) {
            idx = atoi(ch);
            ch = strtok_r(NULL, ",", &tmp_token);
            if (NULL != ch) {
                value = atoi(ch);
                if ((idx >= START_PRE_DEFINE_CLUSTER) && (idx < MAX_PRE_DEFINE_CLUSTER) &&
                    (value >= 0) && (value < mNumCluster) && (NULL != mPredefineClusterMap)) {
                    mPredefineClusterMap[idx] = value;
                }
            }
            ch = strtok_r(NULL, ",", &tmp_token);
        } /*while (NULL != ch)*/
    }

    if (property_get(PROPERTY_PREDEFINED_FREQUENCY_MAP, property, NULL) > 0) {
        QLOGI("  PROPERTY_PREDEFINED_FREQUENCY_MAP to %s", property);
        //ex. expect input "0,800,1,1600,2,2300"
        ResourceInfo tmpr;
        int qindx1 = 0, qindx2 = 0;

        tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
        tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
        qindx1 = tmpr.DiscoverQueueIndex();
        tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
        qindx2 = tmpr.DiscoverQueueIndex();

        ch = strtok_r(property, ",", &tmp_token);
        while (NULL != ch) {
            idx = atoi(ch);
            ch = strtok_r(NULL, ",", &tmp_token);
            if (NULL != ch) {
                value = atoi(ch);
                if ((idx >= LOWEST_FREQ) && (idx < FREQ_MAP_MAX) &&
                    (value > 0)) {
                    if (mValueMap[qindx1].map) {
                        mValueMap[qindx1].map[idx] = value;
                    }
                    if (mValueMap[qindx2].map) {
                        mValueMap[qindx2].map[idx] = value;
                    }
                }
            } /*if (NULL != ch)*/
            ch = strtok_r(NULL, ",", &tmp_token);
        } /*while (NULL != ch)*/
    }
    return 0;
}

/* Returns the value to be written on cpu max freq
   sysfs node after perflock release.
*/
unsigned int Target::getCpuMaxFreqResetVal(int cluster) {
    if (cluster >= 0 && cluster < MAX_CLUSTER) {
        return mCpuMaxFreqResetVal[cluster];
    }
    return FAILED;
}

/* Store all the available GPU freq in an integer array */
void Target::mInitGpuAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGI("Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuFreq >= GPU_FREQ_LVL) {
                    QLOGE("Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuAvailFreq[mTotalGpuFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuAvailFreq, mGpuAvailFreq + mTotalGpuFreq);
    } else {
        QLOGE("Initialization of GPU available freq failed, as %s not present", GPU_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int Target::findNextGpuFreq(int freq) {
    int i = 0;

    for (i = 0; i < mTotalGpuFreq; i++) {
        if (mGpuAvailFreq[i] >= freq) {
            return mGpuAvailFreq[i];
        }
    }

    if ((mTotalGpuFreq != 0) && (i ==  mTotalGpuFreq)) {
        return mGpuAvailFreq[mTotalGpuFreq-1];
    }

    return FAILED;
}

/* Store all the available GPU bus freq in an integer array */
void Target::mInitGpuBusAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_BUS_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGI("Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuBusFreq >= GPU_FREQ_LVL) {
                    QLOGE("Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuBusAvailFreq[mTotalGpuBusFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuBusAvailFreq, mGpuBusAvailFreq + mTotalGpuBusFreq);
    } else {
        QLOGE("Initialization of GPU Bus available freq failed, as %s not present", GPU_BUS_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int Target::findNextGpuBusFreq(int freq) {
    int i = 0;

    for (i = 0; i < mTotalGpuBusFreq; i++) {
        if (mGpuBusAvailFreq[i] >= freq) {
            return mGpuBusAvailFreq[i];
        }
    }

    if ((mTotalGpuBusFreq != 0) && (i ==  mTotalGpuBusFreq)) {
        return mGpuBusAvailFreq[mTotalGpuBusFreq-1];
    }

    return FAILED;
}

int Target::getBoostConfig(int hintId, int type, int *mapArray, int *timeout) {
    int size = 0;

    if (NULL == mapArray) {
        return size;
    }

    if (NULL != mPerfDataStore) {
        size = mPerfDataStore->GetBoostConfig(hintId, type, mapArray, timeout,
                                                  mTargetName, mResolution);
    }
    return size;
}

int Target::getConfigPerflockNode(int resId, char *node, bool &supported) {
    int ret = FAILED;

    if (resId < 0) {
        return 0;
    }

    if (NULL != mPerfDataStore) {
        ret = mPerfDataStore->GetResourceConfigNode(resId, node, supported);
    }
    return ret;
}

void Target::Dump() {
    int i;
    QLOGI("Number of cluster %d \n", mNumCluster);
    QLOGI("Number of cores %d \n", mTotalNumCores);
    for (i =0; i < mNumCluster; i++) {
        if (mCorePerCluster) {
            QLOGI("Cluster %d has %d cores", i, mCorePerCluster[i]);
        }
        if (mPhysicalClusterMap) {
            QLOGI("Logical cluster %d is mapped to physical cluster %d", i, mPhysicalClusterMap[i]);
        }
        if (mLastCoreIndex) {
            QLOGI("End index for physical cluster %d is %d",i, mLastCoreIndex[i]);
        }
        QLOGI("Cluster %d can have max limit frequency of %u ", i, mCpuMaxFreqResetVal[i]);
    }
    for (i=0; i < MAX_PRE_DEFINE_CLUSTER-START_PRE_DEFINE_CLUSTER; i++) {
        if (mPredefineClusterMap) {
            QLOGI("Logical pre-defined cluster %d is mapped to physical cluster %d",
                  i+START_PRE_DEFINE_CLUSTER, mPredefineClusterMap[i]);
        }
    }
    QLOGI("Core_ctl_cpu %d", mCoreCtlCpu);
    QLOGI("min_core_online %d", mMinCoreOnline);
    QLOGI("SyncCore_value %d",mSyncCore);
    QLOGI("PmQosWfi_latency %u", mPmQosWfi);
    QLOGI("GovInstance_type %d",mGovInstanceType);
    QLOGI("CpufreqGov_type %d", mCpufreqGov);

    return;
}
