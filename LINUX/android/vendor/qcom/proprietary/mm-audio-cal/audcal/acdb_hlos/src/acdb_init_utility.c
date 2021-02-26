/*===========================================================================
    FILE:           acdb_init_utility.c

    OVERVIEW:       This file contains the acdb init utility functions
                    implemented specifically in the win32 environment.

    DEPENDENCIES:   None

                    Copyright (c) 2012, 2016 Qualcomm Technologies, Inc.
                    All Rights Reserved.
                    Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */

/*===========================================================================
    EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order. Please
    use ISO format for dates.

    $Header: //source/qcom/qct/multimedia2/Audio/audcal3/acdb_hlos/rel/2.5/src/acdb_init_utility.c#1 $

    when        who     what, where, why
    ----------  ---     -----------------------------------------------------
    2010-07-08  vmn     Initial revision.

========================================================================== */

/* ---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------- */

#include "acdb_init_utility.h"
#include "acdb_init.h"
#include "acdb_command.h"
//modified for QNX build

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define ACDB_DELTA_FILE_PATH "/data/vendor/misc/audio/acdbdata/delta/"
#define ACDB_DELTA_FILE_EXTN "delta"
#define DELTA_FILENAME_SIZE  256

#ifdef _ANDROID_
static int32_t persistenceStatus = ACDB_UTILITY_INIT_SUCCESS;
#else
static int32_t persistenceStatus = ACDB_UTILITY_INIT_FAILURE;
#endif


int32_t AcdbGetFileData (const char* pFilename, uint8_t **pBfr, uint32_t *pFileSize)
{
   int32_t result = ACDB_UTILITY_INIT_SUCCESS;
   FILE *fp = NULL;
   uint32_t nBytesRead = 0;

   if (pFilename == NULL || pFileSize == NULL || pBfr == NULL)
   {
      return ACDB_UTILITY_INIT_FAILURE;
   } else {
      fp = fopen(pFilename, "rb");
      if (fp == NULL)
      {
         return ACDB_UTILITY_INIT_FAILURE;
      }
      fseek (fp, 0, SEEK_END);
      *pFileSize = ftell (fp);
      fseek (fp, 0, SEEK_SET);

      *pBfr = (uint8_t *)malloc(*pFileSize);
      if(*pBfr != NULL)
      {
          nBytesRead = (uint32_t) fread (*pBfr,sizeof (uint8_t),(size_t)*pFileSize,fp);
      } else {
          result = ACDB_UTILITY_INIT_FAILURE;
      }
      if(nBytesRead != *pFileSize)
      {
          result = ACDB_UTILITY_INIT_FAILURE;
          if(*pBfr != NULL)
          {
              free(*pBfr);
          }
      }
      fclose (fp);
   }
   return result;
}

void  AcdbFreeFileData (void *pBfr)
{
   if(pBfr != NULL)
   {
       free(pBfr);
       pBfr = NULL;
   }
}

int32_t AcdbIsPersistenceSupported(void)
{
   return persistenceStatus;
}

uint32_t GetDeltaFileName(const char* pFilename, uint32_t fileNameLen, char* pDeltaFileName, uint32_t dlen)
{
   char *pDeltaFileExtn = ACDB_DELTA_FILE_EXTN;
   char *pDeltaFilePrefix = ACDB_DELTA_FILE_PATH;
   char *ptr;
   uint32_t deltaFileNameSize = 0;

   if(pFilename == NULL || pDeltaFileName == NULL) {
       return 0;
   }

   ptr = strrchr(pFilename, '/');
   if(NULL == ptr) {
       ptr = pFilename;
   } else {
       ptr++;
   }

   deltaFileNameSize = strlen(pDeltaFilePrefix)
                       + strlen(ptr) + strlen(pDeltaFileExtn) + 1;
   if(deltaFileNameSize > dlen) {
       ACDB_DEBUG_LOG("[%s] delta file name size %d is more than memory given %d",
                      __func__, deltaFileNameSize, dlen);
       return 0;
   }

   strlcpy(pDeltaFileName, pDeltaFilePrefix, strlen(pDeltaFilePrefix)+1);
   strlcat(pDeltaFileName, ptr, deltaFileNameSize);
   strlcat(pDeltaFileName, pDeltaFileExtn, deltaFileNameSize);

   return strlen(pDeltaFileName);
}

