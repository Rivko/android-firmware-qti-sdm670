/***********************************************************************
 * Coldplug GPT module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug GPT module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_gpt.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_gpt.h"
#include "coldplug_mbr.h"
#include "coldplug_assert.h"
#include "coldplug_util.h"
#include "coldplug_device.h"
#include "stringl/stringl.h"

#include <string.h>

#define GPT_SIGNATURE "EFI PART"
#define GPT_SIGNATURE_SIZE (8)

/* The UEFI Specification indicates that the partition entry size is a
 * multiple of 128. */
#define GPT_PARTITION_ENTRY_BASE_SIZE_BYTES 128

/* Size of partition name in GPT */
#define COLDPLUG_GPT_PARTITION_NAME_SIZE_BYTES  72

static const struct coldplug_guid COLDPLUG_GPT_GUID_DATA_PARTITION =
{0xEBD0A0A2, 0xB9E5, 0x4433, {0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7}};


PACKED struct coldplug_gpt_header {
	uint8 signature[GPT_SIGNATURE_SIZE];
	uint32 revision;
	uint32 header_size;
	uint32 header_crc32;
	uint32 reserved1;
	uint64 my_lba;
	uint64 alternate_lba;
	uint64 first_usable_lba;
	uint64 last_usable_lba;
	struct coldplug_guid disk_guid;
	uint64 partition_entry_lba;
	uint32 number_of_partition_entries;
	uint32 size_of_partition_entry;
	uint32 partition_entries_crc32;
} PACKED_POST;

PACKED struct coldplug_gpt_partition {
	struct coldplug_guid type_guid;
	struct coldplug_guid unique_guid;
	uint64 start_lba;
	uint64 end_lba;
	uint64 attributes;
	uint8 partition_name[COLDPLUG_GPT_PARTITION_NAME_SIZE_BYTES];
} PACKED_POST;

struct _coldplug_gpt_iter {
	uint32 block_size_bytes;

	uint32 remaining_entries; 
	uint64 curr_entry_array_lba;
	uint64 last_entry_array_lba;
	uint32 size_of_partition_entry;
	uint32 current_entry_offset;
	struct coldplug_pcache_data *data_cache;
};

/* _gpt_iter is used to return a iter handle to the caller */
static struct _coldplug_gpt_iter _gpt_iter;


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
static uint32 gpt_calc_crc32(const uint8 *data, uint32 nbytes, uint32 seed)
{
	uint32  crc = 0;
	uint32  pos;
	const uint8   *pbyte;

	if ((data != NULL) && (nbytes != 0)) {
		crc = 0xFFFFFFFF ^ seed;
		for (pos = 0, pbyte = data; pos < nbytes; pos++, pbyte++) {
			crc = (crc >> 8) ^ crc32_table[(uint8) crc ^ *pbyte];
		}
		crc ^= 0xFFFFFFFF;
	}
	return  crc;
}


/* These set of functions will swap from little endian to host, ARM is little
 * endian, so no need to implement them for now. */
static uint32 little_to_host32(uint32 value)
{
	return value;
}

static uint64 little_to_host64(uint64 value)
{
	return value;
}

static void gpt_header_little_to_host(struct coldplug_gpt_header *header_in,
			struct coldplug_gpt_header *header_out)
{
	/* The current architecture is little endian, data can be read directly
	* from disk with no conversion. If we ever need to compile this module
	* for big endian it should be modified to translate the input gpt
	* header to the output gpt header. */
	(void) header_out;
	(void) little_to_host32(header_in->revision);
	(void) little_to_host32(header_in->header_size);
	(void) little_to_host32(header_in->header_crc32);
	(void) little_to_host32(header_in->reserved1);
	(void) little_to_host64(header_in->my_lba);
	(void) little_to_host64(header_in->alternate_lba);
	(void) little_to_host64(header_in->first_usable_lba);
	(void) little_to_host64(header_in->last_usable_lba);
	(void) little_to_host64(header_in->partition_entry_lba);
	(void) little_to_host32(header_in->number_of_partition_entries);
	(void) little_to_host32(header_in->size_of_partition_entry);
	(void) little_to_host32(header_in->partition_entries_crc32);

	(void) header_in->disk_guid;
}

static void gpt_partition_entry_little_to_host(
		struct coldplug_gpt_partition *entry_in,
		struct coldplug_gpt_partition *entry_out)
{
	(void) entry_out;
	(void) little_to_host64(entry_in->start_lba);
	(void) little_to_host64(entry_in->end_lba);
	(void) little_to_host64(entry_in->attributes);
}


/* Does all the checks necessary to verify GPT is valid. It will udpate the
 * iterator structure to the appropiate values to parse the partitiont table
 *
 * param gpt_lba     LBA of the GPT Header.
 * param alternate_gpt_lba [out] Return value of the LBA of the alternate
 *                         GPT header according to the header at gpt_lba.
 * param gpt_iter    Iterator structure used by the GPT parser.
 *
 * return 0 if a valid GPT header is found, -1 if it failed to find a header.
 */
