//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef SCALERS_H
#define SCALERS_H

// TODO : if there is no difference between this file and modules.h merge them

// TODO : document this file: copyright, functions, etc.

#include "icpcomdef.h"

#define Q1 21

#pragma pack(push)
#pragma pack(4)

typedef struct
{
    uint32_t phase_init;
    uint32_t phase_step;
    uint16_t en;
    uint16_t input_l;
    uint16_t output_l;
    uint16_t pixel_offset;
    uint16_t interp_reso;
    uint16_t rounding_option_v;         //: we might need to adjust rounding mode based on the output pixel locations
    uint16_t rounding_option_h;         //: we might need to adjust rounding mode based on the output pixel locations
    uint16_t dummy_field_for_byte_padding;
    //BOOL roundingBias;                //: add bias due to the horizontal address offset
}
MNScaleDownInStruct_V20_1D;  // HW params

typedef struct
{
    uint16_t start_x;
    uint16_t end_x;
}
stripeROI_1D;

typedef struct
{
    uint16_t en;
    uint16_t input_l;
    uint16_t output_l;
    int16_t pixel_offset;
    uint32_t phase_init;
    uint32_t phase_step;
    int32_t rounding_pattern;
    int32_t upscale_mode;
}
UpscaleInStruct_V11_1D;


typedef struct
{
    uint16_t en;
    uint16_t input_l;
    uint16_t output_l;
    int16_t  preload;
    uint32_t phase_init;
    uint32_t phase_step;
}
UpscaleInStruct_V20_1D;

#pragma pack(pop)

#ifndef Clamp
#define Clamp(x, t1, t2) (((x) < (t1))? (t1): ((x) > (t2))? (t2): (x))
#endif

#ifndef MAX2
#define MAX2(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN2
#define MIN2(a,b) (((a)<(b))?(a):(b))
#endif


#endif  // SCALERS
