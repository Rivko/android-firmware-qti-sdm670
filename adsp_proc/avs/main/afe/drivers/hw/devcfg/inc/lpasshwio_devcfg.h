/*========================================================================
   This file contains HWD device configuration functions

  Copyright (c) 2015-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc. (QTI) Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/inc/lpasshwio_devcfg.h#1 $
 ====================================================================== */

#ifndef LPASSHWIO_DEVCFG_H
#define LPASSHWIO_DEVCFG_H

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "typedef_drivers.h"
#include "qurt_types.h"
#include "avtimer_hal_devcfg_defs.h"


/*==========================================================================
  Macro definitions
  ========================================================================== */


#define MAX_GENERIC_CLK_ID_NUM 43
#define MAX_PCM_OE_CLK_NUM  11

#define MAX_I2S_TYPE_NUM   14

#define MAX_PCM_TYPE_NUM   5

#define MAX_TDM_TYPE_NUM   5

#define MAX_LPAIF_MUX_NUM    5

#define NUM_MAX_VFR_SRC    2

// Maximum number of SLIMBUS HW blocks supported by driver
#define MAX_SLIMBUS_HW_MODULES              2

/* Number of available combinations of max end points
 * supported by SLIMbus master ports*/
#define NUM_SB_EPS_COMB    2

#define AUDIOIF_DEVCFG_DIR_SINK         0x1
#define AUDIOIF_DEVCFG_DIR_SOURCE       0x2

#define LPASS_HW_VER_3_0_2    (0x30000002) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */
/* LPASS h/w version for Bear 9607 Chipset */
#define LPASS_HW_VER_3_5_0    (0x30050000) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for MDM 9x65(STINGRAY), 9x55 Chipsets */
#define LPASS_HW_VER_3_5_1    (0x30050001) /* BIT: 31:28, Major verion
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for 8994 chipset */
#define LPASS_HW_VER_3_8_0    (0x30080000) /* BIT: 31:28, Major verion
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */
/* LPASS h/w version for Bear+ chipsets */
#define LPASS_HW_VER_3_10_0   (0x300A0000) /* BIT: 31:28, Major verion
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for Honeybadger chipsets */
#define LPASS_HW_VER_4_0_0    (0x40000000) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for 8998 chipsets */
#define LPASS_HW_VER_4_0_2    (0x40000002) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */


/* LPASS h/w version for SDM630 chipsets */
#define LPASS_HW_VER_4_0_3    (0x40000003) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for SDM845 chipsets */
#define LPASS_HW_VER_4_1_0    (0x40010000) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */

/* LPASS h/w version for SDM870 (Warlock) chipsets */
#define LPASS_HW_VER_4_2_0    (0x40020000) /* BIT: 31:28, Major verion
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */


/* LPASS h/w version for Bear MDM(9x45/9x55) */
#define LPASS_HW_VER_3_3_1    (0x30030001)


/* LPASS h/w version for Bear 8909 Chipsets */
#define LPASS_HW_VER_3_3_0    (0x30030000)

/* Get the number of elements in a given array list*/
#define NUM_INTF_PORTS(x) ( sizeof(x)/sizeof(x[0]) )

enum
{
  TIMESTAMP_LATCH_TYPE_SOFTWARE = 0,
  TIMESTAMP_LATCH_TYPE_HARDWARE
};

enum  // audioif, pcm, mux index
{
  PRIMARY_MI2S    =  0,      //primary MI2S
  PRIMARY_PCM     =  0,      //primary PCM
  PRIMARY_TDM     =  0,      //primary TDM
  PRIMARY_MUX     =  0,
  SECONDARY_MI2S  =  1,      //Secondary MI2S
  SECONDARY_PCM   =  1,      //Secondary PCM
  SECONDARY_TDM   =  1,      //Secondary TDM
  SECONDARY_MUX   =  1,
  TERTIARY_MI2S   =  2,      //Tertiary MI2S
  TERTIARY_PCM    =  2,      //Tertiary PCM
  TERTIARY_TDM    =  2,      //Tertiary TDM
  TERTIARY_MUX    =  2,
  QUATERNARY_MI2S =  3,      //Quaternary MI2S
  QUATERNARY_PCM  =  3,      //Quaternary PCM
  QUATERNARY_TDM  =  3,      //Quaternary PCM
  QUATERNARY_MUX  =  3,
  SPEAKER_I2S     =  4,      //Speaker I2S
  QUINARY_MI2S    =  5,      //Quinary MI2S
  QUINARY_PCM     =  4,      //Quinary PCM
  QUINARY_TDM     =  4,      ////Quinary TDM
  QUINARY_MUX     =  4,      //Quinary Mux
  SENARY_MI2S     =  6,       //Senary MI2S
  INT_MI2S_0      =  7,       // INT 0 Mi2s
  INT_MI2S_1      =  8,       // INT 1 Mi2s
  INT_MI2S_2      =  9,       // INT 2 Mi2s
  INT_MI2S_3      =  10,      // INT 3 Mi2s
  INT_MI2S_4      =  11,      // INT 4 Mi2s
  INT_MI2S_5      =  12,      // INT 5 Mi2s
  INT_MI2S_6      =  13       // INT 6 Mi2s
};

