/****************************************************************************
* Copyright (c) 2019 Qualcomm Technologies, Inc.                       *
* All Rights Reserved.                                                      *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                *
****************************************************************************/

#ifndef __SHDR3_H__
#define __SHDR3_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FRAME_DROP_SIGNAL (-2)
#define IHIST_NOT_READY_SIGNAL (-3)

#define MAX_EXP_ENTRIES (4)

#define MAX_PING_PONG_BUF (16)

#define MA_TABLE_LENGTH (8)

#define STATS_GRID_X (16)
#define STATS_GRID_Y (16)

#define STATS_GRID_FOCUS_X (16)
#define STATS_GRID_FOCUS_Y (16)

#define MAX_SHDR_BG_STATS_SIZE (STATS_GRID_X*STATS_GRID_Y)
#define MAX_SHDR_BHIST_STATS_SIZE (256)
#define MAX_SHDR_IHIST_STATS_SIZE (256)
#define MAX_SHDR_BHIST_STATS_NONUNIFROM_SIZE (1024)
#define MAX_SHDR_FOCUS_STATS_SIZE (STATS_GRID_FOCUS_X*STATS_GRID_FOCUS_Y)

#define STATS_RATIO_BIN_LOW (0.50f)
#define STATS_RATIO_BIN_MID (0.25f)
#define STATS_RATIO_BIN_HIGH (0.25f)
#define STATS_BIT_DEPTH_DATA_MOBILE (12)
#define STATS_BIT_DEPTH_DATA_IOT (12)
#define STATS_BIT_DEPTH_BIN_MOBILE (10)
#define STATS_BIT_DEPTH_BIN_IOT (10)
#define STATS_NON_UNIFROM_N_IOT (17)
#define STATS_NON_UNIFROM_M_IOT (8)
#define STATS_ROW_SUM_CHANNEL (3)
#define STATS_ROW_SUM_COL_NUM (8)
#define STATS_ROW_SUM_ROW_NUM (384)

#define FLICKER_GAIN_TAB_SIZE (512*3)

#define EPLP_USE_DC4 (0)
#define EPLP_USE_DC8 (1)
#define EPLP_USE_DC16 (2)

#define DEWARP_NUM_MESH_H (65)
#define DEWARP_NUM_MESH_V (97)

#define REFLECTION_LUT_SIZE (32)
#define TM_RESERVED_TABLE_SIZE (32)
#define AF_RESERVED_TABLE_SIZE (32)


#define DEBUG_COLOR_WHITE 0
#define DEBUG_WHITE_R 1
#define DEBUG_WHITE_G 1
#define DEBUG_WHITE_B 1

#define DEBUG_COLOR_RED 1
#define DEBUG_RED_R 1
#define DEBUG_RED_G 0
#define DEBUG_RED_B 0

#define DEBUG_COLOR_GREEN 2
#define DEBUG_GREEN_R 0
#define DEBUG_GREEN_G 1
#define DEBUG_GREEN_B 0

#define DEBUG_COLOR_BLUE 3
#define DEBUG_BLUE_R 0
#define DEBUG_BLUE_G 0
#define DEBUG_BLUE_B 1

#define DEBUG_COLOR_VIOLET 4
#define DEBUG_VIOLET_R 1
#define DEBUG_VIOLET_G 0
#define DEBUG_VIOLET_B 1

#define DEBUG_COLOR_YELLOW 5
#define DEBUG_YELLOW_R 1
#define DEBUG_YELLOW_G 1
#define DEBUG_YELLOW_B 0

#define DEBUG_COLOR_CYAN 6
#define DEBUG_CYAN_R 0
#define DEBUG_CYAN_G 1
#define DEBUG_CYAN_B 1

enum input_format
{
	SHDR_INPUT_IMG_FORMAT_PLAIN16,
	SHDR_INPUT_IMG_FORMAT_P010,
	SHDR_INPUT_IMG_FORMAT_P010_UBWC,
	SHDR_INPUT_IMG_FORMAT_TP10,
	SHDR_INPUT_IMG_FORMAT_TP10_UBWC,
	SHDR_INPUT_IMG_FORMAT_MAX
};

enum encoding_type
{
	HIST_STATS_UNIFORM_ENCODING,
	HIST_STATS_NONUNIFORM_ENCODING
};

typedef enum
{
	FAST_MODE_QUALITY,
	FAST_MODE_FAST,
	FAST_MODE_HYBRID,
	FAST_MODE_MAX
}shdr3_fast_mode_t;

