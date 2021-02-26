/******************************************************************************

                           N M _ M D M P R X Y . H

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef _NM_MDMPRXY_H_
#define _NM_MDMPRXY_H_

#ifndef str
#define str(X) #X
#endif

#ifndef strstr
#define strstr(X) str(X)
#endif

#define NM_MDMPRXY_VAR_UNUSED(var)  (var = var)

#define NM_MDMPRXY_NULL_CHECK(X) \
  if (!X) \
  { \
    netmgr_log_err("%s(): NULL parameter [%s] !!\n", __func__, #X); \
    NETMGR_ABORT("Received Null paramter for MDMPRXY check"); \
  }

#define NM_MDMPRXY_PORT_RANGE_MAX 36000

#define NM_MDMPRXY_IKE_REMOTE_PORT 4500

#define NM_MDMPRXY_DEFAULT_QOS_MARK   0
#define NM_MDMPRXY_MARK               9
#define NM_MDMPRXY_LBO_MARK           8
#define NM_MDMPRXY_DL_PING6_MARK      7


/* Port range which modem uses for iWLAN */
#define NM_MDMPRXY_MODEM_PORT_START       32000
#define NM_MDMPRXY_MODEM_PORT_END         36999

/* Definitions related ephemeral port  */
#define NM_MDMPRXY_MIN_EPHEMERAL_PORT     1025
#define NM_MDMPRXY_MAX_EPHEMERAL_PORT     65535

#define NM_MDMPRXY_SOCKET_HANDLE_INVALID  (-1)
#define NM_MDMPRXY_PORT_INVALID           (-1)

/* TODO: Get rid of all IMS port definitions since we are moving to emphemeral port design */

/* IMS port-range used for tcp handhsake */
#define NM_MDMPRXY_IMS_PORT_START         32100
#define NM_MDMPRXY_IMS_PORT_END           32600

#define NM_MDMPRXY_EX_IMS_PORT_START       40100
#define NM_MDMPRXY_EX_IMS_PORT_END         40150

/* IMS uses a random port between 32100 to 32600 for doing TCP handshake
   We have an iptables rule to mark and forward packets within this range
   The port uses for IKE and ESP needs to be outside this range to prevent
   collision */
#define NM_MDMPRXY_MODEM_PORT_RANGE_START 32601
#define NM_MDMPRXY_MODEM_PORT_RANGE_END   36999

#define NM_MDMPRXY_IMS_SIP_PORT           5060
#define NM_MDMPRXY_IMS_AUDIO_PORT_START   50010
#define NM_MDMPRXY_IMS_AUDIO_PORT_END     50060

#define NM_MDMPRXY_IKE_REMOTE_PORT        4500
#define NM_MDMPRXY_MODEM_NATT_PORT        32012

#define NM_MDMPRXY_IIFACE                 "wlan0"

#define NM_MDMPRXY_GATEWAY_V4_ADDR           0x000100FF          /* Little endian of 255.0.1.0 */
#define NM_MDMPRXY_GATEWAY_V4_ADDR_STR       "255.0.1.0"
#define NM_MDMPRXY_GATEWAY_V6_ADDR           "fd95:d1e:12::"

/* To mitigate fragmentation of IPv6 packets for iWLAN calls we need to restrict the MSS
   of the TCP session to a lower value. We use the value 1140 which was empirically
   determined on a test network that had been experiencing packet loss due to fragmentation

   This is how the 1140 MSS was calculated:
   1. When fragmentation doesn't happen, we see that tunneled packets carry
     140 bytes of overhead (IPv4=20, UDP=8, ESP=52, IPv6=40, TCP=20).
   2. Assuming all routers from MMSC to MT UE at least support 1280 MTU which
     is minimum IPv6 MTU.
   3. 1280-140=1140 TCP payload size */
#define NM_MDMPRXY_TCP_MSS                1140

#define FILEPATH_IP_LOCAL_RESERVED_PORTS "/proc/sys/net/ipv4/ip_local_reserved_ports"
#define FILEPATH_RESERVED_PORT_BIND      "/proc/sys/net/ipv4/reserved_port_bind"

/* TODO: For some reason SOL_UDP definition is not found on off-target environment even
   after including sys/socket.h. To get past the problem for now redeclare the constant here
   till we can figure out how to fix it the correct way */
#ifdef NETMGR_OFFTARGET
  #ifdef SOL_UDP
    #undef SOL_UDP
  #endif
  #define SOL_UDP 17
#endif /* NETMGR_OFFTARGET */

#endif /* _NM_MDMPRXY_H_ */
