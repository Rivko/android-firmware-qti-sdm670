/****************************************************************************
 * hfat_lfn.c
 *
 * Long filename support for HFAT
 * Copyright (C) 2006-2012, 2014 QUALCOMM Technologies, Inc.
 *
 * Implements FAT calls for VFAT (long filename) filesystem.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-08-27   vk    Fix warning
2014-08-26   vm    Integrate string library for XBL compilation.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-10-17   wek   Use strlcpy for string copy to lower.
2011-10-13   wek   Use standard library string copy and string concatenate.
2010-07-12   dks   Fix warning for assignment of signed value to unsigned var.
2010-05-19   dks   Fixed return types for nextdirsector, opendir and seekdir.
2009-11-08   yg    Fix on target build error.
2009-10-13   ds    Combined _f_findfile and _f_findlfilewc.
2009-06-15   ds    Added configuration of long filename support.
2009-05-26   ebb   Removed from protected memory segment.
2009-05-08   yg    For rename if dest dir exist return ISDIR error.
2009-04-03   wek   Added methods to get/set access/mod time, for efs_utime.
2009-03-25   wek   Move/rename f_gettime f_getdate to fs_util.c
2009-03-17   wek   Renamed var ctime/cdate to mtime/mdate to avoid confusion.
2009-01-29   wek   Dynamic allocation of Volumes from a pool.
2008-12-11   wek   Use cached directory entry in _f_findpath.
2008-10-20   yg    Made de cache calls as static.
2008-09-24   ebb   Stripped USE_MALLOC code.
2008-08-15   yg    Validate the size parameter before using it.
2008-07-29   np    Remove trailing dots in filenames or directory names.
2008-07-29   np    Cache HFAT last name-directoryentry pair for performance.
2008-06-27   np    Added support for hfat files with spaces in the name.
2008-05-09   ebb   Added logic to allow renaming to a different case.
2008-04-28   rp    Fix klocwork warnings.
2007-11-27   yg    Fixed small case alpha writing in short entry on update.
2007-10-19   sch   In renaming, updated ntres field for new entry
2007-09-25   sch   Return EOF if there are no more dir entries to enumerate
2007-07-18   sh    Call through fs_convert, not fs_convert_utf8 directly.
2007-05-25   umr   Added support for EFS memory protection feature.
2007-05-17   sch   Fixed lint warnings
2007-04-30   sch   Returned appropriate error codes in case of directory
                   rename failure
2007-03-08   yg    Cleanup some un necessary code.
2007-03-02   umr   Updated copyright year.
2007-02-21   yg    Set NTRES flag properly instead of adding unnecessary LFN.
2007-02-16   yg    Fix per changed struct defintion.
2007-02-15   umr   Allocate F_LFNINT structure to eliminate local declaration.
2007-02-14   yg    Added OEM charset and unicode support for Short file names.
2007-02-14   umr   Allocate F_NAME structure to eliminate local declaration.
2007-02-13   yg    Do proper case conversion on name
2007-02-09   sch   Fixed Lint errors and indentation
2007-02-08   sch   Dropped in HFAT 3.22
2007-02-08   sh    Delcare path buffers through structure
2007-02-07   umr   Eliminate local declaration of file name path buffer.
2006-12-04   sch   Dropped in HFAT 3.16 that fixes filename issues
2006-10-09   sch   Dropped in HFAT 3.04 that added fn_stat and fn_wstat
2006-07-06   sch   Create

===========================================================================*/

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

#include "comdef.h"
#include "hfat_fat.h"
#include "hfat_port_f.h"
#include "hfat_common.h"
#include "fs_err.h"
#include "fs_unicode.h"
#include "fs_util.h"
#include "stringl/stringl.h"
#include "LoaderUtils.h"

#ifdef FS_UNIT_TEST
#include "assert.h"
/* These are used to test the short file name generation off target.
 * Length is more so we can put nul char and print as a string. */
unsigned char ut_short_file_name[10];
unsigned char ut_short_file_ext[4];
unsigned char ut_lfn_option;
#endif

static FN_WFIND find2;
static unsigned char find2_inuse;

//TODO: Cleanup
void wstrlcpy (W_CHAR *dest, const W_CHAR *src, size_t size);

/* Used to enable/disable long filename support. By default LFN support will
 * be enabled. Use the API f_enable_lfn_support(char enabled)to enable or
 * disable it. */
char hfat_lfn_enabled = 1;

#if F_LONGFILENAME

static int
fn_wfindnext_h (F_MULTI * fm, FN_WFIND * find, F_LFNINT *lfn);

static int
fn_wchdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname);
static int
_f_addentry_h (F_VOLUME * vi, F_NAME * fsname, F_POS * pos,
               F_DIRENTRY ** pde, F_LFNINT *lfn);

static int
fn_wrename_h (F_MULTI * fm, const wchar * filename, const wchar * newname,
              F_NAME *fsname, F_NAME *fsnewname, F_LFNINT *lfn);
static int
fn_wmove_h (F_MULTI * fm, const wchar * filename, const wchar * newname,
            F_NAME *fsname, F_NAME *fsnamenew, F_LFNINT *lfn, F_LFNINT *lfn2);

static int
fn_wmkdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname,
             F_LFNINT *lfn);

static long
fn_wfilelength_h (F_MULTI * fm, const wchar * filename, F_NAME *fsname,
                  F_LFNINT *lfn);

