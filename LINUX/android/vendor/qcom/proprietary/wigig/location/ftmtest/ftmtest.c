/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include "nl80211_copy.h"
#include "qca-vendor.h"
#include "aoa_api.h"

#define ETH_ALEN	6

#ifndef SOL_NETLINK
#define SOL_NETLINK	270
#endif

/* minimum number of FTM measurements we need to get distance */
#define FTM_MIN_MEASUREMENTS	3

/* maximum number of FTM measurements we support */
#define FTM_MAX_MEASUREMENTS	5

/* threshold for valid PPM offset in RTT calculation */
#define FTM_RTT_PPM_OFFSET_THRESHOLD	(25.0)

/* burst duration of 250us (defined in 802.11mc draft 5.0 section 9.4.2.167) */
#define FTM_BURST_DURATION_250_US	2

/* speed of light in cm/nanosecond */
#define	SPEED_OF_LIGHT	(29.9702547)

/* default AOA calibration database file name */
#define AOA_DEFAULT_CLDB_FNAME	"aoa_cldb.bin"

/* structure that we use to fill in extra FTM capabilities */
struct loc_extra_capa {
	uint32_t flags;
	uint16_t max_session;
	uint16_t max_dest;
	uint8_t max_burst_exp;
	uint8_t max_meas_per_burst;
	uint32_t aoa_types;
};

/* a single measurement result */
struct ftm_meas {
	uint64_t t1, t2, t3, t4;
};

/* measurement arguments, common to ftm and aoa, extracted from command line */
struct ftm_cmd_meas_args {
	/* destination peer for measurement */
	uint8_t mac_addr[ETH_ALEN];

	/* number of iterations do run */
	uint32_t count;

	/* delay in milliseconds between iterations */
	uint32_t delay;

	/* the maximum time in milliseconds a single measurement
	 * iteration can take
	 */
	uint32_t timeout;
};

/* structure with global state, passed to callback handlers */
struct ftmtest_state {
	/* callbacks handle for synchronous NL commands */
	struct nl_cb *cb;

	/* nl socket handle for synchronous NL commands */
	struct nl_sock *nl;

	/* nl socket handler for events */
	struct nl_sock *nl_event;

	/* family id for nl80211 events */
	int nl80211_id;

	/* interface index of wigig driver */
	int ifindex;

	/* true if driver has indoor location API (LOC_GET_CAPA command) */
	bool has_loc_api;

	/* set to 1 if FTM initiator capability was detected */
	int is_initiator;

	/* set to 1 if FTM responder capability was detected */
	int is_responder;

	/* indoor location capabilities */
	struct loc_extra_capa capa;

	/* true if FTM session is running */
	bool session_started;

	/* cookie counter for assigning unique cookie every session */
	uint32_t cookie_counter;

	/* session cookie we use */
	uint64_t session_cookie;

	/* session done status code */
	uint8_t session_result_code;

	/* recorded mac address of measured peer (FTM or AOA) */
	uint8_t mac_addr[ETH_ALEN];

	/* frequency of peer in MHz or 0 if not specified */
	uint32_t freq;

	/* number of FTM measurements requested */
	uint32_t num_ftm_meas_requested;

	/* recorded FTM measurement results */
	int num_ftm_meas;
	struct ftm_meas ftm_meas[FTM_MAX_MEASUREMENTS];

	/* factor for correcting FTM distance calculation
	 * (workaround for FW bug)
	 */
	double ftm_factor;

	/* true if AOA needed as part of FTM session */
	bool aoa_in_ftm_requested;

	/* AOA algorithm library */
	aoalib_handle_t aoalib_handle;

	/* true if AOA measurement started */
	bool aoa_started;

	/* requested AOA type */
	uint32_t aoa_type;

	/* recorded raw AOA measurement data */
	uint8_t *aoa_data;
	uint32_t aoa_data_length;

	/* result antenna array mask */
	uint32_t antenna_array_mask;

	/* result AOA type */
	uint32_t res_aoa_type;

	/* session done status code for AOA */
	uint8_t aoa_session_result_code;

	/* verbose flag - when set print additional results like ZPFactor */
	bool verbose;
} g_state;

/* policy for parsing FTM vendor commands */
static struct
nla_policy nl80211_loc_policy[QCA_WLAN_VENDOR_ATTR_MAX + 1] = {
	[QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE] = { .type = NLA_U64 },
	[QCA_WLAN_VENDOR_ATTR_LOC_CAPA] = { .type = NLA_NESTED },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_PEERS] = { .type = NLA_NESTED },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_PEER_RESULTS] = { .type = NLA_NESTED },
	[QCA_WLAN_VENDOR_ATTR_FTM_RESPONDER_ENABLE] = { .type = NLA_FLAG },
	[QCA_WLAN_VENDOR_ATTR_FTM_INITIAL_TOKEN] = { .type = NLA_U8 },
	[QCA_WLAN_VENDOR_ATTR_AOA_TYPE] = { .type = NLA_U32 },
	[QCA_WLAN_VENDOR_ATTR_LOC_ANTENNA_ARRAY_MASK] = { .type = NLA_U32 },
};

/* policy for parsing FTM capabilities */
static struct
nla_policy nl80211_loc_capa_policy[QCA_WLAN_VENDOR_ATTR_LOC_CAPA_MAX + 1] = {
	[QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAGS] = { .type = NLA_U32 },
	[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_SESSIONS] = { .type = NLA_U16 },
	[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_PEERS] = { .type = NLA_U16 },
	[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_BURSTS_EXP] = { .type = NLA_U8 },
	[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_MEAS_PER_BURST] = { .type = NLA_U8 },
	[QCA_WLAN_VENDOR_ATTR_AOA_CAPA_SUPPORTED_TYPES] = { .type = NLA_U32 },
};

/* policy for parsing a single peer measurement results */
static struct
nla_policy nl80211_ftm_peer_res_policy[
	QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAX + 1] = {
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAC_ADDR] = { .minlen = ETH_ALEN },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_STATUS] = { .type = NLA_U8 },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_FLAGS] = { .type = NLA_U32 },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_VALUE_SECONDS] = { .type = NLA_U8 },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_LCI] = { .type = NLA_UNSPEC },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_LCR] = { .type = NLA_UNSPEC },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MEAS_PARAMS] = {
		 .type = NLA_NESTED },
	[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MEAS] = { .type = NLA_NESTED },
};

/* policy for parsing a measurement result */
static struct
nla_policy nl80211_ftm_meas_res_policy[
	QCA_WLAN_VENDOR_ATTR_FTM_MEAS_MAX + 1] = {
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T1] = { .type = NLA_U64 },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T2] = { .type = NLA_U64 },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T3] = { .type = NLA_U64 },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T4] = { .type = NLA_U64 },
	[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_RSSI] = { .type = NLA_U32 },
};

__attribute__ ((format (printf, 1, 2)))
static int log_printf(const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vfprintf(stdout, fmt, ap);
	va_end(ap);
	fflush(stdout);
	return (ret);
}

