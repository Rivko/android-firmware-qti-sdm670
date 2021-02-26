/**
    @file  i2c_iface.c
    @brief I2C driver implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/05/17   ska     Made changes to add cancel sequence for NACK (As per HPG)
 30/08/17   ska     Made changes to propogate proper error to client
 13/07/17   vg      Updated firmware API
 13/07/17   vg      Fixed KW issues
 31/05/17   vg      Copyright  updated 
=============================================================================*/

#include "i2c_drv.h"
#include <i2c_config.h>
#include "i2c_iface.h"
#include "i2c_plat.h"
#include "i2c_log.h"
#include "i2c_api.h"
#include "hw_io.h"
#include "i2c_fw_load.h"


#define BUS_IFACE_TCE_STATUS_BIT_NACK       (1 << 10)
#define BUS_IFACE_TCE_STATUS_BIT_ARB_LOST   (1 << 12)
#define BUS_IFACE_TCE_STATUS_BIT_BUS_ERROR  (1 << 13)

#define TX_PACKING_CFG0_VAL 0x7F8FE
#define TX_PACKING_CFG1_VAL 0xFFEFE
#define RX_PACKING_CFG0_VAL 0x7F8FE
#define RX_PACKING_CFG1_VAL 0xFFEFE

#define POLL_TIME_DEFAULT_US    409600
#define POLL_INTERVAL_US        5

static i2c_status i2c_handle_cancel_sequence(uint8 *base_addr , i2c_status i_status);

typedef struct hsr_setting {
    uint32 index;
    uint32 value;
} hsr_setting;

/* Values from Image txt*/
#define GENI_FW_REVISION		0x333
#define GENI_S_FW_REVISION		0x0
#define GENI_INIT_CFG_REVESION	0x2


typedef enum cmd_opcode
{
    CMD_INVALID                 = 0,
    CMD_I2C_WRITE               = 1,
    CMD_I2C_READ                = 2,
    CMD_I2C_WRITE_THEN_READ     = 3,
    CMD_I2C_ADDRESS_ONLY        = 4,
    CMD_I2C_BUS_CLEAR           = 6,
    CMD_I2C_STOP_ON_BUS         = 7,
    CMD_I3C_PRIVATE_WRITE       = 9,
    CMD_I3C_PRIVATE_READ        = 10,
    CMD_I3C_HDR_DDR_WRITE       = 11,
    CMD_I3C_HDR_DDR_READ        = 12,
    CMD_I3C_READ_IBI            = 15,
    CMD_I3C_BROADCAST_CCC_WRITE = 16,
    CMD_I3C_DIRECT_CCC_WRITE    = 17,
    CMD_I3C_DIRECT_CCC_READ     = 18,

} cmd_opcode;


void bus_iface_process_ibi(client_ctxt *c_ctxt)
{
}

void bus_iface_write_to_fifo (uint8 *base_addr, transfer_ctxt *t_ctxt)
{
    i2c_descriptor *dc = t_ctxt->desc + t_ctxt->transfer_count;
    uint32 transferred = t_ctxt->transferred;
    uint8 *buffer = dc->buffer + transferred;
    uint32 length = dc->length;
    uint32 word = 0, i = 0;
    uint32 tx_watermark = REG_INF(base_addr, SE_GENI_TX_WATERMARK_REG, TX_WATERMARK);
    uint32 tx_fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_0, TX_FIFO_DEPTH);
    uint32 bytes_to_write = (tx_fifo_depth << 2) - (tx_watermark << 2);

    if ((length - transferred) < bytes_to_write)
    {
        bytes_to_write = length - transferred;
        // if all data will be written, then disable watermark
        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, 0));
    }

    for (i = 0; i < bytes_to_write; i++)
    {
        if (i && ((i & 0x3) == 0))
        {
            REG_OUT(base_addr, SE_GENI_TX_FIFOn, word);
            word = 0;
        }
        word = word | (buffer[i] << ((i & 0x3) << 3));
    }
    REG_OUT(base_addr, SE_GENI_TX_FIFOn, word);
    t_ctxt->transferred += bytes_to_write;
}

