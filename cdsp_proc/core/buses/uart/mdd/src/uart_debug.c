/*==============================================================================

                                  uart_debug.c

GENERAL DESCRIPTION

  This module implements debug code for the UART driver to dump the state of the
  UART registers and the TLMM GPIOs used by the UART driver at the time of a
  crash, so these registers can be recovered from a RAM dump to aid in post-
  mortem debugging.


EXTERNALIZED FUNCTIONS

  siors_mdd_api_dump_uart_status()
    Dumps the hardware state of all open UART ports. This function may be called
    directly by external code at any time.

  siors_mdd_debug_init()
    Initializes the hardware debug information for a specific UART port. This
    function must be called by the UART driver before the other two functions
    exported by this module.

  siors_mdd_debug()
    Dumps the hardware state of a specific UART port. This function may be
    called by the UART driver at any time.


                    Copyright (c) 2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     QUALCOMM Confidential and Proprietary

==============================================================================*/
/*==============================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/src/uart_debug.c#1 $

==============================================================================*/

/*==============================================================================
                                 INCLUDE FILES
==============================================================================*/

#include "uart_debug.h"

#include "siors232_mdd.h"
#include "uart_log.h"

#include "err.h"

#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDITlmm.h"

/*==============================================================================
                               MACRO DEFINITIONS
==============================================================================*/

#define UART_TRACE_BUFFER_FILE_ID 9

/*==============================================================================
                                LOCAL VARIABLES
==============================================================================*/

/** Flag indicating whether the UART error-fatal handler has been registered. */
static boolean uart_debug_callback_registered = FALSE;

/*==============================================================================
                           LOCAL FUNCTION PROTOTYPES
==============================================================================*/

static DALGpioSignalType read_uart_tlmm_config(DalDeviceHandle *tlmm_dal, 
    DALSYSPropertyHandle proph, const char *propname);
static void uart_debug_hw_all(void);
static void uart_debug_hw_port(sio_mdd_port_type *mdd_port);
static void uart_debug_hw_tlmm(sio_mdd_port_type *mdd_port);

/*==============================================================================
                                LOCAL FUNCTIONS
==============================================================================*/

/*============================================================================*/
/**
 * Reads the UART configuration particular port and returns the GPIO
 * configuration for the specified i/o line.
 *
 * @param[in]   tlmm_dal        DAL handle for TLMM
 * @param[in]   proph           DAL property handle for the specified UART
 *                              device
 * @param[in]   propname        Property name from the UART configuration. This
 *                              should be one of "GpioTxData", "GpioRxData",
 *                              "GpioCtsN", or "GpioRfrN".
 *
 * @return      The TLMM configuration for the specified i/o line.
 */
/*============================================================================*/
static DALGpioSignalType read_uart_tlmm_config(DalDeviceHandle *tlmm_dal, 
    DALSYSPropertyHandle proph, const char *propname)
{
  DALResult             result;
  DALSYSPropertyVar     property_variable;
  DALGpioSignalType     program_config;

  result = DALSYS_GetPropertyValue(proph, propname, 0, &property_variable);
  if(result == DAL_SUCCESS && property_variable.Val.dwVal)
  {
    program_config = property_variable.Val.dwVal;
  }
  else
  {
    program_config = 0;
  }

  return program_config;
}

/*============================================================================*/
/**
 * Dump the current state of all UART ports to the UART log.
 *
 * This function is intended to be called during fatal error handling, so it
 * does not lock mutexes or change the state of the driver or hardware.
 */
/*============================================================================*/
static void uart_debug_hw_all(void)
{
  uart_device_id_type i;

  for(i = UART_DEVICE_FIRST; i < UART_DEVICE_LAST; i++)
  {
    uart_debug_hw_port(&siors_mdd_ports[i]);
  }
}

/*============================================================================*/
/**
 * Dump the current state of the UART hardware to the UART log.
 *
 * This function does not lock any mutexes, so it must be called either with the
 * port lock held, or from the fatal-error handler.
 *
 * @param[in]   mdd_port        The UART port structure to dump.
 */
/*============================================================================*/
static void uart_debug_hw_port(sio_mdd_port_type *mdd_port)
{
  DalDeviceHandle *uart_handle;

  if(mdd_port->stream_data.is_open)
  {
    /* Dump the current state of TLMM */
    uart_debug_hw_tlmm(mdd_port);

    /* Dump the current state of the UART registers */
    uart_handle = mdd_port->port_data.dal_device_handle;
    if(uart_handle)
    {
      DalUart_DumpRegs(uart_handle);
    }
  }
}

/*============================================================================*/
/**
 * Reads the state of the TLMM i/o lines used by the UART driver and writes them
 * to the UART log for debugging.
 *
 * @param[in]   mdd_port        The UART port structure to dump.
 */
