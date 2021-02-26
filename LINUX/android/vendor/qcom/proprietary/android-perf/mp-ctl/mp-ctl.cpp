/******************************************************************************
  @file    mp-ctl.cpp
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include "mp-ctl.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <poll.h>
#include <private/android_filesystem_config.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include <cutils/sockets.h>

#include <dlfcn.h>

#define LOG_TAG           "ANDR-PERF-MPCTL"
#include <cutils/log.h>
#if defined(ANDROID_JELLYBEAN)
#include "common_log.h"
#endif
#include "performance.h"
#include "thermal_client.h"

#include "Request.h"
#include "ActiveRequestList.h"
#include "ResourceQueues.h"
#include "MpctlUtils.h"
#include "ResourceInfo.h"
#include "EventQueue.h"
#include "client.h"
#include "PerfGlueLayer.h"
#include "FeedbackHandler.h"

#define ON                  1
#define POLL_TIMER_MS       60000
#define TRACE_BUF_SZ        512
#define BILLION             1000000000L
#define SERVER_RESPONSE_TIMEOUT 980000000
#define MAX_REQS_TO_RESTART_COUNTER 2147483648
#define boot_cmplt_prop "sys.post_boot.parsed"
#define MAX_PROC_PATH 15

int internal_perf_lock_acq(int handle, int duration, int list[], int num_args, pid_t client_pid,
                           pid_t client_tid, enum client_t client);
Request * internal_perf_lock_acq_verification(int &handle, int duration, int list[], int num_args,
                                              pid_t client_pid, pid_t client_tid,
                                              enum client_t client = REGULAR_CLIENT);

int internal_perf_lock_rel(int handle);
int internal_perf_lock_acq_apply(int handle, Request *req);
static void *chk_core_online_poll (void *data);

profile_handle * profile_verification(int &handle, int profile, int private_p);
static int apply_profile(profile_handle *phandle, int profile, int handle);

struct cpu_freq_resource {
    int ideal_cpu_id;
    int mapped_cpu;
};

/* The reason this is defined as CPU7_MAX_FREQ+1 is because
 * we want an array where we can use the resource id of CPU
 * freq as the index so that we can find its ideal_cpu_id
 * and mapped_cpu easily.  Therefore we'll need an array
 * of at least 39+1 elements since CPU7_MAX_FREQ == 39.
 */
#define CPU_FREQ_RESOURCES_MAX  CPU7_MAX_FREQ+1
static ActiveRequestList request_list;
static ResourceQueue resource_qs;
static pthread_t mpctl_server_thread;
static mpctl_msg_t msg;
static timer_t poll_timer;
static bool poll_timer_created = false;
static pthread_mutex_t perf_veri_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t subm_req_mutex = PTHREAD_MUTEX_INITIALIZER;
static EventQueue evqueue;
static FeedbackHandler fbh;
static pthread_t chk_core_online_thrd;
extern pthread_mutex_t reset_nodes_mutex;
extern pthread_cond_t core_offline_cond;

#define NODES_POLLED    2
#define IO_PFD_IND  0
#define MODE_PFD_IND	1
#define IO_PROFILE	2

#define SINGLE_MODE		1
#define SINGLE_PROF		3
#define MULTI_MODE		2
#define MULTI_PROF		4
#define PERF_CL_PEAK_MODE	4
#define PERF_CL_PEAK_PROF	6

#define POLLPATH  "/sys/module/msm_performance/events/cpu_hotplug"
#define PROP_NAME "debug.trace.perf"
#define LOAD_LIB "libqti-perfd.so"

static int PERF_SYSTRACE;
static bool sInitComplete = false;

/* This array tells us information about each CPU freq resource such as:
 *     1. which ideal CPU to use
 *     1. which CPU is mapped
 */
static struct cpu_freq_resource cpu_freq_state[CPU_FREQ_RESOURCES_MAX];

#if OEM_PROFILE_RESTORE_ON_FAIL
static void reset_profile_mgr_nodes() {
    int i,cpu;
    Target &t = Target::getCurTarget();
    int clusterNo = t.getNumCluster();
    int startCpu = 0, endCpu;
    for (i = 0; i < clusterNo; i++) {
       endCpu = t.getLastCoreIndex(i);
       if((cpu = get_online_core(startCpu, endCpu))!=FAILED)
           reset_cpu_nodes(cpu);
       startCpu = endCpu + 1;
    }
#if 0
    if ((cpu = get_online_core(0, max_num_cpu_clustr_0)) != FAILED) {
      reset_cpu_nodes(cpu);
    }
    if ((cpu = get_online_core(max_num_cpu_clustr_0, max_num_cpu_clustr_1))
        != FAILED) {
      reset_cpu_nodes(cpu);
    }
#endif
}
#endif

/*
 * return number of available cores
 */
static inline int nocs(){
    static int nocs = 0;
    char b[100];
    int rc;
    if( nocs == 0 ){
        QLOGV("cpu/present file %s", CPUP);
        FREAD_STR(CPUP, b, 100, rc);
        if (rc > 2 && rc < 101) {
            QLOGV("cpu/present %s", b);
            //According to above spec, since we are not going to have
            //cores in the middle "not presented", so it will be just
            //last char +1
            nocs = b[rc - 2] - '0' + 1;
        } else {
            nocs = 1;
        }
    }
    return nocs;
}

/* Enable/disable power collapse on all CPUS for display off */
static int toggle_screen_power_collapse(uint16_t opt_data)
{
    return NOT_SUPPORTED;
}

/* SLVP callback function - gets called every time
 * the timer gets rearmed, only function that
 * acquires and release SLVP hint
 */
void set_slvp_cb(sigval_t tdata) {
   struct itimerspec mInterval_out;
   /* perlock hint parameters
    * 2704 - Sets Above Hispeed Dealy of 40ms
    * 2B5F - Sets Go Hispeed Dealy of 95
    * 2C07 - Sets Hispeed Freq of 768 MHz
    * 2F5A - Sets Target Loads of 90
    * 3204 - Above Hispeed Delay of 40ms for sLVT
    * 365F - Go Hispeed Load of 95 for sLVT
    * 3707 - Hispeed Freq of 729 MHz for sLVT
    * 3A5A - Target Load of 90 for sLVT
    */
   int arg[] = {0x2704, 0x2B5F, 0x2C07, 0x2F5A, 0x3204, 0x365F, 0x3707, 0x3A5A};
   static int vid_handle;
   hint_associated_data *data = NULL;

   if (tdata.sival_ptr) {
       data = (hint_associated_data *) tdata.sival_ptr;
   }

   if (NULL == data) {
       return;
   }

   /* Condition1 - Video is stopped, release handle and cancel timer */
   if (data->vid_hint_state < 1) {
        QLOGI("Video stopped, release hint, timer cancel");

        data->slvp_perflock_set = 0;
        /* release handle */
        perf_lock_rel(vid_handle);

        cancel_slvp_timer(data);
   }
   /* Condition2 - Video is still on but multiple display layers
    * Release SLVP hint, but rearm timer
    */
   else if (data->slvp_perflock_set == 1 && data->disp_single_layer != 1) {
        QLOGI("Disp many layers, release hint, rearm timer");

        data->slvp_perflock_set = 0;
        /* release handle */
        perf_lock_rel(vid_handle);

        rearm_slvp_timer(data);
   }
   /* Condition3 - Single instance of video and
    * Single layer Display Hint sent
    * Acquire Lock and Cancel Timer
    */
   else if (!data->slvp_perflock_set && data->vid_hint_state == 1 && data->disp_single_layer == 1 && data->vid_enc_start < 1) {
        QLOGI("SLVP success, timer cancel");

        data->slvp_perflock_set = 1;
        /* acquire lock */
        vid_handle = perf_lock_acq(0, 0, arg, sizeof(arg)/sizeof(arg[0]));

        cancel_slvp_timer(data);
   }
   /* Condition4 - Video encode and decode both
    * are started, so WFD use case, release handle
    * and cancel timer.
    */
   else if (data->slvp_perflock_set == 1 && data->vid_enc_start == 1) {
        QLOGI("Video encode and decode - WFD");

        data->slvp_perflock_set = 0;
        /* release handle */
        perf_lock_rel(vid_handle);

        cancel_slvp_timer(data);
   }
   /* For every other condition, rearm timer */
   else {
        rearm_slvp_timer(data);
    }

    return;
}

