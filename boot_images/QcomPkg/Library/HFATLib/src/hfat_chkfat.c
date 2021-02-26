/****************************************************************************
 * hfat_chkfat.c
 *
 * Short description.
 * Copyright (C) 2006, 2008-2009, 2011-2012 QUALCOMM Technologies, Inc.
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
2012-12-05   nr    Fix compiler warnings.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-10-13   wek   Use standard library string copy and string concatenate.
2009-03-25   wek   Move/rename f_gettime f_getdate to fs_util.c
2009-03-17   wek   Renamed var ctime/cdate to mtime/mdate to avoid confusion.
2008-09-24   ebb   Updated param flags to be orthogonal.
2008-08-06   ebb   Featurized checkfat as FEATURE_HFAT_CHKFAT.
2008-08-05   ebb   Fixed compiler warning by adding debug flag for logging
2008-06-05   ebb   Fixed off-by-one error in macro BOUNDS_CHECK.
2008-06-04   ebb   Fixed lint errors and logic error causing non-seq naming.
2008-05-30   ebb   Added callback to hotplug_checkfat.
2008-05-24   rp    Fix compiler warning, got rid off unsed variable alloc.
2008-04-10   ebb   Made numerous changes and integrated into mainline
2006-10-10   sch   defined _malloc and _free as NULL when USE_MALLOC
                   is not defined
2006-10-09   sch   Dropped in HFAT 3.04
2006-07-06   sch   Create

===========================================================================*/
#include "fs_config_i.h"

#ifdef FEATURE_HFAT_CHKFAT

#include <stdio.h>
#include <stdlib.h>
#include "stringl/stringl.h"
#include "hfat_fat.h"
#include "hfat_common.h"
#include "hfat_chkfat.h"
#include "hfat_port_f.h"
#include "fs_unicode.h"
#include "fs_hotplug.h"
#include "fs_util.h"
#include "fs_err.h"


/* Statically allocate largest table size neccesary
 * Worst case: 32GB media, 32 sectors/cluster
 * (32 * 2^30) / 512 / 32 / 8 (1bit per cluster) + 1
 */
#define CTBL_SIZE 262145

#define SET_CLUSTER_BIT(ctbl,cluster) ctbl[cluster>>3]|=(1<<(cluster&7))
#define CLEAR_CLUSTER_BIT(ctbl,cluster) (ctbl[cluster>>3]&= \
                         (unsigned char)(~((unsigned long)(1<<(cluster&7)))))
#define GET_CLUSTER_BIT(ctbl,cluster) (ctbl[cluster>>3]&(1<<(cluster&7)))
#define BOUNDS_CHECK(cluster)  do { \
        if ((cluster >> 3) > CTBL_SIZE - 1) \
          FS_ERR_FATAL ("0x%08X,Accessing cluster out of bounds\n",cluster>>3,0,0); \
        } while(0)

#define WSIZEOF(x) (sizeof(x) / sizeof(x[0]))

#ifdef FS_UNIT_TEST
  #define LOG(fmt,x,y) printf(fmt, x, y)
#else
  #define LOG(fmt,x,y) FS_MSG_HIGH(fmt, x, y, 0)
#endif

/* Short filename length = 8 (name) + 3 (ext) + '.' + null */
#define SFN_LEN         F_MAXNAME + F_MAXEXT + 2

/* Lost names always between LOST0000 - LOST9999 */
#define LOST_LEN        9

#define FIX_MODE(mode)      ((mode & CHKFAT_FIX_ERRORS) == CHKFAT_FIX_ERRORS)
#define RECOVER_MODE(mode)  ((mode & CHKFAT_RECOVER_FILES) == CHKFAT_RECOVER_FILES)

