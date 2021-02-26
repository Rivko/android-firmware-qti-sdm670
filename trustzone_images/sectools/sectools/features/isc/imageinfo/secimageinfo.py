# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from functools import partial

from sectools.common.utils import c_path
from sectools.common.utils.c_base import ValPolicy
from sectools.common.utils.c_data import remove_duplicates_ordered
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import extract_image_id_from_sw_id
from sectools.common.utils.c_misc import is_TA
from sectools.common.utils.datautils.enumerated_types import enumerated_values, ENABLE, ENABLE_BOUND
from sectools.features.isc import advanced_defines
from sectools.features.isc import defines
from sectools.features.isc.advanced_defines import PLATFORM_BINDING_SOC_VERS, PLATFORM_BINDING_JTAG_ID, \
    PLATFORM_BINDING_INDEPENDENT
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.advanced_defines import SERIAL_BOUND_TAG
from sectools.features.isc.cfgparser.auto_gen_xml_config import complex_multi_serial_numbers
from sectools.features.isc.cfgparser.chipset_profiles import ChipsetProfile
from sectools.features.isc.cfgparser.defines import ACTIVATION_ENABLEMENT_TAG
from sectools.features.isc.cfgparser.defines import DEBUG_TAG
from sectools.features.isc.cfgparser.defines import ENUMERATED_TAG_MAPPING
from sectools.features.isc.cfgparser.defines import JTAGID_SIGN_IDS
from sectools.features.isc.cfgparser.defines import MRC_1_0_CHIPSETS
from sectools.features.isc.cfgparser.defines import MRC_2_0_CHIPSETS
from sectools.features.isc.cfgparser.defines import REVOCATION_ENABLEMENT_TAG
from sectools.features.isc.cfgparser.defines import ROOT_REVOKE_ACTIVATE_ENABLE_TAG
from sectools.features.isc.cfgparser.defines import SECBOOT_1_IMAGE_TYPES
from sectools.features.isc.cfgparser.defines import SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS
from sectools.features.isc.cfgparser.defines import SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.encryption_service.unified.encdec import UIE_1_0_L2_1_1_IDENT
from sectools.features.isc.encryption_service.unified.encdec import UIE_2_0_L2_1_2_IDENT
from sectools.features.isc.encryption_service.unified.encdec import UNIFIED_ENCRYPTION_IDENT
from .imageinfo_base import DestImagePathBase
from .imageinfo_base import ImageInfoBase
from .imageinfo_base import ImageStatusBase
from .imageinfo_base import debug_only
from .imageinfo_utils import StatusInfo
from .imageinfo_utils import resolve_enumerated
from .serial_binding_features import MAX_NUM_SERIALS_MAP
from .serial_binding_features import SerialBoundFeatureSetContextManager
from ..cfgparser import auto_gen_obj_config as agoc
from ..cfgparser import defines as cfgdef
from ..parsegen.config import auto_gen_obj_config as pgagoc

POLICY_OEM_ID_0 = ValPolicy(ValPolicy.WARN)
MULTI_IMAGE_FILENAME = advanced_defines.multi_image_string() + " image"
OEM_MULTI_IMAGE_SIGN_ID = "multi_image"
QTI_MULTI_IMAGE_SIGN_ID = "multi_image_qti"
MULTI_IMAGE_SIGN_ID = {
    defines.AUTHORITY_OEM: OEM_MULTI_IMAGE_SIGN_ID,
    advanced_defines.AUTHORITY_QTI: QTI_MULTI_IMAGE_SIGN_ID,
}

MAX_NUM_SOC_VERS_10 = 10
MAX_NUM_SOC_VERS_12 = 12
MAX_NUM_SOC_VERS_MAP = {
    SECBOOT_VERSION_1_0: MAX_NUM_SOC_VERS_10,
    SECBOOT_VERSION_2_0: MAX_NUM_SOC_VERS_10,
    SECBOOT_VERSION_3_0: MAX_NUM_SOC_VERS_12
}


