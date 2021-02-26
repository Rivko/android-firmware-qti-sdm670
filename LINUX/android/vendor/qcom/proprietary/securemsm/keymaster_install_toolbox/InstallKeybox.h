/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef INSTALLKEYBOX_H
#define INSTALLKEYBOX_H

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <keymaster_defs.h>
#include <UniquePtr.h>
#include <string>
#include <iomanip>
#include <vector>
#include <cstring>

// required for glibc compile
#include <limits.h>
#include <KeymasterKeyProvision.h>

using namespace keymasterdevice;
using namespace std;

namespace keymasterdevice {
#define MAX_KEYS_IN_KEYBOX  2

enum keybox_format {
    FORMAT_NONE,
    PEM
};

typedef enum {
    ERROR_OK = 0,
    GENERIC_FAILURE = -2000,
    ATTESTATION_VERIFICATION_FAILURE = -2001,
    DEVICE_ID_NOT_FOUND = -2002,
}keybox_error;

#define GUARD_EXIT(code)                     \
    do {\
       if (code != 0) {\
          return code;           \
       };\
    } while(0)

class InstallKeybox {
 public:
    InstallKeybox(std::string filename)
            : mFilename(filename), status(-1) {}
    ~InstallKeybox() {
        if(status) {
            KeymasterKeyProvision keymaster_provision;
            keymaster_provision.CleanProvisioningState();
            cout << "Destructor" << endl;
        }
    }

    int Run(const char *myDeviceID, const char *provision_device_id);

 private:

    int ConsumeTag(std::ifstream &inFile, const std::string &tag);
    std::string ConsumeElement(std::ifstream &inFile,
                               const std::string &startTag);
    int cleanKeyBox(km_install_keybox_t *keyboxCtx);

    int ProcessOneXmlKeybox(std::ifstream &inFile, const char *myDeviceID, bool provision_device_id);
    int provisionDeviceID(void);
    int ProcessDeviceID(std::ifstream &inFile, keymaster_blob_t *device_id);
    int ProcessKeyAlgo(std::ifstream &inFile,
                        keymaster_algorithm_t *algorithm);
    int ProcessKeyFormat(std::ifstream &inFile);
    int ProcessCertFormat(std::ifstream &inFile);
    int ProcessPrivKey(std::ifstream &inFile, keymaster_key_blob_t *priv_key);
    int ProcessCertificate(std::ifstream &inFile,
                            keymaster_blob_t *certificate);
    int cleanKeyBox();
    km_install_keybox_t keyboxCtx;
    std::string mFilename;
    std::string Brand;
    std::string Device;
    std::string Product;
    std::string SerialNum;
    std::string Manufacturer;
    std::string Model;
    std::string Imei;
    std::string Meid;
    int status;
};
}
;

#endif
