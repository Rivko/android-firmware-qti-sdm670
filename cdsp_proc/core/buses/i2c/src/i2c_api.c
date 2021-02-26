/** 
    @file  i2c_api.c
    @brief I2C implementation
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_api.h"    // implement the apis defined here
#include "i2c_drv.h"    // generic objects for an i2c driver
#include "i2c_iface.h"  // fifo/dma interface definitions
#include "i2c_plat.h"   // platform apis
#include "i2c_log.h"    // logging

hw_ctxt *i2c_cores_head = NULL;

static hw_ctxt *alloc_hw_ctxt (uint32 core_index)
{
    hw_ctxt *h_ctxt = i2c_cores_head;

    plat_device_config  *dcfg;

    while (h_ctxt != NULL)
    {
        dcfg = (plat_device_config *) h_ctxt->core_config;
        if (dcfg->core_index == core_index)
        {
            return h_ctxt;
        }
        h_ctxt = h_ctxt->next;
    }

    h_ctxt = (hw_ctxt *) plat_mem_alloc (core_index,
                                         sizeof(hw_ctxt),
                                         HW_CTXT_TYPE);

    if (h_ctxt != NULL)
    {
        // attach to the head of the list
        h_ctxt->next    = i2c_cores_head;
        i2c_cores_head  = h_ctxt;
    }

    return h_ctxt;
}

static void free_hw_ctxt (hw_ctxt *h_ctxt)
{
    hw_ctxt *h_curr = i2c_cores_head;
    hw_ctxt *prev = NULL;
    boolean core_found = FALSE;

    while (h_curr != NULL)
    {
        if (h_curr == h_ctxt)
        {
            core_found = TRUE;
            break;
        }
        prev = h_curr;
        h_curr = h_curr->next;
    }

    if (core_found)
    {
        if (prev != NULL)
        {
            // removal from end or middle
            prev->next = h_curr->next;
        }
        else
        {
            // removal from head
            i2c_cores_head = h_curr->next;
        }

        plat_mem_free (h_curr, sizeof(hw_ctxt), HW_CTXT_TYPE);
    }
}

i2c_status init_hw_ctxt (uint8 core_i, hw_ctxt *h_ctxt)
{
    i2c_status i_status = I2C_SUCCESS;
    plat_device_config *dcfg;

    // get configuration
    dcfg = plat_get_device_config (core_i);
    if (dcfg == NULL)
    {
        return I2C_ERROR_PLATFORM_GET_CONFIG_FAIL;
    }
    h_ctxt->core_config = dcfg;

    // create an instance lock
    h_ctxt->core_lock = plat_mutex_instance_init (core_i, CRIT_SEC_TYPE);
    if (h_ctxt->core_lock == NULL)
    {
        return I2C_ERROR_PLATFORM_CRIT_SEC_FAIL;
    }

    // create a lock for the queue
    h_ctxt->queue_lock = plat_mutex_instance_init (core_i, Q_MUTEX_TYPE);
    if (h_ctxt->queue_lock == NULL)
    {
        return I2C_ERROR_PLATFORM_CRIT_SEC_FAIL;
    }

    // register interrupt
    if (!plat_interrupt_register (dcfg, bus_iface_isr, h_ctxt))
    {
        return I2C_ERROR_PLATFORM_REG_INT_FAIL;
    }
    h_ctxt->core_state |= I2C_DRV_HW_INTERRUPT_REGISTERED;

    // initialize bus interface
    if (plat_clock_enable (dcfg) == FALSE)
    {
        return I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL;
    }

    i_status = bus_iface_init (h_ctxt);

    if (plat_clock_disable(dcfg) == FALSE)
    {
        return I2C_ERROR_PLATFORM_CLOCK_DISABLE_FAIL;
    }

    if (I2C_SUCCESS(i_status))
    {
        h_ctxt->core_state |= I2C_DRV_BUS_INTERFACE_INITIALIZED;
    }

    return i_status;
}

i2c_status de_init_hw_ctxt (hw_ctxt *h_ctxt)
{
    plat_device_config *dcfg = (plat_device_config *) h_ctxt->core_config;
    i2c_status i_status = I2C_SUCCESS;

    // destroy the instance lock
    if (h_ctxt->core_lock != NULL)
    {
        plat_mutex_instance_de_init (h_ctxt->core_lock, CRIT_SEC_TYPE);
    }

    // destroy the lock for the queue
    if (h_ctxt->queue_lock != NULL)
    {
        plat_mutex_instance_de_init (h_ctxt->queue_lock, Q_MUTEX_TYPE);
    }

    // de register the interrupt
    if (h_ctxt->core_state & I2C_DRV_HW_INTERRUPT_REGISTERED)
    {
        plat_interrupt_deregister (dcfg);
        h_ctxt->core_state &= ~(I2C_DRV_HW_INTERRUPT_REGISTERED);
    }

    // de initialize the bus interface
    if (h_ctxt->core_state & I2C_DRV_BUS_INTERFACE_INITIALIZED)
    {
        plat_clock_enable (dcfg);
        i_status = bus_iface_de_init (h_ctxt);
        plat_clock_disable(dcfg);

        h_ctxt->core_state &= ~(I2C_DRV_BUS_INTERFACE_INITIALIZED);
    }

    // release device configuration
    if (dcfg != NULL)
    {
        plat_release_device_config (dcfg->core_index, dcfg);
    }

    return i_status;
}

void de_init_client_ctxt (client_ctxt *c_ctxt)
{
    if (c_ctxt->t_ctxt.signal != NULL)
    {
        plat_signal_free (c_ctxt->t_ctxt.signal);
    }
    if (c_ctxt->t_ctxt.io_ctxt != NULL)
    {
        bus_iface_release_io_ctxt (c_ctxt->t_ctxt.io_ctxt);
    }
    plat_mem_free (c_ctxt, sizeof(client_ctxt), CLIENT_CTXT_TYPE);
}

i2c_status init_client_ctxt (uint8 core_i, client_ctxt **clnt_ctxt)
{
    client_ctxt *c_ctxt;

    // alloc client context
    c_ctxt = (client_ctxt *) plat_mem_alloc (core_i,
                                             sizeof(client_ctxt),
                                             CLIENT_CTXT_TYPE);
    if (c_ctxt == NULL)
    {
        return I2C_ERROR_HANDLE_ALLOCATION;
    }

    // initialize the client handle
    c_ctxt->t_ctxt.signal = plat_signal_alloc (core_i);
    if (c_ctxt->t_ctxt.signal == NULL)
    {
        return I2C_ERROR_PLATFORM_SIGNAL_FAIL;
    }
    c_ctxt->t_ctxt.io_ctxt = bus_iface_get_io_ctxt (core_i);
    if (c_ctxt->t_ctxt.io_ctxt == NULL)
    {
        return I2C_ERROR_IO_CTXT_ALLOCATION;
    }

    *clnt_ctxt = c_ctxt;

    return I2C_SUCCESS;
}

i2c_status i2c_open(i2c_instance instance, void **i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;
    client_ctxt *c_ctxt = NULL;
    hw_ctxt *h_ctxt = NULL;
    uint8 core_i = (uint8) instance;

    I2C_LOG(LEVEL_INFO, "open core %d", instance);
    I2C_LOG(LEVEL_PERF, I2C_API_OPEN, instance);

    // param validation
    if (i2c_handle == NULL)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR open invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }
    *i2c_handle = NULL;

    if (plat_in_exception_context())
    {
        return I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
    }

    plat_mutex_global_lock();

    // get or allocate a hw context
    h_ctxt = alloc_hw_ctxt (core_i);
    if (h_ctxt == NULL)
    {
        i_status = I2C_ERROR_HW_INFO_ALLOCATION;
        goto exit;
    }

    // initialize the hw context if needed
    if (h_ctxt->core_ref_count == 0)
    {
        i_status = init_hw_ctxt (core_i, h_ctxt);
        if (I2C_ERROR(i_status))
        {
            goto exit;
        }
    }

    i_status = init_client_ctxt (core_i, &c_ctxt);
    if (I2C_ERROR(i_status))
    {
        goto exit;
    }

    c_ctxt->h_ctxt = h_ctxt;
    h_ctxt->core_ref_count++;
    *i2c_handle = c_ctxt;

    I2C_LOG(LEVEL_INFO, "open handle 0x%08x", c_ctxt);

exit: 

    if (I2C_ERROR(i_status))
    {
        I2C_LOG(LEVEL_ERROR, "open core %d, ERROR %d", instance, i_status);
        if ((h_ctxt != NULL) && (h_ctxt->core_ref_count == 0))
        {
            if (I2C_SUCCESS != de_init_hw_ctxt (h_ctxt))
            {
                I2C_LOG(LEVEL_ERROR, "de_init_hw_ctxt failed on ERROR clean up");
            }
            free_hw_ctxt (h_ctxt);
        }

        if (c_ctxt != NULL)
        {
            de_init_client_ctxt (c_ctxt);
        }
    }

    plat_mutex_global_unlock();

    return i_status;
}

i2c_status i2c_close(void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    i2c_status i_status = I2C_SUCCESS;
    plat_device_config *dcfg;
    uint8 core_i;

    I2C_LOG(LEVEL_PERF, I2C_API_CLOSE, i2c_handle);

    if (plat_in_exception_context())
    {
        return I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
    }

    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        I2C_LOG(LEVEL_ERROR, "ERROR close invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }

    h_ctxt = c_ctxt->h_ctxt;

    plat_mutex_global_lock();

    dcfg = (plat_device_config *) h_ctxt->core_config;
    core_i = dcfg->core_index;

    I2C_LOG(LEVEL_INFO, "close core %d", core_i);

    if (h_ctxt->core_ref_count)
    {
        h_ctxt->core_ref_count--;
        if (h_ctxt->core_ref_count == 0)
        {
            i_status = de_init_hw_ctxt (h_ctxt);
            free_hw_ctxt (h_ctxt);
        }
    }

    if (c_ctxt != NULL)
    {
        de_init_client_ctxt (c_ctxt);
    }

    I2C_LOG(LEVEL_INFO, "close handle 0x%08x", c_ctxt);

    plat_mutex_global_unlock();

    return i_status;
}

i2c_status i2c_power_on (void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    plat_device_config *dcfg;
    bus_iface_hw_ctxt *g_ctxt;

    I2C_LOG(LEVEL_PERF, I2C_API_POWER_ON, i2c_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_iface == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        I2C_LOG(LEVEL_ERROR, "ERROR on invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (plat_device_config *) h_ctxt->core_config;
    g_ctxt = h_ctxt->core_iface;

    plat_mutex_instance_lock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);
    if (h_ctxt->power_ref_count == 0)
    {
        I2C_LOG(LEVEL_INFO, "ON 0x%08x", i2c_handle);
        if (plat_clock_enable(dcfg) == FALSE)
        {
            return I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL;
        }
        gpi_iface_active(g_ctxt->params.gpi_handle, TRUE);
        if (plat_gpio_enable (dcfg) == FALSE)
        {
            return I2C_ERROR_PLATFORM_GPIO_ENABLE_FAIL;
        }
    }
    h_ctxt->power_ref_count++;
    plat_mutex_instance_unlock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    return I2C_SUCCESS;
}

i2c_status i2c_power_off (void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    plat_device_config *dcfg;
    bus_iface_hw_ctxt *g_ctxt;

    I2C_LOG(LEVEL_PERF, I2C_API_POWER_OFF, i2c_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_iface == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        I2C_LOG(LEVEL_ERROR, "ERROR off invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (plat_device_config *) h_ctxt->core_config;
    g_ctxt = h_ctxt->core_iface;

    plat_mutex_instance_lock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);
    if (h_ctxt->power_ref_count)
    {
        h_ctxt->power_ref_count--;
        if (h_ctxt->power_ref_count == 0)
        {
            I2C_LOG(LEVEL_INFO, "OFF 0x%08x", i2c_handle);
            gpi_iface_active(g_ctxt->params.gpi_handle, FALSE);
            if (plat_clock_disable(dcfg) == FALSE)
            {
                return I2C_ERROR_PLATFORM_CLOCK_DISABLE_FAIL;
            }
            if (plat_gpio_disable (dcfg) == FALSE)
            {
                return I2C_ERROR_PLATFORM_GPIO_DISABLE_FAIL;
            }
        }
    }
    plat_mutex_instance_unlock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    return I2C_SUCCESS;
}

i2c_status i2c_setup_lpi_resource (i2c_instance instance, uint32 bus_frequency_khz)
{
    i2c_status i_status = I2C_SUCCESS;
    plat_device_config *dcfg;

    I2C_LOG(LEVEL_INFO, "setup lpi rsc %d:%d", instance, bus_frequency_khz);
    I2C_LOG(LEVEL_PERF, I2C_API_LPI_RESOURCE_ON, i2c_handle);

    if (bus_frequency_khz == 0)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR lpi setup invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }

    plat_mutex_global_lock();
    dcfg = plat_get_device_config (instance);
    if (dcfg == NULL)
    {
        return I2C_ERROR_PLATFORM_GET_CONFIG_FAIL;
    }

    if (FALSE == plat_setup_se_clock (dcfg, bus_frequency_khz))
    {
        i_status = I2C_ERROR_PLATFORM_RESOURCE_SETUP_FAIL;
    }
    plat_mutex_global_unlock();

    return i_status;
}

i2c_status i2c_reset_lpi_resource (i2c_instance instance)
{
    i2c_status i_status = I2C_SUCCESS;
    plat_device_config *dcfg;

    I2C_LOG(LEVEL_INFO, "reset lpi rsc %d", instance);
    I2C_LOG(LEVEL_PERF, I2C_API_LPI_RESOURCE_OFF, i2c_handle);

    plat_mutex_global_lock();
    dcfg = plat_get_device_config (instance);
    if (dcfg == NULL)
    {
        return I2C_ERROR_PLATFORM_GET_CONFIG_FAIL;
    }

    if (FALSE == plat_unset_se_clock (dcfg))
    {
        i_status = I2C_ERROR_PLATFORM_RESOURCE_RESET_FAIL;
    }
    plat_mutex_global_unlock();

    return i_status;
}

i2c_status i2c_cancel_transfer (void *i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;
    client_ctxt     *c_ctxt = (client_ctxt *) i2c_handle;
    client_ctxt     *c_curr = NULL, *c_prev = NULL;
    transfer_ctxt   *t_ctxt;
    hw_ctxt         *h_ctxt;
    boolean         found = FALSE;

    I2C_LOG(LEVEL_INFO, "cancel 0x%08x", i2c_handle);
    I2C_LOG(LEVEL_PERF, I2C_API_CANCEL, i2c_handle);

    if (plat_in_exception_context())
    {
        return I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
    }

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        I2C_LOG(LEVEL_ERROR, "ERROR cancel invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }
    h_ctxt = c_ctxt->h_ctxt;

    plat_mutex_instance_lock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    c_curr = h_ctxt->client_ctxt_head;
    while (c_curr != NULL)
    {
        if (c_curr == c_ctxt) { found = TRUE; break; }
        c_prev = c_curr;
        c_curr = c_curr->next;
    }
    if (!found) { i_status = I2C_TRANSFER_INVALID; goto exit; }

    t_ctxt = &c_ctxt->t_ctxt;
    if (t_ctxt->transfer_state == TRANSFER_DONE)
    {
        i_status = I2C_TRANSFER_COMPLETED;
        goto exit;
    }

    if (c_prev == NULL)
    {
        i_status = bus_iface_cancel (h_ctxt, c_ctxt);
        if (I2C_SUCCESS(i_status))
        {
            t_ctxt->transfer_state  = TRANSFER_CANCELED;
            t_ctxt->transfer_status = I2C_TRANSFER_FORCE_TERMINATED;
            i_status = I2C_TRANSFER_FORCE_TERMINATED;
        }
    }
    else
    {
        c_prev->next = c_curr->next;
    }
    c_curr->next = NULL;

exit: 

    plat_mutex_instance_unlock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    return i_status;
}

i2c_status i2c_lock (void *i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;

    return i_status;
}

i2c_status i2c_unlock (void *i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;

    return i_status;
}

uint32 i2c_get_start_timestamp (void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    if (c_ctxt == NULL)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR s_ts invalid param");
        return 0;
    }
    return c_ctxt->t_ctxt.start_bit_timestamp;
}

uint32 i2c_get_stop_timestamp (void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    if (c_ctxt == NULL)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR p_ts invalid param");
        return 0;
    }
    return c_ctxt->t_ctxt.stop_bit_timestamp;
}

static boolean i2c_validate_transfer_params (
        client_ctxt *c_ctxt,
        i2c_slave_config *config,
        i2c_descriptor *desc,
        uint16 num_descriptors,
        callback_func func,
        void *ctxt
        )
{
    uint16 i;
    uint8 noise_reject_level;
    boolean direction_change = TRUE;

    if ((c_ctxt == NULL)                        ||
        (c_ctxt->h_ctxt == NULL)                ||
        (c_ctxt->h_ctxt->core_config == NULL)   ||
        (config == NULL)                        ||
        (num_descriptors == 0))
    {
        return FALSE;
    }

    // log parameters
    I2C_LOG(LEVEL_VERBOSE, "bus_frequency_khz %d",
            config->bus_frequency_khz);
    I2C_LOG(LEVEL_VERBOSE, "slave_address 0x%02x",
            config->slave_address);
    I2C_LOG(LEVEL_VERBOSE, "mode %d",
            config->mode);
    I2C_LOG(LEVEL_VERBOSE, "slave_max_clock_stretch_us %d",
            config->slave_max_clock_stretch_us);
    I2C_LOG(LEVEL_VERBOSE, "core_configuration1 %d",
            config->core_configuration1);
    I2C_LOG(LEVEL_VERBOSE, "core_configuration2 %d",
            config->core_configuration2);
    I2C_LOG(LEVEL_VERBOSE, "num_descriptors %d",
            num_descriptors);
    I2C_LOG(LEVEL_INFO, "func 0x%08x: ctxt 0x%08x",
            func, ctxt);

    // validate descriptors
    for (i = 0; i < num_descriptors; i++)
    {
        uint8 has_start, has_stop, has_ts_s, has_ts_p, has_read, has_write;

        if (desc == NULL)
        {
            return I2C_ERROR_INVALID_PARAMETER;
        }

        /*
			If this is a Address Query only, then we expect only single desc with no length & only Start Flag set.
		*/
		if ((desc->length == 0) && (num_descriptors == 1) && (!(desc->flags^I2C_FLAG_START)))
        {
            // address query
			I2C_LOG(LEVEL_INFO, "Address Query for slave_address 0x%02x", config->slave_address);
            break;
        }

        if (desc->buffer == NULL)
        {
            return I2C_ERROR_INVALID_PARAMETER;
        }


        I2C_LOG(LEVEL_INFO, "[%d] buffer 0x%08x: length %d: flags 0x%08x",
                i, desc->buffer, desc->length, desc->flags);

        has_start   = desc->flags & I2C_FLAG_START;
        has_stop    = desc->flags & I2C_FLAG_STOP;
        has_write   = desc->flags & I2C_FLAG_WRITE;
        has_read    = desc->flags & I2C_FLAG_READ;
        has_ts_s    = desc->flags & I2C_FLAG_TIMESTAMP_S;
        has_ts_p    = desc->flags & I2C_FLAG_TIMESTAMP_P;

        if ((!(has_read || has_write))  ||
            (has_ts_s && !has_start)    ||
            (has_ts_p && !has_stop)     ||
            (direction_change && !has_start))
        {
            return I2C_ERROR_INVALID_PARAMETER;
        }

        if (i < (num_descriptors - 1))
        {
            desc++;
            // note that here you are checking the next transfer
            if (((desc->flags & I2C_FLAG_WRITE) == has_write) || 
                ((desc->flags & I2C_FLAG_READ)  == has_read))
            {
                direction_change = FALSE;
            }
            else
            {
                direction_change = TRUE;
            }
        }
    }

    // support only 100 KHz, 400 KHz and 1000 KHz
    if ((config->mode == I2C) || (config->mode == SMBUS))
    {
        if ((config->bus_frequency_khz != I2C_STANDARD_MODE_FREQ_KHZ)   &&
            (config->bus_frequency_khz != I2C_FAST_MODE_FREQ_KHZ)       &&
            (config->bus_frequency_khz != I2C_FAST_MODE_PLUS_FREQ_KHZ))
        {
            return FALSE;
        }

        // validate noise config
        noise_reject_level = I2C_SLAVE_CONFIG_1_GET(config->core_configuration1,
                                                    NOISE_REJECT_LEVEL);
        if ((noise_reject_level >  0)   &&
            (noise_reject_level != 50)  &&
            (noise_reject_level != 100) &&
            (noise_reject_level != 150))
        {
            return FALSE;
        }
    }

    return TRUE;
}