/**
 * nl callback handler for disabling sequence number checking
 */
static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

/**
 * nl callback handler called on error
 */
static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	int *ret = (int *)arg;
	*ret = err->error;
	log_printf("error handler with error: %d\n", *ret);
	return NL_SKIP;
}

/**
 * nl callback handler called after all messages in
 * a multi-message reply are delivered. It is used
 * to detect end of synchronous command/reply
 */
static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

/**
 * nl callback handler called when ACK is received
 * for a command. It is also used to detect end of
 * synchronous command/reply
 */
static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_STOP;
}

/**
 * handler for resolving multicast group (family) id
 * used in nl_get_multicast_id below
 */
struct family_data {
	const char *group;
	int id;
};

static int family_handler(struct nl_msg *msg, void *arg)
{
	struct family_data *res = (struct family_data *)arg;
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *mcgrp;
	int tmp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);
	if (!tb[CTRL_ATTR_MCAST_GROUPS])
		return NL_SKIP;

	nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], tmp) {
		struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX, nla_data(mcgrp),
			  nla_len(mcgrp), NULL);
		if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] ||
		    !tb2[CTRL_ATTR_MCAST_GRP_ID] ||
		    strncmp(nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]),
			    res->group,
			    nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME])) != 0)
			continue;
		res->id = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
		break;
	};

	return NL_SKIP;
}

/**
 * send NL command and receive reply synchronously
 */
static int nl_cmd_send_and_recv(struct nl_sock *nl,
				struct nl_cb *cb,
				struct nl_msg *msg,
				int (*valid_handler)(struct nl_msg *, void *),
				void *valid_data)
{
	struct nl_cb *cb_clone = nl_cb_clone(cb);
	int err;

	if (!cb_clone) {
		log_printf("failed to allocate cb_clone\n");
		return -ENOMEM;
	}

	err = nl_send_auto_complete(nl, msg);
	if (err < 0) {
		log_printf("failed to send message, err %d\n", err);
		nl_cb_put(cb_clone);
		return err;
	}

	err = 1;
	nl_cb_err(cb_clone, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb_clone, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb_clone, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	if (valid_handler)
		nl_cb_set(cb_clone, NL_CB_VALID, NL_CB_CUSTOM,
			  valid_handler, valid_data);

	while (err > 0) {
		int res = nl_recvmsgs(nl, cb_clone);

		if (res < 0) {
			log_printf("nl_recvmsgs failed: %d\n", res);
			/* TODO should we exit the loop here?
			   similar code in supplicant does not */
		}
	}

	nl_cb_put(cb_clone);
	return err;
}

/**
 * send NL command and receive reply synchronously,
 * for non-blocking sockets
 */
static int nl_cmd_send_and_recv_nonblock(
				struct nl_sock *nl,
				struct nl_cb *cb,
				struct nl_msg *msg,
				int (*valid_handler)(struct nl_msg *, void *),
				void *valid_data)
{
	struct nl_cb *cb_clone = nl_cb_clone(cb);
	struct pollfd fds;
	int err;

	if (!cb_clone) {
		log_printf("failed to allocate cb_clone\n");
		return -ENOMEM;
	}

	err = nl_send_auto_complete(nl, msg);
	if (err < 0) {
		log_printf("failed to send message, err %d\n", err);
		nl_cb_put(cb_clone);
		return err;
	}

	err = 1;
	nl_cb_err(cb_clone, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb_clone, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb_clone, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	if (valid_handler)
		nl_cb_set(cb_clone, NL_CB_VALID, NL_CB_CUSTOM,
			  valid_handler, valid_data);

	memset(&fds, 0, sizeof(fds));
	fds.fd = nl_socket_get_fd(nl);
	fds.events = POLLIN;
	while (err > 0) {
		int res = poll(&fds, 1, -1);

		if (res < 0) {
			log_printf("poll error %d\n", res);
			break;
		}
		if (fds.revents & POLLIN) {
			res = nl_recvmsgs(nl, cb_clone);
			if (res < 0) {
				log_printf("nl_recvmsgs failed: %d\n", res);
				break;
			}
		}
	}

	nl_cb_put(cb_clone);
	return err;
}

/**
 * get a multicast group id for registering
 * (such as for vendor events)
 */
static int nl_get_multicast_id(struct ftmtest_state *state,
			       const char *family, const char *group)
{
	struct nl_msg *msg;
	int ret;
	struct family_data res = { group, -ENOENT };

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;
	if (!genlmsg_put(msg, 0, 0, genl_ctrl_resolve(state->nl, "nlctrl"),
			 0, 0, CTRL_CMD_GETFAMILY, 0) ||
	    nla_put_string(msg, CTRL_ATTR_FAMILY_NAME, family)) {
		nlmsg_free(msg);
		return -ENOBUFS;
	}

	ret = nl_cmd_send_and_recv(state->nl, state->cb, msg,
				   family_handler, &res);
	if (ret == 0)
		ret = res.id;

	return ret;
}

/**
 * add a multicast group to an NL socket so it
 * can receive events for that group
 */
int nl_socket_add_membership(struct nl_sock *sk, int group_id)
{
	int err;
	int s_fd = nl_socket_get_fd(sk);

	if (s_fd == -1 || group_id < 0)
		return -EINVAL;

	err = setsockopt(s_fd, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP,
			 &group_id, sizeof(group_id));

	if (err < 0)
		return err;

	return 0;
}

/**
 * convert struct timespec to milliseconds
 */
static uint64_t timespec_to_ms(struct timespec *ts)
{
	return ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
}

/**
 * handler for NL80211_CMD_GET_WIPHY results
 */
static int wiphy_info_handler(struct nl_msg *msg, void* arg)
{
	struct ftmtest_state *state = (struct ftmtest_state *)arg;
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *attr;
	struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	struct nl80211_vendor_cmd_info *cmd;
	int tmp;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh,0) , NULL);

	if (tb[NL80211_ATTR_VENDOR_DATA]) {
		nla_for_each_nested(attr, tb[NL80211_ATTR_VENDOR_DATA], tmp) {
			if (nla_len(attr) != sizeof(*cmd)) {
				log_printf("unexpected vendor cmd info\n");
				continue;
			}
			cmd = nla_data(attr);
			if (cmd->vendor_id == OUI_QCA &&
			    cmd->subcmd ==
			    QCA_NL80211_VENDOR_SUBCMD_LOC_GET_CAPA) {
				state->has_loc_api = true;
				break;
			}
		}
	}
	return NL_SKIP;
}

/**
 * handler for QCA_NL80211_VENDOR_SUBCMD_FTM_GET_CAPA results
 */
