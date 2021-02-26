/******************************************************************************
  @file    TargetInit.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET-INIT"
#include <cutils/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <sys/utsname.h>

#include "Request.h"

#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"

#define KPM_NUM_CLUSTERS        "/sys/module/msm_performance/parameters/num_clusters"
#define KPM_MANAGED_CPUS        "/sys/module/msm_performance/parameters/managed_cpus"
#define KPM_CPU_MAX_FREQ_NODE       "/sys/module/msm_performance/parameters/cpu_max_freq"

// Adding define for min freq nodes
#define PROP_MIN_FREQ_0    "ro.min_freq_0"
#define PROP_MIN_FREQ_4    "ro.min_freq_4"

/* A single TargetInit function for all the targets which
   parses XML file for target configs.
 * For adding a new Target, create a targetconfig.xml file.
   And for any other the target specific initializations provided
   an ExtendedTargetInit fucntion which works on Socids.
*/

//Default frequency map for value mapping
int msmDefaultFreqMap[FREQ_MAP_MAX] = {
    800,    /*LOWEST_FREQ*/
    1100,   /*LEVEL1_FREQ*/
    1300,   /*LEVEL2_FREQ*/
    1500,   /*LEVEL3_FREQ*/
    1650    /*HIGHEST_FREQ*/
};

//Default pre-defined cluster map .
int msmDefaultPreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,  /*LAUNCH_CLUSTER*/
    1,  /*SCROLL_CLUSTER*/
    1,  /*ANIMATION_CLUSTER*/
};

/* Init Sequence
 * 1. Pouplate the target Config
 * 2. Init the target
 * 3. Populate the target resources config
 * 4. Init the resources node
 */

void Target::InitializeTarget() {
    int ret = 0;
    int socid = 0;
    char tmp_s[NODE_MAX];
    int rc = 0;
    int qindx = 0;
    char clk_scaling_s[NODE_MAX];
    int res = 0;
    ResourceInfo tmpr;

    QLOGI("Inside InitializeTarget");
    mSetAllResourceSupported();
    mInitAllResourceValueMap();
    mResetResourceSupported(MISC_MAJOR_OPCODE, UNSUPPORTED_OPCODE);
    mInitGpuAvailableFreq();
    mInitGpuBusAvailableFreq();

    socid = readSocID();

    //get resolution
    (void) readResolution();
    res = getResolution();

    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;

    // Identifying storage device type. (UFS/EMMC)
    FREAD_STR(STORAGE_UFS_CLK_SCALING_DISABLE, clk_scaling_s, NODE_MAX, rc);
    if (rc > 0) {
        strlcpy(mStorageNode, STORAGE_UFS_CLK_SCALING_DISABLE,
                strlen(STORAGE_UFS_CLK_SCALING_DISABLE)+1);
    } else {
        strlcpy(mStorageNode, STORAGE_EMMC_CLK_SCALING_DISABLE,
                strlen(STORAGE_EMMC_CLK_SCALING_DISABLE)+1);
    }

    /*
     * Initialize kernel version
     */
    struct utsname utsname_buf;
    rc = uname(&utsname_buf);
    mKernelVersion = -1;
    mSchedBoostConcurrencySupported = false;
    // uname success on rc==0
    if (!rc) {
        rc = sscanf(utsname_buf.release, "%d.", &mKernelVersion);
        if (rc != 1) {
            mKernelVersion = -1;
            QLOGE("sscanf failed, kernel version set to -1");
        } else {
            mSchedBoostConcurrencySupported =
              (min_kernel_version_with_sched_boost_concurrency_support <=
                  mKernelVersion);
            QLOGI("kernel version is %d, sched_boost Cocurrency supported? %d"
                        , mKernelVersion, mSchedBoostConcurrencySupported);
        }
    } else
        QLOGE("uname failed, kernel version set to -1");

    rc = 0;
    //populate boost conigs, target configs  & params mapping tables
    mPerfDataStore = PerfDataStore::getPerfDataStore();
    if (NULL != mPerfDataStore) {
        mPerfDataStore->Init();

        /* All the target related information parsed from XML file are initialized in the TargetInit()
        function which remains same for all the targets. For any target specific initializations provided
        an ExtendedTargetInit() function, which works on SocId. */

        TargetInit();
        ExtendedTargetInit();

        /* Init for per target resource file.
         * Moved the call after TargetInit as it need target name
         */
        mPerfDataStore->TargetResourcesInit();

        //cluster map from xml
        ret = mPerfDataStore->GetClusterMap(&mPredefineClusterMap, mTargetName);
    }

    //default cluster map if not available
    if ((NULL == mPredefineClusterMap) || !ret) {
        mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    }

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    if (NULL != mPerfDataStore) {
        mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res, &mValueMap[qindx].map, mTargetName);
    }

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    if (NULL != mPerfDataStore) {
        mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res,
                                           &mValueMap[qindx].map, mTargetName);
    }

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }

    //overriding predefined target based tables with values from
    //read from system properties
    OverrideTables();
}

void invoke_wa_libs() {
#define KERNEL_WA_NODE "/sys/module/app_setting/parameters/lib_name"
   int rc;
   const char *wl_libs[] = {
      "libvoH264Dec_v7_OSMP.so",
      "libvossl_OSMP.so",
      "libTango.so"
   };
   int i;

   int len = sizeof (wl_libs) / sizeof (*wl_libs);

   for(i = 0; i < len; i++) {
      FWRITE_STR(KERNEL_WA_NODE, wl_libs[i], strlen(wl_libs[i]), rc);
      QLOGI("Writing to node (%s)  (%s) rc:%d\n", KERNEL_WA_NODE, wl_libs[i], rc);
   }

}

