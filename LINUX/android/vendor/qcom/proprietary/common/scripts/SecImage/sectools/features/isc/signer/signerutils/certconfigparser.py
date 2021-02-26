# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import string

from sectools.common.crypto import utility_functions
from sectools.common.data_provisioning.data_prov import DataProvisioner
from sectools.common.utils import c_misc
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger

'''Constants used for locating DP_assets'''
#*************************************************
SIGNING_STR = 'Signing'
LOCAL_STR = 'Local'
CERT_TAG = 'root_cert'
PRIV_KEY_TAG = 'root_private_key'
ATTEST_CA_CERT_TAG = 'attest_ca_cert'
ATTEST_CA_PRIV_KEY_TAG = 'attest_ca_private_key'
ATTEST_CERT_TAG = 'attest_cert'
ATTEST_PRIV_KEY_TAG = 'attest_private_key'
#*************************************************

KEY_PREFIX = "-key"
EXP_PREFIX = "_exp"
PAD_PREFIX = "_padding"
HASH_PREFIX = "_hash"
CURVE_PREFIX = "-"


def get_cert_data(imageinfo, dp_path):
    """Returns the cert data object.

    :type imageinfo: ImageInfo
    :type dp_path: str
    :rtype: CertData
    """
    sa = imageinfo.signing_attributes
    cert_info_dict = {}
    cert_info_dict['id'] = imageinfo.cert_config
    cert_info_dict['chip'] = imageinfo.chipset
    cert_info_dict['mrc_index'] = sa.mrc_index
    cert_info_dict['dp_path'] = dp_path
    cert_info_dict['dsa_type'] = sa.dsa_type
    cert_info_dict['ecdsa_curve'] = sa.ecdsa_curve
    cert_info_dict['keysize'] = sa.key_size
    cert_info_dict['exponent'] = sa.exponent
    cert_info_dict['padding'] = sa.rsa_padding
    cert_info_dict['hash_algorithm'] = sa.hash_algorithm
    return CertData(cert_info_dict)


class CertProperty(object):

    def __init__(self, cert_path=None, priv_path=None):
        self.cert_path = cert_path
        self.priv_path = priv_path
        self.params = {}

    def init_from_params_dict(self, params):
        """
        :type params: dict
        """
        self.cert_path = params.pop('certificate_path', None)
        self.priv_path = params.pop('private_key_path', None)
        self.params = params

    def validate(self):
        for key in self.params:
            if key not in ['C', 'CN', 'L', 'O', 'ST', 'OU', 'emailAddress']:
                return False
        return True

class CertsInfo(object):

    def __init__(self):
        self.root = CertProperty()
        self.ca = CertProperty()
        self.attest = CertProperty()
        self.root_certs = []


