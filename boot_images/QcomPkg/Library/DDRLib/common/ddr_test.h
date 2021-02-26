#ifndef DDR_TEST_H
#define DDR_TEST_H

/**
 * @file ddr_test.h
 * @brief
 * Header file for DDR Test Framework.
 */

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/15/13   SL      Added ddr_debug().
08/11/10   SL      Initial version.
================================================================================
                   Copyright 2010,2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/

#include "ddr_common.h"


/**
 * DDR test suites.
 */
typedef enum
{
  DDR_TEST_SUITE_EMPTY = 0,         /**< Empty test suite */
  DDR_TEST_SUITE_ACCESS,            /**< Test suite for DDR access */
  DDR_TEST_SUITE_SELF_REFRESH,      /**< Test suite for DDR self refresh */
  DDR_TEST_SUITE_DEEP_POWER_DOWN,   /**< Test suite for DDR deep power down */
  DDR_TEST_SUITE_CLOCK_SWITCHING,   /**< Test suite for DDR clock switching */
  DDR_CUSTOM_TEST,
  DDR_TEST_SUITE_MAX = 0x7FFFFFFF,  /**< Force enum to 32 bits */
} ddr_test_suite;

/**
 * DDR test error types.
 */
typedef enum
{
  DDR_TEST_ERR_NONE,              /**< No error */
  DDR_TEST_ERR_UNATTACHED,        /**< No DDR attached */
  DDR_TEST_ERR_MEM_MAP,           /**< Memory map error */
  DDR_TEST_ERR_DATA_LINES,        /**< Data line error */
  DDR_TEST_ERR_ADDR_LINES,        /**< Address line error */
  DDR_TEST_ERR_OWN_ADDR,          /**< Own address error */
  DDR_TEST_ERR_WALKING_ONES,      /**< Walking ones error */
  DDR_TEST_ERR_SELF_REFRESH,      /**< Self refresh error */
  DDR_TEST_ERR_DEEP_POWER_DOWN,   /**< Deep power down error */
  DDR_TEST_ERR_CLOCK_SWITCHING,   /**< Clock Switching error */
  DDR_TEST_ERR_MAX = 0x7FFFFFFF,  /**< Force enum to 32 bits */
} ddr_test_error_type;

/**
 * DDR test log.
 */
struct ddr_test_log
{
  const char          *message;     /**< Test message */
  ddr_test_error_type  error_type;  /**< Error type */
  uint32               error_addr;  /**< Address where error occurs */
  uint32               error_data;  /**< Data pattern where error occurs */
};

/* =============================================================================
**  Function : ddr_test
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at specific interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed or no DDR attached at the interface/CS
*   @retval  FALSE    Test failed
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

boolean ddr_test
(
  ddr_test_suite    test_suite,
  uint32            test_level,
  DDR_CHANNEL   interface,
  DDR_CHIPSELECT  chip_select,
  uint32            clk_speed
);

boolean ddr_custom_test
(
  ddr_test_suite    test_suite,
  uint32            test_level,
  uint64 ddr_base,
  uint64 ddr_size,
  uint32            clk_speed
);

/* =============================================================================
**  Function : ddr_test_all
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at each interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed for all attached DDR
*   @retval  FALSE    Test failed for some attached DDR
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/

boolean ddr_test_all
(
  ddr_test_suite  test_suite,
  uint32          test_level,
  uint32          clk_speed
);

/* =============================================================================
**  Function : ddr_test_exit
** =============================================================================
*/
/**
*   @brief
*   Exit point of DDR Test Framework: an infinite loop.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/

void ddr_test_exit( void );

/* =============================================================================
**  Function : ddr_debug
** =============================================================================
*/
/**
*   @brief
*   DDR Debug Mode engine: receive and echo user's inputs, and execute commands
*   if ready.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Transportation layer (e.g., USB) must be set up, and all data must be moved
*   out of DDR (otherwise would be corrupted).
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_DDR_DEBUG_MODE
void ddr_debug( void );
#else
static inline void ddr_debug ( void )
{}
#endif


#endif /* DDR_TEST_H */
