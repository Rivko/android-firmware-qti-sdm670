#ifndef DIAGBUF_H
#define DIAGBUF_H

/*! 
@ingroup  diag_misc
@file diagbuf.h
@brief 
APIs to access diag's internal buffer directly. 
*/


/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag output allocation manager.

Copyright (c) 1999-2010, 2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                          Edit History

      $Header: //components/rel/core.qdsp6/2.1.c4/api/services/diagbuf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/22/13   sr      Removed PACK from pointer references
11/30/10   hm      Moved diagbuf_alloc, diagbuf_commit to diagbuf_v.h
06/17/10   mad     changes in doxygen comments, fixed doxygen warnings
11/04/09   cahn    DIAG Health
09/30/09   as      Removed inclusion of customer.h 
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/15/09   mad     Removed inclusion of diag.h and diagpkt.h
07/14/09   mad     Featurized inclusion of customer.h
06/11/09   JV      Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09   vk      removed dependencies from diagbuf_v.h 
12/12/08   vg      CMI split diagbuf.h and diagbuf_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
06/09/08   mad     change to define default sleep time and threshold 
                   conditionally
02/05/08   pc      Added support for dynamic threshold and sleep values
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
06/27/01   lad     diagbuf_blocking_alloc(): moved 'code' to diagpkt_alloc().
02/23/01   lad     Moved constants to diagtune.h.  Cosmetic cleanup for
                   rearchitected diag release.
11/07/00   jal     Renamed DIAGBUF_DATA_AVAIL_S to DIAGBUF_READY_S
09/01/00   pj      Increased DIAGBUF_MAX_SIZE to 10KB. Such a large buffer is
                   considered a temporary measure until the GPS SRCH code can
                   be sped up (it is currently starving the diag task, thus
                   requiring a large buffer).
08/28/00   lad     Removed use of DIAGBUF_MAX_SIZE.
06/19/00   lad     Moved some macros and prototypes from diagi.h.
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added DIAGBUF_NICE_SIZE for load balancing.  Updated API.
02/28/00   lad     Created file.

===========================================================================*/
#include "comdef.h"

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
  extern "C" {
#endif




/*!
@ingroup diag_misc
@brief
 This function reduces the length field of a previously allocated buffer.

 'ptr' must point to the same address that was returned by a prior call to
  diagpkt_alloc().
 
  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.

@param[in] *ptr             Specifying the ptr whose allocation size is to be reduced
@param[in] length          Length requested.

@dependencies
None
@sideeffects
None
@return
None
@sa
diagbuf_alloc, diagbuf_commit
*/

void
diagbuf_shorten ( void * ptr, unsigned int length);



/*!
@ingroup diag_misc
@brief
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed.

  diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

@dependencies
None
@sideeffects
None
@return
None
*/


void
diagbuf_flush (void);


#ifdef __cplusplus
  }
#endif

#endif /* DIAGBUF_H  */
