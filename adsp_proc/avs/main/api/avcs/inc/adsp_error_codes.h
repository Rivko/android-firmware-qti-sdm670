#ifndef _ADSP_ERROR_CODES_H_
#define _ADSP_ERROR_CODES_H_

/**
@file adsp_error_codes.h
@brief This file contains aDSP error code definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      AV_Core_Service_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2010, 2014, 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/avcs/inc/adsp_error_codes.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/02/16    rr      Merged Doxy markup/comments from 2.7
12/08/14    rr      Merged Doxy comments from 2.6.
09/22/10    sd      (Tech Pubs) Edited Doxygen markup and comments.
04/15/10   rkc      Created file.

========================================================================== */

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
/** Operation cannot be accepted or processed. */
#define ADSP_EBUSY        ( 0x0000000C )
/** Operation was aborted due to an error. */
#define ADSP_EABORTED     ( 0x0000000D )
/** Operation was preempted by a higher priority. */
#define ADSP_EPREEMPTED   ( 0x0000000E )
/** Operation requests an intervention to complete. */
#define ADSP_ECONTINUE    ( 0x0000000F )
/** Operation requests an immediate intervention to complete. */
#define ADSP_EIMMEDIATE   ( 0x00000010 )
/** Operation was not implemented. */
#define ADSP_ENOTIMPL     ( 0x00000011 )
/** Operation needs more data or resources. */
#define ADSP_ENEEDMORE    ( 0x00000012 )

/** Operation does not have memory. */
#define ADSP_ENOMEMORY     ( 0x00000014 )

/** Item does not exist. */
#define ADSP_ENOTEXIST     ( 0x00000015 )

/** Operation is finished. */
#define ADSP_ETERMINATED   ( 0x00011174 )

/** Status messages (error codes) returned by all aDSP command responses. */
typedef int ADSPResult;

/** @} */  /* end_addtogroup adsp_error_codes */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ERROR_CODES_H_ */
