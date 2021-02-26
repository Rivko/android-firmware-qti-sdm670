/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  WCDMA Targets Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2001 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_tools.c_v   1.6   02 Apr 2002 11:36:40   amcintos  $
$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/toolsdiag.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/17/14   ph      Removed diag_strlcpy() and replaced with strlcpy().
11/22/13   sr      Removed PACK from pointer references 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
09/20/11   hm      Register for only the DLOAD command on Sparrow 
04/27/11   is      Resolve compiler warnings
03/30/11   sg      Fixed compilation error 
03/01/11   hm      Replaced deprecated systemdrivers API (hw_partnum_version) 
03/01/11   is      Migrate to diag_strlcpy()
01/21/11   is      Featurize DLoad feature
10/25/10   is      Diag feature flag cleanup
09/17/10   is      Migrate from deprecated clk driver APIs
06/10/10   is      Core image feature flag cleanup
06/02/10   vs      Added feature FEATURE_TOOLSDIAG_ESN
04/29/10   is      Reduce some feature flag uses
03/11/09   vs      fix compiler warnings
07/10/09   as      Updated usage of accessor function instead of CDMA sturct
10/03/08   vg      Updated code to use PACK() vs. PACKED
03/27/08   as      Added support to 32 bit HW model numbers.
07/12/06   as      Featurized for AppsProc,to prevent access to modem code.
10/31/05   as      Fixed lint errors.
04/25/05   as      Added extended build id response to include MSM 
                   hardware revision register under HW_REV_TYPE_V2.
12/17/04   as      Added check for FEATURE_HW_FULL_VERSION to get hw version
12/02/03   yll     Changed the mob_cai_rev to reflect the NV setting.
01/08/03   ma      featurization for GSM PLT
04/01/02   ATM     Added check for NULL after call to diagpkt_alloc
01/10/02   ATM     Corrected the ESN response length
09/21/01   abp     Using correct mobile model number for WCDMA
09/18/01   jal     Removed DIAG_STAT_F support
08/23/01   jal     Removed redundant DIAG_TS_F packet processing
05/08/01   cpe     renamed this file. Previously diagpkt_wcdma.c
05/07/01   lad     Cleaned up file.
02/23/01   lad     Created file.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "customer.h"

#ifdef DIAG_TOOLSDIAG_INIT
#include "comdef.h"
#include "diag.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagi.h"
#include "diagnv.h"
#include "toolsdiag.h"
#include "stringl.h"

#ifdef DIAG_TOOLSDIAG_DLOAD
  #include "dload.h"
  #include "DALSys.h"
#endif

#ifndef DIAG_TOOLSDIAG_DLOAD_ONLY
#include "err.h"
#include "hw.h"
#include "mobile.h"
#include "nv.h"


#ifdef DIAG_IS2000
#include "mccdma.h"
#endif

#include <string.h>
#include <DDIChipInfo.h>
#include "DALDeviceId.h"
/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
   used in some of the packets, as this is dictated by the awesome design */
/* lint -esym(715,pkt_len,req_ptr) */

/*=========================================================================== 
 
FUNCTION DIAG_HW_PARTNUM_VERSION 
 
DESCRIPTION 
  This procedure replaces the deprecated systemdrivers API hw_partnum_version
  Uses Chipinfo DAL APIs
 
===========================================================================*/
void diag_hw_partnum_version(diag_hw_rev_type *pmRev)
{
  uint32 nPartNum;
  uint32 nVersion;
  DalDeviceHandle *phChipInfo;

  /*-----------------------------------------------------------------------*/
  /* Connect to the DAL.                                                   */
  /*-----------------------------------------------------------------------*/
  if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
 {
   return;
 }

  DalChipInfo_GetRawChipId(phChipInfo, &nPartNum);
  DalChipInfo_GetRawChipVersion(phChipInfo, &nVersion);

  pmRev->version = (uint8)nVersion;
  pmRev->partnum = (uint16)nPartNum;
  pmRev->hw_register =
    (pmRev->version << 28) | (pmRev->partnum << 12);

} /* diag_hw_partnum_version */