/* Set up the ideal CPU ids associated with each CPU freq resource
 * In turn set up the cpu freq resource mappings for each CPU
 */
void setup_cpu_freq_mappings() {
    int i;
    for (i = 0; i < CPU_FREQ_RESOURCES_MAX; i++){
        cpu_freq_state[i].ideal_cpu_id = -1;
    }
    cpu_freq_state[CPU0_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU0_MAX_FREQ].ideal_cpu_id = 0;
    cpu_freq_state[CPU1_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU1_MAX_FREQ].ideal_cpu_id = 1;
    cpu_freq_state[CPU2_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU2_MAX_FREQ].ideal_cpu_id = 2;
    cpu_freq_state[CPU3_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU3_MAX_FREQ].ideal_cpu_id = 3;
    cpu_freq_state[CPU4_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU4_MAX_FREQ].ideal_cpu_id = 4;
    cpu_freq_state[CPU5_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU5_MAX_FREQ].ideal_cpu_id = 5;
    cpu_freq_state[CPU6_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU6_MAX_FREQ].ideal_cpu_id = 6;
    cpu_freq_state[CPU7_MIN_FREQ].ideal_cpu_id = cpu_freq_state[CPU7_MAX_FREQ].ideal_cpu_id = 7;
}

/* Allow all requestes of the reqtype until len
 * to be mapped to a single core specified with dstcpu
 */
void static_map_core(int reqtype, int len, int dstcpu) {
   int i = reqtype;
   for ( ; i <= reqtype+len; i++) {
       cpu_freq_state[i].ideal_cpu_id = dstcpu;
   }
}
/*=========================================================================================================================*/
/* PerfLock related functions                                                                                              */
/*=========================================================================================================================*/
void reset()
{
    QLOGI("Initiating PerfLock reset");
    OptsData &d = OptsData::getInstance();
    ResetHandler::getInstance().reset_to_default_values(d);
    request_list.Reset();
    resource_qs.Reset();
    QLOGI("PerfLock reset completed");
}

static void run_ksm_aggressive()
{
    int params[] = {0};
    params[0]=0x1D00;
    //Run KSM aggreddivly for 120 Sec
    internal_perf_lock_acq(0,120*1000,params, sizeof(params)/sizeof(params[0]), getpid(), gettid(), REGULAR_CLIENT);
}

struct file {
    char *name;
    char *value;
    char o[NODE_MAX];
    size_t len;
};

struct therm {
    char *desc;
    char *field;
    int mask;
    unsigned int type;
    void *data;
    int num_data;
};

struct profile {
    int *params;
    int num_params;
    struct file *files;
    int num_files;
    int *irq_affinity;
    int *old_irq_affinity;
    struct therm *therms;
    unsigned int num_therms;
    struct config_instance *cur_therms;
    int num_cur_therms;
    struct config_instance *old_therms;
    int num_old_therms;
    int handle;
    unsigned int priority;
    enum client_t client;
};

static struct profile *profiles;
static int num_profiles;
static int current_profile = -1;
int ncpus = 0;

static int compare_files(const void *_a, const void *_b)
{
    const struct file *a = (struct file *) _a, *b = (struct file *) _b;
    return strcmp(a->name, b->name);
}

static int read_write_avl_node(char *node, char *prefix, char *val, int len,
                              int cpunode, int do_write)
{
    int rc = FAILED;
    int startcpu,endcpu;
    Target &t = Target::getCurTarget();
    if(cpunode) {
        char *mod = node + strlen(prefix);
        const char substr[] = "core_ctl";
        int cpu, oldcpu;
        sscanf(mod, "%d", &cpu);
        oldcpu = cpu;
#if 0
        cpu = cpu < max_num_cpu_clustr_0 ? 0 : max_num_cpu_clustr_0;
        cpu = get_online_core(cpu, (cpu < max_num_cpu_clustr_0 ?
                              max_num_cpu_clustr_0 : max_num_cpu_clustr_1));
#endif
        if (strstr(node, substr) == NULL) {
            t.getClusterForCpu(cpu, startcpu, endcpu);
            cpu = get_online_core(startcpu, endcpu);
        }

        if (cpu != oldcpu && cpu != FAILED) {
            mod[0] = cpu + '0';
            if (do_write) {
                QLOGI("oldcpu=%d, online cpu=%d, writing node=%s, val=%s",
                       oldcpu, cpu, node, val);
                FWRITE_STR(node, val, len, rc);
            } else {
                FREAD_STR(node, val, len, rc);
                QLOGI("oldcpu=%d, online cpu=%d, read node=%s, val=%s",
                       oldcpu, cpu, node, val);
            }
            mod[0] = oldcpu + '0';
            return rc;
        }
    }
    if (do_write) {
        QLOGI("writing node=%s, val=%s", node, val);
        FWRITE_STR(node, val, len, rc);
    } else {
        FREAD_STR(node, val, len, rc);
        QLOGI("read node=%s, val=%s", node, val);
    }
    return rc;
}

static void update_files(struct profile *newprofile, struct profile *old)
{
    int i, ret, cpunode;
    struct file *f, *fo;
    const char prefix[] = "/sys/devices/system/cpu/cpu";

    for (i = 0, f = &newprofile->files[i]; i < newprofile->num_files; i++, f++) {
        /* Copy over old defaults if they exist */
        if (old) {
            fo = (struct file *) bsearch(f, old->files, old->num_files,
                        sizeof(old->files[0]), compare_files);
        } else {
            fo = NULL;
        }

        cpunode = (strncmp(f->name, prefix, strlen(prefix)) == 0) ? 1: 0;
        if (fo && fo->len) {
            strlcpy(f->o, fo->o, sizeof(f->o));
            f->len = fo->len;
            fo->len = 0;
            QLOGI("binary search success, stored old val=%s for node=%s",
                   fo->o, f->name);
        } else {
            /* cpu governor's boostpulse node cannot be read. It can be only
             * written with values. Don't trigger resetting governor params for
             * read failure in boostpulse sysfs nodes. */
            const char postfix[] = "/cpufreq/interactive/boostpulse";
            char * mod = NULL;
            mod = strstr(f->name, postfix);
            unsigned int postfix_len = (mod == NULL) ? 0: strlen(mod);
            if (strlen(postfix) != postfix_len) {
                ret = read_write_avl_node(f->name, (char *)prefix, f->o, sizeof(f->o),
                                          cpunode, 0);
                if (ret <= 0 && cpunode) {
                    QLOGE("update_files: cannot read node %s, rc = %d", f->name,
                          ret);
                    /*we failed to read cpu node, bail out of the loop*/
                    goto failure;
                } else if (ret > 0) {
                    f->len = ret;
                    QLOGI("binary search failure, stored old val=%s for node=%s",
                           f->o, f->name);
                } else {
                    /* There may be some sysfs node which can be only written.
                     * Values for those nodes cannot be resoted in any way */
                    QLOGI("binary search failure, read failure, not storing any restore val for node=%s",
                          f->name);
                }
            } else {
                QLOGI("skipped storing any backup value for boostpulse node");
            }
        }
        ret = read_write_avl_node(f->name, (char *)prefix, f->value, strlen(f->value),
                                  cpunode, 1);
        if (ret <= 0 && cpunode) {
           QLOGE("update_files: Prof write rc =-1 write failed");
           /*we failed to write node, bail out of the loop*/
           goto failure;
        }

    }

return;

failure:
    signal_chk_poll_thread(f->name, ret);
}

