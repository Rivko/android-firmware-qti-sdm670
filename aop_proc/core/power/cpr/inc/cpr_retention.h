/**
* @file:  cpr_retention.h
* @brief:
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* QUALCOMM Proprietary and Confidential.
*
*/

#ifndef CPR_RETENTION_H
#define CPR_RETENTION_H

typedef struct
{
   uint32 address;
   uint8 offset;
   uint8 mask;
} fuse_t;

typedef struct
{
   const uint32 *retention_table;
   const fuse_t  fuse;
} pvs_data_t;

typedef struct
{
    const uint32 voltage;
    const fuse_t fuse;
} aon_data_t;

void cpr_retention_init(void);

#endif
