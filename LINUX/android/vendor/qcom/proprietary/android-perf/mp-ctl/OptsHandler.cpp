/******************************************************************************
  @file    OptsHandler.cpp
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <dirent.h>


#define LOG_TAG "ANDR-PERF-OPTSHANDLER"
#include <cutils/log.h>

#include "Request.h"

#include "MpctlUtils.h"
#include "OptsData.h"
#include "OptsHandler.h"
#include "ResourceQueues.h"
#include "Target.h"

#define ENABLE_PREFER_IDLE  1
#define WRITE_MIGRATE_COST  0

#define DISABLE_PC_LATENCY  1
#define MAX_LPM_BIAS_HYST 100
#define DEFAULT_LPM_BIAS_HYST 0

//Assumes max # cpus == 32
#define CPUSET_STR_SIZE 86

#define RELEASE_LOCK 0
#define ACQUIRE_LOCK 1


static pthread_t id_ka_thread;
static bool tKillFlag = true;
static bool isThreadAlive = false;
pthread_mutex_t ka_mutex = PTHREAD_MUTEX_INITIALIZER;


OptsHandler OptsHandler::mOptsHandler;

OptsHandler::OptsHandler() {
    InitializeOptsTable();
}

OptsHandler::~OptsHandler() {
}

int OptsHandler::Init() {
    return 0;
}

void OptsHandler::InitializeOptsTable() {
/* Assuming that all the information related to resources, such as CompareOpts., are dependend only the
resource and not on targets. If this assumption changes it's better to move mOptsTable to TargetInit.
 * Assigning default ApplyOts and ResetOpts function to all the resources and also CompareOpts to
 higher_is_better by default. If any resource does not follow the default types, they need to be changed.*/

    int idx = 0;
    for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
        mOptsTable[idx] = {modify_sysfsnode, modify_sysfsnode, higher_is_better};
    }

    /* changing CompareOpts to lower_is_better basing on the resource index, which is
       the sum of it's major group start id and minor id. */
    mOptsTable[POWER_COLLAPSE_START_INDEX + L2_POWER_COLLAPSE_PERF_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_UPMIGRATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_DOWNMIGRATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_STATIC_CPU_PWR_COST_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_RESTRICT_CLUSTER_SPILL_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_GROUP_UPMIGRATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_GROUP_DOWNMIGRATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_FREQ_AGGR_THRESHOLD_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_RATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_SLACK_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_MAX_FREQ_HYSTERESIS_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + SCHEDUTIL_HISPEED_LOAD_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[ONDEMAND_START_INDEX + OND_SAMPLING_RATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[GPU_START_INDEX + GPU_MIN_POWER_LEVEL].mCompareOpts = lower_is_better;

    /* changing CompareOpts to always_apply basing on the resource index.*/
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_TOP_APP_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_SYSTEM_BACKGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_BACKGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHEDTUNE_PREFER_IDLE_OPCODE].mCompareOpts = always_apply;
    mOptsTable[MISC_START_INDEX + STORAGE_CLK_SCALING_DISABLE_OPCODE].mCompareOpts = always_apply;

    /* changing ApplyOts and ResetOpts functions to call different function.*/
    mOptsTable[DISPLAY_START_INDEX + DISPLAY_OFF_OPCODE] = {dummy, dummy, higher_is_better};

    mOptsTable[POWER_COLLAPSE_START_INDEX + POWER_COLLAPSE_OPCODE] =
                        {pmQoS_cpu_dma_latency, pmQoS_cpu_dma_latency, lower_is_better};

    /* cpu freq */
    mOptsTable[CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE] = {cpu_options, cpu_options, higher_is_better};
    mOptsTable[CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE] = {cpu_options, cpu_options, higher_is_better};

    /* sched group */
    mOptsTable[SCHED_START_INDEX + SCHED_BOOST_OPCODE] = {sched_boost, sched_boost, lower_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_GROUP_OPCODE] = {add_sched_group, reset_sched_group, always_apply};
    mOptsTable[SCHED_START_INDEX + SCHED_FREQ_AGGR_GROUP_OPCODE] =
                        {sched_add_freq_aggr_group, sched_reset_freq_aggr_group, always_apply};

    /* hotplug */
    mOptsTable[CORE_HOTPLUG_START_INDEX + CORE_HOTPLUG_MIN_CORE_ONLINE_OPCODE] =
                        {lock_min_cores, lock_min_cores, higher_is_better};
    mOptsTable[CORE_HOTPLUG_START_INDEX + CORE_HOTPLUG_MAX_CORE_ONLINE_OPCODE] =
                        {lock_max_cores, lock_max_cores, lower_is_better};

    /* cpubw hwmon */
    mOptsTable[CPUBW_HWMON_START_INDEX + CPUBW_HWMON_HYST_OPT_OPCODE] =
                        {cpubw_hwmon_hyst_opt, cpubw_hwmon_hyst_opt, higher_is_better};

    /* video */
    mOptsTable[VIDEO_START_INDEX + VIDEO_ENCODE_PB_HINT] =
                        {handle_vid_encplay_hint, handle_vid_encplay_hint, higher_is_better};
    mOptsTable[VIDEO_START_INDEX + VIDEO_DECODE_PB_HINT] =
                        {handle_vid_decplay_hint, handle_vid_decplay_hint, higher_is_better};
    mOptsTable[VIDEO_START_INDEX + VIDEO_DISPLAY_PB_HINT] =
                        {handle_disp_hint, handle_disp_hint, higher_is_better};

    /* ksm */
    mOptsTable[KSM_START_INDEX + KSM_ENABLE_DISABLE_OPCODE] =
                        {disable_ksm, enable_ksm, higher_is_better};
    mOptsTable[KSM_START_INDEX + KSM_SET_RESET_OPCODE] =
                        {set_ksm_param, reset_ksm_param, higher_is_better};

    /* gpu */
    mOptsTable[GPU_START_INDEX + GPU_DISABLE_GPU_NAP_OPCODE] =
                        {gpu_disable_gpu_nap, gpu_disable_gpu_nap, higher_is_better};

    /* miscellaneous */
    mOptsTable[MISC_START_INDEX + UNSUPPORTED_OPCODE] = {unsupported, unsupported, higher_is_better};
    mOptsTable[MISC_START_INDEX + IRQ_BAL_OPCODE] = {irq_balancer, irq_balancer, higher_is_better};
    mOptsTable[MISC_START_INDEX + NET_KEEP_ALIVE_OPCODE] = {keep_alive, dummy, higher_is_better};

    /*llcbw hwmon*/
    mOptsTable[LLCBW_HWMON_START_INDEX + LLCBW_HWMON_HYST_OPT_OPCODE] =
                        {llcbw_hwmon_hyst_opt, llcbw_hwmon_hyst_opt, higher_is_better};
    /* memlat */
    mOptsTable[MEMLAT_START_INDEX + L3_MEMLAT_MINFREQ_OPCODE] = {l3_min_freq, l3_min_freq, higher_is_better};
}

int OptsHandler::ApplyOpt(Resource &resObj) {
    int ret = FAILED;
    int idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx >= 0 && idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mApplyOpts(resObj, dataObj);
    } else {
        QLOGE("Failed to call apply optimization for 0x%x", resObj.qindex);
    }
    return ret;
}

int OptsHandler::ResetOpt(Resource &resObj) {
    int ret = FAILED;
    int idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx >= 0 && idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mResetOpts(resObj, dataObj);
    } else {
        QLOGE("Failed to call reset optimization for 0x%x", idx);
    }
    return ret;
}

int OptsHandler::CompareOpt(int qindx, unsigned int reqVal, unsigned int curVal) {
    int ret = ADD_NEW_REQUEST;

    if (curVal != 0) {
        //First resource id present
        if (qindx >=0 && qindx < MAX_MINOR_RESOURCES) {
            ret = mOptsTable[qindx].mCompareOpts(reqVal, curVal);
        } else {
            QLOGE("Cannot find a compareOpt function");
            return FAILED;
        }
    }
    return ret;
}

/*Function for reqval overflow check during multiplication*/
unsigned int OptsHandler::MultiplyReqval(unsigned int rval, unsigned int factor) {
    unsigned int fin_rval;
    if (rval < (UINT_MAX/factor)) {
       fin_rval = rval * factor;
    } else {
       fin_rval = UINT_MAX;
    }
    return fin_rval;
}

