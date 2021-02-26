/****************************************************************************
* Copyright (c) 2019 Qualcomm Technologies, Inc.                            *
* All Rights Reserved.                                                      *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                *
****************************************************************************/

//#ifndef __LOG_UTIL_H__
//#define __LOG_UTIL_H__
#ifndef __TRIGGER_UTIL_H__
#define __TRIGGER_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define IDX_START 0
#define IDX_END 1
#define NUM_ENTRY_PICKUP 2

typedef struct {
	float start;
	float end;
}trigger_region;

typedef struct{
  trigger_region region;
  float strength;
}entry_type;


int file_read_table_3d(char *filename, int l, int m, int n, float*** table);
int file_read_table_2d(char *filename, int num, int tab_length, float** table);

entry_type **dyn_arry_2d_entry(unsigned int m, unsigned int n);
float **dyn_arry_2d(unsigned int m, unsigned int n);
float ***dyn_arry_3d(unsigned int l, unsigned int m, unsigned int n);

void dyn_arry_2d_free_entry(entry_type **arry, unsigned int m);
void dyn_arry_2d_free(float **arry, unsigned int m);
void dyn_arry_3d_free(float ***arry, unsigned int l, unsigned int m);

float bilinear_intp(float x_in, float x0, float x1, float y0, float y1);

void interpolate_strength_RedDragon(float trigger, int number_of_entries, entry_type* entry, float* strength);

void trigger_interpolation_1_layer(char *file_zone_trig1, char *file_zone_data
                                          , int num_entry1, int length_table, float trig1, float *strength);

void trigger_interpolation_2_layer(char *file_zone_trig1, char *file_zone_trig2, char *file_zone_data
                                          , int num_entry1, int num_entry2, int length_table, float trig1, float trig2, float *strength);

void trigger_interpolation_update(shdr3_tuning_t *tuning, float ExpRatio, float Gain, float LuxIdx);

void trigger_interpolation_release();

void SetupInterpolationData(void* pChromatixYUVSHDR);

void SetupDiffThresholdLUT(int num_entry1, int num_entry2, int length_table);

void SetupMEFactor(int num_entry1, int num_entry2, int length_table);

void SetupLowPassFilter(int num_entry1, int length_table);

void SetupToneMapping(int num_entry1, int length_table);

void SetupGammaLUT(int num_entry1, int length_table);

void SetupDeSaturateLUT(int num_entry1, int length_table);

void SetupAutoLTMLUT(int num_entry1, int length_table);

void SetupMaxGainToneMapping(int num_entry1, int length_table);

void SetupLumaThresholdLUT(int num_entry1, int length_table);

void InterpolateDiffThresholdLUT(shdr3_tuning_t *tuning, int num_entry1, int num_entry2, int length_table, float trig1, float trig2);

void InterpolateMEFactorLUT(shdr3_tuning_t *tuning, int num_entry1, int num_entry2, int length_table, float trig1, float trig2);

void InterpolateLowPassFilter(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void InterpolateToneMapping(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void InterpolateGammaLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void InterpolateDeSaturateLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void Interpolate2Layer(int num_entry1, int num_entry2, int length_table, float trig1, float trig2, float *strength,
                float ***table, float **trigger1, float **trigger2, float **table_tmp, entry_type **entry2, entry_type **entry1_pickup);

void Interpolate1Layer(int num_entry1, int length_table, float trig1, float *strength,
                float **table, float **trigger1, entry_type **entry1_pickup);

void InterpolateAutoLTMLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void InterpolateLumaThresholdLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

void InterpolateMaxGainToneMapping(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1);

#ifdef __cplusplus
}
#endif
#endif
