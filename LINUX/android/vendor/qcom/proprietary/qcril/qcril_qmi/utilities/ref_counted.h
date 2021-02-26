/*!
  @file
  ref_counted.h

  @brief
  Macros and utility functions to create a reference counted structure

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef REF_COUNTED_H_INCLUDED
#define REF_COUNTED_H_INCLUDED

struct ref_counted_struct;
typedef struct ref_counted_struct ref_counted;

typedef void (*ref_counted_delete_cb)(void *strct, ref_counted *rfcnt);

/**
  * Mark the structure containing this macro as reference-counted
  */
#define REF_COUNTED_DECL \
    ref_counted *__ref_counted

/**
  * Initialize a reference-counted structure, providing a delete callback,
  * which will be invoked when the reference count reaches 0
  */
#define REF_COUNTED_INIT(to_init_strct, to_init_delete_cb) \
({ \
    typeof(to_init_strct) _to_init_strct = (to_init_strct); \
    ref_counted_delete_cb _to_init_delete_cb = (to_init_delete_cb); \
    ref_counted_init(_to_init_strct, _to_init_strct ? &(_to_init_strct->__ref_counted) : NULL, _to_init_delete_cb); \
})

/**
  * Increment the reference count of a ref_counted object
  * If to_incr_strct is NULL, this is a no-op.
  */
#define REF_COUNTED_INCR(to_incr_strct) \
({ \
    typeof(to_incr_strct) _to_incr_strct = (to_incr_strct); \
    ref_counted *_to_incr_ref = _to_incr_strct ? _to_incr_strct->__ref_counted : NULL; \
    (typeof(to_incr_strct))ref_counted_incr(_to_incr_strct, _to_incr_ref); \
})

/**
  * Decrement the reference count of a ref_counted object
  * If to_decr_strct is NULL, this is a no-op.
  */
#define REF_COUNTED_DECR(to_decr_strct) \
({ \
    typeof(to_decr_strct) _to_decr_strct = (to_decr_strct); \
    ref_counted *_to_decr_ref = _to_decr_strct ? _to_decr_strct->__ref_counted : NULL; \
    ref_counted_decr(_to_decr_strct, _to_decr_ref); \
})

/**
  * Replace one ref-counted object with another,
  * increasing the ref count in the source (the
  * replacement) , and decrementing the ref count
  * in the destination (the object being replaced)
  * Both src and dst can be NULL, in which case,
  * only a simple assignment occurs.
  */
#define REF_COUNTED_REPL(rfcntd_dst, rfcntd_src) \
({ \
    typeof(rfcntd_src) _rfcntd_src = (rfcntd_src); \
    typeof(rfcntd_dst) _rfcntd_dst = (rfcntd_dst); \
    rfcntd_dst = REF_COUNTED_INCR(_rfcntd_src); \
    REF_COUNTED_DECR(_rfcntd_dst); \
})

#define REF_COUNTED_SET_DEFAULT_DELETE(strctptr) REF_COUNTED_SET_DELETE(strctptr, ref_counted_default_free )
#define REF_COUNTED_DEFAULT_DELETE (ref_counted_default_free)

void *ref_counted_incr(void *strct, ref_counted *rfcnt);
unsigned long ref_counted_decr(void *strct, ref_counted *rfcnt);
void ref_counted_init(void *strct, ref_counted **ref_cnt, ref_counted_delete_cb delete_cb);
void ref_counted_default_free(void *strct, ref_counted *rfcnt);
#endif
