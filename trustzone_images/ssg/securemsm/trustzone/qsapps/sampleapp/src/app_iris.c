/*
  @file app_iris.c
  @brief Iris unit test.

 */
/*===========================================================================
  Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_iris.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------
  # 05/01/17   yb      Added Iris unit test
  ===========================================================================*/
#include <comdef.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "qsee_log.h"
#include "qsee_timer.h"
#include "sp_iris.h"
#include "iris_dummy_templates.h"


#ifdef INCLUDE_IPASS_TEST
#include "iris_templates_ipass.h"
#endif


#define TEMPLATE_SIZE 2560 //2.5K
#define TWO_TEMPLATES_SIZE (TEMPLATE_SIZE*2) //5K

/*-------------------------------------------------------------------------
 * Structures and enums
 * ----------------------------------------------------------------------*/
typedef enum {
   IRIS_ENROLL_TEST = 0, //no erase
   IRIS_MATCH_TEST = 1, //no erase
   IRIS_ERASE_TEST = 2, //just erase
   IRIS_ERASE_ALL_TEST = 3, //5 enroll + erase_all
   IRIS_NO_INIT_TEST = 4, //no enroll
   IRIS_INVALID_PARAM_TEST = 5, //no enroll
   IRIS_ENROLL_ERASE_TEST = 6, //enroll + erase
   IRIS_REENROLL_TEST = 7, //enroll+erase
   IRIS_GET_IDS_TEST = 8, //enroll+erase
   IRIS_CONTEXT_TEST = 9, //enroll+erase
   #ifdef INCLUDE_IPASS_TEST
   IRIS_IPASS_TEST = 10, //ipass match function
   #endif
} iris_test_id_e;


#pragma pack(push, 1) // exact fit - no padding

typedef struct iris_test_cmd {
   uint32_t cmd_id;
   uint32_t test_id;
   uint32_t len;
   uint64_t addr;
} iris_test_cmd_t;

typedef struct iris_test_rsp {
   uint8_t ret;
} iris_test_rsp_t;

#pragma pack(pop)


/*-------------------------------------------------------------------------
 * Internal functions
 * ----------------------------------------------------------------------*/
/**
 * @brief: The test enrolls templates pairs(dummy) of 6 ids
 */
