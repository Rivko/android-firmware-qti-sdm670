/**************************************************************************
 * FILE: devprg_gpt.c
 *
 * GPT functions for firehose.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_gpt.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Update the arguments for the write to storage API.
2016-02-17   wek   Create.

===========================================================================*/

#include "comdef.h"
#include "devprg_firehose.h"
#include "devprg_log.h"
#include "devprg_storage.h"
#include <limits.h>
#include <string.h>

#define GPT_SIGNATURE          "EFI PART"
#define GPT_SIGNATURE_SIZE     8
#define GPT_PARTITION_ENTRY_BASE_SIZE_BYTES 128


PACKED struct devprg_guid
{
  uint32 data1;
  uint16 data2;
  uint16 data3;
  uint8  data4[8];
} PACKED_POST;

PACKED struct devprg_gpt_header
{
  uint8 signature[GPT_SIGNATURE_SIZE]; /* Should be "EFI PART" */
  uint32 revision;    /* Header revision number, this header is 1.0 */
  uint32 header_size; /* Size of this header in bytes. */
  uint32 header_crc32; /* CRC 32 of this header, set to zero for calculations*/
  uint32 reserved1;
  uint64 my_lba;  /* Logical Block Address of this header. */
  uint64 alternate_lba; /* Logical Block Address of alternate header. */
  uint64 first_usable_lba; /* Min LBA for any partition. */
  uint64 last_usable_lba; /* Max LBA for any partition. */
  struct devprg_guid disk_guid;
  uint64 partition_entry_lba; /* Partiton entry table LBA. */
  uint32 number_of_partition_entries; /* Number of entires in partition array*/
  uint32 size_of_partition_entry; /* Size of each partition entry. */
  uint32 partition_entries_crc32; /* CRC32 of partition entry array. */
  /* The rest of the block is reserved. The header size should be 92 bytes. */
} PACKED_POST;

/* Size of partition name in GPT */
#define DEVPRG_GPT_PARTITION_NAME_SIZE_BYTES  72
PACKED struct devprg_gpt_entry
{
  struct devprg_guid partition_type_guid;
  struct devprg_guid partition_guid; /* Each partition has its own GUID */
  uint64 starting_lba;
  uint64 ending_lba;
  uint64 attributes;
  uint8 partition_name[DEVPRG_GPT_PARTITION_NAME_SIZE_BYTES];
} PACKED_POST;



/* Read cache info. */
struct pparser_sector_cache
{
  struct devprg_storage *handle;
  uint64 block_count; /* Number of blocks in the device */

  uint8 *buffer; /* Points to the buffer that holds the logical block data. */
  uint32 buffer_size_blocks; /* Number of Logical Blocks that can fit in the
                                buffer. */
  uint32 cache_size_blocks; /* Number of blocks in the cache. */
  uint64 first_cached_lba; /* LBA of the first cached block in the buffer. */
  uint16 block_size_bytes; /* Size of a Logical Block in bytes */
};

static struct pparser_sector_cache _pparser_cache;


/* CRC32 calculation data and implementation. */
#define CRC32_SEED   0

