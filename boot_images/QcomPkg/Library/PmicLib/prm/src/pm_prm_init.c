/*! \file
 *  
 *  \brief  pm_prm_init.c ---- Initializes PRM PAM Client Layer 
 *  
 * Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 * QUALCOMM Proprietary.  Export of this technology or software is regulated 
 * by the U.S. Government. Diversion contrary to U.S. law prohibited.  
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/src/pm_prm_init.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "comdef.h"
#include <err.h>
#include "npa.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "npa_log.h"
#include "pmapp_npa.h"
#include "pm_prm_driver.h"
#include "pm_prm_init.h"
#include "pm_prm_device.h"
#include "pm_target_information.h"
#include "pm_log_utils.h"
#include "pm_utils.h"
#include "DDIPlatformInfo.h"
#include "CoreVerify.h"
#include "cmd_db.h"
#include "pm_ldo_driver.h"
#include "pm_vs_driver.h"
#include "pm_clk_driver.h"
#include "pm_smps_driver.h"
#include "device_info.h"
#include "hw_module_type.h"

/*===========================================================================

                MACRO DEFINITIONS

===========================================================================*/

#define MAX_STR_SIZE       25
#define CMD_DB_STR_SIZE     9
#define PMIC_STR_SIZE       2

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*PAM Layer Data*/
uint32                    *num_of_pam_nodes = 0;
npa_node_definition       *pm_prm_pam_nodes = NULL;
npa_resource_definition   *pm_prm_pam_rsrcs = NULL;

/*Device Layer Data*/
npa_resource_definition   *pm_prm_dev_rsrcs = NULL;
npa_node_definition       *pm_prm_dev_nodes = NULL;
pm_prm_resource_type      *pm_prm_rsrc_data = NULL;
boolean                   pm_prm_dev_log_disable = FALSE;

/*Stub Flag*/
extern boolean            bPMICStub;

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

static void pm_prm_rem_dev_log(char *log_name, struct npa_resource *resource);

static uint32 pm_prm_num_rsrcs(pm_prm_rsrc_data_type *rsrc_data, uint32 i);

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
/* PAM Layer Init */
void
pm_prm_init(void)
{
  uint32 i;
  pm_prm_node_resource_info   *nodeRscArray = NULL;
  pm_prm_node_resource_info   *nodeRsc      = NULL;
  
  /* Device Layer Init */
  pm_prm_device_init();

  /* Get the PAM info */
  nodeRscArray = (pm_prm_node_resource_info *)
    pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRCS);

  num_of_pam_nodes = (uint32 *)
    pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRC_NUM);

  if ((num_of_pam_nodes == 0) || (nodeRscArray == NULL))
  {
    PM_LOG_MSG_ERR_FATAL("Failed to initialize PAM Client Layer !!!");
  }

  pm_malloc(sizeof(npa_node_definition) * (*num_of_pam_nodes),
            (void **)&pm_prm_pam_nodes);

  if (pm_prm_pam_nodes == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Not enough memory to initialize PAM Client nodes !!!");
  }

  pm_malloc(sizeof(npa_resource_definition) * (*num_of_pam_nodes),
            (void **)&pm_prm_pam_rsrcs);

  if (pm_prm_pam_nodes == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Not enough memory to initialize PAM Client Resources !!!");
  }

  /* Construct the PMIC Client Nodes and Resources */
  for (i = 0; i < (*num_of_pam_nodes); i++)
  {
    nodeRsc = &nodeRscArray[i];

    pm_prm_pam_rsrcs[i].name             = nodeRsc->group_name;
    pm_prm_pam_rsrcs[i].units            = "ModeID";
    pm_prm_pam_rsrcs[i].max              = nodeRsc->max_mode;
    pm_prm_pam_rsrcs[i].attributes       = nodeRsc->resource_attributes;
    pm_prm_pam_rsrcs[i].data             = nodeRsc->resource_data;
    pm_prm_pam_rsrcs[i].query_fcn        = pm_prm_query_fcn;

    if (pm_prm_pam_rsrcs[i].attributes & NPA_RESOURCE_SINGLE_CLIENT)
    {
      pm_prm_pam_rsrcs[i].plugin       = &npa_identity_plugin;
    }
    else
    {
      pm_prm_pam_rsrcs[i].plugin       = &npa_max_plugin;
    }

    //Populate Node Data
    pm_prm_pam_nodes[i].name             = nodeRsc->node_name;
    pm_prm_pam_nodes[i].driver_fcn       = pm_prm_pam_driver_fcn;
    pm_prm_pam_nodes[i].attributes       = nodeRsc->node_attributes;
    pm_prm_pam_nodes[i].data             = nodeRsc->node_data;
    pm_prm_pam_nodes[i].dependencies     = nodeRsc->node_dependencies;
    pm_prm_pam_nodes[i].dependency_count = nodeRsc->dependency_count;
    pm_prm_pam_nodes[i].resource_count   = 1;
    pm_prm_pam_nodes[i].resources        = NULL;
  }

  //Register PMIC Client Layer Nodes and Resources with NPA
  for (i = 0; i < (*num_of_pam_nodes); i++)
  {
    pm_prm_pam_nodes[i].resources = &pm_prm_pam_rsrcs[i];
    if (bPMICStub == TRUE)
    {
      npa_stub_resource(pm_prm_pam_rsrcs[i].name);
    }
    else
    {
      npa_define_node(pm_prm_pam_nodes + i, NULL, NULL);
    }
  }


} /* end of pm_prm_init() */

