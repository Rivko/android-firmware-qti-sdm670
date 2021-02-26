#ifndef __DALREMOTETEST_H__
#define __DALREMOTETEST_H__

#include "DalDevice.h"

#define DALREMOTETEST_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

#define DALREMOTETEST_REGULAR_EVENT 0
#define DALREMOTETEST_PAYLOAD_EVENT 1

#define DALREMOTETEST_INPUT_UINT1_VALUE  0x01010101
#define DALREMOTETEST_INPUT_UINT2_VALUE  0x20202020
#define DALREMOTETEST_INPUT_UINT3_VALUE  0x12121212

#define DALREMOTETEST_INPUT_HANDLE_VALUE  0xDA1FDA1F

#define DALREMOTETEST_OUTPUT_UINT_VALUE 0xBEEFDEAD

#define DALREMOTETEST_ERROR_BAD_PARAM      0x10

#define DALREMOTETEST_MAX_BUF_SIZE_BYTES(nParams)  512-sizeof(DALRemoteMsg)-((nParams-1)*sizeof(uint32))
#define DALREMOTETEST_MAX_BUF_SIZE_UINT32(nParams) (512-sizeof(DALRemoteMsg)-((nParams-1)*sizeof(uint32)))/sizeof(uint32)


typedef struct RemoteTestStruct RemoteTestStruct;
struct RemoteTestStruct
{
   DALSYSEventHandle hRegularEvent;
   uint32 test[32];
   DALSYSEventHandle hPayloadEvent;
};

DALResult UnitTestFcn0(DalDeviceHandle * h, uint32 u1);

DALResult UnitTestFcn1(DalDeviceHandle * h, uint32 u1, uint32 u2);

DALResult UnitTestFcn2(DalDeviceHandle * h, uint32 u1, uint32* p_u2);

DALResult UnitTestFcn3(DalDeviceHandle * h, uint32 u1, uint32 u2, uint32 u3);

DALResult UnitTestFcn4(DalDeviceHandle * h, uint32 u1, uint32 u2, uint32* p_u3 );

DALResult UnitTestFcn5(DalDeviceHandle * h, 
                       /*istruct*/ const RemoteTestStruct * test_struct);

DALResult UnitTestFcn6(DalDeviceHandle * h, 
                       uint32 u1, 
                       /*ibuf*/ const uint32 * ibuf, uint32 ilen);

DALResult UnitTestFcn7(DalDeviceHandle * h,
                       /*istruct*/ const RemoteTestStruct * test_struct, 
                       /*obuf*/ uint32 * obuf, uint32 olen, uint32 *oalen);

DALResult UnitTestFcn8(DalDeviceHandle * h,
                       /*ibuf*/ const byte * ibuf, uint32 ilen, 
                       /*ostruct*/ RemoteTestStruct * test_struct);

DALResult UnitTestFcn9(DalDeviceHandle * h, 
                       /*obuf*/ byte * obuf, uint32 olen );

DALResult UnitTestFcn10(DalDeviceHandle * h, 
                        uint32 s1, 
                        /*ibuf*/ const byte * ibuf, uint32 ilen, 
                        /*obuf*/ byte * obuf, uint32 olen, uint32 * oalen);

DALResult UnitTestFcn11(DalDeviceHandle * h,
                        uint32 s1, 
                        /*obuf*/ byte * obuf, uint32 olen);

DALResult UnitTestFcn12(DalDeviceHandle * h, 
                        uint32 s1, 
                        /*obuf*/ byte * obuf, uint32 olen, uint32 *oalen);

DALResult UnitTestFcn13(DalDeviceHandle * h, 
                        /*ibuf*/const uint32 * ibuf, uint32 ilen, 
                        /*istruct*/ const RemoteTestStruct * ip_test_struct, 
                        /*ostruct*/ RemoteTestStruct * op_test_struct);

DALResult UnitTestFcn14(DalDeviceHandle * h,
                        /*ibuf*/uint32 * ibuf, uint32 ilen, 
                        /*obuf*/ byte * obuf1, uint32 olen1,
                        /*obuf*/ byte * obuf2, uint32 olen2, uint32 *oalen2);

DALResult UnitTestFcn15(DalDeviceHandle * h, 
                        /*ibuf*/ const uint32 * ibuf, uint32 ilen, 
                        /*istruct*/ const RemoteTestStruct * ip_test_struct, 
                        /*obuf*/ byte * obuf, uint32 olen, uint32 *oalen, 
                        /*ostruct*/ RemoteTestStruct * op_test_struct );

