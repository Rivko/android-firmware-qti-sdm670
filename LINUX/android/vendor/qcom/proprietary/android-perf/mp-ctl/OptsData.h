/******************************************************************************
  @file    OptsData.h
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_DATA__H_
#define __OPTS_DATA__H_

#include <cstdlib>
#include <fcntl.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <unordered_set>
#include "Target.h"
#include "MpctlUtils.h"
#include <utils/TypeHelpers.h>

#define SYSFS_PREFIX            "/sys/devices/system/"
#define DEVFREQ_PATH            "/sys/class/devfreq/"
#define CPUP                    "/sys/devices/system/cpu/present"

#define SCHED_FOREGROUND_BOOST "/dev/cpuset/foreground/boost/cpus"
/*SCHED_FREQ_AGGREGATE_NODE is being used in class sched_freq_aggr_group_data
to decide whether the node is presnet ot not.*/
#define SCHED_FREQ_AGGREGATE_NODE  "/proc/sys/kernel/sched_freq_aggregate"

#define KPM_MAX_CPUS            "/sys/module/msm_performance/parameters/max_cpus"

#define KSM_RUN_NODE  "/sys/kernel/mm/ksm/run"
#define KSM_SLEEP_MILLI_SECS_NODE "/sys/kernel/mm/ksm/sleep_millisecs"
#define KSM_PAGES_TO_SCAN_NODE "/sys/kernel/mm/ksm/pages_to_scan"

//used in target.cpp
#define GPU_AVAILABLE_FREQ  "/sys/class/kgsl/kgsl-3d0/devfreq/available_frequencies"
#define GPU_BUS_AVAILABLE_FREQ  "/sys/class/devfreq/soc:qcom,gpubw/available_frequencies"

//following 4 are updated in same call.
#define GPU_FORCE_RAIL_ON   "/sys/class/kgsl/kgsl-3d0/force_rail_on"
#define GPU_FORCE_CLK_ON    "/sys/class/kgsl/kgsl-3d0/force_clk_on"
#define GPU_IDLE_TIMER      "/sys/class/kgsl/kgsl-3d0/idle_timer"
#define GPU_FORCE_NO_NAP    "/sys/class/kgsl/kgsl-3d0/force_no_nap"

//lock min cores and lock max cores
#define CORE_CTL_MIN_CPU        (SYSFS_PREFIX"cpu/cpu%d/core_ctl/min_cpus")
#define CORE_CTL_MAX_CPU        (SYSFS_PREFIX"cpu/cpu%d/core_ctl/max_cpus")

#define AVL_FREQ_NODE          (SYSFS_PREFIX"cpu/cpu%d/cpufreq/scaling_available_frequencies")
#define CPUINFO_FREQ_NODE      (SYSFS_PREFIX"cpu/cpu%d/cpufreq/cpuinfo_%s_freq")
#define STORAGE_EMMC_CLK_SCALING_DISABLE "/sys/class/mmc_host/mmc0/clk_scaling/enable"
#define STORAGE_UFS_CLK_SCALING_DISABLE "/sys/class/scsi_host/host0/../../../clkscale_enable"
#define KERNEL_VERSION_NODE  "/proc/sys/kernel/osrelease"

/*TODO Check its usage*/
#define c0f   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"                    // cpu0 available freq

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define NODE_MAX                150

#define MAX_CPUS            8

#define MAX_FREQ_CPUBW      50
#define MAX_FREQ_LLCBW      50
#define MAX_FREQ_L3L        50
#define MAX_FREQ_L3B        50
#define FREQLIST_MAX            32
#define FREQLIST_STR            1024
#define ENABLE_PC_LATENCY   0

#define CPU_INDEX 27  //cpu index in all sysfs node paths related to managing cores

