/********************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

//------------------------------------------------------------------------
// object.h : invocable objects
//------------------------------------------------------------------------

#ifndef __OBJECT_H
#define __OBJECT_H

#include <stddef.h>    // size_t
#include <stdint.h>    // int32_t


//------------------------------------------------------------------------
// ObjectOp
//------------------------------------------------------------------------

typedef uint32_t ObjectOp;

// Method bits are not modified by transport layers.  These describe the
// method (member function) being requested by the client.

#define ObjectOp_METHOD_MASK     ((ObjectOp) 0x0000FFFFu)
#define ObjectOp_methodID(op)    ((op) & ObjectOp_METHOD_MASK)

// end of user-defined range

#define ObjectOp_METHOD_USERMAX  ((ObjectOp) 0x00003FFF)

// Modifier bits are reserved for transport-layer semantics.

#define ObjectOp_MODIFIER_MASK   ((ObjectOp) 0xFFFF0000u)

// ObjectOp_REMOTE_BUFS is set by transports when buffers may reside in
//    untrusted memory and buffer alignment is untrusted. Implementations of
//    invoke may skip some validation and/or copying when this is clear.

#define ObjectOp_REMOTE_BUFS     ((ObjectOp) 0x00010000u)

// Potential extensions...
// #define ObjectOp_ONEWAY       ((ObjectOp) 0x01000000u)
// #define ObjectOp_COALESCE     ((ObjectOp) 0x02000000u)
// #define ObjectOp_REALTIME     ((ObjectOp) 0x04000000u)

// Local ops are not forwarded by transports.

#define ObjectOp_LOCAL           ((ObjectOp) 0x00008000U)

#define ObjectOp_isLocal(op)     (((op) & ObjectOp_LOCAL) != 0)


//------------------------------------------------------------------------
// ObjectCounts
//
// The number and kinds of arguments passed to invoke are encoded in a
// 32-bit quantity `ObjectCounts`. Currently only 16-bits are used; the
// remainder are reserved for future enhancements.
//------------------------------------------------------------------------

typedef uint32_t ObjectCounts;

#define ObjectCounts_pack(nBuffersIn, nBuffersOut, nObjectsIn, nObjectsOut) \
   ((ObjectCounts) ((nBuffersIn) |         \
                    ((nBuffersOut) << 4) | \
                    ((nObjectsIn) << 8)  | \
                    ((nObjectsOut) << 12)))

#define ObjectCounts_maxBI   0xF
#define ObjectCounts_maxBO   0xF
#define ObjectCounts_maxOI   0xF
#define ObjectCounts_maxOO   0xF

// unpack counts

#define ObjectCounts_numBI(k)   ( (size_t) ( ((k) >> 0) & ObjectCounts_maxBI) )
#define ObjectCounts_numBO(k)   ( (size_t) ( ((k) >> 4) & ObjectCounts_maxBO) )
#define ObjectCounts_numOI(k)   ( (size_t) ( ((k) >> 8) & ObjectCounts_maxOI) )
#define ObjectCounts_numOO(k)   ( (size_t) ( ((k) >> 12) & ObjectCounts_maxOO) )
#define ObjectCounts_numBuffers(k)   (ObjectCounts_numBI(k) + ObjectCounts_numBO(k))
#define ObjectCounts_numObjects(k)   (ObjectCounts_numOI(k) + ObjectCounts_numOO(k))

// Indices into args[]

#define ObjectCounts_indexBI(k)   0
#define ObjectCounts_indexBO(k)   (ObjectCounts_indexBI(k) + ObjectCounts_numBI(k))
#define ObjectCounts_indexOI(k)   (ObjectCounts_indexBO(k) + ObjectCounts_numBO(k))
#define ObjectCounts_indexOO(k)   (ObjectCounts_indexOI(k) + ObjectCounts_numOI(k))
#define ObjectCounts_total(k)     (ObjectCounts_indexOO(k) + ObjectCounts_numOO(k))
#define ObjectCounts_indexBuffers(k)   ObjectCounts_indexBI(k)
#define ObjectCounts_indexObjects(k)   ObjectCounts_indexOI(k)


//------------------------------------------------------------------------
// Object
//------------------------------------------------------------------------

typedef struct Object Object;
typedef struct ObjectBuf ObjectBuf;
typedef struct ObjectBufIn ObjectBufIn;
typedef union ObjectArg ObjectArg;
typedef void *ObjectCxt;

typedef int32_t (*ObjectInvoke)(ObjectCxt h,
                                ObjectOp op,
                                ObjectArg *args,
                                ObjectCounts counts);

struct Object {
   ObjectInvoke invoke;
   ObjectCxt context;    // context data to pass to the invoke function
};

struct ObjectBuf {
   void *ptr;
   size_t size;
};

struct ObjectBufIn {
   const void *ptr;
   size_t size;
};

union ObjectArg {
   ObjectBuf b;
   ObjectBufIn bi;
   Object o;
};


static inline int32_t Object_invoke(Object o, ObjectOp op, ObjectArg *args, ObjectCounts k)
{
   return o.invoke(o.context, op, args, k);
}


#define Object_NULL                ( (Object) { NULL, NULL } )


//------------------------------------------------------------------------
// Object_invoke return codes
//------------------------------------------------------------------------

#define Object_isOK(err)        ((err) == 0)
#define Object_isERROR(err)     ((err) != 0)

// Generic error codes

#define Object_OK                  0   // non-specific success code
#define Object_ERROR               1   // non-specific error
#define Object_ERROR_INVALID       2   // unsupported/unrecognized request
#define Object_ERROR_SIZE_IN       3   // supplied buffer/string too large
#define Object_ERROR_SIZE_OUT      4   // supplied output buffer too small

#define Object_ERROR_USERBASE     10   // start of user-defined error range

// Transport layer error codes

#define Object_ERROR_DEFUNCT     -90   // object no longer exists
#define Object_ERROR_ABORT       -91   // calling thread must exit
#define Object_ERROR_BADOBJ      -92   // invalid object context
#define Object_ERROR_NOSLOTS     -93   // caller's object table full
#define Object_ERROR_MAXARGS     -94   // too many args
#define Object_ERROR_MAXDATA     -95   // buffers too large
#define Object_ERROR_UNAVAIL     -96   // the request could not be processed
#define Object_ERROR_KMEM        -97   // kernel out of memory
#define Object_ERROR_REMOTE      -98   // local method sent to remote object


//------------------------------------------------------------------------
// Base interface: `Object` also serves as the name of the interface
//     that all interfaces implicitly inherit.
//------------------------------------------------------------------------

#define Object_OP_release       (ObjectOp_METHOD_MASK - 0)
#define Object_OP_retain        (ObjectOp_METHOD_MASK - 1)

#define Object_release(o)       Object_invoke((o), Object_OP_release, 0, 0)
#define Object_retain(o)        Object_invoke((o), Object_OP_retain, 0, 0)


//----------------------------------------------------------------
// Utilities
//----------------------------------------------------------------

#define Object_isNull(o)           ( (o).invoke == NULL )


#define Object_RELEASE_IF(o)                                            \
   do { Object o_ = (o); if (!Object_isNull(o_)) (void) Object_release(o_); } while (0)

static inline void Object_replace(Object *loc, Object objNew)
{
  if (!Object_isNull(*loc)) {
    Object_release(*loc);
  }
  if (!Object_isNull(objNew)) {
    Object_retain(objNew);
 }
  *loc = objNew;
}

#define Object_ASSIGN(loc, obj)  Object_replace(&(loc), (obj))
#define Object_ASSIGN_NULL(loc)  Object_replace(&(loc), Object_NULL)

#define Object_INIT(loc, obj)                   \
  do {                                          \
    Object o_ = (obj);                          \
    (loc) = o_;                                 \
    if (!Object_isNull(o_)) {                   \
      Object_retain(o_);                        \
    }                                           \
  } while(0)

#endif /* __OBJECT_H */
