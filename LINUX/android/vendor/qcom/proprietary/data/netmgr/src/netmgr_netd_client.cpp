

/******************************************************************************

                  N E T M G R _ N E T D _C L I E N T . CPP

******************************************************************************/

/******************************************************************************

  @file    netmgr_netd_client.cpp
  @brief   Network manager netd client

  DESCRIPTION
  Implementation of netd client module

******************************************************************************/
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#if (!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))
extern "C" {
#include "netmgr_kif.h"
}

#ifdef __cplusplus
#include "netmgr_netd_client.h"
#include <android/system/net/netd/1.0/INetd.h>

using ::android::hardware::Return;
using ::android::sp;
using ::android::wp;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::manager::V1_0::IServiceNotification;
using ::android::hardware::hidl_string;
using ::android::hardware::Void;

using android::system::net::netd::V1_0::INetd;

class NetmgrNetdClient
{
public:
  static NetmgrNetdClient *getInstance();

  NetmgrNetdClient();

  static NetmgrNetdClient *instancePtr;
  struct NetdProxy : public hidl_death_recipient,
                           public IServiceNotification
  {
public:
    NetdProxy(NetmgrNetdClient &);
    void serviceDied(uint64_t cookie, const wp<IBase>& who) override;
    Return<void> onRegistration(const hidl_string &fqName,
                                const hidl_string &name,
                                bool preexisting) override;
    virtual ~NetdProxy();
    NetmgrNetdClient &com;
    sp<INetd> mService;
    bool registerServerNotification();
    bool unregisterLinkToDeath();
    std::tuple<uint64_t, uint32_t> registerNetwork();
    pthread_t getServiceThread;
    bool startGetServiceThread();
    static void *getServiceImpl(void *);
  };
  sp<NetdProxy> mApiService;
};

#define NETD_SERVICE ""

#else
  typedef struct NetmgrNetdClient NetmgrNetdClient;
#endif /* __cplusplus*/

#ifdef __cplusplus
#define NETMGR_SUCCESS (0)
#define NETMGR_FAILURE (-1)

  /* C++ treats anything between "" as string literals which is interpreted as const char*.
   * The current logging macros do not have support for handling const char* so we created
   * a new function to handle this. If required we can move it into ds_util module */
#define NETMGR_MAX_LOG_MSG_SIZE      512
/*---------------------------------------------------------------------------
   Function for formating log messages
---------------------------------------------------------------------------*/
  LOCAL void
  netmgr_format_msg (char* buf_ptr, const char* fmt,...)
  {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf_ptr, NETMGR_MAX_LOG_MSG_SIZE, fmt, ap);
    va_end(ap);
  }
#if defined(FEATURE_DATA_LOG_QXDM) && defined(FEATURE_DATA_LOG_ADB)

  #define DS_LOG_TAG "QC-NETMGR-LIB"
  #undef  LOG_TAG
  #define LOG_TAG DS_LOG_TAG

  #define NETMGR_LOG_ERR(...)                                         \
  do                                                                  \
  {                                                                   \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, "%s", buf);  \
    LOGE(__VA_ARGS__);                                                \
  }while (0)

  #define NETMGR_LOG_LOW(...)                                         \
  do{                                                                 \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, "%s", buf);    \
    LOGI(__VA_ARGS__);                                                \
  }while (0)

  #define NETMGR_LOG_MED(...)                                         \
  do{                                                                 \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, "%s", buf);    \
    LOGI(__VA_ARGS__);                                                \
  }while (0)

  #define NETMGR_LOG_HIGH(...)                                        \
  do{                                                                 \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, "%s", buf);   \
    LOGD(__VA_ARGS__);                                                \
  }while (0)

#elif defined(FEATURE_DATA_LOG_QXDM)
  #define NETMGR_LOG_ERR(...)                                         \
  {                                                                   \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, "%s", buf);  \
  }

  #define NETMGR_LOG_LOW(...)                                         \
  {                                                                   \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, "%s", buf);    \
  }

  #define NETMGR_LOG_MED(...)                                         \
  {                                                                   \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, "%s", buf);    \
  }

  #define NETMGR_LOG_HIGH(...)                                        \
  {                                                                   \
    char buf[NETMGR_MAX_LOG_MSG_SIZE];                                \
    netmgr_format_msg(buf, __VA_ARGS__);                              \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, "%s", buf);   \
  }
