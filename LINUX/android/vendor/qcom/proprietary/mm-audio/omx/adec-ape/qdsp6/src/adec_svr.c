/*--------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <adec_svr.h>
#include <string.h>
#include <errno.h>

/**
  @brief This function starts command server

  @param cb pointer to callback function from the client
  @param client_data reference client wants to get back
  through callback
  @return handle to msging thread
 */
struct ape_ipc_info *omx_ape_thread_create(
        message_func cb,
        void* client_data,
        char* th_name)
{
    int r;
    int fds[2];
    struct ape_ipc_info *ape_info;

    ape_info = calloc(1, sizeof(struct ape_ipc_info));
    if (!ape_info) return 0;

    ape_info->client_data = client_data;
    ape_info->process_msg_cb = cb;
    strlcpy(ape_info->thread_name,th_name, sizeof (ape_info->thread_name));

    if (pipe(fds)) {
        DEBUG_PRINT_ERROR("%s: pipe creation failed\n", __func__);
        goto fail_pipe;
    }

    ape_info->pipe_in = fds[0];
    ape_info->pipe_out = fds[1];


    r = pthread_create(&ape_info->thr, 0, omx_ape_msg, ape_info);
    if (r < 0) goto fail_thread;

    DEBUG_DETAIL("Created thread for %s", ape_info->thread_name);
    return ape_info;


fail_thread:
    close(ape_info->pipe_in);
    close(ape_info->pipe_out);

fail_pipe:
    free(ape_info);

    return 0;
}

/**
  @brief This function starts the event thread

  @param cb pointer to callback function from the client
  @param client_data reference client wants to get back
  through callback
  @return handle to msging thread
 */
struct ape_ipc_info *omx_ape_event_thread_create(
        message_func cb,
        void* client_data,
        char* th_name)
{
    int r;
    int fds[2];
    struct ape_ipc_info *ape_info;

    ape_info = calloc(1, sizeof(struct ape_ipc_info));
    if (!ape_info) return 0;

    ape_info->client_data = client_data;
    ape_info->process_msg_cb = cb;
    strlcpy(ape_info->thread_name,th_name, sizeof (ape_info->thread_name));

    if (pipe(fds)) {
        DEBUG_PRINT_ERROR("\n%s: pipe creation failed\n", __FUNCTION__);
        goto fail_pipe;
    }

    ape_info->pipe_in = fds[0];
    ape_info->pipe_out = fds[1];


    r = pthread_create(&ape_info->thr, 0, omx_ape_events, ape_info);
    if (r < 0) goto fail_thread;

    DEBUG_DETAIL("Created thread for %s \n", ape_info->thread_name);
    return ape_info;


fail_thread:
    close(ape_info->pipe_in);
    close(ape_info->pipe_out);

fail_pipe:
    free(ape_info);

    return 0;
}

/**
  @brief This function processes posted messages

  Once thread is being spawned, this function is run to
  start processing commands posted by client

  @param info pointer to context

 */
void *omx_ape_msg(void *info)
{
    struct ape_ipc_info *ape_info = (struct ape_ipc_info*)info;
    unsigned char id;
    ssize_t n;

    DEBUG_DETAIL("\n%s: message thread start\n", __FUNCTION__);
    while (!ape_info->dead) {
        n = read(ape_info->pipe_in, &id, 1);
        if (n == 0)
        {
            DEBUG_DETAIL("%s: message pipe read zero\n", __FUNCTION__);
            break;
        }
        if (n == 1) {
            DEBUG_DETAIL("\n%s-->pipe_in=%d pipe_out=%d\n",
                    ape_info->thread_name,
                    ape_info->pipe_in,
                    ape_info->pipe_out);

            ape_info->process_msg_cb(ape_info->client_data, id);

        }
        if ((n < 0) && (errno != EINTR))
        {
            DEBUG_PRINT_ERROR("%s:message pipe read n=%lu err=0x%8x\n",ape_info->thread_name,n,errno);
            break;
        }
    }
    DEBUG_DETAIL("%s: message thread stop\n", ape_info->thread_name);

    return 0;
}

void *omx_ape_events(void *info)
{
    struct ape_ipc_info *ape_info = (struct ape_ipc_info*)info;
    unsigned char id = 0;

    DEBUG_DETAIL("%s: message thread start\n", ape_info->thread_name);
    ape_info->process_msg_cb(ape_info->client_data, id);
    DEBUG_DETAIL("%s: message thread stop\n", ape_info->thread_name);
    return 0;
}


void omx_ape_thread_stop(struct ape_ipc_info *ape_info)
{
    int rc = 0;
    DEBUG_DETAIL("%s: message thread close\n", ape_info->thread_name);
    ape_info->dead = 1;
    close(ape_info->pipe_in);
    close(ape_info->pipe_out);
    rc = pthread_join(ape_info->thr,NULL);
    ape_info->pipe_out = -1;
    ape_info->pipe_in = -1;
    free(ape_info);
}

void omx_ape_post_msg(struct ape_ipc_info *ape_info, unsigned int id) {
    DEBUG_DETAIL("\n%s id=%d\n", __FUNCTION__,id);
    write(ape_info->pipe_out, &id, 1);
}
