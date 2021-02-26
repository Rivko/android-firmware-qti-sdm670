
/*=============================================================================   
    @file  i2c_api.c
    @brief I2C public API implementation
   
    Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 30/08/17   ska     Fixed number of bytes read/written count
 13/07/17   vg      Fixed KW issues
 31/05/17   vg      Copyright & API  updated 
=============================================================================*/

#include "i2c_api.h"    // implement the apis defined here
#include "i2c_drv.h"    // generic objects for an i2c driver
#include "i2c_iface.h"  // fifo/dma interface definitions
#include "i2c_plat.h"   // platform apis
#include "i2c_log.h"    // logging
#include <string.h>

hw_ctxt *i2c_cores_head = NULL;

static void clear_hw_ctxt (hw_ctxt *h_ctxt)
{
    h_ctxt->core_config         = NULL;
    h_ctxt->core_debug          = NULL;
    h_ctxt->core_iface          = NULL;
    h_ctxt->core_lock           = NULL;
    h_ctxt->core_signal         = NULL;
    h_ctxt->queue_lock          = NULL;
    h_ctxt->client_ctxt_head    = NULL;
    h_ctxt->client_ctxt_tail    = NULL;
    h_ctxt->core_ref_count      = 0;
    h_ctxt->power_ref_count     = 0;
    h_ctxt->core_state          = 0;
    h_ctxt->next                = NULL;
}

static hw_ctxt *alloc_hw_ctxt (uint32 core_index)
{
    hw_ctxt *h_ctxt = i2c_cores_head;
    plat_device_config  *dcfg = NULL;

    while (h_ctxt != NULL)
    {
        dcfg = (plat_device_config *) h_ctxt->core_config;
        if (dcfg->core_index == core_index)
        {
            goto exit;
        }
        h_ctxt = h_ctxt->next;
    }

    h_ctxt = (hw_ctxt *) plat_mem_alloc (core_index,
                                         sizeof(hw_ctxt),
                                         HW_CTXT_TYPE);
    if (h_ctxt != NULL)
    {
        clear_hw_ctxt (h_ctxt);

        // attach to the head of the list
        h_ctxt->next    = i2c_cores_head;
        i2c_cores_head  = h_ctxt;
    }

exit:
    return h_ctxt;
}

static void free_hw_ctxt (uint32 core_index)
{
    hw_ctxt *h_ctxt = i2c_cores_head;
    hw_ctxt *prev = NULL;
    boolean core_found = FALSE;
    plat_device_config  *dcfg = NULL;

    while ((h_ctxt != NULL) && (h_ctxt->core_config != NULL))
    {
        dcfg = (plat_device_config *) h_ctxt->core_config;
        if (dcfg->core_index == core_index)
        {
            core_found = TRUE;
            break;
        }
        prev = h_ctxt;
        h_ctxt = h_ctxt->next;
    }

    if (core_found)
    {
        if (prev != NULL)
        {
            // removal from end or middle
            prev->next = h_ctxt->next;
        }
        else
        {
            // removal from head
            i2c_cores_head = h_ctxt->next;
        }

        clear_hw_ctxt (h_ctxt);
        plat_mem_free (h_ctxt, sizeof(hw_ctxt), HW_CTXT_TYPE);
    }
}

