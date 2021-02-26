/**
  @file spi_api.c
  @brief SPI Driver API implementation
*/
/*
  ===========================================================================

  Copyright (c) 2016-2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/spi/src/spi_api.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "spi_driver.h"
#include "spi_gpi.h"
#include "spi_plat.h"
#include "qup_common.h"
#include "spi_log.h"
#include <stdlib.h>
#include <stringl/stringl.h>

// #include "qurt_printf.h"

spi_hw_ctxt *spi_cores_head = NULL;

static spi_hw_ctxt *spi_get_hw_ctxt (uint32 core_index)
{
    spi_hw_ctxt *h_ctxt = spi_cores_head;
    while (h_ctxt != NULL)
    {
        if (((spi_plat_device_config *) h_ctxt->core_config)->core_index == core_index)
        {
            return h_ctxt;
        }
        h_ctxt = h_ctxt->next;
    }

    /* Allocate a new context. This may not be a real allocation in platforms
       like the SDC, but we try to abstract this to maintain a common driver layer */
    h_ctxt = (spi_hw_ctxt *) spi_plat_mem_alloc (core_index, sizeof(spi_hw_ctxt), HW_CTXT_TYPE);

    if (h_ctxt != NULL)
    {
        h_ctxt->core_ref_count      = 0;
        h_ctxt->power_ref_count      = 0;
        h_ctxt->core_config         = NULL;
        h_ctxt->core_debug          = NULL;
        h_ctxt->client_ctxt_head    = NULL;
        h_ctxt->client_ctxt_tail    = NULL;

        // attach to the head of the list
        h_ctxt->next        = spi_cores_head;
        spi_cores_head      = h_ctxt;
    }
    return h_ctxt;
}

