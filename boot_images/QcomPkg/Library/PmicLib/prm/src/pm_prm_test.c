/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM NPA TEST SERVICES

GENERAL DESCRIPTION
  This file contains initialization function definition for PMIC NPA tests.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/src/pm_prm_test.c#1 $
$DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/2017  pb      Added pm_prm_get_resource_info
01/19/2017  rl      ported 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "npa.h"
#include "npa_resource.h"
#include "pmapp_npa.h"
#include "pm_target_information.h"
#include "pm_prm_test.h"
#include "pm_prm_driver.h"
#include "pm_log_utils.h"
#include "pm_utils.h"
/*===========================================================================

                GLOBAL VARIABLES

===========================================================================*/

//Following variables are defined as extern inside pm_prm_test.h

char                        *rsrc_test_name = "/pmic/tst_cl/";
pm_prm_node_resource_info   *node_rsrc_arr  = NULL;
uint32                      num_of_test_nodes  = 0;

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

void pm_prm_test_init (void)
{
   uint32 i=0;
   pm_prm_node_resource_info          *node_rsrc = NULL;
   static npa_node_definition         *pm_prm_test_nodes = NULL;
   static npa_resource_definition     *pm_prm_test_rsrc = NULL;
   static boolean                      pm_prm_test_initialized = FALSE;
   char                               *node_test_name = "/node/pmic/tst_cl/";
   uint32                             *node_num_ptr = NULL;
   
   if (pm_prm_test_initialized)
   {
      return;
   }
   
   //Enable Verbose Ulog
   //pm_log_level = PM_LOG_LEVEL_DEBUG;
   
   // Get the PAM info
   node_rsrc_arr = (pm_prm_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRCS );
   node_num_ptr = (uint32*)pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRC_NUM);

   if (node_num_ptr != NULL)
   {
      num_of_test_nodes = *node_num_ptr;
   }

   if(num_of_test_nodes && node_rsrc_arr)
   {
       pm_malloc(sizeof(npa_node_definition)*num_of_test_nodes, (void**)&pm_prm_test_nodes);
       pm_malloc(sizeof(npa_resource_definition)*num_of_test_nodes, (void**)&pm_prm_test_rsrc);

       for(i = 0; i<num_of_test_nodes; i++)
       {
           node_rsrc = &node_rsrc_arr[i];

           /* Construct the PMIC RPM test resources */

           /* Dynamically allocate memory for test resource name and then copy the actual
              resource name to the memory by changing the initial fixed part (to rsrc_test_name) */
           pm_malloc((sizeof(char)*(strlen(node_rsrc->group_name)+1)), (void**)&pm_prm_test_rsrc[i].name);

           /* Change the initial fixed part of the actual resource name by copying the
              rsrc_test_name first to the allocated memory */
               strlcpy((char*)pm_prm_test_rsrc[i].name, rsrc_test_name, (strlen(node_rsrc->group_name)+1));

           /* Then copy the non-fixed part of the actual resource name to the allocated memory starting
              from after the fixed part till the end based on the actual resource name length */
           strlcpy((char*)(pm_prm_test_rsrc[i].name + strlen(rsrc_test_name)), 
                   (node_rsrc->group_name + strlen(rsrc_test_name)), 
                   (strlen(node_rsrc->group_name) - strlen(rsrc_test_name) + 1));

           pm_prm_test_rsrc[i].units = "ModeID";
           pm_prm_test_rsrc[i].max = node_rsrc->max_mode;
           pm_prm_test_rsrc[i].attributes = node_rsrc->resource_attributes;
           if ((pm_prm_test_rsrc[i].attributes & NPA_RESOURCE_SINGLE_CLIENT) == 
               NPA_RESOURCE_SINGLE_CLIENT)
           {
             pm_prm_test_rsrc[i].plugin = &npa_identity_plugin;
           }
           else
           {
             pm_prm_test_rsrc[i].plugin = &npa_max_plugin;
           }
           pm_prm_test_rsrc[i].data = node_rsrc->resource_data;

           /* Construct the PMIC RPM test nodes */

           /* Dynamically allocate memory for test node name and then copy the actual
              node name to the memory by changing the initial fixed part (to node_test_name) */
           pm_malloc((sizeof(char)*(strlen(node_rsrc->node_name)+1)), (void**)&pm_prm_test_nodes[i].name);

           /* Change the initial fixed part of the actual node name by copying the
              node_test_name first to the allocated memory */
               strlcpy((char*)pm_prm_test_nodes[i].name, node_test_name, (strlen(node_rsrc->node_name)+1));

           /* Then copy the non-fixed part of the actual node name to the allocated memory starting
              from after the fixed part till the end based on the actual node name length*/
           strlcpy((char*)(pm_prm_test_nodes[i].name + strlen(node_test_name)), 
                   (node_rsrc->node_name + strlen(node_test_name)), 
                   (strlen(node_rsrc->node_name) - strlen(node_test_name) + 1));

           pm_prm_test_nodes[i].driver_fcn = pm_prm_pam_driver_fcn;
           pm_prm_test_nodes[i].attributes = node_rsrc->node_attributes;
           pm_prm_test_nodes[i].data = node_rsrc->node_data;
           pm_prm_test_nodes[i].dependencies = node_rsrc->node_dependencies;
           pm_prm_test_nodes[i].dependency_count = node_rsrc->dependency_count;
           pm_prm_test_nodes[i].resource_count = 1;
           pm_prm_test_nodes[i].resources = NULL;


       }
   }
   else
   {
       PM_LOG_MSG_ERR_FATAL("PMIC PAM test initialization failed!"); // we should never get here
   }

   for (i = 0; i < num_of_test_nodes; i++)
   {
      pm_prm_test_nodes[i].resources = &pm_prm_test_rsrc[i];
      npa_define_node (pm_prm_test_nodes + i, NULL, NULL);
   }

   pm_prm_test_initialized = TRUE;
   
} /* end of pm_prm_test_init() */

/**
  Duplicates ASCII string 
  
  @param  Source  Null Terminated string of characters 
  @param  Length  source String length  
  @return  Duplicated String  if successful otherwise return NULL
  
  note: Caller responsibility to Free returned String
**/
CHAR8* AsciiStrNDup(
   CONST CHAR8               *Source,
   UINTN                     Length
   )
{
  CHAR8 *Dest = NULL;
  if (NULL == Source || 0 == Length) 
  return NULL;

  Dest = AllocatePool(Length + 1);
  if (NULL == Dest)
  {
    return NULL;
  }

  AsciiStrnCpy(Dest, Source, Length + 1);

  return Dest;
}

int pm_prm_get_resource_info(PMIC_PRM_TEST_RESOURCEINFO *PrmResourceInfo)
{
  //EFI_STATUS  Status           = EFI_SUCCESS;
  if (!PrmResourceInfo)
  {
    //return EFI_INVALID_PARAMETER;
    return 1;
  }

  PrmResourceInfo->num_of_test_nodes = num_of_test_nodes;
  PrmResourceInfo->rsrc_test_name    = AsciiStrNDup(rsrc_test_name, AsciiStrLen(rsrc_test_name) + 1);
  PrmResourceInfo->node_rsrc_arr     = node_rsrc_arr;

  //return Status;
  return 0;

}
