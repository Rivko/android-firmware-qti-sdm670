/**
@file mm_iter.cpp***********************

@brief module manager acdb file reader

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_acdb_file_reader.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#include "mm_acdb_file_reader.h"

#ifndef SIM 
#define USE_FOPEN_WITH_ENV
#endif

#ifdef STAND_ALONE
#include "test_main.h"
#endif

typedef struct wrap_amdb_cfg_file_struct_t wrap_amdb_cfg_file_struct_t;
struct wrap_amdb_cfg_file_struct_t
{
   amdb_cfg_file_header_t header;
   /** file header. */
   uint8_t                version_info_key[AMDB_CFG_SECTION_KEY_LEN];
   /** AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY */
   uint32_t               version_info_len;
   /** length of the version info in bytes. sizeof(amdb_cfg_version_info_t).
    *
    * following this are version info bytes as shown in below.*/

   amdb_cfg_version_info_t   version_info;
};

#define LOCAL_BUFFER_SIZE 4096 //bytes

/**
 * File:                |-------------------|
 *  ftell:                                  ^
 * Local buffer:                   |----------|
 *  start_pos:                           ^
 *  end_pos:                                ^
 */
typedef struct local_buf_info_t local_buf_info_t;
struct local_buf_info_t
{
   uint32_t  start_pos; // start of valid data
   uint32_t  end_pos; // end of valid data.

   uint32_t  ftell_end; //ftell position corresponding to end_pos

   uint8_t   buf[LOCAL_BUFFER_SIZE];
};

typedef struct iacdb_file_reader_t iacdb_file_reader_t;

struct iacdb_file_reader_t
{
   FILE *file_ptr;
   uint32_t file_len;

   uint32_t built_in_sec_pos;
   uint32_t built_in_sec_len;

   uint32_t mreg_info_sec_pos;
   uint32_t mreg_info_sec_len;

   uint32_t section_found_mask;

   //private, temporary:
   uint32_t cur_sec_id;
   uint32_t cur_sec_payload_pos;
   uint32_t cur_sec_len;

   //active section
   uint32_t active_section_id;
   local_buf_info_t  *local_buf_info; //local buffer for caching the file reads. always goes with active section.

   //current max permissible read position
   //reads cannot happen beyond this position.
   //but can happen at this location
   //0=>dont use this.
   uint32_t cur_max_rd_pos;

};

static ADSPResult parse_top_level(iacdb_file_reader_t *acdb_reader_ptr);

/**
 * file_len is including header
 *
 * When func is entered, if rd_ptr->cur_sec_len !=0, then it's treated as previous sec len
 *
 * doesnt depend on active_section
 *
 * this func is not affected by local buffering.
 */
