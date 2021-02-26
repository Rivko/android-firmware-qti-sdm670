#ifndef SMD_MEMCPY_H
#define SMD_MEMCPY_H
/*===========================================================================

                Shared Memory Driver Memcpy Processing Header File


 Copyright (c) 2005, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/11   hwu     Added smd_rx_peek() to check for data left in receive FIFO.
10/12/09   bfc/rs  Moved WM functions to DSM layer, part of new SMD bridge 
                   changes and control/data port demux
10/25/07   hwu     Added cb_data to smd_wm_memcpy_open_2.
01/04/06   bfc     Broke SMD MEMCPY and DMOV out into components.
09/27/06   bfc     Added new API to drop pending transmit buffers and fixed
                   a bug in smd_memcpy_read_skip.
01/03/06   ptm     Update API names.
02/07/05   adm     Initial version. 
===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smd_memcpy_open

DESCRIPTION   This routine handles an open command for a memcpy stream. It
              allocates and initializes a shared memory structure, finishes
              initializing the stream info structure and sends the stream
              state machine an OPEN event.

ARGUMENTS     port_id - SMD port to open

RETURN VALUE  SMD_UNAVAIL - if smd_memcpy is not supported
              SMD_DONE - if smd_memcpy is supported

SIDE EFFECTS  None
===========================================================================*/
smd_api_return_type smd_memcpy_open( smd_port_id_type port_id );

/** 
 * Returns the number of bytes available in the receive FIFO.
 *
 *
 * @param[in]    port_id  port id
 *
 * @return       The number of bytes available in receive FIFO. (Determined by how 
 *               many bytes have been written so far and not yet read.)
 *
 * @sideeffects  None
 */
uint32 smd_rx_peek( smd_port_id_type  port_id );

#endif /* SMD_MEMCPY_H */
