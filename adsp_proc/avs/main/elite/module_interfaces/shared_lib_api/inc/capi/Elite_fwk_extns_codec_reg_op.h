#ifndef ELITE_FWK_EXTNS_CODEC_REG_OP_H
#define ELITE_FWK_EXTNS_CODEC_REG_OP_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_codec_reg_op.h

@brief frame work extensions for codec register operation header file

  This file defines a framework extension and corresponding private properties
  needed for codec register operation needed by modules.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/* =========================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
  ========================================================================== */
/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   08/28/17    sw     (Tech Pubs) Edited Doxygen comments for AVS.2.8 (SDM845).
   08/09/17   udayB      Initial Version.
   ========================================================================= */

/** @weakgroup weakf_capiv2_ext_cdc_reg_ops_intro

The #FWK_EXTN_CDC_REG_OP framework extension enables support for codec
register operations in CAPIv2 modules. This framework provides a utility
to read/write/update the codec registers as requested by the module.

The Elite framework uses #CAPI_V2_PARAM_ID_CDC_REG_OP_VTBL to send information
about the codec register operation virtual function table to a module. The
module must store this virtual table and use the functions to perform codec
register operations.

The module must register with the codec by calling the registration function,
cdc_reg_access_registration(). If registration is successful, the Elite
framework returns a non-NULL handle. The module must use the handle for
further register operations.

Based on the function called by the module, the Elite framework reads, writes,
and updates the codec registers. The module must know information related to
the codec registers that it wants to operate, such as the register address,
register bit width, register field bit mask, write value, and so on.

The module must de-register from the codec by calling the de-registration
function, cdc_reg_access_deregistration().

This framework works only for the Qualcomm Wireless Codec Decoder (WCD).
*/

#include "Elite_CAPI_V2_types.h"


/** @addtogroup capiv2_ext_cdc_reg_op
@{ */

/** Unique identifier of a module that supports the codec register operation
    framework extension.

    This extension supports the #CAPI_V2_PARAM_ID_CDC_REG_OP_VTBL parameter.
*/
#define FWK_EXTN_CDC_REG_OP                       0x000102AE

/* Data type for codec device handle*/
typedef void* capi_v2_cdc_handle;

/* Minor version of codec register virtual table*/
#define CDC_REG_OP_VTBL_VERSION                   0x1

/** ID of the parameter used by the Elite framework to send the
    virtual function table address to a module.

    To request codec register operations, the module must call function
    pointers that are provided in the virtual table (capi_v2_cdc_reg_op_vtbl_t).

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t *vtbl_ptr;
capi_v2_buf_t client_buf;
client_buf.data_ptr = (int8_t*)vtbl_ptr;
client_buf.actual_data_len = sizeof(capi_v2_cdc_reg_op_vtbl_t*);
client_buf.max_data_len = sizeof(capi_v2_cdc_reg_op_vtbl_t*);
capi_ptr->vtbl_ptr->set_param(so.capi_ptr, CAPI_V2_PARAM_ID_CDC_REG_OP_VTBL, &port_info, &client_buf);
@endcode @vertspace{9}

  @par
   Callee:
@code
// Read data_ptr and max_data_len
// Read and Cache the vtbl_ptr for future use
@endcode @newpage
 */
#define CAPI_V2_PARAM_ID_CDC_REG_OP_VTBL          0x000102AF

/** Virtual function table that contains function pointers to codec register
    operations.
 */
