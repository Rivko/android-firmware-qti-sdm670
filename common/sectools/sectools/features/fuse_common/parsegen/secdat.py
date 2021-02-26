# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from collections import namedtuple

# Used by other modules
from sectools.common.parsegen.secdat.format import ParseGenSecdat
from sectools.common.parsegen.secdat.segment.type_fuse.defines import \
    FUSEPROV_OPERATION_ETYPE_DICT, FUSEPROV_REGION_ETYPE_DICT
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import load_data_from_file
from sectools.features.fuse_common.datamodel import auto_gen_model
from sectools.features.fuse_common.parsegen import BaseDataParseGen

#------------------------------------------------------------------------------
# Mapping from the config values to the fuseprov values
#------------------------------------------------------------------------------
FUSE_REGION_DICT = \
{
    'QFPROM_RAW_OEM_SEC_BOOT'           : 'OEM_SEC_BOOT',
    'QFPROM_RAW_OEM_PK_HASH'            : 'OEM_PK_HASH',
    'QFPROM_RAW_SEC_KEY_DERIVATION_KEY' : 'SEC_HW_KEY',
    'QFPROM_RAW_OEM_CONFIG'             : 'OEM_CONFIG',
    'QFPROM_RAW_RD_WR_PERM'             : 'READ_WRITE_PERM',
    'QFPROM_RAW_SPARE_REG19'            : 'SPARE_REG19',
    'QFPROM_RAW_GENERAL'                : 'GENERAL',
    'QFPROM_RAW_FEC_EN'                 : 'FEC_EN',
    'QFPROM_RAW_ANTI_ROLLBACK_2'        : 'ANTI_ROLLBACK_2',
    'QFPROM_RAW_ANTI_ROLLBACK_3'        : 'ANTI_ROLLBACK_3',
    'QFPROM_RAW_CUST_KEY1_L1'           : 'GENERAL',
    'QFPROM_RAW_CUST_KEY1_ROT'          : 'GENERAL',
    'QFPROM_RAW_PK_HASH0'               : 'OEM_PK_HASH',
    'QFPROM_RAW_PK_HASH1'               : 'PK_HASH1',
    'QFPROM_RAW_IMAGE_ENCR_KEY1'        : 'IMAGE_ENCR_KEY1',
    'QFPROM_RAW_RD_PERM'                : 'READ_WRITE_PERM',
    'QFPROM_RAW_WR_PERM'                : 'READ_WRITE_PERM',
    'QFPROM_RAW_OEM_IMAGE_ENCR_KEY'     : 'GENERAL',
    'QFPROM_RAW_OEM_PKHASH'             : 'OEM_PK_HASH',
    'QFPROM_RAW_SPARE_REG27'            : 'GENERAL',
    'QFPROM_RAW_OEM_SECURE'             : 'OEM_SECURE',
    'QFPROM_RAW_RD_WR_PERMISSION'       : 'READ_WRITE_PERM',
    'QFPROM_RAW_MRC_2_0'                : 'MRC_2_0', # based on feedback from SecBoot team
    'QFPROM_RAW_SPARE_REG18'            : 'GENERAL',
}

FUSE_OPERATION_DICT = \
{
    'BLOW'          : 'BLOW',
    'VERIFYMASK0'   : 'VERIFYMASK0',
}


