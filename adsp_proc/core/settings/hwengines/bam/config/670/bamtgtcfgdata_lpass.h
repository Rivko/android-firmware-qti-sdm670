/**
  @file bamtgtcfgdata_lpass.h
  @brief
  This file contains configuration data for the BAM driver for the 
  msm670 LPASS system.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/04/17   KS      Updated Slimbus pipe configurations for Warlock
06/05/17   KS      Branched from msm845 and updated for msm670
07/29/16   SS      Branched and updated for msm845
01/25/16   SS      Branched from 8996 and updated for 8998

===============================================================================
                   Copyright (c) 2017 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "bam_targetcfg.h"
/** Configs supported for
    SLIMBUS
    SLIMBUS1
    CE1
    BAM_TGT_CFG_LAST
 */
bam_sec_config_type bam_tgt_slimbus_aud_secconfig_670 =
{
    /*.ee*/         {
                       /*[0]*/          {
                       /*.pipe_mask*/       0x0003ff87,
                       /*.vmid*/            0x0,
                       /*.pipe_mask_ac*/    0x0,
                       /*.vmid_ac*/         0x0
                                        },
                       /*[1]*/          {
                       /*.pipe_mask*/       0x007c0078,
                       /*.vmid*/            0x0,
                       /*.pipe_mask_ac*/    0x0,
                       /*.vmid_ac*/         0x0
                                        },
                       /*[2]*/          {
                       /*.pipe_mask*/       0x00000000,
                       /*.vmid*/            0x0,
                       /*.pipe_mask_ac*/    0x0,
                       /*.vmid_ac*/         0x0
                                         },
                       /*[3]*/          {
                       /*.pipe_mask*/       0x00000000,
                       /*.vmid*/            0x0,
                       /*.pipe_mask_ac*/    0x0,
                       /*.vmid_ac*/         0x0
                                         }
                    },
    /*.sg*/         {{0x0}},
    /*.ctrl_vmid*/  0x0,
    /*.bam_irq_ee*/ 0x0
};

bam_sec_config_type bam_tgt_slimbus_qca_secconfig_670 =
{
    /*.ee*/           {
                         /*[0]*/            {
                         /*.pipe_mask*/         0x00001f87, 
                         /*.vmid*/              0x0, 
                         /*.pipe_mask_ac*/      0x0, 
                         /*.vmid_ac*/           0x0
                                            },
                         /*[1]*/            {
                         /*.pipe_mask*/         0x00000078, 
                         /*.vmid*/              0x0, 
                         /*.pipe_mask_ac*/      0x0, 
                         /*.vmid_ac*/           0x0
                                            },
                         /*[2]*/            {
                         /*.pipe_mask*/         0x00000000, 
                         /*.vmid*/              0x0, 
                         /*.pipe_mask_ac*/      0x0, 
                         /*.vmid_ac*/           0x0
                                            },
                         /*[3]*/            {
                         /*.pipe_mask*/         0x00000000,
                         /*.vmid*/              0x0,
                         /*.pipe_mask_ac*/      0x0,
                         /*.vmid_ac*/           0x0
                                            }
                      },
    /*.sg*/           {{0x0}},
    /*.ctrl_vmid*/    0x0,
    /*.bam_irq_ee*/   0x0
};

#define BAM_CNFG_BITS_VAL 0xFFFFF004

const bam_target_config_type  bam_tgt_config[] = {
    {                    // SLIMBUS_AUD_BAM
     /* .bam_pa     */   0x62D84000,
     /* .options    */   BAM_TGT_CFG_INIT,
     /* .cfg_bits   */   BAM_CNFG_BITS_VAL,   
     /* .ee         */   0,
     /* .sec_config */   &bam_tgt_slimbus_aud_secconfig_670,
     /* .size       */   BAM_MAX_MMAP
    },
    {                    // SLIMBUS_QCA_BAM
     /* .bam_pa     */   0x62E04000, 
     /* .options    */   BAM_TGT_CFG_INIT,
     /* .cfg_bits   */   BAM_CNFG_BITS_VAL,   
     /* .ee         */   0,
     /* .sec_config */   &bam_tgt_slimbus_qca_secconfig_670,
     /* .size       */   BAM_MAX_MMAP
    },
    {                    // Crypto_BAM
     /* .bam_pa     */   0x1DC4000,
     /* .options    */   BAM_TGT_CFG_NO_INIT,
     /* .cfg_bits   */   BAM_CNFG_BITS_VAL,   
     /* .ee         */   1,
     /* .sec_config */   NULL,
     /* .size       */   BAM_MAX_MMAP
    },
    {                    //dummy config
     /* .bam_pa     */   BAM_TGT_CFG_LAST,
     /* .options    */   0,
     /* .cfg_bits   */   0,
     /* .ee         */   0,
     /* .sec_config */   NULL,
     /* .size       */   0
    },
};