static void drop_old_files(struct profile *old)
{
    int i, ret, cpunode;
    struct file *fo;
    const char prefix[] = "/sys/devices/system/cpu/cpu";
    for (i = 0, fo = &old->files[i]; i < old->num_files; i++, fo++) {
        if (fo->len) {
            cpunode = (strncmp(fo->name, prefix, strlen(prefix)) == 0) ? 1: 0;
            ret = read_write_avl_node(fo->name, (char *)prefix, fo->o, fo->len, cpunode,
                                     1);
            fo->len = 0;
            if (ret <= 0 && cpunode) {
               QLOGE("drop_old_files: Prof write rc =%d write failed", ret);
               signal_chk_poll_thread(fo->name, ret);
               /* we failed to write node, bail out of the loop */
               break;
            }
            QLOGI("Restored profile handle=%d, node=%s, val=%s",
                   old->handle, fo->name, fo->o);
        } else {
            QLOGI("Restore failed for profile handle=%d, node=%s",
                   old->handle, fo->name);
        }
    }
}

static void change_affinity(struct profile *newprofile, struct profile *old)
{
    if (!newprofile->irq_affinity)
        return;

    /* Make a buffer to save away current affinity */
    if (!old || !old->irq_affinity)
        newprofile->old_irq_affinity = (int *) malloc(sizeof(*newprofile->old_irq_affinity) * ncpus);

    /* new->old_irq_affinity is NULL if we're not saving current affinity */
    send_irq_balance(newprofile->irq_affinity, &newprofile->old_irq_affinity);

    /* Copy over old affinity if it exists */
    if (old && old->irq_affinity) {
        newprofile->old_irq_affinity = old->old_irq_affinity;
        old->old_irq_affinity = NULL;
    }
}

static void drop_old_affinity(struct profile *old)
{
    if (!old->old_irq_affinity)
        return;

    send_irq_balance(old->old_irq_affinity, NULL);
    free(old->old_irq_affinity);
    old->old_irq_affinity = NULL;
}

static void
cpy_data(struct field_data *dst, unsigned int type, const void *src, int num)
{
    int i;
    unsigned int j;
    const struct action_info_data *src_info;
    struct action_info_data *dst_info;

    switch (type) {
    case 0:
    case 2:
        memcpy(dst->data, src, sizeof(int) * num);
        break;
    case 1:
        strlcpy((char *)dst->data, (const char *)src, 16); /* 16 matches thermal-engine sources */
        break;
    case 4:
        src_info = (const struct action_info_data *) src;
        dst_info = (struct action_info_data *) dst->data;
        for (i = 0; i < num; i++)
            for (j = 0; j < src_info[i].num_actions; j++)
                dst_info[i].info[j] = src_info[i].info[j];
        break;
    case 3:
        QLOGE("Don't know what to do!");
        break;
    }
}

static void write_config_values(struct config_instance *curc,
                        struct config_instance *oldc, unsigned int num,
                        struct profile *p, int restoring)
{
    unsigned int i, j, k, found;
    const struct therm *t;
    struct config_instance *c, *n;
    struct field_data *f, *g;

    /* Bail out early */
    if (!curc || !oldc)
        return;

    for (i = 0, t = p->therms; i < p->num_therms; i++, t++) {
        found = 0;

        for (j = 0, c = oldc, n = curc; j < num; j++, c++, n++) {
            if (strcmp(c->cfg_desc, t->desc))
                continue;

            for (k = 0, f = c->fields, g = n->fields; k < c->num_fields; k++, f++, g++) {
                if (strcmp(f->field_name, t->field) || f->data_type != t->type)
                    continue;

                /* Whewh! We finally found it! */
                found = 1;
                if (restoring) {
                    /* Copy original config into current config */
                    cpy_data(g, f->data_type, f->data, f->num_data);
                } else {
                    /* Copy new config into current config */
                    cpy_data(g, t->type, t->data, t->num_data);
                }
                n->fields_mask |= t->mask;
                c->fields_mask |= t->mask;
                break;
            }
            break;
        }

        if (!found)
            QLOGE("Couldn't find config %s %s to change!", t->desc, t->field);
    }
}

static void change_thermal(struct profile *newprofile, struct profile *old)
{
    int numo, numc;
    struct config_instance *oldc=NULL, *curc=NULL, *pc=NULL;
    int ret;
    int configs_to_change_count = 0;
    int config_count = 0;
    struct config_instance *req = NULL;
    int sizeof_config_instance = sizeof(struct config_instance);

    if (!newprofile->therms)
        return;

    if (!old || !old->old_therms) {
        /* Save away current values */
        numo = thermal_client_config_query((char *)"all", &oldc);
        /* Get a copy to change */
        numc = thermal_client_config_query((char *)"all", &curc);
    } else {
        oldc = old->old_therms;
        numo = old->num_old_therms;
        old->old_therms = NULL;
        curc = old->cur_therms;
        numc = old->num_cur_therms;
        old->cur_therms = NULL;
    }

    /* For every config in old profile, restore values from oldc to curc */
    if (old)
        write_config_values(curc, oldc, numo, old, 1);

    /* For every config in new profile, set values in curc */
    write_config_values(curc, oldc, numc, newprofile, 0);

    /*
     * This is sad. Thermal will reject unchanged configurations even if
     * we send more than one over. Check for unchanged here instead and
     * send one at a time.
     */
    for (pc = curc; pc && pc < curc + numc; pc++) {
        if (pc->fields_mask) {
            configs_to_change_count++;
        }
    }
    if (configs_to_change_count > 0) {
        req = (struct config_instance *) malloc(sizeof_config_instance * configs_to_change_count);
    }
    if (NULL == req) {
        QLOGE("Unable to create thermal configs to change.");
        return;
    }
    for (pc = curc; pc && pc < curc + numc; pc++) {
        if (pc->fields_mask) {
            memcpy((req+config_count), pc, sizeof_config_instance);
            config_count++;
        }
        pc->fields_mask = 0;
    }
    ret = thermal_client_config_set(req, config_count);
    if (ret <= 0) {
        QLOGE("Request to change thermal config failed.");
    } else {
        QLOGI("Num of thermal configs required: %d, num of thermal configs sent: %d", config_count, ret);
    }
    if (req) {
        free(req);
    }

    newprofile->old_therms = oldc;
    newprofile->num_old_therms = numo;
    newprofile->cur_therms = curc;
    newprofile->num_cur_therms = numc;
}

static void drop_old_thermal(struct profile *old)
{
    int i;
    int ret;
    struct config_instance *pc;
    int configs_to_change_count = 0;
    int config_count = 0;
    struct config_instance *req = NULL;
    int sizeof_config_instance = sizeof(struct config_instance);

    if (old->old_therms) {
        for (i = 0, pc = old->old_therms; i < old->num_old_therms; i++, pc++) {
            if (pc->fields_mask) {
                configs_to_change_count++;
            }
        }
        if (configs_to_change_count > 0) {
            req = (struct config_instance *) malloc(sizeof_config_instance * configs_to_change_count);
        }
        if (NULL == req) {
            QLOGE("Unable to create thermal configs to reset.");
            return;
        }
        for (i = 0, pc = old->old_therms; i < old->num_old_therms; i++, pc++) {
            if (pc->fields_mask) {
                memcpy((req+config_count), pc, sizeof_config_instance);
                config_count++;
            }
        }
        ret = thermal_client_config_set(req, config_count);
        if (ret <= 0) {
            QLOGE("Request to reset thermal config failed.");
        } else {
            QLOGI("Num of thermal configs reset required: %d, num of thermal configs reset sent: %d", config_count, ret);
        }
        thermal_client_config_cleanup(old->old_therms, old->num_old_therms);
        old->old_therms = NULL;
        if (req) {
            free(req);
        }
    }

    if (old->cur_therms) {
        thermal_client_config_cleanup(old->cur_therms, old->num_cur_therms);
        old->cur_therms = NULL;
    }
}

static struct profile_handle *profile_queue;


static int get_cur_prof_handle()
{
   struct profile_handle* hp = profile_queue;
   int handle = 0;
   while(hp != NULL) {

      if (hp->profile == current_profile) {
          handle = hp->handle;
          QLOGE("Handle for the current_profile=%d , hp->profile = %d is %d",
                 current_profile, hp->profile, hp->handle);
          break;
      }
      hp = hp->next;
   }
   return handle;

}

static int switch_profile(int handle, int profile, int private_p) {
    profile_handle *pHandle = NULL;

    pHandle = profile_verification(handle, profile, private_p);
    if (pHandle != NULL ) {
       apply_profile(pHandle, profile, handle);
    }
    return handle;
}

profile_handle * profile_verification(int &handle, int profile, int private_p) {
    int ret, i;
    struct profile_handle *phandle = NULL, **hp = NULL, *hc = NULL;
    static int phandle_counter = 0;



    /* Check for invalid profile: an invalid profile number from user space will
     * force to dereference invalid profile details from global array which is
     * dangerous. So avoid that here. */
    if (profile >= num_profiles || profile < -1) {
        QLOGE("Invalid profile no. %d, total profiles %d only", profile, num_profiles);
        return NULL;
    }

    /* Profile 0 is reserved for perflock */
    if (profile == 0 && private_p != 1) {
        QLOGE("Use another profile, profile 0 is reserved");
        return NULL;
    }

    /* Make a new handle if there is none */
    if (!handle) {
        phandle = (struct profile_handle *) calloc(1, sizeof(struct profile_handle));
        if (!phandle)
            return NULL;

        /* Assume not too many handles will exist at a time */
        handle = ++phandle_counter;
        if (handle == 2000000000)
            phandle_counter = 1;

        if (phandle) {
            phandle->handle = handle;
        }
    } else {
        /* Find the handle and unlink it */
        phandle = profile_queue;
        hp = &profile_queue;
        while (phandle) {
            if (phandle->handle == handle) {
                *hp = phandle->next;
                break;
            }

            hp = &phandle->next;
            phandle = phandle->next;
        }
        if (!phandle)
            return NULL;
    }

    return phandle;
}

static int apply_profile(profile_handle *phandle, int profile, int handle) {
    struct profile_handle **hp = NULL, *hc = NULL;
    struct profile *newprofile = NULL, *old = NULL;
    int skip = 0;

    if (profile == -1) {
        if (phandle) {
            profile = profile_queue ? profile_queue->profile : -1;
            free(phandle);
            phandle = NULL;
        }
    } else {
        /* Link phandle into list in sorted order */
        if ((NULL == phandle) || (NULL == profiles) ||
            (profile > num_profiles)) {
            /* Free phandle if the other conditions are true */
            if (phandle) {
                free(phandle);
                phandle = NULL;
            }
            return FAILED;
        }
        phandle->profile = profile;
        phandle->priority = profiles[profile].priority;

        hc = profile_queue;
        hp = &profile_queue;
        while (hc) {
            if (hc->priority < phandle->priority)
                break;

            hp = &hc->next;
            hc = hc->next;
        }

        phandle->next = hc;
        *hp = phandle;
        profile = profile_queue ? profile_queue->profile : -1;
    }

    if ((NULL == profiles) || (profile > num_profiles)) {
        if (phandle) {
            free(phandle);
            phandle = NULL;
        }
        return FAILED;
    }

    /* Nothing to do */
    if (profile == current_profile)
        return handle;

    /* A profile < 0 clears the profile without switching to a new one */
    newprofile = profile >= 0 ? &profiles[profile] : NULL;
    old = current_profile >= 0 ? &profiles[current_profile] : NULL;
    QLOGI("%s: profile=%d, current_profile=%d", __FUNCTION__, profile, current_profile);

    if (newprofile && (newprofile->client == HIGH_PRIORITY_PROFILE_CLIENT) &&
        old && (old->client == HIGH_PRIORITY_PROFILE_CLIENT)) {
        skip = 1;
        internal_perf_lock_rel(old->handle);
        newprofile->handle = internal_perf_lock_acq(0, 0, newprofile->params, newprofile->num_params, getpid(), gettid(), newprofile->client);
    }

    if (newprofile) {
        if (!skip && (newprofile->num_params || newprofile->client == HIGH_PRIORITY_PROFILE_CLIENT))
            newprofile->handle = internal_perf_lock_acq(0, 0, newprofile->params, newprofile->num_params, getpid(), gettid(), newprofile->client);
        update_files(newprofile, old);
        change_affinity(newprofile, old);
        change_thermal(newprofile, old);
    }

    if (old) {
        if (!skip && (old->num_params || old->client == HIGH_PRIORITY_PROFILE_CLIENT))
            internal_perf_lock_rel(old->handle);
        drop_old_files(old);
        drop_old_affinity(old);
        drop_old_thermal(old);
    }

    QLOGI("Switched profile to %d\n", profile);

    current_profile = profile;

    return handle;
}

enum config_sections {
    SECTION_LOCK,
    SECTION_FILE,
    SECTION_AFF,
    SECTION_THERM,
    SECTION_PRIORITY,
    SECTION_MAX
};