int run_enroll_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;

   ret = sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;
   }

   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_SUCCESS) {
      if(ret == SP_IRIS_VERSION_UNKNOWN) {
            ret = sp_iris_set_version(EYELOCK_ALGO, version);
            if(ret != SP_IRIS_SUCCESS) {
                  qsee_log(QSEE_LOG_MSG_ERROR, "Fail to set the version");
                  return -1;
            }
      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
         return -1;
      }

   }


   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
            1, i);
      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         return -1;

      }
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
   }

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }

   return 0;
}
/**
* @brief: The test matches dummy sample template with each of the enrolled templates
*                  with eyelock match algorithm
*/
int run_match_test(uint64_t addr, uint32_t len) {
  sp_iris_result_e ret;
  uint32_t score1, score2;
  uint8_t tamplate_buf[TEMPLATE_SIZE];
  iris_algo_version_t version;

  ret = sp_iris_init(addr, len);
  if(ret != SP_IRIS_SUCCESS ){
     qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
     return -1;

  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

  ret = sp_iris_get_version(&version);
  if(ret != SP_IRIS_SUCCESS) {
     qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
     return -1;
  }

  if(version.major != 1 || version.minor != 0) {
     qsee_log(QSEE_LOG_MSG_ERROR, "Invalid eyelock version");
     return -1;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Get version return with success\n");


  memcpy(tamplate_buf, verify_template_L_id1, verify_template_L_id1_len);

  for(int i = 0; i < MAX_IDS_NUM; i++) {

     ret = sp_iris_match_template(tamplate_buf, verify_template_L_id1_len,
           1, i,
           &score1, &score2);
     if(ret != SP_IRIS_SUCCESS) {
        sp_iris_exit();
        qsee_log(QSEE_LOG_MSG_ERROR, "Match fail \n", ret);
        return -1;

     }

     qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id %d score1 %d score2 %d\n", i, score1, score2);
  }

  ret =  sp_iris_exit();

  if(ret != SP_IRIS_SUCCESS ) {
     qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
     return -1;

  }

  return 0;

}

/**
 * @brief: The test enrolls templates pairs(dummy) of 6 ids and check the get_enrolled returns
 *  all the enrolled ids
 */
int run_get_enrolled_id_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t  fail_count = 0;
   uint32_t num_ids = 0;
   uint32_t ids[MAX_IDS_NUM];
   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;

   ret = sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;
   }

   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_SUCCESS) {
      if(ret == SP_IRIS_VERSION_UNKNOWN) {
            ret = sp_iris_set_version(EYELOCK_ALGO, version);
            if(ret != SP_IRIS_SUCCESS) {
                  qsee_log(QSEE_LOG_MSG_ERROR, "Fail to set the version");
                  return -1;
            }
      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
         return -1;
      }

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
            1, i);
      if(ret != SP_IRIS_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         fail_count++;
         break;

      }
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
   }

   ret =  sp_iris_get_enrolled_ids(1, &num_ids, ids);
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the enrolled ids\n");
      fail_count++;


   }

   if(num_ids!= MAX_IDS_NUM) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get all the enrolled ids id_num %d\n", num_ids);
      fail_count++;


   } else {

      qsee_log(QSEE_LOG_MSG_ERROR, "Get all the enrolled ids\n");
   }

   //Erase all the enrolled id from the enrolled array
   for(int i = 0; i < num_ids; i++) {
      ret = sp_iris_erase_templates(1, ids[i]);
      if(ret != SP_IRIS_SUCCESS) {
         fail_count++;
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase templates for id %d\n", ids[i]);

      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "Erase iris sp app return with success for id %d\n", ids[i]);
      }
   }


   //Check that no enrolled id
   ret =  sp_iris_get_enrolled_ids(1, &num_ids, ids);
   if(ret != SP_IRIS_SUCCESS) {
      fail_count++;
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the enrolled ids\n");

   }

   if(num_ids != 0) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase all the enrolled ids num_ids %d\n", num_ids);
      fail_count++;

   } else {
      qsee_log(QSEE_LOG_MSG_ERROR, "Erase all the enrolled id\n");

   }


   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      fail_count++;

   }


   return       fail_count? -1: 0;
}

/**
 * @brief: The test enrolls different templates pairs number to each context
 * and check that we get different number of ids to each context
 *  all the enrolled ids
 */
int run_context_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t  fail_count = 0;
   uint32_t num_ids = 0;
   uint32_t ids[MAX_IDS_NUM];
   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;
   int i = 0, j = 0;

   ret = sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;
   }

   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_SUCCESS) {
      if(ret == SP_IRIS_VERSION_UNKNOWN) {
            ret = sp_iris_set_version(EYELOCK_ALGO, version);
            if(ret != SP_IRIS_SUCCESS) {
                  qsee_log(QSEE_LOG_MSG_ERROR, "Fail to set the version");
                  return -1;
            }
      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
         return -1;
      }

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(j = 0; j < MAX_CONTEXT_NUM; j++) {
      for(i = 0; i < (MAX_IDS_NUM -j ); i++) {
         ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
               j, i);
         if(ret != SP_IRIS_SUCCESS) {
            qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d context %d\n", i, j);
            fail_count++;
            break;

         }
         qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d context %d\n", i, j);
      }
   }

   for(j = 0; j < MAX_CONTEXT_NUM; j++) {

      ret =  sp_iris_get_enrolled_ids(j, &num_ids, ids);
      if(ret != SP_IRIS_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the enrolled ids for context %d\n", j);
         fail_count++;


      }

      if(num_ids!= (MAX_IDS_NUM - j)) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get all the enrolled ids id_num %d context %d\n", num_ids, j);
         fail_count++;


      } else {

         qsee_log(QSEE_LOG_MSG_ERROR, "Get all the enrolled ids from context %d\n", j);
      }

      //Erase all the enrolled id from the enrolled array
      for(i = 0; i < num_ids; i++) {
         ret = sp_iris_erase_templates(j, ids[i]);
         if(ret != SP_IRIS_SUCCESS) {
            fail_count++;
            qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase templates for id %d context %d\n", ids[i], j);

         } else {
            qsee_log(QSEE_LOG_MSG_DEBUG, "Erase iris sp app return with success for id %d context %d\n", ids[i], j);
         }
      }

      //Check that no enrolled id
      ret =  sp_iris_get_enrolled_ids(j, &num_ids, ids);
      if(ret != SP_IRIS_SUCCESS) {
         fail_count++;
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the enrolled ids for context %d\n", i, j);

      }

      if(num_ids != 0) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase all the enrolled ids num_ids %d context %d\n", num_ids, j);
         fail_count++;

      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Erase all the enrolled id from context %d\n", j);

      }
   }

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      fail_count++;

   }


   return fail_count? -1: 0;
}



