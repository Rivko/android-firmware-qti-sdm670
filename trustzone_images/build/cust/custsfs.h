#ifndef CUSTSFS_H
#define CUSTSFS_H
/*===========================================================================

              C U S T O M I Z A T I O N   O F   S F S

DESCRIPTION
  Configuration for secure msm sfs

  Copyright (c) 2005  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custsfs.h#1 $
$DateTime: 2018/02/06 03:02:41 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/08   ejt     defined FEATURE_SEC_SFS_MSM_DEV_KEY_API2
05/14/07   avm     defined FEATURE_SEC_SFS_JTAG_EFUSE for 6280
04/02/07   avm     defined FEATURE_SEC_USFS and undefined FEATURE_SEC_SFS_JTAG_EFUSE for 6800
04/02/07   avm     undefined FEATURE_SEC_USFS
02/08/07   avm     defined FEATURE_SEC_USFS
22/01/07   avm     Changed FEATURE_NONSEC_NOR to BOOT_MODE_NOR to check for NOR builds
10/09/06   df      Added FEATURE_SEC_SFS_JTAG_EFUSE
05/11/06   df      6800 65nm (no svd image)
04/05/06   df      6800 support secure partitions
04/04/06   df      6800 support for standalone mode
11/13/05   df      changes for secure flash partition
07/22/05   df      Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Security Services
---------------------------------------------------------------------------*/
/* Security Services SFS
*/
#ifdef FEATURE_SEC_SFS

/*disable SFP feature if FEATURE_SEC_EXCLUDE_SFP_10 is defined*/
#define FEATURE_SEC_EXCLUDE_SFP_10

#undef FEATURE_SEC_USFS

#undef FEATURE_SEC_SFS_JTAG_EFUSE

//Use secure MSM DEV KEY
#define FEATURE_SEC_SFS_MSM_DEV_KEY

#ifdef FEATURE_SEC_SFS_MSM_DEV_KEY
  #ifdef IMAGE_MODEM_PROC
     //Define API2 as the API to use to read MSM Dev key from modem register
     #define FEATURE_SEC_SFS_MSM_DEV_KEY_API2
  #endif // IMAGE_MODEM_PROC
#endif // FEATURE_SEC_SFS_MSM_DEV_KEY

/* can only have one of these enabled !!! */
#if defined(FEATURE_SEC_SFS_MSM_DEV_KEY) && \
    defined(FEATURE_SEC_SFS_MARM_HW_DEV_KEY)
#error cannot define both msm_dev_key and marm_hw_dev_key
#endif

  #ifndef BOOT_MODE_NOR
//        #define FEATURE_SEC_SFS_SECURE_PARTITION //needs Sim secure nand partition 
  #endif

#ifndef FEATURE_SEC_SFS_MSM_DEV_KEY
  #ifndef FEATURE_SEC_SFS_SECURE_PARTITION
  /* for 6250 efs implementation define this: */
    #define FEATURE_SEC_SFS_NV_ITEM
  #endif
#endif

#endif  /* FEATURE_SEC_SFS */
#endif  /* CUSTSFS */
