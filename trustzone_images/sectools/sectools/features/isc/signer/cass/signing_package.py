# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import hashlib
import os
from xml.etree.ElementTree import Element, SubElement, tostring

from six import ensure_str

from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.cass import get_signing_request_class

BASE64_ENCODING = "Base64"


class SigningPackage(object):

    def __init__(self, hash_to_sign, data_to_sign, image_path, signing_package_file, signing_attributes):
        self.encoding = BASE64_ENCODING
        self.xml = None
        self.request = None
        self._initialize(hash_to_sign, data_to_sign, image_path, signing_package_file, signing_attributes)
        self._save_to_file(signing_package_file)

    def _initialize(self, hash_to_sign, data_to_sign, image_path, signing_package_file, signing_attributes):
        pathname, fname = os.path.split(signing_package_file)
        c_path.create_dir(pathname)
        self.request = get_signing_request_class(signing_attributes.cass_capability, signing_attributes.secboot_version,
                                                 signing_attributes.sw_id)(hash_to_sign, data_to_sign, image_path, signing_attributes)
        self._convert_to_xml()

    def _convert_to_xml(self):
        sp = Element('SigningPackage', {'xmlns:xsi': 'http://www.w3.org/2001/XMLSchema-instance'})
        count = SubElement(sp, 'Count')
        count.text = "1"
        encoding = SubElement(sp, 'Encoding')
        encoding.text = BASE64_ENCODING
        request_xml = self.request.convert_to_xml()
        logger.debug("CASS Signing Request: " + ensure_str(tostring(request_xml)))
        sp.append(request_xml)
        self.xml = tostring(sp)

    def _save_to_file(self, filename):
        xml_file = open(filename, "wb")
        xml_file.write(self.xml)
        xml_file.close()

    def get_digest(self):
        return ensure_str(hashlib.sha256(self.xml).hexdigest())

    def get_xml(self):
        return self.xml
