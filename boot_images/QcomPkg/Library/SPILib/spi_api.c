
/*=============================================================================   
    @file  spi_api.c
    @brief SPI public API implementation
   
    Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/04/19   NM      Fix CS Assert selection
 01/07/19   BNG     SPI driver for saipan
 
=============================================================================*/

#include "SpiApi.h"    // implement the apis defined here
#include "spi_api.h"
#include "spi_drv.h"    // generic objects for an spi driver
#include "spi_iface.h"  // fifo/dma interface definitions
#include "spi_plat.h"   // platform apis
#include "spi_log.h"    // logging
#include <string.h>
#include <SpiDevice.h>

typedef void (*callback_fn) (uint32 transfer_status, void *callback_ctxt);

spi_hw_ctxt *spi_cores_head = NULL; 
 
static void bus_clear_hw_ctxt(spi_hw_ctxt *h_ctxt)
{
	h_ctxt->core_config		= NULL;
	h_ctxt->core_debug		= NULL;
	h_ctxt->core_iface		= NULL;
	h_ctxt->core_lock		= NULL;
	h_ctxt->core_signal		= NULL;
	h_ctxt->polled 			= TRUE;
	//h_ctxt->saved_config	= {0};
	h_ctxt->queue_lock		= NULL;
	h_ctxt->client_ctxt_head= NULL;
	h_ctxt->client_ctxt_tail= NULL;
	h_ctxt->core_ref_count	= 0;
	h_ctxt->power_ref_count = 0;
	h_ctxt->next			= NULL;
}

spi_hw_ctxt *bus_allocate_hw_ctxt(uint32 core_index)
{
	spi_hw_ctxt *h_ctxt = spi_cores_head;
	spi_plat_device_config *dcfg = NULL;
	
	while(h_ctxt != NULL)
	{
		dcfg = (spi_plat_device_config *) h_ctxt->core_config;
		if(dcfg->core_index == core_index)
		{
			goto exit;
		}
	  	h_ctxt = h_ctxt->next;
	}
	//hw context not already present in the list
	//alloacte memory for new hw context
	h_ctxt = (spi_hw_ctxt *) spi_plat_mem_alloc (core_index,sizeof(spi_hw_ctxt),HW_CTXT_TYPE); //need of core_index ?
	
	if(h_ctxt != NULL)
	{
		bus_clear_hw_ctxt(h_ctxt);
		h_ctxt->next = spi_cores_head;
		spi_cores_head = h_ctxt;
	}
exit:
	return h_ctxt;
}

