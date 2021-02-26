#ifndef _MODULES_H_
#define _MODULES_H_

// TODO : move all deprecated structs to internal header files.

//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  modules.h
// @brief striping output for each IPE module
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */
#include "stripe_common.h"
#include "scalers.h"


/*------------------------------------------------------------------------
*       Definitions and Constants
* ----------------------------------------------------------------------- */
#ifndef AEE_SUCCESS
#define AEE_SUCCESS 0
#endif //AEE_SUCCESS

#ifndef AEE_EFAIL
#define AEE_EFAIL 1
#endif //AEE_EFAIL

#ifndef ASSERT
#define ASSERT(c) {if (!(c)) return AEE_EFAIL;}
#endif

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef Q1
#define Q1 21
#endif

#ifndef BF_STATS_RGNCOUNT_V23
#define BF_STATS_RGNCOUNT_V23 182
#endif

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(4)

typedef struct
{
    int16_t en;
    int16_t mode;
}
ICAv10InputParams;

typedef struct
{
    int32_t DCUSInCropStartX;   // horizontal input crop start in DC pixels. must be even
    int32_t DCUSInCropEndX;     // horizontal input crop end in DC pixels. must be odd
    int32_t DCUSOutCropStartX;  // horizontal input crop start in upscaled pixels. must be even
    int32_t DCUSOutCropEndX;    // horizontal input crop end in upscaled pixels. must be odd -
                                // counted from the start of the upscaled image before left cropping
    int32_t SIM_DCUSInCropStartY;   // SIMULATOR ONLY PARAM -in HW implemented by setting the FE window.
                                    // vertical input crop start in DC pixels. must be even
    int32_t SIM_DCUSInCropEndY;     // SIMULATOR ONLY PARAM -in HW implemented by setting the FE window.
                                    // vertical input crop end in DC pixels. must be odd
    int32_t DCUSOutCropStartY;  // vertical input crop start in upscaled pixels. must be even
    int32_t DCUSOutCropEndY;    // vertical input crop end in upscaled pixels. must be odd -
                                // counted from the start of the upscaled image before top cropping
}
StripeParamsANRDCUS;

typedef struct
{
    uint16_t data_collect_en;
    uint16_t ds_en;
    uint16_t ds_l_in;               //n means n+1 from 8994
    uint16_t ds_l_out;              //n means n+1 from 8994
    uint16_t ds_skip_cnt;
    uint16_t ds_start_count;
    uint32_t ds_phase_init;
    uint32_t ds_phase_multi;
    uint16_t ds_q2;
    uint16_t ds_input_l;                //n means n+1 from 8994
    uint16_t dc_init_cellnum_x;  // 4
    uint16_t dc_init_dx;         // 5
    uint16_t dc_3d_sum_clear;    // 1
    uint16_t dc_conv_start_cell_x;  // 4u    Guo to calculate,  execution is from left to right,   striping calculation is right to left
    uint16_t dc_conv_end_cell_x;    // 4u  Guo to calculate,  execution is from left to right,   striping calculation is right to left
    uint16_t dc_xstart;             // 8u     region controller    note:  within each stripe, index starts with 0
    uint16_t dc_xend;               // 8u     region controller    striping to calculate
    uint16_t dummy_field_for_byte_padding;
}
LTMV13StripeDCParams;

typedef struct
{
   uint16_t ip_cellsize;     // 4
   uint16_t ip_inv_cellsize;           // 11
   uint16_t ip_init_cellnum;     // 4
   uint16_t ip_init_d;           // 11
   uint16_t ip_init_p;           // 18
   int16_t dummy_field_for_byte_padding;
}
LTMV13StripeIPParams_1D;

typedef struct
{
    LTMV13StripeDCParams *dc_out_x;
    LTMV13StripeDCParams *dc_out_y;
    LTMV13StripeIPParams_1D *ip_out_x;
    LTMV13StripeIPParams_1D *ip_out_y;
    int16_t ds_fac;
    int16_t dummy_field_for_byte_padding;
}
LTMv13StripingOutputParams;


typedef struct
{
    uint32_t CYLPFLGYStartImage;
    uint32_t CYLPFLGYExtStartImage;
    uint32_t CYLPFLGXStartImage;
    uint32_t CYLPFLGYRelIn;
    uint32_t CYLPFLGYExtRelInc;
    uint32_t CYLPFLGXRelInc;
}
ANRLNRInputParams;

typedef struct
{
    BOOL     RNF_BYPASS__LUMA;
    BOOL     RNF_BYPASS__CHROMA;
    BOOL     CYLPF_Bypass;
    BOOL     CYLPF_Filter2_Bypass;
    BOOL     DCUS_DITHERING_Y_En;
    BOOL     DCUS_DITHERING_C_En;
    uint32_t CYLPF_DITHERING_seed;
    uint32_t DCUS_DITHERING_seed;
    uint32_t CYLPF_LG_Ystart    ;
    uint32_t CYLPF_LG_YExtStart ;
    uint32_t CYLPF_LG_Xstart    ;
    uint32_t CYLPF_LG_YRelInc   ;
    uint32_t CYLPF_LG_YExtRelInc;
    uint32_t CYLPF_LG_XRelInc   ;
}
ANRv10StripingInputParams;

