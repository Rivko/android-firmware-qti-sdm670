#ifndef URPMH_OS_H
#define URPMH_OS_H

#include "comdef.h"

void URPMH_CORE_VERIFY(uint32 val);
void URPMH_CORE_VERIFY_PTR(void *val);

uint32 urpmh_clz (uint32 num);

#endif //URPMH_OS_H

