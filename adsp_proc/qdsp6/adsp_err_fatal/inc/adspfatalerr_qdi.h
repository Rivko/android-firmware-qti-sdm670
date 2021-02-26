#ifndef ADSPFATALERR_QDI_H_
#define ADSPFATALERR_QDI_H_

#include "qurt.h"
#include "qurt_qdi_driver.h"


/**< Device name for QDI instance*/
#define ADSP_FATALERR_DRV_NAME        "/adsp/fatalerr"
/**< QDI exposed methods */
#define ADSP_FATAL_ERROR            (QDI_PRIVATE+0)

qurt_qdi_obj_t fatalerr_qdiobj;

typedef struct QDI_Adspfatalerr_Ctx_t{
    qurt_qdi_obj_t qdiobj;
} QDI_Adspfatalerr_Ctx_t;

typedef struct{
    qurt_qdi_obj_t qdiobj;
} QDI_Adspfatalerr_Opener_t;;

void QDI_adspfatalerr_init(void);




#endif
