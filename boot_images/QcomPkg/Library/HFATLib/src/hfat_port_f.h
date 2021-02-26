/****************************************************************************
 * hfat_port_f.h
 *
 * OS Porting function prototypes for HCC HFAT
 * Copyright (C) 2006-2007 QUALCOMM Technologies, Inc.
 *
 * HFAT requires some facilities in the host system, and this file defines
 * the 'porting layer' it needs.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-02-15   umr   Allocate F_LFNINT structure to eliminate local declaration.
2007-02-14   umr   Allocate F_NAME structure to eliminate local declaration
2007-02-08   sh    Delcare path buffers through structure
2007-02-07   umr   API to eliminate local declaration of file name path buffer
2006-07-06   sch   Create (used to be hfat_port.h)

===========================================================================*/
#ifndef _PORT_H_
#define _PORT_H_

/****************************************************************************
 *
 *            Copyright (c) 2003-2006 by HCC Embedded
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

#ifdef __cplusplus
extern "C"
{
#endif
#include "hfat_api_f.h"
#include "hfat_fat.h"

/* Temporary buffer for HFAT Unicode paths */
struct hfat_path_buffer
{
  W_CHAR buf[FN_MAXPATH];
  unsigned int in_use;
};

/* Number of path buffers required */
#define MAX_HFAT_PATH_BUFFERS    4

/* Temporary buffer for HFAT Unicode F_NAME Structures */
struct hfat_f_name_buffer
{
  F_NAME f_name;
  unsigned int in_use;
};

/* Number of path buffers required */
#define MAX_HFAT_F_NAME_BUFFERS    2


/* Temporary buffer for HFAT Unicode F_LFNINT Structures */
struct hfat_f_lfnint_buffer
{
  F_LFNINT f_lfnint;
  unsigned int in_use;
};

/* Number of path buffers required */
#define MAX_HFAT_F_LFNINT_BUFFERS    4

  extern unsigned long f_getrand (unsigned long rand);
  extern unsigned short f_getdate (void);
  extern unsigned short f_gettime (void);

#if (!FN_CAPI_USED)
  extern int f_mutex_get (FN_MUTEX_TYPE *);
  extern int f_mutex_put (FN_MUTEX_TYPE *);
  extern int f_mutex_create (FN_MUTEX_TYPE *);
  extern int f_mutex_delete (FN_MUTEX_TYPE *);
  extern long fn_gettaskID (void);
#endif
  struct hfat_path_buffer *hfat_allocate_path_buffer (W_CHAR** buf_ptr);
  void hfat_free_path_buffer (struct hfat_path_buffer *);

  struct hfat_f_name_buffer *hfat_allocate_f_name_buffer (F_NAME** buf_ptr);
  void hfat_free_f_name_buffer (struct hfat_f_name_buffer *);

  struct hfat_f_lfnint_buffer *hfat_allocate_f_lfnint_buffer (F_LFNINT** buf_ptr);
  void hfat_free_f_lfnint_buffer (struct hfat_f_lfnint_buffer *);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of hfat_port_f.h
 *
 ***************************************************************************/

#endif                          /* _PORT_F_H_ */
