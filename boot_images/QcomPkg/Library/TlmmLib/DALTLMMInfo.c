#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
TLMM_DalTlmm_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTLMM_DeviceId[2] = {DALDEVICEID_TLMM,
                                          DALDEVICEID_TLMM_MASTER};

const DALREG_DriverInfo 
DALTLMM_DriverInfo = { TLMM_DalTlmm_Attach,
     					   2,
						   DalTLMM_DeviceId
                         };


