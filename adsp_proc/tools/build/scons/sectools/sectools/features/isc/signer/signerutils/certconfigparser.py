#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os
import string
from sectools.common.crypto import utility_functions
from sectools.common.data_provisioning.data_prov import DataProvisioner
from sectools.common.utils import c_path
from sectools.common.utils import c_misc
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

class CertData(object):

    def log_once(self, msg):
        if msg not in self.message_print_set:
            self.message_print_set.add(msg)

    def __init__(self, cert_dict):

        # Data transfer from initialization dictionary #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        self.cert_config_id = cert_dict['id']
        self.chipset = cert_dict['chip']
        self.cert_keysize = cert_dict['keysize']
        self.cert_exponent = cert_dict['exponent']
        self.cert_mrc_index = cert_dict['mrc_index']
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

        if tag_type == 'root':

            if self.get_root_pre() == 'True':
                params = {}
                params['certificate_path'] = self.get_root_cert()
                params['private_key_path'] = self.get_root_priv_key()
            else:
                # get the cert parameters from config file #
                #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
                param_string = self.get_root_params()
                param_list = filter(None, param_string.split('\n'))

                params = utility_functions.normalize_param_list_into_dict(param_list)

        elif tag_type == 'attest_ca':

            if self.get_attest_ca_pre() == 'True':
                params = {}
                params['certificate_path'] = self.get_attest_ca_cert()
                params['private_key_path'] = self.get_attest_ca_priv_key()
            else:
                # get the cert parameters from config file #
                #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
                param_string = self.get_attest_ca_params()
                param_list = filter(None, param_string.split('\n'))

                params = utility_functions.normalize_param_list_into_dict(param_list)

        elif tag_type == 'attest':

            if self.get_attest_pre() == 'True':
                params = {}
                params['certificate_path'] = self.get_attest_cert()
                params['private_key_path'] = self.get_attest_priv_key()
            else:
                # get the cert parameters from config file #
                #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
                param_string = self.get_attest_cert_params()
                param_list = filter(None, param_string.split('\n'))

                params = utility_functions.normalize_param_list_into_dict(param_list)
        else:
            raise RuntimeError('invalid tag type passed to _extract_params()')

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
        key_str = KEY_PREFIX + str(self.cert_keysize)
        exp_str = EXP_PREFIX + str(self.cert_exponent)
        supported_options = [key_str + exp_str, '']
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
