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
#include <pthread.h>
#include <errno.h>
#include "seccamlib.h"

pthread_t hlos_rcv_thread;

// If the Frame processing time is high, it is expected that the frame is copied to another buffer.
mlvmapp_error_t process_frame(const mlvmapp_frame_info_t* frame_info,
        uint8_t *camera_buffer, uint32_t cam_buf_size, const mlvmapp_buffer_info_t* cam_buf_info,
        const void* custom_req, uint32_t custom_req_size,
        void* custom_rsp, uint32_t custom_rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    int i= 0;
    while(i++ < 10){
        printf("%c ",*(camera_buffer+i));
    }

    printf("\nframe_event: cam_id:%u, cbuf:%p, cbuf_size: %d\n",
           frame_info->cam_id, camera_buffer, cam_buf_size);

    return ret;
}

// Called on receiving INIT cmd from HLOS
mlvmapp_error_t init_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    printf("App Init received \n");

    return ret;
}

// Called on receiving DEINIT cmd from HLOS
mlvmapp_error_t deinit_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_DEINIT_COMMAND;

    printf("App Deinit received \n");

    return ret;
}

// Called on receiving STOP cmd from HLOS
mlvmapp_error_t stop_cmd_handler(void *rcv, uint32_t rcv_size, void *rsp, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;
    mlvmapp_generic_rsp_t *rsp_buf = rsp;

    //Add cleanup code above, i.e before notify_hyp_done
    if(notify_hyp_done() != MLVM_APP_E_SUCCESS){
        rsp_buf->ret = MLVM_APP_E_SUCCESS;
        printf("Failed to send Done notification to hyp \n");
    } else {
        rsp_buf->ret = MLVM_APP_E_SUCCESS;
        printf("%s: Stop success\n",__func__);
    }
    *rsp_size = sizeof(*rsp_buf);
    ret = rsp_buf->ret;

    return ret;
}

// Called on receiving STOP cmd from HLOS
mlvmapp_error_t start_cmd_handler(void *rcv, uint32_t rcv_size, void *rsp, uint32_t *rsp_size)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;
    mlvmapp_generic_rsp_t *rsp_buf = rsp;
    printf("%s -Enter\n",__func__);

    //Add  code above, i.e before notify_hyp_done
    if(wait_for_hyp_notification() != MLVM_APP_E_SUCCESS){
        rsp_buf->ret = MLVM_APP_E_SUCCESS;
        printf("Failed to receive start/open scbuff \n");
    } else {
        rsp_buf->ret = MLVM_APP_E_SUCCESS;
        printf("%s: Start success\n",__func__);
    }
    *rsp_size = sizeof(*rsp_buf);
    ret = rsp_buf->ret;

    return ret;
}

mlvmapp_error_t MLVM_app_receive_cmds(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    wait_for_hlos_app_init_cmd();
    ret = pthread_create(&hlos_rcv_thread, NULL, hlos_rcv_cmds, NULL);
        if(ret){
            printf("failed to start pthread hlos_rcv\n");
            return ret;
        }
    pthread_join(hlos_rcv_thread, NULL);

    return ret;
}

mlvmapp_error_t MLVM_app_shutdown(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    ret = sock_deinit();

    return ret;
}

mlvmapp_error_t MLVM_app_start(void)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    //Establish socket communication with HLOS
    ret = sock_init();
    if (ret != 0){
        printf("%s: sock_init failed %d\n",__func__,ret);
        return ret;
    }
    return ret;
}

int main(int argc, char const **argv)
{
    mlvmapp_error_t ret = MLVM_APP_E_SUCCESS;

    ret = MLVM_app_start();
    if (ret != MLVM_APP_E_SUCCESS){
        printf("MLVM_app_start failed %d\n",ret);
        return 0;
    }

    ret = MLVM_app_receive_cmds();
    if (ret != MLVM_APP_E_SUCCESS){
        printf("MLVM_app_receive_cmds failed %d\n",ret);
        return 0;
    }

    ret = MLVM_app_shutdown();
    if (ret != MLVM_APP_E_SUCCESS){
        printf("MLVM_app_shutdown failed %d\n",ret);
        return 0;
    }
    return ret;
}