typedef enum lpasshwio_irq_type
{
   IRQ_TYPE_UNSUPPORTED = 0,
   IRQ_TYPE_DEFAULT = 1,   /* Interrupt is not gated  */
   IRQ_TYPE_PMU = 2,       /* Interrupt is Power Management 
                              Unit(PMU) gated */
   IRQ_TYPE_PDC = 3,       /* Interrupt is Power Domain 
                              Control (PDC) gated */
   IRQ_TYPE_MAX = 0xFFFFFFFF
}lpasshwio_irq_type_t;

typedef enum lpasshwio_gpio_type
{
   GPIO_TYPE_UNSUPPORTED = 0,
   GPIO_TYPE_DEFAULT = 1,     /* Chip level GPIO, mapped to LPASS
                                 via direct connect lines */
   GPIO_TYPE_LPI = 2,         /* LPASS LPI TLMM GPIO */
   GPIO_TYPE_PDC = 3,         /* Chip level GPIO, mapped to LPASS
                                 via PDC mux */
   GPIO_TYPE_MAX = 0xFFFFFFFF
}lpasshwio_gpio_type_t;

enum // cap for direction
{
  DATA_LINE_SINK       =    1,   //0x1 speaker path only
  DATA_LINE_SOURCE     =    2,   // 0x2 mic path only,
  DATA_LINE_BIDIRECT   =    3,   //  0x3: bidirectional
};

enum
{
  NOT_SUPPORTED    =     0,
  SUPPORTED         =    1,
};


/*==========================================================================
  Structure definitions
  ========================================================================== */

typedef struct
{
   uint32                  hw_ver_reg_base;        /* LPASS hw version reg base addr */
   lpasshwio_gpio_type_t   codec_intr_gpio_type;   /* GPIO type used for codec interrupt */   
   lpasshwio_irq_type_t    codec_intr_type;        /* Codec interrupt type */
   uint32                  gpio_num_cdc_int;       /* Chip_level/LPI GPIO # for cdc intr */
}HwdLpassPropertyType;

typedef struct
{
   uint32 hdmi_reg_base;   // hdmi 1.4 reg base addr
   uint32 hdmi_reg_size;   // hdmi 1.4 reg page size
   uint32 is_supported;
} lpasshwio_prop_hdmi14_struct_t;

typedef struct 
{
  uint32_t intf_idx;    // Interface index number
  uint32_t inf_type_data_line_caps; // maximum # of channel for i2s
  uint32_t inf_type_direction; // 0x1 speaker path only,  0x2 mic path only,   0x3: bidirectional
  uint32_t num_ports;     // num ports for the interface
  uint32_t *port_list;    // List of port IDs
} lpasshwio_intf_port_info_t;

typedef struct
{
  uint32_t i2s_reg_offset_addr;   // i2s reg addr LPASS_LPAIF_I2S_CTLa
  uint32_t int_i2s_reg_offset_addr;   // i2s reg addr LPASS_LPAIF_INT_I2S_CTLa
  uint32_t inf_type_cnt;      // # of i2s interface type count
  lpasshwio_intf_port_info_t *intf_port_info[MAX_I2S_TYPE_NUM];// interface port info ptr
  uint32_t is_supported;
} lpasshwio_prop_i2s_struct_t;

typedef struct
{
   uint32 pcm_reg_offset_addr;   // pcm reg base addr LPASS_LPAIF_PCM_CTLa
   uint32 inf_type_cnt;      // # of pcm interface type count
   uint32 inf_type_list[MAX_PCM_TYPE_NUM];    //
   uint32 inf_type_data_line_caps[MAX_PCM_TYPE_NUM]; // maximum # of channel for pcm
   uint32 inf_type_direction[MAX_PCM_TYPE_NUM];  // 0x1 spacker path only,  0x2 mic path only,   0x3: bidirectional
   uint32 is_supported;
} lpasshwio_prop_pcm_struct_t;


typedef struct
{
   uint32 tdm_reg_offset_addr;   // tdm reg base addr LPASS_LPAIF_PCM_CTLa
   uint32 inf_type_cnt;      // # of tdm interface type count
   uint32 inf_type_list[MAX_TDM_TYPE_NUM];    //
   uint32 inf_type_data_line_caps[MAX_TDM_TYPE_NUM]; // maximum # of channel for tdm
   uint32 inf_type_direction[MAX_TDM_TYPE_NUM];  // 0x1 spacker path only,  0x2 mic path only,   0x3: bidirectional
   uint32 is_supported;
} lpasshwio_prop_tdm_struct_t;