#define MAX_FREQ            50
#define FREQ_MULTIPLICATION_FACTOR  1000
#define TIMER_MULTIPLICATION_FACTOR 1000
#define FREAD_STR(fn, pstr, len, rc)    { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_RDONLY);       \
                                          if (fd >= 0) {                 \
                                              rc = read(fd, pstr, len);  \
                                              pstr[len-1] = '\0';        \
                                              close(fd); \
                                          } \
                                        }
#define FWRITE_STR(fn, pstr, len, rc)   { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_WRONLY);       \
                                          if (fd >= 0) {                 \
                                              rc = write(fd, pstr, len); \
                                              close(fd);                 \
                                          }                              \
                                        }
#define FOVRWT_STR(fn, pstr, len, rc)   { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR); \
                                          if (fd >= 0) {                 \
                                              rc = write(fd, pstr, len); \
                                              fsync(fd);                 \
                                              close(fd);                 \
                                          }                              \
                                        }

static const char *dnsIPs[] = {
    "8.8.8.8",/* Google */
    "8.8.4.4",/* Google */
    "114.114.114.114", /* 114DNS */
    "114.114.115.115", /* 114DNS */
    "1.2.4.8", /* CNNIC SDNS */
    "210.2.4.8", /* CNNIC SDNS */
    "223.5.5.5", /*  Ali DNS */
    "223.6.6.6", /* Ali DNS */
    "216.146.35.35", /* Dyn DNS */
    "216.146.36.36", /* Dyn DNS */
    "208.67.222.123", /* OpenDNS */
    "208.67.220.123" /* OpenDNS */
};

struct cpu_freq_mapping {
    const char * min;
    const char * max;
    const char * online;
    int in_use;
};

struct cpu_freq_resource_value {
    int avl_freq[MAX_FREQ];
    int count;
    int min_freq_pos;
    int valid;
};

typedef struct hint_associated_data {
    struct sigevent mSigEvent;
    struct itimerspec mInterval;
    timer_t tTimerId;
    int disp_single_layer;
    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;
    int timer_created;
}hint_associated_data;

#define HWUI_SCHED_GROUP_SIZE 5
struct thread_group {
    int *tids;
    int num_tids;
    thread_group(): tids(NULL), num_tids(0) {
    }
};

class sched_group_data {
private:
    typedef android::key_value_pair_t<int,
                         thread_group> sched_group_key_value;

    struct HashForEntry : private std::unary_function<sched_group_key_value*, android::hash_t> {
        android::hash_t operator() (const sched_group_key_value* entry) const {
            return android::hash_type(entry->key);
        };
    };

    struct EqualityForHashedEntries : private std::unary_function<sched_group_key_value*, bool> {
        bool operator() (const sched_group_key_value* lhs, const sched_group_key_value* rhs) const {
            return lhs->key == rhs->key;
        };
    };


    typedef std::unordered_set<sched_group_key_value*, HashForEntry, EqualityForHashedEntries> mSchedGroupSet;

    typename mSchedGroupSet::iterator findByKey(const int& key) {
        sched_group_key_value entryForSearch(key, thread_group());
        typename mSchedGroupSet::iterator result = mSchedGroupHashSet.find(&entryForSearch);
        return result;
    }

    mSchedGroupSet mSchedGroupHashSet;

public:
    bool Add(int tid, thread_group tids_of_interest) {
        if (findByKey(tid) != mSchedGroupHashSet.end())
            return false;
        sched_group_key_value* newEntry = new sched_group_key_value(tid, tids_of_interest);
        mSchedGroupHashSet.insert(newEntry);

        return true;
    }

    thread_group Get(int tid) {
        thread_group t;
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return t;
        sched_group_key_value *entry = *find_result;
        return entry->value;
    }

    thread_group Remove(int tid) {
        thread_group ret;
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return ret;
        sched_group_key_value *entry = *find_result;
        ret = entry->value;
        mSchedGroupHashSet.erase(entry);
        delete(entry);
        return ret;
    }

