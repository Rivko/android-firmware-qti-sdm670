# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import re

from six import ensure_str, ensure_binary
from sectools.common.utils.c_attribute import Attribute
from .certificate_base import CertificateBase
from .certificate_base import CertificateOuBase
from .certificate_base import SignAttrBase


class SignAttr(SignAttrBase):

    SN = "SN"
    SERIAL_NUMBERS = "SERIAL_NUMBERS"
    SOC_VERS = "SOC_VERS"
    MASK_SOC_HW_VERSION = "MASK_SOC_HW_VERSION"
    CRASH_DUMP = "CRASH_DUMP"
    ROT_EN = "ROT_EN"
    APP_ID = "APP_ID"
    TCG_MIN = "TCG_MIN"
    TCG_MAX = "TCG_MAX"
    ROOT_CERT_SEL = "ROOT_CERT_SEL"
    OEM_ID_INDEPENDENT = "OEM_ID_INDEPENDENT"
    REVOCATION_ENABLEMENT = "REVOCATION_ENABLEMENT"
    ACTIVATION_ENABLEMENT = "ACTIVATION_ENABLEMENT"
    ROOT_REVOKE_ACTIVATE_ENABLE = "ROOT_REVOKE_ACTIVATE_ENABLE"
    UIE_KEY_SWITCH_ENABLE = "UIE_KEY_SWITCH_ENABLE"


class Certificate_OU(CertificateOuBase):

    OU_DICT = {SignAttr.SW_ID: "01",
               SignAttr.HW_ID: "02",
               SignAttr.DEBUG: "03",
               SignAttr.OEM_ID: "04",
               SignAttr.SW_SIZE: "05",
               SignAttr.MODEL_ID: "06",
               SignAttr.SHA1: "07",
               SignAttr.SHA256: "07",
               SignAttr.SHA384: "07",
               SignAttr.APP_ID: "08",
               SignAttr.CRASH_DUMP: "09",
               SignAttr.ROT_EN: "10",
               SignAttr.SOC_HW_VERSION: "11",
               SignAttr.SOC_VERS: "11",
               SignAttr.MASK_SOC_HW_VERSION: "12",
               SignAttr.IN_USE_SOC_HW_VERSION: "13",
               SignAttr.USE_SERIAL_NUMBER_IN_SIGNING: "14",
               SignAttr.OEM_ID_INDEPENDENT: "15",
               SignAttr.SN: "16",
               SignAttr.ROOT_CERT_SEL: "17",
               SignAttr.REVOCATION_ENABLEMENT: "18",
               SignAttr.ACTIVATION_ENABLEMENT: "19",
               SignAttr.ROOT_REVOKE_ACTIVATE_ENABLE: "20",
               SignAttr.UIE_KEY_SWITCH_ENABLE: "21"
               }


