/******************************************************************************
  @file    Target.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __TARGET_SPECIFICS__H_
#define __TARGET_SPECIFICS__H_

#include "ResourceInfo.h"
#include "mp-ctl.h"

//TODO Remove the usage for this.
#define MAX_CORES 8

#define NODE_MAX 150
#define MAX_CLUSTER 2
#define GPU_FREQ_LVL 16

using namespace std;

class PerfDataStore;
/* Structure for defining value maps.
 * */
typedef struct value_map{
    int *map;
    int mapSize;
}value_map;

enum {
    CLUSTER_BASED_GOV_INSTANCE, /*both cluster based/core based*/
    SINGLE_GOV_INSTANCE,
    MAX_GOVINSTANCETYPE,
}IntGovInstanceType;

enum {
    INTERACTIVE, /*both cluster based/core based*/
    SCHEDUTIL,
    MAX_CPUFREQGOV,
}CpufreqGov;

/* Target class: It contains target
 * specific information. It is a singleton
 * class. On mpctl-server-init a object for this
 * class is initialized with the current target
 * specific information.
 * */
class Target {

private:
    int mSocID;
    bool mSyncCore;  /*true- if target is sync core*/
    unsigned short mNumCluster;  /*total number of cluster*/
    unsigned short mTotalNumCores;    /*total number of cores*/
    int *mCorePerCluster; /*number of cores per cluster*/
    int *mPhysicalClusterMap; /*logical to physical cluster map*/
    int *mPredefineClusterMap; /*pre-defined logical cluster to physical cluster map*/
    int *mLastCoreIndex; /*Maintain end core index for each physical cluster*/
    int mResolution;
    char *mTargetName; /*Store the name of that target*/
    char *mKernelVer; /*Store the kernel version*/

    /*resource supported bitmaps which will let us know which resources are
     *supported on this target. If minor types are greater than 32 then     *
     *this needs to be resized. This is a bitmap of 32bits per Major        *
     *resource. Every bit representing the minor resource in the major      *
     *resource.                                                             */
    unsigned long mResourceSupported[MAX_MAJOR_RESOURCES];

    int mCoreCtlCpu; /*core_ctl is enabled on which physical core*/
    int mMinCoreOnline; /*Minimum number of cores needed to be online*/

    /*Holds the latency to restrict to WFI
     * This value is target specific. Set this to 1 greater than the WFI
     * of the Power Cluster (cluster 0 pm-cpu "wfi"). Look for "wfi". This will
     * ensure the requirement to restrict a vote to WFI for both clusters
     */
    unsigned int mPmQosWfi;

    char mStorageNode[NODE_MAX];

    /* Value map is declared for each resource and defined
     * by a target for specific resources.*/
    struct value_map mValueMap[MAX_MINOR_RESOURCES];

    //defines which type of int gov instance target has
    //paths will vary based on this
    unsigned short mGovInstanceType;

    //returns cpufreq governor
    unsigned short mCpufreqGov;

    // Singelton object of this class
    static Target cur_target;

    /* Array for storing cpu max frequency reset value
     *   for each cluster. */
    unsigned int mCpuMaxFreqResetVal[MAX_CLUSTER];

    /* Array for storing available GPU frequencies */
    int mGpuAvailFreq[GPU_FREQ_LVL];
    int mGpuBusAvailFreq[GPU_FREQ_LVL];

    /* Store no. of gpu freq level */
    int mTotalGpuFreq;
    int mTotalGpuBusFreq;

    //boost configs, target configs and mappings store
    PerfDataStore *mPerfDataStore;

    /*
     * kernl base version
     */
    int mKernelVersion;
    bool mSchedBoostConcurrencySupported;
    const int min_kernel_version_with_sched_boost_concurrency_support = 4;

    void ExtendedTargetInit();
    void InitializeKPMnodes();

    void mLogicalPerfMapPerfCluster();
    void mLogicalPerfMapPowerCluster();
    void mCalculateCoreIdx();
    void mSetAllResourceSupported();
    void mResetResourceSupported(unsigned short, unsigned short);
    void mInitAllResourceValueMap();
    void mInitGpuAvailableFreq();
    void mInitGpuBusAvailableFreq();

    //init support routines
    int readSocID();
    int readResolution();
    void readKernelVersion();

    void readPmQosWfiValue();
    //ctor, copy ctor, assignment overloading
    Target();
    Target(Target const& oh);
    Target& operator=(Target const& oh);

public:
    ~Target();

    void InitializeTarget();
    void TargetInit();

    //get functions
    bool isSyncCore() {
        return mSyncCore;
    }
    unsigned short getNumCluster() {
        return mNumCluster;
    }
    unsigned short getTotalNumCores()  {
        return mTotalNumCores;
    }
    int getCoresInCluster(int cluster);
    int getPhysicalCluster(unsigned short logicalCluster);
    int getFirstCoreOfPerfCluster();
    int getLastCoreIndex(int cluster);
    int getPhysicalCore(int cluster, unsigned short logicalCore);
    bool isResourceSupported(unsigned short major, unsigned short minor);
    int getClusterForCpu(int cpu, int &startcpu, int &endcpu);
    int getMappedValue(int qindx, int val);
    int getAllCpusBitmask();
    unsigned int getCpuMaxFreqResetVal(int cluster);
    int findNextGpuFreq(int freq);
    int findNextGpuBusFreq(int freq);
    int getBoostConfig(int hintId, int type, int *mapArray, int *timeout);
    int getConfigPerflockNode(int resId, char *node, bool &supported);

    int getCoreCtlCpu() {
        return mCoreCtlCpu;
    }

    int getMinCoreOnline() {
        return mMinCoreOnline;
    }

    int getGovInstanceType() {
        return mGovInstanceType;
    }

    int getCpufreqGov() {
        return mCpufreqGov;
    }

    char* getStorageNode() {
        return mStorageNode;
    }

    unsigned int getMinPmQosLatency() {
        return mPmQosWfi;
    }

    int OverrideTables();

    int getResolution() {
        return mResolution;
    }

    char* getTargetName() {
        return mTargetName;
    }

    char* getKernelVersion() {
        return mKernelVer;
    }
    void Dump();

    int getSocID();

    //Initialize cur_target
    static Target &getCurTarget() {
        return cur_target;
    }

    bool isSchedBoostConcurrencySupported() {
        return mSchedBoostConcurrencySupported;
    }
};

#endif /*__TARGET_SPECIFICS__H_*/