i2c_status init_hw_ctxt (hw_ctxt *h_ctxt, plat_device_config *dcfg)
{
    i2c_status i_status = I2C_SUCCESS;
    uint8 core_i = 0;

    if( (h_ctxt == NULL) ||
        (dcfg == NULL) )
    {
        i_status = I2C_ERROR_INVALID_PARAMETER;
        goto exit;
    }

    h_ctxt->core_config = dcfg;
    core_i = dcfg->core_index;

    // create an instance lock
    h_ctxt->core_lock = plat_mutex_instance_init (core_i);
    if (h_ctxt->core_lock == NULL)
    {
        i_status = I2C_ERROR_PLATFORM_CRIT_SEC_FAIL;
		goto exit;
    }

    // create a lock for the queue
    h_ctxt->queue_lock = plat_mutex_instance_init (core_i);
    if (h_ctxt->queue_lock == NULL)
    {
        i_status = I2C_ERROR_PLATFORM_CRIT_SEC_FAIL;
		goto exit;
    }

    // create a signal
    h_ctxt->core_signal = plat_signal_alloc (core_i);
    if (h_ctxt->core_signal == NULL)
    {
        i_status = I2C_ERROR_PLATFORM_SIGNAL_FAIL;
		goto exit;
    }

    // enable interrupt
    if (!plat_interrupt_register (dcfg, bus_iface_isr, h_ctxt))
    {
        i_status = I2C_ERROR_PLATFORM_REG_INT_FAIL;
		goto exit;
    }
    h_ctxt->core_state |= I2C_DRV_HW_INTERRUPT_REGISTERED;
    
    // enable clocks
    if (plat_clock_enable(dcfg) == FALSE)
    {
      i_status = I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL;
	  goto exit;
    }

    // enable gpio's
    if (plat_gpio_enable(dcfg) == FALSE)
    {
      i_status = I2C_ERROR_PLATFORM_GPIO_ENABLE_FAIL;
	  goto exit;
    }

    // initialize bus interface
    i_status = bus_iface_init (h_ctxt);
    if (I2C_SUCCESS(i_status))
    {
        h_ctxt->core_state |= I2C_DRV_BUS_INTERFACE_INITIALIZED;
    }

exit:
    return i_status;
}

void de_init_hw_ctxt (hw_ctxt *h_ctxt, plat_device_config *dcfg)
{
    if (h_ctxt->core_lock != NULL)
    {
        plat_mutex_instance_de_init (h_ctxt->core_lock);
    }

    if (h_ctxt->queue_lock != NULL)
    {
        plat_mutex_instance_de_init (h_ctxt->queue_lock);
    }

    if (h_ctxt->core_state & I2C_DRV_HW_INTERRUPT_REGISTERED)
    {
        plat_interrupt_deregister (dcfg);
        h_ctxt->core_state &= ~(I2C_DRV_HW_INTERRUPT_REGISTERED);
    }

    if (h_ctxt->core_state & I2C_DRV_BUS_INTERFACE_INITIALIZED)
    {
        bus_iface_de_init (h_ctxt);
        h_ctxt->core_state &= ~(I2C_DRV_BUS_INTERFACE_INITIALIZED);
    }

    if (h_ctxt->core_signal != NULL)
    {
        plat_signal_free (h_ctxt->core_signal);
    }
}

i2c_status i2c_open(i2c_instance instance, void **i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;
    client_ctxt *c_ctxt = NULL;
    hw_ctxt *h_ctxt = NULL;
    plat_device_config *dcfg = NULL;
    uint8 core_i = (uint8) instance;
//    boolean interrupt_registered = FALSE;
//    boolean iface_inited = FALSE;

    I2C_LOG(LEVEL_INFO, "\n[I2C] open core %d\n", instance);

    if (plat_in_exception_context())
    {
        i_status = I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
		goto exit;
    }

    // param validation
    if (i2c_handle == NULL) 
    {
        i_status = I2C_ERROR_INVALID_PARAMETER;
		goto exit;
    }

	*i2c_handle = NULL;

    plat_mutex_global_lock();

    // get configuration
    dcfg = plat_get_device_config (core_i);
    if (dcfg == NULL)
    {
        i_status = I2C_ERROR_PLATFORM_GET_CONFIG_FAIL;
        goto exit;
    }

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
        i_status = init_hw_ctxt (h_ctxt, dcfg);
        if (I2C_ERROR(i_status))
        {
            goto exit;
        }
    }

    // alloc client context
    c_ctxt = (client_ctxt *) plat_mem_alloc (core_i,
                                             sizeof(client_ctxt),
                                             CLIENT_CTXT_TYPE);
    if (c_ctxt == NULL)
    {
        i_status = I2C_ERROR_HANDLE_ALLOCATION;
        goto exit;
    }

    // initialize the client handle
    h_ctxt->core_ref_count++;
    c_ctxt->h_ctxt = h_ctxt;
    *i2c_handle = c_ctxt;

    I2C_LOG(LEVEL_INFO, "\n[I2C] open handle 0x%08X\n", c_ctxt);

