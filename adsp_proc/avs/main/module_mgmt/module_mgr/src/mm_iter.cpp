/**
@file mm_iter.cpp***********************

@brief module manager iterator

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_iter.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#include "mm_main.h"
#include "mm_acdb_file_reader.h"
#include "mm_mem_blob_reader.h"


/**
 * this module_info struct is obtained by parsing external bit-streams
 * obtained from ACDB file or from mem blob.
 */
//typedef struct module_info_t module_info_t;
struct module_info_t
{
   //below fields are valid for all sources
   uint32_t mod_info_version;
   uint32_t mod_info_size;

   bool_t mod_id_chunk_present;
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;

   bool_t flags_chunk_present;
   uint16_t needed;
   uint16_t preload;

   //below fields are valid for MM_MODULE_SOURCE_ACDB_MREG_INFO & MM_MODULE_SOURCE_REG_API
   bool_t   load_info_chunk_present;
   uint32_t itype;
   uint32_t file_name_len;
   uint8_t  *file_name_ptr;
   uint32_t tag1_len;
   uint8_t  *tag1_ptr;
   uint32_t tag2_len;
   uint8_t  *tag2_ptr;

   //below fields are valid only for MM_MODULE_SOURCE_REG_API
   bool_t err_code_chunk_present;
   ADSPResult *err_code_ptr;
};

typedef ADSPResult (*reader_func)(void *buf_ptr, uint32_t el_size, uint32_t count, void *obj);
typedef ADSPResult (*seek_func)(void *obj, uint32_t offset, uint32_t origin);
typedef bool_t (*eos_func)(void *obj);
typedef uint32_t (*tell_func)(void *obj);
typedef ADSPResult (*set_max_rd_pos)(void *obj, uint32_t max_rd_pos);
typedef uint32_t *(*get_pointer_to_current_loc)(void *obj);

typedef struct mod_info_iterator_t mod_info_iterator_t;
struct mod_info_iterator_t
{
   mm_module_source_t src;
   reader_func rd_f;
   seek_func seek_f;
   eos_func eos_f; //end of section
   tell_func tell_f;
   set_max_rd_pos set_max_rd_pos_f;
   get_pointer_to_current_loc get_pointer_to_current_loc_f;
   void *reader_obj;
};

typedef struct chunk_header_t chunk_header_t;
struct chunk_header_t
{
   uint32_t chunk_id;
   uint32_t version;
   uint32_t chunk_size;
};
typedef struct module_id_t module_id_t;
struct module_id_t
{
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;
} ;

typedef struct flags_t flags_t;
struct flags_t
{
   uint16_t needed;
   uint16_t preload;
};

static ADSPResult iter_deinit(mod_info_iterator_t *iter, mm_module_source_t src, void* reader_obj)
{
   ADSPResult result = ADSP_EOK;
   switch(src)
   {
   case MM_MODULE_SOURCE_ACDB_BUILT_IN:
      result = acdb_file_reader_release_active_section( (acdb_file_reader_t*)reader_obj, SECTION_ID_BUILTIN);
      break;
   case MM_MODULE_SOURCE_ACDB_MREG_INFO:
      result = acdb_file_reader_release_active_section( (acdb_file_reader_t*)reader_obj, SECTION_ID_MREG);
      break;
   case MM_MODULE_SOURCE_REG_API_OUT_OF_BAND:
   case MM_MODULE_SOURCE_REG_API_IN_BAND:
      break;
   default:
      return ADSP_EUNSUPPORTED;
   }
   return result;
}

/**
 * iterator init moves the file position to the built-in or mreg info section.
 * next() function only needs to seek from CURRENT.
 *
 *
 */
