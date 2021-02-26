/****************************************************************************
 * hfat_fat.h
 *
 * Short description.
 * Copyright (C) 2006-2010 QUALCOMM Technologies, Inc.
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
2009-06-15   ds    Added configuration of long filename support.
2009-04-23   yog   Added new param seek_extra_offset in struct _FN_FILEINT. 
2009-04-03   wek   Added methods to get/set access/mod time, for efs_utime.
2009-03-17   wek   Renamed var ctime/cdate to mtime/mdate to avoid confusion.
2008-10-20   yg    Made de cache calls as static.
2008-09-24   ebb   Stripped USE_MALLOC code.
2008-08-12   np    Cache HFAT last name-directoryentry pair for performance.
2008-01-30   ebb   Added data structures to support MBR and partition table
2008-01-24   sch   Added fn_getsize_from_fp
2007-02-21   yg    Added IS_ALPHA Macro
2007-02-13   yg    Added seperate flags for name ext case conversion.
2007-02-08   sch   Dropped in HFAT 3.22
2006-12-04   sch   Dropped in HFAT 3.16
2006-11-09   sch   Added udata write handlers to the F_DRIVER structure
2006-10-06   sch   Dropped in HFAT 3.04 that added F_STAT structure
2006-07-06   sch   Create

===========================================================================*/
#ifndef _HCC_FAT_H_
#define _HCC_FAT_H_

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

#include "hfat_udefs_f.h"
#include "hfat_api_f.h"
#include "comdef.h" /* for uint32 */

#define F_SECTOR_SIZE 512

  typedef struct
  {
    unsigned char jump_code[3];
    unsigned char OEM_name[8];
    unsigned short bytes_per_sector;
    unsigned char sector_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_FATs;
    unsigned short max_root_entry;
    unsigned short number_of_sectors_less32;    /*  <32M  */
    unsigned char media_descriptor;
    unsigned short sector_per_FAT;
    unsigned short sector_per_Track;
    unsigned short number_of_heads;
    unsigned long number_of_hidden_sectors;
    unsigned long number_of_sectors;

    /* only on fat32 */
    unsigned long sector_per_FAT32;
    unsigned short extflags;
    unsigned short fsversion;
    unsigned long rootcluster;
    unsigned short fsinfo;
    unsigned short bkbootsec;
    unsigned char reserved[12];

    /* fat12-fat16-fat32 */
    unsigned short logical_drive_num;
    unsigned char extended_signature;
    unsigned long serial_number;
    unsigned char volume_name[11];
    unsigned char FAT_name[8];
    unsigned char executable_marker[2];
  } F_BOOTRECORD;

/* number of sectors after mbr */
#define F_SPACE_AFTER_MBR       63

/* media descriptor to be set in getphy function */
#define F_MEDIADESC_REMOVABLE   0xf0
#define F_MEDIADESC_FIX                 0xf8

/* Field within MBR that specifies whether we boot from partition */
#define F_PARTITION_BOOTABLE    0x80
#define F_PARTITION_NONBOOTABLE 0x0
  
  /* This structure contains the information for a single 
  *  partition on disk. If the fat is going on SD there should 
  *  be only 1 partition table filled in.  CHS information is 
  *  approximated based on disk size for compatibility */
  typedef struct
  {
    unsigned char boot_indicator;       /* Indicates if partition is bootable */
    unsigned char starting_head;        /* Head where partition begins */
    unsigned short starting_sector_cyl;  /* Sector/cylinder where part begins */
    unsigned char system_id;            /* ID related to partition type/ size*/
    unsigned char ending_head;          /* Head where partition ends */
    unsigned short ending_sector_cyl;   /* sector/cylinder where part ends */
    unsigned long relative_sector;     /* Sectors before partition boot sec */
    unsigned long total_sector;        /* Total sectors in partition */ 
  } F_PARTITION_TB;


  typedef struct
  {
    /* Placeholder where exe would reside if booting from disk - 446 bytes */
    unsigned char *executable;
    F_PARTITION_TB partition1;        /* Should be the only active partition */
    F_PARTITION_TB partition2;        /* Should be all zeros */
    F_PARTITION_TB partition3;        /* Should be all zeros */
    F_PARTITION_TB partition4;        /* Should be all zeros */
    unsigned short signature;         /* MBR signature - always 0xAA55 */
  } F_MASTER_BOOTRECORD;

