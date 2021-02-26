# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import hashlib
import os
from xml.etree.ElementTree import Element, SubElement, tostring

from sectools.common.utils import c_path
from sectools.features.isc.signer.cass import get_signing_request_class

BASE64_ENCODING = "Base64"


class SigningPackage(object):

    def __init__(self, to_sign, image_path, signing_package_file, signing_attributes):
        self.encoding = BASE64_ENCODING
        self.xml = None
        self.request = None
        self._initialize(to_sign, image_path, signing_package_file, signing_attributes)
        self._save_to_file(signing_package_file)

    def _initialize(self, to_sign, image_path, signing_package_file, signing_attributes):
        pathname, fname = os.path.split(signing_package_file)
        c_path.create_dir(pathname)

        self.request = get_signing_request_class(signing_attributes.cass_capability)(to_sign, "image_to_sign={0}".format(image_path), signing_attributes)
        self._convert_to_xml()

    def _convert_to_xml(self):
        sp = Element('SigningPackage', {'xmlns:xsi': 'http://www.w3.org/2001/XMLSchema-instance'})
        count = SubElement(sp, 'Count')
        count.text = "1"
        encoding = SubElement(sp, 'Encoding')
        encoding.text = BASE64_ENCODING
        sp.append(self.request.convert_to_xml())
        self.xml = tostring(sp)

    def _save_to_file(self, filename):
        xml_file = open(filename, "w")
        xml_file.write(self.xml)
        xml_file.close()

    def get_digest(self):
        return hashlib.sha256(self.xml).hexdigest()

    def get_xml(self):
        return self.xml
