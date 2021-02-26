/*==================================================================================================

FILE: tal_tlmm.c

DESCRIPTION: This module defines the "stock" implementation of the TLMM APIs for the
             Target Abstraction Layer.  This implementation simply forwards requests
             to the DAL TLMM.

                            Copyright (c) 2012-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/tal/src/tal_tlmm.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_tlmm_close
   tal_tlmm_get_rx_gpio_num
   tal_tlmm_gpio_disable
   tal_tlmm_gpio_enable
   tal_tlmm_open

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDITlmm.h"
#endif

#include "tal.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   TX,
   RX,
   CTS,
   RFR,
   NUM_GPIOS
}GPIOS;

typedef struct 
{
  char                     *gpio_str_name;
  DalTlmm_GpioConfigIdType *config;
  DALGpioIdType             gpio_id;
} GPIO_PROPERTIES;

typedef struct tal_tlmm_context
{
   DALDEVICEID       client_id;
   DalDeviceHandle  *tlmm_dal;
   GPIO_PROPERTIES   gpios[NUM_GPIOS];
} TAL_TLMM_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static boolean get_gpio_config        (TAL_TLMM_CONTEXT *handle);
static void    get_property_string    (DALDEVICEID id, DALSYSPropertyHandle handle,
                                       const char *property_name, char **property_value,
                                       char *default_value);
static void    get_property_struct_ptr(DALDEVICEID id, DALSYSPropertyHandle handle,
                                       const char *property_name, void **property_value,
                                       void *default_value);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: get_gpio_config

DESCRIPTION:
   
==================================================================================================*/
static boolean get_gpio_config(TAL_TLMM_CONTEXT *tlmm_ctxt)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   DALResult        result;
   DALDEVICEID      id = tlmm_ctxt->client_id;
   GPIO_PROPERTIES *gpios = tlmm_ctxt->gpios;
   uint32           i = 0;

   result = DALSYS_GetDALPropertyHandle(id, handle);
   if (result != DAL_SUCCESS) { goto error; }

   get_property_string(id, handle, "GpioTxDataName",  &gpios[ TX].gpio_str_name, NULL);
   get_property_string(id, handle, "GpioRxDataName",  &gpios[ RX].gpio_str_name, NULL);
   get_property_string(id, handle, "GpioCtsDataName", &gpios[CTS].gpio_str_name, NULL);
   get_property_string(id, handle, "GpioRfrDataName", &gpios[RFR].gpio_str_name, NULL);

   get_property_struct_ptr(id, handle, "GpioTxConfig",  (void**)&gpios[ TX].config, NULL);
   get_property_struct_ptr(id, handle, "GpioRxConfig",  (void**)&gpios[ RX].config, NULL);
   get_property_struct_ptr(id, handle, "GpioCtsConfig", (void**)&gpios[CTS].config, NULL);
   get_property_struct_ptr(id, handle, "GpioRfrConfig", (void**)&gpios[RFR].config, NULL);

   for (i = 0; i < NUM_GPIOS; i++)
   {
      if (gpios[i].gpio_str_name && !gpios[i].config)
      {
         DALSYS_LogEvent(id, 
                         DALSYS_LOGEVENT_ERROR, 
                         "get_gpio_config: No configuration available for gpio: %s",
                         gpios[i].gpio_str_name);
         goto error;
      }

      if (gpios[i].gpio_str_name) 
      {
         // DalTlmm_GetGpioId will lock the gpio id which will make any more attempt to acquire 
         // this gpio id to fail. So on close we need to release the id.
         result = DalTlmm_GetGpioId(tlmm_ctxt->tlmm_dal, 
                                    gpios[i].gpio_str_name, 
                                    &gpios[i].gpio_id);
         if (result != DAL_SUCCESS) 
         { 
            goto error; 
         }
      }
      else
      {
         gpios[i].gpio_id = NULL; 
      }
   }

   if (!gpios[TX].gpio_str_name || !gpios[RX].gpio_str_name)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_WARNING, 
                      "get_gpio_config: TX or RX GPIO properties not available.");
   }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "get_gpio_config: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: get_property_string

DESCRIPTION:
   Retrieve a string property.

==================================================================================================*/
static void get_property_string(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name, char **property_value,
                                char *default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.pszVal;
   }
   else
   {
      *property_value = default_value;
   }

   DALSYS_LogEvent(id, 
                   DALSYS_LOGEVENT_INFO, 
                   "get_property_string: %s = %s", 
                   property_name, 
                   ( (*property_value == NULL) ? "NULL" : *property_value ));

}