typedef struct
{
   uint32 mux_reg_offset_addr;   // mux reg base addr  LPASS_LPAIF_PCM_I2S_SELa
   uint32 mux_inf_type_cnt;    // # of interfaces that  is shared the output with mux
   uint32 shared_inf_type_list_map[MAX_LPAIF_MUX_NUM];   // 1:1 mapping for i2s  interface
   uint32 is_supported;
} lpasshwio_prop_lpaif_mux_struct_t;


typedef struct
{
   uint32 lpaif_reg_base;   // lpaif reg base addr
   uint32 lpaif_reg_size;    // lpaif reg page size
  lpasshwio_prop_i2s_struct_t i2s_prop;
  lpasshwio_prop_pcm_struct_t pcm_prop;
  lpasshwio_prop_tdm_struct_t tdm_prop;
  lpasshwio_prop_hdmi14_struct_t hdmi_prop;
  lpasshwio_prop_lpaif_mux_struct_t lpaif_mux_prop;
   uint32 hw_revision;
} lpasshwio_prop_lpaif_struct_t;

typedef struct
{
   uint32 dma_reg_base;         // dma reg base addr
   uint32 dma_reg_size;         // dma reg size
   uint32 dma_int_reg_offset_addr;   // dma int reg base addr  LPASS_LPAIF_IRQ_ENa
   uint32 rddma_reg_offset_addr; // rd dma addr LPASS_LPAIF_RDDMA_CTLa
   uint32 wrdma_reg_offset_addr; // wr dma addr LPASS_LPAIF_WRDMA_CTLa
   uint32 stc_rddma_reg_offset_addr;   // stc rddma reg base addr  LPASS_LPAIF_RDDMA_STC_LSBa
   uint32 stc_wrdma_reg_offset_addr;   // stc wrdma reg base addr  LPASS_LPAIF_WRDMA_STC_LSBa
   uint32 wrdma5_reg_offset_addr; // wr dma addr LPASS_LPAIF_WRDMA_CTL5
   uint32 stc_wrdma5_reg_offset_addr; // wr dma addr LPASS_LPAIF_WRDMA_STC_LSB5
   uint32 sink_dma_cnt;     // read DMA channel count
   uint32 source_dma_cnt;   // write DMA channel count
   uint32 int_irq_no;       // intr irq #
   uint32 int_output_index; // interrupt line #
   uint32 hw_revision;      // hw revision
} lpasshwio_prop_afe_dma_struct_t;

typedef struct
{
   uint32 tcsrRegBase;   // tcsr reg base addr
   uint32 tcsrRegSize;   // tcsr reg size
   uint32 hw_revision;
   uint32 clk_mgr_coreid; // LPASS CLK MGR CORE ID: MSMS: MMPM_CORE_ID_LPASS_CORE, Bear Arch: MMPM_CORE_ID_LPASS_AIF
   uint32 supportedClkIdCnt; /* # of supported clock id.This field is non-zero
                               only for targets which uses latest clock APIs. */
   uint32 supportedClkId[MAX_GENERIC_CLK_ID_NUM];    // clock-id supported list.
   uint32 infPcmOeCnt;                               // Number of PCM ClkId support OE.
   uint32 infPcmOeSupportedClkId[MAX_PCM_OE_CLK_NUM]; // List of PCM ClkId that support OE
   uint32 infPcmOeClkIdToUse[MAX_PCM_OE_CLK_NUM];    // List of OE ClkId to use for above PCM ClkId
   uint32 infPcmOeClkFreq[MAX_PCM_OE_CLK_NUM];       // List of OE frequency for above OE ClkId
} HwdGenericClkPropertyType;

typedef struct
{
   uint32 baseRegAddr; // avtimer reg base addr LPASS_LPASS_SYNC_WRAPPER
   uint32 regSize;     // avtimer reg page size
  /* AV timer HW instances information */
  avtimer_instance_info_t avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_MAX];
   uint32 audsync_offset_addr;  // aud sync reg addr LPASS_AUD_SYNC_CTL
   uint32 isrHookPinNum;   // avtimer intr irq #
   uint32 hw_revision;   // avtimer hw revision
} HwdAvtimerPropertyType;

typedef struct
{
   uint32 baseRegAddr;   // hw resampler reg base addr
   uint32 regSize;   // hw resampler reg page size
   uint32 isrHookPinNum;   // hw resampler intr irq #
   uint32 hw_revision;   // resampler hw revision
} HwdResamplerPropertyType;

