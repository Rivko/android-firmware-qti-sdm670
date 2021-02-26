/*
 *                            I2C TEST Application
 *                            --------------------
 *       Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated.
 *                            All Rights Reserved.
 *                   Qualcomm Confidential and Proprietary
 */

//
// this example client verifies i2c transfers that are batched by the same
// client as well as multiple clients trying to queue transfers. verification
// for the use cases is done in trace32
//

#define SLPI_TEST_ENABLE

#include "i2c_api.h"
#ifdef SLPI_TEST_ENABLE
#include "i2c_qdi.h"
#endif
#include "spi_api.h"
#ifdef SLPI_TEST_ENABLE
#include "qurt.h"
#include "qurt_printf.h"
#include "pram_mgr.h"
#else
#include "rex.h"
#include "i2c_log.h"
#endif
#include "busywait.h"

#ifndef SLPI_TEST_ENABLE
typedef rex_tcb_type *qurt_signal_t;
#define qurt_signal_init(x)             *(x) = rex_self()
#define qurt_signal_destroy(x)          *(x) = NULL
// TCM workaround
// #define qurt_signal_wait(x, sig, mask)  rex_wait(sig)
#define qurt_signal_wait(x, sig, mask)  while(!(rex_get_sigs(*(x)) & TEST_SIGNAL_GENERIC_SIGNAL))
#define qurt_signal_clear(x, sig)       rex_clr_sigs(*x, sig)
#define qurt_signal_set(x, sig)         rex_set_sigs(*x, sig)
#endif

// logging
#ifdef SLPI_TEST_ENABLE
#define I2C_TEST_LOG(level,args...) qurt_printf (args)
#else
#define I2C_TEST_LOG(level,args...) I2C_LOG(level,args)
#endif

// debug
#define iassert(expr) if (!(expr)) { while (1); }

// signals
#define TEST_SIGNAL_GENERIC_SIGNAL      0x4
#define TEST_SIGNAL_GENERIC_WAIT_ALL    0x4

// max
#define MAX_DESCRIPTORS     4
#define MAX_CLIENTS         4

// client context
typedef struct test_client_ctxt
{
    void                *handle;
    i2c_slave_config    config;
    i2c_descriptor      desc[MAX_DESCRIPTORS];
    i2c_status          status;
    uint32              num_descs;
    qurt_signal_t       test_signal;

} test_client_ctxt;

// buffers
#ifdef SLPI_TEST_ENABLE
#define MAX_BUF_SIZE 4096
static uint8 *wbuffer = NULL;
static uint8 *rbuffer = NULL;
#else
#define MAX_BUF_SIZE 256
static uint8 wbuffer[MAX_BUF_SIZE];
static uint8 rbuffer[MAX_BUF_SIZE];
#endif

// handles
static void *spi_handle;
static test_client_ctxt c[MAX_CLIENTS];
static void *h1, *h2, *h3, *h4, *h5, *h6, *h7, *h8;

// t32 configurable parameters
volatile boolean         i2c_external_slave  = TRUE;             // test on external slave 
volatile boolean         i2c_polled_mode     = TRUE;             // set for polled mode testing
volatile uint32          i2c_infinite_mode   = 1;                // run one or more tests infinitely
volatile uint32          i2c_test_from       = 1;                // specific test to run
volatile uint32          i2c_test_to         = 24;               // specific test to run
volatile uint32          i2c_bus_speed       = 400;              // bus speed
volatile uint8           i2c_slave_address   = 0x10;             // slave address
volatile uint8           i2c_write_offset    = 0xC;              // slave offset
volatile uint32          i2c_bytes_to_write  = 1;                // bytes to write
volatile uint32          i2c_bytes_to_read   = 2;                // bytes to read
volatile boolean         i2c_sync_mode       = FALSE;            // run in synchronous mode
volatile i2c_instance    i2c_test_instance1  = I2C_INSTANCE_001; // instance1 to run test on
volatile i2c_instance    i2c_test_instance2  = I2C_INSTANCE_006; // instance2 to run test on
volatile i2c_mode        i2c_protocol_mode   = I2C;              // protocol for the test