static int gpt_check_header(uint64 gpt_lba, uint64 *alternate_gpt_lba,
			struct _coldplug_gpt_iter *gpt_iter)
{
	struct coldplug_gpt_header *gpt_header;
	uint8 *buffer;
	uint32 original_crc;
	uint32 crc;
	int result;
	uint64 entries_lba;
	uint32 partition_entry_count;
	uint32 partition_entry_size_bytes;
	uint32 entry_array_size_bytes;
	uint32 entry_lba_count;
	uint32 dev_size_blocks;

	buffer = coldplug_pcache_read(gpt_lba, 1, gpt_iter->data_cache);
	if (buffer == NULL)
		return -1;

	gpt_header = (struct coldplug_gpt_header *) buffer;
	result = memcmp(gpt_header->signature, GPT_SIGNATURE,
				GPT_SIGNATURE_SIZE);
	if (result != 0)
		return -1;

	original_crc = gpt_header->header_crc32;
	gpt_header->header_crc32 = 0;
	/* The header should be 92 bytes. */
	ASSERT(sizeof(struct coldplug_gpt_header) == 92);
	crc = gpt_calc_crc32(buffer, sizeof(struct coldplug_gpt_header),
				CRC32_SEED);
	/* Return the buffer to the original state. */
	gpt_header->header_crc32 = original_crc;
	if (crc != original_crc)
		return -1;

	/* For now this function does nothing.*/
	gpt_header_little_to_host(gpt_header, NULL);
	if (gpt_lba != gpt_header->my_lba)
		return -1;

	*alternate_gpt_lba = gpt_header->alternate_lba;

	/* There has to be at least 1 partition entry */
	partition_entry_count = gpt_header->number_of_partition_entries;
	if (partition_entry_count == 0)
		return -1;

	partition_entry_size_bytes = gpt_header->size_of_partition_entry;
	if (partition_entry_size_bytes == 0 ||
		(partition_entry_size_bytes %
		GPT_PARTITION_ENTRY_BASE_SIZE_BYTES) != 0)
		return -1;

	entry_array_size_bytes = (partition_entry_count *
				partition_entry_size_bytes);
	entry_lba_count =
	((entry_array_size_bytes - 1) / gpt_iter->block_size_bytes) + 1;
	entries_lba = gpt_header->partition_entry_lba;

	/* store the imporant values in our iterator structure */
	gpt_iter->remaining_entries = gpt_header->number_of_partition_entries;
	gpt_iter->current_entry_offset = 0;
	gpt_iter->size_of_partition_entry =
		gpt_header->size_of_partition_entry;
	gpt_iter->curr_entry_array_lba =
		gpt_header->partition_entry_lba;
	gpt_iter->last_entry_array_lba =
		(gpt_header->partition_entry_lba + entry_lba_count - 1);
	dev_size_blocks = coldplug_pcache_get_block_count(gpt_iter->data_cache);
	/* Check for wrap around of last_entry_lba */
	if (gpt_iter->last_entry_array_lba < gpt_header->partition_entry_lba ||
			gpt_iter->last_entry_array_lba > dev_size_blocks)
		return -1;

	/* Calculate CRC 32 over the partition entry array */
	original_crc = gpt_header->partition_entries_crc32;
	crc = CRC32_SEED;
	result = -1;

	while (entry_array_size_bytes > 0) {
		uint32 crc_size;
		uint32 read_ahead;

		read_ahead = (uint32) 
			(gpt_iter->last_entry_array_lba - entries_lba + 1);
		buffer = coldplug_pcache_read(entries_lba, read_ahead,
				gpt_iter->data_cache);
		if (buffer == NULL)
			break;

		entries_lba++;
		crc_size = MIN(gpt_iter->block_size_bytes,
				entry_array_size_bytes);
		crc = gpt_calc_crc32(buffer, crc_size, crc);
		entry_array_size_bytes -= crc_size;
	}

	if (crc == original_crc)
		result = 0;

	return result;
}


static struct _coldplug_gpt_iter* coldplug_gpt_parse_open(
		struct coldplug_lun *clun,
		struct coldplug_pcache_data *data_cache)
{
	int result = 0;
	int mbr_sig;
	struct _coldplug_gpt_iter *gpt_iter;
	uint32 total_lba = 0;