#if F_LONGFILENAME

  typedef struct
  {
    W_CHAR name[261];           /* with zero term */
    unsigned char ord;
    unsigned char chksum;
    unsigned char state;
    unsigned long start;
    unsigned long end;
  } F_LFNINT;

  enum
  {
                                /* 0 */ F_LFNSTATE_LFN,
                                /* lfn is useable */
                                /* 1 */ F_LFNSTATE_SFN,
                                /* lfn is useable, contains short filename */
                                /* 2 */ F_LFNSTATE_NEXT,
                                /* lfn need more entry */
                                /* 3 */ F_LFNSTATE_INV
                                /* invalid lfn */
  };

#endif                          /* F_LONGFILENAME */

  typedef struct
  {
    char name[F_MAXNAME];       /* 8+3 filename */
    char ext[F_MAXEXT];         /* 8+3 extension */
    unsigned char attr;         /* 00ADVSHR */

    unsigned char ntres;
    unsigned char crttimetenth;
    unsigned char crttime[2];
    unsigned char crtdate[2];
    unsigned char lastaccessdate[2];

    unsigned char clusterhi[2]; /* FAT32 only */
    unsigned char mtime[2];
    unsigned char mdate[2];
    unsigned char clusterlo[2]; /* fat12,fat16,fat32 */
    unsigned char filesize[4];
  } F_DIRENTRY;

/* 1st char in 8+3 if entry is deleted*/
#define F_DELETED_CHAR ((char)0xe5)

/* lower case name */
#define NTRES_NAME_LOW 0x08

/* lower case ext */
#define NTRES_EXT_LOW 0x10

/* define for long filename entry in directory entry*/
#define F_ATTR_LFN      (F_ATTR_VOLUME|F_ATTR_SYSTEM|F_ATTR_HIDDEN|F_ATTR_READONLY)

#if F_LONGFILENAME

  typedef struct
  {
    unsigned char ord;

    unsigned char lfn_1;
    unsigned char lfnhi_1;

    unsigned char lfn_2;
    unsigned char lfnhi_2;

    unsigned char lfn_3;
    unsigned char lfnhi_3;

    unsigned char lfn_4;
    unsigned char lfnhi_4;

    unsigned char lfn_5;
    unsigned char lfnhi_5;

    unsigned char attr;         /* 00ADVSHR */
    unsigned char type;         /* always 0 */

    unsigned char chksum;

    unsigned char lfn_6;
    unsigned char lfnhi_6;

    unsigned char lfn_7;
    unsigned char lfnhi_7;

    unsigned char lfn_8;
    unsigned char lfnhi_8;

    unsigned char lfn_9;
    unsigned char lfnhi_9;

    unsigned char lfn_10;
    unsigned char lfnhi_10;

    unsigned char lfn_11;
    unsigned char lfnhi_11;

    unsigned char clusterlo[2]; /* fat12,fat16,fat32 */

    unsigned char lfn_12;
    unsigned char lfnhi_12;

    unsigned char lfn_13;
    unsigned char lfnhi_13;

  } F_LFN;

#endif                          /* F_LONGFILENAME  */

/* definitions for FAT entry */
#define F_CLUSTER_FREE     ((unsigned long)0x00000000)
#define F_CLUSTER_RESERVED ((unsigned long)0x0ffffff0)
#define F_CLUSTER_BAD      ((unsigned long)0x0ffffff7)
#define F_CLUSTER_LAST     ((unsigned long)0x0ffffff8)
#define F_CLUSTER_LASTF32R ((unsigned long)0x0fffffff)

  typedef struct
  {
    unsigned long sector;       /* start sector */
    unsigned long num;          /* number of sectors */
  } F_SECTOR;


#if F_MAXFILES>0xffff           /* maximum number of files */
#error F_MAXFILES should be less than 65535
#elif F_MAXFILES>0x7fff
#define F_MAXFILES_SHIFT 16
#elif F_MAXFILES>0x3fff
#define F_MAXFILES_SHIFT 15
#elif F_MAXFILES>0x1fff
#define F_MAXFILES_SHIFT 14
#elif F_MAXFILES>0x0fff
#define F_MAXFILES_SHIFT 13
#elif F_MAXFILES>0x07ff
#define F_MAXFILES_SHIFT 12
#elif F_MAXFILES>0x03ff
#define F_MAXFILES_SHIFT 11
#elif F_MAXFILES>0x01ff
#define F_MAXFILES_SHIFT 10
#elif F_MAXFILES>0x00ff
#define F_MAXFILES_SHIFT 9
#elif F_MAXFILES>0x007f
#define F_MAXFILES_SHIFT 8
#elif F_MAXFILES>0x003f
#define F_MAXFILES_SHIFT 7
#elif F_MAXFILES>0x001f
#define F_MAXFILES_SHIFT 6
#elif F_MAXFILES>0x000f
#define F_MAXFILES_SHIFT 5
#else
#define F_MAXFILES_SHIFT 4
#endif

