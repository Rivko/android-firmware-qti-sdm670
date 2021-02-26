#ifndef MSG_H
#define MSG_H
/*!
  @ingroup diag_message_service
  @file msg.h 
  @brief
  Diagnostic Debug Message Service (a.k.a F3 message) header file.

  @details
  This includes support for the extended debugging capabilities (MSG_1 etc)
  as well as the legacy debug messaging scheme (MSG_HIGH etc).
         
  This file contains necessary re-directions to achieve commonality of 
  Diagnostic Debug message service API header in different Operating systems. 
  Clients need to include this header file to use Diag Debug message service 
  macros. This file in-turn includes correct header files needed by clients, 
  based on client configuration. 
*/

/*
Copyright (c) 2002 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/services/msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/13   xy      Removed code under FEATURE_WINCE  
05/26/10   mad     Doxygenated
03/02/10   mad     Removed uldr feature.
01/20/10   sg      Moved inclusion of msg_qsr.h to msg_diag_service.h
11/18/09   ps      Added qsr feature support.
09/30/09   as      Removed inclusion of customer.h 
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/17/09   mad     Included customer.h conditionally.
03/16/09   mad     Removed inclusion of customer.h
10/23/08   mad     Separated out OS-specific redirections of MSG macros into
                   different header files. Now, diag message service macros 
                   are defined in msg_diag_service.h.
01/02/08   mad     Enabled real diag F3 messages for FEATURE_WINCE, 
                   instead of retail messages.
09/01/04   eav     Code reorganization
07/08/04   eav     Moved F3 trace saving typedefs to diagdebug.h
06/02/04   eav     Added "debug_mask".  Determines whether or not to save
                   the F3 trace to the RAM buffer.
04/21/03   as      Added MSG_SPRINTF macros for 4,5,6,7 &8 arguments.
03/12/04   eav     Added msg_copy_to_efs_check prototype to be called from
                   tmc.c. 
02/20/04   eav     Added savetime and savevar flags to msg_const_type struct. 
09/23/03   as      defined (TARGET_UNIX) to support the DSPE - a Linux based 
                   phone emulator used by data team in testing and debugging.
07/24/02   lad     Updated to reflect requirements changes and final 
                   implementation of extended message service.
02/06/02   igt     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*!
@defgroup diag_message_service Diagnostic Debug Message Service

This service can be configured by the host tool and delivers un-solicited
debug data from the device when triggered by system operation.
*/


#include "msg_diag_service.h"


#endif              /* MSG_H */