/*===========================================================================

FUNCTION TOOLSDIAG_VERNO

DESCRIPTION
  This procedure processes a version number request.  It returns the version
  number, and the date and time of compilation.

===========================================================================*/
void * toolsdiag_verno (void * req_pkt, uint16 pkt_len)
{
  int i;
  word ver;
  DIAG_VERNO_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_VERNO_F_rsp_type);
  int8 sci_p = 0;
  byte scm = 0;
  byte mob_cai_rev = 0;

  diag_hw_rev_type new_ver = {0,0,0}; /* Buffer for the new MSM version information */

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rsp_ptr = (DIAG_VERNO_F_rsp_type *)diagpkt_alloc(DIAG_VERNO_F, rsp_len);

  if (rsp_ptr != NULL)
  {   /* Format response packet with version, date and time. */
      for ( i = 0; i < VERNO_DATE_STRLEN; i++ ) {
          rsp_ptr->comp_date[i] = ver_date[i];
          rsp_ptr->rel_date[i]  = rel_date[i];
      }

      for ( i=0; i<VERNO_TIME_STRLEN; i++ ) {
          rsp_ptr->comp_time[i] = ver_time[i];
          rsp_ptr->rel_time[i]  = rel_time[i];
      }

      for ( i=0; i<VERNO_DIR_STRLEN; i++ ) {
          rsp_ptr->ver_dir[i] = ver_dir[i];
      }

#if defined (DIAG_IS2000) 
      mccdma_get_cur_cdma_info(&scm,&mob_cai_rev,&sci_p);
#endif /* DIAG_IS2000 */

    rsp_ptr->mob_model = 0xff;   /* Invalid */
    rsp_ptr->mob_firm_rev = mob_firm_rev;
    rsp_ptr->slot_cycle_index = sci_p;
    rsp_ptr->scm = scm;
    rsp_ptr->mob_cai_rev = mob_cai_rev;

    /* Supply the hardware version */
    /* NOTE: This should be in 0xF0nn format where F0 is hw_maj_ver */
    diag_hw_partnum_version(&new_ver);
    ver = new_ver.partnum;

    rsp_ptr->hw_maj_ver = ver >> 8;     /* Upper nibble */
    rsp_ptr->hw_min_ver = (byte)(ver & 0x00FF); /* Lower nibble */
  }

  return (rsp_ptr);

} /* toolsdiag_verno */

/*===========================================================================

FUNCTION TOOLSDIAG_ESN

DESCRIPTION
  This procedure processes a received Mobile Station ESN request.

===========================================================================*/

