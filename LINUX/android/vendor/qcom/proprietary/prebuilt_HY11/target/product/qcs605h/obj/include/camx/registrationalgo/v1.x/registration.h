// NOWHINE NC009 <- Shared file with system team so uses non-CamX file naming
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  registration.h
/// @brief registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE DC002 :
// NOWHINE FILE GR017 : intrinsic type
// NOWHINE FILE CF003 :
// NOWHINE FILE CP006 :
// NOWHINE FILE CP010 :
// NOWHINE FILE CP021 :


// NOWHINE FILE NC003 :
#ifndef REGISTRATION_H
#define REGISTRATION_H

// NOWHINE PR007:

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// NOWHINE NC010:
int register_mf(void* anchor_img, void* ref_img, int width, int height, int bpp, bool data_has_2bps,
    float gmv[9], int &confidence, // output from registration
    int resize_to_width = 0, int resize_to_height = 0,
    int new_origin_x = 0, int new_origin_y = 0, double resize_factor_x = 1, double resize_factor_y = 1); // optional params
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // REGISTRATION_H
