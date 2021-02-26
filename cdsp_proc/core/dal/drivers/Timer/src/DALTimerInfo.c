#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult
Timer_DalTimer_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTimer_DeviceId[1] = {DALDEVICEID_TIMER};

const DALREG_DriverInfo
DALTimer_DriverInfo = { Timer_DalTimer_Attach,
                       1,
                       DalTimer_DeviceId
                       };

// Dummy placeholder for system device
const DALREG_DriverInfo
DALSystem_DriverInfo = { Timer_DalTimer_Attach,
                       1,
                       DalTimer_DeviceId
                       };