typedef struct
{
#define TBD_PLACE_HOLDER_MAX_IIR_COEFF_NUM (16)
	float coeff[TBD_PLACE_HOLDER_MAX_IIR_COEFF_NUM]; // 6 coeffa, 6 coeffb, coeff_len, and FV_min
} shdr3_tuning_focus_iir_t;

typedef struct
{
#define TBD_PLACE_HOLDER_MAX_FIR_COEFF_NUM (16)
	float coeff[TBD_PLACE_HOLDER_MAX_FIR_COEFF_NUM];
} shdr3_tuning_focus_fir_t;

typedef struct {
	float start_x; // normalized to 0...1
	float start_y; // normalized to 0...1
	float end_x; // normalized to 0...1
	float end_y; // normalized to 0...1
} focus_win_t;

typedef struct {
	unsigned int frame_id;
	int sHDR_settled; // pass to defog for indexing sHDR settled
	int num_bg_grid_w;
	int num_bg_grid_h;
	int bhist1_encoding_type;
	int bhist1_num_of_bins;
	int bhist2_encoding_type;
	int bhist2_num_of_bins;
	int rowsum_ds_ratio;
	int rowsum_col_num;
	int rowsum_row_num;
	int num_focus_win;
	int fv1_stats_size;
	int fv2_stats_size;
#define TBD_PLACE_HOLDER_MAX_FOCUS_WIN (1)
	focus_win_t focus_win[TBD_PLACE_HOLDER_MAX_FOCUS_WIN];
	unsigned int r_avg[MAX_SHDR_BG_STATS_SIZE];
	unsigned int g_avg[MAX_SHDR_BG_STATS_SIZE];
	unsigned int b_avg[MAX_SHDR_BG_STATS_SIZE];
	unsigned int bin[MAX_SHDR_BHIST_STATS_SIZE];
	unsigned int bin_vhist2[MAX_SHDR_BHIST_STATS_NONUNIFROM_SIZE];//non-uniform, iot
	unsigned int ihist[MAX_SHDR_IHIST_STATS_SIZE];
	unsigned int rowsum[MAX_EXP_ENTRIES][STATS_ROW_SUM_CHANNEL][STATS_ROW_SUM_COL_NUM][STATS_ROW_SUM_ROW_NUM]; // # of exp, 3-channel rgb, # of col, # of row. Currently, only use 1-channel luma.
	shdr3_tuning_focus_iir_t focus_iir;
	shdr3_tuning_focus_fir_t focus_fir;
	unsigned int fv1[MAX_SHDR_FOCUS_STATS_SIZE]; // gpu fv
	unsigned int fv2[MAX_SHDR_FOCUS_STATS_SIZE]; //paaf
} shdr3_stats_info_t;

typedef struct
{
	int width;
	int height;
	int stride;
	unsigned int buf_size;
	unsigned int offset;
	unsigned char *buf;
	int fd;
	void *handle;
	int cached_ion;
} shdr3_buffer_t;

typedef struct {
	unsigned int iterations;
	unsigned int radius;
	float eps;
	float sigma;
} shdr3_tuning_eplp_t;

typedef struct {
	float gamma; // for method 1 & 2
	float gamma_region; // for method 2
	float alpha; // for method 3 & 5
	float highlight_compression_strength; // for method 4
	float boost_strength; // for method 6
	float factor; 
	float max_total_gain; 
	float pre_gamma;
	unsigned int method;
	float max_gain;
	float min_gain;
	float desat_th;
	float MaxVal;
	float detail_smoothing_weight_pass_full;
	float detail_smoothing_weight_pass_dc4;
	float reserved[TM_RESERVED_TABLE_SIZE];
} shdr3_tuning_tm_t;

typedef struct {
	unsigned int enable_detail_enhancement;
	float luma_adapation_a;
	float luma_adapation_b;
	float luma_adapation_c;
	float regulation_fat_thin;
	float regulation_center;
} shdr3_tuning_de_t;

typedef struct {
	float EXPOSURE_RATIO_REAL[MAX_EXP_ENTRIES];
	float EXPOSURE_RATIO_SENSOR[MAX_EXP_ENTRIES];

	unsigned int enable_motion_adaptation;
	unsigned int enable_debug_pair;
	float ma_luma_th1_exp[MAX_EXP_ENTRIES];
	float ma_luma_th2_exp[MAX_EXP_ENTRIES];
	float ma_diff_th1_exp[MAX_EXP_ENTRIES];
	float ma_diff_th2_exp[MAX_EXP_ENTRIES];

	float multipass_me_factor;
	unsigned int morphology_decision;

	float motion_factor[3];
	float blur_factor[3];
	unsigned int enable_ma_blurring[3];
	unsigned int enable_ma_multipass[3]; // for each pass, 0: ignore motion14, 1: reference motion14.
	float ma_reserved_tables[64];
} shdr3_tuning_fusion_t;

