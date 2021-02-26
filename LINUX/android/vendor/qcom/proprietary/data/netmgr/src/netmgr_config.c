/*!
  @file
  netmgr_config.c

  @brief
  This file provides implementation of netmgr configration using configdb.
*/

/*===========================================================================
  Copyright (c) 2013-2017, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "netmgr_config.h"
#include "netmgr_util.h"
#include "configdb.h"
#include "ds_util.h"

#include "netmgr_cb.h"
#include "netmgr_main_cb.h"

#define NETMGR_CONFIG_CB_MODNAME "CONFIG"

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION:  netmgr_process_iwlan_config
===========================================================================*/
/*!
    @brief
    This function processes the iwlan related config items

    @param[in] target: configuration to be used within the XML file.

    @return  0 on error.
    @return -1 on failure.
*/
/*=========================================================================*/
LOCAL
int netmgr_process_iwlan_config
(
  const char* target
)
{
  int result = 0;
  int ret;
  int i,j, rev_min;
  char prop_name[NETMGR_CFGDB_STRVAL_MAX];
  char prop_val_str[NETMGR_CFGDB_STRVAL_MAX];
  int32 prop_val_int;
  int32 rev_control_ports_len = 0;
  int32 rev_data_ports_len = 0;

  rev_min = 0;
  /* iwlan_enable */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".iwlan_enable");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto err;
  }
  switch(prop_val_int)
  {
    case 0:
     netmgr_main_cfg.iwlan_enabled = IWLAN_DISABLE;
     break;

    case 1:
     netmgr_main_cfg.iwlan_enabled = IWLAN_ENABLE;
     break;

    case 2:
     netmgr_main_cfg.iwlan_enabled = NSWO_ONLY;
     break;

    default:
     netmgr_main_cfg.iwlan_enabled = IWLAN_DISABLE;
     netmgr_log_err("Error reading property [%s] property value[%d]", prop_name, prop_val_int);
  }

  /* The reverse ports starting and ending index depends on the
   * modem being used. Query the correct starting and ending indexes
   */
  if (netmgr_main_cfg.modem_enable[0])
  {
    rev_min = NETMGR_MAIN_GET_INST_MIN_REV(NETMGR_MODEM_MSM);
  }
  else if(netmgr_main_cfg.modem_enable[1])
  {
    rev_min = NETMGR_MAIN_GET_INST_MIN_REV(NETMGR_MODEM_MDM);
  }

  /* rev_control_ports_len and rev_control_ports */
  j = 0;
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".rev_control_ports_len");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &rev_control_ports_len, sizeof(rev_control_ports_len));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto err;
  }

  for (i = rev_min; i < (rev_min + rev_control_ports_len); i++)
  {
    snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s%d", "netmgr_config.", target, ".rev_control_ports.",j);
    memset(prop_val_str, 0, sizeof(prop_val_str));
    ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, &prop_val_str, sizeof(prop_val_str));
    if (ret != CFGDB_SUCCESS)
    {
      netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
      result = -1;
      goto err;
    }
    strlcpy(netmgr_ctl_port_array[i].qmi_conn_id, prop_val_str, sizeof(netmgr_ctl_port_array[i].qmi_conn_id));
    netmgr_ctl_port_array[i].enabled = TRUE;
    j++;
  }

  /* rev_data_ports_len and rev_data_ports */
  j = 0;
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".rev_data_ports_len");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &rev_data_ports_len, sizeof(rev_data_ports_len));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto err;
  }
  netmgr_main_cfg.num_rev_data_ports = rev_data_ports_len;

  for (i = rev_min; i < (rev_min + rev_data_ports_len); i++)
  {
    snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s%d", "netmgr_config.", target, ".rev_data_ports.",j);
    memset(prop_val_str, 0, sizeof(prop_val_str));
    ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, &prop_val_str, sizeof(prop_val_str));
    if (ret != CFGDB_SUCCESS)
    {
      netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
      result = -1;
      goto err;
    }
    strlcpy(netmgr_ctl_port_array[i].data_ctl_port, prop_val_str, sizeof(netmgr_ctl_port_array[i].data_ctl_port));
    netmgr_ctl_port_array[i].enabled = TRUE;
    j++;
  }

  /* Remove prefix table as an enhancement later */
  if (netmgr_main_cfg.rmnet_data_enabled)
  {
    if (netmgr_main_cfg.modem_enable[0])
    {
      /* Change prefix for reverse links */
      netmgr_log_high("%s", "Changing MSM rev prefixes");
      strlcpy(NETMGR_MAIN_GET_DEV_REV_PREFIX(NETMGR_MODEM_MSM),
              NETMGR_MAIN_REV_RMNET_DATA_PREFIX,
              NETMGR_IF_NAME_MAX_LEN);
    }
    else if(netmgr_main_cfg.modem_enable[1])
    {
      /* Change prefix for reverse links */
      netmgr_log_high("%s", "Changing MDM rev prefixes");
      strlcpy(NETMGR_MAIN_GET_DEV_REV_PREFIX(NETMGR_MODEM_MDM),
              NETMGR_MAIN_REV_RMNET_DATA_PREFIX,
              NETMGR_IF_NAME_MAX_LEN);
    }
  }

