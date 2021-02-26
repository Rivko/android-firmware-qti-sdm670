/****************************************************************************
* Copyright (c) 2019 Qualcomm Technologies, Inc.                            *
* All Rights Reserved.                                                      *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                *
****************************************************************************/
#include <shdr3.h>
#include "trigger_util.h"
#include "camxchinodeyuvshdr.h"
#include "tuningsetmanager.h"
#include "chituningmodeparam.h"

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_DIFF_THRSHOLD 16
#define NUM_ENTRY_TRIG2_DIFF_THRSHOLD 16
#define LENGTH_TABLE_DIFF_THRSHOLD 8

float ***diff_threshold_table;
float **diff_threshold_trigger1, **diff_threshold_trigger2, **diff_threshold_table_tmp;
entry_type **diff_threshold_entry2, **diff_threshold_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_LUMA_THRSHOLD 16
#define LENGTH_TABLE_LUMA_THRSHOLD 8

float **luma_threshold_table;
float **luma_threshold_trigger1;
entry_type **luma_threshold_entry1_pickup;

///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_ME_FACOTR 16
#define NUM_ENTRY_TRIG2_ME_FACOTR 16
#define LENGTH_TABLE_ME_FACOTR 1


float ***me_factor_table;
float **me_factor_trigger1, **me_factor_trigger2, **me_factor_table_tmp;
entry_type **me_factor_entry2, **me_factor_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_LOW_PASS_FILTER 16
#define LENGTH_TABLE_LOW_PASS_FILTER 1

float **low_pass_table;
float **low_pass_trigger1;
entry_type **low_pass_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_TONE_MAPPING 16
#define LENGTH_TABLE_TONE_MAPPING 4

float **tone_mapping_table;
float **tone_mapping_trigger1;
entry_type **tone_mapping_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_TONE_MAPPING_MAX_GAIN 8
#define LENGTH_TABLE_TONE_MAPPING_MAX_GAIN 1

float **tone_mapping_maxgain_table;
float **tone_mapping_maxgain_trigger1;
entry_type **tone_mapping_maxgain_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_GAMMA 16
#define LENGTH_TABLE_GAMMA 1

float **gamma_LUT_table;
float **gamma_LUT_trigger1;
entry_type **gamma_LUT_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_DESATURATE 16
#define LENGTH_TABLE_DESATURATE 1

float **desaturate_LUT_table;
float **desaturate_LUT_trigger1;
entry_type **desaturate_LUT_entry1_pickup;
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
#define NUM_ENTRY_TRIG1_AUTOLTM 16
#define LENGTH_TABLE_AUTOLTM 12

float **AutoLTM_LUT_table;
float **AutoLTM_LUT_trigger1;
entry_type **AutoLTM_LUT_entry1_pickup;

///////////////////////////////////////////////////////////////////////////////////////

shdr_3_0_0::shdr30Type* pChromatixYUVSHDR;

float bilinear_intp(float x_in, float x0, float x1, float y0, float y1)
{
    float w1 = (x_in - x0) / (x1 - x0);
    float w0 = 1.0f - w1;
    return y0 * w0 + y1 * w1;
}

void interpolate_strength_RedDragon(float trigger, int number_of_entries, entry_type* entry, float* strength)
{
    //printf("++interpolate_strength_RedDragon()\n");
    /*
    ---|--Region1--|
                    \
                     \
                      |--Region2--|
                                   \
                                    \
                                     |--Region3--|---
    */
      float strength_local = 1.0f;
      if (trigger < entry[0].region.start)
       strength_local = entry[0].strength;
      else if (trigger >= entry[number_of_entries-1].region.end)
       strength_local = entry[number_of_entries-1].strength;
      else{
       int i, interpolation_needed = 0;

       for (i = 0; i < number_of_entries; i++) {
        float start = entry[i].region.start;
        float start_next = entry[i+1].region.start;
        float end = entry[i].region.end;

        if (trigger >= start && trigger < end) {
          interpolation_needed = 0;
          break;
        }

        if (trigger >= end && trigger < start_next) {
          interpolation_needed = 1;
          break;
        }
       }

        if (interpolation_needed == 0) {
          strength_local = entry[i].strength;
        } else {
          float x1, x2, y1, y2;
          //x1 = entry[i].region.start;
          //x2 = entry[i].region.end;
          x1 = entry[i].region.end;
          x2 = entry[i+1].region.start;
          y1 = entry[i].strength;
          y2 = entry[i+1].strength;

          strength_local = bilinear_intp(trigger, x1, x2, y1, y2);
        }
      }

      *strength = strength_local;

    //  printf("--interpolate_strength_RedDragon()\n");

}


