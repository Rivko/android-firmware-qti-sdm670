/**

@file   fft.c
@brief  fft for AUDIO FTM External Loopback
====================================================================================================
Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

**/
#ifdef __cplusplus
extern "C" {
#endif

#include "fft.h"
#include <math.h>
#include <stdlib.h>

/* Returns the pos of peak power in spectrum */
int32_t get_max_pos(complex_for_fft *spectrum, uint32_t length)
{
    uint32_t i;
    uint32_t pos = 0;
    float tmp = 0;
    float max = 0;

    if(NULL == spectrum)
    {
        fprintf(stderr, "spectrum buffer is empty.\n");
        return -1;
    }

    max = spectrum[0].real * spectrum[0].real + spectrum[0].imag* spectrum[0].imag;
    for(i = 1; i < length; i++)
    {
        tmp = spectrum[i].real * spectrum[i].real + spectrum[i].imag* spectrum[i].imag;
        if(max < tmp)
        {
            max = tmp;
            pos = i;
        }
    }

    return pos;
}

/* Creates the sin lookup table */
int32_t create_sin_table(float* sin_table_buffer, uint32_t n_of_fft)
{
    uint32_t i = 0;

    if(NULL == sin_table_buffer)
    {
        fprintf(stderr, "sin_table_buffer buffer is empty.\n");
        return -1;
    }

    for(i = 0; i <= n_of_fft/4; i++)
        sin_table_buffer[i] = sin(PI*i/ (n_of_fft/2));

    return 0;
}

/* lookup sin value from lookup table */
float sin_find(float x, float* sin_table_buffer, uint32_t n_of_fft)
{
    uint32_t i = (uint32_t)(n_of_fft*x);

    if(NULL == sin_table_buffer)
    {
        fprintf(stderr, "sin_table_buffer buffer is empty.\n");
        return -1;
    }

    i = i>>1;
    if(i > (n_of_fft/4))
        i = (n_of_fft/2) - i;

    return sin_table_buffer[i];
}

/* lookup cos value from lookup table */
float cos_find(float x, float* sin_table_buffer, uint32_t n_of_fft)
{
    uint32_t i = (uint32_t)(n_of_fft*x);

    if(NULL == sin_table_buffer)
    {
        fprintf(stderr, "sin_table_buffer buffer is empty.\n");
        return -1;
    }

    i = i>>1;
    if(i < n_of_fft/4 )
        return sin_table_buffer[n_of_fft/4 - i];
    else
        return -sin_table_buffer[i - n_of_fft/4];
}

/* reverse the data sequence for FFT */
int32_t bit_reverse(complex_for_fft *input_data, uint32_t n_of_fft)
{
    int32_t nextValue,nextM,i,k,j=0;
    complex_for_fft temp;

    if(NULL == input_data)
    {
        fprintf(stderr, "sin_table_buffer buffer is empty.\n");
        return -1;
    }

    nextValue = n_of_fft/2;
    nextM = n_of_fft - 1;
    for(i = 0; i < nextM; i++)
    {
        if(i < j)
        {
            temp = input_data[j];
            input_data[j] = input_data[i];
            input_data[i] = temp;
        }
        k = nextValue;
        while(k <= j)
        {
            j = j - k;
            k = k/2;
        }
        j = j + k;
    }

    return 0;
}

int32_t float_fft(complex_for_fft *workspace, uint32_t n_of_fft, uint32_t fft_stage)
{
    uint32_t current_stage = 0;
    int32_t denominator = 0;
    int32_t n = 0;
    uint32_t k = 0;
    int32_t k_pair = 0;
    int32_t step = 0;
    float angle;
    complex_for_fft twiddle,temp;
    float *sin_lookup_table;

    if(NULL == workspace)
    {
        fprintf(stderr, "workspace buffer is empty.\n");
        return -1;
    }

    sin_lookup_table = (float*)malloc(n_of_fft * sizeof(float));
    if (!sin_lookup_table) {
        fprintf(stderr, "Unable to allocate %d bytes\n", n_of_fft * sizeof(float));
        return -1;
    }

    create_sin_table(sin_lookup_table, n_of_fft);
    bit_reverse(workspace, n_of_fft);

    /* Cooley-Tukey FFT algorithm */
    for(current_stage = 1; current_stage <= fft_stage; current_stage++)
    {
        /* step size for current stage */
        step = 1<<current_stage;

        /* exp(j*2*PI*n/N) = exp(j*PI*n/(N/2)). N/2 is the denominator */
        denominator = step>>1;
        for(n = 0; n < denominator; n++)
        {
            angle = (float)n/denominator;
            twiddle.imag =  -sin_find(angle, sin_lookup_table, n_of_fft);
            twiddle.real =   cos_find(angle, sin_lookup_table, n_of_fft);

            /* butterfly calculation */
            for(k = n; k < n_of_fft; k = k + step)
            {
                /* data[k_pair] and data[k] work in pair.  */
                k_pair = k + denominator;
                temp.real = workspace[k_pair].real * twiddle.real -
                            workspace[k_pair].imag*twiddle.imag;
                temp.imag = twiddle.imag*workspace[k_pair].real +
                            workspace[k_pair].imag*twiddle.real;
                workspace[k_pair].real = workspace[k].real - temp.real;
                workspace[k_pair].imag = workspace[k].imag - temp.imag;
                workspace[k].real = workspace[k].real + temp.real;
                workspace[k].imag = workspace[k].imag + temp.imag;
            }
        }
    }
    free(sin_lookup_table);
    return 0;
}

#ifdef __cplusplus
}
#endif
