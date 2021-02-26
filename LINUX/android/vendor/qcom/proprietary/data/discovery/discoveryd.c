/******************************************************************************
                        D I S C O V E R Y D . C

  DESCRIPTION
  User-space daemon to facilitate camera discovery.
******************************************************************************/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <android/log.h>
#include <utils/Log.h>

#define SUCCESS			1
#define FAILURE			0

#define PROBE_DEST_PORT		3702
#define PROBE_SRC_PORT		3703

#define DISCOVERY_MC_GROUP	"239.255.255.250"
#define BUFLEN			2048

/* ifaces of interest. */
#define SOFTAP_IFACE		"softap0"
#define WLAN_IFACE		"wlan0"
#define ETH_IFACE		"eth0"
#define IFACE_COUNT		3

/* android logging. */
#define LOG_TAG "discoveryd"

#define DISC_LOGV(...)			ALOGV(__VA_ARGS__);
#define DISC_LOGI(...)			ALOGI(__VA_ARGS__);
#define DISC_LOGE(...)			ALOGE(__VA_ARGS__);

#define DISC_LOG_FUNCTION_ENTRY		DISC_LOGV("ENTRY: %s\n", __func__)
#define DISC_LOG_FUNCTION_EXIT		DISC_LOGV("EXIT: %s\n", __func__)

/* iface_info: alloc a struct for each iface. if the iface is:
 *    enumerated:            index is non-zero.
 *    assigned an address:   addr and mask are non-zero.
 */
struct iface_info {
	char			name[IFNAMSIZ];
	unsigned int		index;
	struct in_addr		addr;
	struct in_addr		mask;
};

struct iface_info *ifaces;

#define CAMERA_NAME_LEN 20
char camera_info[CAMERA_NAME_LEN];


/*             Function Prototypes, and brief descriptions:                   */

/* Function to process incomming probe */
static void process_probe(int disc_fd);
/* Function to process IPv4 address and interface notifications */
static void process_nl_msg(int nl_fd, int disc_fd);

/* Functions to process RTM_NEWLINK, RTM_DELLINK, RTM_NEWADDR & RTM_DELADDR */
static void process_add_link(int idx);
static void process_del_link(int idx);
static void process_add_addr(int idx, int disc_fd);
static void process_del_addr(int idx);

/* Check if iface is ready. i.e enumerated and has a IPv4 address assigned.
 * fill iface index and address into ifaces[]. */
static int is_iface_ready(int idx);

/* Check if iface is among those we are interested in.
 * If yes, return index in ifaces[] */
static int iface_of_interest(unsigned int index, int *idx);

/* Find the interface from which the packet must have come. Peer address and
 * iface address must be in the same subnet. Iterate among iface addresses and
 * return iface's address. */
static int match_iface_addr(struct in_addr *peer_addr, struct in_addr *iface_addr);

/* Register to the Multicast group for every address assigned interface.
 * To find interfaces with successful registration run:
 *       /proc/net/dev_mcast | grep 7ffffa$
 *       0x7ffffa == inet_aton("MC_GROUP") & 0x7fffff  (final 23 bits) */
static void add_membership(int disc_fd, int idx);

static void get_camera_id();

/* main():
 *   start with no arguments as a service. take camera-id from conf file.
 */
