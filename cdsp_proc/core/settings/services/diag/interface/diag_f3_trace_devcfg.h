#ifndef DIAG_F3_TRACE_DEVCFG_H
#define DIAG_F3_TRACE_DEVCFG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F3 Trace Device Configuration
                
GENERAL DESCRIPTION
  DAL Device Configuration support for F3 Trace Configuration

Copyright (c) 2011-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/settings/services/diag/interface/diag_f3_trace_devcfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/13   rh      Transferred file ownership and location to Diag 
07/27/12   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/
//based on legacy NV bitmask values
typedef enum
{
  DIAG_F3_TRACE_DISABLE                 = 0x0,
  DIAG_F3_TRACE_ENABLE                  = 0x1,
  DIAG_F3_TRACE_DIAG_MASK_DEBUG_MODE    = 0x9,

  SIZEOF_DIAG_F3_TRACE_CONTROL_TYPE
} diag_f3_trace_control_type;



#endif /* DIAG_F3_TRACE_DEVCFG_H */
