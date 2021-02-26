/*===========================================================================
GENERAL DESCRIPTION
  This file is just to give an entry point to the pmic elf image

 &copy; Copyright 2013-2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Change: 18535996 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/02/16     yps     Avoid infinite-recursion error by the new compiler
09/01/15     aab     Updated driver to support SBL Conditional configuration feature
11/10/14     aab      Initialized pm_psi_header.signature
06/06/14     akm     Created for psii elf image creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "com_dtypes.h"
#include "pm_config_sbl.h"
/*===========================================================================

                           DEFINITIONS

===========================================================================*/
/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
extern void psi_init(void);

/*===========================================================================

DESCRIPTION
  This function inits ZI and loops

DEPENDENCIES
   None

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern pm_sbl_pdm_config_type* pm_psi_header;

#pragma clang diagnostic ignored "-Winfinite-recursion"

void main_c(void)
{
  pm_psi_header->pm_psi_info->signature = PSI_SIGNATURE;
  main_c();
} /* main_c */

void psi_init(void)
{
  psi_init();
}