#ifdef DIAG_TOOLSDIAG_ESN
void * toolsdiag_esn (void * req_pkt, uint16 pkt_len)
{
  DIAG_ESN_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_ESN_F_rsp_type);

  if (pkt_len != sizeof (DIAG_ESN_F_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp_ptr = (DIAG_ESN_F_rsp_type *)diagpkt_alloc(DIAG_ESN_F, rsp_len);
  
  if (rsp_ptr != NULL)
  {
    /* Format response packet ESN. */
    rsp_ptr->esn = diagnv_get_esn ();
  }
 
  return(rsp_ptr);

} /* toolsdiag_esn */
#endif /*DIAG_TOOLSDIAG_ESN */


/*===========================================================================

FUNCTION TOOLSDIAG_EXT_BUILD_ID

DESCRIPTION
  This procedure processes a version number request.  It returns the version
  number, and the date and time of compilation.

===========================================================================*/
void * toolsdiag_ext_build_id (void * req_pkt, uint16 pkt_len)
{
  toolsdiag_ext_build_id_rsp_type *build_id = NULL;
  char *str = NULL;
  uint32 rsp_len = FPOS (toolsdiag_ext_build_id_rsp_type, ver_strings);
  uint32 build_id_strlen = 0, model_strlen = 0; /* Includes NULL term char */
  diag_hw_rev_type new_ver = {0,0,0}; /* Buffer for the new MSM version information */

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* calc length for the "build_id" field */
  build_id_strlen = strlen (mob_sw_rev) + 1; /* NULL char */
  rsp_len += build_id_strlen;

  //XXX not supported
  model_strlen = 1; /* NULL char */
  rsp_len += model_strlen;

  build_id = (toolsdiag_ext_build_id_rsp_type *) 
    diagpkt_alloc (DIAG_EXT_BUILD_ID_F, rsp_len);

  if (build_id != NULL)
  {   
    build_id->msm_hw_version_format = 0;
    build_id->reserved[0] = 0;
    build_id->reserved[1] = 0;

#if !defined(CUST_MOB_MODEL_EXT)
     build_id->mobile_model_id = mob_model;
#else
     build_id->mobile_model_id = mob_model_ext;
#endif

    diag_hw_partnum_version(&new_ver);
#if defined HW_REV_TYPE_V2
    build_id->msm_hw_version = new_ver.hw_register;
    build_id->msm_hw_version_format = 2;
#else
    build_id->msm_hw_version =  new_ver.partnum << 4;
    build_id->msm_hw_version |= new_ver.version;
    build_id->msm_hw_version_format = 1;
#endif

    str = build_id->ver_strings;
    
    memset (str, 0, build_id_strlen + model_strlen);

    strlcpy(str, mob_sw_rev, build_id_strlen);
  }

  return (build_id);

} /* toolsdiag_ext_build_id */

#endif /* if NOT defined DIAG_TOOLSDIAG_DLOAD_ONLY */

/*===========================================================================
FUNCTION TOOLSDIAG_DLOAD_JUMP

DESCRIPTION
  Processes a request for the DMSS to jump to the download protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  A download will occur as soon as this packet has been acknowledged. Normal
  processing will cease.

===========================================================================*/
#ifdef DIAG_TOOLSDIAG_DLOAD
void * toolsdiag_dload_jump (void * req_pkt, uint16 pkt_len)
{
  toolsdiag_dload_rsp_type *rsp;
  const int rsp_len = sizeof (toolsdiag_dload_rsp_type);
  int j;

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp = (toolsdiag_dload_rsp_type *) diagpkt_alloc (DIAG_DLOAD_F, rsp_len);

  if (rsp)
  {
    diagpkt_commit (rsp);
    rsp = NULL;
  }

  /* This call disables all streamed data services and flushes the buffers. */
  diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

  /* pause briefly to allow the response to reach QPST */
  for(j=0; j<1000; j++)
  {
    DALSYS_BusyWait(1000);
  }

  dload_entry ();
  /*lint -unreachable*/

  return (rsp);

} /* toolsdiag_dload_jump */
#endif /* DIAG_TOOLSDIAG_DLOAD */

static const diagpkt_user_table_entry_type toolsdiag_tbl[] =
{
#ifndef DIAG_TOOLSDIAG_DLOAD_ONLY
  {DIAG_VERNO_F, DIAG_VERNO_F, toolsdiag_verno},
  {DIAG_EXT_BUILD_ID_F, DIAG_EXT_BUILD_ID_F, toolsdiag_ext_build_id},
#ifdef DIAG_TOOLSDIAG_ESN
  {DIAG_ESN_F, DIAG_ESN_F, toolsdiag_esn},
#endif
#endif /* !DIAG_TOOLSDIAG_DLOAD_ONLY*/
#ifdef DIAG_TOOLSDIAG_DLOAD
  {DIAG_DLOAD_F, DIAG_DLOAD_F, toolsdiag_dload_jump}
#endif
};

#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);
#else
  void toolsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);
  }
#endif
#endif /* DIAG_TOOLSDIAG_INIT */

/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif
