#ifndef CAPI_V2_H
#define CAPI_V2_H

/* ========================================================================*/
/**
 @file capi_v2.h
 Common Audio Processing Interface v2 header file

 This file defines a generalized C interface that can wrap a wide
 variety of audio processing modules, so that they can be treated the
 same way by control code.
 */
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_CAPIv2_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*======================================================================
 Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 Export of this technology or software is regulated by the U.S. Government,
 Diversion contrary to U.S. law prohibited.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 05/14/15    sw     (Tech Pubs) Merged Doxygen markup/comments from 8994 (2.6).
 07/15/13   dg      Created file.
 ========================================================================= */

#include "capi_v2_types.h"
#include "capi_v2_events.h"
#include "capi_v2_properties.h"

typedef struct capi_v2_t capi_v2_t;

typedef struct capi_v2_vtbl_t capi_v2_vtbl_t;

/** @ingroup capiv2_virt_func_table
    Function table for plain C implementations of CAPIv2-compliant objects.

    Objects must have a pointer to a function table as the first element in
    their instance structure. This structure is the function table type for all
    such objects.
*/
struct capi_v2_vtbl_t
{
    /**
    Processes input data on all input ports and provides output on all output
    ports.

    @datatypes
    capi_v2_t \n
    capi_v2_stream_data_t

    @param[in,out] _pif   Pointer to the module object.
    @param[in,out] input  Array of pointers to the input data for each input
                          port. \n
                          The length of the array is the number of input ports.
                          The client sets the number of input ports using the
                          #CAPI_V2_PORT_NUM_INFO property. \n
                          The function must modify the actual_data_len field
                          to indicate how many bytes were consumed.
    @param[out] output    Array of pointers to the output data for each output
                          port. \n
                          The client sets the number of output ports using the
                          #CAPI_V2_PORT_NUM_INFO property. \n
                          The function sets the actual_data_len field to
                          indicate how many bytes were generated.

    @detdesc
    This is a generic processing function.
    @par
    On each call to capi_v2_vtbl_t::process(), the behavior of the module
    depends on the value it returned for the #CAPI_V2_REQUIRES_DATA_BUFFERING
    property. For a description of the behavior, refer to the comments for
    CAPI_V2_REQUIRES_DATA_BUFFERING.
    @par
    No debug messages are allowed in this function.

    @return
    Indication of success or failure.

    @dependencies
    A valid input media type must have been set on each input port using the
    #CAPI_V2_INPUT_MEDIA_FORMAT property. @newpage
    */
    capi_v2_err_t (*process)(capi_v2_t* _pif,
           capi_v2_stream_data_t* input[],
           capi_v2_stream_data_t* output[]);

   /**
    Frees any memory allocated by the module.

    @datatypes
    capi_v2_t

    @param[in,out] _pif    Pointer to the module object.

    @note1hang After calling this function, _pif is no longer a valid CAPIv2
               object. Do not call any CAPIv2 functions after using it.

    @return
    Indication of success or failure.

    @dependencies
    None. @newpage
    */
   capi_v2_err_t (*end)(capi_v2_t* _pif);

    /**
    Sets a parameter value based on a unique parameter ID.

    @datatypes
    capi_v2_t \n
    capi_v2_port_info_t \n
    capi_v2_buf_t

    @param[in,out] _pif      Pointer to the module object.
    @param[in] param_id      ID of the parameter whose value is to be set.
    @param[in] port_info_ptr Pointer to the information about the port on
                             which this function must operate. \n
                             If a valid port ID is not provided, either the
                             port index does not matter for the param_id or
                             the param_id is applicable to all ports.
    @param[in] params_ptr    Pointer to the buffer containing the value of the
                             parameter.
                             The format of the data in the buffer depends on
                             the implementation.

    @detdesc
    In the event of a failure, the appropriate error code is to be returned.
    @par
    The actual_data_len field of the parameter pointer is to be at least the
    size of the parameter structure. Therefore, the following check must be
    exercised for each tuning parameter ID:
    @par
    @code
    if (params_ptr->actual_data_len >= sizeof(asm_gain_struct_t))
    {
    :
    :
    }
    else
    {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"CAPI_V2 Libname Set, Bad param size
    %lu",params_ptr->actual_data_len);
    return ADSP_ENEEDMORE;
    }

    @endcode
    @par
    Optionally, some parameter values can be printed for tuning verification.
    @par
    @note1 In this code sample, asm_gain_struct is an example only.
           Use the correct structure based on the parameter ID.

    @return
    None.

    @dependencies
    None. @newpage
    */
    capi_v2_err_t (*set_param)(capi_v2_t* _pif,
              uint32_t param_id,
              const capi_v2_port_info_t *port_info_ptr,
              capi_v2_buf_t *params_ptr);

