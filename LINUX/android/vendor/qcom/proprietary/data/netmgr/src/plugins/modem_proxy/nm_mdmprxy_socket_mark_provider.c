/******************************************************************************

     N M _ M D M P R X Y _ S O C K E T _ M A R K _ P R O V I D E R . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmprxy_socket_mark_provider.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy plugin module is responsible for managing routing rules
  and packet marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <pthread.h>
#include <errno.h>

/* QMI DFS Includes */
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_client.h"
#include "qmi.h"

/* Socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/udp.h>
#ifndef NETMGR_OFFTARGET
#include <netinet/in6.h>
#endif

/* Standard Linux includes */
#include <unistd.h>
#include <error.h>
#include <time.h>

/* Netmgr core includes */
#include "ds_util.h"
#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_config.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_mark_provider_factory.h"
#include "nm_mdmprxy_iptables_helper.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
#define MAX_UDP_ENCAP_SOCKETS 5
#define MAX_OPEN_SOCKETS 601
#define NM_MDMPRXY_MAX_BIND_RETRIES 10
#define NM_MDMPRXY_MAX_RANDOM_PORT_RETRIES 100

#define NM_MDMPRXY_QMI_TIMEOUT  (10000)

typedef struct
{
  boolean is_v4_marking_rule_installed;
  boolean is_v6_marking_rule_installed;
  boolean is_v4_apps_marking_rule_installed;
  boolean is_v6_apps_marking_rule_installed;
} nm_mdmprxy_cfg_t;

static nm_mdmprxy_cfg_t global_config;

#define NM_MDMPRXY_GET_MARKING_RULES_INSTALLED(family)                 \
  ((AF_INET == family) ?                                               \
   global_config.is_v4_marking_rule_installed :                        \
   global_config.is_v6_marking_rule_installed)

#define NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(family, status)         \
  *(((AF_INET == family) ?                                             \
    &global_config.is_v4_marking_rule_installed :                      \
    &global_config.is_v6_marking_rule_installed)) = status

#define NM_MDMPRXY_GET_APPS_MARKING_RULES_INSTALLED(family)            \
  ((AF_INET == family) ?                                               \
   global_config.is_v4_apps_marking_rule_installed :                   \
   global_config.is_v6_apps_marking_rule_installed)

#define NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(family, status)    \
  *(((AF_INET == family) ?                                             \
    &global_config.is_v4_apps_marking_rule_installed :                 \
    &global_config.is_v6_apps_marking_rule_installed)) = status

#define MAX_SOCKET_LOGS (5000)
static int logging_counter;

#define NM_MDMPRXY_SOCKET_LOG(level,...)                                     \
  do {                                                                       \
    if (logging_fd)                                                          \
    {                                                                        \
      char timstamp[MAX_TIMESTAMP_LEN];                                      \
                                                                             \
      /* We need to prevent the log file from growing beyond a */            \
      /* certain point. Short term solution would be to empty the */         \
      /* file and restart logging. */                                        \
                                                                             \
      /* TODO: Proper solution would be to implement a circular log */       \
      /* buffer*/                                                            \
      if (MAX_SOCKET_LOGS == logging_counter)                                \
      {                                                                      \
        if(fclose(logging_fd))                                               \
        {                                                                    \
          /* Failed to close the file and we have exceeded the */            \
          /* the limit */                                                    \
          perror("Failed to close log file");                                \
          logging_fd = NULL;                                                 \
          break;                                                             \
        }                                                                    \
                                                                             \
        /* Re-open log file with w+ mode so that old contents */             \
        /* are cleared */                                                    \
        logging_fd = fopen(NETMGR_LOG_FILE_LOCATION, "w+e");                 \
        if (!logging_fd)                                                     \
        {                                                                    \
          log_to_file = FALSE;                                               \
          perror("failed to re-open log file");                              \
          break;                                                             \
        }                                                                    \
                                                                             \
        fprintf(logging_fd, "Reached maximum logging limit....restarting");  \
        fprintf(logging_fd,"\n");                                            \
        /* Reset logging counter */                                          \
        logging_counter = 0;                                                 \
      }                                                                      \
                                                                             \
      TIMESTAMP(timstamp);                                                   \
      fprintf(logging_fd,"%25s %70s %40s():%3u (INF):",                      \
              timstamp, __FILE__, __func__, __LINE__);                       \
      fprintf(logging_fd,__VA_ARGS__);                                       \
      fprintf(logging_fd,"\n");                                              \
      fflush(logging_fd);                                                    \
      logging_counter++;                                                     \
    }                                                                        \
    ds_log_##level(__VA_ARGS__);                                             \
} while (0);

/* QMI Handle information */
int                    pr_qmi_state;
qmi_client_type        pr_qmi_dfs_clnt_hndl;
qmi_cci_os_signal_type pr_qcci_os_params;
#define PR_QMI_STATE_UNINITIALIZED 0
#define PR_QMI_STATE_INITIALIZED   1

/* List of open handles. Caching these for SSR / cleanup only */
static int             pr_fds[MAX_OPEN_SOCKETS];
static int             udp_encap_hndls[MAX_UDP_ENCAP_SOCKETS];

/* Synchronize the v-table callbacks with QMI callbacks */
static pthread_mutex_t nm_mdmprxy_mark_lock = PTHREAD_MUTEX_INITIALIZER;

static const struct nm_mdmprxy_cmd static_rule_list[] =
{
  {"%s %s -t mangle -N nm_mdmprxy_pkt_skmark",
   "%s %s -t mangle -X nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  {"%s %s -t mangle -A nm_mdmprxy_pkt_skmark -j RETURN",
   "%s %s -t mangle -D nm_mdmprxy_pkt_skmark -j RETURN",
   nm_mdmprxy_basic_formatter},

  {"%s %s -t mangle -A nm_mdmprxy_mark_prov_chain -p udp"
     " -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   "%s %s -t mangle -D nm_mdmprxy_mark_prov_chain -p udp"
     " -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  {"%s %s -t mangle -A nm_mdmprxy_mark_prov_chain -p tcp"
     " -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   "%s %s -t mangle -D nm_mdmprxy_mark_prov_chain -p tcp"
     " -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  /********************************************************************************/
  /* TODO: Common junk which needs to be de-duplicated from legacy provider       */
  /********************************************************************************/

  /* save any packet mark on to the connection (for handling fragmented IPv6 pkts) */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    nm_mdmprxy_connmark_save_formatter },

  /* mark traffic destined to the standard SIP port to be forwarded to the modem */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p udp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p udp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p tcp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p tcp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    nm_mdmprxy_basic_formatter },

  /* Insert a jump rule to the SPI matching table from pkt_marker table */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    nm_mdmprxy_basic_formatter },

  /* Jump to mark prov chain which hosts the modem port-range rules */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    nm_mdmprxy_basic_formatter },

  /* Restore mark on icmp packets originating from modem using connection tracking
     framework to enable pings originiating from modem to be successful */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    nm_mdmprxy_basic_formatter }
};

/* Hosts all the application specific port-based packet marking rules
   Currently IMS application is the only one which has specific port requirements

   32100 - 32600 : Ports used for TCP connection
   40100 - 40150 : Additional ports used for operator specific requirements
   50010 - 50060 : Audio ports

   nm_mdmprxy_mngl_pre_ex chain will be used to host all these rules */
