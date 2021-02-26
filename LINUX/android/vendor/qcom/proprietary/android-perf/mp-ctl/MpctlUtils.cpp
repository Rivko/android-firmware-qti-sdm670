/******************************************************************************
  @file    MpctlUtils.cpp
  @brief   Implementation of mpctl utils functions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define LOG_TAG "ANDR-PERF-UTIL"
#include <cutils/log.h>

#include "OptsData.h"
#include "MpctlUtils.h"

#define SLVP_TIMEOUT_MS 500 /* SLVP callback kicked every 500ms */

static int core_status[MAX_CORES];
pthread_mutex_t reset_nodes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t core_offline_cond = PTHREAD_COND_INITIALIZER;
extern int ncpus;

void write_to_file(FILE *output, const char *input_node, const char *input_val) {
    char buf[NODE_MAX];
    char tmp_s[NODE_MAX];
    int len = 0;
    int rc = 0;

    if (output == NULL) {
        QLOGE("Invalid default value file passed");
        return;
    }

    memset(buf, 0, NODE_MAX);
    FREAD_STR(input_val, buf, NODE_MAX, rc);
    if (rc >= 0) {
        snprintf(tmp_s, NODE_MAX, "%s;%s", input_node, buf);
        len = strlen(tmp_s);
        fwrite(tmp_s, sizeof(char), len, output);
        return;
    }
    QLOGW("Unable to write node value %s to default values file", input_node);
}

int  write_to_node(FILE *read_file, const char *node_str, const char *node) {
    char line[NODE_MAX];
    const char *delimit = ";";
    char *token = NULL;
    char *tmp_token = NULL;
    int rc = 0;

    if (read_file == NULL) {
        QLOGE("Invalid default value filefile passed");
        return rc;
    }

    rewind(read_file);

    while (fgets(line, sizeof(line), read_file) != NULL) {
        if (strstr(line, node_str)) {
            token = strtok_r(line, delimit, &tmp_token);
            token = strtok_r(NULL, delimit, &tmp_token);
            if (token != NULL) {
                FWRITE_STR(node, token, strlen(token), rc);
             }
            return rc;
        }
    }
    QLOGW("Unable to read node value %s from default values file", node_str);
    return rc;
}

void signal_chk_poll_thread(char *node, int rc) {
  if (rc <= 0) {
      QLOGE("read/write failed for %s, rc=%d", node, rc);
      pthread_mutex_lock (&reset_nodes_mutex);
      /*Signal the thread to look for cores to come online*/
      pthread_cond_signal(&core_offline_cond);
      /*Unlock the mutex*/
      pthread_mutex_unlock (&reset_nodes_mutex);
      /*we failed to write node, bail out of the loop*/
  }
}

/* Function used to populate an array to keep track of core status
 * Warning: the array will be flushed on each call
 * Returns: the number of cores online or -1 on error
 */
inline int get_online_cpus (int * status_array, int array_sz) {
    #define co_node "/sys/devices/system/cpu/online"
    #define CO_NODE_MAX_LEN     32
    int start = 0;
    int end = 0;
    int range = 0;
    int i = 0;
    int j = 0;
    int online_count = 0;
    char online[CO_NODE_MAX_LEN] = {0};
    int len = 0;
    int rc = 0;

    FREAD_STR(co_node, online, CO_NODE_MAX_LEN, rc);

    len = strlen(online);
    if ((len < 1) || (len > CO_NODE_MAX_LEN) || (NULL == status_array)) {
        return -1;
    }

    memset(status_array, 0, sizeof(int)*array_sz);

    for (j = 0; j < len; j++) {
        if (isdigit(online[j])) {
            if (range) {
                end = online[j] - '0';
                for (i = start+1; i < end+1; i++) {
                    status_array[i] = 1;
                    online_count++;
                }
                range = 0;
            } else {
                start = online[j] - '0';
                status_array[start] = 1;
                online_count++;
            }
        }
        else if (online[j] == '-') {
           range = 1;
        }
    }
    return online_count;
}

int get_core_status(int cpu) {
    if (cpu < 0 || cpu >= Target::getCurTarget().getTotalNumCores()) {
        QLOGI("Invalid core %d", cpu);
        return FAILED;
    }
    return core_status[cpu];
}

