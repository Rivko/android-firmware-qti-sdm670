/** 
    @file  spi_qdi.h
    @brief SPI qdi interface
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_QDI_H__
#define __SPI_QDI_H__

#include "spi_api.h"
#include "qurt_qdi_driver.h"
#include "qurt.h"
#include "qurt_error.h"

#define SPI_DRIVER_NAME "/dev/spi"
#define SPI_THREAD_NAME "spi_qdi_cb"

#define NUM_PDS                 2
#define NUM_CLIENTS_PER_PD      16
#define CB_PIPE_NUM_PARAMETERS  3
#define NUM_QDI_SPI_MAX_DESCS       3
#define MAX_CONCURRENT_CLIENTS      4


#define SPI_QDI_OPEN                    (0 + QDI_PRIVATE)
#define SPI_QDI_CLOSE                   (1 + QDI_PRIVATE)
#define SPI_QDI_POWER_ON                (2 + QDI_PRIVATE)
#define SPI_QDI_POWER_OFF               (3 + QDI_PRIVATE)
#define SPI_QDI_FULL_DUPLEX             (4 + QDI_PRIVATE)
#define SPI_QDI_GET_TIMESTAMP           (5 + QDI_PRIVATE)
#define SPI_QDI_GET_CALLBACK_DATA       (6 + QDI_PRIVATE)
#define SPI_QDI_SETUP_ISLAND_RESOURCE   (7 + QDI_PRIVATE)
#define SPI_QDI_RESET_ISLAND_RESOURCE   (8 + QDI_PRIVATE)

typedef struct spi_qdi_opener spi_qdi_opener;

typedef struct spi_qdi_ctxt
{
    void                *spi_handle;
    void                *ctxt;
    spi_descriptor_t     desc[NUM_QDI_SPI_MAX_DESCS];
    spi_config_t         config;
    spi_qdi_opener      *opener;
    boolean             in_use;
} spi_qdi_ctxt;

typedef struct spi_qdi_opener
{
    qurt_qdi_obj_t      obj;
    qurt_pipe_t         pipe;
    qurt_pipe_attr_t    attr;
    qurt_pipe_data_t    data[CB_PIPE_NUM_PARAMETERS * MAX_CONCURRENT_CLIENTS];
    qurt_mutex_t        mutex;
    void                *c_handle[NUM_CLIENTS_PER_PD];
    spi_qdi_ctxt        q_ctxt_list[MAX_CONCURRENT_CLIENTS];
    boolean             in_use;
} spi_qdi_opener; 

typedef struct SPI_qdi_callback
{
    callback_fn     func;
    uint32          status;
    void            *ctxt;
} spi_qdi_callback;

void spi_notify_completion (callback_fn func, uint32 status, void *context);

#endif //__SPI_QDI_H__
