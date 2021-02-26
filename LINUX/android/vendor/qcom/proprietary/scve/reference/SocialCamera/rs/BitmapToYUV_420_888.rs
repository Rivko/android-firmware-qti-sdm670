/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma version(1)
#pragma rs java_package_name(com.qualcomm.qti.fr_camera.imageutils)
#pragma rs_fp_relaxed

static const float3 ycbcr_offset = { 0.0625f, 0.5f, 0.5f };
static const float clamp_min = 16.0f;
static const float clamp_max_y = 235.0f;
static const float clamp_max_chroma = 240.0f;

// User must set these parameters
float3 y_transform;
float3 cb_transform;
float3 cr_transform;
rs_allocation rgb_alloc; // Provides the RGB source data
rs_allocation y_alloc; // Receives the Y plane
rs_allocation v_alloc; // Receives the V plane

uint2 size;

void setup()
{
    size = (uint2){ rsAllocationGetDimX(rgb_alloc), rsAllocationGetDimY(rgb_alloc) };
}

// N.B. Returns U plane. Y and V are populated via rs_allocation objects.
uchar __attribute__((kernel)) convertRGBToYUV(uint32_t x, uint32_t y)
{
    uint2 luma_x = min((uint2){ x * 2, x * 2 + 1 }, size.x );
    uint2 luma_y = min((uint2){ y * 2, y * 2 + 1 }, size.y );

    // Load pixel values for the four pixels which make up the output chroma pixel. If the load
    // request would overflow the allocation, clamp to the edge.
    float3 p00 = rsUnpackColor8888(rsGetElementAt_uchar4(rgb_alloc, luma_x.S0, luma_y.S0)).xyz;
    float3 p10 = rsUnpackColor8888(rsGetElementAt_uchar4(rgb_alloc, luma_x.S1, luma_y.S0)).xyz;
    float3 p01 = rsUnpackColor8888(rsGetElementAt_uchar4(rgb_alloc, luma_x.S0, luma_y.S1)).xyz;
    float3 p11 = rsUnpackColor8888(rsGetElementAt_uchar4(rgb_alloc, luma_x.S1, luma_y.S1)).xyz;

    float4 y_values = (float4){ dot(y_transform, p00), dot(y_transform, p10),
            dot(y_transform, p01), dot(y_transform, p11) } + ycbcr_offset.S0;
    uchar4 y_values_uchar = convert_uchar4(clamp(round(y_values * 255), clamp_min, clamp_max_y));

    // Convert and store the Y values
    rsSetElementAt_uchar(y_alloc, y_values_uchar.S0, luma_x.S0, luma_y.S0);
    rsSetElementAt_uchar(y_alloc, y_values_uchar.S1, luma_x.S1, luma_y.S0);
    rsSetElementAt_uchar(y_alloc, y_values_uchar.S2, luma_x.S0, luma_y.S1);
    rsSetElementAt_uchar(y_alloc, y_values_uchar.S3, luma_x.S1, luma_y.S1);

    // Get the average of the four pixels
    float3 p = (p00 + p01 + p10 + p11) / 4;

    float2 cbcr_value = (float2){ dot(cb_transform, p), dot(cr_transform, p) } + ycbcr_offset.S12;
    uchar2 cbcr_value_uchar = convert_uchar2(clamp(round(cbcr_value * 255),
            clamp_min, clamp_max_chroma));

    rsSetElementAt_uchar(v_alloc, cbcr_value_uchar.S1, x, y);

    return cbcr_value_uchar.S0;
}
