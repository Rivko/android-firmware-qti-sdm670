/*============================================================================
@file CoreFileLimited.c

This file provides an abstraction layer for file operations on the top of
efs_get()/efs_put() API. This file should only be used if no full
set of EFS API is available

Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreFileLimited.c#1 $
============================================================================*/

/* switch for using real EFS APIs v.s. unit test */
#define COREFILE_USE_EFS

/* switch for using ulog for debugging */
//#define COREFILE_USE_ULOG

#include "CoreFile.h"
#include "CoreString.h"
#include "CoreHeap.h"
#include "CoreString.h"
#include "CoreMutex.h"
#include "CoreMacros.h"
#include "CoreAtomicOps.h"
#include "CoreVerify.h"
#include <string.h>

#include "fs_public.h"
#include "fs_errno.h"

#ifdef COREFILE_USE_ULOG
#include "UlogFront.h"
#include "DALSys.h"
#endif

#define FILE_PATH_MAX_LEN           128
#define CORE_FILE_ULOG_BUF_SIZE     4096

#ifdef COREFILE_USE_EFS
#define CoreFile_LoadFileHelper efs_get
#else
#define CoreFile_LoadFileHelper CoreFile_UnitTestLoad
#endif
/*
 * sizes of buffers for holding the local copy of the files
 */
static const int fileBufferSizes[] = {1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192};

/*
 * Core File structure type
 */
struct CoreFileType {
  CoreMutex           *lock;                      /* lock for data integrity */
  char                path[FILE_PATH_MAX_LEN];    /* file path */
  unsigned int        fileSize;                   /* file size */
  char                *dataBuf;                   /* buffer for file data */
  int                 curPos;                     /* current access position of the file */
  struct CoreFileType *prev;                      /* previous file in the file list */
  struct CoreFileType *next;                      /* next file in the file list */
};

/*
 * Header of the list to hold all opened files
 */
static struct CoreFileType coreFileList = {
  NULL,
  "CoreFile List Head",
  0,
  0,
  0,
  &coreFileList,
  &coreFileList
};

#ifdef COREFILE_USE_ULOG
/*
 * ulog handle for CoreFileLimited
 */
static ULogHandle coreFileULog = NULL;

/*
 * buffer for ulog
 */
#define COREFILE_ULOG_BUF_SIZE  256
static char coreFileUlogBuf[COREFILE_ULOG_BUF_SIZE];

/*
 * lock for ulog buffer
 */
static CoreMutex *coreFileUlogBufLock = NULL;

/*
 * set up ulog for core file limited
 */
static void CoreFile_UlogSetup()
{
    ULogResult ulog_result;
  
   /* create the ulog handle */
   ulog_result = ULogFront_RealTimeInit( &coreFileULog,
                                          "CoreFile",
                                          CORE_FILE_ULOG_BUF_SIZE,
                                          ULOG_MEMORY_LOCAL,
                                          ULOG_LOCK_OS );

    CORE_VERIFY( ulog_result == DAL_SUCCESS );
    CORE_VERIFY_PTR( coreFileULog );
	
    /* initialize the ulog buffer */
    memset(coreFileUlogBuf, 0, COREFILE_ULOG_BUF_SIZE);
	
	/* initialize the ulog buffer lock */
    coreFileUlogBufLock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT );
    CORE_VERIFY_PTR( coreFileUlogBufLock );
}

#define CoreFile_Log(format, args) \
    do { \
        Core_MutexLock( coreFileUlogBufLock ); \
		snprintf( coreFileUlogBuf, COREFILE_ULOG_BUF_SIZE - 1, format, args); \
        ULogFront_RealTimeString( coreFileULog, coreFileUlogBuf ); \
        Core_MutexUnlock( coreFileUlogBufLock ); \
    } while(0)

#else /* COREFILE_USE_ULOG */

static void CoreFile_UlogSetup()
{
}

#define CoreFile_Log(format, args) do {} while (0)

#endif /* !COREFILE_USE_ULOG */

/*
 * Initialize the file list
 */
static int CoreFile_ListInit(void)
{
    coreFileList.lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT );
    CORE_VERIFY_PTR( coreFileList.lock );

    return 0;
}

/*
 * Check if a file is on the file list
 *
 * Note: The list's lock is not aquired by this function. The invoker
 *       is required to do so before calling this function
 */
static unsigned char CoreFile_IsFileOnList(struct CoreFileType *file)
{
    struct CoreFileType *cur;

    for ( cur = coreFileList.next; cur != &coreFileList; cur = cur->next) {
        if ( cur == file )
            return TRUE;
    }
    return FALSE;
}

