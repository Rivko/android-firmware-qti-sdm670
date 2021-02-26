/*==============================================================================
@file CoreFile.h

This file provides an abstraction layer for file operations.

Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreFile.h#1 $
==============================================================================*/
#ifndef CORE_FILE_H
#define CORE_FILE_H

/* ==================================================================
                 GLOBAL VARIABLES AND DEFINITIONS
   ================================================================== */              

/**
 * @brief File permissions supported by this subsystem.  These 
 *        can be OR'd together into a bitmask.
 */
#define CORE_FILE_READ_ONLY   1


/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/**
 * CoreFile_Open
 *
 * @brief Opens the file. 
 *  
 * This function will use the file name and flags passed in, and 
 * open the file with the desired permissions.  A file handle is 
 * returned, to be used on all subsequent file operations. 
 *
 * @param filename:   Name of the file to open.
 * @param flags:      Permissions to use when opening the file.
 * @param fileHandle: Output buffer to write the file handle.
 * 
 * @return FALSE if an error occurred when opening the file.
 *         TRUE if the file was opened successfully.
 */ 
unsigned char CoreFile_Open( const char *filename, int flags, int *fileHandle );

/**
 * CoreFile_Close
 *
 * @brief Closes the file.
 *
 * This function will close the file corresponding to the handle 
 * passed in. 
 *
 * @param Handle to the file to close. 
 */ 
int CoreFile_Close( int fileHandle );

/**
 * CoreFile_GetNextLine
 *
 * @brief Gets the next line in the file.
 *
 * @param fileHandle:  Handle the file to get the next line in.
 * @param output:      Buffer to write the string for the next line.
 * @param output_size: Size of the buffer to write the string.
 * 
 * @return FALSE if an error or end of file is reached.
 *         TRUE if bytes were read successfully.
 */ 
unsigned char CoreFile_GetNextLine( int fileHandle, char *output, 
                                    unsigned int output_size );

/**
 * CoreFile_Rewind
 *
 * @brief Rewinds file position indicator to the beginning of
 *        the file..
 *
 * @param fileHandle:  Handle the file to rewind.
 * 
 * @return FALSE if operation is unsuccessful. 
 *         TRUE if operation is successful.
 */ 
unsigned char CoreFile_Rewind( int fileHandle );

#endif /* !CORE_FILE_H */
