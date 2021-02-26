/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/config/lpasshwio_devcfg_8953.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE: lpasshwio_devcfg_8953.c

DESCRIPTION: 8953 Device Config

 Copyright (c) 2015-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
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
   0x0C050000,        /**< LPASS register base address */
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
    AFE_PORT_ID_PRIMARY_MI2S_RX_3
};

// List of secondary mi2s ports
static uint32 sec_mi2s_ports[]     = { AFE_PORT_ID_SECONDARY_MI2S_RX };

// List of tertiary mi2s ports
static uint32 ter_mi2s_ports[]     = { AFE_PORT_ID_TERTIARY_MI2S_TX };
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

// List of Quinary mi2s ports
static uint32 quin_mi2s_ports[]    =
{
    AFE_PORT_ID_QUINARY_MI2S_RX,
    AFE_PORT_ID_QUINARY_MI2S_TX
};

// List of Senary mi2s ports
static uint32 sen_mi2s_ports[]     = { AFE_PORT_ID_SENARY_MI2S_TX };

// Create port info containers
static  lpasshwio_intf_port_info_t pri_mi2s_port_info     =
{
    PRIMARY_MI2S, /* interface index */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(pri_mi2s_ports), /* no of ports available */
    pri_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t sec_mi2s_port_info     =
{
    SECONDARY_MI2S, /* interface index */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(sec_mi2s_ports), /* no of ports available */
    sec_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t ter_mi2s_port_info     =
{
    TERTIARY_MI2S, /* interface index */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(ter_mi2s_ports), /* no of ports available */
    ter_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t quat_mi2s_port_info    =
{
    QUATERNARY_MI2S, /* interface index */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(quat_mi2s_ports), /* no of ports available */
    quat_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t quin_mi2s_port_info    =
{
    QUINARY_MI2S, /* interface index */
    8,            /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(quin_mi2s_ports), /* no of ports available */
    quin_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t sen_mi2s_port_info    =
{
    SENARY_MI2S, /* interface index */
    2,            /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(sen_mi2s_ports), /* no of ports available */
    sen_mi2s_ports     /* port list */
};

/**< LPAIF Configuration */
lpasshwio_prop_lpaif_struct_t lpaif_prop =
{
    0xC0C0000,    /**< LPAIF reg base addr: LPASS_LPAIF_VERSION */
    (128*1024),   /**< LPAIF overal reg page size */

    /**< I2S Device Config Start */
    {
        0xC0C4000,   /**< LPASS_LPAIF_I2S_CTL0, offset addr */
        0,           /**< Not required :internal i2s reg offset : LPASS_LPAIF_INT_I2S_CTL0, */
        6,           /**< Number of supported mi2s interfaces */
        {
            /**< Info for each mi2s */
            &pri_mi2s_port_info,
            &sec_mi2s_port_info,
            &ter_mi2s_port_info,
            &quat_mi2s_port_info,
            &quin_mi2s_port_info,
            &sen_mi2s_port_info
        },
        SUPPORTED   /**< Mi2s Interface is supported */
    },  /**< End of I2S Device Config */

    /**< PCM Device Config Start */
    {
        0xC0C0010,  /**< LPASS_LPAIF_PCM_CTL0, offset addr */
        2,          /**< Number of supported PCM interfaces */
        {
            /**< List of supported PCM interfaces */
            PRIMARY_PCM,
            SECONDARY_PCM,
            0,
            0
        },
        {
            /**< Max # of channel for each PCM intf */
            8,  /**< Pri PCM */
            8,  /**< Sec PCM */
            0,  /**< Un-supported */
            0   /**< Un-supported */
        },
        {
            DATA_LINE_BIDIRECT, /**< Pri PCM, bi-dir */
            DATA_LINE_BIDIRECT, /**< Sec PCM, bi-dir */
            0,
            0
        },
        SUPPORTED   /**< PCM Interface is supported */
    }, /**< PCM Device Config end */

    /**< TDM Device Config Start */
    {
        0xC0C0010, /**< LPASS_LPAIF_PCM_CTL0, offset addr */
        2, /**< Number of supported TDM interfaces */
        {
            /**< List of supported TDM interfaces */
            PRIMARY_PCM,
            SECONDARY_PCM,
            0,
            0
        },
        {
            /**< Max # of channel for each TDM intf */
            8,
            8,
            0,
            0
        },
        {
            DATA_LINE_BIDIRECT,
            DATA_LINE_BIDIRECT,
            0,
            0
        },
        SUPPORTED
    }, /**< TDM Device Config end */

    /**< HDMI 1.4 Device Config */
    {
        0,               /**< HDMI reg base addr */
        0,               /**< Reg page size */
        NOT_SUPPORTED    /**< HDMI 1.4 is un-supported */
    },

    /**< I2S/PCM selection mux */
    {
        0,             /**< Mux reg offset addr */
        0,             /**< Number of selection mux */
        {0, 0, 0, 0},  /**< List of supported mux  */
        NOT_SUPPORTED  /**< Mux is un-supported */
    },

    /**< LPASS h/w revision */
    LPASS_HW_VER_3_10_0
};

/**< AudioIF DMA Configuration */
lpasshwio_prop_afe_dma_struct_t audioif_dma_prop =
{
    0xC0C0000,              /**< LPAIF reg base addr: LPASS_LPAIF_VERSION */
    (64*1024),              /**< LPAIF reg page size */
    0xC0CF000,              /**< DMA offset: LPASS_LPAIF_IRQ_EN0 : interrupt */
    0xC0D2000,              /**< DMA offset: LPASS_LPAIF_RDDMA_CTLa */
    0xC0D8000,              /**< DMA offset: LPASS_LPAIF_WRDMA_CTLa */
    0xC0D2028,              /**< STC rddma offset: LPASS_LPAIF_RDDMA_STC_LSB0 */
    0xC0D8028,              /**< STC wrdma offset: LPASS_LPAIF_WRDMA_STC_LSB0 */
    0,    /**<  not required  LPASS_LPAIF_WRDMA_CTL5*/
    0,    /**< not required  LPASS_LPAIF_WRDMA_STC_LSB5  */
    4,                      /**< Read DMA channel count */
    4,                      /**< Write DMA channel count */
    19,                     /**< DMA intr L2VIC # */
    2,                      /**< interrupt line # */
    LPASS_HW_VER_3_10_0     /**< LPASS hw revision */
};

/**< HDMI Out DMA Configuration */
lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop =
{
    0,    /**<  hdmi output reg base addr */
    0,    /**<  hdmi output reg page size */
    0,    /**<  not requried */
    0,    /**<  not required */
    0,    /**<  not requried */
    0,    /**<  not required */
    0,    /**<  not required */
    0,    /**<  not required  LPASS_LPAIF_WRDMA_CTL5*/
    0,    /**< not required  LPASS_LPAIF_WRDMA_STC_LSB5  */
    0,    /**<  sink DMA channel count */
    0,    /**<  source DMA channel count */
    0,    /**<  l2vic id for hdmi interrupt */
    0,    /**<  there is no interrupt host */
    0     /**<  hdmiout hw revision */
};

/**< AV TImer + Aud Sync Configuration */
HwdAvtimerPropertyType avtimer_prop =
{
    0x0C0A3000,   /**< Reg base address: LPASS_LPASS_SYNC_WRAPPER */
    0x2000,       /**< Register page size */

    /* AV timer Hw instances information */
    {
        /* AVTIMER_HW_INSTANCE_INDEX_0 */
        {
            0xC0A3000,               /**< AVT reg base addr: LPASS_AVTIMER_HW_VERSION>*/
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

    0xC0A4000,    /**< AudSync reg base addr: LPASS_AUD_SYNC_CTL */
    50,           /**< AV Timer intr L2VIC # */
    0x30040000    /**< AV Timer h/w version */
};

/**< HW Resampler Configuration */
HwdResamplerPropertyType resampler_prop =
{
    0,       /**< HW resampler reg base address */
    0,       /**< Reg page size */
    0,       /**< HW resampler intr irq # */
    0        /**< Resampler hw version */
};

/**< VFR + Timestamp Configuration */
lpasshwio_vfr_prop_t vfr_prop =
{
    0xC043000,       /**<  VFR reg base address  */
    0x9000,          /**<  VFR register page size */
    0xC047000,       /**<  LPASS_VFR_IRQ_MUX_CTL_1 */
    0xC049000,       /**<  LPASS_GP_IRQ_TS_MUX_CTL_0 */
    0xC04A000,       /**<  LPASS_GP_IRQ_TS_MUX_CTL_1 */
    0x10000000,      /**<  VFR hw revision */
    0x1,             /**<  VFR hw latching version */
    0x2,             /**<  Number of VFR timestamp mux */
    2,               /**<  Number of General Purpose timestamp mux */
    {
        /**<  L2VIC # for each VFR interrupt */
        51,   /**< VFR_0 intr L2VIC # */
        124   /**< VFR_1 intr L2VIC # */
    },
    {
        /** Mux input index for each VFR source */
        {0x0, 0x1},     /**<  vfr_src_prop[0]: {mux_ctl_in, hw_supported} */
        {0x7, 0x1}      /**<  vfr_src_prop[1]: {mux_ctl_in, hw_supported} */
    },
    IRQ_TYPE_DEFAULT   /**< VFR Interrupt type */
};

/**< BT-FM (WCNSS Interface) Configuration */
lpasshwio_prop_riva_struct_t afe_riva_prop =
{
    1,                   /**< H/w revision */
    0xA206008,           /**< CCU MB0 physical address */
    0xC285000,           /**< Q6SS IPC register physical address */
    16,                  /**< Q6ss IPC acknowledge interrupt bit position */
    0x20,                /**< Mail box offset form CCU MB0 */
    17,                  /**< Internal BT Tx L2VIC# */
    10,                  /**< Internal BT Rx L2VIC# */
    16,                  /**< Internal FM Tx/Rx L2VIC# */
    15,                  /**< Internal BTFM IPC L2VIC# */
    1                   /**< AVTIMER HW latching support */
};

/**< SLIMbus Configuration */
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

/**< HDMI Out Configuration */
lpasshwio_prop_hdmi_output_struct_t hdmi_output_prop =
{
    0x0,  /**<  hdmi output reg base addr */
    0x0,  /**<  hdmi output reg page size */
    0x0,  /**<  hdmi output reset reg base addr */
    0x0,  /**<  hdmi output reset reg page size */
    0x0,  /**<  hdmi core output reg base addr */
    0x0,  /**<  hdmi core output reg page size */
    0x0   /**<  hdmi output hw version */
};

/**< LPM Configuration */
lpm_prop_struct_t lpm_prop =
{
    0xC0E0000,  /**< Starting physical address, 64-bit */
    32*1024,    /**< Size of LPM in bytes */
    32*1024,    /**< Size allocated for AFE DMA buffers, in bytes */
    0,          /**< Size allocated for AFE working buffer, in bytes */
    QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE,  /**< Cache attribute: L1 writeback L2 uncached */
    2 /**< AHB bus vote scaling factor to reach LPM. 1 - if uncached or cached with writethrough (Unidirectional access)
                                                    2 - if cached and writeback (Duplex access) */
};

HwdGenericClkPropertyType genericclk_prop =
{
    0xC050000,              /**< TCSR reg base address */
    (64*1024),              /**< TCSR reg size in bytes */
    LPASS_HW_VER_3_10_0,    /**< hw revision */
    MMPM_CORE_ID_LPASS_CORE, /**< clk_mgr_coreid */
    20,                     /**< Number of clk-id's defined below   */

    /**< List of clock ids */
    {
        /**< Mi2s interface bit/osr clocks */
        0x100, 0x101,  /**< Pri mi2s IBIT / EBIT clock */
        0x102, 0x103,  /**< Sec mi2s IBIT / EBIT clock */
        0x104, 0x105,  /**< Ter mi2s IBIT / EBIT clock */
        0x106, 0x107,  /**< Qua mi2s IBIT / EBIT clock */
        0x10B, 0x10C,  /**< Qui mi2s IBIT / EBIT clock */
        0x10D, 0x10E,  /**< Sen mi2s IBIT / EBIT clock */

        /**< PCM interface bit clocks */
        0x200, 0x201,  /**< Pri PCM IBIT / EBIT clock */
        0x202, 0x203,  /**< Sec PCM IBIT / EBIT clock */

        /**< mclk / dig codec clock */
        0x300,   /**< mclk1 */
        0x301,   /**< mclk2 */
        0x303,   /**< Internal dig codec core clock */

        /**< PCM OE clk-id (internal definition) */
        0x20000
    },

    4, /**< Number of PCM clock defined with OE set availability  */

    /**< List of PCM Clk Id that supported OE clk  */
    {0x200, 0x201, 0x202, 0x203},

    /**< Clk Id to use to enable individual PCM OE  */
    {0x20000,  0x20000,  0x20000,  0x20000},

    /**< Frequency (Hz) of PCM OE clock */
    {61440000, 61440000, 61440000, 61440000}
};

lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
