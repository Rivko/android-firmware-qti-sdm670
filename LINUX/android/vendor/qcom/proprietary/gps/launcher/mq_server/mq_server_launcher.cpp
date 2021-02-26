/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 mq-server launcher

 GENERAL DESCRIPTION
 This component launches the mq server thread.

 Copyright (c) 2015, 2017  Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc
 =============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <base_util/log.h>
#include <pthread.h>
#include "mq_server.h"
#include <mq_client/mq_client.h>
#ifndef IZAT_OFFLINE
#include <loc_cfg.h>
#include <loc_pla.h>
#else
#include <base_util/config_file.h>
#endif

using namespace qc_loc_fw;

#ifdef IZAT_OFFLINE
#ifndef ROOT_DIR
#define ROOT_DIR ""
#endif
#define CONFIG_NAME ROOT_DIR "xtwifi.conf"
#else
#define CONFIG_NAME LOC_PATH_IZAT_CONF
#endif // #ifdef IZAT_OFFLINE

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO_RC(ERR,RC,X) if(0!=(RC=(X))) {result = (ERR); break;}

static int sIzatDebugLevel = (int)EL_WARNING;
#ifndef IZAT_OFFLINE
static const loc_param_s_type mq_conf_param_table[] =
{
    {"IZAT_DEBUG_LEVEL", &sIzatDebugLevel, NULL, 'n'},
};
#endif

typedef void* (*pthread_func) (void*);

void location_mq_run(void* /*arg*/)
{
  int result = 1;

  MessageQueueServer * service = 0;

#ifdef IZAT_OFFLINE
  ConfigFile * config = 0;
#endif

  do
  {
    sigset_t set;
    // Block signal SIGCHLD and SIGALRM in this thread
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#ifndef IZAT_OFFLINE
    UTIL_READ_CONF(CONFIG_NAME, mq_conf_param_table);
#else
    ConfigFile * config = ConfigFile::createInstance(CONFIG_NAME);
    BREAK_IF_ZERO(2, config);
    if(config->loaded())
    {
      BREAK_IF_NON_ZERO(4, config->get_PZ_Int32Default("DEBUG_LOC_MQ_LOG_LEVEL", sIzatDebugLevel, (int)EL_WARNING));
    }
#endif

    BREAK_IF_NON_ZERO(5, log_set_local_level_for_tag("MessageQ", (ERROR_LEVEL)sIzatDebugLevel));
    BREAK_IF_NON_ZERO(6, log_set_local_level_for_tag("MessageQueueClientInfo", (ERROR_LEVEL)sIzatDebugLevel));

    service = MessageQueueServer::createInstance();
    BREAK_IF_ZERO(7, service);

    BREAK_IF_NON_ZERO(8, service->setServerNameDup(MQ_SERVER_SOCKET_NAME));
    BREAK_IF_NON_ZERO(9, service->openListenerSocket());

#ifndef IZAT_OFFLINE
    log_debug("location-mq","SRV Socket-Opend");
    pthread_mutex_lock(&srv_sockt_mutex);
    pthread_cond_signal(&srv_sockt_cond);
    pthread_mutex_unlock(&srv_sockt_mutex);
#endif
    service->run();

    BREAK_IF_NON_ZERO(20, log_flush_local_level_for_tag("MessageQ"));
    BREAK_IF_NON_ZERO(21, log_flush_local_level_for_tag("MessageQueueClientInfo"));

    result = 0;
  } while (0);

  delete service;
  service = 0;

#ifdef IZAT_OFFLINE
  delete config;
  config = 0;
#endif

  if(0 != result)
  {
    log_error("location-mq", "queue server creation failed %d", result);
  }
  log_error("location-mq", "terminated");
}

#if defined (__ANDROID__) || defined (USE_GLIB)
int mq_server_launch()
{
#ifdef __ANDROID__
  (void)log_set_global_level(EL_LOG_ALL);
#endif
  pthread_t threadId;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int result = pthread_create(&threadId, &attr,
                 reinterpret_cast <pthread_func>(location_mq_run), NULL);
  if (result != 0)
  {
    log_error("location-mq", "pthread_create failed error = %d", result);
  }
  return result;
}
#else
int main()
{
  (void)log_set_global_level(EL_LOG_ALL);
  location_mq_run(NULL);
  return 0;
}
#endif
