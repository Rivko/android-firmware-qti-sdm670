#ifndef DIAGDIAG_H
#define DIAGDIAG_H
/*!
    @ingroup diag_misc
    @file diagdiag.h
    @brief
      Diagnostic Packet Definitions
      Packet definitions between the diagnostic subsystem and the external
      device.

      Packet definitions are documented in Diag's ICD 80-V1294-10.
 
    @note
      All member structures of diag packets must be PACK.
 
    @warning
      Each command code number is part of the externalized
      diagnostic command interface.  This number *MUST* be assigned
      by a member of QCT's tools development team.
 */ 

/*
    Copyright (c) 2001-2010,2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 */
   
/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1.c4/api/services/diagdiag.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/12/12   ph      Removed the references of peek and poke code.
12/06/10   is      Removed DIAG_ERR_READ_F and DIAG_ERR_CLEAR_F since moved 
                   to error services
07/10/10   vs      Moved definitions of diagdiag peek/poke to here
05/18/10   is      Doxygenated.
09/30/09   as      Removed inclusion of customer.h 
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/15/09   mad     Removed un-necessary include files:
                   log_codes.h,qw.h and diag.h
07/14/09   mad     Featurized includion of customer.h
07/10/09   as      Removed inclusion of feature.h
06/11/09   JV      Moved the definition of diag_dipsw_req_type from diagdiag_v.h
                   to here.
06/11/09   JV      Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09   vk      removed dependencies from diagdiag_v.h 
10/03/08   vg      Updated code to use PACK() vs. PACKED
05/17/05   as      Added Dual processor Diag support.
06/15/04   gr      Added support for getting and setting the event mask.
05/18/04   as      Removed support for DIAG_USER_CMD_F & DIAG_PERM_USER_CMD_F
01/07/03   djm     add RPC support for WCDMA_PLT
08/20/02   lad     Moved DIAG_DLOAD_F packet def to dloaddiag.h.
                   Moved DIAG_SERIAL_CHG_F definition to diagcomm_sio.c.
01/28/02   as      Support for DIAG_LOG_ON_DEMAND_F (log on demand).
09/18/01   jal     Support for DIAG_CONTROL_F (mode reset/offline)
08/20/01   jal     Support for Diag packet: DIAG_TS_F (timestamp),
                   DIAG_SPC_F (service programming code), DIAG_DLOAD_F
		           (start downloader), DIAG_OUTP_F/DIAG_OUTPW_F (IO
		           port byte/word output), DIAG_INP_F/DIAG_INPW_F (IO
		           port byte/word input) 
06/27/01   lad     Use of constants in message response packet.
                   Added packet definition for DIAG_LOG_CONFIG_F.  This
                   replaces extended logmask processing.
                   Cleaned up DIAG_STREAMING_CONFIG_F and added subcmd to
                   get diagbuf size.
04/06/01   lad     Added packet definitions for the following:
                   Peek/Poke
                   DIAG_STREAMING_CONFIG_F
                   Debug messages
                   Log services
                   Event services
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"

/*!  @def DIAGPKT_SEC_CODE_SIZE
     Size of Diag security code
 */
#define  DIAGPKT_SEC_CODE_SIZE 6
 
/*!
    Type to hold Diag security code. The security code is fixed length
    and is stored as an ASCII string.
 */
typedef PACK(struct) {
  byte      digits[DIAGPKT_SEC_CODE_SIZE];     /**< -- Security code array */
} diagpkt_sec_code_type;

/*!  @def DIAGPKT_MAX_ERR
     Maximum number of bytes (starting with the first byte of the packet)
     from the received packet which will be echoed back to the Diagnostic 
     Monitor if an error is detected in the packet.
 */
#define DIAGPKT_MAX_ERR 16

/*!
     @cond DOXYGEN_BLOAT
 */

/*===========================================================================

PACKET   DIAG_SUBSYS_CMD_F
PURPOSE  This is a wrapper command to dispatch diag commands to various 
         subsystems in the DMSS.  This allows new diag commands to be 
         developed in those subsystems without requiring a change to diag 
         code. 
         
         The use of this command allows subsystems to manage their own
         diagnostics content.

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_SUBSYS_CMD_F)
  
  uint8  subsys_id;
  uint16 subsys_cmd_code;

  uint8 pkt[1]; /* The subsystem's request.  Variable length. */

DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_SUBSYS_CMD_F)
  
  uint8  subsys_id;
  uint16 subsys_cmd_code;
  
  uint8 pkt[1]; /* The subsystem's response.  Variable length. */

DIAGPKT_RSP_END


/* Type to communicate an error in a received packet */
DIAGPKT_RSP_DEFINE(DIAG_BAD_CMD_F)
  byte pkt[DIAGPKT_MAX_ERR];   /* first 16 bytes of received packet */
DIAGPKT_RSP_END


/*==========================================================================

PACKET   DIAG_SPC_F

PURPOSE  Request sent from the DM to the DMSS to enter the Service
         Programming Code (SPC), enabling service programming.  Response
         indicates whether or not the SPC was accepted as correct.

============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_SPC_F )

  diagpkt_sec_code_type sec_code;

DIAGPKT_REQ_END


DIAGPKT_RSP_DEFINE( DIAG_SPC_F )

  boolean sec_code_ok;

DIAGPKT_RSP_END

/*===========================================================================

PACKET   diag_dipsw_req_type

ID       DIAG_DIPSW_F

PURPOSE  Sent by DM to retreive the current dip switch settings

RESPONSE DMSS performs the test, then responds.

===========================================================================*/
typedef PACK(struct)
{
  byte cmd_code;			
  word switches;			
} diag_dipsw_req_type;

typedef diag_dipsw_req_type diag_dipsw_rsp_type;

/*!
     @endcond DOXYGEN_BLOAT
 */

/*!
@ingroup diag_misc
@brief
 This function is obsolete now.


@dependencies
None

@sideeffects
None

@return
<ul>
<li> The function always returns FALSE.
</ul>
 
@sa
diag_unregister_memory_address_cb
*/


boolean
diag_register_memory_address_cb (uint32 start, uint32 end,  
                                       const diagpkt_user_table_entry_type * tbl,int count);

/*!
@ingroup diag_misc
@brief
 This function is obselete now. 


@dependencies
None

@sideeffects
None

@return
<li> The function always returns FALSE.
</ul>
 
@sa
diag_register_memory_address_cb
*/

boolean
diag_unregister_memory_address_cb (uint32 start, uint32 end,  
                                       const diagpkt_user_table_entry_type * tbl);



/*!
@ingroup diag_misc
@def DIAG_REGISTER_MEMOP_TBL 
 This macro is obsolete now.

*/

#define DIAG_REGISTER_MEMOP_TBL(xx_start, xx_end, xx_tbl) \
      diag_register_memory_address_cb(xx_start,xx_end,xx_tbl, sizeof(xx_tbl)/sizeof(xx_tbl[0]));  

#endif /* DIAGDIAG_H */