// functions
void i2c_test_pd_restart (void);
void i2c_qdi_user_init (void);

// callback
static void clnt_callback (uint32 i_status, uint32 transferred, void *ctxt);
static callback_func cb_func = clnt_callback;

void i2c_test_signal_init ()
{
    uint32 i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        qurt_signal_init (&(c[i].test_signal));
    }
}

void i2c_test_signal_destroy ()
{
    uint32 i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        qurt_signal_destroy (&(c[i].test_signal));
    }
}

void i2c_test_wait_for_event (test_client_ctxt *ct)
{
    qurt_signal_wait(&(ct->test_signal),
            TEST_SIGNAL_GENERIC_SIGNAL,
            TEST_SIGNAL_GENERIC_WAIT_ALL);

    qurt_signal_clear(&(ct->test_signal),
            TEST_SIGNAL_GENERIC_SIGNAL);
}

void i2c_test_signal_event (test_client_ctxt *ct)
{
    qurt_signal_set(&(ct->test_signal), TEST_SIGNAL_GENERIC_SIGNAL);
}

// client code
void i2c_clients_init (void)
{
    uint32 i = 0;

    i2c_test_signal_init();

#ifdef SLPI_TEST_ENABLE
    rbuffer = (uint8 *) pram_acquire_partition("SENSORS", &i);
    iassert (i >= (MAX_BUF_SIZE << 1));
    wbuffer = rbuffer + MAX_BUF_SIZE;
#endif

    for (i = 0; i < MAX_BUF_SIZE; i++)
    {
        wbuffer[i] = 0x0c;
        rbuffer[i] = 0xff;
    }

    if (i2c_external_slave)
    {
        i2c_bytes_to_write  = MAX_BUF_SIZE;
        i2c_bytes_to_read   = MAX_BUF_SIZE;
    }
}

void clnt_init
(
    test_client_ctxt *ct,
    i2c_instance core_num,
    uint32 slave_address,
    uint32 bus_speed_khz
)
{
    i2c_status i_status = I2C_SUCCESS;
    i_status = i2c_open(core_num, &ct->handle);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_open returned %d\n", i_status);
    iassert(I2C_SUCCESS(i_status));

    ct->config.bus_frequency_khz           = bus_speed_khz;
    ct->config.slave_address               = slave_address;
    ct->config.mode                        = i2c_protocol_mode;
    ct->config.slave_max_clock_stretch_us  = 0;
    ct->config.core_configuration1         = 0;
    ct->config.core_configuration2         = 0;

    ct->num_descs   = 0;
    ct->status      = I2C_SUCCESS;
}

void clnt_deinit (test_client_ctxt *ct)
{
    i2c_status i_status = I2C_SUCCESS;
    i_status = i2c_close (ct->handle);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_close returned %d\n", i_status);
    iassert(I2C_SUCCESS(i_status));

    ct->handle = NULL;

    ct->config.bus_frequency_khz           = 0;
    ct->config.slave_address               = 0;
    ct->config.mode                        = 0;
    ct->config.slave_max_clock_stretch_us  = 0;
    ct->config.core_configuration1         = 0;
    ct->config.core_configuration2         = 0;

    ct->num_descs   = 0;
    ct->status      = 0;
}

void clnt_signal_event(test_client_ctxt *ct)
{
    if (cb_func != NULL)
    {
        i2c_test_signal_event (ct);
    }
}

void clnt_wait_for_event(test_client_ctxt *ct)
{
    if (cb_func != NULL)
    {
        i2c_test_wait_for_event (ct);
    }
}

