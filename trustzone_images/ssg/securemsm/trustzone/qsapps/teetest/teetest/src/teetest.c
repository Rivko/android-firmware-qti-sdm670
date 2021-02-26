
/*===========================================================================
Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include <string.h>
#include <stringl.h>
#include "teetest.h"
#include "teetest_public.h"
#include "teetest_mmu.h"
#include "teetest_ipc.h"
#include "object.h"
#include "teetest_object_table.h"

#include "IDeviceID.h"
#include "IEnv.h"
#include "IPVC.h"

#include "CPVC.h"
#include "CCipher.h"
#include "ICipher.h"
#include "CUptime.h"
#include "IUptime.h"
#include "CDeviceID.h"
#include "CHdmiStatus.h"
#include "IHdmiStatus.h"

#include "CGuestVM_open.h"
#include "CEnv_open.h"


#define UNREFERENCED_PARAMETER(x) (void)(x)

typedef struct aes_vector_type
{
  uint8_t   alg;
  uint8_t   mode;
  uint8_t*  pt;
  size_t    pt_len;
  uint8_t*  key;
  size_t    key_len;
  uint8_t*  iv;
  size_t    iv_len;
  uint8_t*  ct;
  size_t    ct_len;
} __attribute__ ((packed)) aes_vector_type_t;

/*===========================================================================
 * Individual Test Data and Functions
 ===========================================================================*/
uint8_t aes_key_1[] =
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

/* AES-128 ECB test vectors */
uint8_t aes_plain_text_1[] =
{
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

uint8_t aes_cipher_text_1[] =
{
  0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
  0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

/* AES-128 CBC test vectors */
uint8_t aes_plain_text_2[] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f,
  0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93,
  0x17, 0x2a
};

uint8_t aes_key_2[] = {
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2,
  0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf,
  0x4f, 0x3c
};

uint8_t aes_iv_2[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
  0x0e, 0x0f
};

uint8_t aes_cipher_text_2[] = {
   0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
   0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};

/* AES-128 and AES-256 test vectors array */
aes_vector_type_t aes_test_vectors[] =
{
  {CCipherAES128_UID, ICipher_MODE_CTS, aes_plain_text_2,
   sizeof(aes_plain_text_2), aes_key_2, 16, aes_iv_2,
   16, aes_cipher_text_2, 16},

  {CCipherAES128_UID, ICipher_MODE_ECB, aes_plain_text_1,
   sizeof(aes_plain_text_1), aes_key_1, 16, NULL,
   16, aes_cipher_text_1, 16}
};

/*===========================================================================
 * External Data and Functions
 ===========================================================================*/

extern int tee_test_i2c_init(Object TEE_RemoteEnv_Object);
extern int tee_test_i2c(void);
extern int tee_test_i2c_cleanup(void);

extern int tee_test_crypto(void);

/*===========================================================================
 * Local Data and Functions
 ===========================================================================*/
static        void   tee_loop          (void);

void tee_test_c_entry(
  uintnt r0,
  uintnt r1,
  uintnt r2,
  uintnt r3)
{

  /* 'C' entry point for the TEE test application */

  /* Save the input parameters, we probably need these */
  tee_boot_info_t *boot_info = (tee_boot_info_t *)r0;
  uintnt           arch      = r1;
  UNREFERENCED_PARAMETER(      r2);
  UNREFERENCED_PARAMETER(      r3);

  uintptr_t tee_physical_load_address  = (uintptr_t)boot_info->ddr_mem.start;
  size_t    tee_length                 = (size_t)(boot_info->ddr_mem.end - boot_info->ddr_mem.start);
  ipc_addr  = r0;
  ipc_size  = SIZE_4KB;
  
  tee_mmu_init(tee_physical_load_address, tee_length, ipc_addr, ipc_size);
  
  /* At this point the TEE has booted and the MMU is enabled. Now we enter the
   * TEE test code */
  tee_loop();
}

static void tee_loop(void)
{
  int ret = Object_OK;
  volatile uint32_t loop_flag = 1;
  Object TEE_Outbound_Object;
  /* Prepare a local ENV object to handle inbounds*/
  Object TEE_LocalEnv_Object = CEnv_create(ENV_LOCAL);
  /* Add the ENV object to the object table */
  (void) tee_object_table_add(TEE_LocalEnv_Object);

  /* Prepare an opener object */
  Object TEE_RemoteEnv_Object = CEnv_create(ENV_REMOTE);

  /* Open the interface on the remote OS */
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CDeviceID_UID, &TEE_Outbound_Object));

  /* IDeviceID_readJtagID Test */
  uint32_t jtag_id = 0xDEAD;
  TEST_OK(IDeviceID_readJtagID(TEE_Outbound_Object, &jtag_id))

  if(jtag_id == 0xDEAD)
  {
    ret = Object_ERROR;
  }

  /* IDeviceID_getComponentVersion Test */
  uint8_t imp_version[128] = {0};
  size_t  imp_version_len  = sizeof(imp_version);
  TEST_OK(IDeviceID_getComponentVersion(TEE_Outbound_Object, IDeviceID_QSEE_VERSION, imp_version, imp_version_len, &imp_version_len));
  /* IDeviceID_getImplementationVersion Test with short buffer */
  imp_version_len = 100;
  TEST_NOT_OK(IDeviceID_getComponentVersion(TEE_Outbound_Object, IDeviceID_QSEE_VERSION, imp_version, imp_version_len, &imp_version_len));

  /* IEnv_log Test*/
  char   *log      = "Hello This is TEE";
  uint32_t log_size = strlen(log) + 1;
  TEST_OK(IEnv_log(TEE_RemoteEnv_Object, log, log_size);)

  /* IDeviceID_release Test */
  TEST_OK(IDeviceID_release(TEE_Outbound_Object))
  /* Bad IDeviceID_readJTAG Test */
  TEST_NOT_OK(IDeviceID_readJtagID(TEE_Outbound_Object, &jtag_id))

  /* IEnv_open Test */
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CDeviceID_UID, &TEE_Outbound_Object))
  /* Bad IOpener_open Test */
  TEST_NOT_OK(IOpener_open(TEE_Outbound_Object, CDeviceID_UID, &TEE_Outbound_Object))

  /* HDMI STATUS Test */
  // Skip controlled by SConscript based on CHIPSET
  #ifndef TEETEST_SKIP_HDMI
     uint32_t hdmiEnable, hdmiSense, hdcpAuth;
     TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CHdmiStatus_UID, &TEE_Outbound_Object))
     TEST_OK(IHdmiStatus_hdmiStatusRead(TEE_Outbound_Object, &hdmiEnable, &hdmiSense, &hdcpAuth))
     TEST_OK(IHdmiStatus_release(TEE_Outbound_Object))
  #endif

  uint8_t encrypted_key[100];
  aes_test_vectors[0].pt = &encrypted_key[0];
  aes_test_vectors[0].pt_len = sizeof(encrypted_key);

  /* IEnv Open Test */
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, aes_test_vectors[0].alg, &TEE_Outbound_Object))
  /* ICipher_setParamAsData Test */
  TEST_OK(ICipher_setParamAsData(TEE_Outbound_Object, ICipher_PARAM_KEY, aes_test_vectors[0].key, aes_test_vectors[0].key_len))
  /* ICipher_setParamAsU32 Test */
  TEST_OK(ICipher_setParamAsU32(TEE_Outbound_Object, ICipher_PARAM_MODE, aes_test_vectors[0].mode))
  /* ICipher_setParamAsU32 Test */
  TEST_OK(ICipher_setParamAsData(TEE_Outbound_Object, ICipher_PARAM_IV,  aes_test_vectors[0].iv, aes_test_vectors[0].iv_len))

  /* ICipher_encrypt Test */
  size_t ct_len_out;
  TEST_OK(ICipher_encrypt(TEE_Outbound_Object, aes_test_vectors[0].pt, aes_test_vectors[0].pt_len, aes_test_vectors[0].ct, aes_test_vectors[0].ct_len, &ct_len_out))

  /* ICipher_release Test */
  TEST_OK(ICipher_release(TEE_Outbound_Object))

  /* IUptime Test */
  uint64_t upTime;
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CUptime_UID, &TEE_Outbound_Object))
  /* IUptime_getUptime Test */
  TEST_OK(IUptime_getUptime(TEE_Outbound_Object, &upTime))
  /* IUptime_getUptimeUS Test */
  TEST_OK(IUptime_getUptimeUS(TEE_Outbound_Object, &upTime))
  TEST_OK(IUptime_release(TEE_Outbound_Object))

  /* I2C Test */
