#ifndef MOBILE_H
#define MOBILE_H
/**
  @file mobile.h
  @brief This file contains constants and air interface definitions that
  are utilized primarily by the modem.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The mobile_mainpage.dox file contains all the file/group descriptions that
      are in the output PDF gnerated using Doxygen and LaTex. To edit or
      update any of the file/group text in the PDF, edit the mobile_mainpage.dox
      file or contact Tech Pubs.

      The above description for this file is part of the mobile group
      description in the mobile_mainpage.dox file.
=============================================================================*/
/*=============================================================================
  Copyright (c) 1992-2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

              M O B I L E    H E A D E R    F I L E

DESCRIPTION
  Header file for the module containing the Main procedure for the AMSS Mobile
  software.  All the declarations and definitions necessary to support access
  to the version string and link date/time of the software.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/api/debugtools/mobile.h#1 $

===========================================================================*/

#include "comdef.h" /* Definitions for byte, word, etc. */

#if defined(__cplusplus)
extern "C" {
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/** @addtogroup mobile_api
@{ */

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define MOBILE_DEPRECATED_ATTRIBUTE __attribute__((deprecated))

/** @name Legacy Mobile Values
  These legacy values and timer values are replaced in functionality
  by the Mobile API.
  @{
*/
/** @} */  /* end_name Legacy Mobile Values */

/** Type directive for data belonging in the "Notice" segment
*/
#define NOTICE_TYPE       const char
#define NOTICE_SEG_TYPE

/** Compile and Release Date/Time
*/
extern NOTICE_TYPE ver_date[];       /* Compile date string        */
extern NOTICE_TYPE ver_time[];       /* Compile time string        */
extern NOTICE_TYPE ver_dir[];        /* Compile version string     */
extern NOTICE_TYPE ver_asic[];       /* Compile ASIC Software Revs */

extern NOTICE_TYPE rel_date[];       /* Release date string        */
extern NOTICE_TYPE rel_time[];       /* Release time string        */

/** Type directive for mobile GUID
*/
typedef PACKED unsigned long mob_guid_type[ 4 ];

/** Station Class Mark (CAI Section 2.3.3)
*/
extern const byte NOTICE_SEG_TYPE station_class_mark;

#define SCM_RSVD_80_M           0x80    /* Reserved, always 0          */
#define SCM_RSVD_80_V           0x00    /* Reserved, always 0          */

#define SCM_DUAL_MODE_M         0x40    /* Dual Mode phone             */
#define SCM_DUAL_MODE_V         0x40    /* Dual Mode phone             */
#define SCM_CDMA_ONLY_V         0x00    /* CDMA Only                   */

#define SCM_SLOTTED_CLASS_M     0x20    /* Slotted Mode                */
#define SCM_NON_SLOTTED_V       0x00    /* Non-Slotted                 */
#define SCM_SLOTTED_V           0x20    /* Slotted                     */

#define SCM_IS54_POWER_CLASS_M  0x10    /* IS-54 Power class, always 0 */
#define SCM_IS54_POWER_CLASS_V  0x00    /* IS-54 Power class, always 0 */

#define SCM_25_MHZ_BANDWIDTH_M  0x08    /* 25 MHz bandwidth, always 1  */
#define SCM_25_MHZ_BANDWIDTH_V  0x08    /* 25 MHz bandwidth, always 1  */

#define SCM_TRANSMISSION_M      0x04    /* Transmission                */
#define SCM_CONTINOUS_V         0x00    /* Continuous transmission     */
#define SCM_DISCONTINOUS_V      0x04    /* Discontinuous transmission  */

#define SCM_POWER_CLASS_M       0x03    /* Power Class                 */
#define SCM_POWER_CLASS_I_V     0x00    /* Class I                     */
#define SCM_POWER_CLASS_II_V    0x01    /* Class II                    */
#define SCM_POWER_CLASS_III_V   0x02    /* Class III                   */
#define SCM_POWER_CLASS_RSVD_V  0x03    /* Reserved                    */

/* definitions for WCDMA version packet */
#define CLASS_MARK       0x01 /* W-CDMA class mark */
/* 3GPP REV, 4LSBs set to month,  4MSBs 0-1999, 1-2000 */
#define G3GPP_REV        0x1c /* OBSOLETE -- use G3GPP_REV_MONTH and G3GPP_REV_YEAR */
#define VOC_DSP_VER       0x608 /* Vocoder DSP version Number */

#define DRX_INDEX         0x1 /* Indicates if DRX is used for cell updating */

/** Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE slot_cycle_index;

/** Mobile CAI Revision (CAI Section 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE mob_cai_rev;

/** Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
extern const word NOTICE_SEG_TYPE mob_firm_rev;

/** Mobile Model (CAI Section 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE mob_model;          // 8 bits. do not modify

/** Mobile Firmware Marketing Software Revision
*/
extern NOTICE_TYPE mob_sw_rev[];

extern NOTICE_TYPE mob_manufacturer_id[];
extern NOTICE_TYPE mob_model_id[];
extern const byte NOTICE_SEG_TYPE mob_svn;
extern const uint32 NOTICE_SEG_TYPE mob_model_ext;    // 32 bits. do not modify

/* Device manufacturer id defaults to QCOM, It should be overwritten by
** individual phone manufacturers.Only first 4 char's are used.
*/
extern const  char mob_gpsone_device_manufacturer[];

/* Revision Id should also be updated by phone manufactures.
*/
extern dword  mob_gpsone_device_rev_id;

/** @} */ /* end_addtogroup mobile_api */

#if defined(__cplusplus)
}
#endif

#endif  /* MOBILE_H */
