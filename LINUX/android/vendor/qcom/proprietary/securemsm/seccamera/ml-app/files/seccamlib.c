/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
=============================================================

                          EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ------------------------------------------
06/29/18   ac      Initial version
=============================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "seccamlib.h"

#define MAX_PAYLOAD_SIZE 512
//#define COG_H 1

//TODO ADD LOGE LOGD macro
#if COG_H
#define SCBUFF_DEVICE_NODE "/dev/scbuf0"
#endif
//sock params
guest_socket socket_handle;
guest_sockaddr sockaddr;
int g_init_status = 0, g_session = 0;
int scbuf_fd = -1;
int mapped_buffers = 0;

int sock_wait_for_cmd(guest_socket socket_handle, guest_sockaddr *rcv_sockaddr,
                      guest_sockaddr_len_type *rcv_addrlen,
                      void *req, uint32_t req_size)
{
    int32_t bytes = 0;

    memset(rcv_sockaddr, 0, sizeof(guest_sockaddr));
    guest_socket_recvfrom(&socket_handle, req, req_size, rcv_sockaddr, &bytes);

    if(bytes <= 0)
        printf("recvfrom failed %d %s-%d\n", bytes, strerror(errno), errno);

    return bytes;
}

int sock_send_response(guest_socket socket_handle, guest_sockaddr *rsp_sockaddr,
                       guest_sockaddr_len_type *rsp_addrlen,
                       void *rsp, uint32_t rsp_size)
{
    int32_t bytes = 0;

    guest_socket_sendto(&socket_handle, rsp, rsp_size, rsp_sockaddr, &bytes);

    return bytes;
}

mlvmapp_error_t sock_init()
{
    mlvmapp_error_t ret = MLVM_APP_E_SOCK;
    printf("sock_init -ENTER\n");
    do{
        //TODO Need to get unique SEVICE_ID
        guest_socket_set_addr_by_name(&sockaddr, MLAPP_SERVER_SERVICE_ID, 1);
        if(guest_socket_open(&socket_handle) == SOCK_E_FAIL)
            break;
        if(guest_socket_bind(&socket_handle, &sockaddr) == SOCK_E_FAIL)
            break;
        ret = MLVM_APP_E_SUCCESS;
    }while(0);
    printf("sock_init ret =%d\n", ret);

    return ret;
}

mlvmapp_error_t sock_deinit()
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    printf("sock_deinit -ENTER\n");
    if(guest_socket_close(&socket_handle) == SOCK_E_FAIL)
    {
        printf("sock_deinit Failed\n");
        ret = MLVM_APP_E_SOCK;
    }
    printf("sock_deinit ret =%d\n", ret);

    return ret;
}
mlvmapp_error_t demoExchangeTimestamp(mlvmapp_if_timestamp_send_cmd_t *cmd, uint32_t cmd_size,
                                  mlvmapp_if_timestamp_send_cmd_rsp_t* rsp, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    if(sizeof(*cmd) < cmd_size){
        printf("demoExchangeTimestamp: cmd_size mismatch 0x%x bytes 0x%x\n",
               cmd_size, sizeof(*cmd));
        return MLVM_APP_E_FAIL;
    }
    printf("demoExchangeTimestamp: received time %lu\n",cmd->cmd_data);
    *rsp_size = sizeof(mlvmapp_if_timestamp_send_cmd_rsp_t);
    rsp->ret_data = time(NULL);
    printf("demoExchangeTimestamp: send time %lu\n",rsp->ret_data);

    return ret;
}

mlvmapp_error_t is_buffer_secure(uint32_t buff_hyp_handle, int32_t* buff_fd)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

#if COG_H
    *buff_fd = ioctl(scbuf_fd, IOCTL_COG_SCBUF_LOOKUP_HANDLE,
                     (unsigned)buff_hyp_handle);
    if(*buff_fd < 0){
        printf("IOCTL request to check frame buffer %x failed %08X:(%s)\n",
               buff_hyp_handle, errno, strerror(errno));
        return MLVM_APP_E_FAIL;
    }
    printf("IOCTL received buffer_fd [%x %x] ret=%d \n", buff_hyp_handle,
                                                        *buff_fd, ret);
#endif
    return ret;
}