static const struct nm_mdmprxy_cmd apps_ports_rule_list[] =
{
  {"%s %s -t mangle -A nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s %s -t mangle -D nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter },

  {"%s %s -t mangle -A nm_mdmprxy_mngl_pre_ex -p udp"
       " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s %s -t mangle -D nm_mdmprxy_mngl_pre_ex -p udp"
       " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter },


  {"%s %s -t mangle -A nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s %s -t mangle -D nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter }
};

/* All IKE in UDP packets destined to the Modem NATT port must be sent to the
 * Modem and other IKE/ESP in UDP to the AP network stack for decryption and
 * subsequent forwarding based on innner packet contents. This is how we can
 * differentiate between the two: the first 4 bytes of the UDP payload of
 * IKE in UDP is always 0 and non-zero for ESP in UDP packets (contains SPI)
 *
 * Start at the IP header, extract IHL and add the offset to get to the
 * start of UDP header and then offset 8 (UDP header) bytes to get to the
 * payload. If the 4 bytes are 0 then mark the packet so that it gets
 * forwarded to the Modem */

static const struct nm_mdmprxy_cmd udp_encap_rule_list[] =
{
  {"%s  %s -t mangle -I nm_mdmprxy_mark_prov_chain -p udp"
     " --sport 500 --dport %d -j MARK --set-mark %u",
   "%s  %s -t mangle -D nm_mdmprxy_mark_prov_chain -p udp"
     " --sport 500 --dport %d -j MARK --set-mark %u",
   nm_mdmprxy_encaprule_dport_formatter},

  {"%s %s -t mangle -I nm_mdmprxy_mark_prov_chain -p udp"
     " --dport %d -m u32 --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
   "%s %s -t mangle -D nm_mdmprxy_mark_prov_chain -p udp"
     " --dport %d -m u32 --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
   nm_mdmprxy_encaprule_dport_formatter}
};

extern int
nm_mdmprxy_common_mark_prov_install_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_remove_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_install_portfwd_rule(sa_family_t, boolean);

extern int
nm_mdmprxy_common_mark_prov_remove_portfwd_rule(sa_family_t, boolean);

extern boolean
nm_mdmprxy_common_mark_prov_query_portfwd_status(sa_family_t);

extern int
nm_mdmprxy_common_mark_prov_install_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_remove_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_oos_cleanup(void);

static int
nm_mdmprxy_socket_mark_prov_init(void);

static int
nm_mdmprxy_socket_mark_prov_is_hdlr(void);

static int
nm_mdmprxy_socket_mark_prov_oos_hdlr(void);

static void
nm_mdmprxy_socket_mark_prov_status(void);

extern void
nm_mdmprxy_common_mark_prov_setup_iptables(struct mark_prov_iptables *);

nm_mdmprxy_mark_prov_t nm_mdmprxy_socket_mark_provider =
{
  .install_rules        = 0,
  .remove_rules         = 0,

  .install_spi_rule     = nm_mdmprxy_common_mark_prov_install_spi_rule,
  .remove_spi_rule      = nm_mdmprxy_common_mark_prov_remove_spi_rule,
  .install_portfwd_rule = nm_mdmprxy_common_mark_prov_install_portfwd_rule,
  .remove_portfwd_rule  = nm_mdmprxy_common_mark_prov_remove_portfwd_rule,
  .query_portfwd_status = nm_mdmprxy_common_mark_prov_query_portfwd_status,
  .install_icmp_rule    = nm_mdmprxy_common_mark_prov_install_icmp_rules,
  .remove_icmp_rule     = nm_mdmprxy_common_mark_prov_remove_icmp_rules,

  .init                 = nm_mdmprxy_socket_mark_prov_init,
  .is_hdlr              = nm_mdmprxy_socket_mark_prov_is_hdlr,
  .oos_hdlr             = nm_mdmprxy_socket_mark_prov_oos_hdlr,
  .sigusr1_hdlr         = nm_mdmprxy_socket_mark_prov_status,
};

#define nm_mdmprxy_socket_mark_lock() \
  pthread_mutex_lock(&nm_mdmprxy_mark_lock); \
  netmgr_log_low("%s(): locked nm_mdmprxy_mark_lock", __func__);

#define nm_mdmprxy_socket_mark_unlock() \
  pthread_mutex_unlock(&nm_mdmprxy_mark_lock); \
  netmgr_log_low("%s(): unlocked nm_mdmprxy_mark_lock", __func__);

static struct mark_prov_iptables socket_mark_prov_cmd_tbl;

/* Information about allocated adjacent sockets */
struct adj_alloc_port_info
{
  int even_port;
  int even_handle;
  int odd_port;
  int odd_handle;
};

/* Information about allocated symmetric sockets */
struct symm_alloc_port_info
{
  int tcp_port_v4;
  int tcp_port_v6;
  int udp_port_v4;
  int udp_port_v6;
  int tcp_handle_v4;
  int tcp_handle_v6;
  int udp_handle_v4;
  int udp_handle_v6;
};

/* Default socket allocation information */
struct def_alloc_port_info
{
  int port;
  int handle;
};

/* OUT params which will be filled in by the different socket allocation functions
   This is mainly used to return the information about the allocated sockets back
   to the calling function. In case of errors the caller can use this information
   to perform cleanup */
typedef union
{
  struct adj_alloc_port_info  adj_ports_info;
  struct symm_alloc_port_info symm_ports_info;
  struct def_alloc_port_info  def_ports_info;
} allocated_port_info_t;

/* IN params which is passed to socket allocation functions to provide information about modem
   port preference */
typedef struct port_params
{
  int port;               /* Provided by modem in the request TLV, can be zero */
  uint16_t min_range;     /* Base port */
  uint16_t max_range;     /* Max port */
  int mark;               /* Mark to be used in setsockopt */
  int xport_prot;         /* Provided by modem in the request TLV */
  int af;                 /* Address family, AF_UNSPEC if modem wants ports for both V4 and V6 */
} requested_port_params_t;

/*===========================================================================
                     Socket fanciness
===========================================================================*/
#define for_each_fd(X) \
  for (X = 0; X < MAX_OPEN_SOCKETS; X++) \
    if (pr_fds[i] != 0)

/* Macro to read modem provided port-range and convert it into internal
   port range values which can be used for socket allocations */
#define ADJUST_PORT_RANGE(base_port_value, max_port_value,                  \
                            min_range, max_range, port_range_valid)         \
  if (base_port_value > (uint16_t) NM_MDMPRXY_MAX_EPHEMERAL_PORT)           \
  {                                                                         \
    netmgr_log_err("%s(): base_port_value[%d] invalid!",                    \
                   __func__, base_port_value);                              \
    port_range_valid = FALSE;                                               \
  }                                                                         \
  else if (max_port_value > (uint16_t) NM_MDMPRXY_MAX_EPHEMERAL_PORT)       \
  {                                                                         \
    netmgr_log_err("%s(): max_port_value[%d] invalid!",                     \
                   __func__, max_port_value);                               \
    port_range_valid = FALSE;                                               \
  }                                                                         \
  else if ( (max_port_value != 0) && (base_port_value > max_port_value) )   \
  {                                                                         \
    netmgr_log_err("%s(): base_port_value[%d] cannot be greater than"       \
                   " max_port_value[%d]!",                                  \
                   __func__, base_port_value, max_port_value);              \
    port_range_valid = FALSE;                                               \
  }                                                                         \
  else if (base_port_value == 0 && max_port_value > 0)                      \
  {                                                                         \
    min_range = NM_MDMPRXY_MIN_EPHEMERAL_PORT;                              \
    max_range = max_port_value;                                             \
  }                                                                         \
  else if (base_port_value > 0 && max_port_value == 0)                      \
  {                                                                         \
    min_range = base_port_value;                                            \
    max_range = NM_MDMPRXY_MAX_EPHEMERAL_PORT;                              \
  }                                                                         \
  else if (base_port_value > 0 && max_port_value > 0)                       \
  {                                                                         \
    min_range = base_port_value;                                            \
    max_range = max_port_value;                                             \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    min_range = NM_MDMPRXY_MODEM_PORT_RANGE_START;                          \
    max_range = NM_MDMPRXY_MODEM_PORT_RANGE_END;                            \
  }

/* Macro to compute the maximum sockets to be allocted for a given
   combination of port preference, ip family and xport protocol */
#define NO_OF_SOCKETS(no_of_sockets, port_prop, ip_family, xport_prot)           \
  switch (port_prop)                                                             \
  {                                                                              \
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:                                    \
    if(AF_INET == ip_family || AF_INET6 == ip_family)                            \
    {                                                                            \
      if (DFS_PROTO_TCPUDP_V01 == xport_prot)                                    \
      {                                                                          \
        no_of_sockets = 4;                                                       \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        no_of_sockets = 2;                                                       \
      }                                                                          \
    }                                                                            \
    else if(AF_UNSPEC == ip_family)                                              \
    {                                                                            \
      if (DFS_PROTO_TCPUDP_V01 == xport_prot)                                    \
      {                                                                          \
        no_of_sockets = 8;                                                       \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        no_of_sockets = 4;                                                       \
      }                                                                          \
    }                                                                            \
    break;                                                                       \
  case DFS_PORT_PROP_SYMMETRIC_V01:                                              \
    if(AF_INET == ip_family || AF_INET6 == ip_family)                            \
    {                                                                            \
      no_of_sockets = 2;                                                         \
    }                                                                            \
    else if(AF_UNSPEC == ip_family)                                              \
    {                                                                            \
      no_of_sockets = 4;                                                         \
    }                                                                            \
    break;                                                                       \
  case DFS_PORT_PROP_MIN_V01:                                                    \
  default:                                                                       \
    no_of_sockets = 1;                                                           \
    break;                                                                       \
  }

/* Macro to get random even and odd port to be used for adjacent socket allocation
   requests */
#define GET_EVEN_AND_ODD_PORTS(even_port, odd_port, modem_req_port, min_range, max_range)  \
  int k = 0;                                                                               \
                                                                                           \
  while (k < NM_MDMPRXY_MAX_RANDOM_PORT_RETRIES)                                           \
  {                                                                                        \
    even_port = NM_MDMPRXY_PORT_INVALID;                                                   \
    odd_port = NM_MDMPRXY_PORT_INVALID;                                                    \
                                                                                           \
    if (modem_req_port == 0)                                                               \
    {                                                                                      \
      even_port = min_range + (rand() % (max_range - min_range));                          \
      if (even_port % 2 != 0)                                                              \
      {                                                                                    \
        even_port++;                                                                       \
        if (even_port > NM_MDMPRXY_MAX_EPHEMERAL_PORT || even_port > max_range)            \
        {                                                                                  \
          k++;                                                                             \
          continue;                                                                        \
        }                                                                                  \
      }                                                                                    \
                                                                                           \
      odd_port = even_port + 1;                                                            \
      if (odd_port > NM_MDMPRXY_MAX_EPHEMERAL_PORT || odd_port > max_range)                \
      {                                                                                    \
        k++;                                                                               \
        continue;                                                                          \
      }                                                                                    \
                                                                                           \
      break;                                                                               \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
      even_port = modem_req_port;                                                          \
      odd_port  = even_port + 1;                                                           \
                                                                                           \
      if (odd_port > NM_MDMPRXY_MAX_EPHEMERAL_PORT || odd_port > max_range)                \
      {                                                                                    \
        netmgr_log_err("%s(): Modem provided port [%d] is incorrect! Caused odd"           \
                       " port to exceed beyond max ephemeral port range!",                 \
                       __func__, modem_req_port);                                          \
        even_port = NM_MDMPRXY_PORT_INVALID;                                               \
        odd_port = NM_MDMPRXY_PORT_INVALID;                                                \
      }                                                                                    \
                                                                                           \
      break;                                                                               \
    }                                                                                      \
  }

/* Macro to get random even and odd port to be used for symmetric socket allocation
   requests */
#define GET_SYMMETRIC_PORT(symm_port, modem_req_port, min_range, max_range)                \
  int k = 0;                                                                               \
                                                                                           \
  while (k < NM_MDMPRXY_MAX_RANDOM_PORT_RETRIES)                                           \
  {                                                                                        \
    symm_port = NM_MDMPRXY_PORT_INVALID;                                                   \
                                                                                           \
    if (modem_req_port == 0)                                                               \
    {                                                                                      \
      symm_port = min_range + (rand() % (max_range - min_range));                          \
      if (symm_port > NM_MDMPRXY_MAX_EPHEMERAL_PORT || symm_port > max_range)              \
      {                                                                                    \
        /* We cannot go beyond the max ephemeral port range */                             \
        k++;                                                                               \
        continue;                                                                          \
      }                                                                                    \
                                                                                           \
      break;                                                                               \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
      symm_port = modem_req_port;                                                          \
      if (symm_port > NM_MDMPRXY_MAX_EPHEMERAL_PORT || symm_port > max_range)              \
      {                                                                                    \
        netmgr_log_err("%s(): Modem provided port [%d] is incorrect! Caused odd"           \
                       " symmetric port to exceed beyond max"                              \
                       " port range!",                                                     \
                       __func__, modem_req_port);                                          \
        symm_port = NM_MDMPRXY_PORT_INVALID;                                               \
      }                                                                                    \
                                                                                           \
      break;                                                                               \
    }                                                                                      \
  }

/* Macro to get random even and odd port to be used for default socket allocation
   requests */
#define GET_RANDOM_PORT(r_port, modem_req_port, min_range, max_range)                      \
  do                                                                                       \
  {                                                                                        \
    if (modem_req_port == 0)                                                               \
    {                                                                                      \
      r_port = min_range + (rand() % (max_range - min_range));                             \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
      r_port = modem_req_port;                                                             \
    }                                                                                      \
} while (0);

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_set_reserved_ports
===========================================================================*/
/*!
@brief
  Sets the 'reserved_port_bind' proc entry to block explicit bind
  to a specific range of ports
*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_set_reserved_ports(void)
{
  FILE *fp = NULL;
  const char* enable_blocking_bind = "0";
  const char* port_range = strstr(NM_MDMPRXY_IMS_PORT_START) "-" strstr(NM_MDMPRXY_IMS_PORT_END) ","
                         strstr(NM_MDMPRXY_EX_IMS_PORT_START) "-" strstr(NM_MDMPRXY_EX_IMS_PORT_END) ","
                         strstr(NM_MDMPRXY_IMS_AUDIO_PORT_START) "-" strstr(NM_MDMPRXY_IMS_AUDIO_PORT_END);

  if (DFS_REMOTE_SOCKET_HANDLING_V1_V01 == nm_mdmprxy_get_dfs_service_version())
  {
    netmgr_log_low("%s(): nothing to do", __func__);
    return;
  }

  /* Check if we can access the reserved_port_bind */
  if (access(FILEPATH_RESERVED_PORT_BIND, F_OK))
  {
    /* We are unable to access the file */
    netmgr_log_err("%s(): unable access reserved_port_bind /proc entry [%s]",
                   __func__, strerror(errno));
    return;
  }

  fp = fopen(FILEPATH_RESERVED_PORT_BIND, "we");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open reserved_port_bind /proc file [%s]",
                   __func__, strerror(errno));
    return;
  }

  fprintf(fp, "%s", enable_blocking_bind);
  fclose(fp);
  fp = NULL;

  /* Check if we can access the ip_local_reserved_ports */
  if (access(FILEPATH_IP_LOCAL_RESERVED_PORTS, F_OK))
  {
    netmgr_log_err("%s(): unable to access reserved_port_bind /proc entry [%s]",
                   __func__, strerror(errno));
    return;
  }

  fp = fopen(FILEPATH_IP_LOCAL_RESERVED_PORTS, "we");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open ip_local_reserved_ports /proc file [%s]",
                   __func__, strerror(errno));
    return;
  }

  fprintf(fp, "%s", port_range);
  fclose(fp);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_port
===========================================================================*/
/*!
@brief
  Query the kernel to get the port number of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  port number otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_port
(
  unsigned int handle
)
{
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  int rc;

  rc = getsockname(pr_fds[handle], (struct sockaddr *)&addr, &len);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to get socket name for handle %u",
                   __func__, handle);
    return 0;
  }

  switch(addr.ss_family)
  {
    case AF_INET:
      return ntohs(((struct sockaddr_in *)&addr)->sin_port);

    case AF_INET6:
      return ntohs(((struct sockaddr_in6 *)&addr)->sin6_port);

    default:
      netmgr_log_err("%s(): Unkown AF: %d", __func__, addr.ss_family);
      return 0;
  }
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_af
===========================================================================*/
/*!
@brief
  Query the kernel to get the address family of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  address family otherwise (AF_INET/AF_INET6)

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_af
(
  unsigned int handle
)
{
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  int rc;

  rc = getsockname(pr_fds[handle], (struct sockaddr *)&addr, &len);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to get socket name for handle %u",
                   __func__, handle);
    return 0;
  }

  return addr.ss_family;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_type
===========================================================================*/
/*!
@brief
  Query the kernel to get the type of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  socket type otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_type
(
  unsigned int handle
)
{
  int type, rc;
  socklen_t optlen = sizeof(type);

  rc = getsockopt(pr_fds[handle], SOL_SOCKET, SO_PROTOCOL, &type, &optlen);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to getsockopt for handle %u",
                   __func__, handle);
    return 0;
  }
  /* TODO: Check optlen */
  return type;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_print_ports
===========================================================================*/
/*!
@brief
  Print all ports opened by netmgr port reserver to the logs

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_prov_status()
{
  int i;
  nm_mdmprxy_socket_mark_lock();
  for_each_fd(i)
  {
    netmgr_log_low("sockets[%d]: handle:%d af:%d type:%d port:%d",
                   i,
                   pr_fds[i],
                   nm_mdmprxy_socket_mark_get_socket_af(i),
                   nm_mdmprxy_socket_mark_get_socket_type(i),
                   nm_mdmprxy_socket_mark_get_socket_port(i));
  }
  nm_mdmprxy_socket_mark_unlock();
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_if_udp_encap_cleanup_rules
===========================================================================*/
/*!
@brief
  Check if the handle passed in is for a UDP encap port, if yes cleanup
  the UDP encap rules

@arg handle - pr_fds handle to close
@arg port   - port to close

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_if_udp_encap_cleanup_rules
(
  int handle,
  int port
)
{
  int i = 0;
  int rc = NETMGR_FAILURE;

  for (i = 0; i < MAX_UDP_ENCAP_SOCKETS; i++)
  {
    if (udp_encap_hndls[i] >= 0 && handle == udp_encap_hndls[i])
    {
      netmgr_log_med("%s(): cleanup UDP_ENCAP handle [%d], port [%d]",
                     __func__, udp_encap_hndls[i], port);

      /* Invalidate handle */
      udp_encap_hndls[i] = -1;

      /* Cleanup IKE port references */
      netmgr_main_cfg_cleanup_ike_port(port);

      rc = nm_mdmprxy_uninstall_udpencap_rules(port, NM_MDMPRXY_IKE_REMOTE_PORT, NM_MDMPRXY_MARK);
      if (rc != NETMGR_SUCCESS)
      {
        netmgr_log_err("%s(): Failed to uninstall UDP Encap rules!", __func__);
      }

      break;
    }
  }

  if (MAX_UDP_ENCAP_SOCKETS == i)
  {
    rc = NETMGR_FAILURE;
  }

  return rc;
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_close_socket
===========================================================================*/
/*!
@brief
  Check if the handle is a UDP ENCAP. If it is, do some cleanup.
  Then close the socket.

@arg handle - pr_fds handle to close

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_close_socket
(
  unsigned int handle
)
{
  int port, rc;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Error: handle out of range (%u)", __func__, handle);
    return;
  }

  if (pr_fds[handle] == 0)
  {
    /* Nothing to do */
    netmgr_log_med("%s(): Could not find reference to handle [%u]", __func__, handle);
    return;
  }

  port = nm_mdmprxy_socket_mark_get_socket_port(handle);

  if (NETMGR_SUCCESS == nm_mdmprxy_socket_mark_if_udp_encap_cleanup_rules(handle, port))
  {
    /* Log the message to file */
    NM_MDMPRXY_SOCKET_LOG(err, "Destroy UDP encap socket handle: %u port:%u",
                          handle, port);
  }
  else
  {
    NM_MDMPRXY_SOCKET_LOG(err, "Close socket handle: %u port:%u", handle, port);
  }

  close(pr_fds[handle]);
  pr_fds[handle] = 0;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_close_all_ports
