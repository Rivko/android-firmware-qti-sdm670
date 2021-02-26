/*========================================================================
   sp_iris.h
   This file exposes the API of the sp iris HLOS library module.


Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
#ifndef INC_SP_IRIS_H_
#define INC_SP_IRIS_H_

/**
 * The function loads SP iris application, and allocates ION TZ-SP buffer
 * in the size of enroll_templates_size.
 *
 * @param enroll_templates_size(input/output) -   input:The size of two enrolled templates / 0 if the size is unknown
 *                                                output: The actual allocated size
 *
 * @param out_ion_data_fd(output)      -   The file descriptor	of the TZ-SP shared buffer

 *
 * @return 0 on success and -1 otherwise.
 */
int sp_iris_init(uint32_t* enroll_templates_size, int32_t* out_ion_data_fd);

/**
 * The function cleans up the framework
 *
 * @return 0 on success and -1 otherwise.
 */
int sp_iris_exit();

#endif /* INC_SP_IRIS_H_ */
