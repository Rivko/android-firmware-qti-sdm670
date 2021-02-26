#ifdef OPENDSP

#include "qurt_elite.h"
#include "qurt_elite_example_launcher.h"
#include "qurt_elite_examples.h"

extern qurt_elite_globalstate_t qurt_elite_globalstate;

static const int g_threadPriority = 100;

int RunTest(char* pszName, int (*fn) (void*), const int nInstances, int nStack, int nPrio, void *arg)
{
   int i;
   qurt_elite_thread_t t[nInstances];
   int result = 0;
   int64_t time1, time2;
   int nMalloc, nFree;

#if defined(SIM)
   printf("Now running test: %s...\n", pszName);
#endif

   time1 = qurt_elite_profile_get_pcycles();

   // Track previous memory allocation.
   nMalloc = qurt_elite_globalstate.heap_stats.num_mallocs;
   nFree   = qurt_elite_globalstate.heap_stats.num_frees;


   for (i = 0; i < nInstances; i++)
   {
      if (ADSP_FAILED (result = qurt_elite_thread_launch(&(t[i]), pszName, NULL, nStack, nPrio, fn, arg, QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "thread failed to launch\n");
         break;
      }
   }

   if (ADSP_SUCCEEDED(result))
   {
      for (i = 0; i < nInstances; i++)
      {
         qurt_elite_thread_join(t[i], &result);
         if (ADSP_FAILED (result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "returned failure code = %d!!\n", result);
            break;
         }
      }
      if ( ( qurt_elite_globalstate.heap_stats.num_mallocs - nMalloc) 
           != ( qurt_elite_globalstate.heap_stats.num_frees - nFree ) )
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "had a memory leak!!\n");
         result = ADSP_EFAILED;
      }
      time2 = qurt_elite_profile_get_pcycles();
#if defined(SIM)
      printf("took %llu total Pcycles\n", time2-time1);
#endif
   }

   if (ADSP_SUCCEEDED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SUCCESS\n");
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILURE code %d\n", result);
   }

   return result;
}


int qurt_elite_app_tst_launcher(void)
{
   int result;
   int nTotalTests = 0;
   int nFailedTests = 0;
   qurt_elite_thread_t t1, t2, t3, t4;

#ifdef QURT_ELITE_EXAMPLES_TRACE32
   qurt_elite_test_type = QURT_ELITE_TEST_NONE;
   while (1)   {
       switch (qurt_elite_test_type)   {
           case QURT_ELITE_TEST_NONE:
           // Do nothing
           break;

           case QURT_ELITE_TEST_THREAD:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_thread", qurt_elite_examples_thread,
               1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

           case QURT_ELITE_TEST_FIFO_QUEUES:
           // Test Q Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_queue", qurt_elite_examples_queue,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

           case QURT_ELITE_TEST_LIFO_QUEUES:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_lifoqueue", qurt_elite_examples_lifoqueue,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

           case QURT_ELITE_TEST_BUFFERMANAGER:
           // Test Q Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_bufmgr", qurt_elite_examples_bufmgr,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

           case QURT_ELITE_TEST_SIGNALS:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_signal", qurt_elite_examples_signal,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

               case QURT_ELITE_TEST_CHANNELS:
           // Test Q Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_channel", qurt_elite_examples_channel,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

               case QURT_ELITE_TEST_MUTEX:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_mutex",qurt_elite_examples_mutex,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

               case QURT_ELITE_TEST_MALLOC:
           // Test Q Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_malloc", qurt_elite_examples_malloc,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

               case QURT_ELITE_TEST_NEW_DELETE:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_newdelete", qurt_elite_examples_newdelete,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
               break;

               case QURT_ELITE_TEST_TIMER:
           // Test Q Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_timer", qurt_elite_examples_timer,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

               case QURT_ELITE_TEST_MAP:
           // Test Pthreads Basics
           nTotalTests++;
           nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_memorymap", qurt_elite_examples_memorymap,
              1, 4096, g_threadPriority, NULL))?1:0);
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;

           case QURT_ELITE_TEST_DONE:
           if (0 < nTotalTests)  {
               if (0 < nFailedTests) {
                   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "%d tests ADSP_FAILED out of %d tests run.\n", nFailedTests, nTotalTests);
               }
               else {
                   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "=========== ALL %d TESTS PASSED ===========.\n", nTotalTests);
               }
           }
           return nFailedTests;

           default:
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Bad Test case \n");
           qurt_elite_test_type = QURT_ELITE_TEST_NONE;
           break;
       }
   }
#else   //QURT_ELITE_EXAMPLES_TRACE32
// QURT_ELITE_TEST_THREAD
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_thread", qurt_elite_examples_thread,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_FIFO_QUEUES
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_queue", qurt_elite_examples_queue,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_LIFO_QUEUES
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_lifoqueue", qurt_elite_examples_lifoqueue,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_BUFFERMANAGER
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_bufmgr", qurt_elite_examples_bufmgr,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_SIGNALS
  nTotalTests++;
  nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_signal", qurt_elite_examples_signal,
     1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_CHANNELS
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_channel", qurt_elite_examples_channel,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_MUTEX
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_mutex", qurt_elite_examples_mutex,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_MALLOC
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_malloc", qurt_elite_examples_malloc,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_NEW_DELETE
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_newdelete", qurt_elite_examples_newdelete,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_TIMER
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_timer", qurt_elite_examples_timer,
      1, 4096, g_threadPriority, NULL))?1:0);

// QURT_ELITE_TEST_MAP
   nTotalTests++;
   nFailedTests += (ADSP_FAILED(RunTest("qurt_elite_examples_memorymap", qurt_elite_examples_memorymap,
      1, 4096, g_threadPriority, NULL))?1:0);

   if (0 < nTotalTests)
   {
      if (0 < nFailedTests)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "%d tests ADSP_FAILED out of %d tests run.\n", nFailedTests, nTotalTests);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "=========== ALL %d TESTS PASSED ===========.\n", nTotalTests);
      }
   }
   return nFailedTests;
#endif  //!QURT_ELITE_EXAMPLES_TRACE32
}
#endif //OPENDSP
