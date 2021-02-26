# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from past.builtins import cmp
import re

from six import ensure_str

from sectools.common import crypto
from sectools.common.utils import c_misc
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_misc import properties_repr


class SignAttrBase(object):

    HW_ID = "HW_ID"
    SOC_HW_VERSION = "SOC_HW_VERSION"
    IN_USE_SOC_HW_VERSION = "IN_USE_SOC_HW_VERSION"
    USE_SERIAL_NUMBER_IN_SIGNING = "USE_SERIAL_NUMBER_IN_SIGNING"
    OEM_ID = "OEM_ID"
    MODEL_ID = "MODEL_ID"
    SW_ID = "SW_ID"
    DEBUG = "DEBUG"
    SW_SIZE = "SW_SIZE"
    SHA_ALGO = "SHA_ALGO"
    SHA1 = "SHA1"
    SHA256 = "SHA256"
    SHA384 = "SHA384"
    EXPONENT = 'EXPONENT'


class CertificateOuBase(object):

    OU_DICT = {SignAttrBase.SW_ID: "01",
               SignAttrBase.HW_ID: "02",
               SignAttrBase.DEBUG: "03",
               SignAttrBase.OEM_ID: "04",
               SignAttrBase.SW_SIZE: "05",
               SignAttrBase.MODEL_ID: "06",
               SignAttrBase.SHA1: "07",
               SignAttrBase.SHA256: "07",
               SignAttrBase.SHA384: "07",
               SignAttrBase.SOC_HW_VERSION: "11",
               SignAttrBase.IN_USE_SOC_HW_VERSION: "13",
               SignAttrBase.USE_SERIAL_NUMBER_IN_SIGNING: "14",
               }

    def _validate(self, attribute_name):
        if attribute_name not in self.OU_DICT:
            raise RuntimeError("Attribute name {0} is not supported in Certificate_OU"
                               .format(attribute_name))

    def GetID(self, attribute_name):
        self._validate(attribute_name)

        return self.OU_DICT.get(attribute_name)

    def GetText(self, attribute_name, attribute):
        self._validate(attribute_name)
        assert(isinstance(attribute, Attribute))

        ou_text = "{0} {1} {2}".format(self.OU_DICT.get(attribute_name),
                             attribute.str[2:],  # exclude 0x
                             attribute_name)
        return ou_text