static ADSPResult acdb_file_reader_get_next_section(iacdb_file_reader_t *rd_ptr)
{
   int rc=0;
   uint32_t prev_sec_len = rd_ptr->cur_sec_len;

   rd_ptr->cur_sec_id = SECTION_ID_UNKNOWN;
   rd_ptr->cur_sec_payload_pos = 0;
   rd_ptr->cur_sec_len=0;

   //if previous section len is known then seek that much to save time searching.
   if (0 != prev_sec_len)
   {
      //printf (" prev_sec_len = %lu", prev_sec_len);
      rc=fseek(rd_ptr->file_ptr, prev_sec_len, SEEK_CUR);
      if (rc || ferror(rd_ptr->file_ptr))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  file seek error.");
         return ADSP_EEOF;
      }
   }

   uint8_t buf[8+1];
   memset(buf,0,sizeof(buf));
   while(1)
   {
      //Read next 8 bytes to know the section
      uint32_t bytes_read = fread(buf, 1, AMDB_CFG_SECTION_KEY_LEN, rd_ptr->file_ptr);
      MSG_9(MSG_SSID_QDSP6, DBG_LOW_PRIO,"   Buf[%lu]: 0x%02x 0x%02x 0x%02x 0x%02x  0x%02x 0x%02x 0x%02x 0x%02x", bytes_read, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
      if ((bytes_read<AMDB_CFG_SECTION_KEY_LEN) || feof(rd_ptr->file_ptr) || ferror(rd_ptr->file_ptr))
      {
         if ( (ftell(rd_ptr->file_ptr) == (int32_t)rd_ptr->file_len) ) //when file length given in the top header is diff than actual file length, then it's an error.
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"  EOF reached.");
            return ADSP_EEOF;
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  Next section not found. bytes_read=%lu",bytes_read);
            return ADSP_EFAILED;
         }
      }

      if (0==memcmp(buf, AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY, sizeof(AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY)))
      {
         rd_ptr->cur_sec_id = SECTION_ID_VER;
      }
      else if (0==memcmp(buf, AMDB_CFG_FILE_SECTION_BUILT_IN_MOD_INFO_KEY, sizeof(AMDB_CFG_FILE_SECTION_BUILT_IN_MOD_INFO_KEY)))
      {
         rd_ptr->cur_sec_id = SECTION_ID_BUILTIN;
      }
      else if (0==memcmp(buf, AMDB_CFG_FILE_SECTION_CUSTOM_MOD_INFO_KEY, sizeof(AMDB_CFG_FILE_SECTION_CUSTOM_MOD_INFO_KEY)))
      {
         rd_ptr->cur_sec_id = SECTION_ID_MREG;
      }
      else
      {
         rd_ptr->cur_sec_id = SECTION_ID_UNKNOWN;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Unknown section %s. Ignoring.", buf);
         continue; //continue, so as to support new file versions in this DSP.
      }

      if (rd_ptr->cur_sec_id != SECTION_ID_UNKNOWN)
      {
         //Read next 4 bytes to know the length of section
         uint32_t el_read = fread(&rd_ptr->cur_sec_len, 4, 1, rd_ptr->file_ptr);
         if ((el_read<1) || feof(rd_ptr->file_ptr))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  file is ending before hitting len of the section %s.el_read=%lu",buf,el_read);
            return ADSP_EFAILED;
         }
         if ( rd_ptr->cur_sec_len & 0x3 )
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Current section %lu length %lu is not multiple of 4.",
                  rd_ptr->cur_sec_id, rd_ptr->cur_sec_len);
            return ADSP_EFAILED;
         }
         rd_ptr->cur_sec_payload_pos = ftell(rd_ptr->file_ptr);

         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  Length of section %s is %lu, section position %lu",buf,rd_ptr->cur_sec_len, rd_ptr->cur_sec_payload_pos);
         return ADSP_EOK;
      }
   }
   return ADSP_EOK;
}

/**
 * this function does not create the local buf for reading files.
 * acdb_file_reader_set_active_section creates it .
 */
ADSPResult acdb_file_reader_create(acdb_file_reader_t **acdb_reader_ptr_ptr)
{
   ADSPResult rc=ADSP_EOK;
   *acdb_reader_ptr_ptr = NULL;
   iacdb_file_reader_t *rd_ptr=(iacdb_file_reader_t*)
         qurt_elite_memory_malloc(sizeof(iacdb_file_reader_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == rd_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," No memory creating acdb_file_reader_t");
      return ADSP_ENOMEMORY;
   }
   memset(rd_ptr, 0, sizeof(iacdb_file_reader_t)); //initializes active section to UNKNOWN

#ifdef USE_FOPEN_WITH_ENV
   //ENV_VAR_CONTAINING_PATH_TO_ACDB_FILE, ENV_VAR_DELIMITER, AMDB_CFG_FILE_NAME
   FILE *fp = fopen("file://adsp_avs_config.acdb?env=ADSP_AVS_CFG_PATH&delim=;", "rb");
#else
   FILE *fp = fopen(AMDB_CFG_FILE_NAME,"rb");
#endif
   if (fp == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error opening ACDB file %s (ACDB file is optional, hence error might be ok)", AMDB_CFG_FILE_NAME);
      qurt_elite_memory_free(rd_ptr);
      return ADSP_ENOTIMPL;
   }
   rd_ptr->file_ptr = fp;

   *acdb_reader_ptr_ptr = (acdb_file_reader_t*)rd_ptr;

   rc = parse_top_level(rd_ptr);
   if (rc == ADSP_ENOTIMPL)
   {
      acdb_file_reader_destroy(acdb_reader_ptr_ptr);
      return rc;
   }
   else if (ADSP_FAILED(rc))
   {
      acdb_file_reader_destroy(acdb_reader_ptr_ptr);
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

/**
 *
 */
ADSPResult acdb_file_reader_destroy(acdb_file_reader_t **acdb_reader_ptr_ptr)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)*acdb_reader_ptr_ptr;
   if (rd_ptr->file_ptr)
   {
      fclose(rd_ptr->file_ptr);
      rd_ptr->file_ptr=NULL;
   }
   qurt_elite_memory_free(rd_ptr);
   *acdb_reader_ptr_ptr = NULL;

   if (rd_ptr->local_buf_info)
   {
      qurt_elite_memory_free(rd_ptr->local_buf_info);
      rd_ptr->local_buf_info = NULL;
   }
   return ADSP_EOK;
}

