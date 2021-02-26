#ifndef APP_MAILBOX_H
#define APP_MAILBOX_H

/** @file app_mailbox.h
 * @brief
 * This file contains the interface definition for QSEE mailbox testing routines.
 */
/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/inc/app_mailbox.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/15   el      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/**
  Invokes QSEE Mailbox testing procedure

  @return
  0      All tests passed successfully.
  -1     One or more test cases failed.
  
*/
int run_mailbox_tests(void);



#endif // APP_MAILBOX_H