static int load_profiles(void)
{
    struct profile *p;
    FILE *fp;
    char *line = NULL, *linep, *found_nl, *parsed, *p2, *v;
    char *savep1, *savep2;
    char name[45];
    size_t len = 0, count = 0;
    ssize_t read;
    int param, num = 0, found, i, j;
    char *value, *file;
    enum config_sections section;
    struct therm *t;
    struct action_info_data *aid;
    unsigned int perflock_priority = 0;

    ncpus = sysconf(_SC_NPROCESSORS_CONF);
    if (ncpus < 0 || ncpus > 4096)
        ncpus = 0;

    for (;;) {
        section = SECTION_MAX;
        snprintf(name, sizeof(name), VENDOR_DIR "/perf/perf-profile%u.conf", num);

        fp = fopen(name, "r");
        if (!fp)
            break;

        num_profiles++;

        profiles = (struct profile *) realloc(profiles, num_profiles * sizeof(*profiles));
        if (!profiles)
            goto err_profile;

        p = &profiles[num];
        memset(p, 0, sizeof(*p));
        p->client = PROFILE_CLIENT;

        while ((read = getline(&line, &len, fp)) != -1) {
            if (!strcmp(line, "\n")) {
                continue;
            } else if (!strncmp(line, "[locks]", 7)) {
                section = SECTION_LOCK;
                continue;
            } else if (!strncmp(line, "[files]", 7)) {
                section = SECTION_FILE;
                continue;
            } else if (!strncmp(line, "[irq_affinity]", 14)) {
                section = SECTION_AFF;
                continue;
            } else if (!strncmp(line, "[thermal]", 9)) {
                section = SECTION_THERM;
                continue;
            } else if (!strncmp(line, "[priority]", 10)) {
                section = SECTION_PRIORITY;
                continue;
            }

            switch (section) {
            case SECTION_PRIORITY:
                p->priority = strtoul(line, NULL, 0);
                /* Record the perflock priority */
                if (num == 0)
                        perflock_priority = p->priority;
                /* Mark as high priority if this profile trumps perflock reqs */
                if (p->priority > perflock_priority)
                    p->client = HIGH_PRIORITY_PROFILE_CLIENT;
                break;
            case SECTION_LOCK:
                p->num_params++;
                p->params = (int *) realloc(p->params, p->num_params * sizeof(*p->params));
                if (!p->params)
                    goto err;

                /* format of <key> << 8 | <value> as one number */
                param = strtol(line, NULL, 0);
                p->params[p->num_params - 1] = param;
                break;
            case SECTION_FILE:
                p->num_files++;
                p->files = (struct file *) realloc(p->files, p->num_files * sizeof(*p->files));
                if (!p->files)
                    goto err;

                /* format of <value> <file> */
                found = strrchr(line, ' ') - line;

                value = strndup(line, found);
                file = strdup(line + found + 1);
                found_nl = strrchr(file, '\n');
                if (found_nl)
                    *found_nl = '\0';
                p->files[p->num_files - 1].value = value;
                p->files[p->num_files - 1].name = file;
                memset(&p->files[p->num_files - 1].o, 0, sizeof(p->files[0].o));
                p->files[p->num_files - 1].len = 0;
                break;
            case SECTION_AFF:
                p->irq_affinity = (int *) malloc(sizeof(p->irq_affinity[0]) * ncpus);
                if (!p->irq_affinity)
                    goto err;
                linep = line;
                /* One value for each CPU split by spaces "<v> <v> <v>..." */
                for (i = 0; i < ncpus; i++) {
                    parsed = strsep(&linep, " ");
                    if (!parsed)
                        goto err;
                    p->irq_affinity[i] = atoi(parsed);
                }
                break;
            case SECTION_THERM:
                /* <cfg_desc> <field_name> <field_mask> <field_data_type> <value> */
                p->num_therms++;
                p->therms = (therm *) realloc(p->therms, p->num_therms * sizeof(*p->therms));
                if (!p->therms)
                    goto err;
                linep = line;
                t = &p->therms[p->num_therms - 1];

                parsed = strtok_r(linep, " ", &savep1);
                if (!parsed)
                    goto err;
                t->desc = strdup(parsed);
                parsed = strtok_r(NULL, " ", &savep1);
                if (!parsed)
                    goto err;
                t->field = strdup(parsed);
                parsed = strtok_r(NULL, " ", &savep1);
                if (!parsed)
                    goto err;
                t->mask = strtol(parsed, NULL, 0);
                parsed = strtok_r(NULL, " ", &savep1);
                if (!parsed)
                    goto err;
                t->type = strtoul(parsed, NULL, 0);
                t->data = NULL;

                switch (t->type) {
                case 0: /* Int (FIELD_INT) */
                case 2: /* Int array (FIELD_INT_ARR) */
                    for (i = 0; ; i++) {
                        parsed = strtok_r(NULL, " ", &savep1);
                        if (!parsed || (i && t->type == 0))
                            break;
                        t->data = realloc(t->data, (i + 1) * sizeof(int));
                        if (!t->data)
                            goto err;
                        ((int *)t->data)[i] = atoi(parsed);
                    }
                    t->num_data = i;
                    break;
                case 1: /* String (FIELD_STR) */
                case 3: /* String array (FIELD_ARR_STR) */
                    for (i = 0; ; i++) {
                        parsed = strtok_r(NULL, " ", &savep1);
                        if (!parsed || (i && t->type == 1))
                            break;
                        found_nl = strrchr(parsed, '\n');
                        if (found_nl)
                            *found_nl = '\0';
                        t->data = realloc(t->data, (i + 1) * sizeof(char *));
                        if (!t->data)
                            goto err;
                        ((char **)t->data)[i] = strdup(parsed);
                    }
                    t->num_data = i;
                    break;
                case 4: /* Int Matrix (FIELD_ARR_INT_ARR) */
                    for (i = 0; ; i++) {
                        parsed = strtok_r(NULL, " ", &savep1);
                        if (!parsed)
                            break;
                        t->data = realloc(t->data, (i + 1) * sizeof(struct action_info_data));
                        if (!t->data)
                            goto err;
                        aid = (struct action_info_data *) t->data;
                        memset(&aid[i], 0, sizeof(aid[i]));
                        for (p2 = parsed, j = 0; ; p2 = NULL, j++) {
                            v = strtok_r(p2, "+", &savep2);
                            if (!v)
                                break;
                            aid[i].info[j] = atoi(v);
                        }
                        aid[i].num_actions = j;
                    }
                    t->num_data = i;
                }
                break;
            default:
                break;
            }
        }
        if (NULL != p->files)
            qsort(p->files, p->num_files, sizeof(p->files[0]), compare_files);
        free(line);
        line = NULL;

        fclose(fp);
        num++;
    }

    QLOGI("Loaded %d profiles\n", num_profiles);

    return 0;

err:
    free(line);
err_profile:
    fclose(fp);
    return -1;

}

