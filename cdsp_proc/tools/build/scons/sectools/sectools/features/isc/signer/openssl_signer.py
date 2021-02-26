#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from base_signer import BaseSigner, SignerOutput
from sectools.common.crypto import utility_functions, crypto_functions
from sectools.common.utils import c_misc, c_path
from ..hasher import Hasher
from sectools.common.utils.c_logging import logger
import os
import signerutils
from config_error import ConfigError
from signerutils.certconfigparser import CertData
from sectools.common.utils.c_attribute import Attribute
from certificate import Certificate
from sectools.features.isc.signer import openssl_fetch as openssl_fetch_module
from sectools.common.crypto.openssl_binary_implementation import SHA1_OU_STRING, SHA256_OU_STRING

class OpenSSLSigner(BaseSigner):
    '''
    classdocs
    '''
    def __init__(self, config):
        '''
        Constructor
        '''
        self.config = config
        self.openssl_info = openssl_fetch_module.OpenSSLPathsObject(config)


    def sign(self, hash_to_sign, imageinfo, binary_to_sign=None, debug_dir=None, sha_algo=None):
        # Check that openssl is available
        try:
            crypto_functions.are_available([crypto_functions.MOD_OPENSSL])
        except Exception as e:
            raise RuntimeError('Cannot sign: ' + str(e))
        return self._sign(hash_to_sign,
                          imageinfo,
                          binary_to_sign,
                          sha_algo)


    def _sign(self, hash_to_sign,
                    imageinfo,
                    binary_to_sign,
                    sha_algo):

        # abstract some of the image information
        signing_attributes = imageinfo.signing_attributes
        general_properties = imageinfo.general_properties

        # GET OPENSSL DATA
        openssl_configfile = self.openssl_info.openssl_config
        openssl_attest_ca_xts = self.openssl_info.attest_ca_xts
        openssl_ca_cert_xts = self.openssl_info.ca_cert_xts

        # GET SIGNING ATTRIBUTE DATA
        debug_val = int(signing_attributes.debug, 16) if signing_attributes.debug is not None else None
        oem_id = int(signing_attributes.oem_id, 16) & 0xFFFF
        model_id = int(signing_attributes.model_id, 16) & 0xFFFF
        num_certs_in_certchain = general_properties.num_certs_in_certchain
        app_id = int(signing_attributes.app_id, 16) if signing_attributes.app_id is not None else None
        crash_dump = int(signing_attributes.crash_dump, 16) if signing_attributes.crash_dump is not None else None
        rot_en = int(signing_attributes.rot_en, 16) if signing_attributes.rot_en is not None else None
        soc_hw_version = int(signing_attributes.soc_hw_version, 16) if signing_attributes.soc_hw_version is not None else None
        mask_soc_hw_version = int(signing_attributes.mask_soc_hw_version, 16) if signing_attributes.mask_soc_hw_version is not None else None
        in_use_soc_hw_version = signing_attributes.in_use_soc_hw_version if signing_attributes.in_use_soc_hw_version is not None else None
        use_serial_number_in_signing = signing_attributes.use_serial_number_in_signing if signing_attributes.use_serial_number_in_signing is not None else None

        # GET CERTIFICATE INFORMATION
        cert_dict = {}
        cert_dict['id'] = imageinfo.cert_config
        cert_dict['chip'] = self.config.metadata.chipset
        cert_dict['keysize'] = general_properties.key_size
        cert_dict['exponent'] = general_properties.exponent
        cert_dict['mrc_index'] = general_properties.mrc_index

        # Can't use imageinfo.data_prov_basepath because MockImage can't use it
        cert_dict['dp_path'] = self.config.data_provisioning.base_path

        self.cert_data_object = CertData(cert_dict)
        crypto_params_dict = self.cert_data_object.get_crypto_params()

        # Create the attestation_certificate_key_pair
        attestation_certificate_key_pair = None

        root_certificate_params = crypto_params_dict['root_certificate_properties']
        root_certificate_params_is_valid, generate_new_root_certificate = self._validate_certificate_params_dict(root_certificate_params)
        if root_certificate_params_is_valid:
            if generate_new_root_certificate:
                logger.info('Generating new Root certificate and a random key')
                generated_root_key_pair = crypto_functions.gen_rsa_key_pair(general_properties.key_size, key_exponent=signing_attributes.exponent)
                root_cert, root_key_pair = crypto_functions.create_root_certficate(root_certificate_params, generated_root_key_pair, 7300, openssl_configfile, 1)
            else:
                logger.info('Using a predefined Root certificate and a predefined key')
                logger.info('Key Used: ' + root_certificate_params['private_key_path'])
                logger.info('Certificate Used: ' + root_certificate_params['certificate_path'])
                root_cert, root_key_pair = self._get_certificate_and_key_pair_from_files(root_certificate_params)
        else:
            logger.error("Root certificate params are invalid! Please check config file.")
            raise RuntimeError("Root certificate params are invalid! Please check config file.")

        if num_certs_in_certchain > 2:
            logger.debug("Generating Attestation CA certificate, since certchain size is greater than 2")
            attestation_ca_certificate_params = crypto_params_dict['attest_ca_certificate_properties']
            attestation_ca_params_is_valid, generate_new_attestation_ca = self._validate_certificate_params_dict(attestation_ca_certificate_params)
            if attestation_ca_params_is_valid:
                if generate_new_attestation_ca:
                    logger.info('Generating new Attestation CA certificate and a random key')
                    generated_attestation_ca__key_pair = crypto_functions.gen_rsa_key_pair(general_properties.key_size, key_exponent=signing_attributes.exponent)
                    attestation_ca_certificate, attestation_ca_certificate_key_pair = \
                        crypto_functions.create_certificate(attestation_ca_certificate_params,
                                                            generated_attestation_ca__key_pair,
                                                            root_cert,
                                                            root_key_pair,
                                                            days=7300,
                                                            configfile=openssl_configfile,
                                                            serial_num=1,
                                                            extfile_name=openssl_ca_cert_xts)
                else:
                    logger.info('Using a predefined Attestation CA certificate and a predefined key')
                    logger.info('Key Used: ' + attestation_ca_certificate_params['private_key_path'])
                    logger.info('Certificate Used: ' + attestation_ca_certificate_params['certificate_path'])
                    attestation_ca_certificate, attestation_ca_certificate_key_pair = self._get_certificate_and_key_pair_from_files(attestation_ca_certificate_params)
            else:
                logger.error("Attestation CA certificate params are invalid! Please check config file.")
                raise RuntimeError("Attestation CA certificate params are invalid! Please check config file.")

        attestation_certificate_params = crypto_params_dict['attest_certificate_properties']
        attestation_certificate_params_is_valid, generate_new_attestation_certificate = self._validate_certificate_params_dict(attestation_certificate_params)

        if attestation_certificate_params_is_valid:
            if generate_new_attestation_certificate:
                # TCG support
                if self._is_oid_supported(signing_attributes) is True:
                    if self.validate_oid_from_config(attestation_ca_certificate_params['certificate_path'], signing_attributes) is False:
                        raise ConfigError("{0} min and max are not set correctly in configuration."\
                                          "Signing will not continue.".format(signing_attributes.object_id.name)
                                          )
                    attestation_certificate_extensions_path = self._generate_attestation_certificate_extensions(
                                                                    openssl_attest_ca_xts,
                                                                    signing_attributes.object_id.name,
                                                                    signing_attributes.object_id.min,
                                                                    signing_attributes.object_id.max)
                else:
                    attestation_certificate_extensions_path = openssl_attest_ca_xts

                logger.info('Generating new Attestation certificate and a random key')
                hmac_params = signerutils.get_hmac_params_from_config(signing_attributes)
                certificate_ou_sw_id = "01 " + hmac_params.sw_id_str + " SW_ID"
                certificate_ou_hw_id = "02 " + hmac_params.msm_id_str + " HW_ID"
                certificate_ou_oem_id = "04 " + "%0.4X" % oem_id + " OEM_ID"
                certificate_ou_sw_size = "05 " + "%0.8X" % len(binary_to_sign) + " SW_SIZE"
                certificate_ou_model_id = "06 " + "%0.4X" % model_id + " MODEL_ID"
                certificate_hash_alg = SHA1_OU_STRING if sha_algo == 'sha1' else SHA256_OU_STRING

                certificate_ou = [
                                  certificate_ou_sw_id,
                                  certificate_ou_hw_id,
                                  certificate_ou_oem_id,
                                  certificate_ou_sw_size,
                                  certificate_ou_model_id,
                                  certificate_hash_alg
                                 ]
                # Optional attributes
                if debug_val is not None:
                    certificate_ou_debug_id = "03 " + "%0.16X" % debug_val + " DEBUG"
                    certificate_ou.append(certificate_ou_debug_id)
                if app_id is not None:
                    certificate_app_id = "08 " + "%0.16X" % app_id + " APP_ID"
                    certificate_ou.append(certificate_app_id)
                if crash_dump is not None:
                    certificate_crash_dump = "09 " + "%0.16X" % crash_dump + " CRASH_DUMP"
                    certificate_ou.append(certificate_crash_dump)
                if rot_en is not None:
                    certificate_rot_en = "10 " + "%0.16X" % rot_en + " ROT_EN"
                    certificate_ou.append(certificate_rot_en)
                if soc_hw_version is not None:
                    certificate_soc_hw_version = "11 " + "%0.8X" % soc_hw_version + " SOC_HW_VERSION"
                    certificate_ou.append(certificate_soc_hw_version)
                if mask_soc_hw_version is not None:
                    certificate_mask_soc_hw_version = "12 " + "%0.4X" % mask_soc_hw_version + " MASK_SOC_HW_VERSION"
                    certificate_ou.append(certificate_mask_soc_hw_version)
                if in_use_soc_hw_version is not None:
                    certificate_in_use_soc_hw_version = "13 " + "%0.4X" % in_use_soc_hw_version + " IN_USE_SOC_HW_VERSION"
                    certificate_ou.append(certificate_in_use_soc_hw_version)
                if use_serial_number_in_signing is not None:
                    certificate_use_serial_number_in_signing = "14 " + "%0.4X" % use_serial_number_in_signing + " USE_SERIAL_NUMBER_IN_SIGNING"
                    certificate_ou.append(certificate_use_serial_number_in_signing)

                if 'OU' in attestation_certificate_params.keys():
                    if type(attestation_certificate_params['OU']) == list:
                        for item in attestation_certificate_params['OU']:
                            certificate_ou.append(item)
                    else:
                        certificate_ou.append(attestation_certificate_params['OU'])

                attestation_certificate_params['OU'] = certificate_ou

                if attestation_certificate_key_pair is None:
                    attestation_certificate_key_pair = crypto_functions.gen_rsa_key_pair(key_exponent=signing_attributes.exponent,
                                                                                         key_size_in_bits=general_properties.key_size)
                if num_certs_in_certchain > 2:  # sign the attestation cert with the attestation_ca_cert
                    attestation_certificate, attestation_certificate_key_pair = \
                        crypto_functions.create_certificate(attestation_certificate_params,
                                                            attestation_certificate_key_pair,
                                                            attestation_ca_certificate,
                                                            attestation_ca_certificate_key_pair,
                                                            days=7300,
                                                            configfile=openssl_configfile,
                                                            serial_num=1,
                                                            extfile_name=attestation_certificate_extensions_path)
                else:  # sign the attestation cert with the root cert
                    attestation_certificate, attestation_certificate_key_pair = \
                        crypto_functions.create_certificate(attestation_certificate_params,
                                                            attestation_certificate_key_pair,
                                                            root_cert,
                                                            root_key_pair,
                                                            days=7300,
                                                            configfile=openssl_configfile,
                                                            serial_num=1,
                                                            extfile_name=attestation_certificate_extensions_path)
                attestation_certificate = crypto_functions.cert_pem_to_der(attestation_certificate)

                # Clean temp file
                if self._is_oid_supported(signing_attributes) is True:
                    c_path.clean_file(attestation_certificate_extensions_path)


            else:  # generate_new_attestation_certificate == False
                logger.info('Using a predefined Attestation certificate and a predefined key')
                logger.info('Key Used: ' + attestation_certificate_params['private_key_path'])
                logger.info('Certificate Used: ' + attestation_certificate_params['certificate_path'])
                attestation_certificate, attestation_certificate_key_pair = self._get_certificate_and_key_pair_from_files(attestation_certificate_params)
                attestation_certificate = crypto_functions.cert_pem_to_der(attestation_certificate)

                # Since the get_hmac_params_from_certificate_chain always works with the first cert in the cert chain,
                # this function will work for a single der certificate as well.
                hmac_params = crypto_functions.get_hmacparams_from_certificate_chain(attestation_certificate)
                hasher = Hasher()
                hash_to_sign = hasher.qcom_hmac(binary_to_sign, hmac_params, sha_algo)

            signature = crypto_functions.encrypt_with_private_key(hash_to_sign, attestation_certificate_key_pair['private_key'])
        else:
            logger.error("Attestation certificate params are invalid! Please check config file.")
            raise RuntimeError("Attestation certificate params are invalid! Please check config file.")

        if num_certs_in_certchain > 2:
            attestation_ca_certificate = crypto_functions.cert_pem_to_der(attestation_ca_certificate)
        else:
            attestation_ca_certificate = None

        root_cert = crypto_functions.cert_pem_to_der(root_cert)

        root_cert_list = self.cert_data_object.get_rootcerts(general_properties.num_root_certs)

        certificate_list = self._get_certificate_list(general_properties.num_root_certs,
                                                      num_certs_in_certchain,
                                                      attestation_certificate,
                                                      attestation_ca_certificate,
                                                      root_cert,
                                                      root_cert_list)

        cert_chain = crypto_functions.create_certificate_chain(certificate_list)

        signer_output = SignerOutput()
        signer_output.root_cert = root_cert
        signer_output.attestation_ca_cert = attestation_ca_certificate
        signer_output.attestation_cert = attestation_certificate
        signer_output.signature = signature
        signer_output.cert_chain = cert_chain
        signer_output.root_cert_list = root_cert_list
        signer_output.root_key = root_key_pair['private_key']
        # Make sure the variable is defined
        try:
            attestation_ca_certificate_key_pair
        except Exception: pass
        else:
            signer_output.attestation_ca_key = attestation_ca_certificate_key_pair['private_key']
        signer_output.attestation_key = attestation_certificate_key_pair['private_key']

        return signer_output

    def validate_config(self, imageinfo):

        self._validate_config(imageinfo.cert_config, imageinfo.general_properties)

        BaseSigner._validate_oid_values(self, imageinfo.signing_attributes,
                                    imageinfo.general_properties)

    def _get_certificate_list(self, num_root_certs,
                              num_certs_in_certchain,
                              attestation_certificate,
                              attestation_ca_certificate,
                              root_cert,
                              root_cert_list):
        certificate_list = [attestation_certificate]
        if num_certs_in_certchain > 2:
            certificate_list.append(attestation_ca_certificate)
        if num_root_certs == 1:
            certificate_list.append(root_cert)
        else:
            # multirootcert case
            for i in range(0, num_root_certs):
                certificate_list.append(root_cert_list[i])

        return certificate_list

    def _get_certificate_and_key_pair_from_files(self, certificate_params_dict):
        key_pair = {}
        with open(certificate_params_dict['private_key_path'], 'rb') as private_key_file:
            private_key = private_key_file.read()
            if 'BEGIN RSA PRIVATE KEY' not in private_key:
                # key is in DER format
                private_key = crypto_functions.privkey_der_to_pem(private_key)
            public_key = crypto_functions.get_public_key_from_private_key(private_key)
            key_pair['public_key'] = public_key
            key_pair['private_key'] = private_key

        with open(certificate_params_dict['certificate_path'], 'rb') as cert_file:
            cert_raw = cert_file.read()
            if 'BEGIN CERTIFICATE' in cert_raw:
                cert = cert_raw
            else:
                cert = crypto_functions.cert_der_to_pem(cert_raw)

        return (cert, key_pair)

    def _validate_certificate_params_dict(self, certificate_params_dict):
        certificate_params_is_valid = False
        generate_new_certificate = False
        for key in certificate_params_dict:
            if key not in ['C', 'CN', 'L', 'O', 'ST', 'OU']:
                if key not in ['private_key_path', 'certificate_path']:
                    logger.error("Invalid Key is being passed in configuration!" + repr(key))
                    raise RuntimeError("Invalid Key is being passed in configuration!")
                else:
                    if os.path.exists(certificate_params_dict['private_key_path']) is False:
                        err_str = "private_key_path does not exist: {0}!".format(certificate_params_dict['private_key_path'])
                        logger.error(err_str)
                        certificate_params_is_valid = False
                        raise RuntimeError(err_str)

                    if os.path.exists(certificate_params_dict['certificate_path']) is False:
                        err_str = "certificate_path does not exist: {0}!".format(certificate_params_dict['certificate_path'])
                        logger.error(err_str)
                        certificate_params_is_valid = False
                        raise RuntimeError(err_str)

                    generate_new_certificate = False
                    certificate_params_is_valid = True

            else:
                certificate_params_is_valid = True
                generate_new_certificate = True

        return certificate_params_is_valid, generate_new_certificate


    def _validate_config(self, cert_config, general_properties):

        if (general_properties.num_root_certs == 0):
            raise ConfigError("Number of root certificates cannot be set zero")

        if (general_properties.num_root_certs > 16):
            raise ConfigError("Number of root certificates cannot be more than 16")

        if (general_properties.mrc_index and
            general_properties.mrc_index >= general_properties.num_root_certs):
            raise ConfigError("Multirootcert index {0} must be smaller than the number of root certs {1}"
                              .format(general_properties.mrc_index, general_properties.num_root_certs))


    def _generate_oid_config(self, oid_name, min_str, max_str):
        min_attr = Attribute.init(num_bits=32, string=min_str)
        max_attr = Attribute.init(num_bits=32, string=max_str)

        oid_str = "%.8X%.8X" % (min_attr.value, max_attr.value)
        oid_cfg = "\n%s=DER:%s:%s:%s:%s:%s:%s:%s:%s" % \
                (Certificate.GetOIDByName(oid_name), oid_str[0:2], oid_str[2:4], oid_str[4:6], oid_str[6:8], \
                 oid_str[8:10], oid_str[10:12], oid_str[12:14], oid_str[14:16])

        return oid_cfg

    def _generate_attestation_certificate_extensions(self,
                                                     attestation_certificate_extensions_path,
                                                     oid_name,
                                                     min_str,
                                                     max_str):
        v3_attest_file = c_misc.load_data_from_file(attestation_certificate_extensions_path)
        v3_attest_file_new = v3_attest_file + \
                                   self._generate_oid_config(oid_name, min_str, max_str)
        v3_attest_file_temp = utility_functions.store_data_to_temp_file(v3_attest_file_new)

        return v3_attest_file_temp
