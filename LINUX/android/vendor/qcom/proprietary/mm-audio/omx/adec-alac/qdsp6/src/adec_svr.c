/*--------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
  --------------------------------------------------------------------------*/
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
struct alac_ipc_info *omx_alac_thread_create(
        message_func cb,
        void* client_data,
        char* th_name)
{
    int r;
    int fds[2];
    struct alac_ipc_info *alac_info;

    alac_info = calloc(1, sizeof(struct alac_ipc_info));
    if (!alac_info) return 0;

    alac_info->client_data = client_data;
    alac_info->process_msg_cb = cb;
    strlcpy(alac_info->thread_name,th_name, sizeof (alac_info->thread_name));

    if (pipe(fds)) {
        DEBUG_PRINT_ERROR("%s: pipe creation failed\n", __func__);
        goto fail_pipe;
    }

    alac_info->pipe_in = fds[0];
    alac_info->pipe_out = fds[1];


    r = pthread_create(&alac_info->thr, 0, omx_alac_msg, alac_info);
    if (r < 0) goto fail_thread;

    DEBUG_DETAIL("Created thread for %s", alac_info->thread_name);
    return alac_info;


fail_thread:
    close(alac_info->pipe_in);
    close(alac_info->pipe_out);

fail_pipe:
    free(alac_info);

    return 0;
}

/**
  @brief This function starts the event thread

  @param cb pointer to callback function from the client
  @param client_data reference client wants to get back
  through callback
  @return handle to msging thread
 */
struct alac_ipc_info *omx_alac_event_thread_create(
        message_func cb,
        void* client_data,
        char* th_name)
{
    int r;
    int fds[2];
    struct alac_ipc_info *alac_info;

    alac_info = calloc(1, sizeof(struct alac_ipc_info));
    if (!alac_info) return 0;

    alac_info->client_data = client_data;
    alac_info->process_msg_cb = cb;
    strlcpy(alac_info->thread_name,th_name, sizeof (alac_info->thread_name));

    if (pipe(fds)) {
        DEBUG_PRINT_ERROR("\n%s: pipe creation failed\n", __FUNCTION__);
        goto fail_pipe;
    }

    alac_info->pipe_in = fds[0];
    alac_info->pipe_out = fds[1];


    r = pthread_create(&alac_info->thr, 0, omx_alac_events, alac_info);
    if (r < 0) goto fail_thread;

    DEBUG_DETAIL("Created thread for %s \n", alac_info->thread_name);
    return alac_info;


fail_thread:
    close(alac_info->pipe_in);
    close(alac_info->pipe_out);

fail_pipe:
    free(alac_info);

    return 0;
}

/**
  @brief This function processes posted messages

  Once thread is being spawned, this function is run to
  start processing commands posted by client

  @param info pointer to context

 */
void *omx_alac_msg(void *info)
{
    struct alac_ipc_info *alac_info = (struct alac_ipc_info*)info;
    unsigned char id;
    ssize_t n;

    DEBUG_DETAIL("\n%s: message thread start\n", __FUNCTION__);
    while (!alac_info->dead) {
        n = read(alac_info->pipe_in, &id, 1);
        if (n == 0)
        {
            DEBUG_DETAIL("%s: message pipe read zero\n", __FUNCTION__);
            break;
        }
        if (n == 1) {
            DEBUG_DETAIL("\n%s-->pipe_in=%d pipe_out=%d\n",
                    alac_info->thread_name,
                    alac_info->pipe_in,
                    alac_info->pipe_out);

            alac_info->process_msg_cb(alac_info->client_data, id);

        }
        if ((n < 0) && (errno != EINTR))
        {
            DEBUG_PRINT_ERROR("%s:message pipe read n=%lu err=0x%8x\n",alac_info->thread_name,n,errno);
            break;
        }
    }
    DEBUG_DETAIL("%s: message thread stop\n", alac_info->thread_name);

    return 0;
}

void *omx_alac_events(void *info)
{
    struct alac_ipc_info *alac_info = (struct alac_ipc_info*)info;
    unsigned char id = 0;

    DEBUG_DETAIL("%s: message thread start\n", alac_info->thread_name);
    alac_info->process_msg_cb(alac_info->client_data, id);
    DEBUG_DETAIL("%s: message thread stop\n", alac_info->thread_name);
    return 0;
}


void omx_alac_thread_stop(struct alac_ipc_info *alac_info)
{
    int rc = 0;
    DEBUG_DETAIL("%s: message thread close\n", alac_info->thread_name);
    alac_info->dead = 1;
    close(alac_info->pipe_in);
    close(alac_info->pipe_out);
    rc = pthread_join(alac_info->thr,NULL);
    alac_info->pipe_out = -1;
    alac_info->pipe_in = -1;
    free(alac_info);
}

void omx_alac_post_msg(struct alac_ipc_info *alac_info, unsigned int id) {
    DEBUG_DETAIL("\n%s id=%d\n", __FUNCTION__,id);
    write(alac_info->pipe_out, &id, 1);
}