int OptsHandler::ValidateClusterAndCore(int cluster, int core, int resourceStatus) {
    Target &t = Target::getCurTarget();
    int supportedCore = -1, core_no = -1;

    //First check cluster and core are in valid range
    if ((cluster < 0) || (cluster > t.getNumCluster())) {
        QLOGE("Invalid cluster no. %d", cluster);
        return FAILED;
    }
    if ((core < 0) || (core > t.getLastCoreIndex(cluster))) {
        QLOGE("Invalid core no. %d", core);
        return FAILED;
    }

    //Second check resource is supported on that core or not
    core_no = t.getLastCoreIndex(0)+1;
    switch (resourceStatus) {
        case SYNC_CORE: //Resource is sync core based, only first core of cluster is supported
            if ((core != 0) && (core != core_no)) {
                QLOGE("Core %d is not supported for this perflock resource, instead use core 0/%d", core, core_no);
                return FAILED;
            }
            break;
       case ASYNC_CORE: //Resource is async core based, all core accepted
            QLOGI("Core %d is supported for this perflock resource", core);
            break;
       case CORE_INDEPENDENT: //Resource does not depend on core, only first core of perf cluster is accepted
            supportedCore = t.getFirstCoreOfPerfCluster();
            if ((supportedCore == -1) || (core != supportedCore)) {
                QLOGE("Core %d is not supported for this perflock resource, instead use core %d", core, supportedCore);
                return FAILED;
            }
            break;
       default:
            QLOGE("Invalid resource based core status");
            return FAILED;
    }
    return SUCCESS;
}

/*All possible modifications to the requested value and any pre acquire node
updations can to be done in this function and finally update the acqval with
the string with which the requested node needs to be acquired.*/
int OptsHandler::CustomizeRequestValue(Resource &r, OptsData &d, char *acqval) {
    unsigned int reqval = r.value;
    int idx = r.qindex, rc = FAILED, cpu = -1, setval = FAILED;
    char *tmp_here = NULL, tmp_node[NODE_MAX] = "";
    bool valid_bound = true;
    unsigned int valCluster1 = 0, valCluster2 = 0;
    int hwmon_max_freq_index = 0, hwmon_min_freq_index = 0;
    Target &target = Target::getCurTarget();

    switch (idx) {
    case POWER_COLLAPSE_START_INDEX + L2_POWER_COLLAPSE_PERF_OPCODE:
    case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
    case SCHED_START_INDEX + SCHED_ENABLE_THREAD_GROUPING_OPCODE:
    case SCHED_START_INDEX + SCHED_RESTRICT_CLUSTER_SPILL_OPCODE:
    case SCHED_START_INDEX + SCHEDTUNE_PREFER_IDLE_OPCODE:
    case ONDEMAND_START_INDEX + OND_IO_IS_BUSY_OPCODE:
    case ONDEMAND_START_INDEX + OND_ENABLE_STEP_UP:
    case INTERACTIVE_START_INDEX + INTERACTIVE_BOOST_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_IO_IS_BUSY_OPCODE:
    case INTERACTIVE_START_INDEX + SCHEDUTIL_PREDICTIVE_LOAD_OPCODE:
        reqval = !!reqval;
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case SCHED_START_INDEX + SCHED_MIGRATE_COST_OPCODE:
        snprintf(acqval, NODE_MAX, "%d", WRITE_MIGRATE_COST);
        break;

    case SCHED_START_INDEX + SCHED_CPUSET_TOP_APP_OPCODE:
    case SCHED_START_INDEX + SCHED_CPUSET_SYSTEM_BACKGROUND_OPCODE:
    case SCHED_START_INDEX + SCHED_CPUSET_BACKGROUND_OPCODE:
        tmp_here = cpuset_bitmask_to_str(reqval);
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            delete[] tmp_here;
        } else {
             QLOGE("Failed to get the cpuset bitmask for requested %u value", reqval);
             return FAILED;
        }
        break;

    case SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE:
        /* Assumption, foreground/boost/cpus and foreground/cpus have same
        bitmask all the times. During acquire first update foreground/boost/cpus
        follwed by foreground/cpus and during release first release foreground/cpus
        foolowed by foreground/boost/cpus */
        tmp_here = cpuset_bitmask_to_str(reqval);
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            FWRITE_STR(SCHED_FOREGROUND_BOOST, acqval, strlen(acqval), rc);
            QLOGI("Updated %s with %s, return value %d", SCHED_FOREGROUND_BOOST, acqval, rc);
            delete[] tmp_here;
        } else {
            QLOGE("Failed to get the cpuset bitmask for requested value = %u", reqval);
            return FAILED;
        }
        break;

    case POWER_COLLAPSE_START_INDEX + LPM_BIAS_HYST_OPCODE:
        if (reqval <= 0 || reqval >= MAX_LPM_BIAS_HYST) {
             QLOGI("Requested value is %u out of limits, resetting to default bias of %d",
                                                        reqval, DEFAULT_LPM_BIAS_HYST);
             reqval = DEFAULT_LPM_BIAS_HYST;
         }
         snprintf(acqval, NODE_MAX, "%d", reqval);
         break;

    //todo: check this
    case INTERACTIVE_START_INDEX + INTERACTIVE_USE_SCHED_LOAD_OPCODE:
    //todo: need to ask power team to send a value instead of setting it to zero over here
    case INTERACTIVE_START_INDEX + INTERACTIVE_USE_MIGRATION_NOTIF_OPCODE:
    /* turn off ignore_hispeed_on_notify */
    case INTERACTIVE_START_INDEX + INTERACTIVE_IGNORE_HISPEED_NOTIF_OPCODE:
    /* Removes input boost during keypress scenarios */
    case MISC_START_INDEX + INPUT_BOOST_RESET_OPCODE:
        snprintf(acqval, NODE_MAX, "%d", 0);
        break;

    case INTERACTIVE_START_INDEX + SCHEDUTIL_HISPEED_FREQ_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_HISPEED_FREQ_OPCODE:
        if (reqval > 0 ) {
            reqval = MultiplyReqval(reqval, FREQ_MULTIPLICATION_FACTOR);
            reqval = d.find_next_cpu_frequency(r.core, reqval);
        }
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case ONDEMAND_START_INDEX + OND_SYNC_FREQ_OPCODE:
    case ONDEMAND_START_INDEX + OND_OPIMAL_FREQ_OPCODE:
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, FREQ_MULTIPLICATION_FACTOR);
            reqval = d.find_next_avail_freq(reqval);
        }
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case SCHED_START_INDEX + SCHED_PREFER_IDLE_OPCODE:
        snprintf(acqval, NODE_MAX, "%d", ENABLE_PREFER_IDLE);
        break;

    case CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MAXFREQ_OPCODE:
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, 100);
            setval = d.find_next_cpubw_available_freq(reqval);
        }
        if (setval == FAILED) {
            QLOGE("Error! Perflock failed, invalid freq value %d", setval);
            return FAILED;
        }
        snprintf(acqval, NODE_MAX, "%d", setval);
        /* read hwmon min_freq */
        hwmon_min_freq_index = CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MINFREQ_OPCODE;
        FREAD_STR(d.sysfsnode_path[hwmon_min_freq_index], tmp_node, NODE_MAX, rc);
        if (rc < 0) {
            QLOGE("Failed to read %s", d.sysfsnode_path[hwmon_min_freq_index]);
            return FAILED;
        }
        /*max_freq cannot be lower than min_freq, if reqval
        is lower than minfreq, request is denied*/
        valid_bound = minBoundCheck(d.sysfsnode_storage[idx], tmp_node, acqval);
        if (!valid_bound) {
            QLOGE("Min bounds check failed");
            return FAILED;
        }
        break;

    case CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MINFREQ_OPCODE:
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, 100);
            setval = d.find_next_cpubw_available_freq(reqval);
        }
        if (setval == FAILED) {
            QLOGE("Error! Perflock failed, invalid freq value %d", setval);
            return FAILED;
        }
        snprintf(acqval, NODE_MAX, "%d", setval);
        /* read hwmon max_freq  */
        hwmon_max_freq_index = CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MAXFREQ_OPCODE;
        FREAD_STR(d.sysfsnode_path[hwmon_max_freq_index], tmp_node, NODE_MAX, rc);
        if (rc < 0) {
            QLOGE("Failed to read %s", d.sysfsnode_path[hwmon_max_freq_index]);
            return FAILED;
        }
        /*min_freq cannot be higher than max_freq, if reqval
         is higher than maxfreq,request is denied*/
        valid_bound = maxBoundCheck(tmp_node, d.sysfsnode_storage[idx], acqval);
        if (!valid_bound) {
            QLOGE("Max bounds check failed");
            return FAILED;
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_ABOVE_HISPEED_DELAY_OPCODE:
        if (reqval == 0xFE) {
            snprintf(acqval, NODE_MAX, "19000 1400000:39000 1700000:19000");
        } else if (reqval == 0xFD) {
            snprintf(acqval, NODE_MAX, "%d", 19000);
        } else {
            reqval = MultiplyReqval(reqval, 10000);
            snprintf(acqval, NODE_MAX, "%d", reqval);
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_TARGET_LOADS_OPCODE:
        if (reqval == 0xFE) {
            snprintf(acqval, NODE_MAX, "85 1500000:90 1800000:70");
        } else {
            snprintf(acqval, NODE_MAX, "%d", reqval);
        }
        break;

    case SCHED_START_INDEX + SCHED_MOSTLY_IDLE_FREQ_OPCODE:
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, FREQ_MULTIPLICATION_FACTOR);
            if ((cpu = get_online_core(0, target.getLastCoreIndex(0)))!= FAILED) {
                 valCluster1 =  d.find_next_cpu_frequency(cpu, reqval);
                QLOGI("valCluster1=%d", valCluster1);
            }
            if ((cpu = get_online_core(target.getLastCoreIndex(0)+1, target.getLastCoreIndex(1))) != FAILED) {
                 valCluster2 =  d.find_next_cpu_frequency(cpu, reqval);
                QLOGI("valCluster2=%d", valCluster2);
            }
        }

        if (d.node_type[idx] == UPDATE_CORES_PER_CLUSTER) {
            snprintf(acqval, NODE_MAX, "%d,%d", valCluster1, valCluster2);
        } else {
            snprintf(acqval, NODE_MAX, "%d", valCluster1);
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_BOOSTPULSE_DURATION_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_MIN_SAMPLE_TIME_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_RATE_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_SLACK_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_MAX_FREQ_HYSTERESIS_OPCODE:
    case ONDEMAND_START_INDEX + OND_SAMPLING_RATE_OPCODE: // 0xff - (data & 0xff); //todo
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, TIMER_MULTIPLICATION_FACTOR);
        }
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case GPU_START_INDEX + GPU_MIN_FREQ_OPCODE:
    case GPU_START_INDEX + GPU_MAX_FREQ_OPCODE:
        /* find the nearest >= available freq lvl, after multiplying
        * input value with 1000000 */
        reqval = MultiplyReqval(reqval, 1000000);
        reqval = target.findNextGpuFreq(reqval);
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case GPU_START_INDEX + GPU_BUS_MIN_FREQ_OPCODE:
    case GPU_START_INDEX + GPU_BUS_MAX_FREQ_OPCODE:
        /* find the nearest >= available freq lvl */
        reqval = target.findNextGpuBusFreq(reqval);
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case ONDEMAND_START_INDEX + NOTIFY_ON_MIGRATE:
        if (reqval != 1) {
            reqval = 0;
        }
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case MISC_START_INDEX + STORAGE_CLK_SCALING_DISABLE_OPCODE:
        if (reqval == 1) {
            reqval = 0;
        } else {
            reqval = 1;
        }
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;

    case LLCBW_HWMON_START_INDEX + LLCBW_HWMON_MINFREQ_OPCODE:
        if (reqval > 0) {
            reqval = MultiplyReqval(reqval, 100);
            tmp_here = get_devfreq_new_val(reqval, d.llcbw_avail_freqs, d.llcbw_avail_freqs_n, d.llcbw_maxfreq_path);
        }
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            free(tmp_here);
        } else {
            QLOGE("Unable to find the new devfreq_val for the requested value %u", reqval);
            return FAILED;
        }
        break;

    default:
        snprintf(acqval, NODE_MAX, "%d", reqval);
        break;
    }

    return SUCCESS;
}

