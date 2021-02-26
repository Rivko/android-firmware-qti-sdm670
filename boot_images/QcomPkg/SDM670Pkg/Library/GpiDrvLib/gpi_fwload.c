/**
 * @file  gpi_fwload.c
 * @brief Implements the firmware loading procedure for GSI and associated SEs 
 */
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/20/17   pc      Updated Imagesize calculation with proper blocksize
04/11/17   ts      Created to load qupv3 firmware from flash

===============================================================================
              Copyright (c) 2017 QUALCOMM Technologies, Inc.
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Include/scm_sip_interface.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIScm.h>
#include <Library/DebugLib.h>
#include "gpi_ee.h"


/* partition guid for QUPV3 firmware ELF in UFS */
STATIC EFI_GUID qupv3_fw_elf_partition_guid = { 0x21D1219F, 0x2ED1, 0x4AB4, { 0x93, 0x0A, 0x41, 0xA1, 0x6A, 0xE7, 0x5F, 0x7F } };

/* Global static variables */
STATIC UINT64 ScmParam[SCM_MAX_NUM_PARAMETERS];
STATIC UINT64 ScmResult[SCM_MAX_NUM_RESULTS];
STATIC QCOM_SCM_PROTOCOL * QcomScmProtocol = NULL;  

extern EFI_STATUS  efi_split_elf_file(void *  elf_base, void ** meta_base, void ** prog_base, void *  prog_size);

STATIC inline EFI_STATUS gpi_tz_pil_init_id(UINT64 ELFMetaBase)
{
  EFI_STATUS Status = EFI_LOAD_ERROR;

  /* Subsys ID */
  ScmParam[0] = 0x13;
  /* Memory address of ELF meta data */
  ScmParam[1] = ELFMetaBase;

  if (QcomScmProtocol == NULL)
  {
    return Status;
  }

  /*
   * Authenticate ELF meta data with PIL
   */
  Status = QcomScmProtocol->ScmSipSysCall(QcomScmProtocol,
                                 TZ_PIL_INIT_ID,
                                 TZ_PIL_INIT_ID_PARAM_ID,
                                 ScmParam,
                                 ScmResult);

  return Status;
}

STATIC inline EFI_STATUS gpi_tz_pil_mem_id(UINT64 imageBase, UINTN imageSize)
{
  EFI_STATUS Status = EFI_LOAD_ERROR;

  /* Subsys ID */
  ScmParam[0] = 0x13;
  /* Start address of the image to be loaded */
  ScmParam[1] = imageBase; 
  /* Length of the image to be loaded */
  ScmParam[2] = (UINT64)imageSize;

  if (QcomScmProtocol == NULL)
  {
    return Status;
  }

  /*
   * Confirms the memory address to be loaded is different
   * than in the ELF
   */
  Status = QcomScmProtocol->ScmSipSysCall(QcomScmProtocol,
                                 TZ_PIL_MEM_ID,
                                 TZ_PIL_MEM_ID_PARAM_ID,
                                 ScmParam,
                                 ScmResult);
  return Status;
}

STATIC inline EFI_STATUS gpi_tz_pil_auth_reset_id(void)
{
  EFI_STATUS Status = EFI_LOAD_ERROR;

  /* Subsys ID */
  ScmParam[0] = 0x13;

  if (QcomScmProtocol == NULL)
  {
    return Status;
  }

  /*
   * Authenticate and load the image
   */
  Status = QcomScmProtocol->ScmSipSysCall(QcomScmProtocol,
                                 TZ_PIL_AUTH_RESET_ID,
                                 TZ_PIL_AUTH_RESET_ID_PARAM_ID,
                                 ScmParam,
                                 ScmResult);

  return Status;
}