static void bus_free_hw_ctxt(uint32 core_index)
{
	spi_hw_ctxt *h_ctxt = spi_cores_head;
	spi_plat_device_config *dcfg = NULL;
	boolean core_found = FALSE;
	spi_hw_ctxt *prev = NULL;
	
	while(h_ctxt != NULL && h_ctxt->core_config != NULL)
	{
		dcfg = (spi_plat_device_config *)h_ctxt->core_config;
		if(dcfg->core_index == core_index) //record index to remove from list
		{
			core_found = TRUE;
			break;
		}
		prev = h_ctxt;
		h_ctxt = h_ctxt->next;
	}
	if(core_found)
	{
		if(prev != NULL)
		{
			prev->next = h_ctxt->next;
		}
		else
		{
			spi_cores_head = h_ctxt->next;
		}
		
	}
}
spi_status bus_init_hw_ctxt(spi_hw_ctxt *h_ctxt,spi_plat_device_config *dcfg)
{
	spi_status status =  SPI_SUCCESS;
	uint8 core_i = dcfg->core_index;
	//boolean interrupt_registered = FALSE;
	
	h_ctxt->core_config		= dcfg;
	h_ctxt->core_lock		= spi_plat_mutex_instance_init(core_i,CRIT_SEC_TYPE);
	if(h_ctxt->core_lock == NULL)
	{
		status = SPI_ERROR_MUTEX;
		SPI_LOG(LEVEL_ERROR,"bus_init_hw_ctxt : failed to init core lock,");
	}
	
	h_ctxt->core_signal		= spi_plat_signal_init (core_i);
		
	h_ctxt->queue_lock		= spi_plat_mutex_instance_init (core_i, QUEUE_LOCK_TYPE);
	if(h_ctxt->queue_lock == NULL)
	{
		status = SPI_ERROR_MUTEX;
		SPI_LOG(LEVEL_ERROR,"bus_init_hw_ctxt : failed to init queue lock,");
	}
	
	/*if(!spi_plat_interrupt_register (dcfg, spi_gpi_isr, h_ctxt))
	{
		//erorr log
		status = SPI_ERROR_PLAT_INTERRUPT_REGISTER;
		goto exit;
	}
	interrupt_registered = TRUE;*/
	if(spi_plat_clock_enable(dcfg) == FALSE)
	{
		status = SPI_ERROR_PLAT_CLK_ENABLE_FAIL;
		SPI_LOG(LEVEL_ERROR,"bus_init_hw_ctxt : clock enable fail");
		goto exit;
	}
	
	if(spi_plat_gpio_enable(dcfg) == FALSE)
	{
		status = SPI_ERROR_PLAT_GPIO_ENABLE_FAIL;
		SPI_LOG(LEVEL_ERROR,"bus_init_hw_ctxt : GPIO enable fail");
		goto exit;
	}
  	
	status = bus_iface_init (h_ctxt);
    if (status != SPI_SUCCESS)
    {
       spi_plat_clock_disable(dcfg);
	   SPI_LOG(LEVEL_ERROR,"bus_init_hw_ctxt : bus iface init fail");
       goto exit;
    }
	return status;
exit:
    return status;
}

void bus_de_init_hw_ctxt(spi_hw_ctxt *h_ctxt,spi_plat_device_config *dcfg)
{
	if(h_ctxt->core_lock != NULL)
	{
		spi_plat_mutex_instance_unlock (h_ctxt->core_lock);
	}
	if(h_ctxt->queue_lock != NULL)
	{
		spi_plat_mutex_instance_unlock (h_ctxt->queue_lock);
	}
	if(h_ctxt->core_signal != NULL)
	{
		spi_plat_signal_free(h_ctxt->core_signal);
	}
	bus_iface_deinit(h_ctxt);
	
}

spi_status spi_open (spi_instance instance, void **spi_handle)
{
	spi_status status =  SPI_SUCCESS;
	spi_client_ctxt *c_ctxt = NULL;
	spi_hw_ctxt *h_ctxt = NULL;
	spi_plat_device_config *dcfg = NULL;
	uint8 core_i = (uint8)(instance);
	
	if (spi_plat_in_exception_context())
    {
        SPI_LOG(LEVEL_ERROR,"spi_open : invalid execution level, instance %d",instance);
        status = SPI_ERROR_INVALID_EXECUTION_LEVEL;
		goto exit;
    }
	if(spi_handle == NULL)
	{
		status = SPI_ERROR_INVALID_PARAM;
		SPI_LOG(LEVEL_ERROR,"spi_open : invalid parameters ");
		goto exit;
	}
	*spi_handle = NULL;
	
	spi_plat_mutex_global_lock();
	
	dcfg = spi_plat_get_device_config(core_i);
	if(dcfg == NULL)
	{
		status = SPI_ERROR_PLAT_GET_CONFIG_FAIL;
		SPI_LOG(LEVEL_ERROR,"spi_open : spi plat get device config failed, instance %d",instance);
		goto exit;
	}
	
	h_ctxt = bus_allocate_hw_ctxt(core_i);
	
	if(h_ctxt ==  NULL)
	{
		status = SPI_ERROR_HW_INFO_ALLOCATION;
		SPI_LOG(LEVEL_ERROR,"spi_open : hw allocation failed ");
		goto exit;
	}
	 // initialize the hw context if needed
    if (h_ctxt->core_ref_count == 0)
    {
        status = bus_init_hw_ctxt (h_ctxt, dcfg);
        if (SPI_ERROR(status))
        {
			SPI_LOG(LEVEL_ERROR,"spi_open : init hw ctxt failed");
			spi_plat_mutex_global_unlock();
            goto exit;
        }
    }
    c_ctxt = (spi_client_ctxt *)spi_plat_mem_alloc (core_i,sizeof(spi_client_ctxt),CLIENT_CTXT_TYPE);
	if(c_ctxt == NULL)
	{
		status = SPI_ERROR_HANDLE_ALLOCATION;
		SPI_LOG(LEVEL_ERROR,"spi_open : client handle allocation fail");
		spi_plat_mutex_global_unlock();
		goto exit;
	}	

    h_ctxt->core_ref_count++;
    c_ctxt->h_ctxt = h_ctxt;	
	*spi_handle = c_ctxt;
	
exit:
    if(SPI_ERROR(status))
	{
		if ((h_ctxt != NULL) && (h_ctxt->core_ref_count == 0))
        {
            bus_de_init_hw_ctxt (h_ctxt, dcfg);
            bus_free_hw_ctxt (core_i);
        }

        if (c_ctxt != NULL)
        {
            spi_plat_mem_free (c_ctxt, sizeof(spi_client_ctxt), CLIENT_CTXT_TYPE);
        }
	}
	spi_plat_mutex_global_unlock();
	
	return status;
}

