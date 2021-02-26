/*
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 */

#include <string.h>
#include <AEEStdDef.h>

#ifndef __ACD_MODEL_PARSER_UTILS_H__
#define __ACD_MODEL_PARSER_UTILS_H__

///////////////////////////////////////////////////////////////
//  MACRO FUNCTIONS ///////////////////////////////////////////
///////////////////////////////////////////////////////////////
#define ROUNDUP_MULTIPLE2(x) (((x) + 1) & ~1)
#define ROUNDUP_MULTIPLE4(x) (((x) + 3) & ~3)
#define ROUNDUP_MULTIPLE8(x) (((x) + 7) & ~7)

#define SELECT_MAX(x, y) ((x) > (y)  ? (x) : (y))
#define SELECT_MIN(x, y) ((x) < (y)  ? (x) : (y))


#define SERIALIZE_STATIC_FIELD(x,y)								\
		{														\
			memscpy((void *)(y), sizeof((x)), (void *)(&(x)), sizeof((x)));	\
			(y) += sizeof((x));									\
		}

#define SERIALIZE_DYNAMIC_FIELD(x,y,z)				\
		{											\
			memscpy((void *)(y), (z), (void *)(x), (z));	\
			(y) += (z);								\
		}

#define DECODER_VALUE_FIELD(field, field_type, buffer)			\
		{														\
			(field) = *(field_type *)(buffer);					\
			(buffer) += sizeof(field_type);						\
		}														

#define DECODER_ARRAY_FIELD(field, field_type, cnt, buffer)		\
		{														\
			(field) = (field_type *)(buffer);					\
			(buffer) += sizeof(field_type) * (cnt);				\
		}														

#define ASSIGN_U16_FROM_ADDRESS_AND_MOVE(dst, src) {\
	dst = *(uint16 *)src;							\
	src += sizeof(dst);								\
	}												\

#define ASSIGN_S16_FROM_ADDRESS_AND_MOVE(dst, src) {\
	dst = *(int16 *)src;							\
	src += sizeof(dst);								\
	}												\
	
#define ASSIGN_U32_FROM_ADDRESS_AND_MOVE(dst, src) {\
	dst = *(uint32 *)src;							\
	src += sizeof(dst);								\
	}												\

#define ASSIGN_S32_FROM_ADDRESS_AND_MOVE(dst, src) {\
	dst = *(int32 *)src;							\
	src += sizeof(dst);								\
	}												\

#define ASSIGN_S16P_FROM_ADDRESS_AND_MOVE(dst, src, len) {\
	dst = (int16 *)src;							\
	src += len;										\
	}												\

#define ADD_S16_FROM_ADDRESS_AND_MOVE(dst, src) {   \
	dst += *(int16 *)src;							\
	src += sizeof(dst);								\
	}												\

#define float_to_fixed_s16(x, q)		((int16) ((x)*((double)(1 << q)) + (x>=0 ? 0.5 : -0.5)))
#define float_to_fixed_s16_sat(x, q)	((int16) ((((x)*((double)(1 << q)) + (x>=0 ? 0.5 : -0.5)) > MAX_INT16) ? MAX_INT16 : ((((x)*((double)(1 << q)) + (x>=0 ? 0.5 : -0.5)) < MIN_INT16) ? MIN_INT16 : ((x)*((double)(1 << q)) + (x>=0 ? 0.5 : -0.5)) )))
#define float_to_fixed_u16(x, q)		((uint16) ((x)*((double)(1 << q)) + 0.5))
#define float_to_fixed_u16_sat(x, q)	((uint16) ((((x)*((double)(1 << q)) + 0.5) > MAX_UINT16) ? MAX_UINT16 : ((((x)*((double)(1 << q)) + 0.5) < 0) ? 0 : ((x)*((double)(1 << q)) + 0.5) )))
#define float_to_fixed_s32(x, q)		((int32) ((x)*((double)((long long)1 << q)) + (x>=0 ? 0.5 : -0.5)))
#define float_to_fixed_s32_sat(x, q)	((int32) ((((x)*((double)((long long)1 << q)) + (x>=0 ? 0.5 : -0.5)) > MAX_INT32) ? MAX_INT32 : ((((x)*((double)((long long)1 << q)) + (x>=0 ? 0.5 : -0.5)) < MIN_INT32) ? MIN_INT32 : ((x)*((double)((long long)1 << q)) + (x>=0 ? 0.5 : -0.5)) )))
#define float_to_fixed_u32(x, q)		((uint32) ((x)*((double)((long long)1 << q)) + 0.5))
#define float_to_fixed_u32_sat(x, q)	((uint32) ((((x)*((double)((long long)1 << q)) + 0.5) > MAX_UINT32) ? MAX_UINT32 : ((((x)*((double)((long long)1 << q)) + 0.5) < 0) ? 0 : ((x)*((double)((long long)1 << q)) + 0.5) )))
#define fixed_to_float(x, q)			((float)(x) / (1LL << q))

#endif /* __ACD_MODEL_PARSER_UTILS_H__ */

