/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
=============================================================

                          EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ------------------------------------------
07/09/18   ac      Initial version
=============================================================*/
#ifndef SOCK_COMM_H
#define SOCK_COMM_H

#include <unistd.h>
#include <stdint.h>
//TODO Added only for compilation. To replace with orignal.
#include "dummy_ipc.h"

//TODO This is temp service id. Need to register for dedicated service id.
#define MLAPP_SERVER_SERVICE_ID 5000

typedef enum {
    SOCK_E_SUCCESS            = 0,    /**< Success. */
    SOCK_E_FAIL               = 1,    /**< General error. */
    /** @endcond */
} sock_error_t;

typedef int guest_socket;
typedef struct sockaddr_msm_ipc guest_sockaddr;
typedef socklen_t guest_sockaddr_len_type;
typedef struct pollfd guest_socket_pollfd;

sock_error_t guest_socket_open (guest_socket *socket_handle);

sock_error_t guest_socket_close (guest_socket *socket_handle);

void guest_socket_set_addr_by_name (guest_sockaddr *sockaddr,
                              uint32_t service_id, uint32_t instance_id);

sock_error_t guest_socket_bind(guest_socket *socket_handle,
                              guest_sockaddr *local_addr);

int guest_socket_recvfrom(guest_socket *socket_handle, void *buffer,
                       int32_t buffer_size, guest_sockaddr *remote_addr,
                       int32_t *recd_size);

int guest_socket_sendto(guest_socket *socket_handle, void *buffer,
                       int32_t buffer_size, guest_sockaddr *remote_addr,
                       int32_t *sent_size);

int guest_socket_lookup(guest_socket fd, struct msm_ipc_port_name *name,
                    guest_sockaddr *addrs);

#endif
