# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from collections import defaultdict
import glob
import os
import subprocess
import sys
import tempfile
import zipfile

from sectools.common.utils import c_misc, c_path
from sectools.common.utils.c_logging import logger
from sectools.features.isc.cfgparser import HmacParams


MESG_MULTIPLE_ZIP = "Multiple ZIP files exist in {0}. Please PLACE ONE ZIP FILE only\n"
MESG_SIG_NOT_FOUND = "Signature output not found in {0}\n"

class SigFiles(object):
    attest_cert_id = '-attestation_cert.cer'
    attest_ca_cert_id = '-attestation_ca_cert.cer'
    root_cert_prefix = '-root_cert'
    signature_id = '-signature.bin'

    def __init__(self, signature=None,
                        attestation_cert=None,
                        attestation_ca_cert=None,
                        root_cert_list=None):
        self.signature = signature
        self.attestation_cert = attestation_cert
        self.attestation_ca_cert = attestation_ca_cert
        self.root_cert_list = root_cert_list

def _findMatched(list, matched_substring, returnAll=False):
    matched_list = []
    returnVal = matched_list

    for eachfile in list:
        if isinstance(eachfile, zipfile.ZipInfo):
            find = eachfile.filename.find
            matched_info = eachfile.filename
        else:
            find = eachfile.find
            matched_info = eachfile

        if find(matched_substring) >= 0:
            matched_list.append(matched_info)
            if returnAll is True:
                continue
            else:
                break

    if returnAll is False:
        if len(matched_list) == 0:
            returnVal = None
        else:
            returnVal = matched_list[0]

    return returnVal

def readFilesFromZip(zipfp,
                     signature,
                     attestation_cert,
                     attestation_ca_cert,
                     root_cert_list):
    cert_chain_list = []
    cert_chain_list.append(zipfp.read(attestation_cert))
    if attestation_ca_cert is not None:
        cert_chain_list.append(zipfp.read(attestation_ca_cert))
    for root_cert in root_cert_list:
        cert_chain_list.append(zipfp.read(root_cert))

    return [zipfp.read(signature), cert_chain_list]

def sortRootCertList(root_cert_list, root_cert_prefix):
    sorted_list = []

    if len(root_cert_list) == 1:
        sorted_list = root_cert_list
    else:
        for i in range (0, len(root_cert_list)):
            substring = "{0}{1}.cer".format(root_cert_prefix, i)
            matched_file = _findMatched(root_cert_list, substring)
            if matched_file is None:
                raise RuntimeError, "Expected root ends with %s is not in the zip package" % substring
            sorted_list.append(matched_file)

    return sorted_list

def getSigFilesFromZip(zipfilename):
    zipfp = zipfile.ZipFile(zipfilename, "r")
    unzipped_infolist = zipfp.infolist()

    sigFiles = SigFiles()
    sigFiles.attestation_cert = _findMatched(unzipped_infolist, sigFiles.attest_cert_id)
    if (sigFiles.attestation_cert == None):
        raise RuntimeError("Attestation cert is not found in {0}".format(zipfilename))

    sigFiles.attestation_ca_cert = _findMatched(unzipped_infolist, sigFiles.attest_ca_cert_id)

    sigFiles.signature = _findMatched(unzipped_infolist, sigFiles.signature_id)
    if (sigFiles.signature == None):
        raise RuntimeError("Signature is not found in {0}".format(zipfilename))

    root_cert_list = _findMatched(unzipped_infolist,
                                  sigFiles.root_cert_prefix,
                                  returnAll=True)
    if (len(root_cert_list) == 0):
        raise RuntimeError("Root cert is not found in {0}".format(zipfilename))

    sigFiles.root_cert_list = sortRootCertList(root_cert_list, sigFiles.root_cert_prefix)

    return [zipfp, sigFiles]

def getSigPackage(cert_folder):
    from sectools.features.isc.signer.base_signer import ExternalSignerError
    sig_package = None
    zipfiles = glob.glob(c_path.join(cert_folder, '*.zip'))
    if len(zipfiles) == 1:
        sig_package = zipfiles[0]
    elif len(zipfiles) > 1:
        raise ExternalSignerError(MESG_MULTIPLE_ZIP.format(cert_folder))
    return sig_package