static const uint32  crc32_table[256] = {
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
  0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
  0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
  0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
  0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
  0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
  0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
  0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
  0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
  0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
  0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
  0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
  0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
  0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
  0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
  0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
  0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
  0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
  0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
  0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
  0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
  0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/* Calculates CRC32 (Cyclic redundancy check) over the number of bytes
 * specified.
 *
 * param data   Buffer to use to calculate crc32
 * param nbytes Calculate CRC32 over this number of bytes.
 * param seed   CRC32 seed. Used to chain CRC32 calculations.
 *
 * return CRC32 over the buffer */
static uint32
gpt_calc_crc32 (const uint8 *data, uint32 nbytes, uint32 seed)
{
  uint32  crc = 0;
  uint32  pos;
  const uint8   *pbyte;

  if ((data != NULL) && (nbytes != 0))
  {
    crc = 0xFFFFFFFF ^ seed;
    for (pos = 0, pbyte = data; pos < nbytes; pos++, pbyte++)
    {
      crc = (crc >> 8) ^ crc32_table[(uint8) crc ^ *pbyte];
    }
    crc ^= 0xFFFFFFFF;
  }
  return  crc;
}

uint32
devprg_calc_crc32(const uint8 *data, uint32 nbytes)
{
  return gpt_calc_crc32(data, nbytes, CRC32_SEED);
}

static uint16
pparser_cache_get_block_size (struct pparser_sector_cache *data_cache)
{
  return data_cache->block_size_bytes;
}

static uint64
pparser_cache_get_block_count (struct pparser_sector_cache *data_cache)
{
  return data_cache->block_count;
}

/* Reads the number of sectors specified by blk_cnt into the cache. Reads start
 * from the given Logical Block Address (lba).*/
static uint8 *
pparser_cache_read (uint64 lba, struct pparser_sector_cache *data_cache,
                    uint32 blk_cnt)
{
  uint8 *buffer = data_cache->buffer;
  uint32 max_blk_cnt;
  uint64 dev_size_blocks;
  dp_res_t error;

  if (blk_cnt > data_cache->buffer_size_blocks)
    blk_cnt = data_cache->buffer_size_blocks;

  dev_size_blocks = pparser_cache_get_block_count (data_cache);
  /* The LBA should be less than the device size */
  if (lba >= dev_size_blocks)
    return NULL;

  max_blk_cnt = dev_size_blocks - lba;
  if (blk_cnt > max_blk_cnt)
    blk_cnt = max_blk_cnt;

  data_cache->cache_size_blocks = 0;
  error = devprg_storage_read (data_cache->handle, buffer, lba, blk_cnt);
  if (error != DEVPRG_SUCCESS)
    return NULL;

  data_cache->first_cached_lba = lba;
  data_cache->cache_size_blocks = blk_cnt;

  return buffer;
}


/* Initalizes the read cache. */
struct pparser_sector_cache *
pparser_cache_init (struct devprg_storage *handle, void *buffer,
                    size_t buf_size)
{
  struct pparser_sector_cache *data_cache = &_pparser_cache;
  uint8 *buf;
  uint32 buffer_size_blocks;
  uint64 block_cnt = 0;
  uint16 bytes_per_block = 0;

  struct devprg_storage_info dev_info;
  dp_res_t error = DEVPRG_SUCCESS;

  /* Get and validate device sizes. */
  error = devprg_storage_get_info(handle, &dev_info);
  if (error != DEVPRG_SUCCESS)
    return NULL;

  bytes_per_block = dev_info.block_size;
  block_cnt = dev_info.total_blocks;

  if (bytes_per_block == 0 || block_cnt == 0)
    return NULL;

  data_cache->handle = handle;
  data_cache->block_size_bytes = bytes_per_block;
  data_cache->block_count = block_cnt;
  data_cache->buffer = buffer;

  /* If the cache size in blocks is greater than the number of blocks in
   * the device, then use the device block count as the cache size.
   * We can't cache more sectors than what is in disk. */
  buffer_size_blocks = buf_size / bytes_per_block;
  if (buffer_size_blocks > block_cnt)
    buffer_size_blocks = block_cnt;
  data_cache->buffer_size_blocks = buffer_size_blocks;

  buf = pparser_cache_read (1ULL, data_cache, 1);
  if (buf == NULL)
    return NULL;

  return data_cache;
}

/* Checks if we have the LBA cached.
 * returns pointer to the buffer if we do, NULL if the lba is not cached. */
static uint8 *
pparser_read_cache_check (uint64 lba, struct pparser_sector_cache *data_cache)
{
  uint64 first_cached_lba = data_cache->first_cached_lba;

  if (first_cached_lba <= lba)
  {
    uint64 lba_offset = lba - first_cached_lba;
    uint64 cached_cnt = ((uint64) data_cache->cache_size_blocks) & UINT_MAX;
    if (cached_cnt > lba_offset)
    {
      uint8 *buffer = data_cache->buffer;
      /* we have the lba cached, retrurn it. */
      buffer += (lba_offset * data_cache->block_size_bytes);
      return buffer;
    }
  }

  return NULL;
}

static uint8 *
pparser_read_lba (uint64 lba, uint32 read_ahead_hint,
                  struct pparser_sector_cache *data_cache)
{
  uint8 *buffer;

  buffer = pparser_read_cache_check (lba, data_cache);

  /* if the lba is not cached, cache it */
  if (buffer == NULL)
  {
    buffer = pparser_cache_read (lba, data_cache, read_ahead_hint);
    if (buffer != NULL)
      buffer = pparser_read_cache_check (lba, data_cache);
  }

  return buffer;
}



/* Checks if a given gpt_entry is valid.
 * Checks that is doing:
 *    - Is the type GUID not zero.
 *
 * Returns 1 if the entry is valid, 0 if it is empty.
 */
static int
gpt_is_entry_valid (struct devprg_gpt_entry *gpt_entry)
{
  uint32 *buffer;
  uint32 size;
  int i;
  int result = 0;

  /* Check if Type GUID is zeros */
  buffer = (uint32 *)&gpt_entry->partition_type_guid.data1;
  size = sizeof (gpt_entry->partition_type_guid);
  for (i = size; i > 0; i -= sizeof (uint32))
  {
    if (*buffer != 0)
    {
      result = 1;
      break;
    }
    buffer++;
  }

  return result;
}

/* Fixes all the fields that are associated with disk size in the GPT.
 *
 * param handle    Handle to storage device.
 * param cache     Sector cache to speed up access.
 * param grow_last Indicate if to grow the last partition.
 *
 * return  DEVPRG_SUCCESS if GPT was properly fixed.
 */
static dp_res_t
gpt_fix_data (struct devprg_storage *handle,
              struct pparser_sector_cache *cache,
              int grow_last)
{
  struct devprg_gpt_header *gpt_header;
  uint8 *buffer;
  uint32 crc = CRC32_SEED;
  int result;
  uint64 entries_lba, current_lba;
  uint32 partition_entry_count;
  uint32 partition_entry_size;
  uint32 entry_array_size;
  uint32 entry_lba_count;
  uint32 dev_size_blocks;
  uint64 last_entry_array_lba, first_usable_lba, last_usable_lba;
  uint32 block_size, min_array_size, min_array_size_blocks;
  dp_res_t error = DEVPRG_SUCCESS;
  uint64 gpt_lba = 1; /* Primary should be at 1. */
  uint64 lba;

  buffer = pparser_read_lba (gpt_lba, 1, cache);
  if (buffer == NULL)
  {
    DP_LOGE("Can't header at %d for fixing GPT", gpt_lba);
    return DEVPRG_ERROR_STORAGE;
  }

  gpt_header = (struct devprg_gpt_header *) buffer;
  result = memcmp (gpt_header->signature, GPT_SIGNATURE, GPT_SIGNATURE_SIZE);
  if (result != 0)
  {
    DP_LOGE("Not a valid GPT signature at sector %d", gpt_lba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  /* There has to be at least 1 partition entry */
  partition_entry_count = gpt_header->number_of_partition_entries;
  if (partition_entry_count == 0)
  {
    DP_LOGE("GPT indcates there are no entries in table %d", gpt_lba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  block_size = pparser_cache_get_block_size (cache);
  partition_entry_size = gpt_header->size_of_partition_entry;
  if (partition_entry_size == 0 ||
      (partition_entry_size % GPT_PARTITION_ENTRY_BASE_SIZE_BYTES) != 0 ||
      partition_entry_size > block_size)
  {
    DP_LOGE("GPT Invalid size of entries %d blk_size %d",
            partition_entry_size, block_size);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  entry_array_size = partition_entry_count * partition_entry_size;
  entry_lba_count = ((entry_array_size - 1) / block_size) + 1;

  entries_lba = gpt_header->partition_entry_lba;
  if(gpt_lba == 1 && entries_lba != 2)
  {
    DP_LOGW("GPT primary table not at LBA 2, in %d", entries_lba);
  }
  last_entry_array_lba = entries_lba + entry_lba_count - 1;

  dev_size_blocks = pparser_cache_get_block_count (cache);
  /* Check for wrap around of last_entry_lba */
  if (last_entry_array_lba < gpt_header->partition_entry_lba ||
      last_entry_array_lba > dev_size_blocks)
  {
    DP_LOGE("GPT Last LBA for entry array is invalid %d", last_entry_array_lba);
    DP_LOGE("GPT Check Entry size %d entry_count %d and entry_lba %d",
            partition_entry_size, partition_entry_count, entries_lba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  current_lba = entries_lba;
  entry_array_size = partition_entry_count * partition_entry_size;
  /* Compute the first usable LBA, UEFI Spec says that a minimum of 16Kbytes
   should be reserved for the GPT entry array. */
  min_array_size = (entry_array_size >= 16384) ? entry_array_size : 16384;
  min_array_size_blocks = ((min_array_size - 1) / block_size) + 1;
  first_usable_lba = entries_lba + min_array_size_blocks;
  /* -1 for last LBA, -1 for GPT header*/
  last_usable_lba = dev_size_blocks - 1 - 1 - min_array_size_blocks;

  if (grow_last)
  {
    uint64 lba_partition = 0;
    uint64 lba_of_entry = 0;
    uint32 offset;
    /* First loop to find the partition entry that allocates the last
     * LBA in disk. */
    while (entry_array_size > 0)
    {
      uint32 crc_size;
      uint32 read_ahead;

      read_ahead = (uint32)(last_entry_array_lba - current_lba + 1);
      buffer = pparser_read_lba (current_lba, read_ahead, cache);
      if (buffer == NULL)
      {
        DP_LOGE("GPT Failed to read LBA %d", current_lba);
        return DEVPRG_ERROR_STORAGE;
      }
      /* Loop through each entry and find the one with the highest LBA */
      for(offset = 0; offset < block_size; offset+= partition_entry_size)
      {
        struct devprg_gpt_entry *gpt_entry;
        gpt_entry = (struct devprg_gpt_entry *)&buffer[offset];
        if (gpt_is_entry_valid(gpt_entry))
        {
          if(gpt_entry->starting_lba < first_usable_lba)
          {
            DP_LOGW("GPT entry start LBA %d before the fist usable lba %d",
                    gpt_entry->starting_lba, first_usable_lba);
          }
          /* Get the highest starting LBA to find the last partition. */
          if(lba_partition < gpt_entry->starting_lba)
          {
            lba_partition = gpt_entry->starting_lba;
            lba_of_entry = current_lba;
          }
        }
      }

      current_lba++;
      crc_size = MIN (block_size, entry_array_size);
      crc = gpt_calc_crc32 (buffer, crc_size, crc);
      entry_array_size -= crc_size;
    }
    /* We found a valid last partition, grow it to the end of disk. */
    if(lba_partition != 0)
    {
      buffer = pparser_read_lba(lba_of_entry, 1, cache);
      if(buffer == NULL)
      {
        DP_LOGE("GPT failed to read gpt table to grow partition lba %d",
                lba_of_entry);
      }
      else
      {
        /* Loop through each entry and find the one with the highest LBA */
        for(offset = 0; offset < block_size; offset+= partition_entry_size)
        {
          struct devprg_gpt_entry *gpt_entry;
          gpt_entry = (struct devprg_gpt_entry *)&buffer[offset];
          if (gpt_is_entry_valid(gpt_entry))
          {
            /* update the last partition with the highest ending LBA.*/
            if(lba_partition == gpt_entry->starting_lba)
            {
              if(gpt_entry->starting_lba > dev_size_blocks)
              {
                DP_LOGE("GPT Last partition LBA %d beyond end of disk %d",
                        gpt_entry->starting_lba, dev_size_blocks);
                return DEVPRG_ERROR_INVAL_PARAM;
              }
              gpt_entry->ending_lba = last_usable_lba;
            }
          }
        }

        error = devprg_storage_write(handle, buffer, lba_of_entry, 1);
        if (error != DEVPRG_SUCCESS)
        {
          DP_LOGE("GPT failed to grow last partition error %d lba %d",
                  error, lba_of_entry);
        }
      }
    }
  }

  /* Calculate CRC 32 over the partition entry array */
  crc = CRC32_SEED;
  current_lba = entries_lba;
  entry_array_size = partition_entry_count * partition_entry_size;
  while (entry_array_size > 0)
  {
    uint32 crc_size;
    uint32 read_ahead;

    read_ahead = (uint32)(last_entry_array_lba - current_lba + 1);
    buffer = pparser_read_lba (current_lba, read_ahead, cache);
    if (buffer == NULL)
    {
      DP_LOGE("GPT Failed to read LBA %d for crc", current_lba);
      return DEVPRG_ERROR_STORAGE;
    }

    current_lba++;
    crc_size = MIN (block_size, entry_array_size);
    crc = gpt_calc_crc32 (buffer, crc_size, crc);
    entry_array_size -= crc_size;
  }

  buffer = pparser_read_lba (gpt_lba, 1, cache);
  if (buffer == NULL)
  {
    DP_LOGE("GPT Can't re-read header!? %d", gpt_lba);
    return DEVPRG_ERROR_STORAGE;
  }

  gpt_header = (struct devprg_gpt_header *) buffer;

  /* Update the fields in the GPT header. */
  gpt_header->partition_entries_crc32 = crc;
  gpt_header->my_lba = gpt_lba;
  gpt_header->alternate_lba = dev_size_blocks - 1;
  gpt_header->first_usable_lba = first_usable_lba;
  gpt_header->last_usable_lba = last_usable_lba;

  gpt_header->header_crc32 = 0;

  /* The header should be 92 bytes. */
  crc = gpt_calc_crc32 (buffer, sizeof (struct devprg_gpt_header),CRC32_SEED);
  gpt_header->header_crc32 = crc;

  lba = gpt_header->my_lba;
  error = devprg_storage_write(handle, buffer, lba, 1);
  if (error != DEVPRG_SUCCESS)
  {
    DP_LOGE("GPT failed to write GPT header error %d lba %d",
            error, gpt_header->my_lba);
    return DEVPRG_ERROR_STORAGE;
  }

  /* Now fix the alternate */
  gpt_header->my_lba = gpt_header->alternate_lba;
  gpt_header->alternate_lba = 1;
  gpt_header->partition_entry_lba = gpt_header->my_lba - entry_lba_count;
  gpt_header->header_crc32 = 0;

  crc = gpt_calc_crc32 (buffer, sizeof (struct devprg_gpt_header),CRC32_SEED);
  gpt_header->header_crc32 = crc;

  lba = gpt_header->my_lba;
  error = devprg_storage_write(handle, buffer, lba, 1);
  if (error != DEVPRG_SUCCESS)
  {
    DP_LOGE("GPT failed to write alternate GPT header error %d lba %d",
            error, gpt_header->my_lba);
    return DEVPRG_ERROR_STORAGE;
  }

  /* copy the table from primary to alternate */
  current_lba = entries_lba;
  entry_array_size = partition_entry_count * partition_entry_size;
  entries_lba = gpt_header->partition_entry_lba;
  while (current_lba < last_entry_array_lba)
  {
    uint32 read_ahead;
    uint64 lba, lba_count = 0;

    read_ahead = (uint32)(last_entry_array_lba - current_lba + 1);
    buffer = pparser_read_lba (current_lba, read_ahead, cache);
    if (buffer == NULL)
    {
      DP_LOGE("GPT Failed to re-read LBA %d", current_lba);
      return DEVPRG_ERROR_STORAGE;
    }

    for(lba = current_lba; lba < last_entry_array_lba; lba++)
    {
      if(pparser_read_cache_check (lba, cache) != NULL)
        lba_count++;
    }

    error = devprg_storage_write(handle, buffer, entries_lba, lba_count);
    if (error != DEVPRG_SUCCESS)
    {
      DP_LOGE("GPT failed to write alternate GPT table error %d lba %d %d",
              error, current_lba, lba_count);
      return DEVPRG_ERROR_STORAGE;
    }

    current_lba += lba_count;
    entries_lba += lba_count;
  }

  return DEVPRG_SUCCESS;
}


/* Updates the values in the GPT header that depend on device size.
 *
 * @param handle   Storage handle for the device.
 * @param grow_last_partition   If set to 1, grow the last partition
 *                              to cover the whole device.
 *
 */
dp_res_t devprg_fix_gpt(struct devprg_storage *handle,
                        int grow_last_partition)
{
  char *buffer;
  size_t buf_size;
  dp_res_t error = DEVPRG_ERROR_STORAGE;

  struct pparser_sector_cache *pparser_cache;

  buffer = devprg_get_transfer_buffer(&buf_size);
  if (buffer==NULL)
  {
    DP_LOGE("devprg_get_transfer_buffer returned null %d", 0);
    return DEVPRG_ERROR_STORAGE;
  }

  pparser_cache = pparser_cache_init (handle, buffer, buf_size);
  if(pparser_cache == NULL)
  {
    DP_LOGE("Failed to initialize the GPT read routines %d", 0);
    error = DEVPRG_ERROR_STORAGE;
    goto cleanup;
  }

  error = gpt_fix_data (handle, pparser_cache, grow_last_partition);
  if(error != DEVPRG_SUCCESS)
  {
    DP_LOGE("Failed to fix GPT %d", error);
    goto cleanup;
  }

cleanup:
  devprg_free_transfer_buffer(buffer);
  return error;
}
