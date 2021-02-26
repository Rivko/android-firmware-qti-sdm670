#ifndef _AFE_I2S_DRIVER_I_H_
#define _AFE_I2S_DRIVER_I_H_
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
 */

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sDriver_i.h#3 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEI2sDriver.h"
#include "AFEDeviceDriverInterface.h"
#include "lpasshwio_devcfg.h"
#include "audio_basic_op.h"


#if defined(__cplusplus)
extern "C"
{
#endif

  /*==========================================================================
  Macro Defines
  ========================================================================== */
  static const uint32_t DEFAULT_I2S_DMA_BUFFERS        =  2;
static const uint32_t MAX_I2S_DMA_BUFFERS            =  8;
  static const uint32_t DEAFULT_I2S_WATERMARK          =  8;
  static const uint32_t I2S_DMA_BUFFER_16BYTE_ALIGNMENT  = 0xF;     //for 16 bytes alignment
  static const uint32_t I2S_DMA_BUFFER_OFFSET_ALIGNMENT  = 0x1F;     //for 32 bytes alignment
  static const uint32_t PACKED_60958_BYTES_PER_SAMPLES   =  4;

#define IEC60958_BLOCK_SIZE   192   //one IEC60958 block has 192 frames, one IEC60958 frame has two subframes, and one subframe has 32 time slots.

#define IEC60958_P_BIT31          31
#define IEC60958_C_BIT30          30
#define IEC60958_U_BIT29          29
#define IEC60958_V_BIT28          28

#define IEC60958_PREAMBLE_BIT3    3
#define IEC60958_PREAMBLE_BIT2    2
#define IEC60958_PREAMBLE_BIT1    1
#define IEC60958_PREAMBLE_BIT0    0

#define IEC60958_AUX_START_BIT4   4
#define IEC60958_MSB_ALIGN_BIT8   8


#define IEC60958_MSB_ALIGN_BITS_MASK  0x4
#define IEC60958_GET_FLAGS_INFO(x)    ((x >> 28) & 0x0F)
#define IEC60958_GET_AUDIO_SAMPLE(x)  (x & 0x0FFFFFF0)


  /* Preamble bits  */
#define IEC60958_PREAMBLE_M_VAL   0x02    //0b0010
#define IEC60958_PREAMBLE_W_VAL   0x04    //0b0100
#define IEC60958_PREAMBLE_B_VAL   0x08    //0b1000

#define IEC60958_PREAMBLE_MASK         0x0F


#define IEC60958_V_BIT_MASK    0x1
#define IEC60958_U_BIT_MASK    0x2
#define IEC60958_C_BIT_MASK    0x4
#define IEC60958_P_BIT_MASK    0x8


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
  arg1 = s32_shl_s32_sat(arg2, shift_val);        \
  arg2 = swap_var;                                \
    }

  /**
   * Swap two 32-bit signed integers, left shift arg1 by shift_val
   * amount
   */

#define afe_swap_s32_lsh_s1(arg1, arg2, shift_val) \
    {                                                  \
  int32_t swap_var;                               \
  swap_var = s32_shl_s32_sat(arg1, shift_val);    \
  arg1 = arg2;                                    \
  arg2 = swap_var;                                \
    }

  /**
   * Swap two 32-bit signed integers Shift left the swapped
   * integers by shift_val amount
   */
