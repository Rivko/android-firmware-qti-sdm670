# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.crypto import cert as cert_functions
from sectools.common.crypto import ecdsa as ecdsa_functions
from sectools.common.crypto import rsa as rsa_functions
from sectools.common.crypto import utils
from sectools.common.utils import c_path
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_logging import logger
from sectools.features.isc import data_provision_enquirer as openssl_fetch_module
from sectools.features.isc.cfgparser.defines import IOT_MRC_CHIPSETS
from sectools.features.isc.signer import signer_mgr
from sectools.features.isc.signer.base_signer import BaseSigner
from sectools.features.isc.signer.signerutils.certconfigparser import get_cert_data
from sectools.features.isc.signer.signerutils.certificate import Certificate
from sectools.features.isc.signer.utils import misc as utils_misc
from sectools.features.isc.signer.utils.certkey import CertKeyPair
from sectools.features.isc.signer.utils.hmac import HMAC

DEBUG_DISABLED = 2

SHA1_OU_STRING      = '07 0000 SHA1'
SHA256_OU_STRING    = '07 0001 SHA256'
SHA384_OU_STRING    = '07 0002 SHA384'

certificate_hash_alg_map = \
    {
        'sha1':     SHA1_OU_STRING,
        'sha256':   SHA256_OU_STRING,
        'sha384':   SHA384_OU_STRING,
        None:       SHA256_OU_STRING,
    }


'''Signer output class
'''
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
            if val and cert_functions.get_cert_format(val) != utils.FORMAT_DER:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_PEM,
                                                        utils.FORMAT_DER)
            setattr(self, tag, val)

        # Update root cert list
        for idx in range(len(self.root_cert_list)):
            val = self.root_cert_list[idx]
            if val and cert_functions.get_cert_format(val) != utils.FORMAT_DER:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_PEM,
                                                        utils.FORMAT_DER)
            self.root_cert_list[idx] = val

        # Update keys
        for tag in ['root_key', 'attestation_ca_key', 'attestation_key']:
            val = getattr(self, tag)
            if val and rsa_functions.get_key_format(val) != utils.FORMAT_DER:
                val = rsa_functions.get_key_in_format(val, utils.FORMAT_PEM,
                                                      utils.FORMAT_DER)
            setattr(self, tag, val)

    def generate_cert_chain(self):
        certs = ([self.attestation_cert, self.attestation_ca_cert] +
                 [c for c in self.root_cert_list])
        certs = [c for c in certs if c is not None]
        self.cert_chain = ''.join(certs)
        return self.cert_chain


'''Exception classes
'''
class ConfigError(RuntimeError):
    pass
class ExternalSignerError(RuntimeError):
    pass


