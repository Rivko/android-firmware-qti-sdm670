//-------------------------------------------------------------------------
// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  stripeIFE.h
// @brief 
//------------------------------------------------------------------------


#ifndef STRIPE_IFE_H
#define STRIPE_IFE_H

#include "list.h"
#include "stripe_common.h"
#include "modules.h"

#define NUM_OF_BAF_REGIONS  10

typedef struct
{
    // for stats
    int16_t frame_hoffset;
    int16_t frame_hnum;
    int16_t frame_hgrid_width;
}stats_input_t;

typedef struct
{
    uint16_t baf_region_index;
    uint16_t baf_region_hoffset;
    uint16_t baf_region_width;
}baf_stats_region_input_t;

typedef struct
{
    uint16_t iir_enabled;
    uint16_t baf_downscale_enabled;
    int16_t  bafDownScaleOutWidth;
    int16_t  balDownScaleOutHeight;
    uint16_t numberOfBafRegions;
    baf_stats_region_input_t  bafRegion[NUM_OF_BAF_REGIONS];
}baf_stats_input_t;

typedef struct
{
    uint16_t stats_hoffset;
    uint16_t num_of_h_regions;
}stats_output_t;

typedef struct
{
    uint16_t baf_region_index;
    uint16_t baf_region_hoffset;
    uint16_t baf_region_width;
    uint16_t baf_region_location;    // 0: within_left_stripe;  1: within_right_stripe;  2: split between two stripes--- this means need to be merged afterwards;
}baf_single_region_output_t;


typedef struct
{
    uint16_t  numberOfBafRegions;
    baf_single_region_output_t  baf_region_out[NUM_OF_BAF_REGIONS];
}stats_baf_output_t;

typedef struct
{
    uint16_t zoom_enable;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t width;
    uint16_t height;
} ife_zoom_window;

typedef struct{
    uint32_t Bwidth_l;
    uint32_t Bheight_l;
    uint32_t x_delta_l;
    uint32_t y_delta_l;
    uint32_t MeshGridBwidth_l;
    uint32_t MeshGridBheight_l;
    uint32_t Ly_start_l;                    // block index (vertical)
    uint32_t By_start_l;                    // sub block index (vertical)
    uint32_t By_e1_l;
    uint32_t By_init_e1_l;
    uint32_t intp_factor;                   // Bicubic interpolation factor for left image, 0~3 (1/2/4/8 subgrids)
    uint32_t grid_index;                    // Start block x index, 6u
    uint32_t subgrid_index;                 // Start subgrid x index within start block, 3u
    uint32_t pixel_index_within_subgrid;    // x coordinate of top left pixel in start block/subgrid, 9u
    int32_t deltah;     // this one is not used in SWI,  intermediate value stored here for convenience.  should be moved to a better place later.
    uint32_t num_meshgain_h;
    uint32_t num_meshgain_v;

} RollOffPedOutputParameters;

typedef struct
{
    int32_t edgeStripeLT;
    int32_t edgeStripeRB;

    // fetch coordinates
    int16_t fetchFirstPixel;          // relative to frame origin (0,0)
    int16_t fetchLastPixel;

    // HVX TO pixel coordinates
    int16_t hvxTapoffFirstPixel;
    int16_t hvxTapoffLastPixel;

    // write coordinates
    int16_t outRange_fd[2];
    int16_t outRange_full[2];
    int16_t outRange_1to4[2];
    int16_t outRange_1to16[2];
    int16_t    outRange_raw[2];
    int16_t    outRange_pdaf[2];

    // crop amounts
    crop_1D outCrop_fd_Y;
    crop_1D outCrop_fd_C;
    crop_1D outCrop_full_Y;
    crop_1D outCrop_full_C;
    crop_1D outCrop_1to4_Y;
    crop_1D outCrop_1to4_C;
    crop_1D outCrop_1to16_Y;
    crop_1D outCrop_1to16_C;
    crop_1D outCrop_raw;

    crop_1D preDS4Crop_1to4_Y;
    crop_1D preDS4Crop_1to4_C;
    crop_1D preDS4Crop_1to16_Y;
    crop_1D preDS4Crop_1to16_C;

    //// scalers
    MNScaleDownInStruct_V16_1D mndsConfig_y_fd;
    MNScaleDownInStruct_V16_1D mndsConfig_c_fd;

    MNScaleDownInStruct_V16_1D mndsConfig_y_full;
    MNScaleDownInStruct_V16_1D mndsConfig_c_full;

    MNScaleDownInStruct_V16_1D bafDownscaler;

    RollOffOutputParameters  rolloffOutStripe;
    PedestalOutputParameters pedestalOutStripe;

    hdrOutStruct_v20 hdr_out;
    abfOutStruct_v34 abf_out;
    PDPCv11stripingOutputParams  pdpc_out;
    HDRBayerHist_v13OutputParams hdrBhist_out;
    HDRBayerHist_v13OutputParams bHist_out;
    iHistv12OutputParam          iHist_out;
    BayerGridv15OutputParam      bg_tintless_out;
    BayerGridv15OutputParam      bg_awb_out;
    BayerExpv15OutputParam       be_out;
    rowSumColSumv14OutputParam   rscs_out;
    //int32_t PDAF_start_pixel;              // for right stripe it is needed to recalcualte the 64x64 mask

    //stats_output_t  bg_out;
    //stats_output_t  be_out;            // should be before roll off
    uint16_t  numberOfBafRegions;
    int16_t dummy_field_for_byte_padding;
    BayerFocusv23OutputParam  baf_out;

    WriteEngineStripeParam bg_tintlessWriteEngineStripeParam;
    WriteEngineStripeParam bg_awbWriteEngineStripeParam;
    WriteEngineStripeParam beWriteEngineStripeParam;
    WriteEngineStripeParam bafWriteEngineStripeParam;
    WriteEngineStripeParam rowSumWriteEngineStripeParam;
    WriteEngineStripeParam colSumWriteEngineStripeParam;
    WriteEnginePassParam   bafWriteEnginePassParam;
}
IFEStripeOutput;