spi_status spi_close(void *spi_handle)
{
	spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_hw_ctxt *h_ctxt;
    spi_status status = SPI_SUCCESS;
    spi_plat_device_config *dcfg;

    if (spi_plat_in_exception_context())
    {
        status = SPI_ERROR_INVALID_EXECUTION_LEVEL;
		SPI_LOG(LEVEL_ERROR,"spi_close : invalid execution level");
		goto exit;
    }

    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL))
    {
        status = SPI_ERROR_INVALID_PARAM;
		SPI_LOG(LEVEL_ERROR,"spi_close : invalid parameters ");
		goto exit;
    }

    h_ctxt = c_ctxt->h_ctxt;

    spi_plat_mutex_global_lock();

    SPI_LOG(LEVEL_INFO, "\n[SPI] close core %d\n", spi_handle);

    dcfg = (spi_plat_device_config *) h_ctxt->core_config;
	
	if (spi_plat_clock_disable(dcfg) == FALSE)
	{
		status = SPI_ERROR_PLAT_CLK_DISABLE_FAIL;
		SPI_LOG(LEVEL_ERROR,"spi_close : clock disable fail  ");
		spi_plat_mutex_global_unlock();
		goto exit;
	}
	if (spi_plat_gpio_disable (dcfg) == FALSE)
	{
		status = SPI_ERROR_PLAT_GPIO_DISABLE_FAIL;
		SPI_LOG(LEVEL_ERROR,"spi_close : gpio disable fail  ");
		spi_plat_mutex_global_unlock();
		goto exit;				
	}
	
    if (h_ctxt->core_ref_count)
    {
        h_ctxt->core_ref_count--;
        if (h_ctxt->core_ref_count == 0)
        {
            bus_de_init_hw_ctxt (h_ctxt, dcfg);
            bus_free_hw_ctxt (dcfg->core_index);
        }
    }

	SPI_LOG(LEVEL_INFO, "\n[SPI] close handle 0x%08X\n", c_ctxt);
    if (c_ctxt != NULL)
    {
        spi_plat_mem_free (c_ctxt, sizeof(spi_client_ctxt), CLIENT_CTXT_TYPE);
    }

    spi_plat_mutex_global_unlock();

exit:
    return status;
}


