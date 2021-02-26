#ifndef _GP_PERSIST_OBJ_FILEIO_H_
#define _GP_PERSIST_OBJ_FILEIO_H_

/**
@file gpPersistObjFileIO.h
@brief Temporary stubs to FTS File IO APIs
*/

/*=============================================================================
Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include <stddef.h>
#include <stdint.h>
#include "qsee_fts_fileio.h"
#include "tee_internal_api.h"

#define UNUSED __attribute__((unused))

static inline TEE_Result
gpFileIO_Read_File(UNUSED void* handle , const char* name,
                   void* buffer, size_t size, size_t offset, uint32_t* count) {
  return qsee_fts_read_file(name, buffer, size, offset, count);
}

static inline TEE_Result
gpFileIO_Write_File(UNUSED void* handle, const char* name,
                    void* buffer, size_t size, size_t offset, uint32_t* count) {
  return qsee_fts_write_file(name, buffer, size, offset, count);
}

static inline TEE_Result
gpFileIO_Remove_File(UNUSED void* handle, const char* name) {
  return qsee_fts_remove_file(name);
}

static inline TEE_Result
gpFileIO_Rename_File(UNUSED void* handle, const char* from, const char* to) {
  return qsee_fts_rename_file(from, to);
}

#endif //_GP_PERSIST_OBJ_FILEIO_H_
