#ifndef CUSTREMOTEAPIS_H
#define CUSTREMOTEAPIS_H
/*===========================================================================

                      C U S T R E M O T E A P I S . H

DESCRIPTION
  This file defines which APIs are remoted via ONCRPC.

  Copyright (c) 2006 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Id: //components/rel/build.tz/2.0.2/cust/custremoteapis.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/07   ptm     Add ONCRPC router message server feature.
04/10/07   gfr     Support for HW and TLMM remote apis.
03/20/07   gj      Pick up nonwinmob remote apis for 7500A; re-wrapped UIM/RUIM functions
03/01/07   gfr     Do not pick up nonwinmob remote apis for 7500A.
02/27/07   gfr     Added FEATURE_EXPORT_KEYPAD
01/02/07   rr      ATCOP_APIS only for CDMA
07/20/06   bfc     Merged in the changes for USB on APPS
07/03/06   jhs     Changed FEATURE_EXPORT_BUS_MON to
                   FEATURE_EXPORT_BUS_MON_REMOTE.
06/30/06   bfc     Added some changes for data-on-apps builds
06/15/06   TMR     Added support for data-on-apps builds
05/01/06   bfc     Added support for remote SMD APIS
04/19/06   bfc     Added support for ds707_extif
04/19/06   bfc     Added support for pm_strobe_flash
04/13/06   nrh     Added FEATURE_BATTERY_CHARGER
03/23/06    cl     Added FEATURE_EXPORT_PDSM and FEATURE_EXPORT_PDSM_ATL
12/22/05   ~SN     Added FEATURE_APPS_KEEPALIVE.
12/12/05    aw     Added definition of FEATURE_EXPORT_MVS
08/13/05   adm     Added feature support for joystick objects.
08/04/05   jhs     Removed FEATURE_NV_RPC_SUPPORT, moved to msm7500.builds.
07/15/05   aw      Added definition of FEATURE_EXPORT_AUDMGR
06/28/05   ptm     Remove // comments for L4 merge.
06/06/05   HN      Added definition of FEATURE_EXPORT_PBMLIB
04/22/05   HN      Added definition of FEATURE_EXPORT_RDEVMAP
04/05/05   HN      Moved #define FEATURE_NV_RPC_SUPPORT to this file right
                   after the #define FEATURE_EXPORT_NV.
04/04/05   HN      Reorganized order of API's in the list a little bit and
                   added FEATURE_EXPORT_NV protected by FEATURE_NV_RPC_SUPPORT
03/24/05   PTM     Initial release.
===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_VBATT_TASK 
      //#define FEATURE_EXPORT_VBATT_REMOTE
#endif

#ifdef FEATURE_EXPORT_7K_APIS

#ifdef FEATURE_ONCRPC_RTR_MSG
  /* Export the ONCRPC Server */
  #define FEATURE_EXPORT_ONCRPC_RTR_MSG
#endif /* FEATURE_ONCRPC_RTR_MSG */

  /* Export CM API */
  #define FEATURE_EXPORT_CM
  #define FEATURE_EXPORT_PBMLIB


/* Export BT API */
  #ifdef FEATURE_BT
//    #define FEATURE_EXPORT_BT
 //   #define FEATURE_EXPORT_FTM_BT
 //   #ifdef FEATURE_BT_EXTPF_SAP
 //   #define FEATURE_EXPORT_BTPF
 //   #endif /* FEATURE_BT_EXTPF_SAP */
  #endif

  #ifndef FEATURE_THIN_UI

    /* Export MVS API */
    #define FEATURE_EXPORT_MVS
  #endif /* ! FEATURE_THIN_UI */

  /* Export PDSM API */
  #ifdef FEATURE_CGPS
	#define FEATURE_EXPORT_PDSM_ATL
	#define FEATURE_EXPORT_PDAPI
    #define FEATURE_EXPORT_LOC_API
	/* Export PDCOMM and PDCOMM CLIENT API from modem to apps*/
//	#define FEATURE_EXPORT_PD_COMMS_API  
//	#define FEATURE_EXPORT_PD_COMMS_CLIENT_API  
  #endif /* FEATURE_CGPS */
  /* Export the miscellaneous APIs from modem to apps */
