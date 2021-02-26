/**
    @file  spi_iface.c
    @brief SPI driver implementation
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/19/19   NM      Support FIFO Packing
 01/07/19   bng		initial version
=============================================================================*/

#include "spi_drv.h"
#include <spi_config.h>
#include "spi_iface.h"
#include "spi_plat.h"
#include "spi_log.h"
#include "SpiApi.h"
#include "spi_api.h"
#include "hw_io.h"
#include "spi_fw_load.h"


#define BUS_IFACE_TCE_STATUS_BIT_NACK       (1 << 10)
#define BUS_IFACE_TCE_STATUS_BIT_ARB_LOST   (1 << 12)
#define BUS_IFACE_TCE_STATUS_BIT_BUS_ERROR  (1 << 13)

#define POLL_TIME_DEFAULT_US    40960000
#define POLL_INTERVAL_US        5

/* Values from Image txt*/
#define GENI_FW_REVISION		0x333
#define GENI_S_FW_REVISION		0x0
#define GENI_INIT_CFG_REVESION	0x2


#define BITS_IN_BYTE 8

#define GENI_M_CMD0_SPI_PARAM_PRE_CMD_DLY      0x00000001 /**< CS Asssertion to CLK ON delay. */
#define GENI_M_CMD0_SPI_PARAM_TIMESTAMP_BFR    0x00000002 /**< Record timestamp before to transfer starts. */
#define GENI_M_CMD0_SPI_PARAM_FRAGMENTATION    0x00000004 /**< Keep CS asserted at the end of the sequence. */
#define GENI_M_CMD0_SPI_PARAM_TIME_STAMP_AFT   0x00000008 /**< Record timestamp after transfer is finished. */
#define GENI_M_CMD0_SPI_PARAM_POST_CMD_DLY     0x00000010 /**< . */

typedef enum spi_cmd_opcode
{
    GENI_M_CMD0_SPI_OPCODE_INVALID = 0,
    GENI_M_CMD0_SPI_OPCODE_TX_ONLY = 1,
    GENI_M_CMD0_SPI_OPCODE_RX_ONLY = 2,
    GENI_M_CMD0_SPI_OPCODE_FULL_DUPLEX = 3,
    GENI_M_CMD0_SPI_OPCODE_TX_THEN_RX = 7,
    GENI_M_CMD0_SPI_OPCODE_CS_ASSERT = 8,
    GENI_M_CMD0_SPI_OPCODE_CS_DEASSERT = 9,
    GENI_M_CMD0_SPI_OPCODE_SCK_ONLY = 10,
} spi_cmd_opcode;

typedef struct spi_packing
{
    uint8 start;
    uint8 direction;
    uint8 length;
    uint8 stop;
} spi_packing;

spi_status spi_iface_enable_m_irqs(spi_hw_ctxt *h_ctxt)
{
    spi_plat_device_config  *dcfg;
    uint8                   *base_addr;

    dcfg = (spi_plat_device_config *)h_ctxt->core_config;

    if (dcfg == NULL)
    {
        return SPI_ERROR_INVALID_PARAM;
    }

    base_addr = dcfg->core_base_addr + dcfg->core_offset;

    REG_OUT(
        base_addr,
        SE_GENI_M_IRQ_EN_SET,
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_CMD_OVERRUN_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_ILLEGAL_CMD_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_CMD_FAILURE_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_CMD_CANCEL_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_CMD_ABORT_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_TIMESTAMP_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, IO_DATA_ASSERT_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, IO_DATA_DEASSERT_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, RX_FIFO_RD_ERR_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, RX_FIFO_WR_ERR_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, TX_FIFO_RD_ERR_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, TX_FIFO_WR_ERR_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, RX_FIFO_WATERMARK_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, TX_FIFO_WATERMARK_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, RX_FIFO_LAST_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, M_CMD_DONE_EN_SET, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_SET, SEC_IRQ_EN_SET, 1));

    return SPI_SUCCESS;
}

