# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from functools import partial

from sectools.common.utils.c_data import remove_duplicates_ordered
from sectools.common.utils.datautils.enumerated_types import ENABLE, ENABLE_BOUND
from sectools.common.utils.datautils.enumerated_types import enumerated_values
from sectools.features.isc.advanced_defines import MULTI_SERIAL_NUMBERS_TAG, SECBOOT_VERSION_3_0
from sectools.features.isc.cfgparser import defines
from sectools.features.isc.cfgparser.auto_gen_xml_config import complex_multi_serial_numbers
from sectools.features.isc.cfgparser.defines import DEBUG_TAG, ENUMERATED_DEBUG_MAPPING, \
    USE_SERIAL_NUMBER_IN_SIGNING_TAG, UIE_KEY_SWITCH_ENABLE_TAG, ENUMERATED_DISABLE_ENABLE_BOUND_MAPPING, \
    ROOT_REVOKE_ACTIVATE_ENABLE_TAG


def debug_single_serial_bound(unconditional=False):
    def wrapper(func):
        def decorated(cls, config):
            if len(config.valid_serial_numbers) == 1:
                debug = int(config.general_properties.debug, 16)
                if unconditional:
                    # Debug enabled unconditionally, bound to one serial number
                    debug = cls.debug_enablement_value
                if debug == cls.debug_enablement_value:
                    debug |= int(config.valid_serial_numbers[0], 16) << 32
                config.general_properties.debug = "0x%016X" % debug
                config.general_properties.multi_serial_numbers = \
                    complex_multi_serial_numbers([])
            func(cls, config)
        return decorated
    return wrapper


def reverse_debug_single_serial_bound(func):
    def decorated(cls, image_signing_attributes, serial_binding_notes):
        if image_signing_attributes.debug:
            debug = int(image_signing_attributes.debug, 16)
            if debug & 0x00000000FFFFFFFF == cls.debug_enablement_value:
                serial_number = image_signing_attributes.debug[:-8]
                serial_binding_notes.setdefault(defines.DEBUG_TAG, []).append(serial_number)
        func(cls, image_signing_attributes, serial_binding_notes)
    return decorated


