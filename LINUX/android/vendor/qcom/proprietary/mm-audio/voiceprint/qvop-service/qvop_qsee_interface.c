/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <stdint.h>
#include <string.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <stddef.h>
#include <time.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>

//#include <errno.h>
#include "QSEEComAPI.h"

#include "qvop_qsee_interface.h"
//#include "vop_hlos_tz_interface.h"
#if !defined(_MSC_VER) && !defined(_ANDROID_)
#include <dirent.h>
#include "common_log.h"
#include <sys/mman.h>
#include <getopt.h>
#include <linux/msm_ion.h>
#include <utils/Log.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>


#else
#define //LOGD printf
#define //LOGE printf
#define __func__ ""
#endif

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "QVOP_TEST "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable //LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV
#define NON_LEGACY_CMD_LINE_ARGS 1


#define RUN_TESTS 0


#define __64KB__ 0x10000
#define __32KB__ 0x8000
#define __16KB__ 0x4000
#define __8KB__ 0x2000
#define __4KB__ 0x1000
#define __2KB__ 0x800
#define __1KB__ 0x400

/* Error code: status sent as response to command from sample client*/

#define SUCCESS 0
#define FAILED -1

#define MAX_APP_NAME 25
static char *appname;



int32_t qvop_issue_send_cmd_req(struct QSEECom_handle *l_QSEEComHandle,
                                         void *send_cmd , uint32_t cmd_size, void *resP_ptr);
#if !defined(_MSC_VER) && !defined(_ANDROID_)
struct qsc_ion_info {
    int32_t ion_fd;
    int32_t ifd_data_fd;
    struct ion_handle_data ion_alloc_handle;
    unsigned char * ion_sbuffer;
    uint32_t sbuf_len;
};

static int32_t qsc_ION_memalloc(struct qsc_ion_info *handle,
                                uint32_t size)
{
    int32_t ret = 0;
    int32_t iret = 0;
    int32_t fd = 0;
    unsigned char *v_addr;
    struct ion_allocation_data ion_alloc_data;
    int32_t ion_fd;
    int32_t rc;
    struct ion_fd_data ifd_data;
    struct ion_handle_data handle_data;

    /* open ION device for memory management
     * O_DSYNC -> uncached memory
    */

    if(handle == NULL){
      ////LOGE("Error:: null handle received");
      return -1;
    }

    ion_fd  = open("/dev/ion", O_RDONLY);

    if (ion_fd < 0) {
        ////LOGE("Error::Cannot open ION device\n");
        printf("%s: Error:: Cannot open ION device\n", __func__);
        return -1;
    }

    handle->ion_sbuffer = NULL;
    handle->ifd_data_fd = 0;

    /* Size of allocation */
    ion_alloc_data.len = (size + 4095) & (~4095);

    /* 4K aligned */
    ion_alloc_data.align = 4096;

    /* memory is allocated from EBI heap */
    ion_alloc_data.heap_id_mask= ION_HEAP(ION_QSECOM_HEAP_ID);

    /* Set the memory to be uncached */
    ion_alloc_data.flags = 0;

    /* IOCTL call to ION for memory request */
    rc = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);

    if (rc) {
       //LOGE("Error::Error while trying to allocate data\n");
       goto alloc_fail;
    }

    if (0 != ion_alloc_data.handle) {
       ifd_data.handle = ion_alloc_data.handle;
    } else {
       //LOGE("Error::ION alloc data returned a NULL\n");
       goto alloc_fail;
    }

    /* Call MAP ioctl to retrieve the ifd_data.fd file descriptor */
    rc = ioctl(ion_fd, ION_IOC_MAP, &ifd_data);

    if (rc) {
       //LOGE("Error::Failed doing ION_IOC_MAP call\n");
       goto ioctl_fail;
    }

    /* Make the ion mmap call */
    v_addr = (unsigned char *)mmap(NULL, ion_alloc_data.len,
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, ifd_data.fd, 0);
    if (v_addr == MAP_FAILED) {
       //LOGE("Error::ION MMAP failed\n");
       ret = -1;
       goto map_fail;
    }

    handle->ion_fd = ion_fd;
    handle->ifd_data_fd = ifd_data.fd;
    handle->ion_sbuffer = v_addr;
    handle->ion_alloc_handle.handle = ion_alloc_data.handle;
    handle->sbuf_len = size;
    return ret;

map_fail:
    if(handle->ion_sbuffer != NULL)
    {
        ret = munmap(handle->ion_sbuffer, ion_alloc_data.len);
        if (ret) {
            //LOGE("Error::Failed to unmap memory for load image. ret = %d\n",
            //     ret);
        }
    }

ioctl_fail:
    handle_data.handle = ion_alloc_data.handle;

    if (handle->ifd_data_fd) {
        close(handle->ifd_data_fd);
    }

    iret = ioctl(ion_fd, ION_IOC_FREE, &handle_data);

    if (iret) {
        //LOGE("Error::ION FREE ioctl returned error = %d\n",iret);
    }

alloc_fail:
    if (ion_fd) {
        close(ion_fd);
    }
    return ret;

}

/** @brief: Deallocate ION memory
 *
 *
 */