static mlvmapp_error_t frame_notification_handler(mlvmapp_frame_cmd_req_t *cmd, uint32_t cmd_size,
						mlvmapp_frame_cmd_rsp_t* rsp, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;
    uint32_t skipped_cameras = 0;
    void* custom_req = NULL;
    uint32_t custom_req_size = 0;
    void* custom_rsp = NULL;
    uint32_t custom_rsp_size = 0;
    int32_t capture_buff_handle = -1;
    uint32_t capture_buff_size = 0;
    int32_t buff_fd = -1;
    uint8_t *capture_buff_ipa = NULL;
    uint64_t time_temp = time(NULL);
    *rsp_size = sizeof(*rsp);

    //check for invalid size
    if (sizeof(*cmd) > cmd_size) {
        printf("%s: invalid input\n", __func__);
        ret = MLVM_APP_E_INVALID_PARAM;
        goto ERROR;
    }
    capture_buff_handle = cmd->frame_data[0].in_buffer.buffer_handle;
    capture_buff_size = cmd->frame_data[0].in_buffer.size;

    //Prepare and validate custom request structure
    if (cmd->custom_req_offset != 0 && cmd->custom_req_size != 0) {
        //check that the start of custom region is after the end of the mlvmapp_frame_cmd_req_t struct
        if (cmd->custom_req_offset < sizeof(mlvmapp_frame_cmd_req_t) ||
            (uint64_t)cmd->custom_req_offset + cmd->custom_req_size > cmd_size)
        {
            printf("%s: invalid custom request offset or size\n", __func__);
            ret = MLVM_APP_E_INVALID_PARAM;
            goto ERROR;
        }

        custom_req = (void*)(((uint8_t*)cmd) + cmd->custom_req_offset);
        custom_req_size = cmd->custom_req_size;
    }

    //prepare and validate custom response structure
    if (cmd->custom_rsp_offset != 0 && cmd->custom_rsp_size != 0) {
        //check that the start of custom region is after the end of the seccam_frame_cmd_rsp_t struct
        if (cmd->custom_rsp_offset < sizeof(mlvmapp_frame_cmd_rsp_t))
        {
            printf("%s: invalid custom response offset or size\n", __func__);
            ret = MLVM_APP_E_INVALID_PARAM;
            goto ERROR;
        }

        custom_rsp = (void*)(((uint8_t*)rsp) + cmd->custom_rsp_offset);
        custom_rsp_size = cmd->custom_rsp_size;
        *rsp_size += custom_rsp_size;
    }

    //check physical buffs with hyp
    if(capture_buff_handle <0){
        printf("invalid handle %x\n",capture_buff_handle);
        goto ERROR;
    }
    if(is_buffer_secure(capture_buff_handle, &buff_fd) != 0){
        printf("check_secure_frame failed\n");
        ret = MLVM_APP_E_FAIL;
        goto ERROR;
    }
    //mmap the buffs
    capture_buff_ipa = mmap(NULL, capture_buff_size, PROT_READ|PROT_WRITE, MAP_SHARED, buff_fd, 0);
    if(capture_buff_ipa == MAP_FAILED){
        printf("mmap failed for buffer handle :%x\n",capture_buff_handle);
        ret = MLVM_APP_E_FAIL;
        goto ERROR;
    }
    printf("mmap buffer ipa :0x%p\n",capture_buff_ipa);
    //copy the frame to local struct
    mlvmapp_frame_info_t frame_info;
    mlvmapp_buffer_info_t cam_buf_info;

    frame_info.cam_id = cmd->frame_data[0].cam_id;
    frame_info.frame_number = cmd->frame_data[0].frame_number;
    frame_info.time_stamp = cmd->frame_data[0].time_stamp;

    cam_buf_info.width = cmd->frame_data[0].in_buffer.width;
    cam_buf_info.height = cmd->frame_data[0].in_buffer.height;
    cam_buf_info.stride = cmd->frame_data[0].in_buffer.stride;
    cam_buf_info.format = cmd->frame_data[0].in_buffer.format;
    //Pass frame to App frame handler
    ret = process_frame(&frame_info, capture_buff_ipa, capture_buff_size, &cam_buf_info,
                        custom_req, custom_req_size, custom_rsp, custom_req_size);

    //Unmap the buffer before exiting
    if(munmap(capture_buff_ipa, capture_buff_size)){
        printf("FAILED unmmap buffer ipa :0x%x\n",capture_buff_ipa);
        ret = MLVM_APP_E_FAIL;
    }
 
    if(close(buff_fd)){
        printf("FAILED to close buff_fd :0x%x %s\n",errno, strerror(errno));
        ret = MLVM_APP_E_FAIL;
    }
ERROR:
    //Fill response buffer
    rsp->ret = ret;
    rsp->rsp_data.duration = time(NULL) - time_temp;

    return ret;
}

mlvmapp_error_t processCommand(void *rcv, uint32_t rcv_size, void *rsp, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_INVALID_PARAM;

    mlvmapp_generic_req_t *gen_req = (mlvmapp_generic_req_t *)rcv;
    if(gen_req->cmd_id < MLVM_APP_CMD_NONE && gen_req->cmd_id > MLVM_APP_CMD_LAST){
        printf("Invalid cmd %d\n",gen_req->cmd_id);
        return ret;
    }
    switch(gen_req->cmd_id){
    case MLVM_APP_CMD_FRAME_NOTIFICATION:
        ret = frame_notification_handler(rcv, rcv_size, rsp, rsp_size);
        break;
    case MLVM_APP_CMD_START:
        ret = start_cmd_handler(rcv, rcv_size, rsp, rsp_size);
        break;
    case MLVM_APP_CMD_STOP:
        ret = stop_cmd_handler(rcv, rcv_size, rsp, rsp_size);
        break;
    case MLVM_APP_CMD_EXCHANGE_TIMESTAMP:
        ret = demoExchangeTimestamp(rcv, rcv_size, rsp, rsp_size);
        break;
    case MLVM_APP_CMD_HLOS_APP_DEINIT:
        ret = deinit_cmd_handler(rcv, rcv_size, rsp, rsp_size);;
        break;
    default:
        printf("no handler for cmd_id %d\n",gen_req->cmd_id);
        ret = MLVM_APP_E_UNKNOWN_COMMAND;
    }
    return ret;
}