spi_status spi_iface_disable_m_irqs(spi_hw_ctxt *h_ctxt)
{
    spi_plat_device_config  *dcfg;
    uint8                   *base_addr;

    dcfg = (spi_plat_device_config *)h_ctxt->core_config;

    if (dcfg == NULL)
    {
        return SPI_ERROR_INVALID_PARAM;
    }

    base_addr = dcfg->core_base_addr + dcfg->core_offset;

    REG_OUT(
        base_addr,
        SE_GENI_M_IRQ_EN_CLEAR,
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, TX_FIFO_WATERMARK_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, RX_FIFO_LAST_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_CMD_OVERRUN_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_ILLEGAL_CMD_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_CMD_FAILURE_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_CMD_CANCEL_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_CMD_ABORT_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_TIMESTAMP_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, IO_DATA_ASSERT_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, IO_DATA_DEASSERT_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, RX_FIFO_RD_ERR_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, RX_FIFO_WR_ERR_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, TX_FIFO_RD_ERR_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, TX_FIFO_WR_ERR_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, RX_FIFO_WATERMARK_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, M_CMD_DONE_EN_CLEAR, 1) |
        FLD_SET(SE_GENI_M_IRQ_EN_CLEAR, SEC_IRQ_EN_CLEAR, 1));

    return SPI_SUCCESS;
}

void bus_iface_write_to_fifo (uint8 *base_addr, spi_xfer_ctxt *t_ctxt)
{
    spi_descriptor_t *dc = t_ctxt->desc + t_ctxt->xfer_count;
    uint32 tx_transferred = t_ctxt->tx_transferred;
    uint8 *buffer = dc->tx_buf + tx_transferred;
    uint32 length = dc->len;
    uint32 word = 0, i = 0, j = 0, bytes_sent = 0;
    uint32 fifo_entry_byte_cnt;
    uint32 free_fifo_entries;
    uint32 bytes_to_write;
    uint32 fifo_width, fifo_depth, watermark;
    boolean xfer_done = FALSE;

    fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_0, TX_FIFO_DEPTH);
    fifo_width = REG_INF(base_addr, SE_SE_HW_PARAM_0, TX_FIFO_WIDTH);
    watermark = REG_INF(base_addr, SE_GENI_TX_WATERMARK_REG, TX_WATERMARK);
    free_fifo_entries = fifo_depth - watermark;
    fifo_entry_byte_cnt = fifo_width / BITS_IN_BYTE;
    bytes_to_write = free_fifo_entries * fifo_entry_byte_cnt;

    // transfer are done
    if (length == t_ctxt->tx_transferred) {
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, FLD_SET(SE_GENI_M_IRQ_CLEAR, TX_FIFO_WATERMARK_CLEAR, 1));
        return;
    }

    if ((length - tx_transferred) < bytes_to_write)
    {
        bytes_to_write = length - tx_transferred;
        // if all the remaining data will be written, then disable watermark
        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, 0));
    }

    bytes_sent = 0;
    for (i = 0; i < free_fifo_entries && !xfer_done; i++)
    {
        // build the buffer to write
        word = 0;
        for (j = 0; j < fifo_entry_byte_cnt && bytes_sent < bytes_to_write; j++)
        {
            word = word | (buffer[bytes_sent] << (j * BITS_IN_BYTE));
            bytes_sent++;
            if (bytes_sent >= bytes_to_write)
            {
                xfer_done = TRUE;
            }
        }
        REG_OUT(base_addr, SE_GENI_TX_FIFOn, word);
    }

    t_ctxt->tx_transferred += bytes_to_write;

    // clear the status bit
    REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_BMSK);
}
void bus_iface_read_from_fifo (uint8 *base_addr, spi_xfer_ctxt *t_ctxt)
{
    uint32 rx_fifo_status = REG_IN(base_addr, SE_GENI_RX_FIFO_STATUS);
    uint8  bytes_in_last_word = 0;
    uint32 bytes_to_read = FLD_GET(rx_fifo_status, SE_GENI_RX_FIFO_STATUS, RX_FIFO_WC) << 2;
    uint32 transferred = t_ctxt->rx_transferred;
    spi_descriptor_t *dc = t_ctxt->desc + t_ctxt->xfer_count;
    uint8 *buffer = dc->rx_buf + transferred;
    uint32 length = dc->len;
    uint32 word = 0, i = 0;

    if (FLD_GET(rx_fifo_status, SE_GENI_RX_FIFO_STATUS, RX_LAST))
    {
        bytes_in_last_word = FLD_GET(rx_fifo_status, SE_GENI_RX_FIFO_STATUS, RX_LAST_BYTE_VALID);
        bytes_to_read = (bytes_to_read - 4) + bytes_in_last_word;
    }

    if ((length - transferred) < bytes_to_read)
    {
        bytes_to_read = length - transferred;
    }

    for (i = 0; i < bytes_to_read; i++)
    {
        if ((i & 0x3) == 0)
        {
            word = REG_IN(base_addr, SE_GENI_RX_FIFOn);
        }
        buffer[i] = (word >> ((i & 0x3) << 3)) & 0xFF;
    }
    t_ctxt->rx_transferred += bytes_to_read;
    
	// clear the status bit
    REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK);	
}

