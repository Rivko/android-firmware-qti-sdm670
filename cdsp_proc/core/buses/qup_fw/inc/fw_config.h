/** 
    @file  fw_config.h
    @brief fw loading interface
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __FW_CONFIG_H__
#define __FW_CONFIG_H__

#include "comdef.h"

// se header
typedef struct __attribute__ ((packed)) elf_se_hdr
{
    uint32 magic;               /* = 'SEFW' */
    uint32 version;             /* Structure version number */
    uint32 core_version;        /* QUPV3_HW_VERSION */
    uint16 serial_protocol;     /* Programmed into GENI_FW_REVISION */
    uint16 fw_version;          /* Programmed into GENI_FW_REVISION */
    uint16 cfg_version;         /* Programmed into GENI_INIT_CFG_REVISION */
    uint16 fw_size_in_items;    /* Number of (uint32) GENI_FW_RAM words */
    uint16 fw_offset;           /* Byte offset of GENI_FW_RAM array */
    uint16 cfg_size_in_items;   /* Number of GENI_FW_CFG index/value pairs */
    uint16 cfg_idx_offset;      /* Byte offset of GENI_FW_CFG indexes array */
    uint16 cfg_val_offset;      /* Byte offset of GENI_FW_CFG values array */

} elf_se_hdr;

// qsi header
typedef struct __attribute__ ((packed)) elf_qsi_hdr
{
    uint32 magic;               /* = 'QSI ' */
    uint32 version;             /* 1 */
    uint32 core_version;        /* QUPV3_HW_VERSION */
    uint32 fw_version;          /* from GSI FW */
    uint16 fw_size_in_items;    /* */
    uint16 fw_offset;           /* */
    uint16 iep_size_in_items;   /* */
    uint16 iep_offset;          /* */

} elf_qsi_hdr;

boolean se_is_fw_loaded (uint32 se_index);
boolean qup_is_fw_loaded (void);
uint32  qup_hw_version (void);

#endif