/* Device Layer Init */
void
pm_prm_device_init(void)
{
  uint32                num_of_rsrcs = 0;
  npa_resource          *resource    = NULL;
  npa_query_handle      query_handle = NULL;
  pm_prm_rsrc_data_type *rsrc_data   = NULL;
  uint32                i            = 0;
  uint32                j            = 0;
  uint32                node_num     = 0;

  pm_prm_rsrc_data = (pm_prm_resource_type *)
    pm_target_information_get_specific_info(PM_PROP_PRM_DEVICE_RSRCS);

  if ((pm_prm_rsrc_data == NULL) || (pm_prm_rsrc_data->num_rsrcs <= 0))
  {
    return;
  }

  rsrc_data = pm_prm_rsrc_data->resource_data;

  pm_malloc(pm_prm_rsrc_data->num_rsrcs * sizeof(npa_resource_definition),
            (void **)&pm_prm_dev_rsrcs);

  pm_malloc(pm_prm_rsrc_data->num_rsrc_nodes * sizeof(npa_node_definition),
            (void **)&pm_prm_dev_nodes);

  for (i = 0; i < pm_prm_rsrc_data->num_rsrcs; i += num_of_rsrcs, node_num++)
  {
    if (node_num >= pm_prm_rsrc_data->num_rsrc_nodes) PM_LOG_MSG_ERR_FATAL("Failed PRM Device Init");

    /*Get Number of Resources for Each Node*/
    num_of_rsrcs = pm_prm_num_rsrcs(rsrc_data, i);

    /*Populate Node Data*/
    pm_prm_dev_nodes[node_num].name =  (char *)rsrc_data[i].rsrc_node;

    if (rsrc_data[i].rsrc_key == PM_RSRC_KEY_VEC)
    {
      pm_prm_dev_nodes[node_num].driver_fcn = pm_prm_vector_driver_fcn;
    }
    else
    {
      pm_prm_dev_nodes[node_num].driver_fcn = pm_prm_scalar_driver_fcn;
    }

    pm_prm_dev_nodes[node_num].attributes       = NPA_NODE_DEFAULT;
    pm_prm_dev_nodes[node_num].data             = NULL;
    pm_prm_dev_nodes[node_num].dependency_count = 0;
    pm_prm_dev_nodes[node_num].resources        = &pm_prm_dev_rsrcs[i];
    pm_prm_dev_nodes[node_num].resource_count   = num_of_rsrcs;

    /*Populate Resource Data*/
    for (j = 0; j < num_of_rsrcs; j++)
    {
      pm_prm_dev_rsrcs[i + j].name  = (char *)rsrc_data[i + j].rsrc_name;
      pm_prm_dev_rsrcs[i + j].units = (char *)rsrc_data[i + j].rsrc_units;

      if (rsrc_data[i + j].rsrc_key == PM_RSRC_KEY_VEC)
      {
        pm_prm_dev_rsrcs[i + j].max        = 32;
        pm_prm_dev_rsrcs[i + j].plugin     = &pm_prm_vector_plugin;
        pm_prm_dev_rsrcs[i + j].attributes = (NPA_RESOURCE_VECTOR_STATE |
                                              NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE);
      }
      else
      {
        //Maxes can be adjusted in future based on the key type we have
        pm_prm_dev_rsrcs[i + j].max        = 0xFFFF;
        pm_prm_dev_rsrcs[i + j].plugin     = &npa_max_plugin;
        pm_prm_dev_rsrcs[i + j].attributes = NPA_RESOURCE_DEFAULT | NPA_RESOURCE_DRIVER_UNCONDITIONAL;
      }

      rsrc_data[i + j].rpmh_data.address =
        cmd_db_query_addr(rsrc_data[i + j].cmd_db_name);

      CORE_VERIFY(rsrc_data[i + j].rpmh_data.address);

      pm_prm_dev_rsrcs[i + j].data = (void *)&rsrc_data[i + j];
    }

    if (bPMICStub == TRUE)
    {
      npa_stub_resource(rsrc_data[i].rsrc_name);
    }
    else
    {
      /*Create nodes for each resource*/
      npa_define_node(&pm_prm_dev_nodes[node_num], NULL, NULL);
    }

    if (pm_prm_dev_log_disable)
    {
      for (j = 0; j < num_of_rsrcs; j++)
      {
        query_handle = npa_create_query_handle(pm_prm_dev_rsrcs[i + j].name);

        if (query_handle != NULL)
        {
          resource = npa_query_get_resource(query_handle);
        }

        if (resource != NULL)
        {
          pm_prm_rem_dev_log(NPA_DEFAULT_LOG_NAME, resource);
        }
      }
    } //End Log Disable
  } //End node Population Loop
}

