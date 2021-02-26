/****************************************************************************
 * hfat_udefs_f.h
 *
 * Short description.
 * Copyright (C) 2006-2009,2012 QUALCOMM Technologies, Inc.
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
2014-10-01   ck    Enabled internal memcpy as compiler memcpy has misalignment issue.
2012-04-12   wek   Change cache default size to avoid defining in cust file.
2012-03-20   wek   Cleanup customer header file inclusion.
2009-06-05   ebb   Made F_MAXFILES overridable with FS_HFAT_MAX_FILES.
2009-01-29   wek   Reduced FN_MAXVOLUME and FN_MAXTASK. Added FN_VOLINDEX.
2008-12-15   yg    Use HFAT_WR_DATACACHE_SIZE to override write cache.
2008-11-24   yg    Set write cache to default 64 sectors and overridable.
2008-09-24   ebb   Stripped USE_MALLOC code.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-05-16   umr   Made FN_MAXVOLUME configurable from cust file.
2007-03-05   sch   Integrating HFAT 3.23
2007-02-08   sch   Dropped in HFAT 3.22
2006-10-06   sch   Turned on the seek optimization and dropped in HFAT 3.04
2006-07-13   sch   Turned F_LONGFILENAME option on
2006-07-06   sch   Create

===========================================================================*/
#ifndef _UDEFS_F_H_
#define _UDEFS_F_H_

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

#ifdef __cplusplus
extern "C"
{
#endif

#include "fs_config.h"

/****************************************************************************
 * enable this if CAPI is used
 ***************************************************************************/
#define FN_CAPI_USED 0


/****************************************************************************
 *
 * OEM name
 ***************************************************************************/
#define OEM_NAME "MSDOS5.0"
/*#define OEM_NAME "EFFSFAT"*/

/****************************************************************************
 *
 * Last error usage
 *
 ***************************************************************************/

#if 0
/* simple asignment */
#define F_SETLASTERROR(ec) (fm->lasterror=(ec))
#define F_SETLASTERROR_NORET(ec) (fm->lasterror=(ec))
#elif 1
/* function calls used for it */
#define F_SETLASTERROR(ec) fn_setlasterror(fm,ec)
#define F_SETLASTERROR_NORET(ec) fn_setlasterror_noret(fm,ec)
#elif 0
/* no last error is used (save code space) */
#define F_SETLASTERROR(ec) (ec)
#define F_SETLASTERROR_NORET(ec)
#endif

/****************************************************************************
 *
 * CAPI selected includes
 *
 ***************************************************************************/

#if FN_CAPI_USED
#include "../../fw_port.h"
#else

/****************************************************************************
 *
 *      if Unicode is used then comment in HCC_UNICODE define
 *
 ***************************************************************************/
#define HCC_UNICODE

#ifndef HCC_UNICODE
#define F_LONGFILENAME 1        /*  0 - 8+3 names   1 - long file names   */
#define W_CHAR char
#else
#define F_LONGFILENAME 1        /* don't change it, because unicode version alvays uses long file name */
#define W_CHAR wchar
#endif

/****************************************************************************
 *
 * volumes definitions
 *
 ***************************************************************************/
#ifndef FS_HFAT_MAXVOLUME_COUNT
#define FN_MAXVOLUME            3       /* maximum number of volumes */
#else
#define FN_MAXVOLUME            FS_HFAT_MAXVOLUME_COUNT /* max num of volumes */
#endif

#ifndef FN_VOLINDEX
  #define FN_VOLINDEX           26     /* For the 26 leters of the alphabet */
#endif

#define FN_MAXTASK              1      /* maximum number of task */

#define FN_MAXPATH              256     /* maximum allowed filename or pathname */

#define FN_MUTEX_TYPE unsigned long

#if 1
#define F_SEPARATORCHAR '/'
#else
#define F_SEPARATORCHAR '\\'
#endif

#endif                          /* FN_CAPI_USED */

#ifdef FS_HFAT_MAX_OPEN_FILES
  #define F_MAXFILES  FS_HFAT_MAX_OPEN_FILES        /* maximum number of open files */
#else
  #define F_MAXFILES  16
#endif

#define F_MAXSEEKPOS 8          /* number of division of fast seeking */

/****************************************************************************
 *
 * functions definitions
 *
 ***************************************************************************/

/* Use internal mem functions (memcpy,memset) or switch to library functions */
#define INTERNAL_MEMFN

/* Enable FAT caching */
#define FATCACHE_ENABLE
#if F_LONGFILENAME
#define DIRCACHE_ENABLE
#endif

#ifdef FATCACHE_ENABLE
#define FATCACHE_BLOCKS 4
#define FATCACHE_READAHEAD 8    /* max. 256 */
#endif

#if F_LONGFILENAME
#ifdef DIRCACHE_ENABLE
#define DIRCACHE_SIZE 32        /* max. 32 (<=max. cluster size) */
#endif
#endif

/* Have the default HFAT dta cache size as 128 sectors.
 * This value can be reduced in cases where there is memory constraints.
 * This value can be increased if the SDC controller supports more than
 * 128 sector writes at a time. */
#ifdef HFAT_WR_DATACACHE_SIZE
  #define WR_DATACACHE_SIZE       HFAT_WR_DATACACHE_SIZE
#else
  #define WR_DATACACHE_SIZE       128
#endif


#ifdef FATCACHE_ENABLE
#define FATCACHE_SIZE (FATCACHE_BLOCKS*FATCACHE_READAHEAD)
#endif



#ifdef INTERNAL_MEMFN
#define _memcpy(d,s,l) _f_memcpy(d,s,l)
#define _memset(d,c,l) _f_memset(d,c,l)
#else
#include <string.h>
#define _memcpy(d,s,l) memcpy(d,s,l)
#define _memset(d,c,l) memset(d,c,l)
#endif


/****************************************************************************
 *
 * Last access date
 *
 ***************************************************************************/

#define F_UPDATELASTACCESSDATE 0
/* it defines if a file is opened for read to update lastaccess time */

/****************************************************************************
 *
 * Opened file size
 *
 ***************************************************************************/

#define F_FINDOPENFILESIZE 1
/* set F_FINDOPENFILESIZE to 0 if filelength needs to return with 0 for an opened file  */
/* other case filelength functions can return with opened file length also */


/****************************************************************************
 *
 * closing bracket for C++
 *
 ***************************************************************************/

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of hfat_udefs_f.h
 *
 ***************************************************************************/

#endif                          /* _UDEFS_H_ */

