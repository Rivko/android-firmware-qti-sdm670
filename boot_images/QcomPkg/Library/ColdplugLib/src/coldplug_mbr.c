/***********************************************************************
 * Coldplug MBR module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug MBR module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_mbr.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_mbr.h"
#include "coldplug_assert.h"

int check_mbr_signature(struct coldplug_pcache_data *data_cache)
{
	struct coldplug_mbr_layout *mbr;
	uint8 *buffer;
	uint32 start_lba_list[4] = {0}; /* atmost 4 records in sector 0 */
	uint8 valid_record_count = 0;
	uint32 total_card_block_count = 0;
	uint8 i, j;

	total_card_block_count = coldplug_pcache_get_block_count(data_cache);

	buffer = coldplug_pcache_read(0ULL, 1, data_cache);
	if (buffer == NULL)
		return -1;

	mbr = (struct coldplug_mbr_layout *) buffer;
	if (mbr->signature[0] != 0x55 || mbr->signature[1] != 0xAA)
		return -1;

	/* Check if this looks like a FAT boot Sector. If this is an FBS the
	 * sector size in bytes 11 and 12 should be 512 i.e. 0x200 but an MBR
	 * could also have the same values at these bytes so this check is
	 * more like a hint to determine the need for further checks. */
	if (mbr->boot_code[11] == 0x00 && mbr->boot_code[12] == 0x02) {

		/* This could be an FBS so performing additional checks.
		 * Check for alteast one valid record in the 4 slots. */

		for (i = 0; i < 4; i++) {
			if ((mbr->records[i].partition_type != 0) &&
				(mbr->records[i].size_in_blocks > 0)) {
				/* Start LBA cannot be zero and should be
				 * within bounds for a valid record in an
				 * MBR or it isnt an MBR. */
				if ((mbr->records[i].start_lba == 0) ||
					(mbr->records[i].start_lba >
						total_card_block_count)) {
					return -1;
				}

				/* Proper record found so put it in the list*/
				start_lba_list[valid_record_count++] =
					mbr->records[i].start_lba;
			}
		}

		/* If no valid record is found then it is not an MBR. return */
		if (valid_record_count == 0)
			return -1;

		/* Check that the start LBA of all valid records are unique. */
		for (i = 0; i < (valid_record_count - 1); i++) {
			for (j = (i + 1); j < valid_record_count; j++) {
				if (start_lba_list[i] == start_lba_list[j]) {
					return -1;
				}
			}
		}
	}


	/* Avoid warnings. */
	(void) mbr->disk_signature;
	(void) mbr->pad;
	(void) mbr->records[0].bootable;
	(void) mbr->records[0].start_CHS;
	(void) mbr->records[0].end_CHS;

	return 0;
}

/* Checks if the protective MBR required for GPT exists.
 *
 * This is a relaxed check since we don't verify all the fields in the
 * partition table, it only checks if the GPT partition type exists.
 * @return 0 if MBR is a valid protective MBR, -1 if not valid protective MBR.
 */
int gpt_check_protective_mbr(struct coldplug_pcache_data *data_cache,
				uint32 *total_lba)
{
	int i;
	struct coldplug_mbr_layout *mbr;
	uint8 *buffer;

	buffer = coldplug_pcache_read(0ULL, 1, data_cache);
	if (buffer == NULL)
	return -1;

	mbr = (struct coldplug_mbr_layout *) buffer;

	/* Go through all the partition entries in the MBR searching for
	* partition type 0xEE. This is a relaxed check for the protective MBR
	* since we are not checking if the size of the partition is correct
	* and all  other partition entries are empty. */
	for (i = 0; i < 4; i++)	{
		if (mbr->records[i].partition_type ==
			COLDPLUG_PARTITION_ID_GPT) {
			*total_lba = mbr->records[i].size_in_blocks;
			return 0;
		}
	}

	return -1;
}

struct coldplug_mbr_partition_data {
	uint8 bootable;
	uint8 partition_type;
	uint32 start_lba;
	uint32 size_in_blocks;
};


struct _coldplug_mbr_iter {
	uint32 next_ext_sector;
	uint32 start_ext_partition;
	uint8 parsing_extended; /* Boolean, true when parsing, flase when done
			      parsing or invalid extended partition. */

	uint8  current_index; /* Wich of the 4 MBR entries are we parsing. */
	uint8  next_index; /* Wich of the 4 MBR entries are we parsing next. */
	uint64 mbr_sector; /* Sector we are currently parsing */

	struct coldplug_mbr_partition_data partition_data;

	struct coldplug_pcache_data *data_cache; /* Holds the raw data from 
					sectors read from the device. */
};

static struct _coldplug_mbr_iter _mbr_iter;

