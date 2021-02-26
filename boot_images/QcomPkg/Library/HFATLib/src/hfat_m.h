/****************************************************************************
 * hfat_m.h
 *
 * Short description.
 * Copyright (C) 2006,2009-2010 QUALCOMM Technologies, Inc.
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
2009-03-17   wek   Renamed var ctime/cdate to mtime/mdate to avoid confusion.
2006-10-09   sch   Dropped in HFAT 3.04
2006-07-06   sch   Create

===========================================================================*/
#ifndef _FAT_M_H_
#define _FAT_M_H_

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

#include "hfat_api_f.h"

#ifdef __cplusplus
extern "C"
{
#endif

  extern char *fm_getversion (void);
  extern int fm_initvolume (int drvnumber, F_DRIVERINIT driver_init,
                            const void *driver_param);
  extern int fm_initvolumepartition (int drvnumber, F_DRIVER * driver,
                                     int partition);
  extern int fm_createpartition (F_DRIVER * driver, int parnum,
                                 F_PARTITION * par);
  extern int fm_getpartition (F_DRIVER * driver, int parnum,
                              F_PARTITION * par);
  extern int fm_createdriver (F_DRIVER ** driver, F_DRIVERINIT driver_init,
                              int driveno, const void *driver_param);
  extern int fm_delvolume (int drvnumber);
  extern int fm_checkvolume (int drvnumber);
  extern int fm_get_volume_count (void);
  extern int fm_get_volume_list (int *buf);
  extern int fm_format (int drivenum, long fattype);
  extern int fm_getcwd (char *buffer, int maxlen);
  extern int fm_getdcwd (int drivenum, char *buffer, int maxlen);
  extern int fm_chdrive (int drivenum);
  extern int fm_getdrive (void);
  extern int fm_getfreespace (int drivenum, FN_SPACE * pspace);

  extern int fm_chdir (const char *dirname);
  extern int fm_mkdir (const char *dirname);
  extern int fm_rmdir (const char *dirname);

  extern int fm_findfirst (const char *filename, FN_FIND * find);
  extern int fm_findnext (FN_FIND * find);
  extern int fm_rename (const char *filename, const char *newname);
  extern int fm_move (const char *filename, const char *newname);
  extern long fm_filelength (const char *filename);

  extern int fm_close (FN_FILE * filehandle);
  extern int fm_flush (FN_FILE * filehandle);
  extern FN_FILE *fm_open (const char *filename, const char *mode);
  extern FN_FILE *fm_truncate (const char *filename, unsigned long length);
  extern int fm_ftruncate (FN_FILE *filehandle, unsigned long length);

  extern long fm_read (void *buf, long size, long size_st,
                       FN_FILE * filehandle);
  extern long fm_write (const void *buf, long size, long size_st,
                        FN_FILE * filehandle);

  extern int fm_seek (FN_FILE * filehandle, unsigned long offset, long whence);

  extern long fm_tell (FN_FILE * filehandle);
  extern int fm_getc (FN_FILE * filehandle);
  extern int fm_putc (int ch, FN_FILE * filehandle);
  extern int fm_rewind (FN_FILE * filehandle);
  extern int fm_eof (FN_FILE * filehandle);
  extern int fm_seteof (FN_FILE * filehandle);

  extern int fm_gettimedate (const char *filename, unsigned short *pmtime,
                             unsigned short *pmdate);
  extern int fm_settimedate (const char *filename, unsigned short mtime,
                             unsigned short mdate);
  extern int fm_delete (const char *filename);
  extern int fm_stat (const char *filename, F_STAT * stat);

  extern int fm_getattr (const char *filename, unsigned char *attr);
  extern int fm_setattr (const char *filename, unsigned char attr);

  extern int fm_getlabel (int drivenum, char *label, long len);
  extern int fm_setlabel (int drivenum, const char *label);

  extern int fm_get_oem (int drivenum, char *str, long maxlen);

  extern void fm_enable_lfn_support (char enabled);

#ifdef HCC_UNICODE
  extern int fm_wgetcwd (wchar * buffer, int maxlen);
  extern int fm_wgetdcwd (int drivenum, wchar * buffer, int maxlen);
  extern int fm_wchdir (const wchar * dirname);
  extern int fm_wmkdir (const wchar * dirname);
  extern int fm_wrmdir (const wchar * dirname);
  extern int fm_wfindfirst (const wchar * filename, FN_WFIND * find);
  extern int fm_wfindnext (FN_WFIND * find);
  extern int fm_wrename (const wchar * filename, const wchar * newname);
  extern int fm_wmove (const wchar * filename, const wchar * newname);
  extern long fm_wfilelength (const wchar * filename);
  extern FN_FILE *fm_wopen (const wchar * filename, const wchar * mode);
  extern FN_FILE *fm_wtruncate (const wchar * filename, unsigned long length);
  extern int fm_wstat (const wchar * filename, F_STAT * stat);
  extern int fm_wgettimedate (const wchar * filename, unsigned short *pmtime,
                              unsigned short *pmdate);
  extern int fm_wsettimedate (const wchar * filename, unsigned short mtime,
                              unsigned short mdate);
  extern int fm_wdelete (const wchar * filename);
  extern int fm_wgetattr (const wchar * filename, unsigned char *attr);
  extern int fm_wsetattr (const wchar * filename, unsigned char attr);
#endif


#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fat_m.h
 *
 ***************************************************************************/

#endif                          /* _FAT_M_H_ */
