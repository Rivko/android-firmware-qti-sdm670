#ifndef MOBILE_H
#define MOBILE_H
/*===========================================================================

              M O B I L E    H E A D E R    F I L E

DESCRIPTION
  Header file for the module containing the Main procedure for the DMSS Mobile
  software.  All the declarations and definitions necessary to support access
  to the version string and link date/time of the software.

Copyright (c) 1992, 1997 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1998-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/14   kedara  Include com_dtypes.h instead of comdef.h
04/03/06   ptm     Export exhandler_tcb.
03/09/06   dp      7200 GSM bringup changes
02/03/06   dp      Added VOC_DSP_VER for MSM7200
11/07/02   cah     Add GSM defines.
10/17/02   jct     Added guid definition
03/23/02   ssw     Added FEATURE_GPSONE
10/09/00   dsb     Mainlined T_ARM.
03/31/99   ms      Added #include comdef.h.
11/10/98   jah     Ported ARM changes, added target.h.
                   Removed target conditionals on extern of mob_sw_rev[].
03/27/98   dak     Added T_I2 to targets for mob_sw_rev so data compiles.
03/24/98   aks     Added Odie (T_O) target configuration.
06/17/97   dhh     Added target T_Q for Q Phone.
04/28/97   jjn     Configured for MODULE (T_MD)
11/02/96   jah     Configured for TGP (T_T)
09/07/95   cah     Added mob_sw_rev for new marketing software version display
04/10/95   jah     Changed NOTICE_* for PC & 2nd Generation to _CODE to make
                   the relocatable linker cooperate.
11/02/93   jah     Fixed SCM_IS54_POWER_CLASS_V to be 0 instead of 0x10.
09/14/92    ip     Added ver_dir for directory string.
09/09/92    ip     Deleted version, to be replaced by mob_firm_rev, and
                   added release date and time.
07/30/92   jah     Added externs and definitions for station_class_mark,
                   slot_cycle_index, mob_cai_rev, mob_firm_rev, and mob_model.
06/23/92   jah     Created

===========================================================================*/

#include "com_dtypes.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Type directive for data belonging in the "Notice" segment
*/
#define NOTICE_TYPE       const char
#define NOTICE_SEG_TYPE

/* Compile and Release Date/Time
*/
extern NOTICE_TYPE ver_date[];       /* Compile date string        */
extern NOTICE_TYPE ver_time[];       /* Compile time string        */
extern NOTICE_TYPE ver_dir[];        /* Compile directory string   */

extern NOTICE_TYPE rel_date[];       /* Release date string        */
extern NOTICE_TYPE rel_time[];       /* Release time string        */

typedef PACKED unsigned long mob_guid_type[ 4 ];

/* Station Class Mark (CAI Section 2.3.3)
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

/* Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE slot_cycle_index;

/* Mobile CAI Revision (CAI Section 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE mob_cai_rev;

/* Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
extern const word NOTICE_SEG_TYPE mob_firm_rev;

/* Mobile Model (CAI Section 6.7.4.7)
*/
extern const byte NOTICE_SEG_TYPE mob_model;          // 8 bits. do not modify

/* Mobile Firmware Marketing Software Revision 
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

#endif  /* MOBILE_H */