/*==================================================================================================

FUNCTION: get_property_struct_ptr

DESCRIPTION:
   Retrieve a struct pointer.

==================================================================================================*/
static void get_property_struct_ptr(DALDEVICEID id, DALSYSPropertyHandle handle,
                                    const char *property_name, void **property_value,
                                    void *default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = (void*)property_variable.Val.pStruct;
   }
   else
   { 
      *property_value = default_value;
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_struct_ptr: %s = 0x%08X", 
                   property_name, *property_value);

}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_tlmm_close

DESCRIPTION:

==================================================================================================*/
void tal_tlmm_close(TAL_TLMM_HANDLE handle)
{
   DALResult        result;
   DALDEVICEID      id = handle->client_id;
   GPIO_PROPERTIES *gpio;
   uint32           i;

   // Release the GPIO Ids. 
   for (i = 0; i < NUM_GPIOS; i++)
   {
      gpio = &handle->gpios[i];

      if (gpio->gpio_id) 
      {
         result = DalTlmm_ReleaseGpioId(handle->tlmm_dal, gpio->gpio_id);
         if (result != DAL_SUCCESS)
         {
            DALSYS_LogEvent(id, 
                            DALSYS_LOGEVENT_ERROR, 
                            "tal_tlmm_close: DalTlmm_ReleaseGpioId failed for gpio: %s",
                            gpio->gpio_str_name);
         }
      }
   }

   DalDevice_Close(handle->tlmm_dal);
   DAL_DeviceDetach(handle->tlmm_dal);
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_tlmm_get_rx_gpio_num

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_get_rx_gpio_num(TAL_TLMM_HANDLE handle, uint32 *gpio_num)
{
   DalTlmm_GpioIdSettingsType gpio_settings;
   DALResult                  result;

   if (!handle->gpios[RX].gpio_id)
   {
      return TAL_ERROR;
   }

   result = DalTlmm_GetGpioIdSettings(handle->tlmm_dal, 
                                       handle->gpios[RX].gpio_id, 
                                       &gpio_settings); 

   if (result == DAL_SUCCESS)
   {
      *gpio_num = gpio_settings.nGpioNumber;
      return TAL_SUCCESS;
   }

   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_tlmm_gpio_disable

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_gpio_disable(TAL_TLMM_HANDLE handle)
{
   DALResult result = DAL_SUCCESS;
   uint32 i;

   for (i = 0; i < NUM_GPIOS; i++) 
   {
      if (handle->gpios[i].gpio_id) 
      {
         result = DalTlmm_ConfigGpioIdInactive(handle->tlmm_dal, handle->gpios[i].gpio_id);
           
         if (result != DAL_SUCCESS)
         {
            return TAL_ERROR;
         }
      }
   }
   return TAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: tal_tlmm_gpio_enable

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_gpio_enable(TAL_TLMM_HANDLE handle)
{
   DALResult result = DAL_SUCCESS;
   uint32 i;

   for (i = 0; i < NUM_GPIOS; i++)
   {
      if (handle->gpios[i].gpio_id) 
      {
         result = DalTlmm_ConfigGpioId(handle->tlmm_dal, 
                                       handle->gpios[i].gpio_id, 
                                       handle->gpios[i].config);
         if (result != DAL_SUCCESS)
         {
            return TAL_ERROR;
         }
      }
   }
   return TAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: tal_tlmm_open

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_open(TAL_TLMM_HANDLE *phandle, uint32 client_id)
{
   TAL_TLMM_CONTEXT *tlmm_ctxt = NULL;
   DalDeviceHandle  *tlmm_dal = NULL;
   DALResult         result;

   result = DALSYS_Malloc(sizeof(TAL_TLMM_CONTEXT), (void **)&tlmm_ctxt);
   if (result != DAL_SUCCESS) { goto error; }

   result = DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_dal);
   if (result != DAL_SUCCESS) { goto error; }

   result = DalDevice_Open(tlmm_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS) { goto error; }

   tlmm_ctxt->client_id = client_id;
   tlmm_ctxt->tlmm_dal = tlmm_dal;

   if(!get_gpio_config(tlmm_ctxt)) { goto error;}

   *phandle = tlmm_ctxt;
   return TAL_SUCCESS;
error:
   if (tlmm_ctxt) { DALSYS_Free(tlmm_ctxt); }
   if (tlmm_dal)  { DAL_DeviceDetach(tlmm_dal); }
   *phandle = NULL;
   return TAL_ERROR;
}