static int ftm_capa_info_handler(struct nl_msg *msg, void* arg)
{
	struct ftmtest_state *state = (struct ftmtest_state *)arg;
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb1[QCA_WLAN_VENDOR_ATTR_MAX + 1];
	struct nlattr *tb2[QCA_WLAN_VENDOR_ATTR_LOC_CAPA_MAX + 1];
	struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh,0) , NULL);

	if (!tb[NL80211_ATTR_VENDOR_DATA])
		return NL_SKIP;

	if (nla_parse_nested(tb1, QCA_WLAN_VENDOR_ATTR_MAX,
			     tb[NL80211_ATTR_VENDOR_DATA],
			     nl80211_loc_policy)) {
		log_printf("%s: failed to parse vendor data\n", __func__);
		return NL_SKIP;
	}

	if (!tb1[QCA_WLAN_VENDOR_ATTR_LOC_CAPA])
		return NL_SKIP;

	if (nla_parse_nested(tb2, QCA_WLAN_VENDOR_ATTR_LOC_CAPA_MAX,
			     tb1[QCA_WLAN_VENDOR_ATTR_LOC_CAPA],
			     nl80211_loc_capa_policy)) {
		log_printf("%s: fail to parse location capabilities\n",
		       __func__);
		return NL_SKIP;
	}

	if (tb2[QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAGS])
		state->capa.flags = nla_get_u8(
			tb2[QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAGS]);
	if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_SESSIONS])
		state->capa.max_session = nla_get_u16(
			tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_SESSIONS]);
	if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_PEERS])
		state->capa.max_dest = nla_get_u16(
			tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_PEERS]);
	if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_BURSTS_EXP])
		state->capa.max_burst_exp = nla_get_u8(
		     tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_NUM_BURSTS_EXP]);
	if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_MEAS_PER_BURST])
		state->capa.max_meas_per_burst = nla_get_u8(
		     tb2[QCA_WLAN_VENDOR_ATTR_FTM_CAPA_MAX_MEAS_PER_BURST]);
	if (tb2[QCA_WLAN_VENDOR_ATTR_AOA_CAPA_SUPPORTED_TYPES])
		state->capa.aoa_types = nla_get_u32(
			tb2[QCA_WLAN_VENDOR_ATTR_AOA_CAPA_SUPPORTED_TYPES]);

	return NL_SKIP;
}

/**
 * handle for vendor events
 * monitors FTM results and detects when FTM session is done
 */
static int ftm_event_handler(struct nl_msg *msg, void *arg)
{
	struct ftmtest_state *state = (struct ftmtest_state *)arg;
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb2[QCA_WLAN_VENDOR_ATTR_MAX + 1];
	struct nlattr *tb3[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAX + 1];
	struct nlattr *tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_MAX + 1];
	struct nlattr *mres;
	struct genlmsghdr *gnlh = (struct genlmsghdr *)
		nlmsg_data(nlmsg_hdr(msg));
	int tmp, index;
	uint32_t cmd;
	uint64_t cookie;
	bool has_cookie = false;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0) , NULL);

	if (!tb[NL80211_ATTR_VENDOR_ID] ||
	    !tb[NL80211_ATTR_VENDOR_SUBCMD] ||
	    !tb[NL80211_ATTR_VENDOR_DATA])
		return NL_SKIP;

	if (nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]) != OUI_QCA)
		return NL_SKIP;

	if (nla_parse_nested(tb2, QCA_WLAN_VENDOR_ATTR_MAX,
			    tb[NL80211_ATTR_VENDOR_DATA], nl80211_loc_policy)) {
		log_printf("failed to parse FTM vendor command\n");
		return NL_SKIP;
	}

	cmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);
	log_printf("received vendor event: %u\n", cmd);
	switch (cmd) {
	case QCA_NL80211_VENDOR_SUBCMD_FTM_SESSION_DONE:
		if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE]) {
			cookie = nla_get_u64(
				tb2[QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE]);
			has_cookie = true;
		}
		if (!has_cookie || state->session_cookie != cookie) {
			log_printf("skipping FTM_SESSION_DONE, invalid cookie\n");
			if (has_cookie) {
				log_printf("expected %" PRIu64 " got %" PRIu64 "\n",
				       state->session_cookie, cookie);
			}
			return NL_SKIP;
		}
		if (tb2[QCA_WLAN_VENDOR_ATTR_LOC_SESSION_STATUS])
			state->session_result_code = nla_get_u32(
				tb2[QCA_WLAN_VENDOR_ATTR_LOC_SESSION_STATUS]);
		state->session_started = false;
		return NL_STOP;
	case QCA_NL80211_VENDOR_SUBCMD_FTM_MEAS_RESULT:
		if (tb2[QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE]) {
			cookie = nla_get_u64(
				tb2[QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE]);
			has_cookie = true;
		}
		if (!has_cookie || state->session_cookie != cookie) {
			log_printf("skipping FTM_MEAS_RESULT, invalid cookie\n");
			if (has_cookie) {
				log_printf("expected %" PRIu64 " got %" PRIu64 "\n",
				       state->session_cookie, cookie);
			}
			return NL_SKIP;
		}
		/* extract single measurement result */
		if (!tb2[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_PEER_RESULTS]) {
			log_printf("no peer results found\n");
			return NL_SKIP;
		}
		if (nla_parse_nested(
			tb3, QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAX,
			tb2[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_PEER_RESULTS],
			nl80211_ftm_peer_res_policy)) {
			log_printf("failed to parse peer results\n");
			return NL_SKIP;
		}
		if (tb3[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAC_ADDR])
			memcpy(state->mac_addr,
			       nla_data(tb3[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MAC_ADDR]),
			       ETH_ALEN);
		if (!tb3[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MEAS]) {
			log_printf("no measurements found\n");
			return NL_SKIP;
		}
		index = state->num_ftm_meas;
		nla_for_each_nested(
			mres,
			tb3[QCA_WLAN_VENDOR_ATTR_FTM_PEER_RES_MEAS],
			tmp) {
			if (index >= FTM_MAX_MEASUREMENTS) {
				log_printf("too many measurements, some were lost\n");
				break;
			}
			if (nla_parse_nested(
				tb4, QCA_WLAN_VENDOR_ATTR_FTM_MEAS_MAX,
				mres, nl80211_ftm_meas_res_policy)) {
				log_printf("fail to parse measurement\n");
				continue;
			}
			if (!tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T1] ||
			    !tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T2] ||
			    !tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T3] ||
			    !tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T4]) {
				log_printf("incomplete measurement data\n");
				continue;
			}
			state->ftm_meas[index].t1 = nla_get_u64(
				tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T1]);
			state->ftm_meas[index].t2 = nla_get_u64(
				tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T2]);
			state->ftm_meas[index].t3 = nla_get_u64(
				tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T3]);
			state->ftm_meas[index].t4 = nla_get_u64(
				tb4[QCA_WLAN_VENDOR_ATTR_FTM_MEAS_T4]);
			index++;
		}
		state->num_ftm_meas = index;
		return NL_SKIP;
	case QCA_NL80211_VENDOR_SUBCMD_AOA_MEAS_RESULT:
		if (tb2[QCA_WLAN_VENDOR_ATTR_MAC_ADDR])
			memcpy(state->mac_addr,
			       nla_data(tb2[QCA_WLAN_VENDOR_ATTR_MAC_ADDR]),
			       ETH_ALEN);
		if (tb2[QCA_WLAN_VENDOR_ATTR_LOC_SESSION_STATUS])
			state->aoa_session_result_code = nla_get_u32(
				tb2[QCA_WLAN_VENDOR_ATTR_LOC_SESSION_STATUS]);
		if (tb2[QCA_WLAN_VENDOR_ATTR_LOC_ANTENNA_ARRAY_MASK])
			state->antenna_array_mask = nla_get_u32(tb2[
				QCA_WLAN_VENDOR_ATTR_LOC_ANTENNA_ARRAY_MASK]);
		if (tb2[QCA_WLAN_VENDOR_ATTR_AOA_TYPE])
			state->res_aoa_type = nla_get_u32(
				tb2[QCA_WLAN_VENDOR_ATTR_AOA_TYPE]);
		if (tb2[QCA_WLAN_VENDOR_ATTR_AOA_MEAS_RESULT]) {
			uint32_t len = nla_len(
				tb2[QCA_WLAN_VENDOR_ATTR_AOA_MEAS_RESULT]);
			state->aoa_data = malloc(len);
			if (state->aoa_data) {
				memcpy(state->aoa_data,
				       nla_data(tb2[QCA_WLAN_VENDOR_ATTR_AOA_MEAS_RESULT]),
				       len);
				state->aoa_data_length = len;
			} else {
				log_printf("failed to allocate room for AOA measurement data\n");
			}
		}
		state->aoa_started = false;
		return NL_SKIP;
	default:
		log_printf("unknown event %d\n", cmd);
		break;
	}

	return NL_SKIP;
}

