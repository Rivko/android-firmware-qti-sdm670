/*========================================================================
Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */
/**
@file qurt_elite_example_launcher.h
*/

#ifdef OPENDSP

#ifndef QURT_ELITE_APP_TST_LAUNCHER_H
#define QURT_ELITE_APP_TST_LAUNCHER_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_eg_launcher
@{ */

/**
  Example of an application test launcher.

  @return
  None.

  @dependencies
  None.
*/
int qurt_elite_app_tst_launcher(void);

#ifndef QURT_ELITE_EXAMPLES_NO_GUI

/** Indicates the test type. */
uint32_t qurt_elite_test_type;

/** Valid test types.
*/
typedef enum
{
	QURT_ELITE_TEST_NONE=0,        /**< No test. */
	QURT_ELITE_TEST_THREAD,        /**< Test the thread. */
	QURT_ELITE_TEST_FIFO_QUEUES,   /**< Test the FIFO queues. */
	QURT_ELITE_TEST_LIFO_QUEUES,   /**< Test the LIFO queues. */
	QURT_ELITE_TEST_BUFFERMANAGER, /**< Test the buffer manager. */
	QURT_ELITE_TEST_SIGNALS,       /**< Test the signals. */
	QURT_ELITE_TEST_CHANNELS,      /**< Test the channels. */
	QURT_ELITE_TEST_MUTEX,         /**< Test the mutex. */
	QURT_ELITE_TEST_MALLOC,        /**< Test the memory allocation. */
	QURT_ELITE_TEST_NEW_DELETE,    /**< Test for new delete operators. */
	QURT_ELITE_TEST_TIMER,         /**< Test the timer. */
	QURT_ELITE_TEST_MAP,           /**< Test memory mapping. */
    QURT_ELITE_TEST_DONE      //keep at end
                                   /**< Placeholder to indicate the end of
                                        the tests; it does not signify a test. */
}  QURT_ELITE_TESTTYPE;
#endif  //QURT_ELITE_EXAMPLES_NO_GUI

/** @} */ /* end_addtogroup qurt_elite_eg_launcher */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //QURT_ELITE_APP_TST_LAUNCHER_H
#endif //OPENDSP
