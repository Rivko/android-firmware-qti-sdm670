/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
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
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <poll.h>
#include <pthread.h>
#include <stdbool.h>

#include "debug.h"
#include "cnss_gw_update.h"
#include "qca-vendor_copy.h"

static struct cnss_gw_update gw_update_info;
static struct cnss_gw_data gw_data;

static int nud_stats_rsp_handler (struct nl_msg *msg, void *arg);
static int nl_response_handler(struct nl_msg *msg, void *arg);

/* utility function - print gateway params */
static void cnss_print_gw_params()
{
	uint32_t i;
	char ipaddr[64] = { 0 };
	struct cnss_gw_info *new_gw_info = &gw_update_info.new_gw_info;

	/* ipv4 gateway */
	if (AF_INET == new_gw_info->ipv4_type) {
		inet_ntop(AF_INET, new_gw_info->ip_addr,
			ipaddr, INET_ADDRSTRLEN);
		wsvc_printf_dbg("gateway ipv4 address: %s\n", ipaddr);
	}

	/* ipv6 gateway */
	if (AF_INET6 == new_gw_info->ipv6_type) {
		inet_ntop(AF_INET6, new_gw_info->ip_addr,
			ipaddr, INET6_ADDRSTRLEN);
		wsvc_printf_dbg("gateway ipv6 address: %s\n", ipaddr);
	}

	wsvc_printf_mac_addr(MSG_DEBUG, "gateway mac address",
			     new_gw_info->mac_addr);
}

/* function to extract gateway ip and mac addresses */
static void cnss_get_gw_params(struct cnss_gw_data *gw_data)
{
	uint32_t i, gw_addr[16] = { 0 };
	char ip_arr[16] = { 0 };
	char ip_str[64] = { 0 };
	int8_t mac_addr[ETH_ALEN] = { 0 };
	struct cnss_gw_info *new_gw_info = &gw_update_info.new_gw_info;
	struct route_entry_tag *rentry;
	struct arp_entry *aentry;

	for(i = 0; i < MAX_COUNT; i++) {

		rentry = &gw_data->r_table.r_entry[i];

		/* any entry which has ifname = wlan0 and gw_addr populated */
		if ((strcmp(rentry->ifname, "wlan0") == 0) &&
			rentry->gw_addr_len != 0) {

			/* copy gw ip address */
			memcpy(new_gw_info->ip_addr, rentry->gw_addr,
				rentry->gw_addr_len);

			/* if we have ipv4 gateway */
			if (AF_INET == new_gw_info->ipv4_type) {
				/* translate ip address to byte array */
				inet_ntop(AF_INET, new_gw_info->ip_addr,
					&ip_str[0], INET_ADDRSTRLEN);

				/* convert ip address string to byte array */
				inet_pton(AF_INET, &ip_str[0], &ip_arr[0]);

				memcpy(&new_gw_info->ipv4_addr[0],
					&ip_arr[0], IPV4_NUM_BYTES);
				/* Cache the ip to monitor for NUD events */
				memcpy(&gw_data->nud_sm.ipv4_addr[0],
					&ip_arr[0], IPV4_NUM_BYTES);
			}

			/* if we have ipv6 gateway */
			if (AF_INET6 == new_gw_info->ipv6_type) {
				/* translate ip address to byte array */
				inet_ntop(AF_INET6, new_gw_info->ip_addr,
					&ip_str[0], INET6_ADDRSTRLEN);

				/* convert ip address string to byte array */
				inet_pton(AF_INET6, &ip_str[0], &ip_arr[0]);

				memcpy(&new_gw_info->ipv6_addr[0],
					&ip_arr[0], IPV6_NUM_BYTES);
			}

			/* also update iface idx */
			new_gw_info->ifidx = if_nametoindex(rentry->ifname);
		}
	}

	/* get gw mac addr from the arp table */
	for (i = 0; i < MAX_COUNT; i++) {
		/* if ip addr matches with gateway ip address indicated in
		 * route update and the corresponding mac address is non-null
		 * which is recorded from the neighbor response message
		 * "mac_addr" is all zeroes here
		 */
		aentry = &gw_data->a_table.a_entry[i];
		if (strlen(ip_str) > 0 &&
		   (strcmp(ip_str, aentry->a_ipstr) == 0) &&
		   (memcmp(aentry->mac_addr, mac_addr, ETH_ALEN) != 0)) {
			memcpy(new_gw_info->mac_addr, aentry->mac_addr,
				ETH_ALEN);
		}
	}

	cnss_print_gw_params();
}

/* Extract ip address of neighbor from neighbor data */
static void get_ip_from_neighbor_data(struct rtattr *attr,
				      int len, uint32_t *ip)
{
	if (NULL == attr || len <= 0) {
		wsvc_printf_err("%s: neighbor attr list is empty\n", __func__);
		return;
	}

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
		if (attr->rta_type == NDA_DST) {
			*ip = *(uint32_t *)RTA_DATA(attr);
			return;
		}
	}
}

/* Return true if we are watching for the NUD events from ip_arr */
static bool is_watching(struct cnss_gw_data *gw_data, uint8_t *ip_arr)
{
	if (!memcmp(&gw_data->nud_sm.ipv4_addr[0], ip_arr, IPV4_NUM_BYTES))
		return true;
	return false;
}