class DestImagePath(DestImagePathBase):

    @property
    def decrypted_file(self):
        file_name = (self.image_name_base +
                     advanced_defines.DEST_FILE_DECRYPTED +
                     self.image_name_ext)
        return c_path.join(self.image_dir, file_name)

    @property
    @debug_only
    def debug_dir_encdec(self):
        return c_path.join(self.debug_dir, self._mid, advanced_defines.DEST_DEBUG_DIR_ENCDEC)

    @property
    @debug_only
    def debug_file_parsegen_encrypted(self):
        file_name = (self.image_name_base +
                     advanced_defines.DEST_DEBUG_FILE_PARSEGEN_ENCRYPTED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_signer_qti_signature(self):
        return c_path.join(self.debug_dir_signer,
                           advanced_defines.DEST_DEBUG_FILE_SIGNER_QTI_SIGNATURE)

    @property
    @debug_only
    def debug_file_signer_qti_cert_chain(self):
        return c_path.join(self.debug_dir_signer,
                           advanced_defines.DEST_DEBUG_FILE_SIGNER_QTI_CERT_CHAIN)


class ImageStatus(ImageStatusBase):

    def __init__(self):
        super(ImageStatus, self).__init__()
        self.encrypt = StatusInfo()
        self.validate_encrypt = StatusInfo()


class ImageInfo(ImageInfoBase):

    image_status_class = ImageStatus

    def __init__(self, image_path, sign_id, img_config_block, img_config_parser,
                 parsegen_config, gen_multi_image=False, authority=None,
                 verbatim_config=False, disable_serial_binding=False, platform_binding=None, **kwargs):
        if not gen_multi_image:
            assert isinstance(image_path, str)

        self.verbatim_config = verbatim_config
        self.disable_serial_binding = disable_serial_binding
        self.validation_root_cert_hash = None
        self.image_signing_attribute_notes = {}
        self.platform_binding = platform_binding

        super(ImageInfo, self).__init__(
            image_path, sign_id, img_config_block, img_config_parser,
            parsegen_config, gen_multi_image=gen_multi_image, authority=authority,
            agoc=agoc, pgagoc=pgagoc, cfgdef=cfgdef,
            run_before_sanitize=partial(self.run_before_sanitize, img_config_block))

        # Set platform_binding based on general properties values if not provided by user
        if not self.platform_binding:
            self.platform_binding = self._determine_platform_binding()
        # Some parsegen classes, i.e. MBN header, do not have access to the full ImageInfo object and only have access
        # to the general_properties despite needing to know the value of platform_binding
        self.general_properties.platform_binding = self.platform_binding

    def run_before_sanitize(self, img_config_block):
        self.dest_image = DestImagePath()
        self.encdec = None

        # Properties obtained from the config file
        self.pil_split = img_config_block.pil_split
        self.post_process_commands = img_config_block.post_process_commands

        self.is_multi_image = self.sign_id in MULTI_IMAGE_SIGN_ID.values()
        self.image_under_operation = (self.src_image.image_path if
                                      self.src_image.image_path else MULTI_IMAGE_FILENAME)

        resolve_enumerated(self.general_properties, DEBUG_TAG)

    @ImageInfoBase.signer_config.setter
    def signer_config(self, value):
        ImageInfoBase.signer_config.fset(self, value)
        self._signer_config.signing = self.config.signing

    #--------------------------------------------------------------------------
    # Helper functions
    #--------------------------------------------------------------------------
    def _determine_platform_binding(self):
        platform_binding = PLATFORM_BINDING_JTAG_ID
        # MBN v3, MBN v5, and MBN v6 Metadata v0.0 only support binding to soc_hw_version/soc_vers or msm_part
        if self.general_properties.in_use_soc_hw_version == 1:
            platform_binding = PLATFORM_BINDING_SOC_VERS
        return platform_binding

    def _sanitize_general_properties(self):

        def _sanitize_mrc_value(value, num_root_certs, secboot_version):
            sanitized_value = value
            if value is not None and secboot_version != SECBOOT_VERSION_3_0:
                lower_8_bytes = int(value, 16) & 0xFFFFFFFF
                # if not MRC use case, to pass validation, enforce value to be zero because attest cert will not have it
                if num_root_certs == 1 or lower_8_bytes == 0:
                    sanitized_value = "0x0000000000000000"
            return sanitized_value

        # Sanitize secboot_version
        if (self.general_properties.secboot_version is None or
                self.image_type.id in SECBOOT_1_IMAGE_TYPES):
            self.general_properties.secboot_version = SECBOOT_VERSION_1_0

        self.sanitize_sw_id_anti_rollback_version()
        self.sanitize_soc_vers()
        self.sanitize_serial_numbers()

        # Apply OEM ID independent and Model ID independent rules for MBN v6
        if self.general_properties.secboot_version == SECBOOT_VERSION_3_0:
            # Apply rules for metadata v1.0 (v1.0 metadata will only be created when
            # platform_binding is set to INDEPENDENT)
            if self.platform_binding == PLATFORM_BINDING_INDEPENDENT:
                if self.general_properties.oem_id_independent:
                    self.general_properties.oem_id = "0x0000"
                # Unconditionally set model_id to 0 when platform_binding is set to INDEPENDENT
                self.general_properties.model_id = "0x0000"
            # Apply rules for MBN v6 metadata v0.0
            else:
                if self.general_properties.oem_id_independent:
                    self.general_properties.oem_id = "0x0001"
                    self.general_properties.model_id = "0x0000"
        # Apply OEM ID independent and Model ID independent rules for MBN v3 and v5
        else:
            if self.general_properties.oem_id_independent:
                self.general_properties.oem_id = "0x0001"
                self.general_properties.model_id = "0x0000"

        # Sanitize revocation_enablement
        self.general_properties.revocation_enablement = _sanitize_mrc_value(
            self.general_properties.revocation_enablement, self.general_properties.num_root_certs,
            self.general_properties.secboot_version)

        # Sanitize activation_enablement
        self.general_properties.activation_enablement = _sanitize_mrc_value(
            self.general_properties.activation_enablement, self.general_properties.num_root_certs,
            self.general_properties.secboot_version)

        # Sanitize root_revoke_activate_enable
        self.general_properties.root_revoke_activate_enable = _sanitize_mrc_value(
            self.general_properties.root_revoke_activate_enable, self.general_properties.num_root_certs,
            self.general_properties.secboot_version)

        for attr_name in (ROOT_REVOKE_ACTIVATE_ENABLE_TAG, ACTIVATION_ENABLEMENT_TAG, REVOCATION_ENABLEMENT_TAG):
            allowed_values = enumerated_values[ENUMERATED_TAG_MAPPING[attr_name]]
            attr_value = int(getattr(self.general_properties, attr_name), 16) & 0x00000000FFFFFFFF
            if (attr_value in (allowed_values[ENABLE], allowed_values[ENABLE_BOUND]) and
                    self.general_properties.num_root_certs == 1):
                raise RuntimeError("{0} should not be enabled when num_root_certs is 1".format(attr_name))

        # Sanitize selected_encryptor
        if (self.general_properties.selected_encryptor == UNIFIED_ENCRYPTION_IDENT and
                extract_image_id_from_sw_id(self.general_properties.sw_id) > 0x1F):
            # Upgrade the encryptor to one that supports larger sw_id values.
            self.general_properties.selected_encryptor = UIE_1_0_L2_1_1_IDENT

        # Sanitize app_id for IP Protector image
        if self.general_properties.selected_encryptor == UIE_2_0_L2_1_2_IDENT:
            # Set app_id to feature_id
            self.general_properties.app_id = self.general_properties.UIE_feature_id

    def sanitize_soc_vers(self):

        def set_soc_hw_version(soc_ver):
            self.general_properties.soc_hw_version = soc_ver + "0000"

        max_soc_vers_count = MAX_NUM_SOC_VERS_MAP[self.general_properties.secboot_version]

        if self.general_properties.secboot_version in (SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0):
            max_soc_vers_count += 1  # soc_hw_version + soc_vers combined

        soc_ver_candidates = [self.general_properties.soc_hw_version[:-4]]
        if self.general_properties.soc_vers is not None:
            soc_ver_candidates.extend(self.general_properties.soc_vers.split())
        soc_ver_candidates = remove_duplicates_ordered(*soc_ver_candidates)

        soc_vers_count = len(soc_ver_candidates)
        if soc_vers_count > max_soc_vers_count:
            verb_be = "is" if max_soc_vers_count == 1 else "are"
            raise RuntimeError("{0} soc_vers were provided. A maximum of {1} {2} allowed.".format(soc_vers_count,
                                                                                                  max_soc_vers_count,
                                                                                                  verb_be))
        # Sanitize soc_vers values based on platform_binding value
        if self.platform_binding:
            if self.platform_binding == PLATFORM_BINDING_INDEPENDENT:
                if self.general_properties.secboot_version != SECBOOT_VERSION_3_0 or \
                        not is_TA(self.general_properties.sw_id):
                    raise RuntimeError("platform_binding {0} is only supported for "
                                       "Secure Boot 3.0 Trusted Application".format(self.platform_binding))
                soc_ver_candidates = []
                self.general_properties.msm_part = "0x00000000"
            elif self.platform_binding == PLATFORM_BINDING_JTAG_ID:
                soc_ver_candidates = []
            elif self.platform_binding == PLATFORM_BINDING_SOC_VERS:
                self.general_properties.in_use_soc_hw_version = 1

        if self.verbatim_config is False:
            if not soc_ver_candidates:
                self.general_properties.in_use_soc_hw_version = 0
                self.general_properties.soc_vers = None
                self.general_properties.soc_hw_version = "0x00000000"
            elif self.general_properties.secboot_version in (SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0):
                if len(soc_ver_candidates) == 1:
                    set_soc_hw_version(soc_ver_candidates[0])
                    self.general_properties.soc_vers = None
                else:
                    self.general_properties.in_use_soc_hw_version = 1
                    set_soc_hw_version(soc_ver_candidates[0])
                    self.general_properties.soc_vers = " ".join(soc_ver_candidates[1:])
            elif self.general_properties.secboot_version == SECBOOT_VERSION_3_0:
                if len(soc_ver_candidates) > 1:
                    self.general_properties.in_use_soc_hw_version = 1
                self.general_properties.soc_vers = " ".join(soc_ver_candidates)

            # If msm_part will be ignored by secboot, wipe the msm_part value to avoid user confusion
            if self.general_properties.in_use_soc_hw_version == 1:
                self.general_properties.msm_part = "0x00000000"

        # Sanitize soc_vers and handle interdependence with in_use_soc_hw_version (for Secboot 1 and 2)
        if self.general_properties.soc_vers is not None:
            self.general_properties.soc_vers = self.general_properties.soc_vers.split()
            if (self.general_properties.secboot_version != SECBOOT_VERSION_3_0 and
                    self.general_properties.in_use_soc_hw_version != 1):
                self.general_properties.soc_vers = None

        # soc_hw_version only uses Family and Device number
        self.general_properties.soc_hw_version = self.general_properties.soc_hw_version[:6] + "0000"
        # Disable in_use_soc_hw_version if chipset needs to be signed with JTAG ID
        if (self.chipset in JTAGID_SIGN_IDS and self.sign_id in JTAGID_SIGN_IDS[self.chipset] and
                self.general_properties.in_use_soc_hw_version == 1):
            raise RuntimeError("For chipset {0}, in_use_soc_hw_version must be disabled".format(self.chipset))

    def sanitize_serial_numbers(self):
        """ Sanitize all general properties related to serial numbers.

        Specifically
            1. Pool all unique non-zero serial numbers.
            2. Assign them to their respective attributes
        """
        serial_numbers = []
        multi_serial_number_count = 0
        if self.general_properties.multi_serial_numbers is not None:
            serial_numbers = remove_duplicates_ordered(
                *self.general_properties.multi_serial_numbers.serial)
            multi_serial_number_count = len(serial_numbers)

        if self.general_properties.secboot_version in (SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0):
            max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_2_0]
        else:
            max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_3_0]

        # This is rudimentary max serial number count check, simply to ensure that
        # the certificate OU[SN] field spec is not violated.
        if len(serial_numbers) > max_serial_number_count:
            verb_be = "is" if max_serial_number_count == 1 else "are"
            raise RuntimeError(
                "{0} serials were provided. A maximum of {1} {2} allowed.".format(
                    len(serial_numbers), max_serial_number_count, verb_be))

        if self.verbatim_config:
            if self.general_properties.use_serial_number_in_signing == 1:
                if self.general_properties.secboot_version in (SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0):
                    if self.general_properties.serial_number is None or int(
                            self.general_properties.serial_number, 16) == 0:
                        raise RuntimeError(
                            "serial_number must be set when use_serial_number_in_signing "
                            "is enabled.".format(self.general_properties.secboot_version))
                elif self.general_properties.secboot_version == SECBOOT_VERSION_3_0:
                    if multi_serial_number_count == 0:
                        raise RuntimeError(
                            "multi_serial_numbers must be set with at least one non-zero "
                            "serial number when use_serial_number_in_signing is enabled.")

            self.valid_serial_numbers = []
        else:
            if self.disable_serial_binding:
                self.valid_serial_numbers = []
            else:
                if self.general_properties.serial_number is not None:
                    serial_numbers.append(self.general_properties.serial_number)

                for attr_name in (cfgdef.DEBUG_TAG,
                                  cfgdef.ROT_EN_TAG,
                                  cfgdef.ACTIVATION_ENABLEMENT_TAG,
                                  cfgdef.REVOCATION_ENABLEMENT_TAG,
                                  cfgdef.ROOT_REVOKE_ACTIVATE_ENABLE_TAG,
                                  cfgdef.UIE_KEY_SWITCH_ENABLE_TAG):
                    # All the signing attributes listed in the for-loop have default values.
                    attr_value = getattr(self.general_properties, attr_name)
                    # Save the serial number bound to the signing attribute.
                    serial_numbers.append(attr_value[:-8])
                    # Remove any serial numbers already bound to the signing attributes, while
                    # coalescing all the serial numbers. All serial-binding signing attributes
                    # will only reflect their state, for example enable or disable. This is
                    # to pave the way for the downstream logic to complete the serial-binding
                    # operations.
                    setattr(self.general_properties, attr_name, ("0x%08x" % 0) + attr_value[-8:])

                self.valid_serial_numbers = remove_duplicates_ordered(*serial_numbers)

            if self.valid_serial_numbers:
                chipset_profile = ChipsetProfile(
                    self.general_properties.secboot_version,
                    self.general_properties.in_use_soc_hw_version,
                    self.general_properties.msm_part,
                    getattr(self.general_properties, "soc_vers", []),
                    self.general_properties.soc_hw_version,
                    self.platform_binding == PLATFORM_BINDING_INDEPENDENT)
                with SerialBoundFeatureSetContextManager(
                        chipset_profile, self.valid_serial_numbers) as feature_set:
                    feature_set.bind(self)
            else:
                self.general_properties.use_serial_number_in_signing = 0
                self.general_properties.serial_number = "0x%08x" % 0
                self.general_properties.multi_serial_numbers = complex_multi_serial_numbers([])

    def sanitize_sw_id_anti_rollback_version(self):
        """ Sanitize sw_id and anti_rollback_version general properties.

        By the time this sanitation logic is called, all the general
        properties have been overridden if applicable.

        general property[sw_id] is a 64-bit hex string, where
            sw_id[63:32] => anti_rollback_version
            sw_id[31:0] => image_id
        general property[anti_rollback_version] is a 32-bit hex string
            This property is for users to provide it separately.

          Sanitation   |  sw_id[63:32]
                       +-------+-------+--------
          Truth Table  |   0   |   A   |   B
        ---------+-----+-------+-------+--------
          anti   |  0  |   0** |   A***|   B***
                 +-----+-------+-------+--------
        rollback |  A  |   A** |   A***| Error
                 +-----+-------+-------+--------
         version |  B  |   B** | Error |   B***

        *   Secboot versions 1.0 and 2.0 use cases.
        **  Secboot version 3.0 use cases.
        *** Newly supported use cases.

        The resulting sw_id and anti_rollback_version are such that:
        For secboot versions 1.0 and 2.0:
            anti_rollback_version is set to None
            sw_id[63:32] <= anti_rollback_version
            sw_id[31:0] <= image_id
        For secboot version 3.0:
            sw_id[63:0] <= image_id
            anti_rollback_version <= sw_id[63:32]
        """
        secboot_version = self.general_properties.secboot_version
        sw_id = int(self.general_properties.sw_id, 16)
        sw_id_anti_rollback = sw_id >> 32

        anti_rollback_version = int(self.general_properties.anti_rollback_version, 16)

        if anti_rollback_version == 0 or sw_id_anti_rollback == anti_rollback_version:
            anti_rollback_version = sw_id_anti_rollback
        elif sw_id_anti_rollback == 0 and anti_rollback_version:
            sw_id = (anti_rollback_version << 32) | sw_id
        else:
            raise RuntimeError(
                "anti-rollback version in anti_rollback_version, %s, and sw_id, %s, mismatch. "
                "Specify version in either anti_rollback_version or sw_id, but not both." % (
                    self.general_properties.anti_rollback_version,
                    self.general_properties.sw_id))

        if secboot_version == SECBOOT_VERSION_3_0:
            self.general_properties.sw_id = "0x%016X" % (sw_id & 0x00000000FFFFFFFF)
            self.general_properties.anti_rollback_version =\
                "0x%08X" % anti_rollback_version
        else:
            self.general_properties.sw_id = "0x%016X" % sw_id
            self.general_properties.anti_rollback_version = None

    def _populate_general_properties(self):
        super(ImageInfo, self)._populate_general_properties()

        resolve_enumerated(self.general_properties, DEBUG_TAG)

        # Private overrides
        if self.config_parser.spec_overrides is not None:
            for tag, override in self.config_parser.spec_overrides.get_properties(has_value=True).items():
                if tag == 'cass_password':
                    self.config.signing.signer_attributes.cass_signer_attributes.user_identity.token_password = override.value

    def _override_value(self, block_from, block_to, key):
        override = getattr(block_from, key, None)
        if override is not None:
            if key == advanced_defines.MULTI_SERIAL_NUMBERS_TAG:
                if not override.serial:
                    # The multi_serial_numbers list is empty, don't override.
                    return
            setattr(block_to, key, override)

    def _add_error(self, error):
        self.errors.append("\nsign_id={0}: ".format(self.sign_id) + error)

    def _validate_properties(self):
        self.errors = []
        sa = self.signing_attributes

        if not POLICY_OEM_ID_0.is_ignore():
            if int(sa.oem_id, 16) == 0:
                POLICY_OEM_ID_0.run('OEM ID is set to 0 for sign_id "' + str(self.sign_id) + '"')

        # Secboot v1 requires oem permissions only
        if sa.secboot_version == SECBOOT_VERSION_1_0:
            if not sa.oem_sign:
                # oem_sign cannot be false for secboot v1
                self._add_error('OEM operations cannot be disabled for a Secure Boot ' +
                                str(SECBOOT_VERSION_1_0) + ' image.')
            if sa.qti_sign:
                # qti_sign cannot be true for secboot v1
                self._add_error('Cannot perform QTI operations on a Secure Boot ' +
                                str(SECBOOT_VERSION_1_0) + ' image.')
        # If all authority permissions are disabled, throw error
        elif not sa.qti_sign and not sa.oem_sign:
            self._add_error('Cannot perform image operations because OEM and QTI operations are disabled for image.')

        # Raise warnings for Secboot v2.1 and greater chipsets
        if self.chipset in SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS + SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS:
            if int(sa.rot_en, 16) == 1:
                logger.warning("rot_en should not be set to \"{0}\" for chipset \"{1}\"."
                               .format(sa.rot_en, self.chipset))
            if sa.hash_algorithm == "sha1":
                logger.warning("hash_algorithm should not be set to \"{0}\" for chipset \"{1}\"."
                               .format(sa.hash_algorithm, self.chipset))
            if sa.exponent == 3:
                logger.warning("exponent should not be set to \"{0}\" for chipset \"{1}\"."
                               .format(sa.exponent, self.chipset))

            if sa.dsa_type == "ecdsa":
                sa.rsa_padding = None
            else:
                if sa.rsa_padding == "pkcs":
                    logger.warning("rsa_padding should not be set to RSAPKCS for chipset \"{0}\"."
                                   .format(self.chipset))

        # Ignore max_num_root_certs value if target is not MRC v2.0 double sign target
        if not (self.chipset in MRC_2_0_CHIPSETS and sa.qti_sign and sa.oem_sign):
            sa.max_num_root_certs = None

        # Ignore max_num_certs_in_certchain if image is not double signable
        if not (sa.qti_sign and sa.oem_sign):
            sa.max_num_certs_in_certchain = None

        if self.is_multi_image and sa.multi_image_segment_addr is None:
            self._add_error(advanced_defines.multi_image_string() +
                            " operation requires multi_image_segment_addr config value.")

        if sa.secboot_version == SECBOOT_VERSION_3_0:
            # Secboot 3 only support sha256 and sha384
            for attr_name in ["hash_algorithm", "segment_hash_algorithm"]:
                if getattr(sa, attr_name) not in ["sha256", "sha384"]:
                    self._add_error("Secure Boot {0} requires 'sha256' or 'sha384' {1}."
                                    .format(sa.secboot_version, attr_name))

        multi_serial_numbers = sa.multi_serial_numbers.serial if sa.multi_serial_numbers is not None else []

        self._validate_serial_bound_value(sa.crash_dump,
                                          "crash_dump",
                                          chipset=self.chipset,
                                          ignore_num_root_certs=True,
                                          allowed_lsb_values=[0, 1])

        self._validate_serial_bound_value(sa.rot_en,
                                          "rot_en",
                                          chipset=self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                          allowed_lsb_values=[0, 1])

        # Validate uie_key_switch_enable
        self._validate_serial_bound_value(sa.uie_key_switch_enable,
                                          "uie_key_switch_enable",
                                          chipset=self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0)

        # MRC 1.0 use case
        # Validate revocation_enablement
        self._validate_serial_bound_value(sa.revocation_enablement,
                                          "revocation_enablement",
                                          chipset=self.chipset,
                                          allowed_chipsets=MRC_1_0_CHIPSETS,
                                          num_root_certs=sa.num_root_certs)
        # Validate activation_enablement
        self._validate_serial_bound_value(sa.activation_enablement,
                                          "activation_enablement",
                                          chipset=self.chipset,
                                          allowed_chipsets=MRC_1_0_CHIPSETS,
                                          num_root_certs=sa.num_root_certs)
        # MRC 2.0 use case
        # Validate root_revoke_activate_enable
        self._validate_serial_bound_value(sa.root_revoke_activate_enable,
                                          "root_revoke_activate_enable",
                                          chipset=self.chipset,
                                          allowed_chipsets=MRC_2_0_CHIPSETS,
                                          num_root_certs=sa.num_root_certs,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0)

        self._validate_serial_bound_value(sa.debug,
                                          "debug",
                                          chipset=self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                          allowed_lsb_values=[0, 2, 3] if sa.secboot_version != SECBOOT_VERSION_3_0 else None,
                                          require_serial_number=(self.verbatim_config or len(self.valid_serial_numbers) == 0))

        if self.errors:
            raise RuntimeError("".join(self.errors))

    def _validate_serial_bound_value(self,
                                     value,
                                     value_name,
                                     chipset=None,
                                     allowed_chipsets=None,
                                     num_root_certs=None,
                                     ignore_num_root_certs=False,
                                     disable_in_value_binding=False,
                                     additional_serials_are_bindable=False,
                                     allowed_lsb_values=None,
                                     require_serial_number=True):
        upper_4_bytes = int(value, 16) & 0xFFFFFFFF00000000
        lower_4_bytes = int(value, 16) & 0x00000000FFFFFFFF

        if allowed_lsb_values is None:
            allowed_lsb_values = [0, 1, 2]
        allowed_lsb_hex_values = ", ".join([hex(x) for x in allowed_lsb_values])

        if (lower_4_bytes == allowed_lsb_values[-1] and
                chipset is not None and allowed_chipsets is not None and
                chipset not in allowed_chipsets):
            self._add_error("{0} is only allowed to be set for chipsets: {1}."
                            .format(value_name, ", ".join(allowed_chipsets)))
        elif ignore_num_root_certs or num_root_certs > 1:
            if disable_in_value_binding:
                if int(value, 16) not in allowed_lsb_values:
                    self._add_error("{0} must be be set to one of the following: {1}. {0} = {2}."
                                    .format(value_name, allowed_lsb_hex_values, value))
                if (upper_4_bytes != 0 or lower_4_bytes == allowed_lsb_values[-1] and
                        not additional_serials_are_bindable):
                    self._add_error("to enable {0}, provide at least one serial number."
                                    .format(value_name))
            else:
                if lower_4_bytes not in allowed_lsb_values:
                    self._add_error("{0} must end with one of the following: {1}. {0} = {2}."
                                    .format(value_name, ", ".join(str(v) for v in allowed_lsb_values), value))
                # If lower 4 bytes == 1, enabling with no serial binding so use as is
                # If lower 4 bytes == 2, enabling with serial binding so check upper 4 bytes are valid serial
                if require_serial_number and upper_4_bytes == 0 and lower_4_bytes == allowed_lsb_values[-1]:
                    self._add_error("to enable {0}, provide at least one serial number."
                                    .format(value_name))

    def get_serial_bound_notes(self):
        return self.image_signing_attribute_notes.setdefault(SERIAL_BOUND_TAG, {})

    def create_serial_bound_notes(self, authority):
        serial_bound_notes = self.get_serial_bound_notes()
        serial_bound_notes[authority] = {}
        return serial_bound_notes[authority]
