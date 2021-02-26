/**
  @file  i2c_qdi_root.c
  @brief I2C root pd interface to QDI
 */
/*=============================================================================
  Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
  =============================================================================*/

#include "i2c_api.h"
#include "i2c_log.h"
#include "i2c_qdi.h"
#include <stdlib.h>
#include <stringl/stringl.h>
#include EXT_DEP_HEADER

static i2c_qdi_opener   user_pd_openers[NUM_USER_PDS];
static qurt_qdi_obj_t   i2c_qdi_root_obj;

int i2c_qdi_open(int client_handle);
void i2c_qdi_root_release (qurt_qdi_obj_t *obj);

void i2c_qdi_root_wait_for_event (void *signal)
{
  unsigned int mask = 0;
  if (QURT_EOK == qurt_signal_wait_cancellable (signal,
              I2C_CANCEL_SIGNAL,
              I2C_CANCEL_SIGNAL,
              &mask))
  {
      qurt_signal_clear(signal, I2C_CANCEL_SIGNAL);
  }
}

void i2c_qdi_root_signal_event (void *signal)
{
    qurt_signal_set(signal, I2C_CANCEL_SIGNAL);
}

void i2c_notify_completion (callback_func func, uint32 status, uint32 transferred, void *context)
{
    qurt_pipe_data_t    data1 = 0;
    qurt_pipe_data_t    data2 = 0;
    qurt_pipe_data_t    data3 = 0;
    qurt_pipe_data_t    data4 = 0;
    qurt_pipe_t         *pipe   = NULL;
    i2c_qdi_ctxt        *q_ctxt = NULL;
    i2c_qdi_opener      *opener = NULL;
    boolean             xfer_canceled = FALSE;

    I2C_LOG(LEVEL_INFO, "qdi root i2c_notify_completion 0x%08x:%d:%d:0x%08x",
            func, status, transferred, context);

    q_ctxt = (i2c_qdi_ctxt *) context;
    if (q_ctxt == NULL) { return; }

    opener = q_ctxt->opener;
    if (opener == NULL) { return; }

    qurt_mutex_lock   (&opener->mutex);
    if (opener->cancel_in_progress)
    {
        xfer_canceled = TRUE;
        opener->cancel_in_progress = FALSE;
    }
    qurt_mutex_unlock (&opener->mutex);

    if (xfer_canceled)
    {
        i2c_qdi_root_signal_event (&opener->signal);
        I2C_LOG(LEVEL_VERBOSE, "signal cancel completion");
        return;
    }

    pipe = &opener->pipe;

    memscpy(&data1, sizeof(data1), (void *) &func,                  sizeof(callback_func));
    memscpy(&data2, sizeof(data2), (void *) &status,                sizeof(uint32));
    memscpy(&data3, sizeof(data3), (void *) &transferred,           sizeof(uint32));
    memscpy(&data4, sizeof(data4), (void *) &(q_ctxt->ctxt),        sizeof(void *));

    I2C_LOG(LEVEL_PERF, I2C_QDI_ROOT_SEND_TO_PIPE, pipe);

    if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data1))
    {
        I2C_LOG(LEVEL_ERROR, "i2c_notify_completion: qdi pipe send failed for func");
        return;
    }
    if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data2))
    {
        I2C_LOG(LEVEL_ERROR, "i2c_notify_completion: qdi pipe send failed for status");
        return;
    }
    if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data3))
    {
        I2C_LOG(LEVEL_ERROR, "i2c_notify_completion: qdi pipe send failed for transferred");
        return;
    }
    if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data4))
    {
        I2C_LOG(LEVEL_ERROR, "i2c_notify_completion: qdi pipe send failed for context");
        return;
    }

    qurt_mutex_lock   (&opener->mutex);
    q_ctxt->in_use = FALSE;
    I2C_LOG(LEVEL_VERBOSE, "q_ctxt freed 0x%08x", q_ctxt);
    qurt_mutex_unlock (&opener->mutex);
}

int i2c_qdi_root_cb_handler (i2c_qdi_opener *opener, i2c_qdi_callback *cb)
{
    qurt_pipe_data_t callback;
    qurt_pipe_data_t status;
    qurt_pipe_data_t transferred;
    qurt_pipe_data_t context;
    qurt_pipe_t      *pipe;

    int ret         = QURT_EOK;

    I2C_LOG(LEVEL_INFO, "qdi root waiting for callback");
    pipe = &opener->pipe;

    if ((ret = qurt_pipe_receive_cancellable(pipe, &callback))     != QURT_EOK)    { return ret; }
    if ((ret = qurt_pipe_receive_cancellable(pipe, &status))       != QURT_EOK)    { return ret; }
    if ((ret = qurt_pipe_receive_cancellable(pipe, &transferred))  != QURT_EOK)    { return ret; }
    if ((ret = qurt_pipe_receive_cancellable(pipe, &context))      != QURT_EOK)    { return ret; }

    I2C_LOG(LEVEL_PERF, I2C_QDI_ROOT_RCVD_FROM_PIPE, pipe);

    cb->func        = (callback_func) callback;
    cb->status      = (uint32) status;
    cb->transferred = (uint32) transferred;
    cb->ctxt        = (void *) context;

    I2C_LOG(LEVEL_INFO, "qdi root cb 0x%08x:%d:%d:0x%08x",
            cb->func, cb->status, cb->transferred, cb->ctxt);

    return ret;
}