typedef struct
{
    int16_t CNR_FD_Offset;
    int16_t DCUS_IN_CROP_START;
    int16_t DCUS_IN_CROP_END;
    int16_t DCUS_OUT_CROP_START;
    int16_t DCUS_OUT_CROP_END;
    int16_t lnrStart;
    int32_t CYLPF_LG_start;
}
ANRv10Striping1dOutputParams;

typedef struct
{
    // TODO : remove pointers: out_x and out_y
    ANRv10Striping1dOutputParams *out_x;
    ANRv10Striping1dOutputParams *out_y;
    int32_t CYLPF_DITHERING_seed;
    int32_t DCUS_DITHERING_seed;
    int32_t CYLPF_LG_YExtStart;
    int16_t PDI_Pack_StartPhase;
    int16_t PDI_Pack_Crop_HorizStart;
    int16_t PDI_Pack_Crop_HorizEnd;
    int16_t dummy_field_for_byte_padding;
    BOOL    pdi_fetch_en;
}
ANRv10StripingOutputParams;

typedef struct
{
    int16_t firstOut;
    int16_t lastOut;
}
CRCv10Striping1dOutputParams;

typedef struct
{
    BOOL enable;
    CRCv10Striping1dOutputParams *out_x;
    CRCv10Striping1dOutputParams *out_y;
}
CRCv10StripingOutputParams;

typedef struct
{
    BOOL en;
    BOOL is_tfi_disable;
    BOOL morph_Erode_En;
    BOOL morph_Dilate_En;
    BOOL ditherY;
    BOOL ditherCb;
    BOOL ditherCr;
    BOOL useImgAsMctfpInput;    //during TF blending, the original image is used as the ref in. we therefore doesn't need to configure ref fetch
    int16_t TfOut_CropOutHorizAssumeSize;       //    ASSUME_HORIZ_FILTER_SIZE_NONE = 0, ASSUME_HORIZ_FILTER_SIZE_5 = 1, ASSUME_HORIZ_FILTER_SIZE_13 = 2,
    int16_t trEnable;       //0 for FULL pass
    int16_t trStartX; // range: 1...276
    int16_t trBlockNumX; // it can be 1...10 per image
    int16_t trBlockSizeX;
    int16_t dummy_field_for_byte_padding;
    int32_t prngSeed;
    int32_t LnrStartIdxH;
    int32_t LnrScaleH;
    int32_t LnrStartIdxV;
    int32_t LnrScaleV;
}
TFv10StripingInputParams;

typedef struct
{
    CRCv10Striping1dOutputParams imgCropIn;
    CRCv10Striping1dOutputParams mctfpCropIn;
    CRCv10Striping1dOutputParams outIndicationsCropOut;
}
TFv10Striping1dOutputParams;


typedef struct
{
    uint16_t input_l;
    uint16_t output_l;
    int16_t  preload;
    int16_t dummy_field_for_byte_padding;
    uint32_t phase_init;
    uint32_t phase_step;
}
Upscalev20Striping1dOutputParams;

typedef struct
{
    BOOL op_mode_en;
    Upscalev20Striping1dOutputParams *out_x;
    Upscalev20Striping1dOutputParams *out_y;
}
Upscalev20StripingOutputParams;


typedef struct
{
    BOOL tfi_in_en;
    BOOL useIndications;
    int16_t trEnable;
    int16_t trStartX;
    int16_t trBlockNumX;
    int16_t trStartBlockIndex;              //to configure the write engine; to be removed once we set up write engine interfaces
    int32_t prngSeed;
    int32_t LnrStartIdxH;
    int32_t LnrStartIdxV;
    CRCv10StripingOutputParams imgCropIn;
    CRCv10StripingOutputParams mctfpCropIn;
    CRCv10StripingOutputParams outIndicationsCropOut;
    BOOL    Morph_Erode_cropIn_horizLeft;
    BOOL    Morph_Erode_cropIn_horizRight;
    int16_t indUpscale_cropOutHorizLeft;
    int16_t indUpscale_cropOutHorizRight;
    int16_t indUpscale_cropOutVertTop   ;
    int16_t indUpscale_cropOutVertBottom;
    BOOL    indUpscale_cropIn_horizLeft;    // TODO : why BOOL and not uint16_t ?
    BOOL    indUpscale_cropIn_horizRight;   // TODO : why BOOL and not uint16_t ?
    int16_t Morph_Erode_cropOutVert_start;
    int16_t Morph_Erode_cropOutVert_end;
    int16_t Morph_Dilate_cropOutVert_start;
    int16_t Morph_Dilate_cropOutVert_end;
    BOOL    tfRefRangeInROI;
}
TFv10StripingOutputParams;

