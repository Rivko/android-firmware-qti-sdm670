#ifndef ATE_LOG_H__
#define ATE_LOG_H__

#include "apr_dbg_msg.h"

#define ateNOTE_INFO(str)                         APR_DBG_0(APR_DBG_MEDIUM, str)
#define ateNOTE1_INFO(str, p1)                    APR_DBG_1(APR_DBG_MEDIUM, str, p1)
#define ateNOTE2_INFO(str, p1, p2)                APR_DBG_2(APR_DBG_MEDIUM, str, p1, p2)
#define ateNOTE3_INFO(str, p1, p2, p3)            APR_DBG_3(APR_DBG_MEDIUM, str, p1, p2, p3)

#define ateNOTE_ERROR(str)                        APR_DBG_0(APR_DBG_ERROR, str)
#define ateNOTE1_ERROR(str, p1)                   APR_DBG_1(APR_DBG_ERROR, str, p1)
#define ateNOTE2_ERROR(str, p1, p2)               APR_DBG_2(APR_DBG_ERROR, str, p1, p2)
#define ateNOTE3_ERROR(str, p1, p2, p3)           APR_DBG_3(APR_DBG_ERROR, str, p1, p2, p3)

#define ateEXPECT_EQ(lhs, rhs, msg, p1)           ateEXPECT(lhs, ==, rhs, msg, p1)
#define ateEXPECT_NE(lhs, rhs, msg, p1)           ateEXPECT(lhs, !=, rhs, msg, p1)
#define ateEXPECT_LT(lhs, rhs, msg, p1)           ateEXPECT(lhs, < , rhs, msg, p1)
#define ateEXPECT_LE(lhs, rhs, msg, p1)           ateEXPECT(lhs, <=, rhs, msg, p1)
#define ateEXPECT_GT(lhs, rhs, msg, p1)           ateEXPECT(lhs, > , rhs, msg, p1)
#define ateEXPECT_GE(lhs, rhs, msg, p1)           ateEXPECT(lhs, >=, rhs, msg, p1)

#define ateASSERT_EQ(lhs, rhs, msg, p1)           ateASSERT(lhs, ==, rhs, msg, p1)
#define ateASSERT_NE(lhs, rhs, msg, p1)           ateASSERT(lhs, !=, rhs, msg, p1)
#define ateASSERT_LT(lhs, rhs, msg, p1)           ateASSERT(lhs, < , rhs, msg, p1)
#define ateASSERT_LE(lhs, rhs, msg, p1)           ateASSERT(lhs, <=, rhs, msg, p1)
#define ateASSERT_GT(lhs, rhs, msg, p1)           ateASSERT(lhs, > , rhs, msg, p1)
#define ateASSERT_GE(lhs, rhs, msg, p1)           ateASSERT(lhs, >=, rhs, msg, p1)

#define ateEXPECT(lhs, op, rhs, msg, p1)                                           \
{                                                                                  \
    if (!((lhs) op (rhs))) {                                                       \
        APR_DBG_1(APR_DBG_ERROR, "(" #lhs #op #rhs ") failed; " #msg, (p1));       \
    }                                                                              \
}

#define ateASSERT(lhs, op, rhs, msg, p1)                                           \
{                                                                                  \
    if (!((lhs) op (rhs))) {                                                       \
        APR_DBG_1(APR_DBG_ERROR, "(" #lhs #op #rhs ") failed; " #msg, (p1));       \
        return;                                                                    \
    }                                                                              \
}

#endif // ATE_LOG_H__