static void arm_poll_timer(int arm)
{
    int rc = -1;
    int uTime = 0;

    if (arm) {

       uTime = POLL_TIMER_MS;
    }

    struct itimerspec mTimeSpec;
    mTimeSpec.it_value.tv_sec = uTime / TIME_MSEC_IN_SEC;
    mTimeSpec.it_value.tv_nsec = (uTime % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;

    if (poll_timer_created) {
        rc = timer_settime(poll_timer, 0, &mTimeSpec, NULL);
        if (rc != 0) {
            QLOGE("Failed to arm poll timer");
        }
    }
}

/* Timer callback function for polling clients */
static void poll_cb(sigval_t)
{
    perf_lock_cmd(MPCTL_CMD_PERFLOCKPOLL);
    arm_poll_timer(1);
}

/* Poll to see if clients of PerfLock are still alive
 * If client is no longer active, release its lock
 */
static void poll_client_status() {
    int size = 0, rc = 0;
    int handles[ActiveRequestList::ACTIVE_REQS_MAX];
    int pids[ActiveRequestList::ACTIVE_REQS_MAX];
    char buf[MAX_PROC_PATH];

    size = request_list.GetActiveReqsInfo(handles, pids);
    QLOGI("Total ACTIVE_REGS: %d", size);

    for (int i = 0; i < size; i++) {
        QLOGI("ACTIVE_REQS process pid: %d", pids[i]);
        snprintf(buf, MAX_PROC_PATH , "proc/%d", pids[i]);
        rc = access(buf, F_OK);
        //access() returns 0 on success(path exists) and -1 on failure(path does not exists)
        if (rc == -1) {
            QLOGI("Client %d does not exist, remove lock (handle=%d) associated with client",
                  pids[i], handles[i]);
            perf_lock_rel(handles[i]);
        }
    }
}

static int lockcount;
static int profilehandle;

int internal_perf_lock_acq(int handle, int duration, int list[], int num_args,
                           pid_t client_pid, pid_t client_tid, enum client_t client) {
    Request *req = NULL;

    req = internal_perf_lock_acq_verification(handle, duration, list, num_args,
                                              client_pid, client_tid, client);
    if (req != NULL ) {
        internal_perf_lock_acq_apply(handle, req);
    }
    return handle;
}


Request* internal_perf_lock_acq_verification(int &handle, int duration, int list[], int num_args,
                                              pid_t client_pid, pid_t client_tid, enum client_t client) {
    char trace_buf[TRACE_BUF_SZ];
    char trace_arg_buf[TRACE_BUF_SZ];
    char trace_args_buf[TRACE_BUF_SZ] = "list=";
    int arg = 0, tmpHandle = -1;
    static int handle_counter = 0;
    int rc = 0, i = 0, resource = 0, value = 0;
    int status = 0, timer_rc= -1;
    Request *req = NULL, *exists = NULL;
    struct q_node *current_resource = NULL, *pended = NULL;
    struct q_node *recent =NULL, *iter = NULL;
    bool ret = false;

    //args sanity
    //1.num_args specified but no list
    //2.num_args not specified but is not a high profile client
    if ((num_args && !list) ||
        ((num_args <= 0) && (client != HIGH_PRIORITY_PROFILE_CLIENT))) {
        QLOGE("Num args is not proper, No optimizations performed");
        return NULL;
    }

    //1.check if active request exceeded limit
    if (request_list.GetNumActiveRequests() >= ActiveRequestList::ACTIVE_REQS_MAX) {
        QLOGE("Active req limit reached, No optimizations performed");
        request_list.Dump();
        return NULL;
    }

    for (arg = 0; arg < num_args; arg++) {
        snprintf(trace_arg_buf, TRACE_BUF_SZ, "0x%0X ", list[arg]);
        strlcat(trace_args_buf, trace_arg_buf, TRACE_BUF_SZ);
        /* MPCTLV3_SCHED_FREQ_AGGR_GROUP will specify client pid specifically
         * as argument and changing client pid in request helps to enable
         * crash recovery for that particuler pid which is given along with
         * opcode as argument*/
        if (list[arg] == MPCTLV3_SCHED_FREQ_AGGR_GROUP && list[arg+1] != 1)
            client_tid = client_pid = list[arg+1];
    }

     /* Enable this log by adding debug.trace.perf=1 into build.prop */
    if (perf_debug_output) {
        snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_acq: client_pid=%d, client_tid=%d, inupt handle=%d, duration=%d ms, num_args=%d, %s",\
             client_pid, client_tid, handle, duration, num_args, trace_args_buf);
        QLOGE("%s", trace_buf);
    }

    if (PERF_SYSTRACE) {
        ATRACE_BEGIN(trace_buf);
    }
    //2.create a new request
    req = new Request(duration, client_pid, client_tid, client);
    if (NULL == req) {
        QLOGE("Failed to create new request, no optimizations performed");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return NULL;
    }

    //3.process and validate the request
    if (!req->Process(num_args, list)) {
        delete req;
        QLOGE("invalid request, no optimizations performed");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return NULL;
    }

    //4.avoid duplicate requests
    exists = request_list.IsActive(handle, req);
    if (exists != NULL) {
        QLOGI("Request exists reset the timer");
        /* if the request exists and is not of an indefinite_duration
           simply reset its timer and return handle to client */
        if (exists->GetDuration() != INDEFINITE_DURATION) {
            exists->SetTimer();
        }
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }

        //As this request already exist in list, remove it
        delete req;
        return NULL;
    }

    pthread_mutex_lock(&perf_veri_mutex);
    handle_counter = (handle_counter + 1) % MAX_REQS_TO_RESTART_COUNTER;
    if (handle_counter == 0)
        handle_counter++;
    tmpHandle = handle_counter;
    pthread_mutex_unlock(&perf_veri_mutex);

    //5. Create_timer and init the duration
    if (duration != INDEFINITE_DURATION) {
        timer_rc = req->CreateTimer(tmpHandle);
        QLOGI("timer_rc for create_timer %d", timer_rc);
        if (timer_rc != 0) {
            delete req;
            QLOGE("Failed to create timer, no optimizations performed");
            if (PERF_SYSTRACE) {
                ATRACE_END();
            }
            return NULL;
        }
    }

    snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_acq: output handle=%d", tmpHandle);
    QLOGI("%s", trace_buf);

    if (PERF_SYSTRACE) {
        ATRACE_BEGIN(trace_buf);
        ATRACE_END();
    }
    handle = tmpHandle;
    if (PERF_SYSTRACE) {
        ATRACE_END();
    }
    return req;
}

//Main function used when PerfLock acquire is requested
int internal_perf_lock_acq_apply(int handle, Request *req) {

    char trace_buf[TRACE_BUF_SZ] = {0};

    snprintf(trace_buf, TRACE_BUF_SZ, "internal_perf_lock_acq_apply: handle=%d", handle);
    if (PERF_SYSTRACE) {
        QLOGI("%s", trace_buf);
        ATRACE_BEGIN(trace_buf);
    }

    // Force anything but the highest priority profile off
    if (req->GetClient() == REGULAR_CLIENT && lockcount++ == 0)
        profilehandle = switch_profile(0, 0, 1);

    //handle display off case or a high priority profile client
    if (req->GetPriority() == HIGH_PRIORITY) {
        resource_qs.PendCurrentRequests();
    }

    //apply operation
    //even if we fail to apply all resource locks we still add this request
    //to our db as we had sent the handle to client, once client asks for
    //release, we delete from our db, no need to check for failure of the request
    //at this point of time
    resource_qs.AddAndApply(req);

    arm_poll_timer(1);

    // handle display off case
    if (req->GetPriority() == HIGH_PRIORITY) {
        resource_qs.LockCurrentState(req);
        arm_poll_timer(0);
    }

    // add this request to our database
    request_list.Add(req, handle);

    if (req->GetDuration() != INDEFINITE_DURATION) {
        req->SetTimer();
    } else {
        QLOGI("Lock with Indfeinite_duration %d", req->GetDuration());
    }

    if (PERF_SYSTRACE) {
        ATRACE_END();
    }

    return handle;
}

/* Main function used when PerfLock release is requested */
int internal_perf_lock_rel(int handle) {
    char trace_buf[TRACE_BUF_SZ];
    int rc = 0;
    int resource = 0;
    int reset_opt_data = 0;
    Request *req = NULL;
    int i;
    ResourceInfo r;
    int level=0;
    int opcde;

    struct q_node *del = NULL;
    struct q_node *pending;
    struct q_node *current_resource;

    snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_rel: input handle=%d", handle);
    QLOGI("%s", trace_buf);
    if (PERF_SYSTRACE) {
        ATRACE_BEGIN(trace_buf);
    }

    //1.check whether the request exists
    req = request_list.DoesExists(handle);
    if (req == NULL) {
        QLOGW("Request does not exist, nothing released");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return FAILED;
    }

    //2.apply operation i.e. release
    resource_qs.RemoveAndApply(req);

    //handle display off case
    if (req->GetPriority() == HIGH_PRIORITY) {
        resource_qs.UnlockCurrentState(req);
        arm_poll_timer(1);
    }

    //3.remove from our db
    request_list.Remove(req);

    if (req->GetClient() == REGULAR_CLIENT && --lockcount == 0) {
        /* Release profile lock */
        switch_profile(profilehandle, -1, 1);
        profilehandle = 0;
    }

    if (req) {
        delete req;
        req = NULL;
    }

    if (0 == request_list.GetNumActiveRequests())
        arm_poll_timer(0);

    if (PERF_SYSTRACE) {
        ATRACE_END();
    }
    return SUCCESS;
}

void check_display_status_lock_cores_online(int num_cpus) {
    int fd;
    char buf[20] = {0};
    int params[] = {0};

    if (num_cpus < 2 || num_cpus > nocs()) {
        return;
    }

    params[0] = ((CLUSTR_0_CPUS_ON & 0xff) << 8) | (num_cpus & 0xff);

    fd = open("/sys/class/graphics/fb0/mdp/power/runtime_status", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf) - 1) == -1) {
            QLOGW("Unable to read display status");
        }
        else if (!(strncmp(buf, "active", 6))) {
            internal_perf_lock_acq(0, 0, params, sizeof(params)/sizeof(params[0]), getpid(), gettid(), REGULAR_CLIENT);
        }

        close(fd);
    }
}

/*=========================================================================================================================*/
static void Init() {
    Target &t = Target::getCurTarget();
    t.InitializeTarget();
    OptsData::getInstance().Init();
    OptsHandler::getInstance().Init();
    ResetHandler::getInstance().Init();
    resource_qs.Init();
}

