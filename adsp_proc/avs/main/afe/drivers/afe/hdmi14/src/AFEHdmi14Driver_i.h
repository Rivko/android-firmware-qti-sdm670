#ifndef _AFE_HDMI14_DRIVER_I_H_
#define _AFE_HDMI14_DRIVER_I_H_
/**
@file AFEPcmDriver_i.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHdmi14Driver_i.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEHdmi14Driver.h"
#include "AFEDeviceDriverInterface.h"


#if defined(__cplusplus)
extern "C" {
#endif

/*==========================================================================
  Macro Defines
  ========================================================================== */
static const uint32_t DEFAULT_HDMI14_DMA_BUFFERS   		 =  2;
static const uint32_t DEAFULT_HDMI14_WATERMARK     		 =  8;
static const uint32_t HDMI14_DMA_BUFFER_16BYTE_ALIGNMENT  = 0xF;     //for 16 bytes alignment
static const uint32_t HDMI14_DMA_BUFFER_OFFSET_ALIGNMENT  = 0x1F;     //for 32 bytes alignment
static const uint32_t HDMI14_PACKED_60958_BYTES_PER_SAMPLES   =  4;
/**
 * Swap two 16-bit signed integers 
*/ 
#define afe_swap_s16(arg1, arg2) \
{                                \
   int16_t swap_var;             \
   swap_var = arg1;              \
   arg1 = arg2;                  \
   arg2 = swap_var;              \
}

/** 
 * Swap two 32-bit signed integers 
*/
 
#define afe_swap_s32(arg1, arg2) \
{                                \
   int32_t swap_var;             \
   swap_var = arg1;              \
   arg1 = arg2;                  \
   arg2 = swap_var;              \
}

/** 
 * Swap two 32-bit signed integers, arith right shift arg2 by 
 * shift_val amount 
*/
#define afe_swap_s32_rsh_s2(arg1, arg2, shift_val) \
{                                                  \
   int32_t swap_var;                               \
   swap_var = arg1;                                \
   arg1 = (arg2 >> shift_val);                     \
   arg2 = swap_var;                                \
}

/** 
 * Swap two 32-bit signed integers, arith right shift arg1
*/
#define afe_swap_s32_rsh_s1(arg1, arg2, shift_val) \
{                                                  \
   int32_t swap_var;                               \
   swap_var = (arg1 >> shift_val);                 \
   arg1 = arg2;                                    \
   arg2 = swap_var;                                \
}

/** 
 * Swap two 32-bit signed integers, left shift arg2 by shift_val 
 * amount 
*/

#define afe_swap_s32_lsh_s2(arg1, arg2, shift_val) \
{                                                  \
   int32_t swap_var;                               \
   swap_var = arg1;                                \
   arg1 = (arg2 << shift_val);                     \
   arg2 = swap_var;                                \
}

/** 
 * Swap two 32-bit signed integers, left shift arg1 by shift_val 
 * amount 
*/

#define afe_swap_s32_lsh_s1(arg1, arg2, shift_val) \
{                                                  \
   int32_t swap_var;                               \
   swap_var = (arg1 << shift_val);                 \
   arg1 = arg2;                                    \
   arg2 = swap_var;                                \
}

/** 
 * Swap two 32-bit signed integers Shift left the swapped 
 * integers by shift_val amount 
*/
#define afe_swap_shiftl(arg1, arg2, shift_val)  \
{                                               \
   int32_t swap_var = (arg1 << shift_val);      \
   arg1 = (arg2 << shift_val);                  \
   arg2 = swap_var;                             \
}

/** 
 * Swap two 32-bit signed integers, Arithmetic shift right the 
 * swapped integers by shift_val amount 
*/

#define afe_swap_shiftr(arg1, arg2, shift_val)  \
{                                               \
   int32_t swap_var = (arg1 >> shift_val);      \
   arg1 = (arg2 >> shift_val);                  \
   arg2 = swap_var;                             \
}

typedef enum
{
   HDMI14_DRIVER_DEINIT = 0,
   HDMI14_DRIVER_INIT
} hdmi14_driver_state_t;


/**
* The following type must be used whenever a source/sink are to be represented.
*/
enum
{
    HDMI14_DIR_INVALID = -1,
    HDMI14_SINK = 0 ,
    HDMI14_SOURCE
};


typedef struct
{
   uint32 hdmi14_reg_base;   // hdmi reg base addr
   uint32 hdmi14_reg_size;    // hdmi reg page size
   uint32 hw_revision;   //  pcm hw revision
} hdmi14_dev_cfg_struct_t;