/*Based on resource type, we select which all nodes to be updated. Default case
is to update the given single node with value node_strg.*/
int OptsHandler::update_node_param(int node_type, const char node[NODE_MAX],
                                            char node_strg[NODE_MAX], int node_strg_l) {
    int rc = -1, i = 0;
    char tmp_node[NODE_MAX] = "", tmp_s[NODE_MAX] = "";
    char *pch = NULL;
    Target &target = Target::getCurTarget();
    unsigned int valCluster1 = 0, valCluster2 = 0;

    switch (node_type) {
    case UPDATE_ALL_CORES:
        /* In current implementation we are considering all cores should have same
        value. If this assumption changes, code needs to be updated.*/
        strlcpy(tmp_node, node, NODE_MAX);
        for (i = 0; i < target.getTotalNumCores(); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, node_strg, node_strg_l);
        }
        break;

    case UPDATE_CORES_PER_CLUSTER:
        /* During acquire call, cores of diffrent cluster are changed with different value.
        and on release all the cores are reset to previously stored value from core 0. */
        valCluster1 = strtol(node_strg, NULL, 0);
        pch = strchr(node_strg, ',');
        if(pch != NULL) {
            valCluster2 = strtol(pch+1, NULL, 0);
        } else {
            valCluster2 = valCluster1;
        }

        strlcpy(tmp_node, node, NODE_MAX);
        snprintf(tmp_s, NODE_MAX, "%d", valCluster1);
        for (i = 0; i <= target.getLastCoreIndex(0); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, tmp_s, strlen(tmp_s));
        }
        snprintf(tmp_s, NODE_MAX, "%d", valCluster2);
        for (i = target.getLastCoreIndex(0)+1; i <= target.getLastCoreIndex(1); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, tmp_s, strlen(tmp_s));
        }
        break;

    default:
        rc = change_node_val(node, node_strg, node_strg_l);
        break;
    }
    return rc;
}

/* Wrapper functions for function pointers */
int OptsHandler::dummy(Resource &r,  OptsData &d) {
    return 0;
}

/* Unsupported resource opcode*/
int OptsHandler::unsupported(Resource &r, OptsData &d) {
    QLOGE("Error: This resource is not supported");
    return FAILED;
}

/*For each request, we first validate the cores and clusters and then basing on resource type,
we get the node storage pointers for that resource. Also necessary changes are made to node
path basing on core and cluster of request.*/
int OptsHandler::GetNodeStorageLink(Resource &r, OptsData &d, char **node_storage, int **node_storage_length) {
    int idx = r.qindex, rc = FAILED;
    unsigned char minor = r.minor;
    int cpu = -1, core = r.core;
    Target &t = Target::getCurTarget();

    switch(d.node_type[idx]) {
    case SELECT_CORE_TO_UPDATE:
    /* we find the node storage paths basing on the requested core.*/
        rc = ValidateClusterAndCore(r.cluster, r.core, ASYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }
        d.sysfsnode_path[idx][CPU_INDEX] = core + '0';
        if (idx == (SCHED_START_INDEX + SCHED_STATIC_CPU_PWR_COST_OPCODE)) {
            *node_storage = d.sch_cpu_pwr_cost_s[core];
            *node_storage_length = &d.sch_cpu_pwr_cost_sl[core];
        }
        else if (idx == (SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE)) {
            *node_storage = d.sch_load_boost_s[core];
            *node_storage_length = &d.sch_load_boost_sl[core];
        }
        break;

    case INTERACTIVE_NODE:
    /*If target's govinstance type is of cluster based, then we get the node storage
      paths of first online cpu in requested cluster and returns failed if all cores in
      that cluster are offline. Default case is to return the cluster 0 node storage paths.*/
        if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
            rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
            if (rc == FAILED) {
                QLOGE("Request on invalid core or cluster");
                return FAILED;
            }

            if (CLUSTER0 == r.cluster) {
                if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                    d.sysfsnode_path[idx][CPU_INDEX] = cpu + '0';
                    *node_storage = d.cluster0_interactive_node_storage[minor];
                    *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
                } else {
                    QLOGE("Error! No core is online for cluster %d", r.cluster);
                    return FAILED;
                }
            } else {
                if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                    d.sysfsnode_path[idx][CPU_INDEX] = cpu + '0';
                    *node_storage = d.cluster1_interactive_node_storage[minor];
                    *node_storage_length = &d.cluster1_interactive_node_storage_length[minor];
                } else {
                    QLOGE("Error! No core is online for cluster %d", r.cluster);
                    return FAILED;
                }
            }
        } else {
            rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
            if (rc == FAILED) {
                QLOGE("Request on invalid core or cluster");
                return FAILED;
            }
            *node_storage = d.cluster0_interactive_node_storage[minor];
            *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
        }
        break;

    case INTERACTIVE_ALL_CORES:
    /*For these interactive nodes, updating one core changes all the cores of both clusters,
      So, we can find an online core from total number of cores.*/
        if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
            rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
            if (rc == FAILED) {
                QLOGE("Request on invalid core or cluster");
                return FAILED;
            }

            cpu = get_online_core(0, t.getTotalNumCores() - 1);
            d.sysfsnode_path[idx][CPU_INDEX] = cpu + '0';
            *node_storage = d.cluster0_interactive_node_storage[minor];
            *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
        } else {
            rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
            if (rc == FAILED) {
                QLOGE("Request on invalid core or cluster");
                return FAILED;
            }
            *node_storage = d.cluster0_interactive_node_storage[minor];
            *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
        }
        break;

    default:
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        *node_storage = d.sysfsnode_storage[idx];
        *node_storage_length = &d.sysfsnode_storage_length[idx];
        break;
    }

    return SUCCESS;
}