class DebugUnconditionalSingleSerialBound(object):
    """ Enable debug attribute when a single serial number is provided. """
    @classmethod
    @debug_single_serial_bound(unconditional=True)
    def bind(cls, config):
        super(DebugUnconditionalSingleSerialBound, cls).bind(config)

    @classmethod
    @reverse_debug_single_serial_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(DebugUnconditionalSingleSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class DebugConditionalSingleSerialBound(object):
    """ Bind debug attribute to a single serial number if debug is enabled. """
    @classmethod
    @debug_single_serial_bound(unconditional=False)
    def bind(cls, config):
        super(DebugConditionalSingleSerialBound, cls).bind(config)

    @classmethod
    @reverse_debug_single_serial_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(DebugConditionalSingleSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


def debug_multi_serial_bound(unconditional=False):
    def wrapper(func):
        def decorated(cls, config):
            if len(config.valid_serial_numbers) > 1:
                debug = int(config.general_properties.debug, 16)
                if unconditional:
                    # Debug enabled unconditionally, bound to one or more serial numbers
                    debug = cls.debug_enablement_value
                config.general_properties.debug = "0x%016X" % debug
                if debug == cls.debug_enablement_value:
                    config.general_properties.multi_serial_numbers = \
                        complex_multi_serial_numbers(config.valid_serial_numbers)
            func(cls, config)
        return decorated
    return wrapper


def reverse_debug_multi_serial_bound(func):
    def decorated(cls, image_signing_attributes, serial_binding_notes):
        if image_signing_attributes.debug:
            debug = int(image_signing_attributes.debug, 16)
            if debug & 0x00000000FFFFFFFF == cls.debug_enablement_value:
                if image_signing_attributes.multi_serial_numbers:
                    multi_serial_numbers = image_signing_attributes.multi_serial_numbers
                else:
                    multi_serial_numbers = []
                serial_numbers = remove_duplicates_ordered(*multi_serial_numbers)
                serial_binding_notes.setdefault(defines.DEBUG_TAG, []).extend(serial_numbers)
        func(cls, image_signing_attributes, serial_binding_notes)
    return decorated


class DebugUnconditionalMultiSerialBound(object):
    """ Enable debug attribute if multiple serial numbers are provided. """
    @classmethod
    @debug_multi_serial_bound(unconditional=True)
    def bind(cls, config):
        super(DebugUnconditionalMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_debug_multi_serial_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(DebugUnconditionalMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class DebugConditionalMultiSerialBound(object):
    """ Bind debug attribute to multiple serial numbers if debug is enabled. """
    @classmethod
    @debug_multi_serial_bound(unconditional=False)
    def bind(cls, config):
        super(DebugConditionalMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_debug_multi_serial_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(DebugConditionalMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


def reverse_single_serial_number_bound(func):
    def decorated(cls, image_signing_attributes, serial_binding_notes):
        if (image_signing_attributes.use_serial_number_in_signing and
                image_signing_attributes.hw_id):
            serial_numbers = remove_duplicates_ordered(
                "0x" + image_signing_attributes.hw_id[-8:])
            serial_binding_notes.setdefault(
                defines.USE_SERIAL_NUMBER_IN_SIGNING_TAG, []).extend(serial_numbers)
        func(cls, image_signing_attributes, serial_binding_notes)
    return decorated


class SerialBoundNotSupported(object):

    @classmethod
    def bind(cls, config):
        config.general_properties.use_serial_number_in_signing = 0
        super(SerialBoundNotSupported, cls).bind(config)


class MultiSerialBoundNotSupported(object):

    @classmethod
    def bind(cls, config):
        if len(config.valid_serial_numbers) > 1:
            config.general_properties.use_serial_number_in_signing = 0
        super(MultiSerialBoundNotSupported, cls).bind(config)


class SingleMultiSerialBound(object):
    """ Bind the device to one or more serial numbers. """
    @classmethod
    def bind(cls, config):
        config.general_properties.use_serial_number_in_signing = 0
        config.general_properties.multi_serial_numbers = \
            complex_multi_serial_numbers(config.valid_serial_numbers)
        super(SingleMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_single_serial_number_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        if image_signing_attributes.multi_serial_numbers:
            serial_numbers = remove_duplicates_ordered(
                *image_signing_attributes.multi_serial_numbers)
            serial_binding_notes.setdefault(MULTI_SERIAL_NUMBERS_TAG, []).extend(serial_numbers)
        super(SingleMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class SingleSerialBound(object):
    """ Bind the device to a single serial number. """
    @classmethod
    def bind(cls, config):
        if len(config.valid_serial_numbers) == 1:
            config.general_properties.use_serial_number_in_signing = 1
            config.general_properties.serial_number = config.valid_serial_numbers[0]
            config.general_properties.multi_serial_numbers = \
                complex_multi_serial_numbers([])
        super(SingleSerialBound, cls).bind(config)

    @classmethod
    @reverse_single_serial_number_bound
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(SingleSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


def single_serial_number_binding(binding_value, attr_name):
    def wrapper(func):
        def decorated(cls, config):
            attr_value = int(getattr(config.general_properties, attr_name), 16)
            if attr_value == enumerated_values[defines.ENUMERATED_TAG_MAPPING[attr_name]][binding_value]:
                if len(config.valid_serial_numbers) > 1:
                    raise RuntimeError("%s cannot bind to multiple serial numbers for chipset %s." %
                                       (attr_name, config.chipset))
                # Bind one and only serial number to the signing attribute.
                attr_value |= int(config.valid_serial_numbers[0], 16) << 32
            setattr(config.general_properties, attr_name, "0x%016x" % attr_value)
            func(cls, config)
        return decorated
    return wrapper


def serial_binding_not_supported(binding_value, attr_name):
    def wrapper(func):
        def decorated(cls, config):
            attr_value = int(getattr(config.general_properties, attr_name), 16)
            if attr_value == enumerated_values[defines.ENUMERATED_TAG_MAPPING[attr_name]][binding_value]:
                raise RuntimeError("%s serial binding is not supported for chipset %s." %
                                   (attr_name, config.chipset))
            func(cls, config)
        return decorated
    return wrapper


def reverse_single_serial_number_binding(binding_value, attr_name):
    def wrapper(func):
        def decorated(cls, image_signing_attributes, serial_binding_notes):
            enablement_value = enumerated_values[
                defines.ENUMERATED_TAG_MAPPING[attr_name]][binding_value]
            attr_value = getattr(image_signing_attributes, attr_name)
            if attr_value:
                if (int(attr_value, 16) & 0x00000000FFFFFFFF) == enablement_value:
                    serial_numbers = remove_duplicates_ordered(attr_value[:-8])
                    serial_binding_notes.setdefault(attr_name, []).extend(serial_numbers)
            func(cls, image_signing_attributes, serial_binding_notes)
        return decorated
    return wrapper


enable_bound_hex64_single_serial_number = partial(single_serial_number_binding, ENABLE_BOUND)
enable_bound_hex64_not_supported = partial(serial_binding_not_supported, ENABLE_BOUND)
reverse_enable_bound_hex64_single_serial_number = partial(
    reverse_single_serial_number_binding, ENABLE_BOUND)


class ActivationEnablementSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.ACTIVATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(ActivationEnablementSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_bound_hex64_single_serial_number(defines.ACTIVATION_ENABLEMENT_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(ActivationEnablementSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class ActivationEnablementNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.ACTIVATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(ActivationEnablementNotSupported, cls).bind(config)


class RevocationEnablementSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.REVOCATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(RevocationEnablementSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_bound_hex64_single_serial_number(defines.REVOCATION_ENABLEMENT_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(RevocationEnablementSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class RevocationEnablementNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.REVOCATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(RevocationEnablementNotSupported, cls).bind(config)


class RootRevokeActivateEnableSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.ROOT_REVOKE_ACTIVATE_ENABLE_TAG)
    def bind(cls, config):
        super(RootRevokeActivateEnableSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_bound_hex64_single_serial_number(defines.ROOT_REVOKE_ACTIVATE_ENABLE_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(RootRevokeActivateEnableSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class RootRevokeActivateEnableNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.ROOT_REVOKE_ACTIVATE_ENABLE_TAG)
    def bind(cls, config):
        super(RootRevokeActivateEnableNotSupported, cls).bind(config)


class UIEKeySwitchEnableSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.UIE_KEY_SWITCH_ENABLE_TAG)
    def bind(cls, config):
        super(UIEKeySwitchEnableSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_bound_hex64_single_serial_number(defines.UIE_KEY_SWITCH_ENABLE_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(UIEKeySwitchEnableSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class UIEKeySwitchEnableNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.UIE_KEY_SWITCH_ENABLE_TAG)
    def bind(cls, config):
        super(UIEKeySwitchEnableNotSupported, cls).bind(config)


enable_hex64_single_serial_bound = partial(single_serial_number_binding, ENABLE)
enable_hex64_not_supported = partial(serial_binding_not_supported, ENABLE)
reverse_enable_hex64_single_serial_number = partial(
    reverse_single_serial_number_binding, ENABLE)


class RotEnSingleNoMultiSerialBound(object):

    @classmethod
    @enable_hex64_single_serial_bound(defines.ROT_EN_TAG)
    def bind(cls, config):
        super(RotEnSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_hex64_single_serial_number(defines.ROT_EN_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(RotEnSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class RotEnNotSupported(object):

    @classmethod
    @enable_hex64_not_supported(defines.ROT_EN_TAG)
    def bind(cls, config):
        super(RotEnNotSupported, cls).bind(config)


class CrashDumpSingleNoMultiSerialBound(object):

    @classmethod
    @enable_hex64_single_serial_bound(defines.CRASH_DUMP_TAG)
    def bind(cls, config):
        super(CrashDumpSingleNoMultiSerialBound, cls).bind(config)

    @classmethod
    @reverse_enable_hex64_single_serial_number(defines.CRASH_DUMP_TAG)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(CrashDumpSingleNoMultiSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class CrashDumpNotSupported(object):

    @classmethod
    @enable_hex64_not_supported(defines.CRASH_DUMP_TAG)
    def bind(cls, config):
        super(CrashDumpNotSupported, cls).bind(config)


def reverse_metadata_serial_binding(attr_name, enablement_value):
    def wrapper(func):
        def decorated(cls, image_signing_attributes, serial_binding_notes):
            attr_value = getattr(image_signing_attributes, attr_name)
            if attr_value is not None:
                try:
                    attr_value = int(attr_value, 16)
                except TypeError:
                    # Attribute value is already an integer.
                    pass
                if attr_value == enablement_value:
                    serial_binding_notes.setdefault(attr_name, []).extend(cls.serial_numbers)
            func(cls, image_signing_attributes, serial_binding_notes)
        return decorated
    return wrapper


class MetaDataDebug(object):

    @classmethod
    @reverse_metadata_serial_binding(
        DEBUG_TAG, ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_3_0])
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(MetaDataDebug, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class MetaDataSerialBound(object):

    @classmethod
    @reverse_metadata_serial_binding(USE_SERIAL_NUMBER_IN_SIGNING_TAG, 1)
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(MetaDataSerialBound, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class MetaDataUIEKeySwtichEnable(object):

    @classmethod
    @reverse_metadata_serial_binding(
        UIE_KEY_SWITCH_ENABLE_TAG, ENUMERATED_DISABLE_ENABLE_BOUND_MAPPING[ENABLE_BOUND])
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(MetaDataUIEKeySwtichEnable, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)


class MetaDataRootRevokeActivateEnable(object):

    @classmethod
    @reverse_metadata_serial_binding(
        ROOT_REVOKE_ACTIVATE_ENABLE_TAG, ENUMERATED_DISABLE_ENABLE_BOUND_MAPPING[ENABLE_BOUND])
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        super(MetaDataRootRevokeActivateEnable, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)
