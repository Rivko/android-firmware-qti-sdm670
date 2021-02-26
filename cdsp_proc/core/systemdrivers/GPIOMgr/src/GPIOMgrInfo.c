#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
GPIOMgr_GPIOMgrIF_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

const DALREG_DriverInfo 
DALGPIOMgr_DriverInfo = { GPIOMgr_GPIOMgrIF_Attach, 0, NULL };


