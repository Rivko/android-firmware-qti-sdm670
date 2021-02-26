/**
    @file  i2c_iface.c
    @brief I2C driver implementation
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_drv.h"
#include "i2c_iface.h"
#include "i2c_plat.h"
#include "i2c_log.h"
#include "qup_common.h"

#define ADVANCE_TRE(dir, count, num_avail_tre, tre, base_tre)       \
    if ((count) > num_avail_tre)                                    \
    {                                                               \
        return I2C_ERROR_DMA_INSUFFICIENT_RESOURCES;                \
    }                                                               \
    else                                                            \
    {                                                               \
        (count)++;                                                  \
        (tre)++;                                                    \
        if ((base_tre + MAX_##dir##_TRE_LIST_SIZE_PER_CORE) == tre) \
        {                                                           \
            tre = base_tre;                                         \
        }                                                           \
    }

#define QN_GP_IRQ_OFFSET    9
#define TCE_GP_IRQ_OFFSET   5
#define GP_IRQ(x,offset) ((x >> offset) & 0x3f)

#define STATUS_BIT_NACK       (1 << 1)
#define STATUS_BIT_ARB_LOST   (1 << 3)
#define STATUS_BIT_BUS_ERROR  (1 << 4)
#define STATUS_BIT_TIMESTAMP  (1 << 6)

#define STATUS_BIT_CMD_CANCEL (1 << 4)
#define STATUS_BIT_CMD_DONE   (1 << 0)

#define POLL_TIME_DEFAULT_US    409600
#define POLL_INTERVAL_US        10

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

extern void i2c_notify_completion (callback_func func, uint32 status, uint32 transferred, void *context);
i2c_status bus_iface_queue_transfer (client_ctxt *c_ctxt);
boolean bus_iface_issue_cmd (hw_ctxt           *h_ctxt,
                             GPI_CHAN_CMD       gpi_chan_cmd,
                             GPI_CHAN_STATE     gpi_exp_state,
                             GPI_CHAN_TYPE      gpi_chan_type,
                             boolean            asynchronous);

uint32 bus_iface_get_bytes_transferred (transfer_ctxt *t_ctxt)
{
    i2c_descriptor *dc = t_ctxt->desc;
    int8 i = 0;
    uint32 dc_transferred = 0;
    uint32 length = t_ctxt->transferred;

    while ((i < t_ctxt->num_descs) && (length > 0))
    {
        if (length > dc->length)
        {
            dc_transferred = dc->length;
        }
        else
        {
            dc_transferred = length;
        }
        length -= dc_transferred;

        if (dc->flags & I2C_FLAG_READ)
        {
            plat_mem_invalidate_cache (dc->buffer, dc_transferred, ATTRIB_BUFFER);
        }
        i++;
        dc++;
    }

    return t_ctxt->transferred;
}

void process_transfer_completion (hw_ctxt *h_ctxt, i2c_status i_status, transfer_state t_state)
{
    client_ctxt         *temp;
    transfer_ctxt       *t_ctxt;
    plat_device_config  *dcfg = (plat_device_config *) h_ctxt->core_config;

    boolean has_internal_queueing = bus_iface_has_queueing(h_ctxt);

    do
    {
        plat_mutex_instance_lock (h_ctxt->queue_lock, EL_THREAD, SYNC_WITH_THREAD);

        // remove the client from the queue
        temp = h_ctxt->client_ctxt_head;
        if (temp == NULL)
        {
            I2C_LOG(LEVEL_INFO, "NOTE: client_ctxt_head already completed");
            plat_mutex_instance_unlock (h_ctxt->queue_lock, EL_THREAD, SYNC_WITH_THREAD);
            return;
        }
        h_ctxt->client_ctxt_head = temp->next;
        temp->next = NULL;

        // get transfer context
        t_ctxt = &(temp->t_ctxt);

        // store next client ctxt in temp
        temp = h_ctxt->client_ctxt_head;

        plat_mutex_instance_unlock (h_ctxt->queue_lock, EL_THREAD, SYNC_WITH_THREAD);

        if (t_ctxt->transfer_status == I2C_SUCCESS)
        {
            t_ctxt->transfer_status = i_status;
        }
        else
        {
            i_status = t_ctxt->transfer_status;
        }
        t_ctxt->transferred = bus_iface_get_bytes_transferred (t_ctxt);

        // this has to be done in the end to avoid race between polling thread
        // and gpi thread
        t_ctxt->transfer_state = t_state;

        if (t_ctxt->callback != NULL)
        {
            // should be implemented in a qdi abstraction
            i2c_notify_completion(t_ctxt->callback,
                                  t_ctxt->transfer_status,
                                  t_ctxt->transferred,
                                  t_ctxt->ctxt);
        }
        else if (dcfg->polled_mode == FALSE)
        {
            I2C_LOG(LEVEL_VERBOSE, "signal completion 0x%08x", t_ctxt->signal);
            // for synchronous calls
            plat_signal_event(t_ctxt->signal);
        }

        // check if more clients are queued
        if ((temp != NULL) && (has_internal_queueing == FALSE))
        {
            i_status = bus_iface_queue_transfer (temp);
            if (I2C_SUCCESS(i_status))
            {
                t_ctxt = &(temp->t_ctxt);
                t_ctxt->transfer_state = TRANSFER_IN_PROGRESS;
                break;
            }
        }
    }
    while ((temp != NULL) && (has_internal_queueing == FALSE));
}

void noop_tres (bus_iface_hw_ctxt *g_ctxt, bus_iface_io_ctxt *io_ctxt)
{
    uint32 i;

    gpi_ring_elem *tx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[0].ring_base_va;
    gpi_ring_elem *rx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[1].ring_base_va;
    
    gpi_ring_elem *tx_tre = (&(io_ctxt->tx_tre_req))->tre_list;
    gpi_ring_elem *rx_tre = (&(io_ctxt->rx_tre_req))->tre_list;

    uint32 tx_num_tre = (io_ctxt->tx_tre_req).num_tre;
    uint32 rx_num_tre = (io_ctxt->rx_tre_req).num_tre;

    // noop TX
    for (i = 0; i < (tx_num_tre - 1); i++)
    {
        tx_tre->ctrl = GPI_BUILD_TRE_CTRL(TRE_NOOP_MAJOR, TRE_NOOP_MINOR, 0, 0, 0, 0);
        tx_tre++;
        if ((tx_base_tre + MAX_TX_TRE_LIST_SIZE_PER_CORE) == tx_tre) { tx_tre = tx_base_tre; }
    }
    I2C_LOG(LEVEL_VERBOSE, "TX noop");

    // noop RX
    for (i = 0; i < rx_num_tre; i++)
    {
        rx_tre->ctrl = GPI_BUILD_TRE_CTRL(TRE_NOOP_MAJOR, TRE_NOOP_MINOR, 0, 0, 0, 0);
        rx_tre++;
        if ((rx_base_tre + MAX_RX_TRE_LIST_SIZE_PER_CORE) == rx_tre) { rx_tre = rx_base_tre; }
    }
    I2C_LOG(LEVEL_VERBOSE, "RX noop");
}

i2c_status bus_iface_cancel (hw_ctxt *h_ctxt, client_ctxt *c_ctxt)
{
    transfer_ctxt       *t_ctxt  = (transfer_ctxt *)     &c_ctxt->t_ctxt;
    bus_iface_io_ctxt   *io_ctxt = (bus_iface_io_ctxt *) t_ctxt->io_ctxt;

    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_STOP, GPI_CHAN_STOPPED, GPI_OUTBOUND_CHAN, TRUE))
    { return I2C_ERROR_DMA_TX_CHAN_STOP_FAIL; }
    io_ctxt->clean_cmd |= CLEAN_TX_CHANNEL;
    io_ctxt->clean_cmd |= CLEAN_RX_CHANNEL;
    I2C_LOG(LEVEL_INFO, "cancelling TX");
    return I2C_SUCCESS;
}

void bus_iface_callback (gpi_result_type *result)
{
    hw_ctxt             *h_ctxt  = NULL;
    client_ctxt         *c_ctxt  = NULL;
    transfer_ctxt       *t_ctxt  = NULL;
    bus_iface_hw_ctxt   *g_ctxt  = NULL;
    bus_iface_io_ctxt   *io_ctxt = NULL;
    plat_device_config  *dcfg    = NULL;
    transfer_state      t_state = TRANSFER_INVALID;

    i2c_status          i_status = I2C_SUCCESS;
    uint32              length;

    I2C_LOG(LEVEL_VERBOSE, "result 0x%08x", result);

    if (result == NULL) { return; }

    I2C_LOG(LEVEL_PERF, I2C_IFACE_CALLBACK, result->type, result->code);

    length = result->length;

    I2C_LOG(LEVEL_INFO, "chan %d: type %d: code %d: length %d: tre_idx %d: dword_0 0x%08x: dword_1 0x%08x: status 0x%08x: user_data 0x%08x",
                        result->chan,
                        result->type,
                        result->code,
                        result->length,
                        result->tre_idx,
                        result->dword_0,
                        result->dword_1,
                        result->status,
                        result->user_data);

    h_ctxt = (hw_ctxt *) result->user_data;
    if (h_ctxt == NULL) { return; }

    g_ctxt  = (bus_iface_hw_ctxt *) h_ctxt->core_iface;
    if (g_ctxt == NULL) { return; }

    c_ctxt = h_ctxt->client_ctxt_head;
    if (c_ctxt != NULL)
    {
        t_ctxt  = (transfer_ctxt *)     &c_ctxt->t_ctxt;
        io_ctxt = (bus_iface_io_ctxt *) t_ctxt->io_ctxt;
    }

    if (result->type == EVT_CMD_COMPLETION)
    {
        dcfg                = (plat_device_config *) h_ctxt->core_config;
        g_ctxt->chan_state  = (GPI_CHAN_STATE)       result->dword_0;

        if ((c_ctxt != NULL) && (io_ctxt->clean_cmd))
        {
            if (g_ctxt->chan_state == GPI_CHAN_STOPPED)
            {
                if (result->chan == GPI_OUTBOUND_CHAN)
                {
                    // outbound channel stopped, we are in error handling sequence
                    i_status = t_ctxt->transfer_status;
                    if (!bus_iface_issue_cmd(h_ctxt,
                                GPI_CHAN_CMD_STOP,
                                GPI_CHAN_STOPPED,
                                GPI_INBOUND_CHAN,
                                TRUE))
                    { I2C_LOG(LEVEL_ERROR, "CLEANUP ERROR: failed to stop RX chan"); }
                }
                else
                {
                    noop_tres(g_ctxt, io_ctxt);
                    if (!bus_iface_issue_cmd(h_ctxt,
                                GPI_CHAN_CMD_START,
                                GPI_CHAN_RUNNING,
                                GPI_INBOUND_CHAN,
                                TRUE))
                    { I2C_LOG(LEVEL_ERROR, "CLEANUP ERROR: failed to start RX chan"); }
                }
            }
            else if (g_ctxt->chan_state == GPI_CHAN_RUNNING)
            {
                if (result->chan == GPI_INBOUND_CHAN)
                {
                    if (!bus_iface_issue_cmd(h_ctxt,
                                GPI_CHAN_CMD_START,
                                GPI_CHAN_RUNNING,
                                GPI_OUTBOUND_CHAN,
                                TRUE))
                    { I2C_LOG(LEVEL_ERROR, "CLEANUP ERROR: failed to start TX chan"); }

                    io_ctxt->unlock_pending = 1;
                }
                else
                {
                    io_ctxt->clean_cmd = 0;
                    if (io_ctxt->unlock_pending == 0) { t_state = TRANSFER_DONE; }
                }
            }
        }
        else if (dcfg->polled_mode == FALSE)
        {
            plat_signal_event (g_ctxt->command_wait_signal);
        }
        if (t_state != TRANSFER_DONE) { return; }
    }

    if ((io_ctxt == NULL) || (t_ctxt == NULL)) { return; }

    if (result->type == EVT_QUP_NOTIF)
    {
        if (h_ctxt->core_state & I2C_DRV_BUS_INTERFACE_INITIALIZED)
        {
            if ((result->dword_0 & STATUS_BIT_CMD_CANCEL) &&
                (result->dword_0 & STATUS_BIT_CMD_DONE))
            {
                t_ctxt->transferred += length;
            }
            else if (GP_IRQ(result->dword_0, QN_GP_IRQ_OFFSET) & STATUS_BIT_NACK)
            {
                t_ctxt->transfer_status = I2C_ERROR_ADDR_NACK;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR NACK");
            }
            else if (GP_IRQ(result->dword_0, QN_GP_IRQ_OFFSET) & STATUS_BIT_ARB_LOST)
            {
                t_ctxt->transfer_status = I2C_ERROR_ARBITRATION_LOST;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR ARB LOST");
            }
            else if (GP_IRQ(result->dword_0, QN_GP_IRQ_OFFSET) & STATUS_BIT_BUS_ERROR)
            {
                t_ctxt->transfer_status = I2C_ERROR_START_STOP_UNEXPECTED;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR BUS ERROR");
            }
            if (result->dword_0 & STATUS_BIT_TIMESTAMP)
            {
                if (t_ctxt->start_bit_timestamp == 0)
                {
                    t_ctxt->start_bit_timestamp = result->dword_1;
                }
                else
                {
                    t_ctxt->stop_bit_timestamp  = result->dword_1;
                }
            }
        }
        else
        {
            I2C_LOG(LEVEL_ERROR, "ERROR QUP_NOTIF during command phase");
        }
    }
    else if (result->type == GPI_EVT_OTHER)
    {
        if (h_ctxt->core_state & I2C_DRV_BUS_INTERFACE_INITIALIZED)
        {
            if (result->dword_0 == GPI_CHAN_ERROR)
            {
                if (io_ctxt->pending_evts) { io_ctxt->pending_evts--; }

                if (result->chan == GPI_OUTBOUND_CHAN)
                { io_ctxt->clean_cmd |= CLEAN_TX_CHANNEL; }
                else
                { io_ctxt->clean_cmd |= CLEAN_RX_CHANNEL; }
            }
            else
            {
                i_status = I2C_ERROR_DMA_EVT_OTHER;
            }
            I2C_LOG(LEVEL_ERROR, "ERROR DMA EVT OTHER during data phase");
        }
        else
        {
            I2C_LOG(LEVEL_ERROR, "ERROR DMA EVT OTHER during command phase");
        }
    }
    else if (result->type == EVT_XFER_COMPLETION)
    {
        // process the transfer completion here

        // mark tre indices as processed in eob
        if ((result->code == EVT_COMPLETION_EOB) ||
            (result->code == EVT_COMPLETION_EOT))
        {
            if (result->chan == GPI_INBOUND_CHAN)
            {
                io_ctxt->rx_tre_remaining--;
            }
            else if (result->chan == GPI_OUTBOUND_CHAN)
            {
                io_ctxt->tx_tre_remaining--;
            }
            t_ctxt->transferred += length;
        }

        if (io_ctxt->unlock_pending)
        {
            io_ctxt->unlock_pending = 0;
            if (io_ctxt->clean_cmd == 0) { t_state = TRANSFER_DONE; }
            if (t_state != TRANSFER_DONE) { return; }
        }
        
        if ((io_ctxt->tx_tre_remaining | io_ctxt->rx_tre_remaining) == 0) { t_state = TRANSFER_DONE; }

        if (GP_IRQ(result->status, TCE_GP_IRQ_OFFSET) != 0)
        {
            if (GP_IRQ(result->status, TCE_GP_IRQ_OFFSET) & STATUS_BIT_NACK)
            {
                t_ctxt->transfer_status = I2C_ERROR_ADDR_NACK;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR TCE NACK");
            }
            else if (GP_IRQ(result->status, TCE_GP_IRQ_OFFSET) & STATUS_BIT_ARB_LOST)
            {
                t_ctxt->transfer_status = I2C_ERROR_ARBITRATION_LOST;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR TCE ARB LOST");
            }
            else if (GP_IRQ(result->status, TCE_GP_IRQ_OFFSET) & STATUS_BIT_BUS_ERROR)
            {
                t_ctxt->transfer_status = I2C_ERROR_START_STOP_UNEXPECTED;
                io_ctxt->pending_evts--;
                I2C_LOG(LEVEL_ERROR, "ERROR TCE BUS ERROR");
            }
        }
    }

    if (t_state == TRANSFER_DONE)
    {
        process_transfer_completion (h_ctxt, i_status, t_state);
    }
    else if (io_ctxt->pending_evts == 0)
    {
        if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_STOP, GPI_CHAN_STOPPED, GPI_OUTBOUND_CHAN, TRUE))
        { I2C_LOG(LEVEL_ERROR, "CLEANUP ERROR: failed to stop TX chan"); }
    }
}

void *bus_iface_get_io_ctxt (uint8 core_index)
{
    return (bus_iface_io_ctxt *) plat_mem_alloc (core_index,
                                                 sizeof(bus_iface_io_ctxt),
                                                 IO_CTXT_TYPE);
}

void bus_iface_release_io_ctxt (void *ptr)
{
    plat_mem_free (ptr, sizeof(bus_iface_io_ctxt), IO_CTXT_TYPE);
}

i2c_status bus_iface_init (hw_ctxt *h_ctxt)
{
    plat_device_config *dcfg = (plat_device_config *) h_ctxt->core_config;
    bus_iface_hw_ctxt *g_ctxt;
    gpi_ring_elem *tx_tre_base, *rx_tre_base;
    i2c_status i_status = I2C_SUCCESS;
    boolean gpi_registered = FALSE;

    g_ctxt = (bus_iface_hw_ctxt *) plat_mem_alloc (dcfg->core_index,
                                                   sizeof(bus_iface_hw_ctxt),
                                                   CORE_IFACE_TYPE);
    if (g_ctxt == NULL) { return I2C_ERROR_NO_MEM; }

    h_ctxt->core_iface = g_ctxt;

    // init gpii
    g_ctxt->params.gpi_handle = NULL;
    g_ctxt->params.gpii_id = dcfg->gpi_index;
    g_ctxt->params.protocol = GPI_I2C_PROTOCOL;
    g_ctxt->params.qup_type = (QUP_BLOCK_TYPE) dcfg->qupv3_instance;

    g_ctxt->params.int_modt_val = 0;
    g_ctxt->params.int_modc_val = 0;

    // force polled mode if qup core cannot support interrupts
    if (qup_common_get_hw_version () == QUP_V1_0)
    {
        dcfg->polled_mode = TRUE;
    } 

    // polled mode is bus_iface implementation
    if (dcfg->polled_mode)
    {
        g_ctxt->params.irq_mode = FALSE;
    }
    else
    {
        g_ctxt->params.irq_mode = TRUE;
    }
    g_ctxt->params.cb = bus_iface_callback;

    // alloc all tx and rx ring elems that are needed for this gpii
    tx_tre_base = (gpi_ring_elem *) plat_mem_alloc (
            dcfg->core_index,
            sizeof(gpi_ring_elem) * MAX_TX_TRE_LIST_SIZE_PER_CORE,
            TX_TRANSFER_ELEM_TYPE);

    g_ctxt->params.chan[0].ring_base_va  = (int64) tx_tre_base;
    g_ctxt->params.chan[0].ring_base_pa  = (int64) plat_virt_to_phys ((void *) tx_tre_base, ATTRIB_TRE);
    if (g_ctxt->params.chan[0].ring_base_pa == NULL)
    { i_status = I2C_ERROR_DMA_TX_CHAN_ADDRESS_MAP_FAIL; goto error; }
    g_ctxt->params.chan[0].tre_ring_size = MAX_TX_TRE_LIST_SIZE_PER_CORE;

    rx_tre_base = (gpi_ring_elem *) plat_mem_alloc (
            dcfg->core_index,
            sizeof(gpi_ring_elem) * MAX_RX_TRE_LIST_SIZE_PER_CORE,
            RX_TRANSFER_ELEM_TYPE);

    g_ctxt->params.chan[1].ring_base_va  = (int64) rx_tre_base;
    g_ctxt->params.chan[1].ring_base_pa  = (int64) plat_virt_to_phys ((void *) rx_tre_base, ATTRIB_TRE);
    if (g_ctxt->params.chan[1].ring_base_pa == NULL)
    { i_status = I2C_ERROR_DMA_RX_CHAN_ADDRESS_MAP_FAIL; goto error; }
    g_ctxt->params.chan[1].tre_ring_size = MAX_RX_TRE_LIST_SIZE_PER_CORE;

    g_ctxt->params.user_data = h_ctxt;

    if (GPI_STATUS_SUCCESS != gpi_iface_reg(&g_ctxt->params))
    {
        i_status = I2C_ERROR_DMA_REG_FAIL;
        goto error;
    }
    gpi_registered = TRUE;

    g_ctxt->chan_state = GPI_CHAN_ERROR;
    g_ctxt->command_wait_signal = plat_signal_alloc (dcfg->core_index);
    if (g_ctxt->command_wait_signal == NULL)
    {
        i_status = I2C_ERROR_PLATFORM_SIGNAL_FAIL;
        goto error;
    }

    plat_signal_init (&(g_ctxt->command_wait_signal));

    // enable interrupts
    gpi_iface_active(g_ctxt->params.gpi_handle, TRUE);

    // allocate channels
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_OUTBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_TX_CHAN_ALLOC_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_INBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_RX_CHAN_ALLOC_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_EVT_RING, FALSE))
    { i_status = I2C_ERROR_DMA_EV_CHAN_ALLOC_FAIL; goto error; }

    // start channels
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_START, GPI_CHAN_RUNNING, GPI_OUTBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_TX_CHAN_START_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_START, GPI_CHAN_RUNNING, GPI_INBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_RX_CHAN_START_FAIL; goto error; }

    // disable interrupts
    gpi_iface_active(g_ctxt->params.gpi_handle, FALSE);

error:

    if (g_ctxt->command_wait_signal != NULL)
    {
        plat_signal_de_init (&(g_ctxt->command_wait_signal));
    }

    if (I2C_ERROR(i_status))
    {
        if (gpi_registered)
        {
            gpi_iface_dereg(g_ctxt->params.gpi_handle);
        }

        if (g_ctxt->params.chan[0].ring_base_va != NULL)
        {
            plat_mem_free (
                    (void *) g_ctxt->params.chan[0].ring_base_va,
                    sizeof(gpi_ring_elem) * MAX_TX_TRE_LIST_SIZE_PER_CORE,
                    TX_TRANSFER_ELEM_TYPE);
        }

        if (g_ctxt->params.chan[1].ring_base_va != NULL)
        {
            plat_mem_free (
                    (void *) g_ctxt->params.chan[1].ring_base_va,
                    sizeof(gpi_ring_elem) * MAX_RX_TRE_LIST_SIZE_PER_CORE,
                    RX_TRANSFER_ELEM_TYPE);
        }

        if (g_ctxt->command_wait_signal != NULL)
        {
            plat_signal_free (g_ctxt->command_wait_signal);
            g_ctxt->command_wait_signal = NULL;
        }
    }

    return i_status;
}

i2c_status bus_iface_de_init (hw_ctxt *h_ctxt)
{
    bus_iface_hw_ctxt *g_ctxt = (bus_iface_hw_ctxt *) h_ctxt->core_iface;
    i2c_status i_status = I2C_SUCCESS;

    if (g_ctxt == NULL) { return I2C_ERROR_INVALID_PARAMETER; }

    plat_signal_init (&(g_ctxt->command_wait_signal));

    // enable interrupts
    gpi_iface_active(g_ctxt->params.gpi_handle, TRUE);

    // stop channels
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_STOP, GPI_CHAN_STOPPED, GPI_OUTBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_TX_CHAN_STOP_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_STOP, GPI_CHAN_STOPPED, GPI_INBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_RX_CHAN_STOP_FAIL; goto error; }

    // reset channels
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_RESET, GPI_CHAN_ALLOCATED, GPI_OUTBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_TX_CHAN_RESET_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_RESET, GPI_CHAN_ALLOCATED, GPI_INBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_RX_CHAN_RESET_FAIL; goto error; }

    // de-allocate channels
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_DE_ALLOC, GPI_CHAN_DISABLED, GPI_OUTBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_TX_CHAN_DE_ALLOC_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_DE_ALLOC, GPI_CHAN_DISABLED, GPI_INBOUND_CHAN, FALSE))
    { i_status = I2C_ERROR_DMA_RX_CHAN_DE_ALLOC_FAIL; goto error; }
    if (!bus_iface_issue_cmd(h_ctxt, GPI_CHAN_CMD_DE_ALLOC, GPI_CHAN_DISABLED, GPI_EVT_RING, FALSE))
    { i_status = I2C_ERROR_DMA_EV_CHAN_DE_ALLOC_FAIL; goto error; }

    // disable interrupts
    gpi_iface_active(g_ctxt->params.gpi_handle, FALSE);

error:

    plat_signal_de_init (&(g_ctxt->command_wait_signal));

    if (GPI_STATUS_SUCCESS != gpi_iface_dereg(g_ctxt->params.gpi_handle))
    {
        i_status = I2C_ERROR_DMA_DE_REG_FAIL;
    }

    if (g_ctxt->params.chan[0].ring_base_va != NULL)
    {
        plat_mem_free (
                (void *) g_ctxt->params.chan[0].ring_base_va,
                sizeof(gpi_ring_elem) * MAX_TX_TRE_LIST_SIZE_PER_CORE,
                TX_TRANSFER_ELEM_TYPE);
    }

    if (g_ctxt->params.chan[1].ring_base_va != NULL)
    {
        plat_mem_free (
                (void *) g_ctxt->params.chan[1].ring_base_va,
                sizeof(gpi_ring_elem) * MAX_RX_TRE_LIST_SIZE_PER_CORE,
                RX_TRANSFER_ELEM_TYPE);
    }

    if (g_ctxt->command_wait_signal != NULL)
    {
        plat_signal_free (g_ctxt->command_wait_signal);
        g_ctxt->command_wait_signal = NULL;
    }

    plat_mem_free (
            g_ctxt,
            sizeof(bus_iface_hw_ctxt),
            CORE_IFACE_TYPE);

    return i_status;
}

// keep it simple. commands execute synchronously and one command at a time
boolean bus_iface_issue_cmd (hw_ctxt           *h_ctxt,
                             GPI_CHAN_CMD       gpi_chan_cmd,
                             GPI_CHAN_STATE     gpi_exp_state,
                             GPI_CHAN_TYPE      gpi_chan_type,
                             boolean            asynchronous)
{
    bus_iface_hw_ctxt *g_ctxt = (bus_iface_hw_ctxt *) h_ctxt->core_iface;
    plat_device_config *dcfg = (plat_device_config *) h_ctxt->core_config;

    g_ctxt->chan_state  = GPI_CHAN_ERROR;
    I2C_LOG(LEVEL_PERF, I2C_IFACE_ISSUE_CMD, gpi_chan_type, gpi_chan_cmd);
    if (GPI_STATUS_SUCCESS != gpi_issue_cmd(g_ctxt->params.gpi_handle,
                                            gpi_chan_type,
                                            gpi_chan_cmd,
                                            h_ctxt))
    {
        return FALSE;
    }

    if (asynchronous) { return TRUE; }

    if (dcfg->polled_mode == FALSE)
    {
        plat_wait_for_event(g_ctxt->command_wait_signal);
    }
    else
    {
        uint32 timeout_us = POLL_TIME_DEFAULT_US;
        while ((g_ctxt->chan_state != gpi_exp_state) && (timeout_us != 0))
        {
            bus_iface_isr (h_ctxt);
            plat_delay_us (POLL_INTERVAL_US);
            timeout_us -= POLL_INTERVAL_US;
        }
    }

    if (g_ctxt->chan_state != gpi_exp_state)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR chan_state = %d: expected = %d", g_ctxt->chan_state, gpi_exp_state);
        return FALSE;
    }

    return TRUE;
}

uint8 bus_gpi_get_tre_i2c_go_command (uint32 t_flags, uint32 length, i2c_mode mode)
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

uint8 bus_gpi_get_tre_i3c_go_flags (i2c_descriptor *dc)
{
    uint8 flags = 0;

    if (dc->flags & I3C_FLAG_USE_7E)
    {
        flags |= 1 << GPI_FIELD_SHIFT(TRE_I3C_GO_I3C_FLAGS_USE_7E);
    }

    if (dc->flags & I3C_FLAG_IBI_CTRL)
    {
        flags |= 1 << GPI_FIELD_SHIFT(TRE_I3C_GO_I3C_FLAGS_IBI_CONTROL);
    }

    return flags;
}

uint8 bus_gpi_get_tre_i2c_go_flags (i2c_descriptor *dc, boolean delay, boolean last)
{
    uint8 flags = 0;
    uint32 t_flags = dc->flags;
    boolean has_start       = t_flags & I2C_FLAG_START;
    boolean has_stop        = t_flags & I2C_FLAG_STOP;
    boolean has_read        = t_flags & I2C_FLAG_READ;
    boolean has_timestamp_s = t_flags & I2C_FLAG_TIMESTAMP_S;
    boolean has_timestamp_p = t_flags & I2C_FLAG_TIMESTAMP_P;
    boolean raw_read        = FALSE;

    if (delay)
    {
        flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_PRE_CMD_DELAY));
    }
    if (has_timestamp_s)
    {
        flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_TIMESTAMP_BEFORE));
    }
    else if (has_timestamp_p)
    {
        flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_TIMESTAMP_AFTER));
    }
    if (!has_stop)
    {
        flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_STOP_STRETCH));

        if (!last && has_read)
        {
            // check for raw read
            i2c_descriptor *dc_next = dc + 1;
            if ((dc_next->flags & I2C_FLAG_READ) &&
               !(dc_next->flags & I2C_FLAG_START))
            {
                flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_RX_ACK));
                raw_read = TRUE;
            }
        }
    }

    if (!has_start)
    {
        flags |= (1 << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS_BYPASS_ADDR_PHASE));
    }

    return flags;
}

boolean bus_gpi_tre_non_data_opcode (gpi_ring_elem *tre)
{
    uint8 command = (tre->dword_0 >> GPI_FIELD_SHIFT(TRE_I2C_GO_CMD)) &
                                     GPI_FIELD_MASK (TRE_I2C_GO_CMD);

    if ((command == GPI_I2C_GO_CMD_TX_ADDR_ONLY)    ||
        (command == GPI_I2C_GO_CMD_BUS_CLEAR)       ||
        (command == GPI_I2C_GO_CMD_STOP_ONLY))
    {
        return TRUE;
    }

    return FALSE;
}

void bus_gpi_set_tre_i2c_go (gpi_ring_elem *tre, transfer_ctxt *t_ctxt)
{
    uint8 command, slave_id, flags, i3c_flags, ccc = 0, eot = 0, eob = 1, chain = 1;
    uint32 rx_length = 0;
    i2c_mode mode = t_ctxt->config->mode;
    i2c_descriptor *dc = (t_ctxt->desc + t_ctxt->transfer_count);
    boolean delay = FALSE;
    boolean first = FALSE;
    boolean last  = FALSE;

    boolean in_i2c_mode = ((t_ctxt->config->mode == I2C) ||
                           (t_ctxt->config->mode == SMBUS));

    boolean in_cmd_mode = ((t_ctxt->config->mode == I3C_BROADCAST_CCC)  ||
                           (t_ctxt->config->mode == I3C_DIRECT_CCC)     ||
                           (t_ctxt->config->mode == I3C_HDR_DDR));

    if (t_ctxt->transfer_count == 0) { first = TRUE; }
    if (t_ctxt->delay && first)      { delay = TRUE; }
    if (t_ctxt->transfer_count == (t_ctxt->num_descs - 1)) { last = TRUE; }

    command     = bus_gpi_get_tre_i2c_go_command (dc->flags, dc->length, mode);
    slave_id    = (uint8) t_ctxt->config->slave_address;
    flags       = bus_gpi_get_tre_i2c_go_flags (dc, delay, last);
    i3c_flags   = bus_gpi_get_tre_i3c_go_flags (dc);

    if (dc->flags & I2C_FLAG_READ)
    {
        rx_length = dc->length;
        chain = 0;
    }

    if (in_i2c_mode)
    {
        tre->dword_0 = GPI_BUILD_TRE_I2C_GO_0(command, slave_id, flags);
        tre->dword_1 = 0;
    }
    else
    {
        if (in_cmd_mode)
        {
            ccc = dc->cmd & 0xFF;
        }
        tre->dword_0 = GPI_BUILD_TRE_I3C_GO_0(command, slave_id, ccc, flags);
        tre->dword_1 = GPI_BUILD_TRE_I3C_GO_1(i3c_flags);
    }
    tre->dword_2 = GPI_BUILD_TRE_I2C_GO_2(rx_length);
    if (bus_gpi_tre_non_data_opcode (tre))
    {
        eot = 1;
        eob = 0;
        chain = 0;
    }
    tre->ctrl = GPI_BUILD_TRE_CTRL(TRE_GO_MAJOR, TRE_GO_MINOR, 0, eot, eob, chain);
}

boolean bus_gpi_set_tre_i2c_config_0 (gpi_ring_elem *tre, client_ctxt *c_ctxt)
{
    hw_ctxt *h_ctxt                 = c_ctxt->h_ctxt;
    transfer_ctxt *t_ctxt           = &c_ctxt->t_ctxt;
    plat_device_config *dcfg        = (plat_device_config *) (h_ctxt->core_config);
    uint32 bus_frequency_khz        = t_ctxt->config->bus_frequency_khz;
    uint8 dfs_index, divider, t_cycle, t_high, t_low;

    uint32 delay_cycles = 0, clock_period_ns;

    if (FALSE == plat_clock_get_config (dcfg,
                                        bus_frequency_khz,
                                        &dfs_index,
                                        &divider,
                                        &t_cycle,
                                        &t_low,
                                        &t_high,
                                        &clock_period_ns))
    {
        return FALSE;
    }

    if (clock_period_ns > 0)
    {
        delay_cycles = (t_ctxt->delay * 1000)/clock_period_ns;
    }

    tre->dword_0 = GPI_BUILD_TRE_I2C_CONFIG_0_0(t_low, t_high, t_cycle, I2C_PACKING_DEFAULT);
    if (t_ctxt->config->mode == I2C)
    {
        tre->dword_1 = GPI_BUILD_TRE_I2C_CONFIG_0_1(I2C_NOISE_REJECT_LEVEL_DEFAULT, delay_cycles);
        tre->dword_2 = GPI_BUILD_TRE_I2C_CONFIG_0_2(divider, dfs_index);
    }
    else
    {
        tre->dword_1 = GPI_BUILD_TRE_I3C_CONFIG_0_1(t_high, t_low, delay_cycles);
        tre->dword_2 = GPI_BUILD_TRE_I3C_CONFIG_0_2(divider, dfs_index);
    }
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_CONFIG_0_MAJOR, TRE_CONFIG_0_MINOR, 0, 0, 1, 1);

    return TRUE;
}

void bus_gpi_set_tre_dma (gpi_ring_elem *tre, transfer_ctxt *t_ctxt)
{
    i2c_descriptor *dc  = (t_ctxt->desc + t_ctxt->transfer_count);
    uint64 dma_addr     = (uint64) plat_virt_to_phys(dc->buffer, ATTRIB_BUFFER);

    plat_mem_flush_cache (dc->buffer, dc->length, ATTRIB_BUFFER);

    tre->dword_0 = GPI_BUILD_DMA_W_BUFFER_TRE_0((uint32) dma_addr);
    tre->dword_1 = GPI_BUILD_DMA_W_BUFFER_TRE_1((uint32) (dma_addr >> 32));
    tre->dword_2 = GPI_BUILD_DMA_W_BUFFER_TRE_2(dc->length);
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_DMA_W_BUFFER_MAJOR, TRE_DMA_W_BUFFER_MINOR, 0, 1, 0, 0);
}

void bus_gpi_tre_flush (gpi_ring_elem *tre, gpi_ring_elem *base, uint8 num_tres, boolean is_tx)
{
    uint32 tre_list_size = is_tx ? MAX_TX_TRE_LIST_SIZE_PER_CORE : MAX_RX_TRE_LIST_SIZE_PER_CORE;

    I2C_LOG(LEVEL_INFO, "%s", is_tx ? "TX" : "RX");

    while (num_tres)
    {
        plat_mem_flush_cache(tre, sizeof(gpi_ring_elem), ATTRIB_TRE);

        I2C_LOG(LEVEL_INFO, "%08x: %08x %08x %08x %08x",
                tre, tre->dword_0, tre->dword_1, tre->dword_2, tre->ctrl);

        tre++;
        if (tre == (base + tre_list_size)) { tre = base; }

        num_tres--;
    }
}

uint32 gpi_tre_available (bus_iface_hw_ctxt *g_ctxt, GPI_CHAN_TYPE chan, gpi_ring_elem **next_available_tre)
{
    chan_status_info chan_info;

    if (GPI_STATUS_SUCCESS == gpi_query_chan_status(g_ctxt->params.gpi_handle,
                                                    chan,
                                                    &chan_info))
    {
        *next_available_tre = ((gpi_ring_elem *) g_ctxt->params.chan[chan].ring_base_va) + chan_info.wp_index;
        I2C_LOG(LEVEL_INFO, "available 0x%08x free in chan[%d] = %d", *next_available_tre, chan, chan_info.num_avail_tre);
        return chan_info.num_avail_tre; 
    }

    return 0;
}

void bus_gpi_set_tre_lock (gpi_ring_elem *tre)
{
    tre->dword_0 = 0;
    tre->dword_1 = 0;
    tre->dword_2 = 0;
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_LOCK_MAJOR, TRE_LOCK_MINOR, 0, 0, 1, 0);
}

void bus_gpi_set_tre_unlock (gpi_ring_elem *tre)
{
    tre->dword_0 = 0x10;
    tre->dword_1 = 0;
    tre->dword_2 = 0;
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_UNLOCK_MAJOR, TRE_UNLOCK_MINOR, 0, 0, 1, 0);
}

i2c_status bus_iface_queue_transfer (client_ctxt *c_ctxt)
{
    i2c_status              i_status    = I2C_SUCCESS;
    hw_ctxt                 *h_ctxt     = c_ctxt->h_ctxt;
    transfer_ctxt           *t_ctxt     = &c_ctxt->t_ctxt;
    plat_device_config      *dcfg       = (plat_device_config *) (h_ctxt->core_config);
    bus_iface_io_ctxt       *io_ctxt    = (bus_iface_io_ctxt *)  t_ctxt->io_ctxt;
    bus_iface_hw_ctxt       *g_ctxt     = (bus_iface_hw_ctxt *)  h_ctxt->core_iface;
    boolean                 any_reads   = FALSE;

    uint16 i;

    uint32 *t_count, *r_count;
    uint32 num_avail_tx_tre = 0;
    uint32 num_avail_rx_tre = 0;
    gpi_ring_elem *t_tre = NULL;
    gpi_ring_elem *r_tre = NULL;

    gpi_ring_elem *tx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[0].ring_base_va;
    gpi_ring_elem *rx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[1].ring_base_va;

    if ((num_avail_tx_tre = gpi_tre_available (g_ctxt, GPI_OUTBOUND_CHAN, &t_tre)) == 0)
    {
        return I2C_ERROR_DMA_INSUFFICIENT_RESOURCES;
    }
    io_ctxt->tx_tre_req.tre_list = t_tre;

    for (i = 0; i < t_ctxt->num_descs; i++)
    {
        if ((t_ctxt->desc + i)->flags & I2C_FLAG_READ)
        {
            any_reads = TRUE;
            break;
        }
    }

    if (any_reads &&
       ((num_avail_rx_tre = gpi_tre_available (g_ctxt, GPI_INBOUND_CHAN, &r_tre)) == 0))
    {
        return I2C_ERROR_DMA_INSUFFICIENT_RESOURCES;
    }
    io_ctxt->rx_tre_req.tre_list = r_tre;

    (io_ctxt->tx_tre_req).num_tre = 0;
    (io_ctxt->rx_tre_req).num_tre = 0;

    t_count = (uint32 *) &((io_ctxt->tx_tre_req).num_tre);
    r_count = (uint32 *) &((io_ctxt->rx_tre_req).num_tre);

    io_ctxt->tx_tre_req.handle       = g_ctxt->params.gpi_handle;
    io_ctxt->tx_tre_req.chan         = GPI_OUTBOUND_CHAN;

    io_ctxt->rx_tre_req.handle       = g_ctxt->params.gpi_handle;
    io_ctxt->rx_tre_req.chan         = GPI_INBOUND_CHAN;

    io_ctxt->tx_tre_remaining = 0;
    io_ctxt->rx_tre_remaining = 0;

    io_ctxt->pending_evts = 3;
    io_ctxt->clean_cmd = 0;
    io_ctxt->unlock_pending = 0;

    bus_gpi_set_tre_lock (t_tre);
    ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)

    if (bus_gpi_set_tre_i2c_config_0 (t_tre, c_ctxt) == FALSE)
    {
        return I2C_ERROR_DMA_CONFIGURATION_FAIL;
    }
    ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)

    for (i = 0; i < t_ctxt->num_descs; i++)
    {
        bus_gpi_set_tre_i2c_go (t_tre, t_ctxt);
        if (bus_gpi_tre_non_data_opcode (t_tre))
        {
            ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
            break;
        }
        ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)

        if ((t_ctxt->desc + i)->flags & I2C_FLAG_WRITE)
        {
            bus_gpi_set_tre_dma (t_tre, t_ctxt);
            ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
        }
        else
        {
            bus_gpi_set_tre_dma (r_tre, t_ctxt);
            ADVANCE_TRE(RX, *r_count, num_avail_rx_tre, r_tre, rx_base_tre)
        }
        t_ctxt->transfer_count++;
    }

    bus_gpi_set_tre_unlock (t_tre);
    ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)

    if (*r_count)
    {
        io_ctxt->rx_tre_req.user_data = h_ctxt;
        bus_gpi_tre_flush  (io_ctxt->rx_tre_req.tre_list, rx_base_tre, *r_count, FALSE);
    }

    if (*t_count)
    {
        io_ctxt->tx_tre_req.user_data = h_ctxt;
        bus_gpi_tre_flush  (io_ctxt->tx_tre_req.tre_list, tx_base_tre, *t_count, TRUE);
    }

    if (*r_count)
    {
        io_ctxt->rx_tre_remaining = *r_count;
        I2C_LOG(LEVEL_PERF, I2C_IFACE_RX_TRE_QUEUED, *r_count);
        if (GPI_STATUS_SUCCESS != gpi_process_tre(&io_ctxt->rx_tre_req))
        {
            return I2C_ERROR_DMA_PROCESS_TRANSFER_FAIL;
        }
    }

    if (*t_count)
    {
        io_ctxt->tx_tre_remaining = *t_count;
        I2C_LOG(LEVEL_PERF, I2C_IFACE_TX_TRE_QUEUED, *t_count);
        if (GPI_STATUS_SUCCESS != gpi_process_tre(&io_ctxt->tx_tre_req))
        {
            return I2C_ERROR_DMA_PROCESS_TRANSFER_FAIL;
        }
    }

    // polled mode logic, sensitive code
    if (dcfg->polled_mode)
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
            // if we timed out, process transfer completion
            i_status = I2C_ERROR_TRANSFER_TIMEOUT;
            I2C_LOG(LEVEL_ERROR, "ERROR TRANSFER TIMEOUT");
            process_transfer_completion (h_ctxt, i_status, TRANSFER_TIMED_OUT);
        }
        else if (t_ctxt->callback == NULL)
        {
            // if we did not time out and callback is NULL, update the status
            i_status = t_ctxt->transfer_status;
        }
    }

    // if the callback is not NULL status is always returned in the callback,
    // make sure that I2C_SUCCESS is returned here in that case. If the callback
    // is NULL, then we need to return the status here.
    return i_status;
}

void bus_iface_isr (void *int_handle)
{
    hw_ctxt             *h_ctxt = (hw_ctxt *) int_handle;
    bus_iface_hw_ctxt   *g_ctxt = (bus_iface_hw_ctxt *)  h_ctxt->core_iface;

    gpi_iface_poll(g_ctxt->params.gpi_handle);
}

boolean bus_iface_has_queueing(hw_ctxt *h_ctxt)
{
    return TRUE;
}

boolean bus_iface_has_interrupt_consumer(hw_ctxt *h_ctxt)
{
    return FALSE;
}