uint32 bus_iface_get_bytes_transferred (spi_xfer_ctxt *t_ctxt)
{
    uint32 i, transferred = 0;
    spi_descriptor_t *dc = t_ctxt->desc;
    
	if(t_ctxt->xfer_count == 0)
	{
	  return 0;
	}
	
    for (i = 0; i < (t_ctxt->xfer_count - 1); i++)
    {
        transferred += dc->len;
        dc++;
    }

    return (transferred);
}



static spi_cmd_opcode spi_iface_get_opcode(spi_descriptor_t *dc)
{
    if (dc->rx_buf && dc->tx_buf)
    {
        return GENI_M_CMD0_SPI_OPCODE_FULL_DUPLEX;
    }
    else if (dc->tx_buf && (dc->rx_buf == NULL))
    {
        return GENI_M_CMD0_SPI_OPCODE_TX_ONLY;
    }
    else if (dc->rx_buf && (dc->tx_buf == NULL))
    {
        return GENI_M_CMD0_SPI_OPCODE_RX_ONLY;
    }

    return GENI_M_CMD0_SPI_OPCODE_INVALID;
}

static uint32 spi_iface_get_params(spi_config_t *config)
{
    uint32 params = 0;

    if (config->cs_clk_delay_cycles)
    {
        params |= GENI_M_CMD0_SPI_PARAM_PRE_CMD_DLY;
    }

    return params;
}

// The code supports only 8, 16, 32 bits packing in little endianness format
static void spi_iface_get_packing_config(spi_config_t *config, spi_packing *packing_vec)
{
    uint8 ws = config->spi_bits_per_word;
    uint8 c_ws = 0;
    uint8 run_up_ws = 0;
    uint8 pack_words;
    uint8 iter = 0;
    uint8 i, idx, start_idx;
	
	if (ws <= 8)
        pack_words = 4;
    else if (ws <= 16)
        pack_words = 2;
    else
        pack_words = 1;

    run_up_ws = (ws & (BITS_IN_BYTE - 1)) ?
        ((ws & ~(BITS_IN_BYTE - 1)) + BITS_IN_BYTE) : ws;

    iter = (run_up_ws * pack_words) >> 3;

    start_idx = ws - 1;
    idx = start_idx;
    c_ws = ws;

    for (i = 0; i < iter; i++)
    {
        c_ws = c_ws ? c_ws : ws;
        packing_vec[i].start = idx;
        packing_vec[i].direction = 1;
        packing_vec[i].length = (c_ws > BITS_IN_BYTE) ? (BITS_IN_BYTE - 1) : (c_ws - 1);
        packing_vec[i].stop = 0;

        c_ws = (c_ws > (packing_vec[i].length + 1)) ? (c_ws - (packing_vec[i].length + 1)) : 0;
        idx = c_ws ? (idx - (packing_vec[i].length + 1)) : (start_idx + ((i + 1) * BITS_IN_BYTE));
    }

    packing_vec[i - 1].stop = 1;
}

