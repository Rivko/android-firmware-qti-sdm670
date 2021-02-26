#ifndef __CLOCKULOGSTUB_H__
#define __CLOCKULOGSTUB_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
                   U L O G   S T U B   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains stubs for ULOG functions and variable types.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/src/ClockULogStub.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
10/19/11   dcf     Initial release.

==========================================================================*/ 

/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

typedef void* ULogHandle;

#define ULOG_RT_PRINTF_0(stub1, stub2) {}
#define ULOG_RT_PRINTF_1(stub1, stub2, stub3) {}
#define ULOG_RT_PRINTF_2(stub1, stub2, stub3, stub4) {}

#define ULogFront_RealTimeInit(param1, param2, param3, param4, param5) {}
#define ULogCore_HeaderSet(param1, param2) {}

#define ULOG_MEMORY_LOCAL 0
#define ULOG_LOCK_OS      0


#endif /* !__CLOCKULOGSTUB_H__ */

