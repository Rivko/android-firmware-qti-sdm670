/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/config/lpasshwio_devcfg_670.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE: lpasshwio_devcfg_670.c

DESCRIPTION: SDM670 Device Config

Copyright (c) 2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
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
   0x62CC0000,       /**< LPASS register base address */
   GPIO_TYPE_PDC,    /**< Codec Intr GPIO type: LPI */ 
   IRQ_TYPE_PDC,     /**< Codec Intr type: PDC gated */
   79,               /**< GPIO# for codec interrupt */
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
static uint32 ter_mi2s_ports[]     =
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

// List of Quinary mi2s ports
static uint32 quin_mi2s_ports[]    =
{
    AFE_PORT_ID_QUINARY_MI2S_RX,
    AFE_PORT_ID_QUINARY_MI2S_TX
};

// List of int0 mi2s ports
static uint32 int0_mi2s_ports[]    = { AFE_PORT_ID_INT0_MI2S_RX };

// List of int1 mi2s ports
static uint32 int1_mi2s_ports[]    = { AFE_PORT_ID_INT1_MI2S_RX };

// List of int2 mi2s ports
static uint32 int2_mi2s_ports[]    = { AFE_PORT_ID_INT2_MI2S_TX };

// List of int3 mi2s ports
static uint32 int3_mi2s_ports[]    = { AFE_PORT_ID_INT3_MI2S_TX };

// List of int4 mi2s ports
static uint32 int4_mi2s_ports[]    = { AFE_PORT_ID_INT4_MI2S_RX };

// List of int5 mi2s ports
static uint32 int5_mi2s_ports[]    = { AFE_PORT_ID_INT5_MI2S_TX };

// List of int6 mi2s ports
static uint32 int6_mi2s_ports[]    = { AFE_PORT_ID_INT6_MI2S_TX };

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

/* For warlock, spkr_i2s interface is updated
 * as bi-directional and named as quinary i2s.*/
static  lpasshwio_intf_port_info_t quin_mi2s_port_info    =
{
    SPEAKER_I2S, /* interface index
    This index is used to configure mux and I2s control */
    4,            /* maximum # of channel for audio if. */
    DATA_LINE_BIDIRECT, /* Direction */
    NUM_INTF_PORTS(quin_mi2s_ports), /* no of ports available */
    quin_mi2s_ports     /* port list */
};