/* Allocate and initialize nlmsg to send the vendor command vendor_cmd */
struct nl_msg * alloc_nlmsg(struct nud_stats_sm *nud_sm, int vendor_cmd)
{
	int id;
	struct nl_msg *nlmsg;
	void *msg_head = NULL;
	int ifindex = if_nametoindex("wlan0");

	nlmsg = nlmsg_alloc();
	if (NULL == nlmsg) {
		wsvc_printf_err("nl msg alloc failed\n");
		return NULL;
	}

	if (nud_sm->family_id == 0) {
		nud_sm->family_id =
		    genl_ctrl_resolve(gw_update_info.wifi_sockfd, "nl80211");
		if (nud_sm->family_id < 0) {
			wsvc_printf_err("nl ctrl resolve failed \n");
			goto fail;
		}
	}

	msg_head = genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0,
		/*family*/nud_sm->family_id, /* hdrlen*/ 0,/* flags*/0,
		/* cmd*/ NL80211_CMD_VENDOR, /* version = */ 0);

	if (NULL == msg_head) {
		wsvc_printf_err("genlmsg put failed\n");
		goto fail;
	}

	/* OUI goes first */
	if (nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, OUI_QCA) < 0) {
		wsvc_printf_err("nlmsg put failed for QCA OUI\n");
		goto fail;
	}

	/* Vendor subcmd */
	if (nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, vendor_cmd) < 0) {
		wsvc_printf_err("nlmsg put failed for vendor subcmd\n");
		goto fail;
	}

	/* iface on which the message is to be sent */
	wsvc_printf_dbg("ifindex: %d\n", ifindex);
	if (nla_put_u32(nlmsg, NL80211_ATTR_IFINDEX, ifindex) < 0) {
		wsvc_printf_err("nlmsg put failed for ifindex\n");
		goto fail;
	}

	return nlmsg;
fail:
	nlmsg_free(nlmsg);
	return NULL;
}

/* Send nlmsg to kernel and wait for response */
static int send_recv_nlmsg(struct nl_msg *nlmsg, struct nl_cb *cb)
{
	int err;

	/* send auto complete */
	err = nl_send_auto_complete(gw_update_info.wifi_sockfd, nlmsg);
	if (err < 0) {
		wsvc_printf_err("nl auto complete err: %d\n", err);
		return err;
	}

	err = nl_recvmsgs(gw_update_info.wifi_sockfd, cb);
	if (err)
		wsvc_printf_err("nl_recvmsgs failed: %d\n", err);

	return err;
}

/* Populate nud stats set data to nlmsg as vendor data */
static int populate_nud_set_data(struct nl_msg *nlmsg, uint32_t gw_addr)
{
	struct nlattr *params;

	params = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
	if (NULL == params) {
		wsvc_printf_err("nla nest start failed\n");
		return -EMSGSIZE;
	}

	if (nla_put_flag(nlmsg, QCA_ATTR_NUD_STATS_SET_START) ||
		nla_put_u32(nlmsg, QCA_ATTR_NUD_STATS_GW_IPV4, gw_addr)) {
		wsvc_printf_err("nla put failed for SET_START/GW_IPV4\n");
		return -ENOMEM;
	}

	nla_nest_end(nlmsg, params);

	return 0;
}

/* Populate nud stats get data to nlmsg as vendor data */
static int populate_nud_get_data(struct nl_msg *nlmsg)
{
	struct nlattr *params;

	params = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
	if (NULL == params) {
		wsvc_printf_err("nla nest start failed\n");
		return -EMSGSIZE;
	}
	nla_nest_end(nlmsg, params);
	return 0;
}

/* Populate nud stats clear data to nlmsg as vendor data */
static int populate_nud_clear_data(struct nl_msg *nlmsg)
{
	struct nlattr *params;

	params = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
	if (NULL == params) {
		wsvc_printf_err("nla nest start failed\n");
		return -EMSGSIZE;
	}
	nla_nest_end(nlmsg, params);
	return 0;
}

/* Flush the nud stats collected in driver/firmware */
static int clear_nud_stats(struct cnss_gw_data *gw_data)
{
	struct nl_msg *nlmsg;
	int ret = -1;

	wsvc_printf_dbg("NUD_STATS CLEAR\n");
	//Flush the NUD stats by sending nl80211 vendor command
	nlmsg = alloc_nlmsg(&gw_data->nud_sm,
			QCA_NL80211_VENDOR_SUBCMD_NUD_STATS_SET);
	if (!nlmsg)
		return ret;
	ret = populate_nud_clear_data(nlmsg);
	if (ret < 0)
		goto end;
	ret = send_recv_nlmsg(nlmsg, gw_data->g_cb);
end:
	nlmsg_free(nlmsg);
	return ret;
}