static FN_FILE *
fn_wopen_h (F_MULTI * fm, const wchar * filename, const wchar * mode,
            F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wstat_h (F_MULTI * fm, const wchar * filename, F_STAT * stat,
            F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wsettimedate_h (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                   unsigned short mdate, F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wgettimedate_h (F_MULTI * fm, const wchar * filename, unsigned short *pmtime,
                   unsigned short *pmdate, F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wsetaccessdate_h (F_MULTI * fm, const wchar * filename, unsigned short atime,
                     F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wgetaccessdate_h (F_MULTI * fm, const wchar * filename,
                     unsigned short *patime, F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wsetaccmodtime_h (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                     unsigned short mdate, unsigned short adate,
                     F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wgetaccmodtime_h (F_MULTI * fm, const wchar * filename,
                     unsigned short *pmtime, unsigned short *pmdate,
                     unsigned short *padate, F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wdelete_h (F_MULTI * fm, const wchar * filename, F_NAME *fsname,
              F_LFNINT *lfn);
static int
fn_wgetattr_h (F_MULTI * fm, const wchar * filename, unsigned char *attr,
               F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wsetattr_h (F_MULTI * fm, const wchar * filename, unsigned char attr,
               F_NAME *fsname, F_LFNINT *lfn);

static int
fn_wrmdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname,
             F_LFNINT *lfn);
static int
fn_setlabel_h (F_MULTI * fm, int drivenum, const char *label, F_NAME *fsname);

static FN_FILE *
fn_wtruncate_h (F_MULTI * fm, const wchar * filename, unsigned long length,
                F_NAME *fsname, F_LFNINT *lfn);


/*
 *  Directory entry cache routines
 */
static void _init_cached_de (t_CachedDe *lf);
static void _add_cached_de_entry (t_CachedDe *lf, const W_CHAR *path,
                           const W_CHAR *name, F_DIRENTRY *dep, F_POS *pos);
static F_DIRENTRY * _find_cached_de_entry (t_CachedDe *lf, const W_CHAR *path,
                                           const W_CHAR *name, F_POS *pos);
static void _delete_cached_de_entry (t_CachedDe *lf);

/****************************************************************************
 *
 * THIS MODULE IS FOR FAT WITH LONG FILE NAMES
 * PLEASE CHECK HFAT_UDEFS_F.H WITH F_LONGFILENAME DECLARATION
 *
 ***************************************************************************/

/****************************************************************************
 *
 * _towchar
 *
 * convert a string into wide char format
 *
 * INPUTS
 *
 * nconv - where to convert
 * s - original string
 *
 * RETURNS
 *
 * wide char string
 *
 ***************************************************************************/

#ifdef HCC_UNICODE
wchar *
_towchar (wchar * nconv, const char *s)
{
  int a;

  FS_ERR_FATAL ("Some called _towchar",0,0,0);
  for (a = 0; a < FN_MAXPATH - 1; a++)
  {
    unsigned char ch = (unsigned char) *s++;
    if (!ch)
      break;
    nconv[a] = ch;
  }

  nconv[a] = 0;                 /* terminates it */
  return nconv;
}
#endif

/****************************************************************************
 *
 * _fromwchar
 *
 * convert a wide char format to string
 *
 * INPUTS
 *
 * s - destination string
 * nconv - original wide char string
 *
 * RETURNS
 *
 * wide char string
 *
 ***************************************************************************/

#ifdef HCC_UNICODE
char *
_fromwchar (char *s, wchar * nconv)
{
  int a;

  FS_ERR_FATAL ("Someone called _fromwchar",0,0,0);
  for (a = 0; a < FN_MAXPATH - 1; a++)
  {
    *s++ = (unsigned char) nconv[a];
  }

  *s = 0;                       /* terminates it */
  return s;
}
#endif

/****************************************************************************
 *
 * _f_getdirsector
 *
 * read a directory sector
 *
 * INPUTS
 *
 * vi - volume pointer
 * sector - which sector is needed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
_f_getdirsector (F_VOLUME * vi, unsigned long sector)
{
  int ret = F_NO_ERROR;

  ret = _f_readsector (vi, vi->direntry, sector, 1);
  if (!ret)
    vi->direntrysector = sector;
  else
    vi->direntrysector = (unsigned long) (-1);
  return ret;
}

/****************************************************************************
 *
 * _f_writedirsector
 *
 * Write directory sector, which was last read
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
_f_writedirsector (F_VOLUME * vi)
{
  if (vi->direntrysector != (unsigned long) (-1))
  {
    /* erase the cached DE if any update happens to any directory entry
     * whether its related to the cached ones or not. */
    _delete_cached_de_entry (&vi->last_entry);
#ifdef DIRCACHE_ENABLE
    if (vi->direntrysector - vi->dircache_start < vi->dircache_size)
    {
      (void) _memcpy (vi->dircache +
                      (((vi->direntrysector) - (vi->dircache_start)) << 9),
                      vi->direntry, F_SECTOR_SIZE);
    }
#endif
#ifdef FS_UNIT_TEST
    /* We should never write small case letters into the short filename entry
     * it would render the file not operable on windows. Catch here if we
     * are about write a directory entry sector with a small case letter in
     * the short filename directory entry. */
    {
      unsigned i, j;
      F_DIRENTRY *fat_de = (F_DIRENTRY *)vi->direntry;

      for (i = 0; i < F_SECTOR_SIZE / sizeof (F_DIRENTRY); ++i)
      {
        /* Skip if long file name entry */
        if (fat_de->attr == 0x0F)
          continue;
        /* Directory entry deleted, so no validation */
        if (fat_de->name[0] == (char)F_DELETED_CHAR)
          continue;
        /* If last entry, skip. Is this the rule that nothing will follow..?? */
        if (fat_de->name[0] == 0)
          continue;

        /* Ok, its a short file name entry, now do the validation. */
        for (j = 0; j < 8; ++j)
          ASSERT(!((fat_de->name[j] <= 'z') && (fat_de->name[j] >= 'a')));

        for (j = 0; j < 3; ++j)
          ASSERT(!((fat_de->ext[j] <= 'z') && (fat_de->ext[j] >= 'a')));

        ++fat_de;
      }

    }
#endif
    return _f_writesector (vi, vi->direntry, vi->direntrysector, 1);
  }

  return F_ERR_INVALIDSECTOR;
}

/****************************************************************************
 *
 * _f_checknameinv
 *
 * checking filename for invalid char and/not wildcard character
 *
 * INPUTS
 *
 * lname - filename (e.g.: filename)
 * wcflag - wildcard flag, wild card needs to be checked
 *
 * RETURNS
 *
 * 0 - filename is valid
 * other - filename is invalid
 *
 ***************************************************************************/

#define _f_checkname(lname) _f_checknameinv(lname,0)
#define _f_checknamewc(lname) _f_checknameinv(lname,1)

static int
_f_checknameinv (const W_CHAR * lname, char wcflag)
{
  if (!lname)
    return 1;                   /* null ptr */

  if (*lname == (W_CHAR) (F_DELETED_CHAR & 0x00ff))
    return 1;                   /* not allowed to start with this */

  for (;;)
  {
    W_CHAR ch = *lname++;

    if (!ch)
      return 0;

    if (ch > 0 && ch < 0x20)
      return 1;

    if (wcflag)
    {
      if (ch == '?')
        return 1;
      if (ch == '*')
        return 1;
    }

    if (ch == '|')
      return 1;
    if (ch == '<')
      return 1;
    if (ch == '>')
      return 1;
    if (ch == '/')
      return 1;
    if (ch == '\\')
      return 1;
    if (ch == ':')
      return 1;
    if (ch == 34)
      return 1;                 /* " */
  }
}

/****************************************************************************
 *
 * is_trailing_dot_or_doubledots
 *
 * check whether filename is only dot or double dot
 *
 * INPUTS
 *
 * name - file or path name
 * name_length - The length of the name.
 *
 * RETURNS
 *
 * true if name is "..", ".", "dir/.." or "dir/."
 * false if any other case.
 *
 ***************************************************************************/

int
is_trailing_dot_or_doubledots (W_CHAR * name, int length)
{
  length--;

  /* Return if the last W_CHAR is not ".". */
  if (name[length] != '.')
    return 0;

  /* Check if name is simply "." */
  if (length == 0)
    return 1;

  /* Check if name is  "<dir>/." */
  if ((name[length-1] == '/') || (name[length-1] == '\\'))
    return 1;

  /* Return if the second last W_CHAR is not ".". */
  if (name[length-1] != '.')
    return 0;

  /* Check if it is ".." */
  if (length == 1)
    return 1;

  /* Check if it is "<dir>/.." */
  if ((name[length-2] == '/') || (name[length-2] == '\\'))
    return 1;

  return 0;
}
/****************************************************************************
 *
 * remove_trailing_dots_and_spaces
 *
 * Copy the directory or file name to the path. The trailing dots and spaces
 * are not copied except when they reduce to "." or "..".
 *
 * INPUTS
 *
 * name - name is a segment of the combined name which can be used in the path.
 *        For example,  The pathname "A:/Brian J /RedRoses.../newsong.txt"
 *        contains names "Brian J ", "RedRoses..." and "newsong.txt".
 * name_length - The length of the name.
 *
 * RETURNS
 * position at the end of the string without the dots and spaces.
 *
 ***************************************************************************/
static int
remove_trailing_dots_and_spaces (W_CHAR *name, int name_length)
{
  int i = name_length;

  /* remove end spaces */
  i--;
  while (i > 0)
  {
     if (name[i] == ' ')
     {
       name[i] = 0; /*remove the trailing spaces */
       i--;
     }
     else if (name[i] == '.')
     {
       /* If the remainder of the string contains a ".", "..",
        * "<dir>/." or "<dir>/..", we should not remove any more
        * dots.
        */
       if (is_trailing_dot_or_doubledots (name, i+1))
         break;
       name[i] = 0; /*remove the trailing dots */
       i--;
     }
     else {
       break;
     }
  }

  return ++i;
}

/****************************************************************************
 *
 * _f_setfsname
 *
 * convert a single string into F_NAME structure
 *
 * INPUTS
 *
 * name - combined name with drive,path,filename,extension used for source
 * fsname - where to fill this structure with separated drive,path,name,ext
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if name contains invalid path or name
 *
 ***************************************************************************/

static int
_f_setfsname (F_MULTI * fm, const W_CHAR * name, F_NAME * fsname)
{
  int pathpos = 0;
  int namepos = 0;
  int a;

  if (!name[0])
    return 1;                   /* no name */

  if (name[1] == ':')
  {
    int drv = _f_toupper (name[0]);
    if (drv >= 'A' && drv <= 'Z')
    {
      fsname->drivenum = drv - 'A';
      name += 2;                /* skip drive number */
    }
    else
      return 1;                 /* invalid drive */
  }
  else
  {
    fsname->drivenum = fn_getdrive (fm);
    if (fsname->drivenum == -1)
      return 1;
  }

  if (name[0] != '/' && name[0] != '\\')
  {
#ifdef HCC_UNICODE
    if (fn_wgetdcwd (fm, fsname->drivenum, fsname->path, FN_MAXPATH))
      return 1;                 /* error */
#else
    if (fn_getdcwd (fm, fsname->drivenum, fsname->path, FN_MAXPATH))
      return 1;                 /* error */
#endif
    for (pathpos = 0; fsname->path[pathpos];)
    {
      fsname->path[pathpos] = fsname->path[pathpos + 1];        /* remove starting dir separator */
      pathpos++;
    }

    pathpos--;
  }

  if (pathpos < 0)
    pathpos = 0;

  for (;;)
  {
    W_CHAR ch = *name++;

    if (!ch)
      break;

    if (ch == ':')
      return 1;                 /* not allowed */

    if (ch == '/' || ch == '\\')
    {
      if (pathpos > 0)
      {
        if (fsname->path[pathpos - 1] == '/')
          return 1;             /* not allowed double  */
        if (fsname->path[pathpos - 1] == '\\')
          return 1;             /* not allowed double  */

        if (pathpos >= FN_MAXPATH - 2)
          return 1;             /* path too long */
        fsname->path[pathpos++] = F_SEPARATORCHAR;
      }

      namepos = remove_trailing_dots_and_spaces (fsname->lname, namepos);
      for (a = 0; a < namepos; a++)
      {
        if (pathpos >= FN_MAXPATH - 2)
          return 1;             /* path too long */
        fsname->path[pathpos++] = fsname->lname[a];
      }
      namepos = 0;
      continue;
    }

    if (namepos >= F_MAXLNAME - 2)
      return 1;                 /* name too long */
    fsname->lname[namepos++] = ch;
  }

  fsname->lname[namepos] = 0;   /* terminates it */
  fsname->path[pathpos] = 0;    /* terminates it */

  namepos = remove_trailing_dots_and_spaces (fsname->lname, namepos);

  /* if fsname->lname is "" return */
  if (!namepos)
    return 2;                   /* no name */

  return 0;
}

/****************************************************************************
 *
 * _f_cpytolower
 *
 * convert characters in a string into lower case if needed for specified
 * length and copy them into the destination string. If needed change in
 * place, make the destination same as source.
 *
 * NOTE: this function expects 8 bit chars NOT WIDE 16 bit chars..!!
 *
 * INPUTS
 *
 * dst - destination where to copy converted string
 * src - source input string to convert
 * len - length of the string to copy.
 * to_lower - flag whether to change case to lower
 *                (typically part of the NTRES flag).
 *
 ***************************************************************************/

static void
_f_cpytolower (char *dst, const char *src, uint32 len, int to_lower)
{
  uint32 i;
  size_t out_len;

  out_len = strlcpy (dst, src, len);
  /* Take the minimum of len and number of bytes copied. */
  if (out_len > len)
    out_len = len;

  if (to_lower)
  {
    for (i = 0; i < out_len; ++i)
    {
      char ch = dst[i];
      if (ch >= 'A' && ch <= 'Z')
        dst[i] = (char) (ch - 'A' + 'a');
    }
  }
}

/****************************************************************************
 *
 * _f_getfilename
 *
 * create a complete filename from name and extension for the given NTRES
 * flag.
 *
 * INPUTS
 *
 * dest - where to store filename
 * name - name of the file
 * ext - extension of the file
 * ntres - ntres flag that need to be considered while doing conversion.
 *
 ***************************************************************************/

static void
_f_getfilename (W_CHAR * dest, const char *name,
                const char *ext, unsigned char ntres)
{
  struct fs_fat_shortname sfnp;
  char temp_name[9], temp_ext[4];

  _f_cpytolower (temp_name, name, 9, (ntres & NTRES_NAME_LOW));
  _f_cpytolower (temp_ext, ext, 4, (ntres & NTRES_EXT_LOW));

  sfnp.name = (unsigned char*)temp_name;
  sfnp.ext = (unsigned char*)temp_ext;

#ifdef FS_UNIT_TEST
  {
    int a;

    for (a = 0; a < 8; ++a) ut_short_file_name[a] = name[a];
    for (a = 0; a < 3; ++a) ut_short_file_ext[a] = ext[a];
  }
#endif

  (void) fs_convert->from_shortname (&sfnp, dest);
}

/****************************************************************************
 *
 * _f_addlfn
 *
 * Add next directory entry that is read into de into long filename entry.
 * Here we go on adding directory entries from de, into long file name as we
 * encounter them, in reverse order for long file name or just once for
 * short file name. After we encounter, last entry for long file name
 * or just 1 entry for short file name, the plfn would
 * have the actual file name represented by this directory entry or the
 * set of the directory entries for long file name entries.
 *
 * Copy the file name component from LFN or SFN directory entry to the buffer.
 *
 *  *de => plfn
 *
 * INPUTS
 *
 * de - directory entry to add, should not change, if modified we would be
 *      modifying the copy in the sector itself.
 * plfn - pointer to lfn structure where to build
 * num - number of directory entry
 *
 ***************************************************************************/

void
_f_addlfn (const F_DIRENTRY * de, F_LFNINT * plfn, unsigned long num)
{
  F_LFN *lfn = (F_LFN *) de;
  int pos;

  /* ntres will be set later if LFN support is enabled */
  unsigned char temp_ntres = 0;

  if ((!de->name[0]) || (de->name[0] == F_DELETED_CHAR))
  {
    plfn->state = F_LFNSTATE_INV;
    return;
  }

  /* If LFN support is disbaled, return Invalid state on seeing an LFN entry */
  if ((!hfat_lfn_enabled) && (de->attr == F_ATTR_LFN))
  {
    plfn->state = F_LFNSTATE_INV;
    return;
  }

  if (de->attr == F_ATTR_LFN)
  {
    if (_f_get16bitl (lfn->clusterlo))
    {                           /* this is always zero in lfn!! */
      plfn->state = F_LFNSTATE_INV;
      return;
    }

    pos = (int) (lfn->ord & (int) 0x3f);

    if (pos < 1 || pos > 20)
    {
      plfn->state = F_LFNSTATE_INV;
      return;
    }

    pos--;
    pos *= 13;                  /* get current char pos; */

    if (lfn->ord & 0x40)
    {                           /* if its the end of lfn (reverse order!) */
      plfn->ord = lfn->ord;
      plfn->state = F_LFNSTATE_NEXT;
      plfn->name[pos + 13] = 0; /* set zero terminate */
      plfn->chksum = lfn->chksum;
      plfn->start = num;
      plfn->end = num;
    }
    else
    {
      if (plfn->chksum != lfn->chksum)
      {
        plfn->state = F_LFNSTATE_INV;
        return;
      }

      if (plfn->state == F_LFNSTATE_NEXT)
      {
        if (lfn->ord != ((plfn->ord & 0x3f) - 1))
        {                       /* if not the next */
          plfn->state = F_LFNSTATE_INV;
          return;
        }

        plfn->ord = lfn->ord;
      }
      else
      {
        plfn->state = F_LFNSTATE_INV;
        return;
      }
    }

#ifdef HCC_UNICODE
    plfn->name[pos] = lfn->lfnhi_1;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_1;
    plfn->name[pos] = lfn->lfnhi_2;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_2;
    plfn->name[pos] = lfn->lfnhi_3;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_3;
    plfn->name[pos] = lfn->lfnhi_4;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_4;
    plfn->name[pos] = lfn->lfnhi_5;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_5;
    plfn->name[pos] = lfn->lfnhi_6;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_6;
    plfn->name[pos] = lfn->lfnhi_7;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_7;
    plfn->name[pos] = lfn->lfnhi_8;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_8;
    plfn->name[pos] = lfn->lfnhi_9;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_9;
    plfn->name[pos] = lfn->lfnhi_10;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_10;
    plfn->name[pos] = lfn->lfnhi_11;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_11;
    plfn->name[pos] = lfn->lfnhi_12;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_12;
    plfn->name[pos] = lfn->lfnhi_13;
    plfn->name[pos] <<= 8;
    plfn->name[pos++] |= lfn->lfn_13;
#else
    plfn->name[pos++] = lfn->lfn_1;
    plfn->name[pos++] = lfn->lfn_2;
    plfn->name[pos++] = lfn->lfn_3;
    plfn->name[pos++] = lfn->lfn_4;
    plfn->name[pos++] = lfn->lfn_5;
    plfn->name[pos++] = lfn->lfn_6;
    plfn->name[pos++] = lfn->lfn_7;
    plfn->name[pos++] = lfn->lfn_8;
    plfn->name[pos++] = lfn->lfn_9;
    plfn->name[pos++] = lfn->lfn_10;
    plfn->name[pos++] = lfn->lfn_11;
    plfn->name[pos++] = lfn->lfn_12;
    plfn->name[pos++] = lfn->lfn_13;
#endif
  }
  else
  {                             /* check sum check */
    if (plfn->state == F_LFNSTATE_NEXT)
    {
      if ((plfn->ord & 0x3f) == 1)
      {
        unsigned char chksum = 0;
        int a;

        for (a = 0; a < 8; a++)
        {
          chksum =
            (unsigned char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
                             de->name[a]);
        }

        for (a = 0; a < 3; a++)
        {
          chksum =
            (unsigned char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
                             de->ext[a]);
        }

        if ((chksum & 0x00ff) == plfn->chksum)  /* 0x00ff because of DSPs */
        {
          plfn->state = F_LFNSTATE_LFN;
          plfn->end = num;
          return;
        }
      }
    }

    plfn->start = plfn->end = num;      /* only short name */

    /* Use case conversion flag only if LFN support is enabled,
     * and ignore otherwise. */
    if (hfat_lfn_enabled)
      temp_ntres = de->ntres;

    /* Populate the name entry in the LFN struct which will be used
     * as a primary entry for comparisions during enumerations.
     * SFN will be used in case of LFN mismatch. */
    _f_getfilename ((W_CHAR *) plfn->name, de->name, de->ext, temp_ntres);

    plfn->state = F_LFNSTATE_SFN;
  }
}

/****************************************************************************
 *
 * _f_getintfile
 *
 * allocate an internal file handler
 *
 * RETURNS
 *
 * internal file pointer if successful
 * zero if there was no free file pointer (see F_MAXFILES)
 *
 ***************************************************************************/

static FN_FILEINT *
_f_getintfile (void)
{
  int a;

  if (f_mutex_get (&fat_gmutex))
    return 0;

  for (a = 0; a < F_MAXFILES; a++)
  {
    if (f_filesystem.files[a].mode == FN_FILE_CLOSE)
    {
      FN_FILEINT *f = &f_filesystem.files[a];

      f->relpos = 0;
      f->abspos = 0;
      f->filesize = 0;
      f->startcluster = 0;
      f->pos.prevcluster = 0;
      f->modified = 0;

      f->file.reference = (void *) ((MagicNumber << F_MAXFILES_SHIFT) + a);

      f->WrDataCache.N = 0;
      f->WrDataCache.ptr = NULL;

      (void) f_mutex_put (&fat_gmutex);

      return f;
    }
  }

  (void) f_mutex_put (&fat_gmutex);

  return 0;                     /* no more file could be opened  */
}

/****************************************************************************
 *
 * _f_opendir
 *
 * open a directory as file
 *
 * INPUTS
 *
 * vi - volumeinfo
 * drivenum - on which drive
 * cluster - directory start cluster
 *
 * RETURNS
 *
 * file pointer or zero if any error
 *
 ***************************************************************************/

static FN_FILE *
_f_opendir (F_VOLUME * vi, int drivenum, unsigned long cluster)
{
  int ret;
  FN_FILEINT *f = 0;

  f = _f_getintfile ();
  if (!f)
    return 0;                   /* no more file could be opened  */

  f->drivenum = drivenum;
  f->startcluster = cluster;

  _f_clustertopos (vi, f->startcluster, &f->pos);

#ifdef DIRCACHE_ENABLE
  {
    unsigned long n = (f->pos.sectorend) - (f->pos.sectorbegin);
    if (n > DIRCACHE_SIZE)
      n = DIRCACHE_SIZE;
    if (vi->dircache_start == (unsigned long) -1
        || (f->pos.sector) - (vi->dircache_start) >= n)
    {
      ret = _f_readsector (vi, vi->dircache, f->pos.sectorbegin, (int) n);
      if (ret)
      {
        vi->dircache_start = (unsigned long) -1;
        return 0;
      }

      vi->dircache_start = f->pos.sectorbegin;
      vi->dircache_size = n;
    }
  }
#else
  if (_f_readsector (vi, f->data, f->pos.sector, 1))
    return 0;
#endif

  f->mode = FN_FILE_RD;
  return (FN_FILE *) (f->file.reference);
}

/****************************************************************************
 *
 * _f_nextdirsector
 *
 * check and go to next directory sector
 *
 * INPUTS
 *
 * vi - volume info
 * f - filepointer
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/

static int
_f_nextdirsector (F_VOLUME * vi, FN_FILEINT * f)
{
  int ret;

  if (f->relpos == F_SECTOR_SIZE)
  {
    if (f->modified)
    {
#ifdef DIRCACHE_ENABLE
      ret =
        _f_writesector (vi,
                        vi->dircache +
                        (((f->pos.sector) - (vi->dircache_start)) << 9),
                        f->pos.sector, 1);
#else
      ret = _f_writesector (vi, f->data, f->pos.sector, 1);
#endif
      if (ret)
        return ret;
      f->modified = 0;
    }

    if ((f->pos.sector + 1) == f->pos.sectorend)
    {
      if (f->pos.cluster >= F_CLUSTER_RESERVED)
        return F_ERR_EOF;

      if (!f->pos.cluster)
      {
        if (vi->mediatype == F_FAT32_MEDIA)
        {
          f->pos.cluster = vi->bootrecord.rootcluster;
        }
        else
          return F_ERR_EOF;     /* root directory */
      }

      {
        unsigned long nextcluster;
        if (_f_getclustervalue (vi, f->pos.cluster, &nextcluster))
          return F_ERR_EOF;     /* not found */

        if (nextcluster < 2 || nextcluster >= F_CLUSTER_RESERVED)
          return F_ERR_EOF;

        _f_clustertopos (vi, nextcluster, &f->pos);
      }
    }
    else
    {
      f->pos.sector++;
    }

#ifdef DIRCACHE_ENABLE
    {
      unsigned long n = (f->pos.sectorend) - (f->pos.sectorbegin);
      if (n > DIRCACHE_SIZE)
        n = DIRCACHE_SIZE;
      if ((f->pos.sector) - (vi->dircache_start) >= n)
      {
        if (f->pos.sectorbegin + n > f->pos.sector)
        {
          ret = _f_readsector (vi, vi->dircache, f->pos.sectorbegin, (int) n);
          if (ret)
          {
            vi->dircache_start = (unsigned long) -1;
            return ret;
          }
          vi->dircache_start = f->pos.sectorbegin;
        }
        else
        {
          ret = _f_readsector (vi, vi->dircache, f->pos.sector, (int) n);
          if (ret)
          {
            vi->dircache_start = (unsigned long) -1;
            return ret;
          }
          vi->dircache_start = f->pos.sector;
        }
        vi->dircache_size = n;
      }
    }
#else
    ret = _f_readsector (vi, f->data, f->pos.sector, 1);
    if (ret)
      return ret;
#endif

    f->relpos = 0;
    f->abspos += F_SECTOR_SIZE;
  }

  return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_seekdir
 *
 * seek into directory
 *
 * INPUTS
 *
 * vi - volumeinfo
 * file - filepointer
 * offset - number of directory entries
 * whence - how to seek
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/

static int
_f_seekdir (F_VOLUME * vi, FN_FILE * file, long offset, long whence)
{
  FN_FILEINT *f = _f_check_handle (file);
  int ret;

  if (!f)
    return F_ERR_NOTOPEN;

  if (offset == -1 && whence == FN_SEEK_CUR && f->relpos)
  {
    f->relpos -= sizeof (F_DIRENTRY);
    return F_NO_ERROR;
  }

  offset *= sizeof (F_DIRENTRY);

  switch (whence)
  {
    case FN_SEEK_CUR:
      offset = f->abspos + f->relpos + offset;
      break;
    case FN_SEEK_SET:
      break;
    default:
      return F_ERR_NOTUSEABLE;
  }

  if (f->modified)
  {
#ifdef DIRCACHE_ENABLE
    ret =
      _f_writesector (vi,
                      vi->dircache +
                      (((f->pos.sector) - (vi->dircache_start)) << 9),
                      f->pos.sector, 1);
#else
    ret = _f_writesector (vi, f->data, f->pos.sector, 1);
#endif
    if (ret)
      return ret;
    f->modified = 0;
  }

  f->relpos = 0;
  f->abspos = 0;

  _f_clustertopos (vi, f->startcluster, &f->pos);

  for (;;)
  {
    if (offset < F_SECTOR_SIZE)
    {
      f->relpos = offset;

#ifdef DIRCACHE_ENABLE
      {
        unsigned long n = (f->pos.sectorend) - (f->pos.sectorbegin);
        if (n > DIRCACHE_SIZE)
          n = DIRCACHE_SIZE;
        if ((f->pos.sector) - (vi->dircache_start) >= n)
        {
          if (f->pos.sectorbegin + n > f->pos.sector)
          {
            ret = _f_readsector (vi, vi->dircache, f->pos.sectorbegin, (int) n);
            if (ret)
            {
              vi->dircache_start = (unsigned long) -1;
              return ret;
            }
            vi->dircache_start = f->pos.sectorbegin;
          }
          else
          {
            ret = _f_readsector (vi, vi->dircache, f->pos.sector, (int) n);
              if (ret)
              {
                vi->dircache_start = (unsigned long) -1;
                return ret;
              }
            vi->dircache_start = f->pos.sector;
          }
          vi->dircache_size = n;
        }
      }
#else
      ret = _f_readsector (vi, f->data, f->pos.sector, 1);
      if (ret)
        return ret;
#endif
      return F_NO_ERROR;
    }

    offset -= F_SECTOR_SIZE;
    f->abspos += F_SECTOR_SIZE;

    if ((f->pos.sector + 1) == f->pos.sectorend)
    {
      if (f->pos.cluster >= F_CLUSTER_RESERVED)
        return F_ERR_EOF;

      if (!f->pos.cluster)
      {
        if (vi->mediatype == F_FAT32_MEDIA)
        {
          f->pos.cluster = vi->bootrecord.rootcluster;
        }
        else
          return F_ERR_EOF;     /* root directory */
      }

      {
        unsigned long nextcluster;
        if (_f_getclustervalue (vi, f->pos.cluster, &nextcluster))
          return F_ERR_EOF;     /* not found */

        if (nextcluster < 2 || nextcluster >= F_CLUSTER_RESERVED)
          return F_ERR_EOF;

        _f_clustertopos (vi, nextcluster, &f->pos);
      }
    }
    else
    {
      f->pos.sector++;
    }
  }
}

/****************************************************************************
 *
 * _f_writedir
 *
 * write directory entry into directory
 *
 * INPUTS
 *
 * vi - volumeinfo
 * de - directory entry needs written
 * file - filepointer
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/

static int
_f_writedir (F_VOLUME * vi, F_DIRENTRY * de, FN_FILE * file)
{
  FN_FILEINT *f = _f_check_handle (file);
  int ret;

  if (!f)
    return F_ERR_NOTOPEN;

  ret = _f_nextdirsector (vi, f);
  if (ret)
    return ret;

#ifdef DIRCACHE_ENABLE
  (void) _memcpy (vi->dircache +
                  (((f->pos.sector) - (vi->dircache_start)) << 9) + f->relpos,
                  (void *) de, sizeof (F_DIRENTRY));
#else
  (void) _memcpy (f->data + f->relpos, (void *) de, sizeof (F_DIRENTRY));
#endif
  f->relpos += sizeof (F_DIRENTRY);

  f->modified = 1;

  return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_readdir
 *
 * read next directory entry
 *
 * INPUTS
 *
 * vi - volumeinfo
 * de - where to store information
 * file - filepointer
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/

static int
_f_readdir (F_VOLUME * vi, F_DIRENTRY * de, FN_FILE * file)
{
  FN_FILEINT *f = _f_check_handle (file);
  int ret;

  if (!f)
    return F_ERR_NOTOPEN;

  ret = _f_nextdirsector (vi, f);
  if (ret)
    return ret;

#ifdef DIRCACHE_ENABLE
  (void) _memcpy ((void *) de,
                  vi->dircache +
                  (((f->pos.sector) - (vi->dircache_start)) << 9) + f->relpos,
                  sizeof (F_DIRENTRY));
#else
  (void) _memcpy ((void *) de, f->data + f->relpos, sizeof (F_DIRENTRY));
#endif
  f->relpos += sizeof (F_DIRENTRY);

  return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_closedir
 *
 * close a directory, and carry on error code
 *
 * INPUTS
 *
 * vi - volumeinfo
 * file - file pointer
 * errcode - errorcode
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/

static int
_f_closedir (F_VOLUME * vi, FN_FILE * file, int errcode)
{
  FN_FILEINT *f = _f_check_handle (file);

  if (!f)
  {
    if (errcode)
      return errcode;
    return F_ERR_NOTOPEN;
  }

  f->mode = FN_FILE_CLOSE;

  if (f->modified)
  {
#ifdef DIRCACHE_ENABLE
    int ret = _f_writesector (vi,
                              vi->dircache +
                              (((f->pos.sector) - (vi->dircache_start)) << 9),
                              f->pos.sector, 1);
#else
    int ret = _f_writesector (vi, f->data, f->pos.sector, 1);
#endif
    if (ret)
    {
      if (errcode)
        return errcode;
      return ret;
    }
    f->modified = 0;
  }

  if (errcode)
    return errcode;
  return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_removelfn
 *
 * remove lfn entry from directory
 *
 * INPUTS
 *
 * vi - volumeinfo
 * drivenum - drive number
 * cluster - cluster of directory
 * lfn - long file name to remove (findfile fills starts and ends)
 *
 * RETURNS
 *
 * 0 - if successfully
 * or error code
 *
 ***************************************************************************/

static int
_f_removelfn (F_VOLUME * vi, int drivenum, unsigned long cluster,
              F_LFNINT * lfn)
{
  F_DIRENTRY de;
  unsigned long a;
  FN_FILE *file;
  int ret;

  if (lfn->state != F_LFNSTATE_LFN && lfn->state != F_LFNSTATE_SFN)
  {
    return F_ERR_INVALIDNAME;
  }

  file = _f_opendir (vi, drivenum, cluster);
  if (!file)
    return F_ERR_INVALIDDIR;

  ret = _f_seekdir (vi, file, lfn->start, FN_SEEK_SET);
  if (ret)
    return _f_closedir (vi, file, ret);

  for (a = lfn->start; a <= lfn->end; a++)
  {
    ret = _f_readdir (vi, &de, file);
    if (ret)
      return _f_closedir (vi, file, ret);

    ret = _f_seekdir (vi, file, -1, FN_SEEK_CUR);
    if (ret)
      return _f_closedir (vi, file, ret);

    de.name[0] = F_DELETED_CHAR;

    ret = _f_writedir (vi, &de, file);
    if (ret)
      return _f_closedir (vi, file, ret);
  }

  return _f_closedir (vi, file, F_NO_ERROR);
}

/***************************************************************************
 *
 *
 * _f_cmp_wname
 *
 * function for checking a name with wild card to original
 *
 * INPUTS
 *
 * wname - wild card name (e.g. *.* or ?a.*)
 * name - original name to check
 *
 * RETURNS
 *
 * 0 - if not equal
 * 1 - if equal or match
 *
 ***************************************************************************/

int
_f_cmp_wname (const W_CHAR * wname, const W_CHAR * name)
{
  for (;;)
  {
    W_CHAR ch = (W_CHAR) _f_toupper (*name);
    W_CHAR wch = (W_CHAR) _f_toupper (*wname);

    if ((!ch) && (!wch))
      return 1;
    if ((!wch) && (ch == '.'))
    {
      name++;
      continue;
    }
    if (wch == ch)
    {
      name++;
      wname++;
      continue;
    }
    if (wch == '?')
    {
      if (ch)
        name++;
      wname++;
      continue;
    }
    if (wch == '*')
    {
      wname++;
      wch = (W_CHAR) _f_toupper (*wname);
      wname++;
      if (!wch)
        return 1;

      for (;;)
      {
        ch = (W_CHAR) _f_toupper (*name);
        if (ch == wch)
        {
          name++;
          break;
        }
        if ((!ch) && (wch == '.'))
          break;
        if (!ch)
          return 0;
        name++;
      }
      continue;
    }
    return 0;
  }
}

typedef enum
{
  FILE_FOUND,
  END_OF_DIR,
  CONTINUE_SEARCH
}dir_search_result;


/****************************************************************************
 *
 * convert_de_to_filename
 *
 * Internal function for preparing filename and comparing it with the name
 * of the file being searched.
 *
 * INPUTS
 *
 * lname             - filename being searched
 * de                - de entry read from disk
 * lfn               - long filename structure pointer
 * name_has_wildcard - does the filename have wildcard chars in it
 *
 * RETURNS
 *
 *   dir_search_result suggesting one of three things to the calling function:
 *   FILE_FOUND      - The file was found, so stop searching
 *   END_OF_DIR      - The directory has no more entries, so stop searching
 *   CONTINUE_SEARCH - The file being searched for doesnt match this entry,
 *                     so continue to the next directory entry
 ***************************************************************************/

static dir_search_result
convert_de_to_filename (W_CHAR * lname, F_DIRENTRY * de, F_LFNINT * lfn,
                        int name_has_wildcard)
{
  /* ntres will be set later if LFN support is enabled */
  unsigned char temp_ntres = 0;
  W_CHAR sname[8 + 3 + 2];
  unsigned int b;

  if (!de->name[0])
    return END_OF_DIR;
  if (de->name[0] == F_DELETED_CHAR)
    return CONTINUE_SEARCH;
  if (de->attr & F_ATTR_VOLUME)
    return CONTINUE_SEARCH;

  /* A file should be accessible by both its long filename and the stored
   * short filename. We comapre the long filename (the actual filename)
   * first. If this fails, then compare the shortname. In case of disabled
   * LFN support, a file with a longfile name can still be accessed through
   * its short filename. In this case when the search is done on its stored
   * shortname, the below long filename comparision should fail, but results
   * in a successful shortname comaprion in the next step */
  if (name_has_wildcard)
  {
    if (_f_cmp_wname (lname, lfn->name))
    {
      return FILE_FOUND;
    }
  }

  for (b = 0;; b++)
  {
    W_CHAR ch = (W_CHAR) _f_toupper (lfn->name[b]);
    if (ch != _f_toupper (lname[b]))
      break;              /* differs */
    if (!ch)
    {
      return FILE_FOUND;
    }
  }

  /* Use case conversion flag only if LFN support is enabled,
   * and ignore otherwise */
  if (hfat_lfn_enabled)
    temp_ntres = de->ntres;

  /* generate the short name for comparision */
  _f_getfilename (sname, (char *) de->name, (char *) de->ext, temp_ntres);

  /* The efs_readdir call always searches for files using the name
   * "*.*", which will always match any file, in the above long filename
   * comparision. The HAFT call fm_findnext, however, is a public API
   * and if any other wildcard comparision which invlolving partial filenames
   * is used, below short filename comparision will still be useful */
  if (name_has_wildcard)
  {
    if (de->ext[0] == ' ')
    {
      int i;
      for (i = 0; sname[i]; i++);
      sname[i] = '.';
      sname[i + 1] = 0;
    }

    if (_f_cmp_wname (lname, sname))
    {
      return FILE_FOUND;
    }
  }
  for (b = 0;; b++)
  {
    W_CHAR ch = (W_CHAR) _f_toupper (sname[b]);
    if (ch != _f_toupper (lname[b]))
      break;              /* differs */
    if (!ch)
    {
      return FILE_FOUND;
    }
  }

  return CONTINUE_SEARCH;
}

/****************************************************************************
 *
 * _f_findfile
 *
 * Internal function to finding file in directory entry.
 * Reads successive directory entries in the parent, till it match is found
 * or the end of directory is reached.
 *
 * INPUTS
 *
 * vi                - volumeinfo structure, where to find file
 * lname             - filename
 * pos               - where to start searching, and contains current position
 *                 (initialized with the start sector for the parent directory)
 * pde               - store back the directory entry pointer
 * lfn               - long file name structure pointer
 * name_has_wildcard - does the filename have wildcard chars in it

 * RETURNS
 *
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/

static int
_f_findfile (F_VOLUME * vi, W_CHAR * lname, F_POS * pos, F_DIRENTRY ** pde,
             F_LFNINT * lfn, int name_has_wildcard)
{
  unsigned long denum = 0;

  /* Initialize the LFN */
  lfn->state = F_LFNSTATE_INV;
  lfn->start = (unsigned long) (-1);
  lfn->end = (unsigned long) (-1);

  /* Read the directory entries from the disk till the end of directory
   * entries for the parent directory is reached or till the reserved cluster,
   * whichever comes first. If a directory is very large, the entires might
   * span across different sectors and clusters */
  while (pos->cluster < F_CLUSTER_RESERVED)
  {
    for (; pos->sector < pos->sectorend; pos->sector++)
    {
      /* Read the sector pointed to by pos into the vi direnrty cache */
      F_DIRENTRY *de =
        (F_DIRENTRY *) (vi->direntry + sizeof (F_DIRENTRY) * pos->pos);

      if (_f_getdirsector (vi, pos->sector))
        return 0;               /* not found */

      /* Search all the directory entries present in the sector read */
      for (; pos->pos < F_SECTOR_SIZE / sizeof (F_DIRENTRY); de++, pos->pos++)
      {
        dir_search_result srch_res;

        /* Populate the name entry struct which will be used
         * as a primary entry for comparisions during enumerations.
         * SFN will be used in case of LFN mismatch. */
         _f_addlfn (de, lfn, denum);

         /* used to track the directory entry number within the directory */
         denum++;

        /* Check if the search should continue */
        srch_res = convert_de_to_filename (lname, de, lfn, name_has_wildcard);

        switch (srch_res)
        {
          case FILE_FOUND:
            if (pde)
              *pde = de;
            return 1;
          case END_OF_DIR:
            return 0;
          case CONTINUE_SEARCH:
          default:
            break;
        }

      }  /* not in this sector, search in the next */

      pos->pos = 0;

    } /* not in this cluster, search in the next */

    /* Get the next cluster to read */

    if (!pos->cluster)
    {
      if (vi->mediatype == F_FAT32_MEDIA)
      {
        pos->cluster = vi->bootrecord.rootcluster;
      }
      else
        return 0;               /* root directory */
    }

    {
      unsigned long nextcluster;
      if (_f_getclustervalue (vi, pos->cluster, &nextcluster))
        return 0;               /* not found */

      if (nextcluster < 2 || nextcluster >= F_CLUSTER_RESERVED)
        return 0;               /* not found */

      _f_clustertopos (vi, nextcluster, pos);
    }
  }

  return 0;
}

/****************************************************************************
 *
 * _f_getnextstring
 *
 * finding in a string a string until / or \ termination
 *
 * INPUTS
 *
 * dest - where to store next string
 * sou - original text
 *
 * RETURNS
 *
 * new start position in the original
 *
 ***************************************************************************/

static W_CHAR *
_f_getnextstring (W_CHAR * dest, W_CHAR * sou)
{
  W_CHAR *tmp_dest = dest;
  int len = 0;

  for (;;)
  {
    W_CHAR ch = *sou;
    if (!ch)
      break;
    sou++;
    if (ch == '/' || ch == '\\')
      break;

    *dest++ = ch;
    len++;
  }
  *dest = 0;

  /* remove trailing end dots when it is not "." or ".." */
  remove_trailing_dots_and_spaces (tmp_dest, len);
  return sou;
}

/****************************************************************************
 *
 * isonlydotsandspaces
 *
 * check whether filename is only spaces and/or dots
 *
 * INPUTS
 *
 * lname - original long file name
 *
 * RETURNS
 *
 * true if it contains only dots and/or spaces
 * false if any other character
 *
 ***************************************************************************/

int
isonlydotsandspaces (W_CHAR * lname)
{
  for (;;)
  {
    W_CHAR ch = *lname++;
    if (!ch)
      return 1;                 /* only dots and/or spaces */
    if (ch == ' ')
      continue;
    if (ch != '.')
      return 0;                 /* not only dots and spaces */
  }
}

/****************************************************************************
 *
 * isonlydotordoubledots
 *
 * check whether filename is only dot or double dot
 *
 * INPUTS
 *
 * lname - original long file name
 *
 * RETURNS
 *
 * true if it contains only dot or double dot
 * false if any other character
 *
 ***************************************************************************/

int
isonlydotordoubledots (W_CHAR * lname)
{
  if (lname[0] != '.')
    return 0;
  if (lname[1] == 0)
    return 1;                   /* simple dot */
  if (lname[1] != '.')
    return 0;
  if (lname[2] == 0)
    return 1;                   /* double dots */

  return 0;
}

/****************************************************************************
 *
 * _f_findpath
 *
 * finding out if path is valid in F_NAME and
 * correct path info with absolute path (removes relatives)
 *
 * INPUTS
 *
 * vi - volumename
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 *
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 ***************************************************************************/

static int
_f_findpath (F_VOLUME * vi, F_NAME * fsname, F_POS * pos)
{
  struct hfat_path_buffer *curr_hpb, *orig_hpb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;
  W_CHAR *currname;
  W_CHAR *oripath;
  F_LFNINT *lfn;
  int a;
  F_DIRENTRY *de;
  int pathpos = 0;
  W_CHAR *path = fsname->path;

  _f_clustertopos (vi, 0, pos);
  de = _find_cached_de_entry (&vi->last_entry, path, NULL, pos);

  if (de != NULL)
  {
    _f_clustertopos (vi, _f_getdecluster (vi, de), pos);
    return 1;
  }

  curr_hpb = hfat_allocate_path_buffer (&currname);
  orig_hpb = hfat_allocate_path_buffer (&oripath);
  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (&lfn);
  oripath[0] = 0;

  for (; *path;)
  {
    path = _f_getnextstring (currname, path);

    if ((pos->cluster == 0) && (currname[0] == '.') && (!currname[1]))
    {
      _f_clustertopos (vi, 0, pos);
      continue;
    }
    else
    {
      if (!_f_findfile (vi, currname, pos, &de, lfn, 0))
        goto Error;
      if (!(de->attr & F_ATTR_DIR))
        goto Error;

      _f_clustertopos (vi, _f_getdecluster (vi, de), pos);
    }

    if (isonlydotordoubledots (lfn->name))
    {
      if (!lfn->name[1])
      {
        continue;               /* nothing to modify */
      }
      else if (lfn->name[1] == '.')
      {
        if (lfn->name[2])
          goto Error;             /* more than 2 dots */

        if (!pathpos)
          goto Error;             /* we are on the top */

        /* set back with one level */
        for (; pathpos;)
        {
          pathpos--;
          if (oripath[pathpos] == F_SEPARATORCHAR)
            break;
        }
      }
    }
    else
    {
      if (isonlydotsandspaces (lfn->name))
        goto Error;

      if (pathpos)
        oripath[pathpos++] = F_SEPARATORCHAR;
      for (a = 0;;)
      {
        W_CHAR ch = lfn->name[a++];
        if (!ch)
          break;
        oripath[pathpos++] = ch;
      }
    }
  }
  oripath[pathpos] = 0;         /* terminates it; */
  (void) _memcpy ((void *) (fsname->path), oripath, sizeof (fsname->path));
  hfat_free_path_buffer (curr_hpb);
  hfat_free_path_buffer (orig_hpb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);

  return 1;                     /* found */

Error:
  hfat_free_path_buffer (curr_hpb);
  hfat_free_path_buffer (orig_hpb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return 0;
}

/****************************************************************************
 *
 * fn_getdcwd
 *
 * getting a drive current working directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * drivenum - drive number of which drive current folder needed
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is FN_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_getdcwd (F_MULTI * fm, int drivenum, char *buffer, int maxlen)
{
  F_VOLUME *vi;
  int a;
  W_CHAR *cwd;
  int ret;
  int8 index;

  index = _f_drivenum_to_index (drivenum);
  if (index < 0)
    return F_SETLASTERROR (F_ERR_INVALIDDRIVE);
  vi = &f_filesystem.volumes[index];

  cwd = fm->f_vols[index].cwd;

  ret = _f_mutex_get (fm, vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (_f_checkstatus (vi, 0))
  {
    fm->f_vols[index].cwd[0] = 0;    /* remove cwd */
    vi->state = F_STATE_NEEDMOUNT;      /* mount is needed */
  }

  if (!maxlen)
    return F_SETLASTERROR (F_NO_ERROR);

  maxlen--;                     /* need for termination */
  if (maxlen)
  {
    *buffer++ = F_SEPARATORCHAR;
    maxlen--;
  }


  for (a = 0; a < maxlen; a++)
  {
    char ch = (char) cwd[a];
    buffer[a] = ch;
    if (!ch)
      break;
  }

  buffer[a] = 0;                /* add terminator at the end */

  return F_SETLASTERROR (F_NO_ERROR);
}

#ifdef HCC_UNICODE
int
fn_wgetdcwd (F_MULTI * fm, int drivenum, wchar * buffer, int maxlen)
{
  F_VOLUME *vi;
  int a;
  wchar *cwd;
  int ret;
  int8 index;

  index = _f_drivenum_to_index(drivenum);
  if (index < 0)
    return F_SETLASTERROR (F_ERR_INVALIDDRIVE);
  vi = &f_filesystem.volumes[index];

  cwd = fm->f_vols[index].cwd;

  ret = _f_mutex_get (fm, vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (_f_checkstatus (vi, 0))
  {
    fm->f_vols[index].cwd[0] = 0;    /* remove cwd */
    vi->state = F_STATE_NEEDMOUNT;      /* mount is needed */
  }

  if (!maxlen)
    return F_SETLASTERROR (F_NO_ERROR);

  maxlen--;                     /* need for termination */
  if (maxlen)
  {
    *buffer++ = F_SEPARATORCHAR;
    maxlen--;
  }


  for (a = 0; a < maxlen; a++)
  {
    wchar ch = cwd[a];
    buffer[a] = ch;
    if (!ch)
      break;
  }

  buffer[a] = 0;                /* add terminator at the end */

  return F_SETLASTERROR (F_NO_ERROR);
}
#endif

/****************************************************************************
 *
 * fn_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is FN_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_getcwd (F_MULTI * fm, char *buffer, int maxlen)
{
  F_VOLUME *vi;
  int a;
  W_CHAR *cwd;
  int ret;
  int drivenum;
  int8 index;

  drivenum = fn_getdrive (fm);

  index = _f_drivenum_to_index(drivenum);
  if (index < 0)
    return F_SETLASTERROR (F_ERR_INVALIDDRIVE);
  vi = &f_filesystem.volumes[index];

  cwd = fm->f_vols[index].cwd;

  ret = _f_mutex_get (fm, vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (_f_checkstatus (vi, 0))
  {
    fm->f_vols[index].cwd[0] = 0;    /* remove cwd */
    vi->state = F_STATE_NEEDMOUNT;      /* mount is needed */
  }

  if (!maxlen)
    return F_SETLASTERROR (F_NO_ERROR);

  maxlen--;                     /* need for termination */
  if (maxlen)
  {
    *buffer++ = F_SEPARATORCHAR;
    maxlen--;
  }

  for (a = 0; a < maxlen; a++)
  {
    char ch = (char) cwd[a];
    buffer[a] = ch;
    if (!ch)
      break;
  }

  buffer[a] = 0;                /* add terminator at the end */

  return F_SETLASTERROR (F_NO_ERROR);
}