    static bool isHWUIThread(char *name) {
        const char *thread_names[3] = { "RenderThread",
                                        "hwuiTask1",
                                        "hwuiTask2" };
        for (int i = 0; i < 3; i++) {
            char *eoline = strstr(name, "\n");
            char *out = strstr(name, thread_names[i]);
            if (eoline == (out + strlen(thread_names[i])))
                return true;
        }
        return false;
    }

    bool Found(int tid) {
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return false;
        return true;
    }

    ~sched_group_data () {
        for (auto entry : mSchedGroupHashSet) {
            delete entry;
        }
        mSchedGroupHashSet.clear();
    }
};

class sched_freq_aggr_group_data {
private:
    typedef android::key_value_pair_t<int,
                         int> sched_freq_aggr_group_key_value;

    struct HashForEntry : private std::unary_function<sched_freq_aggr_group_key_value*, android::hash_t> {
        android::hash_t operator() (const sched_freq_aggr_group_key_value* entry) const {
            return android::hash_type(entry->key);
        };
    };

    struct EqualityForHashedEntries : private std::unary_function<sched_freq_aggr_group_key_value*, bool> {
        bool operator() (const sched_freq_aggr_group_key_value* lhs, const sched_freq_aggr_group_key_value* rhs) const {
            return lhs->key == rhs->key;
        };
    };


    typedef std::unordered_set<sched_freq_aggr_group_key_value*, HashForEntry, EqualityForHashedEntries> mSchedFreqAggrGroupSet;

    typename mSchedFreqAggrGroupSet::iterator findByKey(const int& key) {
        sched_freq_aggr_group_key_value entryForSearch(key, 0);
        typename mSchedFreqAggrGroupSet::iterator result = mSchedFreqAggrGroupHashSet.find(&entryForSearch);
        return result;
    }

    mSchedFreqAggrGroupSet mSchedFreqAggrGroupHashSet;
    bool sched_freq_aggr_present;
public:
    bool Add(int tid, int instanceRefCount) {
        sched_freq_aggr_group_key_value* newEntry = new sched_freq_aggr_group_key_value(tid, instanceRefCount);
        mSchedFreqAggrGroupHashSet.insert(newEntry);
        return true;
    }

    int Get(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return 0;
        sched_freq_aggr_group_key_value *entry = *find_result;
        return entry->value;
    }

    void Remove(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        mSchedFreqAggrGroupHashSet.erase(entry);
        delete(entry);
    }

    void IncrementInsRefCntVal(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        QLOGI("%s, pid=%d instanceRefCount=%d",__FUNCTION__, entry->key, entry->value);
        entry->value++;
    }

    void DecrementInsRefCntVal(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        QLOGI("%s, pid=%d instanceRefCount=%d",__FUNCTION__, entry->key, entry->value);
        if (entry->value > 1)
            entry->value--;
    }

    bool Found(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return false;
        return true;
    }

    ~sched_freq_aggr_group_data () {
        for (auto entry : mSchedFreqAggrGroupHashSet) {
            delete entry;
        }
        mSchedFreqAggrGroupHashSet.clear();
    }

    bool IsSchedFreqAggrPresent() {
        return sched_freq_aggr_present;
    }

    void checkSchedFreqAggrPresence() {
        int rc = -1;
        char sched_freq_aggregate_node_val[NODE_MAX];
        memset(sched_freq_aggregate_node_val, 0, NODE_MAX);
        FREAD_STR(SCHED_FREQ_AGGREGATE_NODE, sched_freq_aggregate_node_val, NODE_MAX, rc);

        if (rc > 0 && atoi(sched_freq_aggregate_node_val)) {
            sched_freq_aggr_present = 1;
            QLOGI("Sched Frequency Aggregation support present");
        } else {
            sched_freq_aggr_present = 0;
            QLOGE("Error: Sched Frequency Aggregation support not present");
        }
    }
};

class OptsData {
private:
    static OptsData mOptsData;

