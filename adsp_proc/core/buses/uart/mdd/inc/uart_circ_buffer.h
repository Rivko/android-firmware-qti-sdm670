#ifndef UART_CIRC_BUFFER_H
#define UART_CIRC_BUFFER_H
/*============================================================================
 
GENERAL DESCRIPTION

  Defines a circular buffer object.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/uart_circ_buffer.h#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
01-Oct-2009   cmihalik   Initial revision.

============================================================================*/

#include "comdef.h"

//----------------------------------------------------------------------------
//  uart_circular_buffer_type
//----------------------------------------------------------------------------

#define UART_CIRCULAR_BUFFER_SIZE   (1*1024)

//  If the storage for the buffer is allocated on the heap,
//  then this structure definition can be moved to the .c 
//  file and handled as an opaque type.

typedef struct uart_circular_buffer_type
{
   uint8                       data[UART_CIRCULAR_BUFFER_SIZE];
   uint8*                      end_ptr;
   uint8*                      read_ptr;
   uint8*                      write_ptr;
   uint32                      size;
   uint32                      bytes;

}  uart_circular_buffer_type;

/*============================================================================
FUNCTION:      uart_circ_buffer_init

DESCRIPTION:   Initialize the circular buffer.

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  None.

SIDE EFFECTS:  The buffer is reinitialized to contain no data.
============================================================================*/

void                           
uart_circ_buffer_init( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_write_loc

DESCRIPTION:   Get the current write location from the circular buffer,
               and the number of bytes that can be written to that
               location.

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr    - pointer to circular buffer.

               num_bytes - number of bytes that can be written to the
                           returned location.

RETURN VALUE:  Location to write within the circular buffer.

SIDE EFFECTS:  None.
============================================================================*/

uint8*                         
uart_circ_buffer_get_write_loc( uart_circular_buffer_type* cb_ptr, uint32* num_bytes );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_read_loc

DESCRIPTION:   Get the current read location from the circular buffer,
               and the number of bytes that can be read from that 
               location.

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr    - pointer to circular buffer.

               num_bytes - number of bytes that can be read from the 
                           returned location.

RETURN VALUE:  Location to read from within the circular buffer.

SIDE EFFECTS:  None.
============================================================================*/

uint8*                         
uart_circ_buffer_get_read_loc( uart_circular_buffer_type* cb_ptr, uint32* num_bytes );

/*============================================================================
FUNCTION:      uart_circ_buffer_wrote_data

DESCRIPTION:   Update the buffer state after writing to it.  The get_write_loc
               routine returns a location to write to.  The client may write
               all bytes to that location or not.  Call this routine after the
               writing to update the circular buffer with the amount of data
               actually written to the write location.

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr    - pointer to circular buffer.

               num_bytes - number of bytes written to the current
                           write location.

RETURN VALUE:  None.

SIDE EFFECTS:  Updates the circular buffer.
============================================================================*/

void                           
uart_circ_buffer_wrote_data( uart_circular_buffer_type* cb_ptr, uint32 num_bytes );

/*============================================================================
FUNCTION:      uart_circ_buffer_read_data

DESCRIPTION:   Update the buffer state after reading from it.  The get_read_loc
               routine returns a location to read from.  The client may read
               all bytes from that location or not.  Call this routine after the
               reading to update the circular buffer with the amount of data
               actually read from the read location.

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr    - pointer to circular buffer.

               num_bytes - number of bytes read from the current
                           read location.

RETURN VALUE:  None.

SIDE EFFECTS:  Updates the circular buffer.
============================================================================*/

void                           
uart_circ_buffer_read_data( uart_circular_buffer_type* cb_ptr, uint32 num_bytes );

/*============================================================================
FUNCTION:      uart_circ_buffer_is_full

DESCRIPTION:   

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  TRUE if the circular buffer is full;
               FALSE otherwise.

SIDE EFFECTS:  None.
============================================================================*/

boolean                        
uart_circ_buffer_is_full( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_is_empty

DESCRIPTION:   

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  TRUE if the circular buffer is empty;
               FALSE otherwise.

SIDE EFFECTS:  None.
============================================================================*/

boolean                        
uart_circ_buffer_is_empty( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_buffer_start_ptr

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

uint8*                         
uart_circ_buffer_get_buffer_start_ptr( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_buffer_end_ptr

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

uint8*                         
uart_circ_buffer_get_buffer_end_ptr( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_read_ptr

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

uint8*                         
uart_circ_buffer_get_read_ptr( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_num_bytes

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

uint32                         
uart_circ_buffer_get_num_bytes( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_get_size

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

uint32                         
uart_circ_buffer_get_size( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_clear

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

void                         
uart_circ_buffer_clear( uart_circular_buffer_type* cb_ptr );

/*============================================================================
FUNCTION:      uart_circ_buffer_log_state

DESCRIPTION:   xxx

DEPENDENCIES:  None.

PARAMETERS:    cb_ptr - pointer to circular buffer.

RETURN VALUE:  xxx

SIDE EFFECTS:  xxx
============================================================================*/

void                         
uart_circ_buffer_log_state( uart_circular_buffer_type* cb_ptr );

#endif

