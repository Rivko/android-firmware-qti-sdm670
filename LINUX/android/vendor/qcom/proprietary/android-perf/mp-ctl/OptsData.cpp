/******************************************************************************
  @file    OptsData.cpp
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>

#define LOG_TAG "ANDR-PERF-OPTSDATA"
#include <cutils/log.h>

#include <dirent.h>
#include "Request.h"

#include "OptsData.h"

OptsData OptsData::mOptsData;

OptsData::OptsData() {
    cpubw_avail_freqs_n = 0;
    is_ksm_supported = -1;
    kpm_hotplug_support = -1;
    lock_max_clust0 = -1;
    lock_max_clust1 = -1;
    core_ctl_present = -1;
    min_cores = 0;
    max_cores = 0xFF;
    min_freq_prop_0_set = false;
    min_freq_prop_4_set = false;
    min_freq_prop_0_val = 0;
    min_freq_prop_4_val = 0;
    irq_bal_sp = NULL;
    stored_irq_balancer = 0;
    is_active = false;

    for (int i=0; i<MAX_MINOR_RESOURCES; i++) {
        sysfsnode_storage_length[i] = -1;
        node_type[i] = SINGLE_NODE;
        is_supported[i] = true;
    }

    for (int i=0; i<MAX_INTERACTIVE_MINOR_OPCODE; i++) {
        cluster0_interactive_node_storage_length[i] = -1;
        cluster1_interactive_node_storage_length[i] = -1;
    }

    for (int i=0; i<MAX_CPUS; i++) {
        sch_cpu_pwr_cost_sl[i] = -1;
        sch_load_boost_sl[i] = -1;
    }
}

OptsData::~OptsData() {
    if (mHintData.timer_created) {
        timer_delete(mHintData.tTimerId);
    }
}

int OptsData::Init() {
    int rc = 0;
    Target &t = Target::getCurTarget();

    init_node_paths();
    init_cpubw_hwmon_path();
    init_llcbw_hwmon_path();
    parse_avail_freq_list();

    //check core control presence
    check_core_ctl_presence();
    //initailize core control min/max cpu nodes
    rc = core_ctl_init();
    if (rc > 0) {
        QLOGI("Core control cpu nodes initialized successfully");
    } else {
        QLOGE("Error: core_ctl not present or min/max cpu nodes initialization failed");
    }

    //check for KPM hot plug support
    kpm_hotplug_support = kpm_support_for_hotplug();

    if (core_ctl_present == 1 && kpm_hotplug_support == 1) {
        QLOGE("Error: KPM hotplug support and core control both are present at the same time");
    }


    if(FAILED == setup_cpu_freq_values()) {
      QLOGE("Frequency initialization failed. Exit perflock server");
      return -1;
    }

    memset(&mHintData, 0x00, sizeof(mHintData));

    irq_bal_sp = NULL;
    stored_irq_balancer = 0;

    //check sched frequency aggregation support presence
    mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();

    return 0;
}

/*Initializing the sysfs node paths for all the resources.
1. First we retrieve all the sysfs node related information from PerfDataStore, which
has updated information considering both common and target resource config files.
2. Initially all the nodes are assumed to be of default type i.e, single node.
3. Followed by all the required node path manipulations and modifications to the default
node types based on resource index which is the sum of it's major group start id and minor id.*/
void OptsData::init_node_paths() {
    int idx = 0, rc = FAILED;
    Target &t = Target::getCurTarget();
    char *pch = NULL;
    char d_name[NODE_MAX] = "", node_path[NODE_MAX] = "";

    for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
        memset(sysfsnode_path[idx], 0, sizeof(sysfsnode_path[idx]));
        memset(d_name, 0, sizeof(d_name));
        memset(node_path, 0, sizeof(node_path));

        rc = t.getConfigPerflockNode(idx, sysfsnode_path[idx], is_supported[idx]);
        if(rc == FAILED) {
            QLOGE("Couldn't find path for idx=%d", idx);
            continue;
        }
        QLOGI("Found path for index=%d as %s and supported=%d", idx, sysfsnode_path[idx], is_supported[idx]);
        if (!is_supported[idx]) {
            continue;
        }

        if (strncasecmp("SPECIAL_NODE", sysfsnode_path[idx], strlen("SPECIAL_NODE")) == 0) {
            /* If a node path starts with the key wprd "SPECIAL_NODE", then it's type is
            assigned as special_node and their ApplyOts and ResetOpts needs to be defined*/
            node_type[idx] = SPECIAL_NODE;
            continue;
        }
        if (idx >= INTERACTIVE_START_INDEX && idx < CPUBW_HWMON_START_INDEX) {
            /*Assumption, All the interactive nodes are part of the same Major group - INTERACTIVE.*/
            switch(idx) {
            /*For interactive_io_is_busy and interactive_timer_rate updating a single core updates
              all the cores of both clusters. This might create trouble when trying to acquire the both
              clusters with different values. So, restricting these two resources only for perf cluster.*/
            case INTERACTIVE_START_INDEX + INTERACTIVE_IO_IS_BUSY_OPCODE:
            case INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_RATE_OPCODE:
                node_type[idx] = INTERACTIVE_ALL_CORES;
                break;
            default:
                node_type[idx] = INTERACTIVE_NODE;
                break;
            }
            continue;
        }

        switch(idx) {
        case MISC_START_INDEX + STORAGE_CLK_SCALING_DISABLE_OPCODE:
        /*StorageNode path depends on whether the device is of type emmc or ufs. Thus
         the path provided in XML file is ignored and calculated seperately.*/
            strlcpy(sysfsnode_path[idx], t.getStorageNode(), NODE_MAX);
            node_type[idx] = SINGLE_NODE;
            break;

        case SCHED_START_INDEX + SCHED_PREFER_IDLE_OPCODE:
        case SCHED_START_INDEX + SCHED_MOSTLY_IDLE_LOAD_OPCODE:
        case SCHED_START_INDEX + SCHED_MOSTLY_IDLE_NR_RUN_OPCODE:
        /* Sysfs node paths for all the above three resources might change basing on the
        target. The resource is assigned to type UPDATE_ALL_CORES only if we have a cpu
        number 0 mentioned in it's path. */
            pch = strchr(sysfsnode_path[idx],'0');
            if (pch != NULL)
                node_type[idx] = UPDATE_ALL_CORES;
            break;

        case SCHED_START_INDEX + SCHED_MOSTLY_IDLE_FREQ_OPCODE:
        /* Sysfs node path might change basing on the target. The resource is assigned to type
        UPDATE_CORES_PER_CLUSTER only if we have a cpu number 0 mentioned in it's path. */
            pch = strchr(sysfsnode_path[idx],'0');
            if (pch != NULL)
                node_type[idx] = UPDATE_CORES_PER_CLUSTER;
            break;

        case SCHED_START_INDEX + SCHED_STATIC_CPU_PWR_COST_OPCODE:
        case SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE:
            node_type[idx] = SELECT_CORE_TO_UPDATE;
            break;

        case MEMLAT_START_INDEX + L3_MEMLAT_MINFREQ_OPCODE:
        /* Assuming that all the l3l_paths and l3b_paths are present in same folder for a given
        cluster. So, we can get these paths from minfreq path mentioned in XML. Considering the
        first cpu in each cluster.*/
            strlcpy(node_path, sysfsnode_path[idx], NODE_MAX);
            get_nodes_folder_path(sysfsnode_path[idx], d_name);
            snprintf(l3Cluster0_path, NODE_MAX, d_name, 0);
            snprintf(l3Cluster0_minfreq_path, NODE_MAX, node_path,0);

            snprintf(l3Cluster1_path, NODE_MAX, d_name, t.getLastCoreIndex(0)+1);
            snprintf(l3Cluster1_minfreq_path, NODE_MAX, node_path,t.getLastCoreIndex(0)+1);
            node_type[idx] = SPECIAL_NODE;
            init_l3_path();
            break;

        case POWER_COLLAPSE_START_INDEX + POWER_COLLAPSE_OPCODE:
        case CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE:
        case CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE:
        case SCHED_START_INDEX + SCHED_BOOST_OPCODE:
            node_type[idx] = SPECIAL_NODE;
            break;

        default:
            node_type[idx] = SINGLE_NODE;
            break;
        }
    }
}

