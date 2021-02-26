/*! \file
 *  
 *  \brief  pm_init.c ---- Initializes PMIC 
 *  
 * Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 * QUALCOMM Proprietary.  Export of this technology or software is regulated 
 * by the U.S. Government. Diversion contrary to U.S. law prohibited.  
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.qdsp6/2.1.c4/pmic/pm/framework/src/pm_init.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "pm_prm_init.h"
#include "pm_init.h"
#include "pm_target_information.h"
#include "pm_rpmh.h"
#include "pm_xo_node.h"
#include "pm_ulog.h"

/* 
* Change bPMICStub to TRUE for stubbing 
* pmic driver in RUMI/Virtio environments 
*/
boolean bPMICStub = FALSE;

/*
*  Function to initialize PMIC driver
*/
void
pm_init (void)
{
    pm_ulog_init();
    
    pm_target_information_init();
    
    pm_prm_init();
        
    if (bPMICStub == FALSE)
    {
        pm_rpmh_init();
    }
    
    pm_xo_node_init();
	
    return;
} 