typedef struct vfr_src_prop
{
   uint32 irq_mux_ctl_sel;    // Mux control for VFR source selection
   uint32 is_supported;       // If this VFR source is hw supported or not
} vfr_src_prop_t;

typedef struct
{
   uint32               reg_base_addr;         // VFR reg base addr  LPASS_VFR_IRQ_MUX_CTL
   uint32               reg_size;              // VFR reg page size
   uint32               vfr_ts_mux_offset1_addr;    // LPASS_VFR_IRQ_MUX_CTL_1
   uint32               gp_ts_mux_offset0_addr;     //  LPASS_GP_IRQ_TS_MUX_CTL_0
   uint32               gp_ts_mux_offset1_addr;     //LPASS_GP_IRQ_TS_MUX_CTL_1
   uint32               hw_revision;         // VFR hw revision
   uint32               hw_latch_ver;        // VFR hw latching version
   uint32               num_vfr_ts_mux;                  // Number of VFR timestamp mux
   uint32               num_gp_ts_mux;                   // Number of General Purpose timestamp mux
   uint32               l2vic_num[NUM_MAX_VFR_SRC];      // Available L2VIC # for VFR interrupts
  vfr_src_prop_t    vfr_src_prop[NUM_MAX_VFR_SRC];
   lpasshwio_irq_type_t vfr_irq_type; /* VFR interrupt type */
}lpasshwio_vfr_prop_t;

typedef struct
{
   uint32 hw_revision; // hw revision, 0-not supported
   uint32 ccu_mb0_ctrl_reg_phy_addr; // malibox register physical address
   uint32 q6ss_ipc_reg_phy_addr; // LPASS IPC register physical address
   uint32 q6ss_ipc_reg_ack_bit_pos; // LPASS IPC interupt register bit position
   uint16 ipc_send_mail_box_offset; // mail box offset from mb0 ctrl register addr
   uint16 internal_bt_tx_int; // BT Mic path interrupt number
   uint16 internal_bt_rx_int; // BT Speaker path interrupt number
   uint16 internal_fm_tx_rx_int; // FM interrupt number
   uint16 internal_bt_fm_ipc_int; // IPC interrupt
   uint16 hw_timestamp_available; //indicates if hardware timestamp block is available
}lpasshwio_prop_riva_struct_t;

typedef struct
{
   uint32 is_multi_endpoint; // Indicates if multi-endpoint support is present in hardware
  /*list of max end points supported by master ports.
   *Max end points should be filled in decreasing order like {4,2} */
   uint32 max_eps_per_master[NUM_SB_EPS_COMB];
}sb_multi_ep_info_t;

typedef struct
{
   uint32 hw_revision; // hw revision, 0-not supported
   uint32 avtimer_latch_type; // Indicates if hardware latching support for AV-timer present
  sb_multi_ep_info_t sb_multi_ep_info[MAX_SLIMBUS_HW_MODULES]; // Contains information related to multi end point
}lpasshwio_prop_slimbus_struct_t;

typedef struct
{
   uint32 base_reg_phy_addr;  // spdiftx reg base addr
   uint32 reg_size;           // spdiftx reg page size
   uint32 reset_reg_phy_addr; // spdiftx reset reg base addr
   uint32 reset_reg_size;     // spdiftx rest reg page size;
   uint32 int_irq_no;         // spdiftx intr irq #
   uint32 num_hw_buf;         // num of hw buffer
   uint32 hw_revision;        // spdiftx hw revision
} lpasshwio_prop_spdif_struct_t;

typedef struct
{
   uint32 lpass_reg_base_addr;          // lpass hdmi output reg base addr
   uint32 lpass_reg_size;               // lpass hdmi output reg page size
   uint32 lpass_reset_reg_base_addr;    // lpass hdmi output reset reg base addr
   uint32 lpass_reset_reg_size;         // lpass hdmi output reset reg page size
   uint32 hdmi_core_reg_base_addr;      // hdmi core output reg base addr
   uint32 hdmi_core_reg_size;           // hdmi core output reg page size
   uint32 hw_revision;                  // hdmi output hw revision
} lpasshwio_prop_hdmi_output_struct_t;

/** @brief LPM Properties Structure
 */

typedef struct  {
  uint64 base_phy_addr;
  /**< Starting physical address, 64-bit */

   uint32 size;
  /**< Size of LPM in bytes */

   uint32 size_afe_dma_buf;
  /**< Size allocated for AFE DMA buffers, in bytes */

   uint32 size_afe_working_buf;
  /**< Size allocated for AFE working buffer, in bytes */

   uint32 cache_attribute;
  /**< Cache attribute defined in qurt_types.h
   */

   uint32 ahb_voting_scale_factor;
  /**< scale factor for AHB bus clock voting to access LPM
   */

} lpm_prop_struct_t;

#endif // #ifndef LPASSHWIODEVCFG_H
