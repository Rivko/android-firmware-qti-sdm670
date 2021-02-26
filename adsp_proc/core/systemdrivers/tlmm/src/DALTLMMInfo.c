#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
TLMM_DalTlmm_AttachEx(const char *pszArg,
                      DALDEVICEID DeviceId, 
                      DalDeviceHandle **phDalDevice,
                      int nClientId);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTLMM_DeviceId[1] = {DALDEVICEID_TLMM};

const DALREG_DriverInfo 
DALTLMM_DriverInfo = { {.pfnDALNewFuncEx = TLMM_DalTlmm_AttachEx}, 1, DalTLMM_DeviceId };