#ifdef HCC_UNICODE
int
fn_wgetcwd (F_MULTI * fm, wchar * buffer, int maxlen)
{
  F_VOLUME *vi;
  int a;
  wchar *cwd;
  int ret;
  int drivenum;
  int8 index;

  drivenum = fn_getdrive (fm);

  index = _f_drivenum_to_index(drivenum);
  if (index < 0)
    return F_SETLASTERROR (F_ERR_INVALIDDRIVE);

  vi = &f_filesystem.volumes[index];

  cwd = fm->f_vols[index].cwd;

  ret = _f_mutex_get (fm, vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (_f_checkstatus (vi, 0))
  {
    fm->f_vols[index].cwd[0] = 0;    /* remove cwd */
    vi->state = F_STATE_NEEDMOUNT;      /* mount is needed */
  }

  if (!maxlen)
    return F_SETLASTERROR (F_NO_ERROR);

  maxlen--;                     /* need for termination */
  if (maxlen)
  {
    *buffer++ = F_SEPARATORCHAR;
    maxlen--;
  }

  for (a = 0; a < maxlen; a++)
  {
    wchar ch = cwd[a];
    buffer[a] = ch;
    if (!ch)
      break;
  }

  buffer[a] = 0;                /* add terminator at the end */

  return F_SETLASTERROR (F_NO_ERROR);
}
#endif

/****************************************************************************
 *
 * fn_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_findfirst (F_MULTI * fm, const char *filename, FN_FIND * find)
{
  struct hfat_path_buffer *hpb;
  int rc;

  /* Check for any existing use of find2 buffer */
  if (find2_inuse == TRUE)
  {
    FS_ERR_FATAL ("Global find2 Buffer in use",0,0,0);
  }
  else
  {
    find2_inuse = TRUE;
  }

  hpb = hfat_allocate_path_buffer (NULL);
  rc = fn_wfindfirst (fm, _towchar (hpb->buf, filename), &find2);
  if (rc == F_NO_ERROR)
  {
    int i;
    (void) _fromwchar (find->filename, find2.filename);
    for (i = 0; i < F_MAXNAME; i++)
      find->name[i] = find2.name[i];
    for (i = 0; i < F_MAXEXT; i++)
      find->ext[i] = find2.ext[i];
    find->attr = find2.attr;
    find->mtime = find2.mtime;
    find->mdate = find2.mdate;
    find->cluster = find2.cluster;
    find->filesize = find2.filesize;
    find->findfsname = find2.findfsname;
    find->pos = find2.pos;
  }
  hfat_free_path_buffer (hpb);
  find2_inuse = FALSE;

  return F_SETLASTERROR (rc);
}

