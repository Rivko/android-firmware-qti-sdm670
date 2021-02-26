#ifndef PERF_H
#define PERF_H

#include <stdint.h>
#include "HAP_perf.h"

/**
 * Simple fast perf counters.
 *
 * PERF(something_counter, //start measurement
 *
 *    if(something) {
 *       do_something();
 *    } else {
 *       do_something_else();
 *    }
 *
 *    PERF_END);           //end measurement
 *
 * This will accumilate the time spent inside the macros.
 *
 * Runnning `rpcperf perf_on` will return all the accumilated
 * counters durring a run.
 */


/**
 * initialize a perf counter. To add this module to your code add
 * PL_DEP(perf).  PL_INIT(perf) must be called before this.
 * See platform_libs.h
 *
 * int mymod_init(void) {
 *    int nErr = 0;
 *    VERIFY(!PL_INIT(perf));
 *    VERIFY(!perf_init("mycounter", &me->perfc));
 * bail:
 *    if(nErr) {
 *       mymod_deinit();
 *    }
 *    return nErr;
 * }
 *
 * @param name, the name of the counter
 * @param pix, the counter index. Its enabled when (*pix) != 0
 * @retval, 0 on success
 */
int perf_init(char* name, uintptr_t* pix);

/**
 * init global counter
 *
 * extern int perf_foobar;
 *
 * int mymod_init(void) {
 *    int nErr = 0;
 *    VERIFY(!PL_INIT(perf));
 *    VERIFY(!PERF_INIT_C(perf_foobar));
 *
 */
#define PERF_INIT_C(ix) perf_init(#ix, (uintptr_t*)&ix)

/**
 * macro to test if the counter is enabled
 */
#define perf_enabled(ix) ((ix) != 0)

/**
 * add sim micorseconds to the counter
 */
int perf_add_usec(uintptr_t ix, uint64_t usec);

/**
 * macro for wrapping code with a perf counter
 *
 * PERF(me->counter,
 *    if(something) {
 *       do_something();
 *    } else {
 *       do_something_else();
 *    }
 *    PERF_END);
 */

#define PERF_COUNT(cnt, cnd) \
  {\
    if(cnd) {\
       perf_add_usec((cnt), 1);\
    }\
  }

#define PERF_END (void)0

#define PERF(cnt, ff) \
  {\
    uint64 start=0; \
    int penable = perf_enabled((cnt));\
    if(penable) {\
       start = HAP_perf_get_time_us();\
    }\
    ff ;\
    if(penable && start != 0) {\
       (void)perf_add_usec((cnt), HAP_perf_get_time_us() - start);\
    }\
  }

#define PERF_CND(cnt, ff, cnd) \
  {\
    uint64 start=0; \
    int penable = perf_enabled((cnt));\
    if(penable && (cnd)) {\
       start = HAP_perf_get_time_us();\
    }\
    ff ;\
    if(penable && (start!=0)) {\
       (void)perf_add_usec((cnt), HAP_perf_get_time_us() - start);\
    }\
  }

#endif //PERF_H
