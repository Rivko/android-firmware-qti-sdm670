# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import re
from abc import ABCMeta

from sectools.common import crypto
from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.crypto import cert as cert_functions
from sectools.common.crypto import ecdsa as ecdsa_functions
from sectools.common.crypto import rsa as rsa_functions
from sectools.common.crypto import utils
from sectools.common.utils import c_path
from sectools.common.utils.c_attribute import Attribute, BaseAttribute
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import create_mismatch_table
from sectools.features.isc import data_provision_enquirer as openssl_fetch_module
from sectools.features.isc.cfgparser.defines import MRC_1_0_CHIPSETS, MRC_2_0_CHIPSETS
from sectools.features.isc.defines import AUTHORITY_OEM, AUTHORITY_QTI
from sectools.features.isc.defines import SECBOOT_VERSION_3_0
from sectools.features.isc.hasher import Hasher, HmacParams
from sectools.features.isc.signer import signer_mgr
from sectools.features.isc.signer.signerutils import get_hmac_params_from_extracted_attributes
from sectools.features.isc.signer.signerutils.attribute_extractor import AttributeExtractor
from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
from sectools.features.isc.signer.signerutils.certconfigparser import get_cert_data
from sectools.features.isc.signer.signerutils.certificate import Certificate
from sectools.features.isc.signer.utils import misc as utils_misc
from sectools.features.isc.signer.utils.certkey import CertKeyPair
from sectools.features.isc.signer.utils.hmac import HMAC


class SignerOutput(object):

    def __init__(self):
        self.signature = None
        self.cert_chain = None
        self.root_cert = None
        self.attestation_ca_cert = None
        self.attestation_cert = None
        self.root_key = None
        self.attestation_ca_key = None
        self.attestation_key = None
        self.root_cert_list = []

    def update_certs_format(self):
        # Update basic certs
        for tag in ['root_cert', 'attestation_ca_cert', 'attestation_cert']:
            val = getattr(self, tag)
            if val:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_DER)
            setattr(self, tag, val)

        # Update root cert list
        for idx in range(len(self.root_cert_list)):
            val = self.root_cert_list[idx]
            if val:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_DER)
            self.root_cert_list[idx] = val

        # Update keys
        for tag in ['root_key', 'attestation_ca_key', 'attestation_key']:
            val = getattr(self, tag)
            if val:
                val = rsa_functions.get_key_in_format(val, utils.FORMAT_DER)
            setattr(self, tag, val)

    def generate_cert_chain(self):
        certs = ([self.attestation_cert, self.attestation_ca_cert] +
                 [c for c in self.root_cert_list])
        certs = [c for c in certs if c is not None]
        self.cert_chain = ''.join(certs)
        return self.cert_chain


class ConfigError(RuntimeError):
    pass


class ExternalSignerError(RuntimeError):
    pass


def ishmac(imageinfo):
    return not imageinfo \
           or not imageinfo.general_properties \
           or imageinfo.general_properties.hmac is None \
           or imageinfo.general_properties.hmac