int main( int argc, char **argv )
{
	int disc_fd, nl_fd;
	fd_set read_fds;
	int nfds;
	struct sockaddr_in my_addr;
	struct sockaddr_nl my_events;
	unsigned int sk_addr_len = sizeof(struct sockaddr_in);
	int i, rcv_ret;

	DISC_LOG_FUNCTION_ENTRY;

	/* get camera id, from the config file*/
	get_camera_id();
	DISC_LOGI("camera id:%s\n",camera_info);

	ifaces = calloc(IFACE_COUNT, sizeof(struct iface_info));
	strlcpy(ifaces[0].name, WLAN_IFACE, IFNAMSIZ);
	strlcpy(ifaces[1].name, SOFTAP_IFACE, IFNAMSIZ);
	strlcpy(ifaces[2].name, ETH_IFACE, IFNAMSIZ);

	nl_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if(nl_fd < 0) {
		DISC_LOGE("Unable to open nl socket:[%s]\n", strerror(errno));
		exit(1);
	}
	memset(&my_events, 0, sizeof(struct sockaddr_nl));
	my_events.nl_family = AF_NETLINK;
	my_events.nl_pid = getpid();

	/* register to link UP/DOWN and Address change events */
	my_events.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;
	if(bind(nl_fd, &my_events, sizeof(struct sockaddr_nl))) {
		DISC_LOGE("nl bind failed:[%s]\n", strerror(errno));
		exit(1);
	}

	/* open discovery recv.er socket, bind to PROBE_DEST_PORT */
	disc_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(disc_fd < 0) {
		DISC_LOGE("Unable to open udp socket:[%s]\n", strerror(errno));
		exit(1);
	}

	memset(&my_addr, 0, sk_addr_len);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PROBE_DEST_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(disc_fd, &my_addr, sk_addr_len)) {
		DISC_LOGE("udp bind failed:[%s]\n", strerror(errno));
		exit(1);
	}

	for(i=0; i<IFACE_COUNT; i++) {
		/* register to MC group on all ifaces with an IP address */
		if((ifaces[i].index = if_nametoindex(ifaces[i].name)) == 0) {
			DISC_LOGE("if_nametoindex(%s) failed:[%s]\n",
					ifaces[i].name, strerror(errno));
			continue;
		}

		if(!is_iface_ready(i))
			continue;

		add_membership(disc_fd, i);
	}

	while(1) {
		/* man select_tut : Select law :
		 *  11. ... if the call is being used in a loop, ...
		 *      sets must be reinitialized before each call.
		 */
		FD_ZERO(&read_fds);
		FD_SET(nl_fd, &read_fds);
		FD_SET(disc_fd, &read_fds);
		nfds = disc_fd > nl_fd ? disc_fd + 1 : nl_fd + 1;
		/* max(disc_fd, nl_fd) + 1 */

		DISC_LOGI("processing done, wait for packets/events\n");

		/* blocking select call. sleep till one of the fds is active */
		if(select(nfds, &read_fds, NULL, NULL, NULL) < 0) {
			DISC_LOGE("select failed:[%s]\n", strerror(errno));
			exit(1);
		}

		/* first check for netlink events. then packets. */
		if(FD_ISSET(nl_fd, &read_fds))
			process_nl_msg(nl_fd, disc_fd);

		if(FD_ISSET(disc_fd, &read_fds))
			process_probe(disc_fd);
	}

	return 0;
}

