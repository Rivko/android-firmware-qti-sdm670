#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

/**
 * this will either crash or stop the exception handler
 *
 * @retval, ignore this value
 */
int exception_handler_stop(void);

int exception_handler_start(void);


int fastrpc_post_crash_signature(char *crash);

#endif // EXCEPTION_HANDLER_H