static ADSPResult iter_init(mod_info_iterator_t *iter, mm_module_source_t src, void* reader_obj)
{
   ADSPResult result = ADSP_EOK;
   iter->src = src;
   switch(src)
   {
   case MM_MODULE_SOURCE_ACDB_BUILT_IN:
      result = acdb_file_reader_set_active_section( (acdb_file_reader_t*)reader_obj, SECTION_ID_BUILTIN);
      if (ADSP_FAILED(result)) return result;

      iter->rd_f = acdb_file_reader_read;
      iter->seek_f = acdb_file_reader_seek;
      iter->eos_f = acdb_file_reader_eos;
      iter->tell_f = acdb_file_reader_tell;
      iter->set_max_rd_pos_f = acdb_file_reader_set_max_rd_pos;
      iter->get_pointer_to_current_loc_f = NULL;
      break;
   case MM_MODULE_SOURCE_ACDB_MREG_INFO:
      result = acdb_file_reader_set_active_section( (acdb_file_reader_t*)reader_obj, SECTION_ID_MREG);
      if (ADSP_FAILED(result))  return result;

      iter->rd_f = acdb_file_reader_read;
      iter->seek_f = acdb_file_reader_seek;
      iter->eos_f = acdb_file_reader_eos;
      iter->tell_f = acdb_file_reader_tell;
      iter->set_max_rd_pos_f = acdb_file_reader_set_max_rd_pos;
      iter->get_pointer_to_current_loc_f = NULL;
      break;
   case MM_MODULE_SOURCE_REG_API_OUT_OF_BAND:
   case MM_MODULE_SOURCE_REG_API_IN_BAND:
      //this is already tuned to correct section.
      iter->rd_f = mem_blob_reader_read;
      iter->seek_f = mem_blob_reader_seek;
      iter->eos_f = mem_blob_reader_eos;
      iter->tell_f = mem_blob_reader_tell;
      iter->set_max_rd_pos_f = mem_blob_reader_set_max_rd_pos;
      iter->get_pointer_to_current_loc_f = mem_blob_reader_get_pointer_to_current_loc;
      break;
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Invalid source %d given for iter_init",src);
      return ADSP_EUNSUPPORTED;
   }
   iter->reader_obj = reader_obj;

   return result;
}

static ADSPResult read_len_and_str(mod_info_iterator_t *iter,
      chunk_header_t *cheader, uint32_t *str_len, uint8_t **str,
      uint32_t *bytes_read, uint32_t *bytes_needed)
{
   int rc=0;
   *str=NULL;
   *str_len=0;
   *bytes_needed += sizeof(uint32_t);

   if (cheader->chunk_size < *bytes_needed)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," chunk size %lu insufficient in load-info to read string len", cheader->chunk_size);
      return ADSP_EFAILED;
   }

   uint32_t len=0;
   ADSPResult result = iter->rd_f(&len, sizeof(uint32_t), 1, iter->reader_obj);
   if (result)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading string length");
      return ADSP_EFAILED;
   }
   *bytes_read+= sizeof(uint32_t);

   //printf("len = %lu\n", len);
   if (len)
   {
      *bytes_needed+=len;
      if (cheader->chunk_size < *bytes_needed)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," chunk size %lu insufficient in load-info to read string of len %lu", cheader->chunk_size, len);
         return ADSP_EFAILED;
      }

      *str = (uint8_t*)qurt_elite_memory_malloc(len+1, QURT_ELITE_HEAP_DEFAULT); //one extra to pad null char in case original is not terminated
      if (*str==NULL)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Malloc failed");
         return ADSP_ENOMEMORY;
      }

      ADSPResult result = iter->rd_f(*str, sizeof(uint8_t), len, iter->reader_obj);
      if (result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading string ");
         qurt_elite_memory_free(*str);
         *str = NULL;
         return ADSP_EFAILED;
      }
      uint8_t *a=(uint8_t*)*str;
      a[len] = '\0';
      *str_len = len;
      *bytes_read+=len;
   }

   if (rc) return ADSP_EFAILED;
   else    return rc;
}

/**
 * processes a chunk
 *
 * note: this func mallocs.
 */
