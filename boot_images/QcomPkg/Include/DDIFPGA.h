#ifndef __DALIFPGA_H__
#define __DALIFPGA_H__

#include "DalDevice.h"
#include "DALStdErr.h"

#define DALFPGA_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef enum {
  DAL_FPGA_LED0,          /* Masks for 16 general purpose LEDs. */
  DAL_FPGA_LED1,
  DAL_FPGA_LED2,
  DAL_FPGA_LED3,
  DAL_FPGA_LED4,
  DAL_FPGA_LED5,
  DAL_FPGA_LED6,
  DAL_FPGA_LED7,
  DAL_FPGA_LED8,
  DAL_FPGA_LED9,
  DAL_FPGA_LED10,
  DAL_FPGA_LED11,
  DAL_FPGA_LED12,
  DAL_FPGA_LED13,
  DAL_FPGA_LED14,
  DAL_FPGA_LED15,
  DAL_FPGA_LED_MAX_NUMBER = 0xFFFF,
  DAL_PLACEHOLDER_DALFPGALedType = 0x7fffffff
}DALFPGALedType;

typedef enum {
  DAL_FPGA_PLATFORM_FFA,
  DAL_FPGA_PLATFORM_TRESTLES,
  DAL_FPGA_PLATFORM_LOWER_TRESTLES, 
  DAL_FPGA_PLATFORM_AERIE3, 
  DAL_FPGA_PLATFORM_AERIE2,
  DAL_FPGA_PLATFORM_AERIE1,
  DAL_FPGA_PLATFORM_MASTADON2,
  DAL_FPGA_PLATFORM_MASTADON,
  DAL_FPGA_PLATFORM_SUNDANCE2,
  DAL_FPGA_PLATFORM_KINNARY,
  DAL_FPGA_PLATFORM_GENERIC,
  DAL_FPGA_PLATFORM_SURF,
  DAL_FPGA_PLATFORM_MAX_NUMBER = 0xFFFF,
  DAL_PLACEHOLDER_DALFPGAPlatformType = 0x7fffffff
}DALFPGAPlatformType;

struct DalFPGAConfig
{
  uint32 register_addr;
  uint32 mask;
  uint32 value;
  uint32 bit_shift;
};

typedef struct DalFPGAConfig DalFPGAConfig;

typedef struct DalFPGA DalFPGA;
struct DalFPGA
{
   struct DalDevice DalDevice;
   DALResult (*SetLED)(DalDeviceHandle * _h, uint32  led);
   DALResult (*ClearLED)(DalDeviceHandle * _h, uint32  led);
   DALResult (*SetLEDMask)(DalDeviceHandle * _h, uint32  mask);
   DALResult (*ClearLEDMask)(DalDeviceHandle * _h, uint32  mask);
   DALResult (*ToggleLEDMask)(DalDeviceHandle * _h, uint32  mask);
   DALResult (*GetAddress)(DalDeviceHandle * _h, void *  name, uint32  size, void *  fpga, uint32  fpga_size, void *  obuf, uint32  olen);
   DALResult (*GetConfig)(DalDeviceHandle * _h, void *  name, uint32  size, void *  fpga, uint32  fpga_size, void *  obuf, uint32  olen);
   DALResult (*MaskedWrite)(DalDeviceHandle * _h, void *  fpga, uint32  ilen);
   DALResult (*MaskedRead)(DalDeviceHandle * _h, void *  fpga, uint32  ilen);
   DALResult (*GetPlatform)(DalDeviceHandle * _h, void *  platform, uint32  ilen);
};

typedef struct DalFPGAHandle DalFPGAHandle; 
struct DalFPGAHandle 
{
   uint32 dwDalHandleId;
   const DalFPGA * pVtbl;
   void * pClientCtxt;
};

#define DAL_FPGADeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALFPGA_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalFPGA_SetLED(DalDeviceHandle * _h, uint32  led)
{ 
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->SetLED( _h, led);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_ClearLED(DalDeviceHandle * _h, uint32  led)
{  
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->ClearLED( _h, led);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_SetLEDMask(DalDeviceHandle * _h, uint32  mask)
{
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->SetLEDMask( _h, mask);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_ClearLEDMask(DalDeviceHandle * _h, uint32  mask)
{ 
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->ClearLEDMask( _h, mask);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_ToggleLEDMask(DalDeviceHandle * _h, uint32  mask)
{  
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->ToggleLEDMask( _h, mask);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_GetAddress(DalDeviceHandle * _h, char* name,uint32* fpga)
{  
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->GetAddress( _h, (void *)name, 32, (void *)fpga, 1, (void *)NULL,0);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_GetConfig(DalDeviceHandle * _h, char*  name,  DalFPGAConfig*  fpga )
{ 
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->GetConfig( _h, (void*)name, 32, (void *)fpga, sizeof(DalFPGAConfig), (void *)NULL,0);
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_MaskedWrite(DalDeviceHandle * _h, DalFPGAConfig *  fpga)
{
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->MaskedWrite( _h, (void*)fpga, sizeof(DalFPGAConfig));
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_MaskedRead(DalDeviceHandle * _h, DalFPGAConfig *  fpga)
{
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->MaskedRead( _h, (void*)fpga, sizeof(DalFPGAConfig));
  }
  else
    return DAL_ERROR;
}
static __inline DALResult
DalFPGA_GetPlatform(DalDeviceHandle * _h, DALFPGAPlatformType *  platform)
{
  if(_h) 
  {
    return ((DalFPGAHandle *)_h)->pVtbl->GetPlatform( _h, (void *)platform, sizeof(1));
  }
  else
    return DAL_ERROR;
}
#endif