/*  definitions for FN_FILE internally used  */

  typedef struct
  {
    int N;
    char *ptr;
    F_POS *pos;
    F_POS posbuf[WR_DATACACHE_SIZE];
  } t_WrDataCache;

  typedef struct FN_FILEINT FN_FILEINT;

  typedef struct FN_FILEINT
  {
    FN_FILE file;
    long modified;
    char mode;
    int drivenum;               /*  0-A 1-B 2-C  */
    unsigned long abspos;
    unsigned long relpos;
    unsigned long filesize;
    unsigned char data[F_SECTOR_SIZE];
    int datawritten;
    t_WrDataCache WrDataCache;
    unsigned long startcluster;
    F_POS pos;
    F_POS dirpos;
    long state;
    long beyond_eof_offset;
#if F_MAXSEEKPOS
    long seekpos[F_MAXSEEKPOS];
    long seekprev[F_MAXSEEKPOS];
    long seekshift;
#endif
    FN_FILEINT *syncfile;
  } _FN_FILEINT;

/* this bit signal if synchronization is required in append and read in state */
#define F_FILE_ST_SYNC 0x0001
#define F_FILE_ST_EOF  0x0002

  typedef struct
  {
    unsigned long clfree;
    unsigned long clused;
    unsigned long clbad;
  } F_CLSPACE;

#ifdef FATCACHE_ENABLE
  typedef struct
  {
    unsigned long sector;
    int modified;
  } t_CacheDsc;

  typedef struct
  {
    int N;
    t_CacheDsc *dsc;
    t_CacheDsc dsc_array[FATCACHE_SIZE];
    unsigned char data[FATCACHE_SIZE * F_SECTOR_SIZE];
  } t_FatCache;
#endif

  /* The following data structure is used to save the last
   * used/found directory entry and its related information.
   */
  typedef struct
  {
    W_CHAR path[FN_MAXPATH];    /*  pathname    */
    W_CHAR lname[F_MAXLNAME];   /* file or directory name */
    F_DIRENTRY de;             /* directory entry. */
    F_POS pos;
    unsigned char used;     /* whether the entry is used. */
  } t_CachedDe;

  typedef struct
  {
    long state;
    
    F_MASTER_BOOTRECORD mbr;
    F_BOOTRECORD bootrecord;

    F_SECTOR firstfat;

    F_SECTOR root;
    F_SECTOR data;

#ifdef FATCACHE_ENABLE
    t_FatCache fatcache;
    unsigned char *fat;
#else
    unsigned char fat[F_SECTOR_SIZE];
#endif
    unsigned long fatsector;
    long fatmodified;

#if F_LONGFILENAME
#ifdef DIRCACHE_ENABLE
    unsigned char dircache[DIRCACHE_SIZE * F_SECTOR_SIZE];
    unsigned long dircache_start;
    unsigned long dircache_size;
#endif
#endif
    t_CachedDe last_entry;
    unsigned char direntry[F_SECTOR_SIZE];
    unsigned long direntrysector;

    unsigned long lastalloccluster;

    W_CHAR *cwd;

    long mediatype;
    F_CLSPACE clspace;          /* calculated disk space */
    char cspaceok;
    int partition;
    unsigned long sectorstart;
    unsigned long sectornum;
    F_PHY phy;

    F_DRIVER *driver;

    unsigned char sectorbuffer[F_SECTOR_SIZE];

  } F_VOLUME;

#define F_FAT12_MAX_CLUSTER 0xFF0
#define F_FAT16_MAX_CLUSTER 0xFFF0

  typedef struct
  {
    F_VOLUME volumes[FN_MAXVOLUME];     /* volumes */
    FN_FILEINT files[F_MAXFILES];
  } FN_FILESYSTEM;

  extern FN_FILESYSTEM f_filesystem;