void Target::TargetInit() {

    char tmp_s[NODE_MAX];
    int rc = 0;

    if (NULL == mPerfDataStore) {
        QLOGE("Initialization of PerfDataStore failed");
        return;
    }

    PerfDataStore::TargetConfigInfo *config = mPerfDataStore->getTargetConfig(mSocID);
    if (NULL == config) {
        QLOGE("Initialization of TargetConfigInfo Object failed");
        return;
    }

    if (!config->mUpdateTargetInfo) {
        QLOGE("Target Initialized with default available values, as mistake in target config XML file");
        return;
    }

    if(config->mCalculatedCores != config->mTotalNumCores) {
        QLOGE("Target Initialized with default values, as mismatch between the TotalNumCores and CalculatedCores.");
        return;
    }

    if (config->mTargetName) {
        mTargetName = new char[TARG_NAME_LEN];
        if (mTargetName) {
            strlcpy(mTargetName, config->mTargetName, TARG_NAME_LEN);
        }
    }
    QLOGI("Init %s",mTargetName ? mTargetName : "Target");

    mNumCluster = config->mNumCluster;
    mTotalNumCores = config->mTotalNumCores;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster) {
        for (int i=0;i<mNumCluster;i++) {
            mCorePerCluster[i] = config->mCorepercluster[i];
            mCpuMaxFreqResetVal[i] = config->mCpumaxfrequency[i];
        }
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }

    mSyncCore = config->mSyncCore;
    if (config->mCoreCtlCpu < 0 || config->mCoreCtlCpu >= mTotalNumCores) {
        QLOGW("CoreCtlCpu is incorrectly specified in XML file, So Initializing to -1");
    } else {
        mCoreCtlCpu = config->mCoreCtlCpu;
    }

    if (config->mMinCoreOnline < 0 || config->mMinCoreOnline > mTotalNumCores) {
        QLOGW("MinCoreOnline is incorrectly specified in XML file, So Initializing to 0");
    } else {
        mMinCoreOnline = config->mMinCoreOnline;
    }

    if (config->mGovInstanceType < 0 || config->mGovInstanceType >= MAX_GOVINSTANCETYPE) {
        QLOGW("GovInstanceType is incorrectly specified in XML file, So Initializing to CLUSTER_BASED_GOV_INSTANCE");
    } else {
        mGovInstanceType = config->mGovInstanceType;
    }

    if (config->mCpufreqGov < 0 || config->mCpufreqGov >= MAX_CPUFREQGOV) {
        QLOGW("CpufreqGov is incorrectly specified in XML file, So Initializing to INTERACTIVE");
    } else {
        mCpufreqGov = config->mCpufreqGov;
    }

    readKernelVersion();
    readPmQosWfiValue();
    mCalculateCoreIdx();
    if (config->mType == 0) {
        //cluster 0 is big
        mLogicalPerfMapPerfCluster();
    } else {
        //cluster 0 is little
        mLogicalPerfMapPowerCluster();
    }

    /*Deleting target configs vector, after target initialized with the required values
      which are parsed from the XML file. As the vector is not needed anymore.*/
    while (!mPerfDataStore->mTargetConfigs.empty()) {
        config = mPerfDataStore->mTargetConfigs.back();
        if (config) {
            delete config;
        }
        mPerfDataStore->mTargetConfigs.pop_back();
    }
}

//KPM node initialization
void Target::InitializeKPMnodes() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if (rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
        snprintf(tmp_s, NODE_MAX , "%d", mNumCluster);
        FWRITE_STR(KPM_NUM_CLUSTERS, tmp_s, strlen(tmp_s), rc);

        if(!mCorePerCluster)
            return;

        //Initializing KPM nodes for each cluster, with their cpu ranges and max possible frequencies.
        for (int i=0, prevcores = 0; i<mNumCluster; i++) {
            snprintf(tmp_s, NODE_MAX , "%d-%d", prevcores, prevcores+mCorePerCluster[i]-1);
            FWRITE_STR(KPM_MANAGED_CPUS, tmp_s, strlen(tmp_s), rc);

            rc = update_freq_node(prevcores, prevcores+mCorePerCluster[i]-1 , mCpuMaxFreqResetVal[i] , tmp_s, NODE_MAX);
            if (rc >= 0) {
                FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
            }
            prevcores += mCorePerCluster[i];
        }
    }
}

void Target::ExtendedTargetInit() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    switch (mSocID) {
    case 246: /*8996*/
    case 291: /*8096*/
        property_set(PROP_MIN_FREQ_0, "384000");
        property_set(PROP_MIN_FREQ_4, "384000");
        invoke_wa_libs();
        break;

    case 305: /*8996Pro*/
    case 312: /*8096Pro*/
        invoke_wa_libs();
        break;

    case 206:
    case 247:
    case 248:
    case 249:
    case 250: /*msm8916*/
        property_set(PROP_MIN_FREQ_0, "800000");
        break;

    case 239:
    case 263:
    case 241:
    case 268:
    case 269:
    case 270:
    case 271: /*msm8939*/
        property_set(PROP_MIN_FREQ_0, "800000");
        property_set(PROP_MIN_FREQ_4, "499200");
        break;
    }

    InitializeKPMnodes();
    Dump();
}