===========================================================================*/
/*!
@brief
  Close all all ports opened by netmgr port reserver

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_close_all_ports()
{
  int i;

  for_each_fd(i)
  {
    nm_mdmprxy_socket_mark_close_socket(i);
  }
  netmgr_log_low("%s(): Closed all ports", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_find_empty_port_index
===========================================================================*/
/*!
@brief
  Simple linear search over pr_fds[] to find an unused index
  It is the caller's responsibility to ensure that the return value
  is within range.

@return
  a positive value from 1 to MAX_OPEN_SOCKETS

*/
/*=========================================================================*/
static unsigned int
nm_mdmprxy_socket_mark_find_empty_port_index()
{
  unsigned int i;

  for (i = 1; i < MAX_OPEN_SOCKETS; i++)
  {
    if (pr_fds[i] == 0)
      return i;
  }

  return i;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_find_empty_udp_encap_handle_index
===========================================================================*/
/*!
@brief
  Search the UDP encap handle array to check if the given handle already
  exists in the list

@return
  NETMGR_FAILURE if the UDP_ENCAP socket for the handle already exists
  NETMGR_SUCCESS otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_check_if_udp_encap_port_exists(unsigned int handle)
{
  unsigned int i;

  for (i = 0; i < MAX_UDP_ENCAP_SOCKETS; i++)
  {
    if (udp_encap_hndls[i] == (int) handle)
    {
      netmgr_log_err("%s(): the requested UDP encap handle [%u] was already processed!"
                     " Duplicate request!",__func__, handle);
      return NETMGR_FAILURE;
    }
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_find_empty_udp_encap_handle_index
===========================================================================*/
/*!
@brief
  Simple linear search over udp_encap_hndls[] to find an unused index
  It is the caller's responsibility to ensure that the return value
  is within range.

@return
  a positive value from 0 to MAX_UDP_ENCAP_SOCKETS

*/
/*=========================================================================*/
static unsigned int
nm_mdmprxy_socket_mark_find_empty_udp_encap_handle_index(void)
{
  unsigned int i;

  for (i = 0; i < MAX_UDP_ENCAP_SOCKETS; i++)
  {
    if (udp_encap_hndls[i] == -1)
      return i;
  }

  return i;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_socket_mark_socket_bind
===========================================================================*/
/*!
@brief
  Finds an empty slot in pr_fds[], allocates a socket,
  sets it to transparent mode, and binds

@arg type - SOCK_DGRAM/SOCK_STREAM
@arg source - AF + Port to bind to. Can be sockaddr_in or sockaddr_in6
@arg slen - size of source struct

@return
  NM_MDMPRXY_SOCKET_HANDLE_INVALID on failure
  pr_fds index otherwise

*/
/*=========================================================================*/
static int
_nm_mdmprxy_socket_mark_socket_bind
(
  int type,
  struct sockaddr_storage *source,
  ssize_t slen,
  uint32_t mark
)
{
  unsigned int idx;
  int fd, rc, val;

  idx = nm_mdmprxy_socket_mark_find_empty_port_index();

  if (idx >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Too many sockets, close some before requesting more",
                   __func__);
    return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  }

  if (source->ss_family == AF_UNSPEC)
    fd = socket(AF_INET6, type | SOCK_CLOEXEC, 0);
  else
    fd = socket(source->ss_family, type | SOCK_CLOEXEC, 0);

  if (fd < 0)
  {
    netmgr_log_err("%s(): Unable to open socket: %d-%s\n",
                   __func__, errno, strerror(errno));
    return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  }

  val = 1;
  if (source->ss_family == AF_INET6)
  {
    rc = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));
    if (rc < 0)
    {
      close(fd);
      netmgr_log_err("Unable to set IPV6_V6ONLY: %d-%s\n",
                     errno, strerror(errno));
      return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
    }
  }

  val = 1;
  rc = setsockopt(fd, SOL_IP, IP_TRANSPARENT, &val, sizeof(val));
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to set IP_TRANSPARENT: %d-%s\n",
                   errno, strerror(errno));
    return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  }

  val = mark;
  rc = setsockopt(fd, SOL_SOCKET, SO_MARK, &val, sizeof(val));
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to set SO_MARK: %d-%s\n", errno, strerror(errno));
    return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  }

  rc = bind(fd, (struct sockaddr*)source, slen);
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to bind socket: %d-%s\n",
                   errno, strerror(errno));
    return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  }

  if (type == SOCK_STREAM)
  {
    rc = listen(fd, SOMAXCONN);
    if (rc < 0)
    {
      close(fd);
      netmgr_log_err("Unable to listen on socket: %d-%s\n",
                     errno, strerror(errno));
      return NM_MDMPRXY_SOCKET_HANDLE_INVALID;
    }
  }

  pr_fds[idx] = fd;

  return (int)idx;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v4_tcp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve TCP V4 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by
  modem are enforced if valid.

  The function is responsible for filling in the resp_req_msg structure
  which is passed in as OUT parameter and also advance the current index
  within the allocated socket list. If modem requested for ephemeral ports
  and bind failed the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v4_tcp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int j = 0;
  int rc = NETMGR_FAILURE;

  /* Port defs */
  int even_port = NM_MDMPRXY_PORT_INVALID;
  int odd_port = NM_MDMPRXY_PORT_INVALID;
  int symm_port = NM_MDMPRXY_PORT_INVALID;
  int r_port = NM_MDMPRXY_PORT_INVALID;

  /* Handle defs */
  int idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd = NM_MDMPRXY_SOCKET_HANDLE_INVALID;;
  int idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  int retries = 0;
  struct sockaddr_in sockaddr_v4;

  if (!port_params || !resp_req_msg || !current_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  if (port_params->xport_prot != DFS_PROTO_TCP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  j = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    do
    {
      idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_odd  = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      GET_EVEN_AND_ODD_PORTS(even_port, odd_port, port_params->port,
                             port_params->min_range, port_params->max_range);

      if (even_port == NM_MDMPRXY_PORT_INVALID || odd_port == NM_MDMPRXY_PORT_INVALID)
      {
        netmgr_log_err("%s(): unable to process port preference request!", __func__);
        break;
      }

      /* Try to bind to V4 even port */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(even_port & 0xFFFF);
      idx_even = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                     (struct sockaddr_storage *)&sockaddr_v4,
                                                     sizeof(sockaddr_v4),
                                                     port_params->mark);

      /* Try to bind to V4 odd port */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(odd_port & 0xFFFF);
      idx_odd = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                    (struct sockaddr_storage *)&sockaddr_v4,
                                                     sizeof(sockaddr_v4),
                                                     port_params->mark);

      if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
            && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check sockets were allocated on the expected ports */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx_even) != even_port
              || nm_mdmprxy_socket_mark_get_socket_port(idx_odd) != odd_port)
        {
          netmgr_log_err("%s(): sockets allocated on the wrong ports!", __func__);

          /* Close the incorrectly allocated sockets */
          nm_mdmprxy_socket_mark_close_socket(idx_even);
          nm_mdmprxy_socket_mark_close_socket(idx_odd);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET, SOCK_STREAM, even_port, retries);
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET, SOCK_STREAM, odd_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                           __func__, AF_INET, SOCK_STREAM, even_port);
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                           __func__, AF_INET, SOCK_STREAM, odd_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_even, AF_INET, SOCK_STREAM, even_port);
        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_odd, AF_INET, SOCK_STREAM, odd_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_even;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_even);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        resp_req_msg->socket_list[j+1].socket_handle = (uint32_t) idx_odd;
        resp_req_msg->socket_list[j+1].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j+1].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_odd);
        resp_req_msg->socket_list[j+1].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j+1].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        j = j + 2;

        /* Update current index */
        *current_index = j;

        if (out_port_info)
        {
          out_port_info->adj_ports_info.even_port = even_port;
          out_port_info->adj_ports_info.even_handle = idx_even;
          out_port_info->adj_ports_info.odd_port = odd_port;
          out_port_info->adj_ports_info.odd_handle = idx_odd;
        }

        rc = NETMGR_SUCCESS;
      }
      else if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_even);
        if (port_params->port == 0)
        {
          /* Even port succeeded but odd port failed */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d odd port: %d try:%d",
                         __func__, AF_INET, SOCK_STREAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d odd port: %d",
                         __func__, AF_INET, SOCK_STREAM, odd_port);
          break;
        }
      }
      else if (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_odd);
        if (port_params->port == 0)
        {
          /* Even port failed but odd port succeeded */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d even port: %d try:%d",
                         __func__, AF_INET, SOCK_STREAM, even_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d even port: %d",
                         __func__, AF_INET, SOCK_STREAM, even_port);
          break;
        }
      }
      else
      {
        /* Both ports failed */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET, SOCK_STREAM, even_port, retries);
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET, SOCK_STREAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                         __func__, AF_INET, SOCK_STREAM, even_port);
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                         __func__, AF_INET, SOCK_STREAM, odd_port);
          break;
        }
      }

      retries++;
    } while ( (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                   && port_params->port == 0
                   && retries < NM_MDMPRXY_MAX_BIND_RETRIES );

    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    /* Create a TCP v4 socket on the given symmetric port */
    symm_port = port_params->port;

    /* Try to bind to v4 symmetric port */
    memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
    sockaddr_v4.sin_family = AF_INET;
    sockaddr_v4.sin_port = htons(symm_port & 0xFFFF);
    idx_tcp = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                   (struct sockaddr_storage *)&sockaddr_v4,
                                                   sizeof(sockaddr_v4),
                                                   port_params->mark);

    if (idx_tcp != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
    {
      /* Double-check whether the sockets were allocated on the expected port */
      if (nm_mdmprxy_socket_mark_get_socket_port(idx_tcp) != symm_port)
      {
        netmgr_log_err("%s(): socket not allocated on the correct port!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx_tcp);
        goto bail;
      }

      netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx_tcp, AF_INET, SOCK_STREAM, symm_port);

      /* Fill response message structure */
      resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_tcp;
      resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
      resp_req_msg->socket_list[j].socket_info.port_no
        = nm_mdmprxy_socket_mark_get_socket_port(idx_tcp);
      resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
      resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

      j = j + 1;

      /* Update current index */
      *current_index = j;

      if (out_port_info)
      {
        out_port_info->symm_ports_info.tcp_handle_v4 = idx_tcp;
        out_port_info->symm_ports_info.tcp_port_v4 = symm_port;
      }

      rc = NETMGR_SUCCESS;
    }

    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      GET_RANDOM_PORT(r_port, port_params->port,
                      port_params->min_range, port_params->max_range);

      /* Try to create a TCP socket */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                (struct sockaddr_storage *)&sockaddr_v4,
                                                sizeof(sockaddr_v4),
                                                port_params->mark);

      if (idx != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check whether the socket was allocated on the expected port */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx) != r_port)
        {
          netmgr_log_err("%s(): socket allocated on the wrong port!", __func__);
          nm_mdmprxy_socket_mark_close_socket(idx);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET, SOCK_STREAM, r_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET, SOCK_STREAM, r_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx, AF_INET, SOCK_STREAM, r_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        j = j + 1;

        /* Update current index */
        *current_index = j;

        rc = NETMGR_SUCCESS;
      }
      else if (port_params->port == 0)
      {
        netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET, SOCK_STREAM, r_port, retries);
      }
      else
      {
        netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET, SOCK_STREAM, r_port);
        break;
      }

      retries++;
    } while (idx == NM_MDMPRXY_SOCKET_HANDLE_INVALID
               && port_params->port == 0
               && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v4_udp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve UDP V4 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by modem
  are enforced if valid.

  The function is responsible for filling in the resp_req_msg structure
  which is passed in as OUT parameter and also advance the current index
  within the allocated socket list. If modem requested for ephemeral ports
  and bind failed the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v4_udp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int j = 0;
  int rc = NETMGR_FAILURE;

  /* Port defs */
  int even_port = NM_MDMPRXY_PORT_INVALID;
  int odd_port = NM_MDMPRXY_PORT_INVALID;
  int symm_port = NM_MDMPRXY_PORT_INVALID;
  int r_port = NM_MDMPRXY_PORT_INVALID;

  /* Handle defs */
  int idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  int retries = 0;
  struct sockaddr_in sockaddr_v4;

  if (!port_params || !resp_req_msg || !current_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  if (port_params->xport_prot != DFS_PROTO_UDP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  j = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    do
    {
      idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_odd  = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      GET_EVEN_AND_ODD_PORTS(even_port, odd_port, port_params->port,
                             port_params->min_range, port_params->max_range);

      if (even_port == NM_MDMPRXY_PORT_INVALID || odd_port == NM_MDMPRXY_PORT_INVALID)
      {
        netmgr_log_err("%s(): unable to process port preference request!", __func__);
        break;
      }

      /* Try to bind to V4 even port */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(even_port & 0xFFFF);
      idx_even = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                     (struct sockaddr_storage *)&sockaddr_v4,
                                                     sizeof(sockaddr_v4),
                                                     port_params->mark);

      /* Try to bind to V4 odd port */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(odd_port & 0xFFFF);
      idx_odd = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                    (struct sockaddr_storage *)&sockaddr_v4,
                                                    sizeof(sockaddr_v4),
                                                    port_params->mark);

      if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
            && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check sockets were allocated on the expected ports */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx_even) != even_port
              || nm_mdmprxy_socket_mark_get_socket_port(idx_odd) != odd_port)
        {
          netmgr_log_err("%s(): sockets allocated on the wrong ports!", __func__);

          /* Close the incorrectly allocated sockets */
          nm_mdmprxy_socket_mark_close_socket(idx_even);
          nm_mdmprxy_socket_mark_close_socket(idx_odd);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET, SOCK_DGRAM, even_port, retries);
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET, SOCK_DGRAM, odd_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                           __func__, AF_INET, SOCK_DGRAM, even_port);
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                           __func__, AF_INET, SOCK_DGRAM, odd_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_even, AF_INET, SOCK_DGRAM, even_port);
        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_odd, AF_INET, SOCK_DGRAM, odd_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_even;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_even);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        resp_req_msg->socket_list[j+1].socket_handle = (uint32_t) idx_odd;
        resp_req_msg->socket_list[j+1].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j+1].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_odd);
        resp_req_msg->socket_list[j+1].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j+1].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        j = j + 2;

        /* Update current index */
        *current_index = j;

        if (out_port_info)
        {
          out_port_info->adj_ports_info.even_port = even_port;
          out_port_info->adj_ports_info.even_handle = idx_even;
          out_port_info->adj_ports_info.odd_port = odd_port;
          out_port_info->adj_ports_info.odd_handle = idx_odd;
        }

        rc = NETMGR_SUCCESS;
      }
      else if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_even);
        if (port_params->port == 0)
        {
          /* Even port succeeded but odd port failed */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d odd port: %d try:%d",
                         __func__, AF_INET, SOCK_DGRAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d odd port: %d",
                         __func__, AF_INET, SOCK_DGRAM, odd_port);
          break;
        }
      }
      else if (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_odd);
        if (port_params->port == 0)
        {
          /* Even port failed but odd port succeeded */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d even port: %d try:%d",
                         __func__, AF_INET, SOCK_DGRAM, even_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d even port: %d",
                         __func__, AF_INET, SOCK_DGRAM, even_port);
          break;
        }
      }
      else
      {
        /* Both ports failed */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET, SOCK_DGRAM, even_port, retries);
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET, SOCK_DGRAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                         __func__, AF_INET, SOCK_DGRAM, even_port);
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                         __func__, AF_INET, SOCK_DGRAM, odd_port);
          break;
        }
      }

      retries++;
    } while ( (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                   && port_params->port == 0
                   && retries < NM_MDMPRXY_MAX_BIND_RETRIES );

    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    /* Create a UDP v4 socket on the given symmetric port */
    symm_port = port_params->port;

    /* Try to bind to v4 symmetric port */
    memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
    sockaddr_v4.sin_family = AF_INET;
    sockaddr_v4.sin_port = htons(symm_port & 0xFFFF);
    idx_udp = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                  (struct sockaddr_storage *)&sockaddr_v4,
                                                  sizeof(sockaddr_v4),
                                                  port_params->mark);

    if (idx_udp != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
    {
      /* Double-check whether the sockets were allocated on the expected port */
      if (nm_mdmprxy_socket_mark_get_socket_port(idx_udp) != symm_port)
      {
        netmgr_log_err("%s(): socket not allocated on the correct port!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx_udp);
        goto bail;
      }

      netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx_udp, AF_INET, SOCK_DGRAM, symm_port);

      /* Fill response message structure */
      resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_udp;
      resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
      resp_req_msg->socket_list[j].socket_info.port_no
        = nm_mdmprxy_socket_mark_get_socket_port(idx_udp);
      resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
      resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

      j = j + 1;

      /* Update current index */
      *current_index = j;

      if (out_port_info)
      {
        out_port_info->symm_ports_info.udp_handle_v4 = idx_udp;
        out_port_info->symm_ports_info.udp_port_v4 = symm_port;
      }

      rc = NETMGR_SUCCESS;
    }

    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      GET_RANDOM_PORT(r_port, port_params->port,
                      port_params->min_range, port_params->max_range);

      /* Try to create a TCP socket */
      memset(&sockaddr_v4, 0, sizeof(sockaddr_v4));
      sockaddr_v4.sin_family = AF_INET;
      sockaddr_v4.sin_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                (struct sockaddr_storage *)&sockaddr_v4,
                                                sizeof(sockaddr_v4),
                                                port_params->mark);

      if (idx != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check whether the socket was allocated on the expected port */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx) != r_port)
        {
          netmgr_log_err("%s(): socket allocated on the wrong port!", __func__);
          nm_mdmprxy_socket_mark_close_socket(idx);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET, SOCK_DGRAM, r_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET, SOCK_DGRAM, r_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx, AF_INET, SOCK_DGRAM, r_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV4_V01;

        j = j + 1;

        /* Update current index */
        *current_index = j;

        rc = NETMGR_SUCCESS;
      }
      else if (port_params->port == 0)
      {
        netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET, SOCK_DGRAM, r_port, retries);
      }
      else
      {
        netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET, SOCK_DGRAM, r_port);
      }

      retries++;
    } while (idx == NM_MDMPRXY_SOCKET_HANDLE_INVALID
               && port_params->port == 0
               && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v4_tcpudp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve TCP & UDP V4 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by modem
  are enforced if valid.

  The function internally calls into v4_tcp and v4_udp functions and pass
  in the resp_req_msg struct. Those individual functions would fill in
  the allocated_socket_list array in the struct and also advance the
  current index. If modem requested for ephemeral ports and bind failed
  the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v4_tcpudp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int curr_idx;
  int rc = NETMGR_SUCCESS;
  int retries = 0;

  requested_port_params_t params;

  /* Port defs */
  int even_port = NM_MDMPRXY_PORT_INVALID;
  int odd_port = NM_MDMPRXY_PORT_INVALID;
  int symm_port = NM_MDMPRXY_PORT_INVALID;
  int r_port = NM_MDMPRXY_PORT_INVALID;

  /* Handle defs */
  int idx_even_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_even_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  if (!port_params || !resp_req_msg || !current_index || !out_port_info)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  /* If modem sends us a request with ip_family set to dual-ip we will call both the V4 and V6
     handlers with the same port_params. Ex. TCPUDP V4V6 adjacent
     In this case the port_params sent from nm_mdmprxy_socket_mark_socket_bind_adjacent_ports
     will have 'af' set to AF_UNSPEC. We will copy those parameters into a local variable and
     set the af to AF_INET since this is a V4 specific handler.  */
  memset(&params, 0, sizeof(requested_port_params_t));
  memcpy(&params, port_params, sizeof(requested_port_params_t));
  params.af = AF_INET;

  if (params.xport_prot != DFS_PROTO_TCPUDP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  curr_idx = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    /* Currently there is no requirement for using the same even-odd port pair for both TCP and
       UDP so we will try to grab different pairs for both TCP and UDP to fulfill the requirement
       If the requirements change we need to update the logic */
    do
    {
      /* If we are retrying with a different port then we need to reset the current index */
      *current_index = curr_idx;

      params.xport_prot = DFS_PROTO_TCP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v4_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to TCP v4 adjacent sockets", __func__);
        break;
      }

      params.xport_prot = DFS_PROTO_UDP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v4_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to UDP v4 adjacent sockets", __func__);

        /* Release the TCP sockets */
        nm_mdmprxy_socket_mark_close_socket(out_port_info->adj_ports_info.even_handle);
        nm_mdmprxy_socket_mark_close_socket(out_port_info->adj_ports_info.odd_handle);
        break;
      }
    } while (0);

    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    /* For symmetric sockets we will need to bind to the same port for both TCP and UDP
       We will use the individual xport and ip family handler functions to achieve this and
       pass in the same port to both the functions. The individual functions will try to bind
       to the provided port and will return immediately on failure. We will retry with a
       different port here. The response message TLV will also be filled in the individual
       functions and the index will be incremented */
    do
    {
      /* If we are retrying with a different port then we need to reset the current index */
      *current_index = curr_idx;

      symm_port = NM_MDMPRXY_PORT_INVALID;
      idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      /* Get a random symmetric port, if modem passes a non-zero port we will use that */
      GET_SYMMETRIC_PORT(symm_port, port_params->port,
                         port_params->min_range, port_params->max_range);

      /* Same symmetric port needs to be used for both TCP and UDP */
      params.port = symm_port;
      params.xport_prot = DFS_PROTO_TCP_V01;

      /* Try to bind to TCP v4 socket on the symmetric port */
      rc = nm_mdmprxy_socket_mark_bind_v4_tcp(DFS_PORT_PROP_SYMMETRIC_V01,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        /* If modem set port as zero then we need to retry, otherwise give up and exit */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to TCP v4 symmetric socket with"
                         " ephemeral port: %d try: %d", __func__, symm_port, retries);
          retries++;
          continue;
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to TCP v4 symmetric socket with port: %d",
                          __func__, symm_port);
          break;
        }
      }

      idx_tcp = out_port_info->symm_ports_info.tcp_handle_v4;
      params.xport_prot = DFS_PROTO_UDP_V01;

      /* Try to bind to UDP v4 socket on the same symmetric port */
      rc = nm_mdmprxy_socket_mark_bind_v4_udp(DFS_PORT_PROP_SYMMETRIC_V01,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        nm_mdmprxy_socket_mark_close_socket(out_port_info->symm_ports_info.tcp_handle_v4);
        /* If modem set port as zero then we need to retry, otherwise give up and exit */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to UDP v4 symmetric socket with"
                         " ephemeral port: %d try: %d", __func__, symm_port, retries);
          retries++;
          continue;
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to UDP v4 symmetric socket with port: %d",
                          __func__, symm_port);
          break;
        }
      }

      idx_udp = out_port_info->symm_ports_info.udp_handle_v4;
    } while ( (idx_tcp== NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_udp == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                  && port_params->port == 0
                  && retries < NM_MDMPRXY_MAX_BIND_RETRIES );

    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      params.xport_prot = DFS_PROTO_TCP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v4_tcp(port_prop,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      idx = out_port_info->def_ports_info.handle;

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to TCP v4 sockets!", __func__);
        break;
      }

      params.xport_prot = DFS_PROTO_UDP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v4_udp(port_prop,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to UDP v4 sockets!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx);
        break;
      }
    } while (0);
    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v6_tcp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve TCP V6 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by modem
  are enforced if valid.

  The function is responsible for filling in the resp_req_msg structure
  which is passed in as OUT parameter and also advance the current index
  within the allocated socket list. If modem requested for ephemeral ports
  and bind failed the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v6_tcp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int j = 0;
  int rc = NETMGR_FAILURE;

  /* Port defs */
  int even_port = NM_MDMPRXY_PORT_INVALID;
  int odd_port = NM_MDMPRXY_PORT_INVALID;
  int symm_port = NM_MDMPRXY_PORT_INVALID;
  int r_port = NM_MDMPRXY_PORT_INVALID;

  /* Handle defs */
  int idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  int retries = 0;
  struct sockaddr_in6 sockaddr_v6;

  if (!port_params || !resp_req_msg || !current_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  if (port_params->xport_prot != DFS_PROTO_TCP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  j = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    do
    {
      idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_odd  = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      GET_EVEN_AND_ODD_PORTS(even_port, odd_port, port_params->port,
                             port_params->min_range, port_params->max_range);

      if (even_port == NM_MDMPRXY_PORT_INVALID || odd_port == NM_MDMPRXY_PORT_INVALID)
      {
        netmgr_log_err("%s(): unable to process port preference request!", __func__);
        break;
      }

      /* Try to bind to V4 even port */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(even_port & 0xFFFF);
      idx_even = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                     (struct sockaddr_storage *)&sockaddr_v6,
                                                     sizeof(sockaddr_v6),
                                                     port_params->mark);

      /* Try to bind to V4 odd port */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(odd_port & 0xFFFF);
      idx_odd = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                    (struct sockaddr_storage *)&sockaddr_v6,
                                                    sizeof(sockaddr_v6),
                                                    port_params->mark);

      if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
            && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check sockets were allocated on the expected ports */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx_even) != even_port
              || nm_mdmprxy_socket_mark_get_socket_port(idx_odd) != odd_port)
        {
          netmgr_log_err("%s(): sockets allocated on the wrong ports!", __func__);

          /* Close the incorrectly allocated sockets */
          nm_mdmprxy_socket_mark_close_socket(idx_even);
          nm_mdmprxy_socket_mark_close_socket(idx_odd);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET6, SOCK_STREAM, even_port, retries);
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET6, SOCK_STREAM, odd_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                           __func__, AF_INET6, SOCK_STREAM, even_port);
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                           __func__, AF_INET6, SOCK_STREAM, odd_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_even, AF_INET6, SOCK_STREAM, even_port);
        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_odd, AF_INET6, SOCK_STREAM, odd_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_even;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_even);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        resp_req_msg->socket_list[j+1].socket_handle = (uint32_t) idx_odd;
        resp_req_msg->socket_list[j+1].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j+1].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_odd);
        resp_req_msg->socket_list[j+1].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j+1].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        j = j + 2;

        /* Update current index */
        *current_index = j;

        if (out_port_info)
        {
          out_port_info->adj_ports_info.even_port = even_port;
          out_port_info->adj_ports_info.even_handle = idx_even;
          out_port_info->adj_ports_info.odd_port = odd_port;
          out_port_info->adj_ports_info.odd_handle = idx_odd;
        }

        rc = NETMGR_SUCCESS;

        break;
      }
      else if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_even);
        if (port_params->port == 0)
        {
          /* Even port succeeded but odd port failed */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d odd port: %d try:%d",
                         __func__, AF_INET6, SOCK_STREAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d odd port: %d",
                         __func__, AF_INET6, SOCK_STREAM, odd_port);
          break;
        }
      }
      else if (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_odd);
        if (port_params->port == 0)
        {
          /* Even port failed but odd port succeeded */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d even port: %d try:%d",
                         __func__, AF_INET6, SOCK_STREAM, even_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d even port: %d",
                         __func__, AF_INET6, SOCK_STREAM, even_port);
          break;
        }
      }
      else
      {
        /* Both ports failed */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET6, SOCK_STREAM, even_port, retries);
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET6, SOCK_STREAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                         __func__, AF_INET6, SOCK_STREAM, even_port);
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                         __func__, AF_INET6, SOCK_STREAM, odd_port);
          break;
        }
      }

      retries++;
    } while ( (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                && port_params->port == 0
                && retries < NM_MDMPRXY_MAX_BIND_RETRIES );

    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    /* Create a TCP v6 socket on the given symmetric port */
    symm_port = port_params->port;

    /* Try to bind to V4 odd port */
    memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
    sockaddr_v6.sin6_family = AF_INET6;
    sockaddr_v6.sin6_port = htons(symm_port & 0xFFFF);
    idx_tcp = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                  (struct sockaddr_storage *)&sockaddr_v6,
                                                  sizeof(sockaddr_v6),
                                                  port_params->mark);

    if (idx_tcp != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
    {
      /* Double-check whether the sockets were allocated on the expected port */
      if (nm_mdmprxy_socket_mark_get_socket_port(idx_tcp) != symm_port)
      {
        netmgr_log_err("%s(): socket not allocated on the correct port!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx_tcp);
        goto bail;
      }

      netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx_tcp, AF_INET6, SOCK_STREAM, symm_port);

      /* Fill response message structure */
      resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_tcp;
      resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
      resp_req_msg->socket_list[j].socket_info.port_no
        = nm_mdmprxy_socket_mark_get_socket_port(idx_tcp);
      resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
      resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

      j = j + 1;

      /* Update current index */
      *current_index = j;

      if (out_port_info)
      {
        out_port_info->symm_ports_info.tcp_handle_v6 = idx_tcp;
        out_port_info->symm_ports_info.tcp_port_v6 = symm_port;
      }

      rc = NETMGR_SUCCESS;
    }

    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      GET_RANDOM_PORT(r_port, port_params->port,
                      port_params->min_range, port_params->max_range);

      /* Try to create a TCP socket */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(SOCK_STREAM,
                                                (struct sockaddr_storage *)&sockaddr_v6,
                                                sizeof(sockaddr_v6),
                                                port_params->mark);

      if (idx != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check whether the socket was allocated on the expected port */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx) != r_port)
        {
          netmgr_log_err("%s(): socket allocated on the wrong port!", __func__);
          nm_mdmprxy_socket_mark_close_socket(idx);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET6, SOCK_STREAM, r_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET6, SOCK_STREAM, r_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx, AF_INET6, SOCK_STREAM, r_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_TCP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        j = j + 1;

        /* Update current index */
        *current_index = j;

        rc = NETMGR_SUCCESS;
      }
      else if (port_params->port == 0)
      {
        netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET6, SOCK_STREAM, r_port, retries);
      }
      else
      {
        netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET6, SOCK_STREAM, r_port);
      }

      retries++;
    } while (idx == NM_MDMPRXY_SOCKET_HANDLE_INVALID
             && port_params->port == 0 && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v6_udp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve UDP V6 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by modem
  are enforced if valid.

  The function is responsible for filling in the resp_req_msg structure
  which is passed in as OUT parameter and also advance the current index
  within the allocated socket list. If modem requested for ephemeral ports
  and bind failed the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v6_udp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int j = 0;
  int rc = NETMGR_FAILURE;

  /* Port defs */
  int even_port = NM_MDMPRXY_PORT_INVALID;
  int odd_port = NM_MDMPRXY_PORT_INVALID;
  int symm_port = NM_MDMPRXY_PORT_INVALID;
  int r_port = NM_MDMPRXY_PORT_INVALID;

  /* Handle defs */
  int idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  int retries = 0;
  struct sockaddr_in6 sockaddr_v6;

  if (!port_params || !resp_req_msg || !current_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  if (port_params->xport_prot != DFS_PROTO_UDP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  j = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    do
    {
      idx_even = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_odd  = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      GET_EVEN_AND_ODD_PORTS(even_port, odd_port, port_params->port,
                             port_params->min_range, port_params->max_range);

      if (even_port == NM_MDMPRXY_PORT_INVALID || odd_port == NM_MDMPRXY_PORT_INVALID)
      {
        netmgr_log_err("%s(): unable to process port preference request!", __func__);
        break;
      }

      /* Try to bind to V6 UDP even port */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(even_port & 0xFFFF);
      idx_even = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                     (struct sockaddr_storage *)&sockaddr_v6,
                                                     sizeof(sockaddr_v6),
                                                     port_params->mark);

      /* Try to bind to V6 UDP odd port */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(odd_port & 0xFFFF);
      idx_odd = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                    (struct sockaddr_storage *)&sockaddr_v6,
                                                    sizeof(sockaddr_v6),
                                                    port_params->mark);

      if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
            && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check sockets were allocated on the expected ports */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx_even) != even_port
              || nm_mdmprxy_socket_mark_get_socket_port(idx_odd) != odd_port)
        {
          netmgr_log_err("%s(): sockets allocated on the wrong ports!", __func__);

          /* Close the incorrectly allocated sockets */
          nm_mdmprxy_socket_mark_close_socket(idx_even);
          nm_mdmprxy_socket_mark_close_socket(idx_odd);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET6, SOCK_DGRAM, even_port, retries);
            netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                           " try: %d", __func__, AF_INET6, SOCK_DGRAM, odd_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                           __func__, AF_INET6, SOCK_DGRAM, even_port);
            netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                           __func__, AF_INET6, SOCK_DGRAM, odd_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_even, AF_INET6, SOCK_DGRAM, even_port);
        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                       __func__, idx_odd, AF_INET6, SOCK_DGRAM, odd_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_even;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_even);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        resp_req_msg->socket_list[j+1].socket_handle = (uint32_t) idx_odd;
        resp_req_msg->socket_list[j+1].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j+1].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx_odd);
        resp_req_msg->socket_list[j+1].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j+1].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        j = j + 2;

        /* Update current index */
        *current_index = j;

        if (out_port_info)
        {
          out_port_info->adj_ports_info.even_port = even_port;
          out_port_info->adj_ports_info.even_handle = idx_even;
          out_port_info->adj_ports_info.odd_port = odd_port;
          out_port_info->adj_ports_info.odd_handle = idx_odd;
        }

        rc = NETMGR_SUCCESS;
      }
      else if (idx_even != NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_even);
        if (port_params->port == 0)
        {
          /* Even port succeeded but odd port failed */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d odd port: %d try:%d",
                         __func__, AF_INET6, SOCK_DGRAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d odd port: %d",
                         __func__, AF_INET6, SOCK_DGRAM, odd_port);
          break;
        }
      }
      else if (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 && idx_odd != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        nm_mdmprxy_socket_mark_close_socket(idx_odd);
        if (port_params->port == 0)
        {
          /* Even port failed but odd port succeeded */
          netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d even port: %d try:%d",
                         __func__, AF_INET6, SOCK_DGRAM, even_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind af:%d type:%d even port: %d",
                         __func__, AF_INET6, SOCK_DGRAM, even_port);
          break;
        }
      }
      else
      {
        /* Both ports failed */
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET6, SOCK_DGRAM, even_port, retries);
          netmgr_log_med("%s(): Failed to bind to ephemeral port af: %d type: %d even port: %d"
                         " try: %d", __func__, AF_INET6, SOCK_DGRAM, odd_port, retries);
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d even port: %d",
                         __func__, AF_INET6, SOCK_DGRAM, even_port);
          netmgr_log_med("%s(): Failed to bind to port af: %d type: %d odd port: %d",
                         __func__, AF_INET6, SOCK_DGRAM, odd_port);
          break;
        }
      }

      retries++;
    } while ( (idx_even == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_odd == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                && port_params->port == 0
                && retries < NM_MDMPRXY_MAX_BIND_RETRIES );

    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    /* Create a UDP v6 socket on the given symmetric port */
    symm_port = port_params->port;

    /* Try to bind to v6 symmetric port */
    memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
    sockaddr_v6.sin6_family = AF_INET6;
    sockaddr_v6.sin6_port = htons(symm_port & 0xFFFF);
    idx_udp = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                  (struct sockaddr_storage *)&sockaddr_v6,
                                                  sizeof(sockaddr_v6),
                                                  port_params->mark);

    if (idx_udp != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
    {
      /* Double-check whether the sockets were allocated on the expected port */
      if (nm_mdmprxy_socket_mark_get_socket_port(idx_udp) != symm_port)
      {
        netmgr_log_err("%s(): socket not allocated on the correct port!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx_udp);
        goto bail;
      }

      netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx_udp, AF_INET6, SOCK_DGRAM, symm_port);

      /* Fill response message structure */
      resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx_udp;
      resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
      resp_req_msg->socket_list[j].socket_info.port_no
        = nm_mdmprxy_socket_mark_get_socket_port(idx_udp);
      resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
      resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

      j = j + 1;

      /* Update current index */
      *current_index = j;

      if (out_port_info)
      {
        out_port_info->symm_ports_info.udp_handle_v6 = idx_udp;
        out_port_info->symm_ports_info.udp_port_v6 = symm_port;
      }

      rc = NETMGR_SUCCESS;
    }
    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      GET_RANDOM_PORT(r_port, port_params->port,
                      port_params->min_range, port_params->max_range);

      /* Try to create a TCP socket */
      memset(&sockaddr_v6, 0, sizeof(sockaddr_v6));
      sockaddr_v6.sin6_family = AF_INET6;
      sockaddr_v6.sin6_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(SOCK_DGRAM,
                                                (struct sockaddr_storage *)&sockaddr_v6,
                                                sizeof(sockaddr_v6),
                                                port_params->mark);

      if (idx != NM_MDMPRXY_SOCKET_HANDLE_INVALID)
      {
        /* Double check whether the socket was allocated on the expected port */
        if (nm_mdmprxy_socket_mark_get_socket_port(idx) != r_port)
        {
          netmgr_log_err("%s(): socket allocated on the wrong port!", __func__);
          nm_mdmprxy_socket_mark_close_socket(idx);

          if (port_params->port == 0)
          {
            netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET6, SOCK_DGRAM, r_port, retries);
            retries++;
            continue;
          }
          else
          {
            netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET6, SOCK_DGRAM, r_port);
            break;
          }
        }

        netmgr_log_low("%s(): Successfully allocated socket %d; af:%d type:%d port%d",
                     __func__, idx, AF_INET6, SOCK_DGRAM, r_port);

        /* Fill response message structure */
        resp_req_msg->socket_list[j].socket_handle = (uint32_t) idx;
        resp_req_msg->socket_list[j].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
        resp_req_msg->socket_list[j].socket_info.port_no
          = nm_mdmprxy_socket_mark_get_socket_port(idx);
        resp_req_msg->socket_list[j].socket_info.xport_prot = DFS_PROTO_UDP_V01;
        resp_req_msg->socket_list[j].socket_info.ip_family  = DFS_IP_FAMILY_IPV6_V01;

        j = j + 1;

        /* Update current index */
        *current_index = j;

        rc = NETMGR_SUCCESS;
      }
      else if (port_params->port == 0)
      {
        netmgr_log_med("%s(): Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                       __func__, AF_INET6, SOCK_DGRAM, r_port, retries);
      }
      else
      {
        netmgr_log_med("%s(): Failed to bind af:%d type:%d port: %d",
                       __func__, AF_INET6, SOCK_DGRAM, r_port);
      }

      retries++;
    } while (idx == NM_MDMPRXY_SOCKET_HANDLE_INVALID
               && port_params->port == 0 && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_v6_tcpudp
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind to help
  reserve TCP & UDP V6 ports for the modem. Formats a sockaddr_storage
  based on the arguments then proceeds to call
  _nm_mdmprxy_socket_mark_socket_bind. Port preferences provided by modem
  are enforced if valid.

  The function internally calls into v4_tcp and v4_udp functions and pass
  in the resp_req_msg struct. Those individual functions would fill in
  the allocated_socket_list array in the struct and also advance the
  current index. If modem requested for ephemeral ports and bind failed
  the function will retry bind with a new port combination
  NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg port_prop - [IN] ADJACENT/SYMMETRIC/DEFAULT
@arg port_params - [IN] Port parameters
@arg resp_req_msg - [OUT] response message struct
@arg current_index - [OUT] Current index in response message socket list
@arg out_port_info - [OUT] Output information about allocated ports

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_v6_tcpudp
(
  dfs_port_property_enum_v01               port_prop,
  requested_port_params_t                  *port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *current_index,
  allocated_port_info_t                    *out_port_info
)
{
  int curr_idx = 0;
  int reti = NETMGR_SUCCESS;
  int rc = NETMGR_FAILURE;

  requested_port_params_t params;

  /* Port defs */
  int even_port = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int odd_port = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int symm_port = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  /* Handle defs */
  int idx_even_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_even_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_odd_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

  int retries = 0;
  struct sockaddr_in source;

  if (!port_params || !resp_req_msg || !current_index || !out_port_info)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  /* If modem sends us a request with ip_family set to dual-ip we will call both the V4 and V6
     handlers with the same port_params. Ex. TCPUDP V4V6 adjacent
     In this case the port_params sent from nm_mdmprxy_socket_mark_socket_bind_adjacent_ports
     will have 'af' set to AF_UNSPEC. We will copy those parameters into a local variable and
     set the af to AF_INET6 since this is a V6 specific handler.  */
  memset(&params, 0, sizeof(requested_port_params_t));
  memcpy(&params, port_params, sizeof(requested_port_params_t));
  params.af = AF_INET6;

  if (params.xport_prot != DFS_PROTO_TCPUDP_V01)
  {
    netmgr_log_err("%s(): incorrect handler for af [%d] xport_prot [%d]",
                   __func__, port_params->af, port_params->xport_prot);
    goto bail;
  }

  curr_idx = *current_index;

  switch (port_prop)
  {
  case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
    do
    {
      params.xport_prot = DFS_PROTO_TCP_V01;
      reti = nm_mdmprxy_socket_mark_bind_v6_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                &params,
                                                resp_req_msg,
                                                current_index,
                                                out_port_info);

      if (NETMGR_SUCCESS != reti)
      {
        netmgr_log_err("%s(): Failed to bind to TCP v6 adjacent sockets!", __func__);
        break;
      }

      params.xport_prot = DFS_PROTO_UDP_V01;
      reti = nm_mdmprxy_socket_mark_bind_v6_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                &params,
                                                resp_req_msg,
                                                current_index,
                                                out_port_info);

      if (NETMGR_SUCCESS != reti)
      {
        netmgr_log_err("%s(): Failed to bind to UDP v6 adjacent sockets!", __func__);

        /* Release the TCP sockets */
        nm_mdmprxy_socket_mark_close_socket(out_port_info->adj_ports_info.even_handle);
        nm_mdmprxy_socket_mark_close_socket(out_port_info->adj_ports_info.odd_handle);
        break;
      }
    } while (0);

    if (NETMGR_SUCCESS != reti)
    {
      goto bail;
    }

    rc = NETMGR_SUCCESS;
    break;

  case DFS_PORT_PROP_SYMMETRIC_V01:
    do
    {
      symm_port = NM_MDMPRXY_PORT_INVALID;
      idx_tcp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_udp = NM_MDMPRXY_SOCKET_HANDLE_INVALID;

      GET_SYMMETRIC_PORT(symm_port, port_params->port,
                         port_params->min_range, port_params->max_range);

      /* If we are retrying with a different port then we need to reset the current index */
      *current_index = curr_idx;

      /* We need to use the same symmetric port for both TCP and UDP */
      params.port = symm_port;
      params.xport_prot = DFS_PROTO_TCP_V01;
      reti = nm_mdmprxy_socket_mark_bind_v6_tcp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                &params,
                                                resp_req_msg,
                                                current_index,
                                                out_port_info);

      if (NETMGR_SUCCESS != reti)
      {
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to TCP v6 symmetric socket with"
                         " ephemeral port: %d try: %d", __func__, symm_port, retries);
          retries++;
          continue;
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to TCP v6 symmetric socket with port: %d",
                          __func__, symm_port);
          break;
        }
      }

      idx_tcp = out_port_info->symm_ports_info.tcp_handle_v6;

      params.xport_prot = DFS_PROTO_UDP_V01;
      reti = nm_mdmprxy_socket_mark_bind_v6_udp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                &params,
                                                resp_req_msg,
                                                current_index,
                                                out_port_info);

      if (NETMGR_SUCCESS != reti)
      {
        nm_mdmprxy_socket_mark_close_socket(out_port_info->symm_ports_info.tcp_handle_v6);
        if (port_params->port == 0)
        {
          netmgr_log_med("%s(): Failed to bind to UDP v6 symmetric socket with"
                         " ephemeral port: %d try: %d", __func__, symm_port, retries);
          retries++;
          continue;
        }
        else
        {
          netmgr_log_med("%s(): Failed to bind to UDP v6 symmetric socket with port: %d",
                          __func__, symm_port);
          break;
        }
      }

      idx_udp = out_port_info->symm_ports_info.udp_handle_v6;
    } while ( (idx_tcp == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_udp == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
                   && port_params->port == 0
                   && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    if (NETMGR_SUCCESS != reti)
    {
      goto bail;
    }

    rc = NETMGR_SUCCESS;

    break;

  case DFS_PORT_PROP_MIN_V01:
  default:
    do
    {
      params.xport_prot = DFS_PROTO_TCP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v6_tcp(port_prop,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      idx = out_port_info->def_ports_info.handle;

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to TCP v6 sockets!", __func__);
        break;
      }

      params.xport_prot = DFS_PROTO_UDP_V01;
      rc = nm_mdmprxy_socket_mark_bind_v6_udp(port_prop,
                                              &params,
                                              resp_req_msg,
                                              current_index,
                                              out_port_info);

      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed to bind to UDP v6 sockets!", __func__);
        nm_mdmprxy_socket_mark_close_socket(idx);
        break;
      }
    } while (0);
    break;
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_adjacent_ports
===========================================================================*/
/*!
@brief
  Top level function to help bind to adjacent ports for the modem. The function
  takes as input the port preference details and creates sockets as per the
  requirement.

@arg *port_params - [IN] port preference details
@arg *resp_req_msg - [OUT] Response message structure
@arg *curr_index - [OUT] current index in the response message socket array

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_adjacent_ports
(
  requested_port_params_t                  *adj_port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *curr_index
)
{
  int no_of_sockets = 0;
  int i = 0, current_idx = 0;
  int ret = NETMGR_FAILURE;
  allocated_port_info_t port_info;

  if (!adj_port_params || !resp_req_msg || !curr_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  memset(&port_info, 0, sizeof(allocated_port_info_t));

  NO_OF_SOCKETS(no_of_sockets, DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                adj_port_params->af, adj_port_params->xport_prot);

  netmgr_log_low("%s(): %d sockets to be created for port_prop [ADJACENT_START_EVEN]"
                 " af [%d] and xport_prot [%d] combination",
                 __func__, no_of_sockets, adj_port_params->af, adj_port_params->xport_prot);

  current_idx = *curr_index;

  for (i = current_idx; i < current_idx + no_of_sockets; i++)
  {
    memset(&resp_req_msg->socket_list[i],
           0,
           sizeof(dfs_allocated_socket_info_type_v01));

    if (adj_port_params->port == 0)
    {
      resp_req_msg->socket_list[i].is_ephemeral = 1;
    }
  }

  if (adj_port_params->port % 2 != 0)
  {
    netmgr_log_err("%s(): modem did not provide an even port!", __func__);
    goto bail;
  }

  if (DFS_PROTO_TCPUDP_V01 == adj_port_params->xport_prot)
  {
    switch (adj_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                  adj_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_tcpudp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                  adj_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                  adj_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_tcpudp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                   adj_port_params,
                                                   resp_req_msg,
                                                   curr_index,
                                                   &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, adj_port_params->af);
      break;
    }
  }
  else if (DFS_PROTO_TCP_V01 == adj_port_params->xport_prot)
  {
    switch (adj_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_tcp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                adj_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, adj_port_params->af);
      break;
    }
  }
  else if (DFS_PROTO_UDP_V01 == adj_port_params->xport_prot)
  {
    switch (adj_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                               adj_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_udp(DFS_PORT_PROP_ADJACENT_START_EVEN_V01,
                                                adj_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, adj_port_params->af);
      break;
    }
  }
  else
  {
    netmgr_log_err("%s(): unknown xport_prot [%d]!", __func__, adj_port_params->xport_prot);
    goto bail;
  }

  if (*curr_index != (current_idx + no_of_sockets))
  {
    netmgr_log_err("%s(): could not allocate the required number of sockets!!", __func__);
    ret = NETMGR_FAILURE;
  }

bail:

  if (NETMGR_FAILURE == ret)
  {
    netmgr_log_err("%s(): failed to create sockets with requested preferences!", __func__);

    for (i = current_idx;
           i < (current_idx + no_of_sockets) && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01;
           i++)
    {
      resp_req_msg->socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
    }

    if(curr_index)
    {
      *curr_index = current_idx + no_of_sockets;
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_symmetric_ports
===========================================================================*/
/*!
@brief
  Top level function to help bind to symmetric ports for the modem. The function
  takes as input the port preference details and creates sockets as per the
  requirement.

@arg *port_params - port preference details
@arg *resp_req_msg - Response message structure
@arg *curr_index - current index in the response message socket array

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_symmetric_ports
(
  requested_port_params_t                  *symm_port_params,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *curr_index
)
{
  int no_of_sockets = 0;
  int i = 0, current_idx = 0;
  int ret = NETMGR_FAILURE;
  int retries = 0;
  int symm_port = 0;
  int idx_tcp_v4 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp_v4 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_tcp_v6 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  int idx_udp_v6 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
  requested_port_params_t params;
  allocated_port_info_t port_info;

  if (!symm_port_params || !resp_req_msg || !curr_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  NO_OF_SOCKETS(no_of_sockets, DFS_PORT_PROP_SYMMETRIC_V01,
                symm_port_params->af, symm_port_params->xport_prot);

  netmgr_log_low("%s(): %d sockets to be created for port_prop [SYMMETRIC]"
                 " af [%d] and xport_prot [%d] combination",
                 __func__, no_of_sockets, symm_port_params->af, symm_port_params->xport_prot);

  current_idx = *curr_index;

  for (i = current_idx; i < current_idx + no_of_sockets; i++)
  {
    memset(&resp_req_msg->socket_list[i],
           0,
           sizeof(dfs_allocated_socket_info_type_v01));

    if (symm_port_params->port == 0)
    {
      resp_req_msg->socket_list[i].is_ephemeral = 1;
    }
  }

  if (symm_port_params->xport_prot != DFS_PROTO_TCPUDP_V01)
  {
    netmgr_log_err("%s(): xport_prot [%d] is not TCPUDP, cannot complete symmetric socket requests",
                   __func__, symm_port_params->xport_prot);
    goto bail;
  }

  memset(&port_info, 0, sizeof(allocated_port_info_t));

  /* Copy the port preference information into local variable */
  memcpy(&params, symm_port_params, sizeof(requested_port_params_t));

  switch (symm_port_params->af)
  {
  case AF_INET:
    ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                symm_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
    break;

  case AF_INET6:
    ret = nm_mdmprxy_socket_mark_bind_v6_tcpudp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                symm_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
    break;

  case AF_UNSPEC:
    /* 1. Get a random symmetric port in the given range if port is zero otherwise use the port
          value passed by modem
       2. Use the symm port to create TCP & UDP v4 sockets
       3. If v4 is unsuccessful and the port passed by modem was zero then retry with a diff port
          otherwise give up and exit
       4. If v4 was successful, then use the same symmetric port for creating v6 TCP and UDP
          sockets
       5. If v6 is unsuccessful and port passed by modem was zero retry from beginning otherwise
          release the v4 sockets, give up and exit */
    do
    {
      idx_tcp_v4 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_udp_v4 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_tcp_v6 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      idx_udp_v6 = NM_MDMPRXY_SOCKET_HANDLE_INVALID;
      symm_port = NM_MDMPRXY_PORT_INVALID;

      /* Reset index back to where we started */
      *curr_index = current_idx;

      /* Get a random port in the given range or use the port passed by modem */
      GET_SYMMETRIC_PORT(symm_port, symm_port_params->port,
                         symm_port_params->min_range, symm_port_params->max_range);


      /* Try to create symmetric v4 sockets with symm_port
         When we pass a non-zero port in the param struct the handler function will try
         to bind only once */
      params.port = symm_port;
      ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                  &params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);

      idx_tcp_v4 = port_info.symm_ports_info.tcp_handle_v4;
      idx_udp_v4 = port_info.symm_ports_info.udp_handle_v4;

      if (NETMGR_SUCCESS != ret)
      {
        /* If modem originally passed zero as the port, retry */
        if (symm_port_params->port == 0)
        {
          netmgr_log_err("%s(): failed to create TCP & UDP v4 sockets for ephemeral port: %d"
                         " try: %d", __func__, symm_port, retries);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v4);
          retries++;
          continue;
        }
        else
        {
          /* We failed to bind to create TCP & UDP v4 sockets using the port modem gave
             give up and exit */
          netmgr_log_err("%s(): failed to create TCP & UDP v4 sockets for ephemeral port: %d",
                         __func__, symm_port);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v4);
          break;
        }
      }

      /* We would have created V4 TCP and UDP sockets, use the same port to create the corresponding
         V6 sockets */
      ret = nm_mdmprxy_socket_mark_bind_v6_tcpudp(DFS_PORT_PROP_SYMMETRIC_V01,
                                                  &params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);

      idx_tcp_v6 = port_info.symm_ports_info.tcp_handle_v6;
      idx_udp_v6 = port_info.symm_ports_info.udp_handle_v6;

      if (NETMGR_SUCCESS != ret)
      {
        /* If modem originally passed zero as the port, retry */
        if (symm_port_params->port == 0)
        {
          netmgr_log_err("%s(): failed to create TCP & UDP v6 sockets for ephemeral port: %d"
                         " try: %d", __func__, symm_port, retries);
          retries++;
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v6);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v6);
          continue;
        }
        else
        {
          /* We failed to bind to create TCP & UDP v6 sockets using the port modem gave
             give up and exit */
          netmgr_log_err("%s(): failed to create TCP & UDP v4 sockets for ephemeral port: %d",
                         __func__, symm_port);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v4);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.tcp_handle_v6);
          nm_mdmprxy_socket_mark_close_socket(port_info.symm_ports_info.udp_handle_v6);
          break;
        }
      }
    } while ( (idx_tcp_v4 == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_udp_v4 == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_tcp_v6 == NM_MDMPRXY_SOCKET_HANDLE_INVALID
                 || idx_udp_v6 == NM_MDMPRXY_SOCKET_HANDLE_INVALID)
               && symm_port_params->port == 0
               && retries < NM_MDMPRXY_MAX_BIND_RETRIES);

    break;

  default:
    netmgr_log_err("%s(): unknown address family [%d]", __func__, symm_port_params->af);
    break;
  }

  if (*curr_index != (current_idx + no_of_sockets))
  {
    netmgr_log_err("%s(): could not allocate the required number of sockets!!", __func__);
    ret = NETMGR_FAILURE;
  }

