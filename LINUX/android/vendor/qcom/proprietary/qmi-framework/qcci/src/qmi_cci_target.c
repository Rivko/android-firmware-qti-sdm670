/******************************************************************************
  @file    qmi_cci_target.c
  @brief   The QMI common client interface target specific module

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2017, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"

#ifdef QMI_FW_ANDROID
#include <cutils/properties.h>
#include <private/android_filesystem_config.h>
#include "mdm_detect.h"

#define QMI_FW_PROPERTY_BASEBAND "ro.baseband"
#define QMI_FW_PROPERTY_BASEBAND_SIZE (PROPERTY_VALUE_MAX)
#define QMI_FW_BASEBAND_VALUE_APQ "apq"
#define QMI_FW_BASEBAND_VALUE_AUTO_NOMDM "auto_nomdm"
#define QMI_FW_BASEBAND_VALUE_UNDEFINED "undefined"
#endif

#define QMI_FW_CONF_FILE "/etc/qmi_fw.conf"
#define QMI_FW_CONF_FILE_VENDOR "/vendor/etc/qmi_fw.conf"
#define MAX_LINE_LENGTH 80
#define QMI_CCI_DBG_CONF_STR "QMI_CCI_DEBUG_LEVEL="

#ifdef QCCI_OVER_QMUX
#include "qmi_client_instance_defs.h"
extern qmi_cci_xport_ops_type qmuxd_ops;
#else
typedef int qmi_client_qmux_instance_type;
#endif

extern qmi_cci_xport_ops_type qcci_ipc_router_ops;
extern int open_lookup_sock_fd(void);
extern void close_lookup_sock_fd(void);
extern int smem_log_init(void);
extern void smem_log_exit(void);
extern void qmi_cci_init(void);
extern void qmi_cci_deinit(void);
extern void qmi_cci_xport_ctrl_port_deinit(void);

static bool mdm_present;
static unsigned int internal_mdm;
static unsigned int external_mdm;

#ifdef QMI_FW_SYSLOG
#define DEFAULT_DBG_LEVEL 4
#else
#define DEFAULT_DBG_LEVEL 5
#endif

unsigned int qmi_cci_debug_level = DEFAULT_DBG_LEVEL;

#if defined (QMI_FW_ANDROID) || defined (QMI_FW_SYSLOG)
static void qmi_cci_debug_init(void)
{
  char line[MAX_LINE_LENGTH];
  char debug_level[2];
  FILE *fp;

  fp = fopen(QMI_FW_CONF_FILE, "r");
  if(!fp)
    fp = fopen(QMI_FW_CONF_FILE_VENDOR, "r");
  if(!fp)
    return;

  while(fgets(line, MAX_LINE_LENGTH, fp))
  {
    if(!strncmp(line, QMI_CCI_DBG_CONF_STR, strlen(QMI_CCI_DBG_CONF_STR)))
    {
      debug_level[0] = line[strlen(QMI_CCI_DBG_CONF_STR)];
      debug_level[1] = '\0';
      if(isdigit(debug_level[0]))
      {
        qmi_cci_debug_level = atoi(debug_level);
        break;
      }
    }
  }
  fclose(fp);
}
#else
static void qmi_cci_debug_init(void)
{
}
#endif /* QMI_FW_ANDROID) || QMI_FW_SYSLOG */

#ifdef QMI_FW_ANDROID
static void qmi_cci_is_mdm_present()
{
  int ret = 0;
  char args[QMI_FW_PROPERTY_BASEBAND_SIZE];

  memset(args, 0, sizeof(args));
  ret = property_get(QMI_FW_PROPERTY_BASEBAND, args,
                     QMI_FW_BASEBAND_VALUE_UNDEFINED);
  if ((ret > 0) && (ret <= QMI_FW_PROPERTY_BASEBAND_SIZE))
  {
    /*In non APQ targets, use QMUXD*/
    if(strncmp(args, QMI_FW_BASEBAND_VALUE_APQ,
               sizeof(QMI_FW_BASEBAND_VALUE_APQ))
	&& strncmp(args, QMI_FW_BASEBAND_VALUE_AUTO_NOMDM,
		sizeof(QMI_FW_BASEBAND_VALUE_AUTO_NOMDM)))
    {
      mdm_present = true;
    }
  }
}

static void qmi_cci_identify_mdm_type()
{
  struct dev_info peripherals;
  int i;

  if(get_system_info(&peripherals) == RET_SUCCESS)
  {
    for(i = 0; i < peripherals.num_modems; i++)
    {
      if(MDM_TYPE_EXTERNAL == peripherals.mdm_list[i].type)
      {
        external_mdm = 1;
      }
      else if(MDM_TYPE_INTERNAL == peripherals.mdm_list[i].type)
      {
        internal_mdm = 1;
      }
    }
  }
}
#else
static void qmi_cci_is_mdm_present()
{
  mdm_present = true;
}

static void qmi_cci_identify_mdm_type()
{
  internal_mdm = 1;
  external_mdm = 1;
}
#endif /* QMI_FW_ANDROID */

