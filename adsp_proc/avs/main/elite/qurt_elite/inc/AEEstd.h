#ifndef AEESTD_H
#define AEESTD_H
/*====================================================================
        Copyright © 2004-2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
======================================================================

DESCRIPTION:  Kept for legacy compatibility in files still used in old platforms.


====================================================================*/

/* =======================================================================
 Legacy compatibility junk
========================================================================== */
#define STD_ARRAY_SIZE(a)             ((int)((sizeof((a))/sizeof((a)[0]))))

#define STD_MIN(a,b)   ((a)<(b)?(a):(b))
#define STD_MAX(a,b)   ((a)>(b)?(a):(b))
//lint -emacro(545,STD_ZEROAT)
#define STD_ZEROAT(p)  std_memset((p), 0, sizeof(*p))

#include <string.h>
#define std_memset(p, c, nLen) (memset((p), (c), (size_t)(nLen)))
#define std_memmove(pTo, cpFrom, nLen) (memmove((pTo), (cpFrom), (size_t)(nLen)))

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

extern int           std_strlprintf(char *pszDest, int nDestSize,
                                    const char *pszFmt, ...);

extern int           std_strlcpy(char *pcDst, const char *pszSrc, int nDestSize);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif // AEESTD_H