/*An optional function, which is called after every acquire/release call for
handling all requests post node updations.*/
void OptsHandler::CustomizePostNodeUpdation(Resource &r, OptsData &d, int perflock_call,
                                            char node_strg[NODE_MAX], int node_strg_l) {
    int idx = r.qindex, rc;
    if (perflock_call == ACQUIRE_LOCK) {
        switch(idx) {
        case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
            d.mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();
            break;
        }
    } else if (perflock_call == RELEASE_LOCK) {
        switch(idx) {
        case SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE:
            /* Only for foreground cpuset: restore foreground/boost/cpus
            after restoring foreground/cpus with all cpus bitmask */
            FWRITE_STR(SCHED_FOREGROUND_BOOST, node_strg, node_strg_l, rc);
            QLOGI("Updated %s with %s, return value %d", SCHED_FOREGROUND_BOOST, node_strg, rc);
            break;

        case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
            d.mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();
            break;
        }
    }
}

/*A common function to handle all the generic perflock calls.*/
int OptsHandler::modify_sysfsnode(Resource &r, OptsData &d) {
    int idx = r.qindex, rc = FAILED;
    char tmp_s[NODE_MAX]= "";
    unsigned int reqval = r.value;
    Target &t = Target::getCurTarget();
    char *node_storage = NULL;
    int *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
         QLOGE("Perflock resource %s not supported", d.sysfsnode_path[idx]);
         return FAILED;
    }

    /*The first step for any request is to validate it and then get the Node storage paths.
    For acqiure call these node storage paths are used to store the current value of nodes
    and for release call the nodes are updated with values in these storage paths.*/
    rc = GetNodeStorageLink(r, d, &node_storage, &node_storage_length);

    /*Release call, release happens only if we have any previously stored value.
      For only interactive type nodes, on release failure we wait on poll_thread.*/
    if (reqval == MAX_LVL) {
        QLOGI("Perflock release call for resource index = %d, path = %s, from function = %s",
                                                        idx, d.sysfsnode_path[idx], __func__);

        if (rc != FAILED && *node_storage_length > 0) {
            rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], node_storage, *node_storage_length);
            CustomizePostNodeUpdation(r, d, RELEASE_LOCK, node_storage, *node_storage_length);
            *node_storage_length = -1;
        } else if (rc == FAILED) {
            QLOGE("Unable to find the correct node storage pointers for resource index=%d, node path=%s",
                                                        idx, d.sysfsnode_path[idx]);
        } else {
            QLOGE("perf_lock_rel: failed for %s as previous value is not stored", d.sysfsnode_path[idx]);
        }

        if(d.node_type[idx] == INTERACTIVE_NODE) {
            if (rc < 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
                signal_chk_poll_thread(d.sysfsnode_path[idx], rc);
            }
        }
        return rc;
    }

    /*steps followed for Acquire call
      1. Ensuring that we got the node storage pointers correctly.
      2. Storing the previous node value, only if it is not already stored.
      3. Customizing the requested value.
      4. Updating all the required nodes basing on the opcode type.*/

    QLOGI("Perflock Acquire call for resource index = %d, path = %s, from function = %s",
                                                        idx, d.sysfsnode_path[idx], __func__);
    QLOGI("Requested value = %d", reqval);
    if (rc == FAILED) {
        QLOGE("Unable to find the correct node storage pointers for resource index=%d, node path=%s",
                                                        idx, d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (*node_storage_length <= 0) {
        *node_storage_length = save_node_val(d.sysfsnode_path[idx], node_storage);
        if (*node_storage_length <= 0) {
            QLOGE("Failed to read %s", d.sysfsnode_path[idx]);
            return FAILED;
        }
    }

    rc = CustomizeRequestValue(r, d, tmp_s);
    if (rc == SUCCESS) {
        QLOGI("After customizing, the request reqval=%s", tmp_s);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
        CustomizePostNodeUpdation(r, d, ACQUIRE_LOCK, tmp_s, strlen(tmp_s));
        return rc;
    } else {
        QLOGE("Error! Perflock failed, invalid request value %d", reqval);
        return FAILED;
    }
}

int OptsHandler::pmQoS_cpu_dma_latency(Resource &r, OptsData &d) {
    int rc = FAILED;
    int idx = r.qindex;
    static char pmqos_cpu_dma_s[NODE_MAX];// pm qos cpu dma latency string
    unsigned int latency = r.value;
    static int fd = FAILED;
    unsigned int min_pmqos_latency = 0;
    Target &t = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("%s:Request on invalid core or cluster", __func__);
        return FAILED;
    }
    /*Set minimum latency(floor) to WFI for the target.
     * Clients thus do not need to explicitly mention the WFI latency
     * & continue to use the existing value of 0x1 to disable PC and use WFI.
     * If a client gives an explicit value > WFI, then
     * client knows what it is doing and hence honor.
     */
     min_pmqos_latency = t.getMinPmQosLatency();
     if (latency < min_pmqos_latency) {
         QLOGI("%s:Requested Latency=0x%x", __func__, latency);
         QLOGI("%s:Target Min WFI Latency is =0x%x, setting same", __func__,min_pmqos_latency);
         latency = min_pmqos_latency;
     } else {
         QLOGI("%s:Client latency=0x%x > trgt min latency=0x%x", __func__, latency,min_pmqos_latency);
         QLOGI("No Override with min latency for target");
     }
    /*If there is a lock acquired, then we close, to release
     *that fd and open with new value.Doing this way
     * in this case actually handles concurrency.
     */
    if (fd >= 0) { //close the device node that was opened.
        rc = close(fd);
        fd = FAILED;
        QLOGI("%s:Released the PMQos Lock and dev node closed, rc=%d", __func__, rc);
    }

    /*Check if Perf lock request to acquire(!= MAX_LVL)*/
    if (latency != MAX_LVL) {
        //FWRITE_STR not used since it closes the fd. This shouldnt be done in this case
        ///kernel/msm-4.4/Documentation/power/pm_qos_interface.txt#95
        fd = open(d.sysfsnode_path[idx], O_WRONLY);
        if (fd >= 0) {
           snprintf(pmqos_cpu_dma_s, NODE_MAX, "%x", latency);
           rc = write(fd, pmqos_cpu_dma_s, strlen(pmqos_cpu_dma_s));// Write the CPU DMA Latency value
           if (rc < 0) {
               QLOGE("%s:Writing to the PM QoS node failed=%d", __func__, rc);
               close(fd);
               fd = FAILED;
           } else {
               //To remove the user mode request for a target value simply close the device node
               QLOGI("%s:Sucessfully applied PMQos Lock for fd=%d, latency=0x%s, rc=%d", __func__, fd, pmqos_cpu_dma_s, rc);
           }
       } else {
           rc = FAILED;
           QLOGE("%s:Failed to Open PMQos Lock for fd=%d, latency=0x%s, rc=%d", __func__, fd, pmqos_cpu_dma_s, rc);
       }
    }
    return rc;
}

