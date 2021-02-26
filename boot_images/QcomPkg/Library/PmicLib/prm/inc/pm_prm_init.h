#ifndef __PM_PRM_INIT_H__
#define __PM_PRM_INIT_H__

/*! \file
 *  
 *  \brief  pm_prm_init.h ----This file contains prototype definitions prm pam layer
 *    This file contains code for Target specific settings and modes.
 *  
 *    Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 *    QUALCOMM Proprietary.  Export of this technology or software is regulated 
 *    by the U.S. Government. Diversion contrary to U.S. law prohibited.
 *
 */
/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/inc/pm_prm_init.h#1 $ 

===========================================================================*/


/*===========================================================================

FUNCTION pm_prm_init

DESCRIPTION
    This function initializes the prm pam layer resources and nodes.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pm_prm_init(void);

/*===========================================================================

FUNCTION pm_prm_device_init

DESCRIPTION
    This function initializes the prm device resourecs 

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pm_prm_device_init(void);

void 
pm_prm_aux_dev_init(void);

#endif //__PM_PRM_INIT_H__