err:
  return result;
}

#endif /* FEATURE_DATA_IWLAN */


/*===========================================================================
  FUNCTION:  netmgr_main_cfg_update_ike_port
===========================================================================*/
/*!
@brief
  Helper function to update the current IKE port being used

  @param[in] port:  host order port.

  @return  port on success.
  @return  NETMGR_MAIN_IKE_PORT_INVALID on failure.
*

*/
/*=========================================================================*/
int netmgr_main_cfg_update_ike_port(int port, char *local_addr, char *dest_addr)
{
  int i = 0;
  int ret = NETMGR_MAIN_IKE_PORT_INVALID;
  size_t bytes = 0;

  if (!local_addr || !dest_addr)
  {
    return NETMGR_MAIN_IKE_PORT_INVALID;
  }

  pthread_mutex_lock(&netmgr_main_cfg_lock);
  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    /* Update port with addresses in rev_ind*/
    if (netmgr_main_cfg.ike_ports[i].ike_port == port)
    {
      /*If local and dest addr is already the same don't update*/
      if (!strcmp(netmgr_main_cfg.ike_ports[i].local_addr, local_addr)
          && !strcmp(netmgr_main_cfg.ike_ports[i].dest_addr, dest_addr))
      {
        break;
      }
      bytes = strlcpy(netmgr_main_cfg.ike_ports[i].local_addr, local_addr, NETMGR_MAX_STR_LENGTH);
      if (bytes >= NETMGR_MAX_STR_LENGTH)
      {
        netmgr_log_err("%s(): failed to copy local address to table!", __func__);
        memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
        goto bail;
      }

      bytes = strlcpy(netmgr_main_cfg.ike_ports[i].dest_addr, dest_addr, NETMGR_MAX_STR_LENGTH);
      if (bytes >= NETMGR_MAX_STR_LENGTH)
      {
        netmgr_log_err("%s(): failed to copy dest address to table!", __func__);
        memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
        memset(netmgr_main_cfg.ike_ports[i].dest_addr, 0, NETMGR_MAX_STR_LENGTH);
        goto bail;
      }
      netmgr_log_med("%s(): update for port %d found at index %d", __func__, port, i);
      break;
    }
  }
  /* Port missing from cfg table*/
  if (i == MAX_IKE_PORTS)
  {
    netmgr_log_err("%s(): port missing from table in update", __func__);
    goto bail;
  }

  ret = port;

