#ifndef QURT_NON_SYSTEM_H
#define QURT_NON_SYSTEM_H 

/**
  @file  qurt_system.h 
  @brief Contains misc system calls

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2012-04-19 	nt			created
   2012-12-17   cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
#include "qurt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        FUNCTIONS
=============================================================================*/

  /**@ingroup func_qurt_system_jump
    Performs a jump from the current physical address to the address specified.
    This function is useful for moving execution from the current image to another image.
   
    @param addr  Address where execution will resume.

    @return
    None.

    @dependencies
    None.
   
   */ 
void qurt_system_jump (qurt_addr_t addr);

#endif /* QURT_SYSTEM_H */