//Return first online core of given cluster
//Cluster is specified through the first core
//and last core in the cluster.
int get_online_core(int startcpu, int endcpu) {
    int i;

    get_online_cpus(core_status, sizeof(core_status)/sizeof(core_status[0]));
    for (i = startcpu; i <= endcpu; i++) {
         if (get_core_status(i) > 0) {
             QLOGI("First online core in this cluster is %d", i);
             return i;
         }
    }
    QLOGI("No online core in this cluster");
    return FAILED;
}

extern void set_slvp_cb(sigval_t);

int vid_create_timer(hint_associated_data *data) {
    int rc = 0;
    struct sigevent sigEvent;

    if (NULL == data) {
        return FAILED;
    }

    sigEvent.sigev_notify = SIGEV_THREAD;
    sigEvent.sigev_notify_function = &set_slvp_cb;
    sigEvent.sigev_value.sival_ptr = data;
    sigEvent.sigev_notify_attributes = NULL;
    rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &data->tTimerId);
    if (rc != 0) {
        QLOGE("Failed to create timer");
        return FAILED;
    } else {
        data->timer_created = 1;
        /* first time arm slvp timer */
        rearm_slvp_timer(data);
    }

    return rc;
}

/* Rearms the SLVP timer
 * SLVP - Single Layer Video Playback
 */
void rearm_slvp_timer(hint_associated_data *data) {
    if (NULL == data) {
        return;
    }

    data->mInterval.it_value.tv_sec = SLVP_TIMEOUT_MS / 1000;
    data->mInterval.it_value.tv_nsec = SLVP_TIMEOUT_MS * 1000000;
    data->mInterval.it_interval.tv_sec = 0;
    data->mInterval.it_interval.tv_nsec = 0;
    //arming or disarming can only be possible if a timer exists
    if (data->timer_created) {
        timer_settime(data->tTimerId, 0, &data->mInterval, NULL);
    }
}

/* Cancels SLVP timer*/
void cancel_slvp_timer(hint_associated_data *data) {
    if (NULL == data) {
        return;
    }

    data->mInterval.it_value.tv_sec = 0;
    data->mInterval.it_value.tv_nsec = 0;
    data->mInterval.it_interval.tv_sec = 0;
    data->mInterval.it_interval.tv_nsec = 0;
    //arming or disarming can only be possible if a timer exists
    if (data->timer_created) {
        timer_settime(data->tTimerId, 0, &data->mInterval, NULL);
    }
}

void send_irq_balance(int *msg, int **ret) {
    int balance_fd;
    int i;
    struct sockaddr_un balance_addr;
    socklen_t len = sizeof(struct sockaddr_un);

    balance_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (balance_fd == -1) {
        QLOGE("Can't make irqbalance socket (%s)", strerror(errno));
        return;
    }

    memset(&balance_addr, 0, sizeof(balance_addr));
    balance_addr.sun_family = AF_UNIX;
    strlcpy(balance_addr.sun_path, "/dev/socket/msm_irqbalance",
            sizeof(balance_addr.sun_path));

    if (connect(balance_fd, (struct sockaddr *)&balance_addr, len) == -1) {
        QLOGE("bad connect (%s)", strerror(errno));
        goto out;
    }

    if (send(balance_fd, msg, ncpus * sizeof(msg[0]), 0) == -1) {
        QLOGE("bad send (%s)", strerror(errno));
        goto out;
    }

    if (ret && recv(balance_fd, *ret, ncpus * sizeof(msg[0]), 0) == -1) {
        QLOGE("bad recv (%s)", strerror(errno));
        goto out;
    }

out:
    close(balance_fd);
}

int update_freq_node(int start, int end, unsigned int val, char *fmtStr, int len) {
    int i = 0, ret = 0;
    char *tmp_s;

    if (fmtStr == NULL) {
        QLOGE("Format string is pointing to null");
        return FAILED;
    }
    memset(fmtStr, 0, len);
    tmp_s = fmtStr;

    if (start >= 0 && end >= 0) {
        for (i = start; i <= end && len > 0; i++) {
             if (i < end) {
                 ret = snprintf(tmp_s, len, "%d:%u ", i, val);
             } else {
                 ret = snprintf(tmp_s, len, "%d:%u", i, val);
             }
             tmp_s += ret;
             len -= ret;
        }
        return SUCCESS;
    }
    return FAILED;
}