#if (!FN_CAPI_USED)
  typedef struct
  {
    long ID;                    /* task id */
    int f_curdrive;             /* current drive */

    struct
    {
      W_CHAR cwd[FN_MAXPATH];   /* current working folders in this volume */
    } f_vols[FN_MAXVOLUME];

    FN_MUTEX_TYPE *pmutex;
    unsigned char current_bank;
    int lasterror;              /* last error in this task */
  } F_MULTI;
#endif

/*  current file opening modes */

  enum
  {
/*      0 */ FN_FILE_CLOSE,
/*      1 */ FN_FILE_RD,
/*      2 */ FN_FILE_WR,
/*      3 */ FN_FILE_A,
/*      4 */ FN_FILE_RDP,
/*      5 */ FN_FILE_WRP,
/*      6 */ FN_FILE_AP,
/*      7 */ FN_FILE_WRERR
  };

#define FN_FILE_ABORT_FLAG 0x40 /* signal for file is aborted */

/*  current drive modes */

  enum
  {
/*  0 */ F_STATE_NONE,
/*  1 */ F_STATE_NEEDMOUNT,
/*  2 */ F_STATE_WORKING
  };

/****************************************************************************
 *
 * externed functions
 *
 ***************************************************************************/

  extern int fn_getsize_from_fp (F_FILE *fp, uint32 *numbytes);
  extern int fn_init (void);
  extern char *fn_getversion (void);
  extern int fn_delvolume (F_MULTI * fm, int drvnumber);
  extern int fn_get_volume_count (F_MULTI * fm);
  extern int fn_get_volume_list (F_MULTI * fm, int *buf);
  extern int fn_checkvolume (F_MULTI * fm, int drvnumber);
  extern int fn_format (F_MULTI * fm, int drivenum, long fattype);
  extern int fn_getcwd (F_MULTI * fm, char *buffer, int maxlen);
  extern int fn_getdcwd (F_MULTI * fm, int drivenum, char *buffer,
                         int maxlen);
  extern int fn_chdrive (F_MULTI * fm, int drivenum);
  extern int fn_getdrive (F_MULTI * fm);
  extern int fn_getfreespace (F_MULTI * fm, int drivenum, FN_SPACE * pspace);

  extern int fn_chdir (F_MULTI * fm, const char *dirname);
  extern int fn_mkdir (F_MULTI * fm, const char *dirname);
  extern int fn_rmdir (F_MULTI * fm, const char *dirname);

  extern int fn_findfirst (F_MULTI * fm, const char *filename,
                           FN_FIND * find);
  extern int fn_findnext (F_MULTI * fm, FN_FIND * find);
  extern int fn_rename (F_MULTI * fm, const char *filename,
                        const char *newname);
  extern int fn_move (F_MULTI * fm, const char *filename,
                      const char *newname);
  extern long fn_filelength (F_MULTI * fm, const char *filename);

  extern int fn_close (F_MULTI * fm, FN_FILE * filehandle);
  extern int fn_flush (F_MULTI * fm, FN_FILE * file);
  extern FN_FILE *fn_open (F_MULTI * fm, const char *filename,
                           const char *mode);
  extern FN_FILE *fn_truncate (F_MULTI * fm, const char *filename,
                               unsigned long length);

  extern long fn_read (F_MULTI * fm, void *buf, long size, long size_st,
                       FN_FILE * filehandle);
  extern long fn_write (F_MULTI * fm, const void *buf, long size,
                        long size_st, FN_FILE * filehandle);

  extern int fn_seek (F_MULTI * fm, FN_FILE * filehandle, unsigned long offset,
                      long whence);
  extern int fn_seteof (F_MULTI * fm, FN_FILE * filehandle);

  extern long fn_tell (F_MULTI * fm, FN_FILE * filehandle);
  extern int fn_getc (F_MULTI * fm, FN_FILE * filehandle);
  extern int fn_putc (F_MULTI * fm, int ch, FN_FILE * filehandle);
  extern int fn_rewind (F_MULTI * fm, FN_FILE * filehandle);
  extern int fn_eof (F_MULTI *, FN_FILE * filehandle);

  extern int fn_gettimedate (F_MULTI * fm, const char *filename,
                             unsigned short *pmtime, unsigned short *pmdate);
  extern int fn_settimedate (F_MULTI * fm, const char *filename,
                             unsigned short mtime, unsigned short mdate);
  extern int fn_setaccessdate (F_MULTI * fm, const char *filename,
                               unsigned short adate);
  extern int fn_getaccessdate (F_MULTI * fm, const char *filename,
                               unsigned short *padate);
  extern int fn_getaccmodtime (F_MULTI * fm, const char * filename,
                                unsigned short *pmtime, unsigned short *pmdate,
                                unsigned short *padate);
  extern int fn_setaccmodtime (F_MULTI * fm, const char * filename,
                                unsigned short mtime, unsigned short mdate,
                                unsigned short adate);
  extern int fn_delete (F_MULTI * fm, const char *filename);
  extern int fn_stat (F_MULTI * fm, const char *filename, F_STAT * stat);

  extern int fn_getattr (F_MULTI * fm, const char *filename,
                         unsigned char *attr);
  extern int fn_setattr (F_MULTI * fm, const char *filename,
                         unsigned char attr);

  extern int fn_getlabel (F_MULTI * fm, int drivenum, char *label, long len);
  extern int fn_setlabel (F_MULTI * fm, int drivenum, const char *label);

  extern int fn_get_oem (F_MULTI * fm, int drivenum, char *str, long maxlen);
  
  extern void fn_enable_lfn_support (char enabled); 