class CertificateBase(object):

    sign_attr = SignAttrBase

    SHA_MAPPING = {
        sign_attr.SHA1: '0000',
        sign_attr.SHA256: '0001',
        sign_attr.SHA384: '0002',
    }

    certificate_ou = CertificateOuBase()

    '''
    Certificate class
    inputs:
        cert_blob: certificate binary blob (DER/PEM)
        path: certificate binary path (DER/PEM)
        cert_blob will take precedence of cert_path
    '''
    def __init__(self, cert_blob=None, path=None, allow_missing_ou=False):
        self.allow_missing_ou = allow_missing_ou
        if cert_blob is not None:
            self.cert_blob = cert_blob
        elif path is not None:
            self.cert_blob = c_misc.load_data_from_file(path)
        else:
            raise RuntimeError("cert_blob and path cannot be both None")

        self.certificate_text = crypto.cert.get_text(self.cert_blob)
        self.asn1_text = crypto.cert.get_asn1_text(self.cert_blob)

    def _get_ou_attr(self, attr_name, certificate_text):
        attr = None

        ou_id = self.certificate_ou.GetID(attr_name)
        attr_pattern = re.compile(r'OU\s*?=\s*?{0} ([a-fA-F\d]+) {1}'.format(ou_id, attr_name))
        attr_match = attr_pattern.search(ensure_str(certificate_text))
        if attr_match is not None:
            attr_str = attr_match.group(1)
            if len(attr_str) in [4, 8, 16]:
                attr = Attribute.init(num_bits=len(attr_str) * 4, string="0x" + attr_str)
            else:
                attr = attr_str

        return attr

    def _get_sha_algo(self):
        ou_id_sha_1 = self.certificate_ou.GetID(self.sign_attr.SHA1)
        ou_id_sha_256 = self.certificate_ou.GetID(self.sign_attr.SHA256)
        ou_id_sha_384 = self.certificate_ou.GetID(self.sign_attr.SHA384)
        if len(set([ou_id_sha_1, ou_id_sha_256, ou_id_sha_384])) != 1:
            raise RuntimeError(self.sign_attr.SHA1 + ", " +
                               self.sign_attr.SHA256 + ", and " +
                               self.sign_attr.SHA384 + " are not set to use the same OU field.")

        sha_algos_found = list()
        attr_pattern = re.compile(r'OU\s*?=\s*?(\d+)\s*?(\d+)\s*?(SHA\d+)')
        attr_pattern_list = attr_pattern.findall(ensure_str(self.certificate_text))
        for attr in attr_pattern_list:
            for algo in [self.sign_attr.SHA1, self.sign_attr.SHA256, self.sign_attr.SHA384]:
                check = [ou_id_sha_1, self.SHA_MAPPING[algo], algo]
                if cmp(check, list(attr)) == 0:
                    sha_algos_found.append(algo.split()[-1])
        if len(sha_algos_found) == 0:
            raise RuntimeError("SHA algorithm OU field is missing from certificate.")
        elif len(sha_algos_found) > 1:
            raise RuntimeError("Multiple SHA algorithm OU fields were found in certificate.")
        else:
            return sha_algos_found[0]

    def get_attr(self, attr_name):
        """ Return Attribute object """
        return self._get_ou_attr(attr_name, self.certificate_text)

    def get_attr_str(self, attr_name):
        """ Return attribute as string """
        attr = self.get_attr(attr_name)
        return None if attr is None else attr.str

    def get_attr_value(self, attr_name):
        """ Return attribute as int/long value """
        attr = self.get_attr(attr_name)
        return None if attr is None else attr.value

    @staticmethod
    def _get_exponent(certificate_text):
        _exponent = None
        exponent_pattern = re.compile(r'Exponent: (.*) ')
        exponent_match = exponent_pattern.search(ensure_str(certificate_text))
        if exponent_match is not None:
            exponent_str = exponent_match.group(1)
            _exponent = int(exponent_str)
        return _exponent

    def _repr_properties(self):
        attributes = (
            (self.sign_attr.SW_ID, self.sw_id),
            (self.sign_attr.HW_ID, self.hw_id),
            (self.sign_attr.IN_USE_SOC_HW_VERSION, self.in_use_soc_hw_version),
            (self.sign_attr.DEBUG, self.debug),
            (self.sign_attr.OEM_ID, self.oem_id),
            (self.sign_attr.SW_SIZE, self.sw_size),
            (self.sign_attr.MODEL_ID, self.model_id),
            (self.sign_attr.SHA_ALGO, self.sha_algo),
            (self.sign_attr.EXPONENT, self.exponent),
        )
        properties = []
        for attr_name, attr_value in attributes:
            if attr_value:
                properties.append((attr_name, attr_value))
        return properties

    def __repr__(self):
        return properties_repr(self._repr_properties())

    @property
    def hw_id(self):
        return self.get_attr(self.sign_attr.HW_ID)

    @property
    def soc_hw_version(self):
        return self.get_attr(self.sign_attr.SOC_HW_VERSION)

    @property
    def in_use_soc_hw_version(self):
        return self.get_attr(self.sign_attr.IN_USE_SOC_HW_VERSION)

    @property
    def use_serial_number_in_signing(self):
        return self.get_attr(self.sign_attr.USE_SERIAL_NUMBER_IN_SIGNING)

    @property
    def oem_id(self):
        return self.get_attr(self.sign_attr.OEM_ID)

    @property
    def model_id(self):
        return self.get_attr(self.sign_attr.MODEL_ID)

    @property
    def sw_id(self):
        return self.get_attr(self.sign_attr.SW_ID)

    @property
    def debug(self):
        return self.get_attr(self.sign_attr.DEBUG)

    @property
    def exponent(self):
        return self._get_exponent(self.certificate_text)

    @property
    def sw_size(self):
        return self.get_attr_value(self.sign_attr.SW_SIZE)

    @property
    def sha_algo(self):
        return self._get_sha_algo()

    @property
    def text(self):
        return self.certificate_text
