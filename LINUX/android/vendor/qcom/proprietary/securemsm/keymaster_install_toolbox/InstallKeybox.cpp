/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include <InstallKeybox.h>
#include <VerifyAttestation.h>

using namespace std;
using namespace keymaster;

namespace keymasterdevice {
#ifdef PRINT_FUNCTIONS
static void PrintCertData(keymaster_blob_t *key_blob) {
    uint8_t *keyboxData = (uint8_t *) key_blob->data;
    cerr << "key_blob->data_length" << key_blob->data_length << endl;
    for (int i = 0; i < (int) key_blob->data_length; i++) {
        char buf[4];
        if (i % 16 == 0)
            cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}

static void PrintKeyData(keymaster_key_blob_t *key_blob) {
    uint8_t *keyboxData = (uint8_t *) key_blob->key_material;
    cerr << "key_blob->key_material_size" << key_blob->key_material_size
         << endl;
    for (int i = 0; i < (int) key_blob->key_material_size; i++) {
        char buf[4];
        if (i % 16 == 0)
            cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}
#endif

int InstallKeybox::ConsumeTag(ifstream &inFile, const string &tag) {
    string input;

    getline(inFile, input, '<');
    getline(inFile, input, '>');
    if (input.compare(tag) != 0) {
        cerr << "Corrupt input, expected tag: '" << tag << "' found '" << input
                << "'" << endl;
        return GENERIC_FAILURE;
    }
    return ERROR_OK;
}

string InstallKeybox::ConsumeElement(ifstream &inFile, const string &startTag) {
    string result;

    ConsumeTag(inFile, startTag);

    getline(inFile, result, '<');
    inFile.putback('<');

    string endTag = "/" + startTag;
    ConsumeTag(inFile, endTag);
    return result;
}

int InstallKeybox::ProcessDeviceID(ifstream &inFile,
                                    keymaster_blob_t *device_id) {
    string input;
    size_t input_length = 0;

    //Seek and discard the first quotation mark in device id
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Keybox DeviceID=") != 0) {
        cerr << "Failed to find 'Keybox DeviceID= in input" << endl;
        return GENERIC_FAILURE;
    }

    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    input_length = input.size();
    device_id->data = (uint8_t*) malloc(input_length);
    if (!device_id->data) {
        cerr << "device_id->data NULL" << endl;
        return GENERIC_FAILURE;
    }

    memcpy((void *) device_id->data, &input[0], input_length);
    device_id->data_length = input_length;

    //Discard '>' at end of <Keybox DeviceID="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::ProcessKeyAlgo(ifstream &inFile,
                                   keymaster_algorithm_t *algorithm) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Key algorithm=") != 0) {
        cerr << "Failed to find 'Key algorithm in input" << endl;
        return GENERIC_FAILURE;
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    //Report current key algorithm and save it off

    if (input.compare("ecdsa") == 0) {
        *algorithm = KM_ALGORITHM_EC;
    } else if (input.compare("rsa") == 0) {
        *algorithm = KM_ALGORITHM_RSA;
    } else {
        cerr << "KeyAlgorithm is not 'ecdsa' or 'rsa' in input" << endl;
        return GENERIC_FAILURE;
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::ProcessKeyFormat(ifstream &inFile) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("PrivateKey format=") != 0) {
        cerr << "Failed to find 'PrivateKey format' in input" << endl;
        return GENERIC_FAILURE;
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    //Report current key format and save it off
    if (input.compare("pem") == 0) {
    } else {
        cerr << "KeyFormat is not 'pem' in input" << endl;
        return GENERIC_FAILURE;
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::ProcessCertFormat(ifstream &inFile) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("Certificate format=") != 0) {
        cerr << "Failed to find 'Certificate format' in input" << endl;
        return GENERIC_FAILURE;
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    if (input.compare("pem") == 0) {
    } else {
        cerr << "certFormat is not 'pem' in input" << endl;
        return GENERIC_FAILURE;
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::ProcessPrivKey(ifstream &inFile,
                                   keymaster_key_blob_t *priv_key) {
    string input;

    // extract the key content and store them in input 
    getline(inFile, input);
    getline(inFile, input, '<');

    //copy hex of key content in private key data
    priv_key->key_material = (uint8_t*) malloc(input.size());
    if (!priv_key->key_material) {
        cerr << "priv_key->key_material NULL" << endl;
        return GENERIC_FAILURE;
    }
    priv_key->key_material_size = 0;
    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size())
            break;
        memcpy((uint8_t *) priv_key->key_material + offset, input1.c_str(),
               input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t *) priv_key->key_material + offset + input1.size(),
               0x0A, 1);
        priv_key->key_material_size += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
    return ERROR_OK;
}

int InstallKeybox::ProcessCertificate(ifstream &inFile,
                                       keymaster_blob_t *certificate) {
    string input;

    // extract the key content and store them in input 
    getline(inFile, input);
    getline(inFile, input, '<');

    certificate->data = (uint8_t*) malloc(input.size());
    if (!certificate->data) {
        cerr << "certificate->data" << endl;
        return GENERIC_FAILURE;
    }
    certificate->data_length = 0;

    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size())
            break;
        memcpy((uint8_t *) certificate->data + offset, input1.c_str(),
               input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t *) certificate->data + offset + input1.size(), 0xA, 1);
        certificate->data_length += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
    return ERROR_OK;
}

int InstallKeybox::ProcessOneXmlKeybox(ifstream &inFile,
                                       const char *myDeviceID,
                                       bool provision_device_id) {
    string input;
    int certCount = 0;
    int keyitem = 0, j;
    keymaster_blob_t* cert_entry;

    memset(&keyboxCtx, 0, sizeof(keyboxCtx));

    //proces device ID:  <Keybox DeviceID=mfg_mod123_0000001>
    GUARD_EXIT(ProcessDeviceID(inFile, &(keyboxCtx.device_id)));
    cerr << "myDeviceID: " << myDeviceID << endl;
    cerr << "keyboxCtx.device_id.data: " << keyboxCtx.device_id.data << endl;

    for (keyitem = 0; keyitem < MAX_KEYS_IN_KEYBOX; keyitem++) {
        //proces key algorithm:  <<Key algorithm=ecdsa"> or <Key algorithm=rsa>
        GUARD_EXIT(ProcessKeyAlgo(inFile, &(keyboxCtx.keybox[keyitem].algo)));
        //proces key format:  <PrivateKey format=pem>
        GUARD_EXIT(ProcessKeyFormat(inFile));
        //process key content  
        GUARD_EXIT(ProcessPrivKey(inFile, &(keyboxCtx.keybox[keyitem].key)));

        //consume tag: <CertificateChain>
        GUARD_EXIT(ConsumeTag(inFile, "CertificateChain"));
        //process number of certificates: <NumberOfCertificates>2</NumberOfCertificates> 
        certCount = atol(ConsumeElement(inFile, "NumberOfCertificates").c_str());

        if (!certCount) {
            cerr << "certCount value is not valid" << endl;
            return GENERIC_FAILURE;
        }

        //allocate certcount number of keymaster_blob_t type of space. 
        //certificate and key shares the same data structure keymaster_blob_t
        keyboxCtx.keybox[keyitem].cert_chain.entry_count = certCount;

        if ((uint32_t)certCount > UINT_MAX / (uint32_t)sizeof(keymaster_blob_t)) {
            cerr << "Value too large to allocate memory" << endl;
            return GENERIC_FAILURE;
        }

        keyboxCtx.keybox[keyitem].cert_chain.entries = (keymaster_blob_t *) malloc(
                        certCount * sizeof(keymaster_blob_t));
        if (!keyboxCtx.keybox[keyitem].cert_chain.entries) {
            cerr << "keyboxCtx.keybox[keyitem].cert_chain.entries NULL"
                    << keyitem << endl;
            return GENERIC_FAILURE;
        }

        cert_entry = keyboxCtx.keybox[keyitem].cert_chain.entries;
        for (j = 0; j < certCount; j++) {
            //proces certificate format:  <Certificate format=pem>
            GUARD_EXIT(ProcessCertFormat(inFile));
            //process certificate content
            GUARD_EXIT(ProcessCertificate(inFile, cert_entry));
            //consume tag: /Certificate
            //ConsumeTag(inFile, "/Certificate");
            cert_entry++;
        }
        //consume tag: /CertificateChain
        GUARD_EXIT(ConsumeTag(inFile, "/CertificateChain"));
        //consume tag: /Key
        GUARD_EXIT(ConsumeTag(inFile, "/Key"));
    }
    //consume tag: /Keybox
    GUARD_EXIT(ConsumeTag(inFile, "/Keybox"));
    //current Max keys in keybox is 2, one for ECDSA, another one for RSA
    if (!memcmp(keyboxCtx.device_id.data, myDeviceID, keyboxCtx.device_id.data_length)) {
        KeymasterKeyProvision keymaster_provision;

        int result = keymaster_provision.KeyMasterProvisionInit();
        if (result) {
            cerr << "KeyMasterProvisionInit error" << result << endl;
            return GENERIC_FAILURE;
        }

        result = keymaster_provision.KeyMasterInstallKeybox(&keyboxCtx);
        if (result) {
            cerr << "KeyMasterInstallKeybox error" << result << endl;
            return GENERIC_FAILURE;
        }

        cerr << "KeyMaster Attestation Key Provisioning success for KeyID" << myDeviceID << endl;
        //cleanKeyBox(&keyboxCtx);

        VerifyAttestation dev(provision_device_id, Brand, Device, Product, SerialNum, Manufacturer, Model, Imei, Meid);
        keymaster_error_t ret = dev.RsaAttestation();
        if(ret) {
            cout << "RsaAttestation failed " << ret << endl;
            return ret;
        }

        ret = dev.EcAttestation();
        if(ret) {
            cout << "EcAttestation failed " << ret << endl;
            return ret;
        }

        result = keymaster_provision.SetProvisioniningSuccess();
        if (result) {
            cerr << "SetProvisioniningSuccess error" << result << endl;
            return result;
        }
        return ERROR_OK;
    } else {
        return DEVICE_ID_NOT_FOUND;
    }
    //cleanKeyBox(&keyboxCtx);
}

static volatile void *secure_memset(volatile void *dst, int c, size_t len) {
  volatile char *buf;

  for (buf = (volatile char *)dst;  len;  buf[--len] = c);
  return dst;
}

int InstallKeybox::cleanKeyBox()
{
    int keysCount = MAX_KEYS_IN_KEYBOX;

    if (keyboxCtx.device_id.data) {
        free((void *) keyboxCtx.device_id.data);
        keyboxCtx.device_id.data = NULL;
    }

    while ((keysCount--) && (keysCount < INT_MAX)) {
        if (keyboxCtx.keybox[keysCount].key.key_material)
            free((void *) keyboxCtx.keybox[keysCount].key.key_material);
        keymaster_free_cert_chain(&keyboxCtx.keybox[keysCount].cert_chain);
    }
    volatile uint8_t *temp = (uint8_t *)&keyboxCtx;
    secure_memset((volatile void *)temp, 0, sizeof(km_install_keybox_t));
    return ERROR_OK;
}

int InstallKeybox::Run(const char *myDeviceID, const char *provision_device_id_str) {
    ifstream inFile(mFilename.c_str());
    string input;
    int keyCount = 0;
    int ret = GENERIC_FAILURE;
    bool provision_device_id = false;

    if(!memcmp(provision_device_id_str, "true", strlen("true")))
        provision_device_id = true;
    else
        provision_device_id = false;

    cerr << "provision_device_id" << provision_device_id << endl;
    if(provision_device_id)
        GUARD_EXIT(provisionDeviceID());

    if (inFile.is_open()) {
        GUARD_EXIT(ConsumeTag(inFile, "?xml version=\"1.0\"?"));
        GUARD_EXIT(ConsumeTag(inFile, "AndroidAttestation"));
        keyCount = atol(ConsumeElement(inFile, "NumberOfKeyboxes").c_str());
        cout << "Number of keyboxes " << keyCount << endl;

        while ((keyCount--) && (keyCount < INT_MAX)) {
            ret = ProcessOneXmlKeybox(inFile, myDeviceID, provision_device_id);
            cleanKeyBox();
            if(ret != DEVICE_ID_NOT_FOUND) {
                break;
            }
        }

        if(ret == DEVICE_ID_NOT_FOUND) {
            //consume tag: /AndroidAttestation
            GUARD_EXIT(ConsumeTag(inFile, "/AndroidAttestation"));
            cerr << "Keybox for '" << myDeviceID << "' not found!" << endl;
        } else if (ret == 0) {
            status = 0;
        }
    } else {
        cerr << "ERROR: Unable to open xml keyfile!" << endl;
        ret = GENERIC_FAILURE;
    }
    inFile.close();
    return ret;
}
}  //end of namespace keymasterdevice.

