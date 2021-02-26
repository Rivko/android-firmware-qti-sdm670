#ifndef HAP_PAR_H
#define HAP_PAR_H

#include <stdint.h>

/**
 * Post a new job the threadpool.
 *
 * @param pfn, the function to run
 * @param ctx, the context for the pfn
 * @retval, 0 on success
 */
int HAP_par_post(void (*pfn)(void*), void *ctx);

/**
 * Wait for any of the jobs to complete
 *
 * This function must be called to cleanup the resources associated with post.
 * A job can be collected only once.
 *
 * @param ctx, array of ctx pointers passed to post
 * @param done, if the job is not done yet, set to 1 if job is complete
 *              Make sure this array is zero initialized before calling
 *              collect the first time.
 * @param num, number of items in ctx and done arrays
 * @param completed, increments by the number of jobs completed.
 *                   Make sure this counter is 0 before calling collect
 *                   the first time.
 * @retval, 0 on success
 *         -1 if waiting would result in a deadlock.  This would occur if
 *         all the entries in ctx that are not done are not qeueud or running.
 */
int HAP_par_collect(void *ctx[], int done[], int num, int *completed);

/**
 * Let the threadpool know that this thread is about to block indefinitely.
 *
 * Call this api when making a system call, or blocking on I/O, or waiting for
 * an interrupt, calling HAP_par_collect, etc...
 */
void HAP_par_block(void);

/**
 * Let the threadpool know that this thread is done blocking.
 */
void HAP_par_unblock(void);

#endif //HAP_PAR_H