typedef struct
{
    BOOL tfi_in_en;
    BOOL useIndications;
    int16_t trEnable;
    int16_t trStartX;
    int16_t trBlockNumX;
    int16_t trStartBlockIndex;              //to configure the write engine; to be removed once we set up write engine interfaces
    int32_t prngSeed;
    int32_t LnrStartIdxH;
    int32_t LnrStartIdxV;
    CRCv10StripingOutputParams imgCropIn;
    CRCv10StripingOutputParams mctfpCropIn;
    CRCv10StripingOutputParams outIndicationsCropOut;
    BOOL    Morph_Erode_cropIn_horizLeft;
    BOOL    Morph_Erode_cropIn_horizRight;
    int16_t indUpscale_cropOutHorizLeft;
    int16_t indUpscale_cropOutHorizRight;
    int16_t indUpscale_cropOutVertTop   ;
    int16_t indUpscale_cropOutVertBottom;
    BOOL    indUpscale_cropIn_horizLeft;    // TODO : why BOOL and not uint16_t ?
    BOOL    indUpscale_cropIn_horizRight;   // TODO : why BOOL and not uint16_t ?
    int16_t Morph_Erode_cropOutVert_start;
    int16_t Morph_Erode_cropOutVert_end;
    int16_t Morph_Dilate_cropOutVert_start;
    int16_t Morph_Dilate_cropOutVert_end;
    BOOL    tfRefRangeInROI;
	uint8_t lmCropAfterUpscale_left;
	uint8_t lmCropAfterUpscale_right;
	uint8_t lmCropAfterUpscale_top;
	uint8_t lmCropAfterUpscale_bottom;
	BOOL    lmCropOut_enable;
	uint16_t lmCropOut_horizStart;
	uint16_t lmCropOut_horizEnd;
	uint16_t lmCropOut_vertStart;
	uint16_t lmCropOut_vertEnd;
}
TFv20StripingOutputParams;

typedef struct
{
	uint16_t US4InCropV;
	uint16_t US4InCropH;
	uint16_t US4OutCropV;
	uint16_t US4OutCropH;
}
LENRv10StripingOutputParams;

typedef struct
{
    BOOL enable;
    BOOL cosited;
    BOOL is_odd;                //0: even
}
ChromaUpv20StripingInputParams;


typedef struct
{
    uint16_t enable;
    uint16_t output_l;          //n means n+1
    int16_t  index_offset;
    int16_t  rounding_pattern;
    uint32_t initial_phase;
}
ChromaUpv201dStripingOutputParams;

typedef struct
{
    ChromaUpv201dStripingOutputParams *out_x;
    ChromaUpv201dStripingOutputParams *out_y;
}
ChromaUpv20StripingOutputParams;


typedef struct
{
    BOOL enable;
    BOOL cac2_en;
    BOOL snr_en;
}
CACv22StripingInputParams;

typedef struct
{
    BOOL enable;
    BOOL mask_filter_en;
    BOOL img_process_en;
	BOOL data_collect_en;
	BOOL data_collection_at_DC16_pass;               //1: use DC16 to do LTM data collection
}
LTMv13StripingInputParams;

typedef struct
{
    BOOL enable;
    BOOL edge_alignment_enable;
    BOOL contrast_enable;
    BOOL layer_2_enable;
    BOOL layer_1_enable;
}
ASFv30StripingInputParams;

typedef struct
{
    int16_t face_horizontal_offset;
    int16_t face_vertical_offset;
    int16_t r_square_shift;
    int16_t r_square_scale;
    int32_t initial_r_square;
    int16_t initial_offset_minus_center_horizontal;
    int16_t initial_offset_minus_center_vertical;
}
ASFv30StripingOutputParams;

//
//typedef struct
//{
//}
//Anrv1OutputParams;


typedef struct
{
    int32_t needLeftStripe;
    int32_t needRightStripe;
    int32_t statsLeftEnd;
} ifeGridPartition;

typedef struct
{
    int32_t leftStripeRgnCnt;
    int32_t rightStripeRgnCnt;
    int32_t statsLeftEnd;
    int32_t statsRightStart;
} ifeBAFPartition;

typedef struct
{
    int32_t IPUCYLPFLGYStart; // this is a HW register
    int32_t IPUCYLPFLGYExtStart; // this is a HW register
    int32_t IPUCYLPFLGXStart; // this is a HW register
}
StripeParamsANRLNR;

typedef struct
{
    int32_t LNRStartIdxH;
    int32_t LNRStartIdxV;
}
StripeParamsTFLNR;

typedef struct
{
    // data-collection
    uint16_t dcInitCellNumX;
    uint16_t dcInitDX;

    // down-scaling
    uint16_t dsHorizontalStartCount;
    uint16_t dsVerticalStartCount;
    uint32_t dsHorizontalPhaseInit;
    uint32_t dsVerticalPhaseInit;
    uint16_t dsHorizontalSkipCnt;
    uint16_t dsVerticalSkipCnt;

    // image processing
    uint16_t ipInitCellNumX;
    uint16_t ipInitDX;
    uint32_t ipInitPX;

    uint32_t ipCellWidth;
}
LTMInStructIPE_V13;