int
fn_wfindfirst (F_MULTI * fm, const wchar * filename, FN_WFIND * find)
{
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, &find->findfsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checkname (find->findfsname.lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name, wildcard is ok */

  ret = _f_getvolume (fm, find->findfsname.drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, &find->findfsname, &find->pos))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);   /* search for path */

#ifdef HCC_UNICODE
  ret = fn_wfindnext (fm, find);
#else
  ret = fn_findnext (fm, find);
#endif
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_findnext (F_MULTI * fm, FN_FIND * find)
{
  int rc;

  /* Check for any existing use of find2 buffer */
  if (find2_inuse == TRUE)
  {
    FS_ERR_FATAL ("Global find2 Buffer in use",0,0,0);
  }
  else
  {
    find2_inuse = TRUE;
  }

  find2.findfsname = find->findfsname;
  find2.pos = find->pos;
  rc = fn_wfindnext (fm, &find2);
  if (rc == F_NO_ERROR)
  {
    int i;
    (void) _fromwchar (find->filename, find2.filename);
    for (i = 0; i < F_MAXNAME; i++)
      find->name[i] = find2.name[i];
    for (i = 0; i < F_MAXEXT; i++)
      find->ext[i] = find2.ext[i];
    find->attr = find2.attr;
    find->mtime = find2.mtime;
    find->mdate = find2.mdate;
    find->cluster = find2.cluster;
    find->filesize = find2.filesize;
    find->findfsname = find2.findfsname;
    find->pos = find2.pos;
  }
  find2_inuse = FALSE;

  return F_SETLASTERROR (rc);
}

int
fn_wfindnext (F_MULTI * fm, FN_WFIND * find)
{
  int ret_val;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);

  ret_val = fn_wfindnext_h (fm, find, &f_lfn_hnb->f_lfnint);

  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

/* This function finds the next entry in the directory. On success,
   'find' is populated with appropriate name. Return F_ERR_EOF if
   there are no more entries */
static int
fn_wfindnext_h (F_MULTI * fm, FN_WFIND * find, F_LFNINT *lfn)
{
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int a;
  int ret;
  unsigned long ofsize;

  ret = _f_getvolume (fm, find->findfsname.drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findfile (vi, find->findfsname.lname, &find->pos, &de, lfn, 1))
    return F_SETLASTERROR (F_ERR_EOF);

  for (a = 0; a < F_MAXNAME; a++)
    find->name[a] = de->name[a];
  for (a = 0; a < F_MAXEXT; a++)
    find->ext[a] = de->ext[a];

  (void) _memcpy ((void *) (find->filename), lfn->name,
                  sizeof (find->filename));
  if (!isonlydotordoubledots (lfn->name))
    /* save the directory entry so that it can be used later. */
    _add_cached_de_entry (&vi->last_entry, find->findfsname.path, lfn->name,
                          de, &find->pos);

  find->attr = de->attr;
  find->mdate = _f_get16bitl (de->mdate);
  find->mtime = _f_get16bitl (de->mtime);
  find->filesize = _f_get32bitl (de->filesize);
  find->cluster = _f_getdecluster (vi, de);

  if (_f_findopensize (&ofsize, find->findfsname.drivenum, &find->pos))
  {
    find->filesize = ofsize;
  }

  find->pos.pos++;              /* goto next position */

  return F_SETLASTERROR (F_NO_ERROR);
}

/****************************************************************************
 *
 * fn_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int
fn_chdir (F_MULTI * fm, const char *dirname)
{
  int return_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  return_val = fn_wchdir (fm, _towchar (hpb->buf, dirname));
  hfat_free_path_buffer (hpb);
  return return_val;
}

int
fn_wchdir (F_MULTI * fm, const wchar * dirname)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;

  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wchdir_h (fm, dirname, &f_name_hnb->f_name);

  hfat_free_f_name_buffer (f_name_hnb);
  return ret_val;
}

static int
fn_wchdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname)
{
  F_POS pos;
  F_VOLUME *vi;
  int len;
  int a;
  int ret;

  ret = _f_setfsname (fm, dirname, fsname);
  if (ret == 1)
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name, wildcard is ok */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  for (len = 0; fsname->path[len];)
    len++;
  if (len)
    fsname->path[len++] = F_SEPARATORCHAR;

  for (a = 0;;)
  {
    W_CHAR ch = fsname->lname[a++];
    fsname->path[len++] = ch;
    if (!ch)
      break;
  }

  if (!(_f_findpath (vi, fsname, &pos)))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  for (a = 0; a < FN_MAXPATH; a++)
    vi->cwd[a] = fsname->path[a];

  return F_SETLASTERROR (F_NO_ERROR);
}

/* Determine if the file name in directory entry needs NTRES flag to be
 * set instead of making unnecessary long file name entries.
 * Returns 0xFF if this needs a long file name entry otherwise
 * ntres flag */
static unsigned char
get_ntres_flag (W_CHAR *lname, F_LFNINT *lfn, char* name, char* ext, int len)
{
  int alpha_count = 0, case_change_count = 0, ext_pos;
  int a;
  unsigned char ntres_flag = 0;

  /* Do the file name conversion from 8.3 to actual file name, do not
   * do any NTRES conversion since we are attempting to findout one. */
  _f_getfilename ((W_CHAR *) lfn->name, name, ext, 0);


  /* Check where the extension is starting from */
  for (ext_pos = len - 1; ext_pos > 0; --ext_pos)
  {
    if (lname[ext_pos] == '.')
      break;
  }

  /* ext_pos has to point to the location of ., if it doesn't
   * exist then we can point it to the null location */
  if (lname[ext_pos] != '.')
  {
    ext_pos = len;
  }

  /* Check if name or extension doesn't fit into the short file name entry */
  if ((ext_pos > 8) || ((len - 1 - ext_pos) > 3))
  {
    return 0xFF;
  }

  /* Decide if the name part's case changed */
  for (a = 0; a < 8; ++a)
  {
    /* Check if the character is an alphabet */
    if (IS_ALPHA(lname[a]))
      ++alpha_count;

    /* If name is finished then break the loop */
    if ((name[a] == ' ') || (lfn->name[a] == '.'))
      break;

    /* If the name is not same, then we can check if the
     * case change happened */
    if (lfn->name[a] != lname[a])
    {
      /* If case change happened then still the name might be
       * same, but if its not, then we need long file name */
      if (lfn->name[a] == _f_toupper(lname[a]))
      {
        ++case_change_count;
      }
      else
      {
        return 0xFF;
      }
    }
  }

  /* If there was a case change then we need attention */
  if (case_change_count > 0)
  {
    /* If all the chars changed their case, just set the flag
     * we don't need long file name for this */
    if (alpha_count == case_change_count)
    {
      ntres_flag |= NTRES_NAME_LOW;
    }
    else
    {
      /* All the chars didn't change their case, its mixed case
       * so need a long file name entry. */
      return 0xFF;
    }
  }

  /* Now handle the same way for extension */
  alpha_count = case_change_count = 0;

  for (a = ext_pos + 1; a < len; ++a)
  {
    /* Check if the character is an alphabet */
    if (IS_ALPHA(lname[a]))
      ++alpha_count;

    /* Still some more bytes left? */
    if ((lfn->name[a] == 0) || (lfn->name[a] == '.'))
      break;

    /* Check if the extension is same or the case changed */
    if (lfn->name[a] != lname[a])
    {
      if (lfn->name[a] == _f_toupper(lname[a]))
      {
        ++case_change_count;
      }
      else
      {
        return 0xFF;
      }
    }
  }

  /* If there was a case change then we need attention */
  if (case_change_count > 0)
  {
    /* If all the chars changed their case, just set the flag */
    if (alpha_count == case_change_count)
    {
      ntres_flag |= NTRES_EXT_LOW;
    }
    else
    {
      /* All the chars didn't change their case, its mixed case
       * so need a long file name entry. */
      return 0xFF;
    }
  }

  /* Don't need a Long file name entry, so return the flag */
  return ntres_flag;
}

/****************************************************************************
 *
 * _f_createlfn
 *
 * Create a raw lfn entry and short name,extension from long filename
 *
 * INPUTS
 *
 * lname - original long file name
 * lfn - lfn structure where to create raw lfn entry
 * name - where to create 8 bytes short name
 * ext - where to create 3 bytes short extension
 * pass - What is the pass number to generate file.
 *
 * OUTPUT
 * NTRES flag generated for the directory entry
 *
 ***************************************************************************/

static unsigned char
_f_createlfn (W_CHAR * lname, F_LFNINT * lfn, char *name, char *ext, int pass)
{
  int len;
  unsigned char ntres_flag = 0;
  struct fs_fat_shortname sfn;

  /* Initialize to default values */
  lfn->state = F_LFNSTATE_SFN;
  lfn->ord = 0;

  /* Call our unicode convert routine to generate a short file name */
  sfn.name = (unsigned char*)name;
  sfn.ext = (unsigned char*)ext;
  len = fs_convert->to_shortname (pass, lname, &sfn);
  lfn->end = len;

  /* If the file name length is more than 11 chars then it would be
   * definitely a long file name, we need not decide on flag */
  if (len > 12)
  {
    lfn->state = F_LFNSTATE_LFN;
    lfn->ord = (unsigned char) ((len + 12) / 13);
  }
  else
  {
    /* Check if we can avoid LFN by setting NTRES flag */
    if ((ntres_flag = get_ntres_flag (lname, lfn, name, ext, len)) == 0xFF)
    {
      ntres_flag = 0;
      lfn->state = F_LFNSTATE_LFN;
      lfn->ord = (unsigned char) ((len + 12) / 13);
    }
  }

  return ntres_flag;
}

/****************************************************************************
 *
 * _f_alloc_cluster_for_dir
 *
 *  Allocate a new cluster and link it to the current directory entry.
 *
 * INPUTS
 *
 * vi - Volume information
 * file - file handle
 * de - Directory entry for temporary usage.
 * fat32root - fat32root
 *
 * RETURNS
 *
 * Error code
 *
 ***************************************************************************/
int
_f_alloc_cluster_for_dir (F_VOLUME * vi, FN_FILE *file,
                          F_DIRENTRY *de, int fat32root)
{
  long a;
  int ret;
  unsigned long cluster;
  FN_FILEINT *f = _f_check_handle (file);

  /* We need correct handle first */
  if (!f)
    return _f_closedir (vi, file, F_ERR_NOMOREENTRY);

  /* If we didn't get a valid cluster, just exit */
  if ((!f->pos.cluster) || (f->pos.cluster >= F_CLUSTER_RESERVED))
  {
    return _f_closedir (vi, file, F_ERR_NOMOREENTRY);       /* EOF!! root FAT16,FAT12; */
  }

  /* Allocate a new cluster */
  ret = _f_alloccluster (vi, &cluster);
  if (ret)
    return _f_closedir (vi, file, ret);

  /* If we got a valid one */
  if (cluster < F_CLUSTER_RESERVED)
  {
    /* This allocated cluster will not have any link, since this
     * will be the last one in the chain for now. */
    if (fat32root)
      ret = _f_setclustervalue (vi, cluster, F_CLUSTER_LASTF32R);
    else
      ret = _f_setclustervalue (vi, cluster, F_CLUSTER_LAST);

    if (ret)
      return _f_closedir (vi, file, ret);

    /* Link this cluster as the next one for the current
     * cluster we were handling. */
    ret = _f_setclustervalue (vi, f->pos.cluster, cluster);
    if (ret)
      return _f_closedir (vi, file, ret);

    /* Write the FAT entries, since we updated as above just now. */
    ret = _f_writefatsector (vi);
    if (ret)
      return _f_closedir (vi, file, ret);
  }
  else
    return _f_closedir (vi, file, F_ERR_NOMOREENTRY);

  /* Zero out the temporary directory entry to write into new
   * allocated cluster */
  (void) _memset (de, 0, sizeof (F_DIRENTRY));

  /* Initialize all the entries in this new cluster */
  a = vi->bootrecord.sector_per_cluster;
  a *= F_SECTOR_SIZE / sizeof (F_DIRENTRY);
  while (a--)
  {
    ret = _f_writedir (vi, de, file);
    if (ret)
      return _f_closedir (vi, file, ret);
  }

  return ret;
}

/****************************************************************************
 *
 * fill_lfn_entry
 *
 * Fill the long file name directory entry structure to write to the media
 * using the data from file name for a given ordinal part of the directory
 * entry.
 *
 * INPUTS
 *
 * de - Destination directory entry structure to fill in the info for LF.
 * a  - The ordinal part of the long file name entries, that we are currently
 *        writing to the disk.
 * lfn - LFN information that is already prepared.
 * fsname - long file name that need to be copied from.
 *
 * RETURNS
 *
 * None
 *
 ***************************************************************************/