static  lpasshwio_intf_port_info_t int0_mi2s_port_info    =
{
    INT_MI2S_0, /* interface index */
    4,               /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(int0_mi2s_ports), /* no of ports available */
    int0_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int1_mi2s_port_info    =
{
    INT_MI2S_1, /* interface index */
    2,               /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(int1_mi2s_ports), /* no of ports available */
    int1_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int2_mi2s_port_info    =
{
    INT_MI2S_2, /* interface index */
    2,               /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(int2_mi2s_ports), /* no of ports available */
    int2_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int3_mi2s_port_info    =
{
    INT_MI2S_3, /* interface index */
    4,               /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(int3_mi2s_ports), /* no of ports available */
    int3_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int4_mi2s_port_info    =
{
    INT_MI2S_4, /* interface index */
    2,               /* maximum # of channel for audio if. */
    DATA_LINE_SINK, /* Direction */
    NUM_INTF_PORTS(int4_mi2s_ports), /* no of ports available */
    int4_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int5_mi2s_port_info    =
{
    INT_MI2S_5, /* interface index */
    4,               /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(int5_mi2s_ports), /* no of ports available */
    int5_mi2s_ports /* port list */
};

static  lpasshwio_intf_port_info_t int6_mi2s_port_info    =
{
    INT_MI2S_6, /* interface index */
    2,               /* maximum # of channel for audio if. */
    DATA_LINE_SOURCE, /* Direction */
    NUM_INTF_PORTS(int6_mi2s_ports), /* no of ports available */
    int6_mi2s_ports /* port list */
};



lpasshwio_prop_lpaif_struct_t lpaif_prop =
{
    0x62E80000,  /* lpaif reg base addr LPASS_LPAIF_VERSION*/
    (256*1024),  /* lpaif reg page size*/
    {
        0x62E81000,   /*i2s reg offset : LPASS_LPAIF_I2S_CTL0*/
        0x62EA0000,   /* internal i2s reg offset : LPASS_LPAIF_INT_I2S_CTL0*/
        12,           /* inf_type_cnt*/
        {  /**< List of supported mi2s interfaces */
            &pri_mi2s_port_info,
            &sec_mi2s_port_info,
            &ter_mi2s_port_info,
            &quat_mi2s_port_info,
            &quin_mi2s_port_info,
            &int0_mi2s_port_info,
            &int1_mi2s_port_info,
            &int2_mi2s_port_info,
            &int3_mi2s_port_info,
            &int4_mi2s_port_info,
            &int5_mi2s_port_info,
            &int6_mi2s_port_info
        },
        SUPPORTED   /* is_supported*/
    },
    {
        0x62E81500, /*pcm reg offset LPASS_LPAIF_PCM_CTL0*/
        5,           /* num_interfaces */
        {
            PRIMARY_PCM,
            SECONDARY_PCM,
            TERTIARY_PCM,
            QUATERNARY_PCM,
            QUINARY_PCM
        },
        {
            4,                    /* maximum # of channel for audio if PRIMARY_PCM.*/
            4,                    /* maximum # of channel for audio if SECONDARY_PCM.*/
            4,                     /* maximum # of channel for audio if TERTIARY_PCM.*/
            4,                       /* maximum # of channel for audio if QUATERNARY_PCM.*/
            4                        /* maximum # of channel for audio if QUINARY_PCM.*/
        },
        {
            DATA_LINE_BIDIRECT,     /* Direction for audio if PRIMARY_PCM.*/
            DATA_LINE_BIDIRECT,     /* Direction for audio if SECONDARY_PCM.*/
            DATA_LINE_BIDIRECT,     /* Direction for audio if TERTIARY_PCM.*/
            DATA_LINE_BIDIRECT,      /* Direction for audio if QUATERNARY_PCM.*/
            DATA_LINE_BIDIRECT      /* Direction for audio if QUINARY_PCM.*/
        },
        SUPPORTED                       /* is pcm supported */
    },

    {
        0x62E81500,                    /*tdm uses the same pcm reg offset LPASS_LPAIF_PCM_CTL0*/
        5,                             /* num_interfaces */
        {
            PRIMARY_TDM,
            SECONDARY_TDM,
            TERTIARY_TDM,
            QUATERNARY_TDM,
            QUINARY_TDM
        },
        {
            8,                      /* maximum # of channel for audio if PRIMARY_TDM.*/
            8,                      /* maximum # of channel for audio if SECONDARY_TDM.*/
            8,                      /* maximum # of channel for audio if TERTIARY_TDM.*/
            8,                       /* maximum # of channel for audio if QUATERNARY_TDM.*/
            8                       /* maximum # of channel for audio if QUINARY_TDM.*/
        },
        {
            DATA_LINE_BIDIRECT,    /* Direction for audio if PRIMARY_TDM.*/
            DATA_LINE_BIDIRECT,    /* Direction for audio if SECONDARY_TDM.*/
            DATA_LINE_BIDIRECT,    /* Direction for audio if TERTIARY_TDM.*/
            DATA_LINE_BIDIRECT,     /* Direction for audio if QUATERNARY_TDM.*/
            DATA_LINE_BIDIRECT     /* Direction for audio if QUINARY_TDM.*/
        },
        SUPPORTED                 /* is tdm supported  */
    },

    {
        0x0AEA0000,  //MMSS_HDMI_CTRL
        (4*1024),  // hdmiAudHwRegSize
        SUPPORTED
    },
    {
        0x62E9B000,    /* mux reg base LPASS_LPAIF_PCM_I2S_SEL0*/
        5,           /* # of interfaces that  is shared the output with mux*/
        {
            PRIMARY_MUX,  /* 1:1 mapping for i2s and pcm  interface*/
            SECONDARY_MUX,
            TERTIARY_MUX,
            QUATERNARY_MUX,
            QUINARY_MUX
        },
        SUPPORTED
    },
    LPASS_HW_VER_4_2_0,  // hw revision
};


lpasshwio_prop_afe_dma_struct_t audioif_dma_prop =
{
    0x62E80000,  // lpaif reg base addr LPASS_LPAIF_VERSION
    (128*1024),  // lpaif reg page size
    0x62E8A000,  // dma offset LPASS_LPAIF_IRQ_EN0 : interrupt
    0x62E8D000,  // dma offset LPASS_LPAIF_RDDMA_CTLa
    0x62E93000,  // dma offset LPASS_LPAIF_WRDMA_CTLa
    0x62E8D028,  // stc rddma offset   LPASS_LPAIF_RDDMA_STC_LSB0
    0x62E93028,  // stc wrdma offset LPASS_LPAIF_WRDMA_STC_LSB0
    0,           // dma offset LPASS_LPAIF_WRDMA_CTL5
    0,           // stc wrdma offset LPASS_LPAIF_WRDMA_STC_LSB5
    6,           // sink DMA channel count
    6,           // source DMA channel count
    19,          // lpaif intr irq #
    2,           // interrupt line #
    LPASS_HW_VER_4_2_0   //  i2s hw revision
};

lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop = {
    0x62CF8000,   // hdmi output reg base addr LPASS_HDMITX_VERSION
    0x8000,       // hdmi output reg page size
    0,            // not requried
    0,            // not required
    0,            // not requried
    0,            // not required
    0,            // not required
    0,            // not required
    0,            // not required
    4,   // sink DMA channel count
    0,   // source DMA channel count
    17,  // l2vic id for hdmi interrupt
    0,   // there is no interrupt host
    LPASS_HW_VER_4_2_0   //  hdmiout hw revision
};


/**< AV TImer + Aud Sync Configuration */
HwdAvtimerPropertyType avtimer_prop =
{
    0x62CF7000,    /**< Reg base address: LPASS_AVTIMER_HW_VERSION */
    0x1000,        /**< Register page size */

    /* AV timer Hw instances information */
    {
        /* AVTIMER_HW_INSTANCE_INDEX_0 */
        {
            0x62CF7000,              /**< AVT reg base addr: LPASS_AVTIMER_HW_VERSION>*/
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

HwdResamplerPropertyType resampler_prop =
{
    0x62D38000,   // baseRegAddr: base address of resampler - LPASS_RESAMPLER_HW_VERSION
    0x8000,       // regSize: hw resampler reg page size
    53,           // isrHookPinNum: hw resampler intr irq #
    0x40000000    // hw_revision: resampler hw version
};

lpasshwio_vfr_prop_t vfr_prop =
{
    0x62B69000,      // VFR reg base address LPASS_VFR_IRQ_MUX_CTL
    0x9000,          // VFR register page size
    0x62B6A000,      // LPASS_VFR_IRQ_MUX_CTL_1
    0x62B6C000,      // LPASS_GP_IRQ_TS_MUX_CTL_0
    0x62B6D000,      // LPASS_GP_IRQ_TS_MUX_CTL_1
    0x10000000,      // VFR hw revision
    0x1,             // VFR hw latching version
    0x2,             // Number of VFR timestamp mux
    2,               // Number of General Purpose timestamp mux
    {36, 40}, // l2vic_num[]: Available L2VIC # for VFR interrupts
    {{0x0, 0x1},     // vfr_src_prop[0]: Mux ctrl for VFR src sel, VFR src is hw-supported or not
   {0x7, 0x1}},     // vfr_src_prop[1]
   IRQ_TYPE_PDC     // VFR Interrupt type
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
            1,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
            {4, 2}
        },

        /* SLIMbus module device id 1*/
        {
            /* indicates if multi-endpoint is supported in hardware */
            1,
            /* list of max end points supported per master.
             * Should be filled in decreasing order*/
            {2,1}
        }
    }
};

lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};

lpasshwio_prop_hdmi_output_struct_t hdmi_output_prop =
{
    0x62CF8000,   // hdmi output reg base addr - LPASS_HDMITX_VERSION
    0x8000,       // hdmi output reg page size
    0x62CC8000,   // hdmi output reset reg base addr - LPASS_HDMITX_CTL
    0x1000,       // hdmi output reset reg page size
    0x0AEA0000,   // hdmi core output reg base addr - MMSS_HDMI_CTRL
    0x1000,       // hdmi core output reg page size
    LPASS_HW_VER_4_0_2  // hdmi output hw version
};

/* LPM configuration for SD660 */
lpm_prop_struct_t lpm_prop =
{
    0x62D20000, /**< Starting physical address, 64-bit */
    64*1024,   /**< Size of LPM in bytes */
    64*1024,   /**< Size allocated for AFE DMA buffers, in bytes */
    0,         /**< Size allocated for AFE working buffer, in bytes */
    QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE,  /**< Cache attribute: Uncached L1 and L2 */
    2 /**< AHB bus vote scaling factor to reach LPM. 1 - if uncached or cached with writethrough (Unidirectional access)
                                                                               2 - if cached and writeback (Duplex access) */
};


HwdGenericClkPropertyType genericclk_prop =
{
    0x62CC0000,     // base address of LPASS_CSR  -- LPASS_CSR
    (152*1024),     // csr reg size
    LPASS_HW_VER_4_2_0,    // hw revision
    MMPM_CORE_ID_LPASS_CORE, /**< clk_mgr_coreid */
    36,            // Number of clk-id defined below
    // List of clock id, correpsonds to AFE_CLOCK_SET_CLOCK_ID_XX
    {

        0x100, 0x101,  /**< Pri mi2s IBIT / EBIT clock */
        0x102, 0x103,  /**< Sec mi2s IBIT / EBIT clock */
        0x104, 0x105,  /**< Ter mi2s IBIT / EBIT clock */
        0x106, 0x107,  /**< Qua mi2s IBIT / EBIT clock */
        0x10B, 0x10C,  0x116, /**< Qui mi2s IBIT / EBIT / OSR clock */
        0x10F,         /**< INT0 mi2s IBIT  clock */
        0x110,         /**< INT1 mi2s IBIT  clock */
        0x111,         /**< INT2 mi2s IBIT  clock */
        0x112,         /**< INT3 mi2s IBIT  clock */
        0x113,         /**< INT4 mi2s IBIT  clock */
        0x114,         /**< INT5 mi2s IBIT  clock */
        0x115,         /**< INT6 mi2s IBIT  clock */

        /**< PCM interface bit clocks */
        0x200, 0x201,  /**< Pri PCM IBIT / EBIT clock */
        0x202, 0x203,  /**< Sec PCM IBIT / EBIT clock */
        0x204, 0x205,  /**< Ter PCM IBIT / EBIT clock */
        0x206, 0x207,  /**< Qua PCM IBIT / EBIT clock */
        0x208, 0x209,  0x20A, /**< Qui PCM IBIT / EBIT / OSR clock */

        /**< mclk / dig codec clock */
        0x300,        /**< mclk1 */
        0x301,        /**< mclk2 */
        0x302,        /**< mclk3 */
        0x305,        /**< int mclk0 */
        0x306,        /**< int mclk1 */
        0x307,        /**< SWR NPL clk */

        /**<  This is the PCM OE clk-id (internal definition) */
        0x20000
    },
    10, // Number of PCM clock defined with OE set availability
    // List of PCM Clk Id that supported OE clk
    {   0x200,      0x201,    0x202,    0x203,    0x204,    0x205,    0x206,    0x207,    0x208,    0x209},
    // Clk Id to use to enable individual PCM OE
    {   0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000,  0x20000},
    // Frequency of PCM OE clock
    {  61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000, 61440000}
};

