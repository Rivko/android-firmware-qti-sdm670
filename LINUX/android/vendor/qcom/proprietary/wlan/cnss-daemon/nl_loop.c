/*
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netinet/in.h>
#include <net/if.h>

#include "debug.h"
#include "nl_loop.h"
#include "cnss_qmi_client.h"
#include "cnss_gw_update.h"
#include "cld80211_lib.h"

#define NL_LOOP_MAX_IND 10

struct nl_loop_ind_table {
	unsigned short ind;
	nl_loop_ind_handler ind_handler;
	void *user_data;
};

struct nl_loop {
	int init_done;
	struct nl_loop_ind_table ind_table[NL_LOOP_MAX_IND];
	int nl_fd;
	int terminate;
	int is_cld80211_lib;
	struct nl_cb *cb;
	struct cld80211_ctx *cldctx;
	int nlmsg_err;
};

static int nl_loop_process_msg_svc(struct wlan_hdr *ani_hdr);
static struct nl_loop nl_loop;

static inline char *nl_loop_ind_to_str(int ind)
{
	switch(ind) {
	case WLAN_MSG_WLAN_STATUS_IND: return "WLAN_MSG_WLAN_STATUS_IND";
	case WLAN_MSG_WLAN_VERSION_IND: return "WLAN_MSG_WLAN_VERSION_IND";
	case WLAN_MSG_WLAN_TP_IND: return "WLAN_MSG_WLAN_TP_IND";
	case WLAN_MSG_WLAN_TP_TX_IND: return "WLAN_MSG_WLAN_TP_TX_IND";
	case WLAN_MSG_RPS_ENABLE_IND: return "WLAN_MSG_RPS_ENABLE_IND";
	case WLAN_MSG_CORE_MINFREQ: return "WLAN_MSG_CORE_MINFREQ";
	}
	return "UNKNOWN";
}

static inline char *nl_loop_type_to_str(int ind)
{
	switch(ind) {
	case WLAN_MSG_SVC: return "WLAN_MSG_SVC";
	}
	return "UNKNOWN";
}

static struct nl_loop_ind_table *nl_loop_find_ind_table(int ind)
{
	int i;
	struct nl_loop_ind_table *ind_table = NULL;

	for (i = 0; i < NL_LOOP_MAX_IND; i++) {
		ind_table = &nl_loop.ind_table[i];
		if (ind_table->ind_handler != NULL && ind_table->ind == ind)
			break;
	}

	return i < NL_LOOP_MAX_IND ? ind_table : NULL;
}

static int cld80211_process_msg(int cmd, struct wlan_hdr *ani_hdr)
{
	wsvc_printf_dbg("cmd: %u, type: %s", cmd, nl_loop_type_to_str(cmd));

	switch (cmd) {
		case WLAN_MSG_SVC:
		nl_loop_process_msg_svc(ani_hdr);
		break;
	}

	return 0;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	UNUSED(msg);
	return NL_STOP;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	UNUSED(msg);
	return NL_SKIP;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
			 void *arg)
{
	int *ret = (int *)arg;
	*ret = err->error;

	UNUSED(nla);
	wsvc_printf_err("%s: error_handler received : %d",
	                getprogname(), err->error);
	return NL_SKIP;
}

static int no_seq_check(struct nl_msg *msg, void *arg)
{
	UNUSED(msg);
	UNUSED(arg);
	return NL_OK;
}

static int event_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *genlh;
	struct nlattr *attrs[CLD80211_ATTR_MAX + 1];
	struct nlattr *tb_vendor[CLD80211_ATTR_MAX + 1];

	UNUSED(arg);
	struct  nlmsghdr *nlh = nlmsg_hdr(msg);

	genlh = (struct genlmsghdr *)nlmsg_data(nlh);
	if (genlh->cmd != WLAN_MSG_SVC)
		return 0;

	int result = nla_parse(attrs, CLD80211_ATTR_MAX, genlmsg_attrdata(
			       genlh, 0), genlmsg_attrlen(genlh, 0), NULL);

	if (result || !attrs[CLD80211_ATTR_VENDOR_DATA]) {
		wsvc_printf_err("No valid data received");
		return 0;
	}

	nla_parse(tb_vendor, CLD80211_ATTR_MAX, (struct nlattr *)nla_data(
		  attrs[CLD80211_ATTR_VENDOR_DATA]),
		  nla_len(attrs[CLD80211_ATTR_VENDOR_DATA]), NULL);

	if (!tb_vendor[CLD80211_ATTR_DATA]) {
		wsvc_printf_err("%s: CLD80211_ATTR_DATA not"
		                " found", __FUNCTION__);
		return 0;
	}

	cld80211_process_msg(genlh->cmd,
			     nla_data(tb_vendor[CLD80211_ATTR_DATA]));

	return 0;
}

static int cld80211_nl_recvmsgs()
{
	int ret = 0;
	ret = nl_recvmsgs(nl_loop.cldctx->sock, nl_loop.cb);
	if(ret < 0)
		wsvc_printf_info("Error :%d while reading nl msg ,"
		                 " err: %d", ret, errno);
	return ret;
}

static int initialize_cld80211lib()
{
	int ret = -1;

	nl_loop.cldctx = cld80211_init();
	if (nl_loop.cldctx == NULL)
		return ret;

	ret = cld80211_add_mcast_group(nl_loop.cldctx, "svc_msgs");
	if (ret) {
		wsvc_printf_err("Failed to add mcast group:%d\n", ret);
		ret = -1;
		goto out;
	}

	nl_loop.cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!nl_loop.cb) {
		wsvc_printf_err("%s: Failed to alloc nl_cb", __func__);
		cld80211_remove_mcast_group(nl_loop.cldctx, "svc_msgs");
		ret = -1;
		goto out;
	}

	nl_loop.nlmsg_err = 1;

	nl_cb_err(nl_loop.cb, NL_CB_CUSTOM, error_handler, &nl_loop.nlmsg_err);
	nl_cb_set(nl_loop.cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler,
		  &nl_loop.nlmsg_err);
	nl_cb_set(nl_loop.cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler,
		  &nl_loop.nlmsg_err);
	nl_cb_set(nl_loop.cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(nl_loop.cb, NL_CB_VALID, NL_CB_CUSTOM, event_handler, NULL);

	return 0;
out:
	cld80211_deinit(nl_loop.cldctx);
	nl_loop.cldctx = NULL;
	return ret;
}

static void deinitialize_cld80211lib()
{
	cld80211_remove_mcast_group(nl_loop.cldctx, "svc_msgs");
	nl_cb_put(nl_loop.cb);
	cld80211_deinit(nl_loop.cldctx);
	nl_loop.cldctx = NULL;
}

int nl_loop_init(void)
{
	struct sockaddr_nl sa;
	int fd = -1;
	int ret = 0;

	if (nl_loop.init_done || nl_loop.is_cld80211_lib) {
		wsvc_printf_err("%s: nl loop already initialized", __func__);
		return -1;
	}

	ret = initialize_cld80211lib();
	if (ret == 0) {
		nl_loop.nl_fd = nl_socket_get_fd(nl_loop.cldctx->sock);
		nl_loop.is_cld80211_lib = 1;
		goto out;
	}

	wsvc_printf_info("Failed to initialize cld80211 family, proceed "
			"with legacy USERSOCK reception\n");

	fd = socket(AF_NETLINK, SOCK_RAW, WLAN_MSG_FAMILY);
	if (fd < 0) {
		wsvc_perror("socket");
		ret = -1;
		goto out;
	}

	memset (&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = WLAN_MSG_MCAST_GRP_ID;
	sa.nl_pid = getpid();

	ret = bind(fd, (struct sockaddr*)&sa, sizeof(sa));

	if (ret < 0) {
		wsvc_perror("bind");
		close(fd);
		goto out;
	}

	nl_loop.nl_fd = fd;
	nl_loop.init_done = 1;

out:
	return ret;
}

int nl_loop_deinit(void)
{
	if (!nl_loop.init_done && !nl_loop.is_cld80211_lib) {
		wsvc_printf_err("%s: nl loop not initialized", __func__);
		return -1;
	}
	if (nl_loop.is_cld80211_lib)
		deinitialize_cld80211lib();
	else
		close(nl_loop.nl_fd);

	memset(&nl_loop, 0, sizeof(nl_loop));

	return 0;
}

int nl_loop_register(int ind, nl_loop_ind_handler ind_handler, void *user_data)
{
	int i;
	struct nl_loop_ind_table *ind_table = NULL;

	if (!nl_loop.init_done && !nl_loop.is_cld80211_lib) {
		wsvc_printf_err("%s: nl loop not initialized", __func__);
		return -1;
	}

	if (ind_handler == NULL) {
		wsvc_printf_err("%s: ind_handler is NULL!", __func__);
		return -1;
	}

	for (i = 0; i < NL_LOOP_MAX_IND; i++) {
		if (nl_loop.ind_table[i].ind_handler == NULL) {
			ind_table = &nl_loop.ind_table[i];
			break;
		}
	}

	if (ind_table == NULL) {
		wsvc_printf_err("%s: Ind table is full: %d, %x",
				__func__, i, ind);
		return -1;
	}

	wsvc_printf_dbg("%s: Registering ind: %x, %pK", __func__, ind,
			ind_handler);

	ind_table->ind = ind;
	ind_table->ind_handler = ind_handler;
	ind_table->user_data = user_data;

	return 0;
}

int nl_loop_unregister(int ind)
{
	struct nl_loop_ind_table *ind_table = NULL;

	if (!nl_loop.init_done && !nl_loop.is_cld80211_lib) {
		wsvc_printf_err("%s: nl loop not initialized", __func__);
		return -1;
	}

	ind_table = nl_loop_find_ind_table(ind);

	if (ind_table == NULL) {
		wsvc_printf_err("%s: Ind table entry not found: %x",
				__func__, ind);
		return -1;
	}

	wsvc_printf_dbg("%s: Unregistering ind: %x", __func__, ind);

	memset(ind_table, 0, sizeof(*ind_table));

	return 0;
}

static void nl_loop_handle_alarm(int sig)
{
	UNUSED(sig);
	wsvc_printf_err("terminate: %d"
		    "Could not terminate loop in 2 seconds! Some bug???",
		    nl_loop.terminate);

	exit(EXIT_FAILURE);
}

int nl_loop_terminate(void)
{
	nl_loop.terminate = 1;

	wsvc_printf_info("%s called", __func__);

	signal(SIGALRM, nl_loop_handle_alarm);
	alarm(2);

	return 0;
}

static int nl_loop_process_msg_svc(struct wlan_hdr *ani_hdr)
{
	struct nl_loop_ind_table *ind_table = NULL;
	void *p_data = NULL;
	ind_table = nl_loop_find_ind_table(ani_hdr->type);

	wsvc_printf_dbg("Ind type: %s, %x, ind_table: %pK",
			nl_loop_ind_to_str(ani_hdr->type),
			ani_hdr->type, ind_table);

	if (!ind_table) {
		wsvc_printf_err("Failed to find ind_table for type %x",
				ani_hdr->type);
		return 0;
	}

	switch (ani_hdr->type) {
	case WLAN_MSG_WLAN_STATUS_IND:
	case WLAN_MSG_WLAN_VERSION_IND:
	case WLAN_MSG_WLAN_TP_IND:
	case WLAN_MSG_WLAN_TP_TX_IND:
	case WLAN_MSG_RPS_ENABLE_IND:
	case WLAN_MSG_CORE_MINFREQ:
		p_data = (char *)ani_hdr + sizeof(struct wlan_hdr);
		ind_table->ind_handler(ani_hdr->type, p_data, ani_hdr->length);
		break;
	default:
		wsvc_printf_err("%s: Unknown message type %d",
				__func__,
				ani_hdr->type);
		return 0;
	}

	return 0;
}

static int nl_loop_process_msg(struct nlmsghdr *nlh, int len)
{
	if (len < 0)
		return -1;

	while (NLMSG_OK(nlh, (unsigned int)len)) {

		wsvc_printf_dbg("nlmsghdr: len: %d, nlmsg_len: %u type: %s, %x,"
				" pid: %u", len, nlh->nlmsg_len,
				nl_loop_type_to_str(nlh->nlmsg_type),
				nlh->nlmsg_type, nlh->nlmsg_pid);

		switch (nlh->nlmsg_type) {
		case WLAN_MSG_SVC:
			nl_loop_process_msg_svc(NLMSG_DATA(nlh));
			break;
		}

		nlh = NLMSG_NEXT(nlh, len);
		if (len < 0)
			break;
	}

	return 0;
}

int nl_loop_run(void)
{
	struct sockaddr_nl dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
	int ret = 0;
	struct pollfd pfd[2];

	if (!nl_loop.init_done && !nl_loop.is_cld80211_lib) {
		wsvc_printf_err("%s: nl loop not initialized", __func__);
		ret = -1;
		goto end;
	}

	memset(&pfd, 0, 2 * sizeof(pfd[0]));
	pfd[0].fd = nl_loop.nl_fd;
	pfd[0].events = POLLIN;

	pfd[1].fd = cnss_get_gw_update_sockfd();
	pfd[1].events = POLLIN;

	if (!nl_loop.is_cld80211_lib) {
		nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(
							WLAN_MSG_MAX_PAYLOAD));
		if (nlh == NULL) {
		        wsvc_printf_err("Cannot allocate memory!");
		        ret = -1;
		        goto end;
		 }

		memset(nlh, 0, NLMSG_SPACE(WLAN_MSG_MAX_PAYLOAD));
		nlh->nlmsg_len = NLMSG_SPACE(WLAN_MSG_MAX_PAYLOAD);
		nlh->nlmsg_pid = getpid();

		iov.iov_base = (void *)nlh;
		iov.iov_len = nlh->nlmsg_len;

		memset(&dest_addr, 0, sizeof(dest_addr));
		dest_addr.nl_family = AF_NETLINK;
		dest_addr.nl_pid = 0; /* For Linux Kernel */
		dest_addr.nl_groups = WLAN_MSG_MCAST_GRP_ID;

		memset(&msg, 0, sizeof(msg));
		msg.msg_name = (void *)&dest_addr;
		msg.msg_namelen = sizeof(dest_addr);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
	}

	while (!nl_loop.terminate) {
		int timeout = -1;
		int result = 0;

		pfd[0].revents = 0;
		pfd[1].revents = 0;

		result = poll(pfd, 2, -1);
		if (result < 0) {
			wsvc_printf_info("Error polling sockets\n");
			continue;
		}

		if (pfd[0].revents & (POLLIN | POLLHUP | POLLERR)) {
			if (nl_loop.is_cld80211_lib)
				ret = cld80211_nl_recvmsgs();
			else
				ret = recvmsg(pfd[0].fd, &msg, 0);
			if (ret < 0 && errno == EINTR) {
				wsvc_printf_info("Loop terminating: EINTR");
				ret = 0;
				break;
			}

			if (ret < 0) {
				wsvc_perror("recvmsg");
				continue;
			}
			if (!nl_loop.is_cld80211_lib) {
				wsvc_hexdump("recvmsg", (uint8_t *)nlh,
				             ret);
				nl_loop_process_msg(nlh, ret);
			}
		}

		if (pfd[1].revents & (POLLIN | POLLHUP | POLLERR)) {
			wsvc_printf_dbg("pfd[1].fd: %d has data to process\n", pfd[1].fd);
			cnss_gw_update_loop(pfd[1].fd);
		}
	}

	if (!nl_loop.is_cld80211_lib)
		free(nlh);

end:
	return ret;
}