void clnt_callback (uint32 i_status, uint32 transferred, void *ctxt)
{
    test_client_ctxt *ct = (test_client_ctxt *) ctxt;
    i2c_descriptor *dc = ct->desc;
    uint32 index = 0;

    I2C_TEST_LOG(LEVEL_INFO, "clnt_callback returned %d\n", i_status);
    I2C_TEST_LOG(LEVEL_INFO, "clnt_callback transferred %d\n", transferred);

    if ((i_status != I2C_ERROR_START_STOP_UNEXPECTED)   &&
        (i_status != I2C_ERROR_DATA_NACK)               &&
        (i_status != I2C_ERROR_ADDR_NACK)               &&
        (i_status != I2C_ERROR_ARBITRATION_LOST)        &&
        (i_status != I2C_TRANSFER_FORCE_TERMINATED))
    {
        iassert(I2C_SUCCESS(i_status));
    }

    // if you had multiple descriptors verify number of bytes transferred
    if (I2C_SUCCESS(i_status))
    {
        for (index = 0; index < ct->num_descs; index++)
        {
            iassert(transferred >= dc->length);
            transferred -= dc->length;
            dc++;
        }
    }

    clnt_signal_event(ct);
}

i2c_status clnt_write (test_client_ctxt *h, uint8 *buffer, uint32 length)
{
    i2c_status i_status = I2C_SUCCESS;

    h->desc->buffer = buffer;
    h->desc->length = length;
    h->desc->flags = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 1;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_read (test_client_ctxt *h, uint8 *buf, uint32 length)
{
    i2c_status i_status = I2C_SUCCESS;

    h->desc->buffer = buf;
    h->desc->length = length;
    h->desc->flags = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 1;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_write_read (test_client_ctxt *h, uint8 *wbuf, uint8 *rbuf, uint16 wlen, uint16 rlen)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;

    dc->buffer = wbuf;
    dc->length = wlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_WRITE;

    dc++;

    dc->buffer = rbuf;
    dc->length = rlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 2;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_write_write (test_client_ctxt *h, uint8 *wbuf, uint16 wlen1, uint16 wlen2)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;

    dc->buffer = wbuf;
    dc->length = wlen1;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_WRITE;

    dc++;

    dc->buffer = wbuf + wlen1;
    dc->length = wlen2;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 2;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_read_read (test_client_ctxt *h, uint8 *rbuf, uint16 rlen1, uint16 rlen2)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;

    dc->buffer = rbuf;
    dc->length = rlen1;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_READ;

    dc++;

    dc->buffer = rbuf + rlen1;
    dc->length = rlen2;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 2;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_read_write (test_client_ctxt *h, uint8 *wbuf, uint8 *rbuf, uint16 wlen, uint16 rlen)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;

    dc->buffer = rbuf;
    dc->length = rlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_READ;

    dc++;

    dc->buffer = wbuf;
    dc->length = wlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;
    h->num_descs = 2;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_write_read_timestamp (test_client_ctxt *h, uint8 *wbuf, uint8 *rbuf, uint16 wlen, uint16 rlen)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;

    dc->buffer = wbuf;
    dc->length = wlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_TIMESTAMP_S;

    dc++;

    dc->buffer = rbuf;
    dc->length = rlen;
    dc->flags  = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP | I2C_FLAG_TIMESTAMP_P;

    h->status = I2C_SUCCESS;
    h->num_descs = 2;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

i2c_status clnt_batch (test_client_ctxt *h, uint8 *wbuf, uint8 *rbuf, uint16 wlen, uint16 rlen, uint32 sequence)
{
    i2c_descriptor *dc = h->desc;
    i2c_status i_status = I2C_SUCCESS;
    uint32 i = 0;

    for (i = 0; i < MAX_DESCRIPTORS; i++)
    {
        if (i == (MAX_DESCRIPTORS - 1))
        {
            dc->flags = I2C_FLAG_STOP;
        }

        if (sequence & (1 << i))
        {
            dc->buffer = wbuf;
            dc->length = wlen;
            dc->flags |= I2C_FLAG_START | I2C_FLAG_WRITE;
        }
        else
        {
            dc->buffer = rbuf;
            dc->length = rlen;
            dc->flags |= I2C_FLAG_START | I2C_FLAG_READ;
        }

        dc++;
    }

    h->status = I2C_SUCCESS;
    h->num_descs = MAX_DESCRIPTORS;

    if (i2c_sync_mode)  { cb_func = NULL; }
    else                { cb_func = clnt_callback; }

    i_status = i2c_transfer (h->handle, &h->config, h->desc, h->num_descs, cb_func, h, 0, NULL);
    I2C_TEST_LOG(LEVEL_INFO, "i2c_transfer returned %d\n", i_status);
    return i_status;
}

volatile uint32 break_here = 0;

void i2c_test_start (void *sth)
{
    uint32 i = 0;

    // 0. init stuff
    i2c_clients_init();

    do
    {
        switch (i2c_test_from)
        {
            case 1:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // test open and close multiple times.
                // - check that hw_ctxt is created only once per core. 
                // - check that plat is initialized only once per i2c core.
                // - check that hw_ctxt is freed on error.
                // - check ref_count is incremented for success.
                // - check that close decrements the count and frees it on 0.
                // - make sure that the hw_ctxt is freed after all cleanup

                // cores_head has link list of 2 cores
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h1)));
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_006, &h2)));

                // instance 3 and 4 should fail
                iassert(I2C_ERROR(i2c_open(I2C_INSTANCE_003, &h3)));
                iassert(I2C_ERROR(i2c_open(I2C_INSTANCE_004, &h4)));

                // increments ref count
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h5)));
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_006, &h6)));

                // instance 7 and 8 should fail
                iassert(I2C_ERROR(i2c_open(I2C_INSTANCE_007, &h7)));
                iassert(I2C_ERROR(i2c_open(I2C_INSTANCE_008, &h8)));

                // decrement ref count
                iassert(I2C_SUCCESS(i2c_close(h1)));
                iassert(I2C_SUCCESS(i2c_close(h2)));

                // instance 3 and 4 should fail
                iassert(i2c_close(h3) == I2C_ERROR_INVALID_PARAMETER);
                iassert(i2c_close(h4) == I2C_ERROR_INVALID_PARAMETER);

                // ref count goes to 0
                iassert(I2C_SUCCESS(i2c_close(h5)));
                iassert(I2C_SUCCESS(i2c_close(h6)));

                // instance 3 and 4 should fail
                iassert(i2c_close(h7) == I2C_ERROR_INVALID_PARAMETER);
                iassert(i2c_close(h8) == I2C_ERROR_INVALID_PARAMETER);

                break;

            case 2:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // test multiple clients opening connection to same core.
                // - check that hw_info is created only once per core. 
                // - check that plat is initialized only once per i2c core.
                // - check ref_count is incremented for success.
                // - check that close decrements the count and frees it on 0.
                // - make sure that the hw_info is freed after all cleanup

                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h1)));
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h2)));
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h3)));
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h4)));

                iassert(I2C_SUCCESS(i2c_close(h1)));
                iassert(I2C_SUCCESS(i2c_close(h2)));
                iassert(I2C_SUCCESS(i2c_close(h3)));
                iassert(I2C_SUCCESS(i2c_close(h4)));

                break;

            case 3:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // test back to back queueing from multiple clients to same core
                // - verify that the hw_ctxts are queued correctly
                // - make sure that the interrupt advances correctly

                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                clnt_init (&c[1], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);

                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));
                iassert(I2C_SUCCESS(i2c_power_on(c[1].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                iassert(I2C_SUCCESS(clnt_read  (&c[1], rbuffer, i2c_bytes_to_read)));

                clnt_wait_for_event (&c[0]);
                clnt_wait_for_event (&c[1]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                iassert(I2C_SUCCESS(i2c_power_off(c[1].handle)));

                clnt_deinit(&c[0]);
                clnt_deinit(&c[1]);

                break;

            case 4:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // simple write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 5:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // simple read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 6:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write followed by read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write_read (&c[0], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 7:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write followed by read in a loop
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                for (i = 0; i < 20; i++)
                {
                    iassert(I2C_SUCCESS(clnt_write_read (&c[0], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                    clnt_wait_for_event (&c[0]);
                }

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 8:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write then read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 9:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write then write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 10:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // read then read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 11:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // read then write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 12:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // access 2 i2c instances
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                clnt_init (&c[1], i2c_test_instance2, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[1].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[1], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[1]);

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[1].handle)));
                clnt_deinit(&c[1]);

                break;

            case 13:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // access i2c then spi instance
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(SPI_SUCCESS == spi_open(2, &spi_handle));
                iassert(SPI_SUCCESS == spi_power_on(spi_handle));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                iassert(SPI_SUCCESS == spi_power_off(spi_handle));
                iassert(SPI_SUCCESS == spi_close(spi_handle));

                break;

            case 14:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // open and close test
                iassert(I2C_SUCCESS(i2c_open(I2C_INSTANCE_001, &h1)));
                iassert(I2C_SUCCESS(i2c_close(h1)));

                break;

            case 15:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // lpi resource setup and reset
                iassert(I2C_SUCCESS(i2c_setup_lpi_resource(I2C_INSTANCE_001,   100)));
                iassert(I2C_SUCCESS(i2c_setup_lpi_resource(I2C_INSTANCE_001,   400)));
                iassert(I2C_SUCCESS(i2c_setup_lpi_resource(I2C_INSTANCE_001,  1000)));
                iassert(I2C_SUCCESS(i2c_setup_lpi_resource(I2C_INSTANCE_001,  8000)));

                iassert(I2C_SUCCESS(i2c_reset_lpi_resource(I2C_INSTANCE_001)));
                iassert(I2C_SUCCESS(i2c_reset_lpi_resource(I2C_INSTANCE_001)));
                iassert(I2C_SUCCESS(i2c_reset_lpi_resource(I2C_INSTANCE_001)));
                iassert(I2C_SUCCESS(i2c_reset_lpi_resource(I2C_INSTANCE_001)));

                iassert(i2c_setup_lpi_resource(I2C_INSTANCE_001, 14000)  == I2C_ERROR_PLATFORM_RESOURCE_SETUP_FAIL);
                iassert(i2c_setup_lpi_resource(I2C_INSTANCE_001, 200000) == I2C_ERROR_PLATFORM_RESOURCE_SETUP_FAIL);

                break;

            case 16:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write followed by write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write_write (&c[0], wbuffer, 1, 1)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 17:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // read followed by read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read_read (&c[0], rbuffer, 2, 2)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 18:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // read followed by write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read_write (&c[0], wbuffer, rbuffer, 1, 2)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 19:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // write followed by read with timestamps
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write_read_timestamp (&c[0], wbuffer, rbuffer, 1, 2)));
                clnt_wait_for_event (&c[0]);

                I2C_TEST_LOG(LEVEL_INFO, "start ts = %d\n", i2c_get_start_timestamp(c[0].handle));
                I2C_TEST_LOG(LEVEL_INFO, "stop  ts = %d\n", i2c_get_stop_timestamp (c[0].handle));

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 20:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // batch
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_batch (&c[0], wbuffer, rbuffer, 1, 2, 5)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 21:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // test NACK
                
                // a client for slave 0x10
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                // a client for slave 0x42
                clnt_init (&c[1], i2c_test_instance1, 0x42, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[1].handle)));

                // success write-read
                iassert(I2C_SUCCESS(clnt_write_read (&c[0], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                // nack write-read
                iassert(I2C_SUCCESS(clnt_write_read (&c[1], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[1]);

                // success write-read
                iassert(I2C_SUCCESS(clnt_write_read (&c[0], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                // nack-write
                iassert(I2C_SUCCESS(clnt_write (&c[1], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[1]);

                // success-write
                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                clnt_wait_for_event (&c[0]);

                // success read
                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                // nack read
                iassert(I2C_SUCCESS(clnt_read (&c[1], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[1]);

                // success read
                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);

                // done client 0
                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                // done client 1
                iassert(I2C_SUCCESS(i2c_power_off(c[1].handle)));
                clnt_deinit(&c[1]);

                break;

            case 22:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // cancel write
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));
                busywait (100);
                if (i2c_polled_mode) { iassert(i2c_cancel_transfer(c[0].handle) == I2C_TRANSFER_INVALID); }
                else                 { iassert(i2c_cancel_transfer(c[0].handle) == I2C_TRANSFER_FORCE_TERMINATED); }
                clnt_wait_for_event (&c[0]);

                // check sanity
                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, 20)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

            case 23:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // cancel read
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));

                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, i2c_bytes_to_read)));
                busywait (100);
                if (i2c_polled_mode) { iassert(i2c_cancel_transfer(c[0].handle) == I2C_TRANSFER_INVALID); }
                else                 { iassert(i2c_cancel_transfer(c[0].handle) == I2C_TRANSFER_FORCE_TERMINATED); }
                clnt_wait_for_event (&c[0]);

                // check sanity
                iassert(I2C_SUCCESS(clnt_read (&c[0], rbuffer, 20)));
                clnt_wait_for_event (&c[0]);

                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;

#ifdef SLPI_TEST_ENABLE
            case 24:
                I2C_TEST_LOG(LEVEL_INFO, "test %d\n", i2c_test_from);
                // pd restart
                // have some clients open i2c connections and some do power on
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));
                clnt_init (&c[1], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[1].handle)));
                clnt_init (&c[2], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                clnt_init (&c[3], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);

                // do a write
                iassert(I2C_SUCCESS(clnt_write (&c[0], wbuffer, i2c_bytes_to_write)));

                // simulate qdi release
                i2c_test_pd_restart ();
                break_here = 1;

                i2c_test_signal_destroy();
                i2c_test_signal_init();

                // simulate qdi open
                i2c_qdi_user_init ();
                break_here = 1;

                // test sanity
                clnt_init (&c[0], i2c_test_instance1, i2c_slave_address, i2c_bus_speed);
                iassert(I2C_SUCCESS(i2c_power_on(c[0].handle)));
                iassert(I2C_SUCCESS(clnt_write_read (&c[0], wbuffer, rbuffer, i2c_bytes_to_write, i2c_bytes_to_read)));
                clnt_wait_for_event (&c[0]);
                iassert(I2C_SUCCESS(i2c_power_off(c[0].handle)));
                clnt_deinit(&c[0]);

                break;
#endif

            default:
                I2C_TEST_LOG(LEVEL_INFO, "[i2c test] invalid test %d\n", i2c_test_from);
        }
        i2c_test_from++;
    } while (i2c_infinite_mode && (i2c_test_from <= i2c_test_to));

    break_here = 1;
}

