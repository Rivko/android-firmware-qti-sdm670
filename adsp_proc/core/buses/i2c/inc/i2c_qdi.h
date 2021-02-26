/** 
    @file  i2c_qdi.h
    @brief I2C qdi interface
 */
/*=============================================================================
            Copyright (c) 2016 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_QDI_H__
#define __I2C_QDI_H__

#include "i2c_api.h"
#include "qurt.h"
#include "qurt_error.h"
#include "qurt_qdi_driver.h"

#define NUM_USER_PDS            2
#define NUM_CLIENTS_PER_PD      16
#define CB_PIPE_NUM_PARAMETERS  4

#define I2C_DRIVER_NAME "/dev/i2c"
#define I2C_THREAD_NAME "i2c_qdi_cb"

#define I2C_QDI_OPEN                    (0  + QDI_PRIVATE)
#define I2C_QDI_CLOSE                   (1  + QDI_PRIVATE)
#define I2C_QDI_CANCEL_TRANSFER         (2  + QDI_PRIVATE)
#define I2C_QDI_POWER_ON                (3  + QDI_PRIVATE)
#define I2C_QDI_POWER_OFF               (4  + QDI_PRIVATE)
#define I2C_QDI_LOCK                    (5  + QDI_PRIVATE)
#define I2C_QDI_UNLOCK                  (6  + QDI_PRIVATE)
#define I2C_QDI_TRANSFER                (7  + QDI_PRIVATE)
#define I2C_QDI_GET_START_TIMESTAMP     (8  + QDI_PRIVATE)
#define I2C_QDI_GET_STOP_TIMESTAMP      (9  + QDI_PRIVATE)
#define I2C_QDI_GET_CALLBACK_DATA       (10 + QDI_PRIVATE)
#define I2C_QDI_SETUP_LPI_RESOURCE      (11 + QDI_PRIVATE)
#define I2C_QDI_RESET_LPI_RESOURCE      (12 + QDI_PRIVATE)
#define I2C_QDI_TEST_PD_RESTART         (14 + QDI_PRIVATE)

#define NUM_QDI_I2C_MAX_DESCS       4
#define MAX_CONCURRENT_CLIENTS      4
#define I2C_CANCEL_SIGNAL           0x2

typedef struct i2c_qdi_opener   i2c_qdi_opener;
typedef struct i2c_qdi_ctxt     i2c_qdi_ctxt;
typedef struct i2c_qdi_callback i2c_qdi_callback;

typedef struct i2c_qdi_ctxt
{
    void                *i2c_handle;
    void                *ctxt;
    i2c_descriptor      desc[NUM_QDI_I2C_MAX_DESCS];
    i2c_slave_config    config;
    i2c_qdi_opener      *opener;
    boolean             in_use;

} i2c_qdi_ctxt;

typedef struct i2c_qdi_opener
{
    qurt_qdi_obj_t      obj;

    qurt_pipe_t         pipe;
    qurt_pipe_attr_t    attr;
    qurt_pipe_data_t    data[CB_PIPE_NUM_PARAMETERS * MAX_CONCURRENT_CLIENTS];

    qurt_mutex_t        mutex;
    qurt_signal_t       signal;

    void                *c_handle[NUM_CLIENTS_PER_PD];
    i2c_qdi_ctxt        q_ctxt_list[MAX_CONCURRENT_CLIENTS];
    boolean             cancel_in_progress;

    boolean             in_use;

} i2c_qdi_opener; 

typedef struct i2c_qdi_callback
{
    callback_func   func;
    uint32          status;
    uint32          transferred;
    void            *ctxt;

} i2c_qdi_callback;

void i2c_notify_completion (callback_func func, uint32 status, uint32 transferred, void *context);

#endif
