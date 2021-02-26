/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/config/lpasshwio_devcfg_845.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE: lpasshwio_devcfg_845.c

DESCRIPTION: SDM845 Device Config

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
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
   0x17080000,       /**< LPASS register base address */
   GPIO_TYPE_PDC,    /**< Codec Intr GPIO type: PDC */ 
   IRQ_TYPE_PDC,     /**< Codec Intr type: PDC */
   53                /**< GPIO# for codec interrupt */
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

/**< LPAIF Configuration */
lpasshwio_prop_lpaif_struct_t lpaif_prop =
{
    0x17100000,      /**< LPAIF reg base addr: LPASS_LPAIF_VERSION */
    (128 * 1024),    /**< LPAIF overal reg page size */

    /**< I2S Device Config Start */
    {
        0x17101000,   /**< LPASS_LPAIF_I2S_CTL0, offset addr */
        0,            /**< internal i2s reg offset : LPASS_LPAIF_INT_I2S_CTL0*/
        5,            /**< Number of supported mi2s interfaces */
        {
            /**< List of supported mi2s interfaces */
            &pri_mi2s_port_info,
            &sec_mi2s_port_info,
            &ter_mi2s_port_info,
            &quat_mi2s_port_info,
            &speaker_mi2s_port_info
        },
        SUPPORTED   /**< Mi2s Interface is supported */
    },  /**< End of I2S Device Config */


    /**< PCM Device Config Start */
    {
        0x17101500,  /**< LPASS_LPAIF_PCM_CTL0, offset addr */
        4,           /**< Number of supported PCM interfaces */
        {
            /**< List of supported PCM interfaces */
            PRIMARY_PCM,
            SECONDARY_PCM,
            TERTIARY_PCM,
            QUATERNARY_PCM
        },
        {
            /**< Max # of channel for each PCM intf */
            4,  /**< Pri PCM */
            4,  /**< Sec PCM */
            4,  /**< Ter PCM */
            4   /**< Quat PCM */
        },
        {
            /**< Direction for each pcm interface */
            DATA_LINE_BIDIRECT, /**< Pri PCM, bi-dir */
            DATA_LINE_BIDIRECT, /**< Sec PCM, bi-dir */
            DATA_LINE_BIDIRECT, /**< Ter PCM, bi-dir */
            DATA_LINE_BIDIRECT  /**< Quat PCM, bi-dir */
        },
        SUPPORTED   /**< PCM Interface is supported */
    }, /**< PCM Device Config end */

    /**< TDM Device Config Start */
    {
        0x17101500, /**< LPASS_LPAIF_PCM_CTL0, offset addr, same as PCM interface */
        4,          /**< Number of supported TDM interfaces */
        {
            /**< List of supported TDM interfaces */
            PRIMARY_TDM,
            SECONDARY_TDM,
            TERTIARY_TDM,
            QUATERNARY_TDM
        },
        {
            /**< Max # of channel for each TDM intf */
            8,  /**< Pri TDM */
            8,  /**< Sec TDM */
            8,  /**< Ter TDM */
            8   /**< Quat TDM */
        },
        {
            /**< Direction for each TDM interface */
            DATA_LINE_BIDIRECT, /**< Pri TDM, bi-dir */
         DATA_LINE_BIDIRECT, /**< Sec TDM, bi-dir */
         DATA_LINE_BIDIRECT, /**< Ter TDM, bi-dir */
         DATA_LINE_BIDIRECT  /**< Qua TDM, bi-dir */
        },
        SUPPORTED    /**< TDM Interface is supported */
    }, /**< TDM Device Config end */

    /**< HDMI 1.4 Device Config */
    {
        0xAEA0000,    /**< MMSS_HDMI_CTRL */
        (4 * 1024),   /**< Reg page size */
        SUPPORTED     /**< HDMI1.4 is supported */
    },

    /**< I2S/PCM selection mux */
    {
        0x1711A000,  /**< LPASS_LPAIF_PCM_I2S_SEL0, offset addr */
        4,           /**< # of interfaces that  is shared the output with mux */
        {
            /**< 1:1 mapping between mi2s and pcm interfaces */
            PRIMARY_MUX,
            SECONDARY_MUX,
            TERTIARY_MUX,
            QUATERNARY_MUX
        },
        SUPPORTED   /**< Selection mux is supported */
    },
    LPASS_HW_VER_4_1_0,  /**< H/w version */
};

