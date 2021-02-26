#ifndef RPMH_IMAGE_OS_H
#define RPMH_IMAGE_OS_H

/*===========================================================================
                             rpmh_image_os.h

DESCRIPTION:
  Any necessary os specific header information 

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

typedef struct
{
} rpmh_os_t;

extern rpmh_os_t rpmh_os;

#define RPMH_LOG_0(a)             rpmh_log_0(a) 
#define RPMH_LOG_1(a, b)          rpmh_log_1(a, b) 
#define RPMH_LOG_2(a, b, c)       rpmh_log_2(a, b, c)
#define RPMH_LOG_3(a, b, c, d)    rpmh_log_3(a, b, c, d)
#define RPMH_LOG_4(a, b, c, d, e) rpmh_log_4(a, b, c, d, e)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

#endif //RPMH_IMAGE_OS