spi_status spi_power_on (void *spi_handle)
{

	spi_status status = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_hw_ctxt *h_ctxt;
    spi_plat_device_config *dcfg;

    SPI_LOG(LEVEL_INFO, "\n[SPI] power on 0x%08x\n", spi_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
    	
		status = SPI_ERROR_INVALID_PARAM;
		SPI_LOG(LEVEL_ERROR,"spi_power_on : invalid parameters ");
		goto ON_EXIT;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (spi_plat_device_config *) h_ctxt->core_config;

    spi_plat_mutex_instance_lock (h_ctxt->core_lock);
    if (h_ctxt->power_ref_count == 0)
    {
        if (spi_plat_clock_enable(dcfg) == FALSE)
        {
			status = SPI_ERROR_PLAT_CLK_ENABLE_FAIL;
			SPI_LOG(LEVEL_ERROR,"spi_power_on : clock enable fail  ");
			spi_plat_mutex_instance_unlock(h_ctxt->core_lock);
			goto ON_EXIT;
        }
        if (spi_plat_gpio_enable (dcfg) == FALSE)
        {
			status = SPI_ERROR_PLAT_GPIO_ENABLE_FAIL;
			SPI_LOG(LEVEL_ERROR,"spi_power_on : GPIO enable fail  ");
			spi_plat_mutex_instance_unlock(h_ctxt->core_lock);
			goto ON_EXIT;
        }
    }
    h_ctxt->power_ref_count++;
    spi_plat_mutex_instance_unlock (h_ctxt->core_lock);

ON_EXIT:
    return status;
}

spi_status spi_power_off (void *spi_handle)
{
	spi_status status = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_hw_ctxt *h_ctxt;
    spi_plat_device_config *dcfg;

    SPI_LOG(LEVEL_INFO, "\n[SPI] power off 0x%08x\n", spi_handle);

    if ((c_ctxt == NULL)         ||
        (c_ctxt->h_ctxt == NULL) || 
        (c_ctxt->h_ctxt->core_config == NULL))
    {
        status = SPI_ERROR_INVALID_PARAM;
		SPI_LOG(LEVEL_ERROR,"spi_power_off : invalid parameters ");
		goto ON_EXIT;
    }
    h_ctxt = c_ctxt->h_ctxt;
    dcfg = (spi_plat_device_config *) h_ctxt->core_config;

    spi_plat_mutex_instance_lock (h_ctxt->core_lock);
    if (h_ctxt->power_ref_count)
    {
        h_ctxt->power_ref_count--;
        if (h_ctxt->power_ref_count == 0)
        {
            if (spi_plat_clock_disable(dcfg) == FALSE)
            {
                status = SPI_ERROR_PLAT_CLK_DISABLE_FAIL;
				SPI_LOG(LEVEL_ERROR,"spi_power_off : clock disable fail  ");
				spi_plat_mutex_instance_unlock(h_ctxt->core_lock);
				goto ON_EXIT;
            }
            if (spi_plat_gpio_disable (dcfg) == FALSE)
            {
                status = SPI_ERROR_PLAT_GPIO_DISABLE_FAIL;
				SPI_LOG(LEVEL_ERROR,"spi_power_off : GPIO disable fail  ");
				spi_plat_mutex_instance_unlock(h_ctxt->core_lock);
				goto ON_EXIT;				
            }
        }
    }
    spi_plat_mutex_instance_unlock (h_ctxt->core_lock);

ON_EXIT:
	return status;

}

uint32 spi_get_start_timestamp (void *spi_handle)
{
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    return c_ctxt->t_ctxt.start_bit_timestamp;
}

uint32 spi_get_stop_timestamp (void *spi_handle)
{
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    return c_ctxt->t_ctxt.stop_bit_timestamp;
}

static boolean spi_validate_transfer_params (
    spi_client_ctxt *c_ctxt,
    spi_config_t *config,
    spi_descriptor_t *desc,
    uint32 num_descriptors
        )
{
    uint32 i = 0;

    // log parameters
    SPI_LOG(LEVEL_INFO, "\n[SPI] spi_mode %d\n",
            config->spi_mode);
    SPI_LOG(LEVEL_INFO, "\n[SPI] spi_cs_polarity 0x%02x\n",
            config->spi_cs_polarity);
    SPI_LOG(LEVEL_INFO, "\n[SPI] spi_bits_per_word %d\n",
            config->spi_bits_per_word);
    SPI_LOG(LEVEL_INFO, "\n[SPI] spi_slave_index %d\n",
            config->spi_slave_index);
    SPI_LOG(LEVEL_INFO, "\n[SPI] clk_freq_hz %d\n",
            config->clk_freq_hz);
    SPI_LOG(LEVEL_INFO, "\n[SPI] cs_clk_delay_cycles %d\n",
            config->cs_clk_delay_cycles);
	SPI_LOG(LEVEL_INFO, "\n[SPI] inter_word_delay_cycles %d\n",
            config->inter_word_delay_cycles);
	SPI_LOG(LEVEL_INFO, "\n[SPI] loopback_mode %d\n",
            config->loopback_mode);	
    SPI_LOG(LEVEL_INFO, "\n[SPI] num_descriptors %d\n",
            num_descriptors);

    if ((c_ctxt == NULL) ||
        (c_ctxt->h_ctxt == NULL) ||
        (c_ctxt->h_ctxt->core_config == NULL) ||
        (config == NULL) ||
        (desc == NULL) ||
        (num_descriptors == 0))
    {
        return FALSE;
    }

    // Validate config parameters.
    if ((config->spi_mode > SPI_MODE_3) ||
        (config->spi_cs_polarity > SPI_CS_ACTIVE_HIGH) ||
        (config->endianness > SPI_BIG_ENDIAN) ||
        (config->spi_bits_per_word < 4) ||
        (config->spi_bits_per_word > 32) ||
        (config->spi_slave_index > 3))
    {
        return FALSE;
    }

    // validate descriptors
    for (i = 0; i < num_descriptors; i++)
    {
        if (((desc[i].tx_buf == NULL) && (desc[i].rx_buf == NULL)) ||
            (desc[i].len == 0) )
        {
            return FALSE;
        }
    }
    return TRUE;
}

spi_status spi_transfer( 
		void	*spi_handle,
		SpiDeviceInfoType	*devInfo,
		CONST UINT8	*write_buffer,
		UINT32	write_len,
		UINT8	*read_buffer,
		UINT32	read_len
)
{
	if(spi_handle == NULL || devInfo == NULL || (write_buffer == NULL && read_buffer == NULL ) )
	{
		SPI_LOG(LEVEL_ERROR,"spi_transfer : invalid parameters");
		return SPI_ERROR_INVALID_PARAM;
	}
	spi_status status = SPI_SUCCESS;
	SpiDeviceParametersType param = devInfo->deviceParameters;
	SpiDeviceBoardInfoType boardInfo = devInfo->deviceBoardInfo;
	SpiTransferParametersType transParam = devInfo->transferParameters;
	  
	spi_config_t config ;
	uint8 polarity = param.eClockPolarity;
	uint8 phase = param.eShiftMode;
	  
	if((polarity || phase) == 0 ) 
	 {
		  config.spi_mode = SPI_MODE_0;
	 }	
	 
	else if((polarity && phase) == 1)
	 {
		 config.spi_mode = SPI_MODE_3;
	 }
	else if(polarity == 0)
	 {
		config.spi_mode = SPI_MODE_1;
	 } 
	else config.spi_mode = SPI_MODE_2;
	  
	  config.spi_cs_polarity = (spi_cs_polarity_t)param.eCSPolarity;
	  config.endianness = 0; //not known
	  config.spi_bits_per_word = transParam.nNumBits; 
	  config.spi_slave_index = boardInfo.nSlaveNumber; 
	  config.clk_freq_hz = param.u32MaxSlaveFrequencyHz;
	  config.cs_clk_delay_cycles = 0; //not known
	  config.inter_word_delay_cycles = param.u32DeassertionTime;
	  config.cs_toggle = 0; //not exposed to client
	  config.loopback_mode = transParam.eLoopbackMode;

	  spi_descriptor_t desc ;//= spi_plat_mem_alloc(0,sizeof(spi_descriptor_t),TX_TRANSFER_ELEM_TYPE);;
	  desc.tx_buf =(uint8 *)write_buffer;
	  desc.rx_buf = (uint8 *)read_buffer;
	  desc.len = write_len ;//should be same as read_len
	  desc.leave_cs_asserted = param.eCSMode;
	  
	  uint32 num_descriptors = 1;
   
      status = spi_full_duplex(spi_handle,&config,&desc,num_descriptors,NULL,NULL,TRUE,!(desc.leave_cs_asserted));
	  read_buffer = (uint8 *) desc.rx_buf;
	  return status;
}

/* spi_full_duplex: Perform full duplex transfer over SPI BUS.
This is the only available transfer function. Callback is invoked upon completion of the full chain of
descriptors or an error condition.
*/
spi_status spi_full_duplex
(
    void *spi_handle,
    spi_config_t *config,
    spi_descriptor_t *desc,
    uint32 num_descriptors,
    callback_fn c_fn,
    void *caller_ctxt,
    boolean get_timestamp,
    boolean is_last
    )
{
    spi_status status = SPI_SUCCESS;
    spi_client_ctxt *c_ctxt = (spi_client_ctxt *)spi_handle;
    spi_xfer_ctxt *t_ctxt;
    spi_hw_ctxt *h_ctxt;
    spi_plat_device_config *dcfg;
    //UNREFERENCED_NP(get_timestamp);

    if (spi_plat_in_exception_context())
    {
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex : invalid execution level ");
		return SPI_ERROR_INVALID_EXECUTION_LEVEL;
    }

    // Validate transfer parameters.
    if (spi_validate_transfer_params(c_ctxt, config, desc, num_descriptors) == FALSE)
    {
        SPI_LOG(LEVEL_ERROR,"spi_full_duplex : invalid parameters ");
		return SPI_ERROR_INVALID_PARAM;
    }

    h_ctxt = c_ctxt->h_ctxt;
    t_ctxt = &(c_ctxt->t_ctxt);
    dcfg = (spi_plat_device_config *)(h_ctxt->core_config);

    t_ctxt->config = config;
    t_ctxt->desc = desc;
    t_ctxt->num_descs = num_descriptors;
    t_ctxt->ctxt = caller_ctxt;
    t_ctxt->callback = c_fn;
    t_ctxt->async = (c_fn == NULL) ? FALSE : TRUE;  
    t_ctxt->xfer_count = 0;
    t_ctxt->xfer_state = TRANSFER_INVALID;
    t_ctxt->xfer_status = SPI_SUCCESS;

    /*timestamp stuff- tbd*/
    c_ctxt->next = NULL;

    spi_plat_mutex_instance_lock(h_ctxt->queue_lock);

    // don't check tail. head == null means that everything is done
    if (h_ctxt->client_ctxt_head == NULL)
    {
        // if we are here, it means there are no active transfers in the core
        h_ctxt->client_ctxt_head = c_ctxt;
        h_ctxt->client_ctxt_tail = c_ctxt;
    }
    else
    {
        // check if the same client is queuing twice
        spi_client_ctxt *temp = h_ctxt->client_ctxt_head;
        while (temp != NULL)
        {
            if (temp == c_ctxt)
            {
                status = SPI_ERROR_PENDING_TRANSFER;
                break;
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
	
	status = bus_iface_queue_transfer(c_ctxt, is_last);
    
    spi_plat_mutex_instance_unlock(h_ctxt->queue_lock);

    // polled mode code. Enable if polling is required. To keep it portable,
    // implement the plat_is_polling_mode in the platform layer.

    if (SPI_SUCCESS(status) && (c_fn == NULL))
    {
        if (!h_ctxt->polled)
            spi_plat_wait_for_event(&(h_ctxt->core_signal));
    }
    return status;
}