class Certificate(CertificateBase):

    sign_attr = SignAttr
    certificate_ou = Certificate_OU()

    # OID defined for TCG in attestation certificate
    TCG_OID = "1.3.6.1.4.1.1449.9.6.3"
    TCG_OID_NAME = "tcg"

    '''
    Certificate class
    inputs:
        cert_blob: certificate binary blob (DER/PEM)
        path: certificate binary path (DER/PEM)
        cert_blob will take precedence of cert_path

    Attributes for query:
        tcg_min
        tcg_max
    '''
    def __init__(self, cert_blob=None, path=None, allow_missing_ou=False):
        super(Certificate, self).__init__(cert_blob=cert_blob, path=path,
                                          allow_missing_ou=allow_missing_ou)
        self.tcg_min, self.tcg_max = self._get_oid(self.asn1_text, name="tcg")

    @classmethod
    def GetOIDByName(cls, name):
        if name == cls.TCG_OID_NAME:
            oid = cls.TCG_OID
        else:
            raise RuntimeError("OID name={0} is not supported".format(name))

        return oid

    @classmethod
    def GetOIDAttrName(cls, name):
        if name == cls.TCG_OID_NAME:
            attr_min = cls.sign_attr.TCG_MIN
            attr_max = cls.sign_attr.TCG_MAX
        else:
            raise RuntimeError("OID name={0} is not supported".format(name))

        return attr_min, attr_max

    def _get_oid(self, asn1_text, name):
        _min = None
        _max = None

        oid = ensure_binary(self.GetOIDByName(name))

        oid_ext = asn1_text.find(oid)
        if oid_ext > -1:
            oid_pattern = re.compile(r'\[HEX DUMP\]:([a-fA-F\d]+)')
            oid_match = oid_pattern.search(ensure_str(asn1_text[oid_ext:]))
            if oid_match is not None:
                # tcg_str = tcg_match.group(1).replace(' ', '') #remove spaces
                oid_str = oid_match.group(1)
                _min = Attribute.init(num_bits=32, string="0x" + oid_str[0:8])
                _max = Attribute.init(num_bits=32, string="0x" + oid_str[8:16])

        return _min, _max

    def _get_soc_vers(self):
        attr = None
        ou_id = self.certificate_ou.GetID(self.sign_attr.SOC_VERS)
        attr_pattern = re.compile(r'OU\s*?=\s*?{0} ([a-fA-F\d ]+) {1}'.format(ou_id, self.sign_attr.SOC_VERS))
        attr_match = attr_pattern.search(ensure_str(self.certificate_text))
        if attr_match is not None:
            attr = attr_match.group(1).replace(" 0000", "").split()
            for index, soc_ver in enumerate(attr):
                attr[index] = "0x" + soc_ver
            attr = " ".join(attr)
        return attr

    def _get_multi_serial_numbers(self):
        attr = None
        ou_id = self.certificate_ou.GetID(self.sign_attr.SN)
        attr_pattern = re.compile(r'OU\s*?=\s*?{0} ([a-fA-F\d ]+) {1}'.format(ou_id, self.sign_attr.SN))
        attr_match = attr_pattern.findall(ensure_str(self.certificate_text))
        if attr_match is not None:
            attr_match.reverse()
            attr = []
            for group in attr_match:
                attr += group.replace(" 00000000", "").split()
            for index, serial in enumerate(attr):
                attr[index] = "0x" + serial
            attr = " ".join(attr)
        return attr

    def get_attr(self, attr_name):
        """ Return Attribute object """
        attr = None
        if attr_name == self.sign_attr.TCG_MIN:
            attr = self.tcg_min
        elif attr_name == self.sign_attr.TCG_MAX:
            attr = self.tcg_max
        else:
            attr = self._get_ou_attr(attr_name, self.certificate_text)

        return attr

    def _repr_properties(self):
        attributes = (
            (self.sign_attr.SW_ID, self.sw_id),
            (self.sign_attr.HW_ID, self.hw_id),
            (self.sign_attr.IN_USE_SOC_HW_VERSION, self.in_use_soc_hw_version),
            (self.sign_attr.MASK_SOC_HW_VERSION, self.mask_soc_hw_version),
            (self.sign_attr.SOC_VERS, self.soc_vers),
            (self.sign_attr.SERIAL_NUMBERS, self.multi_serial_numbers),
            (self.sign_attr.USE_SERIAL_NUMBER_IN_SIGNING, self.use_serial_number_in_signing),
            (self.sign_attr.OEM_ID_INDEPENDENT, self.oem_id_independent),
            (self.sign_attr.DEBUG, self.debug),
            (self.sign_attr.OEM_ID, self.oem_id),
            (self.sign_attr.SW_SIZE, self.sw_size),
            (self.sign_attr.MODEL_ID, self.model_id),
            (self.sign_attr.SHA_ALGO, self.sha_algo),
            (self.sign_attr.APP_ID, self.app_id),
            (self.sign_attr.CRASH_DUMP, self.crash_dump),
            (self.sign_attr.ROT_EN, self.rot_en),
            (self.sign_attr.EXPONENT, self.exponent),
            (self.sign_attr.TCG_MIN, self.tcg_min),
            (self.sign_attr.TCG_MAX, self.tcg_max),
            (self.sign_attr.ROOT_CERT_SEL, self.root_cert_sel),
            (self.sign_attr.REVOCATION_ENABLEMENT, self.revocation_enablement),
            (self.sign_attr.ACTIVATION_ENABLEMENT, self.activation_enablement),
            (self.sign_attr.ROOT_REVOKE_ACTIVATE_ENABLE, self.root_revoke_activate_enable),
            (self.sign_attr.UIE_KEY_SWITCH_ENABLE, self.uie_key_switch_enable)
        )
        properties = []
        for attr_name, attr_value in attributes:
            if attr_value:
                properties.append((attr_name, attr_value))
        return properties

    @property
    def oem_id_independent(self):
        return self.get_attr(self.sign_attr.OEM_ID_INDEPENDENT)

    @property
    def root_cert_sel(self):
        return self.get_attr(self.sign_attr.ROOT_CERT_SEL)

    @property
    def tcg_min(self):
        return self._tcg_min

    @tcg_min.setter
    def tcg_min(self, value):
        self._tcg_min = value

    @property
    def tcg_max(self):
        return self._tcg_max

    @tcg_max.setter
    def tcg_max(self, value):
        self._tcg_max = value

    @property
    def soc_vers(self):
        return self._get_soc_vers()

    @property
    def mask_soc_hw_version(self):
        return self.get_attr(self.sign_attr.MASK_SOC_HW_VERSION)

    @property
    def revocation_enablement(self):
        return self.get_attr(self.sign_attr.REVOCATION_ENABLEMENT)

    @property
    def activation_enablement(self):
        return self.get_attr(self.sign_attr.ACTIVATION_ENABLEMENT)

    @property
    def root_revoke_activate_enable(self):
        return self.get_attr(self.sign_attr.ROOT_REVOKE_ACTIVATE_ENABLE)

    @property
    def uie_key_switch_enable(self):
        return self.get_attr(self.sign_attr.UIE_KEY_SWITCH_ENABLE)

    @property
    def multi_serial_numbers(self):
        return self._get_multi_serial_numbers()

    @property
    def crash_dump(self):
        return self.get_attr(self.sign_attr.CRASH_DUMP)

    @property
    def rot_en(self):
        return self.get_attr(self.sign_attr.ROT_EN)

    @property
    def app_id(self):
        return self.get_attr(self.sign_attr.APP_ID)
