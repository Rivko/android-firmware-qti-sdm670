
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Copyright (c) 2014-2016, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#define LOG_TAG "LocSvc_launcher"

#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <loc_cfg.h>
#include <sys/prctl.h>
#include <loc_misc_utils.h>
#include <sys/capability.h>
#include <cutils/properties.h>
#include <log_util.h>
#include <loc_pla.h>

#define MAX_CHILD_PROC_RESTARTS 5
#define ONE_DAY_SECONDS 86400

pthread_mutex_t srv_sockt_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  srv_sockt_cond  = PTHREAD_COND_INITIALIZER;

// Notify main thread from signal handling thread
pthread_mutex_t signal_handling_done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  signal_handling_done_cond  = PTHREAD_COND_INITIALIZER;

// Handling waitpid for signals from child process
pthread_mutex_t  signal_handler_mutex = PTHREAD_MUTEX_INITIALIZER;

static loc_process_info_s_type *child_proc = nullptr;
static unsigned int proc_list_length = 0;
static unsigned char* num_restarts_ptr = nullptr;
static bool thread_loop_continue = true;

static void* signal_handler_thread_func(void* arg);

extern int mq_server_launch();

int main()
{
    volatile unsigned int i = 0;
    unsigned int j = 0;
    gid_t gid_list[LOC_PROCESS_MAX_NUM_GROUPS];
    sigset_t signal_set;
    int ngroups = 0, ret = 0;
    struct timespec time_info;

    pthread_t signal_handler_thread;
    int result = 0;

    if (0 == getuid()) {
        struct __user_cap_header_struct cap_hdr = {};
        struct __user_cap_data_struct cap_data = {};

        cap_hdr.version = _LINUX_CAPABILITY_VERSION;
        cap_hdr.pid = 0;

        if(prctl(PR_SET_KEEPCAPS, 1) < 0) {
            LOC_LOGE("%s:%d]: Error: prctl failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }

        // Set the group id first and then set the effective userid, to gps.
        if (-1 == setgid(GID_GPS)) {
            LOC_LOGE("%s:%d]: Error: setgid failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }
        // Set the loc_launcher uid to 'gps'.
        if (-1 == setuid(UID_GPS)) {
            LOC_LOGE("%s:%d]: Error: setuid failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }

        cap_data.permitted = (1<<CAP_SETUID) | (1<<CAP_SETGID);
        cap_data.effective = cap_data.permitted;

        LOC_LOGD("cap_data.permitted: %d", (int)cap_data.permitted);

        if(capset(&cap_hdr, &cap_data)) {
            LOC_LOGE("%s:%d]: Error: capset failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }
    }

    LOC_LOGD("Parent- pid: %d, uid: %d, euid: %d, gid: %d, egid: %d\n",
             getpid(), getuid(), geteuid(), getgid(), getegid());

    // Launch message queue server
    mq_server_launch();

    // give time for mq_server to instantiate, to avoid the race conditions
    // with this main thread, ensure that the main thread
    // enters suspend after mq_server instantiation is done.
    clock_gettime(CLOCK_REALTIME,&time_info);
    time_info.tv_sec += 3;

    pthread_mutex_lock(&srv_sockt_mutex);
    pthread_cond_timedwait(&srv_sockt_cond,&srv_sockt_mutex,&time_info);
    pthread_mutex_unlock(&srv_sockt_mutex);

    //Read current group subscriptions
    memset(gid_list, 0, sizeof(gid_list));
    ngroups = getgroups(LOC_PROCESS_MAX_NUM_GROUPS, gid_list);
    if(ngroups == -1) {
        LOC_LOGE("Could not find groups. ngroups:%d\n", ngroups);
    }
    else {
        IF_LOC_LOGD {
            LOC_LOGD("GIDs from parent: ");
            for(ngroups = 0; ngroups<LOC_PROCESS_MAX_NUM_GROUPS; ngroups++) {
                if(gid_list[ngroups]) {
                    LOC_LOGD("%d ", gid_list[ngroups]);
                }
            }
            LOC_LOGD("\n");
        }
    }

    // parse the process conf file: izat.conf
    ret = loc_read_process_conf(LOC_PATH_IZAT_CONF, &proc_list_length, &child_proc);
    if (ret != 0) {
        goto err_conf_file;
    }
    // allocate memory to keep track for number of restarts
    num_restarts_ptr = (unsigned char*) malloc (proc_list_length * sizeof (unsigned char));
    if (num_restarts_ptr == NULL) {
        free (child_proc);
        child_proc = NULL;
        goto err_conf_file;
    }
    memset (num_restarts_ptr, 0, proc_list_length * sizeof (unsigned char));

    // Block signal SIGCHLD and SIGALRM in main process thread
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGCHLD);
    sigaddset(&signal_set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    // Create a thread to handle signals and set a name for it.
    result = pthread_create(&signal_handler_thread, NULL,
            signal_handler_thread_func, (void *) &signal_set);
    if (result != 0) {
        LOC_LOGE("pthread_create failed. Error = %d\n", result);
        goto err_signal_thread_fail;
    }
    result = pthread_setname_np(signal_handler_thread, (char *) "loc_sig_hndlr");
    if (result != 0) {
        LOC_LOGE("pthread_setname_np failed. Error = %d\n", result);
    }

    //Set an alarm to be woken up after 24 hours to clear
    //the number of restarts for active child processes
    alarm(ONE_DAY_SECONDS);

    //Start recurring loop. This loop does the following things:
    //1. Launch enabled child processes
    while(1) {
        //Start making children
        for(i=0; i<proc_list_length;i++) {
            if(child_proc[i].proc_status == ENABLED) {
                LOC_LOGD("%s:%d]: Forking child: %d, name: %s",
                         __func__, __LINE__, i, child_proc[i].name[0]);
                if((child_proc[i].proc_id = fork()) < 0) {
                    LOC_LOGE("%s: fork of child failed:%s\n",
                          child_proc[i].name[0], strerror(errno));
                    exit(1);
                }
                //This block executed by child process
                else if(!child_proc[i].proc_id) {
                    LOC_LOGD("Child born- pid: %d, uid: %d, euid: %d, gid: %d, egid: %d\n",
                             getpid(),getuid(), geteuid(), getgid(), getegid());
                    ngroups = setgroups(child_proc[i].num_groups, child_proc[i].group_list);
                    if(ngroups == -1) {
                        LOC_LOGE("Could not set groups. errno:%d, %s\n",
                              errno, strerror(errno));
                    }
                    LOC_LOGD("%s:%d]: Groups set\n", __func__, __LINE__);

                    memset(gid_list, 0, sizeof(gid_list));
                    ngroups = getgroups(LOC_PROCESS_MAX_NUM_GROUPS, gid_list);
                    if(ngroups == -1) {
                        LOC_LOGE("Could not find groups. ngroups:%d\n", ngroups);
                    }
                    else {
                        IF_LOC_LOGD {
                            LOC_LOGD("GIDs from child:\n");
                            for(ngroups = 0; ngroups<LOC_PROCESS_MAX_NUM_GROUPS; ngroups++) {
                                if(gid_list[ngroups]) {
                                    LOC_LOGD("%d ", gid_list[ngroups]);
                                }
                            }
                            LOC_LOGD("\n");
                        }
                    }
                    ngroups=0;
                    //Set up the child process to receive exit when it receives
                    //a SIGHUP.
                    //SIGHUP is sent to the child process when the parent process
                    //exits. We want all child processes to exit when the launcher
                    //exits so that restarting the launcher will re-launch all the
                    //child processes.
                    if(prctl(PR_SET_PDEATHSIG, SIGHUP, 0, 0, 0) == -1) {
                        LOC_LOGE("%s:%d]: prctl failed. %s", __func__, __LINE__, strerror(errno));
                    }

                    char *args_temp[LOC_PROCESS_MAX_NUM_ARGS];
                    memset (args_temp, 0, sizeof (args_temp));
                    uint32_t args_count;
                    for (args_count = 0; args_count < LOC_PROCESS_MAX_NUM_ARGS; args_count++) {
                        if (child_proc[i].args[args_count][0] == '\0') {
                          break;
                        } else {
                          args_temp[args_count] = child_proc[i].args[args_count];
                        }
                    }
                    execvp(child_proc[i].name[0], args_temp);
                    int exec_error = errno;
                    LOC_LOGE("\nError! execvp() returned. Something is wrong. Could not start %s,"
                             "Error: %d [%s]\n",
                             child_proc[i].name[0], exec_error, strerror(exec_error));
                    exit(1);

                }
                //This block executed by parent process
                else {
                    LOC_LOGD("Child %s created with pid: %d\n",
                             child_proc[i].name[0], child_proc[i].proc_id);
                    child_proc[i].proc_status = RUNNING;
                }
            }
        }
        LOC_LOGD("Parent sleeping. Will wake up when one of the children mess up\n");
        // wait for cond signal from signal handler thread
        pthread_mutex_lock(&signal_handling_done_mutex);
        pthread_cond_wait(&signal_handling_done_cond,&signal_handling_done_mutex);
        pthread_mutex_unlock(&signal_handling_done_mutex);
        LOC_LOGE("Parent woken up!\n");
    }
err:
    if(child_proc) {
        for(j=0; j<proc_list_length; j++)
        {
            if(child_proc[j].proc_id)
                kill(child_proc[j].proc_id, SIGHUP);
            if(child_proc[j].name[0]) {
                free(child_proc[j].name[0]);
            }
        }
        free(child_proc);
    }
    if (num_restarts_ptr) {
        free (num_restarts_ptr);
    }

err_signal_thread_fail:
    // close signal handling thread.
    thread_loop_continue = false;
    pthread_join(signal_handler_thread, NULL);

err_conf_file:
err_capset:
    LOC_LOGE("%s:%d]: Exiting main. ret: %d", __func__, __LINE__, ret);
    return ret;
}

static void* signal_handler_thread_func(void* arg)
{
    int result = 1;
    sigset_t *signal_set = (sigset_t *)arg;
    int signal_num = 0;
    unsigned int i = 0;
    //To be passed in to waitpid
    int child_status;
    int child_exit_status = 0;

    //Recurring thread loop. This loop does the following things:
    //1. Check if SIGCHLD is received and re-enable/disable child processes
    //2. Check if SIGALRM is received and clear restart counter
    while (thread_loop_continue) {
        result = sigwait(signal_set, &signal_num);
        if (0 != result) {
            LOC_LOGE("%s:%d]: sigwait error : %d\n", __func__, __LINE__, result);
        }
        LOC_LOGD("%s:%d]: Got signal %s(%d)\n",  __func__, __LINE__,
                strsignal(signal_num), signal_num);

        //Check if any of the child processes have exited
        if (SIGCHLD == signal_num) {
            LOC_LOGE("%s:%d]: Received SIGCHLD\n", __func__, __LINE__);
            //Loop through the list of child processes and figure out which
            //process has exited
            for (i = 0; i < proc_list_length; i++) {
                child_status = 0;
                child_exit_status = 0;
                if (child_proc[i].proc_status == RUNNING) {
                    LOC_LOGD("%s:%d]: Child_id: %d; child name: %s\n",
                             __func__, __LINE__, child_proc[i].proc_id, child_proc[i].name[0]);
                    pthread_mutex_lock(&signal_handler_mutex);
                    if (waitpid(child_proc[i].proc_id, &child_status, WNOHANG) ==
                       child_proc[i].proc_id) {
                        if (WIFEXITED(child_status)) {
                            child_exit_status = WEXITSTATUS(child_status);
                            if (child_exit_status) {
                                LOC_LOGE("%s:%d]: Exit status: %d. Restarting\n",
                                         __func__, __LINE__, child_exit_status);
                                if (num_restarts_ptr[i] == MAX_CHILD_PROC_RESTARTS) {
                                    child_proc[i].proc_status = DISABLED;
                                    LOC_LOGE("%s:%d]: %s Restarted too many times. Grounded!\n",
                                             __func__, __LINE__, child_proc[i].name[0]);
                                }
                                else {
                                    *(num_restarts_ptr + i) = num_restarts_ptr[i] + 1;
                                    LOC_LOGE("%s:%d]: Restarting %s. Number of restarts: %d\n",
                                             __func__, __LINE__, child_proc[i].name[0],
                                             num_restarts_ptr[i]);
                                    child_proc[i].proc_status = ENABLED;
                                }
                            }
                            else {
                                LOC_LOGE("%s:%d]: Exit status 0. Not Restarting\n",
                                         __func__, __LINE__);
                                child_proc[i].proc_status = DISABLED;
                            }
                        }
                        else if (WIFSIGNALED(child_status)) {
                            LOC_LOGE("%s:%d]: Child exited due to signal: %s\n",
                                     __func__, __LINE__, strsignal(WTERMSIG(child_status)));
                            if(num_restarts_ptr[i] == MAX_CHILD_PROC_RESTARTS) {
                                child_proc[i].proc_status = DISABLED;
                                LOC_LOGE("%s:%d]: %s Restarted too many times. Grounded!\n",
                                         __func__, __LINE__, child_proc[i].name[0]);
                            }
                            else {
                                *(num_restarts_ptr + i) = num_restarts_ptr[i] + 1;
                                LOC_LOGE("%s:%d]: Restarting %s. Number of restarts: %d\n",
                                         __func__, __LINE__, child_proc[i].name[0],
                                         num_restarts_ptr[i]);
                                child_proc[i].proc_status = ENABLED;
                            }
                        }
                        else {
                            LOC_LOGE("%s:%d]:No Reason found\n", __func__, __LINE__);
                        }
                    }
                    else {
                        LOC_LOGD("%s:%d]: Either the %s did not exit or waitpid returned error\n",
                                 __func__, __LINE__, child_proc[i].name[0]);
                    }
                    pthread_mutex_unlock(&signal_handler_mutex);
                }
            }
        } else if (SIGALRM == signal_num) {
            LOC_LOGD("%s:%d]:sigalrm_received\n",__func__, __LINE__);
            for (i = 0; i < proc_list_length; i++) {
                if ((child_proc[i].proc_status == RUNNING) ||
                    (child_proc[i].proc_status == ENABLED)) {
                    LOC_LOGD("%s:%d]: Child_id: %d; child name: %s; num_restarts: %d\n",
                             __func__, __LINE__, child_proc[i].proc_id, child_proc[i].name[0],
                             num_restarts_ptr[i]);
                    *(num_restarts_ptr+i) = 0;
                }
            }
            alarm(ONE_DAY_SECONDS);
        }
        // notify parent of signal handled.
        LOC_LOGD("Notifying parent of signal handling\n");
        pthread_mutex_lock(&signal_handling_done_mutex);
        pthread_cond_signal(&signal_handling_done_cond);
        pthread_mutex_unlock(&signal_handling_done_mutex);
    }
    LOC_LOGD("Exit signal handler thread\n");
    return NULL;
}