#pragma once
/*=============================================================================
  @file sns_osa_target.h

  Target specific details

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

//From IPCAT memorymap
#define SDC_CODE_RAM_PHYS_SIZE   (0x05C10000 - 0x05C00000)
#define SDC_DATA_RAM_PHYS_SIZE   (0x05CE8000 - 0x05CE0000)

#define SDC_IMG_INFO_HEADER_SIZE 0x100
#define SDC_CODE_RAM_SIZE        SDC_CODE_RAM_PHYS_SIZE
#define SDC_DATA_RAM_SIZE        (SDC_DATA_RAM_PHYS_SIZE - SDC_IMG_INFO_HEADER_SIZE)

//From elf header
#define SDC_CODE_RAM_OFFSET      0x0
#define SDC_DATA_RAM_OFFSET      0xE0100

