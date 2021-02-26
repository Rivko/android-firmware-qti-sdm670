#ifndef __VSS_DELETED_PUBLIC_IF_H__
#define __VSS_DELETED_PUBLIC_IF_H__

/**
  @file vss_deleted_public_if.h
*/
/*===========================================================================
NOTE: This file contains the APR API definitions that are deleted and
      unsupported by the Core Voice Driver (CVD). The only purpose of this file
      is to allow CVD's code compilation.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_deleted_public_if.h#1 $
  $Author: pwbldsvc $

===========================================================================*/

/****************************************************************************
 * DELETED CALIBRATION FRAMEWORK DEFINITIONS                                *
 ****************************************************************************/

/** @addtogroup cvd_common_deleted
@{ */

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION COLUMN IDS                                           *
 *--------------------------------------------------------------------------*/

/** Column vocoder class. */
#define VSS_ICOMMON_CAL_COLUMN_VOC_CLASS ( 0x00011359 )

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION VOCODER CLASS COLUMN VALUES                          *
 *--------------------------------------------------------------------------*/

/** Vocoder class is NONE. */
#define VSS_ICOMMON_CAL_VOC_CLASS_NONE ( 0x00011364 )

/** CDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_CDMA ( 0x00011365 )

/** GSM vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_GSM ( 0x00011366 )

/** WCDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_WCDMA ( 0x00011367 )

/** @} */ /* end_addtogroup cvd_common_deleted */

#endif /* __VSS_DELETED_PUBLIC_IF_H__ */

