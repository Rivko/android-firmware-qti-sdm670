/*=============================================================================

                                 boot logger jtag

GENERAL DESCRIPTION
  This file contains the function interface for outputting to a JTAG
  terminal window.

Copyright  2010, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  

when       who        what, where, why
--------   ---        --------------------------------------------------------- 
08/20/14   kedara     Fix compilation warnings.
08/11/10   dh         Initial port from 7x30 
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_logger_jtag.h"

/*============================================================================ 
                          PUBLIC FUNCTION DECLARATIONS
============================================================================*/

extern void boot_jtag_term_putc(char);

/*===========================================================================

**  Function :  boot_log_jtag_dump_buffer

** ==========================================================================
*/
/*!
* 
* @brief
* Takes a pointer to a buffer and a buffer length.  It will output whatever
* is in the buffer to a JTAG terminal window, interpreting each byte as a char.
* This function will timeout, not outputting any character, if a JTAGE terminal
* window is not open.
*
* @param[in] buffer  Address of the buffer
*
* @param[in] length  length of the buffer in bytes
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_jtag_dump_buffer(void *buffer, uint32 length)
{
  uint32 i;
  char *character;
  character = (char *) buffer;

  /*Output "length" amount of characters, starting at the buffer ptr*/
  for(i=0; i < length; i++) 
  {
    /*If null pointer, break and return*/
    if(NULL == character) 
    {
      break;
    }

    /*Print out char, skipping null char's*/
    if(0 != *character) 
    {
      boot_log_jtag_term_putc(*character);
    }
    character++;
  }
}
/*boot_log_jtag_dump_buffer*/


/*===========================================================================

**  Function :  boot_log_jtag_term_putc

** ==========================================================================
*/
/*!
* 
* @brief
* Pushes a single character to the JTAG terminal window.
* It checks to make sure the previous character has been read by the terminal.
* If no JTAG terminal is open then it will skip any remaining output.  
* If it timeouts several times then it will never attempt
* to write out to JTAG again.
* This function only works on ARM7
*
* @param[in] c single character to display.
*
* @par Dependencies
*   None
*   
* @retval
*   None
*
* @par Side Effects
*   None
* 
*/
void boot_log_jtag_term_putc(char c)
{
  boot_jtag_term_putc(c); 
}
/*boot_log_jtag_term_putc*/
