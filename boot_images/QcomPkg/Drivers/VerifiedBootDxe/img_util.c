/** @file
                        img_util.c

  Utility image functions (read/write a partiton and ASCII to Wild character convertion of image name)

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 07/28/16   SA     Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIVerifiedBoot.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Protocol/EFIVerifiedBoot.h>
#include "QcomLib.h"
#include "img_util.h"

#define PAGE_SIZE 4096
#define EFI_FAILURE (80)

/**

	Convert ascii charcater to wild character.

	@param[in]      *source 		Source string.
	@param[out]     *dest           Result string.
	@param[in]      len   	        size of Source.

    @retval         ret             Indicates whetehr convertion was successful.

**/
EFI_STATUS convert_char8_to_char16(UINT8 *source, UINT16 *dest, UINTN len)
{
  EFI_STATUS ret = EFI_FAILURE;
  UINT32 i = 0;
  if (source == NULL || dest == NULL) {
    DEBUG((EFI_D_ERROR, "convert_char8_to_char16: invalid parameters!\n"));
    ret = EFI_FAILURE;
    goto exit;
  }
  for (i = 0; i < len; i++) {

    dest[i] = source[i] & WIDECHAR_TO_CHAR_MASK;
  }
  dest[len] = '\0';
  ret = EFI_SUCCESS;
exit:
  return ret;
}

/**

	Read image from the partition and returns the start address and total size.

	@param[in]      *pPartitionLabel 		Partition label.
	@param[out]     *img_ptr               Pointer to the start address of image.
    @param[out]     img_size                total size of image.

    @retval         Status                  Indicates whether reading image was successful.

**/
EFI_STATUS read_partition(UINT16 *pPartitionLabel, UINT8 *buff, UINTN buff_len)
{

  HandleInfo HandleInfoList[4] = {};
  UINT32 MaxHandles = 0;
  PartiSelectFilter HandleFilter = {0};
  UINT32 Attribs = 0;
  EFI_STATUS Status = EFI_FAILURE;
  EFI_BLOCK_IO_PROTOCOL *BlkIo = NULL;
  UINTN imgSize = 0;
  UINT8 *img_ptr = NULL;

  if (pPartitionLabel == NULL || buff_len == 0 || buff == NULL) {

    DEBUG((EFI_D_ERROR, "read_partition: Invalid input arguments!\n"));
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
  Attribs |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
  HandleFilter.PartitionLabel = pPartitionLabel;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = 0;
  HandleFilter.PartitionType = 0;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "read_partition: Failed to get handle (label = %s)\n", pPartitionLabel));
    goto exit;
  }

  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1) {
    DEBUG((EFI_D_ERROR, "read_partition: Ambiguity in Image partition\n"));
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if (MaxHandles == 0) {
    DEBUG((EFI_D_ERROR, "read_partition: Image partition not found\n"));
    Status = EFI_NOT_FOUND;
    goto exit;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  imgSize = (BlkIo->Media->LastBlock + 1) * BlkIo->Media->BlockSize;
  img_ptr = UncachedAllocatePool(imgSize);

  if (img_ptr == NULL) {
    DEBUG((EFI_D_ERROR, "read_partition: cannot allocated memory pool!\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  Status = BlkIo->ReadBlocks(BlkIo, BlkIo->Media->MediaId, 0, imgSize, img_ptr);
  if (EFI_ERROR(Status)) {

    DEBUG((EFI_D_ERROR, "read_partition: readblocks was unsuccessful\n"));
    goto exit;
  }
  CopyMemS(buff, buff_len, img_ptr, imgSize);
  Status = EFI_SUCCESS;
exit:
  if (img_ptr) {
    UncachedFreePool(img_ptr);
  }
  return Status;
}

/**

	Write to partition

	@param[in]     *pPartitionLabel 		Partition label.
	@param[in]     *buff                    Pointer to the buffer to be written on the partition.
    @param[in]     buff_len                 length of buff.

    @retval         Status                  Indicates whether reading image was successful.
**/
EFI_STATUS write_partition(UINT16 *pPartitionLabel, UINT8 *buff, UINTN buff_len)
{

  HandleInfo HandleInfoList[4] = {};
  UINT32 MaxHandles = 0;
  PartiSelectFilter HandleFilter = {0};
  UINT32 Attribs = 0;
  EFI_STATUS Status = EFI_FAILURE;
  EFI_BLOCK_IO_PROTOCOL *BlkIo = NULL;
  UINTN imgSize = 0;
  UINT8 *img_ptr = NULL;

  if (pPartitionLabel == NULL || buff == NULL || buff_len == 0) {

    DEBUG((EFI_D_ERROR, "write_partition: Invalid input!\n"));
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }
  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
  Attribs |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
  HandleFilter.PartitionLabel = pPartitionLabel;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = 0;
  HandleFilter.PartitionType = 0;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "write_partition: Failed to get handle (label = %s)\n", pPartitionLabel));
    goto exit;
  }

  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1) {
    DEBUG((EFI_D_ERROR, "write_partition: Ambiguity in Image partition\n"));
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if (MaxHandles == 0) {
    DEBUG((EFI_D_ERROR, "write_partition: Image partition not found\n"));
    Status = EFI_NOT_FOUND;
    goto exit;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  imgSize = (BlkIo->Media->LastBlock + 1) * BlkIo->Media->BlockSize;

  if (imgSize < buff_len) {

    DEBUG((EFI_D_ERROR, "write_partition: buffer is too big!\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  img_ptr = UncachedAllocatePool(imgSize);

  if (img_ptr == NULL) {
    DEBUG((EFI_D_ERROR, "write_partition: cannot allocated memory pool!\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  Status = BlkIo->ReadBlocks(BlkIo, BlkIo->Media->MediaId, 0, imgSize, img_ptr);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "write_partition: readblocks was unsuccessful\n"));
    goto exit;
  }

  CopyMemS(img_ptr, imgSize, buff, buff_len);

  Status = BlkIo->WriteBlocks(BlkIo, BlkIo->Media->MediaId, 0, imgSize, img_ptr);
  if (EFI_ERROR(Status)) {

    DEBUG((EFI_D_ERROR, "write_partition: writeblocks was unsuccessful\n"));
    goto exit;
  }
  Status = EFI_SUCCESS;
exit:
  if (img_ptr) {
    UncachedFreePool(img_ptr);
  }
  return Status;
}