void bus_iface_read_from_fifo (uint8 *base_addr, transfer_ctxt *t_ctxt)
{
    uint32 rx_fifo_status = REG_IN(base_addr, SE_GENI_RX_FIFO_STATUS);
    uint8  bytes_in_last_word = 0;
    uint32 bytes_to_read = FLD_GET(rx_fifo_status, SE_GENI_RX_FIFO_STATUS, RX_FIFO_WC) << 2;
    uint32 transferred = t_ctxt->transferred;
    i2c_descriptor *dc = t_ctxt->desc + t_ctxt->transfer_count;
    uint8 *buffer = dc->buffer + transferred;
    uint32 length = dc->length;
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
    t_ctxt->transferred += bytes_to_read;	
}

uint32 bus_iface_get_bytes_transferred (transfer_ctxt *t_ctxt)
{
    uint32 i, transferred = 0;
    i2c_descriptor *dc = t_ctxt->desc;
    
	if(t_ctxt->transfer_count == 0)
	{
	  return 0;
	}
	
    for (i = 0; i < (t_ctxt->transfer_count - 1); i++)
    {
        transferred += dc->length;
        dc++;
    }

    return (transferred + t_ctxt->transferred);
}

plat_clock_config *bus_iface_get_clock_config (plat_device_config *dcfg, uint32 bus_frequency_khz)
{
    uint8 i = 0;
    plat_clock_config *clk_cfg = dcfg->clock_config;

    while (clk_cfg[i].bus_speed_khz != 0)
    {
        if (clk_cfg[i].bus_speed_khz == bus_frequency_khz) { break; }
        i++;
    }

    if (clk_cfg[i].bus_speed_khz == bus_frequency_khz)
    {
        return (clk_cfg + i);
    }

    return NULL;
}

cmd_opcode bus_iface_get_opcode (uint32 t_flags, uint32 length, i2c_mode mode)
{
    switch (mode)
    {
        case SMBUS: 
        case I2C: 
            if (length == 0)
            {
                if ((t_flags & I2C_FLAG_START) == I2C_FLAG_START)
                {
                    return CMD_I2C_ADDRESS_ONLY;
                }
                else if ((t_flags & I2C_FLAG_STOP) == I2C_FLAG_STOP)
                {
                    return CMD_I2C_STOP_ON_BUS;
                }
                else if (t_flags == 0)
                {
                    return CMD_I2C_BUS_CLEAR;
                }
            }
            else
            {
                if (t_flags & I2C_FLAG_WRITE) { return CMD_I2C_WRITE; }
                if (t_flags & I2C_FLAG_READ ) { return CMD_I2C_READ; }
            }
            break;

        case I3C_SDR:
            if (t_flags & I2C_FLAG_WRITE) { return CMD_I3C_PRIVATE_WRITE; }
            if (t_flags & I2C_FLAG_READ ) { return CMD_I3C_PRIVATE_READ; }
            break;

        case I3C_HDR_DDR:
            if (t_flags & I2C_FLAG_WRITE) { return CMD_I3C_HDR_DDR_WRITE; }
            if (t_flags & I2C_FLAG_READ ) { return CMD_I3C_HDR_DDR_READ; }
            break;

        case I3C_BROADCAST_CCC:
            return CMD_I3C_BROADCAST_CCC_WRITE;
            break;

        case I3C_DIRECT_CCC:
            if (t_flags & I2C_FLAG_WRITE) { return CMD_I3C_DIRECT_CCC_WRITE; }
            if (t_flags & I2C_FLAG_READ ) { return CMD_I3C_DIRECT_CCC_READ; }
            break;

        case I3C_IBI_READ:
            return CMD_I3C_READ_IBI;
            break;

        default:
            return CMD_INVALID;
    }

    return 0;
}