i2c_status i2c_qdi_root_transfer
(
    int                 client_handle,
    i2c_qdi_opener     *opener,
    void               *i2c_handle,
    i2c_slave_config   *config,
    i2c_descriptor     *desc,
    uint16              num_descriptors,
    callback_func       func,
    void               *ctxt,
    uint32              delay_us,
    uint32             *transferred
)
{
    uint8 i = 0;
    i2c_status i_status  = I2C_SUCCESS;
    i2c_qdi_ctxt *q_ctxt = NULL;
    i2c_qdi_ctxt *q_list = opener->q_ctxt_list;
    boolean found = FALSE;

    if (num_descriptors > NUM_QDI_I2C_MAX_DESCS)
    {
        I2C_LOG(LEVEL_ERROR, "qdi interface supports only %d descriptors per transfer", NUM_QDI_I2C_MAX_DESCS);
        return I2C_ERROR_INVALID_PARAMETER;
    }

    qurt_mutex_lock   (&opener->mutex);
    for (i = 0; i < MAX_CONCURRENT_CLIENTS; i++)
    {
        if (q_list[i].in_use == FALSE)
        {
            q_ctxt = &q_list[i];
            q_ctxt->in_use = TRUE;
            I2C_LOG(LEVEL_VERBOSE, "q_ctxt alloc 0x%08x", q_ctxt);
            found = TRUE;
        }
        if (found) { break; }
    }
    qurt_mutex_unlock (&opener->mutex);

    if (found == FALSE)
    {
        I2C_LOG(LEVEL_ERROR, "qdi ctxt allocation failed");
        return I2C_ERROR_QDI_CTXT_ALLOC_FAIL;
    }

    if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                            (void *) (&(q_ctxt->config)),
                                            (void *) config,
                                            sizeof(i2c_slave_config)))
    {
        I2C_LOG(LEVEL_ERROR, "qdi copying config failed");
        i_status = I2C_ERROR_QDI_COPY_FAIL;
        goto exit;
    }

    if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                            (void *) (q_ctxt->desc),
                                            (void *) desc,
                                            sizeof(i2c_descriptor) * num_descriptors))
    {
        I2C_LOG(LEVEL_ERROR, "qdi copying descriptors failed");
        i_status = I2C_ERROR_QDI_COPY_FAIL;
        goto exit;
    }

    // since you made a local copy of the i2c_descriptor list, you can
    // change the descriptor buffer to PRAM mapped address and pass it down.
    for (i = 0; i < num_descriptors; i++)
    {
        int mapped_addr  = 0;
        qurt_addr_t addr = (qurt_addr_t) ((desc + i)->buffer);
		
		if(desc->length == 0) // Address Test usecase, we don't expect any buffer.
			break;
		
        mapped_addr = pram_mgr_lookup_phys_addr(client_handle, addr);
        if (mapped_addr == 0)
        {
            i_status = I2C_ERROR_QDI_MEM_MAP_FAIL;
            goto exit;
        }
        (q_ctxt->desc + i)->buffer = (uint8 *) mapped_addr;
    }

    q_ctxt->ctxt        = ctxt;
    q_ctxt->opener      = opener;
    q_ctxt->i2c_handle  = i2c_handle;

    i_status = i2c_transfer (i2c_handle,
                             &q_ctxt->config,
                             q_ctxt->desc,
                             num_descriptors,
                             func,
                             q_ctxt,
                             delay_us,
                             transferred);

exit:

    if (I2C_ERROR(i_status) || (func == NULL))
    {
        if (q_ctxt != NULL)
        {
            qurt_mutex_lock   (&opener->mutex);
            q_ctxt->in_use = FALSE;
            I2C_LOG(LEVEL_VERBOSE, "q_ctxt freed 0x%08x", q_ctxt);
            qurt_mutex_unlock (&opener->mutex);
        }
    }

    return i_status;
}

