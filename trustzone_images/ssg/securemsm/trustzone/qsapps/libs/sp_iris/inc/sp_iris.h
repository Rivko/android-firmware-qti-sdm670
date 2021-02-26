/*===========================================================================
 ** @file sp_iris.h
 *  @brief
 *  This file exposes TZ interface to SP iris application
 *
 *
 *  Copyright (c) 2017 Qualcomm Technologies Incorporated.
 *  All Rights Reserved.
 ** Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/17   yb     Initial Version

===========================================================================*/
#ifndef SP_IRIS_H_
#define SP_IRIS_H_

#include <stdint.h>

#define MAX_IDS_NUM        6
#define MAX_CONTEXT_NUM    1 //Will be changed if we will have use-case
#define INVALID_ID_NUM (0XFFFFFFFF)
#define INVALID_CONTEXT_NUM (0XFFFFFFFF)



typedef enum {
   UNSUPPORTED_ALGO = 0,
   EYELOCK_ALGO = 1,
   IPASS_ALGO = 2,
} sp_iris_algo_e;

typedef enum {
   SP_IRIS_SUCCESS = 0,
   SP_IRIS_INVALID_PARAM = -1,
   SP_IRIS_NOT_INITIALIZED = -2,
   SP_IRIS_ALREADY_INITIALIZED = -3,
   SP_IRIS_VERSION_UNKNOWN = -4,
   SP_IRIS_VERSION_INVALID = -5,
   SP_IRIS_ALGO_MISMATCH = -6,
   SP_IRIS_VERSION_MISMATCH = -7,
   SP_IRIS_ID_NOT_EXIST = -8,
   SP_IRIS_MAX_ENROLLED_IDS = -9,
   SP_IRIS_GENERAL_FAILURE = -10,
   SP_IRIS_SSR_ERROR = -11, //If you get this error you exit TZ + return to TZ and call sp_iris_init
   SP_IRIS_CRITICAL_ERROR = -12,
} sp_iris_result_e;

#pragma pack(push, 1) // exact fit - no padding

typedef struct {
   uint8_t major;
   uint8_t minor;
} iris_algo_version_t;

#pragma pack(pop)

/**
 *   The function initiates TZ-SP connection channel
 *   and send the address of the shared (TZ-SP) buffer
 *   to the SP iris application.
 *
 *   @param phyAddr      The address of the shared TZ-SP buffer
 *   @param len          The length of the shared buffer size
 *                       len >= (2 enrolled templates size)
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_init(uint64_t phyAddr, uint32_t len);

/**
 *   The function returns the current version that was set with given algorithm
 *   If no version was set - SP_IRIS_VERSION_UNKNOWN error is returned
 *
 *   @param out_version   output parameter of version type
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_get_version(iris_algo_version_t* out_version);

/**
 *   The function set/update version of given algorithm
 *   If the version was already set:
 *    1. no enrolled id exist - update the version
 *    2. at least 1 id exit - return SP_IRIS_VERSION_MISMATCH
 *
 *   @param algo      The requested algorithm
 *   @param version   The requested version
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_set_version(sp_iris_algo_e algo, iris_algo_version_t version);

/**
 *   The function gets template eyes pair(must get 2 eyes templates) of a given ID,
 *   and store it in the SP.
 *   10K is the maximum for 1 template eye.
 *   (templatesBuffLen == 2 templates lengths) <= 20K
 *
 *   The function gets contexId with the enrolledId, so the same enrolledId can be enroll
 *   to multiple contexId databases.
 *   That is for applying separation between context database.
 *
 *   Maximum valid enrolled ids is MAX_IDS_NUM per context_id
 *   Maximum valid context ids is MAX_CONTEXT_NUM

 *
 *   @param templateBuff         Internal buffer that holds the enrolled eye templates
 *   @param templateBuffLen      The size of the given templates buffer
 *   @param contexId             Add the given id to contexId database (=! INVALID_CONTEXT_NUM)
 *   @param enrolledId           The id of the given templates (=! INVALID_ID_NUM)
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_enroll_id_templates(uint8_t *templatesBuff, uint32_t templatesBuffLen,
      uint32_t contexId, uint32_t enrolledId);

/**
 *   The function gets the id of the requested database(context), and returns all the ids
 *   that are associated(enrolled) with the given context id
 *
 *   The function returns the number of the existed IDs with the same context,
 *   and sets the given array with the existed IDs(the first out_idsNum in this array).
 *
 *
 *   @param templateBuff         Internal buffer that holds the enrolled eye templates
 *   @param templateBuffLen      The size of the given templates buffer
 *   @param contexId             The id of the requested IDs database
 *   @param out_ids              output param - array with all existed IDs of the same context in size MAX_IDS_NUM
 *   @param out_idNum            output param - the NUM of the existed IDs of the same context < MAX_IDS_NUM

 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_get_enrolled_ids(uint32_t contexId, uint32_t* out_idsNum, uint32_t out_ids[MAX_IDS_NUM]);


/**
 *   The function gets one eye template of the given ID and match it to
 *   the enrolled eyes pair of the same ID that is associated with contexId.
 *   The function returns the distance scores of the given eye template from the
 *   stored first eye template and from the stored second eye template (score1, score 2).
 *
 *   @param templateBuff         Internal buffer that holds the match eye template
 *   @param templateBuffLen      The size of the given template buffer
 *   @param contexId             The id of the requested IDs database
 *   @param enrolledId           The id of the given eye template
 *   @param score1 (output)      The distance of the given template from the enrolled first template
 *   @param score2 (output)      The distance of the given template from the enrolled second template
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_match_template(uint8_t *templateBuff, uint32_t templateBuffLen,
                     uint32_t contexId, uint32_t enrolledId,
						   uint32_t *score1, uint32_t *score2);

/**
 *   The function erase the enrolled template for a given id from the contextID database.
 *
 *   @param enrolledId           The id of the templates to be erased
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_erase_templates(uint32_t contextId, uint32_t enrolledId);

/**
 *   The function erases all the enrolled id of all the context databases.
 *
 *   This is recommended when update version is needed
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_erase_all_templates();

/**
 *   The function closes TZ-SP connection channel.
 *
 *   @return error code.
 */
sp_iris_result_e sp_iris_exit();

#endif /* SP_IRIS_H_ */
