/** @file EFIQdss.h

  Qualcomm Debug Subsystem Protocol for UEFI.

  Copyright (c) 2015,2017 Qualcomm Technologies, Inc. All Rights Reserved.

**/

#ifndef EFIQDSS_H_
#define EFIQDSS_H_

#include <Uefi.h>

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtigroup efi_qdss_constants
@{ */

/** Protocol version. */
#define EFI_QDSS_PROTOCOL_REVISION 0x0000000000000001

/*  Protocol GUID definition */
/** @ingroup efi_qdss_protocol */
#define EFI_QDSS_PROTOCOL_GUID \
   { 0xBC370B7B, 0x4A61, 0x0134, {0x22, 0xBF, 0xA4, 0x8D, 0x6A, 0xF6, 0xB4, 0xD0 } }

/** @} */ /* end_addtogroup efi_qdss_constants */

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */

/** External reference to the QDSS Protocol GUID. */
extern EFI_GUID gEfiQdssProtocolGuid;

/** @endcond */


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** @cond */

/** Protocol declaration. */
typedef struct _EFI_QDSS_PROTOCOL   EFI_QDSS_PROTOCOL;

/** @endcond */

/** @addtogroup efi_qdss_data_types
@{ */

/* None */

/** @} */ /* end_addtogroup efi_qdss_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_QDSS_INIT */
/** @ingroup efi_qdss_init
  @par Summary
  Setup, initialize, and enable the QDSS subsystem for SW tracing.

  @par Description
  Initialize QDSS by setting up the funnel, etfetb, and stm and...

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context. \n
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_INIT)(void);


/* EFI_QDSS_ENABLE_TRACE */
/** @ingroup efi_qdss_enable_trace
  @par Summary
  Enable the QDSS subsystem for SW tracing.

  @par Description
  Enable the QDSS subsystem for SW tracing.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_ENABLE_TRACE)(void);


/* EFI_QDSS_DISABLE_TRACE */
/** @ingroup efi_qdss_disable_trace
  @par Summary
  Disable the QDSS subsystem for SW tracing.

  @par Description
  Disable the QDSS subsystem for SW tracing.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_DISABLE_TRACE)(void);


/* EFI_QDSS_ENABLE_SW_EVENT */
/** @ingroup efi_qdss_enable_sw_event
  @par Summary
  Enable SW event(s) for tracing.

  @par Description
  Enable SW event(s) of the QDSS subsystem for tracing.

  @param[in]    SwEventIdNum    Identifying number of the SW event to enable.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- SwEventIdNum is invalid. \n
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_ENABLE_SW_EVENT)(IN UINT32 SwEventIdNum);


/* EFI_QDSS_DSABLE_SW_EVENT */
/** @ingroup efi_qdss_disable_sw_event
  @par Summary
  Disable SW event(s) for tracing.

  @par Description
  Disable SW event(s) of the QDSS subsystem for tracing.

  @param[in]    SwEventIdNum    Identifying number of the SW event to disable.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- SwEventIdNum is invalid. \n
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_DISABLE_SW_EVENT)(IN UINT32 SwEventIdNum);


/* EFI_QDSS_TRACE_SW_EVENT */
/** @ingroup efi_qdss_trace_sw_event
  @par Summary
  Trace a SW event through QDSS.

  @par Description
  Puts the specified SW event trace into the QDSS trace stream.

  @param[in]    SwEventIdNum    Identifying number of the SW event being traced.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_TRACE_SW_EVENT)(
  IN UINT32  SwEventIdNum
);

/* EFI_QDSS_TRACE_SW_EVENT_VARGS */
/** @ingroup efi_qdss_trace_sw_event_vargs
  @par Summary
  Trace a SW event with data through QDSS.

  @par Description
  Puts the specified SW event trace along with supporting data into the QDSS trace stream.

  @param[in]    SwEventIdNum    Identifying number of the SW event being traced.
  @param[in]    NumberArgs      Number of 32-bit data arguments.
  @param[in]    ...             Arguments associated with the event.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_TRACE_SW_EVENT_VARGS)(
  IN UINT32  SwEventIdNum,
  IN UINT32  NumberArgs,
  ...
);

/* EFI_QDSS_TRACE_SW_EVENT_VALIST */
/** @ingroup efi_qdss_trace_sw_event_valist
  @par Summary
  Trace a SW event with data through QDSS.

  @par Description
  Puts the specified SW event trace along with supporting data into the QDSS trace stream.

  @param[in]    SwEventIdNum    Identifying number of the SW event being traced.
  @param[in]    NumberArgs      Number of 32-bit data arguments.
  @param[in]    varg_list       va_list of arguments associated with the event.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_QDSS_TRACE_SW_EVENT_VALIST)(
  IN UINT32  SwEventIdNum,
  IN UINT32  NumberArgs,
  IN VA_LIST varg_list
);



/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_qdss_protocol
  @par Summary
  QUALCOMM Debug Subsystem (QDSS) Protocol interface.

  @par Parameters
  @inputprotoparams{qdss_proto_params.tex}
*/
struct _EFI_QDSS_PROTOCOL {
  UINT64                            Revision;
  EFI_QDSS_INIT                     Init;
  EFI_QDSS_ENABLE_TRACE             EnableTrace;
  EFI_QDSS_DISABLE_TRACE            DisableTrace;
  EFI_QDSS_ENABLE_SW_EVENT          EnableSwEvent;
  EFI_QDSS_DISABLE_SW_EVENT         DisableSwEvent;
  EFI_QDSS_TRACE_SW_EVENT           TraceSwEvent;
  EFI_QDSS_TRACE_SW_EVENT_VARGS     TraceSwEventVargs;
  EFI_QDSS_TRACE_SW_EVENT_VALIST    TraceSwEventValist;
};

#endif  /* EFIQDSS_H_ */
