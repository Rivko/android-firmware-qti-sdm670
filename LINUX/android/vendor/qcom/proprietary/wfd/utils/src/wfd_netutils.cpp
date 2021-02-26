/*
*  @file wfd_netutils.cpp
*  @par DESCRIPTION:
*       Definition of the wfd network utilities
*
*
*  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <stdlib.h>
#include <errno.h>

#include <string.h>
#include "netlink/genl/genl.h"
#include "netlink/genl/family.h"
#include "netlink/genl/ctrl.h"

#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <linux/errqueue.h>

#include <linux/pkt_sched.h>
#include <net/if.h>

#include <netlink-private/object-api.h>
#include <netlink-private/types.h>

#include "MMDebugMsg.h"

#include "wfd_netutils.h"
#include "WFDDefs.h"

#include <string>
#include <sstream>

#define QCSAP_IOCTL_PRIV_GET_SOFTAP_LINK_SPEED (SIOCIWFIRSTPRIV+31)
#define MAC_ADDR_LEN 6
#define MAC_SIZE 24

#define RTP_PORT_START_NUM 19022
#define RTP_PORT_END_NUM   19189

#define MAX_IFS 64

typedef union address
{
    struct sockaddr sa;
    struct sockaddr_in sa_in;
    struct sockaddr_in6 sa_in6;
    struct sockaddr_storage sa_stor;
}
address_t;

#define UNUSED(x) ((void)x)

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif


#ifndef WFD_ICS
int getP2pInterfaceName(char* pInterfaceName, int size, NetType netType)
{
    struct ifreq *ifr, *ifend;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];
    struct ifreq ifreq;
    int s;

    // check input parameters

    if (netType == LAN) {
        strlcpy(pInterfaceName, "wlan0", IFNAMSIZ);
        return 0;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);

    ifc.ifc_len = static_cast<int>(sizeof(struct ifreq)) * MAX_IFS;
    ifc.ifc_req = ifs;
    if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
    {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                  "getP2pInterfaceName: error at ioctl(SIOCGIFCONF)");
      close(s);
      return -1;
    }

    ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));
    for (ifr = ifc.ifc_req; ifr < ifend; ifr++)
    {
      if (ifr->ifr_addr.sa_family == AF_INET)
      {
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM,
                     "getP2pInterfaceName: ifname = %s", ifr->ifr_name);

        if ((netType == WIGIG_P2P && strncmp(ifr->ifr_name, "wigig", 5) == 0)
                || (netType != WIGIG_P2P && strncmp(ifr->ifr_name, "p2p", 3) == 0))
        {
          strlcpy(pInterfaceName, ifr->ifr_name, MIN(size,IFNAMSIZ));
          MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM,
                       "getP2pInterfaceName: p2p-ifname = %s", pInterfaceName);
           close(s);
           return 0;
        }
      }
    }
    close(s);
    return -1;
}
#endif

int getLocalIpAddress(char* pLocalIpAddr, int size, NetType netType)
{
    //getting local IP address
    int requiredSize = 20;
    struct ifreq ifr;
    int s;
    unsigned int addr;
    char p2pifname[IFNAMSIZ];

    if (pLocalIpAddr == NULL) {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "pLocalIpAddr is NULL");
        return -1;
    }
    if (size < requiredSize) {
        MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_ERROR, "Not enough size for IPv4 ip \
            address. Required min: %d, given: %d", requiredSize, size);
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
#ifdef WFD_ICS
    strlcpy(ifr.ifr_name, "wlan0", IFNAMSIZ);
#else
    if (getP2pInterfaceName(p2pifname, IFNAMSIZ, netType) == -1) {
        if (netType == WIGIG_P2P) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName Failed,"\
                    "Falling back to wigig0");
            strlcpy(ifr.ifr_name, "wigig0", IFNAMSIZ);
        } else {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName Failed,"\
                    "Falling back to p2p0");
            strlcpy(ifr.ifr_name, "p2p0", IFNAMSIZ);
        }
    }
    else
    {
      p2pifname[IFNAMSIZ-1]='\0';
      strlcpy(ifr.ifr_name, p2pifname, IFNAMSIZ);
      MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, "getP2pInterfaceName ,"\
                                            "interface name = %s",ifr.ifr_name);
    }
#endif
    ifr.ifr_name[IFNAMSIZ-1] = 0;

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "getLocalIpAddress :Error in creating socket");
        return -1;
    } else {
        if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
        {
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_ERROR, "getLocalIpAddress :: %s",strerror(errno));
            close(s);
            return -1;
        }
        else
        {
            addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "getLocalIpAddress :addr[%x]",addr);
            snprintf(pLocalIpAddr, requiredSize, "%d.%d.%d.%d",
                addr & 0xff,
                ((addr >> 8) & 0xff),
                ((addr >> 16) & 0xff),
                ((addr >> 24) & 0xff));
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "getLocalIpAddress :local \
                IP Address %s", pLocalIpAddr);
            close(s);
            return 0;
        }
    }
}

/* Get peer Mac address from the ARP table */
int getPeerMacAddress( char* pMacAddr, char* pPeerIpAddr, int size, NetType netType )
{
    struct ifreq ifr;
    char p2pifname[IFNAMSIZ];
    int requiredSize = MAC_SIZE;
    char ip[128]   = { 0x00 };
    char hwa[128]  = { 0x00 };
    char mask[128] = { 0x00 };
    char line[128] = { 0x00 };
    char dev[128]  = { 0x00 };

    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_ERROR, "getPeerMacAddress :Peer IP Address %s", pPeerIpAddr);

    if (NULL == pMacAddr)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "pMacAddr is NULL");
        return -1;
    }

    if (pPeerIpAddr == NULL)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "pPeerIpAddr is NULL");
        return -1;
    }

    if (size < requiredSize)
    {
        MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_ERROR, "Not enough size for MAC address. Required min: %d, given: %d", requiredSize, size);
        return -1;
    }

    memset(p2pifname, 0, sizeof(p2pifname));
    memset(&ifr, 0, sizeof(ifr));


    if (getP2pInterfaceName(p2pifname, IFNAMSIZ, netType) == -1)
    {
        if (netType == WIGIG_P2P) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName failed "\
                    " Falling back to wigig0");
            strlcpy(ifr.ifr_name, "wigig0", IFNAMSIZ);//Do we need to re-query??
        } else {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName failed "\
                    " Falling back to p2p0");
            strlcpy(ifr.ifr_name, "p2p0", IFNAMSIZ);//Do we need to re-query??
        }
    }
    else
    {
        p2pifname[IFNAMSIZ-1]='\0';
        strlcpy(ifr.ifr_name, p2pifname, IFNAMSIZ);
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, "getP2pInterfaceName ,"\
                                            "interface name = %s",ifr.ifr_name);
    }

    ifr.ifr_name[IFNAMSIZ-1] = 0;
    FILE *arpFP = fopen("/proc/net/arp", "r");
    if( !arpFP )
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "getPeerMacAddress \
            :/proc/net/arp file open failed");
        return -1;
    }

    /*
     * Bypass header, read one line.
     */
    fgets( line, sizeof(line), arpFP );
    while( fgets( line, sizeof(line), arpFP ) )
    {
        /*
         * All these strings can't overflow because fgets above reads limited amount of data
         * ARP output is in the format
         * <IP address> <HW type> <Flags> <HW address> <Mask Device>
         */
        int type = 0, flags = 0;
        int num = sscanf( line, "%s 0x%x 0x%x %s %s %s\n", ip, &type, &flags, hwa, mask, dev );
        if( num < 4 )
            continue;
        /*
         * Not on the same interface, skip.
         */
        if( strncmp( dev, ifr.ifr_name, IFNAMSIZ ) != 0 )
            continue;
        /*
         * Unknown or broadcast address, skip.
         */
        if( strncmp( hwa, "00:00:00:00:00:00", 18  ) == 0 ||
            strncmp( hwa, "FF:FF:FF:FF:FF:FF", 18 ) == 0 ||
            strncmp( hwa, "ff:ff:ff:ff:ff:ff", 18 ) == 0 )
            continue;
            /*
             * Found!
             */
        if( strncmp( ip, pPeerIpAddr, sizeof(pPeerIpAddr) ) == 0 )
        {
            strlcpy( pMacAddr, hwa, 18 );
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                        "getPeerMacAddress Peer MAC address found ");
            MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_ERROR,
                        "getPeerMacAddress Peer MAC address  = %s  Peer IP Address = %s",pMacAddr, pPeerIpAddr );
            fclose(arpFP);
            return 0;
        }
    }
    fclose( arpFP );
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "getPeerMacAddress ,"\
                                            "Peer MAC address not found ");
    return -1;
}