/*============================================================================*/
static void uart_debug_hw_tlmm(sio_mdd_port_type *mdd_port)
{
  DALResult                     result;
  uart_device_id_type           device_id;
  sio_mdd_debug_data_type      *debug;
  int                           i;

  device_id = mdd_port->port_data.device_id;

  debug = &mdd_port->debug_data;

  if(debug->tlmm_dal == NULL)
  {
    UART_LOG_1( HIGH, "[%d] UART debug: Debug context not initialized", device_id );
    return;
  }

  for(i = 0; i < 4; i++)
  {
    DALGpioSignalType program_config, actual_config;
    uint32 gpio;

    program_config = debug->gpio[i];

    if(program_config == 0)
    {
      continue;
    }

    gpio = DAL_GPIO_NUMBER(program_config);
    result = DalTlmm_GetCurrentConfig(debug->tlmm_dal, gpio, &actual_config);
    if(result == DAL_SUCCESS)
    {
      UART_LOG_3( HIGH, "[%d] GPIO %d: config should be 0x%08x", device_id, gpio, program_config );
      UART_LOG_3( HIGH, "[%d] GPIO %d: config is 0x%08x", device_id, gpio, actual_config );
    }
    else
    {
      UART_LOG_2( ERROR, "[%d] GPIO %d: Unable to get config", device_id, gpio );
    }
  }
}

/*==============================================================================
                                GLOBAL FUNCTIONS
==============================================================================*/

/*============================================================================*/
/**
 * Dumps the hardware state of all open UART ports.
 *
 * This function may be called directly by external code at any time.
 */
/*============================================================================*/
void siors_mdd_api_dump_uart_status( void )
{
  uart_device_id_type i;

  for(i = UART_DEVICE_FIRST; i < UART_DEVICE_LAST; i++)
  {
    siors_mdd_debug(&siors_mdd_ports[i]);
  }
}

/*============================================================================*/
/**
 * Initializes the hardware debug information for a specific UART port.
 *
 * This function must be called by the UART driver before the other two
 * functions exported by this module.
 *
 * @param[in]   mdd_port        The UART port structure to initialize
 */
/*============================================================================*/
void siors_mdd_debug_init(sio_mdd_port_type *mdd_port)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(handle);

  uart_device_id_type           device_id;
  DALDEVICEID                   dal_device_id;
  DALResult                     result;
  DalDeviceHandle              *tlmm_dal = NULL;
  sio_mdd_debug_data_type      *debug;

  device_id = mdd_port->port_data.device_id;

  debug = &mdd_port->debug_data;

  dal_device_id = mdd_port->port_data.dal_device_id;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_debug_init", device_id );

  result = DALSYS_GetDALPropertyHandle(dal_device_id, handle);
  if (result != DAL_SUCCESS)
  {
    UART_LOG_2( ERROR, "[%d] -siors_mdd_debug_init: DALSYS_GetDALPropertyHandle failed: 0x%x", device_id, result );
    return;
  }

  if(debug->tlmm_dal == NULL)
  {
    result = DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_dal);
    if (result != DAL_SUCCESS)
    {
      UART_LOG_2( ERROR, "[%d] -siors_mdd_debug_init: DAL_DeviceAttach failed: 0x%x", device_id, result );
      return;
    }

    result = DalDevice_Open(tlmm_dal, DAL_OPEN_SHARED);
    if (result != DAL_SUCCESS)
    {
      UART_LOG_2( ERROR, "[%d] -siors_mdd_debug_init: DalDevice_Open failed: 0x%x", device_id, result );
      return;
    }

    debug->tlmm_dal = tlmm_dal;
  }

  debug->gpio[0] = read_uart_tlmm_config(tlmm_dal, handle, "GpioTxData");
  debug->gpio[1] = read_uart_tlmm_config(tlmm_dal, handle, "GpioRxData");
  debug->gpio[2] = read_uart_tlmm_config(tlmm_dal, handle, "GpioCtsN");
  debug->gpio[3] = read_uart_tlmm_config(tlmm_dal, handle, "GpioRfrN");

  /* Register the UART debug handler */
  if(uart_debug_callback_registered == FALSE)
  {
    /* Make sure this callback is only registered once, no matter how many
     * times the UART driver is initialized */
    uart_debug_callback_registered = TRUE;
    err_crash_cb_register(uart_debug_hw_all);
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_debug_init", device_id );
}

/*============================================================================*/
/**
 * Dumps the hardware state of a specific UART port.
 *
 * This function may be called by the UART driver at any time. This function
 * locks the UART driver's port lock handle to guarantee exclusive access in a
 * multi-threaded environment.
 *
 * @param[in]   mdd_port        The UART port structure to dump.
 */
/*============================================================================*/
void siors_mdd_debug(sio_mdd_port_type *mdd_port)
{
  DALSYS_SyncEnter(mdd_port->port_data.lock_handle);
  uart_debug_hw_port(mdd_port);
  DALSYS_SyncLeave(mdd_port->port_data.lock_handle);
}
