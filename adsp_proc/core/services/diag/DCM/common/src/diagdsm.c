/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D I A G D S M . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  diagdsm_init()
  diagdsm_deinit()
  diagdsm_delete()
    Initialize the Data Services Memory pool unit.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2005-2014,2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagdsm.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/12/16   ph      SMD references removal and code clean up.
04/08/15   sr      Dsm removal 
07/08/15   xy      Fixed buffering mode issue
04/10/14   sr      Support for peripheral buffering mode  
09/27/13   xy      Removed code under FEATURE_WINCE  
02/11/13   sr      Mainling Diag central routing  
02/05/13   rh      Added RX/TX Control Channel Pool 
01/03/13   sg      Command Response Channel separation 
03/10/11   is      Move DSM_ALIGNED_ALLOC() from DSM to Diag
01/18/10   JV      Do not initilaize the 2 SMD pools for single processor targets.
10/06/09   JV      Mainlined FEATURE_DSM_DIAG_ITEMS.
09/23/09   JV      Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/24/09    mad    Corrected comment
06/22/09    mad    Featurized inclusion of some diag header files
04/03/09    mad    Moved DiagDSM item sizes etc to a new header file diagdsmi.h
03/26/09    mad    Re-enabled target-specific definitions of DIAG DSM item 
                   SIZ/CNT
03/24/09    mad    Correction in DSM pool macro definitions:
                   enclosed in brackets
03/16/09    mad    Changes for WM7: prevent DSM pool initialization if 
                   already initialized. Added diagdsm_deinit()
03/06/09    mad    Redefined DIAG DSM item sizes/counts.Upper-level 
                   target-specific definitions for these are now over-ridden
                   in this file.
03/05/09    vg     Corrected the DSM pools (SMD_RX_ITEM & SMD_TX_ITEM)  
01/13/09    is     Removed redundant memory allocation due to use of 
                   deprecated DSM API, dsm_init_pool()
05/01/08    as     Fixed featurization on DSM pools for modem only builds.
10/18/07    pj     Changed code to align pools on 32-byte boundary.
09/25/07    ms     Merging functions from diagdsm_wce.c into this file.
09/24/07    ms     diagdsm_init() should return false if allocation fails. 
                   Incorporating RB's changes for LG into mainline.
06/20/07    pj     changes for DSM as a DLL.
01/22/06    as     Featurized code to make DSM count & size defined by target
12/05/06    as     Increased DSM SIO and SMD TX item size.
11/21/06    as     Modified code to use diag internal features.
09/13/06    as     Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"

#include "customer.h"
#include "queue.h"

#include "dsm.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*Just definition in place since it is referenced by UART 
  in ..\core\buses\uart\mdd\src\siors232_mdd.c */
  
#define DSMI_DIAG_SIO_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_RX_ITEM_CNT * \
   (DSMI_DIAG_SIO_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_rx_item_pool;


