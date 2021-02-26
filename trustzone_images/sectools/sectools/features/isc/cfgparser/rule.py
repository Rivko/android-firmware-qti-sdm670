# ===============================================================================
#
#  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils import c_path
from sectools.common.utils.c_data import get_duplicates, remove_duplicates_ordered
from sectools.common.utils.c_data import remove_duplicates
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import is_TA
from sectools.common.utils.c_rule import CoreRuleBase
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.cfgparser.defines import DEBUG_TAG
from sectools.features.isc.cfgparser.defines import ECDSA_CHIPSETS
from sectools.features.isc.cfgparser.defines import MRC_1_0_CHIPSETS
from sectools.features.isc.cfgparser.defines import MRC_2_0_CHIPSETS
from sectools.features.isc.advanced_defines import PLATFORM_BINDING_SOC_VERS
from sectools.features.isc.cfgparser.defines import SECBOOT_1_IMAGE_TYPES
from sectools.features.isc.cfgparser.defines import SECBOOT_2_0_CHIPSETS
from sectools.features.isc.cfgparser.defines import SECBOOT_3_0_CHIPSETS
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.imageinfo.imageinfo_utils import resolve_enumerated
from .auto_gen_xml_config import complex_general_properties
from .auto_gen_xml_config import complex_images_list
from .auto_gen_xml_config import complex_metadata
from .auto_gen_xml_config import complex_signing


class StandardAttribute(object):
    pass


class IntegerAttribute(int, StandardAttribute):
    pass


class StringAttribute(str, StandardAttribute):
    pass


class ObjectIdAttribute(StandardAttribute):

    def __init__(self):
        self.name = None
        self.min = None
        self.max = None


class Attributes(object):

    def __init__(self):
        self.sw_id = StringAttribute()
        self.app_id = StringAttribute()
        self.msm_part = StringAttribute()
        self.soc_hw_version = IntegerAttribute()
        self.soc_vers = StringAttribute()
        self.mask_soc_hw_version = StringAttribute()
        self.in_use_soc_hw_version = IntegerAttribute()
        self.oem_id = StringAttribute()
        self.model_id = StringAttribute()
        self.debug = StringAttribute()
        self.hash_algorithm = StringAttribute()
        self.hmac = StandardAttribute()
        self.rsa_padding = StringAttribute()
        self.num_root_certs = IntegerAttribute()
        self.max_num_root_certs = IntegerAttribute()
        self.num_certs_in_certchain = IntegerAttribute()
        self.max_num_certs_in_certchain = IntegerAttribute()
        self.mrc_index = IntegerAttribute()
        self.secboot_version = StringAttribute()
        self.dsa_type = StringAttribute()
        self.client_id = StringAttribute()
        self.lib_id = StringAttribute()
        self.UIE_server_cert_path = StringAttribute()
        self.object_id = ObjectIdAttribute()


class _GeneralProperties(object):
    """
    Defines the rules for general properties
    """
    def validate(self, general_properties, *args):
        assert (isinstance(general_properties, complex_general_properties))
        return True, ""


class _Signing(object):
    """
    Defines the rules for signing default attributes
    """
    def validate(self, signing, *args):
        assert(isinstance(signing, complex_signing))

        retval = True
        error_str = ''

        # signing paths for trust_keystore and keystore_file from cass_signer_attributes
        cass_signer_attr = signing.get_signer_attributes().get_cass_signer_attributes()
        if cass_signer_attr is not None:
            # keystore rule, they must all exists
            if cass_signer_attr.get_server():
                trust_keystore = cass_signer_attr.get_server().get_trust_keystore()
                if c_path.validate_file(trust_keystore) is False:
                    retval = False
                    error_str += '\n trust_keystore is invalid, path = %s' % trust_keystore

            keystore_file = cass_signer_attr.get_user_identity().get_keystore_file()
            if keystore_file and c_path.validate_file(keystore_file) is False:
                retval = False
                error_str += '\n keystore_file is invalid, path = %s' % keystore_file

        return retval, error_str