unsigned int getLinkSpeed(char *macaddr, unsigned int len, bool& queryIfName, NetType netType) {


    struct iwreq iwrrequest;

    char inputstring[64] ={0};
    int result = 0;
    unsigned int linkspeed = 0;
    static char p2pifname[IFNAMSIZ] = {0};

    if(!macaddr || len < MAC_ADDR_LEN)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                          "NetUtils Invalid Mac Address");
        return 0;
    }

    int nSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if(nSocket < 0)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                          "NetUtils Failed to create socket");
    return 0;
    }
#ifdef WFD_ICS
    strlcpy(iwrrequest.ifr_name, "wlan0", IFNAMSIZ);
#else
    if(queryIfName)
    {
        if (getP2pInterfaceName(p2pifname, IFNAMSIZ, netType) == -1)
        {
            if (netType == WIGIG_P2P) {
                MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName failed "\
                        " Falling back to wigig0");
                strlcpy(p2pifname, "wigig0", IFNAMSIZ);//Do we need to re-query??
            } else {
                MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName failed "\
                        " Falling back to p2p0");
                strlcpy(p2pifname, "p2p0", IFNAMSIZ);//Do we need to re-query??
            }
        }
        else
        {
          p2pifname[IFNAMSIZ-1] = '\0';
          queryIfName = false;//Interface name query was a success , don't redo
          MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, "getP2pInterfaceName ,"\
                                "interface name = %s",iwrrequest.ifr_name);
        }
    }
    if (strlen(p2pifname) != 0)
    {
        strlcpy(iwrrequest.ifr_name, p2pifname, IFNAMSIZ);
    }
    else
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "p2pifname is NULL");
        return 0;
    }