static void spi_iface_config(uint8 *base_addr, spi_config_t *config, uint8 dfs_index, uint32 clk_div)
{
    uint8 cpol = 0, cpha = 0;
    spi_packing packing_vec[4] = { { 0, 0, 0, 0 },
                                   { 0, 0, 0, 0 },
                                   { 0, 0, 0, 0 },
                                   { 0, 0, 0, 0 }};

    // dfs index
    // QUPv3 HPG. 3.2.1. General Configuration
    REG_OUT(base_addr, SE_GENI_CLK_SEL,
        FLD_SET(SE_GENI_CLK_SEL, CLK_SEL, dfs_index));

    // packing
    // QUPv3 HPG. 3.2.1. General Configuration. 3.2.8. Actual configuration
    spi_iface_get_packing_config(config, packing_vec);

    REG_OUT(base_addr, SE_GENI_TX_PACKING_CFG0,
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_1_START_INDEX, packing_vec[1].start) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_1_DIRECTION, packing_vec[1].direction) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_1_LENGTH, packing_vec[1].length) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_1_STOP, packing_vec[1].stop) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_0_START_INDEX, packing_vec[0].start) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_0_DIRECTION, packing_vec[0].direction) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_0_LENGTH, packing_vec[0].length) |
        FLD_SET(SE_GENI_TX_PACKING_CFG0, VEC_0_STOP, packing_vec[0].stop));

    REG_OUT(base_addr, SE_GENI_TX_PACKING_CFG1,
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_3_START_INDEX, packing_vec[3].start) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_3_DIRECTION, packing_vec[3].direction) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_3_LENGTH, packing_vec[3].length) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_3_STOP, packing_vec[3].stop) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_2_START_INDEX, packing_vec[2].start) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_2_DIRECTION, packing_vec[2].direction) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_2_LENGTH, packing_vec[2].length) |
        FLD_SET(SE_GENI_TX_PACKING_CFG1, VEC_2_STOP, packing_vec[2].stop));

    REG_OUT(base_addr, SE_GENI_RX_PACKING_CFG0,
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_1_START_INDEX, packing_vec[1].start) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_1_DIRECTION, packing_vec[1].direction) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_1_LENGTH, packing_vec[1].length) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_1_STOP, packing_vec[1].stop) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_0_START_INDEX, packing_vec[0].start) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_0_DIRECTION, packing_vec[0].direction) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_0_LENGTH, packing_vec[0].length) |
        FLD_SET(SE_GENI_RX_PACKING_CFG0, VEC_0_STOP, packing_vec[0].stop));

    REG_OUT(base_addr, SE_GENI_RX_PACKING_CFG1,
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_3_START_INDEX, packing_vec[3].start) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_3_DIRECTION, packing_vec[3].direction) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_3_LENGTH, packing_vec[3].length) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_3_STOP, packing_vec[3].stop) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_2_START_INDEX, packing_vec[2].start) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_2_DIRECTION, packing_vec[2].direction) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_2_LENGTH, packing_vec[2].length) |
        FLD_SET(SE_GENI_RX_PACKING_CFG1, VEC_2_STOP, packing_vec[2].stop));

    // clk divider
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    REG_OUT(base_addr, SE_GENI_SER_M_CLK_CFG,
        FLD_SET(SE_GENI_SER_M_CLK_CFG, CLK_DIV_VALUE, clk_div) |
        FLD_SET(SE_GENI_SER_M_CLK_CFG, SER_CLK_EN, 1));

    //CPHA & CPOL.
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    switch (config->spi_mode)
    {
    case SPI_MODE_1: cpol = 0; cpha = 1; break;
    case SPI_MODE_2: cpol = 1; cpha = 0; break;
    case SPI_MODE_3: cpol = 1; cpha = 1; break;
    case SPI_MODE_0:
    default: cpol = 0; cpha = 0; break;
    }
    REG_OUT(base_addr, SE_SPI_CPHA,
        FLD_SET(SE_SPI_CPHA, CPHA, cpha));
    REG_OUT(base_addr, SE_SPI_CPOL,
        FLD_SET(SE_SPI_CPOL, CPOL, cpol));

    //Loopback.
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    REG_OUT(base_addr, SE_SPI_LOOPBACK_CFG,
        FLD_SET(SE_SPI_LOOPBACK_CFG, LOOPBACK_MODE, ((config->loopback_mode == 1) ? 1 : 0)));

    //DEMUX Selection (CS selection).
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    if (config->spi_slave_index > 3) {
        config->spi_slave_index = 0;
    }
    REG_OUT(base_addr, SE_SPI_DEMUX_SEL,
        FLD_SET(SE_SPI_DEMUX_SEL, CS_DEMUX_SEL, config->spi_slave_index));

    //DEMUX_OUTPUT_INV.
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    {
        uint16 cs_polarity = (config->spi_cs_polarity << config->spi_slave_index) &
            FLD_BMSK(SE_SPI_DEMUX_OUTPUT_INV, CS_DEMUX_OUTPUT_INV);
        REG_OUT(base_addr, SE_SPI_DEMUX_OUTPUT_INV,
            FLD_SET(SE_SPI_DEMUX_OUTPUT_INV, CS_DEMUX_OUTPUT_INV, cs_polarity));
    }

    //CS toggle b/w words.
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    REG_OUT(base_addr, SE_SPI_TRANS_CFG,
        FLD_SET(SE_SPI_TRANS_CFG, SPI_CS_TOGGLE, ((config->cs_toggle == 1) ? 1 : 0)));

    //CS CLK & Inter word delay.
    // QUPv3 HPG. 3.2.2.1 SPI Specific Configuration
    REG_OUT(base_addr, SE_SPI_DELAYS_COUNTERS,
        FLD_SET(SE_SPI_DELAYS_COUNTERS, SPI_CS_CLK_DLY, config->cs_clk_delay_cycles) |
        FLD_SET(SE_SPI_DELAYS_COUNTERS, SPI_INTER_WORDS_DLY, config->inter_word_delay_cycles));

    //Word length.
    REG_OUT(base_addr, SE_SPI_WORD_LEN,
        FLD_SET(SE_SPI_WORD_LEN, SPI_WORD_LEN, (config->spi_bits_per_word - 4)));
}
static uint32 spi_iface_num_transfers(uint32 num_bytes, uint8 word_size)
{
    uint32 num_transfers = 0;
    if (word_size > 16) {
        num_transfers = num_bytes / sizeof(uint32);
    }
    else if (word_size > 8) {
        num_transfers = num_bytes / sizeof(uint16);
    }
    else {
        num_transfers = num_bytes;
    }
    return num_transfers;
}

