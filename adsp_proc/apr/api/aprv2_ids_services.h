#ifndef __APRV2_IDS_SERVICES_H__
#define __APRV2_IDS_SERVICES_H__

/** @file aprv2_ids_services.h
*/

/*===========================================================================
  Copyright (C) 2010-2011, 2013-2014,2018-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/aprv2_ids_services.h#2 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2014/09/17 mh      Removed VUnicore support
2014/02/27 mh      Added AFE support for VUnicore
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

/** @addtogroup aprv2_constants_macros
@{ */
/*****************************************************************************/
/** @name Global Definitions
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_RESERVED_V    ( 0x00 ) /**< Reserved. */
#define APRV2_IDS_SERVICE_ID_ROUTER_V      ( 0x01 ) /**< Identifies a router. */
#define APRV2_IDS_SERVICE_ID_TESTCLIENT_V  ( 0x02 ) /**< Identifies a test
                                                         client. */
/** @} */ /* end_name Global Definitions */

/*****************************************************************************/
/** @name Modem Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_MODEM_MVS_V    ( 0x03 ) /**< Multimode vocoder
                                                          services. */
#define APRV2_IDS_SERVICE_ID_MODEM_MVM_V    ( 0x04 ) /**< Multimode voice
                                                          manager. */
#define APRV2_IDS_SERVICE_ID_MODEM_CVS_V    ( 0x05 ) /**< Core voice stream. */
#define APRV2_IDS_SERVICE_ID_MODEM_CVP_V    ( 0x06 ) /**< Core voice
                                                          processor.*/
#define APRV2_IDS_SERVICE_ID_MODEM_OEM_CM_V ( 0x07 ) /**< OEM call manager. */
#define APRV2_IDS_SERVICE_ID_MODEM_OEM_V    ( 0x08 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM_OEM1_V   ( 0x09 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM_OEM2_V   ( 0x0A ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM_OEM3_V   ( 0x0B ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM_OEM4_V   ( 0x0C ) /**< OEM customization. */

/** @} */ /* end_name Modem Service IDs */

/*****************************************************************************/
/** @name aDSP Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_ADSP_CORE_V  ( 0x03 ) /**< aDSP core service. */
#define APRV2_IDS_SERVICE_ID_ADSP_AFE_V   ( 0x04 ) /**< Audio front end. */
#define APRV2_IDS_SERVICE_ID_ADSP_VSM_V   ( 0x05 ) /**< Voice stream manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_VPM_V   ( 0x06 ) /**< Voice processing
                                                        manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_ASM_V   ( 0x07 ) /**< Audio stream manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_ADM_V   ( 0x08 ) /**< Audio device manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_MVM_V   ( 0x09 ) /**< Multimode voice
                                                        manager.*/
#define APRV2_IDS_SERVICE_ID_ADSP_CVS_V   ( 0x0A ) /**< Core voice stream. */
#define APRV2_IDS_SERVICE_ID_ADSP_CVP_V   ( 0x0B ) /**< Core voice processor. */
#define APRV2_IDS_SERVICE_ID_ADSP_USM_V   ( 0x0C ) /**< Ultrasound stream
                                                        manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_LSM_V   ( 0x0D ) /**< Listen stream
                                                        manager. */
#define APRV2_IDS_SERVICE_ID_ADSP_OEM_V   ( 0x10 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_ADSP_OEM1_V  ( 0x11 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_ADSP_OEM2_V  ( 0x12 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_ADSP_OEM3_V  ( 0x13 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_ADSP_OEM4_V  ( 0x14 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_ADSP_DSF_V   ( 0x15 ) /**< Dynamic service
                                                        factory. */
#define APRV2_IDS_SERVICE_ID_ADSP_VISM_V  ( 0x16 ) /**< Video Stream Manager. */

/* Note: ADSP Service IDs 0x0D..0x0F are reserved for future audio/voice FW use. */

/** @} */ /* end_name aDSP Service IDs */

/*****************************************************************************/
/** @name Application Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_APPS_DM      ( 0x03 ) /**< Audio device driver. */
#define APRV2_IDS_SERVICE_ID_APPS_AS      ( 0x04 ) /**< Audio stream driver. */
#define APRV2_IDS_SERVICE_ID_APPS_AC      ( 0x05 ) /**< Audio
                                                        pre/postprocessing
                                                        driver. */
#define APRV2_IDS_SERVICE_ID_APPS_VS      ( 0x06 ) /**< Voice stream driver. */
#define APRV2_IDS_SERVICE_ID_APPS_VC      ( 0x07 ) /**< Voice
                                                        pre/postprocessing
                                                        driver. */
#define APRV2_IDS_SERVICE_ID_APPS_ACDB    ( 0x08 ) /**< Audio calibration
                                                        database. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM_MW  ( 0x09 ) /**< OEM middleware. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM_V   ( 0x0A ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM1_V  ( 0x0B ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM2_V  ( 0x0C ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM3_V  ( 0x0D ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_APPS_OEM4_V  ( 0x0E ) /**< OEM customization. */

/** @} */ /* end_name Application Service IDs */

/*****************************************************************************/
/** @name Modem2 Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_MODEM2_MVS_V    ( 0x03 ) /**< Multimode vocoder
                                                           services. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM_CM_V ( 0x04 ) /**< OEM call manager. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM_V    ( 0x05 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM1_V   ( 0x06 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM2_V   ( 0x07 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM3_V   ( 0x08 ) /**< OEM customization. */
#define APRV2_IDS_SERVICE_ID_MODEM2_OEM4_V   ( 0x09 ) /**< OEM customization. */

/** @} */ /* end_name Modem2 Service IDs */
/** @} */ /* end_addtogroup aprv2_constants_macros */

/*****************************************************************************/
/** @name SDSP Compute DSP Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_SDSP_AP_V      ( 0x0E ) /**< AP Service. */

/*****************************************************************************/
/** @name Compute DSP Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_CDSP_AP_V      ( 0x0E ) /**< AP Service. */

/*****************************************************************************/
/** @name Modem Compute DSP Service IDs
@{ */
/****************************************************************************/

#define APRV2_IDS_SERVICE_ID_MDSP_AP_V      ( 0x0E ) /**< AP Service. */

#endif /**< __APRV2_IDS_SERVICES_H__. */

