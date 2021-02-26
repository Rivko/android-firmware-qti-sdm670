#ifndef TOOLSDIAG_H
#define TOOLSDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACK.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001, 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_tools.h_v   1.1   23 Aug 2001 13:33:22   jlindqui  $
  $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/toolsdiag.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/13   xy      Removed code under FEATURE_WINCE  
03/01/11   hm      Replaced deprecated systemdrivers API (hw_partnum_version) 
10/25/10   is      Diag feature flag cleanup
06/10/10   is      Core image feature flag cleanup
09/01/09   JV      Added prototype for toolsdiag_init().
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/23/07   as      Enabled pragma pack support for WINCE targets
08/23/01   jal     Removed redundant DIAG_TS_F packet definitions
05/08/01   cpe	   renamed this file to diagpkt_tools.h from diagpkt_wcdma.h
05/07/01   lad     Cleaned up file.
02/23/01   lad     Created file from what used to be diagpkt.h

===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"

/* Prototype */
#ifdef DIAG_TOOLSDIAG_INIT
void toolsdiag_init (void);
#endif

/* -------------------------------------------------------------------------
** Packet definitions.
** ------------------------------------------------------------------------- */

/*============================================================================

PACKET   DIAG_VERNO_F
PURPOSE  Sent from the DM to the DMSS requesting the DMSS send its
         version number.

============================================================================*/
#define VERNO_DATE_STRLEN 11
#define VERNO_TIME_STRLEN  8
#define VERNO_DIR_STRLEN   8

#ifndef DIAG_VERNO_F
#define DIAG_VERNO_F 0
#endif

/* Request packet */
DIAGPKT_REQ_DEFINE(DIAG_VERNO_F)
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_RSP_DEFINE(DIAG_VERNO_F)

  char comp_date[ VERNO_DATE_STRLEN ];  /* Compile date Jun 11 1991   */
  char comp_time[ VERNO_TIME_STRLEN ];  /* Compile time hh:mm:ss      */
  char rel_date [ VERNO_DATE_STRLEN ];  /* Release date               */
  char rel_time [ VERNO_TIME_STRLEN ];  /* Release time               */
  char ver_dir  [ VERNO_DIR_STRLEN ];   /* Version directory          */
  byte scm;                             /* Station Class Mark         */
  byte mob_cai_rev;                     /* CAI rev                    */
  byte mob_model;                       /* Mobile Model               */
  word mob_firm_rev;                    /* Firmware Rev               */
  byte slot_cycle_index;                /* Slot Cycle Index           */
  byte hw_maj_ver;                      /* Hardware Version MSB       */
  byte hw_min_ver;                      /* Hardware Version LSB       */

DIAGPKT_RSP_END

typedef DIAG_VERNO_F_req_type diag_verno_req_type;
typedef DIAG_VERNO_F_rsp_type diag_verno_rsp_type;

/* Structure passed as an argument for SystemDriver API */
typedef PACK(struct)
{
  uint16 partnum;
  uint8  version;
  uint32 hw_register;
} diag_hw_rev_type;

/*===========================================================================

PACKET   DIAG_ESN_F
PURPOSE  Sent from the DM to the DMSS to request the Mobile's ESN

============================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_ESN_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_ESN_F)

  dword esn;

DIAGPKT_RSP_END

typedef DIAG_ESN_F_req_type diag_esn_req_type;
typedef DIAG_ESN_F_rsp_type diag_esn_rsp_type;

/*===========================================================================

PACKET   DIAG_EXT_BUILD_ID_F
PURPOSE  Sent from the DM to the DMSS to request the Mobile's ESN

============================================================================*/
typedef struct
{
  byte cmd_code;
  byte msm_hw_version_format;
  byte reserved[2]; /* for alignment / future use */

  uint32 msm_hw_version;
  uint32 mobile_model_id;

  /* The following character array contains 2 NULL terminated strings:
     'build_id' string, followed by 'model_string' */
  char ver_strings[1];

} toolsdiag_ext_build_id_rsp_type;

/*==========================================================================

PACKET   DIAG_DLOAD_F

PURPOSE  Request sent from the DM to the DMSS to enter the firmware
         downloader.

============================================================================*/
typedef struct
{
  byte cmd_code;
} toolsdiag_dload_req_type;

typedef struct
{
  byte cmd_code;
} toolsdiag_dload_rsp_type;



#endif /* TOOLSDIAG_H */