void bus_iface_config (uint8 *base_addr, boolean in_i2c_mode, plat_clock_config *clk_cfg)
{
    // dfs index
    REG_OUT(base_addr, SE_GENI_CLK_SEL,
            FLD_SET(SE_GENI_CLK_SEL, CLK_SEL, 0));

    // packing
    REG_OUT(base_addr, SE_GENI_TX_PACKING_CFG0, TX_PACKING_CFG0_VAL);
    REG_OUT(base_addr, SE_GENI_TX_PACKING_CFG1, TX_PACKING_CFG1_VAL);
    REG_OUT(base_addr, SE_GENI_RX_PACKING_CFG0, RX_PACKING_CFG0_VAL);
    REG_OUT(base_addr, SE_GENI_RX_PACKING_CFG1, RX_PACKING_CFG1_VAL);

    // clk divider
    REG_OUT(base_addr, SE_GENI_SER_M_CLK_CFG,
            FLD_SET(SE_GENI_SER_M_CLK_CFG, CLK_DIV_VALUE,   clk_cfg->clk_div) |
            FLD_SET(SE_GENI_SER_M_CLK_CFG, SER_CLK_EN,      1));

    // scl waveform conditioning
    if (in_i2c_mode)
    {
        REG_OUT(base_addr, SE_I2C_SCL_COUNTERS,
                FLD_SET(SE_I2C_SCL_COUNTERS, I2C_SCL_HIGH_COUNTER,  clk_cfg->t_high) |
                FLD_SET(SE_I2C_SCL_COUNTERS, I2C_SCL_LOW_COUNTER,   clk_cfg->t_low)  |
                FLD_SET(SE_I2C_SCL_COUNTERS, I2C_SCL_CYCLE_COUNTER, clk_cfg->t_cycle));
    }
    else
    {
        REG_OUT(base_addr, SE_I3C_SCL_LOW,
                FLD_SET(SE_I3C_SCL_LOW, I3C_SCL_LOW_COUNTER, clk_cfg->t_low));
    }
}

