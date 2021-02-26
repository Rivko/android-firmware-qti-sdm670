/*==========================================================================
 
 General Description
   This modules contain implemnetation of diag_nv_read & diag_nv_write.
   
 Copyright (c) 2003-2010, 2013 by QUALCOMM Technologies, Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/rtos/src/diagnv.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/22/13   sr      Removed PACK from pointer references 
06/10/10   is      Core image feature flag cleanup
11/02/09   as      Merged DUAL sim support 
07/10/09   as      Decoupled code under FEATURE_DIAG_COUPLED_ARCHITECTURE &
                   FEATURE_DIAG_CACHE
05/07/09   vk      changed includes from external to internal headers
10/03/08   vg      Updated code to use PACK() vs. PACKED
09/01/07   pc      Made calls to EFS via NV, synchronous.
10/31/05   as      Fixed lint errors.
04/04/05   as      Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   as      Replaced FEATURE_NV_MEID_ENABLE with
                   FEATURE_DIAG_MEID_ENABLE.
03/10/05   ht      Added code for NV_MEID_I.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/05/02   lad     Integrated into first MSMSHARED_DIAG release.
11/11/01   as      Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "customer.h"

//#include "diagcmd.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include "diagnv_v.h"
#include "msg.h"

#include "nv.h"
#include "task.h"
   

static nv_cmd_type ncmd;  /* Command buffer for NV interface */
static nv_cmd_ext_type ncmd_ext;

nv_item_type nitem; /* NV Item buffer for NV interface */

/*===========================================================================

FUNCTION DIAGNV_CMD_INIT

DESCRIPTION
  Initializes the NV command parameter block used to communicate
  with the NV task.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
void diagnv_cmd_init (void) {

  static int ncmd_is_inited = FALSE;

  if( !ncmd_is_inited ) {

    ncmd_is_inited = TRUE;  /* Do this only once */
                  
    (void) q_link( &ncmd, &ncmd.link );

    ncmd.tcb_ptr = NULL;    /* Always signal diag            */
    ncmd.sigs = DIAG_NV_CMD_SIG;  /* Always reply through this sig */
    ncmd.done_q_ptr = NULL;       /* Not using links               */
  }

} /* diagnv_cmd_init() */


void diagnv_cmd_ext_init (void) {

  static int ncmd_ext_is_inited = FALSE;

  if( !ncmd_ext_is_inited ) {

    ncmd_ext_is_inited = TRUE;  /* Do this only once */
                  
    (void) q_link( &ncmd_ext, &ncmd_ext.link );
    ncmd_ext.nvcmd = &ncmd;
  }

} /* diagnv_cmd_ext_init() */

/*===========================================================================

FUNCTION DIAG_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_read ( 
  nv_items_enum_type item,              /* Which NV item to read */
  void * data_ptr          /* Where to put the data */
)

{
  diagnv_cmd_init();

  ncmd.cmd = NV_READ_F;
  
  if (data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;
  nv_cmd( &ncmd);
  return ( ncmd.status );
} /* diag_nv_read */ 

/*===========================================================================

FUNCTION DIAG_NV_READ_EXT

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task when a 
  context is specified.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/

nv_stat_enum_type diag_nv_read_ext ( 
  nv_items_enum_type item,              /* Which NV item to read */
  void * data_ptr,          /* Where to put the data */
  uint16 item_context
)