#define afe_swap_shiftl(arg1, arg2, shift_val)  \
    {                                               \
  int32_t swap_var = s32_shl_s32_sat(arg1, shift_val);\
  arg1 = s32_shl_s32_sat(arg2, shift_val);             \
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

#define IS_INTERNAL_MI2S(i2s_index)  ( (I2S_INT0_MI2S <= i2s_index) && \
    (I2S_INT6_MI2S >= i2s_index) )

  typedef enum
  {
    I2S_DRIVER_DEINIT = 0,
    I2S_DRIVER_INIT
  } i2s_driver_state_t;

  typedef enum i2s_intf_idx
  {
    I2S_PRIMARY_MI2S      = 0,                  // Primary MI2S
    I2S_SECONDARY_MI2S    = 1,                  // Secondary MI2S
    I2S_TERTIARY_MI2S     = 2,                  // Tertiary MI2S
    I2S_QUATERNARY_MI2S   = 3,                  // Quaternary MI2S
    I2S_SPEAKER_I2S       = 4,                  // Speaker I2S
    I2S_QUINARY_MI2S      = 5,                  // Quinary MI2S
    I2S_SENARY_MI2S       = 6,                  // Senary MI2S
    I2S_INT0_MI2S      =  7,                   // INT 0 MI2S
    I2S_INT1_MI2S      =  8,                   // INT 1 MI2S
    I2S_INT2_MI2S      =  9,                   // INT 2 MI2S
    I2S_INT3_MI2S      =  10,                  // INT 3 MI2S
    I2S_INT4_MI2S      =  11,                  // INT 4 MI2S
    I2S_INT5_MI2S      =  12,                   // INT 5 MI2S
    I2S_INT6_MI2S      =  13,                   // INT 6 MI2S
    I2S_INTERFACE_MAX     = I2S_INT6_MI2S,    // Max index
    I2S_INTERFACE_INVALID = 0x7FFFFFFF          // Invalid index
  } i2s_intf_idx_t;

  /**
   * The following type must be used whenever a source/sink are to be represented.
   */
  enum
  {
    I2S_DIR_INVALID = -1,
    I2S_SINK = 0 ,
    I2S_SOURCE
  };

  typedef enum
  {
    I2S_INTERFACE_STOP = 0,
    I2S_INTERFACE_START
  } i2s_inteface_state_t;


  /**
   * The following are the possible SPKR or MIC modes
   */
  enum
  {
    I2S_CHANNEL_MODE_NONE = 0,
    I2S_CHANNEL_MODE_SD0,               //SD0 only - stereo or mono)
    I2S_CHANNEL_MODE_SD1,                //SD1 only - stereo or mono)
    I2S_CHANNEL_MODE_SD2,                //SD2 only - stereo or mono)
    I2S_CHANNEL_MODE_SD3,                //SD3 only - stereo or mono)
    I2S_CHANNEL_MODE_SD0_AND_SD1,        //SD0 & SD1 = quad channel mode)
    I2S_CHANNEL_MODE_SD2_AND_SD3,        //SD2 & SD3 = quad channel mode)
    I2S_CHANNEL_MODE_6_CHANNELS,         //SD0, SD1 & SD2 = 6 channel mode)
    I2S_CHANNEL_MODE_8_CHANNELS,         //SD0, SD1, SD2 & SD3 = 8 channel mode)
    I2S_CHANNEL_MODE_MAX = 0x7FFFFFFF
  };

  /**
   * Bit mask enum for data line usage
   * BIT0: SD0, BIT1: SD1, BIT2: SD2, BIT3: SD3
   */
  typedef enum i2s_channel_mode_mask
  {
    I2S_CHANNEL_MODE_UNUSED           = 0,
    I2S_CHANNEL_MODE_SD0_MASK         = 0x1,  /* SD0 only - stereo or mono) */
    I2S_CHANNEL_MODE_SD1_MASK         = 0x2,  /* SD1 only - stereo or mono) */
    I2S_CHANNEL_MODE_SD0_AND_SD1_MASK = 0x3,  /* SD0 & SD1 = quad channel mode) */
    I2S_CHANNEL_MODE_SD2_MASK         = 0x4,  /* SD2 only - stereo or mono) */
    I2S_CHANNEL_MODE_SD3_MASK         = 0x8,  /* SD3 only - stereo or mono) */
    I2S_CHANNEL_MODE_SD2_AND_SD3_MASK = 0xC,  /* SD2 & SD2 = quad channel mode */
    I2S_CHANNEL_MODE_6_CHANNELS_MASK  = 0xE,  /* SD0, SD1 & SD2 = 6 channel mode) */
    I2S_CHANNEL_MODE_8_CHANNELS_MASK  = 0xF,  /* SD0, SD1, SD2 & SD3 = 8 channel mode) */
    I2S_CHANNEL_MODE_MASK_MAX         = 0x7FFFFFFF
  }i2s_channel_mode_mask_t;

  /**
   * MSM can be in the master or slave mode. In the I2S interface, this will
   * determine which word select clk (internal or external) is used. In PCM
   * interface, it will determine the src for the sync signal
   */
  enum
  {
    I2S_MSM_MODE_INVALID =-1,
    I2S_MSM_MODE_SLAVE = 0,
    I2S_MSM_MODE_MASTER,
    I2S_MSM_MODE_MAX = 0x7FFFFFFF
  } ;

  enum
  {
    I2S_IEC60958_DATA_SHIFT_LEFT = 0 ,
    I2S_IEC60958_DATA_SHIFT_RIGHT = 1
  };

  typedef struct 
  {
    uint32_t intf_idx;    // Interface index number
    uint32_t inf_type_data_line_caps; // maximum # of channel for i2s
    uint32_t inf_type_direction; // 0x1 speaker path only,  0x2 mic path only,   0x3: bidirectional
    uint32_t num_ports;     // num ports for the interface
    uint32_t *port_list;    // List of port IDs
  }  i2s_intf_port_info_t;

  typedef struct
  {
    uint32_t i2s_reg_base;   // lpaif reg base addr
    uint32_t i2s_reg_size;    // lpaif reg page size
    uint32_t i2s_reg_offset;  // i2s reg addr
    uint32_t int_i2s_reg_offset; //internal i2s reg addr
    uint32_t inf_type_cnt;      // # of i2s interface type count
    i2s_intf_port_info_t *intf_port_info[MAX_I2S_TYPE_NUM];// interface port info ptr
    uint32_t hw_revision;   //  i2s hw revision
  } i2s_dev_cfg_struct_t;

  typedef struct
  {
    uint8_t validity_flag;
    uint8_t user_data;
    uint8_t ch_status;
    uint8_t parity_bit;
  } i2s_IEC60958_subframe_info_t;


  typedef struct i2s_port_state
  {
    // interface configuration data
    i2s_inteface_state_t state;
    i2s_intf_idx_t intf;
    uint32_t direction;
    uint32_t  dev_cfg_minor_ver;      // device minor config version
    uint32_t sample_rate;
    uint32_t bit_width;
    uint32_t bytes_per_channel;
    uint32_t dma_input_bytes_per_sample;
    uint32_t num_channels;
    uint32_t msm_mode;             // MSM is master or slave mode
    uint32_t channel_mode;     // I2S: channel mode (SD0, SD1, SD2, SD3, SD0&SD1, SD2&SD3, Six channel, eight chan)
    uint32_t channel_mode_mask;
    uint16_t data_format;
    uint32_t int_samples_per_period; // number of sampers per interrupt

    uint16_t        q_format_shift_factor;
    dma_device_handle    dma_dev_hdl;      // dma handle

    // interrupt statistics
    uint32_t             i2s_intr_cnt;
    uint32_t             fifo_ovf_cnt;
    uint32_t             ahb_err_cnt;

    // callback function for the interrupt
    afe_dev_int_cb_t cb_info;

    //     IEC60958 Related declarations
    uint32_t            *IEC60958_ch_status_table_ptr;
    uint32_t             IEC60958_frame_index;
    uint32_t             audio_blk_sync; /*Flag to indicate that first audio block is received*/

    // i2s restart use case.
    bool_t           is_restart_seq;
    uint32_t          new_sampling_rate;
  } i2s_port_state_t;

  typedef struct i2s_drv_state
  {
    i2s_driver_state_t      driver_state;
    /* I2S driver init status */
    uint32_t                i2s_virt_addr;
    /* I2S main register virtual address mapping */
    qurt_mem_region_t       i2s_qmem_region;
    /* i2s register mapping region */
    i2s_dev_cfg_struct_t    i2s_hw_info;
    /* information from device config */
    i2s_port_state_t        *p_i2s_intf_state[MAX_I2S_TYPE_NUM*2];
    /* Pointer to individual interface state object */
    qurt_elite_mutex_t      i2s_drv_mutex;
    /* I2S Driver mutex */
  } i2s_drv_state_t;

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_I2S_DRIVER_I_H_)