static ADSPResult process_chunk(mod_info_iterator_t *iter, chunk_header_t *cheader,
      module_info_t *mod_info, uint32_t *bytes_read)
{
#ifdef VERBOSE
   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  process_chunk 0x%lx ", cheader->chunk_id);
#endif
   ADSPResult result=0;
   *bytes_read = 0;
   if (cheader->chunk_size & 0x3)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," chunk size is not a multiple of 4.");
      return ADSP_EFAILED;
   }
   switch(cheader->chunk_id)
   {
   case AMDB_CFG_CHUNK_ID_MODULE_ID:
   {
      if (cheader->version >= AMDB_CFG_MODULE_ID_CHUNK_VERSION)
      {
         if (cheader->chunk_size >= sizeof(amdb_cfg_module_id_t) )
         {
            if (mod_info->mod_id_chunk_present)
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," module-id chunk is repeating, ignoring the second. ");
               return ADSP_EOK;
            }
            mod_info->mod_id_chunk_present=TRUE;

            module_id_t mid;

            result = iter->rd_f(&mid, sizeof(mid), 1, iter->reader_obj);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading module-id ");
               return result;
            }
            *bytes_read +=  sizeof(mid);

            mod_info->mtype = mid.mtype;
            mod_info->id1 = mid.id1;
            mod_info->id2 = mid.id2;

         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," module-id chunk has unsupported size %lu ",cheader->chunk_size);
            return ADSP_EFAILED;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-id chunk has unsupported version %lu ", cheader->version);
         return ADSP_EFAILED;
      }

      break;
   }
   case AMDB_CFG_CHUNK_ID_FLAGS:
   {
      if (cheader->version >= AMDB_CFG_FLAGS_CHUNK_VERSION)
      {
         if (cheader->chunk_size >= sizeof(amdb_cfg_flags_t) )
         {

            if (mod_info->flags_chunk_present)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," flags chunk is repeating, ignoring the second. ");
               return ADSP_EOK;
            }

            mod_info->flags_chunk_present=TRUE;

            flags_t f;

            result = iter->rd_f(&f, sizeof(f), 1, iter->reader_obj);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading flags ");
               return result;
            }
            *bytes_read +=  sizeof(f);

            mod_info->needed = f.needed;
            mod_info->preload = f.preload;

         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," flags chunk has unsupported size %lu ",cheader->chunk_size);
            return ADSP_EFAILED;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," flags chunk has unsupported version %lu ", cheader->version);
         return ADSP_EFAILED;
      }

      break;
   }
   case AMDB_CFG_CHUNK_ID_LOAD_INFO:
   {
      if ( (iter->src == MM_MODULE_SOURCE_ACDB_MREG_INFO) ||
            (iter->src == MM_MODULE_SOURCE_REG_API_OUT_OF_BAND)||
            (iter->src == MM_MODULE_SOURCE_REG_API_IN_BAND))
      {
         if (cheader->version >= AMDB_CFG_LOAD_INFO_CHUNK_VERSION)
         {
            if ( mod_info->load_info_chunk_present )
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Load-info chunk is repeating, ignoring the second. ");
               return ADSP_EOK;
            }
            mod_info->load_info_chunk_present = TRUE;

            uint32_t itype=0;
            uint32_t needed_size = sizeof(*cheader); //keeps accumulating every call to read_len_and_str.
            needed_size += sizeof(itype);

            if (cheader->chunk_size < needed_size)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," chunk size %lu not enough to read itype of size %u",cheader->chunk_size,sizeof(itype));
               return result;
            }

            result = iter->rd_f(&itype, sizeof(itype), 1, iter->reader_obj);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading itype ");
               return result;
            }
            *bytes_read +=  sizeof(itype);
            mod_info->itype = itype;

            result = read_len_and_str(iter, cheader, &mod_info->file_name_len,
                  &mod_info->file_name_ptr, bytes_read, &needed_size);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading file name");
               return result;
            }
            //note: needed_size carries forward.

            result = read_len_and_str(iter, cheader, &mod_info->tag1_len,
                  &mod_info->tag1_ptr, bytes_read, &needed_size);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading tag1");
               return result;
            }

            result = read_len_and_str(iter, cheader, &mod_info->tag2_len,
                  &mod_info->tag2_ptr, bytes_read, &needed_size);
            if (result)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading tag2");
               return result;
            }

         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," load-info chunk has unsupported version %lu ", cheader->version);
            return ADSP_EFAILED;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Ignoring LOAD INFO chunk for source %d", iter->src);
      }
      break;
   }
   case AMDB_CFG_CHUNK_ID_UI_RELATED_INFO:
   {
#ifdef MEDIUM_LOG
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Ignoring UI RELATED INFO chunk");
#endif
      break;
   }
   case AMDB_CFG_CHUNK_ID_ERROR_CODE:
   {
      if (iter->src == MM_MODULE_SOURCE_REG_API_OUT_OF_BAND)
      {
         if (cheader->version >= AMDB_CFG_ERROR_CODE_CHUNK_VERSION)
         {
            if (cheader->chunk_size >= sizeof(amdb_cfg_error_code_t) )
            {
               if ( mod_info->err_code_chunk_present)
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," error-code chunk is repeating, ignoring the second. ");
                  return ADSP_EOK;
               }
               mod_info->err_code_chunk_present = TRUE;

               uint32_t err_code=0;
               ADSPResult *err_code_ptr = NULL;
               err_code_ptr = (ADSPResult*)(iter->get_pointer_to_current_loc_f(iter->reader_obj));

               result = iter->rd_f(&err_code, sizeof(err_code), 1, iter->reader_obj);
               if (result)
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading error-code ");
                  return result;
               }
               mod_info->err_code_ptr = err_code_ptr;

               *bytes_read +=  sizeof(err_code);

            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," error-code chunk has unsupported size %lu ",cheader->chunk_size);
               return ADSP_EFAILED;
            }
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," error-code chunk has unsupported version %lu ", cheader->version);
            return ADSP_EFAILED;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Ignoring ERROR CODE chunk for source %d", iter->src);
      }
      break;
   }
   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Chunk ID 0x%lx unknown. ignoring.", cheader->chunk_id); //ignore for forward compatibility.
      break;
   }
   }
