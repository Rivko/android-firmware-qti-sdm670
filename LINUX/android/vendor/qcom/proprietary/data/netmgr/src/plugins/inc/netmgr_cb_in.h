/******************************************************************************

                        N E T M G R _ C B _ I N . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_cb_in.h
  @brief   Network Manager CB framework common networking definitions

  DESCRIPTION
  Basic descriptions of network addresses without the cruft accumulated in
  netmgr internal definitions

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_CB_IN_H__
#define __NETMGR_CB_IN_H__

#include <inttypes.h>

#ifdef NETMGR_OFFTARGET
  #include <netinet/in.h>
#else
  #ifndef  FEATURE_DATA_LINUX_LE
    #include <netinet/in6.h>
  #endif
#endif

#define NETMGR_KIF_CB_ADDR_INVAL   0
#define NETMGR_KIF_CB_ADDR_IPV4    4
#define NETMGR_KIF_CB_ADDR_IPV6    6
#define NETMGR_KIF_CB_ADDR_IPV4V6  8

struct netmgr_kif_v4addr {
  uint32_t addr;             /* Full 32-bit address        */
  uint32_t mask;             /* Full 32-bit CIDR mask      */
};

struct netmgr_kif_v6addr {
  struct in6_addr addr;      /* Full 128-bit address       */
  uint32_t mask;             /* Prefix length (e.g.. /48)  */
};

struct netmgr_kif_addrstorage {
  uint8_t type;
  union {
    struct netmgr_kif_v4addr _in4;
    struct netmgr_kif_v6addr _in6;
  };
};

#endif /* __NETMGR_CB_IN_H__ */