/* Cache the ip from nud probe and start monitoring for nud_events from it */
static int handle_nud_probe(struct cnss_gw_data *gw_data,struct rtattr *attr,
			     int len)
{
	struct nl_msg *nlmsg;
	uint32_t ipv4_addr;
	uint8_t ip_arr[16] = { 0 };
	char ip_str[64] = { 0 };
	int ret = -1;

	get_ip_from_neighbor_data(attr, len, &ipv4_addr);

	/* translate ip address to byte array */
	inet_ntop(AF_INET, (void *)&ipv4_addr, &ip_str[0], INET_ADDRSTRLEN);

	/* convert ip address string to byte array */
	inet_pton(AF_INET, &ip_str[0], &ip_arr[0]);
	memcpy(&gw_data->nud_sm.ipv4_addr[0], &ip_arr[0], IPV4_NUM_BYTES);

	wsvc_printf_dbg("collect NUD_STATS for ip : %s\n", ip_str);

	nlmsg = alloc_nlmsg(&gw_data->nud_sm,
			QCA_NL80211_VENDOR_SUBCMD_NUD_STATS_SET);
	if (!nlmsg)
		return ret;
	ret = populate_nud_set_data(nlmsg, ipv4_addr);
	if (ret < 0)
		goto end;
	ret = send_recv_nlmsg(nlmsg, gw_data->g_cb);
	if (ret < 0)
		wsvc_printf_err("Failed to send_recv_nlmsg : \n");
end:
	nlmsg_free(nlmsg);
	return ret;
}

/* Flush the stats if the ip is being monitored for nud events */
static int handle_nud_reachable(struct cnss_gw_data *gw_data,
				 struct rtattr *attr, int len)
{
	uint32_t ipv4_addr;
	uint8_t ip_arr[16] = { 0 };
	char ip_str[64] = { 0 };
	int ret = -1;

	get_ip_from_neighbor_data(attr, len, &ipv4_addr);

	/* translate ip address to byte array */
	inet_ntop(AF_INET, (void *)&ipv4_addr, &ip_str[0], INET_ADDRSTRLEN);

	/* convert ip address string to byte array */
	inet_pton(AF_INET, &ip_str[0], &ip_arr[0]);

	if (!is_watching(gw_data, &ip_arr[0]))
		return ret;

	return clear_nud_stats(gw_data);
}

/* Get nud stats from driver if the ip is being monitored for nud events */
static int handle_nud_failed(struct cnss_gw_data *gw_data,
			     struct rtattr *attr, int len)
{
	struct nl_msg *nlmsg;
	uint32_t ipv4_addr;
	uint8_t ip_arr[16] = { 0 };
	char ip_str[64] = { 0 };
	int ret = -1;

	get_ip_from_neighbor_data(attr, len, &ipv4_addr);

	/* translate ip address to byte array */
	inet_ntop(AF_INET, (void *)&ipv4_addr, &ip_str[0], INET_ADDRSTRLEN);

	/* convert ip address string to byte array */
	inet_pton(AF_INET, &ip_str[0], &ip_arr[0]);

	if (!is_watching(gw_data, &ip_arr[0]))
		return ret;

	wsvc_printf_dbg("Get NUD_STATS collected\n");
	nlmsg = alloc_nlmsg(&gw_data->nud_sm,
			QCA_NL80211_VENDOR_SUBCMD_NUD_STATS_GET);
	if (!nlmsg)
		return ret;
	ret = populate_nud_get_data(nlmsg);
	if (ret < 0)
		goto end;
	nl_cb_set(gw_data->g_cb, NL_CB_VALID, NL_CB_CUSTOM,
		  nud_stats_rsp_handler, NULL);
	ret = send_recv_nlmsg(nlmsg, gw_data->g_cb);
	nl_cb_set(gw_data->g_cb, NL_CB_VALID, NL_CB_CUSTOM,
		  nl_response_handler, NULL);

end:
	nlmsg_free(nlmsg);
	return ret;
}

/* helper function to extract neighbor data from the ndm response */
static void __parse_neighbor_data(struct cnss_gw_data *gw_data,
	struct rtattr *attr, int ndm_family, int len, int idx)
{
	unsigned int ipaddr = 0;
	struct in_addr *inp = NULL;
	struct arp_entry *aentry = &gw_data->a_table.a_entry[idx];

	if (NULL == attr || len <= 0) {
		wsvc_printf_err("%s: neighbor attr list is empty\n", __func__);
		return;
	}

	/* flag to indicate the success */
	gw_data->neighbor_success = -1;

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
		wsvc_printf_dbg("attr type: 0x%x\n", attr->rta_type);

		if (attr->rta_type == NDA_LLADDR) {
			wsvc_printf_dbg("NDA_LLADDR received\n");
			memcpy(aentry->mac_addr, RTA_DATA(attr), ETH_ALEN);
			gw_data->neighbor_success++;  /* 0 */
			wsvc_printf_mac_addr(MSG_DEBUG, "neighbor",
					     aentry->mac_addr);
		}

		if (attr->rta_type == NDA_DST) {
			inp = (struct in_addr *) RTA_DATA(attr);
			aentry->ip_addr = *(unsigned long *)RTA_DATA(attr);

			/* ip string */
			inet_ntop(ndm_family, RTA_DATA(attr),
				aentry->a_ipstr,
				sizeof(aentry->a_ipstr));

			gw_data->neighbor_success++;  /* 1 */
			wsvc_printf_dbg("NDA_DST received: %s ul: %lu\n",
				aentry->a_ipstr, aentry->ip_addr);
		}
	}
}