typedef struct hdmi14_port_state
{
   // interface configuration data
   uint32_t direction;   
   uint32_t	dev_cfg_minor_ver;      // device minor config version
   uint32_t sample_rate;      
   uint32_t bit_width;
   uint32_t bytes_per_channel;
   uint32_t dma_input_bytes_per_sample;
   uint32_t num_channels;  

   uint32_t hdmi_ca;    


   uint16_t data_format;     
   uint32_t int_samples_per_period; // number of sampers per interrupt
      
   uint16_t      	q_format_shift_factor;
	dma_device_handle    dma_dev_hdl;      // dma handle

   afe_dev_avt_drift_t hdmi14_avt_drift;

   uint32_t *IEC60958_ch_status_table_ptr;

   uint32_t IEC60958_frame_index;

   // interrupt statistics
   uint32_t             hdmi14_intr_cnt;     
	uint32_t             fifo_ovf_cnt;
	uint32_t 				ahb_err_cnt;

   // callback function for the interrupt
   afe_dev_int_cb_t cb_info;

} hdmi14_port_state_t;


typedef struct hdmi14_drv_state
{
   hdmi14_driver_state_t   driver_state;
   uint32_t                hdmi14_virt_addr;          /* hdmi dma main register virtual address mapping */
   qurt_mem_region_t       hdmi14_qmem_region;        /* hdmi dma register mapping region */  
   uint32_t                hdmi14_dma_virt_addr;      /* hdmi dma main register virtual address mapping */
   qurt_mem_region_t       hdmi14_dma_qmem_region;    /* hdmi dma register mapping region */
   hdmi14_dev_cfg_struct_t hdmi14_hw_info;            /* information from device config */
} hdmi14_drv_state_t;

#define HDMI14_MAX_CA_VALUE 0x1f

//CA value starts from 0, hence array size = HDMI_MAX_CA_VALUE+1.
static const uint8 hdmi14_channel_alloc_array[HDMI14_MAX_CA_VALUE+1][9] =
{

//Ch#1    2     3      4     5     6     7     8     #of channels
    {0,  1,  0,  0,  0,  0,  0,  0,  2},    //ca = 0
    {0,  1,  2,  0,  0,  0,  0,  0,  3},    //ca = 1
    {0,  1,  3,  0,  0,  0,  0,  0,  3},    //ca = 2
    {0,  1,  2,  3,  0,  0,  0,  0,  4},    //ca = 3
    {0,  1,  4,  0,  0,  0,  0,  0,  3},    //ca = 4
    {0,  1,  2,  4,  0,  0,  0,  0,  4},    //ca = 5
    {0,  1,  3,  4,  0,  0,  0,  0,  4},    //ca = 6
    {0,  1,  2,  3,  4,  0,  0,  0,  5},    //ca = 7
    {0,  1,  4,  5,  0,  0,  0,  0,  4},    //ca = 8
    {0,  1,  2,  4,  5,  0,  0,  0,  5},    //ca = 9
    {0,  1,  3,  4,  5,  0,  0,  0,  5},    //ca = 10
    {0,  1,  2,  3,  4,  5,  0,  0,  6},    //ca = 11
    {0,  1,  4,  5,  6,  0,  0,  0,  5},    //ca = 12
    {0,  1,  2,  4,  5,  6,  0,  0,  6},    //ca = 13
    {0,  1,  3,  4,  5,  6,  0,  0,  6},    //ca = 14
    {0,  1,  2,  3,  4,  5,  6,  0,  7},    //ca = 15
    {0,  1,  4,  5,  6,  7,  0,  0,  6},    //ca = 16
    {0,  1,  2,  4,  5,  6,  7,  0,  7},    //ca = 17
    {0,  1,  3,  4,  5,  6,  7,  0,  7},    //ca = 18
    {0,  1,  2,  3,  4,  5,  6,  7,  8},    //ca = 19
    {0,  1,  6,  7,  0,  0,  0,  0,  4},    //ca = 20
    {0,  1,  2,  6,  7,  0,  0,  0,  5},    //ca = 21
    {0,  1,  3,  6,  7,  0,  0,  0,  5},    //ca = 22
    {0,  1,  2,  3,  6,  7,  0,  0,  6},    //ca = 23
    {0,  1,  4,  6,  7,  0,  0,  0,  5},    //ca = 24
    {0,  1,  2,  4,  6,  7,  0,  0,  6},    //ca = 25
    {0,  1,  3,  4,  6,  7,  0,  0,  6},    //ca = 26
    {0,  1,  2,  3,  4,  6,  7,  0,  7},    //ca = 27
    {0,  1,  4,  5,  6,  7,  0,  0,  6},    //ca = 28
    {0,  1,  2,  4,  5,  6,  7,  0,  7},    //ca = 29
    {0,  1,  3,  4,  5,  6,  7,  0,  7},    //ca = 30
    {0,  1,  2,  3,  4,  5,  6,  7,  8},    //ca = 31
};


#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_HDMI14_DRIVER_I_H_)