bail:
  pthread_mutex_unlock(&netmgr_main_cfg_lock);
  return ret;
}

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_get_ike_port
===========================================================================*/
/*!
@brief
  Helper function to query the current IKE port being used
*/
/*=========================================================================*/
int netmgr_main_cfg_get_ike_port(char *local_addr, char *dest_addr)
{
  int port = NETMGR_MAIN_IKE_PORT_INVALID;
  int i = 0;
  size_t bytes = 0;

  pthread_mutex_lock(&netmgr_main_cfg_lock);

  if (!local_addr || !dest_addr)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    /* Search the array for matching local_addr and dest_addr entries */
    if (!strcmp(netmgr_main_cfg.ike_ports[i].local_addr, local_addr)
          && !strcmp(netmgr_main_cfg.ike_ports[i].dest_addr, dest_addr))
    {
      break;
    }
  }

  if (MAX_IKE_PORTS == i)
  {
    /* If there is no matching entry, update one of the existing entries for which
       local_addr and dest_addr are empty and a valid IKE port is set */
    for (i = 0; i < MAX_IKE_PORTS; i++)
    {
      if (NETMGR_MAIN_IKE_PORT_INVALID != netmgr_main_cfg.ike_ports[i].ike_port
            && strlen(netmgr_main_cfg.ike_ports[i].local_addr) == 0
            && strlen(netmgr_main_cfg.ike_ports[i].dest_addr) == 0)
      {
        break;
      }
    }

    if (MAX_IKE_PORTS == i)
    {
      /* If we are still unable to locate an entry return failure */
      netmgr_log_err("%s(): no valid entry available to save IKE port information!", __func__);
      goto bail;
    }

    bytes = strlcpy(netmgr_main_cfg.ike_ports[i].local_addr, local_addr, NETMGR_MAX_STR_LENGTH);
    if (bytes >= NETMGR_MAX_STR_LENGTH)
    {
      netmgr_log_err("%s(): failed to copy local address to table!", __func__);
      memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
      goto bail;
    }

    bytes = strlcpy(netmgr_main_cfg.ike_ports[i].dest_addr, dest_addr, NETMGR_MAX_STR_LENGTH);
    if (bytes >= NETMGR_MAX_STR_LENGTH)
    {
      netmgr_log_err("%s(): failed to copy dest address to table!", __func__);
      memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
      memset(netmgr_main_cfg.ike_ports[i].dest_addr, 0, NETMGR_MAX_STR_LENGTH);
      goto bail;
    }
  }

  port = netmgr_main_cfg.ike_ports[i].ike_port;