static void *mpctl_server(void *data) {
    int rc, cmd;
    struct sigevent sigEvent;
    int ksm = 0, value = 0;
    mpctl_msg_t *msg = NULL;
    char cores_online_prop[PROPERTY_VALUE_MAX];
    char boot_completed[PROPERTY_VALUE_MAX];
    Request *req = NULL;
    profile_handle *pHandle = NULL;
    (void)data;

    QLOGI("Checking boot complete");
    do {
        if (property_get(boot_cmplt_prop, boot_completed, "0")) {
            value = atoi(boot_completed);
        }
        if(!value)
            usleep(100000);
    } while(!value);
    QLOGI("Boot complete, post boot executed");

    load_profiles();
    Init();


    rc = pthread_create(&chk_core_online_thrd, NULL, chk_core_online_poll, NULL);
    if (rc != 0) {
        QLOGE("perflockERR: Unable to create Chk Core Thread\n");
        return NULL;
    }

    //sleep of 100 msec for allowing time to ensure the creation of above threads
    usleep(100000);

    //for async cores (not supported yet)
    setup_cpu_freq_mappings();

    OptsData &d = OptsData::getInstance();
    ResetHandler::getInstance().reset_to_default_values(d);

    if (!poll_timer_created) {
        /* create timer to poll client status */
        sigEvent.sigev_notify = SIGEV_THREAD;
        sigEvent.sigev_notify_function = &poll_cb;
        sigEvent.sigev_notify_attributes = NULL;

        rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &poll_timer);
        if (rc != 0) {
            QLOGE("Failed to create timer for client poll");
        } else {
            poll_timer_created = true;
        }
    }

    if (property_get("ro.qualcomm.perf.cores_online", cores_online_prop, NULL) > 0) {
        if (isdigit(cores_online_prop[0])){
            check_display_status_lock_cores_online(atoi(cores_online_prop));
        }
    }

    //check if KSM us enabled or not if enable than run aggressivly
    ksm = OptsData::getInstance().init_ksm();
    if(ksm == 0)
    {
        run_ksm_aggressive();
    }

    pthread_mutex_lock(&subm_req_mutex);
    sInitComplete = true;
    pthread_mutex_unlock(&subm_req_mutex);

    /* Main loop */
    for (;;) {
        //wait for perflock commands
        EventData *evData = evqueue.Wait();

        if (!evData || !evData->mEvData) {
            continue;
        }

        cmd = evData->mEvType;
        msg = (mpctl_msg_t *)evData->mEvData;

        for (int i=0; i < msg->data && i < MAX_ARGS_PER_REQUEST; i++) {
            QLOGI("Main loop pl_args[%d] = %x", i, msg->pl_args[i]);
        }

        QLOGI("Received time=%d, data=%d, hint_id=0x%x, hint_type=%d handle=%d",
                msg->pl_time, msg->data, msg->hint_id, msg->hint_type, msg->pl_handle) ;
        QLOGI("pid:%d tid:%d ", msg->client_pid, msg->client_tid);

        switch (cmd) {
            case MPCTL_CMD_PERFLOCKHINTACQ:
            case MPCTL_CMD_PERFLOCKACQ:
                req = (Request *)msg->userdata;
                internal_perf_lock_acq_apply(msg->pl_handle, req);
                break;

            case MPCTL_CMD_PERFLOCKREL:
                internal_perf_lock_rel(msg->pl_handle);
                break;

            case MPCTL_CMD_PERFLOCKPOLL:
                poll_client_status();
                break;

            case MPCTL_CMD_PERFLOCKRESET:
                reset();
                break;

            case MPCTL_CMD_PERFLOCK_PROFILE:
                pHandle = (profile_handle *)msg->userdata;
                apply_profile(pHandle, msg->profile, msg->pl_handle);
                break;

            case MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS:
#if OEM_PROFILE_RESTORE_ON_FAIL
                QLOGE("Calling OEM Profile Reset func - reset_profile_mgr_nodes");
                reset_profile_mgr_nodes();
#else
                QLOGE("Calling Generic reset function - reset_to_default_values");
                ResetHandler::getInstance().reset_to_default_values(d);
                QLOGE("Applying the Current Profile %d again since cores are online..", current_profile);
                switch_profile(get_cur_prof_handle(), current_profile, 0);
#endif
                break;

            default:
                QLOGE("Unknown command %d", cmd);
        }

        //give the event data back to memory pool
        evqueue.GetDataPool().Return(evData);
    }

    QLOGI("MPCTL server thread exit due to rc=%d", rc);
    return NULL;
}

static void check_for_prof_change(int cur_mode)
{
    static int single_handle, multi_handle, perf_cl_peak_hndl;
    if ((cur_mode & PERF_CL_PEAK_MODE) && !perf_cl_peak_hndl) {
        perf_cl_peak_hndl = perf_lock_use_profile(perf_cl_peak_hndl, PERF_CL_PEAK_PROF);
        QLOGE("perflock: PERF CL PEAK MODE ON mode:%d\n", cur_mode);
    } else if (!(cur_mode & PERF_CL_PEAK_MODE) && perf_cl_peak_hndl) {
        perf_lock_use_profile(perf_cl_peak_hndl, -1);
        perf_cl_peak_hndl = 0;
        QLOGE("perflock: PERF CL PEAK MODE OFF mode:%d\n", cur_mode);
    }

    if ((cur_mode & SINGLE_MODE) && !single_handle) {
        single_handle = perf_lock_use_profile(single_handle, SINGLE_PROF);
        QLOGE("perflock: SINGLE MODE ON mode:%d\n", cur_mode);
    } else if (!(cur_mode & SINGLE_MODE) && single_handle) {
        perf_lock_use_profile(single_handle, -1);
        single_handle = 0;
        QLOGE("perflock: SINGLE MODE OFF mode:%d\n", cur_mode);
    }

    if ((cur_mode & MULTI_MODE) && !multi_handle) {
        multi_handle = perf_lock_use_profile(multi_handle, MULTI_PROF);
        QLOGE("perflock: MULTI MODE ON mode:%d\n", cur_mode);
    } else if (!(cur_mode & MULTI_MODE) && multi_handle) {
        perf_lock_use_profile(multi_handle, -1);
        multi_handle = 0;
        QLOGE("perflock: MULTI MODE OFF mode:%d\n", cur_mode);
    }
}