/*For a given sysfs node path, returns it's folder path. If unable to
find the folder path, node path itself is returned.*/
void OptsData::get_nodes_folder_path(const char *node_path, char *folder_path) {
    char *pch = NULL;

    if (node_path == NULL) {
        return;
    }

    strlcpy(folder_path, node_path, NODE_MAX);
    pch = strrchr(folder_path,'/');
    if(pch != NULL) {
        *pch = '\0';
    } else {
        QLOGE("Unable to find the folders path for node=%s", node_path);
    }
}

int OptsData::init_available_cpubw_freq() {
   int rc = -1;
   char listf[FREQLIST_STR];
   char *cfL = NULL, *pcfL = NULL;
   char avlnode[NODE_MAX];

   snprintf(avlnode, NODE_MAX, "%s/available_frequencies", cpubw_path);

   FREAD_STR(avlnode, listf, FREQLIST_STR, rc);

   if (rc > 0) {
      listf[rc - 1] = '\0';
      QLOGI("Initializing available cpubw freq as %s", listf);
      cfL = strtok_r(listf, " ", &pcfL);
      if (cfL) {
         cpubw_avail_freqs[cpubw_avail_freqs_n++] = atoi(cfL);
         while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
             cpubw_avail_freqs[cpubw_avail_freqs_n++] = atoi(cfL);
             if(cpubw_avail_freqs_n >= MAX_FREQ) {
                QLOGE("Number of cpubw frequency is more than the size of the array. Exiting");
                return FAILED;
             }
         }
     }
   } else {
     QLOGW("Initialization of available cpubw freq failed, as %s not present", avlnode);
   }
   return SUCCESS;
}

