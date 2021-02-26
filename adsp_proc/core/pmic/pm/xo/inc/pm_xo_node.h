#ifndef __PM_XO_NODE_H__
#define __PM_XO_NODE_H__

/*! \file
 *  
 *  \brief  pm_xo_node.h ----This file contains prototypes and definitions for
 *                           for XO node
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

$Header: //components/rel/core.qdsp6/2.1/pmic/pm/xo/inc/pm_xo_node.h#1 $ 

===========================================================================*/

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
#define PMIC_NODE_XO "node/pmic/client/xo"


typedef struct
{   
  /* Resource data */
  const char        *cmd_db_name;

  /* RPMh Data */
  pm_rpmh_data_type  rpmh_data;
}pm_xo_rsrc_data_type;


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_xo_node_init

DESCRIPTION
    This function initializes xo node.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
pm_xo_node_init (void);



#endif //__PM_XO_NODE_H__



