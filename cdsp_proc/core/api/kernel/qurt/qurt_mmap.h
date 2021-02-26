#ifndef QURT_MMAP_H
#define QURT_MMAP_H

/*
||  Prototype for QuRT extension of the mmap(), mprotect(), munmap() APIs.
*/

void *qurt_mem_mmap(int client_handle,
                    qurt_mem_pool_t pool,
                    qurt_mem_region_t *pRegion,
                    void *addr,
                    size_t length,
                    int prot,
                    int flags,
                    int fd,
                    unsigned long long offset);

int qurt_mem_mprotect(const void *addr,
                      size_t length,
                      int prot);

int qurt_mem_munmap(void *addr,
                    size_t length);

/*
|| The macros here follow the style of the standard mmap() macros, but with
||  QURT_ prepended to avoid name conflicts, and to avoid having a dependency
||  on sys/mman.h.
||
|| Wherever possible, any values here which are also present in sys/mman.h
||  should have the same value in both places so that we can accept "mmap"
||  calls without having to remap parameters to new values.
||
|| In the future, it would be desirable to have a regression test which
||  checks, for instance, that these macros match.  Example:
||
||   assert(QURT_MAP_FAILED == MAP_FAILED);
||   ... repeat as needed ...
*/

#define QURT_PROT_NONE                  0x00
#define QURT_PROT_READ                  0x01
#define QURT_PROT_WRITE                 0x02
#define QURT_PROT_EXEC                  0x04

#define QURT_MAP_SHARED                 0x0001
#define QURT_MAP_PRIVATE                0x0002
#define QURT_MAP_FIXED                  0x0010
#define QURT_MAP_RENAME                 0x0020
#define QURT_MAP_NORESERVE              0x0040
#define QURT_MAP_INHERIT                0x0080
#define QURT_MAP_HASSEMAPHORE           0x0200
#define QURT_MAP_TRYFIXED               0x0400
#define QURT_MAP_WIRED                  0x0800
#define QURT_MAP_FILE                   0x0000
#define QURT_MAP_ANON                   0x1000
#define QURT_MAP_ALIGNED(n)             ((n) << QURT_MAP_ALIGNMENT_SHIFT)
#define QURT_MAP_ALIGNMENT_SHIFT        24
#define QURT_MAP_ALIGNMENT_MASK         QURT_MAP_ALIGNED(0xff)
#define QURT_MAP_ALIGNMENT_64KB         QURT_MAP_ALIGNED(16)
#define QURT_MAP_ALIGNMENT_16MB         QURT_MAP_ALIGNED(24)
#define QURT_MAP_ALIGNMENT_4GB          QURT_MAP_ALIGNED(32)
#define QURT_MAP_ALIGNMENT_1TB          QURT_MAP_ALIGNED(40)
#define QURT_MAP_ALIGNMENT_256TB        QURT_MAP_ALIGNED(48)
#define QURT_MAP_ALIGNMENT_64PB         QURT_MAP_ALIGNED(56)
#define QURT_MAP_FAILED                 ((void *) -1)

/*
|| The macros below are extensions beyond the standard mmap flags, but follow
||  the style of the mmap flags.
*/

// Describe bitfields in (prot)
#define QURT_PROT_CACHE_BOUNDS          16,19,7         /* Bits 16 through 19 are cache attribute, default 7 */
#define QURT_PROT_BUS_BOUNDS            20,21,0         /* Bits 20 through 21 are bus attributes, default 0 */
#define QURT_PROT_USER_BOUNDS           22,23,3         /* Bits 22 through 23 are user mode, default 3;
                                                           default=3 means to derive user mode setting from the
                                                           default mode of the client... */

// Describe bitfields in (flags)
#define QURT_MAP_PHYSADDR_BOUNDS        15,15,0         /* Bits 15 through 15 are physaddr, default 0 */
#define QURT_MAP_TYPE_BOUNDS            16,19,0         /* Bits 16 through 19 are mapping type, default 0 */
#define QURT_MAP_REGION_BOUNDS          20,23,0         /* Bits 20 through 23 are region type, default 0 */

// These macros get OR'ed into (prot)
#define QURT_PROT_CACHE_MODE(n)         QURT_MMAP_BUILD(QURT_PROT_CACHE_BOUNDS,n)
#define QURT_PROT_BUS_ATTR(n)           QURT_MMAP_BUILD(QURT_PROT_BUS_BOUNDS,n)
#define QURT_PROT_USER_MODE(n)          QURT_MMAP_BUILD(QURT_PROT_USER_BOUNDS,n)

// These macros get OR'ed into (flags)
#define QURT_MAP_PHYSADDR               QURT_MMAP_BUILD(QURT_MAP_PHYSADDR_BOUNDS,1)
#define QURT_MAP_TYPE(n)                QURT_MMAP_BUILD(QURT_MAP_TYPE_BOUNDS,n)
#define QURT_MAP_REGION(n)              QURT_MMAP_BUILD(QURT_MAP_REGION_BOUNDS,n)

// These macros extract fields from (prot)
#define QURT_PROT_GET_CACHE_MODE(n)     QURT_MMAP_EXTRACT(QURT_PROT_CACHE_BOUNDS,n)
#define QURT_PROT_GET_BUS_ATTR(n)       QURT_MMAP_EXTRACT(QURT_PROT_BUS_BOUNDS,n)
#define QURT_PROT_GET_USER_MODE(n)      QURT_MMAP_EXTRACT(QURT_PROT_USER_BOUNDS,n)

// These macros extract fields from (flags)
#define QURT_MAP_GET_TYPE(n)            QURT_MMAP_EXTRACT(QURT_MAP_TYPE_BOUNDS,n)
#define QURT_MAP_GET_REGION(n)          QURT_MMAP_EXTRACT(QURT_MAP_REGION_BOUNDS,n)

// Macros for bitfield insertion and extraction
#define QURT_MMAP_MASK(lo,hi)           (~((~0u) << ((hi)-(lo)+1)))       /* Mask of same size as [lo..hi] */
#define QURT_MMAP_BUILD_(lo,hi,def,n)   ((((n)^(def))&QURT_MMAP_MASK(lo,hi))<<(lo))
#define QURT_MMAP_EXTRACT_(lo,hi,def,n) ((((n)>>(lo))&QURT_MMAP_MASK(lo,hi))^(def))
#define QURT_MMAP_BUILD(a,b)            QURT_MMAP_BUILD_(a,b)
#define QURT_MMAP_EXTRACT(a,b)          QURT_MMAP_EXTRACT_(a,b)

#endif