int OptsData::init_devfreq_freqlist(char* parent_path) {
   int rc = -1;
   char listf[FREQLIST_STR];
   char *cfL = NULL, *pcfL = NULL;
   char avlnode[NODE_MAX];
   avail_freqs_n = 0;

   snprintf(avlnode, NODE_MAX, "%s/available_frequencies", parent_path);

   FREAD_STR(avlnode, listf, FREQLIST_STR, rc);

   if (rc > 0) {
      listf[rc - 1] = '\0';
      QLOGI("Initializing available freq as %s", listf);
      cfL = strtok_r(listf, " ", &pcfL);
      if (cfL) {
         avail_freqs[avail_freqs_n++] = atoi(cfL);
         while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
             avail_freqs[avail_freqs_n++] = atoi(cfL);
             if(avail_freqs_n >= MAX_FREQ) {
                QLOGE("Number of available frequency is more than the size of the array. Exiting");
                return FAILED;
             }
         }
      }
   }  else {
         QLOGW("Initialization of available freq failed, as %s not present", avlnode);
         return FAILED;
   }

   if (avail_freqs_n > 0 && avail_freqs[0] > 0)
       return SUCCESS;
   else
       return FAILED;
}

int OptsData::init_cpubw_hwmon_path() {
    int ret = FAILED;
    char d_name[NODE_MAX] = "";

    ret = init_devfreq_child_path("qcom,cpubw", d_name);
    if (ret < 0)
         return FAILED;

    snprintf(cpubw_path, NODE_MAX, "%s%s", DEVFREQ_PATH, d_name);
    init_available_cpubw_freq();
    snprintf(cpubw_hwmon_hyst_count_path, NODE_MAX, "%s/bw_hwmon/hyst_trigger_count", cpubw_path);
    snprintf(cpubw_hwmon_hyst_length_path, NODE_MAX, "%s/bw_hwmon/hyst_length", cpubw_path);
    snprintf(cpubw_hwmon_hist_memory_path, NODE_MAX, "%s/bw_hwmon/hist_memory", cpubw_path);

    return ret;
}