void trigger_interpolation_update(shdr3_tuning_t *tuning, float ExpRatio, float Gain, float LuxIdx)
{
    InterpolateDiffThresholdLUT(tuning, NUM_ENTRY_TRIG1_DIFF_THRSHOLD, NUM_ENTRY_TRIG2_DIFF_THRSHOLD, LENGTH_TABLE_DIFF_THRSHOLD, ExpRatio, Gain);
    InterpolateMEFactorLUT(tuning, NUM_ENTRY_TRIG1_ME_FACOTR, NUM_ENTRY_TRIG2_ME_FACOTR, LENGTH_TABLE_ME_FACOTR, ExpRatio, Gain);
    InterpolateLowPassFilter(tuning, NUM_ENTRY_TRIG1_LOW_PASS_FILTER, LENGTH_TABLE_LOW_PASS_FILTER, ExpRatio);
    InterpolateLumaThresholdLUT(tuning, NUM_ENTRY_TRIG1_LUMA_THRSHOLD, LENGTH_TABLE_LUMA_THRSHOLD, ExpRatio);
    InterpolateMaxGainToneMapping(tuning, NUM_ENTRY_TRIG1_TONE_MAPPING_MAX_GAIN, LENGTH_TABLE_TONE_MAPPING_MAX_GAIN, ExpRatio);
    InterpolateToneMapping(tuning, NUM_ENTRY_TRIG1_TONE_MAPPING, LENGTH_TABLE_TONE_MAPPING, ExpRatio);
    InterpolateGammaLUT(tuning, NUM_ENTRY_TRIG1_GAMMA, LENGTH_TABLE_GAMMA, ExpRatio);
    InterpolateDeSaturateLUT(tuning, NUM_ENTRY_TRIG1_DESATURATE, LENGTH_TABLE_DESATURATE, ExpRatio);
    InterpolateAutoLTMLUT(tuning, NUM_ENTRY_TRIG1_AUTOLTM, LENGTH_TABLE_AUTOLTM, LuxIdx);
}

void Interpolate1Layer(int num_entry1, int length_table, float trig1, float *strength,
                float **table, float **trigger1, entry_type **entry1_pickup)
{
    int get_tab_idx1 = 0, get_tab_idx2 = 0;
    int i, j, k, rc;
    //check the location of triger1
    if (trig1 < trigger1[0][IDX_END])
    {
        get_tab_idx1 = 0;
        get_tab_idx2 = 0;
    }
    else if (trig1 >= trigger1[num_entry1-1][IDX_START])
    {
        get_tab_idx1 = num_entry1 - 1;
        get_tab_idx2 = num_entry1 - 1;
    }
    else
    {
        for (i = 1; i < num_entry1; i++)
        {
            if (trig1 < trigger1[i][IDX_START])
            { //take 2 tables
                get_tab_idx1 = i-1;
                get_tab_idx2 = i;
                break;
            }
            else if (trig1 < trigger1[i][IDX_END])
            {
                //take 1 table repeatedly
                get_tab_idx1 = i;
                get_tab_idx2 = i;
                break;
            }
            else
            {
                continue;
            }
        }
    }

    //interpolation by trigger1 ===========================================================================
    for (k = 0; k < length_table; k++)
    {
        entry1_pickup[k][0].region.start = trigger1[get_tab_idx1][IDX_START];
        entry1_pickup[k][0].region.end = trigger1[get_tab_idx1][IDX_END];
        entry1_pickup[k][0].strength = table[get_tab_idx1][k];

        entry1_pickup[k][1].region.start = trigger1[get_tab_idx2][IDX_START];
        entry1_pickup[k][1].region.end = trigger1[get_tab_idx2][IDX_END];
        entry1_pickup[k][1].strength = table[get_tab_idx2][k];
    }

    for (k = 0; k < length_table; k++)
    {
        interpolate_strength_RedDragon(trig1, NUM_ENTRY_PICKUP, entry1_pickup[k], &strength[k]);
    }
}