/**
 * @brief: The test erases all the enrolled templates of the 6 ids
 */
int run_erase_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplate_buf[TEMPLATE_SIZE];
   uint32_t score1, score2;
   iris_algo_version_t version;
   int fail_count = 0;

   ret = sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init return with success\n");


   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
      return -1;
   }

   if(version.major != 1 || version.minor != 0) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Invalid eyelock version");
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Get version return with success\n");

   for(int i = 0; i< MAX_IDS_NUM; i++) {
      ret = sp_iris_erase_templates(1, i);
      if(ret != SP_IRIS_SUCCESS){
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id %d\n", i);
         fail_count++;

      } else {

         qsee_log(QSEE_LOG_MSG_DEBUG, "erase return with success for id %d", i);
      }
   }

   for(int i = 0; i < MAX_IDS_NUM; i++) {

      ret = sp_iris_match_template(tamplate_buf, verify_template_L_id1_len,
            1, i,
            &score1, &score2);
      if(ret ==  SP_IRIS_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Erase is valid after existence check for id %d\n", i);
         fail_count++;
         break;
      } else if(ret != SP_IRIS_ID_NOT_EXIST) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Got diff fail from id not exist ret %d\n", ret);
         fail_count++;
         break;
      }

      qsee_log(QSEE_LOG_MSG_ERROR, "Erase is valid for id %d\n", i);
   }

   ret =  sp_iris_exit();
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      fail_count++;

   }

   if(fail_count > 0) {
      return -1;
   }

   return 0;

}


/**
 * @brief: The test erases all the enrolled templates of the 6 ids
 */
int run_erase_all_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t score1, score2;
   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;
   int fail_count = 0;

   ret = sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }

   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_SUCCESS) {
      if(ret == SP_IRIS_VERSION_UNKNOWN) {
            ret = sp_iris_set_version(EYELOCK_ALGO, version);
            if(ret != SP_IRIS_SUCCESS) {
                  qsee_log(QSEE_LOG_MSG_ERROR, "Fail to set the version");
                  return -1;
            }
      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
         return -1;
      }

   }


   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
            1, i);
      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         return -1;

      }
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
   }

   for(int i = 0; i < MAX_IDS_NUM; i++) {

         ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
               1, i,
               &score1, &score2);

         if(ret != SP_IRIS_SUCCESS) {
            qsee_log(QSEE_LOG_MSG_ERROR, "fail to match %d\n", ret);
            fail_count++;
            break;
         }

         qsee_log(QSEE_LOG_MSG_ERROR, "Erase is valid for id %d\n", i);
     }



   ret = sp_iris_erase_all_templates();
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase all");
      return -1;
   }

   for(int i = 0; i < MAX_IDS_NUM; i++) {

         ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
               1, i,
               &score1, &score2);

         if(ret != SP_IRIS_ID_NOT_EXIST) {
            qsee_log(QSEE_LOG_MSG_ERROR, "Match ret after erase %d\n", ret);
            fail_count++;
            break;
         }

         qsee_log(QSEE_LOG_MSG_ERROR, "Erase is valid for id %d\n", i);
     }


   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }
   return fail_count? -1: 0;

}