#endif
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, "getLinkSpeed interface name = %s",
                    iwrrequest.ifr_name);
    int offset = static_cast<int>(strlen(inputstring));

    if(macaddr != NULL)
    {
        strlcpy((char*)inputstring, (const char*)macaddr, 64);
    }
    else
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "macaddr is NULL");
    }

    iwrrequest.u.data.pointer = (char*)inputstring;
    iwrrequest.u.data.length = static_cast<unsigned short>(strlen((char*)inputstring));

    result =  ioctl(nSocket,
                    QCSAP_IOCTL_PRIV_GET_SOFTAP_LINK_SPEED,
                    &iwrrequest);

    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                "NetUtils Linkspeed  = %s", inputstring);

    if(result == 0)
    {
        //sscanf(inputstring, "%d", &linkspeed);
        linkspeed = static_cast<unsigned short>(atoi(inputstring));

    }
    else
    {
        linkspeed = 0;
    }

    MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_MEDIUM,
                 "NetUtils:Macaddress = %s Linkspeed result = %d, linkspeed = \
                  %u", inputstring, result, linkspeed * 500000);

    close(nSocket);

    return linkspeed * 500000;
}

bool getIPSockPair(bool bPair, int *sock1, int * sock2,
                    int *nPort1, int *nPort2, bool tcp)
{

   if(sock1 == NULL)
   {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                   "Invalid Arguments to find socket");
       return false;
   }
   int RTPSocket = -1;
   int RTCPSocket = -1;

   //Usually this is used to find RTP/RTCP port pair
   //Hence the naming
   if(bPair)
   {
       sockaddr_in saddr;
       address_t addr;
       int ret = -1;
       unsigned short portNum;

       saddr.sin_family = AF_INET;
       saddr.sin_addr.s_addr = INADDR_ANY;

       for(portNum = RTP_PORT_START_NUM;
            portNum < RTP_PORT_END_NUM; portNum+=2)
       {
           saddr.sin_port = htons(portNum);

           RTPSocket = socket(AF_INET, tcp? SOCK_STREAM:SOCK_DGRAM,
                    tcp? IPPROTO_TCP:IPPROTO_UDP);
           if(RTPSocket < 0)
           {
               return false;
           }


           addr.sa_in = saddr;
           ret = bind(RTPSocket, (sockaddr *)&addr.sa, (socklen_t)sizeof(saddr));

           if(ret < 0)
           {
               //Check if bind failed due to port already being used.
               MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                            "Bind Failed for RTP port num %d Retry", portNum);
               close(RTPSocket);
               continue;
           }
           else
           {
               RTCPSocket = socket(AF_INET, tcp? SOCK_STREAM:SOCK_DGRAM,
                    tcp? IPPROTO_TCP:IPPROTO_UDP);
               saddr.sin_port = htons((unsigned short)(portNum + 1));
               addr.sa_in = saddr;
               ret = bind(RTCPSocket, (sockaddr *)&addr.sa, (socklen_t)sizeof(saddr));
               if(ret < 0)
               {
                   close(RTPSocket);
                   close(RTCPSocket);
                   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                       "RTCP port Bind failed %d, retry ", portNum + 1);
               }
               else
               {
                   MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH,
                               "Found RTP_RTCP port pair %d %d",
                               portNum, portNum+1);
                   if(sock2)
                   {
                       *sock2 = RTCPSocket;
                       if(nPort2)
                       {
                           *nPort2 = portNum + 1;
                       }
                   }
                   else
                   {
                       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,
                               "Invalid Arg for Sock2");
                       close(RTCPSocket);
                       false;
                   }
                   *sock1 = RTPSocket;
                   if(nPort1)
                   {
                       *nPort1 = portNum;
                   }
                   break;
               }
           }
       }
   }
   else
   {
       int ret;
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,
                                     "Find single Port");
       RTPSocket = socket(AF_INET, tcp? SOCK_STREAM:SOCK_DGRAM,
                    tcp? IPPROTO_TCP:IPPROTO_UDP);
       sockaddr_in saddr;
       address_t addr;
       saddr.sin_family = AF_INET;
       saddr.sin_addr.s_addr = INADDR_ANY;
       saddr.sin_port = htons(RTP_PORT_END_NUM);
       addr.sa_in = saddr;
       ret = bind(RTPSocket, (sockaddr *)&addr.sa, (socklen_t)sizeof(saddr));

       if(ret < 0)
       {
           MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,
                                 "RTP port Bind failed");
       }

       int size = (int)sizeof(saddr);

       ret = getsockname(RTPSocket, (struct sockaddr *)&addr.sa, (socklen_t*)(&size));
       if(ret == 0)
       {
           if(nPort1)
           {
               *nPort1 = ntohs(saddr.sin_port);
               MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                           "Server RTP port Num = %d", *nPort1);
           }
       }
       else
           MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,
                                 "getsockname failed");

       *sock1 = RTPSocket;
   }

   return true;
}

