#ifndef U2G_SERVER_H
#define U2G_SERVER_H

struct u2g_server;
int u2g_server_new(int asid, struct u2g_server **po);
int u2g_server_wake(int client_handle, unsigned *fut);
void u2g_server_release(struct u2g_server *me);
void u2g_server_close(struct u2g_server *me);
int u2g_server_futex_create(struct u2g_server *me, int val);

#endif // U2G_SERVER_H