/**
 * destroy the structures for NL communication
 */
static void destroy_nl_globals(struct ftmtest_state *state)
{
	if (state->nl) {
		nl_socket_free(state->nl);
		state->nl = NULL;
	}
	if (state->nl_event) {
		nl_socket_free(state->nl_event);
		state->nl_event = NULL;
	}
	if (state->cb) {
		nl_cb_put(state->cb);
		state->cb = NULL;
	}
	state->nl80211_id = 0;
}

/**
 * initialize structures for NL communication
 */
static bool init_nl_globals(struct ftmtest_state *state)
{
	bool res = false;
	int rc;

	/* specify NL_CB_DEBUG instead of NL_CB_DEFAULT to
	   get defailed traces of NL messages */
	state->cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (state->cb == NULL) {
		log_printf("failed to allocate nl callback\n");
		goto out;
	}
	nl_cb_set(state->cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);

	state->nl = nl_socket_alloc_cb(state->cb);
	if (state->nl == NULL) {
		log_printf("failed to allocate nl handle\n");
		goto out;
	}

	if (genl_connect(state->nl)) {
		log_printf("failed to connect to nl handle\n");
		goto out;
	}

	state->nl80211_id = genl_ctrl_resolve(state->nl, "nl80211");
	if (state->nl80211_id < 0) {
		log_printf("failed to get nl80211 family, error %d\n",
		       state->nl80211_id);
		goto out;
	}

	state->nl_event = nl_socket_alloc_cb(state->cb);
	if (state->nl_event == NULL) {
		log_printf("failed to allocate nl handle for events\n");
		goto out;
	}

	if (genl_connect(state->nl_event)) {
		log_printf("failed to connect to nl_event handle\n");
		goto out;
	}

	/* register for receiving vendor events */
	rc = nl_get_multicast_id(state, "nl80211", "vendor");
	if (rc < 0) {
		log_printf("could not get vendor multicast id, err %d\n", rc);
		goto out;
	}

	rc = nl_socket_add_membership(state->nl_event, rc);
	if (rc < 0) {
		log_printf("could not register for vendor events, err %d\n", rc);
		goto out;
	}

	nl_socket_set_nonblocking(state->nl_event);

	nl_cb_set(state->cb, NL_CB_VALID, NL_CB_CUSTOM,
		  ftm_event_handler, state);

	res = true;
out:
	if(!res)
		destroy_nl_globals(state);
	return res;
}

/**
 * allocate an nl_msg for sending a command
 */
static struct nl_msg *allocate_nl_cmd_msg(int family, int ifindex,
					  int flags, uint8_t cmd)
{
	struct nl_msg *msg = nlmsg_alloc();

	if (!msg) {
		log_printf("failed to allocate nl msg\n");
		return NULL;
	}

	if (!genlmsg_put(msg,
			  0, // pid (automatic)
			  0, // sequence number (automatic)
			  family, // family
			  0, // user specific header length
			  flags, // flags
			  cmd, // command
			  0) // protocol version
	    ) {
		log_printf("failed to init msg\n");
		nlmsg_free(msg);
		return NULL;
	}

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, (uint32_t)ifindex)) {
		log_printf("failed to set interface index\n");
		nlmsg_free(msg);
		return NULL;
	}

	return msg;
}

/**
 * get basic indoor location capabilities for driver
 * using the NL80211_CMD_GET_WIPHY command
 */
static bool get_basic_loc_capabilities(struct ftmtest_state *state)
{
	bool res = false;
	int rc;
	/*
	 * get extended capabilities from driver to see
	 * if it has any FTM capabilities
	 */
	struct nl_msg *msg = allocate_nl_cmd_msg(state->nl80211_id,
						 state->ifindex,
						 NLM_F_DUMP,
						 NL80211_CMD_GET_WIPHY);
	if (msg == NULL) {
		log_printf("failed to allocate msg for GET_WIPHY\n");
		goto out;
	}

	if (nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP)) {
		log_printf("failed to set params for GET_WIPHY\n");
		goto out;
	}

	rc = nl_cmd_send_and_recv(state->nl, state->cb, msg,
				  wiphy_info_handler, state);
	if (rc) {
		log_printf("failed to send GET_WIPHY command, err %d\n", rc);
		goto out;
	}

	res = state->has_loc_api;
out:
	nlmsg_free(msg);
	return res;
}

/**
 * get and optionally print extra indoor location capabilities using
 * QCA_NL80211_VENDOR_SUBCMD_LOC_GET_CAPA command
 */
static bool get_loc_extra_capabilities(struct ftmtest_state *state, bool show)
{
	bool res = false;
	int rc;

	memset(&state->capa, 0, sizeof(state->capa));

	struct nl_msg *msg = allocate_nl_cmd_msg(state->nl80211_id,
						 state->ifindex,
						 0, NL80211_CMD_VENDOR);
	if (msg == NULL) {
		log_printf("failed to allocate msg for FTM_GET_CAPA\n");
		goto out;
	}

	if (nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, OUI_QCA) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_LOC_GET_CAPA)) {
		log_printf("unable to set params for LOC_GET_CAPA\n");
		goto out;
	}

	rc = nl_cmd_send_and_recv(state->nl, state->cb, msg,
				  ftm_capa_info_handler, state);
	if (rc) {
		log_printf("failed to send LOC_GET_CAPA command, err %d\n", rc);
		goto out;
	}

	state->is_initiator = state->capa.flags &
		QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAG_FTM_INITIATOR;
	state->is_responder = state->capa.flags &
		QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAG_FTM_RESPONDER;
	res = true;
	if (show) {
		log_printf("*** indoor location extra capabilities ***\n");
		log_printf("flags: 0x%x\n", state->capa.flags);
		log_printf("max number of sessions: %d\n", state->capa.max_session);
		log_printf("max number of peers: %d\n", state->capa.max_dest);
		log_printf("max number of bursts exponent: %d\n",
		       state->capa.max_burst_exp);
		log_printf("max measurements per burst: %d\n",
		       state->capa.max_meas_per_burst);
		log_printf("AOA supported types: %d\n", state->capa.aoa_types);
	}