typedef struct capi_v2_cdc_reg_op_vtbl_t
{
  /**
   Tracks the configuration of the virtual table.

   When a virtual table is required, the Elite framework introduces the new
   table with an updated minor version. (the new table is an updated version of
   the old table with new variables added at the end).

   A module can detect the minor version and store the table accordingly.

   @values 1 @newpage
  */
  uint32_t        cdc_reg_op_vtbl_minor_version;

  /**
   Registers a module with the codec device and returns a valid handle.

   During registration, the Elite framework opens the codec device and returns
   the device handle.

   @return
   On success, returns a non-NULL handle. \n
   On failure, returns a NULL handle.

   @dependencies
   For any codec register operation, the module must have registered with the
   codec device.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t   *vtbl_ptr;
capi_v2_cdc_handle          cdc_dev_handle = NULL;

cdc_dev_handle  = capi_ptr->vtbl_ptr->cdc_reg_access_registration(void);
@endcode @vertspace{9}

   @par
   Callee:
@code
capi_v2_cdc_handle cdc_dev_handle;

//Register with codec device and return valid handle

return (cdc_dev_handle);

@endcode @newpage
  */
  capi_v2_cdc_handle (*cdc_reg_access_registration)(void);

  /**
   De-registers a module from the codec device.

   @param[in] cdc_handle  Valid codec device handle that is returned during
                          registration.

   @detdesc
   To avoid latency, do not de-register and register the module for every
   operation. De-register the module only if there are no more register
   operations.
   @par
   If the module is registered with the codec device, de-registration is
   mandatory.

   @return
   Indication of success or failure.

   @dependencies
   The module must have been previously registered with the codec device.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t  *vtbl_ptr;
capi_v2_cdc_handle         cdc_handle;

capi_ptr->vtbl_ptr->cdc_reg_access_deregistration(cdc_handle);

cdc_handle = NULL;
@endcode @vertspace{9}

   @par
   Callee:
@code
//De-register from codec device

return (capiv2 error code);

@endcode @newpage
  */
  capi_v2_err_t (*cdc_reg_access_deregistration)(capi_v2_cdc_handle cdc_handle);

  /**
   Reads a single codec register.

   @param[in] cdc_dev_handle  Valid codec device handle that is returned during
                              registration.
   @param[in] reg_addr        Logical address of the codec register.
   @param[in] read_value_ptr  Pointer to the value read from the codec register.
   @param[in] reg_bit_width   Width of the register to read from. \n
                              @values 8, 16, and 32 bits

   @detdesc
   The Elite framework reads the entire register and stores that value in
   read_value_ptr.
   @par
   To get the actual value, the module must apply masking and shifting
   operations to the read value based on the register_field_bit_mask.
   @par
   To avoid corruption, the module must allocate enough memory for read_value_ptr
   to hold the entire read value based on the register bit width.

   @return
   Indication of success or failure.

   @dependencies
   None.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t   *vtbl_ptr;
capi_v2_cdc_handle          cdc_dev_handle;

uint32_t reg_addr           = 0x665;
uint32_t reg_bit_width      = 8;
uint8_t  final_read_value   = 0;

uint32_t  read_value      = 0;
uint32_t  *read_value_ptr = &read_value;

capi_ptr->vtbl_ptr->cdc_register_read(cdc_dev_handle, reg_addr, (uint8_t *)read_value_ptr,reg_bit_width);

uint32_t reg_field_bit_mask = 0xFF;

//Calculate the amount by which the value should be shifted.
uint32_t reg_field_shift = (count trailing 0's in reg_field_bit_mask);

// Clear out the bits as per bit_mask and do proper shift
read_value &= reg_field_bit_mask;
read_value = read_value >> reg_field_shift;

// Get actual value after appropriate typecasting.
final_read_value = (uint8_t) register_value;
@endcode @vertspace{9}

   @par
   Callee:
@code
//Elite framework reads whole 8-bits of 0x665 codec register and store that value in read_value_ptr
return (capi v2 error code);

@endcode @newpage
  */
  capi_v2_err_t (*cdc_register_read)(capi_v2_cdc_handle cdc_dev_handle, uint32_t reg_addr,\
                                     uint8_t* read_value_ptr, uint32_t reg_bit_width);

  /**
   Reads multiple contiguous registers.

   @param[in] cdc_dev_handle      Valid codec device handle that is returned
                                  during registration.
   @param[in] reg_addr            Logical address of the first codec register.
   @param[in] read_value_ptr      Pointer to the value read from the codec
                                  register.
   @param[in] reg_bit_width       Width of the register to read from. \n
                                  @values 8, 16, and 32 bits
   @param[in] no_of_regs_to_read  Number of contiguous registers to read.

   @detdesc
   The Elite framework reads contiguous registers and stores the value of those
   registers in read_value_ptr. The read_value_ptr parameter must be a pointer
   to an array of read values to be stored from the registers.
   @par
   To get the actual value, the module must apply masking and shifting
   operations to each read value.
   @par
   To avoid corruption, the module must allocate enough memory for read_value_ptr
   to hold the all read values.
   @par
   For non-contiguous register reads, the module must use cdc_register_read
   sequentially.

   @return
   Indication of success or failure.

   @dependencies
   None.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t   *vtbl_ptr;
capi_v2_cdc_handle          cdc_dev_handle;

// Example code to read two contiguous registers 0x665 and 0x666

uint32_t reg_addr             = 0x665;
uint32_t no_of_regs_to_read   = 2;
uint32_t reg_bit_width        = 8;
uint8_t  read_value[2]        = {0}; // To avoid corruption, module should
                                     // declare data type of read_value
                                     // based on reg_bit_width.

uint8_t  *read_value_ptr = &read_value;

capi_ptr->vtbl_ptr->cdc_register_multi_read(cdc_dev_handle, reg_addr,\
                (uint8_t *)read_value_ptr, reg_bit_width, no_of_regs_to_read);

//To get final read value for 0x665
uint32_t reg_field_bit_mask = 0xFF; // bit mask for 0x665

//Calculate the amount by which the value should be shifted.
uint32_t reg_field_shift = (count trailing 0's in reg_field_bit_mask);

// Typecast appropriately as per register bit width.
uint8_t *reg_read8 = (uint8_t *)read_value_ptr;

// Clear out the bits as per bit_mask and do proper shift
reg_read8[0] &= reg_field_bit_mask;
reg_read8[0] = reg_read8[0] >> reg_field_shift;


//To get final read value for 0x666
uint32_t reg_field_bit_mask = 0x03; // bit mask for 0x666

//Calculate the amount by which the value should be shifted.
uint32_t reg_field_shift = (count trailing 0's in reg_field_bit_mask);

// Clear out the bits as per bit_mask and do proper shift
uint8_t *reg_read8 = (uint8_t *)read_value_ptr;
reg_read8[1] &= reg_field_bit_mask;
reg_read8[1] = reg_read8[1] >> reg_field_shift;
@endcode @vertspace{9}

   @par
   Callee:
@code
//Elite framework stores 16 bits of value read from 0x665 (8bits) and
//0x666 (8bits) into read_value_ptr.
//To get final read value, caller must do masking/shifting/typecasting
//as per registers bit mask and bit width.

return (capi v2 error code);

@endcode @newpage
  */
  capi_v2_err_t (*cdc_register_multi_read)(capi_v2_cdc_handle cdc_dev_handle, uint32_t reg_addr, uint8_t* read_value_ptr,\
                                           uint32_t reg_bit_width, uint32_t no_of_regs_to_read);

  /**
   Writes a single codec register.

   @param[in] cdc_dev_handle      Valid codec device handle that is returned
                                  during registration.
   @param[in] reg_addr            Logical address of the codec register.
   @param[in] write_value_ptr     Pointer to the value to be written to the
                                  vodec register.
   @param[in] reg_field_bit_mask  Valid bit field mask.
   @param[in] reg_bit_width       Width of the register to write to. \n
                                  @values 8, 16, and 32 bits
   @param[in] is_blocking         Indicates whether a write operation is to be
                                  blocking. \n
                                  @values
                                  - TRUE - Blocking
                                  - FALSE -- Not blocking @tablebulletend

   @detdesc
   This function overwrites the contents of the register with the written value,
   based on reg_field_bit_mask.
   @par
   If is_blocking = TRUE, execution is returned only after the write request is
   completed. If the module must write multiple registers sequentially,
   reduce latency by setting set is_blocking = TRUE only for the last register write.

   @return
   Indication of success or failure.

   @dependencies
   None.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t   *vtbl_ptr;
capi_v2_cdc_handle          cdc_dev_handle;

uint32_t reg_addr             = 0x1F;
uint32_t reg_bit_width        = 8;
uint32_t reg_field_bit_mask   = 0x3F;
uint8_t  write_value          = 0x12;
uint8_t  *write_value_ptr = &write_value;
bool_t is_blocking = FALSE;

capi_ptr->vtbl_ptr->cdc_register_write(cdc_dev_handle, reg_addr, (uint8_t *)\
             write_value_ptr, reg_field_bit_mask, reg_bit_width, is_blocking);
@endcode @newpage

   @par
   Callee:
@code
uint32_t  write_value = 0;

//Typecast based on reg bit width
write_value = *(uint_8_t *)write_value_ptr;

//align the value to be written to the correct position
write_value = (write_value << reg_field_shift) & reg_field_bit_mask;

//And overwrite the register with this write_value.

return (capiv2 error code);

@endcode @newpage
  */
  capi_v2_err_t (*cdc_register_write)(capi_v2_cdc_handle cdc_dev_handle, uint32_t reg_addr, uint8_t* write_value_ptr,\
                                      uint32_t reg_field_bit_mask, uint32_t reg_bit_width,\
                                      bool_t is_blocking);

  /**
   Updates a single codec register.

   @param[in] cdc_dev_handle      Valid codec device handle that is returned
                                  during registration.
   @param[in] reg_addr            Logical address of the codec register.
   @param[in] write_value_ptr     Pointer to the value to be written to the
                                  codec register.
   @param[in] reg_field_bit_mask  Valid bit field mask.
   @param[in] reg_bit_width       Width of the register to write to. \n
                                  @values 8, 16, and 32 bits
   @param[in] is_blocking         Indicates whether an update is to be blocking. \n
                                  @values
                                  - TRUE - Blocking
                                  - FALSE -- Not blocking @tablebulletend

   @detdesc
   This function reads an existing value in the register, clears the bits
   indicated in reg_field_bit_mask, and ORs this value with the value to be
   written. Content of the bits not related to reg_field_bit_mask are retained.
   @par
   If is_blocking = TRUE, execution is returned only after the codec register
   update request is completed. If the module must write multiple registers
   sequentially, reduce latency by setting set is_blocking = TRUE only for the
   last register update.

   @return
   Indication of success or failure.

   @dependencies
   None.

   @par Example
   Caller:
@code
capi_v2_cdc_reg_op_vtbl_t   *vtbl_ptr;
capi_v2_cdc_handle          cdc_dev_handle;

uint32_t reg_addr             = 0x1D;//0x1D is 8-bit register, module should know this information.
uint32_t reg_bit_width        = 8;
uint32_t reg_field_bit_mask   = 0x30;
uint8_t  write_value          = 0x2;
uint8_t  *write_value_ptr = &write_value;
bool_t is_blocking = FALSE;

capi_ptr->vtbl_ptr->cdc_register_update(cdc_dev_handle, reg_addr, (uint8_t *)\
             write_value_ptr, reg_field_bit_mask, reg_bit_width, is_blocking);
@endcode @newpage

   @par
   Callee:
@code
uint32_t write_value = 0, register_read_value = 0;

//Reads existing value in register and store in register_read_value.

// Clear out only the bits indicated in bit_mask
register_value &= ~reg_field_bit_mask;

//Typecast based on reg bit width
write_value = *(uint_8_t *)write_value_ptr;

//align the value to be written to the correct position
write_value = (write_value << reg_field_shift) & reg_field_bit_mask;

// OR the value to be written with the original register contents
write_value |= register_value;

//align the value to be written to the correct position
write_value = (write_value << reg_field_shift) & reg_field_bit_mask;

//And update the register with this write_value.
return (capiv2 error code);

@endcode @newpage
  */
  capi_v2_err_t (*cdc_register_update)(capi_v2_cdc_handle cdc_dev_handle, uint32_t reg_addr, uint8_t* write_value_ptr,\
                                       uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                       bool_t is_blocking);
}/** @cond */capi_v2_cdc_reg_op_vtbl_t/** @endcond */;

/** @} *//* end_addtogroup capiv2_ext_cdc_reg_op */

#endif /* #ifndef ELITE_FWK_EXTNS_CODEC_REG_OP_H */