i2c_status bus_iface_transfer (client_ctxt *c_ctxt)
{
    hw_ctxt             *h_ctxt;
    bus_iface_hw_ctxt   *g_ctxt;
    transfer_ctxt       *t_ctxt;
    plat_device_config  *dcfg;
    plat_clock_config   *clk_cfg;
    i2c_descriptor      *dc;
    uint8               *base_addr;
    uint32              params = 0;
    cmd_opcode          opcode = CMD_INVALID;
    boolean             in_i2c_mode = TRUE;

    if ((c_ctxt         == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        return I2C_ERROR_INVALID_PARAMETER;
    }

    h_ctxt = c_ctxt->h_ctxt;
    g_ctxt = h_ctxt->core_iface;
    t_ctxt = &c_ctxt->t_ctxt;
    dcfg   = (plat_device_config *) h_ctxt->core_config;
    dc     = t_ctxt->desc + t_ctxt->transfer_count;

    if ((dcfg           == NULL) ||
        (t_ctxt         == NULL) ||
        (t_ctxt->config == NULL) ||
        (dc             == NULL))
    {
        return I2C_ERROR_INVALID_PARAMETER;
    }

    t_ctxt->transferred = 0;
    clk_cfg = bus_iface_get_clock_config (dcfg, t_ctxt->config->bus_frequency_khz);
    if (clk_cfg == NULL)
    {
        return I2C_ERROR_PLATFORM_GET_CLOCK_CONFIG_FAIL;
    }

    base_addr = dcfg->core_base_addr + dcfg->core_offset;
    in_i2c_mode = ((t_ctxt->config->mode == I2C) || (t_ctxt->config->mode == SMBUS));

    bus_iface_config (base_addr, in_i2c_mode, clk_cfg);
    
    opcode = bus_iface_get_opcode (dc->flags, dc->length, t_ctxt->config->mode);
    if (opcode == CMD_INVALID)
    {
        return I2C_ERROR_INVALID_CMD_OPCODE;
    }

    if (dc->flags & I2C_FLAG_READ)
    {

        if (in_i2c_mode)
        {
            REG_OUT(base_addr, SE_I2C_RX_TRANS_LEN, dc->length);
        }
        else
        {
            REG_OUT(base_addr, SE_I3C_RX_TRANS_LEN, dc->length);
        }

        if ((t_ctxt->transfer_count < (t_ctxt->num_descs - 1)) &&
            (!(dc->flags & I2C_FLAG_STOP)))
        {
            i2c_descriptor *dc_next = dc + 1;
            if ((dc_next->flags & I2C_FLAG_READ) &&
               !(dc_next->flags & I2C_FLAG_START))
            {
                params = params | FLD_SET(SE_GENI_M_CMD0, I2C_READ_FINISH_WITH_ACK, 1);
            }
        }
    }
    else if (dc->flags & I2C_FLAG_WRITE)
    {
        if (in_i2c_mode)
        {
            REG_OUT(base_addr, SE_I2C_TX_TRANS_LEN, dc->length);
        }
        else
        {
            REG_OUT(base_addr, SE_I3C_TX_TRANS_LEN, dc->length);
        }
    }

    params = params | FLD_SET(SE_GENI_M_CMD0,
                             I2C_SLAVE_ADDRESS,
                             t_ctxt->config->slave_address);

    if ((t_ctxt->delay) && (t_ctxt->transfer_count == 0))
    {
        uint32 delay = (t_ctxt->delay * clk_cfg->se_clock_frequency_khz) / 1000;
        if (in_i2c_mode)
        {
            REG_OUT(base_addr, SE_I2C_DELAY_COUNTER, delay);
        }
        else
        {
            REG_OUT(base_addr, SE_I3C_DELAY_COUNTER, delay);
        }
        params = params | FLD_SET(SE_GENI_M_CMD0, I2C_PRE_CMD_DELAY, 1);
    }

    if (dc->flags & I2C_FLAG_TIMESTAMP)
    {
        if (dc->flags & I2C_FLAG_START)
        {
            params = params | FLD_SET(SE_GENI_M_CMD0, I2C_TIMESTAMP_BEFORE, 1);
        }
        else if (dc->flags & I2C_FLAG_STOP)
        {
            params = params | FLD_SET(SE_GENI_M_CMD0, I2C_TIMESTAMP_AFTER, 1);
        }
    }

    if (dc->length != 0)
    {
        if (!(dc->flags & I2C_FLAG_STOP))
        {
            params = params | FLD_SET(SE_GENI_M_CMD0, I2C_STRETCH, 1);
        }

        if (!(dc->flags & I2C_FLAG_START))
        {
            params = params | FLD_SET(SE_GENI_M_CMD0, I2C_BYPASS_ADDRESS_PHASE, 1);
        }
    }

    if (!in_i2c_mode)
    {
        REG_OUT(base_addr, SE_I3C_TRANS_CFG,
            FLD_SET(SE_I3C_TRANS_CFG, I3C_USE_7E,        1) |
            FLD_SET(SE_I3C_TRANS_CFG, I3C_IBI_NACK_CTRL, 0));
    }

    REG_OUT(base_addr, SE_GENI_M_CMD0,
            FLD_SET(SE_GENI_M_CMD0, OPCODE, opcode) |
            FLD_SET(SE_GENI_M_CMD0, PARAM,  params));

    t_ctxt->transfer_state = TRANSFER_IN_PROGRESS;

    if (opcode == CMD_I2C_WRITE)
    {
        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, 1));
    }
    else if (opcode == CMD_I2C_READ)
    {
    /*    REG_OUT(base_addr, SE_GENI_RX_WATERMARK_REG,
            FLD_SET(SE_GENI_RX_WATERMARK_REG, RX_WATERMARK, g_ctxt->rx_fifo_depth - 2));  */
    }

    return I2C_SUCCESS;
}

void process_transfer_completion (hw_ctxt *h_ctxt, i2c_status i_status)
{
    client_ctxt *temp;
    transfer_ctxt *t_ctxt;
    plat_device_config *dcfg = (plat_device_config *) h_ctxt->core_config;

    do
    {
        plat_mutex_instance_lock (h_ctxt->queue_lock);
        // remove the client from the queue
        temp = h_ctxt->client_ctxt_head;
        h_ctxt->client_ctxt_head = temp->next;
        temp->next = NULL;

        // get transfer context
        t_ctxt = &(temp->t_ctxt);

        // store next client ctxt in temp
        temp = h_ctxt->client_ctxt_head;
        plat_mutex_instance_unlock (h_ctxt->queue_lock);

        t_ctxt->transfer_status = i_status;
        t_ctxt->transferred = bus_iface_get_bytes_transferred (t_ctxt);

        if (t_ctxt->callback != NULL)
        {
          /*
            // should be implemented in a qdi abstraction
            i2c_notify_completion(t_ctxt->callback,
                                  t_ctxt->transfer_status,
                                  t_ctxt->transferred,
                                  t_ctxt->ctxt);*/
        }
        else if (dcfg->polled_mode == FALSE)
        {
            // for synchronous calls
            plat_signal_event(t_ctxt->signal);
        }

        // check if more clients are queued
        if (temp != NULL)
        {
            i_status = bus_iface_transfer (temp);
            if (I2C_SUCCESS(i_status))
            {
                t_ctxt = &(temp->t_ctxt);
                t_ctxt->transfer_state = TRANSFER_IN_PROGRESS;
                break;
            }
        }
    }
    while (temp != NULL);
}