typedef struct
{
    uint16_t input_l;                   //: the counter N; n means n. M and N combined with IFE input size can determine the MNDS output size (effective only when use_zoom_setting_from_external is TRUE)
    uint16_t output_l;                  //: the counter M; n means n. M and N combined with IFE input size can determine the MNDS output size (effective only when use_zoom_setting_from_external is TRUE)

    int32_t rounding_option_h;
    int32_t rounding_option_v;
}
mndsInStruct_v16;

typedef struct
{
    int32_t bx;
    int32_t by;
    int32_t r_square_init;
    int32_t r_square_shft;
    int32_t r_square_scale;
}
abfOutStruct_v34;

typedef struct
{
    int32_t pdaf_global_offset_x;
    int32_t pdaf_x_end;
    uint32_t PDAF_PD_Mask[64];
}
pdpcOutStruct_v11;

typedef struct
{
    int32_t pdaf_global_offset_x;
    int32_t pdaf_x_end;
    uint32_t PDAF_PD_Mask[64];
}
pdpcInStruct_v11;

typedef struct
{
    int16_t hdr_zrec_first_rb_exp;
}
hdrInStruct_v20;


typedef struct
{
    int16_t hdr_zrec_first_rb_exp;
    int16_t dummy_field_for_byte_padding;
}
hdrOutStruct_v20;

typedef struct
{
    uint16_t en;
    uint16_t input_l;                   //: the counter N; n means n
    uint16_t output_l;                  //: the counter M; n means n
    uint16_t pixel_offset; 
    uint16_t cnt_init;
    uint16_t interp_reso;
    uint16_t rounding_option_v;         //: we might need to adjust rounding mode based on the output pixel locations
    uint16_t rounding_option_h;         //: we might need to adjust rounding mode based on the output pixel locations
    uint16_t right_pad_en;
    uint16_t input_processed_length;    // this is used to control the right crop when h_right_pad_en = 1; n means n
    uint32_t phase_init;
    uint32_t phase_step;
}
MNScaleDownInStruct_V16_1D;  // HW parameters

typedef struct
{
    int16_t trEnable;
    int16_t trStartX; // range: 1...276
    int16_t trBlockNumX; // it can be 1...10 per image
    int16_t trBlockSizeX;
}
TFrefinementInputParams;

typedef struct
{
    int16_t trEnable;
    int16_t trStartX; // range: 1...276
    int16_t trBlockNumX; // it can be 1...10 per image
    int16_t trStartBlockIndex;          //the first block index of a stripe
}
TFrefinementOutputParams;

typedef struct
{
    BOOL enable;
}
BasicModuleInput;


typedef struct
{
    MNScaleDownInStruct_V20_1D *out_x;
    MNScaleDownInStruct_V20_1D *out_y;
}
MNDSv20StripingOutputParams;  // HW parameters


typedef struct
{
    uint32_t phase_init;
    uint16_t preload;
    int16_t dummy_field_for_byte_padding;
}
upscale_v20stripingInputParams1D;

typedef struct
{
    upscale_v20stripingInputParams1D upscalar1D_h;
    upscale_v20stripingInputParams1D upscalar1D_v;
}
upscale_v20stripingInputParams_old;

typedef struct
{
    BOOL enable;
    uint32_t y_phase_init_h;
    uint32_t y_phase_init_v;
    uint16_t y_preload_h;
    uint16_t y_preload_v;
}
Upscalarv20StripingInputParams;

typedef struct
{
    int16_t c_scale_in_cfg_rounding_option_h;
    int16_t c_scale_in_cfg_rounding_option_v;
}
MNDSv20StripingInputParams;             //IPE: the DS ratio is decided by other high level parameters

typedef struct
{
    BOOL ENABLE_Y;
    BOOL ENABLE_C;
}
DS4v10StripingInputParams;

typedef struct
{
    uint32_t grain_seed;
    uint32_t mcg_a;
    uint32_t skip_ahead_a_jump;
}
GRAv10stripingInputParams;

typedef struct
{
    uint32_t grain_seed;
}
GRAv10stripingOutputParams;


typedef struct
{
    int32_t pdaf_global_offset;
    int32_t pdaf_table_offset;
    int32_t pdaf_x_end;
    int32_t pdaf_zzHDR_first_rb_exp;
}
PDPCv20stripingOutputParams;
typedef struct
{
    BOOL enable;
    int32_t pdaf_global_offset_x;
    int32_t pdaf_table_x_offset;
    int32_t pdaf_x_end;
    int32_t pdaf_zzHDR_first_rb_exp;
    int16_t pdaf_pdpc_en;
    int16_t pdaf_dsbpc_en;
}
PDPCv20stripingInputParams;
typedef struct
{
    int32_t pdaf_global_offset_x;
    int32_t pdaf_x_end;
    int32_t pdaf_zzHDR_first_rb_exp;
    uint32_t PDAF_PD_Mask[64];
}
PDPCv11stripingInputParams;
typedef struct
{
    int32_t pdaf_global_offset;
    int32_t pdaf_x_end;
    int32_t pdaf_zzHDR_first_rb_exp;
    uint32_t PDAF_PD_Mask[64];
}
PDPCv11stripingOutputParams;