out:
	nlmsg_free(msg);
	return res;
}

/**
 * calculates the RTT from raw FTM measurements (t1,t2,t3,t4)
 * uses PPM compensation algorithm
 * On success return true and fill distance in cm derived from RTT
 */
static bool ftm_calculate_distance(struct ftmtest_state *state, double *distance)
{
	uint64_t t1[FTM_MAX_MEASUREMENTS], t2[FTM_MAX_MEASUREMENTS],
		 t3[FTM_MAX_MEASUREMENTS], t4[FTM_MAX_MEASUREMENTS];
	double ppmOffset[FTM_MAX_MEASUREMENTS], rttSum, tmpOffset, rtt;
	int i;

	if (state->num_ftm_meas < FTM_MIN_MEASUREMENTS) {
		log_printf("need atleast %d measurements for RTT, got %d\n",
		       FTM_MIN_MEASUREMENTS, state->num_ftm_meas);
		return 0.0;
	}

	for (i = 0; i < state->num_ftm_meas; i++) {
		t1[i] = state->ftm_meas[i].t1;
		t2[i] = state->ftm_meas[i].t2;
		t3[i] = state->ftm_meas[i].t3;
		t4[i] = state->ftm_meas[i].t4;
		if (t3[i] < t2[i] || t4[i] < t1[i]) {
			log_printf("bad result due to wraparound at %d\n", i);
			return false;
		}
	}

	for (i = 0; i < (state->num_ftm_meas - 1); i++) {
		if (t2[i + 1] != t2[i]) {
			tmpOffset = (double)(t1[i + 1] - t1[i]) /
				    (double)(t2[i + 1] - t2[i]);
			if (tmpOffset > FTM_RTT_PPM_OFFSET_THRESHOLD)
				tmpOffset = 1.0;
		} else {
			tmpOffset = 1.0;
		}
		ppmOffset[i + 1] = tmpOffset;
	}

	ppmOffset[0] = ppmOffset[1];

	rttSum = 0.0;
	for (i = 0; i < state->num_ftm_meas; i++)
		rttSum += ((double)(t4[i] - t1[i]) -
			ppmOffset[i] * (double)(t3[i] - t2[i]));
	rtt = rttSum / (double)state->num_ftm_meas;
	*distance = state->ftm_factor * 0.5 * SPEED_OF_LIGHT * rtt / 1000.0;

	return true;
}

/**
 * print AOA results
 * called from standalone AOA and AOA in FTM session
 */
static void ftm_print_aoa_results(struct ftmtest_state *state)
{
	int i, rc;
	double az, el, zpfactor;
	aoalib_adv_res_t ares;

	log_printf("AOA type: %d\n", state->res_aoa_type);
	log_printf("Antenna array mask: 0x%08X\n", state->antenna_array_mask);
	if (state->aoa_session_result_code == 0) {
		/* print the raw results */
		uint16_t *raw_data = (uint16_t *)state->aoa_data;
		int raw_data_length = state->aoa_data_length / sizeof(uint16_t);

		log_printf("mac address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		       state->mac_addr[0], state->mac_addr[1],
		       state->mac_addr[2], state->mac_addr[3],
		       state->mac_addr[4], state->mac_addr[5]);
		log_printf("Raw results:");
		for (i = 0; i < raw_data_length; i++) {
			if (i % 8 == 0)
				log_printf("\n");
			log_printf("0x%04X ", raw_data[i]);
		}
		log_printf("\n");
		/* run AOA algorithm and print azimuth and elevation */
		rc = aoa_process(state->aoalib_handle, state->aoa_data,
				 state->aoa_data_length, state->res_aoa_type,
				 state->antenna_array_mask, &az, &el,
				 &zpfactor);
		if (!rc) {
			if (state->verbose) {
				log_printf("Azimuth: %.1lf Elevation: %.1lf ZPFactor: %lf\n",
					   az, el, zpfactor);
				aoa_get_adv_res(state->aoalib_handle, &ares);
				log_printf("AZ1: %.1lf EL1: %.1lf AZ2: %.1lf EL2: %.1lf PR: %lf State: %d\n",
					   ares.primaryAz, ares.primaryEl,
					   ares.secondaryAz, ares.secondaryEl,
					   ares.peaksRatio, (int)ares.trackState);

			} else {
				log_printf("Azimuth: %lf Elevation: %lf\n",
					   az, el);
			}
		} else {
			log_printf("Error %d running AOA algorithm\n", rc);
		}
	}
}

/**
 * reset variables for FTM/AOA session
 */
static void ftm_free_one_shot_session(struct ftmtest_state *state)
{
	state->session_result_code = 0;
	state->num_ftm_meas = 0;
	free(state->aoa_data);
	state->aoa_data = NULL;
	state->aoa_data_length = 0;
	state->res_aoa_type = 0;
	state->antenna_array_mask = 0;
	state->aoa_session_result_code = 0;
}

/**
 * start an FTM session for a one-shot measurement,
 * wait for session done and print results
 */