/* extracts the nl attributes received with kernel response */
static void parse_neighbor_data(struct cnss_gw_data *gw_data,
	struct nlmsghdr *nlmsg, int idx)
{
	struct ndmsg *ndm;
	struct rtattr *attr;
	int len;
	char macaddr[ETH_ALEN] = { 0 };
	int status = 0;

	ndm = (struct ndmsg *)(NLMSG_DATA(nlmsg));

	if (ndm->ndm_family == AF_INET) {
		wsvc_printf_dbg("neighbor response rcvd for ipv4 neighbor\n");
	} else if (ndm->ndm_family == AF_INET6) {
		wsvc_printf_dbg("neighbor response rcvd for ipv6 neighbor\n");
	}

	attr = (struct rtattr *)(RTM_RTA(ndm));
	len = RTM_PAYLOAD(nlmsg);

	/* sometimes kernel ends up sending neighbors which are stale and
	 * unreachable, just skip them
	 */
	wsvc_printf_dbg("ndm_state: %d\n", ndm->ndm_state);
	switch (ndm->ndm_state)
	{
		case NUD_REACHABLE:
		case NUD_PERMANENT:
			/* parse neighbor data */
			__parse_neighbor_data(gw_data, attr,
				ndm->ndm_family, len, idx);
			if (gw_data->nud_sm.state == NUD_PROBE)
				status = handle_nud_reachable(gw_data, attr, len);
		break;
		case NUD_PROBE:
			status = handle_nud_probe(gw_data, attr, len);
		break;
		case NUD_FAILED:
			if (gw_data->nud_sm.state == NUD_PROBE)
				status = handle_nud_failed(gw_data, attr, len);
		break;
		default:
			wsvc_printf_dbg("Stale or unreachable neighbors,"
			                "ndm state: %d\n",
					ndm->ndm_state);
	}
	if (status == 0)
		gw_data->nud_sm.state = ndm->ndm_state;
}

/* parses kernel neighbor response */
static void parse_neighbor_response(struct cnss_gw_data *gw_data,
	char *buf, unsigned int len,
	void (neigh_data_handler_cb)(struct cnss_gw_data *, struct nlmsghdr *, int))
{
	int count = 0;

	struct nlmsghdr *nlmsg = (struct nlmsghdr *)buf;
	for (; NLMSG_OK(nlmsg, len); nlmsg = NLMSG_NEXT(nlmsg, len)) {
		/* parse neighbor data */
		neigh_data_handler_cb(gw_data, nlmsg, count);
		count++;
	}
}

/* helper function to process route attributes in a route indication */
static void __process_route_attributes(struct rtattr *rta,
	struct route_entry_tag *r_entry, uint8_t rtm_family)
{
	char    gw_addr[64] = { 0 };
	struct	cnss_gw_info *new_gw_info = &gw_update_info.new_gw_info;
	uint8_t data_len = (rtm_family == AF_INET) ? 4 : 16;
	uint8_t addr_str_len = (rtm_family == AF_INET) ?
					INET_ADDRSTRLEN :
					INET6_ADDRSTRLEN;

	/* Get the destination address */
	if (rta->rta_type == RTA_DST)
		r_entry->dst_addr = *(unsigned long *) RTA_DATA(rta);

	/* Get the gateway (Next hop) */
	if (rta->rta_type == RTA_GATEWAY) {

		memcpy(r_entry->gw_addr, RTA_DATA(rta), data_len);
		r_entry->gw_addr_len = data_len;

		inet_ntop(AF_INET, r_entry->gw_addr, gw_addr,
			addr_str_len);
		new_gw_info->ipv4_type =
			(rtm_family == AF_INET) ? AF_INET : 0;
		new_gw_info->ipv6_type =
			(rtm_family == AF_INET6) ? AF_INET6 : 0;
		new_gw_info->ip_len = addr_str_len;

		if (rtm_family == AF_INET)
			wsvc_printf_dbg("ip type is ipv4 %s\n", gw_addr);
		else
			wsvc_printf_dbg("ip type is ipv6 %s\n", gw_addr);

		memcpy(new_gw_info->ip_addr, r_entry->gw_addr, data_len);
	}

	/* interface name */
	if (rta->rta_type == RTA_OIF) {
		if_indextoname(*(int *)RTA_DATA(rta), r_entry->ifname);

		/* check if the gateway update is for wlan0 */
		if (strcmp(r_entry->ifname, "wlan0") == 0)
			strlcpy(new_gw_info->if_name, r_entry->ifname,
				sizeof(new_gw_info->if_name));

		wsvc_printf_dbg("interface name: %s\n", r_entry->ifname);
	}

	/* source address */
	if (rta->rta_type == RTA_PREFSRC)
		r_entry->src_addr = *(unsigned long *) RTA_DATA(rta);
}

/* helper function to process route entry */
static void __process_route_entry(struct cnss_gw_data *gw_data,
	struct rtmsg *route_entry, int len, int idx)
{
	int     route_attribute_len = len;
	struct  rtattr *route_attribute;