/**
 * local buffering doesn't affect this.
 */
static ADSPResult parse_top_level(iacdb_file_reader_t *acdb_reader_ptr)
{
   ADSPResult result = ADSP_EOK;
//   int rc=0;

   uint8_t buf[sizeof(wrap_amdb_cfg_file_struct_t)];
   uint32_t bytes_read = fread(buf, 1, sizeof(wrap_amdb_cfg_file_struct_t), acdb_reader_ptr->file_ptr);
   if ((bytes_read<sizeof(wrap_amdb_cfg_file_struct_t)) || feof(acdb_reader_ptr->file_ptr))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Header & version chunk are missing .bytes_read=%lu",bytes_read);
      return ADSP_EFAILED;
   }
   wrap_amdb_cfg_file_struct_t *pHeader=(wrap_amdb_cfg_file_struct_t*)buf;
   if (0!=memcmp(pHeader->header.magic_word1, AMDB_CFG_FILE_MAGIC_WORD1, sizeof(pHeader->header.magic_word1)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Magic word1 %s, is not valid ",pHeader->header.magic_word1);
      return ADSP_EFAILED;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO," ACDB File Version (%lu.%lu) ",pHeader->header.acdb_major_version, pHeader->header.acdb_minor_version);

   if (0!=memcmp(pHeader->header.magic_word2, AMDB_CFG_FILE_MAGIC_WORD2,sizeof(pHeader->header.magic_word2)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Magic word2 %s, is not valid ",pHeader->header.magic_word2);
      return ADSP_EFAILED;
   }

   if (pHeader->header.compression_flag != 0)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Compression flag must be zero ");
      return ADSP_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Uncompressed length = %lu. Actual Length = %lu. ",
         pHeader->header.uncompressed_len, pHeader->header.actual_file_len);

   if (pHeader->header.uncompressed_len != pHeader->header.actual_file_len)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," pHeader->header.uncompressed_len != pHeader->header.actual_file_len ");
      return ADSP_EFAILED;
   }

   if (pHeader->header.actual_file_len==0)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," file length is zero. "); //need to load all modules.
      return ADSP_ENOTIMPL;
   }

   if ( (pHeader->header.uncompressed_len & 0x3) || (pHeader->header.actual_file_len & 0x3) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," actual length %lu and compressed length %lu of the file are not multiple of 4.",
            pHeader->header.actual_file_len, pHeader->header.uncompressed_len);
      return ADSP_EFAILED;
   }

   if (0!=memcmp((char*)pHeader->version_info_key, AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY,sizeof(pHeader->version_info_key)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Version info section key not present, in its position %s is found. ",pHeader->version_info_key);
      return ADSP_EFAILED;
   }
   if (pHeader->version_info_len != sizeof(amdb_cfg_version_info_t))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Version info length %lu not matching expected %d ",pHeader->version_info_len, sizeof(amdb_cfg_version_info_t));
      return ADSP_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO," AMDB CFG Version Information (%lu.%lu) ",
         pHeader->version_info.major_version, pHeader->version_info.minor_version);

   //Dont check min version as future min versions also need to be supported by this DSP.
   //Major version check rejects nonbackward compatible ones.
   if ((pHeader->version_info.major_version != AMDB_CFG_MAJOR_VERSION))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Invalid version in the VERSION INFO %lu.%lu",
            pHeader->version_info.major_version,pHeader->version_info.minor_version);
      return ADSP_EFAILED;
   }
   //(pHeader->version_info.minor_version != AMDB_CFG_MINOR_VERSION) decides which chunks will be present.
      //nothing to check now.

   acdb_reader_ptr->file_len=pHeader->header.actual_file_len+sizeof(amdb_cfg_file_header_t);

   //traverse until different sections are hit.
   while(1)
   {
      result=acdb_file_reader_get_next_section(acdb_reader_ptr);
      if ( result == ADSP_EEOF )
      {
         MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," EoF reached while looking for sections. ");
         result = ADSP_EOK;
         break;
      }
      else if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error finding next section ");
         break;
      }

      if (acdb_reader_ptr->cur_sec_id == SECTION_ID_UNKNOWN)
      {
         MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," EoF reached while looking for sections. ");
         break;
      }
      //seek to next section if version section is hit (as its already processed)
      else if (acdb_reader_ptr->cur_sec_id == SECTION_ID_VER)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Version section not expected");
         return ADSP_EFAILED;
      }
      else if (acdb_reader_ptr->cur_sec_id == SECTION_ID_BUILTIN)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO," BUILT-IN section found at pos %lu, len %lu",
               acdb_reader_ptr->cur_sec_payload_pos, acdb_reader_ptr->cur_sec_len);

         acdb_reader_ptr->section_found_mask |= BUILT_IN_SECTION_MASK;
         acdb_reader_ptr->built_in_sec_len = acdb_reader_ptr->cur_sec_len;
         acdb_reader_ptr->built_in_sec_pos = acdb_reader_ptr->cur_sec_payload_pos;
      }
      else if (acdb_reader_ptr->cur_sec_id == SECTION_ID_MREG)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO," MREGINFO section found at pos %lu, len %lu",
               acdb_reader_ptr->cur_sec_payload_pos, acdb_reader_ptr->cur_sec_len);

         acdb_reader_ptr->section_found_mask |= MREG_INFO_SECTION_MASK;
         acdb_reader_ptr->mreg_info_sec_len = acdb_reader_ptr->cur_sec_len;
         acdb_reader_ptr->mreg_info_sec_pos = acdb_reader_ptr->cur_sec_payload_pos;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO," Undefined section %lu hit. error.", acdb_reader_ptr->cur_sec_id);
         continue;
      }
   }

   if (acdb_reader_ptr->section_found_mask == (BUILT_IN_SECTION_MASK|MREG_INFO_SECTION_MASK))
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," All expected sections found");
      result = ADSP_EOK;
   }
   else if (acdb_reader_ptr->section_found_mask == (BUILT_IN_SECTION_MASK))
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Only built-in sections found");
      result = ADSP_EOK;
   }
   else if (acdb_reader_ptr->section_found_mask == (MREG_INFO_SECTION_MASK))
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," Only mreg-info sections found");
      result = ADSP_EOK;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error: Expected sections are not found section_found_mask=0x%lx", acdb_reader_ptr->section_found_mask);
      //when no sections are present, it's as good as file is absent. need to load all built-in.
      result = ADSP_ENOTIMPL;
   }

   return result;
}