#elif defined(FEATURE_DATA_LOG_ADB)
  #define DS_LOG_TAG "QC-NETMGR-LIB"
  #undef  LOG_TAG
  #define LOG_TAG DS_LOG_TAG

  #define NETMGR_LOG_ERR(...)  LOGE(__VA_ARGS__)

  #define NETMGR_LOG_LOW(...)  LOGI(__VA_ARGS__)

  #define NETMGR_LOG_MED(...)  LOGI(__VA_ARGS__)

  #define NETMGR_LOG_HIGH(...) LOGD(__VA_ARGS__)
#endif

#else
  #define NETMGR_LOG_ERR  netmgr_log_err
  #define NETMGR_LOG_LOW  netmgr_log_low
  #define NETMGR_LOG_MED  netmgr_log_med
  #define NETMGR_LOG_HIGH netmgr_log_high
#endif

#ifdef __cplusplus
NetmgrNetdClient* NetmgrNetdClient::instancePtr = NULL;

NetmgrNetdClient* NetmgrNetdClient::getInstance(void)
{
  if (instancePtr == NULL)
  {
    instancePtr = new NetmgrNetdClient();
  }

  return instancePtr;
}

NetmgrNetdClient::NetdProxy::NetdProxy
(
  NetmgrNetdClient &_com
):com(_com)
{
  memset(&getServiceThread,0,sizeof(pthread_t));
  registerServerNotification();
}

NetmgrNetdClient::NetdProxy::~NetdProxy
(
)
{
    unregisterLinkToDeath();
}

bool NetmgrNetdClient::NetdProxy::registerServerNotification()
{
  sp<NetdProxy> self = this;

    Return<bool> retVal = INetd::registerForNotifications(NETD_SERVICE, self);
    if (!retVal.isOk())
    {
      NETMGR_LOG_ERR("%s(): Failed to register for Netd HAL service", __func__);
      return false;
    }
    else
    {
      NETMGR_LOG_HIGH("%s(): Successfully registered for Netd HAL service", __func__);
      return true;
    }
}

bool NetmgrNetdClient::NetdProxy::unregisterLinkToDeath()
{
    sp<NetdProxy> self = this;
    Return<bool> retVal = mService->unlinkToDeath(self);
    if (!retVal.isOk())
    {
        NETMGR_LOG_ERR("%s(): Failure unregisterLinkToDeath!", __func__);
        return false;
    }
    else
    {
        NETMGR_LOG_ERR("%s(): Success unregisterLinkToDeath!", __func__);
        return true;
    }
}

std::tuple<uint64_t, uint32_t> NetmgrNetdClient::NetdProxy::registerNetwork()
{
    uint32_t _packetMark;
    uint32_t net_id;
    int32_t _errMsg;
    uint64_t _networkHandle;

    if (mService == NULL)
    {
      NETMGR_LOG_ERR("%s(): mSerivce is NULL", __func__);
      return std::make_tuple(0, 0);
    }

    NETMGR_LOG_LOW("%s(): Attempting createOemNetwork", __func__);
    auto retVal = mService->createOemNetwork([&](uint64_t networkHandle, uint32_t packetMark, const auto& errMsg){
      _packetMark = packetMark;
      _networkHandle = networkHandle;
    });

    if (_packetMark == 0)
    {
      NETMGR_LOG_ERR("%s(): Failure in createOemNetwork", __func__);
      return std::make_tuple(0, 0);
    }
    else
    {
      net_id = _packetMark & 0xFFFF;
      NETMGR_LOG_MED("%s(): createOemNetwork succeeded [packet mark : %u] [net id : %u] [network handle : %lu]",
                     __func__, _packetMark, net_id, _networkHandle);
      return std::make_tuple(_networkHandle, _packetMark);
    }
}