static void ftm_run_one_shot_session(struct ftmtest_state *state,
				     void *arg)
{
	uint8_t *mac_addr = (uint8_t *)arg;
	struct nl_msg *msg;
	struct nlattr *vendor_data, *peers, *peer, *params;
	struct pollfd fds;
	int rc, i;
	double distance;

	/* allocate and fill in FTM_START_SESSION command */
	msg = allocate_nl_cmd_msg(state->nl80211_id,
				  state->ifindex, 0, NL80211_CMD_VENDOR);
	if (msg == NULL) {
		log_printf("failed to allocate msg for FTM_START_SESSION\n");
		goto out;
	}

	if (nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, OUI_QCA) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_FTM_START_SESSION)) {
		log_printf("unable to set FTM_START_SESSION params\n");
		goto out;
	}

	vendor_data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
	if (!vendor_data) {
		log_printf("fail nla_nest_start for vendor_data\n");
		goto out;
	}

	/* create a unique cookie from pid (22 bits) and a counter */
	state->session_cookie = (uint64_t)(getpid() & 0x3FFFFF) |
		(state->cookie_counter++ << 22);
	if (nla_put_u64(msg, QCA_WLAN_VENDOR_ATTR_FTM_SESSION_COOKIE,
			state->session_cookie)) {
		log_printf("unable to set session_cookie\n");
		goto out;
	}

	if (state->aoa_in_ftm_requested) {
		if (nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_AOA_TYPE,
				state->aoa_type)) {
			log_printf("unable to set parameters\n");
			goto out;
		}
	}

	peers = nla_nest_start(msg, QCA_WLAN_VENDOR_ATTR_FTM_MEAS_PEERS);
	if (!peers) {
		log_printf("fail nla_nest_start for peers\n");
		goto out;
	}

	/* only a single peer in session */
	peer = nla_nest_start(msg, 0);
	if (!peer) {
		log_printf("fail nla_nest_start for peer\n");
		goto out;
	}

	if (nla_put(msg, QCA_WLAN_VENDOR_ATTR_FTM_PEER_MAC_ADDR,
		    ETH_ALEN, mac_addr)) {
		log_printf("fail set peer mac address\n");
		goto out;
	}

	if (g_state.freq != 0 &&
	    nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_FTM_PEER_FREQ,
			g_state.freq)) {
		log_printf("fail to set peer frequency\n");
		goto out;
	}

	if (nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_FTM_PEER_MEAS_FLAGS,
			QCA_WLAN_VENDOR_ATTR_FTM_PEER_MEAS_FLAG_ASAP)) {
		log_printf("fail set peer measurement flags\n");
		goto out;
	}

	if (state->aoa_in_ftm_requested) {
		if (nla_put_u16(msg,
				QCA_WLAN_VENDOR_ATTR_FTM_PEER_AOA_BURST_PERIOD,
				1)) {
			log_printf("fail set AOA burst period\n");
			goto out;
		}
	}

	/* fill in measurement params for single-shot measurement,
	   500 ms delay before result */
	params = nla_nest_start(msg, QCA_WLAN_VENDOR_ATTR_FTM_PEER_MEAS_PARAMS);
	if (!params) {
		log_printf("fail nla_nest_start for params\n");
		goto out;
	}

	/* configure for single burst, need to request +1 measurement exchanges
	   to get our requested number of results. burst duration == 250us */
	if (nla_put_u8(msg,
		       QCA_WLAN_VENDOR_ATTR_FTM_PARAM_MEAS_PER_BURST,
		       state->num_ftm_meas_requested + 1) ||
	    nla_put_u8(msg,
		       QCA_WLAN_VENDOR_ATTR_FTM_PARAM_NUM_BURSTS_EXP, 0) ||
	    nla_put_u8(msg,
		       QCA_WLAN_VENDOR_ATTR_FTM_PARAM_BURST_DURATION,
		       FTM_BURST_DURATION_250_US) ||
	    nla_put_u16(msg,
			QCA_WLAN_VENDOR_ATTR_FTM_PARAM_BURST_PERIOD, 0)) {
		log_printf("fail to set peer params\n");
		goto out;
	}

	nla_nest_end(msg, params);
	nla_nest_end(msg, peer);
	nla_nest_end(msg, peers);
	nla_nest_end(msg, vendor_data);

	/* note, we send the FTM_START_SESSION command on the nl_event socket
	   not not on the nl socket for synchronized commands.
	   Currently FTM_MEAS_RESULT are sent as vendor events, so any client
	   that is registered for vendor events will receive them (supplicant
	   for example).
	   When there is a high rate of measurement events this is not very
	   efficient.
	   In the future we may move to a unicast model similar to RX
	   management frames, so measurement results will only be sent on the
	   socket that sent the FTM_START_SESSION command.
	   Basically it is possible to send a synchronized command on the
	   nl socket used for events, provided it is an "ack-only" command
	   (a command that returns success/failure without any data).
	   In this case you don't need to supply a valid_handler.
	   If the command returns data, you will need to supply a valid_handler
	   to process the data and this is more difficult, because your
	   valid_handler must handle other events that may be received
	   otherwise they can be missed.
	   Also make sure to send the command on the same thread that is
	   waiting for events on this socket.
	*/
	state->session_started = true;
	state->num_ftm_meas = 0;
	log_printf("sending FTM_START_SESSION\n");
	rc = nl_cmd_send_and_recv_nonblock(state->nl_event, state->cb,
					   msg, NULL, NULL);
	if (rc) {
		log_printf("failed to send FTM_START_SESSION command, err %d\n",
		       rc);
		goto out;
	}

	/* wait for events, until FTM_SESSION_DONE arrives and session is
	   complete. Typically this loop should run in a separate thread */
	memset(&fds, 0, sizeof(fds));
	fds.fd = nl_socket_get_fd(state->nl_event);
	fds.events = POLLIN;
	log_printf("waiting for results...\n");
	while (state->session_started) {
		int res = poll(&fds, 1, -1);

		if (res < 0) {
			log_printf("poll err %d waiting for events\n", res);
			goto out;
		}
		if (fds.revents & POLLIN) {
			res = nl_recvmsgs(state->nl_event, state->cb);
			if (res < 0) {
				log_printf("nl_recvmsgs failed: %d\n", res);
				/* something is wrong with the events socket */
				goto out;
			}
		}
	}

	/* print results */
	log_printf("*** session completed ***\n");
	log_printf(" session complete status: %d\n", state->session_result_code);
	if (state->session_result_code == 0) {
		log_printf(" mac address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		       state->mac_addr[0], state->mac_addr[1],
		       state->mac_addr[2], state->mac_addr[3],
		       state->mac_addr[4], state->mac_addr[5]);
		if (state->num_ftm_meas > 0) {
			for (i = 0; i < state->num_ftm_meas; i++) {
				log_printf("[%d]: t1: %" PRIu64 " t2: %" PRIu64
				       " t3: %" PRIu64 " t4: %" PRIu64 "\n",
				       i, state->ftm_meas[i].t1,
				       state->ftm_meas[i].t2,
				       state->ftm_meas[i].t3,
				       state->ftm_meas[i].t4);
			}
			if (ftm_calculate_distance(state, &distance))
				log_printf("Distance(cm): %lf\n", distance);
			else
				log_printf("Distance calculation failed\n");
		} else {
			log_printf("strange, no results reported\n");
		}
		if (state->aoa_in_ftm_requested) {
			log_printf("*** AOA measurement reported ***\n");
			log_printf(" AOA session complete status: %d\n", state->aoa_session_result_code);
			ftm_print_aoa_results(state);
		}
	}
out:
	nlmsg_free(msg);
	ftm_free_one_shot_session(state);
}

/**
 * measure AOA with a single peer
 * send the AOA_MEAS command, wait for AOA_MEAS_RESULT event,
 * run the AOA algorithm on the result and print resulting
 * azimuth and elevation
 */
