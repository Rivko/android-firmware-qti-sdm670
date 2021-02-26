/******************************************************************************
  @file    OptsHandler.h
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_HANDLER__H_
#define __OPTS_HANDLER__H_

#include "ResourceInfo.h"
#include "OptsData.h"

enum req_action_type {
    ADD_NEW_REQUEST = 0,
    ADD_AND_UPDATE_REQUEST = 1,
    PEND_REQUEST = 2
};

typedef int (*ApplyOpts)(Resource &, OptsData &);
typedef int (*ResetOpts)(Resource &, OptsData &);
typedef int (*CompareOpts)(unsigned int, unsigned int);

typedef struct OptsTable {
    ApplyOpts mApplyOpts;
    ResetOpts mResetOpts;
    CompareOpts mCompareOpts;
}OptsTable;

class OptsHandler {
    private:
        OptsTable mOptsTable[MAX_MINOR_RESOURCES];

        //singleton
        static OptsHandler mOptsHandler;

    private:
        OptsHandler();
        OptsHandler(OptsHandler const& oh);
        OptsHandler& operator=(OptsHandler const& oh);

    private:
        static int update_node_param(int opcode_type, const char node[NODE_MAX],
                                        char node_strg[NODE_MAX], int node_strg_l);
    private:
        //all minor resource actions
        static int dummy(Resource &r, OptsData &d);

        /*a common function to handle all the generic perflock calls.*/
        static int modify_sysfsnode(Resource &r, OptsData &d);

        /*set pmQos latency values*/
        static int pmQoS_cpu_dma_latency(Resource &r, OptsData &d);

        /*cpu freq actions*/
        static int cpu_options(Resource &r, OptsData &d);

        /*sched actions*/
        static int sched_boost(Resource &r, OptsData &d);
        static int sched_add_freq_aggr_group(Resource &r, OptsData &d);
        static int sched_reset_freq_aggr_group(Resource &r, OptsData &d);

        /*corectl actions*/
        static int lock_min_cores(Resource &r, OptsData &d);
        static int lock_max_cores(Resource &r, OptsData &d);

        /*cpubw hwmon actions*/
        static int cpubw_hwmon_hyst_opt(Resource &r, OptsData &d);

        /*video hints actions*/
        static int handle_disp_hint(Resource &r, OptsData &d);
        static int handle_vid_decplay_hint(Resource &r, OptsData &d);
        static int handle_vid_encplay_hint(Resource &r, OptsData &d);

        /*ksm actions*/
        static int disable_ksm(Resource &r, OptsData &d);
        static int enable_ksm(Resource &r, OptsData &d);
        static int set_ksm_param(Resource &r, OptsData &d);
        static int reset_ksm_param(Resource &r, OptsData &d);

        /*gpu actions*/
        static int gpu_disable_gpu_nap(Resource &r, OptsData &d);

        /*miscellaneous actions, irq*/
        static int unsupported(Resource &r, OptsData &d);
        static int irq_balancer(Resource &r, OptsData &d);
        static int add_sched_group(Resource &r, OptsData &d);
        static int reset_sched_group(Resource &r, OptsData &d);
        static int keep_alive(Resource &r, OptsData &d);
        static void* keep_alive_thread(void*);

        /*llcbw hwmon actions*/
        static int llcbw_hwmon_hyst_opt(Resource &r, OptsData &d);

        /*memlat (l3, l2) actions*/
        static int l3_min_freq(Resource &r, OptsData &d);

        /*compare functions*/
        static int lower_is_better(unsigned int reqLevel, unsigned int curLevel);
        static int higher_is_better(unsigned int reqLevel, unsigned int curLevel);
        static int always_apply(unsigned int reqLevel, unsigned int curLevel);

        /*Validating function for cluster and core*/
        static int ValidateClusterAndCore(int cluster, int core, int resourceStatus);
        static int CustomizeRequestValue(Resource &r, OptsData &d, char *tmp_s);
        static void CustomizePostNodeUpdation(Resource &r, OptsData &d, int perflock_call,
                                                char node_strg[NODE_MAX], int node_strg_l);
        static int GetNodeStorageLink(Resource &r, OptsData &d, char **node_storage, int **node_storage_length);

        /*miscellaneous utility functions*/
        static int write_sched_freq_aggr_group(int tid, bool enable);
        static char *cpuset_bitmask_to_str(unsigned int cpuset_bitmask);

        static unsigned int MultiplyReqval(unsigned int rval, unsigned int factor);

    public:
        ~OptsHandler();

        int Init();
        void InitializeOptsTable();
        int ApplyOpt(Resource &r);
        int ResetOpt(Resource &r);
        int CompareOpt(int qindx, unsigned int reqVal, unsigned int curVal);

        static OptsHandler &getInstance() {
            return mOptsHandler;
        }
};

#endif /*__OPTS_HANDLER__H_*/