bail:
  pthread_mutex_unlock(&netmgr_main_cfg_lock);
  return port;
}

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_set_ike_port
===========================================================================*/
/*!
@brief
  Sets the IKE port which will be used for iWLAN call
*/
/*=========================================================================*/
void netmgr_main_cfg_set_ike_port(int port)
{
  int i = 0;

  pthread_mutex_lock(&netmgr_main_cfg_lock);

  /* 1. Save the port in an empty slot in the ike_ports array
     2. Set the local and dest addr fields to empty string
     3. When someone calls get_ike_port we will search for a matching address entry
        If found, return the corresponding port
        If not found, update the entry with empty local and dest address fields and return
        the corresponding port */
  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    if (NETMGR_MAIN_IKE_PORT_INVALID == netmgr_main_cfg.ike_ports[i].ike_port)
    {
      /* Add port info */
      netmgr_log_med("%s(): setting IKE port to %d", __func__, port);
      netmgr_main_cfg.ike_ports[i].ike_port = port;

      /* Zero out the address info */
      memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
      memset(netmgr_main_cfg.ike_ports[i].dest_addr, 0, NETMGR_MAX_STR_LENGTH);

      break;
    }
  }

  if (MAX_IKE_PORTS == i)
  {
    netmgr_log_err("%s(): maximum IKE ports reached. Could not save information for port [%d]",
                   __func__, port);
  }

  pthread_mutex_unlock(&netmgr_main_cfg_lock);
}

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_associate_local_addr_with_ike_ports
===========================================================================*/
/*!
@brief
  Update the local address for all valid IKE ports
*/
/*=========================================================================*/
void netmgr_main_cfg_associate_local_addr_with_ike_ports(char *local_addr)
{
  int i = 0;
  size_t bytes = 0;

  pthread_mutex_lock(&netmgr_main_cfg_lock);

  if (!local_addr)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    pthread_mutex_unlock(&netmgr_main_cfg_lock);
    return;
  }

  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    if (NETMGR_MAIN_IKE_PORT_INVALID != netmgr_main_cfg.ike_ports[i].ike_port
          && strlen(netmgr_main_cfg.ike_ports[i].dest_addr) != 0)
    {
      /* Update the local addresses for all valid entries */
      bytes = strlcpy(netmgr_main_cfg.ike_ports[i].local_addr, local_addr,
                      NETMGR_MAX_STR_LENGTH);
      if (bytes >= NETMGR_MAX_STR_LENGTH)
      {
        netmgr_log_err("%s(): failed to copy local address to table!", __func__);
        memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
      }
    }
  }

  pthread_mutex_unlock(&netmgr_main_cfg_lock);
}

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_cleanup_local_addr_for_ike_port
===========================================================================*/
/*!
@brief
  Cleanup the local address associated with all valid IKE ports
*/
/*=========================================================================*/
void netmgr_main_cfg_cleanup_local_addr_for_ike_ports(char *local_addr)
{
  int i = 0;

  pthread_mutex_lock(&netmgr_main_cfg_lock);

  if (!local_addr)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    pthread_mutex_unlock(&netmgr_main_cfg_lock);
    return;
  }

  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    if (NETMGR_MAIN_IKE_PORT_INVALID != netmgr_main_cfg.ike_ports[i].ike_port
          && strlen(netmgr_main_cfg.ike_ports[i].dest_addr) != 0
          && !strcmp(local_addr, netmgr_main_cfg.ike_ports[i].local_addr))
    {
      /* If the local address changes for some reason we need to ensure that
         the local_addr info for all the matching table entries is cleaned up.
         When the new local_addr info is available, the table entries will be
         repopulated */
      memset(netmgr_main_cfg.ike_ports[i].local_addr, 0, NETMGR_MAX_STR_LENGTH);
    }
  }

  pthread_mutex_unlock(&netmgr_main_cfg_lock);
}

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_cleanup_ike_port
===========================================================================*/
/*!
@brief
  Cleanup entry corresponding to the IKE port
*/
/*=========================================================================*/
void netmgr_main_cfg_cleanup_ike_port(int port)
{
  int i = 0;

  pthread_mutex_lock(&netmgr_main_cfg_lock);

  for (i = 0; i < MAX_IKE_PORTS; i++)
  {
    if (netmgr_main_cfg.ike_ports[i].ike_port == port)
    {
      /* Zero out the address info */
      memset(&netmgr_main_cfg.ike_ports[i], 0, sizeof(netmgr_ike_port_info_t));
      netmgr_main_cfg.ike_ports[i].ike_port = NETMGR_MAIN_IKE_PORT_INVALID;
      break;
    }
  }

  if (MAX_IKE_PORTS == i)
  {
    netmgr_log_err("%s(): no information found for port [%d]", __func__, port);
  }

  pthread_mutex_unlock(&netmgr_main_cfg_lock);
}

/*===========================================================================
  FUNCTION:  netmgr_process_cmd_logging_config
===========================================================================*/
/*!
    @brief
    This function processes the ds_exec option overrides

    @return  nothing
*/
/*=========================================================================*/
LOCAL
void netmgr_process_cmd_logging_config
(
  void
)
{
  int result = 0;
  int ret;
  char *prop_name;
  char prop_val_str[NETMGR_CFGDB_STRVAL_MAX];
  int32 prop_val_int;

  prop_name = "netmgr_config.cmd_logging_config.cmd_debug";
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Couldn't read property [%s] Err[%d], defaulting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.exec_opts.cmd_debug = prop_val_int;

  prop_name = "netmgr_config.cmd_logging_config.log_options";
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Couldn't read property [%s] Err[%d], defaulting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.exec_opts.log_options = prop_val_int;

  prop_name = "netmgr_config.cmd_logging_config.outfile";
  memset(prop_val_str, 0, sizeof(prop_val_str));
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, &prop_val_str, sizeof(prop_val_str));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    memset(prop_val_str, 0, sizeof(prop_val_str));
  }
  strlcpy(netmgr_main_cfg.exec_opts.outfile, prop_val_str, sizeof(netmgr_main_cfg.exec_opts.outfile));
}