DALResult UnitTestEventConfig(DalDeviceHandle * h, 
                              /*istruct*/ const RemoteTestStruct * ip_test_struct);
                              
  
DALResult UnitTestEventTrigger(DalDeviceHandle * h, 
                               uint32 event_idx);
                                 



typedef struct DalRemoteTest DalRemoteTest;
struct DalRemoteTest
{
   struct DalDevice DalDevice;
   DALResult (*UnitTestFcn0)(DalDeviceHandle * _h, uint32  u1);
   DALResult (*UnitTestFcn1)(DalDeviceHandle * _h, uint32  u1, uint32  u2);
   DALResult (*UnitTestFcn2)(DalDeviceHandle * _h, uint32  u1, uint32*  p_u2);
   DALResult (*UnitTestFcn3)(DalDeviceHandle * _h, uint32  u1, uint32  u2, uint32  u3);
   DALResult (*UnitTestFcn4)(DalDeviceHandle * _h, uint32  u1, uint32  u2, uint32*  p_u3);
   DALResult (*UnitTestFcn5)(DalDeviceHandle * _h, void * test_struct, uint32 ilen1);
   DALResult (*UnitTestFcn6)(DalDeviceHandle * _h, uint32  u1, void * ibuf, uint32  ilen);
   DALResult (*UnitTestFcn7)(DalDeviceHandle * _h, void * test_struct, uint32 ilen1, void * obuf, uint32  olen, uint32 * oalen);
   DALResult (*UnitTestFcn8)(DalDeviceHandle * _h, void * ibuf, uint32  ilen, void * test_struct, uint32 olen3);
   DALResult (*UnitTestFcn9)(DalDeviceHandle * _h, void * obuf, uint32  olen);
   DALResult (*UnitTestFcn10)(DalDeviceHandle * _h, uint32  s1, void * ibuf, uint32  ilen, void * obuf, uint32  olen, uint32 *  oalen);
   DALResult (*UnitTestFcn11)(DalDeviceHandle * _h, uint32  s1, void * obuf, uint32  olen);
   DALResult (*UnitTestFcn12)(DalDeviceHandle * _h, uint32  s1, void * obuf, uint32  olen, uint32 * oalen);
   DALResult (*UnitTestFcn13)(DalDeviceHandle * _h, void * ibuf, uint32  ilen, void * ip_test_struct, uint32 ilen3, void * op_test_struct, uint32 olen4);
   DALResult (*UnitTestFcn14)(DalDeviceHandle * _h, void * ibuf, uint32  ilen, void * obuf1, uint32  olen1, void * obuf2, uint32  olen2, uint32 * oalen2);
   DALResult (*UnitTestFcn15)(DalDeviceHandle * _h, void * ibuf, uint32  ilen, void * ip_test_struct, uint32 ilen3, void * obuf, uint32  olen, uint32 * oalen, void * op_test_struct, uint32 olen7);
   DALResult (*UnitTestEventConfig)(DalDeviceHandle * _h, void * ip_test_struct, uint32 ilen1);
   DALResult (*UnitTestEventTrigger)(DalDeviceHandle * _h, uint32  event_idx);
};

typedef struct DalRemoteTestHandle DalRemoteTestHandle; 
struct DalRemoteTestHandle 
{
   uint32 dwDalHandleId;
   const DalRemoteTest * pVtbl;
   void * pClientCtxt;
};