/**< AudioIF DMA Configuration */
lpasshwio_prop_afe_dma_struct_t audioif_dma_prop =
{
    0x17100000,              /**< LPAIF reg base addr: LPASS_LPAIF_VERSION */
    (128 * 1024),            /**< LPAIF reg page size */
    0x1710A000,              /**< DMA offset: LPASS_LPAIF_IRQ_EN0 : interrupt */
    0x1710D000,              /**< DMA offset: LPASS_LPAIF_RDDMA_CTLa */
    0x17113000,              /**< DMA offset: LPASS_LPAIF_WRDMA_CTLa */
    0x1710D028,              /**< STC rddma offset: LPASS_LPAIF_RDDMA_STC_LSB0 */
    0x17113028,              /**< STC wrdma offset: LPASS_LPAIF_WRDMA_STC_LSB0 */
    0,                       /**< Not required:  LPASS_LPAIF_WRDMA_CTL5 */
    0,                       /**< Not required:  LPASS_LPAIF_WRDMA_STC_LSB5 */
    5,                       /**< Read/sink DMA channel count */
    4,                       /**< Write/source DMA channel count */
    19,                      /**< DMA intr L2VIC # */
    2,                       /**< interrupt line # */
    LPASS_HW_VER_4_1_0       /**< LPASS hw revision */
};

/**< HDMI Out DMA Configuration */
lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop =
{
    0x170F8000,             /**< HDMI output reg base addr LPASS_HDMITX_VERSION */
    0x8000,                 /**< HDMI output reg page size */
    0,                      /**< Not requried */
    0,                      /**< Not required */
    0,                      /**< Not requried */
    0,                      /**< Not required */
    0,                      /**< Not required */
    0,                      /**< Not required  LPASS_LPAIF_WRDMA_CTL5 */
    0,                      /**< Not required  LPASS_LPAIF_WRDMA_STC_LSB5 */
    4,                      /**< Read/sink DMA channel count */
    0,                      /**< Write/source DMA channel count */
    94,                     /**< l2vic id for HDMI DMA interrupt */
    0,                      /**< There is no interrupt host */
    LPASS_HW_VER_4_1_0      /**< LPASS hw revision */
};