	/* The read portion of the gpt iterator saves memory space by casting
	* and returning the read buffer to the caller. This is OK because the
	* gpt entry is less than 128 bytes; therefore we are guaranteed to
	* always have 128 bytes worth of data in the buffer. If the size of
	* the GPT entry structure changes (e.g. new UEFI Specifications) we
	* have to handle the cases when parsing the GPT entry table the
	* entry is split across sectors. For example, GPT entry size is 378,
	* the second entry in the table will have the data split across
	* sectors. The typical solution is to add an instance of struct
	* coldplug_gpt_partition into the struct _coldplug_gpt_iter, during
	* reads this  structure is populated and returned to the caller.
	* This is done in efs for directory iterators. */
	ASSERT(sizeof(struct coldplug_gpt_partition) == 128);

	gpt_iter = &_gpt_iter;
	/* Initialize the read cache. Failure means device read failed. */
	gpt_iter->data_cache = data_cache;

	mbr_sig = check_mbr_signature(gpt_iter->data_cache);
	/* If there is a valid MBR, it must be a protective MBR to be GPT. */
	if (mbr_sig == 0)
		result = gpt_check_protective_mbr(gpt_iter->data_cache,
							&total_lba);

	if (result == 0) {
		uint64 alternate_gpt_lba, gpt_lba;

		if (clun->is_boot_lun) {
			clun->total_lba = total_lba;
			data_cache->block_count = total_lba;
		}

		gpt_iter->block_size_bytes =
		coldplug_pcache_get_block_size(gpt_iter->data_cache);

		/* Check the primary GPT */
		gpt_lba = 1;
		result = gpt_check_header(gpt_lba, &alternate_gpt_lba,
						gpt_iter);

		/* If the primary GPT is bad, check the last sector
		* in the device for a valid GPT. */
		if (result != 0) {
			uint32 block_cnt;
			block_cnt = coldplug_pcache_get_block_count(
						gpt_iter->data_cache);

			gpt_lba = block_cnt - 1;
			result = gpt_check_header(gpt_lba,
					&alternate_gpt_lba, gpt_iter);
		}
	}

	if (result != 0)
		gpt_iter = NULL;

	return gpt_iter;
}

/* Checks if a given gpt_entry is valid.
 * Checks that is doing:
 *    - Is the type GUID not zero. */
static int gpt_is_entry_valid(struct coldplug_gpt_partition *gpt_entry)
{
	uint32 *buffer;
	uint32 size;
	int i;
	int result = 0;

	/* Check if Type GUID is zeros */
	buffer = (uint32 *) &gpt_entry->type_guid.data1;
	size = sizeof(gpt_entry->type_guid);
	for (i = size; i > 0; i -= sizeof(uint32)) {
		if (*buffer != 0) {
			result = -1;
			break;
		}
		buffer++;
	}

	return result;
}

/* Returns the current partition entry pointed by the iterator and
 * sets up the iterator status fileds to point to the next entry. */
static struct coldplug_gpt_partition*
gpt_get_next_partition_entry(struct _coldplug_gpt_iter *gpt_iter)
{
	uint8 *buffer;
	struct coldplug_gpt_partition *gpt_entry;
	uint32 offset;
	uint32 read_ahead;

	/* Check if we are done. */
	if (gpt_iter->remaining_entries == 0)
		return NULL;

	read_ahead = (uint32) (gpt_iter->last_entry_array_lba -
			gpt_iter->curr_entry_array_lba + 1);
	buffer = coldplug_pcache_read(gpt_iter->curr_entry_array_lba,
				read_ahead, gpt_iter->data_cache);
	/* this is very strange since this values should be valid */
	if (buffer == NULL)
		return NULL;

	offset = gpt_iter->current_entry_offset;
	gpt_entry = (struct coldplug_gpt_partition *) (buffer + offset);

	/* Entry succesfully read, prepare for next entry. */
	gpt_iter->remaining_entries -= 1;
	gpt_iter->current_entry_offset += gpt_iter->size_of_partition_entry;

	if (gpt_iter->current_entry_offset >= gpt_iter->block_size_bytes) {
		gpt_iter->curr_entry_array_lba +=
		(gpt_iter->current_entry_offset / gpt_iter->block_size_bytes);

		gpt_iter->current_entry_offset =
		(gpt_iter->current_entry_offset % gpt_iter->block_size_bytes);
	}

	return gpt_entry;
}

struct coldplug_gpt_partition* coldplug_gpt_parse_read(
		struct _coldplug_gpt_iter *gpt_iter)
{
	struct coldplug_gpt_partition *gpt_entry;
	int result;

	if (gpt_iter == NULL)
		return NULL;

	do {
		gpt_entry = gpt_get_next_partition_entry(gpt_iter);
		if (gpt_entry == NULL)
			break;

		/* Check if this entry is used by checking the type-guid
		 * is not zero. */
		result = gpt_is_entry_valid(gpt_entry);
	} while (result == 0);

	if (gpt_entry != NULL) {
		/* For now this function does nothing.*/
		gpt_partition_entry_little_to_host(gpt_entry, NULL);
	}

	return gpt_entry;
}