void
fill_lfn_entry (F_DIRENTRY* de, int a, F_LFNINT* lfn, F_NAME *fsname)
{
  unsigned long i, j;
  unsigned long npos = (a - 1) * 13;
  unsigned char* lfn_entry;
  F_LFN *plfn = (F_LFN *) (de);

  /* All the non filled entries need to be 0xFF so prefill it */
  (void) _memset (plfn, 0xFF, sizeof (F_LFN));

  /* Check if this is the last entry, then we need indicat this,
   * otherwise just the entry number is enough */
  if (a == lfn->ord)
    plfn->ord = (unsigned char) (a + 0x40);
  else
    plfn->ord = (unsigned char) a;

  /* Copy other available entries and initialize rest of them */
  plfn->chksum = lfn->chksum;
  plfn->attr = F_ATTR_LFN;
  plfn->type = 0;
  plfn->clusterlo[0] = 0;
  plfn->clusterlo[1] = 0;

  /* Rest is file name, we can fit only 13 unicode chars max
   * instead of doing a long assignments with struct members
   * consider this as char array and fill the entries appropriately
   * if FAT specification changes then this might change which
   * would not happen so easily. */
  lfn_entry = (unsigned char*) plfn;
  for (j = 1, i = npos; (i < npos + 13) && (i <= lfn->end); ++i)
  {
    /* Always long file name entry will have UTF16 chars */
    lfn_entry[j++] = (unsigned char)fsname->lname[i];
    lfn_entry[j++] = (unsigned char)(fsname->lname[i] >> 8);

    /* These are the position which we want to jump over
     * to determine these positions, refer to the structure
     * definition of the Long file name dirctory entry structre */
    if (j == 11) j = 14;
    if (j == 26) j = 28;
  }

}


/****************************************************************************
 *
 * get_sfn_checksum
 *
 * Get checksum for the small file name to write into the long file
 * name entry directory entry
 *
 * INPUTS
 *
 * name - Name component
 * ext  - Extension component
 *
 * RETURNS
 *
 * Byte CRC Checksum of the short file name
 *
 ***************************************************************************/
unsigned char
get_sfn_checksum (char* name, char* ext)
{
  unsigned char chksum = 0;
  int a;

  for (a = 0; a < 8; a++)
  {
    chksum =
      (unsigned
       char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
              name[a]);
  }

  for (a = 0; a < 3; a++)
  {
    chksum =
      (unsigned
       char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
              ext[a]);
  }
  return chksum;
}

/****************************************************************************
 *
 * _f_addentry
 *
 * Add a new directory entry into driectory list
 *
 * INPUTS
 *
 * vi - volumeinfo
 * fsname - filled F_NAME structure
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry where it was added
 *
 * RETURNS
 *
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 ***************************************************************************/

int
_f_addentry (F_VOLUME * vi, F_NAME * fsname, F_POS * pos, F_DIRENTRY ** pde)
{
  int ret_val;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);

  ret_val = _f_addentry_h (vi, fsname, pos, pde, &f_lfn_hnb->f_lfnint);

  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
_f_addentry_h (F_VOLUME * vi, F_NAME * fsname, F_POS * pos,
                  F_DIRENTRY ** pde, F_LFNINT *lfn)
{
  char name[8];
  char ext[3];
  int ret;
  int file_name_compare_failed;
  int a, b, pass_num = 1;
  FN_FILE *file;
  long current_dir_ent_position = 0;
  long free_dir_entries = 0;
  long empty_pos_to_insert = -1;
  long empty_dirs_needed = 0;
  unsigned char nt_res_flag;
  F_DIRENTRY de;
  int fat32root = 0;

  /* Some sanity check on name */
  if (!fsname->lname[0])
    return F_ERR_INVALIDNAME;
  if (isonlydotordoubledots (fsname->lname))
    return F_ERR_INVALIDNAME;
  if (isonlydotsandspaces (fsname->lname))
    return F_ERR_INVALIDNAME;

  /* Create a short file name which can represent this file name */
  nt_res_flag = _f_createlfn (fsname->lname, lfn, name, ext, pass_num);
  pass_num++;

  /* No further processing is needed if the file name is invalid */
  if (lfn->state == F_LFNSTATE_INV)
    return F_ERR_INVALIDNAME;

  /* If LFN support is disabled return invalid state for an LFN entry. */
  if ((!hfat_lfn_enabled) && (lfn->state == F_LFNSTATE_LFN))
    return F_ERR_INVALIDNAME;

  /* If the short file name that we created doesn't exactly represent
   * the actual file name, then that actual file name will need a
   * long file name directory entries. This information is also
   * returned by the createlfn function. So we need as many entries
   * to add this file to the directory entries list */
  empty_dirs_needed = lfn->ord + 1;

  /* Check if we need to add to root in FAT32 Format */
  if ((!pos->cluster) && (vi->mediatype == F_FAT32_MEDIA))
    fat32root = 1;

  /* Open the dir entry to start enumerating */
  file = _f_opendir (vi, fsname->drivenum, pos->cluster);
  if (!file)
    return F_ERR_INVALIDDIR;

  /* Instead of making 2 pass scan, just make 1 pass, simultaneously do the following
   * - Compare the short file name if present in any dir entry
   * - Remember the position of deleted or empty entries that can meet our requirement
   * - If a collision happens, then re calculate the Short File name and restart the
   *   scan from starting again.*/
  for (;;)
  {
    /* Get the next directory entry */
    ret = _f_readdir (vi, &de, file);
    if (ret == F_ERR_EOF)
    {
      break;
    }

    /* If any error occurred while reading, just return */
    if (ret)
      return _f_closedir (vi, file, ret);

    /* If the entries exhausted then our search and match is done */
    if (!de.name[0])
    {
      /* Check if we got enough free entries to insert new ones */
      if (free_dir_entries < empty_dirs_needed)
      {
        /* If we still didn't find empty entries, search until
         * we find enough entries or we run out of dir entries
         * among the allocated clusters. Currently we have one
         * empty entry */
        empty_pos_to_insert = current_dir_ent_position;
        free_dir_entries = 1;

        /* Now attempt a read just to check if we have enough
         * entries after the last one is encountered, until we find
         * enough needed entries. */
        while (free_dir_entries < empty_dirs_needed)
        {
          ret = _f_readdir (vi, &de, file);
          if (ret == F_ERR_EOF)
          {
            break;
          }
          free_dir_entries++;
        }
      }
      break;
    }

    /* Check if the current entry is deleted entry */
    if (de.name[0] == F_DELETED_CHAR)
    {
      if (free_dir_entries < empty_dirs_needed)
      {
        /* We might have more entries after this deleted entry,
         * so it a potential entry we can insert our new entry into */
        if (empty_pos_to_insert == -1)
        {
          empty_pos_to_insert = current_dir_ent_position;
          free_dir_entries = 0;
        }
        /* As long as we have contigous entries */
        free_dir_entries++;
      }

      /* Update our current position */
      current_dir_ent_position++;

      /* We don't have much to do if it is a deleted entry
       * so just continue with further search */
      continue;
    }

    /* If missed the sequential empty entries, and if we
     * still didn't find enough entries
     * we are still searching for empty entries */
    if (free_dir_entries < empty_dirs_needed)
    {
      empty_pos_to_insert = -1;
      free_dir_entries = 0;
    }

    /* Update our current position */
    current_dir_ent_position++;

    /* Now we are here means this entry is a proper valid dir entry
     * but if this entry is LFN entry then we don't have much to
     * do, we can again continue our search */
    if (de.attr == F_ATTR_LFN)
      continue;

    /* Now this is a short file name dir entry, compare our SFN with
     * this one, if a match occurres, then we have to generate the
     * name again. */

    file_name_compare_failed = 0;

    /* Match the extension first since it is small */
    for (b = 0; b < 3; b++)
    {
      if (de.ext[b] != ext[b])
      {
        file_name_compare_failed = 1;
        break;
      }
    }

    /* If the compare failed, then we can continue our search further */
    if (file_name_compare_failed == 1)
      continue;

    /* Our short file name comparison is full comparison, there are
     * no ~ chars added here, so its simple..!! */
    for (b = 0; b < 8; b++)
    {
      if (de.name[b] != name[b])
      {
        file_name_compare_failed = 1;
        break;
      }
    }

    /* Now our comparison of the short file name component is done.
     * If we actually found a match its a very interesting new file
     * and old file names, its a collision of the CRC30 code if we
     * generated this short file name here. But because we collided
     * we need to regenerate the name and start search from beginning
     */
    if (file_name_compare_failed == 0)
    {
      /* Recreate the SFN and start search from beginning */
      nt_res_flag = _f_createlfn (fsname->lname, lfn, name, ext, pass_num);
      pass_num++;

      /* Reset the our directory entries pointer back to first one and
       * start searching all over again. This second pass happens only
       * if a collision is detected. */
      ret = _f_seekdir (vi, file, 0, FN_SEEK_SET);
      if (ret)
        return _f_closedir (vi, file, ret);

      /* Since just now we reset to starting position */
      current_dir_ent_position = 0;
    }

  } /* loop to loop through all dir entries */

  /* We got here means our SFN is unique among the already existing
   * entries. If we didn't get enough free entries we need to allocate
   * new cluster or extend if there are some entries remaining at the end */
  if (free_dir_entries < empty_dirs_needed)
  {
    /* Make sure we make enough space, append a cluster to this chain */
    ret = _f_alloc_cluster_for_dir (vi, file, &de, fat32root);

    /* If we didn't allocate then return */
    if(ret)
      return ret;

    /* Reset the pointer to whereever we were in before. Actually this
     * operation and the read operation may not be needed. */
    ret = _f_seekdir (vi, file, current_dir_ent_position, FN_SEEK_SET);
    if (ret)
      return _f_closedir (vi, file, ret);

    ret = _f_readdir (vi, &de, file); /* read new entry */

    /* If we didn't assign already, then now we can */
    if (empty_pos_to_insert < 0)
      empty_pos_to_insert = current_dir_ent_position;
  }

  /* Now we have enough free entries found, seek to the position where
   * we can insert our now entry */
  ret = _f_seekdir (vi, file, empty_pos_to_insert, FN_SEEK_SET);
  if (ret)
    return _f_closedir (vi, file, ret);

  /* If we have Long file name entries to insert, then do that first. */
  if (lfn->state == F_LFNSTATE_LFN)
  {
      lfn->chksum = get_sfn_checksum (name, ext);

      for (b = 1, a = lfn->ord; a > 0; a--)
      {
        fill_lfn_entry (&de, a, lfn, fsname);

        ret = _f_writedir (vi, &de, file);
        if (ret)
          return _f_closedir (vi, file, ret);
      }
  }

#ifdef FS_UNIT_TEST
  for (a = 0; a < 8; ++a) ut_short_file_name[a] = name[a];
  for (a = 0; a < 3; ++a) ut_short_file_ext[a] = ext[a];
  if (lfn->state == F_LFNSTATE_SFN) ut_lfn_option = 1;
  if (lfn->state == F_LFNSTATE_LFN) ut_lfn_option = 2;
#endif

  /* Prepare our short file name directory entry */
  _f_initentry (&de, name, ext);

  de.ntres = nt_res_flag;

  if ((vi->mediatype == F_FAT32_MEDIA) && (lfn->state == F_LFNSTATE_SFN))
  {
    uint32 curr_time;
    /* if there is realtime clock then create date/time and access date could
     * be set from it. */
    curr_time = fs_time ();
    _f_set16bitl (de.crtdate, fs_posix_to_fat_date (curr_time));
    _f_set16bitl (de.crttime, fs_posix_to_fat_time (curr_time));
    _f_set16bitl (de.lastaccessdate, fs_posix_to_fat_date (curr_time));
  }

  /* Write short file name entry */
  ret = _f_writedir (vi, &de, file);
  if (ret)
    return _f_closedir (vi, file, ret);

  if (pde)
  {
    FN_FILEINT *f = _f_check_handle (file);

    if (!f)
      return _f_closedir (vi, file, F_ERR_NOTOPEN);

    if (f->relpos >= sizeof (F_DIRENTRY))
    {
      vi->direntrysector = f->pos.sector;
#ifdef DIRCACHE_ENABLE
      (void) _memcpy (vi->direntry,
                      vi->dircache +
                      (((f->pos.sector) - (vi->dircache_start)) << 9),
                      F_SECTOR_SIZE);
#else
      (void) _memcpy (vi->direntry, f->data, F_SECTOR_SIZE);
#endif
      *pde =
        (F_DIRENTRY *) (vi->direntry + f->relpos - sizeof (F_DIRENTRY));

      pos->cluster = f->pos.cluster;
      pos->pos = f->relpos / sizeof (F_DIRENTRY) - 1;
      pos->sector = f->pos.sector;
      pos->sectorend = f->pos.sectorend;

    }
    else
      return _f_closedir (vi, file, F_ERR_NOTOPEN);

  }

  return _f_closedir (vi, file, F_NO_ERROR);      /* if ok */

}  /* _f_addentry */

