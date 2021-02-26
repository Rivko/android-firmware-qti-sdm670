/**
 * @file smdl_sig.c
 *
 * Implementation for smdl_sig_set() and smdl_sig_get() functions, used for 
 * RS-232 signaling in SMD Lite.
 */

/*==============================================================================
     Copyright (c) 2011 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   bm      Use new macros to access SMD channel header struct.
04/29/11   bt      Resolve potential compiler warning (break after return).
03/25/11   bt      Added set and get functionality for CD and RI signals.
01/12/11   bt      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_lite.h"
#include "smd_os.h"
#include "smd_internal.h"

/*===========================================================================
                    LOCAL MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Set or clear an RS-232 signal. 
 *
 * @param[in]    port    The port to set or clear the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR, CTS/RTS, 
 *                       CD, or RI.
 * @param[in]    value   Must be 1 for set, or 0 for clear. No other values are
 *                       allowed.
 *
 * @return       0 if successful.
 * @return       -1 if \c port is invalid or an error occurred.
 * 
 * @sideeffects  Writes the signal to shared memory and notifies the remote 
 *               processor if the value of the signal changed.
 */
int32 smdl_sig_set
(
  smdl_handle_type port,
  smdl_sig_type    sig,
  boolean          value
)
{
  if (port == NULL || (value != 0 && value != 1))
  {
    return -1;
  }
  
  switch( sig )
  {
  case SMDL_SIG_DTR_DSR:
	  if (SMD_READ_IF_SIG( port, SMD_TX_HDR, dtr_sig ) != value)
    {
		SMD_WRITE_IF_SIG( port, SMD_TX_HDR, dtr_sig, value );
      smdi_event_send_update( port );
    }
    break;
  case SMDL_SIG_RTS_CTS:
	  if (SMD_READ_IF_SIG( port, SMD_TX_HDR, cts_sig ) != value)
    {
		SMD_WRITE_IF_SIG( port, SMD_TX_HDR, cts_sig, value );
      smdi_event_send_update( port );
    }
    break;
  case SMDL_SIG_CD:
	  if (SMD_READ_IF_SIG( port, SMD_TX_HDR, cd_sig ) != value)
    {
		SMD_WRITE_IF_SIG( port, SMD_TX_HDR, cd_sig, value );
      smdi_event_send_update( port );
    }
    break;
  case SMDL_SIG_RI:
	  if (SMD_READ_IF_SIG( port, SMD_TX_HDR, ri_sig ) != value)
    {
		SMD_WRITE_IF_SIG( port, SMD_TX_HDR, ri_sig, value );
      smdi_event_send_update( port );
    }
    break;
  default:
    return -1;
  }
  return 0;
  
}

/**
 * Query the value of an RS-232 signal. 
 *
 * @param[in]    port    The port to query the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR, CTS/RTS, 
 *                       CD or RI.
 *
 * @return       The value of the signal (0 or 1) if successful.
 * @return       -1 if \c port is invalid or an error occurred.
 */
int32 smdl_sig_get
(
  smdl_handle_type port,
  smdl_sig_type    sig
)
{
  int32   ret;

  if (port == NULL)
  {
    return -1;
  }
  
  switch ( sig )
  {
  case SMDL_SIG_DTR_DSR:
	  ret = (int32)SMD_READ_IF_SIG( port, SMD_RX_HDR, dsr_sig );
    break;
  case SMDL_SIG_RTS_CTS:
	  ret = (int32)SMD_READ_IF_SIG( port, SMD_RX_HDR, rts_sig );
    break;
  case SMDL_SIG_CD:
	  ret = (int32)SMD_READ_IF_SIG( port, SMD_RX_HDR, cd_sig );
    break;
  case SMDL_SIG_RI:
	  ret = (int32)SMD_READ_IF_SIG( port, SMD_RX_HDR, ri_sig );
    break;
  default:
    ret = -1;
    break;
  }
  return ret;
}