typedef struct
{
    // top level
    int16_t tiering;        // determine VFE line buffer width
    int16_t striping;         // 0: no dual vfe,  1: dual vfe
    int16_t extend_for_baf;     //extend stripes for BAF: 0 is to let SW do most of the merge

    int16_t inputFormat;
    int16_t inputWidth;
    int16_t inputHeight;
    BOOL    use_zoom_setting_from_external;
    ife_zoom_window zoom_window;                    //only effective only when use_zoom_setting_from_external == FALSE --> dual IFE lib derives all settings
    stripeROI_1D roi_mnds_out_full;                 //luma frame-level crop after mnds output (end_x-start_x+1==outWidth_full); only effective only when use_zoom_setting_from_external == TRUE
    stripeROI_1D roi_mnds_out_fd;                 //luma frame-level crop after mnds output (end_x-start_x+1==outWidth_full); this setting is ALWAYS effective

    int16_t outFormat_full;
    int16_t outWidth_full ;
    int16_t outHeight_full;
    int16_t outFormat_1to4;
    int16_t outFormat_1to16;

    int16_t outFormat_fd;
    int16_t outWidth_fd;
    int16_t outHeight_fd;

    int16_t outFormat_raw;

    int16_t outFormat_pdaf;

    hdrInStruct_v20 hdr_in;

    mndsInStruct_v16 mnds_in_y_fd;
    mndsInStruct_v16 mnds_in_c_fd;
    mndsInStruct_v16 mnds_in_y_full;
    mndsInStruct_v16 mnds_in_c_full;

    PDPCv11stripingInputParams pdpc_in;
    int16_t pedestal_enable;
    int16_t rolloff_enable ;
    int16_t baf_enable;
    int16_t bg_tintless_enable ;
    int16_t bg_awb_enable ;
    int16_t be_enable ;
    int16_t pdaf_enable;
    int16_t hdr_enable ;
    int16_t bpc_enable ;
    int16_t abf_enable ;

    int16_t tappingPoint_be;                //0: before HDR, 1: before DM

    int16_t tapoffPoint_hvx; // 0: Not used, 1: before Pedestal, 2: before HDR, 3: before BPC/BCC, 4: before WB
    int16_t kernelSize_hvx; // if HVX is used as kernel module, specify kernel size here for crop calculation, etc.

    int16_t fetchLeftStripeEnd;     // fetch coordinate: provided by deriveFetchRange_titanIFE() function
    int16_t fetchRightStripeStart;  // fetch coordinate: provided by deriveFetchRange_titanIFE() function

    int16_t dummy_field_for_byte_padding;

    //stats_input_t  bg_input;
    //stats_input_t  be_input;
    //baf_stats_input_t baf_input;
    Pedestalv13InputParams pedestalParam;
    RollOffv34InputParameters rollOffParam;
    HDRBayerHist_v13InputParams hdrBhist_input;
    HDRBayerHist_v13InputParams bHist_input;
    iHistv12InputParam iHist_input;
    BayerGridv15InputParam bg_tintless_input;
    BayerGridv15InputParam bg_awb_input;
    BayerExpv15InputParam  be_input;
    BayerFocusv23InputParam baf_input;
    rowSumColSumv14InputParam rscs_input;
}
stripingInput_titanIFE;


typedef struct
{
    int16_t numStripes;
    int16_t dummy_field_for_byte_padding_0;
    int32_t dummy_field_for_byte_padding_1;
    ListHandle hStripeList;

    WriteEnginePassParam   bg_tintlessWriteEnginePassParam;
    WriteEnginePassParam   bg_awbWriteEnginePassParam;
    WriteEnginePassParam   beWriteEnginePassParam;
    WriteEnginePassParam   rowSumWriteEnginePassParam;
    WriteEnginePassParam   colSumWriteEnginePassParam;
}
IFEPassOutput;


//#ifdef __cplusplus
//extern "C" {
//#endif
//titanIFEstripingInternalControls * deriveFetchRange_titanIFE(stripingInput_titanIFE * pStripingInput, IFEPassOutput * pPassOut);
//#ifdef __cplusplus
//}
//#endif


#ifdef __cplusplus
extern "C" {
#endif
//int32_t deriveStriping_titanIFE(stripingInput_titanIFE * pStripingInput, titanIFEstripingInternalControls * pInternalControls, IFEPassOutput* pPassOut);
int32_t deriveStriping_titanIFE(stripingInput_titanIFE * pStripingInput, IFEPassOutput* pPassOut);
int32_t deriveFetchRange_titanIFE(stripingInput_titanIFE * pStripingInput);
void ReleaseDualIfePassResult(IFEPassOutput *ife_pass_output);
#ifdef __cplusplus
}
#endif

#endif
