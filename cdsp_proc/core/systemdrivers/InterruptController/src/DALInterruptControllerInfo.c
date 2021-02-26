#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
InterruptController_DalInterruptController_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalInterruptController_DeviceId[1] = {DALDEVICEID_INTERRUPTCONTROLLER};

const DALREG_DriverInfo 
DALInterruptController_DriverInfo = { InterruptController_DalInterruptController_Attach,
     					   1,
						   DalInterruptController_DeviceId
                         };


