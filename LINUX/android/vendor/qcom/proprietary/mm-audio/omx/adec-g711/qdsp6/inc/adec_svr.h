/*--------------------------------------------------------------------------
Copyright (c) 2010, 2014, 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------------*/
#ifndef ADEC_SVR_H
#define ADEC_SVR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <sched.h>

#ifdef _ANDROID_
#define LOG_TAG "QC_G711DEC"
#endif
#include "qc_omx_msg.h"

#ifdef DEBUG_PRINT
#undef DEBUG_PRINT
#define DEBUG_PRINT
#endif

/*comment below 3 lines to get logs*/
#define LOGV(a...) do { } while(0)
#define DEBUG_PRINT(a...) do { } while(0)
#define DEBUG_DETAIL(a...) do { } while(0)

/**
@brief audio decoder command server structure

This structure maintains the command
server context

*/
    typedef void (*message_func)(void* client_data, unsigned char id);

    struct g711_ipc_info
    {
        pthread_t    thr;
        int          pipe_in;
        int          pipe_out;
        int          dead;
        message_func process_msg_cb;
        void         *client_data;
        char         thread_name[128];
    };

    /**
@brief This function starts command server

@param cb pointer to callback function from the client
@param client_data reference client wants to get back
through callback
@return handle to command server
*/
    struct g711_ipc_info *omx_g711_thread_create(message_func cb,
    void* client_data,
    char *th_name);

    struct g711_ipc_info *omx_g711_event_thread_create(message_func cb,
    void* client_data,
    char *th_name);

    void *omx_g711_msg(void *info);

    void *omx_g711_events(void *info);

    /**
@brief This function stop command server

@param svr handle to command server
@return none
*/
    void omx_g711_thread_stop(struct g711_ipc_info *g711_ipc);


    /**
@brief This function post message in the command server

@param svr handle to command server
@return none
*/
    void omx_g711_post_msg(struct g711_ipc_info *g711_ipc, unsigned int id);

    void* omx_g711_comp_timer_handler(void *);
#ifdef __cplusplus
}
#endif

#endif /* ADEC_SVR */
