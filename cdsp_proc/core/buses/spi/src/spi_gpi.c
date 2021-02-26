/**
    @file  spi_gpi.c
    @brief SPI GSI implementation
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_driver.h"
#include "spi_config.h"
#include "spi_gpi.h"
#include "spi_plat.h"
#include "spi_log.h"
#include "spi_qdi.h"
#include "pram_mgr.h"
#include "qurt.h"
#include "qurt_island.h"
#include "qup_common.h"

#define ADVANCE_TRE(dir, count, num_avail_tre, tre, base_tre)           \
    if ((count) > num_avail_tre)                                        \
    {                                                                   \
        return SPI_ERROR_DMA_INSUFFICIENT_RESOURCES;                    \
    }                                                                   \
    else                                                                \
    {                                                                   \
        (count)++;                                                      \
        (tre)++;                                                        \
        if ((base_tre + MAX_##dir##_TRE_LIST_SIZE_PER_CORE) == tre)     \
        {                                                               \
            tre = base_tre;                                             \
        }                                                               \
    }

#define GPI_POLLING_TIMEOUT_US    1000000
#define GPI_POLLING_INTERVAL_US   20

// TODO: set this to 8 to enable IMM DMA
#define SPI_IMM_DMA_LENGTH 0

#define STATUS_BIT_TIMESTAMP  (1 << 6)


static GPI_CLIENT_HANDLE  spi_gpi_handles[8] = {0};
static uint32 spi_island_calls = 0;

extern spi_hw_ctxt *spi_cores_head;

static boolean spi_gpi_issue_cmd (spi_hw_ctxt       *h_ctxt,
                                  GPI_CHAN_CMD       gpi_chan_cmd,
                                  GPI_CHAN_STATE     gpi_exp_state,
                                  GPI_CHAN_TYPE      gpi_chan_type);

static void get_imm_read_data (uint8 *buffer, uint32 dword0, uint32 dword1, uint32 length)
{
    uint8 i, shift;

    for (i = 0; i < length; i++)
    {
        shift = (i & 3) << 3;
        if (i < 4) { buffer[i] = (uint8) (dword0 >> shift); }
        else       { buffer[i] = (uint8) (dword1 >> shift); }
    }
}

static spi_client_ctxt *spi_gpi_get_client_ctxt(uint32 gpii)
{
    spi_hw_ctxt *h_ctxt;
    spi_client_ctxt *c_ctxt = NULL;

    spi_plat_mutex_global_lock();
    h_ctxt = spi_cores_head;
    while (h_ctxt != NULL)
    {
        if (((spi_plat_device_config *) h_ctxt->core_config)->gpi_index == gpii)
        {
            c_ctxt = h_ctxt->client_ctxt_head;
            break;
        }
        h_ctxt = h_ctxt->next;
    }
    spi_plat_mutex_global_unlock();

    return c_ctxt;
}

static void spi_gpi_callback (gpi_result_type *result)
{
    spi_status_t status = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt;
    spi_xfer_ctxt *t_ctxt;
    spi_gpi_io_ctxt *io_ctxt;
    spi_hw_ctxt *h_ctxt;
    spi_gpi_ctxt *g_ctxt;
    spi_descriptor_t *dc;
	spi_xfer_state temp_state = TRANSFER_INVALID;

    SPI_LOG(LEVEL_VERBOSE, "chan %d: type %d: code %d: status 0x%08x:length %d: tre_idx %d: dword_0 0x%08x: dword_1 0x%08x:\
             user_data 0x%08x\n", result->chan, result->type, result->code, result->status, result->length,
             result->tre_idx, result->dword_0, result->dword_1, result->user_data);

	if (result == NULL) return;
	
    if (result->type == EVT_CMD_COMPLETION)
    {
        h_ctxt = (spi_hw_ctxt *) result->user_data;
		if (h_ctxt == NULL) return;
        g_ctxt = (spi_gpi_ctxt *) h_ctxt->core_iface;
        g_ctxt->chan_state = (GPI_CHAN_STATE) result->dword_0;
		if (h_ctxt->polled == FALSE)
		{
           spi_plat_signal_event (h_ctxt->core_signal);
		}
        return;
    }

    if (result->type == EVT_QUP_NOTIF)
    {
        c_ctxt = spi_gpi_get_client_ctxt(result->tre_idx);
    }
    else
    {
        c_ctxt  = (spi_client_ctxt *)result->user_data;
    }

    if (c_ctxt == NULL)
    {
        //This is an error and we need to do something here.
        SPI_LOG(LEVEL_ERROR,"spi_gpi_callback : c_ctxt is NULL");
        return;
    }

    h_ctxt  = c_ctxt->h_ctxt;
    g_ctxt  = (spi_gpi_ctxt *) h_ctxt->core_iface;

    t_ctxt  = (spi_xfer_ctxt *) &c_ctxt->t_ctxt;
    io_ctxt = (spi_gpi_io_ctxt *) t_ctxt->io_ctxt;
    dc      = t_ctxt->desc;

    if (result->type == GPI_EVT_OTHER)
    {
        SPI_LOG(LEVEL_ERROR,"spi_gpi_callback : GPI driver sending EVENT_OTHER type");
        return;
#if 0
        status = SPI_ERROR_DMA_EVT_OTHER;
        t_ctxt->xfer_state = TRANSFER_DONE;
        // TODO: cleanup your transfers on the ring.
#endif
    }
    else if (result->type == EVT_QUP_NOTIF)
    {
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
			else
			{
                SPI_LOG(LEVEL_ERROR,"spi_gpi_callback : GPI driver sending SPI_ERROR_DMA_QUP_NOTIF");
		        status = SPI_ERROR_DMA_QUP_NOTIF;
		        //Add log of the QUP Notif error code. GENI_M_IRQ_STATUS on TX channel and of GENI_S_IRQ_STATUS on RX channel
		        t_ctxt->xfer_state = TRANSFER_DONE;
		        // TODO: cleanup your transfers on the ring.

			}
    }
    else if (result->type == EVT_IMM_DATA)
    {
        //Revisit - copied from I2C driver
        /* if multiple imm read tres are queued, they happen in order
        for (i = 0; i < t_ctxt->num_descs; i++)
        {
            if ((dc->flags & I2C_FLAG_READ) &&
                (dc->transferred == 0)      &&
                (dc->length < SPI_IMM_DMA_LENGTH))
            {
                get_imm_read_data(dc->buffer,
                                  result->dword_0,
                                  result->dword_1,
                                  result->length);
                dc->transferred = result->length;
                break;
            }
            dc++;
        }*/
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
                io_ctxt->rx_tre_consumed++;
            }
            else if (result->chan == GPI_OUTBOUND_CHAN)
            {
                io_ctxt->tx_tre_consumed++;
            }
        }

        // update the transfer state
        if (result->code == EVT_COMPLETION_EOT)
        {
            t_ctxt->eots_rxd++;
            if (t_ctxt->eots_rxd == t_ctxt->eots_set)
            {
                temp_state = TRANSFER_DONE;
                status = SPI_SUCCESS;
            }
        }
    }

    if (temp_state == TRANSFER_DONE)
    {
		uint32 i;
        spi_client_ctxt *temp;
        (io_ctxt->tx_tre_req).num_tre = 0;
        (io_ctxt->rx_tre_req).num_tre = 0;

        //invalidate_read_descriptors if not in island.
		if (!qurt_island_get_status())
		{
			spi_plat_device_config *dcfg = (spi_plat_device_config *) h_ctxt->core_config;
			if (dcfg->core_index == 2)
			{
				for (i = 0; i < t_ctxt->num_descs; i++)
				{
				   spi_descriptor_t *dc = &(t_ctxt->desc[i]);
				   if (dc->rx_buf != NULL)
				   {
						qurt_mem_cache_clean((qurt_addr_t) dc->rx_buf,
											 (qurt_size_t) dc->len,
											 QURT_MEM_CACHE_INVALIDATE,
											 QURT_MEM_DCACHE);
				   }
				}
			}
		}

		spi_plat_mutex_instance_lock (h_ctxt->queue_lock);
        // remove the client from the queue
        temp = h_ctxt->client_ctxt_head;
        if (temp!= NULL)
        {
           h_ctxt->client_ctxt_head = temp->next;
           temp->next = NULL;
        }
        spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);
        t_ctxt->xfer_status = status;
        t_ctxt->xfer_state = temp_state;
        if(t_ctxt->async)
        {
           spi_notify_completion(t_ctxt->callback, status, t_ctxt->ctxt);
        }
      //synchronous (no client callback), and we dont poll for completion
        else if (h_ctxt->polled == FALSE)
        {
            spi_plat_signal_event(h_ctxt->core_signal);
        }
      /* else: synchronous call, we poll for completion.
      This will be handled in the polling logic in spi_gpi_queue_transfer*/
    }
}