/**
 * @brief: The test checks all the invalid cases of the 5 APIs
 */
int run_invalid_param_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t score1, score2;

   ret =sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   ret = sp_iris_enroll_id_templates(NULL, TWO_TEMPLATES_SIZE,
         1, 0);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_enroll_id_templates(tamplates_buf, 0,
         1, 0);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_enroll_id_templates(tamplates_buf, ((len + 4095) & (~4095))+10,
        1,  0);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
         INVALID_ID_NUM, 0);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
         1, INVALID_CONTEXT_NUM);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris return the expected error\n");

   memcpy(tamplates_buf, verify_template_L_id1, verify_template_L_id1_len);

   ret = sp_iris_match_template(0, verify_template_L_id1_len,
         1, 0,
         &score1, &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, 0,
         1, 0,
         NULL , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, ((len + 4095) & (~4095))+10,
         1, 0,
         NULL , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, 0,
         NULL , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, 0,
         &score1 , NULL);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, 0,
         1, 0,
         &score1 , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, (0XFFFFFFFF),
         &score1 , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         (0XFFFFFFFF), 0,
         &score1 , &score2);
   if(ret != SP_IRIS_INVALID_PARAM ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Match return the expected error\n");

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }

   ret = sp_iris_init(addr, 0);
   if(ret != SP_IRIS_INVALID_PARAM ){
      qsee_log(QSEE_LOG_MSG_ERROR, "Init iris returned %d\n", ret);
      return -1;

   }

   ret = sp_iris_init(0, len);
   if(ret != SP_IRIS_INVALID_PARAM ){
      qsee_log(QSEE_LOG_MSG_ERROR, "Init iris returned %d\n", ret);
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init return the expected error\n");


   return 0;
}


/**
 * @brief: The test tries to call enroll/match/erase/exit without init command
 */
int run_no_init_test() {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t score1, score2;


   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
         1, 0);
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll templates returned %d\n", ret);
      return -1;

   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris return the expected error\n");

   memcpy(tamplates_buf, verify_template_L_id1, verify_template_L_id1_len);

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, 0,
         &score1, &score2);
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Match template returned %d\n", ret);
      return -1;

   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "Match return the expected error\n");

   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;

   ret = sp_iris_set_version(IPASS_ALGO, version);
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Set version returned %d\n", ret);
     return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Set version return the expected error\n");

   ret = sp_iris_get_version(&version);
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Get version returned %d\n", ret);
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Get version return the expected error\n");

   uint32_t erase_id = (qsee_get_uptime())%MAX_IDS_NUM;
   ret = sp_iris_erase_templates(1, erase_id);
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Erase templates returned %d\n", ret);
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Erase templates return the expected error\n");

   ret = sp_iris_erase_all_templates();
   if(ret != SP_IRIS_NOT_INITIALIZED ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Erase all templates returned %d\n", ret);
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Erase all templates return the expected error\n");

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }

   return 0;
}

/**
 * @brief: The test enroll templates pairs of 6 ids,
*                 match them with a given template and erase one templates pair
*                 of random id and try do the match again
 */