	if (NULL == route_entry) {
		wsvc_printf_err("invalid route entry\n");
		return;
	}

	/* Get attributes of route_entry */
	route_attribute = (struct rtattr *) RTM_RTA(route_entry);

	memset(&gw_data->r_table.r_entry[idx], 0,
			sizeof(struct route_entry_tag));

	/* process each route attribute in a route entry */
	for ( ; RTA_OK(route_attribute, route_attribute_len); \
		route_attribute =
			RTA_NEXT(route_attribute, route_attribute_len)) {

		/* process route attributes in a route entry */
		__process_route_attributes(route_attribute,
			&gw_data->r_table.r_entry[idx], route_entry->rtm_family);
	}
}

static int wlan_service_set_neigh_ucast_solicit(const char* str)
{
	char *ipv4_usolicit = "/proc/sys/net/ipv4/neigh/wlan0/ucast_solicit";
	char *ipv6_usolicit = "/proc/sys/net/ipv6/neigh/wlan0/ucast_solicit";

	return (wlan_service_update_sys_param (ipv4_usolicit, str) ||
		wlan_service_update_sys_param (ipv6_usolicit, str));
}

static int wlan_service_set_neigh_retrans_time_ms(const char* str)
{
	char *ipv4_time_ms = "/proc/sys/net/ipv4/neigh/wlan0/retrans_time_ms";
	char *ipv6_time_ms = "/proc/sys/net/ipv6/neigh/wlan0/retrans_time_ms";

	return (wlan_service_update_sys_param (ipv4_time_ms, str) ||
		wlan_service_update_sys_param (ipv6_time_ms, str));
}

/** Main loop to handle route and neighbor updates from the kernel */
int cnss_gw_update_loop(int sock)
{
	struct  nlmsghdr *nlh;
	struct  rtmsg *route_entry;
	struct  rtattr *route_attribute;
	char    buffer[CNSS_BUFSIZE] = { 0 };
	int     route_attribute_len = 0;
	int     status, size;
	int     count = 0;
	unsigned int received_bytes = 0;

	/* Receiving netlink socket data */
	received_bytes = recv(sock, buffer, sizeof(buffer), 0);
	wsvc_printf_dbg("%s: received bytes: %d\n", __func__, received_bytes);

	if (received_bytes <= 0)
		CNSS_ERR("recv");

	wsvc_hexdump("recvmsg NETLINK_ROUTE", buffer, received_bytes);
	/* cast the received buffer */
	nlh = (struct nlmsghdr *) buffer;
	wsvc_printf_dbg("%s: nlmsg_type: %d\n", __func__, nlh->nlmsg_type);

	/* If we received all data ---> break */
	if (nlh->nlmsg_type == RTM_NEWROUTE) {

		wsvc_printf_dbg("RTM_NEWROUTE Indication\n");

		/* this is a new route indication */
		gw_data.new_route = 1;
		count = (gw_data.r_table_idx < MAX_TABLE_SIZE) ?
			gw_data.r_table_idx++ : 0 ;

		memset(&gw_data.r_table.r_entry[count], 0,
			sizeof(struct route_entry_tag));

		for ( ; NLMSG_OK(nlh, received_bytes);
			nlh = NLMSG_NEXT(nlh, received_bytes)) {

			/* Get the route data */
			route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

			/* Get the route atttibutes len */
			route_attribute_len = RTM_PAYLOAD(nlh);

			/* process each route entry in the message */
			__process_route_entry(&gw_data, route_entry,
				route_attribute_len, count);
		}

		/* next entry in the routing table, updating global count */
		count++;
		gw_data.r_table_idx = count;
	} else if (nlh->nlmsg_type == RTM_NEWNEIGH ||
		nlh->nlmsg_type == RTM_GETNEIGH ||
		nlh->nlmsg_type == RTM_DELNEIGH) {

		int num_bytes = 0;
		wsvc_printf_dbg("RTM_NEWNEIGH message received: %d\n", nlh->nlmsg_type);

		/* parse new neighbor indication from kernel */
		parse_neighbor_response(&gw_data, buffer, received_bytes,
			&parse_neighbor_data);
	}

	/* try to update gw parameters if we have received new route indication
	 * and we have received new neighbor indication and we have successfully
	 * received neighbor mac address
	 */
	size = sizeof(gw_update_info.new_gw_info.ip_addr) /
			sizeof(gw_update_info.new_gw_info.ip_addr[0]);
	if (gw_data.new_route && gw_data.neighbor_success &&
		size > 0 &&
		(strcmp(gw_update_info.new_gw_info.if_name, "wlan0") == 0)) {

		wsvc_printf_info("Ready to update GW parameters");

		gw_data.new_route = 0;
		gw_data.neighbor_success = 0;

		/* populate gw parameters */
		cnss_get_gw_params(&gw_data);

		/* Do gw param update if not done already */
		cnss_update_wlan_drv(&gw_data);

		wlan_service_set_neigh_ucast_solicit(NUM_UNICAST_PROBES);
		wlan_service_set_neigh_retrans_time_ms(RETRANS_TIME_MS);

		/* update curr_gw_info */
		memcpy(&gw_update_info.curr_gw_info,
			&gw_update_info.new_gw_info,
			sizeof(struct cnss_gw_info));

		memset(&gw_update_info.new_gw_info, 0,
			sizeof(struct cnss_gw_info));
		memset(&gw_data.a_table, 0, sizeof(gw_data.a_table));
		memset(&gw_data.r_table, 0, sizeof(gw_data.r_table));
	}
	return 0;
}

