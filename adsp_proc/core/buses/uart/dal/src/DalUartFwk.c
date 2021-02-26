/*==================================================================================================

FILE: DalUartFwk.c

DESCRIPTION: Interface between UART driver and DAL framework

                           Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/dal/src/DalUartFwk.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   fwk_sleep_ms
   fwk_wait_event

LOCAL FUNCTIONS:
   dal_uart_attach
   dal_uart_detach
   dal_uart_sys_request

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#include "DalUartFwk.tmh"
#else
#include "DALDeviceId.h"
#include "DALFramework.h"
#endif

#include "DalUartFwk.h"
#include "DalUart.h"
#include "DDIUart.h"

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static DALResult dal_uart_attach(const char *arg, DALDEVICEID device_id, DalDeviceHandle **phandle);
static uint32    dal_uart_detach(uint32 ClientHandle, DalDeviceHandle *h);
static DALResult dal_uart_sys_request(uint32 ClientHandle, DalDeviceHandle *h, DalSysReq request,
                                      const void *src_buf, uint32 src_len,
                                      void *dest_buf, uint32 dest_len, uint32 *dest_req);

/*==================================================================================================
                                       LOCAL/GLOBAL VARIABLES
==================================================================================================*/

// This is the list of devices supported by this driver.

static DALDEVICEID device_array[] = { DALDEVICEID_UARTLG_DEVICE_1,
                                      DALDEVICEID_UARTLG_DEVICE_2,
                                      DALDEVICEID_UARTLG_DEVICE_3,
                                      DALDEVICEID_UARTDM_DEVICE_1,
                                      DALDEVICEID_UARTDM_DEVICE_2,
                                      DALDEVICEID_UARTGSBI_DEVICE_0,
                                      DALDEVICEID_UARTGSBI_DEVICE_1,
                                      DALDEVICEID_UARTGSBI_DEVICE_2,
                                      DALDEVICEID_UARTGSBI_DEVICE_3,
                                      DALDEVICEID_UARTGSBI_DEVICE_4,
                                      DALDEVICEID_UARTGSBI_DEVICE_5,
                                      DALDEVICEID_UARTGSBI_DEVICE_6,
                                      DALDEVICEID_UARTGSBI_DEVICE_7,
                                      DALDEVICEID_UARTGSBI_DEVICE_8,
                                      DALDEVICEID_UARTGSBI_DEVICE_9,
                                      DALDEVICEID_UARTGSBI_DEVICE_10,
                                      DALDEVICEID_UARTGSBI_DEVICE_11,
                                      DALDEVICEID_UARTGSBI_DEVICE_12,
                                      DALDEVICEID_UARTBAM_DEVICE_1,
                                      DALDEVICEID_UARTBAM_DEVICE_2,
                                      DALDEVICEID_UARTBAM_DEVICE_3,
                                      DALDEVICEID_UARTBAM_DEVICE_4,
                                      DALDEVICEID_UARTBAM_DEVICE_5,
                                      DALDEVICEID_UARTBAM_DEVICE_6,
                                      DALDEVICEID_UARTBAM_DEVICE_7,
                                      DALDEVICEID_UARTBAM_DEVICE_8,
                                      DALDEVICEID_UARTBAM_DEVICE_9,
                                      DALDEVICEID_UARTBAM_DEVICE_10,
                                      DALDEVICEID_UARTBAM_DEVICE_11,
                                      DALDEVICEID_UARTBAM_DEVICE_12,
                                      DALDEVICEID_UARTCXM,
                                    };

#define NUM_DEVICES (sizeof(device_array)/sizeof(device_array[0]))

// This global variable is exposed to the DAL framework.  It specifies the devices
// whose attach requests should be routed to this driver.

const DALREG_DriverInfo
DALUart_DriverInfo = { dal_uart_attach, NUM_DEVICES, device_array };

// This array keeps track of which devices are currently in use by a client.
// Only one client at a time will be allowed to attach to each device.

static boolean in_use[NUM_DEVICES] = { 0 };

// The UART interface is a function table that provides the external interface
// to the driver.  All calls into the driver must go through this table.