static void
pm_prm_rem_dev_log(char *log_name, struct npa_resource *resource)
{
  if (resource)
  {
    return; //npa_remove_resource_log_by_handle(log_name, resource));
  }
  return;
}



static uint32
pm_prm_num_rsrcs(pm_prm_rsrc_data_type *rsrc_data, uint32 i)
{
  uint32 num_of_rsrcs = 0;

  /* If Vector */
  if (rsrc_data[i].rsrc_key  == PM_RSRC_KEY_VEC)
  {
    num_of_rsrcs  = 1;
    return num_of_rsrcs;
  }

  /* If Scalar */
  if (rsrc_data[i].rsrc_vrm_type == PM_PRM_RSRC_VRM_VREG)
  {
    num_of_rsrcs  = NUM_OF_VREG_KEYS;
  }
  else if (rsrc_data[i].rsrc_vrm_type == PM_PRM_RSRC_VRM_CLK)
  {
    num_of_rsrcs  = NUM_OF_CLK_KEYS;
  }
  else
  {
    CORE_VERIFY(0);
  }

  return num_of_rsrcs;
}

static void 
numtostr(uint8 number, char *arr)
{
  uint8 res       = 0;
  uint8 i         = 0;
  uint8 rev_cnt   = 0;
  char *ptr_front = NULL;
  char *ptr_back  = NULL;
  char temp       = 0;

  if(arr == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Pointer");
  }
  
  do
  {
    res    = number % 10;
    number = number/10;
    arr[i] = res + '0'; //ascii value
    i++;
  }while(number > 0);

  arr[i] = '\0';
  ptr_front = arr;
  ptr_back  = (arr + (i-1));
  rev_cnt = i/2;

  while(rev_cnt)
  {
    temp       = *ptr_front;
    *ptr_front = *ptr_back;
    *ptr_back  = temp;
    ptr_front++;
    ptr_back--;
    rev_cnt--;
  }

  return;
}



