#ifndef SMD_PACKET_H
#define SMD_PACKET_H

/*===========================================================================

                    Shared Memory Driver Packet Header File

-----------------------------------------------------------------------------
Copyright (c) 2007, 2010-2011 Qualcomm Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/11   bt      Removed SMD Block and Data Mover protocols.
04/21/10   tl      Moved SMD packet header definitions for SMD Lite API
08/01/07   bfc     Incorporated peer review comments
03/14/07   bfc     initial creation
===========================================================================*/

/*=============================================================================

                          GLOBAL DEFINITIONS FOR MODULE

=============================================================================*/

#define SMD_PACKET_SCRATCH_SZ                     256

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      smd_packet_memcpy_status_change

DESCRIPTION   This routine is called to insert or remove the primative
              packet layer when the SMD_IOCTL_SET_PACKET_MODE ioctl is
              invoked.

ARGUMENTS     info - stream_info ptr

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  SMD_UNAVAIL - if smd_memcpy is not supported
              SMD_DONE - if smd_memcpy is support

SIDE EFFECTS  Called in SMD context
===========================================================================*/
smd_api_return_type smd_packet_memcpy_status_change
( 
  smd_stream_info_type *info,
  byte *rx_scratch_buf,
  uint32 *rx_packet_header,
  uint32 *tx_packet_header
);

#endif /* SMD_PACKET_H */


