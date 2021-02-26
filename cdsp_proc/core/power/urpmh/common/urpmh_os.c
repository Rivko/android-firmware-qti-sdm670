#include "urpmh_os.h"
#include "CoreVerify.h"

void URPMH_CORE_VERIFY(uint32 val)
{
  CORE_VERIFY(val);	
}

void URPMH_CORE_VERIFY_PTR(void *val)
{
  CORE_VERIFY_PTR(val);	
}

uint32 urpmh_clz (uint32 num)
{
  int i;
  for (i = 0; i < 32; i++)
  {
      if ((num & 0x80000000) != 0)
      {
          return i;
      }
      num <<= 1;
  }
  return 32;
}

