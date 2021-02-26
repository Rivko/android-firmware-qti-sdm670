/**
  @file spi_api.h
  @brief SPI Driver API
*/
/*
  ===========================================================================

  Copyright (c) 2019-2020 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===========================================================================


  ===========================================================================
*/
#ifndef __SPIapi_H__
#define __SPIapi_H__

#include "comdef.h"
#include "SpiDeviceTypes.h"
#include "SpiDevice.h"


#define SPI_SUCCESS(x)  (x == SPI_SUCCESS)
#define SPI_ERROR(x)    (x != SPI_SUCCESS)

typedef void (*callback_fn) (uint32 transfer_status, void *callback_ctxt);




/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/* spi_power_on and off
Setup and power up/down clocks and TLMM pins.*/
spi_status spi_power_on (void *spi_handle);
spi_status spi_power_off (void *spi_handle);


/* spi_full_duplex : Perform full duplex transfer over SPI BUS.
This is the only available transfer function.Callback is invoked upon completion of the full chain of
descriptors or an error condition.*/
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
    );

#endif  /* Double inclusion protection*/