/* CPU options */
int OptsHandler::cpu_options(Resource &r,  OptsData &d) {
    int rc = -1;
    int i = 0;
    int idx = r.qindex;
    char fmtStr[NODE_MAX];
    unsigned int reqval  = r.value;
    unsigned int valToUpdate;
    char nodeToUpdate[NODE_MAX];
    int min_freq = (CPUFREQ_MIN_FREQ_OPCODE == r.minor) ? 1 : 0;
    int cpu = -1;
    static bool is_sync_cores; //Updating a single core freq, updates all cores freq.
    Target &t = Target::getCurTarget();
    int startCpu, endCpu, cluster, coresInCluster;

    is_sync_cores = t.isSyncCore();
    cpu = r.core;

    /* For sync_cores, where changing frequency for one core changes, ignore the requests
     * for resources other than for core0 and core4. This is to ensure that we may not end
     * up loosing concurrency support for cpufreq perflocks.
     */

    cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
    if ((startCpu < 0) || (endCpu < 0) || (cluster < 0)) {
        QLOGE("Could not find a cluster corresponding the core %d", cpu);
        return FAILED;
    }
    if (is_sync_cores && (cpu > startCpu && cpu <= endCpu)) {
        QLOGW("Warning: Resource [%d, %d] not supported for core %d. Instead use resource for core %d", r.major, r.minor,cpu,startCpu);
        return FAILED;
    }
    /* Calculate the value that needs to be updated to KPM.
     * If lock is being released (reqVal == 0) then
     * update with 0 (for min_freq) and cpu max reset value (for max_freq)
     * If lock is being acquired then reqVal is multiplied
     * with 100000 and updated by finding next closest frequency.
     * */
    if (reqval == MAX_LVL) {
        QLOGI("Releasing the node %s", d.sysfsnode_path[idx]);
        valToUpdate = 0;
        strlcpy(nodeToUpdate, d.sysfsnode_path[idx], strlen(d.sysfsnode_path[idx])+1);
        if (!min_freq){
            valToUpdate = t.getCpuMaxFreqResetVal(cluster);
        }
    } else {
        reqval = MultiplyReqval(reqval, FREQ_MULTIPLICATION_FACTOR);
        valToUpdate = d.find_next_cpu_frequency(r.core, reqval);
        strlcpy(nodeToUpdate, d.sysfsnode_path[idx], strlen(d.sysfsnode_path[idx])+1);
    }
    QLOGI("Freq value to be updated %d", valToUpdate);

    /* Construct the formatted string with which KPM node is updated
     */
    coresInCluster = t.getCoresInCluster(cluster);
    if (coresInCluster < 0){
        QLOGE("Cores in a cluster cannot be %d", coresInCluster);
        return FAILED;
    }

    if (cluster == 0) {
        rc = update_freq_node(0, t.getLastCoreIndex(0), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE("Perflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else if (cluster == 1) {
        rc = update_freq_node(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE("Peflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else {
        QLOGE("Cluster Id %d not supported\n",cluster);
        return FAILED;
    }

    /* Finally update the KPM Node. The cluster based node is updated in
     * fmtStr, and KPM node is available in nodeToUpdate.
     * */
     FWRITE_STR(nodeToUpdate, fmtStr, strlen(fmtStr), rc);
     QLOGI("Updated %s with %s return value %d",nodeToUpdate , fmtStr, rc );
     return rc;
}

bool is_sched_boost_nonzero(const char *node_path) {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    bool sched_boost_nonzero = false;

    FREAD_STR(node_path, tmp_s, NODE_MAX, rc);
    if (rc > 0 && atoi(tmp_s) != 0)
        sched_boost_nonzero = true;
    return sched_boost_nonzero;
}

/* Set sched boost */
int OptsHandler::sched_boost(Resource &r, OptsData &d) {
    int rc = -1;
    int idx = r.qindex;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_boost = 0;
    static char sch_boost_s[NODE_MAX]; // sched boost storage
    static int  sch_boost_sl = -1; // sched boost string length
    unsigned int reqval = r.value;
    Target &t = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }
    rc = FAILED; //re-initialize here for coherent error checking
    /*
     * Only kernel 4.4 supports concurrency and multiple shed boost values.
     * Kernel 4.4 also needs to reset sched_boost prior to change in value
     * Do specific steps w.r.t. kernel vaersion w.r.t. kernel version
     */
    bool concurrency_allowed = t.isSchedBoostConcurrencySupported();
    if (concurrency_allowed) {
    /* write 0 to sched_boost node only if its nonzero */
        if (is_sched_boost_nonzero(d.sysfsnode_path[idx]) == true) {
           /* Reset sched boost */
            snprintf(tmp_s, NODE_MAX, "%d", 0);
            FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
        }
    } else if (reqval != MAX_LVL) {
        /*only supported values 0 and 1*/
        reqval = !!reqval;
    }

    if (reqval == MAX_LVL) {
        if (sch_boost_sl > 0) {
            /* write 0 to sched_boost node only if its nonzero */
            if (is_sched_boost_nonzero(d.sysfsnode_path[idx]) == true)
                FWRITE_STR(d.sysfsnode_path[idx], sch_boost_s, sch_boost_sl, rc);
            stored_sched_boost = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.sysfsnode_path[idx], sch_boost_s, rc);
        }
        d.is_active = false;
        return rc;
    }

    if (d.is_active && !concurrency_allowed) {
        QLOGE("Sched boost lock is already acquired. Concurrency not supported for this resource");
        return FAILED;
    }

    if (!stored_sched_boost) {
        FREAD_STR(d.sysfsnode_path[idx], sch_boost_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", d.sysfsnode_path[idx], sch_boost_s, rc);
            sch_boost_sl = strlen(sch_boost_s);
            stored_sched_boost = 1;
        } else {
            QLOGE("Failed to read %s", d.sysfsnode_path[idx]);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock: updated %s with %s return value %d", d.sysfsnode_path[idx], tmp_s, rc);
    if (rc > 0) {
        d.is_active = true;
    }
    return rc;
}

static thread_group get_threads_of_interest_for_sched_group(int tid) {
    char name[NODE_MAX], data[NODE_MAX];
    int rc, *tids = new int[HWUI_SCHED_GROUP_SIZE], num_tids = 0;
    thread_group t;
    t.tids = NULL;
    t.num_tids = 0;

    if (tids != nullptr)
        memset(tids, 0, HWUI_SCHED_GROUP_SIZE * sizeof(int));
     else
         return t;

    memset(data, 0, NODE_MAX);
    memset(name, 0, NODE_MAX);
    snprintf(name, NODE_MAX, "/proc/%d/task", tid);
    DIR* proc_dir = opendir(name);
    if (proc_dir) {
        tids[0] = tid;
        num_tids++;
        struct dirent *ent;
        while (((ent = readdir(proc_dir)) != NULL) && (num_tids < 5)) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                memset(name, 0, NODE_MAX);
                snprintf(name, NODE_MAX, "/proc/%s/status", ent->d_name);
                memset(data, 0, NODE_MAX);
                FREAD_STR(name, data, NODE_MAX - 1, rc)
                if (sched_group_data::isHWUIThread(data)) {
                    tids[num_tids] = atoi(ent->d_name);
                    num_tids++;
                }
            }
        }
        closedir(proc_dir);
    }
    /*
     * Apply sched group only for HWUI Threads.
     */
    if (!num_tids ||
               (num_tids < (HWUI_SCHED_GROUP_SIZE - 1))) {
        num_tids = 0;
        delete[] tids;
        tids = NULL;
        QLOGI("sched_group NOT Found");
    }

    t.tids = tids;
    t.num_tids = num_tids;
    return t;
}

static void write_sched_group(thread_group t, int enable) {
    if (!t.num_tids)
        return;
    char value[NODE_MAX];
    memset(value, 0, NODE_MAX);
    if (enable) {
        snprintf(value, NODE_MAX, "%d", t.tids[0]);
    } else
        value[0] = '0';
    int len = strlen(value), rc;

    for (int i = 0; i < t.num_tids; i++) {
        if (t.tids[i] != 0) {
            char file_name[NODE_MAX];
            memset(file_name, 0, NODE_MAX);
            snprintf(file_name, NODE_MAX, "/proc/%d/sched_group_id",
                                                             t.tids[i]);
            FWRITE_STR(file_name, value, len, rc);
            QLOGI("sched_group_id for tid = %d is %s", t.tids[i], value);
        }
    }
}

/* Add sched group */
int OptsHandler::add_sched_group(Resource &r, OptsData &d) {
    int rc = 0;
    char *bptr;
    char value[NODE_MAX];

    if (d.mSchedGroupData.Found(r.value))
        return rc;

    thread_group t = get_threads_of_interest_for_sched_group(r.value);
    d.mSchedGroupData.Add(r.value, t);
    write_sched_group(t, 1);
    return rc;
}

/* Reset sched group */
int OptsHandler::reset_sched_group(Resource &r, OptsData &d) {
    int rc = 0;
    char *bptr;
    char value[NODE_MAX];

    if (!d.mSchedGroupData.Found(r.value))
        return rc;

    thread_group t = d.mSchedGroupData.Get(r.value);
    if (t.tids) {
        write_sched_group(t, 0);
        delete[] t.tids;
        t.tids = 0;
        t.num_tids = 0;
    }
    d.mSchedGroupData.Remove(r.value);
    return rc;
}



int OptsHandler::write_sched_freq_aggr_group(int tid, bool enable) {
    DIR *dir;
    struct dirent *readDir;
    char file_name[NODE_MAX], dir_name[NODE_MAX], pid_str[NODE_MAX];
    int fd, rc=-1,rc1=-1;

    memset(dir_name, 0, NODE_MAX);
    snprintf(dir_name, NODE_MAX, "/proc/%d/task", tid);

    memset(pid_str, 0, NODE_MAX);
    if (enable) {
        snprintf(pid_str, NODE_MAX, "%d", tid);
    } else {
        pid_str[0] = '0';
    }

    /*find all tasks related to passed TID and set sched_group_id*/
    if((dir = opendir(dir_name))) {
        while((readDir = readdir(dir))) {
            memset(file_name, 0, NODE_MAX);
            snprintf(file_name, NODE_MAX, "/proc/%s/sched_group_id", readDir->d_name);
            fd = open(file_name, O_WRONLY);
            if (fd >= 0) {
                rc1 = write(fd, pid_str, strlen(pid_str));// set sgid
                close(fd);
            }
        } //while(readDir)
        closedir(dir);
    } else { //if(opendir)
        QLOGE("%s, opendir() failed on /proc/%d/task", __FUNCTION__, tid);
        rc=-1;
        return rc;
    }
    /*Scheduler group_id inheritance feature ensures setting of sched_group_id for any
      new child thread getting created after sched_group_id is set for the App process.
      Need to check if there exists any window where above while loop may miss settting
      sched_group_id for any child thread created during above mentioned boundary*/
    rc=0;
    return rc;
}

/* Add sched freq aggr group */
int OptsHandler::sched_add_freq_aggr_group(Resource &r, OptsData &d) {
    int rc = 0, instanceRefCount=1;
    bool enableFA = true;

    QLOGI("%s, pid=%d",__FUNCTION__, r.value);

    if (d.mSchedFreqAggrGroupData.Found(r.value)) {
        /*tid entry already exists, increment instanceRefCount and exit
          as Frequency Aggregation is already enabled on tid*/
        QLOGI("%s, tid %d already in hastable", __FUNCTION__, r.value);
        d.mSchedFreqAggrGroupData.IncrementInsRefCntVal(r.value);
        return rc;
    }
    /*Add instanceRefCount as 1 using tid as key*/
    d.mSchedFreqAggrGroupData.Add(r.value, instanceRefCount);
    write_sched_freq_aggr_group(r.value, enableFA);
    return rc;
}

char *OptsHandler::cpuset_bitmask_to_str(unsigned int bitmask) {
    int i = 0;
    int str_index = 0;
    char *cpuset_str = new char[CPUSET_STR_SIZE];
    if(cpuset_str != NULL) {
        memset(cpuset_str, '\0', CPUSET_STR_SIZE);
        while(bitmask != 0) {
            if(bitmask & 1 && str_index < CPUSET_STR_SIZE - 1) {
                int chars_copied = snprintf(cpuset_str + str_index, CPUSET_STR_SIZE - str_index, "%d,", i);
                str_index += chars_copied;
            }
            bitmask = bitmask >> 1;
            i++;
        }
        if(str_index > 0) {
            cpuset_str[str_index-1] = '\0';
        }
    }
    return cpuset_str;
}

/* Reset sched freq aggr group */
int OptsHandler::sched_reset_freq_aggr_group(Resource &r, OptsData &d) {
    int rc = 0, instanceRefCount;
    bool disableFA = false;

    QLOGI("%s, pid=%d",__FUNCTION__, r.value);

    if (!d.mSchedFreqAggrGroupData.Found(r.value)) {
        QLOGI("%s, tid %d not found in hastable", __FUNCTION__, r.value);
        return rc;
    }

    if ((instanceRefCount = d.mSchedFreqAggrGroupData.Get(r.value)) == 1) {
        write_sched_freq_aggr_group(r.value, disableFA);
        d.mSchedFreqAggrGroupData.Remove(r.value);
    } else if (instanceRefCount > 1) {
        /*more reference to tid entry exists, decrement instanceRefCount
          and exit*/
        d.mSchedFreqAggrGroupData.DecrementInsRefCntVal(r.value);
    }
    return rc;
}

int OptsHandler::l3_min_freq(Resource &r,  OptsData &d) {
    int rc = FAILED;
    char* setval_Cluster0 = NULL;
    char* setval_Cluster1 = NULL;
    unsigned int reqval = r.value;
    static unsigned int stored_l3_min_freq = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }
    if (reqval == MAX_LVL) {
        if (d.l3Cluster0_mf_sl > 0 && d.l3Cluster1_mf_sl > 0) {
            FWRITE_STR(d.l3Cluster0_minfreq_path, d.l3Cluster0_mf_s, d.l3Cluster0_mf_sl, rc);
            QLOGI("perf_lock_rel_l3Cluster0: updated %s with %s return value %d", d.l3Cluster0_minfreq_path, d.l3Cluster0_mf_s, rc);
            FWRITE_STR(d.l3Cluster1_minfreq_path, d.l3Cluster1_mf_s, d.l3Cluster1_mf_sl, rc);
            QLOGI("perf_lock_rel_l3Cluster1: updated %s with %s return value %d", d.l3Cluster1_minfreq_path, d.l3Cluster1_mf_s, rc);
            stored_l3_min_freq = 0;
        }
        return rc;
    }

    if (reqval > 0) {
        reqval = MultiplyReqval(reqval, 100000);
        setval_Cluster0 = get_devfreq_new_val(reqval, d.l3Cluster0_avail_freqs, d.l3Cluster0_avail_freqs_n, d.l3Cluster0_maxfreq_path);
        setval_Cluster1 = get_devfreq_new_val(reqval, d.l3Cluster1_avail_freqs, d.l3Cluster1_avail_freqs_n, d.l3Cluster1_maxfreq_path);
    }

    if (!stored_l3_min_freq) {

        d.l3Cluster0_mf_sl = save_node_val(d.l3Cluster0_minfreq_path, d.l3Cluster0_mf_s);
        d.l3Cluster1_mf_sl = save_node_val(d.l3Cluster1_minfreq_path, d.l3Cluster1_mf_s);
        if (d.l3Cluster1_mf_sl > 0 && d.l3Cluster0_mf_sl > 0)
            stored_l3_min_freq = 1;
    }

    if (setval_Cluster0 != NULL && setval_Cluster1 != NULL) {
        FWRITE_STR(d.l3Cluster0_minfreq_path, setval_Cluster0, strlen(setval_Cluster0), rc);
        QLOGI("perf_lock_acq_l3Cluster0: updated %s with %s return value %d", d.l3Cluster0_minfreq_path, setval_Cluster0, rc);
        free(setval_Cluster0);
        FWRITE_STR(d.l3Cluster1_minfreq_path, setval_Cluster1, strlen(setval_Cluster1), rc);
        QLOGI("perf_lock_acq_l3Cluster1: updated %s with %s return value %d", d.l3Cluster1_minfreq_path, setval_Cluster1, rc);
        free(setval_Cluster1);
    }

    return rc;
}

int OptsHandler::cpubw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_hyst_opt = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_hc_sl > 0) {
           FWRITE_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, d.cpubw_hc_sl, rc);
           if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hyst_count_path);
               return FAILED;
            }
        }
        if (d.cpubw_hm_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, d.cpubw_hm_sl, rc);
            if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hist_memory_path);
               return FAILED;
            }
        }
        if (d.cpubw_hl_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, d.cpubw_hl_sl, rc);
            if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hyst_length_path);
               return FAILED;
            }
        }
        if (rc >= 0)
           stored_hyst_opt = 0;
        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hc_sl = strlen(d.cpubw_hc_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hm_sl = strlen(d.cpubw_hm_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hl_sl = strlen(d.cpubw_hl_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.cpubw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    FWRITE_STR(d.cpubw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    FWRITE_STR(d.cpubw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    return rc;
}

int OptsHandler::llcbw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_hyst_opt = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.llcbw_hc_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, d.llcbw_hc_sl, rc);
            if (rc < 0) {
                QLOGE("Failed to write %s", d.llcbw_hwmon_hyst_count_path);
                return FAILED;
            }
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, rc);
        }
        if (d.llcbw_hm_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, d.llcbw_hm_sl, rc);
            if (rc < 0) {
                QLOGE("Failed to write %s", d.llcbw_hwmon_hist_memory_path);
                return FAILED;
            }
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, rc);
        }
        if (d.llcbw_hl_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, d.llcbw_hl_sl, rc);
            if (rc < 0) {
                QLOGE("Failed to write %s", d.llcbw_hwmon_hyst_length_path);
                return FAILED;
            }
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, rc);
        }
        if (rc >= 0)
            stored_hyst_opt = 0;
        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hc_sl = strlen(d.llcbw_hc_s);
        } else {
            QLOGE("Failed to read %s", d.llcbw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hm_sl = strlen(d.llcbw_hm_s);
        } else {
            QLOGE("Failed to read %s", d.llcbw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hl_sl = strlen(d.llcbw_hl_s);
        } else {
            QLOGE("Failed to read %s", d.llcbw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.llcbw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.llcbw_hwmon_hyst_count_path, tmp_s, rc);
    FWRITE_STR(d.llcbw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.llcbw_hwmon_hist_memory_path, tmp_s, rc);
    FWRITE_STR(d.llcbw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.llcbw_hwmon_hyst_length_path, tmp_s, rc);
    return rc;
}

/* Function to handle single layer display hint
 * state = 0, is not single layer
 * state = 1, is single layer
 */
int OptsHandler::handle_disp_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.disp_single_layer = reqval;
    QLOGI("Display sent layer=%d", d.mHintData.disp_single_layer);

    /* Video is started, but display sends multiple
     * layer hint, so rearm timer, handled as
     * condition2 in slvp callback
     */
    if (d.mHintData.slvp_perflock_set == 1 && d.mHintData.disp_single_layer != 1) {
        QLOGI("Display to rearm timer,layer=%d",d.mHintData.disp_single_layer);

        /* rearm timer here, release handle in SLVP callback */
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to receive video playback hint
 * state = 0, video stopped
 * state = 1, single instance of video
 */
int OptsHandler::handle_vid_decplay_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.vid_hint_state = reqval;

    /* timer is created only once here on getting video hint */
    if (d.mHintData.vid_hint_state == 1 && !d.mHintData.timer_created) {
        QLOGI("Video sent hint, create timer");
        return vid_create_timer(&d.mHintData);
    } else {
        /* only rearm here, handle conditions in SLVP callback */
        QLOGI("Video rearm timer");
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to recieve video encode hint
 * for WFD use case.
 */
int OptsHandler::handle_vid_encplay_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    /* reqval - 1, encode start
     * reqval - 0, encode stop
     */
    d.mHintData.vid_enc_start = reqval;
    /* rearm timer if encode is atarted
     * handle condition4 in callback */
    if (d.mHintData.slvp_perflock_set == 1) {
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

int OptsHandler::disable_ksm(Resource &r, OptsData &d) {
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(0);
}

int OptsHandler::enable_ksm(Resource &r, OptsData &d) {
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(1);
}

int OptsHandler::set_ksm_param(Resource &r, OptsData &d) {
    int rc = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       char sleep_time[PROPERTY_VALUE_MAX];
       char scan_page[PROPERTY_VALUE_MAX];
       memset(sleep_time, 0, sizeof(sleep_time));
       memset(scan_page, 0, sizeof(scan_page));
       property_get("system.ksm_sleeptime", sleep_time, "20");
       property_get("system.ksm_scan_page", scan_page, "300");
       FWRITE_STR(d.ksm_param_sleeptime, sleep_time, strlen(sleep_time), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, scan_page, strlen(scan_page), rc);
    }
    return rc;
}

int OptsHandler::reset_ksm_param(Resource &r, OptsData &d) {
    int rc = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       FWRITE_STR(d.ksm_param_sleeptime, d.ksm_sleep_millisecs, strlen(d.ksm_sleep_millisecs), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, d.ksm_pages_to_scan, strlen(d.ksm_pages_to_scan), rc);
    }
    return rc;
}

int OptsHandler::lock_min_cores(Resource &r,  OptsData &d) {
    Target &t = Target::getCurTarget();
    if (d.core_ctl_present && t.isResourceSupported(r.major, r.minor)) {
        int rc = -1, coresInCluster;
        char tmp_s[NODE_MAX];
        unsigned int reqval = r.value;

        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        //TODO confirm this check implementation
        if (((t.getCoreCtlCpu() == 0) && (r.cluster != 0)) ||
            ((t.getCoreCtlCpu() == t.getLastCoreIndex(0)+1) && (r.cluster != 1))) {
            QLOGE("Warning: Core control support not present for lock_min_cores on cluster %d for this target", r.cluster);
            return FAILED;
        }

        if (reqval == MAX_LVL) {
            d.min_cores = atoi(d.core_ctl_min_cpu);
            FWRITE_STR(d.core_ctl_min_cpu_node, d.core_ctl_min_cpu, strlen(d.core_ctl_min_cpu), rc);
            QLOGI("perf_lock_rel: updating %s with %s", d.core_ctl_min_cpu_node, d.core_ctl_min_cpu);
            return rc;
        }

        d.min_cores = reqval;
        coresInCluster = t.getCoresInCluster(r.cluster);
        if ((coresInCluster >= 0) && (d.min_cores > coresInCluster)) {
            d.min_cores = coresInCluster;
        }

        if (d.min_cores > d.max_cores) {
            d.min_cores = d.max_cores;
        }

        snprintf(tmp_s, NODE_MAX, "%d", d.min_cores);
        FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
        QLOGI("perf_lock_acq: updating %s with %s", d.core_ctl_min_cpu_node, tmp_s);
        return rc;
    }
    else {
       QLOGI("lock_min_cores perflock is not supported");
       return FAILED;
    }
    return 0;
}

int OptsHandler::lock_max_cores(Resource &r,  OptsData &d) {
    char tmp_s[NODE_MAX];
    Target &t = Target::getCurTarget();
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (d.core_ctl_present > 0) {

       //TODO confirm this check implementation
       if (((t.getCoreCtlCpu() == 0) && (r.cluster != 0)) ||
           ((t.getCoreCtlCpu() == t.getLastCoreIndex(0)+1) && (r.cluster != 1))) {
           QLOGE("Warning: Core control support not present for lock_max_cores on cluster %d for this target", r.cluster);
           return FAILED;
       }

       if (reqval == MAX_LVL) {
           /* Update max_core first otherwise \
              min_core wiil not update */
           d.max_cores = atoi(d.core_ctl_max_cpu);
           FWRITE_STR(d.core_ctl_max_cpu_node, d.core_ctl_max_cpu, strlen(d.core_ctl_max_cpu), rc);
           QLOGI("perf_lock_rel: updating %s with %s ", d.core_ctl_max_cpu_node, d.core_ctl_max_cpu);
           if (d.min_cores > 0) {
               snprintf(tmp_s, NODE_MAX, "%d", d.min_cores);
               FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
               QLOGI("perf_lock_rel: updating %s with %s ", d.core_ctl_min_cpu_node, tmp_s);
           }
           return rc;
       }

       if (((int)reqval < t.getMinCoreOnline()) || ((int)reqval > t.getCoresInCluster(r.cluster))) {
           QLOGE("Error: perf-lock failed, invalid no. of cores requested to be online");
           return FAILED;
       }

       d.max_cores = reqval;
       snprintf(tmp_s, NODE_MAX, "%d", d.max_cores);
       FWRITE_STR(d.core_ctl_max_cpu_node, tmp_s, strlen(tmp_s), rc);
       QLOGI("perf_lock_acq: updating %s with %s ", d.core_ctl_max_cpu_node, tmp_s);
       return rc;
    }
    else if (d.kpm_hotplug_support > 0) {
       if (t.getCoresInCluster(r.cluster) <= 0) {
           QLOGW("Warning: Cluster %d does not exist, resource is not supported", r.cluster);
           return FAILED;
       }

       if (reqval == MAX_LVL) {
           if (r.cluster == 0) {
               d.lock_max_clust0 = -1;
           } else if (r.cluster == 1) {
               d.lock_max_clust1 = -1;
           }
       } else {
           d.max_cores = reqval;

           if (d.max_cores > t.getCoresInCluster(r.cluster)) {
               QLOGE("Error! perf-lock failed, invalid no. of cores requested to be online");
               return FAILED;
           }

           if (r.cluster == 0) {
               d.lock_max_clust0 = d.max_cores;
           } else if (r.cluster == 1) {
               d.lock_max_clust1 = d.max_cores;
           }
       }

       snprintf(tmp_s, NODE_MAX, "%d:%d", d.lock_max_clust0, d.lock_max_clust1);
       QLOGE("Write %s into %s", tmp_s, KPM_MAX_CPUS);
       FWRITE_STR(KPM_MAX_CPUS, tmp_s, strlen(tmp_s), rc);
       return rc;
    }
    else
       return FAILED;
    return 0;
}

/* Disable GPU Nap */
int OptsHandler::gpu_disable_gpu_nap(Resource &r, OptsData &d) {
    int rc = FAILED;
    unsigned int reqval = r.value;
    char tmp_s1[NODE_MAX];
    char tmp_s2[NODE_MAX];
    int ret[4] = {-1,-1,-1,-1};
    static char gpu_nap_s[4][NODE_MAX]; // input value node
    static int  gpu_nap_sl[4] = {-1,-1,-1,-1}; // input value string length
    static unsigned int stored_gpu_nap = 0;

    Target &target = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }
    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (gpu_nap_sl[0] > 0 && gpu_nap_sl[1] > 0 && gpu_nap_sl[2] && gpu_nap_sl[3]) {
            FWRITE_STR(GPU_FORCE_RAIL_ON, gpu_nap_s[0], gpu_nap_sl[0], ret[0]);
            FWRITE_STR(GPU_FORCE_CLK_ON, gpu_nap_s[1], gpu_nap_sl[1], ret[1]);
            FWRITE_STR(GPU_IDLE_TIMER, gpu_nap_s[2], gpu_nap_sl[2], ret[2]);
            FWRITE_STR(GPU_FORCE_NO_NAP, gpu_nap_s[3], gpu_nap_sl[3], ret[3]);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", GPU_FORCE_RAIL_ON, gpu_nap_s[0], ret[0]);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", GPU_FORCE_CLK_ON, gpu_nap_s[1], ret[1]);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", GPU_IDLE_TIMER, gpu_nap_s[2], ret[2]);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", GPU_FORCE_NO_NAP, gpu_nap_s[3], ret[3]);
            stored_gpu_nap = 0;
        }
        rc = ret[0] && ret[1] && ret[2] && ret[3];
        return rc;
    }

    if (!stored_gpu_nap) {
        FREAD_STR(GPU_FORCE_RAIL_ON, gpu_nap_s[0], NODE_MAX, ret[0]);
        FREAD_STR(GPU_FORCE_CLK_ON, gpu_nap_s[1], NODE_MAX, ret[1]);
        FREAD_STR(GPU_IDLE_TIMER, gpu_nap_s[2], NODE_MAX, ret[2]);
        FREAD_STR(GPU_FORCE_NO_NAP, gpu_nap_s[3], NODE_MAX, ret[3]);
        if (ret[0] > 0 && ret[1] > 0 && ret[2] > 0 && ret[3] > 0) {
            QLOGI("%s read with %s return value %d", GPU_FORCE_RAIL_ON, gpu_nap_s[0], ret[0]);
            QLOGI("%s read with %s return value %d", GPU_FORCE_CLK_ON, gpu_nap_s[1], ret[1]);
            QLOGI("%s read with %s return value %d", GPU_IDLE_TIMER, gpu_nap_s[2], ret[2]);
            QLOGI("%s read with %s return value %d", GPU_FORCE_NO_NAP, gpu_nap_s[3], ret[3]);
            gpu_nap_sl[0] = strlen(gpu_nap_s[0]);
            gpu_nap_sl[1] = strlen(gpu_nap_s[1]);
            gpu_nap_sl[2] = strlen(gpu_nap_s[2]);
            gpu_nap_sl[3] = strlen(gpu_nap_s[3]);
            stored_gpu_nap = 1;
        } else {
            if (ret[0] < 0)
                QLOGE("Failed to read %s", GPU_FORCE_RAIL_ON);
            if (ret[1] < 0)
                QLOGE("Failed to read %s", GPU_FORCE_CLK_ON);
            if (ret[2] < 0)
                QLOGE("Failed to read %s", GPU_IDLE_TIMER);
            if (ret[3] < 0)
                QLOGE("Failed to read %s", GPU_FORCE_NO_NAP);
            return FAILED;
        }
    }

    snprintf(tmp_s1, NODE_MAX, "%d", 1);
    snprintf(tmp_s2, NODE_MAX, "%d", 100000);
    FWRITE_STR(GPU_FORCE_RAIL_ON, tmp_s1, strlen(tmp_s1), ret[0]);
    FWRITE_STR(GPU_FORCE_CLK_ON, tmp_s1, strlen(tmp_s1), ret[1]);
    FWRITE_STR(GPU_IDLE_TIMER, tmp_s2, strlen(tmp_s2), ret[2]);
    FWRITE_STR(GPU_FORCE_NO_NAP, tmp_s1, strlen(tmp_s1), ret[3]);

    QLOGI("perf_lock_acq: updated %s with %s return value %d", GPU_FORCE_RAIL_ON, tmp_s1, ret[0]);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", GPU_FORCE_CLK_ON, tmp_s1, ret[1]);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", GPU_IDLE_TIMER, tmp_s2, ret[2]);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", GPU_FORCE_NO_NAP, tmp_s1, ret[3]);

    if (ret[0] < 0 && ret[1] < 0 && ret[2] < 0 && ret[3] < 0) {
        QLOGE("Acquiring GPU nap lock failed\n");
        return FAILED;
    }
    rc = ret[0] && ret[1] && ret[2] && ret[3];
    return rc;
}

