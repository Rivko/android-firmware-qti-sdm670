#ifndef __AFE_DEVICE_DRIVER_INTERFACE_H__
#define __AFE_DEVICE_DRIVER_INTERFACE_H__
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEDeviceDriverInterface.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/09    GY      Port AFE/HAL unified API changes from dev branch
07/13/09    RA      Retract changes for sidetone optimization
07/01/09    AY      Sidetone optimization
06/03/09    AY      Merge the critical section change for ECRefSource and
                    and sidetone.
04/15/09    AY      Add header tag
04/09/09    AY      Sidetone support for both mono and stereo data
11/06/07    RA      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#if defined(__cplusplus)
extern "C" {
#endif

typedef void* afe_drv_h_t;

// afe driver property ids.
enum
{
   AFE_DEVICE_PORT_PROP = 1,
   AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP,
   AFE_DEVICE_RESTART_PROP,
   AFE_DEVICE_GROUP_PROP,
};

typedef void (*afe_dev_cb_func_t) (void *cb_arg);

typedef struct afe_dev_int_cb
{
   afe_dev_cb_func_t afe_dev_int_cb;
   void *cb_arg;                                  ///< callback context.
} afe_dev_int_cb_t;


//it might be changed and move to open command later.
typedef struct dev_start_params
{
   uint32_t            int_samples_per_period; // This can be removed once open command is introduced.
   afe_dev_int_cb_t    cb_info;
}dev_start_params_t;

typedef struct afe_driver_func_tbl_t afe_driver_func_tbl_t;
struct afe_driver_func_tbl_t
{
   /** Processes stream data on port; readwrite    */
   ADSPResult (*process)(afe_drv_h_t afe_drv_h, 
           int16_t* buf_ptr,
           uint32_t num_samples);

   /** Sets a parameter value based on a unique param id.*/
   ADSPResult (*set_param)(afe_drv_h_t afe_drv_h,
              uint32_t param_id,
              int8_t *params_ptr,
              uint32_t payload_size);

   /** Gets a parameter value based on a unique param id. */
   ADSPResult (*get_param)(afe_drv_h_t afe_drv_h,
       uint32_t param_id,
       int8_t *param_ptr,
       uint32_t payload_size, 
       uint16_t *params_buffer_len_req_ptr);

   /** Start data transfer on the device. */
   ADSPResult (*start)(afe_drv_h_t afe_drv_h,
              dev_start_params_t *dev_start_params_ptr);

   /** Stop data transfer on the device. */
   ADSPResult (*stop)(afe_drv_h_t afe_drv_h);    


   /** Sets a list of property values for afe internal use*/
   ADSPResult (*set_properties)(afe_drv_h_t afe_drv_h,
                          uint32_t prop_id,
                          int8_t *prop_ptr,
                          uint32_t payload_size);

   /** Gets a list of property values  for afe internal use */
   ADSPResult (*get_properties)(afe_drv_h_t afe_drv_h,
                                uint32_t prop_id,
                                int8_t *prop_ptr,
                                uint32_t payload_size);

};

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // !defined(__AFEDEVICEDRIVERINTERFACE_H__)