typedef struct
{
    BOOL hdr_zrec_first_rb_exp;         //: shift of zigzag pattern when the striping boundary is not aligned with it   // 0-1
    BOOL hdr_mac_smooth_filter_en;      //:   if mac enable,  add 11x1 filter, 5 each side
    int16_t hdr_mac_dilation;           //: configurable dilation size of HDR MAC    // 0-5  (11x1, 5 each side)
    int16_t moduleHDRRecOption;   //  0 = iHDR, 1 = zzHDR
    int16_t moduleEnableHDRRecon;
    int16_t moduleEnableHDRMAC;
    int16_t hdr_rec_linear_mode;
    int16_t hdr_mac_linear_mode;
    int16_t hdr_exp_ratio;
    int16_t dummy_field_for_byte_padding;
}
HDRv20stripingInputParams;


typedef struct
{
    int16_t hdr_zrec_first_rb_exp;
}
HDRv20stripingOutputParams;


typedef struct
{
    BOOL enable;
    int32_t enable_gic;
    int32_t enable_pnr;
}
GICv30InputParams;


typedef struct
{
    int16_t bx;
    int16_t by; // TODO : move this to the pass level, no need to keep many copies of the same variable
    int32_t r_square_init;
    int16_t r_square_shift;
    int16_t r_square_scale;
}
RnrStripingOutputParams;

typedef struct
{
    RnrStripingOutputParams rnr_out;
    uint16_t face_horizontal_offset;
    uint16_t face_vertical_offset;
}
HNRv10StripingOutputParams;

typedef struct
{
    int32_t bihist_enabled;
    int32_t bihist_roi_h_offset;           //
    int32_t bihist_rgn_h_num;              //
}
HDRBayerHist_v13InputParams;

typedef struct
{
    int32_t bihist_enabled;
    int32_t bihist_roi_h_offset;           //
    int32_t bihist_rgn_h_num;              //
    int32_t writeEngineEnable;              //1 at the stripe that covers ROI end and 0 otherwise
}
HDRBayerHist_v13OutputParams;

typedef struct
{
    int32_t bg_enabled;
    int32_t bg_rgn_v_num;               //for stats packing only
    int32_t bg_rgn_h_num;
    int32_t bg_region_sampling;
    int32_t bg_rgn_width;
    int32_t bg_roi_h_offset;
    int32_t bg_sat_output_enable;
    int32_t bg_Y_output_enable;
}
BayerGridInputParam;



typedef struct
{
    int32_t bg_enabled;
    int32_t bg_rgn_v_num;               //to configure write engine
    int32_t bg_rgn_h_num;
    int32_t bg_region_sampling;
    int32_t bg_rgn_width;
    int32_t bg_roi_h_offset;
    int32_t bg_sat_output_enable;
    int32_t bg_Y_output_enable;
}
BayerGridv15InputParam;

typedef struct
{
    int32_t bg_enabled;
    int32_t gridOutStart_x;             //for stats packing
    int32_t gridOutEnd_x;                   //for stats packing
    int32_t bg_rgn_h_num_frame;         //for stats packing, n means n+1
    int32_t bg_roi_h_offset;
    int32_t bg_rgn_h_num_stripe;        //n means n+1
}
BayerGridv15OutputParam;

typedef struct
{
    int32_t be_enable;
    int32_t be_zHDR_first_rb_exp;   // 1u, 0x0: T1 (long exp), 0x1: T2 (short exp)
    int32_t be_roi_h_offset;        // 13u
    int32_t be_rgn_width;           // 9u
    int32_t be_rgn_h_num;           // 7u,
    int32_t be_rgn_v_num;           // 7u,
    int32_t be_sat_output_enable;   // 1u, 0x0: disable sat data collection, 0x1: enable sat data collection
    int32_t be_Y_output_enable;
}
BayerExpv15InputParam;

typedef struct
{
    int32_t be_enable;
    int32_t be_roi_h_offset;        // 13u
    int32_t be_rgn_h_num;           // 7u,
    int32_t be_zHDR_first_rb_exp;   // 1u, 0x0: T1 (long exp), 0x1: T2 (short exp)
    int32_t gridOutStart_x;
}
BayerExpv15OutputParam;


typedef struct
{
    BOOL enable;
    int16_t fnr_en;
    int16_t snr_en;
    int16_t snr_skin_smoothing_str;
    int16_t dummy_field_for_byte_padding;
}
HNRInputParam;

typedef struct
{
    int32_t bufStride1;
    int32_t stripeHeight1;
    int32_t bufStride2;
    int32_t stripeHeight2;
}
WriteEnginePassParam;

typedef struct
{
    int32_t enable1;
    int32_t hInit1;
    int32_t stripeWidth1;
    int32_t enable2;
    int32_t hInit2;
    int32_t stripeWidth2;
}
WriteEngineStripeParam;