STATIC inline EFI_STATUS gpi_tz_pil_unlock_xpu_id(void)
{
  EFI_STATUS Status = EFI_LOAD_ERROR;

  /* Subsys ID */
  ScmParam[0] = 0x13;

  if (QcomScmProtocol == NULL)
  {
    return Status;
  }

  /*
   * Unlock the XPU and free the memory
   */
  Status = QcomScmProtocol->ScmSipSysCall(QcomScmProtocol,
                                 TZ_PIL_UNLOCK_XPU_ID,
                                 TZ_PIL_UNLOCK_XPU_ID_PARAM_ID,
                                 ScmParam,
                                 ScmResult);
  
  return Status;
}

EFI_STATUS gpi_load_firmware(void)
{
  VOID                    * ElfBase = NULL;
  VOID                    * MetaBase = NULL;
  VOID                    * ProgSegBase = NULL;
  UINT32                    ProgSegSize = 0;
  EFI_BLOCK_IO_PROTOCOL   * pBlkIo;
  UINTN                     ImgSize;
  UINT32                    Attribs = 0;
  HandleInfo                HandleInfoList[4];
  UINT32                    MaxHandles;
  PartiSelectFilter         HandleFilter;
  EFI_STATUS                Status = EFI_SUCCESS;

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie UFS device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = &qupv3_fw_elf_partition_guid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: GetBockIOHandles Failed\r\n", __func__));
    return Status;
  }
  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1)
  {
    DEBUG(( EFI_D_WARN, "%a: QUPV3 partition handle invalid\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  if (MaxHandles == 0)
  {
    DEBUG(( EFI_D_WARN, "%a: QUPV3 ELF partition not found\r\n", __func__));
    return EFI_NOT_FOUND;
  }

  /* Get the Block IO protocol instance from the handle */
  pBlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  ImgSize = (pBlkIo->Media->LastBlock + 1) * pBlkIo->Media->BlockSize;

  /* Allocate page aligned buffer for ELF buffer from flash */
  ElfBase = UncachedAllocatePool (ImgSize);
  if (ElfBase == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Could not allocate uncached mem\r\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  /*
   * Load the ELF image from flash partition to DDR
   */
  Status = pBlkIo->ReadBlocks (pBlkIo, pBlkIo->Media->MediaId, 0, ImgSize, ElfBase);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Could not read ELF from UFS partition\r\n", __func__));
    return Status;
  }

  /*
   * This function will take ELF buffer base as input and perform:
   *
   * 1. Parse the ELF buffer
   * 2. Create Meta buffer from the ELF buffer 
   * 3. Provide the base address for program segment within ELF buffer
   */
  Status = efi_split_elf_file(ElfBase, &MetaBase, &ProgSegBase, &ProgSegSize);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed to parse ELF successfully\r\n", __func__));
    return Status;
  }

  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&QcomScmProtocol);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed to locate QcomScmProtocol\r\n", __func__));
    return Status;
  }

  /* TZ PIL init */
  Status = gpi_tz_pil_init_id((UINT64)MetaBase);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed TZ_PIL_INIT\r\n", __func__));
    return Status;
  }

  /*
   * Assign prog seg size till the end of ELF from the first prog seg base.
   * We are calculating this here because, the program segment should start
   * and end at 4K aligned address for XPU protection reasons
   */
  ProgSegSize = (ImgSize - ((uint8 *)ProgSegBase - (uint8 *)ElfBase));

  /* TZ PIL mem */
  Status = gpi_tz_pil_mem_id((UINT64)ProgSegBase, ProgSegSize);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed TZ_PIL_MEM\r\n", __func__));
    return Status;
  }

  /* TZ PIL auth reset */
  Status = gpi_tz_pil_auth_reset_id();
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed TZ_PIL_AUTH_RESET\r\n", __func__));
    return Status;
  }

  /* TZ PIL unlock xpu */
  Status = gpi_tz_pil_unlock_xpu_id();
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Failed TZ_PIL_UNLOCK_XPU\r\n", __func__));
    return Status;
  }

  return Status;
}
