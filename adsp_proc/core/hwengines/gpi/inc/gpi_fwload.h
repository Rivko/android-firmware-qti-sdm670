#ifndef __GPI_FWLOAD_H__
#define __GPI_FWLOAD_H__

/**
  @file gpi_fwload.h
  @brief
  This file contains implementation of interfaces that is used to
  parse GSI firmware buffer.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/13/17   ts      Created

===============================================================================
                   Copyright (c) 2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "gsihwio.h"

/* Fimrware Magic in ASCII: 'QSI ' */
#define GSI_FW_MAGIC 0x20495351
/* Number of bytes per firmware line in ELF firmware segment */
#define GSI_FW_BYTES_PER_LINE 8

typedef enum {
  QUPV3_GSI_0   = 0,
  QUPV3_GSI_1   = 1,
  QUPV3_GSI_SSC = 2,
  QUPV3_GSI_MAX = 3
}qupv3_inst_t;

uint64 qupv3_gsi_base[QUPV3_GSI_MAX] =
{
  GSI_0_REG_BASE,
  GSI_1_REG_BASE,
  GSI_REG_BASE
};

uint32 qupv3_gsi_base_size[QUPV3_GSI_MAX] =
{
  GSI_0_REG_BASE_SIZE,
  GSI_1_REG_BASE_SIZE,
  GSI_SSC_REG_BASE_SIZE
};

typedef struct __attribute__((__packed__))
{
    uint32 magic               ;// 'QSI '
    uint32 version             ;// 1
    uint32 core_version        ;// QUPV3_HW_VERSION
    uint32 fw_version          ;// from GSI FW
    uint16 fw_size_in_items    ;// no. of GSI FW lines (each 8 bytes)
    uint16 fw_offset           ;// offset of first GSI FW lines
    uint16 iep_size_in_items   ;// no. of IEP items
    uint16 iep_offset          ;// offset of first IEP entry
}gsi_fw_hdr_t;

typedef struct __attribute__((__packed__))
{
    uint32 iram_dword0;// word 0 of a single IRAM firmware entry
    uint32 iram_dword1;// word 1 of a single IRAM firmware entry
}gsi_fw_iram_t;

typedef struct __attribute__((__packed__))
{
    uint32 offset;// offset from QUPV3_GSI_TOP
    uint32 value ;// value to be written into above offset
}gsi_fw_iep_t;

#endif /* #define __GPI_FWLOAD_H__ */