static void
pm_prm_create_rsrc_str(char* str_ptr, char* rsrc_name, 
                       char* key_name, uint32 rsrc_number, 
                       char* pmic_name)
{
  char rsrc_num_str[5] = {0, 0, 0, 0, 0};
  
  if(str_ptr == NULL || rsrc_name == NULL || 
     key_name == NULL || pmic_name == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Input");
  }
  
  numtostr(rsrc_number, rsrc_num_str); 
  strlcpy(str_ptr, "/pm/"       ,MAX_STR_SIZE);
  strlcat(str_ptr, rsrc_name    ,MAX_STR_SIZE);
  strlcat(str_ptr, pmic_name    ,MAX_STR_SIZE);
  strlcat(str_ptr, rsrc_num_str ,MAX_STR_SIZE);
  strlcat(str_ptr, "/"          ,MAX_STR_SIZE);
  strlcat(str_ptr, key_name     ,MAX_STR_SIZE);
  
  return; 
}

static void
pm_prm_create_node_str(char* str_ptr, char* rsrc_name, 
                       uint32 rsrc_number, char* pmic_name)
{
  char rsrc_num_str[5] = {0, 0, 0, 0, 0};
  
  if(str_ptr == NULL || rsrc_name == NULL || pmic_name == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Input Pointers");
  }
 
 //"/node/" #rtype #chipid #index
  numtostr(rsrc_number, rsrc_num_str); 
  strlcpy(str_ptr, "/node/",     MAX_STR_SIZE);
  strlcat(str_ptr, rsrc_name,    MAX_STR_SIZE);
  strlcat(str_ptr, pmic_name,    MAX_STR_SIZE);
  strlcat(str_ptr, rsrc_num_str, MAX_STR_SIZE);
  
  return; 
}

void
pm_prm_create_cmddb_str(char* str_ptr, char* rsrc_name, 
                        uint32 rsrc_number, char *pmic_name)
{
  char rsrc_num_str[5] = {0, 0, 0, 0, 0};
  
  if(str_ptr == NULL || rsrc_name == NULL || pmic_name == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Input Pointers");
  }
 
  numtostr(rsrc_number, rsrc_num_str); 
  strlcpy(str_ptr, rsrc_name,    CMD_DB_STR_SIZE);
  strlcat(str_ptr, pmic_name,    CMD_DB_STR_SIZE);
  strlcat(str_ptr, rsrc_num_str, CMD_DB_STR_SIZE);
  
  return; 
}