{
  diagnv_cmd_init();
  diagnv_cmd_ext_init();

  ncmd.cmd = NV_READ_F;
  
  if (data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;
  ncmd_ext.context = item_context;
  nv_cmd_ext( &ncmd_ext);
  return ( ncmd.status );
} /* diag_nv_read */ 

/*===========================================================================

FUNCTION DIAG_NV_WRITE
DESCRIPTION
  This procedure encapsulates an NV write operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_write ( 
  nv_items_enum_type item,
  void * data_ptr
)

{
  diagnv_cmd_init();

  ncmd.cmd = NV_WRITE_F;

  if( data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;

  nv_cmd( &ncmd);

  return ( ncmd.status );
} /* diag_nv_write */ 

/*===========================================================================

FUNCTION DIAG_NV_WRITE_EXT
DESCRIPTION
  This procedure encapsulates an NV write operation for the diag task when
  there is a context specified.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/

nv_stat_enum_type diag_nv_write_ext ( 
  nv_items_enum_type item,
  void * data_ptr,
  uint16 item_context
)

{
  diagnv_cmd_init();
  diagnv_cmd_ext_init();

  ncmd.cmd = NV_WRITE_F;

  if( data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;
  ncmd_ext.context = item_context;

  nv_cmd_ext( &ncmd_ext);

  return ( ncmd.status );
} /* diag_nv_write_ext */ 

/*=========================================================================*/
nv_stat_enum_type diagnv_peek (nv_peek_type *peek)
{
  if (!peek)
  {  
    return NV_BADPARM_S;  /* Error */
  }
  else
  {
    diagnv_cmd_init ();

    ncmd.cmd = NV_PEEK_F;

    ncmd.data_ptr = (nv_item_type *) peek;

    nv_cmd (&ncmd);

    return (ncmd.status);
  }
} /* diagnv_peek */ 



/*=========================================================================*/
nv_stat_enum_type diagnv_poke (nv_poke_type *poke)
{
  if (!poke)
  {  
    return NV_BADPARM_S;  /* Error */
  }
  else
  {
    diagnv_cmd_init ();

    ncmd.cmd = NV_POKE_F;

    ncmd.data_ptr = (nv_item_type *) poke;

    nv_cmd (&ncmd);

    return (ncmd.status);
  }
} /* diagnv_poke */ 


/*--------------------------------------------------------------------------
  Regional (to diag) buffers for nv information that is read at initializa-
  tion from NV.
--------------------------------------------------------------------------*/
  diag_nv_cache_type  diagnv;  /* Cached version of NV data */

/*===========================================================================*/
void diagnv_refresh_cache (void)
{
#ifdef DIAG_IS2000
    byte          i, j;
#endif /* DIAG_IS2000 */

  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Refreshing DIAG NV cache.");

  if ( diag_nv_read ( NV_ESN_I, &nitem ) != NV_DONE_S ) {
    MSG( MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to read ESN");
  } else {
    diagnv.esn = nitem.esn.esn;
  }

#ifdef DIAG_IS2000
  if (diag_nv_read (NV_CURR_NAM_I, &nitem) != NV_DONE_S)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to read CURR_NAM");
  } else
  {
    diagnv.current_nam = nitem.curr_nam;
  }

  for (i = 0; i < NV_MAX_NAMS; i++ )
  {
    nitem.min1.nam = i;

    if (diag_nv_read (NV_MIN1_I, &nitem) != NV_DONE_S)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to read MIN1 for NAM%d", i);
    } else
    {
      for (j = 0; j < NV_MAX_MINS; j++)
      {
        diagnv.min1[i][j] = nitem.min1.min1[j];
      }
    }

    nitem.min2.nam = i;

    if (diag_nv_read (NV_MIN2_I, &nitem) != NV_DONE_S)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to read MIN2 for NAM%d", i);
    } else
    {
      for (j = 0; j < NV_MAX_MINS; j++)
      {
        diagnv.min2[i][j] = nitem.min2.min2[j];
      }
    }
  }
#endif /* DIAG_IS2000 */

} /* diag_nv_refresh_cache */


/*===========================================================================*/
dword diagnv_get_esn (void)
{
#ifdef DIAG_IS2000
  static dword esn = 0;

  if (!esn)
  {
    /* No ESN or not initialized.  Read it from NV. */
    if (diag_nv_read (NV_ESN_I, (void *) &esn) != NV_DONE_S)
    {
      esn = 0;
    }
  }
  
  return esn;
#else
  return 0xDEADD00DU; /* Dummy ESN value */
#endif /* DIAG_IS2000 */
}


/*===========================================================================

FUNCTION DIAGNV_GET_MEID

DESCRIPTION
  This function retrieves the Mobile Equipment IDentifier (MEID) from NV.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV Status

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid (qword meid)
{

  nv_item_type  nv_item;  /* NV Item buffer for NV interface */

  nv_stat_enum_type nvstatus;  /* NV Status */

  nvstatus = diag_nv_read (NV_MEID_I, &nv_item);

  if (nvstatus == NV_DONE_S)
  {
    /* read successful, copy the meid */
    qw_equ(meid, nv_item.meid);
  }
  else
  {
    /* read was not successful */
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MEID READ Failed code = %d", nvstatus);
  }
  return nvstatus;

}

/*===========================================================================

FUNCTION DIAGNV_GET_MEID_EXT

DESCRIPTION
  This function retrieves the Mobile Equipment IDentifier (MEID) from NV
  when a context is specified.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV Status

SIDE EFFECTS
  None.
 
===========================================================================*/

nv_stat_enum_type diagnv_get_meid_ext (qword meid, uint16 meid_context)
{

  nv_item_type  nv_item;  /* NV Item buffer for NV interface */

  nv_stat_enum_type nvstatus;  /* NV Status */

  nvstatus = diag_nv_read_ext (NV_MEID_I, &nv_item, meid_context);

  if (nvstatus == NV_DONE_S)
  {
    /* read successful, copy the meid */
    qw_equ(meid, nv_item.meid);
  }
  else
  {
    /* read was not successful */
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MEID READ Failed code = %d", nvstatus);
  }
  return nvstatus;

}