void *NetmgrNetdClient::NetdProxy::getServiceImpl(void *arg)
{
    NetmgrNetdClient::NetdProxy *self = (NetmgrNetdClient::NetdProxy*)(arg);

    self->mService = INetd::getService();
    if (self->mService == NULL)
    {
      NETMGR_LOG_ERR("%s(): service is null", __func__);
    }
    else
    {
      NETMGR_LOG_LOW("%s(): INetd discovered", __func__);
    }

    pthread_exit(0);
}

bool NetmgrNetdClient::NetdProxy::startGetServiceThread()
{
    pthread_attr_t tattr;
    void *args = reinterpret_cast<void *> (this);
    if (pthread_attr_init(&tattr))
    {
        NETMGR_LOG_ERR("Thread attribute object init failed");
        return false;
    }
    else if (pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED))
    {
        NETMGR_LOG_ERR("Could not set detached on the receiving thread");
        pthread_attr_destroy(&tattr);
        return false;
    }
    else if (pthread_create(&getServiceThread, &tattr, getServiceImpl, args))
    {
        NETMGR_LOG_ERR("Could not create monitoring thread");
        pthread_attr_destroy(&tattr);
        return false;
    }
    else
    {
        pthread_attr_destroy(&tattr);
        return true;
    }
}

NetmgrNetdClient::NetmgrNetdClient()
{
  mApiService = new NetdProxy(*this);
}

Return<void> NetmgrNetdClient::NetdProxy::onRegistration
(
   const hidl_string &fqName,
   const hidl_string &name,
   bool preexisting
)
{
    #pragma unused (fqName)
    #pragma unused (name)

    NETMGR_LOG_MED("%s(): Netd service preexisting : [%d]", __func__, preexisting);

    if (preexisting)
    {
      NETMGR_LOG_MED("%s(): Service exists, not a restart", __func__);
    }
    else
    {
      NETMGR_LOG_HIGH("%s(): Service init, netd restart detected", __func__);
      netmgr_kif_handle_netd_event();
    }

    NETMGR_LOG_MED("%s(): Starting Netd getService thread", __func__);
    startGetServiceThread();
    return Void();
}

void NetmgrNetdClient::NetdProxy::serviceDied(uint64_t cookie, const wp<IBase>& who)
{
  NETMGR_LOG_ERR("%s(): Netd service died", __func__);
}
#endif /* __cplusplus*/

extern "C" {
int NetmgrNetdClientInit(void)
{
  const char *dev_id;
  int rc = NETMGR_FAILURE;
  pthread_mutexattr_t attr;

  NETMGR_LOG_LOW("%s(): Looking for Netd service", __func__);
  NetmgrNetdClient* netd_client = NetmgrNetdClient::getInstance();
  if (netd_client == NULL)
  {
    NETMGR_LOG_ERR("%s(): Failed to allocate netd client", __func__);
  }
  else
  {
    NETMGR_LOG_MED("%s(): Created netd client", __func__);
  }

  return NETMGR_SUCCESS;
}

int NetmgrNetdClientRegisterNetwork(struct networkSettings *ns)
{
  const char *dev_id;
  int rc = NETMGR_FAILURE;
  pthread_mutexattr_t attr;
  uint64_t networkHandle = 0;
  uint32_t packetMark = 0;

  if (ns == NULL)
  {
    NETMGR_LOG_ERR("%s(): Network Settings input buffer is NULL", __func__);
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_LOW("%s(): Looking for Netd service", __func__);
  NetmgrNetdClient* netd_client = NetmgrNetdClient::getInstance();
  if (netd_client == NULL)
  {
    NETMGR_LOG_ERR("%s(): Failed to allocate netd client", __func__);
    return NETMGR_FAILURE;
  }
  else
  {
    NETMGR_LOG_MED("%s(): Created netd client", __func__);
  }

  std::tie(networkHandle, packetMark) = netd_client->mApiService->registerNetwork();
  NETMGR_LOG_MED("%s(): [packet mark : %u] [network handle : %lu]", __func__, packetMark,  networkHandle);

  if ((networkHandle) && (packetMark))
  {
    ns->networkHandle = networkHandle;
    ns->packetMark = packetMark;
    return NETMGR_SUCCESS;
  }
  else
  {
    return NETMGR_FAILURE;
  }

}
}
#endif /* !defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID)  */