spi_status_t spi_gpi_init (spi_hw_ctxt *h_ctxt)
{
    spi_plat_device_config *dcfg = (spi_plat_device_config *) h_ctxt->core_config;
    spi_gpi_ctxt *g_ctxt = (spi_gpi_ctxt *) h_ctxt->core_iface;
    gpi_ring_elem *tx_tre_base, *rx_tre_base;
    GPI_RETURN_STATUS gpi_ret_val = GPI_STATUS_SUCCESS;

    // init gpii
    if (spi_gpi_handles[dcfg->core_index] == NULL)
    {
       g_ctxt->params.gpi_handle = NULL;
    }
    else
    {
       g_ctxt->params.gpi_handle = spi_gpi_handles[dcfg->core_index];
    }
    g_ctxt->params.gpii_id = dcfg->gpi_index;
    g_ctxt->params.protocol = GPI_SPI_PROTOCOL;
    g_ctxt->params.qup_type = QUP_SSC;

    g_ctxt->params.int_modt_val = 0;
    g_ctxt->params.int_modc_val = 0;

    // polled mode is bus_iface implementation
    if (dcfg->polled_mode)
    {
        g_ctxt->params.irq_mode = FALSE;
    }
    else
    {
        g_ctxt->params.irq_mode = TRUE;
    }
    g_ctxt->params.cb = spi_gpi_callback;

    if (spi_gpi_handles[dcfg->core_index] == NULL)
    {
        // alloc all tx and rx ring elems that are needed for this gpii
        tx_tre_base = (gpi_ring_elem *) spi_plat_mem_alloc (
              dcfg->core_index,
              sizeof(gpi_ring_elem) * MAX_TX_TRE_LIST_SIZE_PER_CORE,
              TX_TRANSFER_ELEM_TYPE);


        g_ctxt->params.chan[0].ring_base_va  = (int64) tx_tre_base;
        g_ctxt->params.chan[0].ring_base_pa  = (int64) pram_mgr_lookup_phys_addr_by_name ("BUSES", (qurt_addr_t)tx_tre_base);
        g_ctxt->params.chan[0].tre_ring_size = MAX_TX_TRE_LIST_SIZE_PER_CORE;

        rx_tre_base = (gpi_ring_elem *) spi_plat_mem_alloc (
              dcfg->core_index,
              sizeof(gpi_ring_elem) * MAX_RX_TRE_LIST_SIZE_PER_CORE,
              RX_TRANSFER_ELEM_TYPE);

        g_ctxt->params.chan[1].ring_base_va  = (int64) rx_tre_base;
        g_ctxt->params.chan[1].ring_base_pa  = (int64) pram_mgr_lookup_phys_addr_by_name ("BUSES", (qurt_addr_t)rx_tre_base);
        g_ctxt->params.chan[1].tre_ring_size = MAX_RX_TRE_LIST_SIZE_PER_CORE;

        gpi_ret_val = gpi_iface_reg(&g_ctxt->params);
        if (GPI_STATUS_SUCCESS != gpi_ret_val)
        {
            SPI_LOG(LEVEL_ERROR,"spi_gpi_init : gpi_iface_reg failed with ret %d",gpi_ret_val);
            return SPI_ERROR_DMA_REG_FAIL;
        }
        g_ctxt->chan_state = GPI_CHAN_ERROR;

        // allocate channels
        if (!spi_gpi_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_OUTBOUND_CHAN))
        { return SPI_ERROR_DMA_TX_CHAN_ALLOC_FAIL; }
        if (!spi_gpi_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_INBOUND_CHAN))
        { return SPI_ERROR_DMA_RX_CHAN_ALLOC_FAIL; }
        if (!spi_gpi_issue_cmd(h_ctxt, GPI_CHAN_CMD_ALLOCATE, GPI_CHAN_ALLOCATED, GPI_EVT_RING))
        { return SPI_ERROR_DMA_EV_CHAN_ALLOC_FAIL; }

        // start channels
        if (!spi_gpi_issue_cmd(h_ctxt, GPI_CHAN_CMD_START, GPI_CHAN_RUNNING, GPI_OUTBOUND_CHAN))
        { return SPI_ERROR_DMA_TX_CHAN_START_FAIL; }
        if (!spi_gpi_issue_cmd(h_ctxt, GPI_CHAN_CMD_START, GPI_CHAN_RUNNING, GPI_INBOUND_CHAN))
        { return SPI_ERROR_DMA_RX_CHAN_START_FAIL; }

        spi_gpi_handles[dcfg->core_index] = g_ctxt->params.gpi_handle;
    }
    return SPI_SUCCESS;
}