#ifdef VERBOSE
   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  process_chunk 0x%lx, size=%lu, bytes-read=%lu ", cheader->chunk_id, cheader->chunk_size, *bytes_read);
#endif
   return result;
}

/**
 * if init was not called before then header will be invalid.
 */
static ADSPResult iter_get_next(mod_info_iterator_t *iter, module_info_t *mod_info, uint32_t *location)
{
   ADSPResult result=0;
   uint32_t total_bytes_read = 0;
   uint32_t seekby = 0;

   *location = iter->tell_f(iter->reader_obj);

   //read at least below number of bytes for header.
   //depending on header version more bytes may need to be read.
#ifdef VERBOSE
   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO," Trying to read header at location %lu", *location);
#endif
   amdb_cfg_module_info_header_t header;
   result = iter->rd_f(&header, sizeof(header), 1, iter->reader_obj);
   if (ADSP_FAILED(result))
   {
      if (result != ADSP_EEOF)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error reading module info header ");
      }
      return result;
   }

   total_bytes_read=sizeof(header);

   //uint32_t header_size = 0;
   if (header.header_version >= AMDB_CFG_MODULE_INFO_HEADER_VERSION)
   {
      //greater is ok for supporting new elements in this struct in future.
      if (header.header_size < sizeof(amdb_cfg_module_info_header_t))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Header size %lu is not sufficient for header version %lu ", header.header_size, header.header_version);
         return ADSP_EFAILED;
      }
      if (header.header_size & 0x3)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," header size is not a multiple of 4.");
         return ADSP_EFAILED;
      }
      if (header.module_info_version >=  AMDB_CFG_MODULE_INFO_VERSION)
      {
         mod_info->mod_info_version = header.module_info_version;
         if (header.module_info_size & 0x3)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," module info size is not a multiple of 4.");
            return ADSP_EFAILED;
         }
         if (header.module_info_size<header.header_size)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," module info size %lu is less than header size %lu",mod_info->mod_info_size,header.header_size);
            return ADSP_EFAILED;
         }
         mod_info->mod_info_size = header.module_info_size;

         //Any EOF after this is EOF for this read position.
         result = iter->set_max_rd_pos_f(iter->reader_obj, mod_info->mod_info_size-header.header_size);
         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error: setting max position ");
            goto __handle_fail;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error: module info version %lu is not right ", header.module_info_version);
         return ADSP_EFAILED;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error: module info header version %lu is not right ", header.header_version);
      return ADSP_EFAILED;
   }

#ifdef VERBOSE
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Header read. trying to seek to chunks");
#endif
   //seek to chunks
   seekby=header.header_size-sizeof(header);
   result = iter->seek_f(iter->reader_obj, seekby, SEEK_FROM_CURRENT);
   if (result)
   {
      if (result != ADSP_EEOF)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," seek failed ");
      }
      goto __handle_fail;
   }

   //now we are at first chunk
   while(TRUE)
   {
      uint32_t temp_bytes_read=0;
      uint32_t bytes_read_in_chunk=0;
      chunk_header_t cheader;

#ifdef VERBOSE
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Reading chunk header");
#endif

      result = iter->rd_f(&cheader, sizeof(cheader), 1, iter->reader_obj);
      if (ADSP_FAILED(result))
      {
         if (result != ADSP_EEOF)
         {
            MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Error reading chunk header ");
         }
         goto __handle_fail;
      }

      bytes_read_in_chunk+=sizeof(cheader);
      total_bytes_read+=sizeof(cheader);

      result = process_chunk(iter, &cheader, mod_info, &temp_bytes_read);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Process chunk failed ");
         //goto next module.
         goto __handle_fail;
      }

      bytes_read_in_chunk+=temp_bytes_read;
      total_bytes_read+=temp_bytes_read;

      uint32_t seekby=cheader.chunk_size-bytes_read_in_chunk;
      result = iter->seek_f(iter->reader_obj, seekby, SEEK_FROM_CURRENT);
      if (ADSP_FAILED(result))
      {
         if (result != ADSP_EEOF)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," seek failed ");
         }
         goto __handle_fail;
      }

      bytes_read_in_chunk+=seekby;
      total_bytes_read+=seekby;

#ifdef VERBOSE
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"bytes_read_in_chunk = %lu, chunk-size=%lu. current position %lu total_bytes_read in mod-info %lu",
            bytes_read_in_chunk,cheader.chunk_size, iter->tell_f(iter->reader_obj), total_bytes_read);