class BaseSignerV2(BaseSigner):

    # Error message strings
    MSG_INVALID_32_INTEGER = '{0} {1}:{2} is not a valid 32 bit integer'

    # Certificate types
    ROOT = 'Root'
    CA = 'CA'
    ATTEST = 'Attest'
    CERT_LIST = [ROOT, CA, ATTEST]

    # Certificate properties
    CERT_DAYS = 7300
    CERT_SERIAL = 1

    # Padding types
    PAD_PSS = cert_functions.PAD_PSS
    PAD_PKCS = cert_functions.PAD_PKCS

    # PSS Fixes properties
    PAD_PSS_SALT_1 = cert_functions.PAD_PSS_SALT_1
    PAD_PSS_HASH_SHA256 = cert_functions.HASH_ALGO_SHA256

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

    def validate_config(self, imageinfo):
        sa = imageinfo.signing_attributes
        if sa.num_root_certs == 0:
            raise ConfigError('Number of root certificates cannot be set to zero')
        elif sa.num_root_certs > 16:
            raise ConfigError('Number of root certificates cannot be more than 16')
        elif sa.mrc_index and sa.mrc_index >= sa.num_root_certs:
            raise ConfigError('Multirootcert index ' + str(sa.mrc_index) + ' must be smaller than the number of root certs ' + str(sa.num_root_certs))
        self._validate_oid_values(sa, sa)

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
        self.certs = {}
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
            logger.info('Using ' + hasher.hmac_type + ' (' + hasher.hash_algo + ') for hash segment')

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
        logger.info('Initialization with dataprov. These fields might not be used in final output if overriden')

        # Extract the private and public key
        if cert_property.priv_path and c_path.validate_file(cert_property.priv_path):
            logger.info('Using a predefined ' + cert_type + ' private key from: ' + cert_property.priv_path)
            with open(cert_property.priv_path, 'rb') as fp:
                priv_key = fp.read()
                if self.using_ecdsa:
                    if ecdsa_functions.get_key_format(priv_key) != utils.FORMAT_PEM:
                        priv_key = ecdsa_functions.get_key_in_format(priv_key, utils.FORMAT_DER, utils.FORMAT_PEM)
                        pub_key = ecdsa_functions.get_public_key_from_private(priv_key)
                else:
                    if rsa_functions.get_key_format(priv_key) != utils.FORMAT_PEM:
                        priv_key = rsa_functions.get_key_in_format(priv_key, utils.FORMAT_DER, utils.FORMAT_PEM)
                        pub_key = rsa_functions.get_public_key_from_private(priv_key)

        # Extract the certificate
        if cert_property.cert_path and c_path.validate_file(cert_property.cert_path):
            logger.info('Using a predefined ' + cert_type + ' certificate from: ' + cert_property.cert_path)
            with open(cert_property.cert_path, 'rb') as fp:
                cert = fp.read()
                if cert_functions.get_cert_format(cert) != utils.FORMAT_PEM:
                    cert = cert_functions.get_cert_in_format(cert,
                                                             utils.FORMAT_DER,
                                                             utils.FORMAT_PEM)
        return cert, priv_key, pub_key

    def sign(self, data_to_sign, imageinfo, debug_dir=None, is_hash=False):
        '''
        This function returns a SignerOutput object which has all the security assets generated
        by the signer.
        '''
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

        # Get the hmac params from attestation cert
        hmac_from_cert = HMAC()
        hmac_from_cert.init_from_cert(signer_output.attestation_cert)

        # Get the hmac params from config
        hmac_from_config = HMAC()
        hmac_from_config.init_from_config(self.signing_attributes)

        # Recreate the hash to sign if necessary
        if (hmac_from_config.hmac_type == hmac_from_config.HMAC_TYPE_QC and
                not hmac_from_cert.is_equal(hmac_from_config)):
            if self.data_to_sign is not None:
                self.hash_to_sign = hmac_from_cert.hmac(self.data_to_sign)
            else:
                raise RuntimeError('HMAC params from attestation certificate cannot be used with pre-generated hash.')

        # Set the signature
        signer_output.signature = self.get_signature()
        signer_output.unsigned_hash = self.hash_to_sign

        # Update the certs
        signer_output.update_certs_format()

        # Set the cert chain
        signer_output.generate_cert_chain()

        # Print certificate properties (to make tests pass and give good debug information)
        logger.info('\nAttestation Certificate Properties:\n' +
                    str(Certificate(signer_output.attestation_cert)))

        return signer_output

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
        # Check if the cert is in der format
        if cert_functions.get_cert_format(cert) != utils.FORMAT_PEM:
            cert = cert_functions.get_cert_in_format(cert, utils.FORMAT_DER,
                                                     utils.FORMAT_PEM)
        cert_type_obj.cert = cert

        return cert_type_obj.cert, cert_type_obj.priv_key

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
                                          padding=padding,
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
                                        padding=padding,
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

        def create_ou_field_from_hex_list(ou_num, ou_name, hex_list, remove_0x, max_num_items_in_ou):
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
        debug_serials = self.signing_attributes.debug_serials.serial if self.signing_attributes.debug_serials is not None else []
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

        if self._is_oid_supported(self.signing_attributes) is True:
            if self.validate_oid_from_config(self.crypto_params.attest_ca_certificate_properties['certificate_path'], self.signing_attributes) is False:
                raise ConfigError('{0} min and max are not set correctly in configuration.' \
                                  'Signing will not continue.'.format(self.signing_attributes.object_id.name)
                                  )
            self.certs[self.ATTEST].extfile = self._generate_attestation_certificate_extensions(
                self.openssl_info.attest_ca_xts,
                self.signing_attributes.object_id.name,
                self.signing_attributes.object_id.min,
                self.signing_attributes.object_id.max)
        else:
            self.certs[self.ATTEST].extfile = self.openssl_info.attest_ca_xts

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
        certificate_hash_alg = certificate_hash_alg_map[self.signing_attributes.hash_algorithm]

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
        for index in xrange(0, len(debug_serials), 6):
            serial_sublist = debug_serials[index: index+6]
            certificate_ou_sn_list.append(create_ou_field_from_hex_list(16, "SN", serial_sublist, True, 6))
        certificate_ou_sn_list.reverse()
        certificate_ou.extend(certificate_ou_sn_list)

        # multiple soc hw version use case
        if soc_vers:
            if self.padding != self.PAD_PSS:
                logger.warning("soc_vers should be used with RSAPSS")
            certificate_ou.append(create_ou_field_from_hex_list(11, "SOC_VERS", soc_vers, True, 10))

        # IOT MRC use case
        if num_root_certs > 1 and self.config.metadata.chipset in IOT_MRC_CHIPSETS:
            certificate_root_cert_sel = '17 ' + '%0.4X' % self.signing_attributes.mrc_index + ' ROOT_CERT_SEL'
            certificate_ou.append(certificate_root_cert_sel)
            if revocation_enablement is not None and revocation_enablement != 0:
                certificate_revocation_enablement = '18 ' + '%0.16X' % revocation_enablement + ' REVOCATION_ENABLEMENT'
                certificate_ou.append(certificate_revocation_enablement)
            if activation_enablement is not None and activation_enablement != 0:
                certificate_activation_enablement = '19 ' + '%0.16X' % activation_enablement + ' ACTIVATION_ENABLEMENT'
                certificate_ou.append(certificate_activation_enablement)

        # Handle OU property binding
        params = dict(in_params)
        if 'OU' in params.keys():
            if type(params['OU']) == list:
                for item in params['OU']:
                    certificate_ou.append(item)
            else:
                certificate_ou.append(params['OU'])

        params['OU'] = certificate_ou
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

    def get_openssl_subject_params(self, attest_cert_params):
        return cert_functions.get_subject_from_params(attest_cert_params)