int getLocalIpSocket (int sock, char* IP)
{
    sockaddr_in             saddr;
    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port        = 0;
    char* localIP         = NULL;
    int size              = (int)sizeof(saddr);
    address_t addr;

    addr.sa_in = saddr;
    int ret = getsockname(sock, (struct sockaddr *)&addr.sa, (socklen_t*)(&size));
    if(ret == 0)
    {
        localIP = inet_ntoa(saddr.sin_addr);
        if (localIP == NULL || IP == NULL)
        {
           MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                       "WFD NetUtils:Local IP is null or input param failure return failed");
           return -1;
        }
        else
        {
           strlcpy(IP,localIP,strlen(localIP)+1);

           MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                        "WFD NetUtils: LocalIP from socket  = %s", IP);
           return 0;
        }
    }
    else
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
                    "WFD NetUtils:getsockname failed");
    }
    return -1;
}

int socketClose(int nSock)
{
    if(nSock > 0)
    {
        return close(nSock);
    }
    else
        return -1;
}


#define WIFI_HAL_CMD_SOCK_PORT       648

#define NL80211_CMD_GET_STATION 17
#define NL80211_ATTR_MAC 6
#define NL80211_ATTR_IFINDEX 3

#define NL80211_STA_INFO_SIGNAL 7
#define NL80211_STA_INFO_SIGNAL_AVG 13