#endif

      if ( (result == ADSP_EEOF) || iter->eos_f(iter->reader_obj)) //end of moduleinfosize or section.
      {
#ifdef MEDIUM_LOG
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," End of sections or end of module hit ");
#endif
         result = ADSP_EEOF;
         break;
      }
   }

#ifdef VERBOSE
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"mod_info->mod_info_size %lu total_bytes_read in mod-info %lu. result %d",
         mod_info->mod_info_size, total_bytes_read, result);
#endif

   //always seek to next section before returning.
   __handle_fail:
   if (result != ADSP_EEOF)
   {
#ifdef VERBOSE
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"seeking to next module: mod_info->mod_info_size %lu total_bytes_read in mod-info %lu. result %d",
            mod_info->mod_info_size, total_bytes_read, result);
#endif
      if (mod_info->mod_info_size > total_bytes_read)
      {
         seekby=mod_info->mod_info_size-total_bytes_read;
         result = iter->seek_f(iter->reader_obj, seekby, SEEK_FROM_CURRENT);
         if ((result != ADSP_EEOF) && (result != ADSP_EOK))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," seek failed ");
         }
      }
   }

   //only this module-info is ended. so do not return EOF.
   if (result == ADSP_EEOF)
   {
      result = ADSP_EOK;
   }
   //reset max rd position as this module ended.
   iter->set_max_rd_pos_f(iter->reader_obj, 0);

   return result;
}

static void print_mod_info(mod_info_iterator_t *iter, module_info_t *mod_info, uint32_t location, uint32_t num_module)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,  "   [MODULE INFO] : ");
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : Module source %d, at location %lu bytes, num_module %lu",iter->src, location,num_module);
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : Itype, Mtype, Id1, Id2, preload, needed = {%lu, %lu, 0x%lx, 0x%lx, %u, %u} ",
         mod_info->itype, mod_info->mtype, mod_info->id1, mod_info->id2, mod_info->preload, mod_info->needed);
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : Chunks present mod-id %d, flags %d, load-info %d, err-code %u, ", mod_info->mod_id_chunk_present,
         mod_info->flags_chunk_present, mod_info->load_info_chunk_present, mod_info->err_code_chunk_present);

   if (mod_info->load_info_chunk_present)
   {
      if ( (mod_info->file_name_len==0) || (mod_info->tag1_len == 0))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : file name and tag1 cannot be of zero length");
      }
      else
      {
         if (mod_info->tag2_len)
         {
            MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : File name, Tag1, Tag2 = {%lu, %lu, %lu}, {%s, %s, %s}",
                  mod_info->file_name_len, mod_info->tag1_len, mod_info->tag2_len,
                  mod_info->file_name_ptr, mod_info->tag1_ptr, mod_info->tag2_ptr);
         }
         else
         {
            MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : File name, Tag1, Tag2 = {%lu, %lu, %lu}, {%s, %s}",
                  mod_info->file_name_len, mod_info->tag1_len, mod_info->tag2_len,
                  mod_info->file_name_ptr, mod_info->tag1_ptr);
         }
      }
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"   [MODULE INFO] : \n");
}

