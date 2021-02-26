#include "qurt_qdi.h"
#include "qurt_types.h"
#include "fastrpc_port.h"

#define QDI_FASTRPC_PORT_NAME      "/dev/smd_port"
#define QDI_PORT_BASE              256
#define QDI_PORT_INIT              (QDI_PORT_BASE + 0)
#define QDI_PORT_INVOKE_RSP        (QDI_PORT_BASE + 3)

#define QDI_PORT_MSGS              64

struct fastrpc_port_queue {
   uint32 fut;
   uint32 rd_idx;
   uint32 wr_idx;
   int port_status;
   struct fastrpc_port_msg msg[QDI_PORT_MSGS];
};

static __inline int fp_qdi_open(void) {
   return qurt_qdi_open(QDI_FASTRPC_PORT_NAME);
}
static __inline int fp_qdi_process_init(int handle, struct fastrpc_port_queue** pqmsg) {
   return qurt_qdi_handle_invoke(handle, QDI_PORT_INIT, pqmsg);
}
static __inline int fp_qdi_invoke_rsp(int handle, struct fastrpc_port_msg* pmsg, int ret) {
   return qurt_qdi_handle_invoke(handle, QDI_PORT_INVOKE_RSP, pmsg, ret);
}

void fastrpc_port_open(void);
void fastrpc_port_close(void);
int  fastrpc_port_enqueue(struct fastrpc_port_msg* pmsg);