static int i2c_qdi_root_invoke
(
    int             client_handle,
    qurt_qdi_obj_t *obj,
    int             method,
    qurt_qdi_arg_t  a1,
    qurt_qdi_arg_t  a2,
    qurt_qdi_arg_t  a3,
    qurt_qdi_arg_t  a4,
    qurt_qdi_arg_t  a5,
    qurt_qdi_arg_t  a6,
    qurt_qdi_arg_t  a7,
    qurt_qdi_arg_t  a8,
    qurt_qdi_arg_t  a9
)
{
    int ret = QURT_EOK;
    i2c_status i_status = I2C_SUCCESS;
    uint32 timestamp = 0, i;
    i2c_qdi_opener *opener;
    boolean found;

    if (obj == NULL) { return QURT_EINVALID; }

    opener = (i2c_qdi_opener *) obj;

    I2C_LOG(LEVEL_PERF, I2C_QDI_ROOT_QDI_INVOKE, obj);
    switch(method)
    {
        case QDI_OPEN:
            ret = i2c_qdi_open (client_handle);
            break;

        case QDI_ISLAND_CHECK_ALLOC:
            ret = 1;
            break;

        case I2C_QDI_OPEN:
            i_status = i2c_open (a1.num, a2.ptr);
            if (I2C_SUCCESS(i_status))
            {
                qurt_mutex_lock   (&opener->mutex);
                found = FALSE;
                for (i = 0; i < NUM_CLIENTS_PER_PD; i++)
                {
                    if (opener->c_handle[i] == NULL)
                    {
                        opener->c_handle[i] = *((void **) (a2.ptr));
                        found = TRUE;
                        break;
                    }
                }
                if (!found)
                {
                    I2C_LOG(LEVEL_VERBOSE, "exceeding max supported clients per pd");
                }
                qurt_mutex_unlock (&opener->mutex);
            }
            break;

        case I2C_QDI_CLOSE:
            i_status = i2c_close (a1.ptr);
            qurt_mutex_lock   (&opener->mutex);
            found = FALSE;
            for (i = 0; i < NUM_CLIENTS_PER_PD; i++)
            {
                if (opener->c_handle[i] == a1.ptr)
                {
                    opener->c_handle[i] = NULL;
                    found = TRUE;
                    break;
                }
            }
            if (!found)
            {
                I2C_LOG(LEVEL_VERBOSE, "client handle not found in pd");
            }
            qurt_mutex_unlock (&opener->mutex);
            break;

        case I2C_QDI_CANCEL_TRANSFER:
            i_status = i2c_cancel_transfer (a1.ptr);
            break;

        case I2C_QDI_POWER_ON:
            i_status = i2c_power_on (a1.ptr);
            break;

        case I2C_QDI_POWER_OFF:
            i_status = i2c_power_off (a1.ptr);
            break;

        case I2C_QDI_LOCK:
            i_status = i2c_lock (a1.ptr);
            break;

        case I2C_QDI_UNLOCK:
            i_status = i2c_unlock (a1.ptr);
            break;

        case I2C_QDI_TRANSFER:
            i_status = i2c_qdi_root_transfer (
                    client_handle,
                    opener,
                    a1.ptr,     // void *i2c_handle
                    a2.ptr,     // i2c_slave_config *config
                    a3.ptr,     // i2c_descriptor *desc
                    a4.num,     // uint16 num_descriptors
                    a5.ptr,     // callback_func func
                    a6.ptr,     // void *ctxt
                    a7.num,     // uint32 delay
                    a8.ptr);    // uint32 *transferred
            break;

        case I2C_QDI_GET_START_TIMESTAMP:
            timestamp = i2c_get_start_timestamp (a1.ptr);
            break;

        case I2C_QDI_GET_STOP_TIMESTAMP:
            timestamp = i2c_get_stop_timestamp (a1.ptr);
            break;

        case I2C_QDI_SETUP_LPI_RESOURCE:
            i_status = i2c_setup_lpi_resource (a1.num, a2.num);
            break;

        case I2C_QDI_RESET_LPI_RESOURCE:
            i_status = i2c_reset_lpi_resource (a1.num);
            break;

        case I2C_QDI_GET_CALLBACK_DATA:
            ret = i2c_qdi_root_cb_handler (opener, a1.ptr);
            break;

        case I2C_QDI_TEST_PD_RESTART:
            i2c_qdi_root_release (obj);
            break;

        default:
            ret = qurt_qdi_method_default(
                    client_handle,
                    obj, method,
                    a1, a2, a3, a4,
                    a5, a6, a7, a8,
                    a9);
            break;
    }

    if (timestamp   != 0)        { return timestamp; }
    if (I2C_SUCCESS != i_status) { return i_status; }
    if (QURT_EOK    != ret)      { return ret; }

    return ret;
}

int i2c_qdi_root_pipe_init (i2c_qdi_opener *opener)
{
    qurt_pipe_t         *pipe = &opener->pipe;
    qurt_pipe_attr_t    *attr = &opener->attr;
    qurt_pipe_data_t    *data = opener->data;

    qurt_pipe_attr_init(attr);
    qurt_pipe_attr_set_buffer(attr, data);
    qurt_pipe_attr_set_buffer_partition(attr, 1);
    qurt_pipe_attr_set_elements(attr, CB_PIPE_NUM_PARAMETERS * MAX_CONCURRENT_CLIENTS);

    return qurt_pipe_init(pipe, attr);
}

