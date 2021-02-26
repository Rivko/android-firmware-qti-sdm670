/******************************************************************************
  @file    RestoreHandler.cpp
  @brief   Implementation of nodes restoration

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

#define LOG_TAG "ANDR-PERF-RESETHANDLER"
#include <cutils/log.h>

#include "Request.h"

#define RESET_SCHED_BOOST   0

#include "OptsData.h"
#include "RestoreHandler.h"
#include "MpctlUtils.h"

#define DEFAULT_VALUES_FILE "/data/vendor/perfd/default_values"

//strings for storing and identifying the sysfs node values in the default values file.
#define INDEX_ONLY "index_%d"
#define INDEX_WITH_CORE "index_%d_core_%d"
#define INDEX_WITH_CLUSTER "index_%d_cluster_%d"

ResetHandler ResetHandler::mResetHandler;

ResetHandler::ResetHandler() {
}

ResetHandler::~ResetHandler() {
}

int ResetHandler::Init() {
    return 0;
}

void ResetHandler::reset_to_default_values(OptsData &d) {
    FILE *defval;
    int j;
    int i, rc, cpu, idx;
    char buf[NODE_MAX], val_str[NODE_MAX], buf2[NODE_MAX], tmp[NODE_MAX];

    Target &t = Target::getCurTarget();
    int clusterNum, startCpu, endCpu;

    clusterNum = t.getNumCluster();
    defval = fopen(DEFAULT_VALUES_FILE, "a+");
    if (defval == NULL) {
        QLOGE("Cannot open/create default values file");
        return;
    }

    fseek (defval, 0, SEEK_END);

    if (ftell(defval) == 0) {
        //All the special node types and resource with multiple node updations are hard coded.
        write_to_file(defval, "ksm_run_node", d.ksm_run_node);
        write_to_file(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_file(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_file(defval, "gpu_force_rail_on", GPU_FORCE_RAIL_ON);
        write_to_file(defval, "gpu_force_clk_on", GPU_FORCE_CLK_ON);
        write_to_file(defval, "gpu_idle_timer", GPU_IDLE_TIMER);
        write_to_file(defval, "gpu_force_no_nap", GPU_FORCE_NO_NAP);
        write_to_file(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_file(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_file(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_file(defval, "llchistmem", d.llcbw_hwmon_hist_memory_path);
        write_to_file(defval, "llchystlen", d.llcbw_hwmon_hyst_length_path);
        write_to_file(defval, "llchystcnt", d.llcbw_hwmon_hyst_count_path);
        write_to_file(defval, "l3Cluster0minf", d.l3Cluster0_minfreq_path);
        write_to_file(defval, "l3Cluster1minf", d.l3Cluster1_minfreq_path);
        write_to_file(defval, "foreground_boost", SCHED_FOREGROUND_BOOST);

        for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
            if (!d.is_supported[idx])
                continue;

            strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
            switch (d.node_type[idx]) {
            case INTERACTIVE_NODE:
            case INTERACTIVE_ALL_CORES:
            /*For cluster based all the interactive nodes of same cluster have same values.
              So, we only store the values of first online cpu of each cluster in the format
              "index_resourceId_with_clusterId". For single gov instance type, we can store the
              value with format "index_resourceId" */
                if (t.getGovInstanceType() == CLUSTER_BASED_GOV_INSTANCE) {
                    for (i = 0, startCpu = 0; i < clusterNum; i++) {
                        cpu = get_online_core(startCpu, t.getLastCoreIndex(i));
                        buf[CPU_INDEX] = cpu + '0';
                        snprintf(tmp, NODE_MAX, INDEX_WITH_CLUSTER, idx, i);
                        QLOGI("writing from node %s to default file as : %s", buf, tmp);
                        write_to_file(defval, tmp, buf);
                        startCpu = t.getLastCoreIndex(i)+1;
                    }
                } else {
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    QLOGI("writing from node %s to default file as : %s", buf, tmp);
                    write_to_file(defval, tmp, buf);
                }
                break;

            case SELECT_CORE_TO_UPDATE:
            /*For this node_type we store the values of all the cores in format
              "Index_resourceId_with_coreId", as we can have individual perflock calls
              for all the cores.*/
                for (i = 0; i < t.getTotalNumCores(); i++) {
                    buf[CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_WITH_CORE, idx, i);
                    QLOGI("writing from node %s to default file as : %s", buf, tmp);
                    write_to_file(defval, tmp, buf);
                }
                break;

            case SPECIAL_NODE:
            /*All the nodes of special type need to be written seperately, as they
              might have multitple sysfs nodes to store.*/
                break;

            default:
            /*All the nodes of type single, update_all_cores and update_cores_per_cluster
              can be stored with format "index_resourceId".*/
                snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                QLOGI("writing from node %s to default file as : %s", buf, tmp);
                write_to_file(defval, tmp, buf);
                break;
            }
        }

        fwrite("File created", sizeof(char), strlen("File created"), defval);
   } else {
        reset_freq_to_default(d);

        reset_cores_status(d);
        //Even while restoring, all the special node types are hard coded.
        write_to_node(defval, "ksm_run_node", d.ksm_run_node);
        write_to_node(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_node(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_node(defval, "gpu_force_rail_on", GPU_FORCE_RAIL_ON);
        write_to_node(defval, "gpu_force_clk_on", GPU_FORCE_CLK_ON);
        write_to_node(defval, "gpu_idle_timer", GPU_IDLE_TIMER);
        write_to_node(defval, "gpu_force_no_nap", GPU_FORCE_NO_NAP);
        write_to_node(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_node(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_node(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_node(defval, "llchistmem", d.llcbw_hwmon_hist_memory_path);
        write_to_node(defval, "llchystlen", d.llcbw_hwmon_hyst_length_path);
        write_to_node(defval, "llchystcnt", d.llcbw_hwmon_hyst_count_path);
        write_to_node(defval, "l3Cluster0minf", d.l3Cluster0_minfreq_path);
        write_to_node(defval, "l3Cluster1minf", d.l3Cluster1_minfreq_path);

        for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
            if (!d.is_supported[idx])
                continue;

            strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
            switch (d.node_type[idx]) {
            case UPDATE_ALL_CORES:
            case UPDATE_CORES_PER_CLUSTER:
            /*For these node types, when storing it is enough to store only the values
              of first cpu. But when restoring it is required to update all the cores,
              as these node_types modify all the cores for an acquire call. */
                for (i = 0; i < t.getTotalNumCores(); i++) {
                    buf[CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    QLOGI("writing from default file as : %s to node %s", tmp, buf);
                    write_to_node(defval, tmp, buf);
                }
                break;

            case SELECT_CORE_TO_UPDATE:
            /*For this node type the same way as storing we restore for all the cores.*/
                for (i = 0; i < t.getTotalNumCores(); i++) {
                    buf[CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_WITH_CORE, idx, i);
                    QLOGI("writing from default file as : %s to node %s", tmp, buf);
                    write_to_node(defval, tmp, buf);
                }
                break;

            case SINGLE_NODE:
                snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                QLOGI("writing from default file as : %s to node %s", tmp, buf);
                write_to_node(defval, tmp, buf);
                break;

            default:
            /*All the interactive and special node types are restored separately.*/
                break;
            }
        }

        //restore foreground/boost/cpus after restoring foreground/cpus
        write_to_node(defval, "foreground_boost", SCHED_FOREGROUND_BOOST);

        /* resotring of interactive nodes done differently by calling the function reset_cpu_nodes,
        inorder to retain the previous method.*/
        if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
            startCpu = 0; //core 0 cluster 0
            for (i = 0; i < clusterNum; i++) {
                if ((cpu = get_online_core(startCpu, t.getLastCoreIndex(i))) != FAILED) {
                    reset_cpu_nodes(cpu);
                }
                startCpu = t.getLastCoreIndex(i)+1;
            }
        } else {
            for (idx = INTERACTIVE_START_INDEX; idx < CPUBW_HWMON_START_INDEX; ++idx) {
                if (d.is_supported[idx]) {
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    write_to_node(defval, tmp, d.sysfsnode_path[idx]);
                }
            }
        }

        reset_sched_boost(d);
    }
    fclose(defval);
}

void ResetHandler::reset_freq_to_default(OptsData &d) {
    int i = 0, prevcores = 0;
    int rc = -1;
    Target &t = Target::getCurTarget();
    unsigned int restoreVal = 0;
    char tmp_s[NODE_MAX] = "";

    for (i=0, prevcores = 0; i < t.getNumCluster(); i++) {
        rc = update_freq_node(prevcores, t.getLastCoreIndex(i) , 0, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGI("reset_freq_to_default reset min freq req for CPUs %d-%d: %s", prevcores, t.getLastCoreIndex(i), tmp_s);
            FWRITE_STR(d.sysfsnode_path[CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE], tmp_s, strlen(tmp_s), rc);
        }

        restoreVal = t.getCpuMaxFreqResetVal(i);
        rc = update_freq_node(prevcores, t.getLastCoreIndex(i), restoreVal, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGI("reset_freq_to_default reset max freq req for CPUs %d-%d: %s", prevcores, t.getLastCoreIndex(i), tmp_s);
            FWRITE_STR(d.sysfsnode_path[CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE], tmp_s, strlen(tmp_s), rc);
        }
        prevcores += t.getLastCoreIndex(i) + 1;
    }
}

void ResetHandler::reset_cores_status(OptsData &d) {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    Target &t = Target::getCurTarget();

    if (d.core_ctl_present > 0) {
        if (property_get("ro.vendor.qti.core_ctl_max_cpu", tmp_s, NULL) > 0) {
            FWRITE_STR(d.core_ctl_max_cpu_node, tmp_s, strlen(tmp_s), rc);
            if (rc < 1) {
                QLOGE("Warning: core_ctl_max_cpu not updated, write failed");
            } else {
                QLOGI("Updating %s with %s and return value %d", d.core_ctl_max_cpu_node, tmp_s, rc);
            }
        } else {
            QLOGE("Warning: core_ctl_max_cpu property not defined, can not reset");
        }

        if (property_get("ro.vendor.qti.core_ctl_min_cpu", tmp_s, NULL) > 0) {
            FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
            if (rc < 1) {
                QLOGE("Warning: core_ctl_min_cpu not updated, write failed");
            } else {
                QLOGI("Updating %s with %s and return value %d", d.core_ctl_min_cpu_node, tmp_s, rc);
            }
        } else {
            QLOGE("Warning: core_ctl_min_cpu property not defined, can not reset");
        }
    }
    else if (d.kpm_hotplug_support > 0){
        snprintf(tmp_s, NODE_MAX, "%d:%d", t.getCoresInCluster(0), t.getCoresInCluster(1));
        FWRITE_STR(KPM_MAX_CPUS, tmp_s, strlen(tmp_s), rc);
        if (rc > 0) {
            QLOGE("Reset cores success");
        } else {
            //QLOGE("Could not update the %s node \n", KPM_MANAGED_CPUS);
        }
    }
    else {
        QLOGE("Error: KPM hotplug and core control both are not present, can't reset");
    }
}

void ResetHandler::reset_sched_boost(OptsData &d) {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    snprintf(tmp_s, NODE_MAX, "%d", RESET_SCHED_BOOST);
    FWRITE_STR(d.sysfsnode_path[SCHED_START_INDEX + SCHED_BOOST_OPCODE], tmp_s, strlen(tmp_s), rc);
}

void ResetHandler::reset_cpu_nodes(int cpu) {
    FILE *defval;
    char buf[NODE_MAX] = "";
    char tmp[NODE_MAX] = "";
    int rc = FAILED, cluster = 0, idx = 0;
    int startcpu = 0, endcpu = 0;
    Target &t = Target::getCurTarget();
    OptsData &d = OptsData::getInstance();

    defval = fopen(DEFAULT_VALUES_FILE, "r");
    if(defval == NULL) {
        QLOGE("Cannot read default values file");
        return;
    }
    //TODO: confirm
    cluster = t.getClusterForCpu(cpu, startcpu, endcpu);
    if (cluster >= 0) {
        for (idx = INTERACTIVE_START_INDEX; idx < CPUBW_HWMON_START_INDEX; ++idx) {
            if (d.is_supported[idx]) {
                strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
                buf[CPU_INDEX] = cpu + '0';
                snprintf(tmp, NODE_MAX, INDEX_WITH_CLUSTER, idx, cluster);
                QLOGI("writing from %s to node: %s", tmp, buf);
                rc = write_to_node(defval, tmp, buf);
                if ((rc <= 0) && (SINGLE_GOV_INSTANCE != t.getGovInstanceType()))
                    signal_chk_poll_thread(buf, rc);
            }
        }
    }

    fclose(defval);
    QLOGI("CPU:%u Reset Nodes relevant to Profile Manager", cpu);
    return;
}