#ifdef QCCI_OVER_QMUX
/*===========================================================================
  FUNCTION  qmi_cci_xport_register
===========================================================================*/
qmi_client_error_type qmi_cci_qmux_xport_register
(
 qmi_client_qmux_instance_type instance
)
{
  if ((instance < QMI_CLIENT_QMUX_BASE_INSTANCE) ||
      (instance > QMI_CLIENT_QMUX_MAX_INSTANCE_IDS))
    return QMI_INTERNAL_ERR;

  qmi_cci_xport_start(&qmuxd_ops, (void *)instance);
  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_unregister
===========================================================================*/
qmi_client_error_type qmi_cci_qmux_xport_unregister
(
 qmi_client_qmux_instance_type instance
)
{
  if ((instance < QMI_CLIENT_QMUX_BASE_INSTANCE) ||
      (instance > QMI_CLIENT_QMUX_MAX_INSTANCE_IDS))
    return QMI_INTERNAL_ERR;

  qmi_cci_xport_stop(&qmuxd_ops, (void *)instance);
  return QMI_NO_ERR;
}

static void qmi_cci_start_def_qmux_xports_internal()
{
  if (internal_mdm)
    qmi_cci_xport_start(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_INSTANCE_0);
}

static void qmi_cci_start_def_qmux_xports_external()
{
  if (external_mdm)
  {
    qmi_cci_xport_start(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0);
    qmi_cci_xport_start(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0);
    qmi_cci_xport_start(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0);
  }
}

static void qmi_cci_stop_def_qmux_xports()
{
  if(internal_mdm)
  {
    qmi_cci_xport_stop(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_INSTANCE_0);
  }

  if(external_mdm)
  {
    qmi_cci_xport_stop(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0);
    qmi_cci_xport_stop(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0);
    qmi_cci_xport_stop(&qmuxd_ops, (void *)QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0);
  }
}

#else
qmi_client_error_type qmi_cci_qmux_xport_register
(
 qmi_client_qmux_instance_type instance
)
{
  return QMI_INTERNAL_ERR;
}

qmi_client_error_type qmi_cci_qmux_xport_unregister
(
 qmi_client_qmux_instance_type instance
)
{
  return QMI_INTERNAL_ERR;
}

static void qmi_cci_start_def_qmux_xports_internal()
{
}

static void qmi_cci_start_def_qmux_xports_external()
{
}

static void qmi_cci_stop_def_qmux_xports()
{
}
#endif /* QCCI_OVER_QMUX */

/*===========================================================================
FUNCTION: qmi_fw_cci_init

DESCRIPTION:
   Initialize the QCCI library.  This function is called when the QCCI
   shared library is loaded, before application's main() is started.

DEPENDENCIES:
   None

ARGUMENTS:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
===========================================================================*/
#ifdef __GNUC__
void __attribute__ ((constructor)) qmi_fw_cci_init(void)
{
  qmi_cci_init();
#ifndef QMI_CCI_SMEM_LOG_DISABLE
  smem_log_init();
#endif /*QMI_CCI_SMEM_LOG_DISABLE*/

  qmi_cci_is_mdm_present();
  if(mdm_present)
    qmi_cci_identify_mdm_type();

  qmi_cci_start_def_qmux_xports_external();
  qmi_cci_xport_start(&qcci_ipc_router_ops, NULL);
  qmi_cci_start_def_qmux_xports_internal();

  qmi_cci_debug_init();
}
#endif

/*===========================================================================
FUNCTION: qmi_fw_cci_deinit

DESCRIPTION:
   Cleans up the QCCI library.  This function is called after exit() or
   after application's main() completes.

DEPENDENCIES:
   None

ARGUMENTS:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
===========================================================================*/
#ifdef __GNUC__
void __attribute__ ((destructor)) qmi_fw_cci_deinit(void)
{
  qmi_cci_xport_stop(&qcci_ipc_router_ops, NULL);
  qmi_cci_stop_def_qmux_xports();
  /* The lookup socket file descriptor is not opened in the constructor due
   * to boot up delay issues.*/
  close_lookup_sock_fd();
  qmi_cci_xport_ctrl_port_deinit();
#ifndef QMI_CCI_SMEM_LOG_DISABLE
  smem_log_exit();
#endif /*QMI_CCI_SMEM_LOG_DISABLE*/
  qmi_cci_deinit();
}
#endif

void qmi_cci_os_signal_wait(QMI_CCI_OS_SIGNAL *ptr, unsigned int timeout_ms)
{
  do{
    ptr->timed_out = 0;
    if(timeout_ms)
    {
      int rc = 0;
      struct timeval tv = {0, 0};
      struct timespec ts = {0, 0};
      if (ptr->clock == CLOCK_MONOTONIC)
      {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_sec = ts.tv_sec + timeout_ms / 1000;
        ts.tv_nsec = ts.tv_nsec + (timeout_ms % 1000) * 1000 * 1000;
      }
      else
      {
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + timeout_ms / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + (timeout_ms % 1000) * 1000 * 1000;
      }
      if (ts.tv_nsec >= 1000000000)
      {
        ts.tv_sec++;
        ts.tv_nsec = (ts.tv_nsec % 1000000000);
      }
      pthread_mutex_lock(&ptr->mutex);
      while(!ptr->sig_set)
      {
        rc = pthread_cond_timedwait(&ptr->cond, &ptr->mutex, &ts);
        if(rc == ETIMEDOUT)
        {
          ptr->timed_out = 1;
          break;
        }
      }
      pthread_mutex_unlock(&ptr->mutex);
    } else {
      pthread_mutex_lock(&ptr->mutex);
      while(!ptr->sig_set)
        pthread_cond_wait(&ptr->cond, &ptr->mutex);
      pthread_mutex_unlock(&ptr->mutex);
    }
  } while(0);
}