class _ImageList(object):
    """
    Defines the rules for image parameters to be signed
    """
    def __init__(self, verbatim_config=False, platform_binding=None):
        self.mask_warning = True
        self.version_warning = True
        self.verbatim_config = verbatim_config
        self.platform_binding = platform_binding

    def validate(self, images, *args):

        def add_error(sign_id_, error):
            retval[0] = False
            errors.append("\nsign_id={0}: ".format(sign_id_) + error)

        assert(isinstance(images, complex_images_list))

        retval = [True]
        errors = []

        # expect args[0] to be instance of signing
        # the following default signing attributes are checked if signing is TCG
        assert(isinstance(args[0], complex_general_properties))
        general_properties = args[0]
        defaults = Attributes()
        for gp_name, gp_instance in vars(defaults).items():
            if not isinstance(gp_instance, StandardAttribute):
                continue
            setattr(defaults, gp_name, getattr(general_properties, "get_" + gp_name)())

        resolve_enumerated(defaults, DEBUG_TAG)

        default_oid_min = None if defaults.object_id is None else defaults.object_id.min
        default_oid_max = None if defaults.object_id is None else defaults.object_id.max

        assert(isinstance(args[1], complex_metadata))
        chipset = args[1].get_chipset()

        for image in images.get_image():
            sign_id = image.get_sign_id()
            overrides = image.get_general_properties_overrides()
            attributes = Attributes()

            for gp_name, gp_instance in vars(attributes).items():
                if not isinstance(gp_instance, StandardAttribute):
                    continue
                value = getattr(overrides, "get_" + gp_name)()
                setattr(attributes, gp_name, getattr(defaults, gp_name) if value is None else value)

            resolve_enumerated(attributes, DEBUG_TAG)

            if attributes.object_id is None:
                oid_min = default_oid_min
                oid_max = default_oid_max
            else:
                oid = attributes.object_id
                oid_min = oid.min
                oid_max = oid.max

            # Sanitize secboot_version
            if (attributes.secboot_version is None or
                    image.get_image_type() in SECBOOT_1_IMAGE_TYPES):
                attributes.secboot_version = SECBOOT_VERSION_1_0

            self.sanitize_soc_vers(sign_id, attributes, add_error)

            if (attributes.num_root_certs > 1 and
                    attributes.secboot_version != SECBOOT_VERSION_1_0 and
                    chipset not in (SECBOOT_2_0_CHIPSETS + SECBOOT_3_0_CHIPSETS)):
                # Multiple root certs not allowed with secboot 2.0 and greater multi-party signing
                add_error(sign_id, "Multiple root certs are not supported for Secure Boot {0} chipset {1}."
                          .format(str(attributes.secboot_version), chipset))

            if attributes.dsa_type == "ecdsa":
                # Check that ecdsa value is only used with supported chipset
                if chipset not in ECDSA_CHIPSETS:
                    add_error(sign_id, "ecdsa is not supported for chipset {0}.".format(chipset))
            else:
                # RSAPSS requires SHA256 or SHA384
                if attributes.rsa_padding == "pss":
                    if attributes.hash_algorithm not in ["sha256", "sha384"]:
                        add_error(sign_id, "rsapss requires 'sha256' or 'sha384' hash. hash_algorithm={0}."
                                  .format(attributes.hash_algorithm))
                    if attributes.hmac:
                        add_error(sign_id, "rsapss cannot be used with hmac. hmac must be set as False. hmac={0}."
                                  .format(attributes.hmac))
                else:
                    if not attributes.hmac:
                        add_error(sign_id, "rsapkcs should be used with hmac.")

            # TZ apps rule, must have app_id set
            if is_TA(attributes.sw_id):
                if int(attributes.app_id, 16) == 0:
                    add_error(sign_id, "app_id is not set for TZ apps: sw_id={0}."
                              .format(attributes.sw_id))

            # Ensure MRC targets are properly configured to avoid OU field conflicts
            misconfigured_mrc_chipsets = get_duplicates(MRC_1_0_CHIPSETS + MRC_2_0_CHIPSETS)
            if misconfigured_mrc_chipsets:
                raise RuntimeError("MRC 1.0 and MRC 2.0 chipsets must be mutually exclusive. The following chipsets "
                                   "are configured incorrectly: {0}.".format(", ".join(misconfigured_mrc_chipsets)))

            # MRC 1.0 support up to 4 roots, MRC 2.0 supports 1 or 4 roots
            if chipset in MRC_1_0_CHIPSETS + MRC_2_0_CHIPSETS:
                if attributes.num_root_certs > attributes.max_num_root_certs:
                    add_error(sign_id, "num_root_certs must be less than or equal to max_num_root_certs: "
                              "num_root_certs={0}, max_num_root_certs={1}."
                              .format(attributes.num_root_certs, attributes.max_num_root_certs))
                if attributes.num_root_certs not in [1, 2, 3, 4]:
                    add_error(sign_id, "num_root_certs must be in range [1-4].")
            # Legacy MRC supports up to 16 roots
            elif attributes.num_root_certs < 1 or attributes.num_root_certs > 16:
                add_error(sign_id, "num_root_certs must be in range [1-16].")

            if attributes.mrc_index >= attributes.num_root_certs:
                add_error(sign_id, "Index out of range: mrc_index={0}, num_root_certs={1}."
                          .format(attributes.mrc_index, attributes.num_root_certs))

            # Ensure num_certs_in_certchain does not exceed max_num_certs_in_certchain
            if attributes.num_certs_in_certchain > attributes.max_num_certs_in_certchain:
                add_error(sign_id, "num_certs_in_certchain must be less than or equal to max_num_certs_in_certchain: "
                                   "num_certs_in_certchain={0}, max_num_certs_in_certchain={1}."
                          .format(attributes.num_certs_in_certchain, attributes.max_num_certs_in_certchain))

            # TCG rules
            if oid_min is not None and oid_max is None:
                add_error(sign_id, "{0} min is set, must also set max."
                          .format(defaults.object_id.name))
            elif oid_max is not None and oid_min is None:
                add_error(sign_id, "{0} max is set, must also set min."
                          .format(defaults.object_id.name))
            elif defaults.object_id is not None:
                # Move the min > max checking to signer. It should be validated after valid 32
                # bit integer is checked. Otherwise, int() conversion will throw an exception

                oid_min_config_str = oid_min
                oid_max_config_str = oid_max
                if oid_min_config_str and oid_max_config_str:
                    oid_min = int(oid_min, 16) if oid_min.startswith("0x") else int(oid_min)
                    oid_max = int(oid_max, 16) if oid_max.startswith("0x") else int(oid_max)

                    if oid_min > oid_max:
                        add_error(sign_id, "{0} min must be less than max, min={1} max={2}."
                                  .format(defaults.object_id.name,
                                          oid_min_config_str, oid_max_config_str))
                if int(attributes.sw_id, 16) != 0:
                    add_error(sign_id, "For {0}, sw_id must be 0, sw_id = {1}."
                              .format(defaults.object_id.name, attributes.sw_id))
                if int(attributes.msm_part, 16) != 0:
                    add_error(sign_id, "For {0}, msm_part must be 0, msm_part = {1}."
                              .format(defaults.object_id.name, attributes.msm_part))
                if int(attributes.oem_id, 16) != 0:
                    add_error(sign_id, "For {0}, oem_id must be 0, oem_id = {1}."
                              .format(defaults.object_id.name, attributes.oem_id))
                if int(attributes.model_id, 16) != 0:
                    add_error(sign_id, "For {0}, model_id must be 0, model_id = {1}."
                              .format(defaults.object_id.name, attributes.model_id))
                if int(attributes.debug, 16) != 2:
                    add_error(sign_id, "For {0}, debug must be 2, debug = {1}."
                              .format(defaults.object_id.name, attributes.debug))

            if attributes.client_id and int(attributes.client_id, 16) == 0:
                add_error(sign_id, "client_id must be a non-zero value, client_id={0}."
                          .format(attributes.client_id))
            if attributes.lib_id and int(attributes.lib_id, 16) == 0:
                add_error(sign_id, "lib_id must be a non-zero value, lib_id={0}."
                          .format(attributes.lib_id))

            if (attributes.UIE_server_cert_path and
                    c_path.validate_file(attributes.UIE_server_cert_path) is False):
                add_error(sign_id, "UIE_server_cert_path is invalid, path={0}"
                          .format(attributes.UIE_server_cert_path))

        return retval[0], "".join(sorted(remove_duplicates(errors)))

    def sanitize_soc_vers(self, sign_id, attributes, add_error):

        if attributes.secboot_version == SECBOOT_VERSION_3_0:
            if attributes.hmac:
                add_error(sign_id, "Secure Boot 3.0 does not support hmac.")
            if attributes.mask_soc_hw_version is not None:
                add_error(sign_id, "Secure Boot 3.0 does not support mask_soc_hw_version")
        else:
            if (attributes.mask_soc_hw_version is not None and
                    attributes.in_use_soc_hw_version == 1 and
                    self.mask_warning is True):
                logger.warning(
                    "The mask_soc_hw_version field is set and will mask the soc_hw_version during signing. "
                    "Please ensure this is the desired result.")
                self.mask_warning = False

        if self.verbatim_config:
            if attributes.secboot_version == SECBOOT_VERSION_3_0:
                if not attributes.soc_vers and attributes.in_use_soc_hw_version == 1:
                    add_error(sign_id, "in_use_soc_hw_version is enabled, but no soc_vers is provided.")
            else:
                # Make sure in_use_soc_hw_version's specification exists
                if int(attributes.soc_hw_version[:6], 16) == 0 and attributes.in_use_soc_hw_version == 1:
                    add_error(sign_id, "in_use_soc_hw_version is enabled, but no soc_hw_version is provided")
                # mask_soc_hw_version may only exist if soc_hw_version exists
                if attributes.soc_hw_version[:6] == "0x0000" and attributes.mask_soc_hw_version is not None:
                    add_error(sign_id, "mask_soc_hw_version can not exist without soc_hw_version.")
        else:
            soc_vers_candidates = [attributes.soc_hw_version[:6]]
            if attributes.soc_vers is not None:
                soc_vers_candidates.extend(attributes.soc_vers.split())
            soc_vers_candidates = remove_duplicates_ordered(*soc_vers_candidates)

            if self.platform_binding:
                # Make sure there are soc_ver_candidates when binding to SOC HW Version is requested
                if self.platform_binding == PLATFORM_BINDING_SOC_VERS and not soc_vers_candidates:
                    add_error(sign_id,
                              "platform_binding is set to {0}, "
                              "but soc_hw_version and soc_vers were not provided.".format(PLATFORM_BINDING_SOC_VERS))
            else:
                # Make sure there are soc_ver_candidates when in_use_soc_hw_version is enabled
                if not soc_vers_candidates and attributes.in_use_soc_hw_version == 1:
                    add_error(sign_id, "in_use_soc_hw_version is enabled, but no soc_hw_version or soc_vers is provided.")

                # For secboot versions other than 3.0, mask_soc_hw_version may only exist if soc_hw_version or soc_vers exists
                if (attributes.secboot_version != SECBOOT_VERSION_3_0 and
                        not soc_vers_candidates and attributes.mask_soc_hw_version is not None):
                    add_error(sign_id, "mask_soc_hw_version can not exist without soc_hw_version or soc_vers.")