static void ftm_run_aoa_meas(struct ftmtest_state *state, void *arg)
{
	uint8_t *mac_addr = (uint8_t *)arg;
	struct nl_msg *msg;
	struct nlattr *vendor_data;
	struct pollfd fds;
	int rc;

	log_printf("AOA measurement to %02X:%02X:%02X:%02X:%02X:%02X\n",
	       mac_addr[0], mac_addr[1], mac_addr[2],
	       mac_addr[3], mac_addr[4], mac_addr[5]);

	/* allocate and fill in AOA_MEAS command */
	msg = allocate_nl_cmd_msg(state->nl80211_id,
				  state->ifindex, 0, NL80211_CMD_VENDOR);
	if (msg == NULL) {
		log_printf("failed to allocate msg for AOA_MEAS\n");
		goto out;
	}

	if (nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, OUI_QCA) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_AOA_MEAS)) {
		log_printf("unable to set AOA_MEAS params\n");
		goto out;
	}

	vendor_data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
	if (!vendor_data) {
		log_printf("fail nla_nest_start for vendor_data\n");
		goto out;
	}

	if (nla_put(msg, QCA_WLAN_VENDOR_ATTR_MAC_ADDR,
		    ETH_ALEN, mac_addr) ||
	    (g_state.freq != 0 &&
	     nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_FREQ, g_state.freq)) ||
	    nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_AOA_TYPE,
			g_state.aoa_type)) {
		log_printf("unable to set parameters\n");
		goto out;
	}

	nla_nest_end(msg, vendor_data);

	state->aoa_started = true;

	log_printf("sending AOA_MEAS\n");
	rc = nl_cmd_send_and_recv_nonblock(state->nl_event, state->cb,
					   msg, NULL, NULL);
	if (rc) {
		log_printf("failed to send AOA_MEAS command, err %d\n",
		       rc);
		goto out;
	}

	/* wait for events, until AOA_MEAS_RESULT arrives
	   Typically this loop should run in a separate thread */
	memset(&fds, 0, sizeof(fds));
	fds.fd = nl_socket_get_fd(state->nl_event);
	fds.events = POLLIN;
	log_printf("waiting for results...\n");
	while (state->aoa_started) {
		int res = poll(&fds, 1, -1);

		if (res < 0) {
			log_printf("poll err %d waiting for events\n", res);
			goto out;
		}
		if (fds.revents & POLLIN) {
			res = nl_recvmsgs(state->nl_event, state->cb);
			if (res < 0) {
				log_printf("nl_recvmsgs failed: %d\n", res);
				/* something is wrong with the events socket */
				goto out;
			}
		}
	}

	/* print results */
	log_printf("*** AOA measurement completed ***\n");
	log_printf(" session complete status: %d\n", state->aoa_session_result_code);
	ftm_print_aoa_results(state);
out:
	nlmsg_free(msg);
	ftm_free_one_shot_session(state);
}

/**
 * Run a measurement loop (either AOA or FTM)
 * Perform <count> iterations with <delay> milliseconds delay in-between.
 * If timeout > 0, abort the loop if a measurement took longer than
 * <timeout> milliseconds.
 * The <measure> function runs the actual measurement (FTM or AOA)
 */
void ftm_run_meas_loop(struct ftmtest_state *state,
		       int count, int delay, int timeout,
		       void (*measure)(struct ftmtest_state *state, void *arg),
		       void *arg)
{
	struct timespec req, before, after;
	uint64_t delta;

	req.tv_sec = delay / 1000;
	req.tv_nsec = (delay % 1000) * 1000000;
	while (count > 0) {
		clock_gettime(CLOCK_REALTIME, &before);
		measure(state, arg);
		clock_gettime(CLOCK_REALTIME, &after);
		delta = timespec_to_ms(&after) - timespec_to_ms(&before);
		log_printf("Measurement took %" PRIu64 " milliseconds\n", delta);
		if (timeout > 0 && delta > (uint32_t)timeout) {
			log_printf("MEASUREMENT TOOK TOO LONG, ABORTING\n");
			break;
		}
		if (delay > 0)
			nanosleep(&req, NULL);
		count--;
	}
}

/**
 * extract common measurement arguments from command line
 *
 */
static bool ftm_extract_meas_cmd_args(int argc, char *argv[],
				      struct ftm_cmd_meas_args *args)
{
	int res;

	if (argc < 1)
		return false;

	memset(args, 0, sizeof(*args));
	args->count = 1;

	/* extract the MAC address */
	res = sscanf(argv[0], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
	             &args->mac_addr[0], &args->mac_addr[1],
		     &args->mac_addr[2], &args->mac_addr[3],
		     &args->mac_addr[4], &args->mac_addr[5]);
	if (res != 6) {
		log_printf("invalid MAC address\n");
		return false;
	}

	if (argc >= 3) {
		args->count = atoi(argv[1]);
		args->delay = atoi(argv[2]);
		if (argc == 4)
			args->timeout = atoi(argv[3]);
		if (args->count >= INT32_MAX || args->delay >= INT32_MAX ||
		    args->timeout >= INT32_MAX) {
			log_printf("invalid measurement count/delay/timeout\n");
			return false;
		}
	}

	return true;
}

void ftm_cmd_capa_help(void)
{
	log_printf("capa - show location capabilities\n");
}

void ftm_cmd_capa(struct ftmtest_state *state, int argc, char *argv[])
{
	if (get_basic_loc_capabilities(state))
		get_loc_extra_capabilities(state, true);
}

void ftm_cmd_run_ftm_help(void)
{
	log_printf("ftm <mac_addr> [<count> <delay> [<timeout>]]: perform single-shot FTM measurement\n");
	log_printf("   if count and delay are specified, perform <count> measurements\n");
	log_printf("   delaying <delay> milliseconds in-between\n");
	log_printf("   if timeout is specified, abort the loop if a measurement took over <timeout> milliseconds\n");
	log_printf("   example: ftm 01:02:03:04:05:06 10 500 1000\n");
}

void ftm_cmd_run_ftm(struct ftmtest_state *state, int argc, char *argv[])
{
	struct ftm_cmd_meas_args args;

	if (argc != 1 && argc != 3 && argc != 4) {
		log_printf("command usage:\n");
		ftm_cmd_run_ftm_help();
		return;
	}

	if (!ftm_extract_meas_cmd_args(argc, argv, &args))
		return;

	/* get capabilities (without printing) to see if driver supports FTM */
	if (get_basic_loc_capabilities(state))
		get_loc_extra_capabilities(state, false);

	if (!state->has_loc_api) {
		log_printf("driver does not support FTM\n");
		return;
	}
	if (!state->is_initiator) {
		log_printf("driver cannot run FTM session\n");
		return;
	}

	ftm_run_meas_loop(state, args.count, args.delay, args.timeout,
			  ftm_run_one_shot_session, args.mac_addr);
}

void ftm_cmd_run_aoa_help(void)
{
	log_printf("aoa <mac_addr> [<count> <delay> [<timeout>]]: perform AOA measurement\n");
	log_printf("   if count and delay are specified, perform <count> measurements\n");
	log_printf("   delaying <delay> milliseconds in-between\n");
	log_printf("   if timeout is specified, abort the loop if a measurement took over <timeout> milliseconds\n");
	log_printf("   example: aoa 01:02:03:04:05:06 10 500 1000\n");
}