void *hlos_rcv_cmds(void *arg)
{
    uint8_t rcv_buf[MAX_PAYLOAD_SIZE] = {0}, rsp_buf[MAX_PAYLOAD_SIZE] = {0};
    uint32_t rcv_size = 0, rsp_size = 0;
    guest_sockaddr rcv_sockaddr = {0};
    guest_sockaddr_len_type rcv_sockaddrlen = {0};

    while(1){
        if((rcv_size = sock_wait_for_cmd(socket_handle, &rcv_sockaddr,
                                         &rcv_sockaddrlen, rcv_buf, 
                                         sizeof(rcv_buf))) > 0){
            if(processCommand((void *)rcv_buf, rcv_size, (void *)rsp_buf,
                              &rsp_size) == MLVM_APP_E_DEINIT_COMMAND){
                break;
            }
            if(rsp_size > 0){
                if(sock_send_response(socket_handle, &rcv_sockaddr, 
                                      &rcv_sockaddrlen, rsp_buf, rsp_size) <= 0){
                    printf("send response failed\n");
                }else {
                    printf("send response passed %d\n",rsp_size);
                }
            }
        }else{
            printf("invalid size cmd received %dbytes\n", rcv_size);
        }
        rsp_size = 0;
    }
    return NULL;
}

mlvmapp_error_t wait_for_hlos_app_init_cmd(void)
{
    uint8_t rcv_buf[512], rsp_buf[512];
    uint32_t rcv_size = 0, rsp_size = 0;
    guest_sockaddr rcv_sockaddr;
    guest_sockaddr_len_type rcv_sockaddrlen;
    mlvmapp_generic_req_t *gen_req;
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;
    do{
        if((rcv_size = sock_wait_for_cmd(socket_handle, &rcv_sockaddr,
                                         &rcv_sockaddrlen, rcv_buf,
                                         sizeof(rcv_buf))) > 0){
            gen_req = (mlvmapp_generic_req_t *)rcv_buf;
            if(gen_req->cmd_id != MLVM_APP_CMD_HLOS_APP_INIT){
                printf("invalid command %d\n", gen_req->cmd_id);
                ret = MLVM_APP_E_UNKNOWN_COMMAND;
                break;
            }
            ret = init_cmd_handler(rcv_buf, rcv_size, rsp_buf, &rsp_size);
            if(rsp_size != 0)
                if(sock_send_response(socket_handle, &rcv_sockaddr,
                                      &rcv_sockaddrlen, rsp_buf, rsp_size) <= 0){
                    printf("Send response failed \n");
                }
        }else{
            printf("invalid command size buffer received\n");
            ret = MLVM_APP_E_UNKNOWN_COMMAND;
        }
    }while(0);
    return ret;
}

mlvmapp_error_t wait_for_hyp_notification(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_FAIL;
    struct pollfd fd;
    fd.events = POLLPRI;
    int timeout = -1;

    do{
        if(hyp_dev_open() != MLVM_APP_E_SUCCESS){
            printf("Failed to open SCBUFF_DEVICE_NODE \n");
            break;
        }
        printf("hyp_dev_open\n");
        fd.fd = scbuf_fd;
        if(poll(&fd, 1, timeout) <= 0){
            hyp_dev_close();
            printf("poll failed to receive event\n");
            break;
        }
        if(!(fd.revents & POLLPRI)){
            hyp_dev_close();
            printf("invalid event received %d\n",fd.revents);
            break;
        }
        printf("POLLPRI received %d\n",fd.revents);
        ret = MLVM_APP_E_SUCCESS;
    }while(0);

    return ret;
}

mlvmapp_error_t notify_hyp_done(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    if(hyp_dev_close() != MLVM_APP_E_SUCCESS){
        ret = MLVM_APP_E_FAIL;
    }
    printf("%s: Exit\n",__func__);

    return ret;
}

mlvmapp_error_t hyp_dev_open(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_FAIL;

#if COG_H
    if((scbuf_fd = open(SCBUFF_DEVICE_NODE, O_RDONLY)) < 0){
        printf("Failed to open SCBUFF_DEVICE_NODE:0x%x %s\n",errno, strerror(errno));
        return ret;
    }
#endif

    return MLVM_APP_E_SUCCESS;
}

mlvmapp_error_t hyp_dev_close(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

#if COG_H
    if(close(scbuf_fd) < 0){
        printf("Failed to close device node %s\n",SCBUFF_DEVICE_NODE);
        ret = MLVM_APP_E_FAIL;
    }
#endif
    printf("%s: Exit\n",__func__);

    return ret;
}
