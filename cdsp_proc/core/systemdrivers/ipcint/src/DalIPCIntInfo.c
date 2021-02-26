#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
IPCInt_DalIPCInt_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalIPCInt_DeviceId[1] = {DALDEVICEID_IPCINT};

const DALREG_DriverInfo 
DALIPCInt_DriverInfo = { IPCInt_DalIPCInt_Attach,
     					   1,
						   DalIPCInt_DeviceId
                         };


