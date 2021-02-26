# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Jan 28, 2016

@author: coryf
'''

import binascii
from xml.etree.ElementTree import Element, SubElement

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer import signerutils
from sectools.features.isc.signer.cass import CASS_signing_request_mgr
from sectools.features.isc.signer.signerutils.certificate import Certificate
from sectools.features.isc import secimage


class BaseCASSSigningRequest(SecPluginIntf_Abs):

    EXP_3 = "EXP_3"
    EXPONENT = "EXPONENT"
    CERT_SHA256 = "CERT_SHA256"

    @classmethod
    def is_plugin(cls):
        return False

    @classmethod
    def get_plugin_id(cls):
        return "base_cass_signing_request"

    @classmethod
    def get_plugin_manager(cls):
        return CASS_signing_request_mgr

    @classmethod
    def get_supported_capabilities(cls):
        """
        :return: A list of CASS capabilities that are supported by this class
        """
        raise NotImplementedError("Plugin CASS signing request class must implement get_supported_capabilities.")

    def __init__(self, to_sign, description, signing_attributes):
        self.description = description
        self.digest = binascii.b2a_hex(to_sign)
        logger.debug("Digest to sign: \"{0}\"".format(self.digest))
        self.capability = signing_attributes.cass_capability

        # Call derived classes' attribute methods
        self.attributes_to_add = self._create_attributes_to_add()
        self.attributes_to_override = self._create_attributes_to_override()
        self.attributes_to_remove = self._create_attributes_to_remove()

        # Construct attribute dictionary
        self._init_attribute_dict(to_sign, signing_attributes)
        self._apply_attribute_additions()
        self._apply_attribute_overrides()
        self._apply_attribute_removals()

    def _init_attribute_dict(self, to_sign, signing_attributes):
        # Initialize attribute dict values from signing_attributes
        self.attribute_dict = dict()
        # TODO: once cass server hex value spec is updated use utils format_hex function to properly format hex attributes

        # Add required attributes

        # SW_ID
        self.attribute_dict.update({Certificate.SIGNATTR_SW_ID: signing_attributes.sw_id})

        # HW_ID
        self.attribute_dict.update({Certificate.SIGNATTR_HW_ID: "0x%s" % signerutils.get_hmac_params_from_config(signing_attributes).msm_id_str})

        # DEBUG is added in derived request classes

        # OEM_ID
        self.attribute_dict.update({Certificate.SIGNATTR_OEM_ID: signing_attributes.oem_id})

        # SW_SIZE
        self.attribute_dict.update({Certificate.SIGNATTR_SW_SIZE: "0x%.8X" % len(to_sign)})

        # MODEL_ID
        self.attribute_dict.update({Certificate.SIGNATTR_MODEL_ID: signing_attributes.model_id})

        # SHA256
        if signing_attributes.hash_algorithm is not None and signing_attributes.hash_algorithm.lower() == "sha1":
            self.attribute_dict.update({Certificate.SIGNATTR_SHA256: "FALSE"})
        elif signing_attributes.hash_algorithm is None or signing_attributes.hash_algorithm.lower() == "sha256":
            self.attribute_dict.update({Certificate.SIGNATTR_SHA256: "TRUE"})
        else:
            raise RuntimeError("Hash algorithm value of {0} is invalid for CASS Signing Request.".format(signing_attributes.hash_algorithm))

        # EXP_3 is added in derived SBL request classes

        # Add optional attributes

        # APP_ID
        if signing_attributes.app_id is not None:
            if signing_attributes.sw_id.lower().endswith("c"):
                self.attribute_dict.update({Certificate.SIGNATTR_APP_ID: signing_attributes.app_id})
            else:
                raise RuntimeError("sw_id value of {0} is invalid for CASS Signing Request when specifying an app_id.".format(signing_attributes.sw_id))

        # CRASH_DUMP
        if signing_attributes.crash_dump is not None:
            self.attribute_dict.update({Certificate.SIGNATTR_CRASH_DUMP: signing_attributes.crash_dump})

    def _apply_attribute_additions(self):
        if self.attributes_to_add and len(self.attributes_to_add) > 0:
            for key, value in self.attributes_to_add.iteritems():
                if key not in self.attribute_dict.keys():
                    self.attribute_dict.update({key: value})
                    logger.debug("Added \"{0}\" attribute with value \"{1}\" to CASS signing request.".format(key, value))
                else:
                    raise RuntimeError("Attempted to add \"{0}\" attribute to CASS signing request but \"{0}\" attribute already exists in CASS signing request.\n"
                                       "If you would like to override the value of \"{0}\", add {{\"{0}\": \"{1}\"}} to the dictionary created in _create_attributes_to_override".format(key, value))
        else:
            logger.debug("No additional attributes were added to CASS signing request.")

    def _apply_attribute_overrides(self):
        if self.attributes_to_override and len(self.attributes_to_override) > 0:
            for key, value in self.attributes_to_override.iteritems():
                if key in self.attribute_dict.keys():
                    # add new entry to dictionary
                    self.attribute_dict.update(value)
                    # remove old dictionary entry
                    if key != value.keys()[0]:
                        del self.attribute_dict[key]
                    logger.debug("Overrode CASS signing request's \"{0}\" attribute with {1}.".format(key, value))
                else:
                    raise RuntimeError("Attempted to override CASS signing request's \"{0}\" attribute with \"{1}\" but \"{0}\" is not an attribute of CASS signing request.\n"
                                       "If you would like to add \"{1}\" to CASS signing request's attributes, add {2} to the dictionary created in _create_attributes_to_add".format(key, value.keys()[0], value))
        else:
            logger.debug("No CASS signing request attributes were overridden.")

    def _apply_attribute_removals(self):
        if self.attributes_to_remove and len(self.attributes_to_remove) > 0:
            for item in self.attributes_to_remove:
                if item in self.attribute_dict.keys():
                    del self.attribute_dict[item]
                    logger.debug("Removed CASS signing request's \"{0}\" attribute.".format(item))
                else:
                    raise RuntimeError("Attempted to remove CASS signing request's \"{0}\" attribute but \"{0}\" is not an attribute of CASS signing request.".format(item))
        else:
            logger.debug("No CASS signing request attributes were removed.")

    def convert_to_xml(self):
        sr = Element('SigningRequest', {'sequenceId': '1'})
        sid = SubElement(sr, 'SourceID')
        sid.text = secimage.__version__
        desc = SubElement(sr, 'Description')
        desc.text = self.description
        cap = SubElement(sr, 'Capability', {'name': self.capability})
        if len(self.attribute_dict) > 0:
            attrs = SubElement(cap, 'SigningAttributes')
            for key in self.attribute_dict:
                attr = SubElement(attrs, 'SigningAttribute', {'name': key})
                attr.text = self.attribute_dict[key]
        digest = SubElement(sr, 'Digest')
        digest.text = self.digest
        return sr

    def _create_attributes_to_add(self):
        """
        :return: A dictionary of values to add to those present in BaseCASSSigningRequest.
        The dictionary must be in the form {key_of_attribute_to_add_1: value_of_attribute_to_add_1, key_of_attribute_to_add_2: value_of_attribute_to_add_2, ...}.
        """
        raise NotImplementedError("Plugin CASS signing request class must implement _create_attributes_to_add.")

    def _create_attributes_to_override(self):
        """
        :return: A dictionary of dictionaries to override values present in BaseCASSSigningRequest.
        The dictionary entries must consist of old keys mapping to new dictionary key-value pairs.
        The dictionary must be in the form {old_attribute_key_1: {new_attribute_key_1: new_attribute_value_1}, old_attribute_key_2: {new_attribute_key_2: new_attribute_value_2}, ...}.
        """
        raise NotImplementedError("Plugin CASS signing request class must implement _create_attributes_to_override.")

    def _create_attributes_to_remove(self):
        """
        :return: A list of values to remove from those present in BaseCASSSigningRequest.
        The list must be in the form [key_of_attribute_to_delete_1, key_of_attribute_to_delete_2, key_of_attribute_to_delete_3, ...].
        """
        raise NotImplementedError("Plugin CASS signing request class must implement _create_attributes_to_remove.")