int32_t AcdbIsDeltaFileAvailable (const char* pFilename, uint32_t fileNameLen, uint32_t* pDeltaFileDataLen)
{
   FILE *fp = NULL;
   char deltaFileName[DELTA_FILENAME_SIZE];
   uint32_t len=0;

   *pDeltaFileDataLen = 0;
   if(pFilename == NULL || fileNameLen == 0)
   {
      return ACDB_UTILITY_INIT_FAILURE;
   }
   else
   {
      len = GetDeltaFileName(pFilename, fileNameLen, deltaFileName, DELTA_FILENAME_SIZE);
      if (len == 0)
      {
         ACDB_DEBUG_LOG("[%s] no delta file found for %s", __func__, pFilename);
         return ACDB_UTILITY_INIT_FAILURE;
      }

      fp = fopen( deltaFileName, "ab+");
      if (fp == NULL)
      {
         ACDB_DEBUG_LOG("[%s] failed to open delta file %s", __func__, deltaFileName);
         return ACDB_UTILITY_INIT_FAILURE;
      }

      fseek (fp, 0, SEEK_END);
      *pDeltaFileDataLen = ftell (fp);
      fclose (fp);
   }

   return ACDB_UTILITY_INIT_SUCCESS;
}

int32_t AcdbGetDeltaFileData (const char* pFilename, uint32_t fileNameLen, uint8_t *pFileBfr, uint32_t nFileBfrSize)
{
   int32_t result = ACDB_UTILITY_INIT_SUCCESS;
   FILE *fp = NULL;
   uint32_t nBytesRead = 0;
   uint32_t fileSize = 0;
   char deltaFileName[DELTA_FILENAME_SIZE];
   uint32_t len=0;

   if (pFilename == NULL)
   {
      return ACDB_UTILITY_INIT_FAILURE;
   }
   else
   {
      len = GetDeltaFileName(pFilename, fileNameLen, deltaFileName, DELTA_FILENAME_SIZE);
      if (len == 0)
      {
         ACDB_DEBUG_LOG("[%s] no delta file found for %s", __func__, pFilename);
         return ACDB_UTILITY_INIT_FAILURE;
      }

      fp = fopen (deltaFileName, "rb");
      if (fp == NULL)
      {
         result = ACDB_UTILITY_INIT_FAILURE;
         goto done;
      }

      fseek (fp, 0, SEEK_END);
      fileSize = ftell (fp);
      fseek (fp, 0, SEEK_SET);

      if(pFileBfr != NULL && fileSize == nFileBfrSize)
      {
         nBytesRead = (uint32_t) fread (pFileBfr,sizeof (uint8_t),(size_t)fileSize,fp);
      } else {
         result = ACDB_UTILITY_INIT_FAILURE;
      }

      if(nBytesRead != fileSize)
      {
         result = ACDB_UTILITY_INIT_FAILURE;
         if(pFileBfr != NULL)
         {
             free(pFileBfr);
         }
      }
      fclose (fp);
   }
done:
   return result;
}

int32_t AcdbWriteDeltaFileData (const char* pFilename, uint32_t fileNameLen, uint8_t *pFileBfr, uint32_t nFileBfrSize)
{
   int32_t result = ACDB_UTILITY_INIT_SUCCESS;
   FILE *fp = NULL;
   uint32_t nBytesWrite = 0;
   char deltaFileName[DELTA_FILENAME_SIZE];
   uint32_t len=0;

   len = GetDeltaFileName(pFilename, fileNameLen, deltaFileName, DELTA_FILENAME_SIZE);
   if (len == 0)
   {
      ACDB_DEBUG_LOG("[%s] no delta file found for %s", __func__, pFilename);
      return ACDB_UTILITY_INIT_FAILURE;
   }

   fp = fopen (deltaFileName, "wb");
   if (fp == NULL)
   {
      return ACDB_UTILITY_INIT_FAILURE;
   }

   if(pFileBfr != NULL && nFileBfrSize != 0)
   {
      nBytesWrite = fwrite(pFileBfr,sizeof(uint8_t),(size_t)nFileBfrSize, fp);
      if(nBytesWrite != nFileBfrSize)
      {
         result = ACDB_UTILITY_INIT_FAILURE;
      }
   }
   else
   {
      result = ACDB_UTILITY_INIT_FAILURE;
   }

   fclose (fp);

   return result;
}

int32_t AcdbDeleteDeltaFileData (const char* pFilename, uint32_t fileNameLen)
{
   int32_t result = ACDB_UTILITY_INIT_SUCCESS;
   int32_t delFileResult = 0;
   char deltaFileName[DELTA_FILENAME_SIZE];
   uint32_t len=0;

   len = GetDeltaFileName(pFilename, fileNameLen, deltaFileName, DELTA_FILENAME_SIZE);
   if (len == 0)
   {
      ACDB_DEBUG_LOG("[%s] no delta file found for %s", __func__, pFilename);
      return ACDB_UTILITY_INIT_FAILURE;
   }

   if(access(deltaFileName, 0) != -1)
   {
      delFileResult = remove(deltaFileName);
      if(delFileResult != 0)
      {
         result = ACDB_UTILITY_INIT_FAILURE;
      }
   }
   return result;
}