static void process_probe(int disc_fd)
{
	struct sockaddr_in peer_addr;
	struct in_addr iface_addr;	/* glorified unsigned int */
	unsigned int sk_addr_len = sizeof(struct sockaddr_in);
	int send_ret, rcv_ret, send_len;
	char msg_buf[BUFLEN];

	DISC_LOG_FUNCTION_ENTRY;
	memset(&peer_addr, 0, sizeof(struct sockaddr));
	iface_addr.s_addr = 0;
	memset(msg_buf, 0, BUFLEN * sizeof(char));

	rcv_ret = recvfrom(disc_fd, msg_buf, BUFLEN, 0, &peer_addr,
				&sk_addr_len);
	if(rcv_ret >= 0) {
		DISC_LOGI("recvd probe from %s:%d, %d bytes msg:[%s]\n",
				inet_ntoa(peer_addr.sin_addr),
				ntohs(peer_addr.sin_port), rcv_ret, msg_buf);
	} else {
		DISC_LOGE("recvfrom failed:[%s]",strerror(errno));
		DISC_LOG_FUNCTION_EXIT;
		return;
	}

	/* check peer port. answer only probes from PROBE_SRC_PORT */
	if(peer_addr.sin_port != htons(PROBE_SRC_PORT)) {
		DISC_LOGE("rogue PROBE ?.\n");
		/* log and exit func, if this probe src rule needs to be
		 * strictly enforced. */
	}

	/* check which iface the packet came from */
	if(match_iface_addr(&peer_addr.sin_addr, &iface_addr) == FAILURE) {
		DISC_LOGE("none of the iface address match!!!\n");
		DISC_LOG_FUNCTION_EXIT;
		return;
	}

	memset(msg_buf, 0, BUFLEN * sizeof(char));
	send_len = sprintf(msg_buf,"{\"id\": \"%s\", \"ipv4\": \"%s\"}",
				camera_info, inet_ntoa(iface_addr));

	DISC_LOGI("send msg:[%s]\n",msg_buf);
	send_ret = sendto(disc_fd, msg_buf, send_len, 0, &peer_addr, sk_addr_len);
	if(send_ret >= 0) {
		DISC_LOGI("sendto returns %d\n", send_ret);
	}
	else {
		DISC_LOGE("sendto failed:[%s]",strerror(errno));
	}

	DISC_LOG_FUNCTION_EXIT;
}

static void process_nl_msg(int nl_fd, int disc_fd)
{
	char msg_buf[BUFLEN];
	int msg_len;
	struct nlmsghdr * nlh;
	struct ifaddrmsg *addr_info;
	struct ifinfomsg *link_info;
	int idx;

	DISC_LOG_FUNCTION_ENTRY;
	if( (msg_len = recv(nl_fd, &msg_buf, BUFLEN, 0)) < 0 ) {
		DISC_LOGE("nl recv failed: [%s]\n", strerror(msg_len));
		return;
	}

	nlh = (struct nlmsghdr *)msg_buf;
	DISC_LOGI("process_nl_msg: received event %d\n", nlh->nlmsg_type);
	while (NLMSG_OK(nlh, msg_len)) {
		switch(nlh->nlmsg_type) {

		case RTM_NEWLINK:	/*event 16*/
			link_info = (struct ifinfomsg*)NLMSG_DATA(nlh);
			if(!(link_info->ifi_change & IFF_UP) ||
			   !iface_of_interest(link_info->ifi_index, &idx))
				break;
			DISC_LOGI("RTM_NEWLINK  %d, IFF_UP: %d, change %d\n",
					link_info->ifi_index,
					link_info->ifi_flags & IFF_UP,
					link_info->ifi_change & IFF_UP);
			process_add_link(idx);
			break;

		case RTM_DELLINK:	/*event 17*/
			link_info = (struct ifinfomsg*)NLMSG_DATA(nlh);
			if(!(link_info->ifi_change & IFF_UP) ||
			   !iface_of_interest(link_info->ifi_index, &idx))
				break;
			DISC_LOGI("RTM_DELLINK on %d, IFF_UP: %d, change %d\n",
					link_info->ifi_index,
					link_info->ifi_flags & IFF_UP,
					link_info->ifi_change & IFF_UP);
			process_del_link(idx);
			break;

		case RTM_NEWADDR:	/*event 20*/
			addr_info = (struct ifaddrmsg*)NLMSG_DATA(nlh);
			if(addr_info->ifa_family != AF_INET ||
			   !iface_of_interest(addr_info->ifa_index, &idx))
				break;
			DISC_LOGI("RTM_NEWADDR on %d, family %d\n",
					addr_info->ifa_index,
					addr_info->ifa_family);
			process_add_addr(idx, disc_fd);
			break;

		case RTM_DELADDR:	/*event 21*/
			addr_info = (struct ifaddrmsg*)NLMSG_DATA(nlh);
			if(addr_info->ifa_family != AF_INET ||
			   !iface_of_interest(addr_info->ifa_index, &idx))
				break;
			DISC_LOGI("RTM_DELADDR on %d, family %d\n",
					addr_info->ifa_index,
					addr_info->ifa_family);
			process_del_addr(idx);
			break;

		default:
			DISC_LOGV("Untracked event: %d\n",nlh->nlmsg_type);
		}
		/* next header */
		nlh = NLMSG_NEXT(nlh, msg_len);
	}
	DISC_LOG_FUNCTION_EXIT;
}


