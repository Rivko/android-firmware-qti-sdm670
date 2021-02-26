/**
 * @file:  cpr_checksum.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/12/19 02:59:36 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr_checksum.c#2 $
 * $Change: 15088264 $
 */
#include "cpr_data.h"
#include "cpr_utils.h"
#include "cpr_image.h"
#include "cpr_logs.h"

// Max number of summation before overflow
#define MAX_NUM_SUMS_16    20
#define MAX_NUM_SUMS_32    359

#define GET_NUM_RAIL_CHECKSUMS(mode_count)  (3 + ((mode_count) * 6))
#define ARRAY_SIZE(arr)                     (sizeof(arr) / sizeof(arr[0]))


/*==============================================================================
 *
 */
static uint16 checksum16(const uint8 *data, uint32 len)
{
    uint16 sum1 = 0xff;
    uint16 sum2 = 0xff;

    while(len)
    {
        uint32 i = (len >= MAX_NUM_SUMS_16) ? MAX_NUM_SUMS_16 : len;

        len -= i;

        do
        {
            sum1 += *data++;
            sum2 += sum1;
        }
        while(--i);

        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
    }

    sum1 = (sum1 & 0xff) + (sum1 >> 8);
    sum2 = (sum2 & 0xff) + (sum2 >> 8);

    return (sum2 << 8) | sum1;
}

/*==============================================================================
 *
 */
static uint32 checksum32(const uint16 *data, uint32 len)
{
    uint32 sum1 = 0xffff;
    uint32 sum2 = 0xffff;

    while(len)
    {
        uint32 i = (len >= MAX_NUM_SUMS_32) ? MAX_NUM_SUMS_32 : len;

        len -= i;

        do
        {
            sum1 += *data++;
            sum2 += sum1;
        }
        while(--i);

        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);

    return (sum2 << 16) | sum1;
}

/*==============================================================================
 *
 */
static void get_rail_checksums(cpr_rail_id_t rail_id, uint16 *checksum_array, uint32 len, uint32 *offset)
{
    cpr_rail* rail = cpr_utils_get_rail( rail_id );

    CPR_ASSERT(rail && rail->vp);

    // make sure the array size is big enough
    CPR_ASSERT(len >= *offset + GET_NUM_RAIL_CHECKSUMS(rail->vp->modesCount));

    checksum_array = &checksum_array[*offset];
    *offset += GET_NUM_RAIL_CHECKSUMS(rail->vp->modesCount);

    // checksum on supported modes
    *checksum_array++ = checksum16((const uint8*)rail->vp->supportedModes, sizeof(cpr_voltage_mode) * rail->vp->modesCount);

    // checksum on KV config
    if(rail->vp->kvCfg)
    {
        *checksum_array++ = checksum16((const uint8*)(rail->vp->kvCfg->kvs), sizeof(cpr_kv) * rail->vp->kvCfg->count);
    }
    else
    {
        *checksum_array++ = checksum16(NULL, 0);
    }

    // checksum on mode settings
    for(int i = 0; i < rail->vp->modesCount; i++)
    {
        struct cpr_voltage_data *mode = &rail->vp->modes[i];

        // checksum on fref
        *checksum_array++ = checksum16((const uint8*)&(mode->fref), sizeof(uint32));

        // checksum on margins
        if(mode->margins)
        {
            *checksum_array++ = checksum16((const uint8*)(mode->margins->data), sizeof(cpr_margin_data) * mode->margins->count);
        }
        else
        {
            *checksum_array++ = checksum16(NULL, 0);
        }

        // checksum on freqDelta
        *checksum_array++ = checksum16((const uint8*)&(mode->freqDelta), sizeof(uint32));

        // checksum on quotients
        if(mode->quotients)
        {
            *checksum_array++ = checksum16((const uint8*)(mode->quotients->quots), sizeof(cpr_quotient) * mode->quotients->count);
        }
        else
        {
            *checksum_array++ = checksum16(NULL, 0);
        }

        // checksum on submodes
        *checksum_array++ = checksum16((const uint8*)(mode->subModes), sizeof(cpr_freq_data) * mode->subModesCount);
    }

    // checksum on settle modes
    *checksum_array++ = checksum16((const uint8*)(rail->settleModes), sizeof(cpr_voltage_mode) * rail->settleModesCount);

    // checksum on enab type
    *checksum_array++ = checksum16((const uint8*)&(rail->enablement->funcId), sizeof(cpr_init_funcs_id));
}

/*==============================================================================
 *
 */
uint32 cpr_cx_mx_settings_checksum(void)
{
    cpr_domain_id rail_ids[] = {CPR_RAIL_MX, CPR_RAIL_CX, CPR_RAIL_VDDA,
        CPR_RAIL_MSS, CPR_RAIL_GFX};
    uint32 max_num_checksums = ARRAY_SIZE(rail_ids) * GET_NUM_RAIL_CHECKSUMS(CPR_VOLTAGE_MODE_COUNT-2);
    uint16 *rail_checksums = NULL;
    uint32 offset = 0;
    uint32 ret = 0;

    // allocate memory with maximum possible size (-2 to ignore OFF and RETENTION mode)
    // it's too big to allocate memory on the stack
    rail_checksums = (uint16 *)cpr_image_malloc( sizeof(uint16) * max_num_checksums );

    for(uint32 i = 0; i < ARRAY_SIZE(rail_ids); i++)
    {
        get_rail_checksums(rail_ids[i], rail_checksums, max_num_checksums, &offset);
    }

    ret = checksum32(rail_checksums, offset);

    CPR_LOG_TRACE("checksum: 0x%08x", ret);

    cpr_image_free(rail_checksums);

    return ret;
}

