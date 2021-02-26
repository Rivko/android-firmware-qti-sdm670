/****************************************************************************
 * hfat_port_f.c
 *
 * OS Porting functions for HCC HFAT
 * Copyright (C) 2006-2007,2009,2012 QUALCOMM Technologies, Inc.
 *
 * HFAT requires some facilities in the host system, and this file provides
 * the 'porting layer' it needs.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2009-05-26   ebb   Removed from protected memory segment.
2009-03-25   wek   Move functionality of f_gettime f_getdate to fs_util.c
2007-05-25   umr   Added support for EFS memory protection feature.
2007-02-15   umr   Allocate F_LFNINT structure to eliminate local declaration.
2007-02-14   umr   Allocate F_NAME structure to eliminate local declaration
2007-01-30   umr   Fill in time fields.
2007-02-08   sh    Delcare path buffers through structure
2007-02-07   umr   API to eliminate local declaration of file name path buffer
2006-07-06   sch   Create (used to be hfat_port.c)

===========================================================================*/
#ifndef _PORT_C_
#define _PORT_C_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
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

#include "hfat_fat.h"
#include "hfat_port_f.h"
#include "fs_util.h"
#include "fs_err.h"


/****************************************************************************
 *
 * f_getrand
 *
 * This function should be ported. It has to return a different 32bit
 * random number whenever it is called. Random number generator could be
 * get from system time, this algorith below is just a simple random
 * number generator
 *
 * INPUTS
 *
 * rand - a number which could be used for random number generator
 *
 * RETURNS
 *
 * 32bit random number
 *
 ***************************************************************************/

static unsigned long dwrand = 0x729a8fb3;

unsigned long
f_getrand (unsigned long rand)
{
  long a;

  dwrand ^= f_gettime ();

  for (a = 0; a < 32; a++)
  {
    if (rand & 1)
    {
      dwrand ^= 0x34098bc2;
    }
    if (dwrand & 0x8000000)
    {
      dwrand <<= 1;
      dwrand |= 1;
    }
    else
      dwrand <<= 1;
    rand >>= 1;
  }

  return dwrand;
}

/****************************************************************************
 *
 * f_getdate
 *
 * need to be ported depending on system, it retreives the
 * current date in DOS format
 *
 * RETURNS
 *
 * current date
 *
 ***************************************************************************/

unsigned short
f_getdate (void)
{
  return (fs_posix_to_fat_date (fs_time()));
}

/****************************************************************************
 *
 * f_gettime
 *
 * need to be ported depending on system, it retreives the
 * current time in DOS format
 *
 * RETURNS
 *
 * current time
 *
 ***************************************************************************/

unsigned short
f_gettime (void)
{
  return (fs_posix_to_fat_time (fs_time()));
}

/****************************************************************************
 *
 * f_mutex_create
 *
 * user function to create a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int
f_mutex_create (FN_MUTEX_TYPE * mutex)
{
  *mutex = 0;
  return 0;
}
#endif

/****************************************************************************
 *
 * f_mutex_delete
 *
 * user function to delete a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int
f_mutex_delete (FN_MUTEX_TYPE * mutex)
{
  *mutex = 0;
  return 0;
}
#endif

/****************************************************************************
 *
 * f_mutex_get
 *
 * user function to get a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int
f_mutex_get (FN_MUTEX_TYPE * mutex)
{
  if (*mutex)
    return 1;

/*  add disable interrupt function here  */
  *mutex = 1;
/*  add enable interrupt function here  */

  return 0;
}
#endif

/****************************************************************************
 *
 * f_mutex_put
 *
 * user function to release a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int
f_mutex_put (FN_MUTEX_TYPE * mutex)
{
  *mutex = 0;
  return 0;
}
#endif

/****************************************************************************
 *
 * fn_gettaskID
 *
 * user function to get current task ID, valid return value must be get
 * from the current running task if its a multitask system, another case
 * this function can always returns with 1. Return value zero is not a valid
 * value.
 *
 * RETURNS
 *   task ID
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
#ifndef _FN_GETTASKID_
long
fn_gettaskID (void)
{
  return 1;                     /* any value except 0 */
}
#endif
#endif

/*
 * HFAT needs scratch buffers for pathnames in several functions.
 * Instead of allocating these on the (limited) stack, we use a static
 * allocation here and dish them out as needed.
 */
struct hfat_path_buffer hfat_path_buffers[MAX_HFAT_PATH_BUFFERS];

/****************************************************************************
 *
 * hfat_allocate_path_buffer
 *
 * Allocate a buffer of size FN_MAXPATH. This function is not thread safe.
 *  Passing W_CHAR** buf_ptr permits the the use of non constant pointers.
 *
 * RETURNS
 *   Pointer to hfat_path_buffer which is to be used during a call to
 *   hfat_free_path_buffer ().
 *
 ***************************************************************************/

struct hfat_path_buffer *
hfat_allocate_path_buffer (W_CHAR** buf_ptr)
{
  int i;

  /* Find first unused buffer */
  for (i = 0; i < MAX_HFAT_PATH_BUFFERS; i++)
  {
    if (!hfat_path_buffers[i].in_use)
      break;
  }

  if (i >= MAX_HFAT_PATH_BUFFERS)
  {
    /* No unused buffers found -- bail out */
    FS_ERR_FATAL ("Out of hfat Path buffers", 0, 0, 0);
  }

  /* Assign the buffer */
  hfat_path_buffers[i].in_use = 1;
  if (buf_ptr != NULL)
    *buf_ptr = hfat_path_buffers[i].buf;

  return &hfat_path_buffers[i];
}

