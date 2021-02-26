/*
  @file qsee_fuses.c
  @brief Contains test code for most of the QSEE fuse APIs.

*/
/*===========================================================================
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_fuses.c#2 $
  $DateTime: 2018/07/30 02:25:51 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------

  ===========================================================================*/
#include <stdbool.h>
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_fuse.h"
#include "qfprom.h"
#include "test_fuse_location.h"

/* Bus frequency is required when calling the qfprom APIs,
   but the value is not actually used.*/
#define BUS_FREQUENCY 0

#if FUSE_BLOWING_TEST_ENABLED

/*This function will flip the least significant unset bit
  For example, input: 000101001
  output: 000101011
*/
int flip_next_bit(uint32* input)
{
   int index = 0;
   uint32 data = *input;

   /*Find the index of the least significant unset bit*/
   while(data % 2) // "While the lest significant bit is '1'"
   {
      data = data >> 1;
      index++;
      if(index==32) // All bits were '1'
      {
         return -1;
      }
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "  flipping bit with index: %d", index);
   qsee_log(QSEE_LOG_MSG_DEBUG, "  before flip: %x", *input);
   *input = *input | (1 << index); //Flip the bit that index point to.
   //*input = (*input >> index) << index; //clear other bits
   qsee_log(QSEE_LOG_MSG_DEBUG, "  after flip: %x", *input);

   return 0;
}

/* This function will read the fuse value, increment the value by flipping a bit,
   write it back to the fuse, and read the fuse again to check if the changes were correctly written.

   Returns 0 on sucess, and -1 on failure.  All failures are logged to the qsee_log.

   Note: You must pass a LSB address to access any fuse.
   Sepecify true/false for the bool 'MSB' to indicate if you want to
   access the MSB region or the LSB region of the fuse.
*/
int blow_LSB_fuse(uint32 lsb_addr, bool MSB)
{
   uint32 qfprom_status1;
   uint32 qfprom_status2;
   uint32 qfprom_status3;

   uint64 row_data_read1;
   uint32 row_data_read1_32;
   uint64 row_data_read2;

   uint64 row_data_write;

   /*Read the fuse value*/
   qsee_fuse_read(lsb_addr, QFPROM_ADDR_SPACE_RAW, (uint32*)(uintnt)(uintnt)&row_data_read1, &qfprom_status1);
   if(qfprom_status1 != QFPROM_NO_ERR)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "Initial read returned error: %d", qfprom_status1);
      return -1;
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_DEBUG, "Initial read value: %x %x", (uint32)(row_data_read1 >> 32), (uint32)row_data_read1);
   }

   /*Extract the lower (or upper) 32 bits, then increment the that fuse value by flipping a bit*/
   if(MSB)
   {
      row_data_read1_32 = (uint32)(row_data_read1>>32);  //Grab upper 32 bits
   }
   else
   {
      row_data_read1_32 = (uint32)row_data_read1;    //Grab lower 32 bits
   }
   if( flip_next_bit(&row_data_read1_32) )
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "Flipping bits failed:  all 32 are already flipped");
      return -1;
   }

   /*Prepare write data, Write this new value back to the fuse*/
   if(MSB)
   {
      row_data_write = (uint64)row_data_read1_32 << 32;
   }
   else
   {
      row_data_write = (uint64)row_data_read1_32;
   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "After flipping a bit, we are writing:: %x %x", (uint32)(row_data_write >> 32), (uint32)row_data_write);
   qsee_fuse_write( lsb_addr, (uint32*)(uintnt)&row_data_write, BUS_FREQUENCY, &qfprom_status2);
   if(qfprom_status2 != QFPROM_NO_ERR)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "Write returned error: %d", qfprom_status2);
      return -1;
   }


   /*Now read back the fuse value and check if it has changed*/
   qsee_fuse_read(lsb_addr, QFPROM_ADDR_SPACE_RAW, (uint32*)(uintnt)&row_data_read2, &qfprom_status3);
   if(qfprom_status3 != QFPROM_NO_ERR)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "Second read returned error: %d", qfprom_status3);
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "After second read we get: %x %x", (uint32)(row_data_read2 >> 32), (uint32)row_data_read2);

   if(MSB)
   {
      if((uint32)(row_data_read2 >> 32) != (uint32)(row_data_write >> 32))
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fuse value read does not match what was written. %x != %x", (uint32)(row_data_read2 >> 32), (uint32)(row_data_write >> 32));
         return -1;
      }
   }
   else
   {
      if((uint32)(row_data_read2) != (uint32)(row_data_write))
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fuse value read does not match what was written. %x != %x",  (uint32)row_data_read2, (uint32)row_data_write);
         return -1;
      }
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Blowing and reading fuse PASSED for (%x, %x)", lsb_addr, MSB);
   return 0;
}

int run_fuse_test()
{
   /*We use the LSB address (as required), and specify if we need access to the MSB region. */
   return blow_LSB_fuse(TEST_FUSE_REGION_LSB_ADDR, TEST_FUSE_IS_MSB);
}

#else

int run_fuse_test()
{
   qsee_log(QSEE_LOG_MSG_DEBUG, "  --> Hard fuse tests commented out.");
   return 0;
   /* Test commented out.   Running the actual test will blow fuses, which is irreversible.
      please inspect the fuse location before running the test.*/
}

#endif



/*Test the software fuse APIs.   We read the fuse, blow it, then read it again.
  For test purposes we use the software fuse called QSEE_WINSECAPP_LOADED_FUSE*/
int run_sw_fuse_test()
{
   int ret;
   bool is_blown = true;

   /*Read the SW fuse*/
   ret = qsee_is_sw_fuse_blown(QSEE_WINSECAPP_LOADED_FUSE, &is_blown, sizeof(bool) );
   if(ret)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "FAILURE in qsee_is_sw_fuse_blown() while reading QSEE_WINSECAPP_LOADED_FUSE.");
   }

   /*Blow the fuse*/
   if(!is_blown)
   {
      ret = qsee_blow_sw_fuse(QSEE_WINSECAPP_LOADED_FUSE);
      if(ret)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "FAILURE in qsee_blow_sw_fuse() while blowing QSEE_WINSECAPP_LOADED_FUSE.");
      }

      ret = qsee_is_sw_fuse_blown(QSEE_WINSECAPP_LOADED_FUSE, &is_blown, sizeof(bool));
      if(ret)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "FAILURE in qsee_is_sw_fuse_blown() while reading QSEE_WINSECAPP_LOADED_FUSE.");
      }

      if(!is_blown)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "FAILURE : Blowing SW fuse then reading value indicates that the fuse was not really blown.");
         return -1;
      }
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "Software fuse already blown, cannot run SW fuse blowing test");
      return 0;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "SW Fuse test PASSED.");
   return 0;
}