/**< AV TImer + Aud Sync Configuration */
HwdAvtimerPropertyType avtimer_prop =
{
    0x170F7000,    /**< Reg base address: LPASS_AVTIMER_HW_VERSION */
    0x1000,        /**< Register page size */

    /* AV timer Hw instances information */
    {
        /* AVTIMER_HW_INSTANCE_INDEX_0 */
        {
            0x170F7000,              /**< AVT reg base addr: LPASS_AVTIMER_HW_VERSION>*/
            AVTIMER_HW_MODE_QCOUNTER /**< HW mode >*/
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

    0,             /**< AudSync reg base addr: LPASS_AUD_SYNC_CTL */
    50,            /**< AV Timer intr L2VIC # */
    0x30040000     /**< AV Timer h/w version */
};

/**< HW Resampler Configuration */
HwdResamplerPropertyType resampler_prop =
{
    0x17138000,    /**< Reg base address: LPASS_RESAMPLER_HW_VERSION */
    0x8000,        /**< Hw resampler reg page size */
    53,            /**< HW resampler intr irq # */
    0x40000000     /**< Resampler hw version */
};

/**< VFR + Timestamp Configuration */
lpasshwio_vfr_prop_t vfr_prop =
{
    0x1708F000,      /**< VFR reg base address: LPASS_VFR_IRQ_MUX_CTL */
    0x9000,          /**< VFR register page size */
    0x17092000,      /**< LPASS_VFR_IRQ_MUX_CTL_1 */
    0x17094000,      /**< LPASS_GP_IRQ_TS_MUX_CTL_0 */
    0x17095000,      /**< LPASS_GP_IRQ_TS_MUX_CTL_1 */
    0x10000000,      /**< VFR hw revision */
    0x1,             /**< VFR hw latching version */
    0x2,             /**< Number of VFR timestamp mux */
    2,               /**< Number of General Purpose timestamp mux */
    {
        /**<  L2VIC # for each VFR interrupt */
        51,          /**< VFR_0 intr L2VIC # */
        124          /**< VFR_1 intr L2VIC # */
    },
    {
        /** Mux input index for each VFR source */
        { 0x0, 0x1 },     /**<  vfr_src_prop[0]: {mux_ctl_in, hw_supported} */
        { 0x7, 0x1 }      /**<  vfr_src_prop[1]: {mux_ctl_in, hw_supported} */
   },
   IRQ_TYPE_DEFAULT  /**< VFR Interrupt type */
};

lpasshwio_prop_slimbus_struct_t afe_slimbus_prop =
{
    /* hw revision, 0-not supported */
    1,
    /* Indicates if hw latching support for AV-timer present */
    1,

    {
        /* SLIMbus module device id 0*/
        {
            /* indicates if multi-endpoint is supported in hardware */
            1,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
         { 4, 2 }
        },

        /* SLIMbus module device id 1*/
        {
            /* indicates if multi-endpoint is supported in hardware */
            1,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
         { 2, 1 }
        }
    }
};

/**< HDMI Out Configuration */
lpasshwio_prop_hdmi_output_struct_t hdmi_output_prop =
{
    0x170F8000,          /**< HDMI output reg base addr: LPASS_HDMITX_VERSION */
    0x8000,              /**< HDMI output reg page size */
    0x170C8000,          /**< HDMI output reset reg base addr: LPASS_HDMITX_CTL */
    0x1000,              /**< HDMI output reset reg page size */
    0xAEA0000,           /**< HDMI core output reg base addr: MMSS_HDMI_CTRL */
    0x1000,              /**< HDMI core output reg page size */
    LPASS_HW_VER_4_1_0   /**< HDMI output hw version */
};

/* LPM configuration */
lpm_prop_struct_t lpm_prop =
{
    0x17120000,                  /**< Starting physical address, 64-bit */
    64 * 1024,                   /**< Size of LPM in bytes */
    64 * 1024,                   /**< Size allocated for AFE DMA buffers, in bytes */
    0,                           /**< Size allocated for AFE working buffer, in bytes */
    QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE,
    /**< Cache attribute: L1 WB cached L1 and L2 un-cached */
    2                            /**< AHB bus vote scaling factor to reach LPM.
                                     1: If uncached or cached with writethrough (Unidirectional access)
                                     2: If cached and writeback (Duplex access) */
};

HwdGenericClkPropertyType genericclk_prop =
{
    0x17080000,              /**< TCSR reg base address */
    (120 * 1024),            /**< TCSR reg size in bytes */
    LPASS_HW_VER_4_1_0,      /**< HW revision */
    MMPM_CORE_ID_LPASS_CORE, /**< clk_mgr_coreid */
    23,                      /**< Number of clk-id's defined below */

    /**< List of clock ids */
    {
        /**< Mi2s interface bit/osr clocks */
        0x100, 0x101,  /**< Pri mi2s IBIT / EBIT clock */
        0x102, 0x103,  /**< Sec mi2s IBIT / EBIT clock */
        0x104, 0x105,  /**< Ter mi2s IBIT / EBIT clock */
        0x106, 0x107,  /**< Qua mi2s IBIT / EBIT clock */

        0x108, 0x109, 0x10A, /**< Spkr i2s IBIT / EBIT / OSR clock */

        /**< PCM interface bit clocks */
        0x200, 0x201,  /**< Pri PCM IBIT / EBIT clock  */
        0x202, 0x203,  /**< Sec PCM IBIT / EBIT clock  */
        0x204, 0x205,  /**< Ter PCM IBIT / EBIT clock  */
        0x206, 0x207,  /**< Quat PCM IBIT / EBIT clock */

        /**< mclk / dig codec clock */
        0x300,   /**< mclk1 */
        0x301,   /**< mclk2 */
        0x302,   /**< mclk3 */

        /**< PCM OE clk-id (internal definition) */
        0x20000
    },

    8, /**< Number of PCM clock defined with OE set availability  */

    /**< List of PCM Clk Id that supported OE clk  */
    { 0x200, 0x201, 0x202, 0x203,
        0x204, 0x205, 0x206, 0x207 },

        /**< Clk Id to use to enable individual PCM OE  */
        { 0x20000,  0x20000,  0x20000,  0x20000,
            0x20000,  0x20000,  0x20000,  0x20000 },

            /**< Frequency (Hz) of PCM OE clock */
            { 61440000, 61440000, 61440000, 61440000,
                61440000, 61440000, 61440000, 61440000 }
};

/**< BT-FM (WCNSS Interface) Configuration */
lpasshwio_prop_riva_struct_t afe_riva_prop = { 0, 0, 0, 0, 0, 0, 0, 0 };

/**< SPDIF Configuration */
lpasshwio_prop_spdif_struct_t spdiftx_prop = { 0, 0, 0, 0, 0, 0, 0 };