bail:

  if (NETMGR_FAILURE == ret)
  {
    netmgr_log_err("%s(): failed to create sockets with requested preferences!", __func__);

    for (i = current_idx;
           i < (current_idx + no_of_sockets) && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01;
           i++)
    {
      resp_req_msg->socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
    }

    if(curr_index)
    {
      *curr_index = current_idx + no_of_sockets;
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_bind_ports
===========================================================================*/
/*!
@brief
  Top level function to help bind to ports for the modem. The function
  takes as input the port preference details and creates sockets as per the
  requirement.

@arg *port_params - port preference details
@arg *resp_req_msg - Response message structure
@arg *curr_index - current index in the response message socket array

@return
  NETMGR_FAILURE on failure
  NETMGR_SUCCESS on success

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_bind_ports
(
  requested_port_params_t                  *def_port_params,
  dfs_port_property_enum_v01               port_prop,
  dfs_remote_socket_allocated_req_msg_v01  *resp_req_msg,
  int                                      *curr_index
)
{
  int no_of_sockets = 0;
  int i = 0, current_idx = 0;
  int ret = NETMGR_FAILURE;
  allocated_port_info_t port_info;

  if (!def_port_params || !resp_req_msg || !curr_index)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  NO_OF_SOCKETS(no_of_sockets, port_prop, def_port_params->af, def_port_params->xport_prot);

  netmgr_log_low("%s(): %d sockets to be created for port_prop [DEFAULT]"
                 " af [%d] and xport_prot [%d] combination",
                 __func__, no_of_sockets, def_port_params->af, def_port_params->xport_prot);

  current_idx = *curr_index;

  for (i = current_idx; i < current_idx + no_of_sockets; i++)
  {
    memset(&resp_req_msg->socket_list[i],
           0,
           sizeof(dfs_allocated_socket_info_type_v01));

    if (def_port_params->port == 0)
    {
      resp_req_msg->socket_list[i].is_ephemeral = 1;
    }
  }

  memset(&port_info, 0, sizeof(allocated_port_info_t));

  if (def_port_params->xport_prot == DFS_PROTO_TCPUDP_V01)
  {
    switch (def_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(port_prop,
                                                  def_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_tcpudp(port_prop,
                                                  def_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcpudp(port_prop,
                                                  def_port_params,
                                                  resp_req_msg,
                                                  curr_index,
                                                  &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_tcpudp(port_prop,
                                                   def_port_params,
                                                   resp_req_msg,
                                                   curr_index,
                                                   &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, def_port_params->af);
      break;
    }
  }
  else if (def_port_params->xport_prot == DFS_PROTO_TCP_V01)
  {
    switch (def_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_tcp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_tcp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_tcp(port_prop,
                                                def_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, def_port_params->af);
      break;
    }
  }
  else if (def_port_params->xport_prot == DFS_PROTO_UDP_V01)
  {
    switch (def_port_params->af)
    {
    case AF_INET:
      ret = nm_mdmprxy_socket_mark_bind_v4_udp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_INET6:
      ret = nm_mdmprxy_socket_mark_bind_v6_udp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);
      break;

    case AF_UNSPEC:
      ret = nm_mdmprxy_socket_mark_bind_v4_udp(port_prop,
                                               def_port_params,
                                               resp_req_msg,
                                               curr_index,
                                               &port_info);

      ret |= nm_mdmprxy_socket_mark_bind_v6_udp(port_prop,
                                                def_port_params,
                                                resp_req_msg,
                                                curr_index,
                                                &port_info);
      break;

    default:
      netmgr_log_err("%s(): unknown address family [%d]", __func__, def_port_params->af);
      break;
    }
  }
  else
  {
    netmgr_log_err("%s(): unknown xport_prot [%d]", __func__, def_port_params->xport_prot);
    goto bail;
  }

  if (*curr_index != (current_idx + no_of_sockets))
  {
    netmgr_log_err("%s(): could not allocate the required number of sockets!!", __func__);
    ret = NETMGR_FAILURE;
  }

bail:

  if (NETMGR_FAILURE == ret)
  {
    netmgr_log_err("%s(): failed to create sockets with requested preferences!", __func__);

    for (i = current_idx;
           i < (current_idx + no_of_sockets) && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01;
           i++)
    {
      resp_req_msg->socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
    }

    if(curr_index)
    {
      *curr_index = current_idx + no_of_sockets;
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_set_udpencap
===========================================================================*/
/*!
@brief
  Set/unset a socket in UDP encapsulation mode. Uses encapsulation mode
  UDP_ENCAP_ESPINUDP. When encapsulation mode is enabled, IP_TRANSPARENT
  mode is disabled. Likewise when encapsulation mode is disabled,
  IP_TRANSPARENT mode is re-applied.

@arg handle - pr_fds[] handle
@arg should_set - TRUE or 1 means set encap mode; FALSE of 0 disabled it


@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_set_udpencap
(
  unsigned int handle,
  int should_set
)
{
  int rc, val;
  int type = UDP_ENCAP_ESPINUDP;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Error: handle out of range (%u)", __func__, handle);
    return NETMGR_FAILURE;
  }

  if (pr_fds[handle] == 0)
  {
    netmgr_log_err("%s(): Tried to change modes on an unused handle (%u)",
                   __func__, handle);
    return NETMGR_FAILURE;
  }

  /* unset transparent */
  if (should_set)
    val = 0;
  else
    val = 1;

  rc = setsockopt(pr_fds[handle], SOL_IP, IP_TRANSPARENT, &val, sizeof(val));
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to set IP_TRANSPARENT: %d-%s\n",
                   __func__, errno, strerror(errno));
    return NETMGR_FAILURE;
  }

  /* Set UDP Encapsulation
   * As of Kernel 3.14, there is no way to un-set UDP_ENCAP once it has
   * been set. As such, if UDP_ENCAP unset is requested, we just ignore
   * the next step. In theory, the TRANSPARENT flag above should take
   * effect earlier in the stack than UDP_ENCAP. As such the packets should
   * still be proxied. This behavior is untested as it is not an anticipated
   * use case */
  if (should_set)
  {
    rc = setsockopt(pr_fds[handle], SOL_UDP, UDP_ENCAP, &type, sizeof(type));
    if (rc < 0)
    {
      netmgr_log_err("%s(): Unable to set UDP_ENCAP: %d-%s\n",
                     __func__, errno, strerror(errno));
      return NETMGR_FAILURE;
    }
  }

  NM_MDMPRXY_SOCKET_LOG(low, "Set{%d} UDP_ENCAP on handle:%d port:%d",
                        should_set, handle,
                        nm_mdmprxy_socket_mark_get_socket_port(handle));

  return NETMGR_SUCCESS;
}

