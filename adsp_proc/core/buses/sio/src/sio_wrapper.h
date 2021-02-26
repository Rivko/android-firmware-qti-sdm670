#ifndef SIO_WRAPPER_H
#define SIO_WRAPPER_H
/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                  W R A P P E R    F U N C T I O N S

                       H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007-2009,2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   amv     created sio_wrapper.h for Qube

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"
#include "sio_priv.h"
#include "amssassert.h"

/*==========================================================================

                         DEFINE FEATURES HERE

============================================================================*/

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/

/* DSM payload type for Unescaped bytes for Async data */
#if defined(FEATURE_DATA_MM) || defined(FEATURE_GSM_PLT) || defined(FEATURE_WCDMA_PLT) || defined (FEATURE_CORE_SERVICE_ONLY)
#define SIO_PAYLOAD       DSM_PS_RAW_APP_PAYLOAD
#else 
#define SIO_PAYLOAD       PS_RAW_APP_PAYLOAD
#endif


/*===========================================================================

FUNCTION sio_enter_crit_section 

DESCRIPTION
  sio_enter_crit_section
      
DEPENDENCIES
  sio_init_crit_section() needs to be called first
      
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_enter_crit_section(void);

/*===========================================================================

FUNCTION sio_leave_crit_section 

DESCRIPTION
  sio_leave_crit_section
      
DEPENDENCIES
  sio_init_crit_section() needs to be called first
      
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_leave_crit_section(void);

#endif  /* SIO_WRAPPER_H */