spi_status_t spi_gpi_deinit (spi_hw_ctxt *h_ctxt)
{
    return SPI_SUCCESS;
}

boolean spi_gpi_issue_cmd (spi_hw_ctxt       *h_ctxt,
                           GPI_CHAN_CMD       gpi_chan_cmd,
                           GPI_CHAN_STATE     gpi_exp_state,
                           GPI_CHAN_TYPE      gpi_chan_type)
{
    spi_gpi_ctxt *g_ctxt = (spi_gpi_ctxt *) h_ctxt->core_iface;
    GPI_RETURN_STATUS gpi_ret_val = GPI_STATUS_SUCCESS;

    g_ctxt->chan_state  = GPI_CHAN_DISABLED;
    gpi_ret_val = gpi_issue_cmd(g_ctxt->params.gpi_handle, gpi_chan_type, gpi_chan_cmd, h_ctxt);
    if (GPI_STATUS_SUCCESS != gpi_ret_val)
    {
        SPI_LOG(LEVEL_ERROR,"spi_gpi_issue_cmd : gpi_iface_reg failed with ret %d",gpi_ret_val);
        return FALSE;
    }

   if (h_ctxt->polled)
   {
        uint32 timeout = GPI_POLLING_TIMEOUT_US;
        while ((g_ctxt->chan_state != gpi_exp_state) && timeout)
        {
            spi_gpi_isr (h_ctxt);
            spi_plat_delay_us (GPI_POLLING_INTERVAL_US);
            timeout -= GPI_POLLING_INTERVAL_US;
        }
   }
   else
   {
       spi_plat_wait_for_event(&(h_ctxt->core_signal));
   }

    if (g_ctxt->chan_state != gpi_exp_state)
    {
        SPI_LOG(LEVEL_ERROR,"spi_gpi_issue_cmd : GPI channel state is not proper");
        return FALSE;
    }

    return TRUE;
}

