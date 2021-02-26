#ifndef ELITE_CAPI_V2_H
#define ELITE_CAPI_V2_H

/* ========================================================================*/
/**
 @file Elite_CAPI_V2.h
 Common Audio Processing Interface v2 header file

 This file defines a generalized C interface that can wrap a wide
 variety of audio processing modules, so that they can be treated the
 same way by control code.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 07/15/13   dg      Created file.
 ========================================================================= */

#include "Elite_CAPI_V2_types.h"
#include "Elite_CAPI_V2_events.h"
#include "Elite_CAPI_V2_properties.h"

typedef struct capi_v2_t capi_v2_t;

typedef struct capi_v2_vtbl_t capi_v2_vtbl_t;
struct capi_v2_vtbl_t
{
   /** @ingroup
    Processes input data on all input ports and provides output on all output ports.

    @datatypes
    capi_v2_t \n
    capi_v2_stream_data_t

    @param[in,out] _pif       Pointer to the module object.
    @param[in,out] input      Array of pointers to the input data for each
    input port. The length of the array is the number
    of input ports. The number of input ports are set by the client using the
    CAPI_V2_PORT_NUM_INFO property. The function must modify the actual_data_len
    field to indicate how many bytes were consumed.
    @param[out]    output     Array of pointers to the output data for each
    output port. The number of input ports are set by the client using the
    CAPI_V2_PORT_NUM_INFO property. The function sets the actual_data_len
    field to indicate how many bytes were generated.

    @detdesc
    This is a generic processing function.
    @par
    On each call to the %process() function, the behavior of the module depends
    on the value it returned for the CAPI_V2_REQUIRES_DATA_BUFFERING property. Please
    refer to the comments for the CAPI_V2_REQUIRES_DATA_BUFFERING property for a
    description of the behavior.
    @par
    No debug messages are allowed in this function.
    @par

    @return
    Indication of success or failure.

    @dependencies
    A valid input media type must have been set on each input port using the
    CAPI_V2_INPUT_MEDIA_FORMAT property.
    */
    capi_v2_err_t (*process)(capi_v2_t* _pif,
           capi_v2_stream_data_t* input[],
           capi_v2_stream_data_t* output[]);

   /** @ingroup
    Frees up any memory allocated by the module.

    @datatypes
    capi_v2_t

    @param[in,out] _pif    Pointer to the module object.

    @note1hang After this function is called, _pif is no longer a valid
    CAPIv2 object. No further CAPIv2 functions should be called using it.

    @return
    Indication of success or failure.

    @dependencies
    None.
    */
   capi_v2_err_t (*end)(capi_v2_t* _pif);

   /** @ingroup
    Sets a parameter value based on a unique param id.

    @datatypes
    capi_v2_t \n
    capi_v2_buf_t

    @param[in,out] _pif        Pointer to the module object.
    @param[in]     param_id    Parameter ID of the parameter whose value is to be set.
    @param[in] port_info_ptr        information about the port on which this function must operate.
          If a valid port id is not provided, then either the port index doesn't matter
          for the param_id or the param_id is applicable to all ports.
    @param[in]     params_ptr  Buffer containing the value of the parameter.
    The format depends on the implementation.

    @detdesc
    In the event of a failure, the appropriate error code is to be
    returned.
    The actual_data_len field of the parameter pointer is to be at least the size
    of the parameter structure. Therefore, the following check must be exercised
    for each tuning parameter ID.
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

    @return
    None.

    @dependencies
    None.
    */
    capi_v2_err_t (*set_param)(capi_v2_t* _pif,
              uint32_t param_id,
              const capi_v2_port_info_t *port_info_ptr,
              capi_v2_buf_t *params_ptr);

   /** @ingroup capi_v2_virt_func_get_params
    Gets a parameter value based on a unique param id.

    @datatypes
    capi_v2_t \n
    capi_v2_buf_t

    @param[in,out] _pif        Pointer to the module object.
    @param[in]     param_id    Parameter ID of the parameter whose value is
    being passed in this function. For example:\n
    - CAPI_V2_LIBNAME_ENABLE
    - CAPI_V2_LIBNAME_FILTER_COEFF @tablebulletend
    @param[in] port_info_ptr        information about the port on which this function must operate.
          If port is invalid, then either the port index doesn't matter for the param_id or or the param_id is applicable to all ports.
    @param[out]    params_ptr  Buffer containing the value of the parameter.
    The format depends on the implementation.

    @detdesc
    In the event of a failure, the appropriate error code is to be
    returned.
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
    @endcode
    @newpage
    @code
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

    @return
    Indication of success or failure.

    @dependencies
    None.
    */
   capi_v2_err_t (*get_param)(capi_v2_t* _pif,
         uint32_t param_id,
         const capi_v2_port_info_t *port_info_ptr,
         capi_v2_buf_t *params_ptr);