static void spi_release_hw_ctxt (uint32 core_index)
{
    spi_hw_ctxt *h_ctxt = spi_cores_head;
    spi_hw_ctxt *prev = NULL;
    boolean core_found = FALSE;

    while ((h_ctxt != NULL) && (h_ctxt->core_config != NULL))
    {
        if (((spi_plat_device_config *) h_ctxt->core_config)->core_index == core_index)
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
            spi_cores_head = h_ctxt->next;
        }

        // remove links that may spill over to other cores/clients
        // not need to set ref_count to 0 since this code will only
        // run when ref_count is 0
        h_ctxt->core_config         = NULL;
        h_ctxt->core_debug          = NULL;
        h_ctxt->core_iface          = NULL;
        h_ctxt->client_ctxt_head    = NULL;
        h_ctxt->client_ctxt_tail    = NULL;
        h_ctxt->next = NULL;

        spi_plat_mem_free (h_ctxt, sizeof(spi_hw_ctxt), HW_CTXT_TYPE);
    }
}
/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/* spi_open */
spi_status_t spi_open (spi_instance_t instance, void **spi_handle)
{
   spi_status_t status = SPI_SUCCESS;
   spi_client_ctxt *c_ctxt = NULL;
   spi_hw_ctxt *h_ctxt = NULL;
   spi_plat_device_config *dcfg = NULL;
   uint8 core_i = (uint8) (instance - 1); //TBD validate that core_i is between 0 and MAX inst.
   boolean interrupt_registered = FALSE;
   boolean iface_inited = FALSE;
   
   // qurt_printf ("SPI open instance %d\n", instance);
   
   if (spi_plat_in_exception_context())
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : invalid execution level, instance %d",instance);
        return SPI_ERROR_INVALID_EXECUTION_LEVEL;
    }

    // param validation
    if (spi_handle == NULL)
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : spi handle is invalid, instance %d", instance);
        return SPI_ERROR_INVALID_PARAM;
    }

    *spi_handle = NULL;

    spi_plat_mutex_global_lock();

    // get configuration
    dcfg = spi_plat_get_device_config (core_i);
    if (dcfg == NULL)
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : get device config fail, instance %d", instance);
        status = SPI_ERROR_PLAT_GET_CONFIG_FAIL;
        goto exit;
    }

    // get or allocate a hw context
    h_ctxt = spi_get_hw_ctxt (core_i);
    if (h_ctxt == NULL)
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : get HW config fail, instance %d", instance);
        status = SPI_ERROR_HW_INFO_ALLOCATION;
        goto exit;
    }

    h_ctxt->core_config = dcfg;

    if (h_ctxt->core_ref_count == 0)
    {
        // create an instance lock
        if ((h_ctxt->core_lock = spi_plat_mutex_instance_init (core_i, CRIT_SEC_TYPE)) == NULL)
        {
		    SPI_LOG(LEVEL_ERROR,"spi_open : mutex init CRIT fail, instance %d", instance);
            status = SPI_ERROR_MUTEX;
            goto exit;
        }

        if ((h_ctxt->queue_lock = spi_plat_mutex_instance_init (core_i, QUEUE_LOCK_TYPE)) == NULL)
        {
		    SPI_LOG(LEVEL_ERROR,"spi_open : mutex init QUQUE fail, instance %d", instance);
            status = SPI_ERROR_MUTEX;
            goto exit;
        }

        // create a signal
        h_ctxt->core_signal = spi_plat_signal_init (core_i);

        // enable interrupt
        if (!spi_plat_interrupt_register (dcfg, spi_gpi_isr, h_ctxt))
        {
            SPI_LOG(LEVEL_ERROR,"spi_open : interrupt registration failed, instance %d", instance);
            status = SPI_ERROR_PLAT_INTERRUPT_REGISTER;
            goto exit;
        }
        interrupt_registered = TRUE;

#ifndef TOUCH_USES_PRAM_BUFF
        if (core_i == 2)
        {
			spi_plat_mutex_global_unlock();
            status = spi_setup_island_resource (instance, 16000000);
			spi_plat_mutex_global_lock();
            if (status != SPI_SUCCESS)
            {
                SPI_LOG(LEVEL_ERROR,"spi_open : setup island resource failed, instance %d", instance);
                status = SPI_ERROR_PLAT_SET_RESOURCE_FAIL;
                goto exit;
            }
        }
#endif

        if (spi_plat_clock_enable(dcfg) == FALSE)
        {
            SPI_LOG(LEVEL_ERROR,"spi_open : clock enable failed, instance %d", instance);
            status = SPI_ERROR_PLAT_CLK_ENABLE_FAIL;
            goto exit;
        }

        // force polled mode if qup core cannot support interrupts
        if (qup_common_get_hw_version () == QUP_V1_0)
        {
            dcfg->polled_mode = TRUE;
        } 

		if (dcfg->polled_mode)
		{
			h_ctxt->polled = TRUE;
		}
        SPI_LOG(LEVEL_VERBOSE,"spi_open : polled mode is set to %d for instance %d",h_ctxt->polled,instance);
        // initialize bus interface
        status = spi_gpi_init (h_ctxt);
        if (status != SPI_SUCCESS)
        {
           SPI_LOG(LEVEL_ERROR,"spi_open : GPI init failed, instance %d with status %d",instance, status); 
           spi_plat_clock_disable(dcfg);
           goto exit;
        }

        spi_plat_clock_disable(dcfg);

        iface_inited = TRUE;
    }

    // alloc client context
    c_ctxt = (spi_client_ctxt *) spi_plat_mem_alloc (core_i, sizeof(spi_client_ctxt), CLIENT_CTXT_TYPE);
    if (c_ctxt == NULL)
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : client mem alloc failed, instance %d", instance);
        status = SPI_ERROR_HANDLE_ALLOCATION;
        goto exit;
    }

    // initialize the client handle
    h_ctxt->core_ref_count++;
    c_ctxt->h_ctxt = h_ctxt;
    *spi_handle = c_ctxt;