int run_enroll_erase_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32_t score1, score2;
   uint32_t erase_id;
   uint32_t fail_count = 0;

   ret =sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
            1, i);
      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         return -1;

      }
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
   }

   memcpy(tamplates_buf, verify_template_L_id1, verify_template_L_id1_len);

   for(int i = 0; i < MAX_IDS_NUM; i++) {

      ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
            1, i,
            &score1, &score2);
      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", i);
         return -1;

      }

      qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id %d score1 %d score2 %d\n", i, score1, score2);

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris return the expected error  SP_IRIS_MAX_ENROLLED_IDS for id MAX_IDS_NUM\n");

   erase_id = (qsee_get_uptime())%MAX_IDS_NUM;
   ret = sp_iris_erase_templates(1, erase_id);
   if(ret != SP_IRIS_SUCCESS){
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id %d\n", erase_id);

   } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "erase return with success for id %d",erase_id);
   }

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
            1, i,
            &score1, &score2);
      if(i == erase_id) {
         if(ret != SP_IRIS_ID_NOT_EXIST) {
            qsee_log(QSEE_LOG_MSG_ERROR, "Match return unexpected error for id %d ret %d\n",erase_id, ret);
            sp_iris_exit();
            return -1;

         } else {
            qsee_log(QSEE_LOG_MSG_DEBUG, "Match returned the expected error for id %d\n",erase_id);
         }

      } else if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", i);
         return -1;

      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id %d score1 %d score2 %d\n", i, score1, score2);
      }

   }

   ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
         1, MAX_IDS_NUM);
   if(ret != SP_IRIS_SUCCESS) {
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "Enroll fail for id MAX_IDS_NUM ret = %d\n", ret);
      return -1;

   }

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, MAX_IDS_NUM,
         &score1, &score2);
   if(ret != SP_IRIS_SUCCESS ){
      fail_count++;
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id MAX_IDS_NUM\n");

   } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id MAX_IDS_NUM score1 %d score2 %d\n", score1, score2);
   }


   for(int i = 0; i< MAX_IDS_NUM; i++) {
      ret = sp_iris_erase_templates(1, i);
      if(i == erase_id) {
         if(ret != SP_IRIS_ID_NOT_EXIST) {
            qsee_log(QSEE_LOG_MSG_ERROR, "erase return unexpected error %d != SP_IRIS_ID_NOT_EXIST\n", ret);
            fail_count++;

         } else {
            qsee_log(QSEE_LOG_MSG_DEBUG, "erase return the expected error SP_IRIS_ID_NOT_EXIST\n");
         }

      } else if(ret != SP_IRIS_SUCCESS){
         fail_count++;
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id %d\n", i);

      } else {

         qsee_log(QSEE_LOG_MSG_DEBUG, "erase return with success for id %d", i);
      }
   }

   ret = sp_iris_erase_templates(1, MAX_IDS_NUM);
   if(ret != SP_IRIS_SUCCESS){
      fail_count++;
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id MAX_IDS_NUM\n");

   }

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS ) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      fail_count++;

   }

   if(fail_count > 0) {
      return -1;
   }

   return 0;
}

/**
 * @brief: The test enrolls templates pairs of 6 ids and changes the templates of random id
*                  The match score should be different  after the enrollment
 */
int run_reenroll_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[TWO_TEMPLATES_SIZE];
   uint32 reenroll_id = (qsee_get_uptime())%MAX_IDS_NUM;
   uint32_t score1, score2;
   uint32_t score3, score4;

   ret =sp_iris_init(addr, len);
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   memset(tamplates_buf, 0 , TWO_TEMPLATES_SIZE);
   memcpy(tamplates_buf, enroll_template_L_id1, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id1, TEMPLATE_SIZE);

   for(int i = 0; i < MAX_IDS_NUM; i++) {
      ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
            1, i);
      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         return -1;

      }
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
   }

   memcpy(tamplates_buf, verify_template_L_id1, verify_template_L_id1_len);


   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, reenroll_id,
         &score1, &score2);
   if(ret != SP_IRIS_SUCCESS) {
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", reenroll_id);
      return -1;

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id %d score1 %d score2 %d\n", reenroll_id, score1, score2);

   memcpy(tamplates_buf, enroll_template_L_id2, TEMPLATE_SIZE);
   memcpy(tamplates_buf + TEMPLATE_SIZE, enroll_template_R_id2, TEMPLATE_SIZE);

   ret = sp_iris_enroll_id_templates(tamplates_buf, TWO_TEMPLATES_SIZE,
         1, reenroll_id);
   if(ret != SP_IRIS_SUCCESS) {
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", reenroll_id);
      return -1;

   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris SP app return with success for id %d\n", reenroll_id);

   memcpy(tamplates_buf, verify_template_L_id1, verify_template_L_id1_len);

   ret = sp_iris_match_template(tamplates_buf, verify_template_L_id1_len,
         1, reenroll_id,
         &score3, &score4);
   if(ret != SP_IRIS_SUCCESS) {
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", reenroll_id);
      return -1;

   }

   if((score1 == score3) || (score4 == score2)) {
      sp_iris_exit();
      qsee_log(QSEE_LOG_MSG_ERROR, "The match result should be different after re-enroll\n");
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Match return with success for id %d score1 %d score2 %d\n", reenroll_id, score3, score4);

   for(int i = 0; i< MAX_IDS_NUM; i++) {
      ret = sp_iris_erase_templates(1, i);
      if(ret != SP_IRIS_SUCCESS){
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id %d\n", i);

      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "erase return with success for id %d", i);
      }
   }

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }

   return 0;
}