/****************************************************************************
 *
 * fn_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_rename (F_MULTI * fm, const char *filename, const char *newname)
{
  int ret_val;
  struct hfat_path_buffer *hpb1, *hpb2;

  hpb1 = hfat_allocate_path_buffer (NULL);
  hpb2 = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wrename (fm, _towchar (hpb1->buf, filename), _towchar (hpb2->buf, newname));
  hfat_free_path_buffer (hpb1);
  hfat_free_path_buffer (hpb2);
  return ret_val;
}

int
fn_wrename (F_MULTI * fm, const wchar * filename, const wchar * newname)
{
  int ret_val;
  struct hfat_f_name_buffer *fsname_hnb, *fsnewname_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  fsname_hnb = hfat_allocate_f_name_buffer (NULL);
  fsnewname_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wrename_h (fm, filename, newname, &fsname_hnb->f_name,
                             &fsnewname_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (fsname_hnb);
  hfat_free_f_name_buffer (fsnewname_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wrename_h (F_MULTI * fm, const wchar * filename, const wchar * newname,
             F_NAME *fsname, F_NAME *fsnewname, F_LFNINT *lfn)
{
  F_POS posdir;
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  F_DIRENTRY deold;

  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!(_f_findpath (vi, fsname, &posdir)))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  if (isonlydotordoubledots (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  if (de->attr & F_ATTR_READONLY)
    return F_SETLASTERROR (F_ERR_ACCESSDENIED); /* readonly */

  if (_f_checklocked (fsname->drivenum, &pos))
    return F_SETLASTERROR (F_ERR_LOCKED);

  (void) _memcpy ((void *) (&deold), de, sizeof (F_DIRENTRY));  /* store all properties */

  if (_f_setfsname (fm, newname, fsnewname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsnewname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  if (isonlydotordoubledots (fsnewname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsnewname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  (void) _memcpy ((void *) (fsnewname->path), fsname->path,
                  sizeof (fsname->path));
  fsnewname->drivenum = fsname->drivenum;

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  /* Check these are the same file */
  {
    int i;
    for (i=0; ; i++)
    {
      /* Compare file names */
      if (_f_toupper (fsname->lname[i]) !=
          _f_toupper (fsnewname->lname[i]))
      {
        F_DIRENTRY *temp_de;
        /* If different check if newname exists */
        if (_f_findfile (vi, fsnewname->lname, &pos, &temp_de, lfn, 0))
        {
          if (temp_de->attr == F_ATTR_DIR)
            return F_SETLASTERROR (F_ERR_ISDIR);
          else
            return F_SETLASTERROR (F_ERR_DUPLICATED);
        }
        else
          break;
      }
      if (fsname->lname[i] == 0 ||
          fsnewname->lname[i] == 0)
        break;
    }
  }

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  ret = _f_addentry (vi, fsnewname, &pos, &de);
  if (ret)
    return F_SETLASTERROR (ret);

  /* delete the saved directory entry before it is updated */
  _delete_cached_de_entry (&vi->last_entry);

  (void) _memcpy (deold.name, de->name, 8);     /* update name and ext */
  (void) _memcpy (deold.ext, de->ext, 3);
  deold.ntres = de->ntres;

  (void) _memcpy ((void *) de, &deold, sizeof (F_DIRENTRY));    /* restore all properties */

  ret = _f_writedirsector (vi);
  if (ret)
    return F_SETLASTERROR (ret);

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);     /* try to find again to remove */

  ret = _f_removelfn (vi, fsname->drivenum, posdir.cluster, lfn);
  return F_SETLASTERROR (ret);
}


/****************************************************************************
 *
 * fn_move
 *
 * move file or directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_move (F_MULTI * fm, const char *filename, const char *newname)
{
  int ret_val;
  struct hfat_path_buffer *hpb1, *hpb2;

  hpb1 = hfat_allocate_path_buffer (NULL);
  hpb2 = hfat_allocate_path_buffer (NULL);

  ret_val = fn_wmove (fm, _towchar (hpb1->buf, filename),
                      _towchar (hpb2->buf, newname));

  hfat_free_path_buffer (hpb1);
  hfat_free_path_buffer (hpb2);

  return ret_val;
}

int
fn_wmove (F_MULTI * fm, const wchar * filename, const wchar * newname)
{
  int ret_val;
  struct hfat_f_name_buffer *fsname_hnb, *fsnamenew_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb, *f_lfn_hnb2;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_lfn_hnb2 = hfat_allocate_f_lfnint_buffer (NULL);
  fsname_hnb = hfat_allocate_f_name_buffer (NULL);
  fsnamenew_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wmove_h (fm, filename, newname,
                          &fsname_hnb->f_name, &fsnamenew_hnb->f_name,
                          &f_lfn_hnb->f_lfnint, &f_lfn_hnb2->f_lfnint);


  hfat_free_f_name_buffer (fsname_hnb);
  hfat_free_f_name_buffer (fsnamenew_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb2);
  return ret_val;
}

static int
fn_wmove_h (F_MULTI * fm, const wchar * filename, const wchar * newname,
               F_NAME *fsname, F_NAME *fsnamenew, F_LFNINT *lfn, F_LFNINT *lfn2)
{
  F_POS posdir;
  F_POS posdir2;
  F_POS pos;
  F_POS pos2;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  F_DIRENTRY *de2;
  int ret;
  F_DIRENTRY oldde;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!(_f_findpath (vi, fsname, &posdir)))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  if (isonlydotordoubledots (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  if (de->attr & F_ATTR_READONLY)
    return F_SETLASTERROR (F_ERR_ACCESSDENIED); /* readonly */

  (void) _memcpy (&oldde, de, sizeof (F_DIRENTRY));

  if (de->attr & F_ATTR_DIR)
  {
    unsigned long cluster = _f_getdecluster (vi, &oldde);

    if (_f_setfsname (fm, newname, fsnamenew))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);        /* invalid name */
    if (_f_checknamewc (fsnamenew->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);        /* invalid name */

    if (fsnamenew->drivenum != fsname->drivenum)
      return F_SETLASTERROR (F_ERR_INVALIDDRIVE);       /* can't move on different drive */

    if (!(_f_findpath (vi, fsnamenew, &posdir2)))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);

    pos2.cluster = posdir2.cluster;
    pos2.sector = posdir2.sector;
    pos2.sectorend = posdir2.sectorend;
    pos2.pos = posdir2.pos;

    if (isonlydotordoubledots (fsnamenew->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);
    if (isonlydotsandspaces (fsnamenew->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);

    if (_f_findfile (vi, fsnamenew->lname, &pos2, &de2, lfn2, 0))
    {
      if (de2->attr != F_ATTR_DIR) /* The new path is not a directory */
        return F_SETLASTERROR (F_ERR_INVALIDDIR);
      else /* The new path is a directory */
        return F_SETLASTERROR (F_ERR_NOTEMPTY);
    }

    {                           /* check if target directory is not in original's subdirectory */
      unsigned long cl = pos2.cluster;
      F_POS p;
      F_DIRENTRY *de3 =
        (F_DIRENTRY *) (vi->direntry + sizeof (F_DIRENTRY) * 1);
      for (;;)
      {
        if (cl == cluster)
          return F_SETLASTERROR (F_ERR_INVALIDNAME);

        _f_clustertopos (vi, cl, &p);

        ret = _f_getdirsector (vi, p.sector);
        if (ret)
          return F_SETLASTERROR (ret);

        if (de3->name[0] != '.' || de3->name[1] != '.' || de3->name[2] != 32)
          break;

        cl = _f_getdecluster (vi, de3);
      }
    }


    ret = _f_addentry (vi, fsnamenew, &posdir2, &de2);
    if (ret)
      return F_SETLASTERROR (ret);      /* couldnt be added */

    /* delete the saved directory entry before it is updated. */
    _delete_cached_de_entry (&vi->last_entry);

    (void) _memcpy (oldde.name, de2->name, 8);  /* update name and ext */
    (void) _memcpy (oldde.ext, de2->ext, 3);
    oldde.ntres = de2->ntres;

    (void) _memcpy ((void *) de2, &oldde, sizeof (F_DIRENTRY)); /* restore all properties */

    ret = _f_writedirsector (vi);
    if (ret)
      return F_SETLASTERROR (ret);      /* couldnt be written */


    pos.cluster = posdir.cluster;
    pos.sector = posdir.sector;
    pos.sectorend = posdir.sectorend;
    pos.pos = posdir.pos;

    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);   /* try to find again to remove */

    ret = _f_removelfn (vi, fsname->drivenum, posdir.cluster, lfn);
    if (ret)
      return F_SETLASTERROR (ret);

    {
      FN_FILE *file;
      unsigned long cluster5 = _f_getdecluster (vi, &oldde);    /* set .. to current */
      file = _f_opendir (vi, fsname->drivenum, cluster5);
      if (!file)
        return F_SETLASTERROR (F_ERR_INVALIDDIR);

      ret = _f_seekdir (vi, file, 1, FN_SEEK_SET);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      ret = _f_readdir (vi, &oldde, file);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      ret = _f_seekdir (vi, file, -1, FN_SEEK_CUR);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      if (oldde.name[0] != '.' || oldde.name[1] != '.')
        return F_SETLASTERROR (_f_closedir (vi, file, F_ERR_INVALIDDIR));

      _f_setdecluster (vi, &oldde, posdir2.cluster);

      ret = _f_writedir (vi, &oldde, file);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      return F_SETLASTERROR (_f_closedir (vi, file, F_NO_ERROR));
    }
  }
  else
  {
    if (_f_checklocked (fsname->drivenum, &pos))
      return F_SETLASTERROR (F_ERR_LOCKED);

    if (_f_setfsname (fm, newname, fsnamenew))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);        /* invalid name */
    if (_f_checknamewc (fsnamenew->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);        /* invalid name */

    if (fsnamenew->drivenum != fsname->drivenum)
      return F_SETLASTERROR (F_ERR_INVALIDDRIVE);       /* can't move on different drive */

    if (!(_f_findpath (vi, fsnamenew, &posdir2)))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);

    pos2.cluster = posdir2.cluster;
    pos2.sector = posdir2.sector;
    pos2.sectorend = posdir2.sectorend;
    pos2.pos = posdir2.pos;

    if (isonlydotordoubledots (fsname->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);
    if (isonlydotsandspaces (fsname->lname))
      return F_SETLASTERROR (F_ERR_INVALIDNAME);

    if (_f_findfile (vi, fsnamenew->lname, &pos2, &de2, lfn2, 0))
    {
      /* If the destination is an existing directory */
      if (de2->attr == F_ATTR_DIR)
        return F_SETLASTERROR (F_ERR_ISDIR);
      else
        return F_SETLASTERROR (F_ERR_DUPLICATED);
    }

    ret = _f_addentry (vi, fsnamenew, &posdir2, &de2);
    if (ret)
      return F_SETLASTERROR (ret);      /* couldnt be added */

    (void) _memcpy (oldde.name, de2->name, 8);  /* update name and ext */
    (void) _memcpy (oldde.ext, de2->ext, 3);
    oldde.ntres = de2->ntres;

    (void) _memcpy ((void *) de2, &oldde, sizeof (F_DIRENTRY)); /* restore all properties */

    ret = _f_writedirsector (vi);
    if (ret)
      return F_SETLASTERROR (ret);      /* couldnt be written */

    pos.cluster = posdir.cluster;
    pos.sector = posdir.sector;
    pos.sectorend = posdir.sectorend;
    pos.pos = posdir.pos;

    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);   /* try to find again to remove */

    ret = _f_removelfn (vi, fsname->drivenum, posdir.cluster, lfn);
    return F_SETLASTERROR (ret);
  }


}

/****************************************************************************
 *
 * fn_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_mkdir (F_MULTI * fm, const char *dirname)
{
  int ret_val;
  struct hfat_path_buffer *hpb;
  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wmkdir (fm, _towchar (hpb->buf, dirname));
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wmkdir (F_MULTI * fm, const wchar * dirname)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wmkdir_h (fm, dirname, &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wmkdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname,
                F_LFNINT *lfn)
{
  FN_FILEINT *f;
  F_POS posdir;
  F_POS pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  unsigned long cluster;
  unsigned int a;
  int ret;

  if (_f_setfsname (fm, dirname, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, fsname, &posdir))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  if (isonlydotordoubledots (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  if (_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_DUPLICATED);

  pos.cluster = posdir.cluster;
  pos.sector = posdir.sector;
  pos.sectorend = posdir.sectorend;
  pos.pos = posdir.pos;

  ret = _f_alloccluster (vi, &cluster);
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_addentry (vi, fsname, &pos, &de);
  if (ret)
    return F_SETLASTERROR (ret);        /* couldnt be added */

  de->attr |= F_ATTR_DIR;       /* set as directory */


/*  */

  f = _f_getintfile ();

  if (!f)
    return F_SETLASTERROR (F_ERR_NOMOREENTRY);

  _f_clustertopos (vi, cluster, &f->pos);

  _f_setdecluster (vi, de, cluster);    /* new dir */

/*  */

  de = (F_DIRENTRY *) (f->data);

  _f_initentry (de, ".       ", "   ");
  de->attr = F_ATTR_DIR;        /* set as directory */
  _f_setdecluster (vi, de, cluster);    /* current */
  de++;

  _f_initentry (de, "..      ", "   ");
  de->attr = F_ATTR_DIR;        /* set as directory */
  _f_setdecluster (vi, de, posdir.cluster);     /* parent */
  de++;

  for (a = 2; a < F_SECTOR_SIZE / sizeof (F_DIRENTRY); a++, de++)
  {
    (void) _memset ((void *) de, 0, sizeof (F_DIRENTRY));
  }

  ret = _f_writesector (vi, f->data, f->pos.sector, 1);
  if (ret)
    return F_SETLASTERROR (ret);

  f->pos.sector++;

/*  */
  de = (F_DIRENTRY *) (f->data);

  for (a = 0; a < 2; a++, de++)
  {
    (void) _memset (de, 0, sizeof (F_DIRENTRY));
  }

  while (f->pos.sector < f->pos.sectorend)
  {
    de = (F_DIRENTRY *) (f->data);

    while ((ret =
            _f_writesector (vi, f->data, f->pos.sector, 1)) != F_NO_ERROR)
    {
      if (vi->state != F_STATE_WORKING)
        return F_SETLASTERROR (ret);    /* check if still working volume */

      ret = _f_dobadblock (vi, f);
      if (ret)
        return F_SETLASTERROR (ret);
    }

    f->pos.sector++;
  }

  ret = _f_setclustervalue (vi, f->pos.cluster, F_CLUSTER_LAST);
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_writefatsector (vi);
  if (ret)
    return F_SETLASTERROR (ret);
#ifdef FATCACHE_ENABLE
  ret = _f_fatcache_flush (vi, 0);
  if (ret)
    return F_SETLASTERROR (ret);
#endif

  return F_SETLASTERROR (_f_writedirsector (vi));       /* writing  direntry */
}


/****************************************************************************
 *
 * fn_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file or -1 if any error
 *
 ***************************************************************************/

long
fn_filelength (F_MULTI * fm, const char *filename)
{
  long ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wfilelength (fm, _towchar (hpb->buf, filename));
  hfat_free_path_buffer (hpb);
  return ret_val;
}

long
fn_wfilelength (F_MULTI * fm, const wchar * filename)
{
  long ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wfilelength_h (fm, filename, &f_name_hnb->f_name,
                                 &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static long
fn_wfilelength_h (F_MULTI * fm, const wchar * filename, F_NAME *fsname,
                     F_LFNINT *lfn)
{
  F_VOLUME *vi;
  F_POS pos;
  F_DIRENTRY *de;
  unsigned long ofsize;

  if (_f_setfsname (fm, filename, fsname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return -1;                  /* invalid name */
  }
  if (_f_checknamewc (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return -1;                  /* invalid name */
  }

  if (_f_getvolume (fm, fsname->drivenum, &vi))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDDRIVE);
    return -1;                  /* cannot get it */
  }

  if (!_f_findpath (vi, fsname, &pos))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
    return -1;
  }
  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
  {
    F_SETLASTERROR_NORET (F_ERR_NOTFOUND);
    return -1;
  }

  if (de->attr & F_ATTR_DIR)
  {
    F_SETLASTERROR_NORET (F_ERR_ACCESSDENIED);
    return -1;                  /* directory */
  }

  if (_f_findopensize (&ofsize, fsname->drivenum, &pos))
  {
    F_SETLASTERROR_NORET (F_NO_ERROR);
    return ofsize;
  }

  F_SETLASTERROR_NORET (F_NO_ERROR);
  return _f_get32bitl (de->filesize);
}

/****************************************************************************
 *
 * fn_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_open (F_MULTI * fm, const char *filename, const char *mode)
{
  FN_FILE * ret_val;
  struct hfat_path_buffer *hpb1, *hpb2;

  hpb1 = hfat_allocate_path_buffer (NULL);
  hpb2 = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wopen (fm, _towchar (hpb1->buf, filename),
                      _towchar (hpb2->buf, mode));
  hfat_free_path_buffer (hpb1);
  hfat_free_path_buffer (hpb2);
  return ret_val;
}

FN_FILE *
fn_wopen (F_MULTI * fm, const wchar * filename, const wchar * mode)
{
  FN_FILE *ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wopen_h (fm, filename, mode, &f_name_hnb->f_name,
                           &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static FN_FILE *
fn_wopen_h (F_MULTI * fm, const wchar * filename, const wchar * mode,
           F_NAME *fsname, F_LFNINT *lfn)
{
  FN_FILEINT *fapp = 0;
  FN_FILEINT *f = 0;
  char m_mode = FN_FILE_CLOSE;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;

  if (mode[0] == 'r')
  {
    if (mode[1] == 0)
      m_mode = FN_FILE_RD;
    else if (mode[1] == '+')
    {
      if (mode[2] == 0)
        m_mode = FN_FILE_RDP;
    }
  }
  else if (mode[0] == 'w')
  {
    if (mode[1] == 0)
      m_mode = FN_FILE_WR;
    else if (mode[1] == '+')
    {
      if (mode[2] == 0)
        m_mode = FN_FILE_WRP;
    }
  }
  else if (mode[0] == 'a')
  {
    if (mode[1] == 0)
      m_mode = FN_FILE_A;
    else if (mode[1] == '+')
    {
      if (mode[2] == 0)
        m_mode = FN_FILE_AP;
    }
  }

  if (_f_setfsname (fm, filename, fsname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;                   /* invalid name */
  }
  if (_f_checknamewc (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;                   /* invalid name */
  }

  if (_f_getvolume (fm, fsname->drivenum, &vi))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDDRIVE);
    return 0;                   /* cannot access */
  }

  f = _f_getintfile ();
  if (!f)
  {
    F_SETLASTERROR_NORET (F_ERR_NOMOREENTRY);
    return 0;                   /* no more file could be opened  */
  }

  f->drivenum = fsname->drivenum;
  f->state = 0;
  f->syncfile = 0;

  if (isonlydotordoubledots (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;
  }
  if (isonlydotsandspaces (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;
  }

  switch (m_mode)
  {
    case FN_FILE_RDP:          /* r+ */
    case FN_FILE_RD:           /* r */
      /* The saved directory entry is searched first. If a match
       * is not found the directory entries in the device is searched.
       */
      de = _find_cached_de_entry (&vi->last_entry, fsname->path,
                                  fsname->lname, &f->dirpos);
      if (!de)
      {
        if (!_f_findpath (vi, fsname, &f->dirpos))
        {
          F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
          return 0;
        }
        if (!_f_findfile (vi, fsname->lname, &f->dirpos, &de, lfn, 0))
        {
          F_SETLASTERROR_NORET (F_ERR_NOTFOUND);
          return 0;
        }
      }
      /* Lets invalidate the directory entry because the file will be
       * modified.
       */
      _delete_cached_de_entry (&vi->last_entry);
      if (de->attr & F_ATTR_DIR)
      {
        F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
        return 0;               /* directory */
      }
      if (m_mode == FN_FILE_RDP)
      {
        if (de->attr & F_ATTR_READONLY)
        {
          F_SETLASTERROR_NORET (F_ERR_ACCESSDENIED);
          return 0;
        }
        if (_f_checklocked (fsname->drivenum, &f->dirpos))
        {
          F_SETLASTERROR_NORET (F_ERR_LOCKED);
          return 0;             /* locked */
        }
      }
      else
      {
        if (_f_checkreadlocked (vi, fsname->drivenum, &f->dirpos, &fapp))
        {
          F_SETLASTERROR_NORET (F_ERR_LOCKED);
          return 0;             /* locked */
        }
      }

      if (!fapp)
      {
        f->startcluster = _f_getdecluster (vi, de);

        if (f->startcluster)
        {
          _f_clustertopos (vi, f->startcluster, &f->pos);

          f->filesize = _f_get32bitl (de->filesize);
#if F_MAXSEEKPOS
          _fn_initseekdivisor (f, vi);
#endif
          f->abspos = (unsigned long) -F_SECTOR_SIZE;   /* forcing seek to read 1st sector! abspos=0; */
          ret = _f_fseek (vi, fm, f, 0);
          if (ret)
          {
            F_SETLASTERROR_NORET (ret);
            return 0;
          }
        }
        else
        {
          f->pos.sector = 0;
          f->pos.sectorend = 0;
#if F_MAXSEEKPOS
          _fn_initseekdivisor (f, vi);
#endif
        }
      }
      else
      {
        f->pos.sector = 0;
        f->pos.sectorend = 0;

        f->filesize = fapp->filesize;   /* set all size */
        f->startcluster = 0;
        f->syncfile = fapp;

#if F_MAXSEEKPOS
        _fn_initseekdivisor (f, vi);
#endif
        if (fapp->startcluster)
        {
          f->startcluster = fapp->startcluster;
          _f_clustertopos (vi, f->startcluster, &f->pos);
          f->abspos = (unsigned long) -F_SECTOR_SIZE;   /* forcing seek to read 1st sector! abspos=0; */
          ret = _f_fseek (vi, fm, f, 0);
          if (ret)
          {
            F_SETLASTERROR_NORET (ret);
            return 0;
          }
        }
      }

#if F_UPDATELASTACCESSDATE
      if (m_mode == FN_FILE_RD) /* read mode access time updating, r+ is updated from close */
      {
        uint16 curr_date = fs_posix_to_fat_date (fs_time ());
        if (_f_get16bitl (de->lastaccessdate) != curr_date)
        {
          _f_set16bitl (de->lastaccessdate, curr_date);      /* if there is realtime clock then lastaccess date could be set from */
          ret = _f_writedirsector (vi);
          if (ret)
          {
            F_SETLASTERROR_NORET (ret);
            return 0;           /* Can't be updated */
          }
        }
      }
#endif

      break;


    case FN_FILE_AP:
    case FN_FILE_A:            /* a */
       /* The saved directory entry is searched first. If a match
       * is not found the directory entries in the device is searched.
       */
      de = _find_cached_de_entry (&vi->last_entry, fsname->path,
                                  fsname->lname, &f->pos);
      if (!de)
      {
        if (!_f_findpath (vi, fsname, &f->dirpos))
        {
          F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
          return 0;
        }
        f->pos.cluster = f->dirpos.cluster;
        f->pos.pos = f->dirpos.pos;
        f->pos.sector = f->dirpos.sector;
        f->pos.sectorend = f->dirpos.sectorend;
        if (!_f_findfile (vi, fsname->lname, &f->pos, &de, lfn, 0))
          de = NULL;
      }

      /* Lets invalidate the directory entry because the file will be
       * modified.
       */
      _delete_cached_de_entry (&vi->last_entry);

      if (de)
      {
        if (de->attr & F_ATTR_DIR)
        {
          F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
          return 0;             /* directory */
        }
        if (de->attr & F_ATTR_READONLY)
        {
          F_SETLASTERROR_NORET (F_ERR_ACCESSDENIED);
          return 0;
        }

        if (_f_checkappendlocked (fsname->drivenum, &f->dirpos, f))
        {
          F_SETLASTERROR_NORET (F_ERR_LOCKED);
          return 0;             /* locked */
        }

        f->startcluster = _f_getdecluster (vi, de);

        f->filesize = _f_get32bitl (de->filesize);
#if F_MAXSEEKPOS
        _fn_initseekdivisor (f, vi);
#endif

        if (f->startcluster)
        {
          _f_clustertopos (vi, f->startcluster, &f->pos);
          f->abspos = (unsigned long) -F_SECTOR_SIZE;   /* forcing seek to read 1st sector! abspos=0; */
          ret = _f_fseek (vi, fm, f, f->filesize);
          if (ret)
          {
            f->mode = FN_FILE_CLOSE;
            F_SETLASTERROR_NORET (ret);
            return 0;
          }
        }
        else
        {
          f->pos.sector = 0;
          f->pos.sectorend = 0;

          f->relpos = 0;
          f->abspos = 0;
          f->filesize = 0;
          f->pos.prevcluster = 0;
          f->modified = 0;
        }
      }
      else
      {
        f->dirpos.cluster = f->pos.cluster;
        f->dirpos.pos = f->pos.pos;
        f->dirpos.sector = f->pos.sector;
        f->dirpos.sectorend = f->pos.sectorend;

        _f_clustertopos (vi, f->dirpos.cluster, &f->pos);

        ret = _f_addentry (vi, fsname, &f->dirpos, &de);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;             /* couldnt be added */
        }

        f->relpos = 0;
        f->abspos = 0;
        f->filesize = 0;
        f->startcluster = 0;
        f->pos.prevcluster = 0;
        f->modified = 0;

        de->attr |= F_ATTR_ARC; /* set as archiv */
        ret = _f_writedirsector (vi);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;
        }

        f->startcluster = 0;
        f->pos.sector = 0;
        f->pos.sectorend = 0;
#if F_MAXSEEKPOS
        _fn_initseekdivisor (f, vi);
#endif
      }


      break;

    case FN_FILE_WR:           /* w */
    case FN_FILE_WRP:          /* w+ */
      /* The saved directory entry is searched first. If a match
       * is not found the directory entries in the device is searched.
       */
      de = _find_cached_de_entry (&vi->last_entry, fsname->path,
                                  fsname->lname, &f->pos);
      if (!de)
      {
        if (!_f_findpath (vi, fsname, &f->dirpos))
        {
          F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
          return 0;
        }
        _f_clustertopos (vi, f->dirpos.cluster, &f->pos);
        if (!_f_findfile (vi, fsname->lname, &f->pos, &de, lfn, 0))
          de = NULL;
      }
      /* Lets invalidate the directory entry because the file will be
       * modified.
       */
      _delete_cached_de_entry (&vi->last_entry);

      if (de)
      {
        uint32 curr_time = fs_time ();
        unsigned long cluster = _f_getdecluster (vi, de);       /* exist */

        if (de->attr & F_ATTR_DIR)
        {
          F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
          return 0;             /* directory */
        }
        if (de->attr & F_ATTR_READONLY)
        {
          F_SETLASTERROR_NORET (F_ERR_ACCESSDENIED);
          return 0;
        }

        f->dirpos.cluster = f->pos.cluster;
        f->dirpos.pos = f->pos.pos;
        f->dirpos.sector = f->pos.sector;
        f->dirpos.sectorend = f->pos.sectorend;

        if (_f_checklocked (fsname->drivenum, &f->dirpos))
        {
          F_SETLASTERROR_NORET (F_ERR_LOCKED);
          return 0;             /* locked */
        }

        de->filesize[0] = 0;    /* reset size; */
        de->filesize[1] = 0;    /* reset size; */
        de->filesize[2] = 0;    /* reset size; */
        de->filesize[3] = 0;    /* reset size; */
        de->attr |= F_ATTR_ARC; /* set as archiv */
        de->clusterlo[0] = 0;   /* no points anywhere */
        de->clusterlo[1] = 0;   /* no points anywhere */
        de->clusterhi[0] = 0;   /* no points anywhere */
        de->clusterhi[1] = 0;   /* no points anywhere */

        /* if there is realtime clock then create date/time and access date
         * could be set from it. */
        _f_set16bitl (de->crtdate, fs_posix_to_fat_date (curr_time));
        _f_set16bitl (de->crttime, fs_posix_to_fat_time (curr_time));
        _f_set16bitl (de->lastaccessdate, fs_posix_to_fat_date (curr_time));

        ret = _f_writedirsector (vi);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;
        }

        ret = _f_removechain (vi, cluster);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;             /* remove  */
        }
      }
      else
      {
        ret = _f_addentry (vi, fsname, &f->dirpos, &de);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;             /* couldnt be added */
        }

        f->relpos = 0;
        f->abspos = 0;
        f->filesize = 0;
        f->startcluster = 0;
        f->pos.prevcluster = 0;
        f->modified = 0;

        de->attr |= F_ATTR_ARC; /* set as archiv */
        ret = _f_writedirsector (vi);
        if (ret)
        {
          F_SETLASTERROR_NORET (ret);
          return 0;
        }
      }

      f->startcluster = 0;
      f->pos.sector = 0;
      f->pos.sectorend = 0;