/*===========================================================================
  FUNCTION:  netmgr_config_load
===========================================================================*/
/*!
    @brief
    This function loads the configuration based on the config file and
    "target" property specified.

    @param[in] xml_file: XML file location string.
    @param[in] target: configuration to be used within the XML file.

    @return  0 if configuration file is loaded properly.
    @return -1 if configuration file file load/parase fails.
*/
/*=========================================================================*/
int netmgr_config_load
(
  const char *xml_file,
  const char *target
)
{
  int result = 0;
  int32 ret;
  int i;
  char prop_name[NETMGR_CFGDB_STRVAL_MAX];
  int32 num_modems = 0;
  int32 control_ports_len = 0;
  int32 data_ports_len = 0;
  int32 prop_val_int;
  char prop_val_str[NETMGR_CFGDB_STRVAL_MAX];

  netmgr_log_med("netmgr_config: Configuring using file %s, target %s\n", xml_file, target);
  ret = configdb_init(CFGDB_OPMODE_CACHED, xml_file);
  if (CFGDB_SUCCESS != ret)
  {
    netmgr_log_err("Unable to open/parse config file [%s] Err [%d]", xml_file, ret);
    result = -1;
    goto bail;
  }

  /* qmi_dpm_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".qmi_dpm_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.dpm_enabled = (prop_val_int ? TRUE : FALSE);


  /* qos_via_idl */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".qos_via_idl");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    netmgr_main_cfg.qos_via_idl = FALSE;
  }
  else
  {
    netmgr_main_cfg.qos_via_idl = (prop_val_int ? TRUE : FALSE);
  }

  /* use_qmuxd */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".use_qmuxd");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d] Default[%d]", prop_name, ret, TRUE);
    prop_val_int = TRUE;
  }
  netmgr_main_cfg.use_qmuxd = (prop_val_int ? TRUE : FALSE);

  /* dpm_retry_timeout value */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dpm_retry_timeout");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    prop_val_int = 10000;
    netmgr_log_err("Error reading property [%s] Err[%d] Default [%d]",
                   prop_name, ret,prop_val_int);
  }
  netmgr_main_cfg.dpm_retry_timeout = prop_val_int;

  /* wda_data_format_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".wda_data_format_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.wda_data_format = (prop_val_int ? TRUE : FALSE);

  /* ep_type */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".ep_type");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]. Default[%d]",
                   prop_name, ret, DS_EP_TYPE_EMBEDDED);
    prop_val_int = DS_EP_TYPE_EMBEDDED;
  }
  netmgr_main_cfg.ep_type = prop_val_int;

  /* single_qmux_ch_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".single_qmux_ch_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.single_qmux_ch_enabled = (prop_val_int ? TRUE : FALSE);

  /* single_qmux_ch_conn_id_str */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".single_qmux_ch_conn_id_str");
  memset(prop_val_str, 0, sizeof(prop_val_str));
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, prop_val_str, sizeof(prop_val_str));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  strlcpy(netmgr_main_cfg.qmux_ch_name, prop_val_str, sizeof(netmgr_main_cfg.qmux_ch_name));

  /* single_qmux_ch_name */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".single_qmux_ch_name");
  memset(prop_val_str, 0, sizeof(prop_val_str));
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, prop_val_str, sizeof(prop_val_str));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  strlcpy(netmgr_main_cfg.smd_ch_name, prop_val_str, sizeof(netmgr_main_cfg.smd_ch_name));

  /* rmnet_data_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".rmnet_data_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.rmnet_data_enabled = (prop_val_int ? TRUE : FALSE);

  /* dataformat_agg_dl_pkt */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_agg_dl_pkt");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.data_format.dl_agg_cnt = (int)prop_val_int;

  /* dataformat_agg_dl_size */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_agg_dl_size");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.data_format.dl_agg_size = (int)prop_val_int;

  /* dataformat_agg_ul_pkt */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_agg_ul_pkt");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Warning: Error reading property [%s] Err[%d]; setting to 0", prop_name, ret);
    netmgr_main_cfg.data_format.ul_agg_cnt = 0;
  }
  netmgr_main_cfg.data_format.ul_agg_cnt = prop_val_int;

  /* dataformat_agg_ul_size */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_agg_ul_size");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Warning: reading property [%s] Err[%d]; setting to 0", prop_name, ret);
    netmgr_main_cfg.data_format.ul_agg_size = 0;
  }
  netmgr_main_cfg.data_format.ul_agg_size = prop_val_int;

  /* dataformat_dl_data_aggregation_protocol */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_dl_data_aggregation_protocol");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.data_format.dl_data_aggregation_protocol = (int)prop_val_int;

  /* dataformat_ul_data_aggregation_protocol */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_ul_data_aggregation_protocol");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.data_format.ul_data_aggregation_protocol = (int)prop_val_int;

  /* dataformat_dl_gro_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_dl_gro_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.data_format.dl_gro_enabled = (int)prop_val_int;

  /* dataformat_ul_gso_enabled */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dataformat_ul_gso_enabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.data_format.ul_gso_enabled = (int)prop_val_int;

  /* phys_net_dev */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".phys_net_dev");
  memset(prop_val_str, 0, sizeof(prop_val_str));
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, prop_val_str, sizeof(prop_val_str));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }

  strlcpy(netmgr_main_cfg.phys_net_dev, prop_val_str, sizeof(netmgr_main_cfg.phys_net_dev));

  /* pnd_rps_mask */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".pnd_rps_mask");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.pnd_rps_mask = (int)prop_val_int;

  /* vnd_rps_mask */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".vnd_rps_mask");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.vnd_rps_mask = (int)prop_val_int;

  /* clat_rps_mask */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".clat_rps_mask");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.clat_rps_mask = (int)prop_val_int;

  /* netdev_budget */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".netdev_budget");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.netdev_budget = (int)prop_val_int;