int coldplug_gpt_parse_close(struct _coldplug_gpt_iter *gpt_iter)
{
	if (gpt_iter == NULL)
		return -1;

	/* No cleanup needed at this time, since we have only one iterator.*/
	return 0;
}

/* Adds a Soft GPT partition to the link list. */
int coldplug_add_gpt_partition(struct coldplug_lun *clun,
	const struct coldplug_gpt_partition *gpt_entry,
	uint32 partition_num)
{
	struct coldplug_partition *parti;
	int result = -1;

	parti = coldplug_partition_alloc(clun);
	if (parti == NULL)
		goto End;

	/* Bit 2 in GPT partition entry attribute is bootable. */
	parti->is_bootable = (gpt_entry->attributes >> 2) & 0x01;

	parti->parti_info.partition_type = COLDPLUG_PARTITION_TYPE_GPT;
	coldplug_memscpy(&parti->parti_info.u.gpt.type_guid,
			sizeof(parti->parti_info.u.gpt.type_guid),
			&gpt_entry->type_guid,
			sizeof(gpt_entry->type_guid));

#ifdef COLDPLUG_GPT_ALL_PARTITION_FIELDS_IN_RAM
	coldplug_memscpy(&parti->parti_info.u.gpt.unique_guid,
			sizeof(parti->parti_info.u.gpt.unique_guid),
			&gpt_entry->unique_guid,
			sizeof(gpt_entry->unique_guid));

	parti->parti_info.u.gpt.start_lba = gpt_entry->start_lba;
	parti->parti_info.u.gpt.end_lba = gpt_entry->end_lba;
	parti->parti_info.u.gpt.attributes = gpt_entry->attributes;

	strlcpy((char *)parti->parti_info.u.gpt.partition_name,
		(char *)gpt_entry->partition_name,
		sizeof(parti->parti_info.u.gpt.partition_name));
#endif /* COLDPLUG_ALL_GPT_PARTITION_FIELDS */

	parti->start_lba = (uint32) gpt_entry->start_lba;
	parti->total_lba = 1 +
		((uint32) (gpt_entry->end_lba - gpt_entry->start_lba));

	result = 0;

End:
	return result;
}


int coldplug_gpt_parse(struct coldplug_lun *clun,
		struct coldplug_pcache_data *data_cache)
{
	struct _coldplug_gpt_iter *gpt_iter;
	struct coldplug_gpt_partition *gpt_entry;
	uint32 parti_num = 1;
	uint32 parti_count = 0;
	int result = -1;

	gpt_iter = coldplug_gpt_parse_open(clun, data_cache);
	if (gpt_iter == NULL)
		goto End; /* If no GPT was found return error. */

	while ((gpt_entry = coldplug_gpt_parse_read(gpt_iter)) != NULL) {
		result = coldplug_add_gpt_partition(clun,
					gpt_entry, parti_num);
		if (result != 0)
			break;

		parti_num++;
		parti_count++;
		if (parti_count >= COLDPLUG_MAX_PARTITIONS_PER_LUN)
			break;
	}

	coldplug_gpt_parse_close(gpt_iter);

	result = 0;

End:
	return result;
}

struct coldplug_handle* coldplug_open_gpt_partition_i(
		enum coldplug_device_type dev_type,
		int is_embedded_device, const struct coldplug_guid *guid)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_partition *cparti = NULL;
	struct coldplug_handle *chdl = NULL;
	int result;
	uint32 i;

	for ( i = 0; i < COLDPLUG_MAX_PARTITIONS; ++i) {
		cparti = &coldplug_partitions[i];
		if (!cparti->is_in_use)
			continue;

		if (cparti->parti_info.partition_type !=
				COLDPLUG_PARTITION_TYPE_GPT)
			continue;

		cslot = cparti->clun->cslot;
		cdev = cparti->clun->cslot->cdev;

		if ((cdev->device_type == dev_type) &&
		    (cslot->is_embedded_device == is_embedded_device)) {
			result = memcmp(&cparti->parti_info.u.gpt.type_guid,
					guid, sizeof(*guid));
			if (result == 0) {
				chdl = coldplug_handle_alloc(cparti);
				return chdl;
			}
		}
	}

	return NULL;
}

int coldplug_gpt_partition_is_fat(struct coldplug_partition *cparti)
{
	int is_fat = 0, result;

	coldplug_partition_validate(cparti);

	COLDPLUG_ASSERT(cparti->parti_info.partition_type ==
				COLDPLUG_PARTITION_TYPE_GPT);

	result = memcmp(&cparti->parti_info.u.gpt.type_guid,
			&COLDPLUG_GPT_GUID_DATA_PARTITION,
			sizeof(cparti->parti_info.u.gpt.type_guid));

	is_fat = (result == 0) ? 1 : 0;

	return is_fat;
}
