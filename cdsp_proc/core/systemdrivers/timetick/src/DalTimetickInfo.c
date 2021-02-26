#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
Timetick_DalTimetick_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

const DALREG_DriverInfo 
DALTimetick_DriverInfo = { Timetick_DalTimetick_Attach,
     					   0,
						   NULL
                         };