#ifdef NETMGR_OFFTARGET
  netmgr_main_cfg.low_latency_filters = FALSE;
#else
  /* low_latency_filters */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".low_latency_filters");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.low_latency_filters = (prop_val_int ? TRUE : FALSE);
#endif

  /* ibfc */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".ibfc");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.ibfc = (int)prop_val_int;

  /* qos_disabled; overrides the android property */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".qos_disabled");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.qos_disabled = (int)prop_val_int;

  /* tc_ul_baserate */

  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".tc_ul_baserate");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Error reading property [%s] Err[%d]; setting to 0", prop_name, ret);
    netmgr_main_cfg.tc_ul_baserate = 0;
  }
  else
  {
    /* TODO: configdb does not have a UL type; we are limited to ~2Gbps */
    netmgr_main_cfg.tc_ul_baserate = (unsigned long)prop_val_int;
  }

  /* dynamic_tc_ul_baserate */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".dynamic_tc_ul_baserate");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.dynamic_tc_ul_baserate = (int)prop_val_int;

  /* tc_ul_burst */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".tc_ul_burst");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_med("Error reading property [%s] Err[%d]; setting to 0", prop_name, ret);
    netmgr_main_cfg.tc_ul_burst = 0;
  }
  else
  {
    netmgr_main_cfg.tc_ul_burst = (unsigned long)prop_val_int;
  }

  /* netdev_max_backlog */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".netdev_max_backlog");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.netdev_max_backlog = (int)prop_val_int;

  /* disable_tcp_hystart_detect */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".disable_tcp_hystart_detect");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.disable_tcp_hystart_detect = (int)prop_val_int;

  /* disable_hystart */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".disable_hystart");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.disable_hystart = (int)prop_val_int;

  /* initial_ssthresh */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".initial_ssthresh");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to -1", prop_name, ret);
    prop_val_int = -1;
  }
  netmgr_main_cfg.initial_ssthresh = (int)prop_val_int;

  /* frag_low_mem */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".frag_low_mem");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.frag_low_mem = (int)prop_val_int;

  /* frag_high_mem */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".frag_high_mem");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.frag_high_mem = (int)prop_val_int;

  /* tcp_ack_prio */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".tcp_ack_prio");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.tcp_ack_prio = (int)prop_val_int;

  /* skip_buffered_qos_modify */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".skip_buffered_qos_modify");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]; Setting to 0", prop_name, ret);
    prop_val_int = 0;
  }
  netmgr_main_cfg.skip_buffered_qos_modify = (int)prop_val_int;

  /* num_modems and enable status */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".num_modems");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &num_modems, sizeof(num_modems));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  for (i = 0; i < num_modems; i++)
  {
    snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s%d", "netmgr_config.", target, ".modems_enabled.",i);
    ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &prop_val_int, sizeof(prop_val_int));
    if (ret != CFGDB_SUCCESS)
    {
      netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
      result = -1;
      goto bail;
    }
    netmgr_main_cfg.modem_enable[i] = (prop_val_int ? TRUE : FALSE);
  }

  /* Disable all the links first, only enable ones present in config file */
  for (i = 0; i < NETMGR_MAX_LINK; i++)
  {
    netmgr_ctl_port_array[i].enabled = FALSE;
  }

  /* control_ports_len and control_ports */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".control_ports_len");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &control_ports_len, sizeof(control_ports_len));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }

  for (i = 0; i < control_ports_len; i++)
  {
    snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s%d", "netmgr_config.", target, ".control_ports.",i);
    memset(prop_val_str, 0, sizeof(prop_val_str));
    ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, &prop_val_str, sizeof(prop_val_str));
    if (ret != CFGDB_SUCCESS)
    {
      netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
      result = -1;
      goto bail;
    }
    strlcpy(netmgr_ctl_port_array[i].qmi_conn_id, prop_val_str, sizeof(netmgr_ctl_port_array[i].qmi_conn_id));
    netmgr_ctl_port_array[i].enabled = TRUE;
  }

  /* data_ports_len and data_ports */
  snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s", "netmgr_config.", target, ".data_ports_len");
  ret = configdb_get_parameter(prop_name, CFGDB_TYPE_INT, &data_ports_len, sizeof(data_ports_len));
  if (ret != CFGDB_SUCCESS)
  {
    netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
    result = -1;
    goto bail;
  }
  netmgr_main_cfg.num_data_ports = data_ports_len;

  for (i = 0; i < data_ports_len; i++)
  {
    snprintf(prop_name, NETMGR_CFGDB_STRVAL_MAX, "%s%s%s%d", "netmgr_config.", target, ".data_ports.",i);
    memset(prop_val_str, 0, sizeof(prop_val_str));
    ret = configdb_get_parameter(prop_name, CFGDB_TYPE_STRING, &prop_val_str, sizeof(prop_val_str));
    if (ret != CFGDB_SUCCESS)
    {
      netmgr_log_err("Error reading property [%s] Err[%d]", prop_name, ret);
      result = -1;
      goto bail;
    }
    strlcpy(netmgr_ctl_port_array[i].data_ctl_port, prop_val_str, sizeof(netmgr_ctl_port_array[i].data_ctl_port));
    netmgr_ctl_port_array[i].enabled = TRUE;
  }