void bus_iface_isr (void *int_handle)
{
    hw_ctxt             *h_ctxt = (hw_ctxt *) int_handle;
    bus_iface_hw_ctxt   *g_ctxt;
    client_ctxt         *c_ctxt = NULL;
    transfer_ctxt       *t_ctxt;
    i2c_status           i_status = I2C_SUCCESS;
    plat_device_config  *dcfg;
    i2c_descriptor      *dc;
    uint8               *base_addr;
    uint32               irq_status;

    if ((h_ctxt                     == NULL)    ||
        (h_ctxt->core_config        == NULL)    ||
        (h_ctxt->core_iface         == NULL))
    {
        return;
    }

    plat_mutex_instance_lock (h_ctxt->queue_lock);
    if (h_ctxt->client_ctxt_head != NULL)
    {
        c_ctxt = h_ctxt->client_ctxt_head;
    }
    plat_mutex_instance_unlock (h_ctxt->queue_lock);
    if (c_ctxt == NULL)
    {
        return;
    }

    g_ctxt = h_ctxt->core_iface;
    t_ctxt = &c_ctxt->t_ctxt;
    dc = t_ctxt->desc + t_ctxt->transfer_count;
    dcfg = (plat_device_config *) h_ctxt->core_config;
    base_addr = dcfg->core_base_addr + dcfg->core_offset;
	
    irq_status = REG_IN(base_addr, SE_GENI_M_IRQ_STATUS);

    if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_GP_IRQ_1))
    {
        i_status = I2C_ERROR_ADDR_NACK;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_GP_IRQ_3))
    {
        i_status = I2C_ERROR_START_STOP_UNEXPECTED;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_GP_IRQ_4))
    {
        i_status = I2C_ERROR_ARBITRATION_LOST;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_RD_ERR))
    {
        i_status = I2C_ERROR_INPUT_FIFO_UNDER_RUN;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_WR_ERR))
    {
        i_status = I2C_ERROR_INPUT_FIFO_OVER_RUN;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_RD_ERR))
    {
        i_status = I2C_ERROR_OUTPUT_FIFO_UNDER_RUN;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_WR_ERR))
    {
        i_status = I2C_ERROR_OUTPUT_FIFO_OVER_RUN;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_OVERRUN))
    {
        i_status = I2C_ERROR_COMMAND_OVER_RUN;
    }
    else if ((FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_CANCEL)) ||
             (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_ABORT)))
    {
        i_status = I2C_TRANSFER_FORCE_TERMINATED;
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_GP_SYNC_IRQ_0))
    {
        bus_iface_process_ibi(c_ctxt);
    }

    if (i_status != I2C_SUCCESS)
    {
        REG_OUT(base_addr, SE_GENI_TX_WATERMARK_REG,
            FLD_SET(SE_GENI_TX_WATERMARK_REG, TX_WATERMARK, 0));
        t_ctxt->transfer_state = TRANSFER_DONE;
		i_status = i2c_handle_cancel_sequence(base_addr, i_status);
		irq_status = REG_IN(base_addr, SE_GENI_M_IRQ_STATUS);
        goto exit;
    }

    if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_TIMESTAMP))
    {
        t_ctxt->start_bit_timestamp = REG_IN(base_addr, SE_GENI_TIMESTAMP);
        t_ctxt->stop_bit_timestamp  = t_ctxt->start_bit_timestamp;
    }

    if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, M_CMD_DONE))
    {
        t_ctxt->transfer_state = TRANSFER_DONE;
        if (dc->flags & I2C_FLAG_READ)
        {
            bus_iface_read_from_fifo (base_addr, t_ctxt);
        }
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, RX_FIFO_WATERMARK))
    {
        bus_iface_read_from_fifo (base_addr, t_ctxt);
    }
    else if (FLD_GET(irq_status, SE_GENI_M_IRQ_STATUS, TX_FIFO_WATERMARK))
    {
        bus_iface_write_to_fifo (base_addr, t_ctxt);
    }

