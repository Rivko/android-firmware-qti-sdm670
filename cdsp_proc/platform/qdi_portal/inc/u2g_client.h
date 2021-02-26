#ifndef U2G_CLIENT_H
#define U2G_CLIENT_H

#include "qurt.h"
#include "qurt_qdi_driver.h"

int u2g_client_open(const char *name);
void u2g_client_close(int handle);

int u2g_client_open_ex(const char *name,
                       qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                       qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                       qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);
void u2g_client_stats(uint64_t *nread, uint64_t *nwrite, uint64_t *wusecs);
int u2g_client_stop(void);
void u2g_client_migrate(void);
int u2g_client_start(void);
int u2g_client_atexit(void(*pfn)(void*), void *ctx);
#endif // U2G_CLIENT_H
