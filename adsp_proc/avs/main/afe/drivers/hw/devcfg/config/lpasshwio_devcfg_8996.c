/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/config/lpasshwio_devcfg_8996.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE: lpasshwio_devcfg_8996.c

DESCRIPTION: 8996 Device Config

 Copyright (c) 2014-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "lpasshwio_devcfg.h"
#include "mmpm.h"
#include "adsp_afe_service_commands.h"

/**< LPASS HW Properties */
HwdLpassPropertyType lpass_prop =
{
   0x09080000,        /**< LPASS register base address */
   GPIO_TYPE_DEFAULT, /**< Codec Intr GPIO type */
   IRQ_TYPE_DEFAULT,  /**< Codec Intr type */
   0,                 /**< GPIO# for codec interrupt
                           Don't care for DEFAULT interrupt
                           type. */
};

// List of primary mi2s ports
static uint32 pri_mi2s_ports[]     =
{
    AFE_PORT_ID_PRIMARY_MI2S_RX,
    AFE_PORT_ID_PRIMARY_MI2S_RX_1,
    AFE_PORT_ID_PRIMARY_MI2S_RX_2,
    AFE_PORT_ID_PRIMARY_MI2S_RX_3,
    AFE_PORT_ID_PRIMARY_MI2S_TX,
    AFE_PORT_ID_PRIMARY_MI2S_TX_1,
    AFE_PORT_ID_PRIMARY_MI2S_TX_2,
    AFE_PORT_ID_PRIMARY_MI2S_TX_3
};

// List of secondary mi2s ports
static uint32 sec_mi2s_ports[]     =
{
    AFE_PORT_ID_SECONDARY_MI2S_RX,
    AFE_PORT_ID_SECONDARY_MI2S_TX
};

// List of tertiary mi2s ports
static uint32 ter_mi2s_ports[]    =
{
    AFE_PORT_ID_TERTIARY_MI2S_RX,
    AFE_PORT_ID_TERTIARY_MI2S_TX
};
// List of Quaternary mi2s ports
static uint32 quat_mi2s_ports[]    =
{
    AFE_PORT_ID_QUATERNARY_MI2S_RX,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_1,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_2,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_3,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_4,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_5,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_6,
    AFE_PORT_ID_QUATERNARY_MI2S_RX_7,
    AFE_PORT_ID_QUATERNARY_MI2S_TX,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_1,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_2,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_3,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_4,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_5,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_6,
    AFE_PORT_ID_QUATERNARY_MI2S_TX_7
};
// List of speaker mi2s ports
static uint32 speaker_mi2s_ports[] = { AUDIO_PORT_ID_I2S_RX };

