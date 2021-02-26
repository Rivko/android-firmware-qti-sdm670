/*============================================================================
@file CoreFile.c

This file provides an abstraction layer for file operations.

Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreFile.c#1 $
============================================================================*/
#include "CoreFile.h"

#ifndef WINSIM

#include "fs_public.h"
#include <string.h>

/*
 * CoreFile_Open
 */
unsigned char CoreFile_Open( const char *filename, int flags, int *fileHandle )
{
  switch ( flags )
  {
    case CORE_FILE_READ_ONLY:
    {
      *fileHandle = efs_open( filename, O_RDONLY );

      if ( *fileHandle < 0 )
      {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/*
 * CoreFile_Close
 */
int CoreFile_Close(int fileHandle)
{
    return efs_close(fileHandle);
}

/*
 * CoreFile_GetNextLine
 */
/* EFS does not provide a function to get the next line.  This one will
 * read the requested number of bytes, and truncate to the end of the
 * line, and also move the EFS seek position to be the end of the line
 * just read. */ 
unsigned char CoreFile_GetNextLine( int fileHandle, char *output, 
                                    unsigned int output_size )
{
  char *end_of_line = NULL;
  int bytes_read = 0;

  /* Clear out the string, to ensure NULL termination. */
  memset( output, 0x0, output_size );

  /* Read the amount of data requested. */
  bytes_read = efs_read( fileHandle, output, output_size );

  if ( bytes_read <= 0 )
  {
    /* If bytes_read is -1 or 0, then either an error occurred, or
     * the end of the file was reached. */
    return FALSE;
  }

  // make sure the string is NULL-terminated before it is passed to strchr function 
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
  efs_lseek( fileHandle, strlen( output ) - bytes_read, SEEK_CUR );

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

  return TRUE;
}

/*
 * CoreFile_Rewind
 */
unsigned char CoreFile_Rewind( int fileHandle )
{
  if ( efs_lseek( fileHandle, 0L, SEEK_SET ) == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#else /* WINSIM */

#include "stdio.h"

/*
 * CoreFile_Open
 */
unsigned char CoreFile_Open( const char *filename, int flags, int *fileHandle )
{
  switch ( flags )
  {
    case CORE_FILE_READ_ONLY:
    {
      *fileHandle = ( int )( fopen( filename, "r" ) );

      if ( *fileHandle == NULL )
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*
 * CoreFile_Close
 */
int CoreFile_Close(int fileHandle)
{
    return fclose( fileHandle );
}

/*
 * CoreFile_GetNextLine
 */
unsigned char CoreFile_GetNextLine( int fileHandle, char *output, 
                                    unsigned int output_size )
{
  if ( fgets( output, output_size, (FILE *)fileHandle ) == NULL)
  {
    return FALSE;
  }

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

  return TRUE;
}

/*
 * CoreFile_Rewind
 */
unsigned char CoreFile_Rewind( int fileHandle )
{
  if ( fseek ( (FILE *)fileHandle, 0L ,SEEK_SET ) == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#endif /* WINSIM */
