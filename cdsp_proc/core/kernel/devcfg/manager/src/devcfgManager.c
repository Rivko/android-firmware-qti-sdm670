#include "DALSysInt.h"
#include "stdlib.h"

extern void devcfg_blob_append(DALProps * newPropInfo, int bid);
extern DALProps DEVCFG_PROPS_INFO;

extern void DAL_setModInfo(DALProps * mod_info_addr, DALREG_DriverInfoList * global_drivers);
extern DALProps DAL_Mod_Info;
extern DALREG_DriverInfoList gDALModDriverInfoList;

void devcfgManagerInit()
{
   DALProps * dalpropsptr = &DEVCFG_PROPS_INFO;
   devcfg_blob_append(dalpropsptr, 0);

   DALProps * mod_info_ptr = &DAL_Mod_Info;
   DALREG_DriverInfoList * global_drivers = &gDALModDriverInfoList;
   DAL_setModInfo(mod_info_ptr, global_drivers);
}


