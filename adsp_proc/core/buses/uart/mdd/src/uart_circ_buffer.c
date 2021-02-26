
/*============================================================================
 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/src/uart_circ_buffer.c#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
01-Oct-2009   cmihalik   Initial revision.

============================================================================*/

#include "comdef.h"

#include "assert.h"
#include "DALSys.h"

#include "uart_circ_buffer.h"
#include "uart_log.h"

//----------------------------------------------------------------------------
//  UART_TRACE_BUFFER_FILE_ID must be unique amongst all files writing to the
//  trace buffer.
//----------------------------------------------------------------------------

#define  UART_TRACE_BUFFER_FILE_ID            3

//----------------------------------------------------------------------------
//  uart_circ_buffer_init
//----------------------------------------------------------------------------

void
uart_circ_buffer_init( uart_circular_buffer_type* cb_ptr )
{
   if( NULL == cb_ptr )
      return;

   cb_ptr->size      = sizeof( cb_ptr->data ) / sizeof( cb_ptr->data[0] );
   cb_ptr->bytes     = 0;
   cb_ptr->end_ptr   = cb_ptr->data + cb_ptr->size;
   cb_ptr->read_ptr  = cb_ptr->data;
   cb_ptr->write_ptr = cb_ptr->data;
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_write_loc
//
//  Returns: write ptr and the number of bytes that can be written to 
//  write ptr. 
//----------------------------------------------------------------------------

uint8*                         
uart_circ_buffer_get_write_loc( uart_circular_buffer_type* cb_ptr, uint32* numBytes )
{
   *numBytes = 0;
   if( NULL == cb_ptr )
      return( NULL );

   //  If the buffer is full, can't write anything.
   if( cb_ptr->size == cb_ptr->bytes )
      *numBytes = 0;

   //  Can write til the end of buffer. 
   else if( cb_ptr->write_ptr >= cb_ptr->read_ptr )
      *numBytes = cb_ptr->end_ptr - cb_ptr->write_ptr; 

   //  Can write til the read pointer.
   else
      *numBytes = cb_ptr->read_ptr - cb_ptr->write_ptr; 

   return( cb_ptr->write_ptr );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_read_loc
//
//  Returns: read ptr and the number of bytes that can be read from read ptr. 
//----------------------------------------------------------------------------

uint8*                         
uart_circ_buffer_get_read_loc( uart_circular_buffer_type* cb_ptr, uint32* numBytes )
{
   *numBytes = 0;
   if( NULL == cb_ptr )
      return( NULL );

   //  If the buffer is empty, can't read anything.
   if( 0 == cb_ptr->bytes )
      *numBytes = 0;

   //  Can read til end of buffer.
   else if( cb_ptr->read_ptr >= cb_ptr->write_ptr )
      *numBytes = cb_ptr->end_ptr - cb_ptr->read_ptr; 

   //  Can read til the write pointer.
   else
      *numBytes = cb_ptr->write_ptr - cb_ptr->read_ptr; 

   return( cb_ptr->read_ptr );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_wrote_data
//----------------------------------------------------------------------------

void                           
uart_circ_buffer_wrote_data( uart_circular_buffer_type* cb_ptr, uint32 numBytes )
{
   uint32                         numAvail;



   if( cb_ptr == NULL )
      return;

   numAvail = cb_ptr->size - cb_ptr->bytes; 
   if( numBytes > numAvail )
      return;

   cb_ptr->bytes += numBytes;

   if( cb_ptr->write_ptr + numBytes >= cb_ptr->end_ptr )
   {
      numBytes -= (cb_ptr->end_ptr - cb_ptr->write_ptr);
      cb_ptr->write_ptr = cb_ptr->data + numBytes;
   }
   else
   {
      cb_ptr->write_ptr += numBytes;
   }
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_read_data
//----------------------------------------------------------------------------

void                           
uart_circ_buffer_read_data( uart_circular_buffer_type* cb_ptr, uint32 numBytes )
{
   uint32                         numAvail;



   if( cb_ptr == NULL )
      return;

   numAvail = cb_ptr->bytes; 
   if( numBytes > numAvail )
      return;

   cb_ptr->bytes -= numBytes;

   if( cb_ptr->read_ptr + numBytes >= cb_ptr->end_ptr )
   {
      numBytes -= (cb_ptr->end_ptr - cb_ptr->read_ptr);
      cb_ptr->read_ptr = cb_ptr->data + numBytes;
   }
   else
   {
      cb_ptr->read_ptr += numBytes;
   }
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_is_empty
//----------------------------------------------------------------------------

boolean
uart_circ_buffer_is_empty( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return( FALSE );

   return( 0 == cb_ptr->bytes );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_is_full
//----------------------------------------------------------------------------

boolean
uart_circ_buffer_is_full( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return( FALSE );

   return( cb_ptr->bytes == cb_ptr->size );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_buffer_start_ptr
//----------------------------------------------------------------------------

uint8*                         
uart_circ_buffer_get_buffer_start_ptr( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return( NULL );

   return( cb_ptr->data );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_buffer_end_ptr
//----------------------------------------------------------------------------

uint8*                         
uart_circ_buffer_get_buffer_end_ptr( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return( NULL );

   return( cb_ptr->end_ptr );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_read_ptr
//----------------------------------------------------------------------------

uint8*                         
uart_circ_buffer_get_read_ptr( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return( NULL );

   return( cb_ptr->read_ptr );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_num_bytes
//----------------------------------------------------------------------------

uint32                         
uart_circ_buffer_get_num_bytes( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return(0);

   return( cb_ptr->bytes );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_get_size
//----------------------------------------------------------------------------

uint32                         
uart_circ_buffer_get_size( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return(0);

   return( cb_ptr->size );
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_clear
//----------------------------------------------------------------------------

void
uart_circ_buffer_clear( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return;

   cb_ptr->bytes     = 0;
   cb_ptr->read_ptr  = cb_ptr->data;
   cb_ptr->write_ptr = cb_ptr->data;
}

//----------------------------------------------------------------------------
//  uart_circ_buffer_log_state
//----------------------------------------------------------------------------

void
uart_circ_buffer_log_state( uart_circular_buffer_type* cb_ptr )
{
   if( cb_ptr == NULL )
      return;

   UART_LOG_0( TRACE, " circular buffer state:");
   UART_LOG_2( TRACE, "   %d / %d bytes", cb_ptr->bytes, cb_ptr->size );
   UART_LOG_2( TRACE, "   start = 0x%08x, end   = 0x%08x", (uint32) cb_ptr->data,     (uint32) cb_ptr->end_ptr );
   UART_LOG_2( TRACE, "   read  = 0x%08x, write = 0x%08x", (uint32) cb_ptr->read_ptr, (uint32) cb_ptr->write_ptr );
}

