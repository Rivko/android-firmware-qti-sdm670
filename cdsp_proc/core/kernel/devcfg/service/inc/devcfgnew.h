#ifndef DEVCFGNEW_H
#define DEVCFGNEW_H

#include "DALSysInt.h"
//#include "DevCfg_ImgParams.h"
//#include "stdlib.h"
//#ifndef DEVCFG_STANDALONE
//    #include "stringl/stringl.h"
//#endif

void devcfg_init_new(void);
void devcfg_blob_append(DALProps * newPropInfo, int bid);
void devcfg_blob_prepend(DALProps * newPropInfo, int bid);
DALProps * devcfgGetBlobByID(int bid);
#endif
