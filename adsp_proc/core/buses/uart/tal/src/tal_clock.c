/*==================================================================================================

FILE: tal_clock.c

DESCRIPTION: This module defines the "stock" implementation of the clock APIs
             for the Target Abstraction Layer.  This implementation selects
             the correct clock for the UART device and invokes the DAL Clock
             layer API to enable or disable the clock and set the clock
             frequency.

                              Copyright (c) 2012, 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/tal/src/tal_clock.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_clock_close
   tal_clock_disable
   tal_clock_enable
   tal_clock_open
   tal_clock_set_frequency

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "DDIClock.h"
#endif

#include "tal.h"
#include "icbarb.h"
#include "npa.h"
/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define UART_ICB_CLIENT_CNT          1
#define UART_PNOC_MSTRSLV_PAIRS_NUM  1
/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef struct tal_clock_context
{
   /** The DAL device id of the UART device.
    *
    * This id is used to select the correct clocks, and to print any log
    * messages originating in this interface.
    */
   DALDEVICEID dal_id;

   /** Handle to DAL device for clock
    */
   DalDeviceHandle *daldev_clock;

   /** The ID for the core clock.
    *
    * This is the ID for the core clock and is associated with a
    * string provided in the XML file.
    */
   ClockIdType core_clk_id;

   /** The ID for the peripheral clock.
    *
    * This is the ID for the peripheral clock and is associated with 
    * a string provided in the XML file.
    */
   ClockIdType periph_clk_id;

   /** Flag indicating whether this interface is responsible for
    * enabling and disabling the peripheral clock.
    *
    * This flag is stored in the XML configuration under the property
    * "ManagePCLK".
    */
   uint32      manage_pclk;

   /** Flag indicating whether this interface is responsible for resetting the
    * clock block before enabling the clock.
    *
    * This flag is stored in the XML configuration under the property
    * "ResetClock".
    */
   uint32       reset_clock;

   /** The fundamental frequency of the clock providing the bit clock used by
    * the UART core.
    *
    * If this value is non-zero, it indicates that the UART bit clock is
    * driven by a simple integer divider. We can call
    * DalClock_SetClockDivider() to set the divider.
    *
    * If this value is zero, it indicates that the UART bit clock is driven by
    * an M/N:D clock divider, so we can call DalClock_SetClockFrequency() to
    * request nearly any frequency.
    *
    * This value is stored in the XML configuration under the property
    * "ClockFundFreq".
    */
   uint32      fundamental_frequency;
   uint32      pnoc_enable;  
   uint32      pnoc_ib_val;
   uint32      pnoc_ab_val;
   uint32      pnoc_master;
   uint32      pnoc_slave;
   /** NPA client for NOC bandwidth  */
   npa_client_handle  npa_pnoc_client;
  
   /** NPA client for the interrupt latency*/
   npa_client_handle  npa_latency_client;
} TAL_CLOCK_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static DALResult get_property_string(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name, char** property_value,
                                char* default_value);

static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name,uint32 *property_value,
                                uint32 default_value);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: get_property_string

DESCRIPTION:
   Retrieve a string property.

==================================================================================================*/
static DALResult get_property_string(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name, char** property_value,
                                char* default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;
   const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.pszVal;
      defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      defaulted_str = " (defaulted)";
   }

   DALSYS_LogEvent(id, 
                   DALSYS_LOGEVENT_INFO, 
                   "get_property_string: %s = %s%s", 
                   property_name, 
                   ((*property_value == NULL) ? "NULL" : *property_value ),
                   defaulted_str);

   return result;
}
/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name,uint32 *property_value,
                                uint32 default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;
   const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
      defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      defaulted_str = " (defaulted)";
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X%s",
                                             property_name, *property_value, defaulted_str);
}
/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: tal_clock_close

DESCRIPTION:
   Close the clock handle, freeing any resources that were allocated when opening the clock handle.

