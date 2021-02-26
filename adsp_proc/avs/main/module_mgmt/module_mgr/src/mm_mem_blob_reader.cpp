
/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_mem_blob_reader.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/



#include "mm_mem_blob_reader.h"

typedef struct imem_blob_reader_t imem_blob_reader_t;

struct imem_blob_reader_t
{
   uint8_t *addr;
   uint32_t size;

   uint32_t cur_pos; //0 to size-1

   //current max permissible read position
   //reads cannot happen beyond this position.
   //but can happen at this location
   //0=>dont use this.
   uint32_t cur_max_rd_pos;

};


ADSPResult mem_blob_reader_create(mem_blob_reader_t **mem_blob_reader_ptr_ptr, void *addr, uint32_t size)
{
   //ADSPResult rc=ADSP_EOK;
   *mem_blob_reader_ptr_ptr = NULL;
   imem_blob_reader_t *rd_ptr=(imem_blob_reader_t*)
         qurt_elite_memory_malloc(sizeof(imem_blob_reader_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == rd_ptr)
   {
      return ADSP_ENOMEMORY;
   }
   memset(rd_ptr, 0, sizeof(imem_blob_reader_t)); //initializes active section to UNKNOWN

   rd_ptr->addr = (uint8_t*)addr;
   rd_ptr->size = size;

   *mem_blob_reader_ptr_ptr = (mem_blob_reader_t*)rd_ptr;

   return ADSP_EOK;
}

ADSPResult mem_blob_reader_destroy(mem_blob_reader_t **mem_blob_reader_ptr_ptr)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)*mem_blob_reader_ptr_ptr;
   qurt_elite_memory_free(rd_ptr);
   *mem_blob_reader_ptr_ptr = NULL;

   return ADSP_EOK;
}

static inline bool_t is_below_max_permissible_read(imem_blob_reader_t *rd_ptr, uint32_t len_bytes)
{
   if (0!=rd_ptr->cur_max_rd_pos)
   {
      //cur_pos
      if (rd_ptr->cur_pos+len_bytes>rd_ptr->cur_max_rd_pos)
      {
#ifdef MEDIUM_LOG
         MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_below_max_permissible_read: item to be read (%lu+%lu) is not within current max read position %lu", rd_ptr->cur_pos,len_bytes, rd_ptr->cur_max_rd_pos);
#endif
         return FALSE;
      }
   }
   //when read func is called it's going to read from addr+cur_pos till addr+cur_pos+len_bytes.
   //this includes addr+cur_pos but not addr+cur_pos+len_bytes, hence '>' check is fine.
   if (rd_ptr->cur_pos+len_bytes>rd_ptr->size)
   {
#ifdef MEDIUM_LOG
      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_below_max_permissible_read: item to be read (%lu+%lu) is not within buffer size %lu", rd_ptr->cur_pos,len_bytes, rd_ptr->size);
#endif
      return FALSE;
   }
   return TRUE;
}


/**
 * reads only in the active section
 */
ADSPResult mem_blob_reader_read(void *buf_ptr, uint32_t el_size, uint32_t count, void *obj)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;

   if (!is_below_max_permissible_read(rd_ptr, el_size*count))
   {
#ifdef MEDIUM_LOG
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  mem_blob_reader_read: not below max rd position");
#endif
      return ADSP_EEOF;
   }

   memscpy(buf_ptr, count*el_size, rd_ptr->addr+rd_ptr->cur_pos, count*el_size);
   rd_ptr->cur_pos+=(count*el_size);

   return ADSP_EOK;
}

bool_t mem_blob_reader_eos (void *obj)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;
   if (is_below_max_permissible_read(rd_ptr, 1)) //if next byte is not in the active section,
   {
      //printf(" mem_blob_reader_eos: not EOS");
      return FALSE;
   }
   else
   {
#ifdef MEDIUM_LOG
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," mem_blob_reader_eos: EOS");
#endif
      return TRUE;
   }
}

/**
 * to be compatible with ftell which returns number of bytes from beginning of the stream.
 */
uint32_t mem_blob_reader_tell(void *obj)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;

   return rd_ptr->cur_pos;
}

//max_rd_pos_from_cur_location=>reset
ADSPResult mem_blob_reader_set_max_rd_pos(void *obj, uint32_t max_rd_pos_from_cur_location)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;


   if (max_rd_pos_from_cur_location)
   {
      if (rd_ptr->cur_pos+max_rd_pos_from_cur_location > rd_ptr->size)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  current position %lu + max_rd %lu > size %lu ", rd_ptr->cur_pos, max_rd_pos_from_cur_location, rd_ptr->size);
         return ADSP_EFAILED;
      }
      rd_ptr->cur_max_rd_pos = rd_ptr->cur_pos+max_rd_pos_from_cur_location;
   }
   else
   {
      rd_ptr->cur_max_rd_pos=0;
   }

#ifdef VERBOSE
   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  Current max read position is %lu (0 is special)", rd_ptr->cur_max_rd_pos);
#endif
   return ADSP_EOK;
}

uint32_t *mem_blob_reader_get_pointer_to_current_loc(void *obj)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;
   QURT_ELITE_ASSERT(0 == (uint32_t(rd_ptr->addr+rd_ptr->cur_pos) % 4));
   return reinterpret_cast<uint32_t*>(rd_ptr->addr+rd_ptr->cur_pos);
}

/**
 * seeks only in the active section.
 *
 * origin: SEEK_FROM_FILE_BEGIN & SEEK_FROM_CURRENT
 */
ADSPResult mem_blob_reader_seek(void *obj, uint32_t offset, uint32_t origin)
{
   imem_blob_reader_t *rd_ptr = (imem_blob_reader_t*)obj;

   //if (offset==0)
   //{
      //return 0; //nothing to seek.
   //}

   uint32_t len=offset;
   if (!is_below_max_permissible_read(rd_ptr, len))
   {
#ifdef MEDIUM_LOG
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  mem_blob_reader_seek: not in  max rd point");
#endif
      return ADSP_EEOF;
   }

   int rc=0;
   switch(origin)
   {
   case SEEK_FROM_CURRENT:
   {
      rd_ptr->cur_pos+=offset;
      break;
   }
   case SEEK_FROM_FILE_BEGIN:
   {
      rd_ptr->cur_pos=offset;
      break;
   }
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Invalid source %lu given for mem_blob_reader_seek",origin);
      return ADSP_EFAILED;
   }
   if (rc)
   {
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}


