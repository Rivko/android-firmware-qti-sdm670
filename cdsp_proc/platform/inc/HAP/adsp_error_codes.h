#ifndef _ADSP_ERROR_CODES_H_
#define _ADSP_ERROR_CODES_H_
/*============================================================================*/
/**
@file adsp_error_codes.h

This file contains the definitions for the standard error codes
*/
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S.
  Government. Diversion contrary to U.S. law prohibited. 
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup adsp_error_codes
  @{ */

/** Success. The operation completed with no errors. */
#define ADSP_EOK          ( 0x00000000 )
/** General failure. */
#define ADSP_EFAILED      ( 0x00000001 )
/** Bad operation parameter. */
#define ADSP_EBADPARAM    ( 0x00000002 )
/** Unsupported routine or operation. */
#define ADSP_EUNSUPPORTED ( 0x00000003 )
/** Unsupported version. */
#define ADSP_EVERSION     ( 0x00000004 )
/** Unexpected problem encountered. */
#define ADSP_EUNEXPECTED  ( 0x00000005 )
/** Unhandled problem occurred. */
#define ADSP_EPANIC       ( 0x00000006 )
/** Unable to allocate resource. */
#define ADSP_ENORESOURCE  ( 0x00000007 )
/** Invalid handle. */
#define ADSP_EHANDLE      ( 0x00000008 )
/** Operation is already processed. */
#define ADSP_EALREADY     ( 0x00000009 )
/** Operation is not ready to be processed. */
#define ADSP_ENOTREADY    ( 0x0000000A )
/** Operation is pending completion. */
#define ADSP_EPENDING     ( 0x0000000B )
/** Operation could not be accepted or processed. */
#define ADSP_EBUSY        ( 0x0000000C )
/** Operation aborted due to an error. */
#define ADSP_EABORTED     ( 0x0000000D )
/** Operation preempted by a higher priority. */
#define ADSP_EPREEMPTED   ( 0x0000000E )
/** Operation requests intervention to complete. */
#define ADSP_ECONTINUE    ( 0x0000000F )
/** Operation requests immediate intervention to complete. */
#define ADSP_EIMMEDIATE   ( 0x00000010 )
/** Operation is not implemented. */
#define ADSP_ENOTIMPL     ( 0x00000011 )
/** Operation needs more data or resources. */
#define ADSP_ENEEDMORE    ( 0x00000012 )


/** Operation does not have memory. */
#define ADSP_ENOMEMORY     ( 0x00000014 )

/** Item does not exist. */
#define ADSP_ENOTEXIST     ( 0x00000015 )

/** Operation is finished. */
#define ADSP_ETERMINATED   ( 0x00011174 )

/** Error codes returned by all aDSP command responses. */
typedef int ADSPResult;

/** Error code test macro. */
#define ADSP_FAILED(x) (ADSP_EOK != (x))

/** @} */  /* end_addtogroup adsp_error_codes */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ERROR_CODES_H_ */