/* **************************************************************************************************************
 *  Start of functions where local buffering is done.
 *
 * **************************************************************************************************************/

/**
 * if el_size*count bytes are not read then it's an error. So either el_size*count is read or not.
 */
static ADSPResult loc_buf_read_file(iacdb_file_reader_t *rd_ptr, void *buf_ptr, uint32_t el_size, uint32_t count)
{
   ADSPResult result = ADSP_EOK;
   if(rd_ptr->local_buf_info)
   {
#ifdef LOC_FILE_BUF_LOGS
      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_read_file fread end %lu, start %lu. to read %lu",
            rd_ptr->local_buf_info->end_pos, rd_ptr->local_buf_info->start_pos, (el_size*count));
#endif
      //read into local buf as needed.
      if (rd_ptr->local_buf_info->end_pos == rd_ptr->local_buf_info->start_pos)
      {
         uint32_t el_read=0;

         el_read = fread(rd_ptr->local_buf_info->buf, sizeof(uint8_t), LOCAL_BUFFER_SIZE, rd_ptr->file_ptr);

         if ((el_read<1) || ferror(rd_ptr->file_ptr))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  loc_buf_read_file fread error. el_read=%lu",el_read);
            el_read=0;
         }
         rd_ptr->local_buf_info->start_pos = 0;
         rd_ptr->local_buf_info->end_pos = el_read;
         rd_ptr->local_buf_info->ftell_end = ftell(rd_ptr->file_ptr);
      }

      if (buf_ptr)
      {
         //if buffer has enough
         uint32_t bytes_in_loc_buf = (rd_ptr->local_buf_info->end_pos - rd_ptr->local_buf_info->start_pos);
         uint32_t bytes_needed = el_size*count;
         uint32_t min_bytes = bytes_in_loc_buf<bytes_needed?bytes_in_loc_buf:bytes_needed;

         memscpy(buf_ptr, min_bytes, rd_ptr->local_buf_info->buf+rd_ptr->local_buf_info->start_pos, min_bytes);
         rd_ptr->local_buf_info->start_pos += min_bytes;
         bytes_needed -=min_bytes;

         if (bytes_needed)
         {
            if (feof(rd_ptr->file_ptr))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  loc_buf_read_file eof when bytes_needed=%lu",bytes_needed);
               return ADSP_EEOF;
            }
            else
            {
               //recursive call
#ifdef LOC_FILE_BUF_LOGS
               MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_read_file recursive call. bytes_needed=%lu",bytes_needed);
#endif
               result = loc_buf_read_file(rd_ptr, ((uint8_t*)buf_ptr)+min_bytes, 1, bytes_needed); //el_size cannot be used as min_bytes may not be aligned to el_size.
            }
         }
      }
   }
   else
   {
      uint32_t el_read=0;

      el_read = fread(buf_ptr, el_size, count, rd_ptr->file_ptr);

      if ((el_read<1) || feof(rd_ptr->file_ptr) || ferror(rd_ptr->file_ptr))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  loc_buf_read_file fread error. el_read=%lu",el_read);
         return ADSP_EEOF;
      }
   }
   return result;
}

