/******************************************************************************

                        N E T M G R _ N E T D _C L I E N T . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_netd_client.h
  @brief   Network manager netd client

  DESCRIPTION
  Header file for netd client module

******************************************************************************/
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#if (!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))
int NetmgrNetdClientInit(void);
#else
static int NetmgrNetdClientInit(void)
{
  return 1;
}
#endif /* !defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID) */
#ifdef __cplusplus
}

extern "C" {
#endif /* __cplusplus */

struct networkSettings
{
  uint64_t networkHandle;
  uint32_t packetMark;
};

#if (!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))
int NetmgrNetdClientRegisterNetwork(struct networkSettings *ns);
#else
static int NetmgrNetdClientRegisterNetwork(struct networkSettings *ns)
{
  return 1;
}
#endif /* !defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID) */
#ifdef __cplusplus
}

#endif /* __cplusplus */
