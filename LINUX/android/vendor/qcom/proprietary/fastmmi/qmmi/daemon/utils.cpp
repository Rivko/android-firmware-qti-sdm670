/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "utils.h"
/*
* purpose
*     read file
*
* requires
*     filepath: absolute path of file
*     buf: buffer for read data
*     size: size of buffer
*
* guarantees
*       return 0 read success
*/
int read_file(const char *filepath, char *buf, int size) {
    int fd, len;

    fd = open(filepath, O_RDONLY);
    if(fd == -1) {
        ALOGE("file(%s) open fail, error=%s\n", filepath, strerror(errno));
        return -1;
    }

    len = read(fd, buf, size - 1);
    if(len > 0) {
        if(buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else
            buf[len] = '\0';
    }

    close(fd);
    return 0;
}

/*
* purpose
*     write buffer to file
*
* requires
*
* guarantees
*       return 0 read success
*/
int write_file(const char *path, const char *content) {

    FILE *fp = fopen(path, "a");

    if(fp == NULL) {
        ALOGE("file(%s) open fail, error=%s\n", path, strerror(errno));
        return -1;
    }

    fwrite(content, sizeof(char), strlen(content), fp);
    fclose(fp);

    return 0;
}

/*
* purpose
*     check file
*
* requires
*
* guarantees
*       return
*/
bool check_file_exist(const char *path) {
    struct stat buffer;
    int err = stat(path, &buffer);

    if(err == 0)
        return true;
    if(errno == ENOENT) {
        ALOGE("file(%s) do not exist, error=%s\n", path, strerror(errno));
        return false;
    }
    return false;
}

/*
* purpose
*     smb ready status
*
* requires
*
* guarantees
*       check /sys/class/power_supply/parallel/
*       return true when the value set
*/
bool smb_ready() {
    /**set ship mode*/
    return check_file_exist(SYS_POWER_SUPPLY_PARALLEL);
}

/*
* purpose
*     enter ship mode
*
* requires
*
* guarantees
*       set /sys/module/qpnp_power_on/parameters/ship_mode_en = 1
*       return true when the value set
*/
bool enter_ship_mode() {
    char buf[NAME_MAX];
    int ret;

    /**set ship mode*/
    ret = write_file(SYS_SHIP_MODE, "1");
    if(ret < 0) {
        ALOGE("unable to open power_supply/usb/online file\n");
        return false;
    }

    /**confirm the  value*/
    ret = read_file(SYS_SHIP_MODE, buf, sizeof(buf));
    if(ret < 0) {
        ALOGE("unable to open power_supply/usb/online file\n");
        return false;
    }

    ALOGI("value from ship mode enable  is %s\n", buf);
    if(strcmp(buf, "1")) {
        ALOGE("USB is not online during init\n");
        return false;
    }

    return true;
}

/*
* purpose
*     enter charger enable
*
* requires
*
* guarantees
*       set /sys/class/power_supply/battery/input_suspend
*       return true when the value set
*/
void charger_enable(bool enable) {
    if(enable)
        write_file(SYS_INPUT_SUSPEND, "0");
    else
        write_file(SYS_INPUT_SUSPEND, "1");
}


/*
* purpose
*     fork a new process
*
* requires
*
* guarantees
*       return 0  success
*/
int fork_daemon(const char *filepath, char **args, int *cid) {

    if(!check_file_exist(filepath)) {
        ALOGE("file(%s) not exist\n", filepath);
        return -1;
    }

    int pid = fork();

    if(pid == 0) {
        int ret = execv(filepath, args);

        // We shouldn't be here...
        if(ret < 0) {
            ALOGE("execv '%s' fail exit, error=%s\n", filepath, strerror(errno));
            _exit(1);
        }
    } else if(pid > 0) {
        ALOGI("Main thread will exit successfully");
        *cid = pid;
        return 0;
    } else if(pid < 0) {
        ALOGE("fork failed, error=%s", strerror(errno));
        return -1;
    }
    return -1;
}

/*
* purpose
*     fork a new process
*
* requires
*
* guarantees
*       return 0  success
*/
int exe_cmd(callback cb, exec_cmd_t * exec_cmd) {
    int pipefd[2];
    int cpid, i = 0, j = 0, ret = -1;
    char line[SIZE_1K] = { 0 };

    if(exec_cmd == NULL) {
        ALOGE("input cmd is NULL");
        return -1;
    }

    if(pipe(pipefd) == -1) {
        ALOGE("create pipe file fail, error=%s\n", strerror(errno));
        return -1;
    }

    cpid = fork();
    if(cpid == -1) {
        ALOGE("fork fail, error=%s\n", strerror(errno));
        return -1;
    } else if(cpid == 0) {      /* Child stdout from pipe */
        close(pipefd[0]);

        if(dup2(pipefd[1], STDOUT_FILENO) == -1) {
            ALOGE("fail to redirect std output and exit, error=%s\n", strerror(errno));
            _exit(100);
        }

        if(dup2(pipefd[1], STDERR_FILENO) == -1) {
            ALOGE("fail to redirect std err and exit, error=%s\n", strerror(errno));
            _exit(100);
        }

        ALOGI("start exec command:'%s', pid=%d\n", exec_cmd->cmd, getpid());
        ret = execv(exec_cmd->cmd, exec_cmd->params);
        if(ret == -1) {
            ALOGE("execv command(%s) fail and exit, error=%s\n", exec_cmd->cmd, strerror(errno));
            _exit(100);
        }
    } else {                    /* Parent read */
        signal(SIGCHLD, SIG_IGN);
        *(exec_cmd->pid) = cpid;
        close(pipefd[1]);
        while((read(pipefd[0], &line[i], 1) > 0) && (j < exec_cmd->size)) {
            if(i < SIZE_1K - 1 && line[i++] == '\n') {
                /**ignore space line*/
                if(strlen(line) > 1)
                    strlcat(exec_cmd->result, line, exec_cmd->size);

                if(cb != NULL)
                    cb(exec_cmd->result, strlen(exec_cmd->result));

                /**Check if need to force exit*/
                if(exec_cmd->exit_str != NULL && strstr(line, exec_cmd->exit_str) != NULL) {
                    ALOGI("'%s' need to force exit\n", exec_cmd->cmd);
                    break;
                }

                memset(line, 0, sizeof(line));
                i = 0;
            }
            j++;
        }

        ALOGI("kill command '%s'(pid=%d)\n", exec_cmd->cmd, cpid);
        kill(cpid, SIGTERM);

        do {
            ret = waitpid(cpid, NULL, 0);
            if(cpid == ret) {
                ALOGI("'%s'(pid=%d) exit\n", exec_cmd->cmd, cpid);
            }
        } while(ret == -1 && errno == EINTR);

        close(pipefd[0]);
    }

    return 0;
}

void signal_handler(int signal) {
    ALOGI("thread(thread id=%lu) received singal(%d) exit\n", pthread_self(), signal);
    pthread_exit(NULL);
}

bool is_proc_exist(int pid) {
    return kill(pid, 0) == 0;
}

void kill_thread(pthread_t tid) {

    if(tid > 0) {
        int ret = pthread_kill(tid, 0);

        if(ret == ESRCH)
            ALOGI("No thread with the ID: ", (unsigned int) tid);
        else if(ret == EINVAL)
            ALOGI("An invalid signal was specified ");
        else {
            pthread_kill(tid, SIGUSR1);
            pthread_join(tid, NULL);
            ALOGI("thread(thread id=%lu) be killed\n", tid);
        }
    }
}

void kill_proc(int pid) {
    int stat;

    if(pid > 0 && is_proc_exist(pid)) {
        kill(pid, SIGKILL);
        waitpid(pid, &stat, 0);
        if(WIFSIGNALED(stat))
            ALOGI("Child process(pid=%d) received signal(%d) and exit\n", pid, WTERMSIG(stat));

    }
}