#ifdef INCLUDE_IPASS_TEST

----------------- IPASS testing------------------------------
#pragma pack(push, 1) // exact fit - no padding

typedef struct _iris_template_i {
    uint32_t which_eye;
    int32_t cols;
    int32_t rows;
    int32_t bits;
    int32_t valid_ratio;
    int32_t quality;
    uint32_t *data;
    uint32_t *mask;
} iris_template_i;

typedef struct _iris_fused_template_i {
    uint32_t valid;
    uint32_t which_eye;
    int32_t cols;
    int32_t rows;
    int32_t bits;
    int32_t fusion_size;
    int32_t num_of_bit_planes;
    int32_t energy_scale;
    int32_t valid_ratio;
    int32_t *bit_plane_weights;
    uint32_t *data;
    uint32_t *mask;
    uint32_t *confidence;
} iris_fused_template_i;

#define FLOAT_INT_SCALE   10000


typedef struct _iris_template {
    uint32_t which_eye;
    int32_t rows;
    int32_t cols;
    int32_t bits;
    float valid_ratio;
    float quality;
    uint32_t *data;
    uint32_t *mask;
} iris_template;

typedef struct _iris_fused_template {
   uint32_t valid;
    uint32_t which_eye;
    int32_t rows;
    int32_t cols;
    int32_t bits;
    int32_t fusion_size;
    int32_t num_of_bit_planes;
    float energy_scale;
    float valid_ratio;
    float *bit_plane_weights;
    uint32_t *data;
    uint32_t *mask;
    uint32_t **confidence;
} iris_fused_template;

#pragma pack(pop)

static void convert_float_match_template(uint8_t* match_template) {
   iris_template* template = (iris_template*)match_template;
   iris_template_i* template_int = (iris_template_i*)match_template;
   template_int->valid_ratio = (int)(template->valid_ratio*FLOAT_INT_SCALE);
   template_int->quality = (int)(template->quality*FLOAT_INT_SCALE);
}

static void convert_float_store_template(uint8_t* store_template) {
   iris_fused_template* template = (iris_fused_template*)store_template;
   iris_fused_template_i* template_int = (iris_fused_template_i*)store_template;

   uint32_t header_size = (uint32_t)(((uint8_t*)&template->bit_plane_weights) - ((uint8_t*)store_template));

   template_int->energy_scale = (int)(template->energy_scale*FLOAT_INT_SCALE);
   template_int->valid_ratio = (int)(template->valid_ratio*FLOAT_INT_SCALE);

   for(int i=0; i < template->num_of_bit_planes; i++) {
      float weight_f = *((float*)(store_template + header_size + i*sizeof(float)));
      uint32_t weight = (int)(weight_f*FLOAT_INT_SCALE);
      memcpy((store_template +header_size+ sizeof(int32_t)*i), (uint8_t*)&weight, sizeof(int32_t));
   }

}