#ifdef FEATURE_DATA_IWLAN
  /* Process iWLAN related configuration items */
  if (0 != netmgr_process_iwlan_config(target))
  {
    netmgr_log_err("%s", "Error processing iWLAN config items");
    result = -1;
    goto bail;
  }
#endif /* FEATURE_DATA_IWLAN */

  netmgr_process_cmd_logging_config();

  /* Remove prefix table as an enhancement later */
  if (netmgr_main_cfg.rmnet_data_enabled)
  {
    if (netmgr_main_cfg.modem_enable[0])
    {
      /* If rmnet_data is enabled, then update the dev-prefix for approriate modem */
      strlcpy(NETMGR_MAIN_GET_DEV_PREFIX(NETMGR_MODEM_MSM),
              NETMGR_MAIN_RMNET_DATA_PREFIX,
              NETMGR_IF_NAME_MAX_LEN);
    }
    else if(netmgr_main_cfg.modem_enable[1])
    {
      /* If rmnet_data is enabled, then update the dev-prefix for approriate modem */
      strlcpy(NETMGR_MAIN_GET_DEV_PREFIX(NETMGR_MODEM_MDM),
              NETMGR_MAIN_RMNET_DATA_PREFIX,
              NETMGR_IF_NAME_MAX_LEN);
    }
  }

bail:
  return result;
}

