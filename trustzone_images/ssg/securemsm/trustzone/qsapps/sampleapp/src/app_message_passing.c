/*
  @file app_message_passing.c
  @brief This file is holds tests cases for QSEE message passinge.

*/
/*===========================================================================
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_message_passing.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------

  ===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_message.h"

extern const char *TA_APP_NAME;

int run_message_passing_test(void)
{
   int retval;

   char mod_name[QSEE_MESSAGE_APP_NAME_MAX_LEN];
   char* sometext = "Hello this is the message buffer and it is exactly 65 bytes long."; //Null character makes it 66
   int sometext_len = 65+1;

   void* message;
   uint32_t message_len;

   void* encapsulated;
   uint32_t encapsulated_len;

   void* plaintext;
   uint32_t plaintext_len;

   message_len = sometext_len;
   encapsulated_len = sometext_len+160; //Allow room for header and mac.
   plaintext_len = sometext_len+160;

   message = qsee_malloc(message_len);
   encapsulated = qsee_malloc(encapsulated_len);
   plaintext = qsee_malloc(plaintext_len);

   if(message && encapsulated && plaintext)
   {
      strlcpy(message, sometext, sometext_len);
      memset(encapsulated, 0, encapsulated_len);
      memset(plaintext, 2, plaintext_len);

      qsee_log(QSEE_LOG_MSG_DEBUG, "Calling qsee_encapsulate_inter_app_message()");
      retval = qsee_encapsulate_inter_app_message((char *)TA_APP_NAME, message, message_len, encapsulated, &encapsulated_len);
      if(retval)
      {
         qsee_free(message);
         qsee_free(encapsulated);
         qsee_free(plaintext);

         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_encapsulate_inter_app_message() FAILED : %x", retval);

         return retval;
      }

      qsee_log(QSEE_LOG_MSG_DEBUG, "Calling qsee_decapsulate_inter_app_message()");
      retval = qsee_decapsulate_inter_app_message(mod_name, encapsulated, encapsulated_len, plaintext, &plaintext_len);
      if(retval)
      {
         qsee_free(message);
         qsee_free(encapsulated);
         qsee_free(plaintext);

         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_decapsulate_inter_app_message() FAILED : %x", retval);

         return retval;
      }

      qsee_free(message);
      qsee_free(encapsulated);
      qsee_free(plaintext);

      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee message passing tests PASSED");

      return 0;
   }
   else
   {
      // free memory if allocation was partial
      if(message)
         qsee_free(message);
      if(encapsulated)
         qsee_free(encapsulated);
      if(plaintext)
         qsee_free(plaintext);

      qsee_log(QSEE_LOG_MSG_ERROR, "qsee malloc fail - message passing FAILED");
      return -1;
   }

}