// use a goto exit above only after mutex is taken. before mutex is taken,
// use a return with error code
exit:

    if (SPI_ERROR(status) && (h_ctxt != NULL))
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : failed , initiating cleanup sequence, instance %d", instance);
        if (h_ctxt->core_lock != NULL)
        {
            spi_plat_mutex_instance_deinit (h_ctxt->core_lock);
        }
		if (h_ctxt->queue_lock != NULL)
		{
		    spi_plat_mutex_instance_deinit (h_ctxt->queue_lock);
		}

        if (interrupt_registered)
        {
            spi_plat_interrupt_deregister (dcfg);
        }

        if ((h_ctxt != NULL) &&
            (h_ctxt->core_ref_count == 0))
        {
            if (iface_inited == TRUE)
            {
                spi_gpi_deinit (h_ctxt);
            }
            spi_release_hw_ctxt (core_i);
        }
        if (h_ctxt->core_signal != NULL)
        {
            spi_plat_signal_deinit (h_ctxt->core_signal);
        }

        if (c_ctxt != NULL)
        {
            spi_plat_mem_free (c_ctxt, sizeof(spi_client_ctxt), CLIENT_CTXT_TYPE);
        }
    }

    spi_plat_mutex_global_unlock();
    
    SPI_LOG(LEVEL_VERBOSE,"spi_open : instance %d, returned handle 0x%x, status %d", instance, spi_handle, status);
    // qurt_printf ("SPI open instance %d, returned handle 0x%x, status %d\n", instance, *spi_handle, status);
    return status;
}

/* spi_power_on and off */
spi_status_t spi_power_on (void *spi_handle)
{
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_plat_device_config *dcfg;
	spi_status_t ret = SPI_SUCCESS;
	GPI_CLIENT_HANDLE gpi_handle;

	// qurt_printf ("SPI power on handle 0x%x\n", spi_handle);
    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL) ||
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"spi_power_on : invalid param");
        return SPI_ERROR_INVALID_PARAM;
    }
    gpi_handle = ((spi_gpi_ctxt *)c_ctxt->h_ctxt->core_iface)->params.gpi_handle;
    
	spi_plat_mutex_instance_lock (c_ctxt->h_ctxt->core_lock);
	do
	{
		// if the ref count is already non zero just increment the count and exit
		if (c_ctxt->h_ctxt->power_ref_count++ > 0)
		{
		   ret = SPI_SUCCESS;
		   break;
		}

		dcfg = (spi_plat_device_config *)c_ctxt->h_ctxt->core_config;

		if (spi_plat_clock_enable(dcfg) == FALSE)
		{
            SPI_LOG(LEVEL_ERROR,"spi_power_on : clock enable failed, handle 0x%x", spi_handle);
			ret = SPI_ERROR_PLAT_CLK_ENABLE_FAIL;
			break;
		}
		if (spi_plat_gpio_enable (dcfg) == FALSE)
		{
            SPI_LOG(LEVEL_ERROR,"spi_power_on : gpio enable failed, handle 0x%x", spi_handle);
			ret = SPI_ERROR_PLAT_GPIO_ENABLE_FAIL;
			break;
		}
		(void)gpi_iface_active(gpi_handle, TRUE);
    }
	while (0);
	spi_plat_mutex_instance_unlock (c_ctxt->h_ctxt->core_lock);
    
    SPI_LOG(LEVEL_VERBOSE,"spi_power_on : handle 0x%x, ret %d, ref count %d", spi_handle, ret, c_ctxt->h_ctxt->power_ref_count);
    return ret;
}

spi_status_t spi_power_off (void *spi_handle)
{
	spi_status_t ret = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_plat_device_config *dcfg;
	GPI_CLIENT_HANDLE gpi_handle;
	
	// qurt_printf ("SPI power off handle 0x%x\n", spi_handle);
    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL) ||
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"spi_power_off : invalid param");
        return SPI_ERROR_INVALID_PARAM;
    }

	gpi_handle = ((spi_gpi_ctxt *)c_ctxt->h_ctxt->core_iface)->params.gpi_handle;
	spi_plat_mutex_instance_lock (c_ctxt->h_ctxt->core_lock);
    do
	{
        // to make sure we dont decrement to a negative value
        if(c_ctxt->h_ctxt->power_ref_count != 0)
        {
		   c_ctxt->h_ctxt->power_ref_count--;
        }
        else
        {
            SPI_LOG(LEVEL_ERROR,"spi_power_off : power_off call unexpected, handle 0x%x", spi_handle);
            ret = SPI_SUCCESS;
		    break;
        }

		// if the ref count is non zero just exit
		if (c_ctxt->h_ctxt->power_ref_count > 0)
		{
		   ret = SPI_SUCCESS;
		   break;
		}

		dcfg = (spi_plat_device_config *) c_ctxt->h_ctxt->core_config;

		(void)gpi_iface_active(gpi_handle, FALSE);

		if (spi_plat_clock_disable(dcfg) == FALSE)
		{
            SPI_LOG(LEVEL_ERROR,"spi_power_off : clock disable failed, handle 0x%x", spi_handle);
			ret = SPI_ERROR_PLAT_CLK_DISABLE_FAIL;
			break;
		}
		if (spi_plat_gpio_disable (dcfg) == FALSE)
		{
            SPI_LOG(LEVEL_ERROR,"spi_power_off : gpio disable failed, handle 0x%x", spi_handle);
			ret = SPI_ERROR_PLAT_GPIO_DISABLE_FAIL;
			break;
		}
	}
	while (0);
    spi_plat_mutex_instance_unlock (c_ctxt->h_ctxt->core_lock);
    SPI_LOG(LEVEL_VERBOSE,"spi_power_off : handle 0x%x, ret %d, ref count %d", spi_handle, ret, c_ctxt->h_ctxt->power_ref_count);
    return ret;
}

