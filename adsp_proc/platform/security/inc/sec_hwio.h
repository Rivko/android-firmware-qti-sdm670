/*=============================================================================

FILE:       sec_hwio.h

SERVICES:   Functions for reading QFPROM fuse-based values

==============================================================================
        Copyright (c) 2013 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifndef SEC_HWIO_H
#define SEC_HWIO_H

#include "com_dtypes.h"

typedef struct {
    uint32 serial_number_addr;
    uint32 chip_id_addr;
    uint32 oem_id_addr;
    uint32 reserved1;
    uint32 reserved2;
    uint32 reserved3;
}pl_sechwio_type;

uint32 SecHWIO_GetSerialNumber(void);
uint16 SecHWIO_GetChipID(void);
uint16 SecHWIO_GetOEMID(void);


#endif   // SEC_HWIO_H

