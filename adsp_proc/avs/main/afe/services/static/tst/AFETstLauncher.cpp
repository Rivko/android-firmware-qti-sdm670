 #include "qurt_elite.h"
 #include "AFETstLauncher.h"
 #include "AFETestHandlers.h"
 #include "Elite.h"
 #include "EliteMsg_Util.h"
 #include "AFEInterface.h"

 void AFE_tst_launch_routine(int (*vpTestThreadLauncher) (void*),
                           int *,int *,int *,qurt_elite_queue_t*);
 int AFE_dummy_test(void* cmdQ);
 int afe_test_enable(void* cmdQ);
 int afe_test_disable(void* cmdQ);

 #define NUM_TESTCASES  11
 /* array of function pointers and a flag whether to execute the test or not
  * first column -> function name, second column -> flag function
  * *** The function launches test thread(s), joins them and returns the OR'ed result ***
  */
#ifdef AFE_TEST
 typedef int (*test_func) (void* /*cmdQ*/);
 static  test_func afe_test_list_ptr[NUM_TESTCASES][2] = {
     {AFE_test_apr_loopback,                 afe_test_disable},                    /* 0 - Loopback Test*/
     {AFE_test_apr_ext_loopback,             afe_test_enable},                    /* 1 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 2 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 3 -  Test */
     {AFE_dummy_test,              	         afe_test_disable},                    /* 4 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 5 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 6 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 7 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 8 -  Test*/
     {AFE_dummy_test,                        afe_test_disable},                    /* 9 -  Test*/
#if (TARGET==8960)
     {AFE_test_riva,                         afe_test_enable},                    /* 10 -  Test*/
#else
     {AFE_dummy_test,                        afe_test_disable},                    /* 10 - dummy Test*/
#endif
 };
#endif

 #ifdef AFE_DAL_TEST
 typedef int (*test_func) (void* /*cmdQ*/);
 static  test_func afe_test_list_ptr[NUM_TESTCASES][2] = {
     {AFE_dummy_test,                           afe_test_disable},                 /* 0 - DAL Loopback Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 1 - DAL Playback Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 2 - DAL Loopback ext Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 3 - APR Test */
     {AFE_dummy_test,                           afe_test_disable},                 /* 4 - playback DAL Test AC-3*/
     {AFE_test_fw_test1Dal,                     afe_test_enable},                  /* 5 - avtimer, dml, resampler Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 6 - APR Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 7 - dummy Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 8 - dummy Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 9 - dummy Test*/
     {AFE_dummy_test,                           afe_test_disable},                 /* 10 - dummy Test*/
 };
#endif

extern qurt_elite_globalstate_t qurt_elite_globalstate;

 int afe_tst_launcher()
 {

    ADSPResult    result = ADSP_EOK;

#if defined(AFE_TEST) || defined(AFE_DAL_TEST)
    int testCaseId, *dummy = NULL;
    int nTotalTests = 0;
    int nFailedTests = 0;

    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "\n\nNow running test: AFETests...\n");

    qurt_elite_queue_t* afeServCmdQ;
    afeServCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
    if (NULL == afeServCmdQ)
    {
       result = ADSP_EFAILED;
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to get static service queue!!\n");
       return result;
    }

    // Tests
    for (testCaseId = 0; testCaseId < NUM_TESTCASES; testCaseId++)
    {
      if (afe_test_list_ptr[testCaseId][1]((void *) dummy ))
      {
        AFE_tst_launch_routine(afe_test_list_ptr[testCaseId][0],&nTotalTests,&nFailedTests,&result,afeServCmdQ);
      }
    }

     if (0 < nTotalTests)
     {
         if (0 < nFailedTests)
         {
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "%d tests ADSP_FAILED out of %d tests run.\n", nFailedTests, nTotalTests);
         }
         else
         {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "=========== ALL %d TESTS PASSED ===========.\n", nTotalTests);
         }
     }

#endif // #if defined(AFE_TEST) || defined(AFE_DAL_TEST)

     return result;
 }

 void AFE_tst_launch_routine(   int (*vpTestThreadLauncher) (void*),
                       int *pTotalTests,
                       int *pFailedTests,
                       int *pResult,
                       qurt_elite_queue_t* afeServCmdQ)
 {
   int threadResult;
   int nMalloc, nFree, nNumLeaks;

   // Track previous memory allocation.
   nMalloc = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_mallocs;
   nFree   = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_frees;

   (*pTotalTests)++;

   threadResult = vpTestThreadLauncher (afeServCmdQ); //launch a thread and join

   nNumLeaks = (qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_mallocs - nMalloc)
               - (qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_frees - nFree);
   if (nNumLeaks != 0)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Test had %d memory leaks.", nNumLeaks);
   }

   if (threadResult)
   {
     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Test End: test failed.");
   }
   else
   {
     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Test End: test passed.");
   }
   if (ADSP_FAILED(threadResult))
   {
     (*pFailedTests)++;

   }
   (*pResult) |= threadResult;
 }

 int AFE_dummy_test(void* cmdQ)
 {
   return 0;
 }
 int afe_test_enable(void* cmdQ)
 {
   return 1;
 }
 int afe_test_disable(void* cmdQ)
 {
   return 0;
 }