static ADSPResult validate_mod_info(mod_info_iterator_t *iter, module_info_t *mod_info, uint32_t location, uint32_t num_module, bool_t is_reg)
{
   bool_t interface_type_known = FALSE;
   bool_t is_built_in = FALSE;

   switch(iter->src)
   {
   case MM_MODULE_SOURCE_ACDB_BUILT_IN:
   {
      if (!(mod_info->mod_id_chunk_present && mod_info->flags_chunk_present))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] module-id %u or flags chunk %u is not found",
               location,num_module, mod_info->mod_id_chunk_present,mod_info->flags_chunk_present);
         return ADSP_EFAILED;
      }
      if (mod_info->err_code_chunk_present)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] error code chunk cannot be present in built-in section",
               location,num_module);
         return ADSP_EFAILED;
      }
      is_built_in = TRUE;
      break;
   }
   case MM_MODULE_SOURCE_ACDB_MREG_INFO:
   {
      if (!(mod_info->mod_id_chunk_present && mod_info->flags_chunk_present && mod_info->load_info_chunk_present))
      {
         MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] module-id %u or flags chunk %u or load-info chunk %u is not found",
               location,num_module, mod_info->mod_id_chunk_present,
               mod_info->flags_chunk_present,mod_info->load_info_chunk_present);
         return ADSP_EFAILED;
      }

      if ( (mod_info->file_name_len == 0) || (mod_info->tag1_len == 0))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] file name and tag1len cannot be zero",
               location,num_module);
         return ADSP_EFAILED;
      }

      if (mod_info->itype == AMDB_INTERFACE_TYPE_CAPI)
      {
         if (mod_info->tag2_len == 0)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] tag2len cannot be zero for CAPI",
                  location,num_module);
            return ADSP_EFAILED;
         }
      }

      if (mod_info->err_code_chunk_present)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] error code chunk cannot be present in mreg-info section",
               location,num_module);
         return ADSP_EFAILED;
      }
      break;
   }
   case MM_MODULE_SOURCE_REG_API_OUT_OF_BAND:
   case MM_MODULE_SOURCE_REG_API_IN_BAND:
   {
      if (is_reg)
      {
         //if load-info chunk is not present, then this API is treated as trying to override the flags of built-in module.
         if (!(mod_info->mod_id_chunk_present && mod_info->flags_chunk_present ))
         {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] module-id %u or flags chunk %u is not found",
                  location,num_module, mod_info->mod_id_chunk_present,
                  mod_info->flags_chunk_present);
            return ADSP_EFAILED;
         }

         if (mod_info->load_info_chunk_present)
         {
            if ( (mod_info->file_name_len == 0) || (mod_info->tag1_len == 0))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] file name and tag1len cannot be zero",
                     location,num_module);
               return ADSP_EFAILED;
            }

            if (mod_info->itype == AMDB_INTERFACE_TYPE_CAPI)
            {
               if (mod_info->tag2_len == 0)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] tag2len cannot be zero for CAPI",
                        location,num_module);
                  return ADSP_EFAILED;
               }
            }
         }
         else
         {
            is_built_in = TRUE;
         }
      }
      else
      {
         //for dereg cmd, only mod-id chunk is sufficient. flags and load-info chunks can be ignored.
         if (!(mod_info->mod_id_chunk_present))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] module-id %u is not found",
                  location,num_module, mod_info->mod_id_chunk_present);
            return ADSP_EFAILED;
         }
         //even in this case, built-in module may come, but it's going to remove only from client-entry.
      }

      if (mod_info->err_code_chunk_present && (NULL == mod_info->err_code_ptr))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] error code chunk is present but result pointer is NULL",
               location,num_module);
      }

      break;
   }
   default:
      return ADSP_EFAILED;
   }

   if ( !((mod_info->mtype == AMDB_MODULE_TYPE_GENERIC) ||
         (mod_info->mtype == AMDB_MODULE_TYPE_DECODER)  ||
         (mod_info->mtype == AMDB_MODULE_TYPE_ENCODER)  ||
         (mod_info->mtype == AMDB_MODULE_TYPE_CONVERTER)  ||
         (mod_info->mtype == AMDB_MODULE_TYPE_PACKETIZER) ||
         (mod_info->mtype == AMDB_MODULE_TYPE_DEPACKETIZER))  )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] Invalid module type %lu",
            location,num_module, mod_info->itype);
      return ADSP_EFAILED;
   }

   if (mod_info->load_info_chunk_present)
   {
      if ( !((mod_info->itype == AMDB_INTERFACE_TYPE_CAPI)  ||
            (mod_info->itype == AMDB_INTERFACE_TYPE_CAPI_V2))  )
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] Invalid interface type %lu",
               location,num_module, mod_info->itype);
         return ADSP_EFAILED;
      }
      interface_type_known = TRUE;
   }

   if (interface_type_known)
   {
      switch(mod_info->itype)
      {
      case AMDB_INTERFACE_TYPE_CAPI:
      {
         if ( !(
               (mod_info->mtype == AMDB_MODULE_TYPE_DECODER)  ||
               (mod_info->mtype == AMDB_MODULE_TYPE_ENCODER)
         )  )
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] Invalid module type %lu for CAPI",
                  location,num_module, mod_info->itype);
            return ADSP_EFAILED;
         }
         break;
      }
      case AMDB_INTERFACE_TYPE_CAPI_V2:
      {
         break;
      }
      default:
      {
         return ADSP_EFAILED;
         break;
      }
      }
   }

   if ( !( (mod_info->preload == AMDB_CFG_PRELOAD_FLAG_ON_USE) ||
         (mod_info->preload == AMDB_CFG_PRELOAD_FLAG_IMMEDIATE) ||
         (mod_info->preload == AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD)
   ))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] Invalid preload value %u ",
            location,num_module, mod_info->preload);
      return ADSP_EFAILED;
   }

   /** for built-in modules only preload can be USE_FROM_BUILD. for others its not an acceptable value.*/
   if (!is_built_in)
   {
      if (mod_info->flags_chunk_present)
      {
         if ( mod_info->preload == AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD )
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Module-info[%lu, %lu] Invalid preload value %u ",
                  location,num_module, mod_info->preload);
            return ADSP_EFAILED;
         }
      }
   }
   return ADSP_EOK;
}