//  #define FEATURE_EXPORT_MISC_MODEM_APIS
// #define FEATURE_EXPORT_MISC_MODEM_APIS_NONWINMOB

  /* Export the miscellaneous APIs from apps to modem */
  /* #define FEATURE_EXPORT_MISC_APPS_APIS
  #define FEATURE_EXPORT_MISC_APPS_APIS_NONWINMOB */
  
  /* Export I2C driver API */
  #ifndef FEATURE_I2C_DIRECT_ON_ARM11
    #define FEATURE_EXPORT_I2C
  #endif /* !FEATURE_I2C_DIRECT_ON_ARM11 */

  /* Export TIME services syncing */
   //#define FEATURE_EXPORT_TIME_REMOTE_MTOA
  #define FEATURE_EXPORT_TIME_REMOTE_ATOM

  /* Export NV Api */
  #define FEATURE_EXPORT_NV

  /* Export CLKRGM_SEC API */
  #define FEATURE_EXPORT_CLKRGM_SEC

  /* Export TLMM API */
  //#define FEATURE_EXPORT_TLMM_REMOTE_ATOM

  /* Export the RDM API */
  #define FEATURE_EXPORT_RDEVMAP

  #ifdef FEATURE_CPRM
    /* Export CPRM API */
 //   #define FEATURE_EXPORT_SDCC_CPRM
 //   #define FEATURE_EXPORT_CPRMINTERFACE
  #endif

  #define FEATURE_EXPORT_REMOTEFS

  /* Export WMS API */
  #define FEATURE_EXPORT_WMS
  
  #ifdef FEATURE_KEYPAD
    /* Export keypad interface */
//    #define FEATURE_EXPORT_KEYPAD
  #endif

  #ifdef FEATURE_HS
    /* Export HS interface */
    #define FEATURE_EXPORT_HS_REM 
  #endif

// #define FEATURE_EXPORT_TOUCH_SCREEN_ADC

  #ifdef FEATURE_APPS_KEEPALIVE
    /* Export dog_keepalive interface */
    #define FEATURE_EXPORT_DOG_KEEPALIVE
  #endif

  #ifdef FEATURE_DATA_ON_APPS

    #ifdef FEATURE_CDMA
 //     #define FEATURE_EXPORT_CP
    #endif

    #ifdef FEATURE_EXPORT_CP
      /* Export MC API */
      #define FEATURE_EXPORT_MC

      /* Export MCCAP API */
      #define FEATURE_EXPORT_MCCAP

      /* Export MCCDMA API */
      #define FEATURE_EXPORT_MCCDMA

      /* Export MCCDS API */
      #define FEATURE_EXPORT_MCCDS

      /* Export MCCSCH API */
      #define FEATURE_EXPORT_MCCSCH

         /* Export MCCSRID API */
      #define FEATURE_EXPORT_MCCSRID
  
      /* Export MCCSYOBJ API */
      #define FEATURE_EXPORT_MCCSYOBJ
  
    #endif /* FEATURE_EXPORT_CP */

    /* Export the Data Services interface */
    #define FEATURE_EXPORT_DATA

    #if defined(FEATURE_EXPORT_DATA) && defined(FEATURE_CDMA)
      #define FEATURE_EXPORT_DATA_COMMON_APPS_APIS
      #define FEATURE_EXPORT_DSRLP_APIS
      #define FEATURE_EXPORT_RLP_APIS
      #define FEATURE_EXPORT_DS707_APIS
      #define FEATURE_EXPORT_DS_MP_SHIM_MODEM
      #define FEATURE_EXPORT_DS_MP_SHIM_APPS
      #define FEATURE_EXPORT_DS_MP_SHIM_APPS_ASYNC
      #ifdef FEATURE_HDR
         #define FEATURE_EXPORT_DSHDR_APIS
         #define FEATURE_EXPORT_DSHDR_MDM_APIS
      #endif /* FEATURE_HDR */
      #define FEATURE_EXPORT_DSBC_MDM_APIS
      #if defined(FEATURE_HDR) && defined(FEATURE_HDR_QOS)
         #define FEATURE_EXPORT_HDRMRLP_APPS_APIS
         #define FEATURE_EXPORT_HDRMRLP_MDM_APIS
      #endif
    #endif /* FEATURE_EXPORT_DATA && FEATURE_CDMA */
  #endif /* FEATURE_DATA_ON_APPS */

  /* Always Export SNM API */
  #ifdef FEATURE_CDMA
//     #define FEATURE_EXPORT_SNM
  #endif

  /* Export the  HDR interface */
  #ifdef FEATURE_HDR
    #define FEATURE_EXPORT_HDR
  #endif /* FEATURE_HDR */

  #ifdef  FEATURE_EXPORT_HDR