static void *chk_core_online_poll (void *data) {
    int rc = 0;
    int cluster0_online_cpu = FAILED, cluster1_online_cpu = FAILED;
    struct pollfd fds;
    char buf[NODE_MAX];
    Target &t = Target::getCurTarget();
    fds.fd  = open(POLLPATH, O_RDONLY);
    if (fds.fd < 0) {
        QLOGE("Unable to open %d", fds.fd);
        return 0;
    }
    rc = pread(fds.fd, buf, NODE_MAX, 0);
    fds.events = POLLERR|POLLPRI;
    fds.revents = 0;
    while (true) {
        /*Mutex Lock*/
        pthread_mutex_lock (&reset_nodes_mutex);
        /*Wait on Signal from the update-files or drop-old files when write fails*/
        QLOGE(" before pthread_cond_wait %d, %s", __LINE__, __FUNCTION__);
        pthread_cond_wait(&core_offline_cond, &reset_nodes_mutex);
        QLOGE(" after pthread_cond_wait %d, %s", __LINE__, __FUNCTION__);
        /*Unlock the mutex and now wait on poll and do the job in thread*/
        pthread_mutex_unlock (&reset_nodes_mutex);
        QLOGE(" after pthread_mutex_unlock %d, %s", __LINE__, __FUNCTION__);
        do {
            cluster0_online_cpu = cluster1_online_cpu = FAILED;
            QLOGE("Block on poll()");
            rc = poll(&fds, 1, 3000);
            if (rc < 0) {
                QLOGE("poll() has returned error for %s", POLLPATH);
            } else if (rc == 0) {
                QLOGE("poll() has timed out for %s", POLLPATH);
            } else {
                QLOGE("Poll has found event, rc = %d, line = %d, func = %s",
                        rc,  __LINE__, __FUNCTION__);
                rc = pread(fds.fd, buf, NODE_MAX, 0);
                if ((cluster0_online_cpu = get_online_core(0,
                                t.getLastCoreIndex(0))) != FAILED) {
                    QLOGE(" CPU %d Online... %d, %s",
                            cluster0_online_cpu, __LINE__, __FUNCTION__);
                } else {
                    QLOGE(" Cluster 0 is Offline... %d, %s",
                            __LINE__, __FUNCTION__);
                }
                if ((cluster1_online_cpu = get_online_core(t.getLastCoreIndex(0)+1,
                                t.getLastCoreIndex(1))) != FAILED) {
                    QLOGE(" CPU %d Online... %d, %s",
                            cluster1_online_cpu, __LINE__, __FUNCTION__);
                } else {
                    QLOGE(" Cluster 1 is Offline... %d, %s",
                            __LINE__, __FUNCTION__);
                }
            }
        } while (!(cluster0_online_cpu != FAILED &&
                    cluster1_online_cpu != FAILED));

        /*Call reset nodes here and again continue in loop to wait for further
         * signals*/
        pthread_mutex_lock (&reset_nodes_mutex);
        perf_lock_cmd(MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS);
        pthread_mutex_unlock (&reset_nodes_mutex);
    }
    close(fds.fd);
}

//callbacks for eventqueue
static void *Alloccb() {
    void *mem = (void *) new mpctl_msg_t;
    if (NULL ==  mem)
        QLOGE("memory not allocated");
    return mem;
}

static void Dealloccb(void *mem) {
    if (NULL != mem) {
        delete (mpctl_msg_t *)mem;
    }
}


////////////////////////////////////////////////////////////////////////
///perf module interface
////////////////////////////////////////////////////////////////////////
static int mpctlEvents[] = {
   VENDOR_PERF_HINT_BEGIN,
   VENDOR_POWER_HINT_END,  //As perfd need to incorporate power hint also
};

static PerfGlueLayer mpctlglue = {
    LOAD_LIB,
    mpctlEvents,
    sizeof(mpctlEvents)/sizeof(mpctlEvents[0])
};

//interface implementation
int perfmodule_init() {
    int rc = 0;
    char trace_prop[PROPERTY_VALUE_MAX];

    /* Enable systraces by adding debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }

    QLOGI("MPCTL server starting");
    evqueue.GetDataPool().SetCBs(Alloccb, Dealloccb);

    rc = pthread_create(&mpctl_server_thread, NULL, mpctl_server, NULL);
    if (rc != 0) {
        QLOGE("perflockERR: Unable to create mpctl server thread");
        return FAILED;
    }
    return SUCCESS;
}

void perfmodule_exit() {
    pthread_join(mpctl_server_thread, NULL);
    pthread_cond_destroy(&core_offline_cond);
    pthread_mutex_destroy(&reset_nodes_mutex);
    pthread_mutex_destroy(&subm_req_mutex);
    pthread_mutex_destroy(&perf_veri_mutex);
}

int perfmodule_submit_request(mpctl_msg_t *msg) {
    int size = 0;
    Target &t = Target::getCurTarget();
    int handle = -1;
    EventData *evData;
    int *pl_args = NULL;
    int timeout = 0;
    Request *req = NULL;
    profile_handle * pHandle = NULL;
    int ret = SUCCESS, cmd = 0;
    int before_handle = -1;
    bool reqExist = false;


    if (NULL == msg) {
        return handle;
    }

    pthread_mutex_lock(&subm_req_mutex);
    if (!sInitComplete) {
        QLOGE("Request not accepted, as target initialization is not complete.");
        pthread_mutex_unlock(&subm_req_mutex);
        return handle;
    }

    QLOGI("Received time=%d, data=%d, hint_id=0x%x hint_type=%d",
              msg->pl_time, msg->data, msg->hint_id, msg->hint_type) ;

    handle = msg->pl_handle;
    cmd = msg->req_type;
    pl_args = msg->pl_args;
    size = msg->data;

    evData = evqueue.GetDataPool().Get();
    if ((NULL == evData) || (NULL == evData->mEvData)) {
        QLOGE("event data pool ran empty");
        pthread_mutex_unlock(&subm_req_mutex);
        return handle;
    }
    mpctl_msg_t *pMsg = (mpctl_msg_t *)evData->mEvData;
    memset(pMsg, 0x00, sizeof(mpctl_msg_t));
    pMsg->data = msg->data;
    pMsg->pl_handle = msg->pl_handle;
    pMsg->req_type = msg->req_type;
    pMsg->profile = msg->profile;
    pMsg->pl_time = msg->pl_time;
    pMsg->client_pid = msg->client_pid;
    pMsg->client_tid = msg->client_tid;
    pMsg->hint_id = msg->hint_id;
    pMsg->hint_type = msg->hint_type;
    strlcpy(pMsg->usrdata_str, msg->usrdata_str, MAX_MSG_APP_NAME_LEN);
    for (int i=0; (i < msg->data) && (msg->data < MAX_ARGS_PER_REQUEST); i++) {
        pMsg->pl_args[i] = msg->pl_args[i];
    }

    switch (cmd) {
        case MPCTL_CMD_PERFLOCKHINTACQ:
            ret = SUCCESS;
            //feedback supplier exists, ask for feedback
            //returns -1 -- failure,
            //         0 -- success and hintlookup needed
            //         1 -- no hintlookup needed
            ret = fbh.FbCb(pMsg);
            QLOGI("Feedback Supplier return status: %d", ret);

            if (ret <= 0) {
                size = t.getBoostConfig(pMsg->hint_id, pMsg->hint_type, pMsg->pl_args, &timeout);
                if (size == 0) {
                    //hint lookup failed, bailout
                    handle = -1;
                    QLOGW("Unsupported hint_id=0x%X, hint_type=%d for this target", pMsg->hint_id, pMsg->hint_type);
                    break;
                }
                if (pMsg->pl_time <= 0) {
                    pMsg->pl_time = timeout;
                }
                pMsg->data = size;
            }
            strlcpy(pMsg->usrdata_str, msg->usrdata_str, MAX_MSG_APP_NAME_LEN);
            //intentional fall through
        case MPCTL_CMD_PERFLOCKACQ:
            before_handle = pMsg->pl_handle;
            req = internal_perf_lock_acq_verification(pMsg->pl_handle, pMsg->pl_time, pMsg->pl_args, size,
                    pMsg->client_pid, pMsg->client_tid);
            handle = pMsg->pl_handle;
            if (req != NULL) {
                pMsg->userdata = (void *) req;
            } else if ((handle > 0) && (handle == before_handle)) {
                reqExist = true;
            } else {
                handle = -1;
            }
            break;
        case MPCTL_CMD_PERFLOCK_PROFILE:
            pHandle = profile_verification(pMsg->pl_handle, pMsg->profile, 0);
            handle = pMsg->pl_handle;
            if (pHandle != NULL) {
                pMsg->userdata = (void *) pHandle;
            } else {
                handle = -1;
            }
            break;
        default:
            break;
    }

    if (reqExist || (handle < 0)) {
        evqueue.GetDataPool().Return(evData);
        evData = NULL;
    }

    if (NULL != evData) {
        evData->mEvType = cmd;
        evqueue.Wakeup(evData);
    }

    pthread_mutex_unlock(&subm_req_mutex);
    return handle;
}
