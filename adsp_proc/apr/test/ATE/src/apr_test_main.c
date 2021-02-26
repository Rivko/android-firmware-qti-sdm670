#include <stdio.h>
#include "mmstd.h"
#include "msg.h"
#include "ATE.h"
#include "ATE_testserver_api.h"

//#define APR_TEST_SERVER_ADDR          0x03C8         /* Test service 200 on Modem */
#define APR_TEST_SERVER1_NAME                "ATE.testserver"
#define APR_TEST_SERVER2_NAME                "qcom.audio.testserver"

int32_t apr_test_main(void)
{
    int32_t rc = APR_EOK;
    testserver_init_param  ts_init_param;

	rc = ate_init();
    if (APR_EOK != rc) goto cleanup;

    /* General Test Cases */
    ate_get_version();
    ate_allocate_free(40);

    /* Query Test Cases */
    ate_query_by_dns();
    ate_query_by_handle();

    /* Register Test Cases */
    ate_register2_by_name("qcom.domain.mydnsname", sizeof("qcom.domain.mydnsname"));
    ate_register2_by_addr();

    /* Send Test Cases */
    //ate_alloc_send("qct.funcarea.toyserver", sizeof("qct.funcarea.toyserver"), 3, 4);
    //ate_alloc_send("oem.toyserver", sizeof("oem.toyserver"), 3, 4);

    mmstd_memset((void *)&ts_init_param, 0, sizeof(ts_init_param));

    mmstd_strlcpy(ts_init_param.name, APR_TEST_SERVER1_NAME, sizeof(APR_TEST_SERVER1_NAME));
    ts_init_param.name_len = sizeof(APR_TEST_SERVER1_NAME);
    testserver_call(TOYSERVER_CALLINDEX_ENUM_INIT, (void *)&ts_init_param, sizeof(ts_init_param));
    ate_alloc_send(APR_TEST_SERVER1_NAME, sizeof(APR_TEST_SERVER1_NAME), 3, 4);
    testserver_call(TOYSERVER_CALLINDEX_ENUM_DEINIT, NULL, 0);

    mmstd_strlcpy(ts_init_param.name, APR_TEST_SERVER2_NAME, sizeof(APR_TEST_SERVER2_NAME));
    ts_init_param.name_len = sizeof(APR_TEST_SERVER2_NAME);
    testserver_call(TOYSERVER_CALLINDEX_ENUM_INIT, (void *)&ts_init_param, sizeof(ts_init_param));
    ate_alloc_send(APR_TEST_SERVER2_NAME, sizeof(APR_TEST_SERVER2_NAME), 3, 4);
    testserver_call(TOYSERVER_CALLINDEX_ENUM_DEINIT, NULL, 0);

cleanup:
    ate_deinit();

	return 0;
}
