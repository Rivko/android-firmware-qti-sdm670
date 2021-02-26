/** @file app_bulletin_board.c
 *  @brief This file implements qsee bulletin board test plan
 */
/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_bulletin_board.c#1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  Jun/8/15   el      Created

  ===========================================================================*/

/*===========================================================================

  INCLUDE FILES FOR MODULE

  ===========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <IxErrno.h>
#include "qsee_bulletin_board.h"
#include "qsee_timer.h"
#include "stringl.h" // strlcpy(), timesafe_memcmp(), secure_memset()
#include "qsee_log.h"

// current application name, taken from app_main.c
extern const char *TA_APP_NAME;

// these values are taken from tzbsp_bulletin_board.c
#define MAX_NOTE_SIZE_IN_BYTES  256
#define MAX_CATEGORIES_NUMBER   4

#define BB_ASSERT_EQUAL(actual, expected)                               \
  if((actual) != (expected))                                            \
  {                                                                     \
    qsee_log(QSEE_LOG_MSG_ERROR, "-----------------------------------------------------------------------"); \
    qsee_log(QSEE_LOG_MSG_ERROR, "TEST FAILED. test name: %s [line %u]", __FUNCTION__, __LINE__); \
    qsee_log(QSEE_LOG_MSG_ERROR, "Expression: %s evaluates to: 0x%X instead of: %s", #actual, actual, #expected); \
    qsee_log(QSEE_LOG_MSG_ERROR, "-----------------------------------------------------------------------"); \
    return false;                                                       \
  }                                                                     \



static uint8_t message_buffer[MAX_NOTE_SIZE_IN_BYTES] = {0};

static qsee_bulletin_board_note g_note = {message_buffer, sizeof(message_buffer), ""};
static qsee_ta_name g_sender = {""};


typedef struct test_result_
{
  unsigned int passed;
  unsigned int failed;
  unsigned int total;

} test_result;


static void set_up(void)
{
  // reset global note structure
  g_note.data = message_buffer;
  g_note.size = sizeof(message_buffer);

  secure_memset(g_note.data, 0, sizeof(message_buffer));
  g_note.category[0] = '\0';
}


static void tear_down(void)
{
  ;
}

static void execute_test_case(bool (*test_case)(void), const char* test_case_name, test_result* result)
{
  bool test_case_result = true;

  // common operations before any test begin
  set_up();

  // execute the test case function
  test_case_result = test_case();

  // common clean up operations after each test is done.
  tear_down();

  qsee_log(QSEE_LOG_MSG_DEBUG, "TEST: %s %s", test_case_name, test_case_result ? "PASSED" : "FAILED");

  // update counters
  result->total++;

  if(test_case_result)
  {
    result->passed++;
  }
  else
  {
    result->failed++;
  }
}


static void set_category(const char* category)
{
  strlcpy(g_note.category, category, sizeof(g_note.category));
}


static int post_number_as_message(uint64_t number, const char* category)
{
  // copy the number into the note buffer.
  memscpy(g_note.data, MAX_NOTE_SIZE_IN_BYTES, &number, sizeof(number));

  // update the size as well
  g_note.size = sizeof(number);

  // copy the category
  set_category(category);

  return qsee_bulletin_board_post(&g_note);
}



//--------------------------------------- Test Cases -----------------------------------------------------

static bool simple_note_passing(void)
{
  // use uptime to keep testing freshness and avoid old messages to effect the result.
  const uint64_t current_time = qsee_get_uptime();

  // post a message
  BB_ASSERT_EQUAL(post_number_as_message(current_time, "cat1"), E_SUCCESS);

  // now the buffer of 'g_note' holds the message string and the category is updates as well.
  // reset the buffer and its size and verify that 'read' really reads the data from the kernel.
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);

  // result verification
  BB_ASSERT_EQUAL(g_note.size, sizeof(current_time));
  BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &current_time, g_note.size), 0);

  // For TAs that define UUID the sender name will be the uuid (in caps) as
  // per the definition of TA Distingushed Name/Distinguished ID
  BB_ASSERT_EQUAL(timesafe_strncmp(g_sender.name, TA_APP_NAME, sizeof(g_sender.name)) &&
                  timesafe_strncmp(g_sender.name, "53616D70-6C65-4170-7000-000000000032", sizeof(g_sender.name)) &&
                  timesafe_strncmp(g_sender.name, "53616D70-6C65-4170-7000-000000000064", sizeof(g_sender.name)), 0);
  return true;
}


static bool read_while_no_note_available(void)
{
  // use a category that no note has been sent under.
  set_category("catcat");

  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), -E_NO_DATA);

  return true;
}


static bool same_category_notes_overwrites_each_other(void)
{
  const uint64_t FIRST_MESSAGE = 1;
  const uint64_t SECOND_MESSAGE = 2;
  const uint64_t THIRD_MESSAGE = 3;

  // post several messages to the same category .
  BB_ASSERT_EQUAL(post_number_as_message(FIRST_MESSAGE, "cat2"), E_SUCCESS);
  BB_ASSERT_EQUAL(post_number_as_message(SECOND_MESSAGE, "cat2"), E_SUCCESS);
  BB_ASSERT_EQUAL(post_number_as_message(THIRD_MESSAGE, "cat2"), E_SUCCESS);

  // now the buffer of 'g_note' holds the message and the category is updates as well.
  // reset the buffer and its size and verify that the only last post is available for reading
  // under the category we used.
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);
  BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &THIRD_MESSAGE, g_note.size), 0);

  // and again
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);
  BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &THIRD_MESSAGE, g_note.size), 0);

  return true;
}


static bool different_category_notes_dont_overwrite_each_other(void)
{
  const uint64_t FIRST_MESSAGE  = 0x5a5a5a5a5a5a5a5a;
  const uint64_t SECOND_MESSAGE = 0x5affffffffff5a5a;

  // post several messages to different categories
  BB_ASSERT_EQUAL(post_number_as_message(FIRST_MESSAGE, "cat3"), E_SUCCESS);
  BB_ASSERT_EQUAL(post_number_as_message(SECOND_MESSAGE, "cat4"), E_SUCCESS);

  // read the note from cat 3
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  set_category("cat3");
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);
  BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &FIRST_MESSAGE, g_note.size), 0);

  // read the note from cat 4
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  set_category("cat4");
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);
  BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &SECOND_MESSAGE, g_note.size), 0);

  return true;
}


static bool full_notes_capacicy(void)
{
  const uint64_t MESSAGES[MAX_CATEGORIES_NUMBER+1] = {1, 2, 3, 4, 5};
  const char*    CATEGORY_NAMES[MAX_CATEGORIES_NUMBER+1] = {"cat 1", "cat 2", "cat 3", "cat 4", "cat 5"};
  unsigned int   i = 0;

  // post messages to MAX_CATEGORIES_NUMBER+1 categories.
  // in this case the note entry for "cat 1" should get overwritten.
  for(i=0 ; i<MAX_CATEGORIES_NUMBER+1 ; i++)
  {
    BB_ASSERT_EQUAL(post_number_as_message(MESSAGES[i], CATEGORY_NAMES[i]), E_SUCCESS);
  }

  // now read and make sure all last MAX_CATEGORIES_NUMBER exist while the first isn't
  for(i=1 ; i<MAX_CATEGORIES_NUMBER+1 ; i++)
  {
    secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
    g_note.size = MAX_NOTE_SIZE_IN_BYTES;
    set_category(CATEGORY_NAMES[i]);
    BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), E_SUCCESS);
    BB_ASSERT_EQUAL(timesafe_memcmp(g_note.data, &MESSAGES[i], g_note.size), 0);

    // For TAs that define UUID the sender name will be the uuid (in caps) as
    // per the definition of TA Distingushed Name/Distinguished ID
    BB_ASSERT_EQUAL(timesafe_strncmp(g_sender.name, TA_APP_NAME, sizeof(g_sender.name)) &&
                    timesafe_strncmp(g_sender.name, "53616D70-6C65-4170-7000-000000000032", sizeof(g_sender.name)) &&
                    timesafe_strncmp(g_sender.name, "53616D70-6C65-4170-7000-000000000064", sizeof(g_sender.name)), 0);
  }

  // first category message shouldn't be available.
  secure_memset(g_note.data, 0, MAX_NOTE_SIZE_IN_BYTES);
  g_note.size = MAX_NOTE_SIZE_IN_BYTES;
  set_category(CATEGORY_NAMES[0]);
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, &g_sender), -E_NO_DATA);

  return true;
}


static bool post_NULL(void)
{
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(NULL), -1);

  return true;
}


static bool post_NULL_message(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // NULL buffer pointer
  note.data = NULL;

  // non-zero size
  note.size = 1;

  // some valid category name
  note.category[0] = 'c';
  note.category[1] = '\0';

  // expect failure since note.data is NULL.
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), -1);

  return true;
}


static bool post_0_size_message(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // non-NULL buffer pointer
  note.data = message_buffer;

  // zero size
  note.size = 0;

  // some valid category name
  note.category[0] = 'c';
  note.category[1] = '\0';

  // expect failure since note.size is 0.
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), -1);

  return true;
}


static bool post_invalid_category(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // non-NULL buffer pointer
  note.data = message_buffer;

  // non-zero size
  note.size = 1;

  // invalid category name (empty string)
  note.category[0] = '\0';

  // expect failure because the category is an empty string
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), -E_INVALID_ARG);

  // try again with string that doesn't end with NULL termination character
  secure_memset(note.category, '1', sizeof(note.category));
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), -E_INVALID_ARG);

  return true;
}


static bool read_NULL(void)
{
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(NULL, &g_sender), -1);
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&g_note, NULL), -1);

  return true;
}


static bool read_NULL_message(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // NULL buffer pointer
  note.data = NULL;

  // non-zero size
  note.size = 1;

  // some valid category name
  note.category[0] = 'c';
  note.category[1] = '\0';

  // expect failure since note.data is NULL.
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&note, &g_sender), -1);

  return true;
}


static bool read_0_size_message(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // non-NULL buffer pointer
  note.data = message_buffer;

  // zero size
  note.size = 0;

  // some valid category name
  note.category[0] = 'c';
  note.category[1] = '\0';

  // expect failure since note.size is 0.
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&note, &g_sender), -1);

  return true;
}


static bool read_invalid_category(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // non-NULL buffer pointer
  note.data = message_buffer;

  // non-zero size
  note.size = 1;

  // invalid category name (empty string)
  note.category[0] = '\0';

  // expect failure because the category is an empty string
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&note, &g_sender), -E_INVALID_ARG);

  // try again with string that doesn't end with NULL termination character
  secure_memset(note.category, '1', sizeof(note.category));
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&note, &g_sender), -E_INVALID_ARG);

  return true;
}


static bool post_with_too_long_buffer(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // indicate a buffer length which is a little larger
  // then the bulletin board can hold.
  note.size = MAX_NOTE_SIZE_IN_BYTES+1;
  note.data = message_buffer;
  note.category[0] = 'c';
  note.category[1] = '\0';

  // expect failure since due to invalid size.
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), -E_INVALID_ARG);

  return true;
}


static bool read_with_too_short_buffer(void)
{
  qsee_bulletin_board_note note = {NULL, 0, ""};

  // this post should succeed.
  note.data = message_buffer; // what's in it doesn't matter for this test.
  note.size = MAX_NOTE_SIZE_IN_BYTES;
  note.category[0] = 'c';
  note.category[1] = '\0';
  BB_ASSERT_EQUAL(qsee_bulletin_board_post(&note), E_SUCCESS);

  // now try to read it with shorter buffer available
  note.size = MAX_NOTE_SIZE_IN_BYTES-1;
  BB_ASSERT_EQUAL(qsee_bulletin_board_read(&note, &g_sender), -E_BUF_OVERFLOW);

  return true;
}

int run_bulletin_board_tests(void)
{
  // this structure is being updated by 'execute_test_case'
  test_result results = {0,0,0};

  // L1: basic functionality
  execute_test_case(simple_note_passing,                                "simple_note_passing",                                &results);
  execute_test_case(read_while_no_note_available,                       "read_while_no_note_available",                       &results);
  execute_test_case(same_category_notes_overwrites_each_other,          "same_category_notes_overwrites_each_other",          &results);
  execute_test_case(different_category_notes_dont_overwrite_each_other, "different_category_notes_dont_overwrite_each_other", &results);
  execute_test_case(full_notes_capacicy,                                "full_notes_capacicy",                                &results);

  // L2: input validation
  execute_test_case(post_NULL,             "post_NULL",             &results);
  execute_test_case(post_NULL_message,     "post_NULL_message",     &results);
  execute_test_case(post_0_size_message,   "post_0_size_message",   &results);
  execute_test_case(post_invalid_category, "post_invalid_category", &results);
  execute_test_case(read_NULL,             "read_NULL",             &results);
  execute_test_case(read_NULL_message,     "read_NULL_message",     &results);
  execute_test_case(read_0_size_message,   "read_0_size_message",   &results);
  execute_test_case(read_invalid_category, "read_invalid_category", &results);

  // L3: resource and memory handling
  execute_test_case(post_with_too_long_buffer,  "post_with_too_long_buffer",  &results);
  execute_test_case(read_with_too_short_buffer, "read_with_too_short_buffer", &results);

  qsee_log(QSEE_LOG_MSG_DEBUG, "qsee bulletin board testing results: [%u/%u passed] [%u/%u failed]", results.passed, results.total, results.failed, results.total);

  // return '0' if and only if all tests passed successfully.
  return (results.passed == results.total ? 0 : -1);
}