/*===========================================================================
                     QMI DFS Client
===========================================================================*/

void nm_mdmprxy_socket_mark_qmi_do_request_resp
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                           *resp_cb_data,
  qmi_client_error_type          transp_err
)
{
  NM_MDMPRXY_VAR_UNUSED(user_handle);
  NM_MDMPRXY_VAR_UNUSED(msg_id);
  NM_MDMPRXY_VAR_UNUSED(resp_c_struct);
  NM_MDMPRXY_VAR_UNUSED(resp_c_struct_len);
  NM_MDMPRXY_VAR_UNUSED(resp_cb_data);
  NM_MDMPRXY_VAR_UNUSED(transp_err);

  netmgr_log_low("%s(): Pointless function that does nothing", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_request_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01 indication.
  Converts modem request into a socket bind request. Then sends a
  QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01 to the modem with the appropriate
  handles.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_request_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type qmi_err;
  dfs_remote_socket_request_ind_msg_v01 req_msg;
  dfs_remote_socket_allocated_req_msg_v01  resp_req_msg;
  dfs_remote_socket_allocated_resp_msg_v01 resp_resp_msg;
  dfs_socket_port_pref_type_v01 port_pref;
  qmi_txn_handle txn_handle;
  int af = 0, port, rc, ret, no_of_socket = 0, current_index = 0;
  int port_range_valid = TRUE;
  uint16_t base_port_value = 0;
  uint16_t max_port_value = 0;
  uint16_t min_range = 0;
  uint16_t max_range = 0;
  uint32_t i = 0;
  requested_port_params_t adj_port_params;
  requested_port_params_t symm_port_params;
  requested_port_params_t def_port_params;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));

  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d", __func__, qmi_err);
    return;
  }

  memset(&port_pref, 0, sizeof(dfs_socket_port_pref_type_v01));
  memset(&resp_req_msg, 0, sizeof(dfs_remote_socket_allocated_req_msg_v01));
  memset(&resp_resp_msg, 0, sizeof(dfs_remote_socket_allocated_resp_msg_v01));

  resp_req_msg.socket_list_len = req_msg.request_socket_list_len;

  /* Check if optional TLVs are set in the request message. The optional TLVs are used to impose
     additional restrictions on port allocation */
  if (TRUE == req_msg.request_identifier_valid)
  {
    resp_req_msg.request_identifier_valid = TRUE;
    resp_req_msg.request_identifier = req_msg.request_identifier;
  }

  if (TRUE == req_msg.port_pref_valid)
  {
    /* Save the base_port and max_port values */
    port_pref = req_msg.port_pref;
    base_port_value = port_pref.base_port_value;
    max_port_value = port_pref.max_port_value;

    /* Use the base_port and max_port values passed by modem to determine the port
       range to be used */
    ADJUST_PORT_RANGE(base_port_value, max_port_value, min_range, max_range, port_range_valid);
    if (FALSE == port_range_valid)
    {
      netmgr_log_err("%s(): modem did not pass valid port range values!", __func__);
      return;
    }
  }
  else
  {
    min_range = NM_MDMPRXY_MODEM_PORT_RANGE_START;
    max_range = NM_MDMPRXY_MODEM_PORT_RANGE_END;
  }

  for (i = 0; i < req_msg.request_socket_list_len && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01; i++)
  {
    port = req_msg.request_socket_list[i].port_no;

    switch(req_msg.request_socket_list[i].ip_family)
    {
      case DFS_IP_FAMILY_IPV4_V01:
        af = AF_INET;
        break;

      case DFS_IP_FAMILY_IPV6_V01:
        af = AF_INET6;
        break;

      case DFS_IP_FAMILY_IPV4V6_V01:
        /* Modem may have set additional port options */
        af = AF_UNSPEC;
        break;

      default:
        netmgr_log_err("%s(): Modem requested a socket of unknown IP family: %d,"
                       " skipping this entry",
                       __func__, req_msg.request_socket_list[i].ip_family);
        continue;
    }

    switch (port_pref.port_property)
    {
    case DFS_PORT_PROP_ADJACENT_START_EVEN_V01:
      memset(&adj_port_params, 0, sizeof(adj_port_params));
      adj_port_params.port = port;
      adj_port_params.af = af;
      adj_port_params.xport_prot = req_msg.request_socket_list[i].xport_prot;
      adj_port_params.mark = NM_MDMPRXY_MARK;
      adj_port_params.min_range = min_range;
      adj_port_params.max_range = max_range;

      ret = nm_mdmprxy_socket_mark_bind_adjacent_ports(&adj_port_params,
                                                       &resp_req_msg,
                                                       &current_index);

      if (NETMGR_SUCCESS != ret)
      {
        netmgr_log_err("%s(): failed to create adjacent sockets, req_socket_list index [%d]",
                       __func__, i);
      }
      break;

    case DFS_PORT_PROP_SYMMETRIC_V01:
      memset(&symm_port_params, 0, sizeof(symm_port_params));
      symm_port_params.port = port;
      symm_port_params.af = af;
      symm_port_params.xport_prot = req_msg.request_socket_list[i].xport_prot;
      symm_port_params.mark = NM_MDMPRXY_MARK;
      symm_port_params.min_range = min_range;
      symm_port_params.max_range = max_range;

      ret = nm_mdmprxy_socket_mark_bind_symmetric_ports(&symm_port_params,
                                                        &resp_req_msg,
                                                        &current_index);

      if (NETMGR_SUCCESS != ret)
      {
        netmgr_log_err("%s(): failed to create symmetric sockets, req_socket_list index [%d]",
                       __func__, i);
      }
      break;

    case DFS_PORT_PROP_MIN_V01:
    default:
      memset(&def_port_params, 0, sizeof(def_port_params));
      def_port_params.port = port;
      def_port_params.af = af;
      def_port_params.xport_prot = req_msg.request_socket_list[i].xport_prot;
      def_port_params.mark = NM_MDMPRXY_MARK;
      def_port_params.min_range = min_range;
      def_port_params.max_range = max_range;

      ret = nm_mdmprxy_socket_mark_bind_ports(&def_port_params,
                                              port_pref.port_property,
                                              &resp_req_msg,
                                              &current_index);

      if (NETMGR_SUCCESS != ret)
      {
        netmgr_log_err("%s(): failed to create sockets, req_socket_list index [%d]",
                       __func__, i);
      }
      break;
    }
  }

  resp_req_msg.socket_list_len = current_index;

  rc = qmi_client_send_msg_async(pr_qmi_dfs_clnt_hndl,
                                 QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01,
                                 &resp_req_msg,
                                 sizeof(dfs_remote_socket_allocated_req_msg_v01),
                                 &resp_resp_msg,
                                 sizeof(dfs_remote_socket_allocated_resp_msg_v01),
                                 nm_mdmprxy_socket_mark_qmi_do_request_resp,
                                 0,
                                 &txn_handle);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01 message: %d",
                   __func__,
                   rc);
  }
  else
  {
    netmgr_log_low("%s(): Sent port request handles back to modem", __func__);
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01 indication.
  Closes the socket specified by the modem.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_release_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type qmi_err;
  dfs_remote_socket_release_ind_msg_v01 req_msg;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));

  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d", __func__, qmi_err);
    return;
  }

  if (!req_msg.socket_handles_valid)
  {
    nm_mdmprxy_socket_mark_close_all_ports();
    netmgr_log_low("%s(): Closed all socket handles", __func__);
  }
  else
  {
    uint32_t i;
    for (i = 0; i < req_msg.socket_handles_len && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01; i++)
    {
      if(req_msg.socket_handles[i] >= MAX_OPEN_SOCKETS)
      {
        netmgr_log_err("%s(): Modem requested invalid socket handle (%u) be closed; skipping",
                       __func__,
                       req_msg.socket_handles[i]);
        continue;
      }
      nm_mdmprxy_socket_mark_close_socket(req_msg.socket_handles[i]);
      netmgr_log_low("%s(): Modem requested socket handle (%u) be closed",
                     __func__,
                     req_msg.socket_handles[i]);
    }
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01 indication.
  Sets any options specified by the modem on a socket. As of now, only
  UDP_ENCAP is handled.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_set_option_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  dfs_remote_socket_set_option_ind_msg_v01 req_msg;
  qmi_client_error_type qmi_err;
  int handle, rc, type;
  int udp_encap_hndl_idx;
  unsigned int port;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));
  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d",
                   __func__, qmi_err);
    return;
  }

  handle = req_msg.socket_handle;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Bad socket handle (%u) from modem",
                   __func__, handle);
    return;
  }

  if (pr_fds[handle] == 0)
  {
    netmgr_log_err("%s(): No matching socket found for handle (%u)",
                   __func__, handle);
    return;
  }

  type = nm_mdmprxy_socket_mark_get_socket_type(handle);
  port = nm_mdmprxy_socket_mark_get_socket_port(handle);

  if (req_msg.is_udp_encaps_valid)
  {
    if(req_msg.is_udp_encaps == 1)
    {
      /* Check if we have already received a request for the same handle */
      if (NETMGR_SUCCESS != nm_mdmprxy_socket_mark_check_if_udp_encap_port_exists(handle))
      {
        return;
      }

      /* Find an empty slot for saving UDP_ENCAP handle information */
      udp_encap_hndl_idx = nm_mdmprxy_socket_mark_find_empty_udp_encap_handle_index();
      if (MAX_UDP_ENCAP_SOCKETS <= udp_encap_hndl_idx)
      {
        netmgr_log_err("%s(): could not find an empty slot for storing UDP_ENCAP handle"
                       " information", __func__);
        return;
      }

      rc = nm_mdmprxy_socket_mark_set_udpencap(handle, 1);
      if (rc != NETMGR_SUCCESS)
      {
        netmgr_log_err("%s(): Failed to set UDP Encap mode", __func__);
        return;
      }

      rc = nm_mdmprxy_install_udpencap_rules(port,
                                             NM_MDMPRXY_IKE_REMOTE_PORT,
                                             NM_MDMPRXY_MARK);
      if (rc != NETMGR_SUCCESS)
      {
        netmgr_log_err("%s(): Failed to install UDP Encap rules", __func__);
        return;
      }

      netmgr_main_cfg_set_ike_port(port);

      /* Store the UDP encap handle reference */
      udp_encap_hndls[udp_encap_hndl_idx] = handle;
    }
    else
    {
      rc = nm_mdmprxy_socket_mark_set_udpencap(handle, 0);
      if (rc != NETMGR_SUCCESS)
      {
        netmgr_log_err("%s(): Failed to unset UDP Encap mode", __func__);
      }

      rc = nm_mdmprxy_socket_mark_if_udp_encap_cleanup_rules(handle, port);
      if (rc != NETMGR_SUCCESS)
      {
        netmgr_log_err("%s(): Failed to uninstall UDP Encap rules", __func__);
      }
    }
  }
  else
  {
    netmgr_log_med("%s(): No action specified", __func__);
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Primary callback registered with QCCI framework. Based on the msg_id,
  the appropriate indication handler is invoked above.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_callback
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type   qmi_err;
  dfs_low_latency_traffic_ind_msg_v01 filterModeStatus;

  (void) ind_cb_data;

  if (ind_buf == NULL)
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return;
  }

  nm_mdmprxy_socket_mark_lock();
  switch(msg_id)
  {
    case QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_request_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    case QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_release_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    case QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_set_option_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    default:
      netmgr_log_err("%s(): Unhandled msg_id: %u", __func__, msg_id);
  }
  nm_mdmprxy_socket_mark_unlock();
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_release_qmi_clients
===========================================================================*/
/*!
@brief
  Cleanup the QMI DFS clients.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_release_qmi_clients()
{
  if (pr_qmi_state == PR_QMI_STATE_UNINITIALIZED)
  {
    netmgr_log_low("%s(): QMI Client is already uninitialized; skipping", __func__);
    return;
  }

  qmi_client_release(pr_qmi_dfs_clnt_hndl);
  pr_qmi_state = PR_QMI_STATE_UNINITIALIZED;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_init_qmi_clients
===========================================================================*/
/*!
@brief
  Initialize the QMI DFS client and register for all of the port reservation
  indications.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_init_qmi_clients()
{
  int rc;
  dfs_indication_register_req_msg_v01 request;
  dfs_indication_register_resp_msg_v01 response;
  dfs_remote_socket_handling_ver_enum_v01 dfs_version;

  /* Initialize the QCCI client */

  if (pr_qmi_state == PR_QMI_STATE_INITIALIZED)
  {
    netmgr_log_med("%s(): QMI Client is already initialized; skipping", __func__);
    return;
  }

  dfs_version = nm_mdmprxy_get_dfs_service_version();

  rc = qmi_client_init_instance(dfs_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                nm_mdmprxy_socket_mark_qmi_callback,
                                NULL,
                                &pr_qcci_os_params,
                                NM_MDMPRXY_QMI_TIMEOUT,
                                &pr_qmi_dfs_clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed on qmi_client_init_instance with rc=%d!\n", __func__, rc);
    return;
  }

  /* Register for socket indications */
  memset(&request, 0, sizeof(request));
  memset(&response, 0, sizeof(response));
  request.remote_socket_handling = TRUE;
  request.remote_socket_handling_valid = TRUE;

  /* Set the version */
  request.remote_socket_handling_version_valid = TRUE;
  request.remote_socket_handling_version = dfs_version;

  rc = qmi_client_send_msg_sync(pr_qmi_dfs_clnt_hndl,
                                QMI_DFS_INDICATION_REGISTER_REQ_V01,
                                &request,
                                sizeof(dfs_indication_register_req_msg_v01),
                                &response,
                                sizeof(dfs_indication_register_resp_msg_v01),
                                NM_MDMPRXY_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI_DFS_INDICATION_REGISTER_REQ_V01 message: %d",
                   __func__,
                   rc);
    goto bail;
  }

  pr_qmi_state = PR_QMI_STATE_INITIALIZED;
  return;