/*===========================================================================
  FUNCTION:  netmgr_config_print
===========================================================================*/
/*!
    @brief
    This function prints the configuration currently loaded for netmgr.

    @param None.
    @return  None.
*/
/*=========================================================================*/
void netmgr_config_print()
{
  int i;
  netmgr_log_med("==== Netmanager Configuration ====");
  netmgr_log_med("..qmi_dpm_enabled        : [%d]",     netmgr_main_cfg.dpm_enabled);
  netmgr_log_med("..dpm_retry_timeout      : [%d]",     netmgr_main_cfg.dpm_retry_timeout);
  netmgr_log_med("..wda_data_format_enabled: [%d]",     netmgr_main_cfg.wda_data_format);
  netmgr_log_med("..ep_type                : [%d]",     netmgr_main_cfg.ep_type);
  netmgr_log_med("..single_qmux_ch_enabled : [%d]",     netmgr_main_cfg.single_qmux_ch_enabled);
  netmgr_log_med("..single_qmux_ch_name    : [%s]",     netmgr_main_cfg.qmux_ch_name);
  netmgr_log_med("..single_qmux_ch_id      : [%s]",     netmgr_main_cfg.smd_ch_name);
  netmgr_log_med("..rmnet_data_enabled     : [%d]",     netmgr_main_cfg.rmnet_data_enabled);
  netmgr_log_med("..phys_net_dev           : [%s]",     netmgr_main_cfg.phys_net_dev);
  netmgr_log_med("..msm_modem_enabled      : [%d]",     netmgr_main_cfg.modem_enable[NETMGR_MODEM_MSM]);
  netmgr_log_med("..mdm_modem_enabled      : [%d]",     netmgr_main_cfg.modem_enable[NETMGR_MODEM_MDM]);
  netmgr_log_med("..modem_ssr_state        : [%d]",     netmgr_main_cfg.modem_ssr_state);
  netmgr_log_med("..df.dl_agg_size         : [%d]",     netmgr_main_cfg.data_format.dl_agg_size);
  netmgr_log_med("..df.dl_agg_cnt          : [%d]",     netmgr_main_cfg.data_format.dl_agg_cnt);
  netmgr_log_med("..df.dl_agg_mode         : [%d]",     netmgr_main_cfg.data_format.dl_data_aggregation_protocol);
  netmgr_log_med("..df.ul_agg_mode         : [%d]",     netmgr_main_cfg.data_format.ul_data_aggregation_protocol);
  netmgr_log_med("..df.dl_gro_enabled      : [%d]",     netmgr_main_cfg.data_format.dl_gro_enabled);
  netmgr_log_med("..exec_opts.log_options  : [0x%08X]", netmgr_main_cfg.exec_opts.log_options);
  netmgr_log_med("..exec_opts.outfile      : [%s]",     netmgr_main_cfg.exec_opts.outfile);
  netmgr_log_med("..df.ul_gso_enabled      : [%d]", netmgr_main_cfg.data_format.ul_gso_enabled);

  for (i = 0; i < NETMGR_MAX_LINK; i++)
  {
    netmgr_log_med("Link[%d] port[%s] qmi[%s] modem_wait[%d] enabled[%d]",
                    netmgr_ctl_port_array[i].link_id,
                    netmgr_ctl_port_array[i].data_ctl_port,
                    netmgr_ctl_port_array[i].qmi_conn_id,
                    netmgr_ctl_port_array[i].modem_wait,
                    netmgr_ctl_port_array[i].enabled);
  }
  netmgr_log_med("========");


  return;
}

/*===========================================================================
  FUNCTION:  netmgr_config_print_cb
===========================================================================*/
/*!
    @brief
    Wrapper around netmgr_config_print() to make it work with CB framework
*/
/*=========================================================================*/
LOCAL void *
netmgr_config_print_cb()
{
  netmgr_config_print();
  return 0;
}

/*===========================================================================
  FUNCTION:  netmgr_config_init
===========================================================================*/
/*!
    @brief
    Initialize the configration framework. Things that don't belong in actual
    configuration loading.
*/
/*=========================================================================*/
void
netmgr_config_init()
{
  /* Register callbacks with the MAIN table */
  netmgr_cb_insert_cb(NETMGR_MAIN_CB_TABLE,
                      NETMGR_MAIN_CB_SIGUSR1,
                      NETMGR_CONFIG_CB_MODNAME,
                      &netmgr_config_print_cb);
}
