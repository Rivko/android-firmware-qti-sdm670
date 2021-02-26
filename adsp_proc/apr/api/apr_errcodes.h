#ifndef __APR_ERRCODES_H__
#define __APR_ERRCODES_H__

/** @file apr_errcodes.h
*/

/*===========================================================================
Copyright (c) 2009-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/apr_errcodes.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.

2016/07/22 sv      addition of new error code
===========================================================================*/

/** @addtogroup apr_core_status_error_codes
@{ */

/* 0x00000000 to 0x00000FFF are GUIDs pre-allocated for router error codes. */

#define APR_EOK          ( 0x00000000 ) /**< Success; completed; no errors. */
#define APR_EFAILED      ( 0x00000001 ) /**< General failure. */
#define APR_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter. */
#define APR_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define APR_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define APR_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define APR_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define APR_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resources. */
#define APR_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define APR_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define APR_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be
                                             processed. */

#define APR_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define APR_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or
                                             processed. */

#define APR_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define APR_EPREEMPTED   ( 0x0000000E ) /**< Operation is preempted by a higher
                                             priority. */
#define APR_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to
                                             complete. */
#define APR_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate
                                             intervention to complete. */
#define APR_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define APR_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or
                                             resources. */
#define APR_ELPC         ( 0x00000013 ) /**< Operation is a local procedure
                                             call. */

#define APR_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate sufficient
                                             memory. */
#define APR_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */
#define APR_ENOSERVICE   ( 0x00000016 ) /**< Operation sent to an unregistered
                                             service. */
#define APR_EDENIED      ( 0x00000017 ) /**< Access is denied. */
#define APR_ENOPROCESS   ( 0x00000018 ) /**< No process due to UPD down. */

/** @} */ /* end_addtogroup apr_core_status_error_codes */

#endif /* __APR_ERRCODES_H__ */