/* spi transfer: Perform full duplex, half duplex or simplex transfer over SPI BUS.
This is the only available transfer function. The descriptor can have NULL tx OR rx buffers if
half duplex transfer is desired. Callback is invoked upon completion of the full chain of
descriptors or an error condition.
*/
spi_status_t spi_full_duplex
(
   void *spi_handle,
   spi_config_t *config,
   spi_descriptor_t *desc,
   uint32 num_descriptors,
   callback_fn c_fn,
   void *caller_ctxt,
   boolean get_timestamp
)
{
    spi_status_t status = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_xfer_ctxt *t_ctxt;
    spi_hw_ctxt *h_ctxt;
    spi_plat_device_config *dcfg;
	
	if (spi_handle == NULL || config == NULL || desc == NULL || num_descriptors == 0)
	{
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex : invalid param");
		return SPI_ERROR_INVALID_PARAM;
	}

	// qurt_printf ("SPI transfer handle 0x%x\n", spi_handle);
    if (spi_plat_in_exception_context())
    {
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex : invalid execution level, handle 0x%x", spi_handle);
        return SPI_ERROR_INVALID_EXECUTION_LEVEL;
    }

    h_ctxt = c_ctxt->h_ctxt;
	
	spi_plat_mutex_instance_lock (h_ctxt->core_lock);
	spi_plat_mutex_instance_lock (h_ctxt->queue_lock);
    if (h_ctxt->client_ctxt_head == NULL)
    {
        // if we are here, it means there are no active transfers in the core
        h_ctxt->client_ctxt_head = c_ctxt;
        h_ctxt->client_ctxt_tail = c_ctxt;
    }
    else
    {
        // check if the same client is queueing twice
        spi_client_ctxt *temp = h_ctxt->client_ctxt_head;
        while (temp != NULL)
        {
            if (temp == c_ctxt)
            {
                SPI_LOG(LEVEL_ERROR,"spi_full_duplex : client already has queued a transfer, handle 0x%x", spi_handle);
                status = SPI_ERROR_PENDING_TRANSFER;
				spi_plat_mutex_instance_unlock (h_ctxt->core_lock);
				spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);
                goto exit;
            }
            temp = temp->next;
        }

        // if the client handle is not in queue, then add it
        if (status != SPI_ERROR_PENDING_TRANSFER)
        {
            h_ctxt->client_ctxt_tail->next = c_ctxt;
            h_ctxt->client_ctxt_tail = c_ctxt;
        }
    }
	spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);
	
    t_ctxt = &(c_ctxt->t_ctxt);
    dcfg   = (spi_plat_device_config *) (h_ctxt->core_config);

    t_ctxt->config      = config;
    t_ctxt->desc        = desc;
    t_ctxt->num_descs   = num_descriptors;
    t_ctxt->ctxt        = caller_ctxt;
    t_ctxt->callback    = c_fn;
    t_ctxt->async = (c_fn == NULL) ? FALSE : TRUE;

    t_ctxt->xfer_count          = 0;
    t_ctxt->xfer_state          = TRANSFER_INVALID;
    t_ctxt->xfer_status         = SPI_SUCCESS;
	t_ctxt->start_bit_timestamp = 0;
    t_ctxt->stop_bit_timestamp  = 0;
	t_ctxt->get_timestamp       = get_timestamp;

	if(memcmp(&h_ctxt->saved_config, config, sizeof(spi_config_t)) != 0)
    {
        t_ctxt->config_0_sent = 0;
        h_ctxt->saved_config = *config;
    }
    else
    {
        t_ctxt->config_0_sent = 1;
    }


    /*timestamp stuff- tbd*/
    c_ctxt->next = NULL;

	if (h_ctxt->power_ref_count == 0) //core is powered off
	{
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex : unclocked accesss , handle 0x%x", spi_handle);
        spi_plat_mutex_instance_unlock (h_ctxt->core_lock);
		return SPI_ERROR_UNCLOCKED_ACCESS;
	}


    status = spi_gpi_queue_transfer (c_ctxt);
    if(SPI_SUCCESS != status)
    {
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex :gpi queue transfer failed with status : %d, handle 0x%x", status, spi_handle);
    }

    if (SPI_SUCCESS(status) && (c_fn == NULL))
    {
	    if (!h_ctxt->polled)
		    spi_plat_wait_for_event (&(h_ctxt->core_signal));
    }
	spi_plat_mutex_instance_unlock (h_ctxt->core_lock);
    