static void
pm_prm_aux_populate_rsrc_data(char                   *rsrc_type_name, 
                              char                   *pmic_name,
                              char                   *cmd_db_name,
                              uint32                 rsrc_num,
                              pm_prm_rsrc_key_type   rsrc_key, 
                              pm_prm_rsrc_data_type  *rsrc_data,
                              pm_prm_rsrc_type       rsrc_type)
{
  char   *rsrc_name = NULL; 
  char   *rsrc_node = NULL;
  uint32 vrm_addr   = 0;
  
  if(rsrc_type_name == NULL || pmic_name == NULL || 
     cmd_db_name    == NULL || rsrc_key  >= PM_RSRC_KEY_INVALID || 
     rsrc_data      == NULL || rsrc_type >= PM_PRM_RSRC_INVALID)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Inputs");
  }
  
  pm_malloc(sizeof(char) * (MAX_STR_SIZE), (void**)&rsrc_name);
  pm_malloc(sizeof(char) * (MAX_STR_SIZE), (void**)&rsrc_node);
  
  if(rsrc_name == NULL || rsrc_node == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Out of Memory");
    return;
  }

  pm_prm_create_node_str(rsrc_node, rsrc_type_name, rsrc_num, pmic_name);
  
  vrm_addr = cmd_db_query_addr(cmd_db_name);
  
  if(vrm_addr == 0)
  {
    PM_LOG_MSG_ERR_FATAL("VRM Address returned 0");
  }
  
  switch(rsrc_key)
  {
    case PM_RSRC_KEY_VOLTAGE:
      pm_prm_create_rsrc_str(rsrc_name, rsrc_type_name, "mV", rsrc_num, pmic_name);                             
      rsrc_data->rsrc_units       = "mV";
      rsrc_data->rsrc_key         = PM_RSRC_KEY_VOLTAGE;
      rsrc_data->rpmh_data.offset = PM_VREG_VOLTAGE_OFFSET;
      break;
      
    case PM_RSRC_KEY_ENABLE:
      pm_prm_create_rsrc_str(rsrc_name, rsrc_type_name, "en", rsrc_num, pmic_name);
      rsrc_data->rsrc_units       = "on_off";
      rsrc_data->rsrc_key         = PM_RSRC_KEY_ENABLE;     
      rsrc_data->rpmh_data.offset = 
                       (rsrc_type == PM_PRM_RSRC_CLK ? PM_CLK_ENABLE_OFFSET : 
                                                      PM_VREG_ENABLE_OFFSET);
      break;
      
    case PM_RSRC_KEY_HEADROOM:
      pm_prm_create_rsrc_str(rsrc_name, rsrc_type_name, "hr", rsrc_num, pmic_name);
      rsrc_data->rsrc_units       = "mV";
      rsrc_data->rsrc_key         = PM_RSRC_KEY_HEADROOM;
      rsrc_data->rpmh_data.offset = PM_VREG_HEADROOM_OFFSET;
      break;
      
    case PM_RSRC_KEY_MODE:
      pm_prm_create_rsrc_str(rsrc_name, rsrc_type_name, "mode", rsrc_num, pmic_name);
      rsrc_data->rsrc_units       = "mode";
      rsrc_data->rsrc_key         = PM_RSRC_KEY_MODE;
      rsrc_data->rpmh_data.offset = PM_VREG_MODE_OFFSET;
      break;
      
    default:
      PM_LOG_MSG_ERR_FATAL("Invalid Rsrc Key");
      break;
  }
  
  rsrc_data->rsrc_name              = rsrc_name;
  rsrc_data->rsrc_node              = rsrc_node;
  rsrc_data->rsrc_type              = rsrc_type;
  rsrc_data->rsrc_vrm_type          = (rsrc_type == PM_PRM_RSRC_CLK ? 
                                      PM_PRM_RSRC_VRM_CLK : 
                                      PM_PRM_RSRC_VRM_VREG); 
  rsrc_data->cmd_db_name            = cmd_db_name;
  rsrc_data->pmic_chip              = pmic_name; 
  rsrc_data->ctrl_type              = PM_PRM_RPMH;
  rsrc_data->periph_id              = rsrc_num - 1; //since 0 based
  rsrc_data->pam_req                = 0; 
  rsrc_data->rpmh_data.address      = vrm_addr;
  rsrc_data->rpmh_data.dirty_active = 0; 
  rsrc_data->rpmh_data.dirty_sleep  = 0;
  
  return;
}