class ConfigRulesManager(CoreRuleBase):
    """
    This is the main SecImage config rules manager that runs the rules
    """
    def __init__(self, verbatim_config=False, platform_binding=None):
        self.configs = dict()
        self.configs['signing'] = _Signing()
        self.configs['images_list'] = _ImageList(verbatim_config=verbatim_config, platform_binding=platform_binding)
        self.configs['general_properties'] = _GeneralProperties()
        self.configs['metadata'] = None
        self.configs['parsegen'] = None
        self.configs['post_process'] = None

    def validate(self, data, data_dict):
        retval = True
        error_str = ''

        # based on the dictionary structure, go through each section from root
        for name in data_dict:
            # check if there's a registered rule object for this config section
            if name in self.configs and self.configs[name] is not None:
                config = getattr(data.root, name)
                if name == 'general_properties':
                    ret, error = self.configs[name].validate(config, getattr(data.root, 'signing'))
                elif name == 'images_list':
                    ret, error = self.configs[name].validate(config, getattr(data.root, 'general_properties'),
                                                             getattr(data.root, 'metadata'))
                else:
                    ret, error = self.configs[name].validate(config, config)

                # accumulate the return values
                if ret is False:
                    retval &= ret
                    error_str += '\n\n<%s>%s' % (name, error)

        if retval is False:
            raise RuntimeError('\nSecImage config validation failed with following error(s): %s' % error_str)