class BaseSigner(SecPluginIntf_Abs):
    __metaclass__ = ABCMeta

    DEBUG_DISABLED = 2

    # Error message strings
    MSG_INVALID_32_INTEGER = '{0} {1}:{2} is not a valid 32 bit integer'

    # Certificate properties
    CERT_DAYS = 7300
    CERT_SERIAL = 1

    # Padding types
    PAD_PSS = cert_functions.SIGN_ALGO_RSA_PSS
    PAD_PKCS = cert_functions.SIGN_ALGO_RSA_PKCS

    # PSS Fixes properties
    PAD_PSS_SALT_1 = cert_functions.PAD_PSS_SALT_1

    # OU hash field strings
    SHA1_OU_STRING = '07 0000 SHA1'
    SHA256_OU_STRING = '07 0001 SHA256'
    SHA384_OU_STRING = '07 0002 SHA384'

    OU_SHA_MAP = {
        SHA1_OU_STRING: "sha1",
        SHA256_OU_STRING: "sha256",
        SHA384_OU_STRING: "sha384",
    }

    SHA_OU_MAP = {algo: ou for ou, algo in OU_SHA_MAP.items()}
    SHA_OU_MAP[None] = SHA256_OU_STRING

    # Certificate types
    ROOT = 'Root'
    CA = 'CA'
    ATTEST = 'Attest'
    CERT_LIST = [ROOT, CA, ATTEST]

    def __init__(self, config=None):
        # Config info
        self.config = config
        self.config_overrides = utils_misc.get_signing_config_overrides()
        self.openssl_path = cert_functions.openssl

        # Configure dsa mode
        self.using_ecdsa = self.config.general_properties.dsa_type == "ecdsa"

        # Fetch the openssl info
        self.openssl_info = None
        try:
            self.openssl_info = openssl_fetch_module.OpenSSLPathsObject(config)
        except Exception as e:
            logger.debug('Error on fetching openssl info: ' + str(e))

        # Certificates properties for data prov
        self.certs_info = None
        self.padding = self.PAD_PKCS

        # Public variables
        self.debug_dir = None
        self.signing_attributes = None

        # Input data
        self.hash_to_sign = None
        self.data_to_sign = None
        self.data_to_sign_len = None

        # Certificates & signature
        self.certs = {}
        self.signature = None

    @classmethod
    def get_plugin_id(cls):
        return cls.signer_id()

    @classmethod
    def get_plugin_manager(cls):
        return signer_mgr

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def signer_id(cls):
        return 'local_v2'

    @classmethod
    def is_prod_signer(cls):
        return False

    def set_input(self, data_to_sign, imageinfo, debug_dir=None, is_hash=False):
        """Set the input.
        :type data_to_sign: str
        :type imageinfo: ImageInfo
        """

        # Validate the image info
        self.validate_config(imageinfo)

        # Set public variables for this session
        self.debug_dir = debug_dir
        sa = self.signing_attributes = imageinfo.general_properties

        # Set the certificates dictionary
        self.certs = dict()
        self.certs[self.ROOT] = CertKeyPair()
        self.certs[self.ATTEST] = CertKeyPair()
        if sa.num_certs_in_certchain == 3:
            self.certs[self.CA] = CertKeyPair()

        # Set the data to sign and the hash to sign
        if is_hash:
            self.hash_to_sign = data_to_sign
            self.data_to_sign = None
        else:
            hasher = HMAC()
            hasher.init_from_config(sa)
            self.hash_to_sign = hasher.hmac(data_to_sign)
            self.data_to_sign = data_to_sign
            self.data_to_sign_len = len(data_to_sign)
            logger.info('Using ' + hasher.hmac_type + ' (' + hasher.hash_algo + ')')

        self.padding = (self.PAD_PSS if (sa.rsa_padding and sa.rsa_padding.lower() == 'pss') else self.PAD_PKCS)
        self.hash_algo = sa.hash_algorithm if sa.hash_algorithm is not None else 'sha256'
        if self.using_ecdsa:
            logger.info("Using ECDSA with {0} curve".format(sa.ecdsa_curve))
        else:
            logger.info('Using ' + self.padding.upper() + ' RSA padding')

        # Initialize the secure assets
        self.initialize(imageinfo)

    def initialize(self, imageinfo):
        """Initialize the secure assets.

        Uses data prov to set secure assets.

        :type imageinfo: ImageInfo
        """
        # Generate cert info object
        cert_data = get_cert_data(imageinfo, self.config.data_provisioning.base_path)
        self.certs_info = cert_data.get_certs_info(self.signing_attributes.num_root_certs)

        # Get the root cert from data prov
        root_obj = self.certs[self.ROOT]
        root_obj.params = self.certs_info.root
        root_obj.cert, root_obj.priv_key, root_obj.pub_key = \
            self._get_cert_key_from_property(root_obj.params, self.ROOT)

        # Get the CA cert from data prov
        if self.CA in self.certs:
            ca_obj = self.certs[self.CA]
            ca_obj.params = self.certs_info.ca
            ca_obj.cert, ca_obj.priv_key, ca_obj.pub_key = \
                self._get_cert_key_from_property(ca_obj.params, self.CA)

        # Get the attest cert from data prov
        attest_obj = self.certs[self.ATTEST]
        attest_obj.params = self.certs_info.attest
        attest_obj.cert, attest_obj.priv_key, attest_obj.pub_key = \
            self._get_cert_key_from_property(attest_obj.params, self.ATTEST)

    def _get_cert_key_from_property(self, cert_property, cert_type):
        """Gets the cert & key from dataprov info

        :type cert_property: CertProperty
        :type cert_type: str
        """
        cert, priv_key, pub_key = None, None, None

        # Validate the cert property
        if not cert_property.validate():
            raise RuntimeError(cert_type.title() + " certificate params are invalid! Please check config file.")
        logger.info('Initialization with dataprov. These fields might not be used in final output if overridden')

        # Extract the private and public key
        if cert_property.priv_path and c_path.validate_file(cert_property.priv_path):
            logger.info('Using a predefined ' + cert_type + ' private key from: ' + cert_property.priv_path)
            with open(cert_property.priv_path, 'rb') as fp:
                priv_key = fp.read()
                if self.using_ecdsa:
                    priv_key = ecdsa_functions.get_key_in_format(priv_key, utils.FORMAT_PEM)
                    pub_key = ecdsa_functions.get_public_key_from_private(priv_key)
                else:
                    priv_key = rsa_functions.get_key_in_format(priv_key, utils.FORMAT_PEM)
                    pub_key = rsa_functions.get_public_key_from_private(priv_key)

        # Extract the certificate
        if cert_property.cert_path and c_path.validate_file(cert_property.cert_path):
            logger.info('Using a predefined ' + cert_type + ' certificate from: ' + cert_property.cert_path)
            with open(cert_property.cert_path, 'rb') as fp:
                cert = fp.read()
                cert = cert_functions.get_cert_in_format(cert, utils.FORMAT_PEM)
        return cert, priv_key, pub_key

    def sign(self, data_to_sign, imageinfo, debug_dir=None, is_hash=False, hash_segment_metadata=None):
        """
        This function returns a SignerOutput object which has all the security assets generated
        by the signer.
        """
        # Set the input information
        self.set_input(data_to_sign, imageinfo, debug_dir, is_hash)

        # Set the certificates and keys for output
        signer_output = SignerOutput()
        signer_output.root_cert, signer_output.root_key = self.get_root_cert_key()
        if self.CA in self.certs:
            signer_output.attestation_ca_cert, signer_output.attestation_ca_key = self.get_ca_cert_key()
        signer_output.attestation_cert, signer_output.attestation_key = self.get_attest_cert_key()

        # Set the root certs for MRC
        signer_output.root_cert_list = self.get_root_cert_list()

        # Get the hmac params from attestation cert or hash segment
        extracted_image_attributes = AttributeExtractor(cert_data=signer_output.attestation_cert, hash_segment_metadata=hash_segment_metadata).attributes
        hmac_from_image = HMAC()
        hmac_from_image.init_from_image_attributes(extracted_image_attributes, self.signing_attributes)

        # Get the hmac params from config
        hmac_from_config = HMAC()
        hmac_from_config.init_from_config(self.signing_attributes)

        # Recreate the hash to sign if necessary
        if hmac_from_config.hmac_type == hmac_from_config.HMAC_TYPE_QTI and not hmac_from_image.is_equal(hmac_from_config):
            if self.data_to_sign is not None:
                self.hash_to_sign = hmac_from_image.hmac(self.data_to_sign)
            else:
                raise RuntimeError('HMAC params from image cannot be used with pre-generated hash.')

        # Set the signature
        signer_output.signature = self.get_signature()
        signer_output.unsigned_hash = self.hash_to_sign

        # Update the certs
        signer_output.update_certs_format()

        # Set the cert chain
        signer_output.generate_cert_chain()

        # Print certificate properties (to make tests pass and give good debug information)
        if hash_segment_metadata is None:
            logger.info('\nAttestation Certificate Properties:\n' +
                        str(Certificate(signer_output.attestation_cert)))

        return signer_output

    @classmethod
    def get_hmac_params_from_cert(cls, certificate=None, extracted_attributes=None):
        """ Return a dictionary of the HMAC params from the certificate subject dictionary
        input:
            certificate_subject_dictionary = dictionary of subject params from certificate

        output:
            hmac_params = Dictionary of HMAC parameters from certificate subject
        """

        if certificate:
            certificate_subject_dictionary = crypto.cert.get_subject_params(crypto.cert.get_subject(certificate))
            sw_id_re = re.compile(r'01 ([0-9A-F]{16}) SW_ID')
            hw_id_re = re.compile(r'02 ([0-9A-F]{16}) HW_ID')
            if 'OU' in certificate_subject_dictionary.keys() and type(certificate_subject_dictionary['OU']) == list:
                certificate_subject_dictionary['OU'].sort()
            sw_id_element = sw_id_re.match(certificate_subject_dictionary['OU'][0])
            hw_id_element = hw_id_re.match(certificate_subject_dictionary['OU'][1])
            logger.debug("Valid certificate: Found SW_ID and HW_ID")
            sw_id = sw_id_element.group(1) if sw_id_element is not None else None
            hw_id = hw_id_element.group(1) if hw_id_element is not None else None
        elif extracted_attributes:
            if extracted_attributes.from_hash_segment:
                hmac_params = get_hmac_params_from_extracted_attributes(extracted_attributes)
                sw_id = hmac_params.sw_id_str
                hw_id = hmac_params.msm_id_str
            else:
                sw_id = extracted_attributes.sw_id
                hw_id = extracted_attributes.hw_id
        else:
            logger.critical("certificate and extracted_attributes cannot both be none")
            raise RuntimeError("certificate and attribute_extractor cannot both be none")

        if sw_id is None:
            logger.critical("Error in image. SW_ID field not found. Exiting")
            raise RuntimeError("Error in image. SW_ID field not found. Exiting")
        elif hw_id is None:
            logger.critical("Error in image. HW_ID field not found. Exiting")
            raise RuntimeError("Error in image. HW_ID field not found. Exiting")

        logger.debug('SW_ID = ' + sw_id)
        logger.debug('HW_ID = ' + hw_id)
        hmac_params = HmacParams(int(hw_id, 16), int(sw_id, 16))
        return hmac_params

    def get_root_cert_list(self):
        if self.certs_info is not None:
            root_cert_list = self.certs_info.root_certs
            if len(root_cert_list) > 0:
                return root_cert_list
        return [self.get_root_cert_key()[0]]

    def get_signature(self):
        # Return existing signature
        if self.signature is not None:
            return self.signature

        # Create the signature
        logger.info('Creating signature')
        signature = self.sign_hash(self.hash_to_sign, self.certs[self.ATTEST].priv_key, self.padding, self.hash_algo)
        # Return the generated signature
        return signature

    def sign_hash(self, hashbin, priv_key, padding, hash_algo):
        if self.using_ecdsa:
            return ecdsa_functions.sign(hashbin, priv_key)
        else:
            return rsa_functions.sign(hashbin, priv_key, padding=padding, hash_algo=hash_algo, salt_len=self.PAD_PSS_SALT_1)

    def get_root_cert_key(self):
        return self._get_cert_key(self.ROOT)

    def get_ca_cert_key(self):
        return self._get_cert_key(self.CA)

    def get_attest_cert_key(self):
        return self._get_cert_key(self.ATTEST)

    def _get_cert_key(self, cert_type):
        cert_type_obj = self.certs[cert_type]

        # Return existing cert
        if cert_type_obj.cert is not None:
            return cert_type_obj.cert, cert_type_obj.priv_key

        # Get the previous cert
        prev_type_obj = None
        for k in self.CERT_LIST:
            if k in self.certs:
                v = self.certs[k]
                if prev_type_obj is None:
                    prev_type_obj = v
                if k == cert_type:
                    break
                prev_type_obj = v
        else:
            raise RuntimeError('Cert type is invalid: ' + cert_type)

        # Generate new cert
        logger.info('Generating new certificate ' + cert_type)
        cert, cert_type_obj.priv_key, cert_type_obj.pub_key = self.generate_new_cert(cert_type,
                                                                                     cert_type_obj,
                                                                                     prev_type_obj,
                                                                                     extfile=cert_type_obj.extfile,
                                                                                     self_sign=(cert_type == self.ROOT),
                                                                                     padding=self.padding)
        # Ensure cert is in pem format
        cert = cert_functions.get_cert_in_format(cert, utils.FORMAT_PEM)
        cert_type_obj.cert = cert

        return cert_type_obj.cert, cert_type_obj.priv_key

    # This routine only supports 2-level and 3-level cert chain
    def _get_signer_output(self, signature, cert_chain_list):
        signer_output = SignerOutput()
        signer_output.attestation_cert = cert_chain_list[0]
        if len(cert_chain_list) == 3:
            signer_output.attestation_ca_cert = cert_chain_list[1]
            signer_output.root_cert = cert_chain_list[2]
        elif len(cert_chain_list) == 2:
            signer_output.root_cert = cert_chain_list[1]
        else:
            raise RuntimeError("Only 2-level or 3-level cert chain is supported. Number of certificates found = {0}\n".format(len(cert_chain_list)))

        cert_chain = crypto.cert.create_cert_chain_bin(cert_chain_list)

        signer_output.signature = signature
        signer_output.cert_chain = cert_chain

        return signer_output

    def generate_new_cert(self, cert_type, cert_info, prev_cert_info, extfile=None, self_sign=False, padding=PAD_PKCS):

        # Update the subject parameters
        subject_params = self.create_subject_params(cert_type, cert_info.params.params)

        # Generate the certificate request
        cert_req, priv_key, pub_key = self.generate_cert_request(cert_type,
                                                                 cert_info,
                                                                 subject_params,
                                                                 self_sign=self_sign,
                                                                 key_exp=self.signing_attributes.exponent,
                                                                 key_size=self.signing_attributes.key_size,
                                                                 padding=padding)

        # Settle on the extfile
        if extfile is None:
            extfile = cert_info.extfile

        # Sign the certificate request
        if not self_sign:
            logger.info('Signing certificate request for ' + cert_type)
            cert = self.sign_cert_request(cert_req, prev_cert_info, extfile, padding=padding)
        else:
            cert = cert_req

        return cert, priv_key, pub_key

    def generate_cert_request(self,
                              cert_type,
                              cert_info,
                              subject_params,
                              self_sign=False,
                              key_exp=None,
                              key_size=None,
                              padding=PAD_PKCS):

        # Ensure that the number of SHA bits is equal to or exceeds the number of ECDSA bits
        if self.using_ecdsa:
            num_sha_bits = int(filter(str.isdigit, self.hash_algo))
            num_ecdsa_bits = ecdsa_functions.curve_size_map[self.signing_attributes.ecdsa_curve]
            if num_sha_bits < num_ecdsa_bits:
                logger.warning("The number of SHA bits is less than the number of ECDSA bits.\n"
                               "SHA is set to {0} bits and ECDSA is set to {1} bits.".format(num_sha_bits, num_ecdsa_bits))

        # Create a new key if there isn't one
        if cert_info.priv_key is None:
            logger.info('Generating new private/public key pair for ' + cert_type)
            cert_info.priv_key, cert_info.pub_key = self.generate_key_pair(key_exp, key_size)

        logger.info('Creating certificate request for ' + cert_type)

        cert = cert_functions.create_cert(cert_info.priv_key,
                                          subject_params=subject_params,
                                          config=self.openssl_info.openssl_config,
                                          hash_algo=self.hash_algo,
                                          serial=self.CERT_SERIAL,
                                          days=self.CERT_DAYS,
                                          self_sign=self_sign,
                                          sign_algo=padding,
                                          pad_salt_len=self.PAD_PSS_SALT_1,
                                          pad_hash_algo=self.hash_algo)
        return cert, cert_info.priv_key, cert_info.pub_key

    def generate_key_pair(self, exponent=None, keysize=None):
        if self.using_ecdsa:
            return ecdsa_functions.gen_keys(self.signing_attributes.ecdsa_curve)
        else:
            return rsa_functions.gen_keys(exponent, keysize)

    def sign_cert_request(self, cert_req, prev_cert_info, extfile=None, padding=PAD_PKCS):
        return cert_functions.sign_cert(cert_req,
                                        prev_cert_info.cert,
                                        prev_cert_info.priv_key,
                                        days=self.CERT_DAYS,
                                        serial=self.CERT_SERIAL,
                                        hash_algo=self.hash_algo,
                                        extfile=extfile,
                                        sign_algo=padding,
                                        pad_salt_len=self.PAD_PSS_SALT_1,
                                        pad_hash_algo=self.hash_algo)

    def create_subject_params(self, cert_type, params):
        subject_param_updater = {
            self.ROOT: lambda x: dict(x),
            self.CA: self.create_subject_params_ca,
            self.ATTEST: self.create_subject_params_attest
        }
        return subject_param_updater[cert_type](params)

    def create_subject_params_ca(self, in_params):
        self.certs[self.CA].extfile = self.openssl_info.ca_cert_xts
        return dict(in_params)

    def create_subject_params_attest(self, in_params):
        # Set exfile
        if self._is_oid_supported(self.signing_attributes):
            if not self.validate_oid_from_config(self.certs_info.ca.cert_path, self.signing_attributes):
                raise ConfigError('{0} min and max are not set correctly in configuration.'
                                  'Signing will not continue.'.format(self.signing_attributes.object_id.name))
            self.certs[self.ATTEST].extfile = self._generate_attestation_certificate_extensions(self.openssl_info.attest_ca_xts,
                                                                                                self.signing_attributes.object_id.name,
                                                                                                self.signing_attributes.object_id.min,
                                                                                                self.signing_attributes.object_id.max)
        else:
            self.certs[self.ATTEST].extfile = self.openssl_info.attest_ca_xts

        # Only allow OU fields to be added to attest cert if they are not being added to hash segment
        if self.signing_attributes.secboot_version in [SECBOOT_VERSION_3_0]:
            # Don't add OU fields
            logger.info("Skipping adding OU fields to attest certificate.")
            return dict(in_params)

        def create_ou_field_from_hex_list(ou_num, ou_name, hex_list, remove_0x, max_num_items_in_ou):
            item_length = 0
            ou_field = str(ou_num)
            for val in hex_list:
                item_length = len(val)-2 if remove_0x else len(val)
                ou_field += " " + (val[2:] if remove_0x else val)
            # fill remainder of OU field with zeros
            zeros = (" " + "0" * item_length) * (max_num_items_in_ou - len(hex_list))
            ou_field += zeros
            ou_field += " " + ou_name
            return ou_field

        # GET SIGNING ATTRIBUTE DATA
        num_root_certs = int(self.signing_attributes.num_root_certs) if self.signing_attributes.num_root_certs is not None else None
        debug_val = int(self.signing_attributes.debug, 16) if self.signing_attributes.debug is not None else None
        multi_serial_numbers = self.signing_attributes.multi_serial_numbers.serial if self.signing_attributes.multi_serial_numbers is not None else []
        oem_id = int(self.signing_attributes.oem_id, 16) & 0xFFFF
        model_id = int(self.signing_attributes.model_id, 16) & 0xFFFF
        app_id = int(self.signing_attributes.app_id, 16) if self.signing_attributes.app_id is not None else None
        crash_dump = int(self.signing_attributes.crash_dump, 16) if self.signing_attributes.crash_dump is not None else None
        rot_en = int(self.signing_attributes.rot_en, 16) if self.signing_attributes.rot_en is not None else None
        mask_soc_hw_version = int(self.signing_attributes.mask_soc_hw_version, 16) if self.signing_attributes.mask_soc_hw_version is not None else None
        in_use_soc_hw_version = self.signing_attributes.in_use_soc_hw_version if self.signing_attributes.in_use_soc_hw_version is not None else None
        soc_vers = self.signing_attributes.soc_vers
        use_serial_number_in_signing = self.signing_attributes.use_serial_number_in_signing if self.signing_attributes.use_serial_number_in_signing is not None else None
        oem_id_independent = self.signing_attributes.oem_id_independent if self.signing_attributes.oem_id_independent is not None else None
        revocation_enablement = int(self.signing_attributes.revocation_enablement, 16) if self.signing_attributes.revocation_enablement is not None else None
        activation_enablement = int(self.signing_attributes.activation_enablement, 16) if self.signing_attributes.activation_enablement is not None else None
        root_revoke_activate_enable = int(self.signing_attributes.root_revoke_activate_enable, 16) if self.signing_attributes.root_revoke_activate_enable is not None else None
        uie_key_switch_enable = int(self.signing_attributes.uie_key_switch_enable, 16) if self.signing_attributes.uie_key_switch_enable is not None else None

        # Get the binary to sign length
        if self.data_to_sign_len is None:
            if self.hash_to_sign is not None:
                self.data_to_sign_len = len(self.hash_to_sign)
            else:
                raise RuntimeError('Length of binary could not be computed')

        logger.info('Generating new Attestation certificate and a random key')
        hmac_params = HMAC()
        hmac_params.init_from_config(self.signing_attributes)
        certificate_ou_sw_id = '01 ' + '%.16X' % hmac_params.sw_id + ' SW_ID'
        certificate_ou_hw_id = '02 ' + '%.16X' % hmac_params.msm_id + ' HW_ID'
        certificate_ou_oem_id = '04 ' + '%0.4X' % oem_id + ' OEM_ID'
        certificate_ou_sw_size = '05 ' + '%0.8X' % self.data_to_sign_len + ' SW_SIZE'
        certificate_ou_model_id = '06 ' + '%0.4X' % model_id + ' MODEL_ID'
        certificate_hash_alg = self.SHA_OU_MAP[self.signing_attributes.hash_algorithm]

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
            certificate_ou_debug_id = '03 ' + '%0.16X' % debug_val + ' DEBUG'
            certificate_ou.append(certificate_ou_debug_id)
        if app_id is not None:
            certificate_app_id = '08 ' + '%0.16X' % app_id + ' APP_ID'
            certificate_ou.append(certificate_app_id)
        if crash_dump is not None:
            certificate_crash_dump = '09 ' + '%0.16X' % crash_dump + ' CRASH_DUMP'
            certificate_ou.append(certificate_crash_dump)
        if rot_en is not None:
            certificate_rot_en = '10 ' + '%0.16X' % rot_en + ' ROT_EN'
            certificate_ou.append(certificate_rot_en)
        if mask_soc_hw_version is not None:
            certificate_mask_soc_hw_version = '12 ' + '%0.4X' % mask_soc_hw_version + ' MASK_SOC_HW_VERSION'
            certificate_ou.append(certificate_mask_soc_hw_version)
        if in_use_soc_hw_version == 1:
            certificate_in_use_soc_hw_version = '13 ' + '%0.4X' % in_use_soc_hw_version + ' IN_USE_SOC_HW_VERSION'
            certificate_ou.append(certificate_in_use_soc_hw_version)
        if use_serial_number_in_signing == 1:
            certificate_use_serial_number_in_signing = '14 ' + '%0.4X' % use_serial_number_in_signing + ' USE_SERIAL_NUMBER_IN_SIGNING'
            certificate_ou.append(certificate_use_serial_number_in_signing)
        if oem_id_independent == 1:
            certificate_oem_id_independent = '15 ' + '%0.4X' % oem_id_independent + ' OEM_ID_INDEPENDENT'
            certificate_ou.append(certificate_oem_id_independent)

        # multiple debug serial use case
        certificate_ou_sn_list = []
        for index in xrange(0, len(multi_serial_numbers), 6):
            serial_sublist = multi_serial_numbers[index: index+6]
            certificate_ou_sn_list.append(create_ou_field_from_hex_list(16, "SN", serial_sublist, True, 6))
        certificate_ou_sn_list.reverse()
        certificate_ou.extend(certificate_ou_sn_list)

        # multiple soc hw version use case
        if soc_vers:
            if self.padding != self.PAD_PSS:
                logger.warning("soc_vers should be used with RSAPSS")
            certificate_ou.append(create_ou_field_from_hex_list(11, "SOC_VERS", soc_vers, True, 10))
        elif hasattr(self.signing_attributes, "ignored_general_properties") and self.signing_attributes.ignored_general_properties["soc_vers"]:
            logger.debug("soc_vers were provided but in_use_soc_hw_version was set to \"0\"")

        # MRC 1.0 use case
        if num_root_certs > 1 and self.config.metadata.chipset in MRC_1_0_CHIPSETS:
            certificate_root_cert_sel = '17 ' + '%0.4X' % self.signing_attributes.mrc_index + ' ROOT_CERT_SEL'
            certificate_ou.append(certificate_root_cert_sel)
            if revocation_enablement is not None and revocation_enablement != 0:
                certificate_revocation_enablement = '18 ' + '%0.16X' % revocation_enablement + ' REVOCATION_ENABLEMENT'
                certificate_ou.append(certificate_revocation_enablement)
            if activation_enablement is not None and activation_enablement != 0:
                certificate_activation_enablement = '19 ' + '%0.16X' % activation_enablement + ' ACTIVATION_ENABLEMENT'
                certificate_ou.append(certificate_activation_enablement)

        # MRC 2.0 use case
        if num_root_certs > 1 and self.config.metadata.chipset in MRC_2_0_CHIPSETS:
            certificate_root_cert_sel = '17 ' + '%0.4X' % self.signing_attributes.mrc_index + ' ROOT_CERT_SEL'
            certificate_ou.append(certificate_root_cert_sel)
            if root_revoke_activate_enable is not None and root_revoke_activate_enable != 0:
                certificate_root_revoke_activate_enable = '20 ' + '%0.16X' % root_revoke_activate_enable + ' ROOT_REVOKE_ACTIVATE_ENABLE'
                certificate_ou.append(certificate_root_revoke_activate_enable)

        if uie_key_switch_enable is not None and uie_key_switch_enable != 0:
            certificate_uie_key_switch_enable = '21 ' + '%0.16X' % uie_key_switch_enable + ' UIE_KEY_SWITCH_ENABLE'
            certificate_ou.append(certificate_uie_key_switch_enable)

        # Handle OU property binding
        params = dict(in_params)
        if 'OU' in params.keys():
            if type(params['OU']) == list:
                for item in params['OU']:
                    certificate_ou.append(item)
            else:
                certificate_ou.append(params['OU'])

        # Add OU fields
        params['OU'] = certificate_ou
        logger.info("Adding OU fields to attest certificate.")

        return params

    def _generate_oid_config(self, oid_name, min_str, max_str):
        min_attr = Attribute.init(num_bits=32, string=min_str)
        max_attr = Attribute.init(num_bits=32, string=max_str)

        oid_str = '%.8X%.8X' % (min_attr.value, max_attr.value)
        oid_cfg = '\n%s=DER:%s:%s:%s:%s:%s:%s:%s:%s' % \
                  (Certificate.GetOIDByName(oid_name), oid_str[0:2], oid_str[2:4], oid_str[4:6], oid_str[6:8],
                   oid_str[8:10], oid_str[10:12], oid_str[12:14], oid_str[14:16])

        return oid_cfg

    def _generate_attestation_certificate_extensions(self,
                                                     attestation_certificate_extensions_path,
                                                     oid_name,
                                                     min_str,
                                                     max_str):
        v3_attest_file = c_path.load_data_from_file(attestation_certificate_extensions_path)
        v3_attest_file_new = v3_attest_file + self._generate_oid_config(oid_name, min_str, max_str)
        v3_attest_file_temp = c_path.store_data_to_temp_file(v3_attest_file_new)

        return v3_attest_file_temp

    def validate_sig_using_hash(self, image_hash, signature, cert_chain_der, signed_authority=None, extracted_image_attributes=None):
        # Check that openssl is available
        try:
            crypto.are_available([crypto.modules.MOD_OPENSSL])
        except Exception as e:
            raise RuntimeError('Cannot validate signing: ' + str(e))

        if extracted_image_attributes is None:
            extracted_image_attributes = AttributeExtractor(cert_data=cert_chain_der[0], hash_segment_metadata=None).attributes
        cert_sign_algo = crypto.cert.get_sign_algo(extracted_image_attributes.cert_text)
        use_pss = cert_sign_algo == crypto.cert.SIGN_ALGO_RSA_PSS
        use_dsa = cert_sign_algo == crypto.cert.SIGN_ALGO_ECDSA

        message = "Image is signed with "
        if signed_authority:
            message = signed_authority + " signed image with "
        if use_pss:
            logger.info(message + "RSAPSS")
        if use_dsa:
            logger.info(message + "ECDSA")
        if not use_dsa and not use_pss:
            logger.info(message + "PKCS")

        cert_chain_pem = []
        for cert in cert_chain_der:
            cert_chain_pem.append(crypto.cert.get_cert_in_format(cert, crypto.utils.FORMAT_PEM))

        attest_cert, ca_cert_chain = cert_chain_pem[0], '\n'.join(cert_chain_pem[-(len(cert_chain_pem) - 1):])
        crypto.cert.validate_cert_chain(attest_cert, ca_cert_chain)
        public_key = crypto.cert.get_pubkey(attest_cert)

        if use_dsa:
            matches = crypto.ecdsa.verify(image_hash, public_key, signature)
        else:
            matches = crypto.rsa.verify(image_hash, public_key, signature,
                                        padding=crypto.rsa.RSA_PAD_PSS if use_pss else crypto.rsa.RSA_PAD_PKCS,
                                        hash_algo=extracted_image_attributes.hash_algorithm)
        return matches

    def validate_sig(self, to_sign, signature, cert_chain_der, signed_authority=None, extracted_image_attributes=None):
        # Check that openssl is available
        try:
            crypto.are_available([crypto.modules.MOD_OPENSSL])
        except Exception as e:
            raise RuntimeError('Cannot validate signing: ' + str(e))

        # Get the hash
        if extracted_image_attributes is None:
            extracted_image_attributes = AttributeExtractor(cert_data=cert_chain_der[0], hash_segment_metadata=None).attributes
        cert_text = extracted_image_attributes.cert_text
        use_pss = crypto.cert.get_sign_algo(cert_text) == crypto.cert.SIGN_ALGO_RSA_PSS
        # Get the hmac params from attestation cert or hash segment
        hmac_params = self.get_hmac_params_from_cert(extracted_attributes=extracted_image_attributes)
        hash_algo = extracted_image_attributes.hash_algorithm
        image_hash = Hasher().get_hash(to_sign, hmac_params=hmac_params if not use_pss else None, sha_algo=hash_algo)

        # Validate the hash
        return self.validate_sig_using_hash(image_hash, signature, cert_chain_der, signed_authority=signed_authority, extracted_image_attributes=extracted_image_attributes)

    def validate_root_cert_hash(self, cert_chain_der, expected_root_cert_hash, extracted_image_attributes=None):
        if len(cert_chain_der) < 2:
            raise RuntimeError('Cert chain must contain at least 2 certificates.')
        if len(cert_chain_der) == 2:  # 2-level cert chain
            root_cert = cert_chain_der[1].rstrip(chr(0xff))
        elif len(cert_chain_der) == 3:  # 3-level cert chain
            root_cert = cert_chain_der[2].rstrip(chr(0xff))
        else:  # MRC use case
            root_cert_concatenation = ''.join(cert_chain_der[2:])
            root_cert = root_cert_concatenation.rstrip(chr(0xff))

        # determine hash algo used from attest cert
        if extracted_image_attributes is None:
            extracted_image_attributes = AttributeExtractor(cert_data=cert_chain_der[0], hash_segment_metadata=None).attributes
        root_cert_hash = HMAC.HASH_ALGO_MAP[extracted_image_attributes.hash_algorithm](root_cert).hexdigest()

        return root_cert_hash == expected_root_cert_hash

    def validate(self, image, root_cert_hash=None, imageinfo=None):
        if image.is_signed():
            # Create error string
            errstr = []

            # Extracted hash segment metadata for all authorities
            metadata = {AUTHORITY_QTI: None, AUTHORITY_OEM: None}

            for signed_authority, data_to_sign, data_signature, cert_chain, hash_segment_metadata in image.get_signing_assets():
                # Save extracted hash segment metadata for signing attribute validation
                metadata[signed_authority] = hash_segment_metadata

                # Check if empty
                if not data_signature and not cert_chain:
                    if signed_authority != image.authority:
                        logger.warning(signed_authority + ' signature is not present')
                    else:
                        raise RuntimeError(signed_authority + ' signature is not present')
                    continue

                # Extract the cert chain list
                cert_chain_der = crypto.cert.split_cert_chain_bin(cert_chain)

                # Extract signing attributes from image
                extracted_image_attributes = AttributeExtractor(cert_data=cert_chain_der[0], hash_segment_metadata=hash_segment_metadata).attributes

                # Signature verification
                if not self.validate_sig(data_to_sign, data_signature, cert_chain_der, signed_authority=signed_authority, extracted_image_attributes=extracted_image_attributes):
                    errstr.append(signed_authority + ' signature is invalid')

                # OID Validation
                if len(cert_chain_der) == 3:
                    if not self.validate_oid_from_certs(cert_chain_der[1], cert_chain_der[0]):
                        errstr.append('OID values in the certificate are invalid')

            # Extract the cert chain list
            cert_chain_blob = image.cert_chain
            cert_chain_der = crypto.cert.split_cert_chain_bin(cert_chain_blob)

            # Extract signing attributes from image
            attribute_extractor = AttributeExtractor(cert_data=cert_chain_der[0], hash_segment_metadata=metadata[image.authority])
            extracted_image_attributes = attribute_extractor.attributes

            # Root cert hash validation
            if root_cert_hash:
                if not cert_chain_der:
                    errstr.append('Root certificate for ' + image.authority + ' is not present in image for root cert hash verification')
                elif not self.validate_root_cert_hash(cert_chain_der, root_cert_hash, extracted_image_attributes):
                    errstr.append('Root certificate from image does not match the given root cert hash value')

            # Signing attributes validation
            if imageinfo is not None:
                if not cert_chain_der:
                    errstr.append('Certificate chain for ' + image.authority + ' is not present in image signing attributes verification')
                else:
                    mismatches = self.validate_signing_attributes(cert_chain_der, imageinfo, extracted_image_attributes)
                    create_mismatch_table(mismatches, errstr, operation="signing", data_type_to_compare="Attribute", image_region=attribute_extractor.image_region)
            if errstr:
                raise RuntimeError('Following validations failed for the image:\n       ' +
                                   '\n       '.join([(str(signed_authority + 1) + '. ' + e) for signed_authority, e in enumerate(errstr)]))
            return True
        else:
            raise RuntimeError("Image supplied is not signed.")

    def validate_signing_attributes(self, cert_chain_der, imageinfo, extracted_image_attributes):
        if not cert_chain_der:
            raise RuntimeError('Cert chain must contain at least 1 certificate.')
        attr_config = SigningAttributes()
        attr_config.update_from_image_info_attrs(imageinfo.signing_attributes)
        return extracted_image_attributes.compare(attr_config, extracted_image_attributes.EXCLUDE_NON_ATTEST)

    def _is_oid_supported(self, signing_attributes):
        isSupported = False
        if signing_attributes.object_id:
            isSupported = True

        return isSupported

    def _validate_oid_values(self, signing_attributes, general_properties, mandatory=True):
        if self._is_oid_supported(signing_attributes) is False:
            return

        oid_name = signing_attributes.object_id.name
        oid_min = signing_attributes.object_id.min
        oid_max = signing_attributes.object_id.max

        config_params_32bits = {
            "min": oid_min,
            "max": oid_max,
        }

        for key in config_params_32bits.keys():
            valid = False
            if config_params_32bits[key] and (Attribute.validate(num_bits=32, string=config_params_32bits[key]) is True):
                valid = True
            elif (not config_params_32bits[key]) and (mandatory is False):
                valid = True

            if valid is False:
                raise ConfigError(self.MSG_INVALID_32_INTEGER.
                                  format(oid_name,
                                         key, config_params_32bits[key]))

        if oid_min and oid_max and (Attribute.init(num_bits=32, string=oid_min).value > Attribute.init(num_bits=32, string=oid_max).value):
            raise ConfigError('{0} min must be less than max, min={1} max={2}'.format(oid_name, oid_min, oid_max))

    def _validate_attributes_with_oid_rule(self, attest_cert_obj):
        is_valid = True
        # Enforce TCG rules
        attributes_zero_list = [
            Certificate.SIGNATTR_SW_ID,
            Certificate.SIGNATTR_HW_ID,
            Certificate.SIGNATTR_OEM_ID,
            Certificate.SIGNATTR_MODEL_ID,
        ]

        attributes_none_list = [
            Certificate.SIGNATTR_APP_ID,
            Certificate.SIGNATTR_CRASH_DUMP,
            Certificate.SIGNATTR_ROT_EN,
        ]

        if attest_cert_obj.tcg_min and attest_cert_obj.tcg_max:
            # Only enforce TCG rules currently
            for attr_name in attributes_zero_list:
                attr = attest_cert_obj.get_attr(attr_name)
                if attr.value != 0:
                    logger.debug("{0} should be 0 under TCG validation rules. Current value is {1}". \
                                 format(attr_name, attr.str))
                    is_valid = False

            for attr_name in attributes_none_list:
                attr = attest_cert_obj.get_attr(attr_name)
                if attr is not None:
                    logger.debug("{0} should be None under TCG validation rules. Current value is {1}". \
                                 format(attr_name, attr.str))
                    is_valid = False

            attr = attest_cert_obj.get_attr(Certificate.SIGNATTR_DEBUG)
            if attr is not None and attr.value != self.DEBUG_DISABLED:
                logger.debug("{0} should be 2 under TCG validation rules. Current value is {1}".format(Certificate.SIGNATTR_DEBUG, attr.str))
                is_valid = False

        return is_valid

    def _validate_oid_raw(self, min_attest, max_attest, min_ca, max_ca):
        tcg_ok = False

        if min_attest is not None:
            assert isinstance(min_attest, BaseAttribute)
        if max_attest is not None:
            assert isinstance(max_attest, BaseAttribute)
        if min_ca is not None:
            assert isinstance(min_ca, BaseAttribute)
        if max_ca is not None:
            assert isinstance(max_ca, BaseAttribute)

        if ((min_attest is None) and (max_attest is None) and
                (min_ca is None) and (max_ca is None)):
            # This is ok. No TCGs in attest cert.
            tcg_ok = True
            logger.debug("\nNo TCGs found in Attestation cert or CA cert. This is OK.")
        elif (min_attest is not None) and (max_attest is not None) and (min_ca is None) and (max_ca is None):
            logger.error("\nTCGs found in Attestation cert, but not in CA cert. This is invalid.")
        elif (min_attest is None) and (max_attest is None) and (min_ca is not None) and (max_ca is not None):
            logger.error("\nNo TCGs found in Attestation cert, but there are TCGs in CA cert. This is invalid.")
        elif (min_attest is not None) and (max_attest is not None) and (min_ca is not None) and (max_ca is not None):
            if (min_ca.value <= min_attest.value <=
                    max_attest.value <= max_ca.value):
                tcg_ok = True
                logger.debug("\nTCG values fall within CA constraints.")
            else:
                logger.error("\nTCG values are outside the CA constraints.")
        else:
            logger.error("\nInvalid TCG values")

        tcg_log_mesg = "\nAttestation cert : tcg_min={0} tcg_max={1}".format(min_attest, max_attest) + \
                       "\nCA cert (allowed): tcg_min={0} tcg_max={1}\n".format(min_ca, max_ca)
        if tcg_ok is False:
            logger.error(tcg_log_mesg)
        else:
            logger.debug(tcg_log_mesg)

        return tcg_ok

    def validate_oid_from_certs(self, ca_cert_der, attest_cert_der):
        attest_cert_obj = Certificate(attest_cert_der)
        ca_cert_obj = Certificate(ca_cert_der)
        is_valid = self._validate_attributes_with_oid_rule(attest_cert_obj) and self._validate_oid_raw(attest_cert_obj.tcg_min,
                                                                                                       attest_cert_obj.tcg_max,
                                                                                                       ca_cert_obj.tcg_min,
                                                                                                       ca_cert_obj.tcg_max)
        return is_valid

    def validate_config(self, imageinfo):
        sa = imageinfo.signing_attributes
        if sa.num_root_certs == 0:
            raise ConfigError('Number of root certificates cannot be set to zero')
        elif sa.num_root_certs > 16:
            raise ConfigError('Number of root certificates cannot be more than 16')
        elif sa.mrc_index and sa.mrc_index >= sa.num_root_certs:
            raise ConfigError('Multirootcert index ' + str(sa.mrc_index) + ' must be smaller than the number of root certs ' + str(sa.num_root_certs))
        self._validate_oid_values(sa, sa)

    def validate_oid_from_config(self, ca_cert_path, signing_attributes):
        ca_cert_obj = Certificate(path=ca_cert_path)

        min_str = signing_attributes.object_id.min
        max_str = signing_attributes.object_id.max

        min_attest = Attribute.init(num_bits=32, string=min_str)
        max_attest = Attribute.init(num_bits=32, string=max_str)

        min_ca = None
        max_ca = None
        if signing_attributes.object_id.name == "tcg":
            min_ca = ca_cert_obj.tcg_min
            max_ca = ca_cert_obj.tcg_max

        return self._validate_oid_raw(min_attest,
                                      max_attest,
                                      min_ca,
                                      max_ca)

    def get_openssl_subject_params(self, attest_cert_params):
        return cert_functions.get_subject_from_params(attest_cert_params)