static void
pm_prm_aux_dev_create(char             *rsrc_type_str, 
                      uint32           num_rsrcs, 
                      uint8            pmic_chip, 
                      pm_prm_rsrc_type rsrc_type)
{
  char                     *en_key_name   = NULL;
  char                     *cmd_db_name   = NULL; 
  npa_query_handle         query_handle   = NULL;
  pm_prm_rsrc_data_type    *rsrc_data     = NULL;
  char                     *pmic_name     = NULL;
  uint32                   vrm_addr       = 0;
  npa_node_definition      *npa_node      = NULL;
  npa_resource_definition  *npa_rsrcs     = NULL; 
  uint8                    j              = 0;
  uint32                   num_of_keys    = 0;

  pm_malloc(sizeof(char)*(PMIC_STR_SIZE), (void**)&pmic_name);
  if(pmic_name == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Falied to fetch memory for pmic chip");
  }
  
  pmic_name[0] = 'a' + (char)pmic_chip;
  pmic_name[1] = '\0';
  
  while(num_rsrcs)
  {
    //free the memory for test en key at end of while loop 
    pm_malloc(sizeof(char)*(MAX_STR_SIZE), (void**)&en_key_name);
    pm_malloc(sizeof(char)*(CMD_DB_STR_SIZE),  (void**)&cmd_db_name);
    
    if(en_key_name == NULL || cmd_db_name == NULL)
    {
      PM_LOG_MSG_ERR_FATAL("Out of Memory");
    }
    
    /*Check if en resource is created, if its there then all resources shouold be available */
    pm_prm_create_rsrc_str(en_key_name, rsrc_type_str, "en", num_rsrcs, pmic_name);

    pm_prm_create_cmddb_str(cmd_db_name,  rsrc_type_str, num_rsrcs, pmic_name); 
    
    vrm_addr = cmd_db_query_addr(cmd_db_name);

  
    query_handle = npa_create_query_handle(en_key_name); //destry this at the end clean up 
    
    if(query_handle == NULL && vrm_addr != 0)
    {      
  
      num_of_keys = (rsrc_type == PM_PRM_RSRC_CLK ? 
                                 NUM_OF_CLK_KEYS : 
                                 NUM_OF_VREG_KEYS);
      
      pm_malloc((num_of_keys * sizeof(pm_prm_rsrc_data_type)), 
                (void**)&rsrc_data);
                
      if(rsrc_data == NULL)
      {
        PM_LOG_MSG_ERR_FATAL("Failed to fetch memory for resources");
      }
      
      pm_prm_aux_populate_rsrc_data(rsrc_type_str, pmic_name, cmd_db_name, 
                                    num_rsrcs, PM_RSRC_KEY_ENABLE, 
                                    &rsrc_data[0], rsrc_type);
      
      if(rsrc_type != PM_PRM_RSRC_CLK)
      {
        pm_prm_aux_populate_rsrc_data(rsrc_type_str, pmic_name, cmd_db_name,
                                      num_rsrcs, PM_RSRC_KEY_VOLTAGE, 
                                      &rsrc_data[1], rsrc_type);
        
        pm_prm_aux_populate_rsrc_data(rsrc_type_str, pmic_name, cmd_db_name, 
                                      num_rsrcs, PM_RSRC_KEY_HEADROOM, 
                                      &rsrc_data[2], rsrc_type);
        
        pm_prm_aux_populate_rsrc_data(rsrc_type_str, pmic_name, cmd_db_name, 
                                    num_rsrcs, PM_RSRC_KEY_MODE, 
                                    &rsrc_data[3], rsrc_type);
      }  
      
      pm_malloc(sizeof(npa_node_definition), (void**)&npa_node);
          
      if(npa_node == NULL)
      {
        PM_LOG_MSG_ERR_FATAL("Not enough memory to initialize npa nodes !!!");    
      }
  
      pm_malloc(sizeof(npa_resource_definition) * num_of_keys, (void**)&npa_rsrcs);
      
      if(npa_rsrcs == NULL)
      {
        PM_LOG_MSG_ERR_FATAL("Not enough memory to initialize npa rsrc !!!");    
      }
      
      /*Populate Node Data*/
      npa_node->name             = (char*)rsrc_data[0].rsrc_node;
      npa_node->driver_fcn       = pm_prm_scalar_driver_fcn;
      npa_node->attributes       = NPA_NODE_DEFAULT;
      npa_node->data             = NULL;
      npa_node->dependency_count = 0;
      npa_node->resources        = npa_rsrcs;
      npa_node->resource_count   = num_of_keys;
      
       /*Populate Resource Data*/
      for(j = 0; j < num_of_keys; j++)
      {
        npa_rsrcs[j].name       = (char*)rsrc_data[j].rsrc_name;
        npa_rsrcs[j].units      = (char*)rsrc_data[j].rsrc_units;    
        npa_rsrcs[j].max        = 0xFFFF;
        npa_rsrcs[j].plugin     = &npa_max_plugin;                                         
        npa_rsrcs[j].attributes = NPA_RESOURCE_DEFAULT | NPA_RESOURCE_DRIVER_UNCONDITIONAL;
        
        rsrc_data[j].rpmh_data.address = 
                             cmd_db_query_addr(rsrc_data[j].cmd_db_name);
                             
        CORE_VERIFY(rsrc_data[j].rpmh_data.address);
        
        npa_rsrcs[j].data = (void*)&rsrc_data[j];
      }
      
      if (bPMICStub == TRUE)
      {
        npa_stub_resource(rsrc_data[j].rsrc_name);
      }
      else
      {
          /*Create nodes for each resource*/
        npa_define_node (npa_node, NULL, NULL);
      }
    }
    
    if(query_handle != NULL)
    {
      npa_destroy_query_handle(query_handle);
    }
    
    /*Clean up local variables for next rsrc*/
    en_key_name  = NULL;
    cmd_db_name  = NULL; 
    query_handle = NULL;
    rsrc_data    = NULL;
    vrm_addr     = 0;
    npa_node     = NULL;
    npa_rsrcs    = NULL; 
    j            = 0;
    num_rsrcs--;    
  }
  return;
}


