#ifndef TIME_SVC_H
#define TIME_SVC_H

/**
@file time_svc.h
@brief
Implements time-keeping functions using various sources.

*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_remote group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2003-2005, 2008-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*==============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using various sources.


EXTERNALIZED FUNCTIONS
  time_get( )
    Retrieves time according to CDMA, HDR, GSM, or slow-clock Time-of-Day

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/time_svc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/13   abh     File created.

=============================================================================*/



/*==============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "qw.h"
#include "qurt.h"
#include "time_types.h"

/*==============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/

/** @addtogroup time_services
@{ */

/*-----------------------------------------------------------------------------
  Time Source, returned by time_get( )
-----------------------------------------------------------------------------*/
/** Identifies the time source type.
*/
typedef enum
{
  TIME_SOURCE_32KHZ, /**< Time is based on the 32-kHz slow clock. */
  TIME_SOURCE_CDMA,  /**< Time is based on the CDMA concept of time. */
  TIME_SOURCE_HDR,   /**< Time is based on the HDR concept of time. */
  TIME_SOURCE_GSM,   /**< Time is based on the GSM concept of time. */
  TIME_SOURCE_WCDMA, /**< Time is based on the WCDMA concept of time. */
  TIME_SOURCE_GPS,   /**< Time is based on the GPS concept of time. */
  TIME_SOURCE_MFLO   /**< Time is based on the MediaFLO concept of time. */
}
time_source_enum_type;




/*=============================================================================

                            FUNCTION DEFINITIONS

=============================================================================*/

/**
Returns the current time.

@param[out] ts_val Location to place the timestamp.

@return
Source of the time used.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
time_source_enum_type time_get
(
  time_type                       ts_val
);

/**

FUNCTION time_get_sum_genoff_ts

DESCRIPTION
  Returns sum generic offset value of system base in timestamp format

DEPENDENCIES
  None

RETURN VALUE
  boolean as return value
  Provides sum generic ts offset value in sum_genoff_ts_val

SIDE EFFECTS
  None

**/
boolean time_get_sum_genoff_ts
(
    time_type        sum_genoff_ts_val
);

/**
Returns the uptime in timestamp format

@param[out] ts_val Location to place the timestamp.

@return
Source of the time used.

*/
time_source_enum_type time_get_uptime
(
  /* OUT: The current time */
  time_type                       ts_val
);

#endif /* TIME_H */