static void free_mod_info(module_info_t *mod_info)
{
   if (mod_info->load_info_chunk_present)
   {
      if (mod_info->file_name_ptr)
      {
         qurt_elite_memory_free(mod_info->file_name_ptr);
         mod_info->file_name_ptr=NULL;
      }
      if (mod_info->tag1_ptr)
      {
         qurt_elite_memory_free(mod_info->tag1_ptr);
         mod_info->tag1_ptr=NULL;
      }
      if (mod_info->tag2_ptr)
      {
         qurt_elite_memory_free(mod_info->tag2_ptr);
         mod_info->tag2_ptr=NULL;
      }
   }
}

/**
 * non-built-in modules come from MREG_INFO or from REG_API
 */
static ADSPResult register_non_built_in_modules(uint32_t amdb_entry_type, module_info_t *mod_info, bool_t is_reg, uint32_t client_id)
{
   ADSPResult result = ADSP_EOK;

   if ( !is_reg && (amdb_entry_type != ADSP_AMDB_ENTRY_TYPE_CLIENT))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," register non-built-in modules entered with invalid entry type 0x%lx for deref",amdb_entry_type);
      return ADSP_EUNSUPPORTED;
   }

   //Modules marked as not needed in the mreginfo or in the API must be registered as stub,
   if (is_reg)
   {
      if (!mod_info->needed)
      {
         result = adsp_amdb_add_stub_with_entry_type(amdb_entry_type, mod_info->mtype, mod_info->id1, mod_info->id2, mod_info->err_code_ptr, client_id);
         return result;
      }
   }
   //for deregistration, remove must be called, same as below switch.
   if (!is_reg)
   {
      result = adsp_amdb_remove_with_entry_type(amdb_entry_type, mod_info->mtype, mod_info->id1, mod_info->id2);
   }
   else
   {
      switch(mod_info->itype)
      {
      case AMDB_INTERFACE_TYPE_CAPI_V2:
      {
         result = adsp_amdb_add_capi_v2_with_entry_type(amdb_entry_type,
               mod_info->mtype, mod_info->id1, mod_info->id2, mod_info->preload, (const char*)mod_info->file_name_ptr,
               (const char*)mod_info->tag1_ptr, mod_info->err_code_ptr, client_id);
         break;
      }
      default:
      {
         result = ADSP_EUNSUPPORTED;
         break;
      }
      }
   }
   return result;
}

/**
 * continues to next module until EOF is hit.
 */
