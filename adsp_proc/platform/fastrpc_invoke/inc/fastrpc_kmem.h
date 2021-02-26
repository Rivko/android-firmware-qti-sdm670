#ifndef FASTRPC_KMEM_H
#define FASTRPC_KMEM_H
#include "qurt.h"
#include "qurt_qdi_driver.h"
#include "u2g_client.h"

#define QDI_KMEM_NAME                   "/dev/fastrpc_kmem"
#define QDI_KMEM_BASE                   256

#define QDI_KMEM_COMMIT_PAGE            (QDI_KMEM_BASE + 0)
#define QDI_KMEM_INV_PAGE               (QDI_KMEM_BASE + 1)

struct smq_phy_page;
static __inline int fastrpc_kmem_commit_page(int handle, void *buf, int size, struct smq_phy_page *p, int offset, int bflush) {
   return qurt_qdi_handle_invoke(handle, QDI_KMEM_COMMIT_PAGE, buf, size, p, offset, bflush);
}
static __inline int fastrpc_kmem_inv_page(int handle, struct smq_phy_page *p, int offset, int size) {
   return qurt_qdi_handle_invoke(handle, QDI_KMEM_INV_PAGE, p, offset, size);
}



#endif