static const UartInterface uart_interface =
{
   {
      dal_uart_attach,
      dal_uart_detach,
      Uart_DeviceInit,
      Uart_DeviceDeInit,
      Uart_Open,
      Uart_Close,
      Uart_Info,
      Uart_PowerEvent,
      dal_uart_sys_request
   },

   Uart_PostInit,
   Uart_OpenEx,
   Uart_Read,
   Uart_Write,
   Uart_SetRate,
   Uart_SetCharFormat,
   Uart_PurgeTx,
   Uart_PurgeRx,
   Uart_SetBreak,
   Uart_ClearBreak,
   Uart_TxSingleChar,
   Uart_GetStatus,
   Uart_TxComplete,
   Uart_SetRtsControl,
   Uart_SetCtsControl,
   Uart_GetCTS,
   Uart_PowerCapabilities,
   Uart_PowerGet,
   Uart_PowerSet,
   Uart_LoopbackSet,
   Uart_SetCxm,
   Uart_DumpRegs,
   Uart_GetCxmTxSticky,
   Uart_CxmTxDirectChar,
   Uart_RxActive,
};

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: dal_uart_attach

DESCRIPTION:
   Since we cannot allocate a synchronization object at compile time we must rely on the
   DAL framework to ensure that only one thread enters this function (or dal_uart_detach)
   at a time.  Newer versions of the framework will ensure this.  Check for a lock in
   _DAL_DeviceAttach() in DALSysCmn.c.

   We will allocate memory for the device handle and mark the device as in use.

==================================================================================================*/
static DALResult dal_uart_attach(const char *arg, DALDEVICEID device_id, DalDeviceHandle **phandle)
{
   uint32 device_index;
   DalUartHandle *uart_handle;
   DALResult ret_val = DAL_SUCCESS;

   DALSYS_LogEvent(device_id, DALSYS_LOGEVENT_INFO, "+dal_uart_attach(0x%08p, 0x%08X, 0x%08p)",
                                                    arg, device_id, phandle);

   *phandle = NULL;  // set returned handle to NULL in case of failure below

   for (device_index = 0; device_index < NUM_DEVICES; device_index++)
   {
      if (device_array[device_index] == device_id) { break; }
   }

   if (device_index == NUM_DEVICES)
   {
      DALSYS_LogEvent(device_id, DALSYS_LOGEVENT_ERROR, "dal_uart_attach: invalid device");
      ret_val = DAL_ERROR; goto exit;
   }

   if (in_use[device_index])
   {
      DALSYS_LogEvent(device_id, DALSYS_LOGEVENT_ERROR, "dal_uart_attach: device already in use");
      ret_val = DAL_ERROR; goto exit;
   }

   if ( DALSYS_Malloc(sizeof(DalUartHandle), (void**) &uart_handle) != DAL_SUCCESS )
   {
      DALSYS_LogEvent(device_id, DALSYS_LOGEVENT_ERROR, "dal_uart_attach: malloc failed");
      ret_val = DAL_ERROR; goto exit;
   }

   memset(uart_handle, 0, sizeof(DalUartHandle));

   uart_handle->dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   uart_handle->pVtbl = &uart_interface;
   uart_handle->pClientCtxt = &uart_handle->client_ctxt;
   uart_handle->pClientCtxt->pDALDevCtxt = &uart_handle->device_ctxt;
   uart_handle->pClientCtxt->pDALDevCtxt->dwRefs = 1;
   uart_handle->pClientCtxt->pDALDevCtxt->DevId = device_id;

   in_use[device_index] = TRUE;
   *phandle = (DalDeviceHandle *)uart_handle;
exit:
   DALSYS_LogEvent(device_id, DALSYS_LOGEVENT_INFO, "-dal_uart_attach(%d)", ret_val);
   return ret_val;
}

