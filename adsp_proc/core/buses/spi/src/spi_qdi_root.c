
/**
  @file  spi_qdi_root.c
  @brief SPI root pd interface to QDI
 */
/*=============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
  =============================================================================*/

#include "spi_api.h"
#include "spi_driver.h"
#include "spi_config.h"
#include "spi_log.h"
#include "spi_qdi.h"
#include "spi_plat.h"
#include "pram_mgr.h"
#include <stdlib.h>
#include <stringl/stringl.h>

#if 0
#define TX_BUF 16
#define RX_BUF TX_BUF
#define LOG_SIZE 64

typedef struct
{
  uint32 len;
  uint8  tx_buf[TX_BUF];
  uint8  rx_buf[RX_BUF];
  uint8  slv_idx;
  spi_status_t spi_status;
  qurt_thread_t thread_id;
}spi_log_entry_t;

typedef struct
{
 uint8 curr_idx;
 spi_log_entry_t entries[LOG_SIZE];
}spi_log_t;

/* SPI transaction log */
volatile spi_log_t spi_debug_log ;
volatile boolean spi_loop_err = TRUE;
#endif

static spi_qdi_opener   user_pd_openers[NUM_PDS];
static qurt_qdi_obj_t   spi_qdi_root_obj;

int  spi_qdi_open(int client_handle);
void spi_qdi_root_release (qurt_qdi_obj_t *obj);

void spi_notify_completion (callback_fn func, uint32 status, void *context)
{
    qurt_pipe_data_t data1 = 0;
    qurt_pipe_data_t data2 = 0;
    qurt_pipe_data_t data3 = 0;
	
    qurt_pipe_t         *pipe   = NULL;
    spi_qdi_ctxt        *q_ctxt = NULL;
    spi_qdi_opener      *opener = NULL;

    q_ctxt = (spi_qdi_ctxt *) context;

    if (q_ctxt == NULL) { return; }

    opener = q_ctxt->opener;
    if (opener == NULL) { return; }

    pipe = &opener->pipe;

    memscpy(&data1, sizeof(data1), (void *) &func,              sizeof(callback_fn));
    memscpy(&data2, sizeof(data2), (void *) &status,            sizeof(uint32));
    memscpy(&data3, sizeof(data3), (void *) &(q_ctxt->ctxt),    sizeof(void *));

	do
	{
		if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data1))
		{
			SPI_LOG(LEVEL_ERROR, "[SPI] spi_notify_completion: qdi pipe send failed for func");
			break;
		}
		if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data2))
		{
			SPI_LOG(LEVEL_ERROR, "[SPI] spi_notify_completion: qdi pipe send failed for status");
			break;
		}
		if (QURT_EOK != qurt_pipe_send_cancellable (pipe, data3))
		{
			SPI_LOG(LEVEL_ERROR,"[SPI] spi_notify_completion: qdi pipe send failed for transferred");
			break;
		}
	}
    while (0);
    qurt_mutex_lock   (&opener->mutex);
    q_ctxt->in_use = FALSE;
    qurt_mutex_unlock (&opener->mutex);
}

int spi_qdi_root_cb_handler (spi_qdi_opener *opener, spi_qdi_callback *cb)
{
    qurt_pipe_data_t callback;
    qurt_pipe_data_t status;
    qurt_pipe_data_t context;
    qurt_pipe_t     *pipe;	
    int ret = QURT_EOK;
	
	pipe = &opener->pipe;
    if ((ret = qurt_pipe_receive_cancellable(pipe, &callback)) != QURT_EOK)    { return ret; }
    if ((ret = qurt_pipe_receive_cancellable(pipe, &status))   != QURT_EOK)    { return ret; }
    if ((ret = qurt_pipe_receive_cancellable(pipe, &context))  != QURT_EOK)    { return ret; }

    cb->func        = (callback_fn) callback;
    cb->status      = (uint32) status;
    cb->ctxt        = (void *) context;
    return ret;
}

