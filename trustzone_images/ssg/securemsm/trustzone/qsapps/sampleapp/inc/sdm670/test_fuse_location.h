#ifndef __SAMPLEAPP_TEST_FUSE_LOCATION_H
#define __SAMPLEAPP_TEST_FUSE_LOCATION_H

/**
@file test_fuse_location.h
@brief Define the test fuse location for sampleapp tests.  
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
08/15/13   nbeier      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*We leave the test disabled since blowing fuses is irreversible.  
  Change this flag to enable the test. */
#define FUSE_BLOWING_TEST_ENABLED 0


/*This is the LSB address corresponding to a test region 
  QFPROM_RAW_SPARE_REG19_LSB : 0x00058000 + 0x000001F8
  (Note: must use LSB address when acccessing fuses)*/

/* Needs updating before enabling the test:
#define TEST_FUSE_REGION_LSB_ADDR 0x581F8 
#define TEST_FUSE_IS_MSB 0
*/

  
#endif
