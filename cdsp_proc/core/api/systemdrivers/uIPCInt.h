#ifndef __uIPCINT_H__
#define __uIPCINT_H__
/*===========================================================================*/

/* M I C R O   I M A G E   I P C   I N T E R R U P T   C O N T R O L L E R   */

/**
  @file uIPCInt.h 
  @brief This file contains the API specification for the Micro Image IPC 
  interrupt controller device driver. The definitions in this module are 
  used externally by the clients of the IPC interrupt controller 
  when they are executing inside the micro image. This interface only
  supports EEs that are within the same power island. 
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
         The above description for this file is part of the "gpioint_api" group 
         description in the mainpage file. 
*/

/*===========================================================================
   Copyright (c) 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/uIPCInt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
6/7/2017   cpaulo  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "DDIIPCInt.h"

/*
 * The error codes are to be kept in syun with the dal error codes.
 */
#define UIPCINT_SUCCESS 0
#define UIPCINT_ERROR  -1

#define UIPC_NULL_BASE         0x0
#define UIPC_NULL_BASE_OFFSET  0x0

/* ============================================================================
**  Function : uIPCInt_Trigger
** ============================================================================*/
/**
  Trigger an interrupt to a different processor within the same power domain,
  accessible within uImage.

  @param eTarget    [in] -- The processor to trigger on.
  @param eInterrupt [in] -- The interrupt to send.

  @return
  UIPCINT_SUCCESS -- Will return DAL_SUCCESS even if the interrupt could
  not be triggered.  To verify support DalIPCInt_IsSupported should be
  used. 
 
  @dependencies
  None.
*/ 

int32 uIPCInt_Trigger
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
);


/* ============================================================================
**  Function : uIPCInt_IsSupported
** ============================================================================*/
/**
  This function returns if the given interrupt is supported to be triggered
  on the given target processor.

  @param eTarget    [in] -- The target processor to check.
  @param eInterrupt [in] -- The interrupt to check.

  @return
  UIPCINT_SUCCESS -- Interrupt is supported.\n
  UIPCINT_ERROR -- Interrupt is not supported.
 
  @dependencies
  None.
*/ 

int32 uIPCInt_IsSupported
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
);

#endif  //__uIPCINT_H__