static spi_status bus_iface_transfer(spi_client_ctxt *c_ctxt, boolean is_last)
{
    spi_hw_ctxt             *h_ctxt;
    bus_iface_hw_ctxt       *g_ctxt;
    spi_xfer_ctxt           *t_ctxt;
    spi_plat_device_config  *dcfg;
    spi_descriptor_t        *dc;
    uint8                   *base_addr;
    uint32                   params = 0;
    spi_cmd_opcode           opcode = GENI_M_CMD0_SPI_OPCODE_INVALID;
    uint32 dfs_index;
    uint32 clk_div;

    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        return SPI_ERROR_INVALID_PARAM;
    }

    h_ctxt = c_ctxt->h_ctxt;
    g_ctxt = h_ctxt->core_iface;
    t_ctxt = &c_ctxt->t_ctxt;
    dcfg = (spi_plat_device_config *)h_ctxt->core_config;
    dc = t_ctxt->desc + t_ctxt->xfer_count;

    if ((dcfg == NULL) ||
        (t_ctxt == NULL) ||
        (t_ctxt->config == NULL) ||
        (dc == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"bus_iface_transfer : invalid parameter ");
		return SPI_ERROR_INVALID_PARAM;
    }

    t_ctxt->tx_transferred = 0;
    t_ctxt->rx_transferred = 0;

    /*if (SPI_SUCCESS != spi_get_dfs_setting(c_ctxt, t_ctxt->config->clk_freq_hz, &dfs_index, &clk_div, NULL))
    {
        return SPI_ERROR_PLAT_GET_CLOCK_CONFIG_FAIL;
    }*/
	if(!spi_plat_clock_set_frequency(dcfg, t_ctxt->config->clk_freq_hz, &dfs_index, &clk_div, FALSE))
	{
		SPI_LOG(LEVEL_ERROR,"bus_iface_transfer : set freq fail");
		return SPI_ERROR_PLAT_CLK_SET_FREQ_FAIL;
	}
	t_ctxt->saved_dfs_index = dfs_index;
    t_ctxt->saved_div = clk_div;
	
    base_addr = dcfg->core_base_addr + dcfg->core_offset;

    spi_iface_config(base_addr, t_ctxt->config, dfs_index, clk_div);

    params = spi_iface_get_params(t_ctxt->config);
    if (!is_last)
        params |= GENI_M_CMD0_SPI_PARAM_FRAGMENTATION;

    opcode = spi_iface_get_opcode(t_ctxt->desc);

    // Calculate the number of words to be transfered and the watermark levels
    // and configure the corresponding HW registers.
    {
        uint32 tx_len = 0, rx_len = 0;
        uint32 tx_watermark = 0, rx_watermark = 0;

        switch (opcode)
        {
        case GENI_M_CMD0_SPI_OPCODE_TX_ONLY:
            tx_len = spi_iface_num_transfers(dc->len, t_ctxt->config->spi_bits_per_word);
            tx_watermark = 1;

            break;

        case GENI_M_CMD0_SPI_OPCODE_RX_ONLY:
            rx_len = spi_iface_num_transfers(dc->len, t_ctxt->config->spi_bits_per_word);
            rx_watermark = 0;

            break;

        case GENI_M_CMD0_SPI_OPCODE_FULL_DUPLEX:
            tx_len = spi_iface_num_transfers(dc->len, t_ctxt->config->spi_bits_per_word);
            rx_len = spi_iface_num_transfers(dc->len, t_ctxt->config->spi_bits_per_word);
            tx_watermark = 1;
            rx_watermark = 0;
            break;

        default:
            return SPI_ERROR_INVALID_CMD_OPCODE;
        }

        // Configure Tx & Rx lengths in TX & RX TRANS LEN HW registers respectively.
        REG_OUT(base_addr, SE_SPI_TX_TRANS_LEN, tx_len);
        REG_OUT(base_addr, SE_SPI_RX_TRANS_LEN, rx_len);

        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, tx_watermark));
        REG_OUT(base_addr, SE_GENI_RX_WATERMARK_REG,
            FLD_SET(SE_GENI_RX_WATERMARK_REG, RX_WATERMARK, rx_watermark));
    }

    // Configure Opcode & Params in GENI_M_CMD0 register.
    REG_OUT(base_addr, SE_GENI_M_CMD0,
        FLD_SET(SE_GENI_M_CMD0, OPCODE, opcode) |
        FLD_SET(SE_GENI_M_CMD0, PARAM, params));

    t_ctxt->xfer_state = TRANSFER_IN_PROGRESS;

    return SPI_SUCCESS;
}
static void process_transfer_completion(spi_hw_ctxt *h_ctxt, spi_status status)
{
    spi_client_ctxt *temp;
    spi_xfer_ctxt *t_ctxt;

    do
    {
        //spi_plat_mutex_instance_lock (h_ctxt->queue_lock);
        // remove the client from the queue
        temp = h_ctxt->client_ctxt_head;
        h_ctxt->client_ctxt_head = temp->next;
        temp->next = NULL;

        // get transfer context
        t_ctxt = &(temp->t_ctxt);

        // store next client ctxt in temp
        temp = h_ctxt->client_ctxt_head;
        //spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);

        t_ctxt->xfer_status = status;

        if (t_ctxt->callback != NULL)
        {
            // should be implemented in a qdi abstraction
            //spi_notify_completion(t_ctxt->callback,
            //    t_ctxt->xfer_status,
            //    t_ctxt->ctxt);
        }
    } while (temp != NULL);
}