i2c_status
i2c_transfer
(
    void *i2c_handle,
    i2c_slave_config *config,
    i2c_descriptor *desc,
    uint16 num_descriptors,
    callback_func func,
    void *ctxt,
    uint32 delay,
    uint32 *transferred
)
{
    i2c_status i_status = I2C_SUCCESS;

    client_ctxt         *c_ctxt = (client_ctxt *) i2c_handle;
    transfer_ctxt       *t_ctxt;
    plat_device_config  *dcfg = NULL;
    hw_ctxt             *h_ctxt;
    sync_type           sync = SYNC_WITH_THREAD;

    boolean send_transfer_to_hw         = FALSE;
    boolean use_signal_for_completion   = FALSE;
    boolean polled_mode                 = FALSE;
    boolean synchronous_mode            = FALSE;
    boolean has_internal_queueing       = FALSE;

    I2C_LOG(LEVEL_PERF, I2C_API_TRANSFER, i2c_handle);

    if (plat_in_exception_context())
    {
        return I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
    }

    if (i2c_validate_transfer_params(
                c_ctxt,
                config,
                desc,
                num_descriptors,
                func,
                ctxt) == FALSE)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR xfer invalid param");
        return I2C_ERROR_INVALID_PARAMETER;
    }

    h_ctxt = c_ctxt->h_ctxt;
        // protect multiple threads calling i2c_transfer
    plat_mutex_instance_lock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    // queue transfer 
    // ==============================================================
    plat_mutex_instance_lock (h_ctxt->queue_lock, EL_THREAD, sync);
    // don't check tail. head == null means no active transfers
    if (h_ctxt->client_ctxt_head == NULL)
    {
        h_ctxt->client_ctxt_head = c_ctxt;
        h_ctxt->client_ctxt_tail = c_ctxt;
        send_transfer_to_hw = TRUE;
    }
    else
    {
        // check if the same client is queueing twice
        client_ctxt *temp = h_ctxt->client_ctxt_head;
        while (temp != NULL)
        {
            if (temp == c_ctxt)
            {
                i_status = I2C_ERROR_HANDLE_ALREADY_IN_QUEUE;
                plat_mutex_instance_unlock (h_ctxt->queue_lock, EL_THREAD, sync);
                goto exit;
            }
            temp = temp->next;
        }

        // if the client handle is not in queue, then add it
        if (i_status != I2C_ERROR_HANDLE_ALREADY_IN_QUEUE)
        {
            h_ctxt->client_ctxt_tail->next = c_ctxt;
            h_ctxt->client_ctxt_tail = c_ctxt;
        }
    }
    plat_mutex_instance_unlock (h_ctxt->queue_lock, EL_THREAD, sync);
    
    t_ctxt = &(c_ctxt->t_ctxt);
    dcfg   = (plat_device_config *) h_ctxt->core_config;

    polled_mode = dcfg->polled_mode;

    if ((func == NULL) && !(polled_mode))
    {
        use_signal_for_completion = TRUE;
    }

    if ((func == NULL) || polled_mode)
    {
        synchronous_mode = TRUE;
    }

    has_internal_queueing   = bus_iface_has_queueing(h_ctxt);
    if (bus_iface_has_interrupt_consumer(h_ctxt))
    {
        sync = SYNC_WITH_INTERRUPT;
    }

    
    t_ctxt->config      = config;
    t_ctxt->desc        = desc;
    t_ctxt->num_descs   = num_descriptors;
    t_ctxt->delay       = delay;
    t_ctxt->callback    = func;
    t_ctxt->ctxt        = ctxt;

    t_ctxt->transfer_count          = 0;
    t_ctxt->transferred             = 0;
    t_ctxt->transfer_state          = TRANSFER_INVALID;
    t_ctxt->transfer_status         = I2C_SUCCESS;

    t_ctxt->start_bit_timestamp     = 0;
    t_ctxt->stop_bit_timestamp      = 0;

    c_ctxt->next = NULL;


    // ==============================================================

    I2C_LOG(LEVEL_VERBOSE, "send_transfer_to_hw         = %d", send_transfer_to_hw);
    I2C_LOG(LEVEL_VERBOSE, "use_signal_for_completion   = %d", use_signal_for_completion);
    I2C_LOG(LEVEL_VERBOSE, "polled_mode                 = %d", polled_mode);
    I2C_LOG(LEVEL_VERBOSE, "has_internal_queueing       = %d", has_internal_queueing);
    I2C_LOG(LEVEL_VERBOSE, "synchronous_mode            = %d", synchronous_mode);

    if (I2C_SUCCESS(i_status))
    {
        // for synchronous interrupt based transfers, create signal
        if (use_signal_for_completion)
        {
            plat_signal_init (&(t_ctxt->signal));
        }

        // queue the transfer to hw only if nothing is running
        if ((send_transfer_to_hw == TRUE) || has_internal_queueing)
        {
            i_status = bus_iface_queue_transfer (c_ctxt);
        }

        if (I2C_SUCCESS(i_status))
        {
            if (use_signal_for_completion)
            {
                plat_wait_for_event (t_ctxt->signal);
                plat_signal_de_init (&(t_ctxt->signal));
                i_status = t_ctxt->transfer_status;
            }
        }
        else
        {
            plat_mutex_instance_lock (h_ctxt->queue_lock, EL_THREAD, sync);
            // remove the client from the queue
            client_ctxt *c_curr = h_ctxt->client_ctxt_head;
            client_ctxt *c_prev = NULL;
            while (c_curr != NULL)
            {
                if (c_curr == c_ctxt)
                {
                    if (c_prev == NULL) { h_ctxt->client_ctxt_head = c_curr->next; }
                    else                { c_prev->next = c_curr->next; }
                    c_curr->next = NULL;
                }
                c_prev = c_curr;
                c_curr = c_curr->next;
            }
            plat_mutex_instance_unlock (h_ctxt->queue_lock, EL_THREAD, sync);
        }

        if ((transferred != NULL) && synchronous_mode)
        {
            *transferred = t_ctxt->transferred;
        }
    }
    
exit:
    plat_mutex_instance_unlock (h_ctxt->core_lock, EL_THREAD, SYNC_WITH_THREAD);

    I2C_LOG(LEVEL_PERF, I2C_API_TRANSFER_RETURN, i2c_handle);

    return i_status; 
}