exit: 

    if (I2C_ERROR(i_status))
    {
        if ((h_ctxt != NULL) && (h_ctxt->core_ref_count == 0))
        {
            de_init_hw_ctxt (h_ctxt, dcfg);
            free_hw_ctxt (core_i);
        }

        if (c_ctxt != NULL)
        {
            plat_mem_free (c_ctxt, sizeof(client_ctxt), CLIENT_CTXT_TYPE);
        }
    }

    plat_mutex_global_unlock();

    return i_status;
}

/* i2c_read */
i2c_status
i2c_read(
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *read,
    uint32 timeout_ms
)
{

	uint8 reg[2] = {0};
	uint32 bytes_read = 0;
	i2c_status result = I2C_SUCCESS;
    i2c_descriptor *desc;
    i2c_descriptor *curr_desc;

	desc = (i2c_descriptor *)plat_sys_mem_alloc(2*sizeof(i2c_descriptor));
	if(!desc) {
		result = I2C_ERROR_INVALID_PARAMETER;
		goto exit;
	}
	memset(desc,0,sizeof(i2c_descriptor));

	reg[0] = (uint8)(offset >> 8);
	reg[1] = (uint8) offset;

	curr_desc = desc;

	curr_desc->buffer = (offset_len == 1 ? &reg[1] : reg);
	curr_desc->length = offset_len;
	curr_desc->flags = I2C_FLAG_START | I2C_FLAG_WRITE;

	curr_desc++;
		
    curr_desc->buffer = buffer;
    curr_desc->length = buffer_len;
    curr_desc->flags = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

	result = i2c_transfer(i2c_handle, config, desc, 2, NULL, NULL, 0, &bytes_read);
	if(I2C_SUCCESS == result) 
	{
		*read = bytes_read - offset_len;
	}

	plat_sys_mem_dealloc(desc);
exit:
	return result;
	
}

/* i2c_write */
i2c_status
i2c_write(
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *written,
    uint32 timeout_ms
)
{
	uint32 bytes_written = 0;
	i2c_status result = I2C_SUCCESS;
    i2c_descriptor *desc;

	desc = (i2c_descriptor *)plat_sys_mem_alloc(sizeof(i2c_descriptor));
	if(!desc) {
		result = I2C_ERROR_MEM_ALLOC_FAIL;
		goto ON_EXIT;
	}
	memset(desc,0,sizeof(i2c_descriptor));
	
	desc->buffer = (uint8*)plat_sys_mem_alloc((buffer_len+offset_len)*sizeof(uint8));
	
	if(!desc->buffer)
	{
		result = I2C_ERROR_MEM_ALLOC_FAIL;
		goto ON_EXIT;
	}

    if(offset_len == 1)
    {
      desc->buffer[0] = (uint8) offset;
    }
    else if (offset_len == 2)
    {
      desc->buffer[0] = (uint8)(offset >> 8);
      desc->buffer[1] = (uint8) offset;
    }
    else
    {
	    // No offset condition, 
    }
	
	memcpy(desc->buffer+offset_len, buffer, buffer_len);
	desc->length = buffer_len + offset_len;
	desc->flags = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP;

	result =  i2c_transfer(i2c_handle, config, desc, 1, NULL, NULL, 0, &bytes_written);
	if(I2C_SUCCESS == result) 
	{
		*written = bytes_written - offset_len;
	}

ON_EXIT:
    if(desc && desc->buffer)
	{
	    plat_sys_mem_dealloc(desc->buffer);
	}
	if(desc)
	{
	    plat_sys_mem_dealloc(desc);
	}
	return result;
}