void Interpolate2Layer(int num_entry1, int num_entry2, int length_table, float trig1, float trig2, float *strength,
                float ***table, float **trigger1, float **trigger2, float **table_tmp, entry_type **entry2, entry_type **entry1_pickup)
{
    int get_tab_idx1 = 0, get_tab_idx2 = 0;
    int i, j, k, rc;

    //check the location of triger1
    if (trig1 < trigger1[0][IDX_END])
    {
        get_tab_idx1 = 0;
        get_tab_idx2 = 0;
    }
    else if (trig1 >= trigger1[num_entry1-1][IDX_START])
    {
        get_tab_idx1 = num_entry1 - 1;
        get_tab_idx2 = num_entry1 - 1;
    }
    else
    {
        for (i = 1; i < num_entry1; i++)
        {
            if (trig1 < trigger1[i][IDX_START])
            { //take 2 tables
                get_tab_idx1 = i-1;
                get_tab_idx2 = i;
                break;
            }
            else if (trig1 < trigger1[i][IDX_END])
            { //take 1 table repeatedly
                get_tab_idx1 = i;
                get_tab_idx2 = i;
                break;
            }
            else
            {
                continue;
            }
        }
    }

    if(get_tab_idx1 == get_tab_idx2)
    {
        for (k = 0; k < length_table; k++)
        {
            for (j = 0; j < num_entry2; j++)
            {
                entry2[k][j].region.start = trigger2[j][IDX_START];
                entry2[k][j].region.end = trigger2[j][IDX_END];
                entry2[k][j].strength = table[get_tab_idx1][j][k];
            }
        }
        for (k = 0; k < length_table; k++)
        {
            interpolate_strength_RedDragon(trig2, num_entry2, entry2[k], &strength[k]);
        }
    }
    else
    {
        //take corrresponding 2 tables by trigger1
        for (i = get_tab_idx1; i <= get_tab_idx2; i++)
        {
            for (k = 0; k < length_table; k++)
            {
                for (j = 0; j < num_entry2; j++)
                {
                    entry2[k][j].region.start = trigger2[j][IDX_START];
                    entry2[k][j].region.end = trigger2[j][IDX_END];
                    entry2[k][j].strength = table[i][j][k];
                }
            }

            //interpolation by trigger2 ==========================================================================
            for (k = 0; k < length_table; k++)
            {
                interpolate_strength_RedDragon(trig2, num_entry2, entry2[k], &table_tmp[k][i - get_tab_idx1]);
            }
        }

        //interpolation by trigger1 ===========================================================================
        for (k = 0; k < length_table; k++)
        {
            entry1_pickup[k][0].region.start = trigger1[get_tab_idx1][IDX_START];
            entry1_pickup[k][0].region.end = trigger1[get_tab_idx1][IDX_END];
            entry1_pickup[k][0].strength = table_tmp[k][0];

            entry1_pickup[k][1].region.start = trigger1[get_tab_idx2][IDX_START];
            entry1_pickup[k][1].region.end = trigger1[get_tab_idx2][IDX_END];
            entry1_pickup[k][1].strength = table_tmp[k][1];
        }
        for (k = 0; k < length_table; k++)
        {
            interpolate_strength_RedDragon(trig1, NUM_ENTRY_PICKUP, entry1_pickup[k], &strength[k]);
        }
    }
}



void InterpolateLowPassFilter(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *low_pass_filter = (float *)calloc(LENGTH_TABLE_LOW_PASS_FILTER, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, low_pass_filter,
        low_pass_table, low_pass_trigger1, low_pass_entry1_pickup);

    tuning->eplp_param.eps = low_pass_filter[0];
    free(low_pass_filter);
}

void InterpolateToneMapping(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *tone_mapping = (float *)calloc(LENGTH_TABLE_TONE_MAPPING, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, tone_mapping,
        tone_mapping_table, tone_mapping_trigger1, tone_mapping_entry1_pickup);

    tuning->tm_param.boost_strength = tone_mapping[0];
    tuning->tm_param.factor         = tone_mapping[1];
    tuning->tm_param.max_total_gain = tone_mapping[2];
    tuning->tm_param.min_gain       = tone_mapping[3];

    free(tone_mapping);
}

void InterpolateMaxGainToneMapping(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *tone_mapping_max = (float *)calloc(LENGTH_TABLE_TONE_MAPPING_MAX_GAIN, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, tone_mapping_max,
        tone_mapping_maxgain_table, tone_mapping_maxgain_trigger1, tone_mapping_maxgain_entry1_pickup);

    tuning->tm_param.max_gain = tone_mapping_max[0];

    free(tone_mapping_max);
}


void InterpolateGammaLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *gamma = (float *)calloc(LENGTH_TABLE_GAMMA, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, gamma,
        gamma_LUT_table, gamma_LUT_trigger1, gamma_LUT_entry1_pickup);

    tuning->tm_param.gamma     = gamma[0];
    tuning->tm_param.pre_gamma = gamma[0];
    free(gamma);
}

void InterpolateLumaThresholdLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *lumaThreshold = (float *)calloc(LENGTH_TABLE_LUMA_THRSHOLD, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, lumaThreshold,
        luma_threshold_table, luma_threshold_trigger1, luma_threshold_entry1_pickup);


    tuning->fusion_param.ma_luma_th1_exp[0] = lumaThreshold[0];
    tuning->fusion_param.ma_luma_th1_exp[1] = lumaThreshold[1];
    tuning->fusion_param.ma_luma_th1_exp[2] = lumaThreshold[2];
    tuning->fusion_param.ma_luma_th1_exp[3] = lumaThreshold[3];
    tuning->fusion_param.ma_luma_th2_exp[0] = lumaThreshold[4];
    tuning->fusion_param.ma_luma_th2_exp[1] = lumaThreshold[5];
    tuning->fusion_param.ma_luma_th2_exp[2] = lumaThreshold[6];
    tuning->fusion_param.ma_luma_th2_exp[3] = lumaThreshold[7];
    free(lumaThreshold);
}


void InterpolateDeSaturateLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *desaturate = (float *)calloc(LENGTH_TABLE_DESATURATE, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, desaturate,
        desaturate_LUT_table, desaturate_LUT_trigger1, desaturate_LUT_entry1_pickup);

    tuning->tm_param.desat_th = desaturate[0];
    free(desaturate);
}

void InterpolateAutoLTMLUT(shdr3_tuning_t *tuning, int num_entry1, int length_table, float trig1)
{
    int i;
    float *autoLTM = (float *)calloc(LENGTH_TABLE_AUTOLTM, sizeof(float));

    Interpolate1Layer(num_entry1, length_table, trig1, autoLTM,
        AutoLTM_LUT_table, AutoLTM_LUT_trigger1, AutoLTM_LUT_entry1_pickup);

    tuning->autoLTM3_param.max_gain_limit = autoLTM[0];
    tuning->autoLTM3_param.center = autoLTM[1];
    tuning->autoLTM3_param.region = autoLTM[2];
    tuning->autoLTM3_param.pre_weight = autoLTM[3];
    tuning->autoLTM3_param.boost_upper_bound = autoLTM[4];
    tuning->autoLTM3_param.boost_lower_bound = autoLTM[5];
    tuning->autoLTM3_param.dark_start = autoLTM[6];
    tuning->autoLTM3_param.dark_end = autoLTM[7];
    tuning->autoLTM3_param.midtone_start = autoLTM[8];
    tuning->autoLTM3_param.midtone_end = autoLTM[9];
    tuning->autoLTM3_param.highlight_start = autoLTM[10];
    tuning->autoLTM3_param.highlight_end = autoLTM[11];
    free(autoLTM);
}



void InterpolateMEFactorLUT(shdr3_tuning_t *tuning, int num_entry1, int num_entry2, int length_table, float trig1, float trig2)
{
    int i;
    float *me_factor = (float *)calloc(LENGTH_TABLE_ME_FACOTR, sizeof(float));

    Interpolate2Layer(num_entry1, num_entry2, length_table, trig1, trig2, me_factor,
        me_factor_table, me_factor_trigger1, me_factor_trigger2, me_factor_table_tmp, me_factor_entry2, me_factor_entry1_pickup);

    tuning->fusion_param.multipass_me_factor = me_factor[0];
    free(me_factor);
}

void InterpolateDiffThresholdLUT(shdr3_tuning_t *tuning, int num_entry1, int num_entry2, int length_table, float trig1, float trig2)
{
    int i;
    float *strength = (float *)calloc(LENGTH_TABLE_DIFF_THRSHOLD, sizeof(float));

    Interpolate2Layer(num_entry1, num_entry2, length_table, trig1, trig2, strength,
        diff_threshold_table, diff_threshold_trigger1, diff_threshold_trigger2, diff_threshold_table_tmp, diff_threshold_entry2, diff_threshold_entry1_pickup);

    for (i = 0; i < 4; i++) {
        tuning->fusion_param.ma_diff_th1_exp[i] = strength[i];
        tuning->fusion_param.ma_diff_th2_exp[i] = strength[i + 4];
    }

    free(strength);
}

