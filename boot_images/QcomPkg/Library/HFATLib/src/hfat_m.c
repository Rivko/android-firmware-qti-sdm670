
/****************************************************************************
 * hfat_m.c
 *
 * Short description.
 * Copyright (C) 2006, 2008-2010 QUALCOMM Technologies, Inc.
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
2010-03-19   yog   Added support for files having max filesize upto 4GB.
2009-08-11   wek   Remove hfat drive number coupled to hotplug device index.
2009-06-15   ds    Added function to configure long filename support.
2009-05-08   yg    On rename decide if moving based on parent path diff.
2009-04-03   wek   Added functions to get/set access/mod time, for efs_utime.
2009-03-17   wek   Renamed var ctime/cdate to mtime/mdate to avoid confusion.
2008-05-09   ebb   Fixed rename functions to call rename with drive in dst.
2008-03-11   sch   Release critical section in fn_setlasterror
2007-02-08   sch   Dropped in HFAT 3.22
2006-12-04   sch   Dropped in HFAT 3.16
2006-10-09   sch   Dropped in HFAT 3.04
2006-07-06   sch   Create

===========================================================================*/
#ifndef _FAT_M_C_
#define _FAT_M_C_

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
#include "hfat_common.h"

#if (!FN_CAPI_USED)

/****************************************************************************
 *
 * fm_getversion
 *
 * returns with the filesystem version string
 *
 * RETURNS
 *
 * string pointer with version number
 *
 ***************************************************************************/

char *
fm_getversion (void)
{
  return fn_getversion ();
}

/****************************************************************************
 *
 * fm_initvolume
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume
 *
 * INPUTS
 *
 * drvnumber - which drive need to be initialized
 * driver_init - driver init function
 * driver_param - parameter to pass to driver init
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_initvolume (int drvnumber, F_DRIVERINIT driver_init,
               const void *driver_param)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_initvolume (fm, drvnumber, driver_init, driver_param);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_createdriver
 *
 * Creating a driver, it calls driver init function
 *
 * INPUTS
 *
 * driver - where to store created driver
 * driver_init - driver init function
 * driver_param - parameter to pass to driver init
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_createdriver (F_DRIVER ** driver, F_DRIVERINIT driver_init,
                 int driveno, const void *driver_param)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_createdriver (fm, driver, driver_init, driveno, driver_param);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_initvolumepartition
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume and a given partition
 *
 * INPUTS
 *
 * drvnumber - which drive need to be initialized
 * driver - driver to be used for this volume
 * partition - selected partition on the drive
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_initvolumepartition (int drvnumber, F_DRIVER * driver, int partition)
{
  int ret;
  F_MULTI *fm;
  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_initvolumepartition (fm, drvnumber, driver, partition);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_createpartition
 *
 * It creates partition on a media
 *
 * INPUTS
 *
 * driver - driver structure
 * parnum - number of partition in par parameter
 * par - F_PARTITION structure, partition descriptor
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_createpartition (F_DRIVER * driver, int parnum, F_PARTITION * par)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_createpartition (fm, driver, parnum, par);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_getpartition
 *
 * getting partition info from drive
 *
 * INPUTS
 *
 * driver - driver structure
 * parnum - number of partition entry in par parameter
 * par - F_PARTITION structure where the result goes to
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getpartition (F_DRIVER * driver, int parnum, F_PARTITION * par)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getpartition (driver, parnum, par);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_checkvolume
 *
 * Deletes a previously initialized volume.
 *
 * INPUTS
 *
 * drvnumber - number of drive to check
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_checkvolume (int drvnumber)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_checkvolume (fm, drvnumber);

  _f_mutex_put (fm);
  return ret;
}

/****************************************************************************
 *
 * fm_delvolume
 *
 * Deletes a previously initialized volume.
 *
 * INPUTS
 *
 * drvnumber - number of drive to delete
 *
 * RETURNS
 *
 * errorcode
 *
 ***************************************************************************/

int
fm_delvolume (int drvnumber)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_delvolume (fm, drvnumber);

  /* if delvolume called releasedriver to delete mutex then this fm->pmutex reference is reset */
  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_releasedriver
 *
 * releasing a driver
 *
 * INPUTS
 *
 * driver - driver structure
 *
 * RETURNS
 *
 * errorcode
 *
 ***************************************************************************/

int
fm_releasedriver (F_DRIVER * driver)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_releasedriver (fm, driver);

  /* putting back the mutex is not necessary because the mutex is deleted in releasedriver,delvolume */

  return ret;
}


/****************************************************************************
 *
 * fm_get_volume_count
 *
 * Returns the number of mounted volumes
 *
 * RETURNS
 *
 * number of mounted volumes
 *
 ***************************************************************************/