#define NL80211_ATTR_MAX 213
#define NL80211_STA_INFO_MAX 27
#define NL80211_ATTR_STA_INFO 21

#define OUI_QCA 0x001374
#define ETH_ALEN 6
#define QCA_NL80211_VENDOR_SUBCMD_LINK_PROPERTIES  101
#define QCA_WLAN_VENDOR_ATTR_MAC_ADDR  6
#define NL80211_CMD_VENDOR 103
#define NL80211_ATTR_VENDOR_ID 195
#define NL80211_ATTR_VENDOR_SUBCMD 196
#define NL80211_ATTR_VENDOR_DATA 197


enum qca_wlan_vendor_attr_link_properties
{
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_INVALID    = 0,
    /* Unsigned 8bit value */
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_NSS        = 1,
    /* Unsigned 8bit value */
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_RATE_FLAGS = 2,
    /* Unsigned 32bit value */
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_FREQ       = 3,

    /* KEEP LAST */
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_MAX =
        QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_AFTER_LAST - 1,
};

typedef struct linkProperties {
    unsigned int nss;
    unsigned int rateflags;
    unsigned int freq;
} tLinkProps;


int ack_handler(struct nl_msg *msg, void *arg)
{
    int *err = (int *)arg;
    struct nl_msg * a;
    a = msg;
    *err = 0;
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "%s invoked",__func__);
    return NL_STOP;
}

int finish_handler(struct nl_msg *msg, void *arg)
{
    int *ret = (int *)arg;
    struct nl_msg * a;
    a = msg;
    *ret = 0;
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "%s called",__func__);
    return NL_SKIP;
}

int error_handler(struct sockaddr_nl *nla,
        struct nlmsgerr *err, void *arg)
{
    int *ret = (int *)arg;
    struct sockaddr_nl * a;
    a = nla;
    *ret = err->error;
    MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH, "%s invoked with error: %s ", __func__, strerror(errno));
    return NL_SKIP;
}

int no_seq_check(struct nl_msg *msg, void *arg)
{
    void *tmp;
    struct nl_msg * a;
    a = msg;
    tmp = arg;
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "no_seq_check received ");
    return NL_OK;
}

int response_handler(struct nl_msg *msg, void *arg)
{
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "response handler ");

    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
    tLinkProps *linkprops = (tLinkProps *)arg;

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
            genlmsg_attrlen(gnlh, 0), NULL);

    if (tb[NL80211_ATTR_VENDOR_DATA]) {
    struct nlattr *nl_vendor = tb[NL80211_ATTR_VENDOR_DATA];
    struct nlattr *tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_MAX + 1];
    struct nla_policy policy[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_MAX + 1] = {0};

    policy[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_NSS].type = NLA_U8;
    policy[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_RATE_FLAGS].type = NLA_U8;
    policy[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_FREQ].type = NLA_U32;

    if (nla_parse(tb_vendor, QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_MAX, (nlattr *)nla_data(nl_vendor),
                nla_len(nl_vendor), policy))
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "nla_parse failed");
        return NL_SKIP;
    }

    if (tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_NSS])
        linkprops->nss =
            (unsigned char) nla_get_u8(tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_NSS]);
    else
        linkprops->nss = 1;

    if (tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_RATE_FLAGS])
        linkprops->rateflags =
            (unsigned char) nla_get_u8(tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_RATE_FLAGS]);
    else
        linkprops->rateflags = 0;

    if (tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_FREQ])
        linkprops->freq =
            (unsigned int) nla_get_u32(tb_vendor[QCA_WLAN_VENDOR_ATTR_LINK_PROPERTIES_FREQ]);
    else
        linkprops->freq = 0;

    MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_HIGH, "received nss:%d rateFlags:0x%x operating freq:%d ",
                                            linkprops->nss, linkprops->rateflags, linkprops->freq);
    }
    return NL_OK;
}