void bus_iface_isr(void *int_handle)
{
    spi_hw_ctxt *h_ctxt = (spi_hw_ctxt *)int_handle;
    bus_iface_hw_ctxt *g_ctxt;
    spi_client_ctxt *c_ctxt = NULL;
    spi_xfer_ctxt *t_ctxt;
    spi_status status = SPI_SUCCESS;
    spi_plat_device_config *dcfg;
    spi_descriptor_t *dc;
    uint8 *base_addr;
    uint32 irq_status, rx_fifo_status;

    if ((h_ctxt == NULL) ||
        (h_ctxt->core_config == NULL) ||
        (h_ctxt->core_iface == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"bus_iface_isr : invalid parameters ");
		return;
    }

    //spi_plat_mutex_instance_lock (h_ctxt->queue_lock);
    if (h_ctxt->client_ctxt_head != NULL)
    {
        c_ctxt = h_ctxt->client_ctxt_head;
    }
    //spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);
    if (c_ctxt == NULL)
    {
        return;
    }

    g_ctxt = h_ctxt->core_iface;
    t_ctxt = &c_ctxt->t_ctxt;
    dc = t_ctxt->desc + t_ctxt->xfer_count;
    dcfg = (spi_plat_device_config *)h_ctxt->core_config;
    base_addr = dcfg->core_base_addr + dcfg->core_offset;
    irq_status = REG_IN(base_addr, SE_GENI_M_IRQ_STATUS);

    // for all the status bits, we shall first handle the condition, and clear
    // the status bit.
    if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_RD_ERR))
    {
        status = SPI_ERROR_INPUT_FIFO_UNDER_RUN;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_WR_ERR))
    {
        status = SPI_ERROR_INPUT_FIFO_OVER_RUN;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_RD_ERR))
    {
        status = SPI_ERROR_OUTPUT_FIFO_UNDER_RUN;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_WR_ERR))
    {
        status = SPI_ERROR_OUTPUT_FIFO_OVER_RUN;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_OVERRUN))
    {
        status = SPI_ERROR_COMMAND_OVER_RUN;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_BMSK);
    }
    else if ((FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_CANCEL)) ||
        (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_ABORT)))
    {
        status = SPI_TRANSFER_FORCE_TERMINATED;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_BMSK);
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_ILLEGAL_CMD))
    {
        status = SPI_ERROR_COMMAND_ILLEGAL;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_FAILURE))
    {
        status = SPI_ERROR_COMMAND_FAIL;
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_BMSK);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_LAST))
    {
        rx_fifo_status = REG_IN(base_addr, SE_GENI_RX_FIFO_STATUS);
        if (!FLD_GET(rx_fifo_status, SE_GENI_RX_FIFO_STATUS, RX_LAST))
        {
            REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK);
        }
    }

    if (status != SPI_SUCCESS)
    {
        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, 0));
        t_ctxt->xfer_state = TRANSFER_DONE;
        goto exit;
    }

    // If the M_CMD_DONE flag is set, check whether there is some data to be read
    // from the FIFO, only when the read transactions are requested by the client.
    if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_DONE))
    {
        t_ctxt->xfer_state = TRANSFER_DONE;

        // clear the status bits
        REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_BMSK);

        if (dc->rx_buf)
        {
           bus_iface_read_from_fifo(base_addr, t_ctxt);
        }
    }
    // Otherwise check for Tx/Rx watermark status for further writing/reading
    // to/from FIFO respectively.
    else
    {
       if ((dc->tx_buf) && (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_WATERMARK)))
        {
            bus_iface_write_to_fifo(base_addr, t_ctxt);
        }
		if ((dc->rx_buf) && (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_WATERMARK)))
        {
            bus_iface_read_from_fifo(base_addr, t_ctxt);
        }

    }