exit :
    SPI_LOG(LEVEL_VERBOSE,"spi_full_duplex : handle 0x%x, ret %d", spi_handle, status);
    return status;
}

/* spi_close */
spi_status_t spi_close (void *spi_handle)
{
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_hw_ctxt *h_ctxt;
    spi_status_t status = SPI_SUCCESS;
    spi_plat_device_config *dcfg;

    if (spi_plat_in_exception_context())
    {
        SPI_LOG(LEVEL_ERROR,"spi_close : invalid execution level");
        return SPI_ERROR_INVALID_EXECUTION_LEVEL;
    }
    // qurt_printf ("SPI close handle 0x%x\n", spi_handle);
    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        SPI_LOG(LEVEL_ERROR,"spi_close : invalid param");
        return SPI_ERROR_INVALID_PARAM;
    }

    h_ctxt = c_ctxt->h_ctxt;

    spi_plat_mutex_global_lock();

    dcfg = (spi_plat_device_config *) h_ctxt->core_config;

    if (h_ctxt->core_ref_count)
    {
        h_ctxt->core_ref_count--;
        if (h_ctxt->core_ref_count == 0)
        {
            spi_plat_mutex_instance_deinit (h_ctxt->core_lock);
			spi_plat_mutex_instance_deinit (h_ctxt->queue_lock);
            if (!spi_plat_interrupt_deregister (dcfg))
            {
                SPI_LOG(LEVEL_ERROR,"spi_close : interrupt de-register failure 0x%x", spi_handle);
                status = SPI_ERROR_PLAT_INTERRUPT_DEREGISTER;
                goto exit;
            }

            if (h_ctxt->core_iface != NULL)
            {
                if (spi_plat_clock_enable(dcfg) == FALSE)
                {
                    SPI_LOG(LEVEL_ERROR,"spi_close : clock enable failed 0x%x", spi_handle);
                    status = SPI_ERROR_PLAT_CLK_ENABLE_FAIL;
                    goto exit;
                }

                status = spi_gpi_deinit (h_ctxt);
                if(SPI_SUCCESS != status)
                {
                    SPI_LOG(LEVEL_ERROR,"spi_close : GPI de-init failed, handle 0x%x with status %d", spi_handle, status);
                }

                spi_plat_clock_disable(dcfg);

#ifndef TOUCH_USES_PRAM_BUFF
                if (dcfg->core_index == 2)
                {
			        spi_plat_mutex_global_unlock();					
                    status = spi_reset_island_resource (dcfg->core_index + 1);
					spi_plat_mutex_global_lock();
                    if (status != SPI_SUCCESS)
                    {
                        SPI_LOG(LEVEL_ERROR,"spi_close : reset island resource failed ret : %d, handle 0x%x",status, spi_handle);
                        status = SPI_ERROR_PLAT_RESET_RESOURCE_FAIL;
                    }
                }
#endif
            }
            spi_plat_signal_deinit (h_ctxt->core_signal);
            spi_release_hw_ctxt (dcfg->core_index);
        }
    }