static void process_add_addr(int idx, int disc_fd)
{
	DISC_LOGV("process_add_addr on %s\n", ifaces[idx].name);

	if(!is_iface_ready(idx)) {
		DISC_LOGE(stderr, "iface %s is NOT ready!!!", ifaces[idx].name);
		return;
	}
	add_membership(disc_fd, idx);
}

static void process_add_link(int idx)
{
	DISC_LOGI("process_add_link on %s\n", ifaces[idx].name);
	/*no action, may need it later...*/
}

static void process_del_addr(int idx)
{
	DISC_LOGI("process_del_addr on %s\n", ifaces[idx].name);
	ifaces[idx].addr.s_addr = 0;
	ifaces[idx].mask.s_addr = 0;
}

static void process_del_link(int idx)
{
	DISC_LOGI("process_del_link on %s\n", ifaces[idx].name);
	ifaces[idx].index = 0;
	ifaces[idx].addr.s_addr = 0;
	ifaces[idx].mask.s_addr = 0;
}


static int iface_of_interest(unsigned int index, int *idx)
{
	int i;
	char iface_name[IFNAMSIZ];

	for(i=0; i<IFACE_COUNT; i++) {
		if(index == ifaces[i].index) {
			*idx = i;
			return SUCCESS;
		}
	}

	memset(iface_name,0,IFNAMSIZ);
	if(!if_indextoname(index, iface_name)) {
		DISC_LOGE("if_indextoname(%d) failed:[%s]\n", index, strerror(errno));
		return FAILURE;
	}

	for(i=0; i<IFACE_COUNT; i++) {
		if(strncmp(iface_name, ifaces[i].name, IFNAMSIZ-1) == 0) {
			ifaces[i].index = index;
			*idx = i;
			DISC_LOGI("iface up %s, index %d\n", ifaces[i].name,
					ifaces[i].index);
			return SUCCESS;
		}
	}
	DISC_LOGI("iface %d:%s, is not of interest\n", index, iface_name);
	return FAILURE;
}

/* check if iface is enumerated and if address is assigned
 * return SUCCESS if iface is ready, FAILURE if not
 */
static int is_iface_ready(int idx)
{
	int ioctl_fd;
	struct ifreq if_request;
	struct iface_info *iface = &ifaces[idx];
	struct sockaddr_in *iface_addr;


	if(iface->addr.s_addr > 0 && iface->index > 0)
		return SUCCESS;

	/* get  iface address */
	ioctl_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(ioctl_fd < 0) {
		DISC_LOGE("Unable to open ioctl fd:[%s]\n",strerror(errno));
		exit(1);
	}
	memset(&if_request, 0, sizeof(struct ifreq));
	iface_addr = (struct sockaddr_in*)&if_request.ifr_addr;
	iface_addr->sin_family = AF_INET;
	strlcpy(if_request.ifr_name, iface->name, IFNAMSIZ);

	if(ioctl(ioctl_fd, SIOCGIFADDR, &if_request)) {
		DISC_LOGE("ioctl SIOCGIFADDR %s iface failed:[%s]\n",
				iface->name, strerror(errno));
		iface->addr.s_addr = 0;
		iface->mask.s_addr = 0;
		return FAILURE;
	}
	iface->addr.s_addr = iface_addr->sin_addr.s_addr;

	/* get iface mask */
	memset(&if_request, 0, sizeof(struct ifreq));
	iface_addr = (struct sockaddr_in*) &if_request.ifr_netmask;
	iface_addr->sin_family = AF_INET;
	strlcpy(if_request.ifr_name, iface->name, IFNAMSIZ);
	if(ioctl(ioctl_fd, SIOCGIFNETMASK, &if_request)) {
		DISC_LOGE("ioctl SIOCGIFNETMASK %s iface failed:[%s]\n",
				iface->name, strerror(errno));
		iface->addr.s_addr = 0;
		iface->mask.s_addr = 0;
		return FAILURE;
	}
	iface->mask.s_addr = iface_addr->sin_addr.s_addr;

	/* dont use more than one inet_ntoa() while logging. The next call
	 * will overwrite the first result. */
	DISC_LOGI("iface %s addr:%s ", iface->name, inet_ntoa(iface->addr));
	DISC_LOGI("iface %s mask:%s. iface is ready\n", iface->name,
			inet_ntoa(iface->mask));
	return SUCCESS;
}