struct _coldplug_mbr_iter* coldplug_mbr_parse_open(
		struct coldplug_pcache_data *data_cache)
{
	struct _coldplug_mbr_iter *mbr_iter;
	int result;

	mbr_iter = &_mbr_iter;
	mbr_iter->data_cache = data_cache;

	/* Check if there is a valid MBR. */
	result = check_mbr_signature(mbr_iter->data_cache);

	if (result != 0)
		return NULL;

	mbr_iter->mbr_sector = 0;
	mbr_iter->current_index = 0;
	mbr_iter->next_index = 0;

	return mbr_iter;
}

struct coldplug_mbr_partition_data* coldplug_mbr_read_primary(
		struct _coldplug_mbr_iter *mbr_iter)
{
	uint8 *buffer;
	struct coldplug_mbr_layout *mbr;
	struct coldplug_mbr_partition_record *precord;
	uint32 index;

	if (mbr_iter == NULL)
		return NULL;

	/* Check if we have parsed all the partitions. */
	if (mbr_iter->next_index >= 4)
		return NULL;

	buffer = coldplug_pcache_read(mbr_iter->mbr_sector, 1,
					mbr_iter->data_cache);
	if (buffer == NULL)
		return NULL;

	mbr = (struct coldplug_mbr_layout *)buffer;

	index = mbr_iter->next_index;

	precord = &mbr->records[index];
	while (precord->partition_type == 0) {
		index++;
		/* Check if we have parsed all the primary partitions. */
		if (index >= 4)
			return NULL;
		precord = &mbr->records[index];
	}

	mbr_iter->current_index = index;
	mbr_iter->next_index = index + 1;

	/* Copy the data over from the partition record. */
	mbr_iter->partition_data.bootable = precord->bootable;
	mbr_iter->partition_data.partition_type = precord->partition_type;
	mbr_iter->partition_data.start_lba = precord->start_lba;
	mbr_iter->partition_data.size_in_blocks = precord->size_in_blocks;

	return &mbr_iter->partition_data;
}

int coldplug_mbr_check_extended(struct _coldplug_mbr_iter *mbr_iter)
{
	uint8 *buffer;
	struct coldplug_mbr_layout *mbr;
	struct coldplug_mbr_partition_record *precord;
	uint32 index;

	if (mbr_iter == NULL)
		return -1;

	mbr_iter->parsing_extended = 0;

	/* Check if we have parsed all the partitions. */
	if (mbr_iter->current_index >= 4)
		return -1;

	buffer = coldplug_pcache_read(mbr_iter->mbr_sector, 1,
					mbr_iter->data_cache);
	if (buffer == NULL)
		return -1;

	mbr = (struct coldplug_mbr_layout *)buffer;

	index = mbr_iter->current_index;
	precord = &mbr->records[index];

	/* if this is not an extended partition, return error. */
	if (precord->partition_type != COLDPLUG_MBR_DOS_EXTENDED_PARTITION &&
	    precord->partition_type != COLDPLUG_MBR_WIN_EXTENDED_PARTITION)
		return -1;

	/* set up the iterator to iterate these extended partitions. */
	mbr_iter->next_ext_sector = precord->start_lba;
	mbr_iter->start_ext_partition = precord->start_lba;
	mbr_iter->parsing_extended = 1;

	return 0;
}

struct coldplug_mbr_partition_data* coldplug_mbr_read_extended(
		struct _coldplug_mbr_iter *mbr_iter)
{
	uint8 *buffer;
	struct coldplug_mbr_layout *ebr;
	struct coldplug_mbr_partition_record *precord;

	if (mbr_iter == NULL)
		return NULL;

	/* Check if we are parsing extended partitions. */
	if (mbr_iter->parsing_extended == 0)
		return NULL;

	buffer = coldplug_pcache_read((uint64) mbr_iter->next_ext_sector, 1,
			     mbr_iter->data_cache);
	if (buffer == NULL)
		goto done_parsing_extended;

	ebr = (struct coldplug_mbr_layout *)buffer;
	/* Check if valid Extended Boot Record (EBR) */
	if (ebr->signature[0] != 0x55 || ebr->signature[1] != 0xAA)
		goto done_parsing_extended;

	/* Validate the partition record */
	precord = &ebr->records[0];
	if (precord->partition_type == 0 || precord->size_in_blocks == 0)
		goto done_parsing_extended;

	/* Copy the data over from the partition record. */
	mbr_iter->partition_data.bootable = precord->bootable;
	mbr_iter->partition_data.partition_type = precord->partition_type;
	mbr_iter->partition_data.start_lba = precord->start_lba +
				       mbr_iter->next_ext_sector;
	mbr_iter->partition_data.size_in_blocks = precord->size_in_blocks;