static int32_t qsc_ion_dealloc(struct qsc_ion_info *handle)
{
    struct ion_handle_data handle_data;
    int32_t ret = 0;

    /* Deallocate the memory for the listener */
    ret = munmap(handle->ion_sbuffer,
                (handle->sbuf_len + 4095) & (~4095));
    if (ret) {
      //LOGE("Error::Unmapping ION Buffer failed with ret = %d\n",
       //    ret);
    }

    handle_data.handle = handle->ion_alloc_handle.handle;
    close(handle->ifd_data_fd);
    ret = ioctl(handle->ion_fd, ION_IOC_FREE, &handle_data);
    if (ret){
        //LOGE("Error::ION Memory FREE ioctl failed with ret = %d\n",
         //    ret);
    }
    close(handle->ion_fd);
    return ret;
}

#endif

/**@brief:  Implement simple application start
 *
 * @param[in/out]    handle.
 * @param[in]        appname.
 * @param[in]        buffer size.
 * @return    zero on success or error count on failure.
 */
int32_t qsc_start_app(struct QSEECom_handle **l_QSEEComHandle,
                      const char *appname, int32_t buf_size)
{
    int32_t ret = 0;

    /* start the application */
    if (l_QSEEComHandle == NULL)
    {
        //LOGE("Null handle error in StartApp");
        return 1;
    }

    ret = QSEECom_start_app(l_QSEEComHandle, "/firmware/image",
                            appname, buf_size);
    if (ret) {
       //LOGE("Loading app -%s failed",appname);
    } else {
       //LOGD("Loading app -%s succeded",appname);
       QSEECom_set_bandwidth(*l_QSEEComHandle, true);
    }

    return ret;
}

/**@brief:  Implement simple shutdown app
 * @param[in]    handle.
 * @return    zero on success or error count on failure.
 */
int32_t qsc_shutdown_app(struct QSEECom_handle **l_QSEEComHandle)
{
    int32_t ret = 0;

    if (l_QSEEComHandle == NULL) {
        return 1;
    }

    //LOGD("qsc_shutdown_app: start");

    /* shutdown the application */
    if (*l_QSEEComHandle != NULL) {
       ret = QSEECom_shutdown_app(l_QSEEComHandle);
       if (ret) {
          //LOGE("Shutdown app failed with ret = %d", ret);
       } else {
          //LOGD("shutdown app: pass");
       }
    } else {
        //LOGE("cannot shutdown as the handle is NULL");
        printf("%s:cannot shutdown as the handle is NULL\n",__func__);
    }
    return ret;
}

void vop_log_time(uint32_t msg_id, uint32_t start_time_sec, unsigned long long start_time_nsec, uint32_t end_time_sec, unsigned long long end_time_nsec)
{
    static FILE *fp;
    if (fp == NULL)
        fp = fopen("/data/vop_profile.csv", "w");
    if (fp)
        fprintf(fp, "msg id, %u, start_sec, %u, start_nsec, %llu ,end_sec, %u ,end_nsec, %llu\n", msg_id, start_time_sec, start_time_nsec, \
            end_time_sec, end_time_nsec);
}

/**@brief:  Exercise send command
 * @param[in]    handle.
 * @param[in]    data to be send to secure app.
 * @return    zero on success or error count on failure.
 */

int32_t qvop_issue_send_cmd_req(struct QSEECom_handle *l_QSEEComHandle,
                                         void *send_cmd , uint32_t cmd_size, void *resp_msg)
{
    int32_t ret = 0;
    int32_t req_len = 0;
    int32_t rsp_len = 0;
#if defined(_ANDROID_) && defined(QVOP_DEBUG)
    struct timespec start_time, end_time;
#endif
    void *msgreq;    /* request data sent to QSEE */
    void *msgrsp;    /* response data sent from QSEE */

    //LOGD("send cmd: start");

    if (l_QSEEComHandle == NULL) {
        return 1;
    }

    /* populate the data in shared buffer */
    msgreq=(void *)l_QSEEComHandle->ion_sbuffer;

    //TBD copy cmd to ion buffer
    memcpy(msgreq,send_cmd,cmd_size);
    //LOGD("Command ready to be send to TZ.");


    req_len = cmd_size;
    rsp_len = MAX_RESP_BUFF_SIZE;

    if (req_len & QSEECOM_ALIGN_MASK) {
        req_len = QSEECOM_ALIGN(req_len);
    }

    if (rsp_len & QSEECOM_ALIGN_MASK) {
        rsp_len = QSEECOM_ALIGN(rsp_len);
    }

    msgrsp= l_QSEEComHandle->ion_sbuffer + req_len;

    //LOGD("req len = %d bytes rsp len %d",req_len,rsp_len);
    //LOGD("CMD_ID = %d bytes",*((int *)msgreq));
    /* send request from HLOS to QSEApp */
#if defined(_ANDROID_) && defined(QVOP_DEBUG)
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif
    ret = QSEECom_send_cmd(l_QSEEComHandle,
                msgreq,
                req_len,
                msgrsp,
                rsp_len);
    if (ret) {
       //LOGE("send command failed with ret = %d\n", ret);
    }

    memcpy(resp_msg,msgrsp, MAX_RESP_BUFF_SIZE);
#if defined(_ANDROID_) && defined(QVOP_DEBUG)
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    vop_log_time(*((uint32_t *)msgreq), start_time.tv_sec, start_time.tv_nsec, end_time.tv_sec, end_time.tv_nsec);
#endif
    return ret;

}
