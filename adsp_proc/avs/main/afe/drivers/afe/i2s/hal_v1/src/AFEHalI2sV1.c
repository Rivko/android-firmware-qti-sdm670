/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/hal_v1/src/AFEHalI2sV1.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "LPASS_ADDRESS_FILE.h"


#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEI2sDriver_i.h"
#include "AFEHalI2s.h"
#include "AFEI2sHalFuncDefs.h"
#include "AFEHalDump.h"

#define I2S_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPAIF_I2S_CTL0)  

#ifdef AUDIOIF_REG_DUMP   
typedef struct hal_i2s_dump_v1
{
  uint32_t LPASS_LPAIF_I2S_CTLa_DUMP[MAX_I2S_TYPE_NUM][LPA_IF_SEQ_MAX];
}  hal_i2s_dump_v1_t;

static hal_i2s_dump_v1_t afe_hal_i2s_dump_v1;

static void hal_i2s_dump_v1(uint32_t intf_index, uint32_t dir, uint32_t dump_seq);
#endif

void hal_i2s_init_v1(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t i2s_offset, uint32_t int_i2s_offset);
void hal_i2s_config_v1(uint32_t i2s_index, uint32_t dir, i2s_config_t *config_ptr);
void hal_i2s_enable_v1(uint32_t i2s_index, uint32_t dir);
void hal_i2s_disable_v1(uint32_t i2s_index, uint32_t dir);
uint32_t hal_i2s_get_intf_v1(uint32_t i2s_index, uint32_t dir);


void hal_i2s_get_func_table_v1(afe_i2s_hal_func_def_t *func_table)
{
  if(NULL == func_table)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_i2s_get_func_table_v1 func_table is NULL");
    return;
  }

  func_table->hal_i2s_init_fptr                 = hal_i2s_init_v1;
  func_table->hal_i2s_config_fptr               = hal_i2s_config_v1;
  func_table->hal_i2s_enable_fptr               = hal_i2s_enable_v1;
  func_table->hal_i2s_disable_fptr              = hal_i2s_disable_v1;
  func_table->hal_i2s_get_intf_fptr             = hal_i2s_get_intf_v1;
#ifdef AUDIOIF_REG_DUMP  
  func_table->hal_i2s_dump_fptr                = hal_i2s_dump_v1;
#endif
}


//global data
/**
 * Address related definitions
 */
static uint32_t                  i2s_phy_base; 
static int32_t                   i2s_virt_offset;
static int32_t                   int_i2s_virt_offset;
static uint32_t                  i2s_version;

void hal_i2s_init_v1(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t i2s_offset, uint32_t int_i2s_offset)
{
  i2s_version  = version;
  i2s_phy_base = phy_base;

  ///< Calculate offset of virtual address in words (4 bytes)
  i2s_virt_offset = reg_virt_offset(virt_base, phy_base, i2s_offset);
  if (0 != int_i2s_offset)
  {
    int_i2s_virt_offset = reg_virt_offset(virt_base, phy_base, int_i2s_offset);
  }


  MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i2s version:[0x%x], i2s phy base:[0x%x], i2s virt base:[0x%x], i2s offset:[0x%x], i2s virt offset:[0x%x],\
         int i2s virt offset:[0x%x]", i2s_version, i2s_phy_base, virt_base, i2s_offset, i2s_virt_offset,int_i2s_virt_offset);

}

/** 
 * config the instance of the I2S specified .     
 * 
 * @param[in] i2s_index      I2S instance that needs to be reset.
 * @param[in] dir       0 : Sink, 1:  Source path
 * @param[in] configPtr Pointer to the configuration parameters. 
 * Registers Affeceted:
 *                LPAIF_I2S_CTLa
 */
void hal_i2s_config_v1(uint32_t i2s_index, uint32_t dir, i2s_config_t *config_ptr)
{
  uint32_t mask,field_value, bit_width =0, mono_info, ws_src;

  /* Reset state will go to master, i.e. internal mode */
  /* (SLAVE|MASTER): NEED TO CHECK THE ADDRESS, THE BASE IS WRONG */

  /* Reset state will go to I2S block */

  if(config_ptr->stereo_mode)
  {
    mono_info = LPAIF_I2S_CTLa__SPKR_MONO__STEREO;

  }
  else
  {
    mono_info = LPAIF_I2S_CTLa__SPKR_MONO__MONO;
  }

  if(I2S_SINK == dir)
  {
    //SPKR path: set SPKR_MODE and SPKR_MONO
    field_value = (config_ptr->channel_mode << LPAIF_I2S_CTLa__SPKR_MODE___S)
               | (mono_info << LPAIF_I2S_CTLa__SPKR_MONO___S);
    mask = LPAIF_I2S_CTLa__SPKR_MODE___M | LPAIF_I2S_CTLa__SPKR_MONO___M;
  }
  else
  {
    //MIC path: set MIC_MODE and MIC_MONO
    field_value = (config_ptr->channel_mode << LPAIF_I2S_CTLa__MIC_MODE___S)
               | (mono_info << LPAIF_I2S_CTLa__MIC_MONO___S);
    mask = LPAIF_I2S_CTLa__MIC_MODE___M | LPAIF_I2S_CTLa__MIC_MONO___M;
  }

  switch(config_ptr->bit_width)
  {
    case 16:
      bit_width = LPAIF_I2S_CTLa__BIT_WIDTH__ENUM_16; // 0
      break;
    case 24:
      bit_width = LPAIF_I2S_CTLa__BIT_WIDTH__ENUM_24; // 1
      break;
    case 32:
      bit_width = LPAIF_I2S_CTLa__BIT_WIDTH__ENUM_32; // 2
      break;

    default:
      assert(0);
      break;
  }

  if(config_ptr->ws_master)
  {
    ws_src = LPAIF_I2S_CTLa__WS_SRC__INT;
  }
  else
  {
    ws_src = LPAIF_I2S_CTLa__WS_SRC__EXT;
  }

  //set WS_SRC and BIT_WIDTH
  field_value |= (ws_src << LPAIF_I2S_CTLa__WS_SRC___S)
           | (bit_width << LPAIF_I2S_CTLa__BIT_WIDTH___S);
  mask |= LPAIF_I2S_CTLa__WS_SRC___M | LPAIF_I2S_CTLa__BIT_WIDTH___M;

  //disable LOOPBACK mode
  field_value |= LPAIF_I2S_CTLa__LOOPBACK__DISABLE << LPAIF_I2S_CTLa__LOOPBACK___S;
  mask |= LPAIF_I2S_CTLa__LOOPBACK___M;


  UpdateRegister1(i2s_phy_base + I2S_REG_OFFSET(LPASS_LPAIF_I2S_CTLa(i2s_index)),i2s_virt_offset,mask,field_value,0);
}