typedef struct
{
    int32_t bg_enabled;
    int32_t gridOutStart_x;             //for stats packing
    int32_t gridOutEnd_x;                   //for stats packing
    //int32_t bg_rgn_width;             //for stats packing, n means n+1
    int32_t bg_rgn_v_num;               //for stats packing, n means n+1
    int32_t bg_rgn_h_num_frame;         //for stats packing, n means n+1
    int32_t bg_region_sampling_frame;   //for stats packing
    int32_t bg_first_hor_rgn_width;     //n means n+1
    int32_t bg_last_hor_rgn_width;      //n means n+1
    int32_t bg_region_sampling_stripe;
    int32_t bg_roi_h_offset;
    int32_t bg_rgn_h_num_stripe;        //n means n+1
}
BayerGridOutputParam;

typedef struct{
    //unsigned int grid_index;                  // Start block x index, 6u
    //unsigned int subgrid_index;                   // Start subgrid x index within start block, 3u
    //unsigned int pixel_index_within_subgrid;    // x coordinate of top left pixel in start block/subgrid, 9u
    uint32_t grid_index;                    // Start block x index, 6u
    uint32_t subgrid_index;                 // Start subgrid x index within start block, 3u
    uint32_t pixel_index_within_subgrid;    // x coordinate of top left pixel in start block/subgrid, 9u
} RollOffOutputParameters;

typedef struct{
    uint16_t Lx_start_l;                    // Start block x index, 6u
    uint16_t Bx_start_l;                    // Start subgrid x index within start block, 3u
    uint16_t Bx_d1_l;                       // x coordinate of top left pixel in start block/subgrid, 9u
    uint16_t Lx_start_r;                    // Start block x index, 6u
    uint16_t Bx_start_r;                    // Start subgrid x index within start block, 3u
    uint16_t Bx_d1_r;                       // x coordinate of top left pixel in start block/subgrid, 9u
}
Rolloffv34Pedestalv13StripingOutputParams;

typedef struct{
    BOOL enable;
    uint16_t Bwidth_l;                      // Subgrid width, 9u, 2*Bwidth is the real width, n mean n+1
    uint16_t MeshGridBwidth_l;              // Meshgrid width, 9u, 2*MeshGridBwidth is the real width, n mean n+1
                                                // not used in rolloff implementation, only as HW counters
    uint16_t Lx_start_l;                    // Start block x index, 6u
    uint16_t Bx_start_l;                    // Start subgrid x index within start block, 3u
    uint16_t Bx_d1_l;                       // x coordinate of top left pixel in start block/subgrid, 9u
    uint16_t num_meshgain_h;                // Number of horizontal mesh gains, n mean n+2
} RollOffv34InputParameters;

typedef struct{
    //unsigned int grid_index;                  // Start block x index, 6u
    //unsigned int subgrid_index;                   // Start subgrid x index within start block, 3u
    //unsigned int pixel_index_within_subgrid;    // x coordinate of top left pixel in start block/subgrid, 9u
    uint32_t grid_index;                    // Start block x index, 6u
    uint32_t subgrid_index;                 // Start subgrid x index within start block, 3u
    uint32_t pixel_index_within_subgrid;    // x coordinate of top left pixel in start block/subgrid, 9u
} PedestalOutputParameters;

typedef struct{
    BOOL enable;
    uint16_t Bwidth_l;                              // Subgrid width, 11u, 2*Bwidth is the real width   (n means n+1)
    uint16_t MeshGridBwidth_l;                      // Meshgrid width, 11u, 2*MeshGridBwidth is the real width  (n means n+1)
    uint16_t Lx_start_l;                            // Start block x index, 4u
    uint16_t Bx_start_l;                            // Start subgrid x index within start block, 3u
    uint16_t Bx_d1_l   ;                            // x coordinate of top left pixel in start block/subgrid, 11u
    uint16_t dummy_field_for_byte_padding;
} Pedestalv13InputParams;

typedef struct
{
    uint64_t baf_roi_indexLUT[BF_STATS_RGNCOUNT_V23];
    uint16_t baf_h_scaler_en;
    uint16_t baf_fir_h1_en;
    uint16_t baf_iir_h1_en;
    uint16_t dummy_field_for_byte_padding;
    MNScaleDownInStruct_V16_1D mndsParam;
}
BayerFocusv23InputParam;

typedef struct
{
    uint64_t baf_roi_indexLUT[BF_STATS_RGNCOUNT_V23];
    MNScaleDownInStruct_V16_1D mndsParam;
    uint16_t enable;
    uint16_t dummy_field_for_byte_padding;
}
BayerFocusv23OutputParam;

typedef struct
{
    int32_t enable;
    int32_t hist_rgn_h_offset;
    int32_t hist_rgn_h_num;
}
iHistv12InputParam;

typedef struct
{
    int32_t enable;
    int32_t hist_rgn_h_offset;
    int32_t hist_rgn_h_num;
}
iHistv12OutputParam;