/****************************************************************************
 *
 * hfat_free_path_buffer
 *
 * Free a allocated buffer. This function is not thread safe.
 *
 * RETURNS
 *   NONE
 *
 ***************************************************************************/

void
hfat_free_path_buffer (struct hfat_path_buffer *buf)
{
  if (!buf || (buf->in_use == 0) )
  {
    FS_ERR_FATAL ("Attempt to free bad hfat path buffer", 0, 0, 0);
  }
  buf->in_use = 0;

  return;
}

/*
 * HFAT needs scratch buffers for F_NAME in several functions.
 * Instead of allocating these on the (limited) stack, we use a static
 * allocation here and dish them out as needed.
 */
struct hfat_f_name_buffer hfat_f_name_buffers[MAX_HFAT_F_NAME_BUFFERS];

/****************************************************************************
 *
 * hfat_allocate_f_name_buffer
 *
 * Allocate a buffer for F_NAME. This function is not thread safe.
 *  Passing F_NAME** buf_ptr permits the the use of non constant pointers.
 *
 * RETURNS
 *   pointer to hfat_f_name_buffer which is to be used during a call to
 *   hfat_free_f_name_buffer ().
 *
 ***************************************************************************/

struct hfat_f_name_buffer *
hfat_allocate_f_name_buffer (F_NAME** buf_ptr)
{
  int i;

  /* Find first unused buffer */
  for (i = 0; i < MAX_HFAT_F_NAME_BUFFERS; i++)
  {
    if (!hfat_f_name_buffers[i].in_use)
      break;
  }

  if (i >= MAX_HFAT_F_NAME_BUFFERS)
  {
    /* No unused buffers found -- bail out */
    FS_ERR_FATAL ("Out of hfat f_name buffers", 0, 0, 0);
  }

  /* Assign the buffer */
  hfat_f_name_buffers[i].in_use = 1;
  if (buf_ptr != NULL)
    *buf_ptr = &hfat_f_name_buffers[i].f_name;

  return &hfat_f_name_buffers[i];
}

/****************************************************************************
 *
 * hfat_free_f_name_buffer
 *
 * Free a allocated buffer. This function is not thread safe.
 *
 * RETURNS
 *   NONE
 *
 ***************************************************************************/

void
hfat_free_f_name_buffer (struct hfat_f_name_buffer *buf)
{
  if (!buf || (buf->in_use == 0) )
  {
    FS_ERR_FATAL ("Attempt to free bad hfat f_name buffer", 0, 0, 0);
  }
  buf->in_use = 0;

  return;
}

/*
 * HFAT needs scratch buffers for F_LFNINT in several functions.
 * Instead of allocating these on the (limited) stack, we use a static
 * allocation here and dish them out as needed.
 */
struct hfat_f_lfnint_buffer hfat_f_lfnint_buffers[MAX_HFAT_F_LFNINT_BUFFERS];

/****************************************************************************
 *
 * hfat_allocate_f_lfnint_buffer
 *
 * Allocate a buffer for F_LFNINT. This function is not thread safe.
 *  Passing F_LFNINT** buf_ptr permits the the use of non constant pointers.
 *
 * RETURNS
 *   Pointer to hfat_f_name_buffer which is to be used during a call to
 *   hfat_free_path_buffer ().
 *
 ***************************************************************************/

struct hfat_f_lfnint_buffer *
hfat_allocate_f_lfnint_buffer (F_LFNINT** buf_ptr)
{
  int i;

  /* Find first unused buffer */
  for (i = 0; i < MAX_HFAT_F_LFNINT_BUFFERS; i++)
  {
    if (!hfat_f_lfnint_buffers[i].in_use)
      break;
  }

  if (i >= MAX_HFAT_F_LFNINT_BUFFERS)
  {
    /* No unused buffers found -- bail out */
    FS_ERR_FATAL ("Out of hfat f_lfnint buffers", 0, 0, 0);
  }

  /* Assign the buffer */
  hfat_f_lfnint_buffers[i].in_use = 1;
  if (buf_ptr != NULL)
    *buf_ptr = &hfat_f_lfnint_buffers[i].f_lfnint;

  return &hfat_f_lfnint_buffers[i];
}

/****************************************************************************
 *
 * hfat_free_f_lfnint_buffer
 *
 * Free a allocated buffer. This function is not thread safe.
 *
 * RETURNS
 *   NONE
 *
 ***************************************************************************/

void
hfat_free_f_lfnint_buffer (struct hfat_f_lfnint_buffer *buf)
{
  if (!buf || (buf->in_use == 0) )
  {
    FS_ERR_FATAL ("Attempt to free bad hfat f_lfnint buffer", 0, 0, 0);
  }
  buf->in_use = 0;

  return;
}

/****************************************************************************
 *
 * end of port.c
 *
 ***************************************************************************/

#endif /* _PORT_C_ */
