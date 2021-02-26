
/* Target: GENERIC*/

#ifndef DAL_CONFIG_H
#define DAL_CONFIG_H


#include "DALPropDef.h"

#define DAL_BUILD_MULTI_PROC_IMAGE

// GCC compiler directiver for align
#define DAL_ALIGN(bits) __attribute__ ((aligned (bits)))

#define DAL_CONFIG_ARCH          DAL_CONFIG_ARCH_QDSP6_V5

#define DALGLBCTXT_SPINLOCK_PID  DAL_CONFIG_PROC_ADSP

#endif /* DAL_CONFIG_H */