void i2c_qdi_root_release (qurt_qdi_obj_t *obj)
{
    int i;
    i2c_status i_status = I2C_SUCCESS;
    i2c_qdi_opener *opener = (i2c_qdi_opener *) obj;
    i2c_qdi_ctxt *q_list;

    I2C_LOG(LEVEL_INFO, "qdi release: obj 0x%08x", obj);
    if (opener == NULL) { return; }

    q_list = opener->q_ctxt_list;

    for (i = 0; i < MAX_CONCURRENT_CLIENTS; i++)
    {
        if (q_list[i].in_use)
        {
            qurt_mutex_lock   (&opener->mutex);
            opener->cancel_in_progress = TRUE;
            i_status = i2c_cancel_transfer(q_list[i].i2c_handle);
            qurt_mutex_unlock (&opener->mutex);
            I2C_LOG(LEVEL_VERBOSE, "qdi release: 0x%08x cancel status %d",
                    q_list[i].i2c_handle, i_status);
            if (i_status == I2C_TRANSFER_FORCE_TERMINATED)
            {
                I2C_LOG(LEVEL_VERBOSE, "qdi_release: 0x%08x waiting on cancel",
                        q_list[i].i2c_handle);
                i2c_qdi_root_wait_for_event (&opener->signal);
            }
            else
            {
                qurt_mutex_lock   (&opener->mutex);
                opener->cancel_in_progress = FALSE;
                qurt_mutex_unlock (&opener->mutex);
            }
            q_list[i].in_use = FALSE;
            I2C_LOG(LEVEL_VERBOSE, "qdi release: q_ctxt freed 0x%08x", &q_list[i]);
        }
    }

    qurt_mutex_lock   (&opener->mutex);
    for (i = 0; i < NUM_CLIENTS_PER_PD; i++)
    {
        if (opener->c_handle[i] != NULL)
        {
            i_status = i2c_power_off (opener->c_handle[i]);
            I2C_LOG(LEVEL_VERBOSE, "qdi release: power off status %d", i_status);

            i_status = i2c_close (opener->c_handle[i]);
            I2C_LOG(LEVEL_VERBOSE, "qdi release: close status %d", i_status);

            opener->c_handle[i] = NULL;
        }
    }
    qurt_mutex_unlock (&opener->mutex);

    qurt_pipe_destroy(&opener->pipe);
    qurt_mutex_destroy(&opener->mutex);
    qurt_signal_destroy(&opener->signal);

    opener->in_use = FALSE;
}

int i2c_qdi_open(int client_handle)
{
    int i       = 0;
    int ret     = QURT_EOK;
    int handle  = -1;
    i2c_qdi_opener *opener = NULL;

    while (i < NUM_USER_PDS)
    {
        if (user_pd_openers[i].in_use == FALSE)
        {
            opener = &user_pd_openers[i];
            opener->in_use = TRUE;
            break;
        }
        i++;
    }

    if (opener == NULL)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR i2c_qdi_opener not free");
        goto error;
    }

    opener->obj.invoke  = i2c_qdi_root_invoke;
    opener->obj.refcnt  = QDI_REFCNT_INIT;
    opener->obj.release = i2c_qdi_root_release;

    ret = i2c_qdi_root_pipe_init (opener);
    if(QURT_EOK != ret)
    {
        I2C_LOG(LEVEL_ERROR, "failed pipe init client %d error %d", client_handle, ret);
        goto error;
    }

    qurt_mutex_init (&opener->mutex);
    qurt_signal_init(&opener->signal);

    handle = qurt_qdi_handle_create_from_obj_t(client_handle, &opener->obj); 
    if (handle < 0)
    {
        I2C_LOG(LEVEL_ERROR, "ERROR qdi handle created failed");
        goto error;
    }

error:

    if (opener != NULL && ((handle < 0) || (ret != QURT_EOK)))
    {
        opener->in_use = FALSE;
    }

    I2C_LOG(LEVEL_INFO, "qdi_handle = 0x%08x", handle);

    return handle; 
}

void i2c_qdi_root_init(void)
{
    // qdi interface init
    i2c_qdi_root_obj.invoke   = i2c_qdi_root_invoke;
    i2c_qdi_root_obj.refcnt   = QDI_REFCNT_INIT;
    i2c_qdi_root_obj.release  = i2c_qdi_root_release;

    I2C_LOG(LEVEL_INFO, "qdi root obj = 0x%08x", &i2c_qdi_root_obj);

    qurt_qdi_register_devname(I2C_DRIVER_NAME, &i2c_qdi_root_obj);
}