    friend class OptsHandler;
    friend class ResetHandler;

private:
    OptsData();
    OptsData(OptsData const& d);
    OptsData& operator=(OptsData const& d);

private:

    char cpubw_path[NODE_MAX];
    char cpubw_hwmon_hist_memory_path[NODE_MAX];
    char cpubw_hwmon_hyst_length_path[NODE_MAX];
    char cpubw_hwmon_hyst_count_path[NODE_MAX];
    unsigned int  cpubw_avail_freqs[MAX_FREQ_CPUBW];
    int  cpubw_avail_freqs_n;
    char cpubw_hm_s[NODE_MAX];             // cpubw hist memory storage
    int  cpubw_hm_sl;                      // cpubw hist memory string length
    char cpubw_hl_s[NODE_MAX];             // cpubw hyst length storage
    int  cpubw_hl_sl;                      // cpubw hyst length string length
    char cpubw_hc_s[NODE_MAX];             // cpubw hyst count storage
    int  cpubw_hc_sl;                      // cpubw hyst count string length


    unsigned int avail_freqs[MAX_FREQ];
    int avail_freqs_n;

    char llcbw_path[NODE_MAX];
    char llcbw_maxfreq_path[NODE_MAX];
    char llcbw_hwmon_hist_memory_path[NODE_MAX];
    char llcbw_hwmon_hyst_length_path[NODE_MAX];
    char llcbw_hwmon_hyst_count_path[NODE_MAX];
    unsigned int  llcbw_avail_freqs[MAX_FREQ_LLCBW];
    int  llcbw_avail_freqs_n;
    char llcbw_hm_s[NODE_MAX];             // llcbw hist memory storage
    int  llcbw_hm_sl;                      // llcbw hist memory string length
    char llcbw_hl_s[NODE_MAX];             // llcbw hyst length storage
    int  llcbw_hl_sl;                      // llcbw hyst length string length
    char llcbw_hc_s[NODE_MAX];             // llcbw hyst count storage
    int  llcbw_hc_sl;                      // llcbw hyst count string length

    char l3Cluster0_path[NODE_MAX];
    char l3Cluster1_path[NODE_MAX];
    char l3Cluster0_minfreq_path[NODE_MAX];
    char l3Cluster1_minfreq_path[NODE_MAX];
    char l3Cluster0_maxfreq_path[NODE_MAX];
    char l3Cluster1_maxfreq_path[NODE_MAX];
    unsigned int  l3Cluster0_avail_freqs[MAX_FREQ_L3L];
    unsigned int  l3Cluster1_avail_freqs[MAX_FREQ_L3B];
    int  l3Cluster0_avail_freqs_n;
    int  l3Cluster1_avail_freqs_n;
    char l3Cluster0_mf_s[NODE_MAX];             // l3 little min freq storage
    int  l3Cluster0_mf_sl;                      // l3 little min freq string length
    char l3Cluster1_mf_s[NODE_MAX];             // l3 big min freq storage
    int  l3Cluster1_mf_sl;                      // l3 big min min freq string length

    char schedb_n[NODE_MAX];                  // sched boost node string

    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;

    char ksm_run_node[NODE_MAX];
    char ksm_param_sleeptime[NODE_MAX];
    char ksm_param_pages_to_scan[NODE_MAX];
    char ksm_sleep_millisecs[PROPERTY_VALUE_MAX];
    char ksm_pages_to_scan[PROPERTY_VALUE_MAX];
    int is_ksm_supported;

    unsigned int  c0fL[FREQLIST_MAX];                // cpu0 list of available freq
    char c0fL_s[FREQLIST_STR];              // cpu0 list of available freq
    int  c0fL_n;                            // cpu0 #freq
    int  c4fL[FREQLIST_MAX];                // cpu4 list of available freq
    char c4fL_s[FREQLIST_STR];              // cpu4 list of available freq string
    int  c4fL_n;                            // cpu4 #freq