exit:

    if (t_ctxt->xfer_state == TRANSFER_DONE)
    {
        t_ctxt->xfer_count++;
        if (SPI_SUCCESS(status) &&
            (t_ctxt->xfer_count < t_ctxt->num_descs))
        {
            //status = spi_iface_transfer(c_ctxt);
            // if this call returned an error, then the transfer_state is still
            // TRANSFER_DONE in which case the below condition will hit and the
            // process_transfer_completion will complete this transfer. do not
            // check the status here.
        }
    }

    //
    // DO NOT CLUB THIS WITH ABOVE CONDITION
    // spi_iface_transfer called above may change transfer_state
    //
    if ((t_ctxt->xfer_state == TRANSFER_DONE) ||
        (t_ctxt->xfer_state == TRANSFER_TIMED_OUT))
    {
        process_transfer_completion(h_ctxt, status);
    }

    spi_iface_enable_m_irqs(h_ctxt);
}

spi_status bus_iface_init (spi_hw_ctxt *h_ctxt)
{
    spi_plat_device_config *dcfg = (spi_plat_device_config *)h_ctxt->core_config;
    bus_iface_hw_ctxt *g_ctxt;
    uint8 *base_addr;

	g_ctxt = (bus_iface_hw_ctxt *)spi_plat_mem_alloc(dcfg->core_index,
        sizeof(bus_iface_hw_ctxt),
        CORE_IFACE_TYPE);

    if (g_ctxt == NULL)
    {
        SPI_LOG(LEVEL_ERROR,"bus_iface_init : could not allocate bus iface hw ctxt ");
		return SPI_ERROR_DMA_INSUFFICIENT_RESOURCES;
    }

    h_ctxt->core_iface = g_ctxt;

    base_addr = dcfg->core_base_addr + dcfg->core_offset;

	REG_OUT(dcfg->core_base_addr, QUPV3_SE_AHB_M_CFG,
    FLD_SET(QUPV3_SE_AHB_M_CFG, AHB_M_CLK_CGC_ON,       1));

    if(SPI_SUCCESS != spi_fw_load(dcfg))
    {
    	  SPI_LOG(LEVEL_ERROR,"bus_iface_init : could not load fw");
		  return SPI_ERROR_FW_LOAD_FALIURE;
	}
    // QUPv3 HPG. 3.1.7.1. DFS interface initialization
    // disable for RUMI
    /*REG_OUT(base_addr, SE_GENI_DFS_IF_CFG,
        FLD_SET(SE_GENI_DFS_IF_CFG, NUM_WAIT_STATES, 0) |
        FLD_SET(SE_GENI_DFS_IF_CFG, DFS_IF_EN, 0));
*/
    base_addr = dcfg->core_base_addr + dcfg->core_offset;
    // QUPv3 HPG. 2.1.1.1. QUPv3 HW parameters registers
    g_ctxt->rx_fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_1, RX_FIFO_DEPTH);
    g_ctxt->tx_fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_0, TX_FIFO_DEPTH);