using namespace keymasterdevice;
int main(int argc, char **argv) {

    if ((argc != 4) ||
          ((memcmp(argv[3], "true", strlen("true"))) &&
           (memcmp(argv[3], "false", strlen("false"))))) {
        cout << "usage: LD_LIBRARY_PATH=/vendor/lib64/hw KmInstallKeybox Keybox_file Device_ID provision_device_ids(true/false)" << endl;
        cout << "e.g.,: LD_LIBRARY_PATH=/vendor/lib64/hw KmInstallKeybox keybox.xml mfgID_xxxx_0000 true" << endl;
        cout << "KmInstallKeybox will install the attestation"
                "keys (both RSA and ECC) along with all the device identifiers\n"
                "(brand, device, product, serial#, IMEI, MEID (if available), model"
                "and manufacturer).\n"
                "\n"
                "The tool will use KM HAL API's to genrate RSA / ECC keys and\n"
                "verify the attestation certificate signed with the provisioned keys.\n"
                "If the validation is successful, the tool will set 'Provisioning Success'"
                "to KM TA.\n"
                "Else all the provisioned keys are deleted and it can be re-provisioned"
                "\n"
                "Once 'Provisioning Success' state is set in KM TA,\n"
                "attestation keys / Device ID's cannot be re-provisioned\n"
                "\n"
                "\n"
                "**RMA Use Case **\n"
                "For RMA use case, OEM's can set the following devcfg parameter in 'keymaster_oem_config.xml'\n"
                "Please ensure to sign this debug devcfg with the serial# of the device as this will enable\n"
                "re-provisioning of keys and if used incorrectly can open up the device to be re-provisioned.\n"
                "<props name=\"allow_reprovision\" type=DALPROP_ATTR_TYPE_UINT32>\n"
                "1" << endl;


        exit(-1);
    }
    InstallKeybox installer(argv[1]);
    int ret = installer.Run(argv[2], argv[3]);
    if(ret) {
        cout << "InstallKeybox Failed!" << endl;
    } else {
        cout << "InstallKeybox is done!" << endl;
    }

    return ret;
}

