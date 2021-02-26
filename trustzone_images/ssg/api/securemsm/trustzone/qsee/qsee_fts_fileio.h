#ifndef _QSEE_FTS_FILEIO_H_
#define _QSEE_FTS_FILEIO_H_

/**
@file   qsee_fts_fileio.h
@brief  Definitions for the FTS File IO module which interfaces with storing
        file data in HLOS vfs.
*/

/*=============================================================================
Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================
===
=== The qsee_fts_fileio module provides an interface for persistent storage
=== of transparent data. This transparent data is stored in the HLOS virtual
=== file system, so its the responsibility of the caller to provide the 
=== necessary security on top of the transparent data. Data is transferred to
=== an HLOS listener which interacts directly with the virtual file system,
=== and because there is overhead in the transfer of data between secure and 
=== non-secure context it is advised to reduce the amount of calls to these
=== APIs by increasing data sizes.
===
=== Version 1.0
===   As part of version 1.0, there are some restrictions on the usage of 
===   these APIs. 
===     1. Files should ONLY be written in one-shot, meaning the write call
===        should be made with a buf equal to the size of the file. This will
===        guarantee power cycle protection provided by the virtual file
===        system.
===     2. As a consequence of restriction 1. the offset provided to a write
===        should also be 0, and this is the only acceptable input for version 
===        1.0.
===
===========================================================================*/

#include <stddef.h>
#include <stdint.h>
#include "tee_internal_api.h"

/*===========================================================================
=== qsee_fts_read_file
===
=== Read data from the specified file.
===
=== Parameters:
===   - [in]name    File name which is used to identify the file in the virtual
===                 file system.
===   - [out]buffer Buffer in which the file data will be copied to. Caller is 
===                 responsible for memory management of buffer.
===   - [in]size    Size of buffer which was allocated from the caller.
===   - [in]offset  The offset in the file to start reading size bytes from.
===   - [out]count  Returns the number of bytes actually read.
===
=== Returns
===   - TEE_SUCCESS         On successful read operation.
===   - TEE_ERROR_GENERIC   On failure of read operation.
===
===========================================================================*/
TEE_Result qsee_fts_read_file
(
  const char* name,
  void*       buffer,
  size_t      size,
  size_t      offset,
  uint32_t*   count
);

/*===========================================================================
=== qsee_fts_write_file
===
=== Write data to the specified file.
===
=== Parameters:
===   - [in]name    File name which is used to identify the file in the virtual
===                 file system.
===   - [in]buffer  Buffer which contains the data to be written to the file.
===                 Caller is responsible for memory management of buffer.
===   - [in]size    Size of buffer which was allocated from the caller.
===   - [in]offset  The offset in the file to start writing size bytes to.
===   - [out]count  Returns the number of bytes actually written.
===
=== Returns
===   - TEE_SUCCESS         On successful write operation.
===   - TEE_ERROR_GENERIC   On failure of write operation.
===
===========================================================================*/
TEE_Result qsee_fts_write_file
(
  const char* name,
  void*       buffer,
  size_t      size,
  size_t      offset,
  uint32_t*   count
);

/*===========================================================================
=== qsee_fts_remove_file
===
=== Remove the file specified from the HLOS virtual file system.
===
=== Parameters:
===   - [in]name      File name which is used to identify the file in the virtual
===                   file system.
===
=== Returns
===   - TEE_SUCCESS         On successful remove operation.
===   - TEE_ERROR_GENERIC   On failure of remove operation.
===
===========================================================================*/
TEE_Result qsee_fts_remove_file
(
  const char* name
);

/*===========================================================================
=== qsee_fts_rename_file
===
=== Rename the file specified by 'from' to the name provided by 'to' 
===
=== Parameters:
===   - [in]from  File name which is used to identify the file in the virtual
===               file system.
===   - [in]to    New name of file specified by from.
===
=== Returns
===   - TEE_SUCCESS         On successful rename operation.
===   - TEE_ERROR_GENERIC   On failure of rename operation.
===
===========================================================================*/
TEE_Result qsee_fts_rename_file
(
  const char* from,
  const char* to
);

#endif //_QSEE_FTS_FILEIO_H_