/*
 * Insert a file into the file list
 */
static void CoreFile_ListInsert(struct CoreFileType *file)
{
    CORE_VERIFY_PTR( file );
    CORE_VERIFY_PTR( coreFileList.lock );

    Core_MutexLock( coreFileList.lock );

    CORE_VERIFY( CoreFile_IsFileOnList( file ) == FALSE );
    file->next = coreFileList.next;
    file->prev = &coreFileList;
    coreFileList.next->prev = file;
    coreFileList.next = file;

    Core_MutexUnlock( coreFileList.lock );	
}

/*
 * Delete a file from the file list
 */
static void CoreFile_ListDelete(struct CoreFileType *file)
{
    CORE_VERIFY_PTR( file );
    CORE_VERIFY_PTR( coreFileList.lock );

    Core_MutexLock(coreFileList.lock);

    CORE_VERIFY( CoreFile_IsFileOnList( file ) == TRUE );
    file->prev->next = file->next;
    file->next->prev = file->prev;
    file->prev = NULL;
    file->next = NULL;

    Core_MutexUnlock( coreFileList.lock );
}

/*
 * Init internal data structure of CoreFile
 */
static void CoreFile_Init( void )
{
    static volatile uint32_t initDone = 0;
    unsigned val;

    val = Core_AtomicInc(&initDone);

    if (val == 1) {
        CoreFile_ListInit();
        CoreFile_UlogSetup();
    }
    initDone = 1;
}

#ifndef COREFILE_USE_EFS
/*
 * This function is for CoreFile Unit test. It loads file cache with local
 * data instead of efs_get().
 */
static int CoreFile_UnitTestLoad(const char *filename, void *buf, int bufSize)
{
  const char fileData[] = "[MPSS_Clock]\nFlags=19\nEnableDCS=1\nMinFreqHz=115200000\nMaxFreqHz=576000000\n[MPSS_VDDCX]EnableDVS=1\nMinCorner=3\nMaxCorner=6\n[MPSS_VDDMSS]\nEnableDVS=1\n";
  
  if (!filename || !buf || bufSize <= 0)
    return -1;

  if (bufSize < sizeof(fileData)) {
    efs_errno = FS_ERANGE;
    return -1;
  }

  memcpy(buf, fileData, sizeof(fileData));
  return sizeof(fileData);
}
#endif

/*
 * Allocate a buffer and load file content
 */
static unsigned char CoreFile_LoadFile( const char *filename, int flags, int *fileHandle )
{
    const int buffer_size_num = sizeof(fileBufferSizes) / sizeof(fileBufferSizes[0]);
    struct CoreFileType *file = NULL;
    int ret;
    int i;
    
    /* allocate file instance */
    file = Core_Malloc( sizeof(struct CoreFileType) );
    CORE_VERIFY_PTR( file );
    memset( file, 0, sizeof(*file) );
	
    /* save file name */
    core_strlcpy( file->path, filename, FILE_PATH_MAX_LEN - 1 );

    /* create file access mutex */
    file->lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT );
    CORE_VERIFY_PTR( file->lock );

    /*
     * allocate local buffer and load file content. different buffer sizes tried
     * until file fits the buffer
     */
    for (i = 0; i < buffer_size_num; i++) {

        /* allocate data buffer */
        file->dataBuf = Core_Malloc(fileBufferSizes[i]);
        CORE_VERIFY_PTR( file->dataBuf );

        /* load file content */
        ret = CoreFile_LoadFileHelper(filename, file->dataBuf, fileBufferSizes[i]);

        /* check loading result */
        if (ret == 0) {
            CoreFile_Log( "file %s is empty", filename );
            break;    /* empty file */
        } else if (ret < 0) {
            /* given buffer not big enough, try a bigger one */
            if (efs_errno == FS_ERANGE) {
                Core_Free(file->dataBuf);
                file->dataBuf = NULL;
                continue;
            } else {
                CoreFile_Log( "efs_get() set efs_errno to %d", efs_errno );
                break;
            }
        } else {
            /* file read succeeds */
            file->fileSize = ret;
            CoreFile_ListInsert(file);
            CoreFile_Log( "CoreFile_LoadFile() succeeds. File handle %x", (int)file );
            *fileHandle = (int)file;
            return TRUE;
        }
    }

    /* if this line is reached, file loading failed. */

    /* file does not fit in the maximum allowed buffer size */
    if (i >= buffer_size_num) 
        CoreFile_Log( "file %s is too big", filename);


    if (file->dataBuf)
        Core_Free( file->dataBuf );
    Core_MutexDestroy( file->lock );
    Core_Free( file );
    CoreFile_Log( "%s failed", __FUNCTION__ );
    return FALSE;
}

