#ifndef TIME_GENOFF_H
#define TIME_GENOFF_H

/**
@file time_genoff.h
@brief
This module implements the Generic framework to handle time bases.
*/
/*=============================================================================
 NOTE: The @brief description and any detailed descriptions above do not appear
       in the PDF.
 
       The ATS_mainpage.dox file contains all file/group descriptions that
       are in the output PDF generated using Doxygen and Latex. To edit or
       update any of the file/group text in the PDF, edit the
       ATS_mainpage.dox file or contact Tech Pubs.
 
       The above description for this file is part of the time_genoff group
       description in the ATS_mainpage.dox file.
 =============================================================================*/
 
 /*=============================================================================
   Copyright (c) 2009-2017 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary
 =============================================================================*/
 /*=============================================================================
 
  T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M
 
 GENERAL DESCRIPTION
  Implements Generic framework to handle Time bases.
 
 =============================================================================*/
 
 
 /*=============================================================================
 
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/time_genoff.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "time_types.h"

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/ 
/**

@function
  time_genoff_get_ts_from_offset_and_timetick

@brief
  Calculates timestamp from passed offset and timetick.

@dependencies
  Need to pas offset details that are obtained from time_genoff_get_sum_genoff_ts

@return
  time in timestamp format in ts_val

**/

void time_genoff_get_ts_from_offset_and_timetick
(
  /* Generic offset in timestamp format */
  time_type           sum_genoff_ts,
  /* Sign of offset. FALSE if +ve, True if -ve */
  boolean             is_offset_negative,
  /* timetick at which timestamp is required */
  uint64              timetick,
  /* Calculated timestamp is returned */
  time_type           ts_val
);

#endif /* TIME_GENOFF_H */