static int match_iface_addr(struct in_addr *peer_addr, struct in_addr *iface_addr)
{
	int i;
	for(i=0; i<IFACE_COUNT; i++) {
		DISC_LOGV("%s: comparing %u == %u, on %s:%d. ", __func__,
				peer_addr->s_addr & ifaces[i].mask.s_addr,
				ifaces[i].addr.s_addr & ifaces[i].mask.s_addr,
				ifaces[i].name, ifaces[i].index);
		DISC_LOGV("%s: on iface %s:%d\n addr:%u, %u\n", __func__,
				ifaces[i].name, ifaces[i].index,
				ifaces[i].addr, ifaces[i].mask);

		if(!ifaces[i].index || !ifaces[i].addr.s_addr)
			continue;
		if((peer_addr->s_addr & ifaces[i].mask.s_addr) ==
		   (ifaces[i].addr.s_addr & ifaces[i].mask.s_addr)){
			iface_addr->s_addr = ifaces[i].addr.s_addr;
			return SUCCESS;
		}
	}
	return FAILURE;
}

static void add_membership(int disc_fd, int idx)
{
	struct ip_mreqn mc_req;

	memset(&mc_req, 0, sizeof(struct ip_mreq));
	mc_req.imr_address.s_addr = ifaces[idx].addr.s_addr;
	mc_req.imr_ifindex = ifaces[idx].index;
	if(inet_aton(DISCOVERY_MC_GROUP, &mc_req.imr_multiaddr) == 0) {
		DISC_LOGE("inet_aton failed:[%s]\n", strerror(errno));
		return;
	}

	if(setsockopt(disc_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mc_req,
					sizeof(struct ip_mreq))) {
		DISC_LOGE("setsockopt failed:[%s]\n",strerror(errno));
	}
	else
		DISC_LOGI("add_membership successful on iface %d:%s\n",
				ifaces[idx].index, ifaces[idx].name);

}

static void get_camera_id()
{
	FILE * fp;
	char *buff;
	int buf_len = 100;
	int line_len;
	int name_len = 0;
	char config1[] = "camera_id=";

	fp = fopen("/vendor/etc/discoveryd.config", "r");
	if(!fp) {
		DISC_LOGE("fopen-ing config file failed:[%s]\n",strerror(errno));
		exit(1);
	}

	if((buff = calloc(buf_len, sizeof(char))) == NULL) {
		DISC_LOGE("string calloc error:[%s]\n", strerror(errno));
		exit(1);
	}

	while((line_len = getline(&buff, &buf_len, fp)) > 0) {
		if(strncmp(buff, config1, strlen(config1)) == 0) {
			name_len = strlcpy(camera_info, buff + strlen(config1),
						CAMERA_NAME_LEN);
			if(name_len == 1) {
				DISC_LOGI("Config Warning: camera id empty\n");
				continue;
			}
			/* remove the last char: \n */
			camera_info[name_len-1]='\0';
		}
	}

	free(buff);
	fclose(fp);
}