#ifdef HCC_UNICODE
  extern int fn_wgetcwd (F_MULTI * fm, wchar * buffer, int maxlen);
  extern int fn_wgetdcwd (F_MULTI * fm, int drivenum, wchar * buffer,
                          int maxlen);
  extern int fn_wchdir (F_MULTI * fm, const wchar * dirname);
  extern int fn_wmkdir (F_MULTI * fm, const wchar * dirname);
  extern int fn_wrmdir (F_MULTI * fm, const wchar * dirname);
  extern int fn_wfindfirst (F_MULTI * fm, const wchar * filename,
                            FN_WFIND * find);
  extern int fn_wfindnext (F_MULTI * fm, FN_WFIND * find);
  extern int fn_wrename (F_MULTI * fm, const wchar * filename,
                         const wchar * newname);
  extern int fn_wmove (F_MULTI * fm, const wchar * filename,
                       const wchar * newname);
  extern long fn_wfilelength (F_MULTI * fm, const wchar * filename);
  extern FN_FILE *fn_wopen (F_MULTI * fm, const wchar * filename,
                            const wchar * mode);
  extern FN_FILE *fn_wtruncate (F_MULTI * fm, const wchar * filename,
                                unsigned long length);
  extern int fn_wstat (F_MULTI * fm, const wchar * filename, F_STAT * stat);
  extern int fn_wgettimedate (F_MULTI * fm, const wchar * filename,
                              unsigned short *pmtime, unsigned short *pmdate);
  extern int fn_wsettimedate (F_MULTI * fm, const wchar * filename,
                              unsigned short mtime, unsigned short mdate);
  extern int fn_wgetaccessdate (F_MULTI * fm, const wchar *filename,
                               unsigned short *padate);
  extern int fn_wsetaccessdate (F_MULTI * fm, const wchar *filename,
                               unsigned short adate);
  extern int fn_wgetaccmodtime (F_MULTI * fm, const wchar *filename,
                                unsigned short *pmtime, unsigned short *pmdate,
                                unsigned short *padate);
  extern int fn_wsetaccmodtime (F_MULTI * fm, const wchar *filename,
                                unsigned short mtime, unsigned short mdate,
                                unsigned short adate);
  extern int fn_wdelete (F_MULTI * fm, const wchar * filename);
  extern int fn_wgetattr (F_MULTI * fm, const wchar * filename,
                          unsigned char *attr);
  extern int fn_wsetattr (F_MULTI * fm, const wchar * filename,
                          unsigned char attr);
#endif

/*
 * Initialize the hfat_library
 */
int
hfat_lfn_init (F_VOLUME *vi);

#include "hfat_m.h"

#define _f_toupper(ch) (((ch)>='a' && (ch)<='z') ? ((ch)-'a'+'A') : (ch))

#define IS_ALPHA(x) ((((x) >= 'a') && ((x) <= 'z')) || (((x) >= 'A') && ((x) <= 'Z')))

#ifdef HCC_UNICODE
  extern wchar *_towchar (wchar * nconv, const char *s);
#endif
  extern int _f_addentry (F_VOLUME * vi, F_NAME * fsname, F_POS * pos,
                          F_DIRENTRY ** pde);
  extern int _f_getdirsector (F_VOLUME * vi, unsigned long sector);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fat.h
 *
 ***************************************************************************/

#endif                          /* _HCC_FAT_H_ */
