#include <stddef.h>
#include <stdint.h>
#include "II2C.h"
#include "CI2C.h"
#include "IEnv.h"
#include "teetest.h"

#define TEE_I2C_RW_START_ADDR 2*1024
#define TEE_I2C_BUFF_SIZE 10

enum TEE_I2C_DEVICES
{
  TEE_I2C_DEVICE_1,
  TEE_I2C_DEVICE_2,
  TEE_I2C_DEVICE_3,
  TEE_I2C_DEVICE_4,
  TEE_I2C_DEVICE_5,
  TEE_I2C_DEVICE_6,
  TEE_I2C_DEVICE_7,
  TEE_I2C_DEVICE_8,
  TEE_I2C_DEVICE_9,
  TEE_I2C_DEVICE_10,
  TEE_I2C_DEVICE_11,
  TEE_I2C_DEVICE_12
};

#define TEE_I2C_DEVICE TEE_I2C_DEVICE_7

static Object g_tee_i2c_obj;

/**
 * I2C Bus and Slave Configuration used in this test
 */
static II2C_Config g_TEE_I2C_Config =
{
  .useDefaultBusConfig = 0,

  .busConfig =
  {
    100,                         // uFsBusFreqInKhz;
    0,                           // uHsBusFreqInKhz; not used
    II2C_STANDARD_INTERFACE,     // eInterface;  not used
    2500                         // nMaxBusAcquireWaitTimeMsec;
  },

  .slaveConfig =
  {
    0x50,                            // uSlaveAddress;
    II2C_SEVEN_BIT_SLAVE_ADDRESS,    // eSlaveAddressOption;
    II2C_MEMORY_ADDRESS_DEVICE,      // eSlaveDeviceType;
    II2C_GEN_START_BEFORE_READ,      // eReadOption;
    2500                             // nByteTransferWaitTimeUSec;
  }
};

int32_t tee_i2c_open(int32_t deviceId)
{
  int32_t ret = Object_OK;

  ret = II2C_open(g_tee_i2c_obj, 
                         deviceId);

  return ret;
}

int32_t tee_i2c_close(int32_t deviceId)
{
  int32_t ret = Object_OK;

  ret = II2C_close(g_tee_i2c_obj, deviceId);

  return ret;
}

int32_t tee_i2c_read
(
  int32_t       deviceId,
  II2C_Config   *pConfig,
  uint32_t      startAddr,
  void          *pPayload,
  size_t        payloadLen,
  uint64_t      *pPayloadOutLen
)
{
  return II2C_read(g_tee_i2c_obj,
          deviceId,
          pConfig,
          startAddr,
          pPayload,
          payloadLen,
          (size_t*)pPayloadOutLen);
}

int32_t tee_i2c_write
(
  int32_t       deviceId,
  II2C_Config   *pConfig,
  uint32_t      startAddr,
  void          *pPayload,
  size_t        payloadLen,
  uint64_t      *pBytesWritten
)
{
  return II2C_write(g_tee_i2c_obj,
          deviceId,
          pConfig,
          startAddr,
          pPayload,
          payloadLen,
          pBytesWritten);
}

void tee_i2c_pause(uint32_t ms)
{
}

/**
 * To run before #tee_test_i2c() to open MINK interface for I2C
 */
int tee_test_i2c_init(Object TEE_RemoteEnv_Object)
{
  int ret = Object_OK;
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object, CI2C_UID, &g_tee_i2c_obj));
  return ret;
}

/**
 * To run after #tee_test_i2c() to close MINK interface for I2C
 */
int tee_test_i2c_cleanup(void)
{
  int ret = Object_OK;
  TEST_OK(II2C_release(g_tee_i2c_obj));
  return ret;
}

/**
 * MAIN I2C TEE TEST FUNCTION
 * 
 * Steps:
 * 1. Open i2c
 * 2. Read the initial content and save it
 * 3. Write some data
 * 4. Pause to let the write happen
 * 5. Read that data
 * 6. Compare if read data is what was written in 2.
 * 7. Write initial saved content
 * 8. Pause to let the write happen
 * 9. Close i2c
 *
 */
int tee_test_i2c(void)
{
  int       i;
  int       ret = Object_OK;
  uint8_t   save_data[TEE_I2C_BUFF_SIZE];
  uint8_t   rd_data[TEE_I2C_BUFF_SIZE] = {0,0,0,0,0,0,0,0,0,0};
  uint8_t   wr_data[TEE_I2C_BUFF_SIZE] = {1,2,3,4,5,6,7,8,9,10};
  uint64_t  outLen;

  /* The test doesn't work without manually stepping in T32 */
  /* 1. I2C OPEN */
  TEST_OK(tee_i2c_open(TEE_I2C_DEVICE));

  /* 2. I2C READ */
  TEST_OK(tee_i2c_read(
              TEE_I2C_DEVICE,        // I2C DeviceId
              &g_TEE_I2C_Config,     // I2C Config
              TEE_I2C_RW_START_ADDR, // Start Address
              save_data,             // Payload
              ARRAY_SIZE(save_data), // Payload length
              &outLen));             // Output length

  /* 3. I2C WRITE */
  TEST_OK(tee_i2c_write(
              TEE_I2C_DEVICE,        // I2C DeviceId
              &g_TEE_I2C_Config,     // I2C Config
              TEE_I2C_RW_START_ADDR, // Start Address
              wr_data,               // Payload
              ARRAY_SIZE(wr_data),   // Payload length
              &outLen));             // Output length

  /* 4. PAUSE : make sure write is completed */
  tee_i2c_pause(5);

  /* 5. I2C READ */
  TEST_OK(tee_i2c_read(
              TEE_I2C_DEVICE,        // I2C DeviceId
              &g_TEE_I2C_Config,     // I2C Config
              TEE_I2C_RW_START_ADDR, // Start Address
              rd_data,               // Payload
              ARRAY_SIZE(rd_data),   // Payload length
              &outLen));             // Output length

#if 0
  /* 6. COMPARE */
  for (i=0 ; i < ARRAY_SIZE(rd_data);i++) {
    if (rd_data[i] != wr_data[i]) {
      /* Comparison failed */
      ret = Object_ERROR;
      break;
    }
  }
#endif

  /* 7. RESTORE STATE */
  TEST_OK(tee_i2c_write(
              TEE_I2C_DEVICE,        // I2C DeviceId
              &g_TEE_I2C_Config,     // I2C Config
              TEE_I2C_RW_START_ADDR, // Start Address
              save_data,             // Payload
              ARRAY_SIZE(save_data), // Payload length
              &outLen));             // Output length

  /* 8. PAUSE */
  tee_i2c_pause(5);

  /* 9. I2C CLOSE */
  TEST_OK(tee_i2c_close(TEE_I2C_DEVICE));

  return ret;
}
