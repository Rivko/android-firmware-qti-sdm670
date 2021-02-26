#ifndef INCLUDE_FLAG_SIORS232TEST_H
#define INCLUDE_FLAG_SIORS232TEST_H

/*===========================================================================

                       S I O R S 2 3 2 T E S T  . H
DESCRIPTION
  This file contains the functionality for echo-packet testing
  This file also contain the implementation of SIO trace capability
   
  
Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/07   rh      Move the UART logging feature here
05/01/06   rh      File creation - Adapted from USBTEST.h
09/27/06   rh      Add linear and AA/55 RX only test method
                               
===========================================================================*/

/*===========================================================================
  
	    PART 1 - SIO ECHO TEST FUNCTIONS
   
===========================================================================*/


/*===========================================================================
    Prototype for externalized functions
===========================================================================*/

/*===========================================================================

FUNCTION siors232test_echo_close                        EXTERNALIZED FUNCTION

DESCRIPTION
  Stop echo test process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_close(void);
/*~ FUNCTION siors232test_echo_close */

/*===========================================================================
 
FUNCTION siors232test_echo_init                         EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_init(void);
/*~ FUNCTION siors232test_echo_init */

/*===========================================================================
 
FUNCTION siors232test_rx_check_init                   EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host
  
PARAMETERS
  Two kind of test are defined, test_type = 1 for linear increment test,
  otherwise AA/55 test is used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/
void siors232test_rx_check_init(uint32 test_type); 

/*===========================================================================

FUNCTION siors232test_command                        EXTERNALIZED FUNCTION

DESCRIPTION
  Test different IOCTL commands

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void siors232test_command(char cmd);


#endif /* INCLUDE_FLAG_SIORS232TEST_H */
