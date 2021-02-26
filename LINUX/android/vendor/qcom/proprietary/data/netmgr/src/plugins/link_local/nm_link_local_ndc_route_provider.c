/******************************************************************************

     N M _ L I N K _ L O C A L _ N D C _ R O U T E _ P R O V I D E R. C

******************************************************************************/

/******************************************************************************

  @file    nm_link_local_nm_link_local_ndc_prov.c
  @brief   Link-local routing plugin

  DESCRIPTION
  Manage the routing rules required for link-local data calls

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>

#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_kif.h"

#include "nm_link_local_route_provider_factory.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define NM_PROVIDER_CUSTOM_LL_OEM_NETWORK "oem10"
#define NETMGR_MAX_RETRY 15
#define NETMGR_BASE_DELAY 50000

static boolean custom_oem_network_created = FALSE;

static int
nm_link_local_ndc_prov_create_network(boolean force);

static int
nm_link_local_ndc_prov_add_route(int link, char* link_name);

static int
nm_link_local_ndc_prov_remove_route(int link, char* link_name);

nm_ll_route_provider_t ll_nm_link_local_ndc_prov =
{
  /* Initialize the function table for
   * the ndc route provider */
  .init         = nm_link_local_ndc_prov_create_network,
  .add_route    = nm_link_local_ndc_prov_add_route,
  .remove_route = nm_link_local_ndc_prov_remove_route
};

/*===========================================================================
  FUNCTION  nm_link_local_ndc_prov_exec_cmd
===========================================================================*/
/*!
@brief
  Helper function to execute ndc commands

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
nm_link_local_ndc_prov_exec_cmd(char* cmd)
{
  int rc = NETMGR_FAILURE;
  char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";
  DS_SYSCALL_DEFAULTS(opts);

  NETMGR_LOG_FUNC_ENTRY;

  if (!cmd)
  {
    netmgr_log_err("%s(): Invalid cmd!\n", __func__);
    goto bail;
  }

  /* ndc commands log everything to stdout even when there is an error
     We need to pass in a valid stdout buffer for the return message
     to be captured so that we can parse the output */
  opts.stdout = err_str;
  opts.stdout_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  rc = netmgr_ds_exec(cmd, &opts);

  if (DS_EXEC_OK != rc)
  {
    netmgr_log_err("%s(): cmd: %s failed!", __func__, cmd);
    goto bail;
  }

  if (strlen(err_str) != 0)
  {
    /* ndc commands print the string 200 0 success on successful
     * execution of the command. We need so search
     * for this keyword to check if the command succeeded.
       If NetD restart handler  and Link local call come up
       simultaneously, one of them will receive error of 400 0
      (File exists) which we need to check against as well*/
    netmgr_log_err("%s() Error string %s \n ", __func__, err_str);
    if ((NULL == strcasestr(err_str, "200 0 success")) && (NULL == strcasestr(err_str, "File exists")))
    {
      netmgr_log_err("%s(): cmd: %s failed! Err: %s", __func__, cmd, err_str);
      rc = NETMGR_FAILURE;
      goto bail;
    }
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_link_local_ndc_prov_create_network
===========================================================================*/
/*!
@brief
  Creates network to manage custom routing rules

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
nm_link_local_ndc_prov_create_network(boolean force)
{
  char ndc_create_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  int rc = NETMGR_FAILURE, retry = 0, ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (force)
  {
    /* 'force' will be set when netd has restarted and we
     need to recreate the oem10 table. We will also piggyback on
     this flag even during bootup initialization */
    custom_oem_network_created = FALSE;
  }

  if (!custom_oem_network_created)
  {
    snprintf(ndc_create_cmd,
             sizeof(ndc_create_cmd),
             "ndc network create %s",
             NM_PROVIDER_CUSTOM_LL_OEM_NETWORK);

    do
    {

      if(retry != 0)
      {
        usleep(NETMGR_BASE_DELAY);
        netmgr_log_med("%s(): Failed to create Link local network ; waiting and retrying [%d]",
                        __func__, retry);
      }

      ret = nm_link_local_ndc_prov_exec_cmd(ndc_create_cmd);

    }while(ret != NETMGR_SUCCESS
           && retry++ < NETMGR_MAX_RETRY);

    if(ret != NETMGR_SUCCESS){
      netmgr_log_err("%s(): Failed to create custom network!\n",
                     __func__);
      goto bail;
    }

    netmgr_log_med("%s(): Successfully created new custom network!\n",
                    __func__);
    custom_oem_network_created = TRUE;
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_link_local_ndc_prov_add_route
===========================================================================*/
/*!
@brief
  Setup routing rule for the provided link

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
nm_link_local_ndc_prov_add_route(int link, char* link_name)
{
  int rc = NETMGR_FAILURE;
  char ndc_add_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";

  NETMGR_LOG_FUNC_ENTRY;

  if (!link_name)
  {
    netmgr_log_err("%s(): link_name cannot be NULL!", __func__);
    goto bail;
  }

  /* If netd had started after netmgr in the bootup sequence then the initial
     attempt to create the oem10 network might have failed. Attempt to create
     the network again here */
  if (NETMGR_SUCCESS != nm_link_local_ndc_prov_create_network(FALSE))
  {
    netmgr_log_err("%s(): Failed to create custom network!", __func__);
    goto bail;
  }

  /* Add the interface to the network */
  snprintf(ndc_add_cmd,
           sizeof(ndc_add_cmd),
           "ndc network interface add %s %s",
           NM_PROVIDER_CUSTOM_LL_OEM_NETWORK,
           link_name);

  if (NETMGR_SUCCESS != nm_link_local_ndc_prov_exec_cmd(ndc_add_cmd))
  {
    netmgr_log_err("%s(): failed to add link-local iface [%d] to custom network",
                   __func__, link);
    goto bail;
  }

  netmgr_log_med( "%s(): successfully added link-local iface [%d]"
                  " to custom network\n", __func__, link);

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_link_local_ndc_prov_remove_route
===========================================================================*/
/*!
@brief
  Remove routing rule for the provided link

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
nm_link_local_ndc_prov_remove_route(int link, char* link_name)
{
  int rc = NETMGR_FAILURE;
  char ndc_remove_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";

  NETMGR_LOG_FUNC_ENTRY;

  if (!link_name)
  {
    netmgr_log_err("%s(): link_name cannot be NULL!", __func__);
    goto bail;
  }

  snprintf(ndc_remove_cmd,
           sizeof(ndc_remove_cmd),
           "ndc network interface remove %s %s",
           NM_PROVIDER_CUSTOM_LL_OEM_NETWORK,
           link_name);

  if (NETMGR_SUCCESS != nm_link_local_ndc_prov_exec_cmd(ndc_remove_cmd))
  {
    netmgr_log_err("%s(): failed to remove link-local iface [%d] to custom network",
                   __func__, link);
    goto bail;
  }

   netmgr_log_med( "%s(): successfully removed link-local iface [%d]"
                    " from custom network\n",
                    __func__, link);

   rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}