void  print_nl_msg_data(struct nl_msg *mMsg)
{
    struct nlmsghdr *nm_nlh;
    uint32_t size, i;
    uint8_t *ptr;

    nm_nlh = mMsg->nm_nlh;

    ptr = (uint8_t*)nlmsg_data(nm_nlh);
    size = nm_nlh->nlmsg_len;

    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "nl_msg: size = %d", size);
    for(i = 0; i < size; i++)
    {
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "nl_msg: byte: %d", *(ptr + i));
    }
}

bool getNetworkProperties(const char *mac_char, NetType netType,
                        unsigned int *nss,
                        unsigned int *rateflags,
                        unsigned int *freq,
                        unsigned int *enable_4k)
{
    int err = 0;
    int ret,value,res;
    struct nl_cb *cb = NULL;
    struct nl_sock *sock = NULL;
    struct nl_msg *mMsg = NULL;
    void *msg_head = NULL;
    srand(getpid());
    struct nlattr *params;
    int i;
    uint32_t pid = getpid() & 0x3FFFFF;
    char macaddr[ETH_ALEN];
    char ifname[IFNAMSIZ] = {"p2p0"};
    tLinkProps sinfo;
    const std::string mac_string(mac_char);
    std::istringstream mac_string_stream(mac_string);
    unsigned int hex_val{0};
    char temp{0};

    i = 0;
    while (i < ETH_ALEN)
    {
        mac_string_stream >> std::hex >> hex_val;
        macaddr[i] = (unsigned char)hex_val;
        MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH, "macaddr[%d]: %d", i, macaddr[i]);
        i++;
        mac_string_stream >> temp;
    }

    memset(&sinfo, 0xFF, sizeof(sinfo));

    /* Get the interface name */
#ifdef WFD_ICS
    strlcpy(ifname, "p2p0", IFNAMSIZ);
#else
    if (getP2pInterfaceName(ifname, IFNAMSIZ, netType) == -1)
    {
        if (netType == WIGIG_P2P) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "getP2pInterfaceName failed "\
                    " Falling back to wigig0");
            strlcpy(ifname, "wigig0", IFNAMSIZ);//Do we need to re-query??
        } else {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "getP2pInterfaceName failed "\
                    " Falling back to p2p0");
            strlcpy(ifname, "p2p0", IFNAMSIZ);//Do we need to re-query??
        }
    }
    else
    {
        ifname[IFNAMSIZ-1] = '\0';
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, "getP2pInterfaceName ,"\
                                        "interface name = %s",ifname);
    }