int OptsData::init_devfreq_child_path(const char *parent, char *d_name) {
    DIR *devfreq_path;
    struct dirent *ep;
    int ret = FAILED;

    if (!parent)
        return ret;

    devfreq_path = opendir(DEVFREQ_PATH);
    if (!devfreq_path) {
        QLOGE("Could not find devfreq path for %s", parent);
        return ret;
    }
    while ((ep = readdir (devfreq_path)) != NULL) {
        if (strstr(ep->d_name, parent) != NULL) {
            break;
        }
    }
    if ((ep != NULL) && (ep->d_name[0] != '\0') && (d_name != NULL)) {
        strlcpy(d_name, ep->d_name, NODE_MAX);
        ret = SUCCESS;
    }
    closedir(devfreq_path);
    return ret;
}

int OptsData::init_llcbw_hwmon_path() {
    char d_name[NODE_MAX] = "";
    int ret = FAILED;

    ret = init_devfreq_child_path("llccbw", d_name);
    if (ret < 0)
        return FAILED;

    snprintf(llcbw_path, NODE_MAX, "%s%s", DEVFREQ_PATH, d_name);
    snprintf(llcbw_maxfreq_path, NODE_MAX, "%s/max_freq", llcbw_path);
    snprintf(llcbw_hwmon_hist_memory_path, NODE_MAX, "%s/bw_hwmon/hist_memory", llcbw_path);
    snprintf(llcbw_hwmon_hyst_length_path, NODE_MAX, "%s/bw_hwmon/hyst_length", llcbw_path);
    snprintf(llcbw_hwmon_hyst_count_path, NODE_MAX, "%s/bw_hwmon/hyst_trigger_count", llcbw_path);

    ret = init_devfreq_freqlist(llcbw_path);

    if (ret == SUCCESS) {
        llcbw_avail_freqs_n = avail_freqs_n;
        for (int i = 1; i < llcbw_avail_freqs_n; i++) {
            llcbw_avail_freqs[i] = avail_freqs[i];
            QLOGI("LLCBW, avail freq is %d", llcbw_avail_freqs[i]);
        }
        return SUCCESS;
    }
    else
        return FAILED;
}

int OptsData::init_l3_path() {

    char d_name[NODE_MAX] = "";
    int ret = FAILED;
    Target &t = Target::getCurTarget();

    /* for first CPU of cluster0 */
    snprintf(l3Cluster0_maxfreq_path, NODE_MAX, "%s/max_freq", l3Cluster0_path);
    ret = init_devfreq_freqlist(l3Cluster0_path);
    if (ret == SUCCESS) {
        l3Cluster0_avail_freqs_n = avail_freqs_n;
        for (int i = 0; i < l3Cluster0_avail_freqs_n; i++) {
            l3Cluster0_avail_freqs[i] = avail_freqs[i];
            QLOGI("L3 CPU0, avail freq is %d", l3Cluster0_avail_freqs[i]);
        }
    }

    /* for first CPU of cluster1*/
    snprintf(l3Cluster1_maxfreq_path, NODE_MAX, "%s/max_freq", l3Cluster1_path);
    ret = init_devfreq_freqlist(l3Cluster1_path);
    if (ret == SUCCESS) {
        l3Cluster1_avail_freqs_n = avail_freqs_n;
        for (int i = 0; i < l3Cluster1_avail_freqs_n; i++) {
            l3Cluster1_avail_freqs[i] = avail_freqs[i];
            QLOGI("L3 CPU%d, avail freq is %d", t.getLastCoreIndex(0)+1, l3Cluster1_avail_freqs[i]);
        }
        return SUCCESS;
    }
    else
        return FAILED;
}

