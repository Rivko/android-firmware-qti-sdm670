#include "qurt_qdi.h"
#include "qurt_types.h"

#define QDI_DRIVER_VR_NAME  "/dev/vr"
#define QDI_VR_BASE         256
#define QDI_VR_INIT         (QDI_VR_BASE + 0)
#define QDI_VR_DEINIT       (QDI_VR_BASE + 1)
#define QDI_VR_CALLBACK     (QDI_VR_BASE + 2)

static __inline int vr_qdi_open(void) {
   return qurt_qdi_open(QDI_DRIVER_VR_NAME);
}
static __inline int vr_qdi_close(int handle) {
   return qurt_qdi_close(handle);
}
static __inline int vr_qdi_init(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_VR_INIT);
}
static __inline int vr_qdi_deinit(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_VR_DEINIT);
}
static __inline int vr_qdi_wait_callback(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_VR_CALLBACK);
}