static void spi_gpi_set_tre_go (gpi_ring_elem *tre, spi_xfer_ctxt *t_ctxt)
{
    spi_descriptor_t *dc = (t_ctxt->desc + t_ctxt->xfer_count);

    uint8 cs = t_ctxt->config->spi_slave_index;
    uint8 N = t_ctxt->config->spi_bits_per_word;
    uint8 flags = 0; // TBD: Use this for configurable timestamps and delays.
    uint32 rx_len = 0;
    uint8 cmd = 0;
	uint8 chain = 1;

    if (N != 0)
    {
       if (dc->rx_buf) rx_len = ( (dc->len * 8) / N) ; // TODO: Handle non multiples of 8
    }
    else
    {
      if (dc->rx_buf) rx_len = dc->len;
    }

    if (dc->rx_buf && dc->tx_buf) cmd = 3;
    else if (dc->tx_buf && dc->rx_buf == NULL) cmd = 1;
    else if (dc->rx_buf && dc->tx_buf == NULL) {cmd = 2; chain = 0;};

    if (dc->leave_cs_asserted)
    {
       flags |= (1 << GPI_TRE_SPI_GO_FLAGS_FRAG__SHIFT);
    }

	if(t_ctxt->get_timestamp)
	{
		flags |= (1<< GPI_TRE_SPI_GO_FLAGS_TSTAMP_BEFORE__SHIFT);
		flags |= (1<< GPI_TRE_SPI_GO_FLAGS_TSTAMP_AFTER__MASK);
	}
	
    tre->dword_0 = GPI_BUILD_TRE_SPI_GO_0 (cmd, cs, flags);
    tre->dword_1 = 0;
    tre->dword_2 = GPI_BUILD_TRE_SPI_GO_2 (rx_len);
    tre->ctrl    = GPI_BUILD_TRE_CTRL (TRE_GO_MAJOR, TRE_GO_MINOR,
                       0,   /*BEI*/
                       0,   /*IEOT*/
                       1,   /*IEOB*/
                       chain);
}

