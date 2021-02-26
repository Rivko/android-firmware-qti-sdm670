/**
    @file  i2c_qdi_user.c
    @brief I2C user pd interface to QDI
 */
/*=============================================================================
            Copyright (c) 2016 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_api.h"
#include "i2c_log.h"
#include "i2c_qdi.h"
#include "qurt.h"
#include "qurt_qdi.h"
#include <stringl/stringl.h>

#define CB_THREAD_STACK_SIZE   2048
#define CB_THREAD_PRIORITY     15

static int qdi_handle = -1;
static uint8 cb_thread_stack[CB_THREAD_STACK_SIZE];
static qurt_thread_t cb_thread;

int i2c_get_callback (i2c_qdi_callback *cb);
void i2c_test_pd_restart (void);

void i2c_qdi_user_cb_handler (void *arg)
{
    int ret;

    while (1)
    {
        i2c_qdi_callback cb = { NULL, 0, 0, NULL };
        ret = i2c_get_callback (&cb);
        if (QURT_EOK == ret)
        {
            if (cb.func != NULL)
            {
                // I2C_LOG(LEVEL_PERF, I2C_QDI_USER_CALLING_CALLBACK, cb.status);
                cb.func (cb.status, cb.transferred, cb.ctxt);
            }
            else
            {
                // I2C_LOG(LEVEL_ERROR, "qdi callback func is null");
            }
        }
        else
        {
            // I2C_LOG(LEVEL_ERROR, "i2c get callback failed %d", ret);
            break;
        }
    }
}

void i2c_qdi_user_init (void)
{
    int ret = 0;
    qurt_thread_attr_t tattr;
    unsigned int stackbase;

    // I2C_LOG(LEVEL_ERROR, "i2c_qdi_user_init entry");

    if (qdi_handle == -1)
    {
        qdi_handle = qurt_qdi_open(I2C_DRIVER_NAME);
    }

    if(qdi_handle < 0)
    {
        // I2C_LOG(LEVEL_ERROR, "qdi open failed %d", qdi_handle);
        return;
    }

    stackbase = (unsigned int) &cb_thread_stack;
    qurt_thread_attr_init (&tattr);
    qurt_thread_attr_set_stack_size (&tattr, (CB_THREAD_STACK_SIZE - 8));
    qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase + 7) & (~7)));
    qurt_thread_attr_set_priority (&tattr, CB_THREAD_PRIORITY - 1);
    qurt_thread_attr_set_tcb_partition (&tattr, 1);
    qurt_thread_attr_set_name (&tattr, I2C_THREAD_NAME);
    ret = qurt_thread_create (&cb_thread, &tattr, i2c_qdi_user_cb_handler, NULL);

    if(QURT_EOK != ret)
    {
        // I2C_LOG(LEVEL_ERROR, "thread init failed %d", ret);
        qurt_qdi_close(qdi_handle);
        return;
    }
}

i2c_status i2c_open (i2c_instance instance, void **i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_OPEN,
            instance,
            i2c_handle);
}

i2c_status i2c_close (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_CLOSE,
            i2c_handle);
}

i2c_status i2c_cancel_transfer (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_CANCEL_TRANSFER,
            i2c_handle);
}

i2c_status i2c_power_on (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_POWER_ON,
            i2c_handle);
}

i2c_status i2c_power_off (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_POWER_OFF,
            i2c_handle);
}

i2c_status i2c_lock (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_LOCK,
            i2c_handle);
}

i2c_status i2c_unlock (void *i2c_handle)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_UNLOCK,
            i2c_handle);
}

i2c_status i2c_transfer (void *i2c_handle,
                         i2c_slave_config *config,
                         i2c_descriptor *desc,
                         uint16 num_descriptors,
                         callback_func func,
                         void *ctxt,
                         uint32 delay_us,
                         uint32 *transferred)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_TRANSFER,
            i2c_handle,
            config,
            desc,
            num_descriptors,
            func,
            ctxt,
            delay_us,
            transferred);
}

uint32 i2c_get_start_timestamp (void *i2c_handle)
{
    return (uint32) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_GET_START_TIMESTAMP,
            i2c_handle);
}

uint32 i2c_get_stop_timestamp (void *i2c_handle)
{
    return (uint32) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_GET_STOP_TIMESTAMP,
            i2c_handle);
}

int i2c_get_callback (i2c_qdi_callback *cb)
{
    return (uint32) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_GET_CALLBACK_DATA,
            cb);
}

i2c_status i2c_setup_lpi_resource (i2c_instance instance, uint32 frequency_hz)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_SETUP_LPI_RESOURCE,
            instance,
            frequency_hz);
}

i2c_status i2c_reset_lpi_resource (i2c_instance instance)
{
    return (i2c_status) qurt_qdi_handle_invoke(
            qdi_handle,
            I2C_QDI_RESET_LPI_RESOURCE,
            instance);
}

void i2c_test_pd_restart ()
{
    qurt_qdi_handle_invoke(qdi_handle, I2C_QDI_TEST_PD_RESTART);
    qdi_handle = -1;
}