typedef struct
{
    int32_t rs_enable;              // 1u
    int32_t cs_enable;              // 1u
    int32_t rs_rgn_h_offset;        // 13
    int32_t rs_rgn_width;           // 13
    int32_t rs_rgn_h_num;           // 4
    int32_t rs_rgn_v_num;           // 4
    int32_t cs_rgn_h_offset;        // 13
    int32_t cs_rgn_width;           // 2
    int32_t cs_rgn_h_num;           // 11
    int32_t cs_rgn_v_num;           // 11
}
rowSumColSumv14InputParam;

typedef struct
{
    int32_t rs_enable;              // 1u
    int32_t cs_enable;              // 1u
    int32_t rs_rgn_h_offset;        // 13
    int32_t rs_rgn_h_num;           // 4
    int32_t cs_rgn_h_offset;        // 13
    int32_t cs_rgn_h_num;           // 11
}
rowSumColSumv14OutputParam;

#pragma pack(pop)

/*------------------------------------------------------------------------
*       External Variables
* ----------------------------------------------------------------------- */

/*------------------------------------------------------------------------
*       Function Declarations
* ----------------------------------------------------------------------- */

int genTFrefinementFirstBlock(const BOOL striping, const BOOL edgeStripeLT, const int tfROIstart, const int tfInStart, const int tfInEnd, const int TFrefineKernel, const int tfRefineLastBlk,
                              const TFrefinementInputParams frameParam );
int genTFrefinementLastBlock(const BOOL edgeStripeRB, const int tfROIstart, const int tfInStart, const int tfInEnd, const int TFrefineKernel, const int cumulativeTFrefineBlk,
                             const TFrefinementInputParams frameParam );
//void tfRefinementConfig(const int tfROIstart, const int tfInStart, const int tfInEnd, const int TFrefineKernel, const int cumulativeTFrefineBlk,
//                      const TFrefinementInputParams frameParam, TFrefinementOutputParams* pStripeParam );
void tfRefinementConfig(const int tfROIstart, const int tfInStart, const int tfRefineFirstBlk, const int tfRefineLastBlk,
                        const TFrefinementInputParams frameParam, TFrefinementOutputParams* pStripeParam );


void pdaf_stripingSetup(const PDPCv20stripingInputParams* frameLevelParam, const int stripeStartX, const int stripeEndX, PDPCv20stripingOutputParams * pStripeLevelParam );
void pdpcv11_stripingSetup(const PDPCv11stripingInputParams* frameLevelParam, const int stripeStartX, PDPCv11stripingOutputParams * pStripeLevelParam );        //for IFE

void bayerGrid_stripingSetup(const BayerGridInputParam* frameLevelParam, const int frameWidth, const int stripeInStart,
                             const int stripeOutStart, const int stripeOutEnd, BayerGridOutputParam* pStripeLevelParam );
void configureBGwriteEnginePassParam(const BayerGridInputParam* frameLevelParam, WriteEnginePassParam* pBgStatsWriteEnginePassParam );
void configureBGWriteEngineStripeParam(const BayerGridInputParam* frameLevelParam, const BayerGridOutputParam* bayerGridParam, WriteEngineStripeParam* pBgStatsWriteEngineStripeParam);

int pedestal_apply_hw_constraint(const int minFetchStart, const Pedestalv13InputParams *frameParams, const int raw_width,
                                 PedestalOutputParameters* pedestalPerStripe, int formatConstraint, int ignoreLatencyConstraint);
int rolloff_apply_hw_constraint(const int raw_width, const int start_x, const RollOffv34InputParameters *frameParams,
                                RollOffOutputParameters* rolloffPerStripe,
                                const int ignoreLatencyConstraint, const int expand_fetch_for_dual_ife);

int32_t
createStripeConfigsANRLNRX(const int xStart,   // relative to main data path of ANR
                           const BOOL leftStripe,
                           const uint32_t CYLPFLGXStartImage,
                           const uint32_t CYLPFLGXRelInc,
                           int32_t *IPUCYLPFLGXStart);

int32_t
createStripeConfigsANRLNRY(const int yStart,   //
                           const uint32_t CYLPFLGYStartImage,
                           const uint32_t CYLPFLGYExtStartImage,
                           const uint32_t CYLPFLGYRelInc,
                           const uint32_t CYLPFLGYExtRelInc,
                           int32_t *IPUCYLPFLGYStart,
                           int32_t *IPUCYLPFLGYExtStart);
int32_t
LTM4CreateStripeCfgs(uint16_t fullImgWidth,
                     uint16_t fullImgHeight,
                     uint16_t stripeStart,
                     uint16_t stripeWidth,
                     uint16_t stripeHeight,
                     uint16_t leftPad,
                     uint16_t rigtPad,
                     uint16_t topPad,
                     uint16_t botPad,
                     LTMInStructIPE_V13 *LTMCfg);


// :: many of those functions are called only once and are very simple, consider making then inline or at least static.
int32_t configureGRAv10(const uint16_t xStart, const GRAv10stripingInputParams* gra_frameParam, GRAv10stripingOutputParams* gra_stripeParam);