static uint32_t loc_buf_get_cur_pos_in_file(iacdb_file_reader_t *rd_ptr)
{
   uint32_t pos;
   if (rd_ptr->local_buf_info)
   {
      pos = rd_ptr->local_buf_info->ftell_end;//ftell(rd_ptr->file_ptr);
#ifdef LOC_FILE_BUF_LOGS
      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_get_cur_pos_in_file fread end %lu, start %lu. file pos  %lu",
            rd_ptr->local_buf_info->end_pos, rd_ptr->local_buf_info->start_pos, (pos));
#endif

      pos = pos - (rd_ptr->local_buf_info->end_pos - rd_ptr->local_buf_info->start_pos);
   }
   else
   {
      pos = ftell(rd_ptr->file_ptr);
   }

   return pos;
}

static ADSPResult loc_buf_seek_in_file(iacdb_file_reader_t *rd_ptr, uint32_t offset, uint32_t origin)
{
   //ADSPResult result = ADSP_EOK;
   int rc=0;
   switch(origin)
   {
   case SEEK_FROM_CURRENT:
   {
      if(rd_ptr->local_buf_info)
      {
#ifdef LOC_FILE_BUF_LOGS
      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_seek_in_file fread end %lu, start %lu. offset %lu",
            rd_ptr->local_buf_info->end_pos, rd_ptr->local_buf_info->start_pos, (offset));
#endif

         uint32_t diff = rd_ptr->local_buf_info->end_pos - rd_ptr->local_buf_info->start_pos;
         if (offset <= diff)
         {
            rd_ptr->local_buf_info->start_pos += offset;
            offset = 0;
         }
         else
         {
            rd_ptr->local_buf_info->start_pos = rd_ptr->local_buf_info->end_pos;
            offset = offset - diff;
         }
      }

      if (offset)
      {
         rc=fseek(rd_ptr->file_ptr, offset, SEEK_CUR);
         if (rc || feof(rd_ptr->file_ptr) || ferror(rd_ptr->file_ptr))
         {
            MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_seek_in_file error seeking from current");
            rc = ADSP_EEOF;
         }
         if (rd_ptr->local_buf_info)
         {
            rd_ptr->local_buf_info->ftell_end = ftell(rd_ptr->file_ptr);
         }
      }
      break;
   }
   case SEEK_FROM_FILE_BEGIN:
   {
      if(rd_ptr->local_buf_info)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  loc_buf_seek_in_file seeking from FILE BEGIN is not implemented!!");
      }

      if (offset)
      {
         rc=fseek(rd_ptr->file_ptr, offset, SEEK_SET);
         if (rc || feof(rd_ptr->file_ptr) || ferror(rd_ptr->file_ptr))
         {
            MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  loc_buf_seek_in_file error seeking from begin ");
            rc = ADSP_EEOF;
         }
      }
      break;
   }
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Invalid source %lu given for loc_buf_seek_in_file",origin);
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