int run_ipass_test(uint64_t addr, uint32_t len) {
   sp_iris_result_e ret;
   uint8_t tamplates_buf[ enrolled_L_id_1_ipass_len + enrolled_R_id_1_ipass_len];
   uint32_t score1, score2;
   uint32_t score3, score4;
   long long before, after;
   iris_algo_version_t version;
   version.major = 1;
   version.minor = 0;

   convert_float_store_template(enrolled_L_id_1_ipass);
   convert_float_store_template(enrolled_R_id_1_ipass);
   convert_float_match_template(verify_R_id_1_ipass);
   convert_float_match_template(verify_L_id_1_ipass);


   qsee_log(QSEE_LOG_MSG_ERROR, "The test should be run with empty NVM partition ");
   before = qsee_get_uptime();
   ret = sp_iris_init(addr, len);
   after = qsee_get_uptime();
   qsee_log(QSEE_LOG_MSG_DEBUG, "sp_iris_init time %d\n", (int)(after-before));
   if(ret != SP_IRIS_SUCCESS){
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to init iris sp app\n");
      return -1;

   }
  qsee_log(QSEE_LOG_MSG_DEBUG, "Init iris sp app return with success\n");

   before = qsee_get_uptime();
   ret = sp_iris_get_version(&version);
   after = qsee_get_uptime();
   qsee_log(QSEE_LOG_MSG_DEBUG, "sp_iris_get_version time %d\n", (int)(after-before));

   if(ret != SP_IRIS_SUCCESS) {
      if(ret == SP_IRIS_VERSION_UNKNOWN) {
            ret = sp_iris_set_version(IPASS_ALGO, version);
            if(ret != SP_IRIS_SUCCESS) {
                  qsee_log(QSEE_LOG_MSG_ERROR, "Fail to set the version - please delete the NVM partition ");
                  return -1;
            }
            qsee_log(QSEE_LOG_MSG_DEBUG, "Set version return with success\n");

      } else {
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to get the version");
         return -1;
      }

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "get/set version return with success\n");

   memset(tamplates_buf, 0 , enrolled_L_id_1_ipass_len + enrolled_R_id_1_ipass_len);
   memcpy(tamplates_buf, enrolled_L_id_1_ipass, enrolled_L_id_1_ipass_len);
   memcpy(tamplates_buf + enrolled_L_id_1_ipass_len, enrolled_R_id_1_ipass, enrolled_R_id_1_ipass_len);

   for(int i = 0; i < 1; i++) {

      before = qsee_get_uptime();
      ret = sp_iris_enroll_id_templates(tamplates_buf, enrolled_L_id_1_ipass_len + enrolled_R_id_1_ipass_len,
            1, i);
      after = qsee_get_uptime();

      if(ret != SP_IRIS_SUCCESS) {
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to enroll templates for id %d\n", i);
         return -1;

      }

      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll iris sp app return with success for id %d\n", i);
      qsee_log(QSEE_LOG_MSG_DEBUG, "Enroll time %d\n", (int)(after-before));

   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Match with verify_L_id_1_ipass\n");


   memcpy(tamplates_buf, verify_L_id_1_ipass, verify_L_id_1_ipass_len);

   for(int i = 0; i < 1; i++) {

      before = qsee_get_uptime();
      ret = sp_iris_match_template(tamplates_buf, verify_L_id_1_ipass_len,
            1, i,
            &score1, &score2);
      after = qsee_get_uptime();
      qsee_log(QSEE_LOG_MSG_DEBUG, "Match time %d\n", (int)(after-before));

      if(ret != SP_IRIS_SUCCESS ){
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", i);
         return -1;

      }

      qsee_log(QSEE_LOG_MSG_DEBUG, "Match verify_L_id_1 return with"
            " success for id %d score1 %d score2 %d\n", i, score1, score2);

   }

   memcpy(tamplates_buf, verify_R_id_1_ipass, verify_R_id_1_ipass_len);

      for(int i = 0; i < 1; i++) {

         before = qsee_get_uptime();
         ret = sp_iris_match_template(tamplates_buf, verify_R_id_1_ipass_len,
               1, i,
               &score1, &score2);
         after = qsee_get_uptime();

         if(ret != SP_IRIS_SUCCESS ){
            sp_iris_exit();
            qsee_log(QSEE_LOG_MSG_ERROR, "Fail to match template for id %d\n", i);
            return -1;

         }

         qsee_log(QSEE_LOG_MSG_DEBUG, "Match verify_R_id_1  return with"
               " success for id %d score1 %d score2 %d\n", i, score1, score2);
         qsee_log(QSEE_LOG_MSG_DEBUG, "Match time %d\n", (int)(after-before));

      }



   for(int i = 0; i< 1; i++) {
      before = qsee_get_uptime();
      ret = sp_iris_erase_templates(1, i);
      after = qsee_get_uptime();

      if(ret != SP_IRIS_SUCCESS){
         sp_iris_exit();
         qsee_log(QSEE_LOG_MSG_ERROR, "Fail to erase template for id %d\n", i);

      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "erase return with success for id %d", i);
         qsee_log(QSEE_LOG_MSG_DEBUG, "Erase time %d\n", (int)(after-before));

      }
   }

   ret =  sp_iris_exit();

   if(ret != SP_IRIS_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Fail to exit iris\n");
      return -1;

   }

   return 0;
}
#endif
//----------------- end of IPASS testing------------------------------
/*-------------------------------------------------------------------------
 * Global function
 * ----------------------------------------------------------------------*/