int OptsData::init_ksm() {
    int rc = 0;

    snprintf(ksm_run_node, NODE_MAX, KSM_RUN_NODE);
    snprintf(ksm_param_sleeptime, NODE_MAX, KSM_SLEEP_MILLI_SECS_NODE);
    snprintf(ksm_param_pages_to_scan, NODE_MAX, KSM_PAGES_TO_SCAN_NODE);

    is_ksm_supported = access(ksm_run_node, F_OK);
    if(is_ksm_supported == 0)
    {
        memset(ksm_sleep_millisecs, 0, sizeof(ksm_sleep_millisecs));
        memset(ksm_pages_to_scan, 0, sizeof(ksm_pages_to_scan));
        FREAD_STR(ksm_param_sleeptime, ksm_sleep_millisecs, sizeof(ksm_sleep_millisecs), rc);
        FREAD_STR(ksm_param_pages_to_scan, ksm_pages_to_scan, sizeof(ksm_pages_to_scan), rc);
    }
    return is_ksm_supported;
}

/* toggle KSM
   0 -- KSM won't run
   1 -- KSM will run */
int OptsData::toggle_ksm_run(int run) {
    int rc=0;
    if(is_ksm_supported == 0)
    {
        if(run == 0)
        {
            /* Disable KSM */
            FWRITE_STR(ksm_run_node, "0", 1, rc);
        }
        else if(run == 1)
        {
            /* Enable KSM */
            FWRITE_STR(ksm_run_node, "1", 1, rc);
        }
    }
    return rc;
}

void OptsData::parse_avail_freq_list() {
    int rc = 0;
    char *cfL = NULL, *pcfL = NULL;

    /* Parse list of available frequencies */
    FREAD_STR(c0f, c0fL_s, FREQLIST_STR, rc);
    if (rc > 0) {
        c0fL_s[rc - 1] = '\0';
        cfL = strtok_r(c0fL_s, " ", &pcfL);
        if (cfL) {
            c0fL[c0fL_n++] = atoi(cfL);
            while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
                c0fL[c0fL_n++] = atoi(cfL);
            }
        }
    }
    return;
}

int OptsData::kpm_support_for_hotplug() {
    int rc = 0;
    char kpm_max_cpus[NODE_MAX];

    //check for KPM hot plug support
    FREAD_STR(KPM_MAX_CPUS, kpm_max_cpus, NODE_MAX, rc);
    if (rc > 0) {
        kpm_hotplug_support = 1;
    } else
        kpm_hotplug_support = 0;

    return kpm_hotplug_support;
}


int OptsData::check_core_ctl_presence() {
    int rc = -1;
    Target &t = Target::getCurTarget();

    if (t.getCoreCtlCpu() == -1) {
        QLOGE("Error: core_ctl_cpu is not initialized for this target");
        return -1;
    } else {
        snprintf(core_ctl_min_cpu_node,NODE_MAX,CORE_CTL_MIN_CPU,t.getCoreCtlCpu());
        snprintf(core_ctl_max_cpu_node,NODE_MAX,CORE_CTL_MAX_CPU,t.getCoreCtlCpu());
        FREAD_STR(core_ctl_min_cpu_node, core_ctl_min_cpu, NODE_MAX, rc);
        if (rc > 0) {
            core_ctl_present = 1;
        } else {
            core_ctl_present = 0;
        }
    }
    return 1;
}

