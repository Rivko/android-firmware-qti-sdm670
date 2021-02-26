#ifndef QTEST_H
#define QTEST_H

/**
 * unit test framework for bulletproofing your code
 * 
 * The purpose of this library is to override malloc/calloc/free and
 * VERIFY apis to sequentialy simulate allocation and exception failures.
 * This will allow the developer to track down hard to find bugs in
 * their code.
 *
 * Usage:
 *
 * In make.d
 * <unit test exe>_LIBS+=qtest
 * <unit test exe>_C_SRCS+=mytest_q
 * mytest_q_DEFINES+=QTEST
 *
 * in your test mytest_q.c file:
 *
 * #include "qtest.h"
 * #include "mycode.c" //code we are going to unit test
 * 
 * 
 * //function to execute tests
 * //customize this to your need
 * 
 * int runtest(int(*pfn)(void)) {
 *    int nErr = -1;
 *    int ii; //! number of exceptions to succeed
 *
 *    //mutex for multi threaded tests
 *    qurt_mutex_t mut;
 *    struct qtest_mutex qm = { 
 *       (void*)&mut, 
 *       (void*)qurt_mutex_lock, 
 *       (void*)qurt_mutex_unlock 
 *    };
 *    qurt_mutex_init(&mut);
 *
 *    for(ii = 0; nErr != 0 && ii <= 10000; ++ii) {
 *       nErr = 0;
 *       FARF(HIGH, "runtest allowing exceptions: %d", ii);
 *       qtest_init_mt(100   //max leaks to track
 *                    ,5     //VERIFY trace depth per leak
 *                    ,ii    //num exceptions to pass
 *                    ,&qm); //mutex
 *       if(pfn) {
 *          //run the function
 *          VERIFY(0 == (nErr = pfn()));
 *       }
 * bail:
 *       //assert will fail only if we the code leaked more items 
 *       //then we initialized the qtest library to track.
 *       //Which is 100 leaks in this case.
 *       assert(0 == qtest_deinit(stderr, 0));
 *    }
 *    qurt_mutex_destroy(&mut);
 *    return nErr;
 * }
 *    
 * <ii>, becomes the number of times VERIFY will succeed before failure,
 * and running this loop will find the minimum number of time it will
 * succeed (up to 10000 in this case).  This should force execution of 
 * most of the exception handling code.
 *
 * Tracing memory leaks:
 * 
 * Simulating VERIFY failures works well in conjunction of tracing
 * memory leaks.  To be as cross platform as possible qtest uses the
 * 'macro/function' override C tricks to replace 'malloc/free' api with
 * its own implementaiton. 
 *
 * Each VERIFY macro pushes the context to the trace stack, allowing
 * us to capture where the leak occured.
 *
 * Only calls that are wrapped with VERIFY are tracked, if you use
 * your own exception macro take a look at the bottom of this file for
 * adding qtest_trace_push and qtest_trace_pop.
 *
 * For example:
 * 
 * static void* myfunc(int size) {
 *    void * mem;
 *    VERIFY(0 != (mem = malloc(size)));
 *    return mem;
 * }
 * qtest_init(100,  5, 10000, &mt)
 * 
 * VERIFY(0 != (mem = myfunc(1024)));
 * 
 * assert(0 == qtest_deinit(stderr, 0));
 *
 * error: leaked: addr: 99530 size: 1 * 1024
 *         0 != (mem = malloc(size))
 *         0 != (mem = myfunc(1024))
 *
 * Since VERIFY wrapped myfunc and malloc we can see it in the trace
 * reported by qtest_deinit.
 *
 * Example:
 * pakman get p4://qctp406/source/qcom/qct/platform/adsp/proj/pls
 *
 * Also checkout adsp/proj/utils/qtest_q.c unit test.
 *
 */
#include <stdint.h>
#include <stdio.h>
/**
 * @returns, 0 N number of times then returns non zero.
 *           N can be specified as the passes param to
 *           qtest_init.
 */
int qtest_test_failure(void);

/**
 * push the context to the trace stack
 */
void qtest_trace_push(const char*);

/**
 * pop the context from the trace stack
 */
void qtest_trace_pop(void);

/**
 * init the unit test environment
 * @param allocs, max number of outstanding allocations to track
 * @param tracedepth, the depth of the trace stack
 * @param passes, number of times qtest_test_failure
 *        will return 0
 */
void qtest_init(int allocs, int tracedepth, int64_t passes);

/**
 * init with a mutex
 */
struct qtest_mutex {
   void *ctx;
   void (*lock)(void*);   //! lock with ctx
   void (*unlock)(void*); //! unlock with ctx
};
void qtest_init_mt(int allocs, int tracedepth, int64_t passes, const struct qtest_mutex *m);

/**
 * get number of bytes used
 */
size_t qtest_bytes_used(void);


/**
 * @param verbose, if non 0, the leaks are printed to verbose
 * @param needed, returns the number of outstanding allocations needed
 * @return, 0 on success, -1, if needed exceeded allocs or number of leak occurred
 */
int qtest_deinit(FILE* verbose, int* needed);

/**
 * overrides stdlib's allocators to use the qtest ones
 */
#include <stdlib.h>

#ifdef QTEST

#define qtest_real_malloc(sz)         ((malloc)(sz))
#define qtest_real_calloc(cnt, sz)    ((calloc)(cnt, sz))
#define qtest_real_realloc(ptr, sz)   ((realloc)(ptr, sz))
#define qtest_real_memalign(al, sz)   ((memalign)(al,sz))
#define qtest_real_free(ptr)          ((free)(ptr))

void *qtest_malloc(size_t size);
void qtest_free(void *ptr);
void *qtest_calloc(size_t nmemb, size_t size);
void *qtest_memalign(size_t al, size_t size);
void *qtest_realloc(void *ptr, size_t size);

#define malloc(sz)         qtest_malloc(sz)
#define calloc(cnt, sz)    qtest_calloc(cnt, sz)
#define realloc(ptr, sz)   qtest_realloc(ptr, sz)
#define memalign(al, sz)   qtest_memalign(al, sz)
#define free(ptr)          qtest_free(ptr)

#define rpc_malloc(sz)         qtest_malloc(sz)
#define rpc_calloc(cnt, sz)    qtest_calloc(cnt, sz)
#define rpc_realloc(ptr, sz)   qtest_realloc(ptr, sz)
#define rpc_memalign(al, sz)   qtest_memalign(al, sz)
#define rpc_free(ptr)          qtest_free(ptr)


/**
 * overrides VERIFY to push and pop functions to trace
 */
#include "verify.h"
#undef VERIFY
#define VERIFY(val) \
   do {\
      int qfail = qtest_test_failure();\
      VERIFY_IPRINTF(":info: calling: %s", #val);\
      qtest_trace_push(__V_FILE_LINE__  " " #val); \
      if ((0 != qfail) || (0 == (val))) {\
         nErr = nErr == 0 ? -1 : nErr;\
         if(qfail) {\
            VERIFY_EPRINTF(":error:qtest: %d: %s", nErr, #val);\
         } else {\
            VERIFY_EPRINTF(":error: %d: %s", nErr, #val);\
         } \
         qtest_trace_pop(); \
         goto bail;\
      } else {\
         VERIFY_IPRINTF(":info: passed: %s", #val);\
         qtest_trace_pop(); \
      }\
   } while(0)
#endif //QTEST

#endif //QTEST_H