/* Set irq_balancer */
int OptsHandler::irq_balancer(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    int tmp_s[MAX_CPUS];
    unsigned int reqval = r.value;
    Target &target = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.irq_bal_sp) {
            send_irq_balance(d.irq_bal_sp, NULL);
            d.stored_irq_balancer = 0;
            free(d.irq_bal_sp);
            d.irq_bal_sp = NULL;
        }
        return rc;
    }

    if (reqval > 0) {
        for (i = 0; i < target.getTotalNumCores(); i++)
            tmp_s[i] = (reqval & (1 << i)) >> i;

        if (!d.stored_irq_balancer) {
            if (!d.irq_bal_sp) {
                d.irq_bal_sp = (int *) malloc(sizeof(*d.irq_bal_sp) * MAX_CPUS);
            }
            send_irq_balance(tmp_s, &d.irq_bal_sp);
            d.stored_irq_balancer = 1;
        } else {
            send_irq_balance(tmp_s, NULL);
        }
    }

    return rc;
}

int OptsHandler::keep_alive(Resource &r, OptsData &d) {
    int rc = -1;
    tKillFlag = (r.value == 0) ? false : true;

    QLOGI("keep alive tKillFlag:%d\n", tKillFlag);
    pthread_mutex_lock(&ka_mutex);
    if (!isThreadAlive) {
        if(!tKillFlag) {
            rc = pthread_create(&id_ka_thread, NULL, keep_alive_thread, NULL);
            if (rc!=0)
            {
                QLOGE("Unable to create keepAlive thread, and error code is %d\n",rc);
            }
        }
    }
    pthread_mutex_unlock(&ka_mutex);
    return rc;
}