exit: // use a goto exit above only after mutex is taken

    if (c_ctxt != NULL)
    {
        spi_plat_mem_free (c_ctxt, sizeof(spi_client_ctxt), CLIENT_CTXT_TYPE);
    }

    spi_plat_mutex_global_unlock();
    SPI_LOG(LEVEL_VERBOSE,"spi_close : handle 0x%x, ret %d", spi_handle, status);
    return status;
}

/*Get the timestamps on the last executed SPI transfer which had get_timestamp enabled.
If the transfer consisted of a number of descriptors, only the timestamp of the LAST
transferred buffer is captured*/
spi_status_t spi_get_timestamp (void *spi_handle, uint64 *start_time, uint64 *completed_time)
{
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_xfer_ctxt *t_ctxt = NULL;

    if (spi_handle == NULL || start_time == NULL || completed_time == NULL)
	{
		return SPI_ERROR_INVALID_PARAM;
	}

	t_ctxt = &(c_ctxt->t_ctxt);
	if (t_ctxt != NULL)
	{
		if(t_ctxt->xfer_state == TRANSFER_DONE)
		{
			*start_time     = t_ctxt->start_bit_timestamp;
			*completed_time = t_ctxt->stop_bit_timestamp;
		}
		else
		{
			return SPI_ERROR_PENDING_TRANSFER;
		}
			
	}

    return SPI_SUCCESS;
}

//Set power resources required for the specific SPI instance before entering low power island mode.
spi_status_t spi_setup_island_resource (spi_instance_t instance, uint32 frequency_hz)
{
    spi_status_t status = SPI_SUCCESS;
    spi_plat_device_config *dcfg;
	uint32 dfs_index = 0, div = 0;

    if (frequency_hz == 0)
    {
        SPI_LOG(LEVEL_ERROR,"spi_setup_island_resource : invalid param");
        return SPI_ERROR_INVALID_PARAM;
    }

    spi_plat_mutex_global_lock();
    dcfg = spi_plat_get_device_config ((uint8)(instance-1));
    if (dcfg == NULL)
    {
		spi_plat_mutex_global_unlock();
        SPI_LOG(LEVEL_ERROR,"spi_setup_island_resource : core is not opened, instance %d", instance);
        return SPI_ERROR_CORE_NOT_OPEN;
    }

    if (FALSE == spi_plat_clock_set_frequency (dcfg, frequency_hz,
                                               &dfs_index, &div, TRUE))
    {
        SPI_LOG(LEVEL_ERROR,"spi_setup_island_resource : set frequency failed %d", instance);
        status = SPI_ERROR_PLAT_SET_RESOURCE_FAIL;
    }
    
    
    spi_plat_mutex_global_unlock();
    SPI_LOG(LEVEL_VERBOSE,"spi_setup_island_resource : instance %d, dfs %d , div %d, status %d", instance, dfs_index, div, status);

    return status;	
}

//If a resource was previously set up for island operation, this releases that resource.
spi_status_t spi_reset_island_resource (spi_instance_t instance)
{
    spi_status_t status = SPI_SUCCESS;
    spi_plat_device_config *dcfg;

    spi_plat_mutex_global_lock();
    dcfg = spi_plat_get_device_config ((uint8)(instance-1));
    if (dcfg == NULL)
    {
        spi_plat_mutex_global_unlock();		
        SPI_LOG(LEVEL_ERROR,"spi_reset_island_resource : core is not opened %d", instance);
        return SPI_ERROR_CORE_NOT_OPEN;
    }

    if (FALSE == spi_plat_src_clk_reset(dcfg))
    {
        SPI_LOG(LEVEL_ERROR,"spi_reset_island_resource : clk reset failed %d", instance);
        status = SPI_ERROR_PLAT_RESET_RESOURCE_FAIL;
    }
    spi_plat_mutex_global_unlock();

    SPI_LOG(LEVEL_VERBOSE,"spi_reset_island_resource : instance %d, status %d", instance, status);
    return status;	
}