/*
 * CoreFile_Open
 */
unsigned char CoreFile_Open( const char *filename, int flags, int *fileHandle )
{
  CoreFile_Init();
  CoreFile_Log( "CoreFile_Open() called on %s",
                    filename ? filename : "NULL" );
  
  CORE_VERIFY_PTR( filename );
  CORE_VERIFY_PTR( fileHandle );
  CORE_VERIFY( flags == CORE_FILE_READ_ONLY ); /* support read operation only */
  
  *fileHandle = 0;
  
  return CoreFile_LoadFile(filename, flags, fileHandle);
}

/*
 * CoreFile_Close
 */
int CoreFile_Close(int fileHandle)
{
  struct CoreFileType *file = (struct CoreFileType *)fileHandle;

  CoreFile_Init();
  CoreFile_Log( "CoreFile_Close(%x) called", fileHandle );

  CORE_VERIFY_PTR( file );

  CoreFile_ListDelete( file );
  Core_MutexLock( file->lock );
  Core_Free( file->dataBuf );
  Core_MutexUnlock( file->lock );
  Core_MutexDestroy( file->lock );
  Core_Free( file );
 
  return 0;
}

/*
 * CoreFile_GetNextLine
 */ 
unsigned char CoreFile_GetNextLine( int fileHandle, char *output, 
                                    unsigned int output_size )
{
  char *end_of_line = NULL;
  int bytes_read = 0;
  struct CoreFileType *file = (struct CoreFileType *)fileHandle;

  CoreFile_Init();
  
  CORE_VERIFY_PTR( file );
  CORE_VERIFY_PTR( output );
  CORE_VERIFY( output_size > 0 );

  Core_MutexLock(coreFileList.lock);
  CORE_VERIFY( CoreFile_IsFileOnList( file ) == TRUE );

  /* get access to the file */
  Core_MutexLock( file->lock );

  /* if EOF reached, we can't return next line */
  if (file->curPos >= file->fileSize) {
    Core_MutexUnlock( file->lock );    
    Core_MutexUnlock(coreFileList.lock);
    return FALSE;
  }

  /* Clear out the string, to ensure NULL termination. */
  memset( output, 0x0, output_size );

  /* Read the amount of data requested, if available. */
  bytes_read = MIN( output_size, file->fileSize - file->curPos );
  memcpy( output, &(file->dataBuf[file->curPos]), bytes_read );

  /* 
   * Note: The following code (thru the end of the function) is legacy code
   *       and subject to future optimization 
   */

  /* make sure the string is NULL-terminated before it is passed to strchr function */
  output[output_size - 1] = 0;
  
  /* Find the end of the line. */
  end_of_line = strchr( output, '\n' );

  if ( end_of_line && ( end_of_line != &output[output_size - 1] ) )
  {
    /* NULL-terminate after the new-line character. */
    *( end_of_line + 1 ) = 0;
  }
  
  /* Set the seek position to start at the end of the line just read.
   * The seek position moved forward when we did the efs_read(), so we
   * need to back it up to the end of the line we just read. */
  file->curPos += strlen( output );

  /* done with the file access, release the lock */
  Core_MutexUnlock( file->lock );
  Core_MutexUnlock(coreFileList.lock);
  
  /* Remove any trailing LF & CR */
  if ( strlen( output ) > 0 )
  {
    if ( output[strlen( output ) - 1] == 0x0A )
    {
      output[strlen( output ) - 1] = 0;
    }
    if ( output[strlen( output ) - 1] == 0x0D )
    {
      output[strlen( output ) - 1] = 0;
    }
  }

  CoreFile_Log( "CoreFile_GetNextLine: %s", output );

  return TRUE;
}

/*
 * CoreFile_Rewind
 */
unsigned char CoreFile_Rewind( int fileHandle )
{
  struct CoreFileType *file = (struct CoreFileType *)fileHandle;

  CoreFile_Init();
  CORE_VERIFY_PTR( file );

  Core_MutexLock(coreFileList.lock);
  CORE_VERIFY( CoreFile_IsFileOnList( file ) == TRUE );

  /* reset file's current position */
  Core_MutexLock( file->lock );	
  file->curPos = 0;
  Core_MutexUnlock( file->lock );	  

  Core_MutexUnlock(coreFileList.lock);

  return TRUE;
}
