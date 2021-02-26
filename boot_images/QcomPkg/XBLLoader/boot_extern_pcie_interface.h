#ifndef BOOT_EXTERN_PCIE_INTERFACE_H
#define BOOT_EXTERN_PCIE_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN PCIe DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pcie drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
09/23/14   ck      Removed feature flag as API is always enabled in XBL
03/25/14   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_comdef.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/* ============================================================================
**  Function : boot_pcie_reg_dump
** ============================================================================
*/
/**
  Calls the PCIe core dump api.
  
  @param[in]     dump_addr  Address where the registers are dumped . 

  @return
  void.
  
  @dependencies
  None.
  
  
*/
void boot_pcie_reg_dump(void * dump_addr,
                        uint32 max_size);

#endif /* BOOT_EXTERN_PCIE_INTERFACE_H */
