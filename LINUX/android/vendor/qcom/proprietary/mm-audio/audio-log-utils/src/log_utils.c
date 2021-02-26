/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "audio_log_utils"
#define NO_OF_MODULES 3
//#define LOG_NDEBUG 0
#include "log_utils.h"
#include "log_xml_parser.h"

int hal_log_mask[HAL_MOD_FILE_MAX];
static char buf[MAX_MODULES][MAX_MODULE_NAME_LEN];
static int count = 0, counter = 0;

static pthread_t monitor_thread;
static int read_module(void);
static int parse_module_xml_file(void);
static void *monitor_mod_file_change(void *);

static volatile bool exit_monitor_thread = false;
static volatile int monitor_thread_wake_fd = -1;

static const char module_name[][MAX_MODULE_NAME_LEN] = {
    "hal",
    "apm",
    "pproc"
};

void register_for_dynamic_logging(char *mod)
{
    int i = 0;

    if (sizeof(mod) >= MAX_MODULE_NAME_LEN) {
        ALOGE(", error registering module, name too long!");
        return;
    }

    if (count >= MAX_MODULES) {
        ALOGE(", error registering module, no more free indexes!");
        return;
    }

    for (; i < count; i++) {
        if (!strncmp(buf[i], mod, sizeof(buf[i]))) {
            ALOGD("mod %s, already registered", mod);
            break;
        }
    }
    if (i == count) {
        strlcpy(buf[count], mod, MAX_MODULE_NAME_LEN);
        count++;
        ALOGD("registered, module %s", mod);
    }
}

void log_utils_init(void)
{
    int i;

    pthread_attr_t attr;
    for (i = 0; i < HAL_MOD_FILE_MAX; i++)
        hal_log_mask[i] = 0;

    read_module();
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&monitor_thread, &attr, monitor_mod_file_change, NULL);
    pthread_attr_destroy(&attr);
}

void log_utils_deinit(void) {
    ALOGD("%s", __func__);

    exit_monitor_thread = true;
    if (monitor_thread_wake_fd != -1) {
        int ret = write(monitor_thread_wake_fd, "W", 1);
        if (ret == 1)
            pthread_join(monitor_thread, NULL);
        else
            ALOGW("error writing on wake fd, thread not created!");
    }
}

int read_module(void) {
    ALOGD("%s", __func__);
    for (int counter = 0; counter < NO_OF_MODULES; counter++) {
        ALOGD("%s: calling parse module xml file", __func__);
        parse_module_xml_file();
        return 0;
    }
    ALOGD("%s: errno %d", __func__, errno);
    return -1;
}

static int parse_module_xml_file(void)
{
    int j = 0;
    while (j < count) {
        if (!strncmp(buf[j], module_name[0], sizeof(buf[j]))) {
            ALOGD("%s: parse %s log mask", __func__, module_name[0]);
            if (parse_log_xml(module_name[0], hal_log_mask, sizeof(hal_log_mask)/(sizeof(int))) != 0) {
                ALOGE("%s: could not parse %s logmask", __func__, module_name[0]);
            }
        }
        j++;
    }
    ALOGD("%s: exit", __func__);
    return 0;
}

static void *monitor_mod_file_change(void *context __unused)
{
    int fd = -1, wd = -1, nbytes = 0, retry_cnt = 0;
    struct inotify_event event;
    int wake_fds[2];
    int wake_pipe_read_fd = -1, wake_pipe_write_fd = -1, ret = 0;

    ALOGD("%s", __func__);
    ret = pipe(wake_fds);
    if (ret) {
        ALOGW("Could not create wake pipe.  errno=%d", errno);
        goto exit;
    }

    wake_pipe_read_fd = wake_fds[0];
    wake_pipe_write_fd = wake_fds[1];

    ret = fcntl(wake_pipe_read_fd, F_SETFL, O_NONBLOCK);
    if (!ret)
        ret = fcntl(wake_pipe_write_fd, F_SETFL, O_NONBLOCK);
    if (ret) {
        ALOGW("Could not make wake pipe fds non-blocking.  errno=%d", errno);
        goto exit;
    }

    monitor_thread_wake_fd = wake_pipe_write_fd;
    fd = inotify_init();
    /* watch module file for any activity and report it back */
    wd = inotify_add_watch(fd, LOG_XML_PATH, IN_MODIFY);
    struct pollfd pfd[2] = {{fd, POLLIN, 0 },
                            {wake_pipe_read_fd, POLLIN, 0}};
    while (!exit_monitor_thread) {
        /* Block until there is something to be read */
        if (poll(pfd, 2, -1) < 0) {
            ALOGE("Couldn't poll(): %s", strerror(errno));
            exit_monitor_thread = true;
        } else if (pfd[0].revents & POLLIN) {
            ALOGD("%s: read data", __func__);
            nbytes = read(fd, &event, sizeof(event));
            ALOGD("out of poll event with mask %d", (int)event.mask);
            if (event.mask & IN_MODIFY) {
                if ((read_module() < 0) && (retry_cnt++ > 10)) {
                    ALOGE("Could not read file");
                    exit_monitor_thread = true;
                }
            }
            pfd[0].revents = 0;
        }
    }

exit:
    ALOGD("%s: exit", __func__);
    inotify_rm_watch(fd, wd);
    monitor_thread_wake_fd = -1;
    if (fd != -1)
        close(fd);
    if (wake_pipe_read_fd != -1)
        close(wake_pipe_read_fd);
    if (wake_pipe_write_fd != -1)
        close(wake_pipe_write_fd);
    return 0;
}
