#ifndef PAR_H
#define PAR_H

/**
 * allow for thread creation
 *
 * call this after shutdown to re-enable the pool
 */
void HAP_par_enable(void);

/**
 * kill all the threads and fail all the posts
 */
void HAP_par_disable(void);

#endif // PAR_H