int run_iris_test(void* cmd, uint32 cmdlen,
      void* rsp, uint32 rsplen) {

   int ret = 0;
   iris_test_cmd_t* test_req = (iris_test_cmd_t*)cmd;
   iris_test_rsp_t* test_rsp = (iris_test_rsp_t*)rsp;

   if(!cmd || !rsp) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Null cmd/rsp init parameters\n");
      return -1;
   }

   if((cmdlen < sizeof(iris_test_cmd_t)) || (rsplen < sizeof(iris_test_rsp_t))) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Invalid cmd/rsp init buffer size\n");
      return -1;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "Start run_iris_test function test_id %d\n", test_req->test_id);

   switch(test_req->test_id) {
   case IRIS_ENROLL_TEST:
      ret = run_enroll_test(test_req->addr, test_req->len);
      break;
   case IRIS_MATCH_TEST:
      ret = run_match_test(test_req->addr, test_req->len);
      break;
   case IRIS_ERASE_TEST:
      ret = run_erase_test(test_req->addr, test_req->len);
      break;
   case IRIS_ERASE_ALL_TEST:
      ret = run_erase_all_test(test_req->addr, test_req->len);
      break;
   case IRIS_NO_INIT_TEST:
      ret = run_no_init_test() ;
      break;
   case IRIS_INVALID_PARAM_TEST:
      ret = run_invalid_param_test(test_req->addr, test_req->len);
      break;
   case IRIS_ENROLL_ERASE_TEST:
      ret = run_enroll_erase_test(test_req->addr, test_req->len);
      break;
   case IRIS_REENROLL_TEST:
      ret = run_reenroll_test(test_req->addr, test_req->len);
      break;
   case IRIS_GET_IDS_TEST:
      ret = run_get_enrolled_id_test(test_req->addr, test_req->len);
      break;
   case IRIS_CONTEXT_TEST:
      ret = run_context_test(test_req->addr, test_req->len);
      break;
   #ifdef INCLUDE_IPASS_TEST
   case IRIS_IPASS_TEST:
      ret = run_ipass_test(test_req->addr, test_req->len);
      break;
   #endif
   default:
      qsee_log(QSEE_LOG_MSG_ERROR, "Unsupported test id\n");
      return -1;

   };


   return ret;

}