void SetupInterpolationData(void* data)
{
    pChromatixYUVSHDR = (shdr_3_0_0::shdr30Type*)data;
    LOG_INFO(CamxLogGroupChi, "shdr_enable :%d", pChromatixYUVSHDR->enable_section.shdr_enable);
    // @todo : need to read all MACRO Template from chromatix
    SetupDiffThresholdLUT(NUM_ENTRY_TRIG1_DIFF_THRSHOLD, NUM_ENTRY_TRIG2_DIFF_THRSHOLD, LENGTH_TABLE_DIFF_THRSHOLD);
    SetupMEFactor(NUM_ENTRY_TRIG1_ME_FACOTR, NUM_ENTRY_TRIG2_ME_FACOTR, LENGTH_TABLE_ME_FACOTR);
    SetupLumaThresholdLUT(NUM_ENTRY_TRIG1_LUMA_THRSHOLD, LENGTH_TABLE_LUMA_THRSHOLD);
    SetupMaxGainToneMapping(NUM_ENTRY_TRIG1_TONE_MAPPING_MAX_GAIN, LENGTH_TABLE_TONE_MAPPING_MAX_GAIN);
    SetupLowPassFilter(NUM_ENTRY_TRIG1_LOW_PASS_FILTER, LENGTH_TABLE_LOW_PASS_FILTER);
    SetupToneMapping(NUM_ENTRY_TRIG1_TONE_MAPPING, LENGTH_TABLE_TONE_MAPPING);
    SetupGammaLUT(NUM_ENTRY_TRIG1_GAMMA, LENGTH_TABLE_GAMMA);
    SetupDeSaturateLUT(NUM_ENTRY_TRIG1_DESATURATE, LENGTH_TABLE_DESATURATE);
    SetupAutoLTMLUT(NUM_ENTRY_TRIG1_AUTOLTM, LENGTH_TABLE_AUTOLTM);
}

void SetupAutoLTMLUT(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    AutoLTM_LUT_trigger1       = dyn_arry_2d(num_entry1, 2);
    AutoLTM_LUT_table          = dyn_arry_2d(num_entry1, length_table);
    AutoLTM_LUT_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        AutoLTM_LUT_trigger1[i][0] = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone_trigger.lux_idx_start;
        AutoLTM_LUT_trigger1[i][1] = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone_trigger.lux_idx_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        AutoLTM_LUT_table[i][0] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_max_gain_limit;
        AutoLTM_LUT_table[i][1] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_center;
        AutoLTM_LUT_table[i][2] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_region;
        AutoLTM_LUT_table[i][3] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_pre_weight;
        AutoLTM_LUT_table[i][4] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_boost_upper_bound;
        AutoLTM_LUT_table[i][5] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_boost_lower_bound;
        AutoLTM_LUT_table[i][6] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_dark_start;
        AutoLTM_LUT_table[i][7] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_dark_end;
        AutoLTM_LUT_table[i][8] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_midtone_start;
        AutoLTM_LUT_table[i][9] =  pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_midtone_end;
        AutoLTM_LUT_table[i][10] = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_highlight_start;
        AutoLTM_LUT_table[i][11] = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone[i].zone.autoLTM3_highlight_end;
    }
}

void SetupDeSaturateLUT(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    desaturate_LUT_trigger1       = dyn_arry_2d(num_entry1, 2);
    desaturate_LUT_table          = dyn_arry_2d(num_entry1, length_table);
    desaturate_LUT_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        desaturate_LUT_trigger1[i][0] = pChromatixYUVSHDR->LTM.desaturate_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        desaturate_LUT_trigger1[i][1] = pChromatixYUVSHDR->LTM.desaturate_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        desaturate_LUT_table[i][0] = pChromatixYUVSHDR->LTM.desaturate_LUT.exp_ratio_zone[i].zone.TM_desat_th;
    }
}