	/* Setup the next extended partition record. */
	precord = &ebr->records[1];

	/* If the next partition record is empty, quit on next call. */
	if (precord->size_in_blocks == 0 ||
	    precord->partition_type != COLDPLUG_MBR_DOS_EXTENDED_PARTITION)
		mbr_iter->parsing_extended = 0;
	else
		mbr_iter->next_ext_sector = precord->start_lba +
					mbr_iter->start_ext_partition;

	return &mbr_iter->partition_data;

done_parsing_extended:
	mbr_iter->parsing_extended = 0;
	return NULL;
}


int coldplug_mbr_parse_close(struct _coldplug_mbr_iter *mbr_iter)
{
	if (mbr_iter == NULL)
		return -1;

	return 0;
}

static int coldplug_add_mbr_partition(struct coldplug_lun *clun,
		struct coldplug_mbr_partition_data *mbr_entry,
		uint32 partition_num)
{
	struct coldplug_partition *parti;
	int result = -1;

	parti = coldplug_partition_alloc(clun);
	if (parti == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto End;
	}

	parti->is_bootable = mbr_entry->bootable;
	parti->parti_info.partition_type = COLDPLUG_PARTITION_TYPE_MBR;
	parti->parti_info.u.mbr.mbr_id = mbr_entry->partition_type;
	parti->start_lba = mbr_entry->start_lba;
	parti->total_lba = mbr_entry->size_in_blocks;

	result = 0;

End:
	return result;
}

int coldplug_mbr_parse(struct coldplug_lun *clun,
		struct coldplug_pcache_data *data_cache)
{
	struct _coldplug_mbr_iter *mbr_iter = NULL;
	struct coldplug_mbr_partition_data *mbr_entry;
	uint32 parti_num = 0;
	uint32 parti_count = 0;
	int result = -1;

	mbr_iter = coldplug_mbr_parse_open(data_cache);
	if (mbr_iter == NULL)
		goto End; /* If no MBR was found, return error. */

	while ((mbr_entry = coldplug_mbr_read_primary(mbr_iter)) != NULL) {

		result = coldplug_add_mbr_partition(clun, mbr_entry,
						parti_num);
		if (result != 0)
			break;

		++parti_num;
		++parti_count;

		/* If this is an extended partition, parse all the
		 * extended entries. */
		if (coldplug_mbr_check_extended(mbr_iter) == 0) {
			int ext_num = 0;

			while ((mbr_entry =
			  coldplug_mbr_read_extended(mbr_iter)) != NULL) {
				if (parti_count >=
					COLDPLUG_MAX_PARTITIONS_PER_LUN)
					break;

				result = coldplug_add_mbr_partition(clun,
						mbr_entry, ext_num);
				if (result != 0)
					break;

				++ext_num;
				++parti_count;
			}
		}

		if (parti_count >= COLDPLUG_MAX_PARTITIONS_PER_LUN)
			break;
	}

	result = 0;
	coldplug_mbr_parse_close(mbr_iter);

End:
	return result;
}

struct coldplug_handle* coldplug_open_mbr_partition_i(
		enum coldplug_device_type dev_type,
		int is_embedded_device, uint32 mbr_id)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_partition *cparti = NULL;
	struct coldplug_handle *chdl = NULL;
	uint32 i;

	for ( i = 0; i < COLDPLUG_MAX_PARTITIONS; ++i)
	{
		cparti = &coldplug_partitions[i];
		if (!cparti->is_in_use)
			continue;

		if (cparti->parti_info.partition_type !=
			COLDPLUG_PARTITION_TYPE_MBR)
			continue;

		cslot = cparti->clun->cslot;
		cdev = cslot->cdev;

		if ((cdev->device_type == dev_type) &&
		    (cslot->is_embedded_device == is_embedded_device)) {
			if (mbr_id == cparti->parti_info.u.mbr.mbr_id) {
				chdl = coldplug_handle_alloc(cparti);
				return chdl;
			}
		}
	}

	return NULL;
}


int coldplug_mbr_partition_is_fat(struct coldplug_partition *cparti)
{
	int is_fat = 0;

	coldplug_partition_validate(cparti);

	COLDPLUG_ASSERT(cparti->parti_info.partition_type ==
		   COLDPLUG_PARTITION_TYPE_MBR);

	switch (cparti->parti_info.u.mbr.mbr_id)
	{
	/* All possible fat partition types */
	case 0x01:
	case 0x04:
	case 0x06:
	case 0x0B:
	case 0x0C:
	case 0x0E:
		is_fat = 1;
		break;

	default:
		break;
	}

	return is_fat;
}