//    #define FEATURE_EXPORT_HDRMC_APIS
//    #define FEATURE_EXPORT_HDRMC_MRLP_APIS
  #endif /* FEATURE_EXPORT_HDR */

  /* Export ATCOP APIS */
  #ifdef FEATURE_DATA_ON_APPS
    #define FEATURE_EXPORT_DSATAPI
  #endif 

  #ifdef FEATURE_MEID_SUPPORT
    /* Export mccmeid interface */
    //#define FEATURE_EXPORT_MCCMEID
  #endif

  #ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
     /* Export ds707_extif interface */
    #define FEATURE_EXPORT_DS707_EXTIF
  #endif

  #if defined(FEATURE_DATA_ON_APPS) && defined(FEATURE_WCDMA)
    #define FEATURE_EXPORT_UMTS_DATA
  #endif /*FEATURE_DATA_ON_APPS*/

  #ifdef FEATURE_WCDMA
     #ifdef  FEATURE_EXPORT_UMTS_DATA
       #define FEATURE_EXPORT_DSMP_UMTS_MODEM_APIS
       #define FEATURE_EXPORT_DSMP_UMTS_APPS_APIS
       #define FEATURE_EXPORT_DSUCSDMPSHIM
       #define FEATURE_EXPORT_DATA_ON_APPS_ATOM_APIS
     #else
     /*  #define FEATURE_EXPORT_DATA_ON_MODEM_MTOA_APIS*/
       #ifndef FEATURE_DATA_ON_APPS
         #define FEATURE_EXPORT_DSUCSDAPPIF_APIS
       #endif
     #endif/*FEATURE_EXPORT_UMTS_DATA*/
  #endif

  #if defined(FEATURE_UIM) || defined(FEATURE_RUIM)
    /* export the UIM API */
    #define FEATURE_EXPORT_UIM

    #ifdef FEATURE_RUIM
      /* export the NVRUIMI API */
      /* #define FEATURE_EXPORT_NVRUIMI */
    #endif

    /* #ifdef FEATURE_GSDI */
    /* export the MMGSDILIB API */
  
    #define FEATURE_EXPORT_MMGSDILIB

    /* export the GSDI_EXP API */
    #define FEATURE_EXPORT_GSDI_EXP
  
    /* export the GSTK_EXP API */
    #define FEATURE_EXPORT_GSTK_EXP
  #endif /* FEATURE_UIM || FEATURE_RUIM */

  #ifdef FEATURE_AUTH
    /* export the AUTH API */
    #define FEATURE_EXPORT_AUTH
  #endif /* FEATURE_AUTH */

  //#define FEATURE_EXPORT_DIAG 

  /* export the PMAPP_GEN API */
  //#define FEATURE_EXPORT_PMAPP_GEN

  /* export the PM_LIB API */
 // #define FEATURE_EXPORT_PM_LIB

  /* export the PM VBATT */ 
  /*#define FEATURE_EXPORT_VBATT_REMOTE*/ 

  #if defined( FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK)
    /* export the CHG API */
   // #define FEATURE_EXPORT_CHARGER
  #endif

  #ifdef FEATURE_SEC
//    #define FEATURE_EXPORT_SECUTIL
  #endif

  #ifndef FEATURE_THIN_UI
 //   #define FEATURE_EXPORT_PM_STROBE_FLASH
  #endif

  #ifdef FEATURE_SMD_PORT_MGR
    #define FEATURE_EXPORT_SMD_PORT_MGR
  #endif

  #ifdef FEATURE_SMD_BRIDGE
//    #define FEATURE_EXPORT_SMD_BRIDGE_ATOM
//    #define FEATURE_EXPORT_SMD_BRIDGE_MTOA
  #endif

  #ifdef FEATURE_BUSPERF
//    #define FEATURE_EXPORT_BUS_PERF
  #endif

  #ifdef FEATURE_BUSMON
//   #define FEATURE_EXPORT_BUS_MON_REMOTE
  #endif

  #ifdef FEATURE_WLAN
    #define FEATURE_EXPORT_WLAN_CP_CM
//    #define FEATURE_EXPORT_WLAN_TRP_UTILS
  #endif

  #ifdef FEATURE_WLAN_FTM 
 //   #define FEATURE_EXPORT_FTM_WLAN
//    #define FEATURE_EXPORT_WLAN_ADP_FTM
  #endif
  /* Export RFM API */
//  #define FEATURE_EXPORT_RFM

  #if defined(FEATURE_HS_USB) && !defined(FEATURE_HS_USB_ON_MODEM_PROC)
   // #define FEATURE_EXPORT_HSU_APP_APIS
   // #define FEATURE_EXPORT_HSU_MDM_APIS
  #endif

#define FEATURE_EXPORT_OEM_RAPI
#define FEATURE_EXPORT_PING_MDM_RPC

#ifdef FEATURE_PMEM_REMOTE
//    #define FEATURE_EXPORT_PMEM_REMOTE
#endif

#ifdef FEATURE_SENSORS
   #define FEATURE_EXPORT_ISENSE
#endif


#endif /* FEATURE_EXPORT_7K_APIS */
#endif /* CUSTREMOTEAPIS_H */