// Create port info containers
static  lpasshwio_intf_port_info_t pri_mi2s_port_info     =
{
    PRIMARY_MI2S, /* interface index */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(pri_mi2s_ports), /* no of ports available */
    pri_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t sec_mi2s_port_info     =
{
    SECONDARY_MI2S, /* interface index */
    4,              /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(sec_mi2s_ports), /* no of ports available */
    sec_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t ter_mi2s_port_info     =
{
    TERTIARY_MI2S, /* interface index */
    4,             /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(ter_mi2s_ports), /* no of ports available */
    ter_mi2s_ports  /* port list */
};

static  lpasshwio_intf_port_info_t quat_mi2s_port_info    =
{
    QUATERNARY_MI2S, /* interface index */
    8,               /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(quat_mi2s_ports), /* no of ports available */
    quat_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t speaker_mi2s_port_info =
{
    SPEAKER_I2S,   /* interface index */
    2,             /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(speaker_mi2s_ports), /* no of ports available */
    speaker_mi2s_ports /* port list */
};

lpasshwio_prop_lpaif_struct_t lpaif_prop = {
    0x09100000,  // lpaif reg base addr LPASS_LPAIF_VERSION
    (128*1024),  // lpaif reg page size
    {0x09101000, //i2s reg offset : LPASS_LPAIF_I2S_CTL0
        0,           // Not required :internal i2s reg offset : LPASS_LPAIF_INT_I2S_CTL0
        5,           // inf_type_cnt ( The total number below, how many interface, max is 5 )
        { /*Interface info*/
            &pri_mi2s_port_info,
            &sec_mi2s_port_info,
            &ter_mi2s_port_info,
            &quat_mi2s_port_info,
            &speaker_mi2s_port_info
        },
        SUPPORTED},  // i2s hw revision}
        {0x09101500, //pcm reg offset LPASS_LPAIF_PCM_CTL0
            4,           // inf_type_cnt ( The total number below, how many interface, max is 5 )
            {PRIMARY_PCM,SECONDARY_PCM,TERTIARY_PCM,QUATERNARY_PCM},
            {4,4,4,4}, // maximum # of channel for audio if.
            {DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT},
            SUPPORTED},  // pcm hw revision}

            {0x09101500, //tdm uses the same pcm reg offset LPASS_LPAIF_PCM_CTL0
                4,          // inf_type_cnt ( The total number below, how many interface, max is 5 )
                {PRIMARY_TDM,SECONDARY_TDM,TERTIARY_TDM,QUATERNARY_TDM},
                {8,8,8,8}, // maximum # of channel for audio if.
                {DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT},
                SUPPORTED}, // tdm hw revision}

                {0x009A0000, //hdmiAudHwRegBase
                    (4*1024),  // hdmiAudHwRegSize
                    SUPPORTED},
                    {0x0911A000, // mux reg base LPASS_LPAIF_PCM_I2S_SEL0
                        4,           // # of interfaces that  is shared the output with mux
                        {PRIMARY_MUX,SECONDARY_MUX,TERTIARY_MUX,QUATERNARY_MUX},   // 1:1 mapping for i2s and pcm  interface
                        SUPPORTED},
                        0x40000000,  // hw revision
};


lpasshwio_prop_afe_dma_struct_t audioif_dma_prop = {
    0x09100000,  // lpaif reg base addr
    (128*1024),  // lpaif reg page size
    0x0910A000,  // dma offset LPASS_LPAIF_IRQ_EN0 : interrupt
    0x0910D000,  // dma offset LPASS_LPAIF_RDDMA_CTLa
    0x09113000,  // dma offset LPASS_LPAIF_WRDMA_CTLa
    0x0910D028,  // stc rddma offset   LPASS_LPAIF_RDDMA_STC_LSB0
    0x09113028,  // stc wrdma offset LPASS_LPAIF_WRDMA_STC_LSB0
    0,    // not required  LPASS_LPAIF_WRDMA_CTL5
    0,    //not required  LPASS_LPAIF_WRDMA_STC_LSB5
    5,   // sink DMA channel count
    4,   // source DMA channel count
    19,  // lpaif intr irq #
    2,   // interrupt line #
    0x40000000   //  i2s hw revision
};

lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop = {
    0x090F8000,   // hdmi output reg base addr
    0x8000,       // hdmi output reg page size
    0,            // not requried
    0,            // not required
    0,            // not requried
    0,            // not required
    0,            // not required
    0,    // not required  LPASS_LPAIF_WRDMA_CTL5
    0,    //not required  LPASS_LPAIF_WRDMA_STC_LSB5
    4,   // sink DMA channel count
    0,   // source DMA channel count
    94,  // l2vic id for hdmi interrupt
    0,   // there is no interrupt host
    0x40000000   //  hdmiout hw revision
};


/**< AV TImer + Aud Sync Configuration */
HwdAvtimerPropertyType avtimer_prop =
{
    0x090F7000,   /**< Reg base address: LPASS_LPASS_SYNC_WRAPPER */
    0x1000,       /**< Register page size */

    /* AV timer Hw instances information */
    {
        /* AVTIMER_HW_INSTANCE_INDEX_0 */
        {
            0x090F7000,              /**< AVT reg base addr: LPASS_AVTIMER_HW_VERSION>*/
            AVTIMER_HW_MODE_L_MN     /**< HW mode >*/
        },

        /* AVTIMER_HW_INSTANCE_INDEX_1*/
        {
            0,                       /**< ATIME1 reg base addr> */
            AVTIMER_HW_MODE_INVALID  /**< HW mode >*/
        },

        /* AVTIMER_HW_INSTANCE_INDEX_2*/
        {
            0,                       /**< ATIME2 reg base addr> */
            AVTIMER_HW_MODE_INVALID  /**< HW mode >*/
        }
    },

    0,            /**< AudSync reg base addr: LPASS_AUD_SYNC_CTL */
    50,           /**< AV Timer intr L2VIC # */
    0x30040000    /**< AV Timer h/w version */
   };

HwdResamplerPropertyType resampler_prop = {
    0x09138000,   // baseRegAddr: base address of resampler
    0x8000,       // regSize: hw resampler reg page size
    53,           // isrHookPinNum: hw resampler intr irq #
    0x30000000    // hw_revision: resampler hw version
  };

lpasshwio_vfr_prop_t vfr_prop = {
    0x0908F000,      // VFR reg base address LPASS_VFR_IRQ_MUX_CTL
    0x9000,          // VFR register page size
    0x09092000,      // LPASS_VFR_IRQ_MUX_CTL_1
    0x09094000,      // LPASS_GP_IRQ_TS_MUX_CTL_0
    0x09095000,      // LPASS_GP_IRQ_TS_MUX_CTL_1
    0x10000000,      // VFR hw revision
    0x1,             // VFR hw latching version
    0x2,             // Number of VFR timestamp mux
    2,               // Number of General Purpose timestamp mux
    {51, 124}, // l2vic_num[]: Available L2VIC # for VFR interrupts
    {{0x0, 0x1},     // vfr_src_prop[0]: Mux ctrl for VFR src sel, VFR src is hw-supported or not
  {0x7, 0x1}},      // vfr_src_prop[1],
  IRQ_TYPE_DEFAULT  // VFR Interrupt type
};

lpasshwio_prop_riva_struct_t afe_riva_prop = {0, 0, 0, 0, 0, 0, 0, 0};

lpasshwio_prop_slimbus_struct_t afe_slimbus_prop =
{
    /* hw revision, 0-not supported */
    1,
    /* Indicates if hw latching support for AV-timer present */
    1,

    {
        /* SLIMbus moudle device id 0*/
        {
            /* indicates if multi-endpoint is supported in hardware */
            0,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
            {0, 0}
        },

        /* SLIMbus module device id 1*/
        {
            /* indicates if multi-endpoint is supported in hardware */
            0,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
            {0,0}
        }
    }
};

lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};

lpasshwio_prop_hdmi_output_struct_t hdmi_output_prop = {
    0x090F8000,   // hdmi output reg base addr
    0x8000,       // hdmi output reg page size
    0x090C8000,   // hdmi output reset reg base addr
    0x1000,       // hdmi output reset reg page size
    0x009A0000,   // hdmi core output reg base addr
    0x1000,       // hdmi core output reg page size
    0x40000000    // hdmi output hw version
};

/* LPM configuration for MSM8996 */
lpm_prop_struct_t lpm_prop = {
    0x9120000, /**< Starting physical address, 64-bit */
    64*1024,   /**< Size of LPM in bytes */
    64*1024,   /**< Size allocated for AFE DMA buffers, in bytes */
    0,         /**< Size allocated for AFE working buffer, in bytes */
    QURT_MEM_CACHE_NONE_SHARED,  /**< Cache attribute: Uncached L1 and L2 */
    1 /**< AHB bus vote scaling factor to reach LPM. 1 - if uncached or cached with writethrough (Unidirectional access)
                                                                               2 - if cached and writeback (Duplex access) */
                              };


HwdGenericClkPropertyType genericclk_prop =
{
    0x9080000,     // base address of tcsr reg
    (120*1024),     // tcsr reg size
    0x40000000,    // hw revision
    MMPM_CORE_ID_LPASS_CORE, /**< clk_mgr_coreid */
    23,            // Number of clk-id defined below
    // List of clock id, correpsonds to AFE_CLOCK_SET_CLOCK_ID_XX
    // 0x100: Pimary MI2S IBIT; 0x101: Primary MI2S EBIT, etc.
    {
        0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10A,
        0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207,
        0x300, 0x301, 0x302,
        // This is the PCM OE clk-id (internal definition)
        0x20000
    },
    8, // Number of PCM clock defined with OE set availability
    // List of PCM Clk Id that supported OE clk
    {   0x200,      0x201,    0x202,    0x203,    0x204,    0x205,    0x206,    0x207},
    // Clk Id to use to enable individual PCM OE
    {   0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000},
    // Frequency of PCM OE clock
    {  61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000}
};