typedef struct {
	float max_gain_limit;
	float pre_weight;
	float center;
	float region;
	float boost_upper_bound;
	float boost_lower_bound;
	unsigned int dark_start;
	unsigned int dark_end;
	unsigned int midtone_start;
	unsigned int midtone_end;
	unsigned int highlight_start;
	unsigned int highlight_end;
} autoltm3_tuning_t;

typedef struct {
	float hybird_heavy_percentage;
	shdr3_fast_mode_t fast_mode;
}shdr3_tuning_fast_mode_t;

typedef struct {
	float factor;
	float up_limit;
	float reflection_lut[REFLECTION_LUT_SIZE];

}shdr3_tuning_blc_t;

typedef struct {
	shdr3_tuning_eplp_t eplp_param;
	shdr3_tuning_tm_t tm_param;
	shdr3_tuning_fusion_t fusion_param;
	autoltm3_tuning_t autoLTM3_param;
	shdr3_buffer_t flicker_gain_table;
	shdr3_tuning_de_t de_param;
	shdr3_tuning_fast_mode_t fast_param;
	shdr3_tuning_blc_t blc_param;
	unsigned int autoLTM3_enable;
	unsigned int LTM_enable;
	float WB_r_gain;
	float WB_b_gain;
	unsigned int anti_banding_enable;
#define TBD_PLACE_HOLDER_MAX_ROW_NUM (128)
	float anti_banding_gain[MAX_EXP_ENTRIES][STATS_ROW_SUM_CHANNEL][TBD_PLACE_HOLDER_MAX_ROW_NUM];
	float anti_banding_reserved[16]; // anti-banding
	float AF_reserved[AF_RESERVED_TABLE_SIZE];
} shdr3_tuning_t;

typedef struct {
	int type;//0: 96x64 mesh, 1: 3x3 global matrix.
	unsigned int x;
	unsigned int y;
	float RT[9];
	shdr3_buffer_t mesh;
} shdr3_warp_mesh_t;

typedef struct
{
	unsigned int fs_width;
	unsigned int fs_height;
	unsigned int fs_stride;
	unsigned int fs_scanline;
	unsigned int dc4_width;
	unsigned int dc4_height;
	unsigned int dc4_stride;
	unsigned int dc4_scanline;
	unsigned int dc16_width;
	unsigned int dc16_height;
	unsigned int dc16_stride;
	unsigned int dc16_scanline;
	unsigned int ds_flag;
	unsigned int num_exposure;
	unsigned int perf_ind;
	unsigned int number_of_input_buffers;
	unsigned int number_of_output_buffers;
	unsigned int input_frame_format;
	unsigned int dump_print_en;
	unsigned int frame_rate;
} shdr3_init_config_t;

typedef struct {
	int offset_x[MAX_EXP_ENTRIES];
	int offset_y[MAX_EXP_ENTRIES];
	shdr3_tuning_t *tuning;
	shdr3_warp_mesh_t warp_mesh[MAX_EXP_ENTRIES];
	shdr3_stats_info_t *stats_info;

	shdr3_buffer_t input_buffer[MAX_EXP_ENTRIES];
	shdr3_buffer_t in_dc4_addr[MAX_EXP_ENTRIES];
	shdr3_buffer_t in_dc16_addr[MAX_EXP_ENTRIES];

	shdr3_buffer_t output_buffer;
	shdr3_buffer_t out_dc4_addr;
	shdr3_buffer_t out_dc16_addr;
} shdr3_input_param_t;

int shdr3_lib_init(void **handle, shdr3_init_config_t *init_config, shdr3_input_param_t* input_param);
void shdr3_lib_deinit(void *handle);
int shdr3_lib_process(
	void *handle, shdr3_input_param_t *input_param,
	void (*shdr_completion_callback)(void *handle, shdr3_input_param_t *input_param, int ihist_ready, void *user_data),
	void (*aec_stats_ready_callback)(shdr3_stats_info_t *stats, void *user_data),
	void *user_data);

#ifdef __cplusplus
}
#endif
#endif
