/** @file
                              soctestlib.h

	Print latest message from the current Hypervisor circular buffer

    Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All rights reserved.

**/

#ifndef __SOC_TEST_LIB__
#define __SOC_TEST_LIB__

#include "com_dtypes.h"
#include <DDITF.h>
#include <scm_sip_interface.h>

/***************** Definations for common test usage ******************/

#define 	tests_daltf_add_tests		sti_add_testlist
#define 	tests_daltf_remove_tests 	sti_remove_testlist

EFI_STATUS sti_add_testlist(CONST TF_TestFunction_Descr**, UINTN);

EFI_STATUS sti_add_testlist_terminated(CONST TF_TestFunction_Descr**);

EFI_STATUS sti_remove_testlist(CONST TF_TestFunction_Descr**, UINTN);

EFI_STATUS sti_remove_testlist_terminated(CONST TF_TestFunction_Descr**);

EFI_STATUS sti_create_mapping(UINTN addr, UINTN size);


/***************** Definations for hyp&TZ testing ******************/

#define HYP_LOG_UPDATE	HypLogUpdate()
#define HYP_LOG_PRINT	PrintHypLog()

#define TZ_LOG_UPDATE 	TzLogUpdate()
#define TZ_LOG_PRINT	PrintTzbspLog()

/**
  Descr: Print all message from the current Hypervisor circular buffer
*/
void PrintHypLog(void);

/**
  print all message from the current Hypervisor circular buffer.
*/
void PrintTzbspLog(void);


void TzLogUpdate(void);
void HypLogUpdate(void);

#define HYP_CORETEST_MAX_PARAM 	5
#define TZ_CORETEST_MAX_PARAM   10

#define HYP_CORETEST_ID			8
#define HYP_TEST_SMC_ID 		TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_TEST_1, 0x4D)


#define	HYP_TEST_GROUP_SHFT				24
#define CORE_HYP_TEST_ID(g,t)			( (((g)&0xff)<<HYP_TEST_GROUP_SHFT) | (t))

/**
  general SMC call
*/
#define HYP_TESTS_OWNER_ARCH 0
#define HYP_TESTS_OWNER_PLATFORM 1

#define SMC_IRQS_DISABLED     1
#define SMC_IRQS_ENABLED      0
#define ARM_SMC_SMC64         1
#define ARM_SMC_SMC32         0
#define ARM_SMC_FAST_CALL     SMC_IRQS_DISABLED
#define ARM_SMC_STANDARD_CALL SMC_IRQS_ENABLED
#define ARM_SMC_RSP_REQ       0x80000000

#define SMC_FAST_CALL(smc_id, param_id, ...) 		sti_arm_scm_v8_call(smc_id, ARM_SMC_SMC64, ARM_SMC_FAST_CALL, param_id, ##__VA_ARGS__)
#define SMC_STANDARD_CALL(smc_id, param_id, ...) 	sti_arm_scm_v8_call(smc_id, ARM_SMC_SMC64, ARM_SMC_STANDARD_CALL, param_id, ##__VA_ARGS__)

uintnt sti_arm_scm_v8_call(uint32 smc_id, uint32 smc_aarch, uint32 smc_type,uint32 param_id, ...);

EFI_STATUS sti_hyp_testing_init(void);
EFI_STATUS sti_tz_testing_init(void);

/**
	TZ test SMC call
*/

/**
 *Macro to create testid.
31**30**************30********************20***********************0
+---+---------------+---------------------+----------------+---------------------+
|  1|   (1)reserved | (10) bit moduleID   |(4) bit groupId | (16) test_subid   |
+---+---------------+---------------------+----------------+---------------------+
*/
#define MSK_MODULE_BIT_SHIFT		(20)
#define MSK_MODULE_MSK				((uint32_t)0x3FF<<MSK_MODULE_BIT_SHIFT)
#define MSK_TECH_GROUP_BIT_SHIFT	(16)
#define MSK_TECT_GROUP_MSK      	((uint32_t)0xF<<MSK_TECH_GROUP_BIT_SHIFT)
#define MSK_CORE_TEST_ID_MSK		(((uint32)1<<MSK_TECH_GROUP_BIT_SHIFT)-1)

#define MODULE_CORE		  			0x10		// module ID of core test
#define CORE_SEC_TEST_ID(group,subid) 		\
					   ((MSK_MODULE_MSK&(MODULE_CORE<<MSK_MODULE_BIT_SHIFT))|	\
						(MSK_TECT_GROUP_MSK&((group)<<MSK_TECH_GROUP_BIT_SHIFT)) |	\
						((subid)& MSK_CORE_TEST_ID_MSK ))
   
#define TZBSP_TEST_FN_1(xx) 	TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_TEST_1, xx)
#define TZBSP_TEST_SECURE_TEST_ID      				TZBSP_TEST_FN_1(43)
#define TZBSP_TEST_SECURE_TEST_ID_PARAM_ID          TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

#define TZ_TEST_SMC_FAST_CALL(tech_group, test_subid, param_num, ...) \
			tz_test_scm_call(CORE_SEC_TEST_ID(tech_group,test_subid), ARM_SMC_SMC64, ARM_SMC_FAST_CALL, param_num, ##__VA_ARGS__)
			
uint32 tz_test_scm_call(uint32 test_id, uint32 smc_aarch, uint32 smc_type,uint32 param_num, ...);


#endif // __SOC_TEST_LIB__







