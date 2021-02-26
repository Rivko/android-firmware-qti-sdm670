#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
<driver>_<interface>_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID Dal<driver>_DeviceId[<driver_num_devices>] = {<device_id_list>};

const DALREG_DriverInfo 
DAL<driver>_DriverInfo = { <driver>_<interface>_Attach,
     					   <driver_num_devices>,
						   Dal<driver>_DeviceId
                         };