ADSPResult acdb_file_reader_release_active_section(acdb_file_reader_t *xrd_ptr, uint32_t section_id)
{
   ADSPResult result = ADSP_EOK;
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)xrd_ptr;

   rd_ptr->active_section_id = SECTION_ID_UNKNOWN;
   if (rd_ptr->local_buf_info)
   {
      qurt_elite_memory_free(rd_ptr->local_buf_info);
      rd_ptr->local_buf_info=NULL;
   }

   return result;
}

/**
 * both reads and writes can happen only in this section.
 */
ADSPResult acdb_file_reader_set_active_section(acdb_file_reader_t *xrd_ptr, uint32_t section_id)
{
   ADSPResult result = ADSP_EOK;
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)xrd_ptr;
   uint32_t locn=0;
   #ifdef PRINT_CUSTOM_MODULE_BYTES
   uint32_t len=0;
   #endif
   switch(section_id)
   {
   case SECTION_ID_BUILTIN:
   {
      if ( (rd_ptr->section_found_mask&BUILT_IN_SECTION_MASK) != BUILT_IN_SECTION_MASK)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"  acdb_file_reader_set_active_section built-in section not found");
         return ADSP_ENOTIMPL;
      }
      locn = rd_ptr->built_in_sec_pos;
      #ifdef PRINT_CUSTOM_MODULE_BYTES
      len = rd_ptr->built_in_sec_len;
	  #endif
      break;
   }
   case SECTION_ID_MREG:
   {
      if ( (rd_ptr->section_found_mask&MREG_INFO_SECTION_MASK) != MREG_INFO_SECTION_MASK)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"  acdb_file_reader_set_active_section mreg-info section not found");
         return ADSP_ENOTIMPL;
      }

      locn = rd_ptr->mreg_info_sec_pos;
	  #ifdef PRINT_CUSTOM_MODULE_BYTES
      len = rd_ptr->mreg_info_sec_len;
	  #endif
      break;
   }
   default:
      return ADSP_EFAILED;
   }

   rd_ptr->active_section_id = section_id;
   int rc=fseek(rd_ptr->file_ptr, locn, SEEK_SET);
   if (rc || feof(rd_ptr->file_ptr) || ferror(rd_ptr->file_ptr))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"  acdb_file_reader_set_active_section error/eof seeking to section %lu at locn %lu", section_id, locn);
      return ADSP_EEOF;
   }
   rd_ptr->cur_max_rd_pos = 0;
   rd_ptr->local_buf_info = (local_buf_info_t*)qurt_elite_memory_malloc(sizeof(local_buf_info_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == rd_ptr->local_buf_info)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," local buffer creation failed. will default to uncached file reads");
   }

   if (rd_ptr->local_buf_info)
   {
      rd_ptr->local_buf_info->end_pos = 0;
      rd_ptr->local_buf_info->start_pos = 0;
      rd_ptr->local_buf_info->ftell_end = 0;
      loc_buf_read_file(rd_ptr, NULL, 0, 0);
   }

#ifdef PRINT_CUSTOM_MODULE_BYTES
   //print the section in bytes
   {
      char c;
      for (int i=0;i<len;i++)
      {
         c=fgetc(rd_ptr->file_ptr);
         printf("0x%02x, ",c);
         int k=i+1;
         if (k==(k/4)*4) printf("\n");
      }
      fseek(rd_ptr->file_ptr, locn, SEEK_SET);
   }
#endif

   return result;
}