/* Wifi NL socket interface */
static int nl_ack_handler(struct nl_msg *msg, void *arg) {

	int *err = (int *)arg;
	struct nl_msg *tmp = msg;

	*err = 0;
	wsvc_printf_dbg("nl ack handler invoked\n");
	return NL_STOP;
}

static int nl_finish_handler(struct nl_msg *msg, void *arg) {
	int *ret = (int *)arg;
	struct nl_msg *tmp = msg;

	*ret = 0;
	wsvc_printf_dbg("nl finish handler invoked\n");
	return NL_SKIP;
}

static int nl_error_handler(struct sockaddr_nl *nla,
	struct nlmsgerr *err, void *arg)
{
	int *ret = (int *)arg;
	struct sockaddr_nl *tmp = nla;

	*ret = err->error;
	wsvc_printf_dbg("nl error handler invoked, err: %s\n",
		strerror(err->error));
	return NL_SKIP;
}

static int nl_no_seq_check(struct nl_msg *msg, void *arg)
{
	void *tmp = arg;
	struct nl_msg *nlm = msg;

	wsvc_printf_dbg("nl no sequence check received\n");
	return NL_OK;
}

static int nl_response_handler(struct nl_msg *msg, void *arg)
{
	void *tmp = arg;
	struct nl_msg *nlm = msg;

	wsvc_printf_dbg("gw update nl response handler invoked\n");
	return 0;
}

static int nud_stats_rsp_handler (struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *genlh;
	struct nlattr *attr[NL80211_ATTR_MAX_INTERNAL + 1];
	char *vendata = NULL;
	int datalen;
	struct nud_stats stats;

	(void *)arg;
	genlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	int result = nla_parse(attr, NL80211_ATTR_MAX_INTERNAL,
			       genlmsg_attrdata(genlh, 0),
		               genlmsg_attrlen(genlh, 0), NULL);

	if (attr[NL80211_ATTR_VENDOR_DATA]) {
		vendata = ((char *)nla_data(attr[NL80211_ATTR_VENDOR_DATA]));
		datalen = nla_len(attr[NL80211_ATTR_VENDOR_DATA]);
	} else {
		wsvc_printf_dbg("NL80211_ATTR_VENDOR_DATA not found\n");
		return -1;
	}

	struct nlattr *tb_vendor[QCA_ATTR_NUD_STATS_GET_MAX + 1];
	nla_parse(tb_vendor, QCA_ATTR_NUD_STATS_GET_MAX,
			  (struct nlattr *)vendata, datalen, NULL);

	memset(&stats, 0, sizeof(struct nud_stats));
	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_FROM_NETDEV])
		stats.arp_req_count_from_netdev = nla_get_u16(tb_vendor[
			QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_FROM_NETDEV]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_TO_LOWER_MAC])
		stats.arp_req_count_to_lower_mac = nla_get_u16(tb_vendor[
				QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_TO_LOWER_MAC]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_REQ_RX_COUNT_BY_LOWER_MAC])
		stats.arp_req_rx_count_by_lower_mac = nla_get_u16(tb_vendor[
			QCA_ATTR_NUD_STATS_ARP_REQ_RX_COUNT_BY_LOWER_MAC]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_TX_SUCCESS])
		stats.arp_req_count_tx_success = nla_get_u16(tb_vendor[
				QCA_ATTR_NUD_STATS_ARP_REQ_COUNT_TX_SUCCESS]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_RSP_RX_COUNT_BY_LOWER_MAC])
		stats.arp_rsp_rx_count_by_lower_mac = nla_get_u16(tb_vendor[
			QCA_ATTR_NUD_STATS_ARP_RSP_RX_COUNT_BY_LOWER_MAC]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_RSP_RX_COUNT_BY_UPPER_MAC])
		stats.arp_rsp_rx_count_by_upper_mac = nla_get_u16(tb_vendor[
			QCA_ATTR_NUD_STATS_ARP_RSP_RX_COUNT_BY_UPPER_MAC]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_RSP_COUNT_TO_NETDEV])
		stats.arp_rsp_count_to_netdev = nla_get_u16(tb_vendor[
				QCA_ATTR_NUD_STATS_ARP_RSP_COUNT_TO_NETDEV]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_ARP_RSP_COUNT_OUT_OF_ORDER_DROP])
		stats.arp_rsp_count_out_of_order_drop = nla_get_u16(tb_vendor[
			   QCA_ATTR_NUD_STATS_ARP_RSP_COUNT_OUT_OF_ORDER_DROP]);

	if (tb_vendor[QCA_ATTR_NUD_STATS_AP_LINK_ACTIVE])
		stats.ap_link_active = 1;

	if (tb_vendor[QCA_ATTR_NUD_STATS_IS_DAD])
		stats.is_duplicate_addr_detection = 1;

	wsvc_printf_info("Req_from_netdev :%d count_to_lower :%d"
		  " count_by_lower :%d"
		  " count_tx_succ :%d rsp_count_lower :%d"
		  " rsp_count_upper :%d  rsp_count_netdev :%d"
		  " out_of_order_drop :%d active_aplink %d"
		  " is_duplicate_addr_detection :%d ",
		  stats.arp_req_count_from_netdev,
		  stats.arp_req_count_to_lower_mac,
		  stats.arp_req_rx_count_by_lower_mac,
		  stats.arp_req_count_tx_success,
		  stats.arp_rsp_rx_count_by_lower_mac,
		  stats.arp_rsp_rx_count_by_upper_mac,
		  stats.arp_rsp_count_to_netdev,
		  stats.arp_rsp_count_out_of_order_drop,
		  stats.ap_link_active,
		  stats.is_duplicate_addr_detection);

	return 0;
}

