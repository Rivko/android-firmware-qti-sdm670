/**
@file mm_iter.cpp***********************

@brief module manager iterator

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_mem_blob_reader.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#ifndef MM_MEM_BLOB_READER_H_
#define MM_MEM_BLOB_READER_H_
#include "mm_main.h"


typedef struct mem_blob_reader_t mem_blob_reader_t;

/**
 * return ADSP_ENORESOURCE when meta-info-file is not present.
 */
ADSPResult mem_blob_reader_create(mem_blob_reader_t **mem_blob_reader_ptr_ptr, void *addr, uint32_t size);
ADSPResult mem_blob_reader_read(void *buf_ptr, uint32_t el_size, uint32_t count, void *obj);
bool_t mem_blob_reader_eos (void *obj);
uint32_t mem_blob_reader_tell(void *obj);
ADSPResult mem_blob_reader_set_max_rd_pos(void *obj, uint32_t max_rd_pos_from_cur_location);
ADSPResult mem_blob_reader_seek(void *obj, uint32_t offset, uint32_t origin);
ADSPResult mem_blob_reader_destroy(mem_blob_reader_t **mem_blob_reader_ptr_ptr);

uint32_t *mem_blob_reader_get_pointer_to_current_loc(void *obj);


#endif //MM_MEM_BLOB_READER_H_
