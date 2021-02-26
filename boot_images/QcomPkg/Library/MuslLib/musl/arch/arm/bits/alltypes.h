#ifndef MUSL_ALL_DEFS_H
#define MUSL_ALL_DEFS_H

#include <Base.h>

#define _Addr INTN
#define _Int64 long long 
#define _Reg INTN
#define wchar CHAR16

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;
typedef float float_t;
typedef double double_t;
typedef struct { long long __ll; long double __ld; } max_align_t;
typedef long time_t;
typedef long suseconds_t;
typedef struct { union { int __i[9]; unsigned __s[9]; } __u; } pthread_attr_t;
typedef struct { union { int __i[6]; volatile void *volatile __p[6]; } __u; } pthread_mutex_t;
typedef struct { union { int __i[6]; volatile void *volatile __p[6]; } __u; } mtx_t;
typedef struct { union { int __i[12]; void *__p[12]; } __u; } pthread_cond_t;
typedef struct { union { int __i[12]; void *__p[12]; } __u; } cnd_t;
typedef struct { union { int __i[8]; void *__p[8]; } __u; } pthread_rwlock_t;
typedef struct { union { int __i[5]; void *__p[5]; } __u; } pthread_barrier_t;
typedef UINTN size_t;
typedef UINTN uintptr_t;
typedef _Addr ptrdiff_t;
typedef _Addr ssize_t;
typedef _Addr intptr_t;
typedef _Addr regoff_t;
typedef _Reg register_t;
typedef signed char     int8_t;
typedef short           int16_t;
typedef int             int32_t;
typedef _Int64          int64_t;
typedef _Int64          intmax_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef UINT64 uint64_t;
typedef UINT64 u_int64_t;
typedef UINT64 uintmax_t;
typedef unsigned mode_t;
typedef UINTN nlink_t;
typedef _Int64 off_t;
typedef UINT64 ino_t;
typedef UINT64 dev_t;
typedef long blksize_t;
typedef _Int64 blkcnt_t;
typedef UINT64 fsblkcnt_t;
typedef UINT64 fsfilcnt_t;
typedef unsigned long wctype_t;
typedef void * timer_t;
typedef int clockid_t;
typedef long clock_t;
struct timeval { time_t tv_sec; suseconds_t tv_usec; };
struct timespec { time_t tv_sec; long tv_nsec; };
typedef int pid_t;
typedef unsigned id_t;
typedef unsigned uid_t;
typedef unsigned gid_t;
typedef int key_t;
typedef unsigned useconds_t;
typedef struct __pthread * pthread_t;
typedef int pthread_once_t;
typedef unsigned pthread_key_t;
typedef int pthread_spinlock_t;
typedef struct { unsigned __attr; } pthread_mutexattr_t;
typedef struct { unsigned __attr; } pthread_condattr_t;
typedef struct { unsigned __attr; } pthread_barrierattr_t;
typedef struct { unsigned __attr[2]; } pthread_rwlockattr_t;
typedef struct _IO_FILE FILE;
typedef unsigned wchar_t;
typedef unsigned wint_t;
typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;
typedef struct __locale_struct * locale_t;
typedef struct __sigset_t { unsigned long __bits[128/sizeof(long)]; } sigset_t;
struct iovec { void *iov_base; size_t iov_len; };
typedef unsigned socklen_t;
typedef unsigned short sa_family_t;
typedef unsigned char byte;

#undef _Addr
#undef _Int64
#undef _Reg

#endif