static F_DIRENTRY de[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
static wchar full_path[F_MAXPATH];
static int lost_pos;
static int dirlevel;
static int err_mask;
static volatile int read_only = 1;

enum
{
  FC_BAD_CLUSTER = 100,
  FC_CROSS_CHAIN,
  FC_LAST_CLUSTER_CHANGED,
  FC_FILE,
  FC_DIR,
  FC_NOT_FOUND,
  FC_ERROR
};

enum
{
  FC_LOG_DIRCHANGE = 1,
  FC_LOG_DIRDEL,
  FC_LOG_DELETE_LOST,
  FC_LOG_DELETE_BAD_CLUSTER,
  FC_LOG_SIZE_CHANGE,
  FC_LOG_CLUSTER_CHANGE,
  FC_LOG_CROSS_LINK,
  FC_LOG_LOSTDIR_SAVE,
  FC_LOG_LOSTFILE_SAVE,
  FC_LOG_LOST_REMOVE,
  FC_LOG_LAST_CHANGED,
  FC_LOG_LOST_MOVE,
  FC_LOG_BACKDIR_ROOT,
  FC_LOG_FAT2_UPDATED,
  FC_LOG_DIR_DEPTH_EXCEEDED,
  FC_LOG_ALLOCATION_ERROR,
  FC_LOG_TMP,
#if F_LONGFILENAME
  FC_LOG_LFN_REMOVE,
#endif
  FC_LOG_NULL
};

#ifdef HFAT_CHKFAT_MESSAGES
static char *chkfat_logmsg[] = {
  "\nDirectory: ",
  "  Directory entry deleted: ",
  "  Lost entry deleted (found in a subdirectory): /",
  "  Entry deleted (reserved/bad cluster): ",
  "  File size changed: ",
  "  Start cluster changed: ",
  "  Entry deleted (cross linked chain): ",
  "  Lost directory chain saved: ",
  "  Lost file chain saved: ",
  "  Lost chain removed (first cluster/cnt): ",
  "  Last cluster changed (bad next cluster value): ",
  "  Moving lost directory: /",
  "  '..' changed to root: ",
  "FAT2 updated according to FAT1.",
  "  Maximum directory depth exceeded.",
  "  Not enough memory to complete operation.",
  "  file: ",
#if F_LONGFILENAME
  "  Long filename entry/entries removed. Count=",
#endif
  "\0"
};
#endif /* HFAT_CHKFAT_MESSAGES */

/* Stubs to redirect destructive calls */

static int
chkfat_fatcache_flush (F_VOLUME * vi, int clear)
{
  if (read_only)
    return 0;
  return _f_fatcache_flush (vi, clear);
}

static int
chkfat_writesector (F_VOLUME * vi, void *data, unsigned long sector, int cnt)
{
  if (read_only)
    return 0;
  return _f_writesector (vi, data, sector, cnt);
}


/* Invalidate cache temporarily so functions won't use cached values */
static void
chkfat_inval_dir_cache (F_VOLUME * vi)
{
  if (vi)
    vi->dircache_start = (unsigned long) -1;
}

static void
chkfat_invalidate_fat_cache (F_VOLUME *vi)
{
  int i;
  for (i = 0; i < vi->fatcache.N; i++)
    (vi->fatcache.dsc + i)->sector = (unsigned long) -1;

  vi->fatsector = (unsigned long) -1;
}
static int
chkfat_setclustervalue (F_VOLUME * vi, unsigned long cluster,
                        unsigned long data)
{
  int ret;
  if (read_only)
    return 0;

  /* Invalidate the cache */
  chkfat_invalidate_fat_cache (vi);

  /* Set the cluster */
  ret =_f_setclustervalue (vi, cluster, data);

  /* Flush the cache */
  _f_writefatsector (vi);
  _f_fatcache_flush (vi, 0);

  return ret;
}

static int
chkfat_getclustervalue (F_VOLUME *vi, unsigned long cluster,
                        unsigned long *pval)
{
  /* Invalidate cache */
  chkfat_invalidate_fat_cache (vi);

  /* Return cluster value */
  return _f_getclustervalue (vi, cluster, pval);
}

static int
chkfat_writedirsector (F_VOLUME * vi)
{
  int ret;
  if (read_only)
    return 0;
  ret = _f_writedirsector (vi);
  return ret;
}


/* Use UTF conversion function */
static wchar *
towchar (char *str)
{
  static wchar wchar_buf[F_MAXPATH];
  (void) fs_convert->to_utf16 (str, wchar_buf, F_MAXPATH);
  return wchar_buf;
}

/*
**  chkfat_setnamecnt
**
**  Convert position to string
**
**  Input:
**     name - name of file
**     pos - position
*/
static void
chkfat_setnamecnt (char *name, int pos)
{
  int i;

  i = 7;
  while (pos)
  {
    name[i--] = (char) ((pos % 10) + '0');
    pos /= 10;
  }
}

/*
**  chkfat_putlog
**
**  Makes an entry in the log file.
**
**  Input:
**      errnum - error number
**      text - text to put
*/
#ifdef HFAT_CHKFAT_MESSAGES
static void
chkfat_putlog (unsigned char errnum, char *text)
{
  if (errnum < 1 || errnum > FC_LOG_NULL)
    LOG ("Invalid chkfat error: %d\n-> %s", errnum, text);
  else
    LOG ("%s %s\n", chkfat_logmsg[errnum-1], text);
}
#else /* HFAT_CHKFAT_MESSAGES not defined */
static void
chkfat_putlog (unsigned char errnum, char *text)
{
  (void) errnum;
  (void) text;
}
#endif /* HFAT_CHKFAT_MESSAGES */


/*
**  chkfat_getlostname
**
**  Get next valid 'LOSTxxxx' entry in a specified directory
**
**  Input:
**     fm - multitask information
**     path - path where to find
**     lost_name - lost filename
**  Ouput:
**     FC_NO_ERROR - found a valid entry
**     FC_ERROR - no more entries available
*/
static int
chkfat_getlostname (F_MULTI * fm, wchar *path, char *lost_name)
{
  FN_WFIND find;

  /* Check length of lost_name !!!! */

  /* Check for a valid filename to put the lost chain in */
  (void) fn_wchdir (fm, path);

  if (lost_name[0] == 0 ||
      fn_wfindfirst (fm, towchar (lost_name), &find) == 0)
  {
    (void) strlcpy (lost_name, "LOST0000", LOST_LEN);
    do
    {
      chkfat_setnamecnt (lost_name, lost_pos++);
    }
    while (fn_wfindfirst (fm, towchar (lost_name), &find) == 0 &&
	   lost_pos < 10000);
  }
  else
    return FC_NO_ERROR;

  if (lost_pos < 10000)
    return FC_NO_ERROR;
  else
    return FC_ERROR;
}


/*
**  chkfat_checklostname
**
**  Checks if a name matches to 'LOSTxxxx'
**
**  Input: name - name to check
**  Output: FC_ERROR-no match / FC_NO_ERROR-match
*/
static int
chkfat_checklostname (char *name)
{
  unsigned int i;
  char lost[] = "LOST";

  for (i = 0; i < strlen (lost) && name[i] == lost[i]; i++);
  if (i == strlen (lost))
  {
    for (i = 4; i < 8 && name[i] >= '0' && name[i] <= '9'; i++);
    if (i == 8 && name[8] == 0)
    {
      return FC_NO_ERROR;
    }
  }

  return FC_ERROR;
}


/*
**  chkfat_getname
**
**  Converts a directory entry name/ext entry to a string
**
**  Input:
**     fde - pointer to directory entry
**     s - pointer to the destination string
*/
void
chkfat_getname (F_DIRENTRY * fde, char *s)
{
  int i, j = 0;

  for (i = 0; i < 8 && fde->name[i] != ' '; i++)
    s[i] = fde->name[i];
  if (fde->ext[0] != ' ')
  {
    s[i++] = '.';
    for (j = i, i = 0; i < 3 && fde->ext[i]; i++)
      s[j + i] = fde->ext[i];
  }
  s[j + i] = 0;
}




/*
**  chkfat_checkclusters
**
**  Check if a cluster chain is valid. If bad/empty or cross chained
**  element found in chain, then change last valid FAT entry to be
**  the last cluster.
**
**  Input:
**     vi - volume information
**     ctbl - pointer to FAT entry usage table
**     cluster - chain start cluster
**     cluster_num - number of clusters in chain
**     param - parameters
**  Output:
**     FC_CLUSTER_ERROR - error writing/reading cluster value
**     FC_LAST_CLUSTER_CHANGED - last cluster changed
**     FC_CROSS_CHAIN - chain first element is cross linked
**     FC_BAD_CLUSTER - chain first element is invalid
**     FC_NO_ERROR - chain valid
*/
static int
chkfat_checkclusters (F_VOLUME * vi, unsigned char *ctbl,
                      unsigned long cluster, unsigned long *cluster_num,
                      int param)
{
  unsigned long next_cluster;
  int rc = FC_NO_ERROR;
  unsigned long min_cluster;

  min_cluster = 2;
  if (vi->mediatype == F_FAT32_MEDIA)
    min_cluster++;
  *cluster_num = 0;

  if (cluster >= min_cluster && cluster != F_CLUSTER_RESERVED
      && cluster != F_CLUSTER_BAD)
  {
    for (;;)
    {
      BOUNDS_CHECK (cluster);

      /* Has this cluster been seen before */
      if (GET_CLUSTER_BIT (ctbl, cluster))
      {
        /* Cross chain encountered */
        err_mask |= CHKFAT_CROSS_CHAINED;

        /* Cross chain */
        if (FIX_MODE (param))
        {
          rc = FC_CROSS_CHAIN;
        }
        break;
      }
      SET_CLUSTER_BIT (ctbl, cluster);

      if (chkfat_getclustervalue (vi, cluster, &next_cluster))
      {
        rc = FC_CLUSTER_ERROR;
        break;
      }
      (*cluster_num)++;

      if (next_cluster < min_cluster || next_cluster == F_CLUSTER_RESERVED
          || next_cluster == F_CLUSTER_BAD)
      {

        /* Bad element found */
        err_mask |= CHKFAT_BAD_ELEMENT;

        /* Invalid cluster value in chain */
        if (FIX_MODE (param))
        {
          rc = FC_BAD_CLUSTER;
        }
        break;
      }
      else if (next_cluster >= F_CLUSTER_RESERVED)
        break;                  /* End of chain */

      cluster = next_cluster;
    }
  }
  else if (cluster)
    rc = FC_BAD_CLUSTER;

  return rc;
}




/*
**  chkfat_removechain
**
**  Remove a chain from FAT table.
**
**  Input:
**     vi - volume information
**     ctbl - pointer to the check table
**     cluster - start cluster of chain
**     size - number of clusters to remove
**     lost - deleting when handling lost chains
**
**  Output: 0-successfull
**          other-failed
*/
static int
chkfat_removechain (F_VOLUME * vi, unsigned char *ctbl, unsigned long cluster,
                    unsigned long size, int lost)
{
  unsigned long next_cluster;
  while (size--)
  {
    BOUNDS_CHECK (cluster);
    if (chkfat_getclustervalue (vi, cluster, &next_cluster))
      return FC_CLUSTER_ERROR;
    if (chkfat_setclustervalue (vi, cluster, 0))
      return FC_CLUSTER_ERROR;
    if (lost)
      SET_CLUSTER_BIT (ctbl, cluster);
    else
      CLEAR_CLUSTER_BIT (ctbl, cluster);
    cluster = next_cluster;
  }
  return FC_NO_ERROR;
}




/*
**  chkfat_getnextentry
**
**  Gets the next entry from a directory structure.
**
**  Input:
**     vi - volume information
**     act_pos - actual directory position
**     dirsectorbuf - buffer where actual directory sector is stored
**     direntry - pointer to the new directory entry (in dirsectorbuf)
**     dirsector - actual sector number (what dirsectorbuf holds)
**     changed - when set save sector at sector change
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_NO_ERROR - directory entry valid
**     FC_ERROR - no more directory entries
*/
int
chkfat_getnextentry (F_VOLUME * vi, F_POS * act_pos,
                     F_DIRENTRY * dirsectorbuf, F_DIRENTRY ** direntry,
                     unsigned long *dirsector, int *changed)
{
  while (act_pos->cluster < F_CLUSTER_RESERVED)
  {
    for (; act_pos->sector < act_pos->sectorend; act_pos->sector++)
    {
      if (*dirsector != act_pos->sector)
      {
        if (_f_readsector (vi, (char *) dirsectorbuf, act_pos->sector, 1))
          return FC_READ_ERROR;
        *dirsector = act_pos->sector;
      }

      for (; act_pos->pos < F_SECTOR_SIZE / sizeof (F_DIRENTRY);
           act_pos->pos++)
      {
        F_DIRENTRY *ade = dirsectorbuf + (act_pos->pos);

        if (ade->name[0] != 0 && ade->name[0] != F_DELETED_CHAR)
        {
          *direntry = ade;
          ++act_pos->pos;
          return FC_NO_ERROR;
        }
      }

      if (*changed)
      {
        if (chkfat_writesector (vi, (char *) dirsectorbuf, act_pos->sector, 1))
          return FC_WRITE_ERROR;
        *changed = 0;
      }

      act_pos->pos = 0;
    }

    if (act_pos->cluster == 0)
    {
      if (vi->mediatype == F_FAT32_MEDIA)
        act_pos->cluster = vi->bootrecord.rootcluster;
      else
        return FC_ERROR;
    }

    {
      unsigned long next_cluster;
      if (chkfat_getclustervalue (vi, act_pos->cluster, &next_cluster))
        return FC_CLUSTER_ERROR;
      if (next_cluster >= F_CLUSTER_RESERVED)
        return FC_ERROR;
      _f_clustertopos (vi, next_cluster, act_pos);
    }
  }

  return FC_ERROR;
}


#if F_LONGFILENAME
/*
**  chkfat_deldirentries
**
**  Delete entries from a directory
**
**  Input:
**     vi - volume information
**     start_pos - start position
**     cnt - number of entries to delete
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_NO_ERROR - success
**     FC_ERROR - no more directory entries
*/
static int
chkfat_deldirentries (F_VOLUME * vi, F_POS * start_pos, int cnt)
{
  static F_DIRENTRY de0[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  F_DIRENTRY *fde;
  unsigned long dirsector;
  int changed;
  int rc;

  dirsector = (unsigned long) (-1);
  changed = 0;
  while ((rc =
          chkfat_getnextentry (vi, start_pos, &(de0[0]), &fde, &dirsector,
                               &changed)) == FC_NO_ERROR)
  {
    fde->name[0] = F_DELETED_CHAR;
    changed = 1;
    if (--cnt == 0)
    {
      if (chkfat_writesector (vi, (char *) &(de0[0]), start_pos->sector, 1))
        return FC_WRITE_ERROR;
      return FC_NO_ERROR;
    }
  }

  return rc;
}
#endif


/*
**  chkfat_lost_changebackdir
**
**  Change '..' of LOSTxxxx directories in root directory to point to
**  root directory  when needed.
**
**  Input:
**     vi - volume information
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_NO_ERROR - success
*/
static int
chkfat_lost_changebackdir (F_VOLUME * vi)
{
  static F_DIRENTRY de0[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  static F_DIRENTRY de1[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  F_DIRENTRY *fde;
  F_POS start_pos, tmp_pos;
  unsigned long dirsector;
  int changed;
  int rc;

  unsigned long start_cluster;
  char fullname[SFN_LEN];

  dirsector = (unsigned long) (-1);
  changed = 0;
  _f_clustertopos (vi, 0, &start_pos);
  while ((rc =
          chkfat_getnextentry (vi, &start_pos, &(de0[0]), &fde, &dirsector,
                               &changed)) == FC_NO_ERROR)
  {
    if (fde->attr & F_ATTR_VOLUME)
      continue;
    if ((fde->attr & F_ATTR_DIR) == 0)
      continue;

    chkfat_getname (fde, fullname);
    start_cluster = _f_getdecluster (vi, fde);

    if (chkfat_checklostname (fullname) == FC_NO_ERROR)
    {
      _f_clustertopos (vi, start_cluster, &tmp_pos);
      if (_f_readsector (vi, (char *) &(de1[0]), tmp_pos.sector, 1))
        return FC_READ_ERROR;
      start_cluster = _f_getdecluster (vi, &(de1[1]));

      if (start_cluster)
      {
        _f_setdecluster (vi, &(de1[1]), 0);
        if (chkfat_writesector (vi, (char *) &(de1[0]), tmp_pos.sector, 1))
          return FC_WRITE_ERROR;
        chkfat_putlog (FC_LOG_BACKDIR_ROOT, fullname);
      }
    }
  }

  if (rc == FC_ERROR)
    rc = FC_NO_ERROR;
  return rc;
}



/*
**  chkfat_lost_checkcluster
**
**  Check if cluster match occurs, if yes delete LOSTxxxx from root directory.
**
**  Input:
**     vi - volume information
**     cluster - cluster to comapre to
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_NO_ERROR - success
**     FC_ERROR - not found
*/
static int
chkfat_lost_checkcluster (F_VOLUME * vi, unsigned long cluster, int param)
{
  static F_DIRENTRY de0[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  F_DIRENTRY *fde;
  F_POS start_pos;
  unsigned long dirsector;
  int changed;
  int rc;

  unsigned long start_cluster;
  char fullname[SFN_LEN];


  dirsector = (unsigned long) (-1);
  changed = 0;
  _f_clustertopos (vi, 0, &start_pos);
  while ((rc =
          chkfat_getnextentry (vi, &start_pos, &(de0[0]), &fde, &dirsector,
                               &changed)) == FC_NO_ERROR)
  {
    if (fde->attr & F_ATTR_VOLUME)
      continue;

    chkfat_getname (fde, fullname);
    start_cluster = _f_getdecluster (vi, fde);

    if (chkfat_checklostname (fullname) == FC_NO_ERROR)
    {
      if (start_cluster == cluster)
      {
        err_mask |= CHKFAT_BAD_DIR_REFERENCE;
        if (FIX_MODE (param))
        {
          fde->name[0] = F_DELETED_CHAR;
          if (chkfat_writesector (vi, (char *) &(de0[0]), start_pos.sector, 1))
            return FC_WRITE_ERROR;
          chkfat_putlog (FC_LOG_DELETE_LOST, fullname);
        }
        return FC_NO_ERROR;
      }
    }
  }

  return rc;
}



/*
**  chkfat_lost_movelostdir
**
**  Check if dst_pos cluster matches to /LOSTxxxx '..' directory, if yes
**  move /LOSTxxxx to dst_pos
**
**  Input:
**     fm - multitask information
**     vi - volume information
**     dst_pos - destination directory position
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_NO_ERROR - success
*/
static int
chkfat_lost_movelostdir (F_MULTI * fm, F_VOLUME * vi, F_POS * dst_pos)
{
  static F_DIRENTRY de0[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  static F_DIRENTRY de1[F_SECTOR_SIZE / sizeof (F_DIRENTRY)];
  F_DIRENTRY de2;
  F_DIRENTRY *fde;
  F_POS start_pos, tmp_pos;
  unsigned long dirsector;
  int changed;
  int rc;
  int i;

  unsigned long start_cluster;
  char fullname[SFN_LEN];

  dirsector = (unsigned long) (-1);
  changed = 0;
  _f_clustertopos (vi, 0, &start_pos);
  while ((rc =
          chkfat_getnextentry (vi, &start_pos, &(de0[0]), &fde, &dirsector,
                               &changed)) == FC_NO_ERROR)
  {
    if (fde->attr & F_ATTR_VOLUME)
      continue;
    if ((fde->attr & F_ATTR_DIR) == 0)
      continue;

    chkfat_getname (fde, fullname);
    start_cluster = _f_getdecluster (vi, fde);

    if (chkfat_checklostname (fullname) == FC_NO_ERROR)
    {
      for (i = 0; i < 8 && full_path[i + 1] == fullname[i]; i++);

      if (i < 8)
      {
        _f_clustertopos (vi, start_cluster, &tmp_pos);
        if (_f_readsector (vi, (char *) &(de1[0]), tmp_pos.sector, 1))
          return FC_READ_ERROR;
        start_cluster = _f_getdecluster (vi, &(de1[1]));
        if (start_cluster == dst_pos->cluster)
        {
          /* Move /LOSTnnnn to dst_pos/LOSTxxxx */
          F_NAME fsname;
          char tmpname[SFN_LEN];

          (void) _memcpy (&de2, fde, sizeof (F_DIRENTRY));
          fde->name[0] = F_DELETED_CHAR;

          if (chkfat_writesector (vi, (char *) &(de0[0]), start_pos.sector, 1))
            return FC_WRITE_ERROR;

          (void) strlcpy (tmpname, fullname, sizeof (tmpname));
          if (chkfat_getlostname (fm, full_path, tmpname) == 0)
          {
#if F_LONGFILENAME
            (void) _memcpy ((void *) (fsname.lname), (void *) tmpname, 8);
            fsname.lname[8] = 0;
#else
            (void) _memcpy ((void *) (fsname.filename), (void *) tmpname, 8);
            (void) _memset (fsname.fileext, ' ', 3);
#endif
            /* Invalidate cache to make sure data is fresh */
            chkfat_inval_dir_cache (vi);
            if ((read_only == 0) &&
                (_f_addentry (vi, &fsname, dst_pos, &fde) == 0))
            {
              (void) _memcpy ((void *) ((char *) fde + F_MAXNAME + F_MAXEXT),
                              (void *) ((char *) (&de2) + F_MAXNAME +
                                        F_MAXEXT),
                              sizeof (F_DIRENTRY) - F_MAXNAME - F_MAXEXT);
              if (chkfat_writedirsector (vi))
                return FC_WRITE_ERROR;
            }
          }

          chkfat_putlog (FC_LOG_LOST_MOVE, fullname);
          return FC_NO_ERROR;
        }
      }
    }
  }

  if (rc == FC_ERROR)
    rc = FC_NO_ERROR;
  return rc;
}



/*
**  chkfat_files
**
**  Go through all the directories and subdirectories and check for
**  inconsistencies.
**
**  Checks:
**      cluster chains,
**      '.'/'..' in directories,
**      size of files.
**
**  Input:
**     vi - volume information
**     ctbl - pointer to the check table
**     act_pos - actual directory position
**     back_pos - previous directory position
**     lost - directory is a lost chain
**     param - parameters
**     depth - actual directory depth
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_NO_ERROR - success
*/
static int
chkfat_files (F_MULTI * fm, F_VOLUME * vi, unsigned char *ctbl,
              F_POS * act_pos, F_POS * back_pos, int lost, int param,
              int depth)
{
  F_POS npos, fpos;
  F_POS *first_pos = &fpos;
  F_POS *next_pos = &npos;
  unsigned long start_cluster;
  unsigned long calc_size = 0, real_size;
  unsigned long tmp;

  char fullname[SFN_LEN];

  int changed = 0;
  int rc;

  if (depth > CHKFAT_MAX_DIR_DEPTH)
  {
    chkfat_putlog (FC_LOG_DIR_DEPTH_EXCEEDED, NULL);
    err_mask |= CHKFAT_DIR_DEPTH_EXCEEDED;
    return FC_ERROR;
  }

  (void) _memcpy ((void *) first_pos, (void *) act_pos, sizeof (F_POS));

  /* Stay within FAT table */
  while (act_pos->cluster < F_CLUSTER_RESERVED)
  {
    for (; act_pos->sector < act_pos->sectorend; act_pos->sector++)
    {
      if (_f_readsector (vi, (char *) &(de[0]), act_pos->sector, 1))
        return FC_READ_ERROR;

      for (; act_pos->pos < F_SECTOR_SIZE / sizeof (F_DIRENTRY);
           act_pos->pos++)
      {
        F_DIRENTRY *ade = &de[act_pos->pos];

        if (ade->name[0] == 0 || ade->name[0] == F_DELETED_CHAR
            || ade->attr & F_ATTR_VOLUME)
          continue;

        chkfat_getname (ade, fullname);
        start_cluster = _f_getdecluster (vi, ade);

        if (!(ade->attr & F_ATTR_DIR)
            || (ade->attr & F_ATTR_DIR && fullname[0] != '.'))
        {
          if (lost)
          {
            rc = chkfat_lost_checkcluster (vi, start_cluster, param);
            if (rc == FC_NO_ERROR)
              continue;
            if (rc != FC_ERROR)
              return rc;
          }

          /* Check cluster chain and get the number of clusters */
          rc =
            chkfat_checkclusters (vi, ctbl, start_cluster, &calc_size, param);

          if (rc == FC_BAD_CLUSTER && FIX_MODE (param))
          {
            /* bad or reserved cluster in chain */
            /* remove file */
            (void) chkfat_removechain (vi, ctbl, start_cluster, calc_size,
                                       lost);
            ade->name[0] = F_DELETED_CHAR;
            changed = 1;
            chkfat_putlog (FC_LOG_DELETE_BAD_CLUSTER, fullname);
            err_mask |= CHKFAT_BAD_ELEMENT;
          }
          else if (rc == FC_CROSS_CHAIN && FIX_MODE (param))
          {
            /* cluster in chain used by an other file */
            (void) chkfat_removechain (vi, ctbl, start_cluster, calc_size,
                                       lost);
            ade->name[0] = F_DELETED_CHAR;
            changed = 1;
            chkfat_putlog (FC_LOG_CROSS_LINK, fullname);
            err_mask |= CHKFAT_CROSS_CHAINED;
          }
          else if (rc == FC_LAST_CLUSTER_CHANGED)
          {
            chkfat_putlog (FC_LOG_LAST_CHANGED, fullname);
            rc = FC_NO_ERROR;
          }
        }
        else
          rc = FC_NO_ERROR;


        if (rc == FC_NO_ERROR)
        {
          if (ade->attr & F_ATTR_DIR)
          {
            if (fullname[0] == '.'
                || (fullname[0] == '.' && fullname[1] == '.'))
            {
              if (back_pos)
              {
                if (fullname[1] == '.')
                {
                  if (start_cluster != back_pos->cluster && FIX_MODE (param))
                  {
                    /* Error, '..' pointing to a wrong place */
                    chkfat_putlog (FC_LOG_CLUSTER_CHANGE, fullname);
                    err_mask |= CHKFAT_BAD_DIR_REFERENCE;
                    _f_setdecluster (vi, ade, back_pos->cluster);
                    changed = 1;
                  }
                }
                else  /* Not '..' */
                {
                  if (start_cluster != act_pos->cluster && FIX_MODE (param))
                  {
                    /* Error, '.' pointing to a wrong place */
                    chkfat_putlog (FC_LOG_CLUSTER_CHANGE, fullname);
                    err_mask |= CHKFAT_BAD_DIR_REFERENCE;
                    _f_setdecluster (vi, ade, act_pos->cluster);
                    changed = 1;
                  }
                }
              }
              else /* back_pos == 0 */
              {
                /* Error, root directory contains '.' / '..'
                   directory, delete them */
                chkfat_putlog (FC_LOG_DIRDEL, fullname);
                ade->name[0] = F_DELETED_CHAR;
                changed = 1;
              }
            }
            else /* fullname[0] == '.' */
            {
              if (changed)
              {
                if (chkfat_writesector
                    (vi, (char *) &(de[0]), act_pos->sector, 1))
                  return FC_WRITE_ERROR;

                changed = 0;
              }

              _f_clustertopos (vi, start_cluster, next_pos);
	      ++depth;
              rc =
                chkfat_files (fm, vi, ctbl, next_pos, first_pos, lost, param,
                              depth);
              --depth;
              if (rc)
                return rc;

              if (_f_readsector (vi, (char *) &(de[0]), act_pos->sector, 1))
                return FC_READ_ERROR;
            }
          }
          else /* NOT F_ATTR_DIR */
          {
            /* Do size check, compare max. size calculated from number
               of clusters and the size stored in the directory entry */
            tmp = vi->bootrecord.sector_per_cluster * F_SECTOR_SIZE;
            calc_size *= tmp;
            real_size = _f_get32bitl (ade->filesize);
            if (calc_size - real_size >= tmp)
            {
              err_mask |= CHKFAT_SHORT_ENTRY_INVALID;

              if (FIX_MODE (param))
              {
                chkfat_putlog (FC_LOG_SIZE_CHANGE, "File Size Updated.");
                _f_set32bitl (ade->filesize, calc_size);
                changed = 1;
              }
            }
          }
        } /* rc != NO_ERROR */
      } /* END for loop */

      if (changed)
      {
        if (chkfat_writesector (vi, (char *) &(de[0]), act_pos->sector, 1))
          return FC_WRITE_ERROR;
        changed = 0;
      }

      act_pos->pos = 0;
    }


    if (act_pos->cluster == 0)
    {
      if (vi->mediatype == F_FAT32_MEDIA)
        act_pos->cluster = vi->bootrecord.rootcluster;
      else
        break;
    }

    {
      unsigned long next_cluster;
      if (chkfat_getclustervalue (vi, act_pos->cluster, &next_cluster))
        return FC_CLUSTER_ERROR;
      if (next_cluster >= F_CLUSTER_RESERVED)
        break;
      _f_clustertopos (vi, next_cluster, act_pos);
    }
  }

  return FC_NO_ERROR;
}



#if F_LONGFILENAME

/* If logging enbaled, count the number of entries deleted */
static int remove_cnt;

/*
**  chkfat_lfn
**
**  Check long filename entries.
**  Delete if checksum,order number or clusterlo is bad.
**
**  Input:
**     vi - volume information
**     act_pos - position to start checking from
**     depth - actual directory depth
**     param - operating parameters
**
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_NO_ERROR - success
*/
static int
chkfat_lfn (F_VOLUME * vi, F_POS * act_pos, int depth, int param)
{
  F_POS next_pos;
  int rc;
  int changed;

  int lfn_remove;
  F_POS lfn_pos;
  unsigned char lfn_chksum;
  unsigned char lfn_ord;
  unsigned char lfn_cnt;


  if (depth > CHKFAT_MAX_DIR_DEPTH)
  {
    chkfat_putlog (FC_LOG_DIR_DEPTH_EXCEEDED, NULL);
    err_mask |= CHKFAT_DIR_DEPTH_EXCEEDED;
    return FC_ERROR;
  }

  lfn_ord = 0;
  lfn_cnt = 0;
  lfn_chksum = 0;
  lfn_remove = 0;
  changed = 0;
  (void) _memset ((void *) (&lfn_pos), 0, sizeof (F_POS));
  while (act_pos->cluster < F_CLUSTER_RESERVED)
  {
    for (; act_pos->sector < act_pos->sectorend; act_pos->sector++)
    {
      if (_f_readsector (vi, (char *) &(de[0]), act_pos->sector, 1))
        return FC_READ_ERROR;

      for (; act_pos->pos < F_SECTOR_SIZE / sizeof (F_DIRENTRY);
           act_pos->pos++)
      {
        F_DIRENTRY *ade = &de[act_pos->pos];

        if (ade->name[0] && ade->name[0] != F_DELETED_CHAR)
        {
          if (ade->attr == F_ATTR_LFN)
          {
            F_LFN *lfn = (F_LFN *) ade;
            unsigned char pos;
            pos = (unsigned char) (lfn->ord & 0x3f);
            if (pos >= 1 && pos <= 20 && _f_get16bitl (lfn->clusterlo) == 0)
            {
              if (lfn_cnt)
              {
                if (lfn->ord + 1 != lfn_ord || lfn_chksum != lfn->chksum)
                  lfn_remove = 1;
                ++lfn_cnt;
              }
              else
              {
                lfn_remove = 0;
                if ((lfn->ord & 0x40) == 0)
                  lfn_remove = 1;
                lfn_chksum = lfn->chksum;
                (void) _memcpy ((void *) (&lfn_pos), (void *) act_pos,
                                sizeof (F_POS));
                lfn_cnt = 1;
              }
              lfn_ord = pos;
            }
            else
            {
              /* Invalid entry encountered */
              err_mask |= CHKFAT_LONG_ENTRY_INVALID;
              if (FIX_MODE (param))
              {
                ade->name[0] = F_DELETED_CHAR;
                changed = 1;
                ++remove_cnt;
              }
            }
            continue;
          }
          else if (ade->attr & F_ATTR_VOLUME)
          {
            lfn_remove = 1;
          }
        }
        else if (lfn_cnt)
          lfn_remove = 1;

        if (lfn_cnt)
        {
          if (lfn_remove == 0)
          {
            unsigned char chksum;
            int i;

            chksum = 0;
            for (i = 0; i < 8; i++)
              chksum =
                (unsigned
                 char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
                        ade->name[i]);
            for (i = 0; i < 3; i++)
              chksum =
                (unsigned
                 char) ((((chksum & 1) << 7) | ((chksum & 0xfe) >> 1)) +
                        ade->ext[i]);
            if (lfn_chksum != chksum)
              lfn_remove = 1;
          }

          if (lfn_remove)
          {
            if (changed && lfn_pos.sector != act_pos->sector)
            {
              if (chkfat_writesector (vi, (char *) &(de[0]),
                                      act_pos->sector, 1))
                return FC_WRITE_ERROR;
              changed = 0;
            }
            (void) chkfat_deldirentries (vi, &lfn_pos, lfn_cnt);
            lfn_remove = 0;
            ++remove_cnt;
            if (_f_readsector (vi, (char *) &(de[0]), act_pos->sector, 1))
              return FC_READ_ERROR;
          }

          lfn_cnt = 0;
        }

        if (ade->name[0] == 0 || ade->name[0] == F_DELETED_CHAR)
          continue;

        if (ade->attr & F_ATTR_VOLUME)
          continue;

        if (ade->attr & F_ATTR_DIR)
        {
          if (ade->name[0] != '.')
          {
            _f_clustertopos (vi, _f_getdecluster (vi, ade), &next_pos);
            ++depth;
            rc = chkfat_lfn (vi, &next_pos, depth, param);
            --depth;
            if (rc)
              return rc;
            if (_f_readsector (vi, (char *) &(de[0]), act_pos->sector, 1))
              return FC_READ_ERROR;
          }
        }
      }

      if (changed)
      {
        if (chkfat_writesector (vi, (char *) &(de[0]), act_pos->sector, 1))
          return FC_WRITE_ERROR;
        changed = 0;
      }

      act_pos->pos = 0;
    }


    if (act_pos->cluster == 0)
    {
      if (vi->mediatype == F_FAT32_MEDIA)
        act_pos->cluster = vi->bootrecord.rootcluster;
      else
        break;
    }

    {
      unsigned long next_cluster;
      if (chkfat_getclustervalue (vi, act_pos->cluster, &next_cluster))
        return FC_CLUSTER_ERROR;
      if (next_cluster >= F_CLUSTER_RESERVED)
        break;
      _f_clustertopos (vi, next_cluster, act_pos);
    }
  }

  return FC_NO_ERROR;
}

#endif




/*
**  chkfat_findlostdir
**
**  1) Checks whether a directory with start cluster 'cluster' exists
**     in the directory structure
**  2) Checks whether LOSTnnnn directory '..' exists in any other
**     LOSTnnnn directory structure,
**     if yes then move entry from '/LOSTnnnn' to the proper place.
**
**  Input:
**     fm - multitask information
**     vi - volume information
**     cluster - first cluster of directory (used for case 1)
**     lost - look only for lost directories and check wheter it's
**            member of any lost directory structure.
**     depth - actual directory depth.
**  Output:
**     0 - found
**     1 - not found
*/
static int
chkfat_findlostdir (F_MULTI * fm, F_VOLUME * vi, unsigned long cluster,
                    int lost, int depth)
{
  FN_WFIND find[1];
  int len;
  char fstr[8];

  F_POS start_pos, dst_pos;
  int rc;

  if (depth > CHKFAT_MAX_DIR_DEPTH)
  {
    chkfat_putlog (FC_LOG_DIR_DEPTH_EXCEEDED, NULL);
    err_mask |= CHKFAT_DIR_DEPTH_EXCEEDED;
    return FC_ERROR;
  }

  _f_clustertopos (vi, 0, &start_pos);
  if (lost && cluster == 0)
  {
    (void) strlcpy (fstr, "LOST*.*", sizeof (fstr));
    ++cluster;
  }
  else
  {
    if (!cluster)
      return 0;
    (void) strlcpy (fstr, "*.*", sizeof (fstr));
  }

  if (fn_wfindfirst (fm, towchar (fstr), find) == 0)
  {
    do
    {
      if ((find->attr) & F_ATTR_DIR)
      {

        if (find->filename[0] != '.')
        {
          len = wstrlen (full_path);
          if (len + wstrlen (find->filename) + 2 < F_MAXPATH - 1)
          {
            if (full_path[1])
              (void) wstrlcat (full_path, towchar ("/"), WSIZEOF (full_path));
            (void) wstrlcat (full_path, find->filename, WSIZEOF (full_path));
          }
          else
          {
            (void) wstrlcat (full_path, towchar ("-"), WSIZEOF(full_path));
          }

          if (lost)
          {
            _f_clustertopos (vi, find->cluster, &dst_pos);
            rc = chkfat_lost_movelostdir (fm, vi, &dst_pos);
            if (rc == FC_NO_ERROR)
            {
              (void) fn_wchdir (fm, find->filename);
              ++dirlevel;
              ++depth;
              rc = chkfat_findlostdir (fm, vi, cluster, lost, depth);
              --depth;
              if (rc != FC_NO_ERROR && rc != FC_ERROR)
              {
                return rc;
              }
              --dirlevel;
              if (dirlevel)
                (void) fn_wchdir (fm, towchar (".."));
              else
                (void) fn_wchdir (fm, towchar ("/"));
              full_path[len] = 0;
            }
            else
            {
              return rc;
            }
          }
          else
          {
            if (find->cluster != cluster)
            {
              (void) fn_wchdir (fm, find->filename);
              ++dirlevel;
              ++depth;
              rc = chkfat_findlostdir (fm, vi, cluster, lost, depth);
              --depth;
              if (rc == FC_NO_ERROR)
              {
                return FC_NO_ERROR;
              }
              --dirlevel;
              if (dirlevel)
                (void) fn_wchdir (fm, towchar (".."));
              else
                (void) fn_wchdir (fm, towchar ("/"));
              full_path[len] = 0;
            }
            else
            {
              return FC_NO_ERROR;
            }
          }
        }
      }
    }
    while (fn_wfindnext (fm, find) == 0);
  }

  return FC_ERROR;
}





/*
**  chkfat_savelostchain
**
**  Checks whether a cluster content is a directory or not.
**
**  Input:
**      fm - multitask information
**      vi - volume information
**      ctbl - cluster table
**      first_cluster - first cluster of the chain
**      csize - number of clusters in the chain
**      param - parameters
**  Output:
**      FC_NO_ERROR-seems to be a directory,
**      FC_ERROR-not a directory
**
*/
static int
chkfat_savelostchain (F_MULTI * fm, F_VOLUME * vi, unsigned char *ctbl,
                      unsigned long first_cluster, unsigned long csize,
                      int param)
{
  F_POS pos, dirpos;
  F_NAME fsname;
  F_DIRENTRY *direntry;
  char lost_name[LOST_LEN];
  unsigned char attr;

  unsigned long back_cluster;
  char fullname[SFN_LEN];
  int rc;
  int dir = 0;

  full_path[0] = '/';
  full_path[1] = 0;
  lost_name[0] = 0;
  attr = F_ATTR_ARC;


  if (chkfat_getlostname (fm, towchar ("/"), lost_name) == 0)
  {
    _f_clustertopos (vi, first_cluster, &pos);
    /* Get data sector of first cluster */
    if (_f_readsector (vi, (char *) &(de[0]), pos.sector, 1))
      return FC_READ_ERROR;

    /* Check if it's a sub-directory */
    chkfat_getname (&(de[0]), fullname);
    if (fullname[0] == '.' && fullname[1] == 0 && de[0].attr & F_ATTR_DIR)
    {
      if (_f_getdecluster (vi, &(de[0])) == first_cluster)
      {
        chkfat_getname (&(de[1]), fullname);
        if (fullname[0] == '.' && fullname[1] == '.' && fullname[2] == 0
            && de[1].attr & F_ATTR_DIR)
        {
          back_cluster = _f_getdecluster (vi, &(de[1]));
          _f_clustertopos (vi, back_cluster, &dirpos);
          rc = chkfat_files (fm, vi, ctbl, &pos, &dirpos, 1, param, 0);
          if (rc)
            return rc;

          (void) fn_wchdir (fm, towchar ("/"));
          if (chkfat_findlostdir (fm, vi, back_cluster, 0, 0) == FC_NO_ERROR)
            dir = 1;
          attr = F_ATTR_DIR;
        }
      }
    }


    if (dir == 0)
      _f_clustertopos (vi, 0, &dirpos);

#if F_LONGFILENAME
    (void) _memcpy ((void *) (fsname.lname),
                    (void *) towchar (lost_name), 16);
    fsname.lname[8] = 0;
#else
    (void) _memcpy ((void *) (fsname.filename), (void *) lost_name, 8);
    (void) _memset (fsname.fileext, ' ', 3);
#endif

    /* Invalidate cache to make sure data is fresh */
    chkfat_inval_dir_cache (vi);

    /* Add a new directory entry named lost_name */
    if ((read_only == 0) &&
        (_f_addentry (vi, &fsname, &dirpos, &direntry) == 0))
    {
      uint32 curr_time;

      direntry->attr = attr;
      if ((attr & F_ATTR_DIR) == 0)
        _f_set32bitl (direntry->filesize,
                    csize * vi->bootrecord.sector_per_cluster *
                    F_SECTOR_SIZE);

      _f_setdecluster (vi, direntry, first_cluster);

      curr_time = fs_time();
      _f_set16bitl (direntry->mdate, fs_posix_to_fat_date (curr_time));
      _f_set16bitl (direntry->mtime, fs_posix_to_fat_time (curr_time));

      /* !ERROR! Previous dir sector getting cached */
      if (chkfat_writedirsector (vi))
        return FC_WRITE_ERROR;
    }


    if (attr & F_ATTR_DIR)
    {
      chkfat_putlog (FC_LOG_LOSTDIR_SAVE, lost_name);
    }
    else
    {
      chkfat_putlog (FC_LOG_LOSTFILE_SAVE, lost_name);
    }
  }
  err_mask |= CHKFAT_LOST_CHAIN_FOUND;
  return FC_NO_ERROR;
}



/*
**  chkfat_buildlostchains
**
**  Try to save lost chains.
**  Currently saves all lost chains, if there is a chain with bad end
**  (no last cluster, cross chain, ...), it puts the last cluster
**  mark to the proper position.
**
**  Input:
**     fm - multitask information
**     vi - volume information
**     ctbl - check table
**     ctbl_size - size of check table
**     param - parameters
**  Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_NO_ERROR - success
*/
int
chkfat_buildlostchains (F_MULTI * fm, F_VOLUME * vi, unsigned char *ctbl,
                        int ctbl_size, int param)
{
  /* Try to recover lost chains */
  int ctbl_pos;
  unsigned char ctbl_tmp;

  unsigned long first_cluster, act_cluster, next_cluster;
  unsigned long max_cluster, min_cluster;
  unsigned long csize, i;

  int rc;


  min_cluster = 2;
  if (vi->mediatype == F_FAT32_MEDIA)
    min_cluster++;
  max_cluster = _f_getmaxcluster (vi);

  csize = 0;
  for (ctbl_pos = 0; ctbl_pos < ctbl_size; ctbl_pos++)
  {
    while (ctbl[ctbl_pos] != 0xff)
    {
      /* Lost cluster */
      ctbl_tmp = ctbl[ctbl_pos];
      first_cluster = (ctbl_pos << 3);

      while (ctbl_tmp & 1)
      {
        /* contains the first lost cluster in the table */
        ++first_cluster;
        ctbl_tmp >>= 1;
      }

      /* Go and find the whole chain */
      csize = 0;
      act_cluster = first_cluster;

      for (;;)
      {
        BOUNDS_CHECK (act_cluster);
        SET_CLUSTER_BIT (ctbl, act_cluster);
        ++csize;
        if (chkfat_getclustervalue (vi, act_cluster, &next_cluster))
          return FC_CLUSTER_ERROR;
        if (next_cluster < min_cluster || next_cluster >= max_cluster
            || GET_CLUSTER_BIT (ctbl, next_cluster))
        {
          if (next_cluster != F_CLUSTER_LAST)
          {
            if (chkfat_setclustervalue (vi, act_cluster, F_CLUSTER_LAST))
              return FC_CLUSTER_ERROR;
          }
          break;
        }
        act_cluster = next_cluster;
      }

      act_cluster = first_cluster + 1;
      for (i = 0; i < max_cluster - min_cluster; i++)
      {
        if (act_cluster >= max_cluster)
          act_cluster = min_cluster;
        BOUNDS_CHECK (act_cluster);
        if (GET_CLUSTER_BIT (ctbl, act_cluster) == 0)
        {
          if (chkfat_getclustervalue (vi, act_cluster, &next_cluster))
            return FC_CLUSTER_ERROR;
          if (next_cluster == first_cluster)
          {
            SET_CLUSTER_BIT (ctbl, act_cluster);
            first_cluster = act_cluster;
            ++csize;
            i = 0;
          }
        }
        ++act_cluster;
      }

      err_mask |= CHKFAT_LOST_CHAIN_FOUND;

      /* Recover supersedes FIX when specified */
      if (RECOVER_MODE (param))
      {
        rc = chkfat_savelostchain (fm, vi, ctbl, first_cluster, csize, param);
        if (rc)
          return rc;
      }
      else if (FIX_MODE (param))
      {
        chkfat_putlog (FC_LOG_LOST_REMOVE, "");
        rc = chkfat_removechain (vi, ctbl, first_cluster, csize, 1);
        if (rc)
          return rc;
      }
    }
  }

  rc = chkfat_findlostdir (fm, vi, 0, 1, 0);
  if (rc == FC_NO_ERROR || rc == FC_ERROR)
  {
    rc = chkfat_lost_changebackdir (vi);
  }

  return rc;
}



/*
**  chkfat_cmpfat
**
**  Compare FAT1 and FAT2 and if there is any difference make FAT2
**  equal to FAT1
**
**  Input:
**     vi - volume information
**     ctbl - pointer to check table (used as intermediate buffer to load FATs)
**     ctbl_size - size of ctbl
**     param - operating parameters
**
**  Output:
**     FC_NO_ERROR - success
**     FC_READ_ERROR - invalid sector in FAT1 and FAT2
*/
static int
chkfat_cmpfat (F_VOLUME * vi, unsigned char *ctbl, unsigned long ctbl_size,
               int param)
{
  unsigned long i;
  unsigned long check_size;
  unsigned long fat_size;
  unsigned long act_size;
  int rc1, rc2, rc;
  unsigned char *fat1, *fat2;
  static unsigned char _fat2[F_SECTOR_SIZE];

  rc = FC_NO_ERROR;
  fat_size = vi->firstfat.num;
  check_size = (ctbl_size / F_SECTOR_SIZE) / 2;
  if (check_size == 0)
  {
    fat1 = (unsigned char *) de;
    fat2 = _fat2;
    check_size = 1;
  }
  else
  {
    if (check_size > 256)
      check_size = 256;
    fat1 = ctbl;
    fat2 = ctbl + check_size * F_SECTOR_SIZE;
  }

  for (i = 0; i < fat_size;)
  {
    if (check_size > (fat_size - i))
      act_size = fat_size - i;
    else
      act_size = check_size;

    rc1 = _f_readsector (vi, fat1, vi->firstfat.sector + i, act_size);
    rc2 =
      _f_readsector (vi, fat2, vi->firstfat.sector + fat_size + i, act_size);

    if (rc1 && rc2)
    {
      rc = FC_READ_ERROR;
      break;
    }

    if (rc1 == 0 && rc2 == 0)
    {
      if (memcmp (fat1, fat2, act_size * F_SECTOR_SIZE))
      {
        err_mask |= CHKFAT_FATS_INCONSISTENT;
        if (FIX_MODE (param))
        {
          (void) chkfat_writesector (vi, fat1, vi->firstfat.sector +
                                     fat_size + i, act_size);
          chkfat_putlog (FC_LOG_FAT2_UPDATED, NULL);
        }
      }
    }

    i += act_size;
  }

  return rc;
}



/*
** fn_checkfat
**
** Main routine for checkfat.
**
** Input:
**    fm - multitask param
**    drivenum - number of the drive
**    param - parameter (CHKFAT_READONLY |CHKFAT_FIX_ERRORS |
**                       CHKFAT_RECOVER_FILES)
**
** Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_ALLOCATION_ERROR - not enough memory
**     FC_NO_ERROR - success
**
*/

static int
fn_checkfat (F_MULTI * fm, int drivenum, int param, void (*callback) (void *),
             void *cparam)
{
  F_VOLUME *vi;
  F_POS first_pos;
  int rc;

  unsigned long ctbl_size;
  unsigned long maxcluster;
  unsigned long i;
  static unsigned char ctbl[CTBL_SIZE];

  /* Set/clear global read_only flag */
  read_only = (param & CHKFAT_READONLY) ? 1 : 0;
  err_mask = 0;

  rc = _f_getvolume (fm, drivenum, &vi);
  if (rc)
    return rc;

  rc = fn_chdrive (fm, drivenum);
  if (rc)
    return rc;


  (void) _memset (full_path, 0, sizeof(full_path));

  maxcluster = _f_getmaxcluster (vi);
  ctbl_size = (maxcluster >> 3) + 1;

  /* Compare and fix FAT1/FAT2 */
  rc = chkfat_cmpfat (vi, ctbl, ctbl_size, param);

  if (rc == FC_NO_ERROR)
  {
    (void) _memset ((void *) ctbl, 0, sizeof(ctbl));
    if (vi->mediatype == F_FAT32_MEDIA)
      /* mark cluster 0,1,2 as used */
      ctbl[0] = 0x07;
    else
      /* mark cluster 0,1 as used */
      ctbl[0] = 0x03;
    _f_clustertopos (vi, 0, &first_pos);

    rc = chkfat_files (fm, vi, ctbl, &first_pos, NULL, 0, param, 0);

    if (rc == FC_NO_ERROR)
    {

      /* Go through FAT table and mark lost clusters */
      for (i = 0; i < maxcluster; i++)
      {
        unsigned long val;
        BOUNDS_CHECK (i);

        /* Let calling process run - kick dog */
        if (i % 200 == 0 && callback)
          callback (cparam);

        if (chkfat_getclustervalue (vi, i, &val))
        {
          rc = FC_CLUSTER_ERROR;
          break;
        }

        if (val && val != F_CLUSTER_BAD && val != F_CLUSTER_RESERVED)
        {
          if (GET_CLUSTER_BIT (ctbl, i))
            CLEAR_CLUSTER_BIT (ctbl, i);
          else
            SET_CLUSTER_BIT (ctbl, i);
        }
      }

      if (vi->mediatype == F_FAT32_MEDIA)
        /* mark cluster 0,1,2 as used */
        ctbl[0] &= ~0x07;
      else
        /* mark cluster 0,1 as used */
        ctbl[0] &= ~0x03;

      if (i == maxcluster)
      {
        /* invert bits */
        for (i = 0; i < ctbl_size; ctbl[i++] ^= 0xff);
        lost_pos = 0;
        dirlevel = 0;


        rc = chkfat_buildlostchains (fm, vi, ctbl, ctbl_size, param);

#if F_LONGFILENAME
        if (rc == FC_NO_ERROR)
        {
          remove_cnt = 0;
          _f_clustertopos (vi, 0, &first_pos);
          rc = chkfat_lfn (vi, &first_pos, 0, param);

          if (remove_cnt && FIX_MODE (param))
          {
            chkfat_putlog (FC_LOG_LFN_REMOVE, "Removed LFN entry");
          }
        }
#endif
      }
    }
  }


#ifdef FATCACHE_ENABLE
  (void) chkfat_fatcache_flush (vi, 0);
#endif

  return rc;
}

/*
** f_checkfat
**
** Main routine for checkfat.
**
** Input:
**    drivenum - number of the drive
**    param - parameter (CHKFAT_READONLY |CHKFAT_FIX_ERRORS |
**                       CHKFAT_RECOVER_FILES)
**
** Output:
**     FC_READ_ERROR - sector read error
**     FC_WRITE_ERROR - sector write error
**     FC_CLUSTER_ERROR - error reading next cluster
**     FC_ALLOCATION_ERROR - not enough memory
**     FC_NO_ERROR - success
**
*/
int
fm_checkfat (int drivenum, int param, int *error_mask,
             void (*callback) (void *), void *cparam)
{
  int ret;
#if (!FN_CAPI_USED)
  F_MULTI *fm;

  if (fnSetTask (&fm))
    return F_ERR_BUSY;          /* busy! */

  ret = fn_checkfat (fm, drivenum, param, callback, cparam);
  _f_mutex_put (fm);
#else
  FW_MULTI *fm;
  if (_fw_settask (&fm))
    return F_ERR_BUSY;          /* busy! */
  ret = fn_checkfat (fm, drivenum, param, callback, cparam);
  _fw_mutex_put (fm);
#endif

  if (error_mask)
  {
    *error_mask = err_mask;
  }
  return ret;
}

#else /* !FEATURE_HFAT_CHKFAT */
int
fm_checkfat (int drivenum, int param, int *error_mask,
             void (*callback) (void *), void *cparam)
{
  (void) drivenum;
  (void) param;
  (void) error_mask;
  (void) callback;
  (void) cparam;

  /* Checkfat not implemented */
  return -1;
}
#endif
