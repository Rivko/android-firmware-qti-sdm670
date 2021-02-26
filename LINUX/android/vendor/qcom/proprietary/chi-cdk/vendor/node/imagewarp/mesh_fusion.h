////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016, 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MESH_FUSION_H__
#define __MESH_FUSION_H__

#if defined(__cplusplus)
extern "C"
{
#endif

//==============================================================================
// Build Config.
//==============================================================================
// *** Disable to run with different mesh size *** //
#define DEF_64x96 (1)

#if DEF_64x96 == 1
	#define WARP_MESH_MAX_W (64)
	#define WARP_MESH_MAX_H (96)
#else
	#define WARP_MESH_MAX_W (32)
	#define WARP_MESH_MAX_H (24)
#endif
#define WARP_GRID_MAX_W (WARP_MESH_MAX_W+1)
#define WARP_GRID_MAX_H (WARP_MESH_MAX_H+1)

// *** Enable to demo MFEIS with alpha channel *** //
// #define ENABLE_ALPHA_CHANNEL

//==============================================================================
// Declarations
//==============================================================================
typedef struct mesh_one_entry_s
{
	float x_in;
	float y_in;
#ifdef ENABLE_ALPHA_CHANNEL
	float alpha;
#endif
} mesh_one_entry_t;

typedef struct mesh_s
{
	unsigned int mesh_vertices_num_x;
	unsigned int mesh_vertices_num_y;
	unsigned int input_width;
	unsigned int input_height;
	unsigned int input_o_center_x;
	unsigned int input_o_center_y;
	unsigned int output_width;
	unsigned int output_height;
	unsigned int output_o_center_x;
	unsigned int output_o_center_y;
	int type; // 0 for pixel shifts, 1 for 3x3 matrix
	mesh_one_entry_t mapping[WARP_GRID_MAX_H][WARP_GRID_MAX_W];
} mesh_t;

typedef struct mesh_fusion_win_s
{
	unsigned int input_width;
	unsigned int input_height;
	unsigned int start_x;
	unsigned int start_y;
	unsigned int end_x;
	unsigned int end_y;
	unsigned int output_width;
	unsigned int output_height;
	unsigned int hblank;
	unsigned int vblank;
	unsigned int hor_binning_en;
	unsigned int binning_en;
} mesh_fusion_win_t;

typedef struct mesh_config_s
{
	int ldc_en;
	int sv_en;
	int eis_en;
	mesh_t *ldc_mesh;
	mesh_t *super_view_mesh;
	mesh_t *eis_mesh;
	mesh_t *output_mesh;
}mesh_config_t;

int mesh_fusion_init(void** handle);
int mesh_fusion_deinit(void* handle);
int mesh_fusion(void* handle, mesh_config_t* config);
int mesh_fusion_adapt_window(void * handle, mesh_config_t* config, mesh_fusion_win_t* sensor_win, mesh_fusion_win_t* isp_win);

#if defined(__cplusplus)
}
#endif

#endif
