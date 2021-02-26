/******************************************************************************
  @file    file_playback.c
  @brief   Android performance iop library contain playback sourcecode

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/



#include "dblayer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <sys/mman.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <sys/syscall.h>
//#include <linux/ioprio.h>
#include <errno.h>

#undef ATRACE_TAG
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>
#define MAX_TOTAL_DATA (500*1024*1024) // 500MBMB
#define MAX_NUM_FILE 300

#undef LOG_TAG
#define LOG_TAG           "ANDR-IOP_PB"

#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#endif

#define FILE_NAME_MAX_LEN 4096

static pthread_t start_playback_pthread_id = 0;
int total_data = 0;
/* Run playback operation */
static void *iop_fileplayback_run(file_details info, struct stat file_stat, int fd)
{
    int page_itrr;
    int total_mincore_pages = (info.file_iop_size+PAGE_SIZE-1)/PAGE_SIZE;
    int total_pages = (file_stat.st_size+PAGE_SIZE-1)/PAGE_SIZE;
    char *mincore_array = info.mincore_array;
    char trace_buf[1024];
    if (total_mincore_pages > 0) {
        snprintf(trace_buf, sizeof(trace_buf), "%s read size:%.4fMB",
                                             info.file_name,
                                             total_mincore_pages*4/1024.0);
    }
    else
    {
        snprintf(trace_buf, sizeof(trace_buf), "%s read size:%.4fMB",
                    info.file_name,file_stat.st_size/1024.0/1024);
    }
    ATRACE_BEGIN(trace_buf);
    int count_pages=0;
    int read_data = 0;
    int ret = 0;
    QLOGI("MINCORE, PLAYBACK %s START, mincore_array: %p",
                info.file_name, mincore_array);
    //load each page by posix_fadvise here to avoid read_ahead
    //and decrease CPU usage of copy io content result to local viriable
    if (mincore_array == NULL) {
        //if all file need read to cache
        int len_size = file_stat.st_size;
        ret = posix_fadvise(fd, 0, len_size, POSIX_FADV_WILLNEED);
        read_data += len_size;
        if (ret !=0) {
            QLOGE("posix_fadvise failed: %s", strerror(errno));
        }
    } else {//load file content based on mincore result
        count_pages=0;
        for(page_itrr = 0; page_itrr < total_pages; page_itrr++) {
            if (mincore_array[page_itrr] & 0x1) {
                if (count_pages>=total_mincore_pages)
                    break;
                ret=posix_fadvise(fd, page_itrr*PAGE_SIZE, PAGE_SIZE, POSIX_FADV_WILLNEED);
                if (ret !=0) {
                    QLOGE("posix_fadvise failed: %s", strerror(errno));
                }
                read_data += PAGE_SIZE;
                count_pages++;
            }
        }
    }
    //after finish preload, mark file cache advise as normal
    ret = posix_fadvise(fd, 0, file_stat.st_size, POSIX_FADV_NORMAL);
    if (ret !=0) {
        QLOGE("posix_fadvise failed: %s", strerror(errno));
    }
    close(fd);
    total_data += read_data;
    QLOGI("MINCORE, PLAYBACK %s END real read file size = %.4fMB",
                                      info.file_name, read_data/1024.0/1024);
    ATRACE_END();

    return NULL;
}

/*
 * Drop cache of a specific file by fadvise syscall
 */
static void drop_file_cache(int fd, file_details *info)
{
    int len_of_range = info->filesize;
    int offset = 0;
    if (posix_fadvise(fd, offset, len_of_range, POSIX_FADV_DONTNEED)) {
        QLOGE("ERROR error not able to dorp file cache of file %s", info->file_name);
    }
}