int
fm_get_volume_count (void)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_get_volume_count (fm);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_get_volume_list
 *
 * get active volume list. It puts into the given buffer the active volumes
 * number
 *
 * INPUTS
 *
 * buf - where function puts active volumes number,
 *		 buf size must be at least F_MAXVOLUME	integers
 *
 * RETURNS
 *
 * number of mounted volumes
 *
 ***************************************************************************/

int
fm_get_volume_list (int *buf)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_get_volume_list (fm, buf);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_format
 *
 * format a media, 1st it checks existing formatting, then master boot record,
 * then media physical
 *
 * INPUTS
 *
 * drivenum - which drive format is needed
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_format (int drivenum, long fattype)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_format (fm, drivenum, fattype);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getcwd (char *buffer, int maxlen)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getcwd (fm, buffer, maxlen);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgetcwd (wchar * buffer, int maxlen)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgetcwd (fm, buffer, maxlen);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_getdcwd
 *
 * getting a drive current working directory
 *
 * INPUTS
 *
 * drivenum - drive number of which drive current folder needed
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getdcwd (int drivenum, char *buffer, int maxlen)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getdcwd (fm, drivenum, buffer, maxlen);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgetdcwd (int drivenum, wchar * buffer, int maxlen)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgetdcwd (fm, drivenum, buffer, maxlen);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_chdrive
 *
 * Change current drive
 *
 * INPUTS
 *
 * drivenum - new current drive number (0-A, 1-B, ...)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_chdrive (int drivenum)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_chdrive (fm, drivenum);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_getdrive
 *
 * Get current drive number
 *
 * RETURNS
 *
 * with the current drive number (0-A, 1-B,...)
 *
 ***************************************************************************/

int
fm_getdrive (void)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return -1;                  /* busy! */
  if (fm->f_curdrive == -1)
    return -1;

  ret = fn_getdrive (fm);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_getfreespace
 *
 * get total/free/used/bad diskspace
 *
 * INPUTS
 *
 * drivenum - which drive free space is requested (0-A, 1-B, 2-C)
 * pspace - pointer where to store the information
 *
 * RETURNS
 * error code
 *
 ***************************************************************************/

int
fm_getfreespace (int drivenum, FN_SPACE * pspace)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;

  ret = fn_getfreespace (fm, drivenum, pspace);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int
