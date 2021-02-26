#ifndef __APR_DBG_MSG_H__
#define __APR_DBG_MSG_H__

#include "mmdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* APR Message Priorities */
#define APR_DBG_LOW      (MSG_MASK_0)	/* (0x00000001) */
#define APR_DBG_MEDIUM   (MSG_MASK_1)	/* (0x00000002) */
#define APR_DBG_HIGH     (MSG_MASK_2)	/* (0x00000004) */
#define APR_DBG_ERROR    (MSG_MASK_3)	/* (0x00000008) */
#define APR_DBG_FATAL    (MSG_MASK_4)	/* (0x00000010) */
#define APR_DBG_DEBUG    (MSG_MASK_5)	/* (0x00000020) */

/*
  This is the macro for messages with no parameters with priority.
 */
#define APR_DBG_0(xx_prio, xx_fmt)	\
  {  \
    MSG ( MSG_SSID_DFLT, xx_prio, xx_fmt ); \
}

/*
  Macro for messages with 1 parameter and a specific priority
*/
#define APR_DBG_1(xx_prio, xx_fmt, xx_arg1) \
{	\
    MSG_1( MSG_SSID_DFLT, xx_prio, xx_fmt, xx_arg1); \
}

/*
  Macro for messages with 2 parameters and a specific priority.
*/
#define APR_DBG_2(xx_prio, xx_fmt, xx_arg1, xx_arg2) \
{ \
    MSG_2( MSG_SSID_DFLT, xx_prio, xx_fmt, xx_arg1, xx_arg2); \
}

/*
  This is the macro for messages with 3 parameters and a specific priority.
*/
#define APR_DBG_3(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3)	\
{ \
    MSG_3( MSG_SSID_DFLT, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3); \
}

#ifdef __cplusplus
}
#endif

#endif /* __APR_DBG_MSG_H__ */