#define DAL_RemoteTestDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALREMOTETEST_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalRemoteTest_UnitTestFcn0(DalDeviceHandle * _h, uint32  u1)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn0 ), _h, u1);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn0( _h, u1);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn1(DalDeviceHandle * _h, uint32  u1, uint32  u2)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn1 ), _h, u1, u2);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn1( _h, u1, u2);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn2(DalDeviceHandle * _h, uint32  u1, uint32*  p_u2)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn2 ), _h, u1, p_u2);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn2( _h, u1, p_u2);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn3(DalDeviceHandle * _h, uint32  u1, uint32  u2, uint32  u3)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_3(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn3 ), _h, u1, u2, u3);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn3( _h, u1, u2, u3);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn4(DalDeviceHandle * _h, uint32  u1, uint32  u2, uint32*  p_u3)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_4(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn4 ), _h, u1, u2, p_u3);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn4( _h, u1, u2, p_u3);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn5(DalDeviceHandle * _h,  const RemoteTestStruct *  test_struct)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn5 ), _h, (void * )test_struct,sizeof(  RemoteTestStruct  ));
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn5( _h, (void * )test_struct,sizeof(  RemoteTestStruct  ));
}
static __inline DALResult
DalRemoteTest_UnitTestFcn6(DalDeviceHandle * _h, uint32  u1,  const uint32 *  ibuf, uint32  ilen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn6 ), _h, u1, (void * )ibuf, ilen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn6( _h, u1, (void * )ibuf, ilen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn7(DalDeviceHandle * _h,  const RemoteTestStruct *  test_struct,  uint32 *  obuf, uint32  olen, uint32 * oalen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_7(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn7 ), _h, (void * )test_struct,sizeof(  RemoteTestStruct  ), (void * )obuf, olen, oalen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn7( _h, (void * )test_struct,sizeof(  RemoteTestStruct  ), (void * )obuf, olen, oalen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn8(DalDeviceHandle * _h,  const byte *  ibuf, uint32  ilen,  RemoteTestStruct *  test_struct)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_8(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn8 ), _h, (void * )ibuf, ilen, (void * )test_struct,sizeof( RemoteTestStruct  ));
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn8( _h, (void * )ibuf, ilen, (void * )test_struct,sizeof( RemoteTestStruct  ));
}
static __inline DALResult
DalRemoteTest_UnitTestFcn9(DalDeviceHandle * _h,  byte *  obuf, uint32  olen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_9(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn9 ), _h, (void * )obuf, olen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn9( _h, (void * )obuf, olen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn10(DalDeviceHandle * _h, uint32  s1,  const byte *  ibuf, uint32  ilen,  byte *  obuf, uint32  olen, uint32 *  oalen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_10(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn10 ), _h, s1, (void * )ibuf, ilen, (void * )obuf, olen, oalen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn10( _h, s1, (void * )ibuf, ilen, (void * )obuf, olen, oalen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn11(DalDeviceHandle * _h, uint32  s1,  byte *  obuf, uint32  olen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_11(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn11 ), _h, s1, (void * )obuf, olen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn11( _h, s1, (void * )obuf, olen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn12(DalDeviceHandle * _h, uint32  s1,  byte *  obuf, uint32  olen, uint32 * oalen)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_12(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn12 ), _h, s1, (void * )obuf, olen, oalen);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn12( _h, s1, (void * )obuf, olen, oalen);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn13(DalDeviceHandle * _h, const uint32 *  ibuf, uint32  ilen,  const RemoteTestStruct *  ip_test_struct,  RemoteTestStruct *  op_test_struct)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_13(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn13 ), _h, (void * )ibuf, ilen, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ), (void * )op_test_struct,sizeof( RemoteTestStruct  ));
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn13( _h, (void * )ibuf, ilen, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ), (void * )op_test_struct,sizeof( RemoteTestStruct  ));
}
static __inline DALResult
DalRemoteTest_UnitTestFcn14(DalDeviceHandle * _h, const uint32 *  ibuf, uint32  ilen,  byte *  obuf1, uint32  olen1,  byte *  obuf2, uint32  olen2, uint32 * oalen2)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_14(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn14 ), _h, (void * )ibuf, ilen, (void * )obuf1, olen1, (void * )obuf2, olen2, oalen2);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn14( _h, (void * )ibuf, ilen, (void * )obuf1, olen1, (void * )obuf2, olen2, oalen2);
}
static __inline DALResult
DalRemoteTest_UnitTestFcn15(DalDeviceHandle * _h,  const uint32 *  ibuf, uint32  ilen,  const RemoteTestStruct *  ip_test_struct,  byte *  obuf, uint32  olen, uint32 * oalen,  RemoteTestStruct *  op_test_struct)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_15(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestFcn15 ), _h, (void * )ibuf, ilen, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ), (void * )obuf, olen, oalen, (void * )op_test_struct,sizeof( RemoteTestStruct  ));
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestFcn15( _h, (void * )ibuf, ilen, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ), (void * )obuf, olen, oalen, (void * )op_test_struct,sizeof( RemoteTestStruct  ));
}
static __inline DALResult
DalRemoteTest_UnitTestEventConfig(DalDeviceHandle * _h,  const RemoteTestStruct *  ip_test_struct)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestEventConfig ), _h, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ));
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestEventConfig( _h, (void * )ip_test_struct,sizeof(  RemoteTestStruct  ));
}
static __inline DALResult
DalRemoteTest_UnitTestEventTrigger(DalDeviceHandle * _h, uint32  event_idx)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalRemoteTestHandle *)_h)->pVtbl, UnitTestEventTrigger ), _h, event_idx);
   }
   return ((DalRemoteTestHandle *)_h)->pVtbl->UnitTestEventTrigger( _h, event_idx);
}
#endif