exit:
    REG_OUT(base_addr, SE_GENI_M_IRQ_CLEAR, irq_status);

    if (t_ctxt->transfer_state == TRANSFER_DONE)
    {
        t_ctxt->transfer_count++;
        if (I2C_SUCCESS(i_status) &&
           (t_ctxt->transfer_count < t_ctxt->num_descs))
        {
            i_status = bus_iface_transfer (c_ctxt);
            // if this call returned an error, then the transfer_state is still
            // TRANSFER_DONE in which case the below condition will hit and the
            // process_transfer_completion will complete this transfer. do not
            // check the i_status here.
        }
    }

    //
    // DO NOT CLUB THIS WITH ABOVE CONDITION
    // bus_iface_transfer called above may change transfer_state
    // 
    if ((t_ctxt->transfer_state == TRANSFER_DONE) ||
        (t_ctxt->transfer_state == TRANSFER_TIMED_OUT))
    {
        process_transfer_completion (h_ctxt, i_status);
    }
}
static i2c_status i2c_handle_cancel_sequence(uint8 *base_addr , i2c_status i_status)
{
	uint32 retry_count = 50;
    
	REG_OUT(base_addr, SE_GENI_M_CMD_CTRL_REG, FLD_SET(SE_GENI_M_CMD_CTRL_REG, M_GENI_CMD_CANCEL, 1));
	while((FLD_GET(REG_IN(base_addr, SE_GENI_M_IRQ_STATUS), SE_GENI_M_IRQ_STATUS, M_CMD_CANCEL) != 1) && retry_count != 0)
	{
		plat_delay_us(100); //delay is in us
		retry_count--;
	}
	if(retry_count == 0)
	{
		//since cancel transfer timed out do abort as per HPG
		retry_count = 50;
		REG_OUT(base_addr, SE_GENI_M_CMD_CTRL_REG, FLD_SET(SE_GENI_M_CMD_CTRL_REG, M_GENI_CMD_ABORT, 1));
		while((FLD_GET(REG_IN(base_addr, SE_GENI_M_IRQ_STATUS), SE_GENI_M_IRQ_STATUS, M_CMD_ABORT ) != 1) && retry_count != 0)
	    {
		   plat_delay_us(100); //delay is in us
		   retry_count--;
	    }
		if(retry_count == 0)
		{
			i_status = I2C_TRANSFER_FORCE_TERMINATED;
		}
		REG_OUT(base_addr, SE_GENI_FORCE_DEFAULT_REG, FLD_SET(SE_GENI_FORCE_DEFAULT_REG, FORCE_DEFAULT, 1));
	}
	return i_status;
}

