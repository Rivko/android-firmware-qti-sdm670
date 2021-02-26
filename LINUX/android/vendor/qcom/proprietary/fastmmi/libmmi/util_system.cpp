/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "utils.h"

bool check_file_exist(const char *path) {
    struct stat buffer;
    int err = stat(path, &buffer);

    if(err == 0)
        return true;
    if(errno == ENOENT) {
        MMI_ALOGE("file(%s) do not exist, error=%s\n", path, strerror(errno));
        return false;
    }
    return false;
}

int clear_file(const char *filepath) {
    FILE *fp = NULL;

    /*open as Write, and then close, mean clear file content */
    fp = fopen(filepath, "w");
    if(!fp) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", filepath, strerror(errno));
        return -1;
    }

    fclose(fp);
    return 0;
}

/**
 *   Delete file if size>limit
 *
 */
int delete_file(const char *file, int limit) {
    struct stat st;

    if(check_file_exist(file)) {
        if(stat(file, &st) == 0) {
            if(st.st_size >= limit) {
                remove(file);
                return 0;
            }
        }
    }

    return -1;
}


int read_file(const char *filepath, char *buf, int size) {
    int fd, len;

    fd = open(filepath, O_RDONLY);
    if(fd == -1) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", filepath, strerror(errno));
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

int write_file(const char *path, const char *content) {

    FILE *fp = fopen(path, "a");

    if(fp == NULL) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", path, strerror(errno));
        return -1;
    }

    fwrite(content, sizeof(char), strlen(content), fp);
    fclose(fp);

    return 0;
}

int copy_file(char *src, char *dest) {

    int fd_src, fd_dest;
    struct stat st;
    int length = 0;
    char buffer[SIZE_1K];
    char *p = buffer;
    int rlen = 0, wlen = 0, tmp = 0;

    if(src == NULL || dest == NULL) {
        MMI_ALOGE("NULL point\n");
        return -1;
    }
    fd_src = open(src, O_RDONLY, 0);
    if(fd_src == -1) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", src, strerror(errno));
        return -1;
    }

    fd_dest = open(dest, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(fd_dest == -1) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", dest, strerror(errno));
        close(fd_src);
        return -1;
    }

    fstat(fd_src, &st);
    length = st.st_size;

    while(length > 0) {
        rlen = read(fd_src, p, SIZE_1K);
        tmp = rlen;
        while(rlen > 0) {
            wlen = write(fd_dest, p, rlen);
            rlen -= wlen;
            p += wlen;
        }
        p = buffer;
        length -= tmp;
    }
    close(fd_src);
    close(fd_dest);
    return 0;

}

void *zmalloc(size_t n) {
    void *r = malloc(n);

    if(r != NULL) {
        memset(r, 0, n);
    } else {
        MMI_ALOGE("malloc fail, error=%s\n", strerror(errno));
    }
    return r;
}

int fork_daemon(const char *filepath, char **args, int *cid) {

    if(!check_file_exist(filepath)) {
        MMI_ALOGE("file(%s) not exist\n", filepath);
        return -1;
    }

    int pid = fork();

    if(pid == 0) {
        int ret = execv(filepath, args);

        // We shouldn't be here...
        if(ret < 0) {
            MMI_ALOGE("execv '%s' fail exit, error=%s\n", filepath, strerror(errno));
            exit(1);
        }
    } else if(pid > 0) {
        MMI_ALOGI("Main thread will exit successfully");
        *cid = pid;
        return 0;
    } else if(pid < 0) {
        MMI_ALOGE("fork failed, error=%s", strerror(errno));
        return -1;
    }
    return -1;
}

int find_pid_by_name(char *proc_name, int *found_pid) {
    DIR *dir;
    struct dirent *d;
    int pid, i;
    char *s;
    int pnlen;

    i = 0;
    found_pid[0] = 0;
    pnlen = strlen(proc_name);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if(!dir) {
        MMI_ALOGE("cannot open '/proc', error=%s", strerror(errno));
        return -1;
    }

    /* Walk through the directory. */
    while((d = readdir(dir)) != NULL) {
        char exe[PATH_MAX + 1];
        char path[PATH_MAX + 1];
        int len;
        int namelen;

        /* See if this is a process */
        if((pid = atoi(d->d_name)) == 0)
            continue;

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if((len = readlink(exe, path, PATH_MAX)) < 0)
            continue;
        path[len] = '\0';

        /* Find ProcName */
        s = strrchr(path, '/');
        if(s == NULL)
            continue;
        s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if(namelen < pnlen)
            continue;

        if(!strncmp(proc_name, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                found_pid[i] = pid;
                i++;
            }
        }
    }

    found_pid[i] = 0;
    closedir(dir);

    return 0;
}
/*
* purpose
*     safe add function
*
* requires
*
* guarantees
*       return -1 when interger overflow
*       return 0 in success
*/
int safe_overflow_add(int x, int y, int *result) {
    *result = 0;

    if((y > 0 && x > INT_MAX - y) || (y < 0 && x < INT_MIN - y)) {
        /* Oh no, overflow */
	MMI_ALOGE("Integer overflow here, please check!");
        return -1;
    }

    *result = x + y;
    return 0;
}
