/*!
 * @file test_mock_registry.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */
#ifndef _TEST_MOCK_REGISTRY_H_
#define _TEST_MOCK_REGISTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

enum test_suite {
    TEST_SUITE_REAL_REGISTRY,
    TEST_SUITE_USECASE,
    TEST_SUITE_VPP_CLIENT,
    TEST_SUITE_VPP_CLIENT_NO_CP,
    TEST_SUITE_PIPELINE,
    TEST_SUITE_PIPELINE_UC_COMP_IS_TWO,
    TEST_SUITE_PIPELINE_UC_COMP_IS_ZERO,
    TEST_SUITE_PIPELINE_CUSTOM_CTRL,
    TEST_SUITE_USECASE_MAX_RES,
};
/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void get_registry_details(enum test_suite ts,
                          uint32_t *pu32Sz,
                          t_StVppUsecase ***pppstUcReg,
                          t_StVppUsecase **ppstUcAuto);
void init_test_registry(enum test_suite ts);
void term_test_registry();

t_StVppUsecase *getUsecaseAt(enum test_suite ts, uint32_t idx);
t_StVppUsecase *getUsecaseAuto(enum test_suite ts);
t_StVppUsecase *getTestUsecase();
void populateInternalCtrl(t_StVppUsecase *pstUc, t_StVppHqvCtrl *pstCtrl,
                          uint32_t bIsAuto);
uint32_t uc_sends_all_input_to_output(t_StVppUsecase *pstUc);
uint32_t uc_frame_interpolate(t_StVppUsecase *pstUc);

#ifdef __cplusplus
 }
#endif

#endif /* _TEST_MOCK_REGISTRY_H_ */