#endif

    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "Creating Socket ");
    sock = nl_socket_alloc();
    mMsg = nlmsg_alloc();
    cb = nl_cb_alloc(NL_CB_DEFAULT);

    bool bOk = true;

    if(NULL == sock || NULL == mMsg || NULL == cb)
    {
        if (sock == NULL) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "Could not create socket handle ");
        }

        if (mMsg == NULL) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "Could not create nlMsg");
        }

        if (NULL == cb)  {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "Could not allocate cb");
        }

        bOk = false;
    }

    if (bOk)
    {
        if (WIFI_HAL_CMD_SOCK_PORT == 0)
        {
            sock->s_flags &= ~NL_OWN_PORT;
        } else {
            sock->s_flags |= NL_OWN_PORT;
        }
        sock->s_local.nl_pid = pid + (WIFI_HAL_CMD_SOCK_PORT << 22);

        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "Connecting socket ");
        if (err = nl_connect(sock, NETLINK_GENERIC)) {
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Could not connect handle: %d", err);
            bOk = false;
        }
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Socket Value:%p ", sock);
    }

    if (bOk)
    {
        int id = genl_ctrl_resolve(sock, "nl80211");
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "id is %d ",id);
        if(id < 0)
        {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "id is not valid");
            bOk = false;
        }
        else
        {
            msg_head = genlmsg_put(mMsg, /* pid = */ 0, /* seq = */ 0, /*family*/id,
                    /* hdrlen*/ 0,/* flags*/0,/* cmd*/ NL80211_CMD_VENDOR, /* version = */ 0);
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "msg_head : %p", msg_head);

            if(msg_head == NULL) {
                MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "error in put ");
                bOk = false;
            }
        }
    }

    if (bOk)
    {
        int ifindex = if_nametoindex(ifname);
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "ifindex is %d",ifindex);

        ret = nla_put_u32(mMsg, NL80211_ATTR_IFINDEX, ifindex);
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "ret from nla_put_u32 %d ",ret);


       // insert the oui in the msg
        value = OUI_QCA ;

        ret = nla_put_u32(mMsg,NL80211_ATTR_VENDOR_ID ,value);
        if (ret < 0)
        {
             bOk = false;
        }
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "vendor id value = %d ", value);
    }

    if (bOk)
    {
        value = QCA_NL80211_VENDOR_SUBCMD_LINK_PROPERTIES;
        ret = nla_put_u32(mMsg, NL80211_ATTR_VENDOR_SUBCMD,value);
        if (ret < 0)
        {
             bOk = false;
        }
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "subcmd id value = %d ", value);
    }

    if (bOk)
    {
        params = nla_nest_start(mMsg, NL80211_ATTR_VENDOR_DATA);
        if (!params) {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "nla_nest_start returned failure");
            return 0;
        }
        ret = nla_put(mMsg, QCA_WLAN_VENDOR_ATTR_MAC_ADDR ,ETH_ALEN ,macaddr);
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "nla_put %d " ,ret);
        if (ret < 0)
        {
            bOk = false;
        }
    }

    if (bOk)
    {
        nla_nest_end(mMsg, params);

        // callback
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "cb is %p",cb);

        nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
        nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
        nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
        nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler,&sinfo);

        err = nl_send_auto_complete(sock, mMsg);
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "auto complete %d " ,err );
        if(err<0)
        {
            bOk = false;
        }
    }

    if (bOk)
    {
        err = 1;

        while (err > 0)
        {
            res = nl_recvmsgs(sock, cb);
            MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "recvmsgs %d",res);
            if (res) {
                MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH, "nl80211: %s->nl_recvmsgs failed: %d",
                                                                            __func__, res);
            }
        }

        if(0xFFFFFFFF != sinfo.nss) /* only if response handler is called update this */
        {
            *nss = sinfo.nss;
            *freq = sinfo.freq;
            *rateflags = sinfo.rateflags;
        }

        if((sinfo.nss > 1) && (sinfo.freq > 5000) && (1)) /* Review the condition for enabling 4k
                                                             if we dont get a callback, we still want
                                                             to keep 4K enabled */
        {
            *enable_4k = 1;
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "wlan permits 4k ");
        }
        else
        {
            // default to high bitrate not allowed
            *enable_4k = 0;
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "wlan does not permit 4k ");
        }
    }

    // freeing allocations

    if (sock)
    {
        nl_socket_free(sock);
        sock = NULL;
    }

    if (mMsg)
    {
        nlmsg_free(mMsg);
        mMsg = NULL;
    }

    if (cb)
    {
        nl_cb_put(cb);
        cb = NULL;
    }

    return bOk;
}