/*==================================================================================================

FUNCTION: dal_uart_detach

DESCRIPTION:
   We are relying on the DAL framework to ensure that only one thread enters this function
   (or dal_uart_attach) at a time.

   We will mark the device not in use and deallocate the memory associated with the handle.

==================================================================================================*/
static uint32 dal_uart_detach(uint32 ClientHandle, DalDeviceHandle *h)
{
   DalUartHandle *uart_handle = (DalUartHandle *)h;
   DALDEVICEID id = uart_handle->pClientCtxt->pDALDevCtxt->DevId;
   uint32 device_index;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+dal_uart_detach(0x%08p)", h);

   for (device_index = 0; device_index < NUM_DEVICES; device_index++)
   {
      if (device_array[device_index] == id) { break; }
   }

   if (device_index == NUM_DEVICES)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dal_uart_detach: invalid device");
      goto exit;
   }

   if (!in_use[device_index])
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dal_uart_detach: device not attached");
      goto exit;
   }

   in_use[device_index] = FALSE;
   DALSYS_Free(uart_handle);
exit:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-dal_uart_detach(0)");
   return 0;
}

/*==================================================================================================

FUNCTION: dal_uart_sys_request

DESCRIPTION:

==================================================================================================*/
static DALResult dal_uart_sys_request(uint32 ClientHandle, DalDeviceHandle *h, DalSysReq request,
                                      const void *src_buf, uint32 src_len,
                                      void *dest_buf, uint32 dest_len, uint32 *dest_req)
{
   DalUartHandle *uart_handle = (DalUartHandle *)h;
   DALDEVICEID id = uart_handle->pClientCtxt->pDALDevCtxt->DevId;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+dal_uart_sys_request(0x%08p, 0x%08X, 0x%08p, 0x%08X, 0x%08p, 0x%08X, 0x%08p)",
                   h, request, src_buf, src_len, dest_buf, dest_len, dest_req);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-dal_uart_sys_request(DAL_ERROR)");
   return DAL_ERROR;
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: fwk_sleep_ms

DESCRIPTION:
   This is clumsy but it is the only way to perform a sleep using the DAL framework.

   Note that duration is converted to microseconds here and then converted right back
   to milliseconds inside DALSYS_EventMultipleWait().  Also note that the duration
   parameter is ignored (and forced to INFINITE) if you don't include a TIMEOUT event
   in the list of events.  These observations were made on WM.

==================================================================================================*/
void fwk_sleep_ms(uint32 duration)
{
   DALSYSEventHandle timer_event = NULL;
   uint32 triggered_event;
   DALResult result;

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT | DALSYS_EVENT_ATTR_NON_DEFERRED,
                               &timer_event, NULL);
   if (result != DAL_SUCCESS) { goto exit; }

   // multiply duration by 1000 to convert from milliseconds to microseconds
   result = DALSYS_EventMultipleWait(&timer_event, 1, duration * 1000, &triggered_event);
   if (result != DAL_ERROR_TIMEOUT) { goto exit; }

exit:
   if (timer_event) { DALSYS_DestroyObject(timer_event); }
}

/*==================================================================================================

FUNCTION: fwk_wait_event

DESCRIPTION:
   This is a helper function for the DAL Framework that makes it less clumsy to wait for an
   event with a timeout.

   Note that the timeout duration is converted to microseconds here and then converted right
   back to milliseconds inside DALSYS_EventMultipleWait().  Also note that the timeout
   parameter is ignored (and forced to INFINITE) if you don't include a TIMEOUT event in the
   list of events.  These observations were made on WM.

==================================================================================================*/
DALResult fwk_wait_event(DALSYSEventHandle caller_event, uint32 timeout_ms)
{
   DALSYSEventHandle event_array[2];
   DALSYSEventHandle timer_event = NULL;
   uint32 triggered_event;
   DALResult result;

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT, &timer_event, NULL);
   if (result != DAL_SUCCESS) { return DAL_ERROR; }

   event_array[0] = caller_event;
   event_array[1] = timer_event;

   // multiply timeout by 1000 to convert from milliseconds to microseconds
   result = DALSYS_EventMultipleWait(event_array, 2, timeout_ms * 1000, &triggered_event);
   DALSYS_DestroyObject(timer_event);

   return(result);
}
