/** @file test_utils.h

 @brief This file contains utility functions and macros to be 
        used by all algorithm tests

 */
/***************************************************************************
* Copyright (c) 2013,2014 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 

/***************************************************************************
* Edit History
*
* when       who     what, where, why
* --------   ---     ------------------------------------------------------- 
* 10/06/13   LD      Initial version
* 01/16/14   LD      Support for run test with function parameter
* --------   ---     ------------------------------------------------------- 
****************************************************************************/ 
#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include "typedef_ultrasound.h"

#ifndef MAXLEN
#define MAXLEN      256
#endif

/**
 * Alternative implementation of memscpy which is not available 
 * when compiling standalone algorithm tests 
 */
size_t  memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

/*--------------------------------------------------------------
 * open_file ()
 *
 * Utility function to open a file.
 * Assumes file is located under test_root_dir/base_path/fname
 * test_root_dir is specified using set_test_root_dir
 * 
 * Param[out]: pf Pointer to file
 * Param[in]: base_path base path where file is stored (see path above)
 * Param[in]: fname - String included File name
 * Param[in]: attr Read/Write attr
 *
 * Return: 0 if success, other value if failure
 *----------------------------------------------------------------*/  
int open_file(FILE **pf, char* base_path, char *fname, char *attr);

/*-------------------------------------------------------------- 
  same as open_file but exits process if failure
 *----------------------------------------------------------------*/  
int open_file_exit_on_fail(FILE **pf, char* base_path, char *fname, char *attr);

/**
 * set the root directory where test vectors and other test data 
 * resides. 
 * the open_file_*** functions automatically use root directory 
 * as base. 
 */
void set_test_root_dir(char* root_dir);

typedef int (*test_func_t)();
typedef int (*test_func_with_param_t)(void*);

/**
 * Run a single test and collect its results
 * 
 * @param test_name test name for information
 * @param test_func function implementing the test
 * 
 * @return int 0 if test success, other value if failed
 */
int run_test(char* test_name, test_func_t test_func);

/**
 * Run a single test and collect its results, 
 * support a test function with parameter 
 *  
 * @param test_name test name for information
 * @param test_func function implementing the test 
 * @param arg argument to test function 
 *  
 * @return int 0 if test success, other value if failed
 *  
 */
int run_test_with_param(char* test_name, test_func_with_param_t test_func, void* param);

/**
 * init test statistics. Call before starting the first 
 * test 
 */
void init_test_statistics();

/**
 * Call after all tests executed. 
 * Prints pass/fail results for all tests 
 */
void print_test_statistics();


/**
 * Parse a configuration value into an array of uint16_t values 
 * configuration value is in a form "1,2,3,..." without spaces 
 * @param value the configuration value (string) 
 * @param out array of uint16_t that will be filled with values 
 * @param maxSize maximum size of the array 
 * @param actualSize upon success, will be filled with actual 
 *                   number of values filled
 * 
 * @return int 0 on success, other value on failure
 */
int us_alg_test_utils_parse_uint16_array(char* value, uint16_t* out, uint32_t maxSize, uint32_t* actualSize);


#endif