#if F_MAXSEEKPOS
      _fn_initseekdivisor (f, vi);
#endif
      break;

    default:
      F_SETLASTERROR_NORET (F_ERR_NOTUSEABLE);
      return 0;                 /* invalid mode */
  }

  f->WrDataCache.pos = f->WrDataCache.posbuf;
  f->mode = m_mode;             /*  lock it  */

  F_SETLASTERROR_NORET (F_NO_ERROR);
  return (FN_FILE *) (f->file.reference);
}

/****************************************************************************
 *
 * fn_stat
 *
 * get status information on a file
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - which file time and date wanted to be retrive
 * stat - pointer where to store information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_stat (F_MULTI * fm, const char *filename, F_STAT * stat)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wstat (fm, _towchar (hpb->buf, filename), stat);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wstat (F_MULTI * fm, const wchar * filename, F_STAT * stat)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wstat_h (fm, filename, stat, &f_name_hnb->f_name,
                           &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wstat_h (F_MULTI * fm, const wchar * filename, F_STAT * stat,
               F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;
  unsigned long ofsize;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  de = _find_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname,
                              &pos);
  if (!de)
  {
    if (!_f_findpath (vi, fsname, &pos))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);
    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);
    _add_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname, de,
                          &pos);
  }
  stat->filesize = _f_get32bitl (de->filesize);
  stat->createdate = _f_get16bitl (de->crtdate);
  stat->createtime = _f_get16bitl (de->crttime);
  stat->modifieddate = _f_get16bitl (de->mdate);
  stat->modifiedtime = _f_get16bitl (de->mtime);
  stat->lastaccessdate = _f_get16bitl (de->lastaccessdate);
  stat->attr = de->attr;
  stat->drivenum = fsname->drivenum;

  if (_f_findopensize (&ofsize, fsname->drivenum, &pos))
  {
    stat->filesize = ofsize;
  }

  return F_SETLASTERROR (F_NO_ERROR);
}

/****************************************************************************
 *
 * fn_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_settimedate (F_MULTI * fm, const char *filename, unsigned short mtime,
                unsigned short mdate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wsettimedate (fm, _towchar (hpb->buf, filename), mtime, mdate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wsettimedate (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                 unsigned short mdate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wsettimedate_h (fm, filename, mtime, mdate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wsettimedate_h (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                 unsigned short mdate, F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, fsname, &pos))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);
  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  _f_set16bitl (de->mtime, mtime);
  _f_set16bitl (de->mdate, mdate);

  ret = _f_writedirsector (vi);
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_gettimedate (F_MULTI * fm, const char *filename, unsigned short *pmtime,
                unsigned short *pmdate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wgettimedate (fm, _towchar (hpb->buf, filename), pmtime, pmdate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wgettimedate (F_MULTI * fm, const wchar * filename, unsigned short *pmtime,
                 unsigned short *pmdate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wgettimedate_h (fm, filename, pmtime, pmdate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wgettimedate_h (F_MULTI * fm, const wchar * filename, unsigned short *pmtime,
                 unsigned short *pmdate, F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  de = _find_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname,
                              &pos);
  if (!de)
  {
    if (!_f_findpath (vi, fsname, &pos))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);
    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);
    _add_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname, de,
                          &pos);
  }

  if (pmtime)
    *pmtime = _f_get16bitl (de->mtime);
  if (pmdate)
    *pmdate = _f_get16bitl (de->mdate);

  return F_SETLASTERROR (F_NO_ERROR);
}

/****************************************************************************
 *
 * fm_setaccessdate
 *
 * Set a file access date.
 *
 * INPUTS
 *
 * filename - file to change date/time attributes.
 * adate - new access date of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
int
fn_setaccessdate (F_MULTI * fm, const char *filename, unsigned short adate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wsetaccessdate (fm, _towchar (hpb->buf, filename), adate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wsetaccessdate (F_MULTI * fm, const wchar *filename, unsigned short adate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wsetaccessdate_h (fm, filename, adate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wsetaccessdate_h (F_MULTI * fm, const wchar * filename,
                     unsigned short adate, F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, fsname, &pos))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);
  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  _f_set16bitl (de->lastaccessdate, adate);

  ret = _f_writedirsector (vi);
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_getaccessdate
 *
 * Get file access date
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - file to change date/time attributes.
 * padate - pointer where to store the date.
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_getaccessdate (F_MULTI * fm, const char *filename, unsigned short *padate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wgetaccessdate (fm, _towchar (hpb->buf, filename), padate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wgetaccessdate (F_MULTI * fm, const wchar *filename, unsigned short *padate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wgetaccessdate_h (fm, filename, padate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wgetaccessdate_h (F_MULTI * fm, const wchar * filename,
                     unsigned short *padate, F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  de = _find_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname,
                              &pos);
  if (!de)
  {
    if (!_f_findpath (vi, fsname, &pos))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);
    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);
    _add_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname, de,
                          &pos);
  }

  if (padate)
    *padate = _f_get16bitl (de->lastaccessdate);

  return F_SETLASTERROR (F_NO_ERROR);
}


/****************************************************************************
 *
 * fn_setaccmodtime
 *
 * Set a file time and date
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - file to change date/time attributes.
 * mtime - new write or modification time of the file
 * mdate - new write or modification date of the file
 * adate - new access date of the file.
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_setaccmodtime (F_MULTI * fm, const char *filename, unsigned short mtime,
                unsigned short mdate, unsigned short adate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wsetaccmodtime (fm, _towchar (hpb->buf, filename), mtime, mdate,
                               adate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wsetaccmodtime (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                 unsigned short mdate, unsigned short adate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wsetaccmodtime_h (fm, filename, mtime, mdate, adate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wsetaccmodtime_h (F_MULTI * fm, const wchar * filename, unsigned short mtime,
                     unsigned short mdate, unsigned short adate,
                     F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, fsname, &pos))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);
  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  _f_set16bitl (de->mtime, mtime);
  _f_set16bitl (de->mdate, mdate);
  _f_set16bitl (de->lastaccessdate, adate);

  ret = _f_writedirsector (vi);
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_getaccmodtime
 *
 * get a file time and date
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - which file time and date wanted to be retrive
 * pmtime - pointer where to store the write or modification time.
 * pmdate - pointer where to store the write or modification date.
 * padate - pointer where to store the access date.
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_getaccmodtime (F_MULTI * fm, const char *filename, unsigned short *pmtime,
                unsigned short *pmdate, unsigned short *padate)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wgetaccmodtime (fm, _towchar (hpb->buf, filename), pmtime,
                               pmdate, padate);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wgetaccmodtime (F_MULTI * fm, const wchar * filename, unsigned short *pmtime,
                unsigned short *pmdate, unsigned short *padate)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wgetaccmodtime_h (fm, filename, pmtime, pmdate, padate,
                                  &f_name_hnb->f_name, &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wgetaccmodtime_h (F_MULTI * fm, const wchar * filename,
        unsigned short *pmtime, unsigned short *pmdate, unsigned short *padate,
        F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  de = _find_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname,
                              &pos);
  if (!de)
  {
    if (!_f_findpath (vi, fsname, &pos))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);
    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);
    _add_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname, de,
                          &pos);
  }

  if (pmtime)
    *pmtime = _f_get16bitl (de->mtime);
  if (pmdate)
    *pmdate = _f_get16bitl (de->mdate);
  if (pmdate)
    *padate = _f_get16bitl (de->lastaccessdate);

  return F_SETLASTERROR (F_NO_ERROR);
}


/****************************************************************************
 *
 * fn_delete
 *
 * delete a file
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_delete (F_MULTI * fm, const char *filename)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wdelete (fm, _towchar (hpb->buf, filename));
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wdelete (F_MULTI * fm, const wchar * filename)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wdelete_h (fm, filename, &f_name_hnb->f_name,
                             &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wdelete_h (F_MULTI * fm, const wchar * filename, F_NAME *fsname,
                 F_LFNINT *lfn)
{
  F_POS dirpos, pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!(_f_findpath (vi, fsname, &dirpos)))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  (void) _memcpy ((void *) &pos, (void *) &dirpos, sizeof (F_POS));     /* copy directory pos */

  if (isonlydotordoubledots (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  if (de->attr & F_ATTR_DIR)
    return F_SETLASTERROR (F_ERR_INVALIDDIR);   /* directory */
  if (de->attr & F_ATTR_READONLY)
    return F_SETLASTERROR (F_ERR_ACCESSDENIED); /* readonly */

  if (_f_checklocked (fsname->drivenum, &pos))
    return F_SETLASTERROR (F_ERR_LOCKED);

  /* Lets invalidate the last cached directory entry. */
  _delete_cached_de_entry (&vi->last_entry);
  ret = _f_removelfn (vi, fsname->drivenum, dirpos.cluster, lfn);
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_removechain (vi, _f_getdecluster (vi, de));
#ifdef FATCACHE_ENABLE
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_fatcache_flush (vi, 0);
#endif
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_getattr (F_MULTI * fm, const char *filename, unsigned char *attr)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wgetattr (fm, _towchar (hpb->buf, filename), attr);
  hfat_free_path_buffer (hpb);
  return ret_val;
}
int
fn_wgetattr (F_MULTI * fm, const wchar * filename, unsigned char *attr)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wgetattr_h (fm, filename, attr, &f_name_hnb->f_name,
                              &f_lfn_hnb->f_lfnint);
  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wgetattr_h (F_MULTI * fm, const wchar * filename, unsigned char *attr,
                  F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_VOLUME *vi;
  F_DIRENTRY *de;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  /* The saved directory entry is searched first. If a match
   * is not found the directory entries in the device is searched.
   */
  de = _find_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname,
                              &pos);
  if (!de)
  {
    if (!_f_findpath (vi, fsname, &pos))
      return F_SETLASTERROR (F_ERR_INVALIDDIR);
    if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
      return F_SETLASTERROR (F_ERR_NOTFOUND);
    _add_cached_de_entry (&vi->last_entry, fsname->path, fsname->lname, de,
                          &pos);
  }

  if (attr)
  {
    *attr = de->attr;
  }

  return F_SETLASTERROR (F_NO_ERROR);
}

/****************************************************************************
 *
 * fn_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_setattr (F_MULTI * fm, const char *filename, unsigned char attr)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wsetattr (fm, _towchar (hpb->buf, filename), attr);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wsetattr (F_MULTI * fm, const wchar * filename, unsigned char attr)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wsetattr_h (fm, filename, attr, &f_name_hnb->f_name,
                              &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wsetattr_h (F_MULTI * fm, const wchar * filename, unsigned char attr,
                  F_NAME *fsname, F_LFNINT *lfn)
{
  F_POS pos;
  F_DIRENTRY *de;
  F_VOLUME *vi;
  int ret;

  if (_f_setfsname (fm, filename, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!_f_findpath (vi, fsname, &pos))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);
  if (!_f_findfile (vi, fsname->lname, &pos, &de, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  /* The attribute change may make the last entry out-dated. */
  _delete_cached_de_entry (&vi->last_entry);

  attr &= F_ATTR_HIDDEN | F_ATTR_READONLY | F_ATTR_ARC | F_ATTR_SYSTEM; /* keep only valid bits */

  attr |= de->attr & (F_ATTR_DIR | F_ATTR_VOLUME);      /* keep some original */

  if (de->attr == attr)
    return F_SETLASTERROR (F_NO_ERROR); /* its the same */

  de->attr = attr;

  ret = _f_writedirsector (vi);
  return F_SETLASTERROR (ret);
}