void SetupGammaLUT(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    gamma_LUT_trigger1       = dyn_arry_2d(num_entry1, 2);
    gamma_LUT_table          = dyn_arry_2d(num_entry1, length_table);
    gamma_LUT_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        gamma_LUT_trigger1[i][0] = pChromatixYUVSHDR->LTM.gamma_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        gamma_LUT_trigger1[i][1] = pChromatixYUVSHDR->LTM.gamma_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        gamma_LUT_table[i][0] = pChromatixYUVSHDR->LTM.gamma_LUT.exp_ratio_zone[i].zone.TM_pre_gamma;
    }
}


void SetupMaxGainToneMapping(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    tone_mapping_maxgain_trigger1       = dyn_arry_2d(num_entry1, 2);
    tone_mapping_maxgain_table          = dyn_arry_2d(num_entry1, length_table);
    tone_mapping_maxgain_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        tone_mapping_maxgain_table[i][0] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        tone_mapping_maxgain_table[i][1] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        tone_mapping_maxgain_table[i][0] = pChromatixYUVSHDR->LTM.maxgain_LUT.exp_ratio_zone->zone.TM_maxgain;
    }
}


void SetupToneMapping(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    tone_mapping_trigger1       = dyn_arry_2d(num_entry1, 2);
    tone_mapping_table          = dyn_arry_2d(num_entry1, length_table);
    tone_mapping_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        tone_mapping_trigger1[i][0] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        tone_mapping_trigger1[i][1] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        tone_mapping_table[i][0] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone.TM_boost_strength;
        tone_mapping_table[i][1] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone.TM_factor;
        tone_mapping_table[i][2] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone.TM_max_total_gain;
        tone_mapping_table[i][3] = pChromatixYUVSHDR->LTM.mingain_LUT.exp_ratio_zone[i].zone.TM_mingain;
    }
}


void SetupLumaThresholdLUT(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    luma_threshold_trigger1       = dyn_arry_2d(num_entry1, 2);
    luma_threshold_table          = dyn_arry_2d(num_entry1, length_table);
    luma_threshold_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        luma_threshold_trigger1[i][0] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        luma_threshold_trigger1[i][1] = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        luma_threshold_table[i][0] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp0;
        luma_threshold_table[i][1] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp1;
        luma_threshold_table[i][2] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp2;
        luma_threshold_table[i][3] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp3;
        luma_threshold_table[i][4] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp0;
        luma_threshold_table[i][5] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp1;
        luma_threshold_table[i][6] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp2;
        luma_threshold_table[i][7] =  pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp3;
    }
}

void SetupLowPassFilter(int num_entry1, int length_table)
{
    int get_tab_idx1, get_tab_idx2;
    int i, j, k, rc;

    low_pass_trigger1       = dyn_arry_2d(num_entry1, 2);
    low_pass_table          = dyn_arry_2d(num_entry1, length_table);
    low_pass_entry1_pickup  = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);

    for (i = 0; i < num_entry1; i++)
    {
        low_pass_trigger1[i][0] = pChromatixYUVSHDR->LTM.low_pass_filter_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        low_pass_trigger1[i][1] = pChromatixYUVSHDR->LTM.low_pass_filter_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        low_pass_table[i][0] = pChromatixYUVSHDR->LTM.low_pass_filter_LUT.exp_ratio_zone[i].zone.EPLP_eps;
    }
}

void SetupMEFactor(int num_entry1, int num_entry2, int length_table)
{
    int    get_tab_idx1, get_tab_idx2;
    int    i, j, k, rc;

    me_factor_trigger1         = dyn_arry_2d(num_entry1, 2);
    me_factor_trigger2         = dyn_arry_2d(num_entry2, 2);
    me_factor_entry2           = dyn_arry_2d_entry(length_table, num_entry2);
    me_factor_entry1_pickup    = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);
    me_factor_table            = dyn_arry_3d(num_entry1, num_entry2, length_table);
    me_factor_table_tmp        = dyn_arry_2d(length_table, 2);

    for (i = 0; i < num_entry1; i++)
    {
        me_factor_trigger1[i][0] = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        me_factor_trigger1[i][1] = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        me_factor_trigger2[i][0] = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone[0].zone.gain_zone[i].zone_trigger.gain_start;
        me_factor_trigger2[i][1] = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone[0].zone.gain_zone[i].zone_trigger.gain_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        for (j = 0; j < num_entry2; j++)
        {
            me_factor_table[i][j][0] = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_multipass_me_factor;
        }
    }
}

