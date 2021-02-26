/*==============================================================================
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "chre/util/nanoapp/app_id.h"
#include "chre_host/host_protocol_host.h"
#include "chre_host/fragmented_load_transaction.h"
#include "chre_host/log.h"
#include "chre_host/socket_client.h"
#include <chre/event.h>

#include "chre_app_oem.h"

#include <inttypes.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/eventfd.h>

#include <fstream>
#include <thread>

#include <cutils/sockets.h>
#include <utils/StrongPointer.h>


#define APP_ID_VENDOR_QUALCOMM UINT64_C(0x2051434f4d) // " QCOM"
#define APP_SEQ_ID_ANY UINT64_C(0xFFFFFF)
#define APP_ID_MAKE(vendor, app) ((((uint64_t)(vendor)) << 24) | ((app) & APP_SEQ_ID_ANY))

#define OEM_FILENAME     "chre_app_oem.so"
#define APP_OEM_APP_ID    APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 20)

int rspSuccess = 0;
static int rspEfd = 0;
static uint64_t rspVal = 0;;
static int initResponse(void)
{
  rspEfd = eventfd(0, 0);
  if (rspEfd == -1) {
    LOGE("failed to create eventfd");
    return -1;
  }
  return 0;
}
static int waitForResponse(void)
{
  LOGI("wait for response");
  read(rspEfd, &rspVal, sizeof(uint64_t));
  return rspSuccess;
}
static void gotResponse(int success)
{
  rspSuccess = success;
  rspVal = 1;
  write(rspEfd, &rspVal, sizeof(uint64_t));
}

static void pause(int secs)
{
  LOGI("pause");
  while (secs--) {
    sleep(1);
    LOGI("%d", secs);
  }
}

/**
 * @file
 * A test utility that connects to the CHRE daemon that runs on the apps
 * processor of MSM chipsets, which is used to help test basic functionality.
 */

using android::sp;
using android::chre::IChreMessageHandlers;
using android::chre::SocketClient;
using android::chre::HostProtocolHost;
using flatbuffers::FlatBufferBuilder;

// Aliased for consistency with the way these symbols are referenced in
// CHRE-side code
namespace fbs = ::chre::fbs;

namespace {

//! The host endpoint we use when sending; set to CHRE_HOST_ENDPOINT_UNSPECIFIED
constexpr uint16_t kHostEndpoint = 0xfffe;

class SocketCallbacks : public SocketClient::ICallbacks,
   public IChreMessageHandlers {
public:
  void onMessageReceived(const void* data, size_t length) override
  {
    if (!HostProtocolHost::decodeMessageFromChre(data, length, *this)) {
      LOGE("Failed to decode message");
    }
  }

  void onConnected(void) override
  {
    LOGI("Socket (re)connected");
  }

  void onConnectionAborted(void) override
  {
    LOGI("Socket (re)connection aborted");
  }

  void onDisconnected(void) override
  {
    LOGI("Socket disconnected");
  }

  void handleNanoappMessage(
  const ::chre::fbs::NanoappMessageT& app_message) override
  {
    int success = 1;
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    if (app_message.message_type >= CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE) {
      LOGI("Got message from nanoapp 0x%" PRIx64 " to endpoint 0x%" PRIx16
           " with type 0x%" PRIx32 " and length %zu", app_message.app_id, app_message.host_endpoint,
           app_message.message_type, app_message.message.size());

      if(app_message.message_type > CHRE_EVENT_SENSOR_FIRST_EVENT && app_message.message_type < CHRE_EVENT_SENSOR_LAST_EVENT) {
        // process message here 
        LOGI("%s: Sensor event: 0x%X", __func__, app_message.message_type);
      }
    } else {
      LOGE("callback with unknown message type %u, size %u, ignore", app_message.message_type, app_message.message.size());
    }
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(success);
  }

  void handleHubInfoResponse(
     const ::chre::fbs::HubInfoResponseT& app_info_response) override
  {
    const char *name = android::chre::getStringFromByteVector(app_info_response.name);
    const char *vendor = android::chre::getStringFromByteVector(app_info_response.vendor);
    const char *toolchain = android::chre::getStringFromByteVector(app_info_response.toolchain);

    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got hub info response:");
    LOGI("  Name: '%s'", name);
    LOGI("  Vendor: '%s'", vendor);
    LOGI("  Toolchain: '%s'", toolchain);
    LOGI("  Legacy versions: platform 0x%08" PRIx32 " toolchain 0x%08" PRIx32,
         app_info_response.platform_version, app_info_response.toolchain_version);
    LOGI("  MIPS %.2f Power (mW): stopped %.2f sleep %.2f peak %.2f",
         app_info_response.peak_mips, app_info_response.stopped_power, app_info_response.sleep_power, app_info_response.peak_power);
    LOGI("  Max message len: %" PRIu32, app_info_response.max_msg_len);
    LOGI("  Platform ID: 0x%016" PRIx64 " Version: 0x%08" PRIx32,
         app_info_response.platform_id, app_info_response.chre_platform_version);
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(1);
  }

  void handleNanoappListResponse(
     const fbs::NanoappListResponseT& response) override
  {
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got nanoapp list response with %zu apps:", response.nanoapps.size());
    for (const std::unique_ptr<fbs::NanoappListEntryT>& nanoapp
         : response.nanoapps){
      LOGI("  App ID 0x%016" PRIx64 " version 0x%" PRIx32 " enabled %d system "
           "%d", nanoapp->app_id, nanoapp->version, nanoapp->enabled,
           nanoapp->is_system);
    }
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(1);
  }

  void handleLoadNanoappResponse(
     const ::chre::fbs::LoadNanoappResponseT& response) override
  {
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got load nanoapp response, transaction ID 0x%" PRIx32 " result %d",
         response.transaction_id, response.success);
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(response.success);
  }