spi_status_t spi_qdi_root_full_duplex
(
    int                 client_handle,
    spi_qdi_opener     *opener,
    void               *spi_handle,
    spi_config_t       *config,
    spi_descriptor_t   *desc,
    uint32              num_descriptors,
    callback_fn         func,
    void               *ctxt,
    boolean             get_timestamp
)
{
    uint32 i = 0; 
    spi_status_t status  = SPI_SUCCESS;
    spi_qdi_ctxt *q_ctxt = NULL;
    spi_qdi_ctxt *q_list = opener->q_ctxt_list;
    boolean found = FALSE;

    spi_client_ctxt *c_ctxt = (spi_client_ctxt *) spi_handle;
    spi_hw_ctxt *h_ctxt;
    spi_plat_device_config *dcfg;
	
	
#if 0 //debug only	
	uint8 idx; uint8 size;
	idx = spi_debug_log.curr_idx;
	size = desc->len > TX_BUF ? TX_BUF : desc->len; 
	*((uint32 *)(spi_debug_log.entries[idx].tx_buf)) = 0xDEADBEEF;
	*((uint32 *)(spi_debug_log.entries[idx].rx_buf)) = 0xDEADBEEF;
	spi_debug_log.entries[idx].len = desc->len;
	spi_debug_log.entries[idx].slv_idx = config->spi_slave_index;
	spi_debug_log.entries[idx].thread_id = qurt_thread_get_id();
	if (desc->tx_buf)
	{
	  memscpy((void *)&spi_debug_log.entries[idx].tx_buf, size, (void *)desc->tx_buf, size);
	}
#endif

    h_ctxt = c_ctxt->h_ctxt;
    dcfg   = (spi_plat_device_config *) (h_ctxt->core_config);


    if (num_descriptors > NUM_QDI_SPI_MAX_DESCS)
    {
        SPI_LOG(LEVEL_ERROR,"[SPI] qdi interface supports only %d descriptors per transfer", NUM_QDI_SPI_MAX_DESCS);
        return SPI_ERROR_INVALID_PARAM;
    }

    qurt_mutex_lock (&opener->mutex);
    for (i = 0; i < MAX_CONCURRENT_CLIENTS; i++)
    {
        if (q_list[i].in_use == FALSE)
        {
            q_ctxt = &q_list[i];
            q_ctxt->in_use = TRUE;
            found = TRUE;
        }
        if (found) { break; }
    }
    qurt_mutex_unlock (&opener->mutex);

    if (found == FALSE)
    {
        SPI_LOG(LEVEL_ERROR,"[SPI] qdi ctxt allocation failed");
        return SPI_ERROR_QDI_ALLOC_FAIL;
    }

    if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                            (void *) (&(q_ctxt->config)),
                                            (void *) config,
                                            sizeof(spi_config_t)))
    {
        SPI_LOG(LEVEL_ERROR,"[SPI] qdi copying config failed");
        status = SPI_ERROR_QDI_COPY_FAIL;
        goto exit;
    }

    if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                            (void *) (q_ctxt->desc),
                                            (void *) desc,
                                            sizeof(spi_descriptor_t) * num_descriptors))
    {
        SPI_LOG(LEVEL_ERROR,"[SPI] qdi copying descriptors failed");
        status = SPI_ERROR_QDI_COPY_FAIL;
        goto exit;
    }


    for (i = 0; i < num_descriptors; i++)
    {
        int tx_mapped_addr = 0, rx_mapped_addr = 0;
        qurt_addr_t tx_addr = (qurt_addr_t) ((desc + i)->tx_buf);
        qurt_addr_t rx_addr = (qurt_addr_t) ((desc + i)->rx_buf);
#ifndef TOUCH_USES_PRAM_BUFF
        if (dcfg->core_index != 2)
#endif
        {

           tx_mapped_addr = pram_mgr_lookup_phys_addr(client_handle, tx_addr);
           rx_mapped_addr = pram_mgr_lookup_phys_addr(client_handle, rx_addr);
           if (tx_mapped_addr == 0 || rx_mapped_addr == 0)
           {
               status = SPI_ERROR_QDI_MMAP_FAIL;
           }
        }
#ifndef TOUCH_USES_PRAM_BUFF
        else
        {
           tx_mapped_addr = (int)(desc + i)->tx_buf;
           rx_mapped_addr = (int)(desc + i)->rx_buf;
        }
#endif
        (q_ctxt->desc+i)->tx_buf = (uint8 *)tx_mapped_addr;
        (q_ctxt->desc+i)->rx_buf = (uint8 *)rx_mapped_addr;
    }

    q_ctxt->ctxt = ctxt;
	q_ctxt->opener = opener;
	q_ctxt->spi_handle = spi_handle;

    status = spi_full_duplex (spi_handle,
                             &q_ctxt->config,
                             q_ctxt->desc,
                             num_descriptors,
                             func,
                             q_ctxt,
                             get_timestamp);


#if 0 //debug only	
	if (desc->rx_buf)
	memscpy((void *)&spi_debug_log.entries[idx].rx_buf, size, (void *)desc->rx_buf, size);
 
	spi_debug_log.entries[idx].spi_status = status;
	spi_debug_log.curr_idx = (spi_debug_log.curr_idx + 1) % LOG_SIZE;
#endif							 
exit:
    if (SPI_ERROR(status) || func == NULL)
    {
        if (q_ctxt != NULL)
        {
            qurt_mutex_lock   (&opener->mutex);
            q_ctxt->in_use = FALSE;
            qurt_mutex_unlock (&opener->mutex);
        }
    }

    return status;
}