#ifdef SLPI_TEST_ENABLE
// threads
#define TEST_THREAD_STACK_SIZE    512

qurt_thread_attr_t       i_test_thread_attr;
qurt_thread_t            i_test_thread;

static char i_test_stack[TEST_THREAD_STACK_SIZE];

void i2c_test_init(void)
{
    // set tf workloop attribute
    qurt_thread_attr_init(&i_test_thread_attr);
    qurt_thread_attr_set_name(&i_test_thread_attr, "i2c_test_thread");
    qurt_thread_attr_set_priority(&i_test_thread_attr, QURT_THREAD_ATTR_PRIORITY_DEFAULT);
    qurt_thread_attr_set_stack_size(&i_test_thread_attr, TEST_THREAD_STACK_SIZE);
    qurt_thread_attr_set_stack_addr(&i_test_thread_attr, (void *)i_test_stack);

    // create workloop
    qurt_thread_create( &i_test_thread,
                        &i_test_thread_attr,
                        i2c_test_start,
                        NULL);
}
#else
// task
#define I2C_TASK_STACK_SIZE       1024

static rex_tcb_type i2c_task_tcb;
static rex_stack_word_type i2c_task_stack[I2C_TASK_STACK_SIZE / sizeof(rex_stack_word_type)];

void i2c_test_init(void)
{
   rex_def_task_ext((rex_tcb_type *) &i2c_task_tcb,
                    (unsigned char *) &i2c_task_stack[0],
                    I2C_TASK_STACK_SIZE,
                    0x5F,
                    (rex_task_func_type) i2c_test_start,
                    0,
                    "i2c bus test",
                    FALSE);
}
#endif
