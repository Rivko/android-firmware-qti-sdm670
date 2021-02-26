/*!
 * @file vpp_def.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */
#ifndef _VPP_DEF_H_
#define _VPP_DEF_H_

#define VPP_UNUSED(x) (void)(x)


enum vpp_bool {
    VPP_FALSE = 0,
    VPP_TRUE = (!VPP_FALSE),
};

typedef enum vpp_resolution t_EVppRes;
typedef enum vpp_color_format t_EVppColorFmt;

typedef enum {
    VPP_IP_HVX,
    VPP_IP_GPU,
    VPP_IP_FRC,
    VPP_IP_HCP,
    VPP_IP_MAX,
} t_EVppIp;

#define RANGE_CHECK(x, min, max) (((x) >= (max)) ? (max) : (((x) <= (min)) ? (min) : (x)))

#define VPP_MAKE_ENUM(V) V,
#define VPP_MAKE_STR(V) #V,

#define VPP_GEN_ENUM(_type, _strFnc, _which) \
    VPP_ENUM(_type, _which) \
    VPP_DBG_ENUM(_type, _strFnc, _which)

#define VPP_ENUM(_type, _which) \
    typedef enum { \
        _which(VPP_MAKE_ENUM) \
    } _type; \

#define VPP_DBG_ENUM(_type, _strFnc, _which) \
    static const char * _type##str[] = { \
        _which(VPP_MAKE_STR) \
    }; \
    const char *_strFnc(uint32_t eState) { \
        if (eState < (sizeof(_type##str)/sizeof(const char *))) { \
            return _type##str[eState]; \
        } else { return "UNKNOWN_STATE"; } \
    }

#define VPP_DBG_ENUM_DECL(_strFnc) const char *_strFnc(uint32_t eState)

#define VPP_RET_EQ(_var, _this, _that)      if ((_var) == (_this)) return _that
#define VPP_RET_STR(_msg, _val)             if ((_msg) == (_val)) return #_val

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/


#endif /* _VPP_DEF_H_ */
