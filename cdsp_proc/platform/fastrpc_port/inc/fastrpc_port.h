#ifndef FASTRPC_PORT_H
#define FASTRPC_PORT_H

#include "fastrpc_internal.h"

#define APSS_PORT  0
#define LPASS_PORT 1
#define MPSS_PORT  2
#define SLPI_PORT  3
#define CDSP_PORT  4

#define NUM_PORTS  5

struct fastrpc_port_msg {
   uint32 mode64;
   struct smq_msg msg;
};

#define FASTRPC_GLINK_TX_RETRY_ATTEMPTS	(100)

void fastrpc_port_respond(struct fastrpc_port_msg* msg, int rsp);
int fastrpc_port_kinit(int (*enqueue)(void* ctx, struct fastrpc_port_msg* msg), 
                       void (*open)(void* ctx),
                       void (*close)(void* ctx, int domain),
                       void* ctx);
int fastrpc_port_kinit_remote(int (*enqueue)(void* ctx, struct fastrpc_port_msg* msg), 
                       void (*open)(void* ctx),
                       void (*close)(void* ctx, int domain),
                       void* ctx);
int fastrpc_port_send_remote(int id, struct fastrpc_port_msg* msg);
int fastrpc_port_get_local(void);

#endif // FASTRPC_PORT_H