fm_chdir (const char *dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_chdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wchdir (const wchar * dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wchdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}
#endif


/****************************************************************************
 *
 * fm_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_mkdir (const char *dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_mkdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wmkdir (const wchar * dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wmkdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_rmdir (const char *dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_rmdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wrmdir (const wchar * dirname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wrmdir (fm, dirname);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *                                                                                                                                                       
 * error code or zero if successful
 *                                                                                                                                                       
 ***************************************************************************/

int
fm_findfirst (const char *filename, FN_FIND * find)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_findfirst (fm, filename, find);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wfindfirst (const wchar * filename, FN_WFIND * find)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wfindfirst (fm, filename, find);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_findnext (FN_FIND * find)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_findnext (fm, find);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wfindnext (FN_WFIND * find)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wfindnext (fm, find);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int
fm_rename (const char *filename, const char *newname)
{
  int ret;
  F_MULTI *fm;
  char ismove = 0;
  int a, len = 0;
 
  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  /* Check length */
  while (newname[len]) len++;

  /* Jump past a:/ */
  if (len >= 3 && 
      (newname[1] == ':') &&
      (newname[2] == '/'))
    a = 3;
  else
    a = 0;
  
  for (;; a++)
  {
    char ch = newname[a];
    if (ch == '/' || ch == '\\')
    {
      ismove = 1;
      break;
    }
    if (!ch)
      break;
  } 
  
  if (ismove)
    ret = fn_move (fm, filename, newname);
  else
    ret = fn_rename (fm, filename, newname);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wrename (const wchar * filename, const wchar * newname)
{
  int ret;
  F_MULTI *fm;
  char ismove = 0;
  const wchar *src_name, *dst_name;
  uint32 src_len, dst_len;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  /* Walk from the end of path until first seperator,
   * then compare until that point to check if the path
   * differs, if the path differs then its a move,
   * otherwise if the path is same then its a rename */  
  src_name = filename;
  dst_name = newname;
  src_len = dst_len = 0;

  /* Findout the length of the src path */
  while (src_name[src_len]) src_len++;

  /* Walk from the end until we get first slash.
   * It would be parent path until that point */
  while (src_len > 1)
  {
    wchar ch;
    --src_len;
    ch = src_name[src_len];
    if ((ch == '/') || (ch == '\\'))
      break;
  }

  /* Findout length of the destination path */
  while (dst_name[dst_len]) dst_len++;

  /* Walk from the end until we get first slash.
   * It would be parent path until that point */
  while (dst_len > 1)
  {
    wchar ch;
    --dst_len;
    ch = dst_name[dst_len];
    if ((ch == '/') || (ch == '\\'))
      break;
  }

  /* Now that we got the lengths of both the parents, now
   * we can decide whether its a move or rename operation.
   * Compare if the parents differ. */
  if (dst_len != src_len)
  {
    ismove = 1;  /* Len doesn't match, its a move */
  }
  else
  {
    /* Since both lengths are same just use one. */
    while (dst_len > 1)
    {
      if (_f_toupper (src_name[dst_len]) != _f_toupper (dst_name[dst_len]))
      {
        ismove = 1; /* First mismatch indicates that its a move */
        break;
      }
      --dst_len;
    }
  }

  if (ismove)
    ret = fn_wmove (fm, filename, newname);
  else
    ret = fn_wrename (fm, filename, newname);
  
  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_move
 *
 * move file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (with path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int
fm_move (const char *filename, const char *newname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_move (fm, filename, newname);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wmove (const wchar * filename, const wchar * newname)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wmove (fm, filename, newname);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file or -1 if any error
 *
 ***************************************************************************/

long
fm_filelength (const char *filename)
{
  long ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return -1;                  /* busy! */

  ret = fn_filelength (fm, filename);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
long
fm_wfilelength (const wchar * filename)
{
  long ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return -1;                  /* busy! */

  ret = fn_wfilelength (fm, filename);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_close (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_close (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_flush
 *
 * flushing current content a file into physical. 
 *
 * INPUTS
 *
 * filehandle - which file needs to be flushed
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int
fm_flush (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_flush (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * FN_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/

FN_FILE *
fm_open (const char *filename, const char *mode)
{
  FN_FILE *ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_open (fm, filename, mode);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
FN_FILE *
fm_wopen (const wchar * filename, const wchar * mode)
{
  FN_FILE *ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_wopen (fm, filename, mode);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long
fm_read (void *buf, long size, long size_st, FN_FILE * filehandle)
{
  long ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_read (fm, buf, size, size_st, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of written bytes
 *
 ***************************************************************************/

long
fm_write (const void *buf, long size, long size_st, FN_FILE * filehandle)
{
  long ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_write (fm, buf, size, size_st, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - FN_FILE structure which file position needed to be modified
 * offset - relative position 
 * whence - where to calculate position (F_SEEK_SET,F_SEEK_CUR,F_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int
fm_seek (FN_FILE * filehandle, unsigned long offset, long whence)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_seek (fm, filehandle, offset, whence);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start or -1 if any error
 *
 ***************************************************************************/

long
fm_tell (FN_FILE * filehandle)
{
  long ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_tell (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully 
 *
 ***************************************************************************/

int
fm_getc (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return -1;                  /* busy! */

  ret = fn_getc (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the written character or -1 if any error
 *
 ***************************************************************************/

int
fm_putc (int ch, FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return -1;                  /* busy! */

  ret = fn_putc (fm, ch, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_rewind (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_rewind (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle
 *
 ***************************************************************************/

int
fm_eof (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_eof (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pmtime - mtime of the file where to store
 * pmdate - mdate of the file where to store
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_gettimedate (const char *filename, unsigned short *pmtime,
                unsigned short *pmdate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_gettimedate (fm, filename, pmtime, pmdate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgettimedate (const wchar * filename, unsigned short *pmtime,
                 unsigned short *pmdate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgettimedate (fm, filename, pmtime, pmdate);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_stat
 *
 * get status information on a file
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * stat - pointer where to store information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_stat (const char *filename, F_STAT * stat)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_stat (fm, filename, stat);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wstat (const wchar * filename, F_STAT * stat)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wstat (fm, filename, stat);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * mtime - new mtime of the file
 * mdate - new mdate of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_settimedate (const char *filename, unsigned short mtime,
                unsigned short mdate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_settimedate (fm, filename, mtime, mdate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wsettimedate (const wchar * filename, unsigned short mtime,
                 unsigned short mdate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wsettimedate (fm, filename, mtime, mdate);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_getaccessdate
 *
 * Get a file access date.
 *
 * INPUTS
 *
 * filename - file to get date/time attributes.
 * padate - pointer where to store the date.
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getaccessdate (const char * filename, unsigned short *padate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getaccessdate (fm, filename, padate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgetaccessdate (const wchar * filename, unsigned short *padate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgetaccessdate (fm, filename, padate);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_setaccessdate
 *
 * Set a file access date.
 *
 * INPUTS
 *
 * filename - file to search for access date.
 * adate - new access date of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_setaccessdate (const char * filename, unsigned short adate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_setaccessdate (fm, filename, adate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wsetaccessdate (const wchar * filename, unsigned short adate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wsetaccessdate (fm, filename, adate);

  _f_mutex_put (fm);

  return ret;
}
#endif


/****************************************************************************
 *
 * fm_getaccmodtime
 *
 * Get a file access date.
 *
 * INPUTS
 *
 * filename - file to get date/time attributes.
 * pmtime - pointer where to store the modification time.
 * pmdate - pointer where to store the modification date.
 * padate - pointer where to store the date.
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
int
fm_getaccmodtime (const char *filename, unsigned short *pmtime,
                unsigned short *pmdate, unsigned short *padate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getaccmodtime (fm, filename, pmtime, pmdate, padate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgetaccmodtime (const wchar * filename, unsigned short *pmtime,
                 unsigned short *pmdate, unsigned short *padate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgetaccmodtime (fm, filename, pmtime, pmdate, padate);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_setaccmodtime
 *
 * Set a file access date.
 *
 * INPUTS
 *
 * filename - file to set date/time attributes.
 * mtime - new write or modification time of the file.
 * mdate - new write or modification date of the file.
 * adate - new access date of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
int
fm_setaccmodtime (const char *filename, unsigned short mtime,
                unsigned short mdate, unsigned short adate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_setaccmodtime (fm, filename, mtime, mdate, adate);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wsetaccmodtime (const wchar * filename, unsigned short mtime,
                 unsigned short mdate, unsigned short adate)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wsetaccmodtime (fm, filename, mtime, mdate, adate);

  _f_mutex_put (fm);

  return ret;
}
#endif


/****************************************************************************
 *
 * fm_delete
 *
 * delete a file 
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_delete (const char *filename)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_delete (fm, filename);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wdelete (const wchar * filename)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wdelete (fm, filename);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getattr (const char *filename, unsigned char *attr)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getattr (fm, filename, attr);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wgetattr (const wchar * filename, unsigned char *attr)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wgetattr (fm, filename, attr);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_setattr (const char *filename, unsigned char attr)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_setattr (fm, filename, attr);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
int
fm_wsetattr (const wchar * filename, unsigned char attr)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_wsetattr (fm, filename, attr);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fm_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's is needed
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_getlabel (int drivenum, char *label, long len)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_getlabel (fm, drivenum, label, len);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's need to be set
 * label - new label for the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_setlabel (int drivenum, const char *label)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_setlabel (fm, drivenum, label);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fn_truncate
 *
 * truncate a file to a specified length, filepointer will be set to the 
 * end
 *
 * INPUTS
 *
 * filename - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * filepointer or zero if fails
 *
 ***************************************************************************/

FN_FILE *
fm_truncate (const char *filename, unsigned long length)
{
  FN_FILE *ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_truncate (fm, filename, length);

  _f_mutex_put (fm);

  return ret;
}

#ifdef HCC_UNICODE
FN_FILE *
fm_wtruncate (const wchar * filename, unsigned long length)
{
  FN_FILE *ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */

  ret = fn_wtruncate (fm, filename, length);

  _f_mutex_put (fm);

  return ret;
}
#endif

/****************************************************************************
 *
 *  fm_get_oem
 *
 *  Get OEM name
 *
 * INPUTS
 *
 * drivenum - drivenumber
 * str - where to store information
 * maxlen - length of the buffer
 *
 * RETURN
 *
 * errorcode or zero if successful
 *
 ***************************************************************************/

int
fm_get_oem (int drivenum, char *str, long maxlen)
{
  F_MULTI *fm;
  int ret;

  if (fnSetTask (&fm))
    return 0;                   /* busy! */
  ret = fn_get_oem (fm, drivenum, str, maxlen);
  _f_mutex_put (fm);
  return ret;
}

/****************************************************************************
 *
 * fm_ftruncate
 *
 * truncate a file to a specified length, filepointer will be set to the 
 * end
 *
 * INPUTS
 *
 * filehandle - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_ftruncate (FN_FILE * filehandle, unsigned long length)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_ftruncate (fm, filehandle, length);

  _f_mutex_put (fm);

  return ret;
}

/****************************************************************************
 *
 * fm_seteof
 *
 * set end of file at the current position
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fm_seteof (FN_FILE * filehandle)
{
  int ret;
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_seteof (fm, filehandle);

  _f_mutex_put (fm);

  return ret;
}


/****************************************************************************
 *
 * fm_getlasterror
 *
 * returns with the last set error
 *
 * RETURNS
 *
 * error code which was set as lasterror
 *
 ***************************************************************************/

int
fm_getlasterror ()
{
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  _f_mutex_put (fm);

  return fm->lasterror;
}


/***************************************************************************
 *
 * fm_enable_lfn_support
 *
 * enables or disbales HFAT's long filename support 
 *
 **************************************************************************/

void
fm_enable_lfn_support (char enabled)
{
  fn_enable_lfn_support (enabled);
} 

/****************************************************************************
 *
 * end of fat_m.c
 *
 ***************************************************************************/

#endif /* !FN_CAPI_USED */

#endif /* _FAT_M_C_ */