#if 0  
  TEST_OK(tee_test_i2c_init(TEE_RemoteEnv_Object));
  TEST_OK(tee_test_i2c());
  TEST_OK(tee_test_i2c_cleanup());
#endif

  /* Run the Crypto Tests */
  TEST_OK(tee_test_crypto());

  /* Initialization is now complete. Return, now waiting for an
   * SMC from the HLOS */
  tee_ipc_return_from_inbound_loading(ret);

  /* SMC has been received */
  while(loop_flag)
  {
    uint32_t smc_id = 0, param_id = 0, nargs = 0, object_handle;
    tee_syscall_entry_t *sc_entry = NULL;
    ObjectCounts k;
    Tee_ObjectArg *args;
    ObjectOp     op;

    smc_id        = (uint32_t     )    (smc[0] & 0xFFFFFFFFULL);
    object_handle = (uint32_t     )    (smc[1] & 0xFFFFFFFFULL);
    op            = (ObjectOp     )    (smc[2] & 0xFFFFFFFFULL);
    k             = (ObjectCounts )    (smc[3] & 0xFFFFFFFFULL);
    args          = (Tee_ObjectArg *)  (smc[4] & 0xFFFFFFFFULL);
    ret           = Object_OK;

    if (!IS_OWNER_TRUSTED_OS(TEE_SYSCALL_OWNER_ID(smc_id)) ||  smc_id != TEE_IPC_INBOUND_SMC_ID)
    {
      ret = Object_ERROR;
    }
    else
    {

      /* Clear the flag set when the TEE receives either an IRQ or an FIQ */
      interrupt_exit = 0;

      /* Allow interrupts to occur from this point onwards. If we exited last time with
       * interrupts disabled, the host may have had handled the interrupt and so TEE will
       * get no indication of the end of the interrupt. We therefore re-enable here on re-entry
       * each time */
      tee_int_enable();

      // Lookup the object from its handle
      const Object TEE_Inbound_Object = tee_object_table_recover(object_handle);

      if (Object_isNull(TEE_Inbound_Object))
      {
        ret = Object_ERROR_BADOBJ;
      }

      if(ret == Object_OK)
      {
        tee_ipc_inbound(args, k);

        /* execute the operation */
        ret = Object_invoke(TEE_Inbound_Object, op, (ObjectArg *)args, k);
      }
    }
    /* Return from the inbound SMC */
    tee_ipc_return_from_inbound(args, (ret == Object_OK) ? k : 0, ret);
  }
}

/* Panic loop!!! */
void tee_panic(void) {
  volatile int i = 1;
  while (i) {
    /* Wait here for the debugger!!! */
  }
}
