# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import string

from .certconfigparser_base import CertDataBase
from sectools.common.utils import c_misc
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from .certconfigparser_base import SIGNING_STR


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
    cert_info_dict['mrc_index'] = sa.mrc_index if hasattr(sa, "mrc_index") else 0
    cert_info_dict['dp_path'] = dp_path
    cert_info_dict['dsa_type'] = sa.dsa_type
    cert_info_dict['ecdsa_curve'] = sa.ecdsa_curve if hasattr(sa, "ecdsa_curve") else ""
    cert_info_dict['keysize'] = sa.key_size
    cert_info_dict['exponent'] = sa.exponent
    cert_info_dict['padding'] = sa.rsa_padding
    cert_info_dict['hash_algorithm'] = sa.hash_algorithm
    return CertData(cert_info_dict)


class CertData(CertDataBase):

    def __init__(self, cert_dict):
        super(CertData, self).__init__(cert_dict)
        self.cert_mrc_index = cert_dict.get('mrc_index', 0)

        # Initialize mrc_index string for injection into cert_path later #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        if self.is_mrc() == 'True':
            if self.cert_mrc_index is not None:
                self.cert_mrc_index = str(self.cert_mrc_index)
            else:
                raise RuntimeError("mrc_index not given in config file but is needed for multi-root certificates")
        else:
            self.cert_mrc_index = ''

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
            # return 'ERROR'

        # Inject the mrc_index into file string before the period #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        asset_file_name = asset_file_name.replace('.', str(self.cert_mrc_index) + '.')

        # check if asset file exists #
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
        asset_file_path = os.path.join(query_result.path, asset_file_name)
        asset_file_path = os.path.normpath(asset_file_path)

        if not c_path.validate_file(asset_file_path):
            raise RuntimeError('DataProvisioner: ' + asset_file_path + ' is not found')

        self.log_once(asset_tag + " = " + asset_file_path)
        logger.debug("DataProvisioner config: " + str(query_result.config))

        return asset_file_path

    def get_rootcerts(self, num_root_certs=None):

        root_cert_list = []

        if self.is_mrc() == 'True':
            if num_root_certs is not None:
                root_cert_list = self.get_cert_list(num_root_certs)
            else:
                raise RuntimeError(
                    'Cert_Data.get_rootcerts() must be called with num_root_certs on mrc\'s')

        return root_cert_list

    def is_mrc(self):
        return self.query_data_taginfo(SIGNING_STR, self.LOCAL_STR, self.asset_dirname,
                                       'is_mrc', default="False")

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