class CertData(object):

    def log_once(self, msg):
        if msg not in self.message_print_set:
            self.message_print_set.add(msg)

    def __init__(self, cert_dict):

        # Data transfer from initialization dictionary #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        self.cert_config_id = cert_dict['id']
        self.chipset = cert_dict['chip']
        self.cert_mrc_index = cert_dict['mrc_index']
        self.dsa_type = cert_dict['dsa_type'] if 'dsa_type' in cert_dict else 'rsa'
        self.cert_mrc_index = cert_dict['mrc_index']

        self.cert_keysize = cert_dict['keysize']
        self.cert_exponent = cert_dict['exponent']
        if 'padding' in cert_dict and cert_dict['padding'] is not None:
            self.cert_padding = cert_dict['padding'].upper()
        else:
            self.cert_padding = 'PKCS'
        if 'hash_algorithm' in cert_dict and cert_dict['hash_algorithm'] is not None:
            self.cert_hash_algorithm = cert_dict['hash_algorithm']
        else:
            self.cert_hash_algorithm = 'sha256'

        if self.dsa_type == 'rsa':
            self.cert_keysize = cert_dict['keysize']
            self.cert_exponent = cert_dict['exponent']
            self.ecdsa_curve = None
        elif self.dsa_type == 'ecdsa':
            self.ecdsa_curve = cert_dict['ecdsa_curve']
            self.cert_keysize = None
            self.cert_exponent = None


        self.dp_basepath = cert_dict['dp_path']
        self.dp_basepath = os.path.normpath(self.dp_basepath)

        # LOCAL_STR changes for test assets #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        self.LOCAL_STR = LOCAL_STR

        # Initialize data-provisioner with dp_basepath #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        if self.dp_basepath is not None:
            self.data_prov = DataProvisioner(self.dp_basepath)
        else:
            raise RuntimeError("Invalid basepath provided to data provisioner.")

        # Construct the asset dirname from cert_dict #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        get_asset_func = self.get_asset
        try:
            import sectools.features.isc.signer.signerutils.test.intf as tester
            get_asset_func = lambda x: tester.get_test_asset(self, x)
        except ImportError:
            pass

        self.LOCAL_STR, self.asset_dirname = get_asset_func(self.cert_config_id)

        # Initialize mrc_index string for injection into cert_path later #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        if self.is_mrc() == 'True':
            if self.cert_mrc_index is not None:
                self.cert_mrc_index = str(self.cert_mrc_index)
            else:
                raise RuntimeError("mrc_index not given in config file but is needed for multi-root certificates")
        else:
            self.cert_mrc_index = ''

        self.message_print_set = set()


    def query_data_taginfo(self, T1_namespace, T2_namespace, T3_namespace, asset_tag):

        query_result = self.data_prov.query(T1_namespace, T2_namespace, T3_namespace)[0]

        # check if asset folder has been found correctly #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        if not query_result.config:
            raise RuntimeError('DataProvisioner: ' + c_path.join(query_result.path, 'config.xml') + ' is not found')

        '''check if config.xml is valid'''
        try:
            tag_info = str(getattr(query_result.config.METACONFIG, asset_tag))
            return tag_info

        except Exception:
            raise RuntimeError('DataProvisioner: ' + asset_tag + ' is not found in config.xml')
            return 'ERROR'


    def query_data_path(self, T1_namespace, T2_namespace, T3_namespace, asset_tag):

        query_result = self.data_prov.query(T1_namespace, T2_namespace, T3_namespace)[0]

        # check if asset folder has been found correctly #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        if not query_result.config:
            raise RuntimeError('DataProvisioner: ' + c_path.join(query_result.path, 'config.xml') + ' is not found')

        # check if config.xml is valid #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        try:
            asset_file_name = str(getattr(query_result.config.METACONFIG, asset_tag))
        except Exception:
            raise RuntimeError('DataProvisioner: ' + asset_tag + ' is not found in config.xml')
            return 'ERROR'

        # Inject the mrc_index into file string before the period #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        asset_file_name = string.replace(asset_file_name, '.', str(self.cert_mrc_index) + '.')

        # check if asset file exists #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        asset_file_path = os.path.join(query_result.path, asset_file_name)
        asset_file_path = os.path.normpath(asset_file_path)

        if not c_path.validate_file(asset_file_path):
            raise RuntimeError('DataProvisioner: ' + asset_file_path + ' is not found')

        self.log_once(asset_tag + " = " + asset_file_path)
        logger.debug("DataProvisioner config: " + str(query_result.config))

        return asset_file_path

    def get_certs_info(self, num_root_certs):
        crypto_params_dict = self.get_crypto_params()
        root_certs = self.get_rootcerts(num_root_certs)
        certs_info = CertsInfo()
        certs_info.root.init_from_params_dict(crypto_params_dict['root_certificate_properties'])
        certs_info.ca.init_from_params_dict(crypto_params_dict['attest_ca_certificate_properties'])
        certs_info.attest.init_from_params_dict(crypto_params_dict['attest_certificate_properties'])
        certs_info.root_certs = root_certs
        return certs_info

    def get_crypto_params(self):

        crypto_params_dict = {}

        if self.get_root_pre() != 'ERROR':
            crypto_params_dict['root_certificate_properties'] = self._extract_params('root')
        else:
            raise RuntimeError("No root field detected in asset's config file")

        if self.get_attest_ca_pre() != 'ERROR':
            crypto_params_dict['attest_ca_certificate_properties'] = self._extract_params('attest_ca')
        else:
            raise RuntimeError("No attest_ca field detected in asset's config file")

        if self.get_attest_pre() != 'ERROR':
            crypto_params_dict['attest_certificate_properties'] = self._extract_params('attest')
        else:
            raise RuntimeError("No attest field detected in asset's config file")

        return crypto_params_dict


    def _extract_params(self, tag_type):

        # set getter functions based on tag_type
        params = {}
        if tag_type == 'root':
            get_pre_funct = self.get_root_pre
            get_cert_funct = self.get_root_cert
            get_key_funct = self.get_root_priv_key
            get_params_funct = self.get_root_params
        elif tag_type == 'attest_ca':
            get_pre_funct = self.get_attest_ca_pre
            get_cert_funct = self.get_attest_ca_cert
            get_key_funct = self.get_attest_ca_priv_key
            get_params_funct = self.get_attest_ca_params
        elif tag_type == 'attest':
            get_pre_funct = self.get_attest_pre
            get_cert_funct = self.get_attest_cert
            get_key_funct = self.get_attest_priv_key
            get_params_funct = self.get_attest_cert_params
        else:
            raise RuntimeError('invalid tag type passed to _extract_params()')

        if get_pre_funct() == 'True':
            params['certificate_path'] = get_cert_funct()
            params['private_key_path'] = get_key_funct()
        else:
            # get the cert parameters from config file #
            #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
            param_string = get_params_funct()
            param_list = filter(None, param_string.split('\n'))

            params = utility_functions.normalize_param_list_into_dict(param_list)

        return params


    def get_rootcerts(self, num_root_certs=None):

        root_cert_list = []

        if self.is_mrc() == 'True':
            if num_root_certs is not None:
                root_cert_list = self.get_cert_list(num_root_certs)
            else:
                raise RuntimeError('Cert_Data.get_rootcerts() must be called with num_root_certs on mrc\'s')

        return root_cert_list


    def get_cert_list(self, num_root_certs):

        save_index = self.cert_mrc_index
        cert_list = []

        for i in range(0, num_root_certs):
            self.cert_mrc_index = i
            cert_path = self.get_root_cert()

            if os.path.isfile(cert_path) is False:
                err_str = "certificate_path does not exist: {0}!".format(cert_path)
                raise RuntimeError(err_str)

            cert = c_misc.load_data_from_file(cert_path)
            cert_list.append(cert)

        self.cert_mrc_index = save_index

        return cert_list

    def get_asset(self, asset_name):
        supported_options = []
        if self.dsa_type == 'rsa' or self.dsa_type is None:
            key_str = KEY_PREFIX + str(self.cert_keysize)
            exp_str = EXP_PREFIX + str(self.cert_exponent)
            pad_str = PAD_PREFIX + str(self.cert_padding) if self.cert_padding == 'PSS' else ''
            hash_str = (HASH_PREFIX + str(self.cert_hash_algorithm.upper())) if self.cert_hash_algorithm != 'sha256' else ''
            supported_options = [key_str + exp_str + pad_str + hash_str, '']
        else:
            curve_str = CURVE_PREFIX + str(self.ecdsa_curve)
            supported_options = [curve_str, '']
        searched_paths = []
        for option in supported_options:
            try:
                searched_paths.append(os.path.join(self.dp_basepath, SIGNING_STR, self.LOCAL_STR, asset_name + option))
                self.data_prov.query(SIGNING_STR, self.LOCAL_STR, asset_name + option)[0]
                return self.LOCAL_STR, asset_name + option
            except Exception:
                pass
        else:
            raise RuntimeError("Selected cert config: '" + asset_name + "' not found in any of the following locations:  " + "\n" + "\n".join(searched_paths) + "\n")

    def is_mrc(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'is_mrc')

    def get_path_globals(self):
        globals_list = {}
        globals_list['SIGNING_STR'] = SIGNING_STR
        globals_list['LOCAL_STR'] = LOCAL_STR
        return globals_list

    def get_root_pre(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'root_pre')

    def get_attest_ca_pre(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'attest_ca_pre')

    def get_attest_pre(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'attest_pre')

    def get_root_params(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'root_cert_params')

    def get_attest_ca_params(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'attest_ca_cert_params')

    def get_attest_cert_params(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, 'attest_cert_params')

    def get_root_cert(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, CERT_TAG)

    def get_root_priv_key(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, PRIV_KEY_TAG)

    def get_attest_ca_cert(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, ATTEST_CA_CERT_TAG)

    def get_attest_ca_priv_key(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, ATTEST_CA_PRIV_KEY_TAG)

    def get_attest_cert(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, ATTEST_CERT_TAG)

    def get_attest_priv_key(self):
        return self.query_data_path(SIGNING_STR, self.LOCAL_STR, self.asset_dirname, ATTEST_PRIV_KEY_TAG)
