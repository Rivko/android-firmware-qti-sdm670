/*--------------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
--------------------------------------------------------------------------*/
#ifndef ADEC_SVR_H
#define ADEC_SVR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <sched.h>
#include "qc_omx_msg.h"

#ifdef _ANDROID_
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "QC_ALACDEC"
#endif
#endif

/*uncomment below line to get logs*/
/*#define VERBOSE_LOGGING*/
#ifndef VERBOSE_LOGGING
#define LOGV(a...) do { } while(0)
#define DEBUG_PRINT(a...) do { } while(0)
#define DEBUG_DETAIL(a...) do { } while(0)
#endif

/**
@brief audio decoder command server structure

This structure maintains the command
server context

*/
    typedef void (*message_func)(void* client_data, unsigned char id);

    struct alac_ipc_info
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
    struct alac_ipc_info *omx_alac_thread_create(message_func cb,
    void* client_data,
    char *th_name);

    struct alac_ipc_info *omx_alac_event_thread_create(message_func cb,
    void* client_data,
    char *th_name);

    void *omx_alac_msg(void *info);

    void *omx_alac_events(void *info);

    /**
@brief This function stop command server

@param svr handle to command server
@return none
*/
    void omx_alac_thread_stop(struct alac_ipc_info *alac_ipc);


    /**
@brief This function post message in the command server

@param svr handle to command server
@return none
*/
    void omx_alac_post_msg(struct alac_ipc_info *alac_ipc, unsigned int id);

    void* omx_alac_comp_timer_handler(void *);
#ifdef __cplusplus
}
#endif

#endif /* ADEC_SVR */