bail:
  nm_mdmprxy_socket_mark_release_qmi_clients();
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_uninstall_static_rules
===========================================================================*/
/*!
@brief
  Remove iptables marking rules

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_uninstall_static_rules
(
  sa_family_t af,
  int start
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  for (i = start; i >= 0; i--)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully removed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, FALSE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_install_static_rules
===========================================================================*/
/*!
@brief
  Install iptables marking rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_install_static_rules
(
  sa_family_t af
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;


  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(static_rule_list);

  for (i = 0; i < num_rules; i++)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_RULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      nm_mdmprxy_socket_mark_uninstall_static_rules(af, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully installed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, TRUE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_uninstall_apps_rules
===========================================================================*/
/*!
@brief
  Remove application specific marking rules

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_uninstall_apps_rules
(
  sa_family_t af,
  int start
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (DFS_REMOTE_SOCKET_HANDLING_V1_V01 == nm_mdmprxy_get_dfs_service_version())
  {
    netmgr_log_low("%s(): application specific port-based rules are not installed"
                   " in this configuration", __func__);
    return NETMGR_SUCCESS;
  }

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  for (i = start; i >= 0; i--)
  {
    if (!apps_ports_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&apps_ports_rule_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully removed application specific marking rules for ip_family [%d]",
                 __func__, af);
  NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(af, FALSE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_install_apps_rules
===========================================================================*/
/*!
@brief
  Install application specific iptables packet marking rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_install_apps_rules
(
  sa_family_t af
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (DFS_REMOTE_SOCKET_HANDLING_V1_V01 == nm_mdmprxy_get_dfs_service_version())
  {
    netmgr_log_low("%s(): application specific port-based rules are not installed"
                   " in this configuration", __func__);
    return NETMGR_SUCCESS;
  }

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(apps_ports_rule_list);

  for (i = 0; i < num_rules; i++)
  {
    if (!apps_ports_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&apps_ports_rule_list[i], NM_MDMPRXY_RUN_RULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      nm_mdmprxy_socket_mark_uninstall_apps_rules(af, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully installed apps specific marking rules for ip_family [%d]",
                 __func__, af);
  NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(af, TRUE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_iptables_init
===========================================================================*/
/*!
@brief
  Reset the iiface handles and install the static fules for AF_INET
  and AF_INET6
*/
/*=========================================================================*/
void
nm_mdmprxy_socket_mark_iptables_init()
{
  nm_mdmprxy_socket_mark_install_static_rules(AF_INET);
  nm_mdmprxy_socket_mark_install_static_rules(AF_INET6);

  /* Install application specific rules */
  nm_mdmprxy_socket_mark_install_apps_rules(AF_INET);
  nm_mdmprxy_socket_mark_install_apps_rules(AF_INET6);

  netmgr_log_low("%s(): Initialization finished", __func__);
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_iptables_uninit
===========================================================================*/
/*!
@brief
  Clean up all iptables rules
*/
/*=========================================================================*/
void
nm_mdmprxy_socket_mark_iptables_uninit()
{
  nm_mdmprxy_socket_mark_uninstall_static_rules(AF_INET,
                                                nm_mdmprxy_rules_len(static_rule_list) - 1);

  nm_mdmprxy_socket_mark_uninstall_static_rules(AF_INET6,
                                                nm_mdmprxy_rules_len(static_rule_list) - 1);

  /* Remove application specific rules */
  nm_mdmprxy_socket_mark_uninstall_apps_rules(AF_INET,
                                              nm_mdmprxy_rules_len(apps_ports_rule_list) - 1);
  nm_mdmprxy_socket_mark_uninstall_apps_rules(AF_INET6,
                                              nm_mdmprxy_rules_len(apps_ports_rule_list) - 1);

  /* Remove common rules */
  nm_mdmprxy_common_mark_prov_oos_cleanup();

  netmgr_log_low("%s(): Un-initialization finished", __func__);
}

static int
nm_mdmprxy_socket_mark_prov_init(void)
{
  socket_mark_prov_cmd_tbl.udp_encap_rule_list       = udp_encap_rule_list;
  socket_mark_prov_cmd_tbl.udp_encap_rule_list_len   = (int) nm_mdmprxy_rules_len(udp_encap_rule_list);
  nm_mdmprxy_common_mark_prov_setup_iptables(&socket_mark_prov_cmd_tbl);
  (void) nm_mdmprxy_register_mark_prov_cmd_tbl(&socket_mark_prov_cmd_tbl);

  /* Begin protected section */
  nm_mdmprxy_socket_mark_lock();
  memset(udp_encap_hndls, -1, sizeof(udp_encap_hndls));

  nm_mdmprxy_socket_mark_set_reserved_ports();
  nm_mdmprxy_socket_mark_init_qmi_clients();
  nm_mdmprxy_socket_mark_iptables_init();
  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider init complete", __func__);

  return 0;
}

static int
nm_mdmprxy_socket_mark_prov_is_hdlr(void)
{
  nm_mdmprxy_socket_mark_lock();
  nm_mdmprxy_socket_mark_init_qmi_clients();

  /* Install IMS audio port-forwarding rules, set the preference to TRUE indicating that we
     want to install the rules since this is an SSR in-service scenario */
  nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET, TRUE);
  nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET6, TRUE);

  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider IS handler complete", __func__);
  return 0;
}

static int
nm_mdmprxy_socket_mark_prov_oos_hdlr(void)
{
  /* Begin protected section */
  nm_mdmprxy_socket_mark_lock();
  nm_mdmprxy_socket_mark_release_qmi_clients();
  nm_mdmprxy_socket_mark_close_all_ports();
  /* Remove common rules */
  nm_mdmprxy_common_mark_prov_oos_cleanup();
  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider OOS handler complete", __func__);

  return 0;
}
