/****************************************************************************
 * hfat_debug.h
 *
 * Short description.
 * Copyright (C) 2006 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-07-06   sch   Create

===========================================================================*/
#ifndef _HCC_DEBUG_H_
#define _HCC_DEBUG_H_

/****************************************************************************
 *
 *            Copyright (c) 2006 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

/* set this define to 1 if program is running on PC and debug file is required */
#if 0

#include "stdio.h"

#ifdef _HCC_COMMON_C_
FILE *debfile = 0;
#else
extern FILE *debfile;
#endif

#define DEBOPEN if (!debfile) debfile=fopen("C:/fattest.txt","wt+");
#define DEBPR0(s) fprintf (debfile,s);
#define DEBPR1(s,p1) fprintf (debfile,s,p1);
#define DEBPR2(s,p1,p2) fprintf (debfile,s,p1,p2);
#define DEBPR3(s,p1,p2,p3) fprintf (debfile,s,p1,p2,p3);

#else

#define DEBOPEN
#define DEBPR0(s)
#define DEBPR1(s,p1)
#define DEBPR2(s,p1,p2)
#define DEBPR3(s,p1,p2,p3)

#endif

/****************************************************************************
 *
 * end of debug.h
 *
 ***************************************************************************/

#endif /* _HCC_DEBUG_H_ */