i2c_status bus_iface_init (hw_ctxt *h_ctxt)
{
    plat_device_config *dcfg = NULL;
    bus_iface_hw_ctxt *g_ctxt;
    uint8 *base_addr;

	if ((h_ctxt == NULL) || (h_ctxt->core_config == NULL))
    {
        return I2C_ERROR_INVALID_PARAMETER;
    }

	dcfg = (plat_device_config *) h_ctxt->core_config;

    g_ctxt = (bus_iface_hw_ctxt *) plat_mem_alloc (dcfg->core_index,
                                                   sizeof(bus_iface_hw_ctxt),
                                                   CORE_IFACE_TYPE);
    if (g_ctxt == NULL)
    {
        return I2C_ERROR_INVALID_PARAMETER;
    }

    h_ctxt->core_iface = g_ctxt;


    base_addr = dcfg->core_base_addr + dcfg->core_offset;

	REG_OUT(dcfg->core_base_addr, QUPV3_SE_AHB_M_CFG,
    FLD_SET(QUPV3_SE_AHB_M_CFG, AHB_M_CLK_CGC_ON,       1));

	if(I2C_SUCCESS != i2c_fw_load(dcfg))
	{
      return I2C_ERROR_FW_LOAD_FALIURE;
	}
		  
    // diable for RUMI
    // REG_OUT(base_addr, SE_GENI_DFS_IF_CFG,
    //         FLD_SET(SE_GENI_DFS_IF_CFG, NUM_WAIT_STATES, 0) |
    //         FLD_SET(SE_GENI_DFS_IF_CFG, DFS_IF_EN,       1));

 /*   REG_OUT(dcfg->core_base_addr, QUPV3_SE_AHB_M_CFG,
            FLD_SET(QUPV3_SE_AHB_M_CFG, AHB_M_CLK_CGC_ON,       1));

	REG_OUT(base_addr, SE_GENI_DMA_MODE_EN,
            FLD_SET(SE_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0));

    REG_OUT(base_addr, SE_DMA_GENERAL_CFG,
            FLD_SET(SE_DMA_GENERAL_CFG, AHB_SEC_SLV_CLK_CGC_ON, 1) |
            FLD_SET(SE_DMA_GENERAL_CFG, DMA_AHB_SLV_CLK_CGC_ON, 1) |
            FLD_SET(SE_DMA_GENERAL_CFG, DMA_TX_CLK_CGC_ON,      1) |
            FLD_SET(SE_DMA_GENERAL_CFG, DMA_RX_CLK_CGC_ON,      1));

    REG_OUT(base_addr, SE_GENI_CGC_CTRL,
            FLD_SET(SE_GENI_CGC_CTRL, EXT_CLK_CGC_ON,           1) |
            FLD_SET(SE_GENI_CGC_CTRL, RX_CLK_CGC_ON,            1) |
            FLD_SET(SE_GENI_CGC_CTRL, TX_CLK_CGC_ON,            1) |
            FLD_SET(SE_GENI_CGC_CTRL, SCLK_CGC_ON,              1) |
            FLD_SET(SE_GENI_CGC_CTRL, DATA_AHB_CLK_CGC_ON,      1) |
            FLD_SET(SE_GENI_CGC_CTRL, CFG_AHB_WR_CLK_CGC_ON,    1) |
            FLD_SET(SE_GENI_CGC_CTRL, CFG_AHB_CLK_CGC_ON,       1));*/
	
    g_ctxt->tx_fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_0, TX_FIFO_DEPTH);
    g_ctxt->rx_fifo_depth = REG_INF(base_addr, SE_SE_HW_PARAM_1, RX_FIFO_DEPTH);

    REG_OUT(base_addr, SE_GENI_RX_RFR_WATERMARK_REG,
            FLD_SET(SE_GENI_RX_RFR_WATERMARK_REG, RX_RFR_WATERMARK, (g_ctxt->rx_fifo_depth - 2)));
    REG_OUT(base_addr, SE_GENI_RX_WATERMARK_REG,
            FLD_SET(SE_GENI_RX_WATERMARK_REG, RX_WATERMARK, 
                   ((REG_INF(base_addr, SE_GENI_RX_RFR_WATERMARK_REG, RX_RFR_WATERMARK)) - 1)));

  /*  REG_OUT(base_addr, SE_GENI_FORCE_DEFAULT_REG,
            FLD_SET(SE_GENI_FORCE_DEFAULT_REG, FORCE_DEFAULT, 1));

    REG_OUT(base_addr, SE_GENI_OUTPUT_CTRL,
            FLD_SET(SE_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x7f));

    REG_OUT(base_addr, SE_GENI_DMA_MODE_EN,
            FLD_SET(SE_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0)); */

	/* For FIFO Polled mode*/
    REG_OUT(base_addr, SE_SE_IRQ_EN,
            FLD_SET(SE_SE_IRQ_EN, DMA_RX_IRQ_EN, 0) |
            FLD_SET(SE_SE_IRQ_EN, DMA_TX_IRQ_EN, 0) |
            FLD_SET(SE_SE_IRQ_EN, GENI_M_IRQ_EN, 0) |
            FLD_SET(SE_SE_IRQ_EN, GENI_S_IRQ_EN, 0));

    REG_OUT(base_addr, SE_SE_GSI_EVENT_EN,
            FLD_SET(SE_SE_GSI_EVENT_EN, DMA_RX_EVENT_EN, 0) |
            FLD_SET(SE_SE_GSI_EVENT_EN, DMA_TX_EVENT_EN, 0) |
            FLD_SET(SE_SE_GSI_EVENT_EN, GENI_M_EVENT_EN, 0) |
            FLD_SET(SE_SE_GSI_EVENT_EN, GENI_S_EVENT_EN, 0));

	if(!dcfg->polled_mode)
	{
	    REG_OUT(base_addr, SE_GENI_M_IRQ_ENABLE,
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_OVERRUN_EN,     1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_ILLEGAL_CMD_EN,     1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_FAILURE_EN,     1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_CANCEL_EN,      1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_ABORT_EN,       1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_TIMESTAMP_EN,       1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, IO_DATA_ASSERT_EN,    1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, IO_DATA_DEASSERT_EN,  1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_RD_ERR_EN,    1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_WR_ERR_EN,    1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_RD_ERR_EN,    1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_WR_ERR_EN,    1));

	    REG_OUT(base_addr, SE_GENI_S_IRQ_ENABLE,
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_OVERRUN_EN,     1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_ILLEGAL_CMD_EN,     1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_CANCEL_EN,      1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_ABORT_EN,       1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_0_EN,        1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_1_EN,        1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_2_EN,        1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_GP_IRQ_3_EN,        1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, RX_FIFO_WR_ERR_EN,    1) |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, RX_FIFO_RD_ERR_EN,    1));

	    REG_OUT(base_addr, SE_GENI_M_IRQ_ENABLE,
	            REG_IN(base_addr, SE_GENI_M_IRQ_ENABLE)                |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_WATERMARK_EN, 1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, TX_FIFO_WATERMARK_EN, 1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, RX_FIFO_LAST_EN,      1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, M_CMD_DONE_EN,        1) |
	            FLD_SET(SE_GENI_M_IRQ_ENABLE, SEC_IRQ_EN,           1));

	    REG_OUT(base_addr, SE_GENI_S_IRQ_ENABLE,
	            REG_IN(base_addr, SE_GENI_S_IRQ_ENABLE)                |
	            FLD_SET(SE_GENI_S_IRQ_ENABLE, S_CMD_DONE_EN, 1));

	}

    return I2C_SUCCESS;
}