static void spi_gpi_set_tre_config_0 (gpi_ring_elem *tre, spi_client_ctxt *c_ctxt)
{
    spi_hw_ctxt *h_ctxt    = c_ctxt->h_ctxt;
    spi_xfer_ctxt *t_ctxt  = &c_ctxt->t_ctxt;
    spi_config_t *c        = t_ctxt->config;
    spi_plat_device_config *dcfg   = (spi_plat_device_config *) (h_ctxt->core_config);
    uint8 config_0_flags = 0, cpol = 0, cpha = 0, coded_spi_word_len;
    uint32 div = 0, dfs_index = 0;
    uint8 over_sample = 1;
    config_0_flags |= c->loopback_mode;
    config_0_flags |= c->cs_toggle << 3;
   /* Provide default values for the divider and index for use in island mode*/
   t_ctxt->saved_div = 3;
   t_ctxt->saved_dfs_index = 2;

    switch (t_ctxt->config->spi_mode)
    {
       case SPI_MODE_1 : cpol = 0; cpha = 1; break;
       case SPI_MODE_2 : cpol = 1; cpha = 0; break;
       case SPI_MODE_3 : cpol = 1; cpha = 1; break;
       case SPI_MODE_0 :
       default         : cpol = 0; cpha = 0; break;
    }
    config_0_flags |= cpha << 4;
    config_0_flags |= cpol << 5;

    /*c->spi_bits_per_word is already validated in api.c*/
    coded_spi_word_len =  c->spi_bits_per_word - 4;

    tre->dword_0 = GPI_BUILD_TRE_SPI_CONFIG_0_0 (coded_spi_word_len, config_0_flags, SPI_RX_TX_PACKING_ENABLE);
    tre->dword_1 = GPI_BUILD_TRE_SPI_CONFIG_0_1 (c->inter_word_delay_cycles, c->cs_clk_delay_cycles, 0);

    if (qup_common_get_hw_version () == QUP_V1_0)
    {
        over_sample = 2;
    }
    //hack for testing island mode - do this once and leave the freq vote on!

    if (!qurt_island_get_status())
    {
       spi_plat_clock_set_frequency(dcfg, c->clk_freq_hz * over_sample, &dfs_index, &div, FALSE);
      t_ctxt->saved_dfs_index = dfs_index;
      t_ctxt->saved_div = div;
      tre->dword_2 = GPI_BUILD_TRE_SPI_CONFIG_0_2 (div, dfs_index);
    }
   else
   {
       spi_plat_get_div_dfs(dcfg, c->clk_freq_hz * over_sample, &(t_ctxt->saved_dfs_index), &(t_ctxt->saved_div));
       tre->dword_2 = GPI_BUILD_TRE_SPI_CONFIG_0_2 (t_ctxt->saved_div, t_ctxt->saved_dfs_index);
   }
    //tre->dword_2 = GPI_BUILD_TRE_SPI_CONFIG_0_2 (div, dfs_index);
    tre->ctrl    = GPI_BUILD_TRE_CTRL (TRE_CONFIG_0_MAJOR, TRE_CONFIG_0_MINOR,
                       0,   /*BEI*/
                       0,   /*IEOT*/
                       1,   /*IEOB*/
                       1);  /*CH*/
}

static boolean spi_gpi_tre_non_data_opcode (gpi_ring_elem *tre)
{
    uint8 command = (tre->dword_0 >> GPI_FIELD_SHIFT(TRE_SPI_GO_CMD)) &
                                     GPI_FIELD_MASK (TRE_SPI_GO_CMD);

    if ((command == GPI_SPI_GO_CMD_CS_ASSERT)    ||
        (command == GPI_SPI_GO_CMD_CS_DEASSERT)  ||
        (command == GPI_SPI_GO_CMD_SCK_ONLY))
    {
        return TRUE;
    }

    return FALSE;
}

