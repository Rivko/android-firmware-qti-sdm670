/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/config/lpasshwio_devcfg_8994.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE: lpasshwio_devcfg_8994.c

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
   0xFE04F000,        /**< LPASS register base address */
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
    0xFE0C0000,  // lpaif reg base addr LPASS_LPAIF_VERSION
    (128*1024),  // lpaif reg page size
    {0xFE0C4000, //LPASS_LPAIF_I2S_CTL0
        0,           // Not required :internal i2s reg offset : LPASS_LPAIF_INT_I2S_CTL0
        5,           // inf_type_cnt ( The total number below, how many interface, max is 5 )
        { /* Interface info */
            &pri_mi2s_port_info,
            &sec_mi2s_port_info,
            &ter_mi2s_port_info,
            &quat_mi2s_port_info,
            &speaker_mi2s_port_info
        },
        SUPPORTED},  // i2s hw revision}
        {0xFE0C0010, //LPASS_LPAIF_PCM_CTL0
            2,           // inf_type_cnt ( The total number below, how many interface, max is 5 )
            {PRIMARY_PCM,SECONDARY_PCM, 0, 0},
            {4,4,0,0}, // maximum # of channel for audio if.
            {DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT, 0, 0},
            SUPPORTED},  // pcm hw revision}

            {0,           //LPASS_LPAIF_PCM_CTL0
                0,           // inf_type_cnt ( The total number below, how many interface, max is 5 )
                {0, 0, 0, 0},
                {0, 0, 0, 0}, // maximum # of channel for audio if.
                {0, 0, 0, 0},
                NOT_SUPPORTED}, // tdm hw revision}

                {0xFD9A8000, //hdmiAudHwRegBase
                    (4*1024),  // hdmiAudHwRegSize
                    SUPPORTED},
                    {0,            // mux reg base LPASS_LPAIF_PCM_I2S_SEL0
                        0,            // # of interfaces that  is shared the output with mux
                        {0,0,0,0},    // 1:1 mapping for i2s and pcm  interface
                        NOT_SUPPORTED},
                        0x30080000,  // hw revision
};


lpasshwio_prop_afe_dma_struct_t audioif_dma_prop = {
    0xFE0C0000, //  lpaif reg base addr
    (64*1024),  //  lpaif reg page size
    0xFE0CF000,  // dma offset LPASS_LPAIF_IRQ_EN0 : interrupt
    0xFE0D2000,  // dma offset LPASS_LPAIF_RDDMA_CTLa
    0xFE0D8000,  // dma offset LPASS_LPAIF_WRDMA_CTLa
    0xFE0D2028,  // stc rddma offset   LPASS_LPAIF_RDDMA_STC_LSB0
    0xFE0D8028,  // stc wrdma offset LPASS_LPAIF_WRDMA_STC_LSB0
    0,    // not required  LPASS_LPAIF_WRDMA_CTL5
    0,    //not required  LPASS_LPAIF_WRDMA_STC_LSB5
    5,   // sink DMA channel count
    4,   // source DMA channel count
    19,  // lpaif intr irq #
    2,   // interrupt line #
    0x30080000   //  i2s hw revision
};

lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop = {
    0,      // hdmi output reg base addr
    0,     // hdmi output reg page size
    0,      // not requried
    0,      // not required
    0,     // not requried
    0,     // not required
    0,      // not required
    0,    // not required  LPASS_LPAIF_WRDMA_CTL5
    0,    //not required  LPASS_LPAIF_WRDMA_STC_LSB5
    0,   // sink DMA channel count
    0,   // source DMA channel count
    0,  // l2vic id for hdmi interrupt
    0,   // there is no interrupt host
    0   //  hdmiout hw revision
};

/**< AV TImer + Aud Sync Configuration */
HwdAvtimerPropertyType avtimer_prop =
{
    0xFE09C000,   /**< Reg base address: LPASS_LPASS_SYNC_WRAPPER */
    0x1000,       /**< Register page size */

    /* AV timer Hw instances information */
    {
        /* AVTIMER_HW_INSTANCE_INDEX_0 */
        {
            0xFE09C000,               /**< AVT reg base addr: LPASS_AVTIMER_HW_VERSION>*/
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
    0xFE0B0000,   // baseRegAddr: base address of resampler
    0x4000,       // regSize: hw resampler reg page size
    53,           // isrHookPinNum: hw resampler intr irq #
    0x20000000    // hw_revision: resampler hw version
           };

lpasshwio_vfr_prop_t vfr_prop = {
    0xFE03C000,     // VFR reg base address
    0x9000,         // VFR register page size
    0xFE040000,      // LPASS_VFR_IRQ_MUX_CTL_1
    0xFE042000,      // LPASS_GP_IRQ_TS_MUX_CTL_0
    0xFE043000,      // LPASS_GP_IRQ_TS_MUX_CTL_1
    0x10000000,     // VFR hw revision
    0x1,            // VFR hw latching version
    0x2,            // Number of VFR timestamp mux
    2,             // Number of General Purpose timestamp mux
    {51, 124}, // l2vic_num[]: Available L2VIC # for VFR interrupts
    {{0x0, 0x1},   // vfr_src_prop[0]: Mux ctrl for VFR src sel, VFR src is hw-supported or not
        {0x7, 0x1}},       // vfr_src_prop[1],
        IRQ_TYPE_DEFAULT   // VFR Interrupt type
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
    0x0,  // hdmi output reg base addr
    0x0,  // hdmi output reg page size
    0x0,  // hdmi output reset reg base addr
    0x0,  // hdmi output reset reg page size
    0x0,   // hdmi core output reg base addr
    0x0,   // hdmi core output reg page size
    0x0    // hdmi output hw version
};

/* LPM configuration for MSM8994 */
lpm_prop_struct_t lpm_prop = {
    0xFE0E0000, /**< Starting physical address, 64-bit */
    64*1024,    /**< Size of LPM in bytes */
    64*1024,    /**< Size allocated for AFE DMA buffers, in bytes */
    0,          /**< Size allocated for AFE working buffer, in bytes */
    QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE,  /**< Cache attribute: L1 writeback L2 uncached */
    2 /**< AHB bus vote scaling factor to reach LPM. 1 - if uncached or cached with writethrough (Unidirectional access)
                                                                               2 - if cached and writeback (Duplex access) */
};

HwdGenericClkPropertyType genericclk_prop = {0x30080000, 0, 0, 0, 0, {0}, 0, {0}, {0}, {0} };