==================================================================================================*/
void tal_clock_close(TAL_CLOCK_HANDLE handle)
{
   // No close needed for clock
    if(handle->npa_pnoc_client)
   {
      icbarb_complete_request(handle->npa_pnoc_client);	   
      icbarb_destroy_client(handle->npa_pnoc_client);
   }

   if (handle->npa_latency_client)
   {
      npa_destroy_client(handle->npa_latency_client);
   }
   DAL_DeviceDetach(handle->daldev_clock);
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_clock_disable

DESCRIPTION:
   Disable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_disable(TAL_CLOCK_HANDLE handle)
{
   DALResult dal_result;
   TAL_RESULT tal_result = TAL_SUCCESS;

   dal_result = DalClock_DisableClock(handle->daldev_clock, handle->core_clk_id);
   if (dal_result != DAL_SUCCESS) { tal_result = TAL_ERROR; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      dal_result = DalClock_DisableClock(handle->daldev_clock, handle->periph_clk_id);
      if (dal_result != DAL_SUCCESS) { tal_result = TAL_ERROR; }
   }

   npa_complete_request(handle->npa_latency_client);
   return tal_result;
}

/*==================================================================================================

FUNCTION: tal_clock_enable

DESCRIPTION:
   Enable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_enable(TAL_CLOCK_HANDLE handle)
{
   DALResult result;

   if(handle->reset_clock)
   {
      result = DalClock_ResetClock(handle->daldev_clock, handle->core_clk_id, CLOCK_RESET_DEASSERT);
      if (result != DAL_SUCCESS) { goto error; }
   }

   result = DalClock_EnableClock(handle->daldev_clock, handle->core_clk_id);
   if (result != DAL_SUCCESS) { goto error; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      result = DalClock_EnableClock(handle->daldev_clock, handle->periph_clk_id);
      if (result != DAL_SUCCESS) { goto error; }
   }

   // 700 ticks is roughly 35us. Vote for a CPU latency of 35us.
   npa_issue_required_request(handle->npa_latency_client,700);
   return TAL_SUCCESS;
error:
   DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_INFO, "Unable to enable the required clocks");
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_clock_open

DESCRIPTION:
   Open a clock handle for the specified UART device. If successful, this function will return a
   clock handle, and may acquire other resources, which must be released by calling
   tal_clock_close().

==================================================================================================*/
TAL_RESULT tal_clock_open(TAL_CLOCK_HANDLE *phandle, uint32 client_id)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);
   TAL_CLOCK_HANDLE handle = NULL;
   DalDeviceHandle *clock_dal = NULL;
   char *clock_name = NULL;
   ICBArb_MasterSlaveType uart_arb_master_slave;
   ICBArb_RequestType pnoc_request;
   ICBArb_ErrorType   arb_error;
   DALResult result;

   if (DALSYS_Malloc(sizeof(TAL_CLOCK_CONTEXT), (void **)&handle) != DAL_SUCCESS) { goto error; }

   if (DAL_DeviceAttach(DALDEVICEID_CLOCK, &clock_dal) != DAL_SUCCESS) { goto error; }

   handle->daldev_clock = clock_dal;

   // No open needed for clock

   handle->dal_id = client_id;
   handle->manage_pclk = FALSE;

   // Read this clock's Clock Regime-specific parameters from the UART configuration:
   //
   // UartClockName: This is the name of the core clock.  It must be present.
   //
   // ManagePCLK: In recent chips there are two clock inputs to the UART HW block: the
   //             core clock and the peripheral bus clock.  The peripheral bus clock
   //             operates the bus interface (it must be enabled when reading/writing
   //             registers).  On some builds CLKREGIM manages the peripheral bus clock
   //             for us and turns it on/off along with the core clock.  On other builds
   //             we must manage the peripheral bus clock ourselves.  This property is
   //             used to specify whether we must manage it ourselves.
   //
   // PClockName: This is the name of the peripheral clock (if present)
   //
   // ResetClock: Flag indicating whether the clock block must be reset before
   //             it can be used.
   //
   // ClockFundFreq: The fundamental frequency of the bit clock, to which an
   //                integer divider can be applied. If unspecified, the clock
   //                has an M/N:D divider and the clock frequency can be
   //                specified directly.

   result = DALSYS_GetDALPropertyHandle(handle->dal_id, property_handle);
   if (result != DAL_SUCCESS) { goto error; }

   // Read the name of the UART clock
   result = get_property_string(handle->dal_id, property_handle, "UartClockName", &clock_name, NULL);
   if (result != DAL_SUCCESS || clock_name == 0) { goto error; }

   // Look up the clock ID for the core clock
   result = DalClock_GetClockId(handle->daldev_clock, clock_name, &handle->core_clk_id);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "Core GetClockId( %s ) = %d", clock_name, result);
      goto error;
   }

   handle->periph_clk_id = 0;
   
   get_property_dword(handle->dal_id,property_handle,"ManagePCLK", &handle->manage_pclk,FALSE);

   if (handle->manage_pclk)
   {
      // Read the name of the peripheral clock 
      result = get_property_string(handle->dal_id, property_handle, "PClockName", &clock_name, NULL);

      // Look up the clock ID for the peripheral clock
      if (result == DAL_SUCCESS && clock_name)
      {
         result = DalClock_GetClockId(handle->daldev_clock, clock_name, &handle->periph_clk_id);
         if (result != DAL_SUCCESS)
         {
            DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "Periph GetClockId(%s) = %d", clock_name, result);
            goto error;
         }
      }
   }

   get_property_dword(handle->dal_id,property_handle,"ResetClock",     &handle->reset_clock,0);
   get_property_dword(handle->dal_id,property_handle,"ClockFundFreq",  &handle->fundamental_frequency,0);
   get_property_dword(handle->dal_id,property_handle,"PnocVoteEnable", &handle->pnoc_enable, 0);

   if(handle->pnoc_enable)
   {
      get_property_dword(handle->dal_id,property_handle,"PnocArbMaster", &handle->pnoc_master,0);
      get_property_dword(handle->dal_id,property_handle,"PnocArbSlave",  &handle->pnoc_slave, 0);
      if(!(handle->pnoc_master)){ goto error;}

      get_property_dword(handle->dal_id,property_handle,"PnocIBval",     &handle->pnoc_ib_val,0);
      get_property_dword(handle->dal_id,property_handle,"PnocABval",     &handle->pnoc_ab_val,0);
      if(!(handle->pnoc_ib_val && handle->pnoc_ab_val)){ goto error;}

      DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_INFO, "Pnoc vote is enabled:Master %d & SLAVE %d",
                                                     handle->pnoc_master,handle->pnoc_slave);
      
      uart_arb_master_slave.eMaster =  handle->pnoc_master;
      uart_arb_master_slave.eSlave  =  handle->pnoc_slave; 
      
      /* Create a NPA client for the "/node/core/bus/uart/pnocclk" client to make PNOC voting request*/
      handle->npa_pnoc_client = icbarb_create_suppressible_client_ex
                                                           ("/node/core/bus/uart/pnocclk",
                                                            &uart_arb_master_slave,
                                                            UART_ICB_CLIENT_CNT, NULL);
      if (NULL == handle->npa_pnoc_client)
      {
         DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "tal_clock_open: "
                                                 "Unable to create a pnoc npa client");
         goto error;
      }
      
      pnoc_request.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */

      /* Bandwidth in Bytes/second */
      pnoc_request.arbData.type3.uIb = (uint64)  handle->pnoc_ib_val;      
      pnoc_request.arbData.type3.uAb = (uint64)  handle->pnoc_ab_val;    
 
      arb_error = icbarb_issue_request(handle->npa_pnoc_client, &pnoc_request,
                                     UART_PNOC_MSTRSLV_PAIRS_NUM);
   
      if(arb_error != ICBARB_ERROR_SUCCESS)
      {
         DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "tal_clock_enable: "
                         "pnoc request is failed witht the error: %d",arb_error);     
         goto error;
      }
   }
   else
   {
      handle->pnoc_master = 0;
      handle->pnoc_slave  = 0;
      handle->pnoc_ib_val = 0;
      handle->pnoc_ab_val = 0;
      handle->npa_pnoc_client = NULL;
      DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_INFO, "Pnoc vote is not enabled");
   }

   handle->npa_latency_client = NULL;
   handle->npa_latency_client = npa_create_sync_client("/core/cpu/latency",     /* resource name */  
                                        "UART",             /* client name   */  
                                        NPA_CLIENT_REQUIRED /* work model    */
                                       );
   if (handle->npa_latency_client == NULL)
   {
      goto error;
   }

   *phandle = handle;
   return TAL_SUCCESS;

