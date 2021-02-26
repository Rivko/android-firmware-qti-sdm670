#include "qurt_qdi.h"
#include "qurt_types.h"

#define QDI_DRIVER_NOTIFY_NAME  "/dev/dspnotify"
#define QDI_NOTIFY_BASE         256
#define QDI_NOTIFY_INIT         (QDI_NOTIFY_BASE + 0)
#define QDI_NOTIFY_DEINIT       (QDI_NOTIFY_BASE + 1)
#define QDI_NOTIFY_WAKE         (QDI_NOTIFY_BASE + 3)
#define QDI_NOTIFY_WAIT         (QDI_NOTIFY_BASE + 4)

static __inline int notify_qdi_open(void) {
   return qurt_qdi_open(QDI_DRIVER_NOTIFY_NAME);
}
static __inline int notify_qdi_close(int handle) {
   return qurt_qdi_close(handle);
}
static __inline int notify_qdi_init(int handle, uint8* name, int len) {
   return qurt_qdi_handle_invoke(handle, QDI_NOTIFY_INIT, name, len);
}
static __inline int notify_qdi_deinit(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_NOTIFY_DEINIT);
}
static __inline int notify_qdi_wake(int handle, int domain) {
   return qurt_qdi_handle_invoke(handle, QDI_NOTIFY_WAKE, domain);
}
static __inline int notify_qdi_wait(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_NOTIFY_WAIT);
}
