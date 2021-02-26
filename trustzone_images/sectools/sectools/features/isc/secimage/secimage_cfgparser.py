# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.advanced_defines import SERIAL_NUMBERS
from sectools.features.isc.cfgparser import CfgParserNamedTuple
from sectools.features.isc.cfgparser import ConfigParser
from sectools.features.isc.cfgparser import auto_gen_obj_config
from sectools.features.isc.cfgparser import auto_gen_xml_config
from sectools.features.isc.cfgparser import defines as cfgparser_defines
from sectools.features.isc.cfgparser.rule import ConfigRulesManager


def secimage_cfgparser(verbatim_config=False, platform_binding=None):
    """
    :param verbatim_config:
        The default is set to False to use the new sanitation.
    :param platform_binding:
        Specifies whether in_use_soc_hw_version value should be
        ignored in place of using platform_binding enumerated value
    :return: named-tuple of cfgparser attributes
    """
    return CfgParserNamedTuple(
        "2.0", ConfigRulesManager(verbatim_config=verbatim_config, platform_binding=platform_binding),
        cfgparser_defines, auto_gen_obj_config, auto_gen_xml_config)


class SecImageConfigParser(ConfigParser):
    """ SecImage-specific ConfigParser

    It sets the default values to ConfigParser constructor arguments.

    The verbatim_config default is set to False to use the new sanitation.
    """
    def __init__(self, config_path, overrides=None, spec_overrides=None,
                 verbatim_config=False, platform_binding=None):
        assert isinstance(config_path, str)

        self.cfgparser = secimage_cfgparser(verbatim_config=verbatim_config, platform_binding=platform_binding)
        self.config_module = self.cfgparser.auto_gen_xml_config
        self.obj_module = self.cfgparser.auto_gen_obj_config
        self.overrides = overrides
        self.spec_overrides = spec_overrides
        super(ConfigParser, self).__init__(self.config_module, config_path)

        if self.overrides is not None:
            command_line_overrides = self.overrides.get_properties(has_value=True)

            # Append serial_numbers to general_properties.multi_serial_numbers.
            self.config_data.general_properties.multi_serial_numbers = \
                self.append_serial_numbers(
                    command_line_overrides.get(SERIAL_NUMBERS),
                    self.config_data.general_properties.multi_serial_numbers,
                    always_append=True)

            # Go through all image configurations to append serial_numbers to
            # general_properties_overrides.multi_serial_number, if applicable.
            for image in self.config_data.images_list.image:
                for tag, override in command_line_overrides.items():
                    if tag == SERIAL_NUMBERS:
                        continue
                    # Override per-image config with command-line inputs.
                    setattr(image.general_properties_overrides, tag, override.value)
                # The general_properties_overrides.multi_serial_numbers should have
                # already been updated, if it is provided on the command-line.
                # Append serial_numbers to it.
                image.general_properties_overrides.multi_serial_numbers = \
                    self.append_serial_numbers(
                        command_line_overrides.get(SERIAL_NUMBERS),
                        image.general_properties_overrides.multi_serial_numbers)

        self.validate()

    def append_serial_numbers(self, serial_numbers, multi_serial_numbers, always_append=False):
        """ Append serial_numbers to multi_serial_numbers.

        :param serial_numbers: str type
               It is a command-line --serial_numbers user input.
        :param multi_serial_numbers: str type
               It could come from three sources:
                 1. general_properties
                 2. per-image general_properties_overrides
                 3. command-line --cfg_multi_serial_numbers user input
               The command-line --cfg_multi_serial_numbers overrides all others.
        :param always_append: bool type
               When True, append serial_numbers even when multi_serial_numbers does not
                 exist. It means a new multi_serial_numbers object is created.
               When False, serial_numbers are appended only when multi_serial_numbers exist.
        :return: The resulting multi_serial_numbers object.
        """
        if serial_numbers is None:
            # No serial_numbers tag. So nothing to append.
            return multi_serial_numbers
        try:
            no_multi_serial_numbers = not multi_serial_numbers.serial
        except AttributeError:
            no_multi_serial_numbers = True
        if no_multi_serial_numbers:
            if always_append:
                # Return a brand new multi_serial_numbers object.
                return auto_gen_xml_config.complex_multi_serial_numbers(
                    serial_numbers.value.split())
            # No appending is needed.
            return multi_serial_numbers
        # Append serial_numbers to the existing multi_serial_numbers.
        multi_serial_numbers.serial.extend(serial_numbers.value.split())
        return multi_serial_numbers
