/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
 PVCLicense Client app.
*********************************************************************/

#include <utils/Log.h>
#include <unistd.h>
#include "TZCom.h"
#include "IClientEnv.h"
#include "CPVCLicense.h"
#include "IPVCLicense.h"
#include "object.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PVCLICENSE_SAMPLE: "


/* Dummy PVC License.
 *
 * WARNING: THIS LICENSE WILL UNLOCK THE PVC FEATURE ONLY ON NON-PRODUCTION
 *          DEVICES.
 *          License validation will take place on non-production as well as
 *          production devices, and the result will be reported as the last
 *          parameter in the call to IPVCLicense_setHavenLicense().
 *
 *          The PVC feature though will be enabled, in non-production devices,
 *          REGARDLESS OF THE OUTCOME OF THE VALIDATION.
 *          I.e., on non-production devices, ANY LICENSE BLOB passed to
 *          IPVCLicense_setHavenLicense() will successfully enable the PVC
 *          feature.
 * */
static const uint8_t license[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

/* How to run as daemon:
 *
 * - modify the Android.mk and uncomment the following line:
 *   #LOCAL_CFLAGS += -DPVCLICENSE_SAMPLE_DAEMON
 *
 * - add this program to one of the init.rc for the target platform (e.g. init.target.rc)
 *
 *    service pvclicense_sample /vendor/bin/pvclicense_sample
 *      class main
 *      user system
 *      group system
 *      oneshot
 *
 * - add an SEPolicy to allow it to execute on boot and access the smcinvoke driver:
 *
 *   Change an existing file_context for the platform:
 *      /vendor/bin/pvclicense_sample                   u:object_r:pvclicense_sample_exec:s0
 *
 *   Create a new policy file:
 *
 *    type pvclicense_sample, domain;
 *    type pvclicense_sample_exec, exec_type, vendor_file_type, file_type;
 *    init_daemon_domain(pvclicense_sample)
 *    allow pvclicense_sample smcinvoke_device:chr_file rw_file_perms;
 *
 * */


int main(int argc, char *argv[])
{
  Object clientEnv      = Object_NULL;
  Object rootEnv        = Object_NULL;
  Object pvcLicense     = Object_NULL;
  int ret               = Object_OK;
  int havenErr          = Object_OK;
  uint8_t credentials[] = {0xa1, 0x01, 0x19, 0x03, 0xe8};
  bool runAsDaemon      = false;

#ifdef PVCLICENSE_SAMPLE_DAEMON
  runAsDaemon = true;
#endif

  if (runAsDaemon) {
    // obtain the root environment
    ret = TZCom_getRootEnvObject(&rootEnv);
    if (Object_isERROR(ret)) {
      ALOGE("FAILED to obtain root environment: %d (0x%08X)", ret, ret);
      return ret;
    }
    // obtain our client environment
    ret = IClientEnv_register(rootEnv, credentials, sizeof(credentials), &clientEnv);
    if (Object_isERROR(ret)) {
      Object_release(rootEnv);
      ALOGE("FAILED to obtain client environment: %d (0x%08X)", ret, ret);
      return ret;
    }
  } else {
    // obtain our client environment
    ret = TZCom_getClientEnvObject(&clientEnv);
    if (Object_isERROR(ret)) {
      ALOGE("FAILED to obtain client environment: %d (0x%08X)", ret, ret);
      return ret;
    }
  }

  do {

    // obtain the proxy object for the PVCLicense service
    ret = IClientEnv_open(clientEnv, CPVCLicense_UID, &pvcLicense);
    if (Object_isERROR(ret)) {
      ALOGE("FAILED to obtain pvcLicense object: %d (0x%08X)", ret, ret);
      pvcLicense = Object_NULL;
      break;
    }

    do {
      // pass the Haven License to enable PVC
      ret = IPVCLicense_setHavenLicense(pvcLicense, license, sizeof(license), &havenErr);
      if (ret == IPVCLicense_ERROR_HAVEN_LICENSE_SERVICE_NOT_AVAILABLE) {
        ALOGD("Waiting for License validation service to be ready");
        usleep(100*1000); // retry in 100ms;
      }
    } while (ret == IPVCLicense_ERROR_HAVEN_LICENSE_SERVICE_NOT_AVAILABLE);

    if (Object_isERROR(ret)) {
      ALOGE("FAILED to set the PVC license: %d (0x%08X)", ret, ret);
      break;
    }

    // check the return code from the license validation
    if (Object_isERROR(havenErr)) {
      ALOGE("Haven License validation for PVC failed: %d (0x%08X)", havenErr, havenErr);
      break;
    }

    ALOGW("*** WARNING *** : DUMMY PVC LICENSE SUPPLIED");
    ALOGW("*** WARNING *** : NOT VALID ON PRODUCTION DEVICES");

    ALOGD("PVC License validation SUCCEEDED");
  } while (0);

  // tear down the environment
  Object_RELEASE_IF(pvcLicense);
  Object_RELEASE_IF(clientEnv);
  Object_RELEASE_IF(rootEnv);
  return ret;
}
