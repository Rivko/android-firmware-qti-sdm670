#ifndef _AFE_PCM_DRIVER_I_H_
#define _AFE_PCM_DRIVER_I_H_
/**
@file AFEPcmDriver_i.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/src/AFEPcmDriver_i.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEPcmDriver.h"
#include "AFEDeviceDriverInterface.h"


#if defined(__cplusplus)
extern "C" {
#endif

/*==========================================================================
  Macro Defines
  ========================================================================== */
static const uint32_t DEFAULT_PCM_DMA_BUFFERS   		 =  2;
static const uint32_t DEAFULT_PCM_WATERMARK     		 =  8;
static const uint32_t MAX_PCM_DMA_BUFFERS              =  4;
static const uint32_t PCM_DMA_BUFFER_16BYTE_ALIGNMENT  = 0xF;     //for 16 bytes alignment
static const uint32_t PCM_DMA_BUFFER_OFFSET_ALIGNMENT  = 0x1F;     //for 32 bytes alignment

#define AFE_AUX_PCM_MAX_NUM_SLOTS       4


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
   PCM_DRIVER_DEINIT = 0,
   PCM_DRIVER_INIT
} pcm_driver_state_t;

/* PCM interface index types, used to configure
 * LPASS_LPAIF_PCM_CTLa and LPASS_LPAIF_PCM_I2S_SELa */
typedef enum pcm_intf_idx
{
   PCM_PRIMARY          = 0, //primary PCM
   PCM_SECONDARY        = 1, //Secondary PCM
   PCM_TERTIARY         = 2, //Tertiary PCM
   PCM_QUATERNARY       = 3, //Quaternary PCM
   PCM_QUINARY          = 4, //Quinary PCM
   PCM_INTERFACE_MAX    = PCM_QUINARY
} pcm_intf_idx_t;

/**
* The following type must be used whenever a source/sink are to be represented.
*/
enum
{
    PCM_DIR_INVALID = -1,
    PCM_SINK = 0 ,
    PCM_SOURCE
};

/**
 * The following type must be used to set the number of bits clocks per PCM
 * Frame. 
 */ 
enum  //frame type
{
   NO_PCMFRAME =-1,
   BITS_PER_FRAME_8 = 0,
   BITS_PER_FRAME_16,
   BITS_PER_FRAME_32,
   BITS_PER_FRAME_64,
   BITS_PER_FRAME_128,
   BITS_PER_FRAME_256
};

/** 
 * PCM codec can be in either aux or pcm modes. PCM mode is also known as short 
 * sync mode & AUX mode is also known as long sync mode     
 */
enum 
{
   NO_SYNC = -1,
   PCM_MODE = 0,
   AUX_MODE 
};

/**
 * The following are the different types of quantizations
 */ 
enum 
{
   NO_QUANTIZATION =  -1,
   A_LAW_WITHOUT_PADDING = 0,
   MU_LAW_WITHOUT_PADDING,
   LINEAR_WITHOUT_PADDING,
   A_LAW_WITH_PADDING,
   MU_LAW_WITH_PADDING,
   LINEAR_WITH_PADDING
};

/** 
 * PCM codec can support multiple masters case by setting DATA_OE to tri-state
 * data out signal. When clear, PCM block must be the only master.     
 */
enum
{
   NO_CTRL_DATA_OE = -1,
   CTRL_DATA_OE_DISABLE = 0,
   CTRL_DATA_OE_ENABLE = 1,
};


/** 
 * MSM can be in the master or slave mode. In the I2S interface, this will 
 * determine which word select clk (internal or external) is used. In PCM 
 * interface, it will determine the src for the sync signal
 */
enum 
{
   PCM_MSM_MODE_INVALID =-1,
   PCM_MSM_MODE_SLAVE = 0,
   PCM_MSM_MODE_MASTER,
   PCM_MSM_MODE_MAX = 0x7FFFFFFF
} ;


typedef struct
{
   uint32 pcm_reg_base;   // lpaif reg base addr
   uint32 pcm_reg_size;    // lpaif reg page size
   uint32 pcm_reg_offset;  // pcm reg page addr
   uint32 inf_type_cnt;      // # of pcm interface type count
   uint32 inf_type_list[MAX_PCM_TYPE_NUM];    // 
   uint32 inf_type_data_line_caps[MAX_PCM_TYPE_NUM]; // maximum # of channel for pcm
   uint32 inf_type_direction[MAX_PCM_TYPE_NUM];  // 0x1 spacker path only,  0x2 mic path only,   0x3: bidirectional 
   uint32 hw_revision;   //  pcm hw revision
} pcm_dev_cfg_struct_t;


typedef struct pcm_port_state
{
   // interface configuration data
   pcm_intf_idx_t intf;
   uint32_t direction;   
   uint32_t	dev_cfg_minor_ver;      // device minor config version
   uint32_t sample_rate;      
   uint32_t bit_width;
   uint32_t bytes_per_channel;
   uint32_t num_channels;  
   
   uint32_t msm_mode;             // MSM is master or slave mode
   uint32_t slot_num;              //  num of slot  max: 4 
   uint32_t slot_number_mapping[AFE_AUX_PCM_MAX_NUM_SLOTS];              //  Slot no. of data (0-31) for 4 channels
   uint32_t frame;                // Currently, only aux or pcm modes are used, but can be easily extended.
   uint32_t sync;                 // Long sync / short sync pulse
   uint32_t quant;                // Linear or Non-linear quantization
   uint32_t ctrl_data_oe;         // set out signal tri-state to support multiple masters or no

   uint16_t data_format;     
   uint32_t int_samples_per_period; // number of sampers per interrupt
      
   uint16_t      	q_format_shift_factor;
	dma_device_handle    dma_dev_hdl;      // dma handle

   afe_dev_avt_drift_t pcm_avt_drift;

   // interrupt statistics
   uint32_t             pcm_intr_cnt;     
	uint32_t             fifo_ovf_cnt;
	uint32_t 				ahb_err_cnt;

   // callback function for the interrupt
   afe_dev_int_cb_t cb_info;

} pcm_port_state_t;


typedef struct pcm_drv_state
{
   pcm_driver_state_t      driver_state;
   uint32_t                pcm_virt_addr;     /* pcm main register virtual address mapping */
   qurt_mem_region_t       pcm_qmem_region;       /* pcm register mapping region */
   pcm_dev_cfg_struct_t    pcm_hw_info;         /* information from device config */
} pcm_drv_state_t;

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_PCM_DRIVER_I_H_)