static int cnss_gw_init_wifi_socket(struct cnss_gw_data *gw_data)
{
	struct nl_cb *cb;
	int pid = getpid() & 0x3FFFFF;

	gw_update_info.wifi_sockfd = nl_socket_alloc();
	if (NULL == gw_update_info.wifi_sockfd) {
		wsvc_printf_err("nl socket alloc failed\n");
		return -1;
	}

	wsvc_printf_dbg("wifi nl socket created, fd: %p\n",
			gw_update_info.wifi_sockfd);

	gw_update_info.wifi_sockfd->s_flags =
		CNSS_CMD_SOCK_PORT(CNSS_WIFI_CMD_SOCK_PORT,
			gw_update_info.wifi_sockfd->s_flags);

	gw_update_info.wifi_sockfd->s_local.nl_pid =
		pid + (CNSS_WIFI_CMD_SOCK_PORT << 22);

	if (nl_connect(gw_update_info.wifi_sockfd, NETLINK_GENERIC)) {
		wsvc_printf_err("wifi nl connect failed\n");
		return -1;
	}

	/* no need to set the netlink socket buffer size, default
	 * value of 4096 bytes is good enough for this application
	 */

	/* configure custom callbacks */
	cb = nl_socket_get_cb(gw_update_info.wifi_sockfd);
	if (cb == NULL) {
		wsvc_printf_err("gw update cb allocation failed\n");
		nl_close(gw_update_info.wifi_sockfd);
		return -ENOMEM;
	}
	gw_data->err_no = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, nl_no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, nl_error_handler, &gw_data->err_no);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, nl_finish_handler, &gw_data->err_no);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, nl_ack_handler, &gw_data->err_no);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nl_response_handler, NULL);
	nl_cb_put(cb);

	/* will be freed during de-init */
	gw_data->g_cb = cb;

	wsvc_printf_dbg("wifi nl socket successfully initialized\n");
	return 0;
}

/* initializes netlink socket to listen to route multicast group messages */
static int cnss_gw_init_route_socket()
{

	int fd = -1;
	int ret = 0;
	struct sockaddr_nl sa;

	/* Netlink route socket */
	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (fd < 0) {
		wsvc_perror("socket");
		ret = -1;
		goto out;
	}

	memset(&sa, 0, sizeof(sa));

	/* Register for routing updates */
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_ROUTE | RTMGRP_NEIGH;

	ret = bind(fd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0) {
		wsvc_perror("bind");
		goto out;
	}

	gw_update_info.route_sockfd = fd;
	wsvc_printf_dbg("cnss gw kernel netlink socket initialized\n");

out:
	if (fd >= 0 && ret < 0) {
		close(fd);
		gw_update_info.route_sockfd = 0;
	}

	return ret;
}

/* we open two kernel netlink sockets, one for listening "route"
 * group of multicast messages and the second one for listening
 * "neighbor" group of multicast messages
 */
static int cnss_gw_init_kernel_sockets()
{
	int ret = 0;

	ret = cnss_gw_init_route_socket();
	if (ret < 0) {
		wsvc_printf_err("cnss gw route socket init failed\n");
		return ret;
	}

	return ret;
}

/* init handler */
int cnss_gw_update_init(void)
{
	int ret;

	memset(&gw_update_info, 0, sizeof(gw_update_info));

	ret = cnss_gw_init_kernel_sockets();
	if (ret < 0) {
		wsvc_printf_err("cnss gw kernel socket init failed\n");
		return ret;
	}

	ret = cnss_gw_init_wifi_socket(&gw_data);
	if (ret < 0) {
		wsvc_printf_err("cnss gw wifi socket init failed\n");
		return ret;
	}

	return 0;
}

/* deinit handler */
int cnss_gw_update_deinit()
{
	if (gw_update_info.route_sockfd) {
		close(gw_update_info.route_sockfd);
		gw_update_info.route_sockfd = 0;
	}

	if (gw_update_info.wifi_sockfd) {
		nl_close(gw_update_info.wifi_sockfd);
		nl_socket_free(gw_update_info.wifi_sockfd);
		gw_update_info.wifi_sockfd = 0;
	}

	memset(&gw_update_info, 0, sizeof(struct cnss_gw_update));
	return 0;
}

/* returns route socket descriptor for monitoring kernel multicast messages */
int cnss_get_gw_update_sockfd()
{
	return gw_update_info.route_sockfd;
}