error:
   if (clock_dal) { DAL_DeviceDetach(clock_dal); }
   if (handle)    { DALSYS_Free(handle); }
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_clock_set_frequency

DESCRIPTION:
   Set the clock input frequency to the UART core.

==================================================================================================*/
TAL_RESULT tal_clock_set_frequency(TAL_CLOCK_HANDLE handle, uint32 input_freq)
{
   DALDEVICEID id = handle->dal_id;
   uint32 result_freq;
   DALResult result;
   uint32 diff_freq;

   if(handle->fundamental_frequency)
   {
      if(handle->fundamental_frequency % input_freq)
      {
         /* If the requested frequency does not divide evenly into the clock's
          * fundamental frequency, we can't set the requested frequency using
          * a simple clock divider. */
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "tal_clock_set_frequency: "
             "Invalid frequency %d", input_freq);
         return TAL_ERROR;
      }
      else
      {
         uint32 divider = handle->fundamental_frequency / input_freq;

         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "tal_clock_set_frequency: "
             "Setting clock divider to %d", divider);

         result = DalClock_SetClockDivider(handle->daldev_clock, handle->core_clk_id, divider);
         if(result != DAL_SUCCESS)
         {
            DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "tal_clock_set_frequency: "
                "SetClockDivider(%d) failed", divider);
            return TAL_ERROR;
         }
      }
   }
   else
   {
      // Request for HZ closest. This is added for the 4Mbps, 64MHz frequency issue.
      // On Badger family, there is no support for 64MHz, the closest we get is 63.167MHz.
      // By requesting Hz closest, we can get exact frequencies for all others except 64MHz,

      result = DalClock_SetClockFrequency( handle->daldev_clock, 
                                           handle->core_clk_id, 
                                           input_freq, 
                                           CLOCK_FREQUENCY_HZ_CLOSEST, 
                                           &result_freq);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      if (result_freq != input_freq)
      {
         if(input_freq == 64000000)
         {
            diff_freq = (result_freq > input_freq) ? (result_freq - input_freq) :
                                                     (input_freq - result_freq);
            
            // Check if the deviation is within 1MHz
            if(diff_freq > 1000000)
            {
               goto error;
            }
            else
            {
               DALSYS_LogEvent(id, DALSYS_LOGEVENT_WARNING, 
                               "tal_clock_set_frequency: Exact freq match not found. Nearest freq: %d",
                               result_freq);
            }
         }
         else
         {
            goto error;
         }
      }
   }
   return TAL_SUCCESS;

error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "tal_clock_set_frequency: unsupported freq");
   return TAL_ERROR;
}