/*
    // QUPv3 HPG. 3.1.7.5. SE IOs initialization
    REG_OUT(base_addr, SE_GENI_FORCE_DEFAULT_REG,
        FLD_SET(SE_GENI_FORCE_DEFAULT_REG, FORCE_DEFAULT, 1));

    REG_OUT(base_addr, SE_GENI_OUTPUT_CTRL,
        FLD_SET(SE_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x7f));
*/
    // QUPv3 HPG. 3.1.7.6. SE Mode initialization. FIFO Mode
    REG_OUT(base_addr, SE_GENI_DMA_MODE_EN,
        FLD_SET(SE_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0));

    REG_OUT(base_addr, SE_SE_IRQ_EN,
        FLD_SET(SE_SE_IRQ_EN, DMA_RX_IRQ_EN, 1) |
        FLD_SET(SE_SE_IRQ_EN, DMA_TX_IRQ_EN, 1) |
        FLD_SET(SE_SE_IRQ_EN, GENI_M_IRQ_EN, 1) |
        FLD_SET(SE_SE_IRQ_EN, GENI_S_IRQ_EN, 1));

    REG_OUT(base_addr, SE_SE_GSI_EVENT_EN,
        FLD_SET(SE_SE_GSI_EVENT_EN, DMA_RX_EVENT_EN, 0) |
        FLD_SET(SE_SE_GSI_EVENT_EN, DMA_TX_EVENT_EN, 0) |
        FLD_SET(SE_SE_GSI_EVENT_EN, GENI_M_EVENT_EN, 0) |
        FLD_SET(SE_SE_GSI_EVENT_EN, GENI_S_EVENT_EN, 0));

    // QUPv3 HPG. 3.1.7.7. Enable GENI interrupts. All Modes.
    REG_OUT(base_addr, SE_GENI_M_IRQ_ENABLE,
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_OVERRUN_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_ILLEGAL_CMD_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_FAILURE_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_CANCEL_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_ABORT_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_TIMESTAMP_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, IO_DATA_ASSERT_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, IO_DATA_DEASSERT_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_RD_ERR_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_WR_ERR_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_RD_ERR_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_WR_ERR_EN, 1));

    REG_OUT(base_addr, SE_GENI_S_IRQ_ENABLE,
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_OVERRUN_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_ILLEGAL_CMD_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_CANCEL_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_ABORT_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_0_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_1_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_2_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_3_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, RX_FIFO_WR_ERR_EN, 1) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, RX_FIFO_RD_ERR_EN, 1));

    // QUPv3 HPG. 3.1.7.7. Enable GENI interrupts. FIFO Mode.
    REG_OUT(base_addr, SE_GENI_M_IRQ_ENABLE,
        REG_IN(base_addr, SE_GENI_M_IRQ_ENABLE) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_WATERMARK_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_WATERMARK_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_LAST_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_DONE_EN, 1) |
        FLD_SET(SE_GENI_M_IRQ_ENABLE, SEC_IRQ_EN, 1));

    REG_OUT(base_addr, SE_GENI_S_IRQ_ENABLE,
        REG_IN(base_addr, SE_GENI_S_IRQ_ENABLE) |
        FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_DONE_EN, 1));

    return SPI_SUCCESS;
}

spi_status bus_iface_deinit(spi_hw_ctxt *h_ctxt)
{
    if (h_ctxt->core_iface != NULL)
    {
        spi_plat_mem_free(h_ctxt->core_iface, sizeof(bus_iface_hw_ctxt), CORE_IFACE_TYPE);
    }

    return SPI_SUCCESS;
}

spi_status bus_iface_queue_transfer (spi_client_ctxt *c_ctxt, boolean is_last)
{	
    spi_hw_ctxt         *h_ctxt = c_ctxt->h_ctxt;
    spi_xfer_ctxt       *t_ctxt = &c_ctxt->t_ctxt;
	
	if ((h_ctxt == NULL) || (t_ctxt == NULL) || (h_ctxt->core_config == NULL) ||
        (t_ctxt->config == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"bus_iface_queue_transfer : invalid parameters ");
		return SPI_ERROR_INVALID_PARAM;
    }
	
    spi_plat_device_config *dcfg = (spi_plat_device_config *)h_ctxt->core_config;
    spi_status          status = SPI_SUCCESS;

    status = bus_iface_transfer(c_ctxt, is_last);

    // polled mode logic
    if (SPI_SUCCESS(status) && dcfg->polled_mode)
    {
        uint32 timeout_us = POLL_TIME_DEFAULT_US;
        while ((t_ctxt->xfer_state != TRANSFER_DONE) && (timeout_us != 0))
        {
            bus_iface_isr(h_ctxt);
            spi_plat_delay_us(POLL_INTERVAL_US);
            timeout_us -= POLL_INTERVAL_US;
        }

        if ((timeout_us == 0) && (t_ctxt->xfer_state != TRANSFER_DONE))
        {
            status = SPI_ERROR_TRANSFER_TIMEOUT;
            t_ctxt->xfer_state = TRANSFER_TIMED_OUT;
            process_transfer_completion(h_ctxt, status);
        }
    }

    return status;
}
