/**
@file mm_iter.cpp***********************

@brief module manager acdb file reader

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_acdb_file_reader.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#ifndef _MM_ACDB_FILE_READER_H_
#define _MM_ACDB_FILE_READER_H_
#include "mm_main.h"

typedef struct acdb_file_reader_t acdb_file_reader_t;

/**
 * return ADSP_ENOTIMPL when meta-info-file is not present.
 */
ADSPResult acdb_file_reader_create(acdb_file_reader_t **acdb_reader_ptr_ptr);
ADSPResult acdb_file_reader_set_active_section(acdb_file_reader_t *rd_ptr, uint32_t section_id);
ADSPResult acdb_file_reader_release_active_section(acdb_file_reader_t *xrd_ptr, uint32_t section_id);
ADSPResult acdb_file_reader_read(void *buf_ptr, uint32_t el_size, uint32_t count, void *obj);
bool_t acdb_file_reader_eos (void *obj);
uint32_t acdb_file_reader_tell(void *obj);
ADSPResult acdb_file_reader_set_max_rd_pos(void *obj, uint32_t max_rd_pos_from_cur_location);
ADSPResult acdb_file_reader_seek(void *obj, uint32_t offset, uint32_t origin);
ADSPResult acdb_file_reader_destroy(acdb_file_reader_t **acdb_reader_ptr_ptr);


#endif //_MM_ACDB_FILE_READER_H_
