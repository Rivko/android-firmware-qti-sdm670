#ifndef ESEDEBUG_H__
#define ESEDEBUG_H__

#include <stdint.h>
#include <stringl.h>

/**
 *
 * Prints a buffer in single byte string for debugging purpose.
 *
 * @param    [in] pBuffer : Buffer whose contents are to be printed.
 * @param    [in] length  : Length of the buffer.
 * @param    [in] pMode   : Debug level string to identify logs further
 *                          e.g. TX or RX or DBG or TA(to print trusted app logs).
 **/
void EseLogBuffer(const uint8_t *pBuffer, size_t length, const char *pMode);

/**
 *
 * Writes the results to QSEE log
 *
 **/
void EseLog(const char* fmt, ...);

#endif // ESEDEBUG_H__