i2c_status bus_iface_de_init (hw_ctxt *h_ctxt)
{
    if (h_ctxt->core_iface != NULL)
    {
        plat_mem_free (h_ctxt->core_iface, sizeof(bus_iface_hw_ctxt), CORE_IFACE_TYPE);
    }

    return I2C_SUCCESS;
}

i2c_status bus_iface_queue_transfer (client_ctxt *c_ctxt)
{
    hw_ctxt             *h_ctxt  = c_ctxt->h_ctxt;
    transfer_ctxt       *t_ctxt  = &c_ctxt->t_ctxt;
    plat_device_config  *dcfg    = NULL;
    i2c_status          i_status = I2C_SUCCESS;

    if ((h_ctxt == NULL) || (t_ctxt->config == NULL))
    {
        return I2C_ERROR_INVALID_PARAMETER;
    }
    
	dcfg    = (plat_device_config *) h_ctxt->core_config;
	
    i_status = bus_iface_transfer (c_ctxt);

    // polled mode logic
    if (I2C_SUCCESS(i_status) && dcfg->polled_mode)
    {
        uint32 timeout_us = POLL_TIME_DEFAULT_US;
        while ((t_ctxt->transfer_state != TRANSFER_DONE) && (timeout_us != 0))
        {
            bus_iface_isr (h_ctxt);
            plat_delay_us (POLL_INTERVAL_US);
            timeout_us -= POLL_INTERVAL_US;
        }

        if ((timeout_us == 0) && (t_ctxt->transfer_state != TRANSFER_DONE))
        {
            i_status = I2C_ERROR_TRANSFER_TIMEOUT;
            t_ctxt->transfer_state = TRANSFER_TIMED_OUT;
            process_transfer_completion (h_ctxt, i_status);
        }
		else
		{
			i_status = t_ctxt->transfer_status;
		}
			
    }

    return i_status;
}

boolean bus_iface_has_queueing(hw_ctxt *h_ctxt)
{
	return TRUE;

}

boolean bus_iface_has_interrupt_consumer(hw_ctxt *h_ctxt)
{
    return FALSE;
}