i2c_status i2c_close(void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    i2c_status i_status = I2C_SUCCESS;
    plat_device_config *dcfg;

    if (plat_in_exception_context())
    {
        i_status = I2C_ERROR_API_INVALID_EXECUTION_LEVEL;
		goto exit;
    }

    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        i_status = I2C_ERROR_INVALID_PARAMETER;
		goto exit;
    }

    h_ctxt = c_ctxt->h_ctxt;

    plat_mutex_global_lock();

    I2C_LOG(LEVEL_INFO, "\n[I2C] close core %d\n", i2c_handle);

    dcfg = (plat_device_config *) h_ctxt->core_config;
    if (h_ctxt->core_ref_count)
    {
        h_ctxt->core_ref_count--;
        if (h_ctxt->core_ref_count == 0)
        {
            de_init_hw_ctxt (h_ctxt, dcfg);
            free_hw_ctxt (dcfg->core_index);
        }
    }
    
	I2C_LOG(LEVEL_INFO, "\n[I2C] close handle 0x%08X\n", c_ctxt);
    if (c_ctxt != NULL)
    {
        plat_mem_free (c_ctxt, sizeof(client_ctxt), CLIENT_CTXT_TYPE);
    }

    

    plat_mutex_global_unlock();

exit:
    return i_status;
}

