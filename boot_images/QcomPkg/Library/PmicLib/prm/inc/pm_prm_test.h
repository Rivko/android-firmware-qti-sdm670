#ifndef __PM_PRM_TEST_H__
#define __PM_PRM_TEST_H__
/*===========================================================================

                  PM NPA TEST HEADER FILE

DESCRIPTION
  This file contains prototype definition for PMIC NPA test initialization.

Copyright (c) 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/inc/pm_prm_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/17   pb      Added pm_prm_get_resource_info
1/17/17    rl      ported
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

//#include <Library/QcomLib.h>
#include "CoreVerify.h"
#include <Library/npa_resource.h>
#include <Library/MemoryAllocationLib.h>
#include "pm_prm_driver.h"

/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/
/*PAM RESOURCEINFO */
typedef struct PMIC_PRM_TEST_RESOURCEINFO{
  uint32 num_of_test_nodes;
  char   *rsrc_test_name;
  pm_prm_node_resource_info *node_rsrc_arr;
  //void   *node_rsrc_arr;
}PMIC_PRM_TEST_RESOURCEINFO;

extern char *rsrc_test_name;
extern uint32 num_of_test_nodes; //Need this for APT test if using test client
extern pm_prm_node_resource_info *node_rsrc_arr; //Need this for APT test if using test client

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
/** 
 * @name pm_prm_test_init 
 *  
 * @brief This initialization function is called to test
 *        PMIC PRM PAM clients.This function creates the PMIC
 *        PRM test resources using the existing PMIC PRM
 *        client's static data but with a slightly
 *        different resource name or group id (using which
 *        the NPA request is usually made).
 * 
 *        For example, resource name for UIM1_ENABLE client
 *        is "/pmic/client/uim1_enable" (Refer to pmapp_npa.h
 *        public header) but the corresponding test client
 *        resource name would be "/pmic/tst_cl/uim1_enable"
 *        using which the NPA test request has to be made.
 * 
 * @param None
 *
 * @return None
 *
 */
void pm_prm_test_init(void);

//EFI_STATUS pm_prm_get_resource_info(PMIC_PRM_TEST_RESOURCEINFO *PrmResourceInfo);
int pm_prm_get_resource_info(PMIC_PRM_TEST_RESOURCEINFO *PrmResourceInfo);

#endif /* __PM_PRM_TEST_H__ */
