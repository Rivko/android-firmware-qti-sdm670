#ifndef _AFE_TDM_DRIVER_I_H_
#define _AFE_TDM_DRIVER_I_H_
/**
@file AFETdmDriver_i.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.

Copyright (c) 2013 - 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmDriver_i.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFETdmDriver.h"
#include "AFEDeviceDriverInterface.h"


#if defined(__cplusplus)
extern "C" {
#endif

/*==========================================================================
  Macro Defines
  ========================================================================== */
static const uint32_t DEFAULT_TDM_DMA_BUFFERS   	   =  2;
static const uint32_t DEAFULT_TDM_WATERMARK     	   =  8;
static const uint32_t MAX_TDM_DMA_BUFFERS            =  4;
static const uint32_t TDM_DMA_BUFFER_16BYTE_ALIGNMENT  = 0xF;     //for 16 bytes alignment
static const uint32_t TDM_DMA_BUFFER_OFFSET_ALIGNMENT  = 0x1F;     //for 32 bytes alignment
static const uint32_t AFE_AUX_TDM_MAX_NUM_SLOTS        = 4;

#define MAX_TDM_PORT_NUM 80

#define MAX_TDM_SLOTS 32

typedef enum
{
   TDM_DRIVER_DEINIT = 0,
   TDM_DRIVER_INIT
} tdm_driver_state_t;

typedef enum tdm_interface
{
   TDM_PRIMARY      =    0,      //primary TDM 
   TDM_SECONDARY    =    1,      //Secondary TDM
   TDM_TERTIARY     =    2,      //Tertiary TDM
   TDM_QUATERNARY   =    3,      //Quaternary TDM
   TDM_QUINARY      =    4,      //Quinary TDM
   TDM_INTERFACE_MAX   = TDM_QUINARY
} tdm_interface_t;

/**
* The following type must be used whenever a source/sink are to be represented.
*/
enum
{
    TDM_DIR_INVALID = -1,
    TDM_SINK = 0 ,
    TDM_SOURCE
};

/** 
 * TDM codec can support multiple masters case by setting DATA_OE to tri-state
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
 * determine which word select clk (internal or external) is used. In TDM 
 * interface, it will determine the src for the sync signal
 */
enum 
{
   TDM_MSM_MODE_INVALID =-1,
   TDM_MSM_MODE_SLAVE = 0,
   TDM_MSM_MODE_MASTER,
   TDM_MSM_MODE_MAX = 0x7FFFFFFF
};

/** 
 * TDM codec can be in either aux or pcm modes. PCM mode is also known as short 
 * sync mode & AUX mode is also known as long sync mode     
 */
enum TdmSyncInfo
{
  TDM_SHORT_SYNC_TYPE = 0,
  TDM_LONG_SYNC_TYPE,
  TDM_SLOT_SYNC_TYPE,
};

/** 
 * TDM long sync can be inverted.
 */
enum TdmLongSyncInfo
{
  TDM_LONG_SYNC_NORMAL = 0,
  TDM_LONG_SYNC_INVERT
};

/** 
 * TDM data delay relative to sync.
 * This is hardware value..
 */
enum TdmDataSyncDelayInfo
{
  TDM_DATA_DELAY_0_CYCLE = 2,
  TDM_DATA_DELAY_1_CYCLE = 1,
  TDM_DATA_DELAY_2_CYCLE = 0
};

enum
{
   TDM_NO_CTRL_DATA_OE = -1,
   TDM_CTRL_DATA_OE_DISABLE = 0,
   TDM_CTRL_DATA_OE_ENABLE = 1,
};


typedef struct
{
   uint32 tdm_reg_base;   // lpaif reg base addr
   uint32 tdm_reg_size;    // lpaif reg page size
   uint32 tdm_reg_offset;  // pcm reg page addr
   uint32 inf_type_cnt;      // # of pcm interface type count
   uint32 inf_type_list[MAX_TDM_TYPE_NUM];    // 
   uint32 inf_type_data_line_caps[MAX_TDM_TYPE_NUM]; // maximum # of channel for pcm
   uint32 inf_type_direction[MAX_TDM_TYPE_NUM];  // 0x1 spacker path only,  0x2 mic path only,   0x3: bidirectional 
   uint32 hw_revision;   //  pcm hw revision
} tdm_dev_cfg_struct_t;


typedef struct tdm_port_state
{
   // interface configuration data
   tdm_interface_t intf;

   uint32_t direction;   
   uint32_t	dev_cfg_minor_ver;      // device minor config version

   uint32_t sample_rate;      
   uint32_t bit_width;
   uint32_t bytes_per_channel;
   uint32_t num_channels;  
   
   uint32_t msm_mode;             // MSM is master or slave mode

   // TDM n-slots per frame
   uint32_t nslots_per_frame;
  
   // TDM sync mode: short, long mode
   uint32_t sync_type;

   // TDM sync invert: normal, invert
   uint32_t ctrl_invert_sync_pulse;

   // TDM delay the data w.r.t. sync pulse
   uint32_t ctrl_sync_data_delay;

   // TDM slot width
   uint32_t slot_width;

   // TDM slot mask
   uint32_t slot_mask;

   uint32_t ctrl_data_oe;         // set out signal tri-state to support multiple masters or no
     
   uint16_t data_format;     
   uint32_t int_samples_per_period; // number of sampers per interrupt
      
   uint16_t             q_format_shift_factor;
   dma_device_handle    dma_dev_hdl;      // dma handle

   // interrupt statistics
   uint32_t             tdm_intr_cnt;     
   uint32_t             fifo_ovf_cnt;
   uint32_t             ahb_err_cnt;

   // callback function for the interrupt
   afe_dev_int_cb_t cb_info;

} tdm_port_state_t;


typedef struct tdm_drv_state
{
   tdm_driver_state_t      driver_state;
   uint32_t                tdm_virt_addr;     /* tdm main register virtual address mapping */
   qurt_mem_region_t       tdm_qmem_region;   /* tdm register mapping region */
   tdm_dev_cfg_struct_t    tdm_hw_info;       /* information from device config */
} tdm_drv_state_t;


/**
  @brief Validate whether active slot mask is set properly based 
         on number of channels

  @param[in] slot_mask  Active slot mask
   
  @param[in] num_channels 

  @return  ADSP_EOK on success, an error code on error

*/

ADSPResult afe_tdm_validate_slotmask_num_channels(uint32_t slot_mask, uint32_t num_channels);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_TDM_DRIVER_I_H_)
