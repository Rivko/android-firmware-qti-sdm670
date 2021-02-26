#ifndef TIME_TYPE_H
#define TIME_TYPE_H

/**
@file time_types.h
@brief
Defines structures and enums that are common to timekeeping functions. 

*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_services group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/time_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
07/12/10   din     Added internal features for better readability.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "qw.h"


/** @addtogroup time_services
@{ */

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Time, in slow-clock counts, from 0 to 0xFFFFFFFF (~1.5 days)
-----------------------------------------------------------------------------*/
/** Time is represented as a 64-bit value, which is currently a qword. A qword 
is a two-element array of unsigned longs that appears in the Qualcomm Utility 
Services. 
*/
typedef qword time_type;


/** @brief Julian time structure.
*/
typedef struct
{
  uint16 year;   /**< Year [1980 through 2100]. */
  uint16 month;  /**< Month of year [1 through 12]. */
  uint16 day;    /**< Day of month [1 through 31]. */
  uint16 hour;   /**< Hour of day [0 through 23]. */
  uint16 minute; /**< Minute of hour [0 through 59]. */
  uint16 second; /**< Second of minute [0 through 59]. */
  uint16 day_of_week; /**< Day of the week [0 through 6] or [Monday through 
                           Sunday]. */
}
time_julian_type;

typedef PACKED struct PACKED_POST
{  
  uint8  u_Version;
  uint64 old_ms;
  uint64 new_ms;
  int64  delta;
  uint32 rex_tcb;
  char   tcb_string[20];
}time_delta_report_log_type;

/* FEATURE_TIME_MULTI_SERVER_PROC - Only defined on modem proc 
when time is remoted. 
   FEATURE_TIME_SINGLE_MULTI_SERVER_PROC – Only defined on proc to 
enable efs_calls, time_ipc etc. ( functions required on modem proc for 
both single and multi processor chips ) */

/* for 9K, both Q6 and ARM9 are modem and hence we need to distinguish */
#ifdef T_MDM9X00

#if ( (defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)) && defined(FEATURE_TIME_QDSP6) ) 
/** Enables remote time features. */
#define FEATURE_TIME_MULTI_SERVER_PROC
/** Enables calls to the time interprocessor communication and embedded file system. */
#ifndef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */
#else /* for ARM9 */
#ifndef FEATURE_TIME_QDSP6 /*for products like 8960, T_MDM9X00 is defined and therefore, TIME_IPC was getting undef for Q6*/
/** Enables Time IPC features. */
#undef FEATURE_TIME_IPC_TASK
#endif
#endif

#else /*else for other targets*/

#if ( (defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)) && defined(IMAGE_MODEM_PROC) ) 
/** Enables remote time features. */
#define FEATURE_TIME_MULTI_SERVER_PROC
/** Enables calls to the time interprocessor communication and embedded file system. */
#ifndef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC && IMAGE_MODEM_PROC */

#endif

#ifndef FEATURE_MULTIPROCESSOR
/** Enables calls to the time interprocessor communication and embedded file system. */
#ifndef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */
#endif /* !FEATURE_MULTIPROCESSOR */


/** @} */ /* end_addtogroup time_sevices */

#endif /* TIME_TYPES_H */