ADSPResult mm_loop_through_modules_and_reg(void *rd_ptr, mm_module_source_t src, bool_t is_reg, uint32_t client_id, void *tracker)
{
   ADSPResult result = ADSP_EOK;
   mod_info_iterator_t iter;
   module_info_t mod_info;
   ADSPResult result_reg_api = ADSP_EOK;

   if ( ((src != MM_MODULE_SOURCE_REG_API_OUT_OF_BAND)&&(src != MM_MODULE_SOURCE_REG_API_IN_BAND))
         && (is_reg == FALSE))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," cannot use dereg for built-in and mreg-info");
      return ADSP_EFAILED;
   }

   result = iter_init(&iter, src, rd_ptr);
   if (ADSP_ENOTIMPL == result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," source %d not found.", src);
      return result;
   }
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," iter_init failed");
      return result;
   }
   uint32_t num_module=0;
   //continues to next module until EOF is hit.
   while(TRUE)
   {
      uint32_t location;
      memset(&mod_info, 0, sizeof(module_info_t));

      //get-next returns EoF
      result = iter_get_next(&iter, &mod_info, &location);
      if ((result != ADSP_EOK) && (result != ADSP_EEOF) )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"iter_get_next returned error %d. Continuing ",result);
      }

      //the result may be EOK or EOF or fail. in any case, before exiting check if mod-id chunk is present.
      if (mod_info.mod_id_chunk_present)
      {
         num_module++; //Increment as long a a module is present even if it's not validated to be correct.

         result = validate_mod_info(&iter, &mod_info, location, num_module, is_reg);
         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"validate_mod_info returned failure");
            result_reg_api |= result;
         }
         else
         {
            print_mod_info(&iter, &mod_info, location, num_module);

            switch(src)
            {
            case MM_MODULE_SOURCE_ACDB_BUILT_IN:
            {
               built_in_mod_info_t acdb_inf;
               acdb_inf.id1 = mod_info.id1;
               acdb_inf.id2 = mod_info.id2;
               acdb_inf.mtype = mod_info.mtype;
               // (mod_info.flags_chunk_present) at this time it's present.
               acdb_inf.needed = mod_info.needed;
               acdb_inf.preload = mod_info.preload;

               result = mm_register_built_in_module(ADSP_AMDB_ENTRY_TYPE_INIT, &acdb_inf, tracker); //goes to INIT entry
               if (ADSP_FAILED(result))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"mm_register_built_in_module returned failure. ignoring");
               }
               result = ADSP_EOK;// ignore the error.
               break;
            }
            case MM_MODULE_SOURCE_ACDB_MREG_INFO:
            {
               result = register_non_built_in_modules(ADSP_AMDB_ENTRY_TYPE_INIT, &mod_info, TRUE, client_id);
               if (ADSP_FAILED(result))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"register_non_built_in_modules returned failure. continuing");
               }
               break;
            }
            case MM_MODULE_SOURCE_REG_API_OUT_OF_BAND:
            case MM_MODULE_SOURCE_REG_API_IN_BAND:
            {
               if (mod_info.load_info_chunk_present) //non-built-in module.
               {
                  result = register_non_built_in_modules(ADSP_AMDB_ENTRY_TYPE_CLIENT, &mod_info, is_reg, client_id);
               }
               else //built-in module
               {
                  if (is_reg)
                  {
                     // instead of using ACDB file, the REGISTER_MODULES API could be used to change the preload/needed flags of built-in modules.
                     // this is useful esp. before the AVCS state changes to state 0x5.
                     built_in_mod_info_t acdb_inf;
                     acdb_inf.id1 = mod_info.id1;
                     acdb_inf.id2 = mod_info.id2;
                     acdb_inf.mtype = mod_info.mtype;
                     acdb_inf.needed = mod_info.needed;
                     acdb_inf.preload = mod_info.preload;

                     result = mm_register_built_in_module(ADSP_AMDB_ENTRY_TYPE_CLIENT, &acdb_inf, tracker); //goes to CLIENT entry
                     if (ADSP_FAILED(result))
                     {
                        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"mm_register_built_in_module returned failure. the module (%lu, 0x%lx, 0x%lx) may not be built in.", mod_info.mtype, mod_info.id1, mod_info.id2);
                     }
                     else
                     {
                        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"now adding pre-req modules for (%lu, 0x%lx, 0x%lx) if any", mod_info.mtype, mod_info.id1, mod_info.id2);
                        //add pre-req modules as well, ignore error as it may be already added due to some other module.
                        add_prereq_module_to_client_entry(mod_info.mtype, mod_info.id1, mod_info.id2, mod_info.needed);
                     }
                  }
                  else
                  {
                     result = adsp_amdb_remove_with_entry_type(ADSP_AMDB_ENTRY_TYPE_CLIENT, mod_info.mtype, mod_info.id1, mod_info.id2);
                     //pre-req modules are not removed during dereg: reason : if multiple mod need pre-req modules, then ref count will be needed. complicated to implement.
                     //registering pre-req mod to client entry & leaving it is ok for functionality (as 'preq-req' module's preload/needed flags are not affected), but may not be memory-wise.
                  }
               }

               if (ADSP_FAILED(result))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Reg/Dereq API handler returned failure. continuing");
                  result_reg_api |= result;
                  if (NULL != mod_info.err_code_ptr)
                  {
                     *mod_info.err_code_ptr = result;
                  }
               }
               result = ADSP_EOK;// ignore the error.
               break;
            }
            default:
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"unsupported source");
               break;
            }
            }
         }
      }
      //conditionally frees
      free_mod_info(&mod_info);

      if (result==ADSP_EEOF)
      {
         result = ADSP_EOK;
         break;
      }
      else
      {
         result_reg_api |= result;
      }
   }

   if (num_module==0)
   {
      result = ADSP_ENOTIMPL; //if no modules found in built-in => customer wants us to load all built-in.
   }

   if (result != ADSP_EEOF)
   {
      result_reg_api|=result;
   }

   iter_deinit(&iter, src, rd_ptr);

   //return error for Reg/Dereg APIs
   if ( (src==MM_MODULE_SOURCE_REG_API_OUT_OF_BAND) || (src == MM_MODULE_SOURCE_REG_API_IN_BAND) )
   {
      if (ADSP_FAILED(result_reg_api))
      {
         return ADSP_EFAILED;
      }
   }

   return result;
}