/****************************************************************************
 *
 * fn_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int
fn_rmdir (F_MULTI * fm, const char *dirname)
{
  int ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wrmdir (fm, _towchar (hpb->buf, dirname));
  hfat_free_path_buffer (hpb);
  return ret_val;
}

int
fn_wrmdir (F_MULTI * fm, const wchar * dirname)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wrmdir_h (fm, dirname, &f_name_hnb->f_name,
                            &f_lfn_hnb->f_lfnint);
  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static int
fn_wrmdir_h (F_MULTI * fm, const wchar * dirname, F_NAME *fsname,
                F_LFNINT *lfn)
{
  F_POS dirpos, pos;
  F_DIRENTRY *pde;
  F_VOLUME *vi;
  FN_FILE *file;
  unsigned long dircluster;
  int ret;

  if (_f_setfsname (fm, dirname, fsname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */
  if (_f_checknamewc (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);  /* invalid name */

  ret = _f_getvolume (fm, fsname->drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  if (!(_f_findpath (vi, fsname, &dirpos)))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  (void) _memcpy ((void *) &pos, (void *) &dirpos, sizeof (F_POS));     /* copy directory pos */

  if (isonlydotordoubledots (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);
  if (isonlydotsandspaces (fsname->lname))
    return F_SETLASTERROR (F_ERR_INVALIDNAME);

  if (!_f_findfile (vi, fsname->lname, &pos, &pde, lfn, 0))
    return F_SETLASTERROR (F_ERR_NOTFOUND);

  if (!(pde->attr & F_ATTR_DIR))
    return F_SETLASTERROR (F_ERR_INVALIDDIR);   /* not a directory */

  if (pde->attr & F_ATTR_READONLY)
    return F_SETLASTERROR (F_ERR_ACCESSDENIED);

  dircluster = _f_getdecluster (vi, pde);

  file = _f_opendir (vi, fsname->drivenum, dircluster);
  if (!file)
    return F_SETLASTERROR (F_ERR_INVALIDDIR);

  for (;;)
  {
    F_DIRENTRY de;
    char ch;

    ret = _f_readdir (vi, &de, file);
    if (ret == F_ERR_EOF)
      break;
    if (ret)
      return F_SETLASTERROR (_f_closedir (vi, file, ret));

    ch = de.name[0];
    if (!ch)
      break;                    /* last entry */
    if (ch == F_DELETED_CHAR)
      continue;                 /* deleted */
    if (de.name[0] == '.' && de.name[1] == ' ')
      continue;
    if (de.name[0] == '.' && de.name[1] == '.' && de.name[2] == ' ')
      continue;

    return F_SETLASTERROR (_f_closedir (vi, file, F_ERR_NOTEMPTY));     /* something is there */
  }

  ret = _f_closedir (vi, file, F_NO_ERROR);
  if (ret)
    return F_SETLASTERROR (ret);

  /* The last entry may become invalid because of the rmdir operation.*/
  _delete_cached_de_entry (&vi->last_entry);

  ret = _f_removelfn (vi, fsname->drivenum, dirpos.cluster, lfn);
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_removechain (vi, dircluster);
#ifdef FATCACHE_ENABLE
  if (ret)
    return F_SETLASTERROR (ret);

  ret = _f_fatcache_flush (vi, 0);
#endif
  return F_SETLASTERROR (ret);
}


/****************************************************************************
 *
 * fn_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 *
 * fm - multi structure pointer
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
fn_getlabel (F_MULTI * fm, int drivenum, char *label, long len)
{
  char tmplabel[11];
  long a;
  F_VOLUME *vi;
  int ret;
  FN_FILE *file;

  if (!len)
    return F_SETLASTERROR (F_NO_ERROR); /* how to store volume name? */

  ret = _f_getvolume (fm, drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  (void) _memcpy (tmplabel, vi->bootrecord.volume_name, 11);

  file = _f_opendir (vi, drivenum, 0);  /* open root; */
  if (!file)
    return F_SETLASTERROR (F_ERR_READ);

  for (;;)
  {
    F_DIRENTRY de;

    ret = _f_readdir (vi, &de, file);
    if (ret == F_ERR_EOF)
      break;
    if (ret)
      break;

    if (!de.name[0])
      break;                    /* empty,last */
    if (de.name[0] == F_DELETED_CHAR)
      continue;                 /* deleted */

    if (de.attr == F_ATTR_VOLUME)
    {
      (void) _memcpy (tmplabel, de.name, 8);
      (void) _memcpy (tmplabel + 8, de.ext, 3);
      break;
    }

  }

  ret = _f_closedir (vi, file, F_NO_ERROR);
  if (ret)
    return F_SETLASTERROR (ret);

  len--;
  for (a = 0; a < 11 && a < len; a++)
  {
    char ch = tmplabel[a];
    if (ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';
    label[a] = tmplabel[a];
  }
  label[a] = 0;                 /* terminate it, this is ok because of len--! */

  for (; a >= 0; a--)
  {                             /* cut the spaces */
    if (label[a] <= 32)
      label[a] = 0;
    else
      break;
  }

  return F_SETLASTERROR (F_NO_ERROR);
}

/****************************************************************************
 *
 * fn_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * drivenum - drive number which label's need to be set
 * label - new label for the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
int
fn_setlabel (F_MULTI * fm, int drivenum, const char *label)
{
  int ret_val;
  struct hfat_f_name_buffer *f_name_hnb;

  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_setlabel_h (fm, drivenum, label, &f_name_hnb->f_name);

  hfat_free_f_name_buffer (f_name_hnb);
  return ret_val;
}

static int
fn_setlabel_h (F_MULTI * fm, int drivenum, const char *label, F_NAME *fsname)
{
  int ret;
  char tmplabel[11];
  int a, b;
  F_VOLUME *vi;
  FN_FILE *file;
  F_POS pos;
  F_DIRENTRY *pde;

  ret = _f_getvolume (fm, drivenum, &vi);
  if (ret)
    return F_SETLASTERROR (ret);

  for (a = b = 0; a < 11; a++)
  {
    char ch = label[b];
    if (ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';

    if (ch >= 32)
      b++;
    else
      ch = 32;

    tmplabel[a] = ch;
  }

  file = _f_opendir (vi, drivenum, 0);  /* open root; */
  if (!file)
    return F_SETLASTERROR (F_ERR_READ);

  for (;;)
  {
    F_DIRENTRY de;

    ret = _f_readdir (vi, &de, file);
    if (ret == F_ERR_EOF)
      break;
    if (ret)
      return F_SETLASTERROR (_f_closedir (vi, file, ret));

    if (!de.name[0])
      break;                    /* empty, last */
    if (de.name[0] == F_DELETED_CHAR)
      continue;                 /* deleted */

    if (de.attr == F_ATTR_VOLUME)
    {
      (void) _memcpy ((void *) (de.name), (void *) tmplabel, 8);
      (void) _memcpy ((void *) (de.ext), (void *) (tmplabel + 8), 3);

      ret = _f_seekdir (vi, file, -1, FN_SEEK_CUR);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      ret = _f_writedir (vi, &de, file);
      if (ret)
        return F_SETLASTERROR (_f_closedir (vi, file, ret));

      return F_SETLASTERROR (_f_closedir (vi, file, F_NO_ERROR));
    }

  }

  ret = _f_closedir (vi, file, F_NO_ERROR);
  if (ret)
    return F_SETLASTERROR (ret);

  _f_clustertopos (vi, 0, &pos);

  fsname->drivenum = drivenum;
  fsname->path[0] = 0;
  fsname->lname[0] = 'X';        /* for dummy */
  fsname->lname[1] = 0;

  if (_f_addentry (vi, fsname, &pos, &pde))
    return F_SETLASTERROR (F_ERR_NOMOREENTRY);  /* couldnt be added */

  _f_initentry (pde, tmplabel, tmplabel + 8);
  pde->attr = F_ATTR_VOLUME;

  return F_SETLASTERROR (_f_writedirsector (vi));
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
 * fm - multi structure pointer
 * filename - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * filepointer or zero if fails
 *
 ***************************************************************************/

FN_FILE *
fn_truncate (F_MULTI * fm, const char *filename, unsigned long length)
{
  FN_FILE * ret_val;
  struct hfat_path_buffer *hpb;

  hpb = hfat_allocate_path_buffer (NULL);
  ret_val = fn_wtruncate (fm, _towchar (hpb->buf, filename), length);
  hfat_free_path_buffer (hpb);
  return ret_val;
}

FN_FILE *
fn_wtruncate (F_MULTI * fm, const wchar * filename, unsigned long length)
{
  FN_FILE * ret_val;
  struct hfat_f_name_buffer *f_name_hnb;
  struct hfat_f_lfnint_buffer *f_lfn_hnb;

  f_lfn_hnb = hfat_allocate_f_lfnint_buffer (NULL);
  f_name_hnb = hfat_allocate_f_name_buffer (NULL);

  ret_val = fn_wtruncate_h (fm, filename, length, &f_name_hnb->f_name,
                               &f_lfn_hnb->f_lfnint);

  hfat_free_f_name_buffer (f_name_hnb);
  hfat_free_f_lfnint_buffer (f_lfn_hnb);
  return ret_val;
}

static FN_FILE *
fn_wtruncate_h (F_MULTI * fm, const wchar * filename, unsigned long length,
                   F_NAME *fsname, F_LFNINT *lfn)
{
  F_DIRENTRY *de;
  F_VOLUME *vi;
  char m_mode = FN_FILE_WRP;
  FN_FILEINT *f = 0;
  int ret;

#ifdef HCC_UNICODE
  if (length == 0)
  {
    wchar mode[3];
    mode[0] = 'w';
    mode[1] = '+';
    mode[2] = 0;

    /* fn_wopen sets last error code */
    return fn_wopen (fm, filename, mode);
  }
#else
  /* fn_open sets last error code */
  if (length == 0)
    return fn_open (fm, filename, "w+");
#endif
  if (_f_setfsname (fm, filename, fsname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;                   /* invalid name */
  }
  if (_f_checknamewc (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;                   /* invalid name */
  }

  if (_f_getvolume (fm, fsname->drivenum, &vi))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDDRIVE);
    return 0;                   /* cant open any */
  }

  f = _f_getintfile ();
  if (!f)
  {
    F_SETLASTERROR_NORET (F_ERR_NOMOREENTRY);
    return 0;                   /* no more file could be opened  */
  }

  f->drivenum = fsname->drivenum;
  f->relpos = 0;
  f->abspos = 0;
  f->filesize = 0;
  f->pos.prevcluster = 0;
  f->modified = 0;
  f->state = 0;
  f->syncfile = 0;

  if (isonlydotordoubledots (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;
  }
  if (isonlydotsandspaces (fsname->lname))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDNAME);
    return 0;
  }

  if (!_f_findpath (vi, fsname, &f->dirpos))
  {
    F_SETLASTERROR_NORET (F_ERR_INVALIDDIR);
    return 0;
  }
  if (!_f_findfile (vi, fsname->lname, &f->dirpos, &de, lfn, 0))
  {
    F_SETLASTERROR_NORET (F_ERR_NOTFOUND);
    return 0;
  }
  if (_f_checklocked (fsname->drivenum, &f->dirpos))
  {
    F_SETLASTERROR_NORET (F_ERR_LOCKED);
    return 0;                   /* locked */
  }

  if (de->attr & F_ATTR_DIR)
  {
    F_SETLASTERROR_NORET (F_ERR_ACCESSDENIED);
    return 0;                   /* directory */
  }

  f->startcluster = _f_getdecluster (vi, de);

  if (f->startcluster)
  {
    long slen;

    _f_clustertopos (vi, f->startcluster, &f->pos);

    f->filesize = _f_get32bitl (de->filesize);
#if F_MAXSEEKPOS
    _fn_initseekdivisor (f, vi);
#endif

    f->abspos = (unsigned long) -F_SECTOR_SIZE; /* forcing seek to read 1st sector! abspos=0; */

    if (length > f->filesize)
      slen = f->filesize;
    else
      slen = length;

    ret = _f_fseek (vi, fm, f, slen);
    if (ret)
    {
      F_SETLASTERROR_NORET (ret);
      return 0;
    }
  }
  else
  {
    f->pos.sector = 0;
    f->pos.sectorend = 0;
#if F_MAXSEEKPOS
    _fn_initseekdivisor (f, vi);
#endif
  }

  if (length < f->filesize)
  {
    unsigned long cluster;

    ret = _f_getclustervalue (vi, f->pos.cluster, &cluster);
    if (ret)
    {
      F_SETLASTERROR_NORET (ret);
      return 0;
    }

    ret = _f_removechain (vi, cluster);
    if (ret)
    {
      F_SETLASTERROR_NORET (ret);
      return 0;
    }

    ret = _f_setclustervalue (vi, f->pos.cluster, F_CLUSTER_LAST);
    if (ret)
    {
      F_SETLASTERROR_NORET (ret);
      return 0;
    }

    f->filesize = length;
  }
  else if (length > f->filesize)
  {
    long remainingbytes = length - f->filesize;

    f->mode = m_mode;           /*  lock it  */

    ret = _f_writezeros (vi, fm, f, remainingbytes);
    if (ret)
    {
      F_SETLASTERROR_NORET (ret);
      return 0;
    }
  }

  f->mode = m_mode;             /*  lock it  */

  f->WrDataCache.pos = f->WrDataCache.posbuf;

  F_SETLASTERROR_NORET (F_NO_ERROR);
  return (FN_FILE *) (f->file.reference);
}

/****************************************************************************
 *
 * f_enable_lfn_support
 *
 * Enables or disables long filename support in hfat
 *
 * INPUTS
 *
 * enabled : 0 will disable LFN support and 1 will enable it.
 *
 ****************************************************************************/

void
fn_enable_lfn_support (char enabled)
{
  hfat_lfn_enabled = enabled;
}

int hfat_lfn_init (F_VOLUME *vi)
{
  _init_cached_de (&vi->last_entry);
  return 0;
}

/*
 * Compare two W_CHAR strings s1 and s2 (case-insensitive).
*/
static int
w_char_casecmp (const W_CHAR *s1, const W_CHAR *s2, size_t size)
{
  unsigned int i;

  for (i = 0; i < size; i++) {
    /* Return if current W_CHARs for the two strings do not match
     * (case-insensitive).
     */
    if (_f_toupper(s1[i]) != _f_toupper(s2[i])) {
      return (s1[i] - s2[i]);
    }
    /* If both strings end return 0.*/
    else if ( (!s1[i]) && (!s2[i])) {
      return 0;
    }
    /* Loop if current W_CHARs of the two strings  match.*/
  }
  return 0;
}

/*
 * Copy W_CHAR string src to W_CHAR string dest. The destination string
 * is ensured to be NULL terminated.
 */
static void
w_char_safe_copy (W_CHAR *dest, const W_CHAR *src, size_t size)
{
#ifdef HCC_UNICODE /* If this is enabled then we use short (16 bits). */
  (void) wstrlcpy (dest, src, size);
#else /* !HCC_UNICODE, then W_CHAR is one byte, use regular string copy. */
  (void) strlcpy (dest, src, size)
#endif /* HCC_UNICODE */

  return;
}

/*
 * Return the length of W_CHAR string. The string has to be NULL terminated.
 */
static uint32
w_char_strlen (const W_CHAR *ptr)
{
  uint32 i=0;

  /* Walk the string untill NULL is found */
  while (*(ptr++) != 0) {
    i++;
  }

  return i;
}

/*
 * Initializes the name cache.
 */
static void
_init_cached_de (t_CachedDe *lf)
{
  lf->used = 0;
  lf->pos.sector = 0;
  return;
}

/*
 * Add a name entry into the name cache.
 * Puts recently read or written directory name or file name and
 * corresponding directy entry sector information.
 */
static void
_add_cached_de_entry (t_CachedDe *lf, const W_CHAR *path, const W_CHAR *name,
                      F_DIRENTRY *dep, F_POS *pos)
{

  lf->used = 1;
  _memcpy (&lf->de, dep, sizeof (F_DIRENTRY));
  w_char_safe_copy (lf->path, path, FN_MAXPATH);
  w_char_safe_copy (lf->lname, name, F_MAXLNAME);
  lf->pos = *pos;

  return;
}

/*
 * Find the name cache entry and return the suggested sector for reading
 * data. This is done by comparing the cached directory entry 'lf' with the
 * path and name given.
 *
 * If name is NULL, comparison is done using only path. It compares lf->path
 * to the fist portion of 'path', and lf->lname with the following portion of
 * 'path'.
 */
static F_DIRENTRY *
_find_cached_de_entry (t_CachedDe *lf, const W_CHAR *path, const W_CHAR *name,
                       F_POS *pos)
{
  if (lf->used)
  {
    /* match only using path. */
    if (name == NULL)
    {
      uint32 len;

      /* lf->path and lf->lname are guaranteed to be NULL terminated,
       * see _add_cached_de_entry() */
      len = w_char_strlen (lf->path);

      /* compare lf->path with path up to the lenght of lf->path.
       * if the cached directory entry corresponds to a file or directory in
       * the root directory (a:/) lf->path will contain an empty string */
      if (len == 0 || (!w_char_casecmp (path, lf->path, len) &&
                       path[len] == F_SEPARATORCHAR))
      {
        /* Account for directory separator if needed. */
        if (len != 0)
          len++;

        /* Move path pointer to compare lname portion. */
        path += len;

        len = w_char_strlen (lf->lname);

        /* Add one character to compare to account for the end of string */
        len++;
        if (!w_char_casecmp (path, lf->lname, len))
        {
          /* Directory entry is cached, we can return it. */
          *pos =  lf->pos;
          return (&lf->de);
        }
      }
    }
    /* Match using path and name */
    else if ((!w_char_casecmp (name, lf->lname, F_MAXLNAME)) &&
             (!w_char_casecmp (path, lf->path, FN_MAXPATH)))
    {
      *pos =  lf->pos;
      return (&lf->de);
    }
  }

  return NULL;
}

/*
 * Delete the cached last directory entry.
 */
static void
_delete_cached_de_entry (t_CachedDe *lf)
{
  lf->used = 0;
}

/****************************************************************************
 *
 * end of fat_lfn.c
 *
 ***************************************************************************/

#endif /*  F_LONGFILENAME  */