void* OptsHandler::keep_alive_thread(void*) {
    int len;
    int i;
    int tmp;
    char cmd[50];

    while (true) {
        pthread_mutex_lock(&ka_mutex);
        if (!tKillFlag) {
            isThreadAlive = true;
            pthread_mutex_unlock(&ka_mutex);
            len = sizeof (dnsIPs) / sizeof (*dnsIPs);
            i = rand()%len;
            snprintf(cmd, 30, "ping -c 1 %s",dnsIPs[i]);
            system(cmd);
            QLOGI("Hello KeepAlive~\n");
            sleep(5);
        } else {
            isThreadAlive = false;
            pthread_mutex_unlock(&ka_mutex);
            break;
        }
    }
    QLOGI("Keep Alive Thread has gone.~~~\n");
    pthread_exit(0);
    return NULL;
}

/* Return ADD_AND_UPDATE_REQUEST if reqVal is greater than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int OptsHandler::higher_is_better(unsigned int reqLevel, unsigned int curLevel) {
    int ret;

    if (reqLevel > curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGI("higher is better called , returning %d", ret);
    return ret;
}

/* Return ADD_AND_UPDATE_REQUEST if reqVal is lower than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int OptsHandler::lower_is_better(unsigned int reqLevel, unsigned int curLevel) {
    int ret;

    if (reqLevel < curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGI("lower_is_better called, returning %d", ret);
    return ret;
}

/* ALways return ADD_AND_UPDATE_REQUEST
 */
int OptsHandler::always_apply(unsigned int reqLevel, unsigned int curLevel) {

    QLOGI("always_apply called, returning %d", ADD_AND_UPDATE_REQUEST);
    return ADD_AND_UPDATE_REQUEST;
}