  void handleUnloadNanoappResponse(
     const ::chre::fbs::UnloadNanoappResponseT& response) override
  {
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got unload nanoapp response, transaction ID 0x%" PRIx32 " result %d",
         response.transaction_id, response.success);
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(response.success);
  }

};

int requestHubInfo(SocketClient& client)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeHubInfoRequest(builder);

  LOGI("Sending hub info request (%" PRIu32 " bytes)", builder.GetSize());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}

int requestNanoappList(SocketClient& client)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeNanoappListRequest(builder);

  LOGI("Sending app list request (%" PRIu32 " bytes)", builder.GetSize());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}


#ifndef CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#endif

int sendMessageToNanoapp(
   SocketClient& client, uint64_t appId, uint32_t messageType, uint8_t* msg, int len)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeNanoappMessage(
     builder, appId, CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + messageType,
     1234 /* transactionId */, msg, len);

  LOGI("Sending message to nanoapp (type 0x%X %" PRIu32 " bytes w/%zu bytes of payload)",
       messageType, builder.GetSize(), len);
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}

int sendLoadNanoappRequest(
   SocketClient& client, const char* filename, uint64_t appId,
   uint32_t appVersion)
{
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file) {
    LOGE("Couldn't open file '%s': %s", filename, strerror(errno));
    return 0;
  }
  ssize_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    LOGE("Couldn't read from file: %s", strerror(errno));
    return 0;
  }
  //android::chre::FragmentedLoadRequest request = android::chre::FragmentedLoadRequest(0,1,appId, appVersion, 0x01000000,size, buffer);
  const android::chre::FragmentedLoadRequest &rrequest(android::chre::FragmentedLoadRequest(0,1,appId, appVersion, 0x01000000,size, buffer));

  FlatBufferBuilder builder(128 + rrequest.binary.size());
  HostProtocolHost::encodeFragmentedLoadNanoappRequest(builder, rrequest);

  LOGI("Sending load nanoapp request (%" PRIu32 " bytes total w/%zu bytes of "
       "payload)", builder.GetSize(), buffer.size());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}

int sendUnloadNanoappRequest(SocketClient& client, uint64_t appId)
{
  FlatBufferBuilder builder(48);
  constexpr uint32_t kTransactionId = 4321;
  HostProtocolHost::encodeUnloadNanoappRequest(
     builder, kTransactionId, appId, true /* allowSystemNanoappUnload */);

  LOGI("Sending unload request for nanoapp 0x%016" PRIx64 " (size %" PRIu32 ")",
       appId, builder.GetSize());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}

}  // anonymous namespace


int main(void)
{
  int ret = 1;
  int config = 0;
  SocketClient client;
  sp<SocketCallbacks> callbacks = new SocketCallbacks();

  if (0 != initResponse()) {
    goto bail;
  }

  if (!client.connect("chre", callbacks)) {
    LOGE("Couldn't connect to socket");
    goto bail;
  }

  requestHubInfo(client);


  // cleanup just in case
  sendUnloadNanoappRequest(client, APP_OEM_APP_ID);


  if (!sendLoadNanoappRequest(client, OEM_FILENAME, APP_OEM_APP_ID, 0x0)) {
    LOGE("failed to load app_oem");
    goto bail;
  }

  if (!requestNanoappList(client)) {
    LOGE("failed to query apps");
    goto bail;
  }

  pause(3);

  if (!sendMessageToNanoapp(client, APP_OEM_APP_ID, APP_OEM_ECHO, 0, 0)) {
    LOGE("failed to send APP_OEM_ECHO");
    goto bail;
  }

  pause(3);

  config = 1;  //1
  if (!sendMessageToNanoapp(client, APP_OEM_APP_ID, APP_OEM_SENSOR_CONFIG, (uint8_t*)&config, sizeof(int))) {
    printf("failed to send APP_OEM_SENSOR_CONFIG config: %u \n", config);
    goto bail;
  }

  pause(3);

  config = 2;
  if (!sendMessageToNanoapp(client, APP_OEM_APP_ID, APP_OEM_SENSOR_CONFIG, (uint8_t*)&config, sizeof(int))) {
    LOGE("failed to send APP_OEM_SENSOR_CONFIG config: %u \n", config);
    goto bail;
  }
  pause(3);

  config = 0;  //0 Mode done.
  if (!sendMessageToNanoapp(client, APP_OEM_APP_ID, APP_OEM_SENSOR_CONFIG, (uint8_t*)&config, sizeof(int))) {
    printf("failed to send Mode Done\n");
    goto bail;
  }

  pause(1);

  printf("success\n");
  ret = 0;

bail:
  if (ret) {
    LOGE("Failed");
  }
  return 0;
}