void ftm_cmd_run_aoa(struct ftmtest_state *state, int argc, char *argv[])
{
	struct ftm_cmd_meas_args args;

	if (argc != 1 && argc != 3 && argc != 4) {
		log_printf("command usage:\n");
		ftm_cmd_run_aoa_help();
		return;
	}

	if (!ftm_extract_meas_cmd_args(argc, argv, &args))
		return;

	/* get capabilities (without printing) to see if driver supports AOA */
	if (get_basic_loc_capabilities(state))
		get_loc_extra_capabilities(state, false);

	if (!state->has_loc_api ||
	    !(state->capa.flags & QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAG_AOA)) {
		log_printf("driver does not support AOA\n");
		return;
	}

	ftm_run_meas_loop(state, args.count, args.delay, args.timeout,
			  ftm_run_aoa_meas, args.mac_addr);

}

void ftm_cmd_run_ftmaoa_help(void)
{
	log_printf("ftmaoa <mac_addr> [<count> <delay> [<timeout>]]: perform FTM followed by AOA measurement\n");
	log_printf("   this is mainly useful for stress tests\n");
	log_printf("   if count and delay are specified, perform <count> FTM+AOA measurements\n");
	log_printf("   delaying <delay> milliseconds in-between\n");
	log_printf("   if timeout is specified, abort the loop if a measurement took over <timeout> milliseconds\n");
	log_printf("   example: ftmaoa 01:02:03:04:05:06 10 500 1000\n");
}

void ftm_cmd_run_ftmaoa(struct ftmtest_state *state, int argc, char *argv[])
{
	struct ftm_cmd_meas_args args;

	if (argc != 1 && argc != 3 && argc != 4) {
		log_printf("command usage:\n");
		ftm_cmd_run_ftmaoa_help();
		return;
	}

	if (!ftm_extract_meas_cmd_args(argc, argv, &args))
		return;

	/* get capabilities (without printing) to see if driver supports FTM */
	if (get_basic_loc_capabilities(state))
		get_loc_extra_capabilities(state, false);

	if (!state->has_loc_api) {
		log_printf("driver does not support location API\n");
		return;
	}
	if (!state->is_initiator) {
		log_printf("driver cannot run FTM session\n");
		return;
	}

	if (!(state->capa.flags & QCA_WLAN_VENDOR_ATTR_LOC_CAPA_FLAG_AOA)) {
		log_printf("driver does not support AOA\n");
		return;
	}

	state->aoa_in_ftm_requested = true;

	ftm_run_meas_loop(state, args.count, args.delay, args.timeout,
			  ftm_run_one_shot_session, args.mac_addr);
}

/**
 * available commands
 */
static struct ftmtest_cmd {
	const char *name;
	bool with_aoa;
	void (*handler)(struct ftmtest_state *state, int argc, char *argv[]);
	void (*print_help)(void);
} ftmtest_cmds[] = {
	{ "capa", false, ftm_cmd_capa, ftm_cmd_capa_help },
	{ "ftm", false, ftm_cmd_run_ftm, ftm_cmd_run_ftm_help},
	{ "aoa", true, ftm_cmd_run_aoa, ftm_cmd_run_aoa_help},
	{ "ftmaoa", true, ftm_cmd_run_ftmaoa, ftm_cmd_run_ftmaoa_help},
	{ NULL, false, NULL, NULL } /* keep last */
};

int main(int argc, char *argv[])
{
	const char *ifname;
	const char *name;
	const char *cldb_fname;
	struct ftmtest_cmd *cmd;
	bool handled = false;
	int f, optidx = 0;
	static const struct option options[] = {
		{ "aoatype", required_argument, 0, 'a' },
		{ "ftm-meas", required_argument, 0, 'n' },
		{ "ftm-factor", required_argument, 0, 'r' },
		{ "verbose", no_argument, 0, 'v' },
		{ 0,0,0,0 }
	};

	memset(&g_state, 0, sizeof(g_state));
	g_state.ftm_factor = 1.0;
	g_state.num_ftm_meas_requested = FTM_MIN_MEASUREMENTS;
	cldb_fname = AOA_DEFAULT_CLDB_FNAME;
	while ((f = getopt_long(argc, argv, "a:f:c:n:r:v", options, &optidx)) != EOF)
		switch(f) {
		case 'a':
			g_state.aoa_type = atoi(optarg);
			break;
		case 'f':
			cldb_fname = optarg;
			break;
		case 'c':
			g_state.freq = atoi(optarg);
			break;
		case 'n':
			g_state.num_ftm_meas_requested = atoi(optarg);
			break;
		case 'r':
			g_state.ftm_factor = atof(optarg);
			break;
		case 'v':
			g_state.verbose = true;
			break;
		default:
			fprintf(stderr, "Unknown option '%c'\n", f);
			goto help;
		}

	argc -= optind;
	argv = &argv[optind];

	if (argc < 2) {
help:
		log_printf("usage: ftmtest [options] interface_name cmd [args]\n");
		log_printf("options:\n");
		log_printf(" -a <type> : AOA type\n");
		log_printf(" -f <filename> : Specify AOA calibration database (default %s)\n",
			   AOA_DEFAULT_CLDB_FNAME);
		log_printf(" -c <freq>: Force FTM/AOA peer listening frequency (MHz)\n");
		log_printf(" -n <value>: Request <value> FTM measurements (default 3)\n");
		log_printf(" -r <factor> : Multiple FTM distance by factor (workaround for FW bug)\n");
		log_printf(" -v : print additional results\n");
		log_printf("possible commands:\n");
		for (cmd = ftmtest_cmds; cmd->print_help; cmd++)
			cmd->print_help();
		return -1;
	}

	ifname = argv[0];
	name = argv[1];

	argc -= 2;
	argv += 2;

	g_state.ifindex = if_nametoindex(ifname);
	if (g_state.ifindex == 0) {
		log_printf("unknown interface %s\n", ifname);
		return -1;
	}
	log_printf("interface index of %s is %d\n", ifname, g_state.ifindex);

	if (!init_nl_globals(&g_state))
		return -1;

	for (cmd = ftmtest_cmds; cmd->name; cmd++) {
		if (!strcmp(name, cmd->name)) {
			if (cmd->with_aoa && !g_state.aoalib_handle) {
				g_state.aoalib_handle = aoa_init(
					cldb_fname, g_state.aoa_type);
				if (!g_state.aoalib_handle) {
					log_printf("fail to init AOA algorithm library, error %d file %s\n",
						   errno, cldb_fname);
					goto out;
				}
			}
			cmd->handler(&g_state, argc, argv);
			handled = true;
			break;
		}
	}

	if (!handled)
		log_printf("unknown command: %s\n", name);

	destroy_nl_globals(&g_state);
	if (g_state.aoalib_handle) {
		aoa_deinit(g_state.aoalib_handle);
		g_state.aoalib_handle = NULL;
	}
	return 0;
out:
	destroy_nl_globals(&g_state);
	return -1;
}