def readSigFromZip(zipfilename):
    from sectools.features.isc.signer.base_signer import ExternalSignerError
    if os.path.isfile(zipfilename) is False:
        raise ExternalSignerError(MESG_SIG_NOT_FOUND.format(zipfilename))

    [zipfp, sigFiles] = getSigFilesFromZip(zipfilename)

    [signature, cert_chain_list] = readFilesFromZip(zipfp,
                                     sigFiles.signature,
                                     sigFiles.attestation_cert,
                                     sigFiles.attestation_ca_cert,
                                     sigFiles.root_cert_list)
    zipfp.close()

    return [signature, cert_chain_list]


def get_hmac_params_from_config(signing_attributes):
    """
    :returns: A HMAC object with the HMAC parameters from the config file.
    :rtype: obj
    """
    # HW_ID MSB can be JTAG_ID or SOC_HW_VERSION
    # Use MSM_PART
    if signing_attributes.in_use_soc_hw_version == 0 or (signing_attributes.msm_part is not None and signing_attributes.soc_hw_version is None):
        msm_identifier = int(signing_attributes.msm_part, 16)

    # Use SOC_HW_VERSION
    else:
        # Solution 5: Mask is supplied
        if signing_attributes.mask_soc_hw_version is not None:
            msm_identifier = (int(signing_attributes.soc_hw_version, 16) & (int(signing_attributes.mask_soc_hw_version, 16) | 0xFFFF0000))

        # Solution 6: Mask is not supplied
        else:
            msm_identifier = (int(signing_attributes.soc_hw_version, 16) & 0xFFFFFFFF)

    # HW_ID LSB can be oem_id + model_id or serial_number
    # Use serial number
    if signing_attributes.use_serial_number_in_signing == 1:
        serial_number = (int(signing_attributes.serial_number, 16) & 0xFFFFFFFF)
        msm_id = msm_identifier << 32 | serial_number
    # Use oem_id and model_id
    else:
        oem_id = (int(signing_attributes.oem_id, 16) & 0xFFFF)
        model_id = (int(signing_attributes.model_id, 16) & 0xFFFF)
        msm_id = msm_identifier << 32 | oem_id << 16 | model_id
    sw_id = signing_attributes.sw_id
    sw_id = int(sw_id, 16)
    return HmacParams(msm_id, sw_id)

def get_sha_algo_from_config(signing_attributes):
    return signing_attributes.hash_algorithm

def get_signing_config_overrides():
    retval = None
    try:
        from sectools.common.engg.config_overrides import get_config_overrides
        retval = get_config_overrides()
    except ImportError:
        pass
    finally:
        return retval

def macro_replace(string_to_replace, macro_name, value, isMandatory=False):
    string_replaced = string_to_replace
    macro_to_search = "${%s}" % macro_name

    try:
        index_start = string_to_replace.index(macro_to_search)
        index_end = string_to_replace.index('}', index_start)
    except ValueError, e:
        if isMandatory is True:
            raise RuntimeError("macro name {0} is not found in string <{1}>".format(macro_name, string_to_replace))
    else:
        string_replaced = string_to_replace[:index_start] + str(value) + string_to_replace[index_end + 1:]

    return string_replaced

class MacroReplaceArgs(object):
    def __init__(self, macro_name, value, isMandatory=False):
        self.macro_name = macro_name
        self.value = value
        self.isMandatory = isMandatory

class StringProcessor(object):
    def __init__(self, string_to_replace):
        self.string_to_replace = string_to_replace
        self.macro_replace_arg_list = []

    def addMacroReplaceArgs(self, macro_name, value, isMandatory=False):
        macro_replace_arg = MacroReplaceArgs(macro_name, value, isMandatory)
        self.macro_replace_arg_list.append(macro_replace_arg)

    def get_replaced_string(self):
        processed_string = self.string_to_replace
        for arg in self.macro_replace_arg_list:
            processed_string = macro_replace(processed_string, arg.macro_name, arg.value, arg.isMandatory)

        self.replaced_string = processed_string
        return self.replaced_string