void SetupDiffThresholdLUT(int num_entry1, int num_entry2, int length_table)
{
    int     get_tab_idx1, get_tab_idx2;
    int     i, j, k, rc;

    diff_threshold_trigger1         = dyn_arry_2d(num_entry1, 2);
    diff_threshold_trigger2         = dyn_arry_2d(num_entry2, 2);
    diff_threshold_entry2           = dyn_arry_2d_entry(length_table, num_entry2);
    diff_threshold_entry1_pickup    = dyn_arry_2d_entry(length_table, NUM_ENTRY_PICKUP);
    diff_threshold_table            = dyn_arry_3d(num_entry1, num_entry2, length_table);
    diff_threshold_table_tmp        = dyn_arry_2d(length_table, 2);

    for (i = 0; i < num_entry1; i++)
    {
        diff_threshold_trigger1[i][0] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_start;
        diff_threshold_trigger1[i][1] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone_trigger.exp_ratio_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        diff_threshold_trigger2[i][0] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[0].zone.gain_zone[i].zone_trigger.gain_start;
        diff_threshold_trigger2[i][1] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[0].zone.gain_zone[i].zone_trigger.gain_end;
    }

    for (i = 0; i < num_entry1; i++)
    {
        for (j = 0; j < num_entry2; j++)
        {
            diff_threshold_table[i][j][0] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th1_exp0;
            diff_threshold_table[i][j][1] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th1_exp1;
            diff_threshold_table[i][j][2] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th1_exp2;
            diff_threshold_table[i][j][3] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th1_exp3;
            diff_threshold_table[i][j][4] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th2_exp0;
            diff_threshold_table[i][j][5] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th2_exp1;
            diff_threshold_table[i][j][6] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th2_exp2;
            diff_threshold_table[i][j][7] = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone[i].zone.gain_zone[j].zone.FUSION_ma_diff_th2_exp3;
        }
    }
}

void trigger_interpolation_release()
{
    LOG_INFO(CamxLogGroupChi, "Releasing Intepolation resource");
/************************************Free diff_threshold_LUT*************************************************/
    dyn_arry_3d_free(diff_threshold_table, NUM_ENTRY_TRIG1_DIFF_THRSHOLD, NUM_ENTRY_TRIG2_DIFF_THRSHOLD);
    dyn_arry_2d_free(diff_threshold_table_tmp, LENGTH_TABLE_DIFF_THRSHOLD);
    dyn_arry_2d_free_entry(diff_threshold_entry1_pickup, LENGTH_TABLE_DIFF_THRSHOLD);
    dyn_arry_2d_free_entry(diff_threshold_entry2, LENGTH_TABLE_DIFF_THRSHOLD);
    dyn_arry_2d_free(diff_threshold_trigger1, NUM_ENTRY_TRIG1_DIFF_THRSHOLD);
    dyn_arry_2d_free(diff_threshold_trigger2, NUM_ENTRY_TRIG2_DIFF_THRSHOLD);

/**************************************Free me_factor_LUT****************************************************/
    dyn_arry_3d_free(me_factor_table, NUM_ENTRY_TRIG1_ME_FACOTR, NUM_ENTRY_TRIG2_ME_FACOTR);
    dyn_arry_2d_free(me_factor_table_tmp, LENGTH_TABLE_ME_FACOTR);
    dyn_arry_2d_free_entry(me_factor_entry1_pickup, LENGTH_TABLE_ME_FACOTR);
    dyn_arry_2d_free_entry(me_factor_entry2, LENGTH_TABLE_ME_FACOTR);
    dyn_arry_2d_free(me_factor_trigger1, NUM_ENTRY_TRIG1_ME_FACOTR);
    dyn_arry_2d_free(me_factor_trigger2, NUM_ENTRY_TRIG2_ME_FACOTR);

/**************************************low pass filter_LUT****************************************************/
    dyn_arry_2d_free(low_pass_table, NUM_ENTRY_TRIG1_LOW_PASS_FILTER);
    dyn_arry_2d_free_entry(low_pass_entry1_pickup, LENGTH_TABLE_LOW_PASS_FILTER);
    dyn_arry_2d_free(low_pass_trigger1, NUM_ENTRY_TRIG1_LOW_PASS_FILTER);

/**************************************tone_mapping_LUT****************************************************/
    dyn_arry_2d_free(tone_mapping_table, NUM_ENTRY_TRIG1_TONE_MAPPING);
    dyn_arry_2d_free_entry(tone_mapping_entry1_pickup, LENGTH_TABLE_TONE_MAPPING);
    dyn_arry_2d_free(tone_mapping_trigger1, NUM_ENTRY_TRIG1_TONE_MAPPING);

/**************************************gamma_LUT****************************************************/
    dyn_arry_2d_free(gamma_LUT_table, NUM_ENTRY_TRIG1_GAMMA);
    dyn_arry_2d_free_entry(gamma_LUT_entry1_pickup, LENGTH_TABLE_GAMMA);
    dyn_arry_2d_free(gamma_LUT_trigger1, NUM_ENTRY_TRIG1_GAMMA);

/**************************************desaturate_LUT****************************************************/
    dyn_arry_2d_free(desaturate_LUT_table, NUM_ENTRY_TRIG1_DESATURATE);
    dyn_arry_2d_free_entry(desaturate_LUT_entry1_pickup, LENGTH_TABLE_DESATURATE);
    dyn_arry_2d_free(desaturate_LUT_trigger1, NUM_ENTRY_TRIG1_DESATURATE);

/**************************************autoLTM_LUT****************************************************/
    dyn_arry_2d_free(AutoLTM_LUT_table, NUM_ENTRY_TRIG1_AUTOLTM);
    dyn_arry_2d_free_entry(AutoLTM_LUT_entry1_pickup, LENGTH_TABLE_AUTOLTM);
    dyn_arry_2d_free(AutoLTM_LUT_trigger1, NUM_ENTRY_TRIG1_AUTOLTM);

/**************************************LumaThreshold_LUT****************************************************/
    dyn_arry_2d_free(luma_threshold_table, NUM_ENTRY_TRIG1_LUMA_THRSHOLD);
    dyn_arry_2d_free_entry(luma_threshold_entry1_pickup, LENGTH_TABLE_LUMA_THRSHOLD);
    dyn_arry_2d_free(luma_threshold_trigger1, NUM_ENTRY_TRIG1_LUMA_THRSHOLD);

/**************************************MaxGainToneMapping****************************************************/
    dyn_arry_2d_free(tone_mapping_maxgain_table, NUM_ENTRY_TRIG1_TONE_MAPPING_MAX_GAIN);
    dyn_arry_2d_free_entry(tone_mapping_maxgain_entry1_pickup, LENGTH_TABLE_TONE_MAPPING_MAX_GAIN);
    dyn_arry_2d_free(tone_mapping_maxgain_trigger1, NUM_ENTRY_TRIG1_TONE_MAPPING_MAX_GAIN);




}