   /** @ingroup
    Sets a list of property values.

    @datatypes
    capi_v2_t \n
    capi_v2_proplist_t

    @param[in,out] _pif        Pointer to the module object.
    @param[in]     proplist_ptr   Contains the property values to be set.

    @detdesc
    In the event of a failure, the appropriate error code is to be
    returned.
    @par
    Optionally, some property values can be printed for debugging.

    @return
    None.

    @dependencies
    None.
    */
   capi_v2_err_t (*set_properties)(capi_v2_t* _pif,
                          capi_v2_proplist_t *proplist_ptr);

   /** @ingroup
    Gets a list of property values.

    @datatypes
    capi_v2_t \n
    capi_v2_proplist_t

    @param[in,out] _pif        Pointer to the module object.
    @param[out]    proplist_ptr   Contains the empty structures that must be
    filled with the appropriate property values
    based on the property ids provided.

    @detdesc
    In the event of a failure, the appropriate error code is to be
    returned.

    @return
    Indication of success or failure.

    @dependencies
    None.
    */
   capi_v2_err_t (*get_properties)(capi_v2_t* _pif,
                  capi_v2_proplist_t *proplist_ptr);
};

/** @ingroup data_types
 Plain C interface wrapper for the virtual function table, capi_v2_vtbl_t.

 This capi_v2_t structure appears to the caller as a virtual function table.
 The virtual function table in the instance structure is followed by other
 structure elements, but those are invisible to the users of the CAPI_V2 object.
 This capi_v2_t structure is all that is publicly visible.
 */
struct capi_v2_t
{
   const capi_v2_vtbl_t *vtbl_ptr; /**< Pointer to the virtual function table. */
};

/** @ingroup
 Used to query for static properties of the module that are independent of the
 instance. Among other things, this function is used to query the memory
 requirements of the module in order to create an instance. The function must
 fill in the data for the properties in the static_proplist. The client must
 pass in the property list that it passes to the capi_v2_init_f function as
 an input to this function. The property values in the init_set_proplist may
 be used by the module to calculate its memory requirements. The same
 properties that are sent to the module in the call to init() are also sent to
 this function to enable the module to calculate the memory requirement.

 @param[in]  init_set_proplist  The same properties that will be sent in the
 call to the capi_v2_init_f() function.
 @param[out] static_proplist   Pointer to the property list structure. The
 client will fill in the property ids for which it needs the property values.
 It will also allocate the memory for the payloads. The module must fill in
 the information in this memory.

 @return
 Indication of success or failure.

 @dependencies
 None.
 */
typedef capi_v2_err_t (*capi_v2_get_static_properties_f)(
                capi_v2_proplist_t *init_set_proplist,
                capi_v2_proplist_t *static_proplist);

/** @ingroup
 Instantiates the module to set up the virtual function table, and also
 allocates any memory required by the module. States within the module must
 be initialized at the same time. All return codes returned by this function
 except CAPI_V2_EOK, are considered to be FATAL. For any unsupported prop id
 passed in init_set_proplist, it should only print a MSG and continue
 processing other prop id's.

 @datatypes
 capi_v2_t \n
 capi_v2_proplist_t \n

 @param[in,out] _pif     Pointer to the module object. The memory has been
 allocated by the client based on the size returned in the
 CAPI_V2_INIT_MEMORY_REQUIREMENT property.
 @param[in]    init_set_proplist Properties set by the service to be used
 while init().

 @return
 Indication of success or failure.

 @dependencies
 */
typedef capi_v2_err_t (*capi_v2_init_f)(
                capi_v2_t* _pif, 
                capi_v2_proplist_t *init_set_proplist);

#endif /* #ifndef CAPI_V2_H */