void 
pm_prm_aux_dev_init(void)
{
  pm_ldo_data_type  *ldo_data  = NULL;
  pm_smps_data_type *smps_data = NULL;
  pm_vs_data_type   *vs_data   = NULL;
  pm_clk_data_type  *clk_data  = NULL;
  uint8             i          = 0; 
  uint8             j          = 0;
  uint32            num_rsrcs  = 0;
  uint32            num_of_clk = 0;
  uint8             num_of_rfclk =0, num_of_lnbbclk = 0, num_of_divclk = 0;

  for(i=0; i<PM_MAX_NUM_PMICS; i++)
  {
    ldo_data = pm_ldo_get_data(i);
    
    if (ldo_data != NULL)
    {
      num_rsrcs = ldo_data->pm_pwr_data.num_of_peripherals;
      pm_prm_aux_dev_create("ldo", num_rsrcs, i, PM_PRM_RSRC_LDO);
      num_rsrcs = 0;
    }
  }//end of ldo
  
  for(i=0; i<PM_MAX_NUM_PMICS; i++)
  {
    smps_data = pm_smps_get_data(i);
    
    if (smps_data != NULL)
    {
      num_rsrcs = smps_data->pm_pwr_data.num_of_peripherals;
      pm_prm_aux_dev_create("smp", num_rsrcs, i, PM_PRM_RSRC_SMPS);
      num_rsrcs = 0;
    }
  }//end of smps
  
  for(i=0; i<PM_MAX_NUM_PMICS; i++)
  {
    vs_data = pm_vs_get_data(i);
    
    if (vs_data != NULL)
    {
      num_rsrcs = vs_data->pm_pwr_data.num_of_peripherals;
      pm_prm_aux_dev_create("vs", num_rsrcs, i, PM_PRM_RSRC_VS);
      num_rsrcs = 0;
    }
  }//end of vs 
  
  for(i=0; i<PM_MAX_NUM_PMICS; i++)
  {
    clk_data = pm_clk_get_data(i);
    if(clk_data != NULL)
    {
      num_rsrcs = clk_data->num_of_peripherals;
      num_of_clk = sizeof(clk_data->periph_subtype)/sizeof(clk_data->periph_subtype[0]);
      for(j = 0; j < num_of_clk; j++)
      {
        switch(clk_data->periph_subtype[j])
        {
          case PM_HW_MODULE_CLOCK_RF_CLK:
            num_of_rfclk += 1;
            break;
                    
          case PM_HW_MODULE_CLOCK_LNBB_CLK:
            num_of_lnbbclk += 1;
            break;
          
          case PM_HW_MODULE_CLOCK_DIV_CLK:
            num_of_divclk +=1; 
            break;
          default:
            break;
        }
      }
      pm_prm_aux_dev_create("rfclk",  num_of_rfclk,   i, PM_PRM_RSRC_CLK);
      pm_prm_aux_dev_create("lnbclk", num_of_lnbbclk, i, PM_PRM_RSRC_CLK);
      pm_prm_aux_dev_create("divclk", num_of_divclk,  i, PM_PRM_RSRC_CLK);
      num_of_rfclk = 0;
      num_of_lnbbclk = 0;
      num_of_divclk = 0;
    }
  }//end of clks
  return;
}