static inline bool_t is_in_active_section_and_below_max(iacdb_file_reader_t *rd_ptr, uint32_t len_bytes)
{

   //ftell: For binary streams, this is the number of bytes from the beginning of the file.
   uint32_t pos = loc_buf_get_cur_pos_in_file(rd_ptr);
   uint32_t len=len_bytes;
   switch(rd_ptr->active_section_id)
   {
   case SECTION_ID_BUILTIN:
      if (! is_val_in_range(pos, pos+len, rd_ptr->built_in_sec_pos, rd_ptr->built_in_sec_pos+rd_ptr->built_in_sec_len) )
      {
#ifdef MEDIUM_LOG
         MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_in_active_section_and_below_max BUILT-IN ranges from %lu to %lu & doesnt contain %lu to %lu. rd_ptr->cur_max_rd_pos %lu ",
               rd_ptr->built_in_sec_pos, rd_ptr->built_in_sec_pos+rd_ptr->built_in_sec_len, pos, pos+len, rd_ptr->cur_max_rd_pos);
#endif
         return FALSE;
      }
      break;
   case SECTION_ID_MREG:
      if (! is_val_in_range(pos, pos+len, rd_ptr->mreg_info_sec_pos, rd_ptr->mreg_info_sec_pos+rd_ptr->mreg_info_sec_len) )
      {
#ifdef MEDIUM_LOG
         MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_in_active_section_and_below_max MREG_INFO ranges from %lu to %lu & doesnt contain %lu to %lu. rd_ptr->cur_max_rd_pos %lu",
               rd_ptr->mreg_info_sec_pos, rd_ptr->mreg_info_sec_pos+rd_ptr->mreg_info_sec_len, pos, pos+len, rd_ptr->cur_max_rd_pos);
#endif
         return FALSE;
      }
      break;
   default:
      MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_in_active_section_and_below_max active section id %lu not valid for read. rd_ptr->cur_max_rd_pos %lu ", rd_ptr->active_section_id, rd_ptr->cur_max_rd_pos);
      return FALSE;
   }

   if (0!=rd_ptr->cur_max_rd_pos)
   {
      if (pos+len_bytes>rd_ptr->cur_max_rd_pos)
      {
#ifdef MEDIUM_LOG
         MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  is_in_active_section_and_below_max: item to be read (%lu+%lu) is not within current max read position %lu", pos,len_bytes, rd_ptr->cur_max_rd_pos);
#endif
         return FALSE;
      }
   }
   return TRUE;
}


/**
 * reads only in the active section
 *
 */
ADSPResult acdb_file_reader_read(void *buf_ptr, uint32_t el_size, uint32_t count, void *obj)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)obj;

   if (!is_in_active_section_and_below_max(rd_ptr, el_size*count))
   {
#ifdef MEDIUM_LOG
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  acdb_file_reader_read: not in active section or not below max rd position %lu", rd_ptr->active_section_id);
#endif
      return ADSP_EEOF;
   }

   return loc_buf_read_file(rd_ptr, buf_ptr, el_size, count);
}

bool_t acdb_file_reader_eos (void *obj)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)obj;
   if (is_in_active_section_and_below_max(rd_ptr, 1)) //if next byte is not in the active section,
   {
      //printf(" acdb_file_reader_eos: not EOS");
      return FALSE;
   }
   else
   {
#ifdef MEDIUM_LOG
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO," acdb_file_reader_eos: EOS");
#endif
      return TRUE;
   }
}

uint32_t acdb_file_reader_tell(void *obj)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)obj;
   return loc_buf_get_cur_pos_in_file(rd_ptr);
}

//max_rd_pos_from_cur_location=>reset
ADSPResult acdb_file_reader_set_max_rd_pos(void *obj, uint32_t max_rd_pos_from_cur_location)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)obj;


   if (max_rd_pos_from_cur_location)
   {
      uint32_t cur_pos = loc_buf_get_cur_pos_in_file(rd_ptr);
      rd_ptr->cur_max_rd_pos = cur_pos+max_rd_pos_from_cur_location;
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

/**
 * seeks only in the active section.
 *
 * origin: SEEK_FROM_FILE_BEGIN & SEEK_FROM_CURRENT
 */
ADSPResult acdb_file_reader_seek(void *obj, uint32_t offset, uint32_t origin)
{
   iacdb_file_reader_t *rd_ptr = (iacdb_file_reader_t*)obj;

   //if (offset==0)
   //{
      //return 0; //nothing to seek.
   //}

   uint32_t len=offset;
   if (!is_in_active_section_and_below_max(rd_ptr, len))
   {
#ifdef MEDIUM_LOG
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"  acdb_file_reader_seek: not in active section %lu or max rd point", rd_ptr->active_section_id);
#endif
      return ADSP_EEOF;
   }

   return loc_buf_seek_in_file(rd_ptr, offset, origin);
}