float **dyn_arry_2d(unsigned int m, unsigned int n)
{
    unsigned int i;

    float **arry = (float**)malloc(m*sizeof(float*));
    for(i = 0; i < m; i++)
    {
        arry[i] = (float *)malloc(n*sizeof(float));
    }

    return arry;
}

void dyn_arry_2d_free(float **arry, unsigned int m)
{
    unsigned int i;

    for(i = 0; i < m; i++)
    {
        free(arry[i]);
    }
    free(arry);
}


entry_type **dyn_arry_2d_entry(unsigned int m, unsigned int n)
{
    unsigned int i;

    entry_type **arry = (entry_type **)malloc(m * sizeof(entry_type));
    for(i = 0; i < m; i++)
    {
        arry[i] = (entry_type *)malloc(n * sizeof(entry_type));
    }

    return arry;
}

void dyn_arry_2d_free_entry(entry_type **arry, unsigned int m)
{
    unsigned int i;

    for(i = 0; i < m; i++)
    {
        free(arry[i]);
    }

    free(arry);
}


float ***dyn_arry_3d(unsigned int l, unsigned int m, unsigned int n)
{

    unsigned int i, j;
    float ***arry=(float ***)malloc(l*sizeof(float **));

    for(i = 0; i < l; i++)
    {
        arry[i]=(float**)malloc(m*sizeof(float*));
        for(j = 0; j < m; j++)
        {
            arry[i][j] = (float *)malloc(n*sizeof(float));
        }
    }

    return arry;
}

void dyn_arry_3d_free(float ***arry, unsigned int l, unsigned int m)
{
    unsigned int i, j;

    for(i = 0; i < l; i++)
    {
        for(j = 0; j < m; j++)
        {
            free(arry[i][j]);
        }
    }

    for(i = 0; i < l; i++)
    {
        free(arry[i]);
    }

    free(arry);
}