/*Initializing both min_cores and max_cores variables.*/
int OptsData::core_ctl_init() {
    int rc = 1;

    if(core_ctl_present > 0) {
    /*During restore of the sysfsnodes core_ctl_max_cpu and core_ctl_min_cpu we use property_get
      and this restoration is done after reading the values max_cores and min_cores. So, there
      is a chance that both max_cores and min_cores are initialized to  incorrect values. To
      avoid this using proprty_get to read both these values, same as in restore handler.*/
        if (property_get("ro.vendor.qti.core_ctl_max_cpu", core_ctl_max_cpu, NULL) > 0) {
            QLOGI("%s read with %s ", core_ctl_max_cpu_node, core_ctl_max_cpu);
            max_cores = atoi(core_ctl_max_cpu);
        } else {
            rc = -1;
            QLOGE("Warning: core_ctl_max_cpu property not defined, cannot read max_cores.");
        }

        if (property_get("ro.vendor.qti.core_ctl_min_cpu", core_ctl_min_cpu, NULL) > 0) {
            QLOGI("%s read with %s ", core_ctl_min_cpu_node, core_ctl_min_cpu);
            min_cores = atoi(core_ctl_min_cpu);
        } else {
            rc = -1;
            QLOGE("Warning: core_ctl_min_cpu property not defined, cannot read min_cores.");
        }
        return rc;
    } else {
       QLOGE("Error: Core ctl not present");
       return -1;
    }
}

int OptsData::setup_cpu_freq_values()
{
  int i = 0;
  int rc;
  char min_freq_prop_0[PROPERTY_VALUE_MAX];
  char min_freq_prop_4[PROPERTY_VALUE_MAX];
  //TODO: this should be removed now.
  /* Check for min_freq_prop and if set read the min_freq*/
  if (property_get("ro.min_freq_0", min_freq_prop_0, NULL) > 0) {
     if (isdigit(min_freq_prop_0[0])) {
        min_freq_prop_0_val = atoi(min_freq_prop_0);
        min_freq_prop_0_set = true;
     }
  }

  if (property_get("ro.min_freq_4", min_freq_prop_4, NULL) > 0) {
     if (isdigit(min_freq_prop_4[0])) {
        min_freq_prop_4_val = atoi(min_freq_prop_4);
        min_freq_prop_4_set = true;
     }
  }

  for (i = 0; i < Target::getCurTarget().getTotalNumCores(); i++ )
      if(FAILED == init_available_freq(i))
         return FAILED;

  return SUCCESS;
}

int OptsData::get_reset_cpu_freq(int cpu, int ftype)
{
   if (cpu_freq_val[cpu].valid != 1) {
      QLOGE("Error: Perf-lock release for an un-initialized cpu %d", cpu);
      return FAILED;
   }
   else {
      if (ftype == MIN_FREQ_REQ)
         return cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].min_freq_pos];
      else if (ftype == MAX_FREQ_REQ)
         return cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count-1];
   }
  return FAILED;
}

unsigned int OptsData::find_next_cpubw_available_freq(unsigned int freq) {
    int setval = -1, i = 0;

    for (i = 0; i < cpubw_avail_freqs_n; i++) {
        if (cpubw_avail_freqs[i] >= freq) {
            setval = cpubw_avail_freqs[i];
            break;
        }
    }

    if (i == cpubw_avail_freqs_n) {
        setval = cpubw_avail_freqs[i - 1];
    }
    return setval;
}

unsigned int OptsData::find_next_avail_freq(unsigned int freq) {
    unsigned int setval = freq;
    int i = 0;

    for (i = 0; i < c0fL_n; i++) {
        if (c0fL[i] >= freq) {
            setval = c0fL[i];
            break;
        }
    }

    if (i == c0fL_n) {
        setval = c0fL[i - 1];
    }
    return setval;
}