    /**
    Gets a parameter value based on a unique parameter ID.

    @datatypes
    capi_v2_t \n
    capi_v2_port_info_t \n
    capi_v2_buf_t

    @param[in,out] _pif      Pointer to the module object.
    @param[in]     param_id  Parameter ID of the parameter whose value is
                             being passed in this function. For example:\n
                             - CAPI_V2_LIBNAME_ENABLE
                             - CAPI_V2_LIBNAME_FILTER_COEFF @tablebulletend
    @param[in] port_info_ptr Pointer to the information about the port on which
                             this function must operate. \n
                             If the port is invalid, either the port index does
                             not matter for the param_id or or the param_id
                             is applicable to all ports.
    @param[out] params_ptr   Pointer to the buffer that is to be filled with
                             the value of the parameter. The format depends on
                             the implementation.

    @detdesc
    In the event of a failure, the appropriate error code is to be returned.
    @par
    The max_data_len field of the parameter pointer must be at least the size
    of the parameter structure. Therefore, the following check must be
    exercised for each tuning parameter ID.
    @par
    @code
    if (params_ptr->max_data_len >= sizeof(asm_gain_struct_t))
    {
    :
    :
    }
    else
    {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"CAPI_V2 Libname Get, Bad param size
    %lu",params_ptr->max_data_len);
    return ADSP_ENEEDMORE;
    }

    @endcode
    @par
    Before returning, the actual_data_len field must be filled with the number
    of bytes written into the buffer.
    @par
    Optionally, some parameter values can be printed for tuning verification.
    @par
    @note1 In this code sample, asm_gain_struct is an example only.
           Use the correct structure based on the parameter ID.

    @newpage
    @return
    Indication of success or failure.

    @dependencies
    None. @newpage
    */
   capi_v2_err_t (*get_param)(capi_v2_t* _pif,
         uint32_t param_id,
         const capi_v2_port_info_t *port_info_ptr,
         capi_v2_buf_t *params_ptr);

    /**
    Sets a list of property values.

    @datatypes
    capi_v2_t \n
    capi_v2_proplist_t

    @param[in,out] _pif      Pointer to the module object.
    @param[in] proplist_ptr  Pointer to the list of property values.

    @detdesc
    In the event of a failure, the appropriate error code is to be returned.
    @par
    Optionally, some property values can be printed for debugging.

    @return
    None.

    @dependencies
    None. @newpage
    */
   capi_v2_err_t (*set_properties)(capi_v2_t* _pif,
                          capi_v2_proplist_t *proplist_ptr);

    /**
    Gets a list of property values.

    @datatypes
    capi_v2_t \n
    capi_v2_proplist_t

    @param[in,out] _pif       Pointer to the module object.
    @param[out] proplist_ptr  Pointer to the list of empty structures that
                              must be filled with the appropriate property
                              values, which are based on the property IDs
                              provided. \n \n
                              The client must fill some elements
                              of the structures as input to the module. These
                              elements must be explicitly indicated in the
                              structure definition.

    @detdesc
    In the event of a failure, the appropriate error code is to be returned.

    @return
    Indication of success or failure.

    @dependencies
    None.
    */
   capi_v2_err_t (*get_properties)(capi_v2_t* _pif,
                  capi_v2_proplist_t *proplist_ptr);
};

/** @ingroup capiv2_func_wrapper
  Plain C interface wrapper for the virtual function table, capi_v2_vtbl_t.

  This capi_v2_t structure appears to the caller as a virtual function table.
  The virtual function table in the instance structure is followed by other
  structure elements, but those are invisible to the users of the CAPI_V2
  object. This capi_v2_t structure is all that is publicly visible.
 */
struct capi_v2_t
{
   const capi_v2_vtbl_t *vtbl_ptr; /**< Pointer to the virtual function table. */
};

/** @ingroup capiv2_func_get_static_prop
  Queries for properties as follows:
  - Static properties of the module that are independent of the instance
  - Any property that is part of the set of properties that can be statically
    queried

  @datatypes
  capi_v2_proplist_t

  @param[in] init_set_proplist Pointer to the same properties that are sent in
                               the call to capi_v2_init_f().
  @param[out] static_proplist  Pointer to the property list structure. \n
                               The client fills in the property IDs for which
                               it needs property values. The client also
                               allocates the memory for the payloads.
                               The module must fill in the information in this
                               memory.

  @detdesc
  This function is used to query the memory requirements of the module to
  create an instance. The function must fill in the data for the properties in
  the static_proplist.
  @par
  As an input to this function, the client must pass in the property list that
  it passes to capi_v2_init_f(). The module can use the property
  values in init_set_proplist to calculate its memory requirements.
  @par
  The same properties that are sent to the module in the call to
  %capi_v2_init_f() are also sent to this function to enable the module to
  calculate the memory requirement.

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
typedef capi_v2_err_t (*capi_v2_get_static_properties_f)(
                capi_v2_proplist_t *init_set_proplist,
                capi_v2_proplist_t *static_proplist);

/** @ingroup capiv2_func_init
  Instantiates the module to set up the virtual function table, and also
  allocates any memory required by the module.

  @datatypes
  capi_v2_t \n
  capi_v2_proplist_t

  @param[in,out] _pif          Pointer to the module object. \n
                               The memory has been allocated by the client
                               based on the size returned in the
                               #CAPI_V2_INIT_MEMORY_REQUIREMENT property.
  @param[in] init_set_proplist Pointer to the properties set by the service
                               to be used while initializing.

  @detdesc
  States within the module must be initialized at the same time.
  @par
  All return codes returned by this function, except #CAPI_V2_EOK, are
  considered to be FATAL.
  @par
  For any unsupported property ID passed in the init_set_proplist parameter,
  the function prints a message and continues processing other property IDs.

  @return
  Indication of success or failure.

  @dependencies
  */
typedef capi_v2_err_t (*capi_v2_init_f)(
                capi_v2_t* _pif, 
                capi_v2_proplist_t *init_set_proplist);

#endif /* #ifndef CAPI_V2_H */
