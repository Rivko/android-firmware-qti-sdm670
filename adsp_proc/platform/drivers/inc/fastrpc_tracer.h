#include "qurt_qdi.h"
#include "qurt_types.h"

#define QDI_DRIVER_TRACER_NAME  "/dev/fastrpc_tracer"
#define QDI_TRACER_BASE         256
#define QDI_TRACER_WRITE_END    (QDI_TRACER_BASE + 0)
#define QDI_TRACER_QUERY_MEM    (QDI_TRACER_BASE + 1)
#define QDI_TRACER_FREE_MEM     (QDI_TRACER_BASE + 2)
#define QDI_TRACER_WRITE        (QDI_TRACER_BASE + 3)

static __inline int fastrpc_tracer_qdi_open(void) {
   return qurt_qdi_open(QDI_DRIVER_TRACER_NAME);
}
static __inline int fastrpc_tracer_qdi_close(int handle) {
   return qurt_qdi_close(handle);
}
static __inline int fastrpc_tracer_qdi_write(int handle, const void *pv, unsigned len) {
   return qurt_qdi_handle_invoke(handle, QDI_WRITE, pv, len);
}
static __inline int fastrpc_tracer_qdi_write_end(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_TRACER_WRITE_END);
}

static __inline int fastrpc_tracer_qdi_create_buffer(int handle, unsigned len, char **pv) {
   return qurt_qdi_handle_invoke(handle, QDI_TRACER_QUERY_MEM, len, pv);
}

static __inline int fastrpc_tracer_qdi_release_buffer(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_TRACER_FREE_MEM);
}

static __inline int fastrpc_tracer_qdi_write_opt(int handle, unsigned len) {
   return qurt_qdi_handle_invoke(handle, QDI_TRACER_WRITE, len);
}