//revisit
static void spi_gpi_set_tre_dma (spi_hw_ctxt *h_ctxt, gpi_ring_elem *tre, uint8 *buf, uint32 len)
{
    /*uint32 data = 0;
    uint16 i;

    if (dc->length <= SPI_IMM_DMA_LENGTH)
    {
        if (dc->flags & I2C_FLAG_WRITE)
        {
            for (i = 0; (i < dc->length) && (i < 4); i++)
            {
                data = data | (dc->buffer[i] << ((3 - i) << 3));
            }
            tre->dword_0 = GPI_BUILD_DMA_IMM_TRE_0(data);
            if (dc->length > 4)
            {
                for (i = 4; (i < dc->length) && (i < 8); i++)
                {
                    data = data | (dc->buffer[i] << ((7 - i) << 3));
                }
                tre->dword_1 = GPI_BUILD_DMA_IMM_TRE_1(data);
            }
        }
        else
        {
            tre->dword_0 = GPI_BUILD_DMA_IMM_TRE_0(data);
            tre->dword_1 = GPI_BUILD_DMA_IMM_TRE_1(data);
        }
        tre->dword_2 = GPI_BUILD_DMA_IMM_TRE_2(dc->length);
        tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_DMA_IMM_MAJOR, TRE_DMA_IMM_MINOR, 0, 0, 0, 0);
    }
    else*/
    {
#ifdef TOUCH_USES_PRAM_BUFF
        uint64 dma_addr = (uint64) buf;
#else
        uint64 dma_addr = (uint64) buf;
        spi_plat_device_config *dcfg = (spi_plat_device_config *) h_ctxt->core_config;
        if (dcfg->core_index == 2) //touch hack
        {
            dma_addr = (uint64)spi_plat_virt_to_phys(buf, ATTRIB_BUFFER);
            if (!qurt_island_get_status())
            {
                spi_plat_mem_flush_cache (buf, len);
            }
        }
#endif
        tre->dword_0 = GPI_BUILD_DMA_W_BUFFER_TRE_0((uint32) dma_addr);
        tre->dword_1 = GPI_BUILD_DMA_W_BUFFER_TRE_1((uint32) (dma_addr >> 32));
        tre->dword_2 = GPI_BUILD_DMA_W_BUFFER_TRE_2(len);
        //each of our descriptors consists only one 1 buf of any direction OR 1 buf of each direction.
        //EOT = 1, and CH = 0 is mandatory, since the next descriptor starts with a GO and is
        //therefore a fresh transfer.
        tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_DMA_W_BUFFER_MAJOR, TRE_DMA_W_BUFFER_MINOR,
                           0,  /*BEI*/
                           1,  /*IEOT*/
                           0,  /*IEOB*/
                           0); /*CHAIN*/
    }
}

static void spi_gpi_dump_tres(gpi_ring_elem *tre_list, gpi_ring_elem *base, uint32 count, boolean is_tx)
{
    uint32 i = 0;
    uint32 tre_list_size = is_tx ? MAX_TX_TRE_LIST_SIZE_PER_CORE : MAX_RX_TRE_LIST_SIZE_PER_CORE;

    while (i < count)
    {
        SPI_LOG(LEVEL_VERBOSE, "spi_gpi_dump_tres : %08x %08x %08x %08x\n", tre_list->dword_0, tre_list->dword_1,
                 tre_list->dword_2, tre_list->ctrl);
        i++;
        tre_list++;
        if (tre_list == (base + tre_list_size))
        {
            tre_list = base;
        }
    }
}

static uint32 spi_gpi_tre_available (spi_gpi_ctxt *g_ctxt, GPI_CHAN_TYPE chan, gpi_ring_elem **next_available_tre)
{
    chan_status_info chan_info;

    if (GPI_STATUS_SUCCESS == gpi_query_chan_status(g_ctxt->params.gpi_handle,
                                                    chan,
                                                    &chan_info))
    {
        *next_available_tre = ((gpi_ring_elem *) g_ctxt->params.chan[chan].ring_base_va) + chan_info.wp_index;
        return chan_info.num_avail_tre;
    }
    return 0;
}

static void spi_gpi_set_tre_lock (gpi_ring_elem *tre)
{
    tre->dword_0 = 1;
    tre->dword_1 = 0;
    tre->dword_2 = 0;
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_LOCK_MAJOR, TRE_LOCK_MINOR, 0, 0, 0, 1);
}