    int kpm_hotplug_support; //1 represent hotplug is supported through KPM, 0 hotplug not supported, -1 not initialized
    int lock_max_clust0;     //store cluster0 value for lock max core perflock, through KPM
    int lock_max_clust1;     //store cluster1 value for lock max core perflock, through KPM

    char core_ctl_min_cpu_node[NODE_MAX];   //Actual path of min_cpu node based on target
    char core_ctl_max_cpu_node[NODE_MAX];   //Actual path of max_cpu node based on target

    int core_ctl_present;    //1 represent core_ctl is present, 0 core ctl not present, -1 not initialized

    char core_ctl_min_cpu[NODE_MAX];        // core control min_cpu storage
    char core_ctl_max_cpu[NODE_MAX];        // core control max_cpu storage

    //char toggle_l2_pc_s[NODE_MAX];    // toggle L2 PC storage
    //int  toggle_l2_pc_sl;             // toggle L2 PC string length

    int min_cores;
    int max_cores;

    /* This array will be used to store max/min frequency
     * for each cpu.
     */
    struct cpu_freq_resource_value cpu_freq_val[MAX_CPUS];

    bool min_freq_prop_0_set;
    bool min_freq_prop_4_set;
    int  min_freq_prop_0_val;
    int  min_freq_prop_4_val;

    //hint data
    hint_associated_data mHintData;

    //irq data
    int  *irq_bal_sp; // irq balancer storage pointer
    unsigned int stored_irq_balancer;
    bool is_active;

    //Common variables for all resources.
    char sysfsnode_path[MAX_MINOR_RESOURCES][NODE_MAX];
    bool is_supported[MAX_MINOR_RESOURCES];
    int node_type[MAX_MINOR_RESOURCES];
    char sysfsnode_storage[MAX_MINOR_RESOURCES][NODE_MAX];
    int sysfsnode_storage_length[MAX_MINOR_RESOURCES];

    //Interactive node storages for both clusters
    int cluster0_interactive_node_storage_length[MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster0_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    int cluster1_interactive_node_storage_length[ MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster1_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    //Storage paths for all cpus of resource SCHED_MOSTLY_IDLE_FREQ_OPCODE
    char sch_cpu_pwr_cost_s[MAX_CPUS][NODE_MAX];
    int  sch_cpu_pwr_cost_sl[MAX_CPUS];

    char sch_load_boost_s[MAX_CPUS][NODE_MAX];
    int  sch_load_boost_sl[MAX_CPUS];
    /* sched group data */
    sched_group_data mSchedGroupData;

    /* sched freq aggr group data */
    sched_freq_aggr_group_data mSchedFreqAggrGroupData;

public:
    int init_available_cpubw_freq();
    unsigned int find_next_cpubw_available_freq(unsigned int freq);
    int init_cpubw_hwmon_path();

    int init_llcbw_hwmon_path();

    void init_node_paths();
    void get_nodes_folder_path(const char *node_path, char *folder_path);
    int init_l3_path();

    int init_devfreq_freqlist(char*);
    int init_devfreq_child_path(const char*, char *);

    int vid_create_timer();

    int init_ksm();
    int toggle_ksm_run(int run);

    void parse_avail_freq_list();
    unsigned int find_next_avail_freq(unsigned int freq);
    int kpm_support_for_hotplug();

    int core_ctl_init();
    int check_core_ctl_presence();

    int setup_cpu_freq_values();

    int get_reset_cpu_freq(int cpu, int ftype);
    int find_next_cpu_frequency(int cpu, unsigned int freq);
    int init_available_freq(int cpu);
    void check_min_freq_prop_set(int cpu);
    int find_frequency_pos(int cpu, int freq);
public:
    ~OptsData();

    int Init();

    static OptsData &getInstance() {
        return mOptsData;
    }

};

#endif /*__OPTS_DATA__H_*/