class SecdatParseGen(BaseDataParseGen):

    def __init__(self, feature_type, data=None):
        BaseDataParseGen.__init__(self)
        self.feature_type = feature_type
        self.set_data(data)

    def __repr__(self):
        return repr(self._secdat_parsegen)

    def set_data(self, data=None):
        self._secdat_parsegen = ParseGenSecdat(data)
        if not self._secdat_parsegen.has_segment(self.feature_type):
            self._secdat_parsegen.add_segment(self.feature_type, None)
        self.update_data_model()

    def read(self, file_path):
        self.set_data(load_data_from_file(file_path))

    def write(self, file_path, version, fuse_version=None, hdr_info=None):
        if fuse_version is not None:
            self._secdat_parsegen.get_segment_data(self.feature_type).version = fuse_version
        self._secdat_parsegen.write_to_file(file_path, version, hdr_info)

    @property
    def data_model(self):
        return self._data_model

    @data_model.setter
    def data_model(self, dm):
        self._data_model = dm
        self.update_from_data_model()

    def update_data_model(self):
        # Constants for data unavailable in secdat
        PSEUDO_NAME = 'UNKNOWN_NAME'
        PSEUDO_ADDRESS = '0x11223344'
        PSEUDO_LSB_ID = 'LSB'
        PSEUDO_MSB_ID = 'MSB'
        PSEUDO_LSB_BITS = '31:0'
        PSEUDO_MSB_BITS = '63:32'
        PSEUDO_OWNER = auto_gen_model.defines.OWNER_OEM

        # Create a dictionary of fuse regions
        fuses = dict()
        fuse_entry = namedtuple('Fuse', 'address operation value')
        norm = lambda x: auto_gen_model.Cfg_Validations.normalize_value(hex(x), auto_gen_model.BIT_LENGTH_32_BITS)

        for fuse in self._secdat_parsegen.get_segment_data(self.feature_type).fuse_entries.values():
            region = self._get_region_type_str(fuse.region)
            operation = self._get_operation_str(fuse.operation)
            address = norm(fuse.address)
            value = norm((fuse.msb << 32) + fuse.lsb)
            region_entry = fuses.setdefault(region, [])
            region_entry.append(fuse_entry(address, operation, value))

        # Create a data model
        data_model = auto_gen_model.Cfg_Fuseblower()

        # Add the top level module
        security_module = auto_gen_model.Cfg_Module()
        security_module.id = PSEUDO_NAME
        security_module.base_address = PSEUDO_ADDRESS
        data_model.add_module(security_module)

        # Add all the fuses
        for region, region_entry in fuses.items():
            # Create a new fuse region and add to the security module
            fuse_region = auto_gen_model.Cfg_Fuse_Region()
            fuse_region.id = region
            security_module.add_fuse_region(fuse_region)

            for idx, fuse_entry in enumerate(region_entry):
                # Create new fuses per address
                fuse = auto_gen_model.Cfg_Fuse()
                fuse.n = idx
                fuse.address = fuse_entry.address
                fuse.ignore = False
                fuse.operation = fuse_entry.operation
                fuse.value = fuse_entry.value

                # Create the two basic fields for LSB & MSB
                field_LSB = auto_gen_model.Cfg_Field()
                field_LSB.id = PSEUDO_LSB_ID
                field_LSB.bits = PSEUDO_LSB_BITS
                field_LSB.owner = PSEUDO_OWNER

                field_MSB = auto_gen_model.Cfg_Field()
                field_MSB.id = PSEUDO_MSB_ID
                field_MSB.bits = PSEUDO_MSB_BITS
                field_MSB.owner = PSEUDO_OWNER

                fuse.add_field(field_MSB)
                fuse.add_field(field_LSB)

                # Add the fuse to the fuse region
                fuse_region.add_fuse(fuse)

        self.data_model = data_model

    def update_from_data_model(self):
        # Remove existing fuses
        segment = self._secdat_parsegen.get_segment_data(self.feature_type)
        segment.clean()

        # Add fuses from data model
        for module in self.data_model.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    if fuse.ignore:
                        logger.info('Ignoring fuse: ' + str(fuse))
                        continue
                    segment.add_fuse(self._get_region_type_enum(fuse_region.id),
                                     int(fuse.address, 16),
                                     self._get_operation_enum(fuse.operation),
                                     int(fuse.value[10:18], 16),
                                     int(fuse.value[02:10], 16)
                                     )
    @classmethod
    def _get_key_for_value(cls, _dict, value):
        for key, _value in _dict.items():
            if value == _value:
                return key
        raise RuntimeError("Unknown value: " + str(value))

    @classmethod
    def _get_region_type_str(cls, value):
        return cls._get_key_for_value(FUSE_REGION_DICT, FUSEPROV_REGION_ETYPE_DICT[value])

    @classmethod
    def _get_region_type_enum(cls, value):
        return cls._get_key_for_value(FUSEPROV_REGION_ETYPE_DICT, FUSE_REGION_DICT[value])

    @classmethod
    def _get_operation_str(cls, value):
        return cls._get_key_for_value(FUSE_OPERATION_DICT, FUSEPROV_OPERATION_ETYPE_DICT[value])

    @classmethod
    def _get_operation_enum(cls, value):
        return cls._get_key_for_value(FUSEPROV_OPERATION_ETYPE_DICT, FUSE_OPERATION_DICT[value])

    @classmethod
    def get_version(cls, data):
        return ParseGenSecdat(data).hdr.version