static void spi_gpi_set_tre_unlock (gpi_ring_elem *tre)
{
    tre->dword_0 = 0x10;
    tre->dword_1 = 0;
    tre->dword_2 = 0;
    tre->ctrl    = GPI_BUILD_TRE_CTRL(TRE_UNLOCK_MAJOR, TRE_UNLOCK_MINOR, 0, 0, 0, 0);
}

spi_status_t spi_gpi_queue_transfer (spi_client_ctxt *c_ctxt)
{
    spi_hw_ctxt   *h_ctxt =  c_ctxt->h_ctxt;
    spi_xfer_ctxt *t_ctxt = &c_ctxt->t_ctxt;
    spi_gpi_io_ctxt  *io_ctxt = (spi_gpi_io_ctxt *)t_ctxt->io_ctxt;
    spi_gpi_ctxt *g_ctxt  = (spi_gpi_ctxt *)  h_ctxt->core_iface;
    GPI_RETURN_STATUS gpi_ret_val = GPI_STATUS_SUCCESS;
	spi_plat_device_config *dcfg = (spi_plat_device_config *) h_ctxt->core_config;

    spi_status_t status = SPI_SUCCESS;
    uint16 i;
    uint32 *t_count, *r_count;
    uint32 num_avail_tx_tre;
    uint32 num_avail_rx_tre;
    gpi_ring_elem *t_tre = NULL;
    gpi_ring_elem *r_tre = NULL;

    gpi_ring_elem *tx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[0].ring_base_va;
    gpi_ring_elem *rx_base_tre = (gpi_ring_elem *) g_ctxt->params.chan[1].ring_base_va;

    if ((num_avail_tx_tre = spi_gpi_tre_available (g_ctxt, GPI_OUTBOUND_CHAN, &t_tre)) == 0)
    {
        return SPI_ERROR_DMA_INSUFFICIENT_RESOURCES;
    }
    io_ctxt->tx_tre_req.tre_list = t_tre;

    if ((num_avail_rx_tre = spi_gpi_tre_available (g_ctxt, GPI_INBOUND_CHAN, &r_tre)) == 0)
    {
        return SPI_ERROR_DMA_INSUFFICIENT_RESOURCES;
    }
    io_ctxt->rx_tre_req.tre_list = r_tre;

    t_count = (uint32*) &((io_ctxt->tx_tre_req).num_tre);
    r_count = (uint32*) &((io_ctxt->rx_tre_req).num_tre);

    io_ctxt->tx_tre_req.handle       = g_ctxt->params.gpi_handle;
    io_ctxt->tx_tre_req.chan         = GPI_OUTBOUND_CHAN;

    io_ctxt->rx_tre_req.handle       = g_ctxt->params.gpi_handle;
    io_ctxt->rx_tre_req.chan         = GPI_INBOUND_CHAN;

    io_ctxt->tx_tre_consumed = 0;
    io_ctxt->rx_tre_consumed = 0;

    t_ctxt->eots_rxd = 0;
    t_ctxt->eots_set = 0;

    if(!dcfg->exclusive)
    {
        spi_gpi_set_tre_lock (t_tre);
        ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
    }

    if(!t_ctxt->config_0_sent)
    {
        spi_gpi_set_tre_config_0 (t_tre, c_ctxt);
        ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
        t_ctxt->config_0_sent = 1;
    }

    for (i = 0; i < t_ctxt->num_descs; i++)
    {
        //set the GO tre for every descriptor. We can have
        //tx only, rx only or tx and rx transfers.
        spi_gpi_set_tre_go (t_tre, t_ctxt);

        /* QUP supports 3 non-transfer GO commands:
        1. Keep CS asserted
        2. Keep CS deasserted and
        3. Send free SCLK cycles.
        At this time, none of the descriptors are for purely non-data transfer,
        so we shouldnt have to worry about handling this. The below if condition is
        for completion sake. */

        if (spi_gpi_tre_non_data_opcode (t_tre))
        {
            /*Q: This is the only TRE. Chain will be 0. This probably needs to update
            control bits to enable IEOB*/
            break;
        }
        ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
        {
           spi_descriptor_t *dc = (t_ctxt->desc + t_ctxt->xfer_count);
           if (dc->tx_buf != NULL)
           {
               spi_gpi_set_tre_dma (h_ctxt, t_tre, dc->tx_buf, dc->len);
               t_ctxt->eots_set++;
               ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
           }
           if (dc->rx_buf != NULL)
           {
               spi_gpi_set_tre_dma (h_ctxt, r_tre, dc->rx_buf, dc->len);
               t_ctxt->eots_set++;
               ADVANCE_TRE(RX, *r_count, num_avail_rx_tre, r_tre, rx_base_tre)
           }
        }
        t_ctxt->xfer_count++;
    }

    if(!dcfg->exclusive)
    {
        spi_gpi_set_tre_unlock (t_tre);
        ADVANCE_TRE(TX, *t_count, num_avail_tx_tre, t_tre, tx_base_tre)
    }

    if (*t_count)
    {
        io_ctxt->tx_tre_req.user_data    = c_ctxt;
        /* the tre list may wrap around, instead of special handling for the wrapped-around portion,
        lets just flush the full tre list*/
        spi_plat_mem_flush_cache (tx_base_tre, sizeof(gpi_ring_elem) * MAX_TX_TRE_LIST_SIZE_PER_CORE);
        spi_gpi_dump_tres(io_ctxt->tx_tre_req.tre_list, tx_base_tre, *t_count, TRUE);
    }

    if (*r_count)
    {
        io_ctxt->rx_tre_req.user_data = c_ctxt;
        spi_plat_mem_flush_cache (rx_base_tre, sizeof(gpi_ring_elem) * MAX_RX_TRE_LIST_SIZE_PER_CORE);
        spi_gpi_dump_tres(io_ctxt->rx_tre_req.tre_list, rx_base_tre, *r_count, FALSE);
    }

    if (qurt_island_get_status ())
    {
        spi_island_calls++;
    }

    if (*r_count)
    {
        gpi_ret_val = gpi_process_tre(&io_ctxt->rx_tre_req);
        if(GPI_STATUS_SUCCESS != gpi_ret_val)
        {
            SPI_LOG(LEVEL_ERROR,"spi_gpi_queue_transfer : gpi_process_tre for RX failed with val %d, handle 0x%x",gpi_ret_val, c_ctxt);
            return SPI_ERROR_DMA_PROCESS_TRE_FAIL;
        }
    }

    if (*t_count)
    {
        gpi_ret_val = gpi_process_tre(&io_ctxt->tx_tre_req);
        if(GPI_STATUS_SUCCESS != gpi_ret_val)
        {
            SPI_LOG(LEVEL_ERROR,"spi_gpi_queue_transfer : gpi_process_tre for TX failed with val %d, handle 0x%x",gpi_ret_val, c_ctxt);
            return SPI_ERROR_DMA_PROCESS_TRE_FAIL;
        }
    }

   //if polled mode = FALSE, do nothing else. spi_gpi_callback will be invoked from GSI interrupt. else..
    if (h_ctxt->polled)
    {
        uint32 timeout = GPI_POLLING_TIMEOUT_US;
        while ((t_ctxt->xfer_state != TRANSFER_DONE) && timeout)
        {
            spi_gpi_isr (h_ctxt);
            spi_plat_delay_us (GPI_POLLING_INTERVAL_US);
            timeout -= GPI_POLLING_INTERVAL_US;
        }
        if ((timeout == 0) && (t_ctxt->xfer_state != TRANSFER_DONE))
        {
         //spi_gpi_callback did not happen, we still owe the client a callback.
            status = SPI_ERROR_TRANSFER_TIMEOUT;
            t_ctxt->xfer_state = TRANSFER_TIMED_OUT;
         if(t_ctxt->async)
         {
            spi_notify_completion(t_ctxt->callback, status, t_ctxt->ctxt);
         }
        }
        else if (t_ctxt->callback == NULL)
        {
            // if we did not time out and callback is NULL, update the status
            status = t_ctxt->xfer_status;
        }
    }

    SPI_LOG(LEVEL_VERBOSE,"spi_gpi_queue_transfer : done handle 0x%x, status %d", c_ctxt, status);
    return status;
}

void spi_gpi_isr (void *int_handle)
{
    spi_hw_ctxt *h_ctxt = (spi_hw_ctxt *) int_handle;
    spi_gpi_ctxt *g_ctxt = (spi_gpi_ctxt *)  h_ctxt->core_iface;

    gpi_iface_poll(g_ctxt->params.gpi_handle);
}