static void* start_playback_thread(void *pkg_name_arg)
{
    file_details *file_detail_ptr;
    int num_file = 0;
    int num_of_files = 0;
    char *pkg_name = (char *)pkg_name_arg;
    int i = 0;
    total_data = 0;
    struct stat file_stat;
    int pagesize = sysconf(_SC_PAGESIZE);
    ATRACE_BEGIN("start_playback_thread: Enter");

    QLOGI("pkg_name = %s",pkg_name);
    num_file = get_total_file(pkg_name);

    if( num_file <= 0)
    {
        QLOGI("no file to read get_total_file %d", num_file);
        remove_pkg(pkg_name);
        free(pkg_name);
        ATRACE_END();
        return NULL;
    }

    file_detail_ptr = (file_details *) malloc(sizeof(file_details) * num_file);
    if (NULL == file_detail_ptr)
    {
        QLOGI("Fail to allocate memory");
        remove_pkg(pkg_name);
        free(pkg_name);
        ATRACE_END();
        return NULL;
    }

    num_of_files = get_file_list(pkg_name,file_detail_ptr,num_file);
    QLOGI("num_of_files = %d",num_of_files);

    if (num_of_files <= 0)
    {
        QLOGI("no file to read get_file_list");
        remove_pkg(pkg_name);
        free(file_detail_ptr);
        free(pkg_name);
        ATRACE_END();
        return NULL;
    }

    if( num_file > MAX_NUM_FILE)
    {
        //free mincore array that exceed MAX_NUM_FILE
        for (i=MAX_NUM_FILE; i<num_of_files; i++) {
            if (file_detail_ptr[i].mincore_array)
            {
                free(file_detail_ptr[i].mincore_array);
                file_detail_ptr[i].mincore_array=NULL;
            }
        }
        num_file = MAX_NUM_FILE;
    }

    for(i = 0; i < num_file;i++)
    {
        int fd = -1;
        char trace_buf[4096];
        if (file_detail_ptr[i].disabled) {
            QLOGE("skip preload disabled file %d %s", i, file_detail_ptr[i].file_name);
            continue;
        }
        if (file_detail_ptr[i].study_finish && file_detail_ptr[i].file_iop_size < pagesize) {
            file_detail_ptr[i].cache_dropped = NEED_MARK_CACHE_NOT_DROPPED;
            QLOGE("skip preload ZERO needed file %d %s", i, file_detail_ptr[i].file_name);
            continue;
        }
        fd = open(file_detail_ptr[i].file_name, O_RDONLY);
        if(fd == -1)
        {
            mark_for_delete(pkg_name,file_detail_ptr[i].file_name);
            continue;
        }
        if ( fstat( fd, &file_stat ) < 0 )
        {
            QLOGI("fail to get file stat");
            mark_for_delete(pkg_name,file_detail_ptr[i].file_name);
            close(fd);
            continue;// goto  next file
        }
        if ( file_stat.st_size == 0 )
        {
            // File zie is zero
            mark_for_delete(pkg_name,file_detail_ptr[i].file_name);
            close(fd);
            continue;// goto  next file
        }
        snprintf(trace_buf,4096,"Opening file %s size = %d",file_detail_ptr[i].file_name
                               , (int)file_stat.st_size);
        ATRACE_BEGIN(trace_buf);
        if (file_stat.st_mtime != file_detail_ptr[i].file_modify_time) {
            if (strstr(file_detail_ptr[i].file_name,".apk")!=NULL
                    && (strstr(file_detail_ptr[i].file_name,"/data/app/")!=NULL ||
                    strstr(file_detail_ptr[i].file_name, "/system/app/")!=NULL ||
                    strstr(file_detail_ptr[i].file_name, "/system/priv-app/")!=NULL))
                {
                    //readonly /data/app/base.apk
                    QLOGI("apk file %s modify time changed %d/%ld, \
                              remove files belong to pkg, and restudy",
                              file_detail_ptr[i].file_name,
                              file_detail_ptr[i].file_modify_time, file_stat.st_mtime);
                    snprintf(trace_buf, sizeof(trace_buf), "apk file %s upgrade, remove pkg"
                                                         , file_detail_ptr[i].file_name);
                    ATRACE_BEGIN(trace_buf);
                    file_detail_ptr[i].mark_for_delete = NEED_MARK_FOR_DELETE_PKG;
                    close(fd);
                    ATRACE_END();

                    ATRACE_END(); // end for opening file
                    ATRACE_END(); //end for start_playback_thread enter
                    return NULL;
                } else {
                    QLOGI("file %s modify time changed update db", file_detail_ptr[i].file_name);
                    //mark study_finish=0 and cache_dropped=1
                    snprintf(trace_buf, sizeof(trace_buf), "file %s modifie mark update"
                                                         , file_detail_ptr[i].file_name);
                    ATRACE_BEGIN(trace_buf);
                    //restudy if file modify time changed for .odex and .dex file
                    if (strstr(file_detail_ptr[i].file_name, ".odex") != NULL
                        || strstr(file_detail_ptr[i].file_name, ".dex") != NULL
                        || strstr(file_detail_ptr[i].file_name, ".vdex") != NULL)
                    {
                        file_detail_ptr[i].mark_for_delete = NEED_MARK_FOR_UPDATE;
                        file_detail_ptr[i].file_modify_time = file_stat.st_mtime;
                        file_detail_ptr[i].filesize = file_stat.st_size;
                        drop_file_cache(fd, &(file_detail_ptr[i]));
                    } else {//disable unstable file
                        file_detail_ptr[i].mark_for_delete = NEED_MARK_FOR_DISABLE;
                        file_detail_ptr[i].disabled = 1;
                    }
                ATRACE_END();
            }
            close(fd);
            continue;
        }
        //try if need drop cache or playback
        if (file_detail_ptr[i].study_finish == 0) {
            QLOGI("Drop cache of file %s", file_detail_ptr[i].file_name);
            snprintf(trace_buf, sizeof(trace_buf), "Drop cache of file %s",
                    file_detail_ptr[i].file_name);
            ATRACE_BEGIN(trace_buf);
            drop_file_cache(fd, &(file_detail_ptr[i]));
            file_detail_ptr[i].cache_dropped = NEED_MARK_CACHE_DROPPED;
            ATRACE_END();
        } else {
            QLOGI("io preload of file %s", file_detail_ptr[i].file_name);
            snprintf(trace_buf, sizeof(trace_buf), "add file %s %.2f / %.2f MB to task",
                    file_detail_ptr[i].file_name,
                    file_detail_ptr[i].file_iop_size/1024.0/1024,
                    file_detail_ptr[i].filesize/1024.0/1024);
            ATRACE_BEGIN(trace_buf);
            iop_fileplayback_run(file_detail_ptr[i],file_stat,fd);
            ATRACE_END();
            if (file_detail_ptr[i].cache_dropped) {
                file_detail_ptr[i].cache_dropped = NEED_MARK_CACHE_NOT_DROPPED;
            }
        }
        close(fd);
        if(total_data > MAX_TOTAL_DATA)
        {
            QLOGI("Max total size limit reached Total = %d",total_data);
            //STOP reading and max total reached
            break;
        }
        ATRACE_END(); // end for opening file
    }
    commit_filelist_info(file_detail_ptr, num_file);
    delete_mark_files();

    ATRACE_END(); //end for start_playback_thread enter
    QLOGI(" Total Data = %.2f MB", total_data/1024.0/1024);
    for (i=0; i<num_of_files; i++) {
        if (file_detail_ptr[i].mincore_array) {
            free(file_detail_ptr[i].mincore_array);
            file_detail_ptr[i].mincore_array=NULL;
        }
    }
    free(pkg_name);
    free(file_detail_ptr);
    return NULL;
}

void start_playback(char *pkg_name_arg) {
    char *pkg_name = NULL;
    int len = 0;
    QLOGE(" start_playback-1 ");

    if(NULL == pkg_name_arg)
        return;

    len = strlen(pkg_name_arg);
    if(len <= 0)
    {
        QLOGW("Incorrect length for pkg_name_arg");
        return;
    }

    pkg_name = (char *) malloc(len + 1);
    if (NULL == pkg_name)
        return;

    strlcpy(pkg_name, pkg_name_arg,len+1);

    QLOGI(" %s %zd\n", pkg_name,strlen(pkg_name));

    if(pthread_create(&start_playback_pthread_id, NULL, start_playback_thread, pkg_name)) {
         return ;
    }
    QLOGI("Exit playback");
    return ;
}

void stop_playback()
{
    QLOGI("Stop Playback ENTER");
    pthread_join(start_playback_pthread_id, (void **)NULL);
    QLOGI("Stop Playback EXIT");
}