int cnss_update_wlan_drv(struct cnss_gw_data *gw_data)
{
	int id, ret, err, res;
	char temp_arr[IPV6_NUM_BYTES] = { 0 };
	struct nl_msg *nlmsg;
	void *msg_head = NULL;
	struct nlattr *params, *nldata;
	struct nl_cb *cb = gw_data->g_cb;
	int ifindex = if_nametoindex("wlan0");
	struct cnss_gw_info *new_gw_info = &gw_update_info.new_gw_info;

	if (NULL == gw_update_info.wifi_sockfd) {
		wsvc_printf_err("nl command socket is not created\n");
		return -ENODEV;
	}

	/* Some more paranoia checks */
	if (!memcmp(new_gw_info->mac_addr, temp_arr, ETH_ALEN)) {
		wsvc_printf_err("Invalid mac address\n");
		return -EINVAL;
	}

	if (new_gw_info->ipv4_type == AF_INET &&
			!memcmp(new_gw_info->ipv4_addr, temp_arr,
				IPV4_NUM_BYTES)) {
		wsvc_printf_err("Invalid ipv4 address\n");
		return -EINVAL;
	}

	if (new_gw_info->ipv6_type == AF_INET6 &&
			!memcmp(new_gw_info->ipv6_addr, temp_arr,
				IPV6_NUM_BYTES)) {
		wsvc_printf_err("Invalid ipv6 address\n");
		return -EINVAL;
	}

	nlmsg = nlmsg_alloc();
	if (NULL == nlmsg) {
		wsvc_printf_err("nl msg alloc failed\n");
		return -ENOMEM;
	}

	id = genl_ctrl_resolve(gw_update_info.wifi_sockfd, "nl80211");
	if (id < 0) {
		wsvc_printf_err("nl ctrl resolve failed\n");
		nlmsg_free(nlmsg);
		return -ENODEV;
	}

	msg_head = genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0,
		/*family*/id, /* hdrlen*/ 0,/* flags*/0,
		/* cmd*/ NL80211_CMD_VENDOR, /* version = */ 0);

	if (NULL == msg_head) {
		wsvc_printf_err("genlmsg put failed\n");
		nlmsg_free(nlmsg);
		return -ENOMEM;
	}

	/* OUI goes first */
	if (nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, OUI_QCA) < 0) {
		wsvc_printf_err("nlmsg put failed for QCA OUI\n");
		nlmsg_free(nlmsg);
		return -ENOMEM;
	}

	/* Vendor subcmd */
	if (nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD,
		QCA_NL80211_VENDOR_SUBCMD_GW_PARAM_CONFIG) < 0) {
		wsvc_printf_err("nlmsg put failed for vendor subcmd\n");
		nlmsg_free(nlmsg);
		return -ENOMEM;
	}

	/* iface on which the message is to be sent */
	wsvc_printf_dbg("ifindex: %d\n", ifindex);
	ret = nla_put_u32(nlmsg, NL80211_ATTR_IFINDEX, ifindex);

	params = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
	if (NULL == params) {
		wsvc_printf_err("nla nest start failed\n");
		nlmsg_free(nlmsg);
		return -EMSGSIZE;
	}

	/* gw mac address  */
	if (nla_put(nlmsg,
			QCA_WLAN_VENDOR_ATTR_GW_PARAM_CONFIG_GW_MAC_ADDR,
			ETH_ALEN, new_gw_info->mac_addr) < 0) {
		wsvc_printf_err("nla put failed for gw mac addr\n");
		nlmsg_free(nlmsg);
		return -ENOMEM;
	}

	/* ipv4 gw address */
	if (new_gw_info->ipv4_type == AF_INET) {
		if (nla_put(nlmsg,
				QCA_WLAN_VENDOR_ATTR_GW_PARAM_CONFIG_IPV4_ADDR,
				IPV4_NUM_BYTES, new_gw_info->ipv4_addr) < 0) {
			wsvc_printf_err("nla put failed for ipv4 addr\n");
			nlmsg_free(nlmsg);
			return -ENOMEM;
		}
	}

	if (new_gw_info->ipv6_type == AF_INET6) {
		if (nla_put(nlmsg,
				QCA_WLAN_VENDOR_ATTR_GW_PARAM_CONFIG_IPV6_ADDR,
				IPV6_NUM_BYTES, new_gw_info->ipv6_addr) < 0) {
			wsvc_printf_err("nla put failed for ipv6 addr\n");
			nlmsg_free(nlmsg);
			return -ENOMEM;
		}
	}

	nla_nest_end(nlmsg, params);

	/* send auto complete */
	err = nl_send_auto_complete(gw_update_info.wifi_sockfd, nlmsg);
	if (err < 0) {
		wsvc_printf_err("gw update nl auto complete err: %d\n", err);
		nlmsg_free(nlmsg);
		return err;
	}

	res = nl_recvmsgs(gw_update_info.wifi_sockfd, cb);
	if (res)
		wsvc_printf_err("gw update message send failed: %d\n", res);
	else
		wsvc_printf_dbg("gw update message successfully sent\n");

	return 0;
}