/** 
 * Enable the specified I2S instances.
 * 
 * @param[in] i2s_index   I2S Instance to be enabled.
 * @param[in] dir    I2S instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_I2S_CTLa 
 */
void hal_i2s_enable_v1(uint32_t i2s_index, uint32_t dir)
{
  uint32_t mask,field_value;


  if(I2S_SINK == dir)
  {
    //enable SPKR path
    field_value = LPAIF_I2S_CTLa__SPKR_EN__ENABLE << LPAIF_I2S_CTLa__SPKR_EN___S;
    mask = LPAIF_I2S_CTLa__SPKR_EN___M;
  }
  else
  {
    //enable MIC path
    field_value = LPAIF_I2S_CTLa__MIC_EN__ENABLE << LPAIF_I2S_CTLa__MIC_EN___S;
    mask = LPAIF_I2S_CTLa__MIC_EN___M;
  }

  UpdateRegister1(i2s_phy_base + I2S_REG_OFFSET(LPASS_LPAIF_I2S_CTLa(i2s_index)),i2s_virt_offset,mask,field_value,0); ///< update register
}

/** 
 * Disable the specified I2S instances.
 * 
 * @param[in] i2s_index  I2S Instance to be enabled.
 * @param[in] dir    I2S instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_I2S_CTLa 
 */
void hal_i2s_disable_v1(uint32_t i2s_index, uint32_t dir)
{
  uint32_t mask,field_value;

  if(I2S_SINK == dir)
  {
    //enable SPKR path
    field_value = LPAIF_I2S_CTLa__SPKR_EN__DISABLE << LPAIF_I2S_CTLa__SPKR_EN___S;
    mask = LPAIF_I2S_CTLa__SPKR_EN___M;
  }
  else
  {
    //enable MIC path
    field_value = LPAIF_I2S_CTLa__MIC_EN__DISABLE << LPAIF_I2S_CTLa__MIC_EN___S;
    mask = LPAIF_I2S_CTLa__MIC_EN___M;
  }

  UpdateRegister1(i2s_phy_base + I2S_REG_OFFSET(LPASS_LPAIF_I2S_CTLa(i2s_index)),i2s_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * get the register value for the specified I2S index  
 * 
 * @param[in] i2s_index   I2S index
 *
 * @return   the register value for the specified I2S index  
 */
uint32 hal_i2s_get_intf_v1(uint32_t i2s_index, uint32_t dir)
{   
  uint32_t field_value = 0;

  switch (i2s_index)
  {
    case I2S_PRIMARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__PRI_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__PRI_MI2S;
      }
      break;
    case I2S_SECONDARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__SEC_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__SEC_MI2S;
      }
      break;
    case I2S_TERTIARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__TER_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__TER_MI2S;
      }
      break;
    case I2S_QUATERNARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__QUA_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__QUA_MI2S;
      }
      break;
    case I2S_SPEAKER_I2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__SPKR_I2S;
      }
      break;
    case I2S_QUINARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__QUI_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__QUI_MI2S;
      }
      break;
    case I2S_SENARY_MI2S:
      if(I2S_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__SEN_MI2S;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__SEN_MI2S;
      }
      break;
    default:
      // TODO: Error case Should not happen. HAL cannot handle it gracefully.
      field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__NONE;
      break;
  }

  return field_value;
}

#ifdef AUDIOIF_REG_DUMP  
static void hal_i2s_dump_v1(uint32_t intf_index, uint32_t dir, uint32_t dump_seq)
{
  if(MAX_I2S_TYPE_NUM > intf_index)
  {
    ReadRegister1(i2s_phy_base + I2S_REG_OFFSET(LPASS_LPAIF_I2S_CTLa(intf_index)), i2s_virt_offset, &afe_hal_i2s_dump_v1.LPASS_LPAIF_I2S_CTLa_DUMP[intf_index][dump_seq]);
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_I2S_CTLa(addr: 0x%x) = 0x%x", LPASS_LPAIF_I2S_CTLa(intf_index), afe_hal_i2s_dump_v1.LPASS_LPAIF_I2S_CTLa_DUMP[intf_index][dump_seq]);
  }
}         
#endif



