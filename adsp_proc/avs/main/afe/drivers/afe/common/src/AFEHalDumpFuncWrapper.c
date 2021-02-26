/** @file HALaudioif_wrapper.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEHalDumpFuncWrapper.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "qurt_elite.h"
#include "AFEHalDumpFuncDefs.h"
#include "AFEHalDump.h"
#include "AFEHalI2s.h"
#include "AFEHalPcm.h"
#include "AFEHalDmaManagerCommon.h"
#include "AFEHalAudioIfCommon.h"
#include "AFEDmaManager.h"

#ifdef AUDIOIF_REG_DUMP  

//global data
/* AFE HAL DUMP  function table */
static afe_hal_dump_func_def_t  afe_hal_dump_table;


#define HAL_AFE_HW_VER_3_8_0  (0x30080000)

#define HAL_AFE_HW_VER_4_0_0  (0x40000000)


inline void hal_afe_dump_init(uint32_t version)
{

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " hal_afe_dump_init() is called version 0x%x", version);

   switch(version)
   {
      case HAL_AFE_HW_VER_3_8_0:
      {
         hal_afe_dump_get_func_table_v1(&afe_hal_dump_table);
      }
      break;
      case HAL_AFE_HW_VER_4_0_0:
      {
         hal_afe_dump_get_func_table_v2(&afe_hal_dump_table);
      }
      break;
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " unknown hal dump version 0x%x", version);
      }
      break;      
   }
 
   afe_hal_dump_table.hal_afe_dump_init_fptr(version);
 
}

inline void hal_afe_dump_deinit(void)
{
   afe_hal_dump_table.hal_afe_dump_deinit_fptr();
}

/** 
 * dump clock register for the interface  .     
 * @param[in] intf_type       interface type :  I2S or PCM 
 * @param[in] pcm_index    interface instance that needs to be dump.
 * @param[in] dump_seq    dump sequnce : start 0 or stop 1
 */
inline void hal_afe_clock_dump(uint32_t intf_type, uint32_t intf_index, uint32_t dump_seq)
{
   afe_hal_dump_table.hal_afe_clock_dump_fptr(intf_type, intf_index, dump_seq);
   
}


/** 
 * dump clock register for the interface  .     
 * @param[in] intf_type       interface type :  I2S or PCM 
 * @param[in] pcm_index    interface instance that needs to be dump.
 * @param[in] dir       0 : Sink, 1:  Source path
 * @param[in] channel  :    dma channel
  * @param[in] dump_seq    dump sequnce : start 0 or stop 1
 */   
inline void hal_afe_audioif_dump(uint32_t intf_type, uint32_t intf_index, uint32_t dir, uint32_t channel, uint32_t dump_seq)
{
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== start hal_afe_audioif intf_type[%d] index[%d] dir[%d] channel[%d] dump_seq[%d]====", \
                intf_type, intf_index, dir, channel, dump_seq);  

   switch(intf_type)
   {
      case LPA_IF_I2S: 
      {
         hal_i2s_dump(intf_index, dir, dump_seq);
      }
      break;
      case LPA_IF_PCM:
      {
         hal_pcm_dump(intf_index, dir, dump_seq);
      }
      break;
      case LPA_IF_HDMI14:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI14 interface ====");         
      }
      break;
      default:
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Undefined Interface %d", intf_index);
      break;
   }

   // mux print 
   hal_afe_audioif_mux_dump(intf_index, dump_seq);

   // dma print
   afe_hal_dma_dump(LPASS_HW_DMA_TYPE0,  dir, channel, dump_seq) ;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== End hal_afe_audioif_dump ====");  
}


#endif

