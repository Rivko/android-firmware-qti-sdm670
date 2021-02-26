#ifndef QURT_SRM_DRIVER_H
#define QURT_SRM_DRIVER_H

#include <qurt.h>

/*
||  Define qurt_srm_driver_t structure.  This represents
||   the "registration" object for an SRM driver.
*/

struct _qurt_srm_driver {
   const char *name;
   qurt_qdi_obj_t *obj;
};

typedef struct _qurt_srm_driver qurt_srm_driver_t;

/*
||  qurt_srm_object_invoke() is an internal equivalent to qurt_qdi_handle_invoke().
||  It behaves the same, but it takes a QDI object pointer instead of a handle.
*/

#define qurt_srm_object_invoke(o,m,...) \
   _QDMPASTE(_QDMSOI,_QDMCNT(QDI_HANDLE_LOCAL_CLIENT,o,m,##__VA_ARGS__))(QDI_HANDLE_LOCAL_CLIENT,o,m,##__VA_ARGS__)
#define _QDMSOI3(a,b,c) qurt_srm_oi3(a,b,c)
#define _QDMSOI4(a,b,c,d) qurt_srm_oi4(a,b,c,(int)(d))
#define _QDMSOI5(a,b,c,d,e) qurt_srm_oi5(a,b,c,(int)(d),(int)(e))
#define _QDMSOI6(a,b,c,d,e,f) qurt_srm_oi6(a,b,c,(int)(d),(int)(e),(int)(f))
#define _QDMSOI7(a,b,c,d,e,f,g) qurt_srm_oi7(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g))
#define _QDMSOI8(a,b,c,d,e,f,g,h) qurt_srm_oi8(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g),(int)(h))
#define _QDMSOI9(a,b,c,d,e,f,g,h,i) qurt_srm_oi9(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g),(int)(h),(int)(i))
#define _QDMSOI10(a,b,c,d,e,f,g,h,i,j) qurt_srm_oi10(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g),(int)(h),(int)(i),(int)(j))
#define _QDMSOI11(a,b,c,d,e,f,g,h,i,j,k) qurt_srm_oi11(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g),(int)(h),(int)(i),(int)(j),(int)(k))
#define _QDMSOI12(a,b,c,d,e,f,g,h,i,j,k,l) qurt_srm_oi12(a,b,c,(int)(d),(int)(e),(int)(f),(int)(g),(int)(h),(int)(i),(int)(j),(int)(k),(int)(l))

int qurt_srm_oi3(int, qurt_qdi_obj_t *, int);
int qurt_srm_oi4(int, qurt_qdi_obj_t *, int, int);
int qurt_srm_oi5(int, qurt_qdi_obj_t *, int, int, int);
int qurt_srm_oi6(int, qurt_qdi_obj_t *, int, int, int, int);
int qurt_srm_oi7(int, qurt_qdi_obj_t *, int, int, int, int, int);
int qurt_srm_oi8(int, qurt_qdi_obj_t *, int, int, int, int, int, int);
int qurt_srm_oi9(int, qurt_qdi_obj_t *, int, int, int, int, int, int, int);
int qurt_srm_oi10(int, qurt_qdi_obj_t *, int, int, int, int, int, int, int, int);
int qurt_srm_oi11(int, qurt_qdi_obj_t *, int, int, int, int, int, int, int, int, int);
int qurt_srm_oi12(int, qurt_qdi_obj_t *, int, int, int, int, int, int, int, int, int, int);

#define QDI_SRM_INIT 192

/*
||  QURT_SRM_DECLARE_DRIVER() is used to declare an SRM driver to the SRM infrastructure.
||
||  The three arguments are:
||   unique_id -- a unique C identifier, unused but must be a unique global symbol.
||   name -- the name of the driver by which an SRM client will attempt to open it.
||   obj -- pointer to the driver's "singleton" object which handles things like
||          initialization and QDI_OPEN requests.
*/

#define QURT_SRM_DECLARE_DRIVER(unique_id, xname, xobj) \
   __attribute__((section(".srm.rodata.user.main.DECL"))) const qurt_srm_driver_t unique_id = \
      { .name = xname, .obj = xobj }

int qurt_srm_mapping_create(int client_handle,
                            unsigned *pageno_virt,
                            unsigned pageno_phys,
                            unsigned page_count,
                            qurt_mem_cache_mode_t cache_attr,
                            qurt_perm_t perm);

#endif /* QURT_SRM_DRIVER_H */