// TODO : unused API - remove if not needed
//void rscsv14_IFEstripingSetup(const rowSumColSumv14InputParam frameParam, const int leftStripe, const int stripeFetchStartX,
//                             const int leftStripeEnd, const int stripeFetchEndX, rowSumColSumv14OutputParam *stripeParam);

void iHistv12_IFEstripingSetup(const iHistv12InputParam* frameParam, const int leftStripe, const int stripeFetchStartX,
                               const int leftStripeEnd, const int stripeFetchEndX, iHistv12OutputParam *stripeParam);
void hdrBhistv13_IFEstripingSetup(const HDRBayerHist_v13InputParams* frameParam, const int leftStripe, const int stripeFetchStartX,
                                  const int leftStripeEnd, const int stripeFetchEndX, HDRBayerHist_v13OutputParams *stripeParam);
void bayerGridv15_IFEstripingSetup(const BayerGridv15InputParam* frameLevelParam, const int leftStripe, const ifeGridPartition* ifePartition,
                                   const int fetchStart, const int fetchEnd, BayerGridv15OutputParam* pStripeLevelParam );
void bayerExpv15_IFEstripingSetup(const BayerExpv15InputParam* frameLevelParam, const int leftStripe, const ifeGridPartition* ifePartition,
                                   const int fetchStart, const int fetchEnd, BayerExpv15OutputParam* pStripeLevelParam );

void configureBGv15writeEnginePassParam  (const BayerGridv15InputParam* frameLevelParam, WriteEnginePassParam* pBgStatsWriteEnginePassParam );
void configureBGv15WriteEngineStripeParam(const BayerGridv15InputParam* frameLevelParam, const BayerGridv15OutputParam* bayerGridParam, WriteEngineStripeParam* pBgStatsWriteEngineStripeParam);
void configureBEv15WriteEngineStripeParam(const BayerExpv15InputParam* frameLevelParam, const BayerExpv15OutputParam* stripeParam, WriteEngineStripeParam* pWriteEngineStripeParam);
void configureBEv15writeEnginePassParam  (const BayerExpv15InputParam* frameLevelParam, WriteEnginePassParam* pWriteEnginePassParam );

void configureGridWriteEnginePassParam  (const int bytePerGrid, const int gridCntH, const int gridCntV, WriteEnginePassParam* pWriteEnginePassParam );
void configureGridWriteEngineStripeParam(const int bytePerGrid, const int gridWidth, const int offset_frame, const int stripeFetchStart,
                                         const int offset_stripe, const int stripeGridCnt, WriteEngineStripeParam* pWriteEngineStripeParam);


int updateLeftFetch_BAFv23(const int originalFetchLeftEnd, const ifeBAFPartition* bafPartition, const int pixelCrop, const BayerFocusv23InputParam* frameParam ,
                           const MNScaleDownInStruct_V16_1D* mndsFrameParam );
int updateRightFetch_BAFv23(const int originalFetchRightStart, const ifeBAFPartition* bafPartition,
                            const int pixelCrop, const BayerFocusv23InputParam* frameParam,
                            const MNScaleDownInStruct_V16_1D* mndsFrameParam );
void bayerFocusv23_IFEstripingSetup(const BayerFocusv23InputParam* frameParam, const MNScaleDownInStruct_V16_1D* mndsFrameParam,
                                    const int leftStripe, const ifeBAFPartition* ifePartition, const int stripeFetchStartX, const int stripeFetchEndX,
                                    BayerFocusv23OutputParam *stripeParam, WriteEnginePassParam* pWriteEnginePassParam,
                                    WriteEngineStripeParam* pWriteEngineStripeParam );

void rsv14_IFEstripingSetup(const rowSumColSumv14InputParam* frameParam, const int leftStripe,
                            const int stripeFetchStartX, const int stripeFetchEndX,
                            const int leftStripeEnd, rowSumColSumv14OutputParam *stripeParam);
void csv14_IFEstripingSetup(const rowSumColSumv14InputParam* frameParam, const int leftStripe,
                            const int stripeFetchStartX, const int stripeFetchEndX,
                            const int leftStripeEnd, rowSumColSumv14OutputParam *stripeParam);


void GRAv10MultiStripeConfig( const int16_t num_stripes, const uint16_t *gra_start_x_list,
                             const GRAv10stripingInputParams* gra_frame_in, uint32_t *gra_grain_seeds);
void NpsDitheringMultiStripeConfig(const int16_t num_stripes, const uint16_t *start_x_list, const uint16_t start_y,
                                   const int32_t seed_frame, uint32_t *dithering_seeds);
void configure_square_parameters(uint32_t posFromUpperLeft, uint32_t shiftMax ,int16_t width, int16_t height, int16_t *r_square_scale, int16_t *r_square_shift);

uint16_t ChromaUpV20RightLumaKernel(void);
uint16_t ChromaUpV20LeftLumaKernel( const BOOL cosited, const BOOL is_odd ) ;

#endif /*_MODULES_H_ */