int OptsData::find_next_cpu_frequency(int cpureq, unsigned int freq) {
   int i, clust_id, clust_cpu_num, cluster = -1;
   int is_success = 0;
   Target &t = Target::getCurTarget();
   int startcpu, endcpu;
   int cpu;

   cluster = t.getClusterForCpu(cpureq, startcpu, endcpu);
   if ((cluster < 0) || (startcpu < 0) || (endcpu < 0)){
       QLOGE("Invalid first and last cpu in the cluster for cpu %d", cpureq);
       return FAILED;
   }

   for (cpu = startcpu; cpu <= endcpu; cpu++) {
        if (cpu_freq_val[cpu].valid != 1)
            if(FAILED == init_available_freq(cpu))
               continue;

        if (cpu_freq_val[cpu].valid != 1)
            continue;

        is_success = 1;
        break;
   }
   if (!is_success)
       return FAILED;

   for (i = cpu_freq_val[cpu].min_freq_pos; i < cpu_freq_val[cpu].count ; i++)
       if (cpu_freq_val[cpu].avl_freq[i] >= freq)
          return cpu_freq_val[cpu].avl_freq[i];

   if (i == cpu_freq_val[cpu].count)
      return cpu_freq_val[cpu].avl_freq[i-1];

   return FAILED;
}

int OptsData::init_available_freq(int cpu) {
   int rc = -1;
   char listf[FREQLIST_STR];
   char *cfL = NULL, *pcfL = NULL;
   char avlnode[NODE_MAX];

   if (cpu < 0 || cpu >= Target::getCurTarget().getTotalNumCores()) {
      QLOGE("Incorrect cpu%d" ,cpu);
      return 0;
   }

   snprintf(avlnode, NODE_MAX, AVL_FREQ_NODE, cpu);
   FREAD_STR(avlnode, listf, FREQLIST_STR, rc);
   if (rc > 0) {
      listf[rc - 1] = '\0';
      QLOGI("Initializing available freq for core %d as %s", cpu, listf);
      cfL = strtok_r(listf, " ", &pcfL);
      if (cfL) {
         cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count++] = atoi(cfL);
         while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
               cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count++] = atoi(cfL);
               if(cpu_freq_val[cpu].count >= MAX_FREQ) {
                  QLOGE("Number of frequency is more than the size of the array.Exiting");
                  return FAILED;
               }
         }
         cpu_freq_val[cpu].valid = 1;
         cpu_freq_val[cpu].min_freq_pos = 0;
         check_min_freq_prop_set(cpu);
     }
   } else {
     QLOGW("Initialization of available freq for core %d failed, as %s not present", cpu, avlnode);
   }
   return SUCCESS;
}

void OptsData::check_min_freq_prop_set(int cpu) {
    Target &t = Target::getCurTarget();

    if ((cpu <= t.getLastCoreIndex(0)) && (min_freq_prop_0_set))
    {
        cpu_freq_val[cpu].min_freq_pos = find_frequency_pos(cpu, min_freq_prop_0_val);
        if (cpu_freq_val[cpu].min_freq_pos == -1) {
            QLOGW("Warning: min prop frequency not found setting it to default" );
            cpu_freq_val[cpu].min_freq_pos = 0;
        }
    }
    else if ((cpu > t.getLastCoreIndex(0)) && (cpu <= t.getLastCoreIndex(1)) && min_freq_prop_4_set)
    {
        cpu_freq_val[cpu].min_freq_pos = find_frequency_pos(cpu, min_freq_prop_4_val);
        if (cpu_freq_val[cpu].min_freq_pos == -1) {
            QLOGW("Warning: min prop frequency not found setting it to default" );
            cpu_freq_val[cpu].min_freq_pos = 0;
        }
    }
    return;
}


/*  Caller of the function should take care of the
 *  error condition when frequency is not found
 */
int OptsData::find_frequency_pos(int cpu, int freq) {
  int i;
  for (i = 0; i < cpu_freq_val[cpu].count; i++) {
      if (cpu_freq_val[cpu].avl_freq[i] == freq)
             return i;
  }
  return -1;
}


