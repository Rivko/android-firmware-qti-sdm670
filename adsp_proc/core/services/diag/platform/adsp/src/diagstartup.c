/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  LEGACY DIAG AND SMD-SIO STARTUP ROUTINE

GENERAL DESCRIPTION
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1990,1991,1992 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1996,1998,1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2009           by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2010-2012      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/qw.c_v   1.5   08 Mar 2002 12:10:28   rajeevg  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
08/30/12   sg      Fixed COmpiler Warnings
07/05/12   sg      Changes to bring up Diag Multi PD image
03/08/12   sg      Remove sio initialization on 8974
5/21/08    sk      Created
04/14/10   JV      Changes for 8660 without CS
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined CUST_H
#include "customer.h"
#endif
#include "comdef.h"

#include "diag.h"
#include "msg.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern void diag_qdi_register(void);
extern void   diagstart(void);
#if defined(DIAG_SIO_SUPPORT)
extern void sio_init();
#endif
/*===========================================================================

FUNCTION QW_SET

DESCRIPTION
  This macro sets the high order 32 bits of a specified qword to a
  specified 32 bit value, and the low order 32 bits to another specified
  value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void diag_startup(void *piEnv)
{
  (void)piEnv; /* Compiler warning */
#if defined(DIAG_SIO_SUPPORT)
   sio_init();
#endif
 
   diagstart();
   diag_qdi_register();

   MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Hello World");
} /* legacy_startup */
