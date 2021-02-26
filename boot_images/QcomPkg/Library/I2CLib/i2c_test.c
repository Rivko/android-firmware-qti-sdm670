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

#include "i2c_api.h"
//#include "rex.h"

#define INTLOCK()

#define MAX_BUF_SIZE 256
#define TEST_SIGNAL_GENERIC_SIGNAL      0x1
#define TEST_SIGNAL_GENERIC_WAIT_ALL    0x1

#define iassert(expr)   \
    if (!(expr))        \
    {                   \
        while (1);      \
    }

#if 0
static void *h1, *h2, *h3, *h4, *h5, *h6, *h7, *h8;
#endif

uint8 wbuffer[MAX_BUF_SIZE];
uint8 rbuffer[MAX_BUF_SIZE];

// 0x10 for read 2 bytes
uint32  i2c_bus_speed       = 400;
uint8   i2c_slave_address   = 0x50;
uint8   offset              = 0x10;
uint8   bytes_to_read       = 8;

//i2c_instance i2c_test_inst = I2C_INSTANCE_006;
i2c_instance i2c_test_inst = I2C_INSTANCE_011;

boolean do_write        = TRUE;
boolean do_read         = FALSE;
boolean do_write_read   = TRUE;

boolean enable_back_to_back = FALSE;



typedef struct test_client_ctxt
{
    void                *handle;
    i2c_slave_config    *config;
    i2c_status          status;
    i2c_descriptor      *desc;
    uint32              num_descs;

} test_client_ctxt;

static i2c_slave_config test_i2c_slave_config[2];
static i2c_descriptor   test_i2c_desc[4];
static test_client_ctxt c1;
// static test_client_ctxt c2;


static uint32 *p_tcb = NULL;
void i2c_test_wait_for_event ()
{
//    rex_wait(TEST_SIGNAL_GENERIC_SIGNAL);
//    rex_clr_sigs(p_tcb, TEST_SIGNAL_GENERIC_SIGNAL);
}

void i2c_test_signal_event ()
{
//    rex_set_sigs(p_tcb, TEST_SIGNAL_GENERIC_SIGNAL);
}

void i2c_clients_init (void)
{
    uint32 i = 0;

    p_tcb = (uint32 *)1;//rex_self();

    c1.config = &test_i2c_slave_config[0];
    // c2.config = &test_i2c_slave_config[1];

    c1.desc = &test_i2c_desc[0];
    // c2.desc = &test_i2c_desc[2];

    for (i = 0; i < MAX_BUF_SIZE; i++)
    {
        wbuffer[i] = 0xff;
        rbuffer[i] = 0xff;
    }
}

void clnt_init
(
    test_client_ctxt *ct,
    i2c_instance core_num,
    uint32 slave_address,
    uint32 bus_speed_khz,
    uint32 num_descs
)
{
    iassert(I2C_SUCCESS(i2c_open(core_num, &ct->handle)));

    ct->config->bus_frequency_khz           = bus_speed_khz;
    ct->config->slave_address               = slave_address;
    ct->config->mode                        = I2C;
    ct->config->slave_max_clock_stretch_us  = 0;
    ct->config->core_configuration1         = 0;
    ct->config->core_configuration2         = 0;

    ct->num_descs = num_descs;
    ct->status = I2C_SUCCESS;
	
    iassert(I2C_SUCCESS(i2c_power_on(ct->handle)));
}

void client_deinit (test_client_ctxt *ct)
{
    iassert(I2C_SUCCESS(i2c_close (ct->handle)));
}

void clnt_signal_event(test_client_ctxt *ct)
{
    i2c_test_signal_event ();
}

void clnt_wait_for_event(test_client_ctxt *ct)
{
    i2c_test_wait_for_event ();
}

void clnt_callback (uint32 i_status, uint32 transferred, void *ctxt)
{
    test_client_ctxt *ct = (test_client_ctxt *) ctxt;
    i2c_descriptor *dc = ct->desc;
    uint32 index = 0;

    if (i_status != I2C_TRANSFER_FORCE_TERMINATED)
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
    h->desc->buffer = buffer;
    h->desc->length = length;
    h->desc->flags = I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;

    return i2c_transfer (h->handle, h->config, h->desc, 1, clnt_callback, h, 0, NULL);
}

i2c_status clnt_read (test_client_ctxt *h, uint8 *buffer, uint32 length)
{
    h->desc->buffer = buffer;
    h->desc->length = length;
    h->desc->flags = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

    h->status = I2C_SUCCESS;

    return i2c_transfer (h->handle, h->config, h->desc, 1, clnt_callback, h, 0, NULL);
}

i2c_status clnt_batch (test_client_ctxt *h, uint8 *wbuffer, uint8 *rbuffer, uint16 wlen, uint16 rlen)
{
    uint32 i = 0;
    i2c_descriptor *dc = h->desc;

    dc->buffer = wbuffer;
    dc->length = wlen;
    dc->flags = I2C_FLAG_START | I2C_FLAG_WRITE;

    dc++;

    dc->buffer = rbuffer;
    dc->length = rlen;

    while (i < rlen)
    {
        rbuffer[i] = 0xFF;
        i++;
    }
    dc->flags = I2C_FLAG_START | I2C_FLAG_READ | I2C_FLAG_STOP;

    return i2c_transfer (h->handle, h->config, h->desc, h->num_descs, clnt_callback, h, 0, NULL);
}

void i2c_test_init (void)
{
    int i = 0;
    // 0. init stuff
    i2c_clients_init();

    while (1)
    {
        // 4. Simple Write
        if (do_write)
        {
            clnt_init (&c1, i2c_test_inst, i2c_slave_address, i2c_bus_speed, 1);
			wbuffer[0] = 0x00;
            wbuffer[1] = offset;
			for(i=2;i<9;i++)
			{
				wbuffer[i] = 0xA5; 
			}	
            iassert(I2C_SUCCESS(clnt_write (&c1, wbuffer, 9)));
            clnt_wait_for_event (&c1);
       //     client_deinit(&c1);
        }

        // 5. Simple Read
        if (do_read)
        {
            clnt_init (&c1, i2c_test_inst, i2c_slave_address, i2c_bus_speed, 1);
            iassert(I2C_SUCCESS(clnt_read (&c1, rbuffer + 1, bytes_to_read)));
            clnt_wait_for_event (&c1);
            client_deinit(&c1);
        }

        // 6. Write followed by Read
        if (do_write_read)
        {
          //clnt_init (&c1, i2c_test_inst, i2c_slave_address, i2c_bus_speed, 2);
            c1.num_descs = 2;
            wbuffer[0] = 0x00;
            wbuffer[1] = offset;
            do
            {
                iassert(I2C_SUCCESS(clnt_batch (&c1, wbuffer, rbuffer, 2, bytes_to_read)));
                clnt_wait_for_event (&c1);
            }
            while (enable_back_to_back);

            client_deinit(&c1);
        }
    }
}
