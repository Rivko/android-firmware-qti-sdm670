/**
    @file  spi_test.c
    @brief
 */
/*=============================================================================
            Copyright (c) 2016-17 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*==================================================================================================
Edit History

$Header: 

when       who          what, where, why
--------   --------     --------------------------------------------------------
08/01/17   ska          Ported for sdm670

==================================================================================================*/

#include "spi_api.h"
#include "qurt.h"
#include "pram_mgr.h"

#define SSC_SPI_PRAM_BASE  0x62BA0000
#define SPI_TX_OFFSET  0x0
#define SPI_RX_OFFSET  0x100

#define MAX_BUF_SIZE 1024
uint8 *tx_buffer = NULL;
uint8 *rx_buffer = NULL;

spi_instance_t spi_test_inst = SPI_INSTANCE_2;
uint32 vin_where_am_i = 0;
qurt_signal_t test_signal;

struct spi_client_ctxt
{
    void                *handle;
    spi_config_t        *config;
    spi_descriptor_t    *desc;
    uint32              num_descs;
};

static spi_config_t test_spi_config;
static spi_descriptor_t test_spi_desc[2];
static struct spi_client_ctxt c1, c2, c3;

#define TEST_SIGNAL_GENERIC_SIGNAL      0x1
#define TEST_SIGNAL_GENERIC_WAIT_ALL    0x1

void spi_test_wait_for_event ()
{
    qurt_signal_wait(&test_signal,
            TEST_SIGNAL_GENERIC_SIGNAL,
            TEST_SIGNAL_GENERIC_WAIT_ALL);

    qurt_signal_clear(&test_signal,
            TEST_SIGNAL_GENERIC_SIGNAL);
}

void spi_test_signal_event ()
{
    qurt_signal_set(&test_signal, TEST_SIGNAL_GENERIC_SIGNAL);
}



void spi_cb (uint32 status, void *ctxt)
{
    struct spi_client_ctxt *c = (struct spi_client_ctxt *)ctxt;
    spi_descriptor_t *dc = c->desc;
    uint32 i = 0;

    if (status == SPI_SUCCESS) //good, verify data
    {
        for (i = 0; i < dc->len; i++)
        {
            if (dc->rx_buf[i] != dc->tx_buf[i])
            {
                while (1);
            }
        }
    }
    //rex_set_sigs(p_tcb, SPI_TEST_SIGNAL_EVENT_COMMAND_DONE);
    spi_test_signal_event();
}

void spi_test_init (void *ptr)
{
    int i;
	uint32 j = 0;
    spi_status_t s;

    //init cb completion signal
    qurt_signal_init (&test_signal);

    //open
    s = spi_open(spi_test_inst, &c1.handle);
    vin_where_am_i++;
    if (s != SPI_SUCCESS) while (1);
    vin_where_am_i++;

    s = spi_open(spi_test_inst, &c2.handle);
    vin_where_am_i++;
    if (s != SPI_SUCCESS) while (1);
    vin_where_am_i++;

    s = spi_open(spi_test_inst, &c3.handle);
    vin_where_am_i++;
    if (s != SPI_SUCCESS) while (1);
    vin_where_am_i++;

    s = spi_close(c2.handle);
    vin_where_am_i++;
    if (s != SPI_SUCCESS) while (1);
    vin_where_am_i++;

    s = spi_close(c3.handle);
    vin_where_am_i++;
    if (s != SPI_SUCCESS) while (1);
    vin_where_am_i++;
    //set configs
    test_spi_config.spi_mode = SPI_MODE_0;
    test_spi_config.spi_cs_polarity = SPI_CS_ACTIVE_LOW;
    test_spi_config.endianness = SPI_NATIVE;
    test_spi_config.spi_bits_per_word = 8;
    test_spi_config.spi_slave_index = 0;
    test_spi_config.clk_freq_hz  = 30000000;
    test_spi_config.cs_clk_delay_cycles = 0;
    test_spi_config.inter_word_delay_cycles = 0;
    test_spi_config.cs_toggle = 0;
    test_spi_config.loopback_mode = 1;

    c1.config = &test_spi_config;

	tx_buffer = (uint8 *) pram_acquire_partition("SENSORS", &j);
	rx_buffer = (uint8 *) tx_buffer + 0x200;
	

	
    //populate descriptors
    test_spi_desc[0].tx_buf = tx_buffer;
    test_spi_desc[0].rx_buf = rx_buffer;
    test_spi_desc[0].len = 20;

    test_spi_desc[1].tx_buf = tx_buffer+40;
    test_spi_desc[1].rx_buf = rx_buffer+40;
    test_spi_desc[1].len = 20;


    //populate buffers with known test data
    for (i = 0; i < test_spi_desc[0].len; i++)
    {
        test_spi_desc[0].tx_buf[i] = i;
        test_spi_desc[0].rx_buf[i] = 0;
        test_spi_desc[1].tx_buf[i] = i+10;
        test_spi_desc[1].rx_buf[i] = 0;
    }

    c1.desc = test_spi_desc;
    c1.num_descs = 2;

    //set the event to wait for transfer completion
    //spi_test_signal_event();

    spi_power_on(c1.handle);
    //loopback transfer
    s = spi_full_duplex (c1.handle, c1.config, c1.desc, c1.num_descs, spi_cb, &c1, 0);
    if (s != SPI_SUCCESS) while (1);

    //wait for completion
    spi_test_wait_for_event();

    //destroy signal
    qurt_signal_destroy (&test_signal);

    //close
    s = spi_close(c1.handle);
    if (s != SPI_SUCCESS) while (1);
}

#define TEST_THREAD_STACK_SIZE    4096

qurt_thread_attr_t       spi_test_thread_attr;
qurt_thread_t            spi_test_thread;

static char i_test_stack[TEST_THREAD_STACK_SIZE];

void spi_thread_test_init(void)
{
    // set tf workloop attribute
    qurt_thread_attr_init(&spi_test_thread_attr);
    qurt_thread_attr_set_name(&spi_test_thread_attr, "spi_test_thread");
    qurt_thread_attr_set_priority(&spi_test_thread_attr, QURT_THREAD_ATTR_PRIORITY_DEFAULT);
    qurt_thread_attr_set_stack_size(&spi_test_thread_attr, TEST_THREAD_STACK_SIZE);
    qurt_thread_attr_set_stack_addr(&spi_test_thread_attr, (void *)i_test_stack);

    // create workloop
    qurt_thread_create( &spi_test_thread,
                        &spi_test_thread_attr,
                        spi_test_init,
                        NULL);
}