static int spi_qdi_root_invoke
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
    int ret = QURT_EOK, i;
    spi_status_t status = SPI_SUCCESS;
	boolean found;
	spi_qdi_opener *opener;

    if (obj == NULL) { return QURT_EINVALID; }

    opener = (spi_qdi_opener *) obj;

    switch(method)
    {
        case QDI_OPEN:
            ret = spi_qdi_open (client_handle);
            break;

        case SPI_QDI_OPEN:
            status = (int) spi_open (a1.num, a2.ptr);
            if (SPI_SUCCESS(status))
            {
                qurt_mutex_lock (&opener->mutex);
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
                    SPI_LOG(LEVEL_ERROR,"SPI: Exceeding max supported clients per pd");
                }
                qurt_mutex_unlock (&opener->mutex);
            }
			
            break;

        case SPI_QDI_CLOSE:
            status = (int) spi_close (a1.ptr);
            qurt_mutex_lock (&opener->mutex);
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
                SPI_LOG(LEVEL_ERROR,"Client handle not found in pd");
            }
            qurt_mutex_unlock (&opener->mutex);
            break;

        case SPI_QDI_POWER_ON:
            status = (int) spi_power_on (a1.ptr);
            break;

        case SPI_QDI_POWER_OFF:
            status = (int) spi_power_off (a1.ptr);
            break;

        case SPI_QDI_FULL_DUPLEX:
            status = (int) spi_qdi_root_full_duplex (
                      client_handle,
					  opener,
                      a1.ptr,     // void *spi_handle
                      a2.ptr,     // spi_config_t *config
                      a3.ptr,     // spi_descriptor_t *desc
                      a4.num,     // uint32 num_descriptors
                      a5.ptr,     // callback_fn func
                      a6.ptr,     // void *ctxt
                      a7.num);     // bool get_timestamp
            break;

        case SPI_QDI_GET_TIMESTAMP:
            status = (int) spi_get_timestamp (a1.ptr, (uint64 *)a2.ptr, (uint64 *)a3.ptr);
            break;

        case SPI_QDI_GET_CALLBACK_DATA:
            ret = spi_qdi_root_cb_handler (opener, a1.ptr);
            break;
		case SPI_QDI_RESET_ISLAND_RESOURCE:
		    status = (int) spi_reset_island_resource (a1.num);
		    break;
		case SPI_QDI_SETUP_ISLAND_RESOURCE:
		    status = (int) spi_setup_island_resource (a1.num, a2.num);
			break;
        default:
            ret = qurt_qdi_method_default(
                      client_handle, obj, method,
                      a1, a2, a3, a4, a5, a6, a7,
                      a8, a9);
            break;
    }
	if (SPI_SUCCESS != status) return status;
    return ret;
}

int spi_qdi_root_pipe_init (spi_qdi_opener *opener)
{
    qurt_pipe_t         *pipe = &opener->pipe;
    qurt_pipe_attr_t    *attr = &opener->attr;
    qurt_pipe_data_t    *data = opener->data;

    qurt_pipe_attr_init(attr);
    qurt_pipe_attr_set_buffer(attr, data);
    qurt_pipe_attr_set_elements(attr, CB_PIPE_NUM_PARAMETERS * MAX_CONCURRENT_CLIENTS);
    qurt_pipe_attr_set_buffer_partition(attr, 1);

    return qurt_pipe_init(pipe, attr);
}

void spi_qdi_root_release (qurt_qdi_obj_t *obj)
{
    int i;
    spi_status_t status = SPI_SUCCESS;
    spi_qdi_opener *opener = (spi_qdi_opener *) obj;
    spi_qdi_ctxt *q_list;

    if (opener == NULL) { return; }

    q_list = opener->q_ctxt_list;

    for (i = 0; i < MAX_CONCURRENT_CLIENTS; i++)
    {
        if (q_list[i].in_use)
        {
			//Should we explicitly stop GSI channels?
            q_list[i].in_use = FALSE;
        }
    }

    qurt_mutex_lock   (&opener->mutex);
    for (i = 0; i < NUM_CLIENTS_PER_PD; i++)
    {
        if (opener->c_handle[i] != NULL)
        {
            status = spi_power_off (opener->c_handle[i]);
            status = spi_close (opener->c_handle[i]);
            opener->c_handle[i] = NULL;
        }
    }
    qurt_mutex_unlock (&opener->mutex);

    qurt_pipe_destroy(&opener->pipe);
    qurt_mutex_destroy(&opener->mutex);
    opener->in_use = FALSE;
}

int spi_qdi_open(int client_handle)
{
    int i       = 0;
    int ret     = QURT_EOK;
    int handle  = -1;
    spi_qdi_opener *opener = NULL;

    while (i < NUM_PDS)
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
        goto error;
    }

    opener->obj.invoke  = spi_qdi_root_invoke;
    opener->obj.refcnt  = QDI_REFCNT_INIT;
    opener->obj.release = spi_qdi_root_release;

    ret = spi_qdi_root_pipe_init (opener);
    if(QURT_EOK != ret)
    {
        goto error;
    }

    qurt_mutex_init (&opener->mutex);

    handle = qurt_qdi_handle_create_from_obj_t(client_handle, &opener->obj); 
    if (handle < 0)
    {
        goto error;
    }

error:
    if (opener != NULL && ((handle < 0) || (ret != QURT_EOK)))
    {
        opener->in_use = FALSE;
    }
    return handle; 
}

void spi_qdi_root_init(void)
{
    // qdi interface init
    spi_qdi_root_obj.invoke   = spi_qdi_root_invoke;
    spi_qdi_root_obj.refcnt   = QDI_REFCNT_INIT;
    spi_qdi_root_obj.release  = spi_qdi_root_release;
    qurt_qdi_register_devname (SPI_DRIVER_NAME, &spi_qdi_root_obj);
}