/* Updates the node_path with the node_strg value only after ensuring
paths are not null and lenght to write is with in limit ranges.*/
int change_node_val(const char *node_path, const char *node_strg, int node_strg_l) {
    int rc = FAILED;
    if ((node_path == NULL) || (strlen(node_path) == 0)) {
        return FAILED;
    }

    if ((node_strg == NULL) || (node_strg_l <= 0) || (node_strg_l > NODE_MAX)) {
        return FAILED;
    }

    FWRITE_STR(node_path, node_strg, node_strg_l, rc);
    if (rc > 0) {
        QLOGI("Updated %s with %s, return value %d", node_path, node_strg, rc);
    } else {
        QLOGE("Failed to update node %s with value %s\n", node_path, node_strg);
    }
    return rc;
}

/* Saves the current value present in path into the value string and returns
the length of the string on success. On failure returns -1.*/
int save_node_val(const char* path, char* value ) {
    int rc = FAILED;
    int storage_len = 0;
    char *bptr = NULL;

    if ((path == NULL) || (strlen(path) == 0)) {
        return FAILED;
    }
    FREAD_STR(path, value, NODE_MAX, rc);
    if (rc >= 0) {
        /* Ensure the string we are storing has no trailing whitespaces or newlines */
        bptr = value + strlen(value);
        while(bptr != value && (*(bptr - 1) == '\n' || *(bptr - 1) == ' ')) {
            --bptr;
        }
        *bptr = '\0';

        QLOGI("%s read with %s return value %d", path, value, rc);
        storage_len = strlen(value);
        return storage_len;
    } else {
        QLOGE("Failed to read %s", path);
        return FAILED;
    }
}

char* get_devfreq_new_val(unsigned int reqval, unsigned int *list, int size, char* maxfreq_path) {
    int setval = FAILED;
    char maxfreq_val[NODE_MAX];
    char tmp_s[NODE_MAX];
    int i, rc = FAILED;
    char* retval = NULL;

    maxfreq_val[0] = 0; //initialize empty array

    if (size < 0 || size > NODE_MAX || maxfreq_path == NULL)
        return NULL;

    for (i = 0; i < size; i++) {
        if (*(list + i) >= reqval) {
            QLOGI("Available freq value is %d",*(list + i));
            setval = *(list + i);
            break;
        }
    }

    if (i == size) {
        setval = *(list + i - 1);
    }

    if (setval == FAILED) {
        QLOGE("Error! Perflock failed, invalid freq value %d", setval);
        return NULL;
    }

    snprintf(tmp_s, NODE_MAX, "%d", setval);

    /* read max_freq  */
    FREAD_STR(maxfreq_path, maxfreq_val, NODE_MAX, rc);

    if (rc < 0 || strlen(maxfreq_val) == 0) //uninitialized maxfreq_val
        return NULL;

    /* min_freq cannot be more than intended value of
     * max_freq, so min_freq maintained same as max_freq
     */
    if (atoi(tmp_s) > atoi(maxfreq_val)) {
        retval = (char*)malloc((strlen(maxfreq_val) + 1) * sizeof(char)); //extra byte for null char
        if (retval != NULL)
            snprintf(retval, NODE_MAX, "%d", atoi(maxfreq_val));
    }
    else {
        retval = (char*)malloc((strlen(tmp_s) + 1) * sizeof(char)); //extra byte for null char
        if (retval != NULL)
            snprintf(retval, NODE_MAX, "%d", atoi(tmp_s));
    }
    if (retval != NULL) {
        QLOGI("perf_lock: value to be acq %s", retval);
        return retval;
    }
    else
        return NULL;
}

bool minBoundCheck(char* max, char* min, char* reqval) {

    if (!max || !min || !reqval) {
        QLOGE("Null buffers in minBoundCheck");
        return false;
    }

    int maxfreq = atoi(max);
    int minfreq = atoi(min);
    int reqfreq = atoi(reqval);

    if (reqfreq >= minfreq) {
        if (maxfreq > reqfreq)
            return true;
        else
            return false;
    } else { /*deny request if reqval is lower than minfreq*/
        return false;
    }
}

bool maxBoundCheck(char* max, char* min, char* reqval) {

    if (!max || !min || !reqval) {
        QLOGE("Null buffers in maxBoundCheck");
        return false;
    }

    int maxfreq = atoi(max);
    int minfreq = atoi(min);
    int reqfreq = atoi(reqval);

    if (reqfreq <= maxfreq) {
        if (minfreq < reqfreq)
            return true;
        else
            return false;
    } else { /*deny request if reqval is higher than maxfreq*/
        return false;
    }
}