i2c_status i2c_power_on (void *i2c_handle)
{

	i2c_status status = I2C_SUCCESS;
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    plat_device_config *dcfg;

    I2C_LOG(LEVEL_INFO, "\n[I2C] power on 0x%08x\n", i2c_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
    	
		status = I2C_ERROR_INVALID_PARAMETER;
		goto ON_EXIT;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (plat_device_config *) h_ctxt->core_config;

    plat_mutex_instance_lock (h_ctxt->core_lock);
    if (h_ctxt->power_ref_count == 0)
    {
        if (plat_clock_enable(dcfg) == FALSE)
        {
			status = I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL;
			goto ON_EXIT;
        }
        if (plat_gpio_enable (dcfg) == FALSE)
        {
			status = I2C_ERROR_PLATFORM_GPIO_ENABLE_FAIL;
			goto ON_EXIT;
        }
    }
    h_ctxt->power_ref_count++;
    plat_mutex_instance_unlock (h_ctxt->core_lock);

ON_EXIT:
    return status;
}

i2c_status i2c_power_off (void *i2c_handle)
{
	i2c_status status = I2C_SUCCESS;
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
    hw_ctxt *h_ctxt;
    plat_device_config *dcfg;

    I2C_LOG(LEVEL_INFO, "\n[I2C] power off 0x%08x\n", i2c_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        status = I2C_ERROR_INVALID_PARAMETER;
		goto ON_EXIT;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (plat_device_config *) h_ctxt->core_config;

    plat_mutex_instance_lock (h_ctxt->core_lock);
    if (h_ctxt->power_ref_count)
    {
        h_ctxt->power_ref_count--;
        if (h_ctxt->power_ref_count == 0)
        {
            if (plat_clock_disable(dcfg) == FALSE)
            {
                status = I2C_ERROR_PLATFORM_CLOCK_DISABLE_FAIL;
				goto ON_EXIT;
            }
            if (plat_gpio_disable (dcfg) == FALSE)
            {
                status = I2C_ERROR_PLATFORM_GPIO_DISABLE_FAIL;
				goto ON_EXIT;				
            }
        }
    }
    plat_mutex_instance_unlock (h_ctxt->core_lock);

ON_EXIT:
	return status;

}

i2c_status i2c_cancel_transfer (void *i2c_handle)
{
    i2c_status i_status = I2C_SUCCESS;

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
    return c_ctxt->t_ctxt.start_bit_timestamp;
}

uint32 i2c_get_stop_timestamp (void *i2c_handle)
{
    client_ctxt *c_ctxt = (client_ctxt *) i2c_handle;
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

    // log parameters
    I2C_LOG(LEVEL_INFO, "\n[I2C] bus_frequency_khz %d\n",
            config->bus_frequency_khz);
    I2C_LOG(LEVEL_INFO, "\n[I2C] slave_address 0x%02x\n",
            config->slave_address);
    I2C_LOG(LEVEL_INFO, "\n[I2C] smbus_mode %d\n",
            config->mode);
    I2C_LOG(LEVEL_INFO, "\n[I2C] slave_max_clock_stretch_us %d\n",
            config->slave_max_clock_stretch_us);
    I2C_LOG(LEVEL_INFO, "\n[I2C] core_configuration1 %d\n",
            config->core_configuration1);
    I2C_LOG(LEVEL_INFO, "\n[I2C] core_configuration2 %d\n",
            config->core_configuration2);
    I2C_LOG(LEVEL_INFO, "\n[I2C] num_descriptors %d\n",
            num_descriptors);
    I2C_LOG(LEVEL_INFO, "\n[I2C] func 0x%08x: ctxt 0x%08x\n",
            func, ctxt);

    if ((c_ctxt == NULL)                        ||
        (c_ctxt->h_ctxt == NULL)                ||
        (c_ctxt->h_ctxt->core_config == NULL)   ||
        (config == NULL)                        ||
        (num_descriptors == 0))
    {
        return FALSE;
    }

    // validate descriptors
    for (i = 0; i < num_descriptors; i++)
    {
        uint8 has_start, has_stop, has_ts, has_read, has_write;
        
		if ((desc == NULL)          ||
            (desc->buffer == NULL)  ||
            (desc->length == 0))
        {
            return I2C_ERROR_INVALID_PARAMETER;
        }
		
        if ((desc->length == 0) && (num_descriptors == 1))
        {
            // address query
            break;
        }

        I2C_LOG(LEVEL_INFO, "\n[I2C] [%d] buffer 0x%08x: length %d: flags 0x%08x\n",
                i, desc->buffer, desc->length, desc->flags);

        has_start   = desc->flags & I2C_FLAG_START;
        has_stop    = desc->flags & I2C_FLAG_STOP;
        has_write   = desc->flags & I2C_FLAG_WRITE;
        has_read    = desc->flags & I2C_FLAG_READ;
        has_ts      = desc->flags & I2C_FLAG_TIMESTAMP;

        if ((!(has_read || has_write))              ||
            (has_ts && !(has_start || has_stop))    ||
            (direction_change && !has_start))
        {
            return FALSE;
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
    transfer_ctxt       *t_ctxt = NULL;;
    plat_device_config  *dcfg = NULL;
    hw_ctxt             *h_ctxt = NULL;
	//	  sync_type 		  sync = SYNC_WITH_THREAD;
	uint32 		  sync = 0x0; //ag.tbd

    boolean send_transfer_to_hw         = FALSE;
    boolean use_signal_for_completion   = FALSE;
    boolean polled_mode                 = FALSE;
    boolean synchronous_mode            = FALSE;
    boolean has_internal_queueing       = FALSE;

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
        return I2C_ERROR_INVALID_PARAMETER;
    }

    h_ctxt = c_ctxt->h_ctxt;
    t_ctxt = &(c_ctxt->t_ctxt);
    dcfg   = (plat_device_config *) h_ctxt->core_config;

    polled_mode = dcfg->polled_mode;

    if ((func == NULL) && !(polled_mode))
    {
    	// ag: Should not come here,  Return witn wrror for UEFI
        use_signal_for_completion = TRUE;
    }

    if ((func == NULL) || polled_mode)
    {
        synchronous_mode = TRUE;
    }

    has_internal_queueing   = bus_iface_has_queueing(h_ctxt);
    if (bus_iface_has_interrupt_consumer(h_ctxt))
    {
        sync = 0x0;
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

    t_ctxt->signal                  = NULL;

    c_ctxt->next = NULL;

    // protect multiple threads calling i2c_transfer in polled mode
    if (polled_mode)
    {
        plat_mutex_instance_lock (h_ctxt->core_lock);
    }

    // queue transfer 
    // ==============================================================
    plat_mutex_instance_lock (h_ctxt->queue_lock);
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
                break;
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
    plat_mutex_instance_unlock (h_ctxt->queue_lock);
    // ==============================================================

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

        if (use_signal_for_completion)
        {
            plat_wait_for_event (t_ctxt->signal);
//            plat_signal_de_init (t_ctxt->signal);
        }

        if ((transferred != NULL) && synchronous_mode)
        {
            *transferred = t_ctxt->transferred;
        }
    }

    if (polled_mode)
    {
        plat_mutex_instance_unlock (h_ctxt->core_lock);
    }

    return i_status; 
}
