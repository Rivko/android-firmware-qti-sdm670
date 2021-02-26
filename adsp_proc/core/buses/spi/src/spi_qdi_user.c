/**
    @file  spi_qdi_user.c
    @brief SPI User PD interface to QDI
 */
/*=============================================================================
            Copyright (c) 2017-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_api.h"
#include "spi_qdi.h"
#include "qurt.h"
#include "qurt_qdi.h"
#include <stringl/stringl.h>

#define SPI_LOG(...) 
#define SPI_CB_THREAD_STACK_SIZE   2048
#define SPI_CB_THREAD_PRIORITY     15

static int spi_qdi_handle = -1;
static uint8 spi_cb_thread_stack[SPI_CB_THREAD_STACK_SIZE];
static qurt_thread_t spi_cb_thread;

int spi_get_callback (spi_qdi_callback *cb);

void spi_qdi_user_cb_handler (void *arg)
{
	while (1)
    {
        spi_qdi_callback cb = {NULL, 0, NULL};
        if (QURT_EOK == spi_get_callback (&cb))
        {
            if (cb.func != NULL)
            {
                cb.func(cb.status, cb.ctxt);
            }
            else
            {
                SPI_LOG("\n[SPI] qdi callback func is null\n");
            }
        }
    }
}

void spi_qdi_user_init (void)
{
    int ret = 0;
    qurt_thread_attr_t tattr;
    unsigned int stackbase;

    if (spi_qdi_handle == -1)
    {
        spi_qdi_handle = qurt_qdi_open(SPI_DRIVER_NAME);
    }

    if(spi_qdi_handle < 0)
    {
        SPI_LOG("\n[SPI] qdi open failed %d\n", spi_qdi_handle);
        return;
    }

    stackbase = (unsigned int) &spi_cb_thread_stack;
    qurt_thread_attr_init (&tattr);
    qurt_thread_attr_set_stack_size (&tattr, (SPI_CB_THREAD_STACK_SIZE - 8));
    qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase + 7) & (~7)));
    qurt_thread_attr_set_priority (&tattr, SPI_CB_THREAD_PRIORITY - 1);
    qurt_thread_attr_set_tcb_partition (&tattr, 1);
    qurt_thread_attr_set_name (&tattr, SPI_THREAD_NAME);
    ret = qurt_thread_create (&spi_cb_thread, &tattr, spi_qdi_user_cb_handler, NULL);

    if(QURT_EOK != ret)
    {
        SPI_LOG("\n[SPI] thread init failed %d\n", ret);
        qurt_qdi_close(spi_qdi_handle);
        return;
    }
}

spi_status_t spi_open (spi_instance_t instance, void **spi_handle)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_OPEN,
            instance,
            spi_handle);
}

spi_status_t spi_close (void *spi_handle)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_CLOSE,
            spi_handle);
}

spi_status_t spi_power_on (void *spi_handle)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_POWER_ON,
            spi_handle);
}

spi_status_t spi_power_off (void *spi_handle)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_POWER_OFF,
            spi_handle);
}

spi_status_t spi_full_duplex (void *spi_handle,
                              spi_config_t *config,
							  spi_descriptor_t *desc,
							  uint32 num_descriptors,
							  callback_fn c_fn,
							  void *c_ctxt,
							  boolean get_timestamp)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_FULL_DUPLEX,
            spi_handle,
            config,
            desc,
            num_descriptors,
            c_fn,
            c_ctxt,
            get_timestamp);
}

spi_status_t spi_get_timestamp (void *spi_handle, uint64 *start_time, uint64 *completed_time)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_GET_TIMESTAMP,
            spi_handle,
            start_time,
            completed_time);
}

int spi_get_callback (spi_qdi_callback *cb)
{
    return (uint32) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_GET_CALLBACK_DATA,
            cb);
}

spi_status_t spi_setup_island_resource (spi_instance_t instance, uint32 frequency_hz)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_SETUP_ISLAND_RESOURCE,
            instance,
            frequency_hz);
	
}

spi_status_t spi_reset_island_resource (spi_instance_t instance)
{
    return (spi_status_t) qurt_qdi_handle_invoke(
            spi_qdi_handle,
            SPI_QDI_RESET_ISLAND_RESOURCE,
            instance);
}
