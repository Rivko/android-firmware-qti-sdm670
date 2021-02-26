# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import struct

from sectools.common.utils.c_data import get_bits_from_bytes, convert_int_to_bits
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import obj_repr
from sectools.common.utils.struct_base import StructBase
from sectools.features.isc.defines import AUTHORITY_OEM, AUTHORITY_QTI


MBN_HDR_VERSION_3 = 3
MBN_HDR_VERSION_5 = 5
MBN_HDR_VERSION_6 = 6
METADATA_MAJOR_VERSION_0 = 0
METADATA_MAJOR_VERSION_QTI_0 = 0
METADATA_MINOR_VERSION_0 = 0
METADATA_MINOR_VERSION_QTI_0 = 0


class MbnHdr40B(StructBase):

    def _unpack_data_list(self, unpacked):
        self.image_id = unpacked[0]
        self.flash_parti_ver = unpacked[1]
        self.image_src = unpacked[2]
        self.image_dest_ptr = unpacked[3]
        self.image_size = unpacked[4]
        self.code_size = unpacked[5]
        self.sig_ptr = unpacked[6]
        self.sig_size = unpacked[7]
        self.cert_chain_ptr = unpacked[8]
        self.cert_chain_size = unpacked[9]

    def _pack_data_list(self):
        return [self.image_id,
                self.flash_parti_ver,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size]

    @classmethod
    def get_format(cls):
        return 'I'*10

    @staticmethod
    def supports_qti_signing():
        return False

    @staticmethod
    def supports_metadata():
        return False

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_3

    def get_version_in_bin(self):
        return self.flash_parti_ver

    def set_version_in_bin(self, val):
        self.flash_parti_ver = val

    def get_original_size(self):
        return self.get_size()

    def validate(self):
        pass


class MbnHdr80B(MbnHdr40B):

    def _unpack_data_list(self, unpacked):
        self.codeword = unpacked[0]
        self.magic = unpacked[1]
        self.image_id = unpacked[2]
        self.reserved_1 = unpacked[3]
        self.reserved_2 = unpacked[4]
        self.image_src = unpacked[5]
        self.image_dest_ptr = unpacked[6]
        self.image_size = unpacked[7]
        self.code_size = unpacked[8]
        self.sig_ptr = unpacked[9]
        self.sig_size = unpacked[10]
        self.cert_chain_ptr = unpacked[11]
        self.cert_chain_size = unpacked[12]
        self.reserved_3 = unpacked[13]
        self.reserved_4 = unpacked[14]
        self.reserved_5 = unpacked[15]
        self.reserved_6 = unpacked[16]
        self.reserved_7 = unpacked[17]
        self.reserved_8 = unpacked[18]
        self.reserved_9 = unpacked[19]

    def _pack_data_list(self):
        return [self.codeword,
                self.magic,
                self.image_id,
                self.reserved_1,
                self.reserved_2,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size,
                self.reserved_3,
                self.reserved_4,
                self.reserved_5,
                self.reserved_6,
                self.reserved_7,
                self.reserved_8,
                self.reserved_9]

    @classmethod
    def get_format(cls):
        return 'I'*20

    @staticmethod
    def supports_qti_signing():
        return False

    @staticmethod
    def supports_metadata():
        return False

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_3

    def get_version_in_bin(self):
        return None

    def set_version_in_bin(self, val):
        pass

    def validate(self):
        pass


class MbnHdr40BV5(MbnHdr40B):

    def _unpack_data_list(self, unpacked):
        self.image_id = unpacked[0]
        self.version = unpacked[1]
        self.sig_size_qti = unpacked[2]
        self.cert_chain_size_qti = unpacked[3]
        self.image_size = unpacked[4]
        self.code_size = unpacked[5]
        self.sig_ptr = unpacked[6]
        self.sig_size = unpacked[7]
        self.cert_chain_ptr = unpacked[8]
        self.cert_chain_size = unpacked[9]

    def _pack_data_list(self):
        pack_order = [self.image_id,
                      self.version,
                      self.sig_size_qti,
                      self.cert_chain_size_qti,
                      self.image_size,
                      self.code_size,
                      self.sig_ptr,
                      self.sig_size,
                      self.cert_chain_ptr,
                      self.cert_chain_size
                      ]
        return pack_order

    @staticmethod
    def supports_qti_signing():
        return True

    @staticmethod
    def supports_metadata():
        return False

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_5

    def get_version_in_bin(self):
        return self.version

    def set_version_in_bin(self, val):
        self.version = val

    def validate(self):
        if self.sig_size and self.get_version_in_bin() != self.get_version():
            raise RuntimeError('MBN header version (%s) does not match expected version (%s).' %
                               (self.get_version_in_bin(), self.get_version()))


class MbnHdrV6(object):
    """ MbnHdrV6 does not derive from StructBase because its format is not static like the other MBN headers """

    # List of matadata sizes and name
    METADATA_0_0_QTI = [("I", "sw_id"),
                        ("I", "hw_id"),
                        ("I", "oem_id"),
                        ("I", "model_id"),
                        ("I", "app_id"),
                        ("I", "flags"),
                        ("I"*12, "soc_vers"),
                        ("I"*8, "multi_serial_numbers"),
                        ("I", "mrc_index"),
                        ("I", "anti_rollback_version")]
    # For metadata versions 0.0 the fields are the same
    METADATA_0_0 = METADATA_0_0_QTI

    # Lists containing position, size, and name of each metadata value
    FLAGS_O_O_QTI = [(0, 1, "rot_en"),
                     (1, 1, "in_use_soc_hw_version"),
                     (2, 1, "use_serial_number_in_signing"),
                     (3, 1, "oem_id_independent"),
                     (4, 2, "root_revoke_activate_enable"),
                     (6, 2, "uie_key_switch_enable"),
                     (8, 2, "debug")]
    # For metadata versions 0.0 the flags are the same
    FLAGS_O_O = FLAGS_O_O_QTI

    QTI_VERSION_TO_METADATA_MAP = {(METADATA_MAJOR_VERSION_QTI_0, METADATA_MINOR_VERSION_QTI_0): METADATA_0_0_QTI}
    VERSION_TO_METADATA_MAP = {(METADATA_MAJOR_VERSION_0, METADATA_MINOR_VERSION_0): METADATA_0_0}
    QTI_VERSION_TO_FLAGS_MAP = {(METADATA_MAJOR_VERSION_QTI_0, METADATA_MINOR_VERSION_QTI_0): FLAGS_O_O_QTI}
    VERSION_TO_FLAGS_MAP = {(METADATA_MAJOR_VERSION_0, METADATA_MINOR_VERSION_0): FLAGS_O_O}

    def __init__(self, data=None):
        data = ('\0' * MbnHdrV6._get_fixed_size()) if data is None else data
        self._unpack(data)
        self.validate()

    def _unpack(self, data):
        offset = 0
        end = MbnHdrV6._get_fixed_size()
        self.set_metadata_version_qti_in_bin(self.get_metadata_version_qti())
        self.set_metadata_version_in_bin(self.get_metadata_version())
        (self.image_id,
         self.version,
         self.sig_size_qti,
         self.cert_chain_size_qti,
         self.image_size,
         self.code_size,
         self.sig_ptr,
         self.sig_size,
         self.cert_chain_ptr,
         self.cert_chain_size,
         self.metadata_size_qti,
         self.metadata_size,) = struct.unpack(self._get_fixed_format(), data[offset:end])

        self.contains_zero_filled_oem_metadata = False
        self.contains_zero_filled_qti_metadata = False

        # Determine original size of header so rest of hash segment can be properly extracted
        if self.version == MbnHdr40B.get_version():
            # interpret MBN v3 as MBN v6
            self.cert_chain_size_qti = 0
            self.sig_size_qti = 0
            self.metadata_size_qti = 0
            self.metadata_size = 0
            self.original_size = MbnHdr40B.get_size()
        elif self.version == MbnHdr40BV5.get_version():
            # interpret MBN v5 as MBN v6
            self.metadata_size_qti = 0
            self.metadata_size = 0
            self.original_size = MbnHdr40BV5.get_size()
        else:
            if self.metadata_size_qti > 0:
                # Unpack qti metadata version and metadata values
                end = self._unpack_metadata(data, end, authority=AUTHORITY_QTI)
            if self.metadata_size > 0:
                # Unpack oem metadata version and metadata values
                self._unpack_metadata(data, end, authority=AUTHORITY_OEM)
            self.original_size = self.get_size()
        self.version = self.get_version()

    def _unpack_metadata(self, data, end, authority=AUTHORITY_QTI):
        # Unpack metadata version
        offset = end
        end = offset + struct.Struct(self._get_metadata_version_format()).size
        metadata_major_version, metadata_minor_version, = struct.unpack(self._get_metadata_version_format(), data[offset:end])
        if authority == AUTHORITY_QTI:
            # Check if qti metadata is packed with zeros (zero filled when generating OEM signed double-signable image and when creating OEM Multi-Image hash entry)
            metadata_start = offset
            metadata_end = metadata_start + self.metadata_size_qti
            if self.get_zero_filled_qti_metadata() == data[metadata_start:metadata_end]:
                self.contains_zero_filled_qti_metadata = True
                return metadata_end
            else:
                self.metadata_qti = {}
                metadata = self.metadata_qti
                self.metadata_major_version_qti = metadata_major_version
                self.metadata_minor_version_qti = metadata_minor_version
                if (metadata_major_version, metadata_minor_version) in MbnHdrV6.QTI_VERSION_TO_METADATA_MAP.keys():
                    metadata_fields = MbnHdrV6.QTI_VERSION_TO_METADATA_MAP[(metadata_major_version, metadata_minor_version)]
                    flag_fields = MbnHdrV6.QTI_VERSION_TO_FLAGS_MAP[(metadata_major_version, metadata_minor_version)]
                else:
                    raise RuntimeError("Unpacked MBN header contains invalid QTI metadata version {0}.{1}".format(metadata_major_version, metadata_minor_version))
        else:
            # Check if oem metadata is packed with zeros (zero filled when generating QTI signed double-signable image and when creating QTI Multi-Image hash entry)
            metadata_start = offset
            metadata_end = metadata_start + self.metadata_size
            if self.get_zero_filled_oem_metadata() == data[metadata_start:metadata_end]:
                self.contains_zero_filled_oem_metadata = True
                return metadata_end
            else:
                self.metadata = {}
                metadata = self.metadata
                self.metadata_major_version = metadata_major_version
                self.metadata_minor_version = metadata_minor_version
                if (metadata_major_version, metadata_minor_version) in MbnHdrV6.VERSION_TO_METADATA_MAP.keys():
                    metadata_fields = MbnHdrV6.VERSION_TO_METADATA_MAP[(metadata_major_version, metadata_minor_version)]
                    flag_fields = MbnHdrV6.VERSION_TO_FLAGS_MAP[(metadata_major_version, metadata_minor_version)]
                else:
                    raise RuntimeError("Unpacked MBN header contains invalid OEM metadata version {0}.{1}".format(metadata_major_version, metadata_minor_version))
        # Unpack metadata values
        for field in metadata_fields:
            field_format = field[0]
            field_name = field[1]
            offset = end
            end += struct.Struct(field_format).size
            # Handle multi-value metadata values like soc_vers and serial_nums
            if len(field_format) > 1:
                value = list(struct.unpack(field_format, data[offset:end]))
            # Handle single-value metadata values
            else:
                value, = struct.unpack(field_format, data[offset:end])

            if field_name == "flags":
                # Unpack metadata flag values
                MbnHdrV6._unpack_metadata_flags(value, flag_fields, metadata)
            else:
                metadata[field_name] = value
        return end

    @staticmethod
    def _unpack_metadata_flags(flags, flag_fields, metadata):
        for flag in flag_fields:
            field_position = flag[0]
            field_size_in_bits = flag[1]
            field_name = flag[2]
            value = get_bits_from_bytes(flags, field_position, field_size_in_bits)
            metadata[field_name] = value

    def pack(self):
        data = struct.pack(self._get_fixed_format(),
                           self.image_id,
                           self.version,
                           self.sig_size_qti,
                           self.cert_chain_size_qti,
                           self.image_size,
                           self.code_size,
                           self.sig_ptr,
                           self.sig_size,
                           self.cert_chain_ptr,
                           self.cert_chain_size,
                           self.metadata_size_qti,
                           self.metadata_size)
        if self.metadata_size_qti > 0:
            if self.contains_zero_filled_qti_metadata:
                # Pack qti metadata with zeros (zero filled when generating OEM-signed double-signable image and when creating OEM Multi-Image hash entry)
                data += self.get_zero_filled_qti_metadata()
            else:
                # Pack QTI metadata version
                if (self.metadata_major_version_qti, self.metadata_minor_version_qti) in MbnHdrV6.QTI_VERSION_TO_METADATA_MAP.keys():
                    data += struct.pack(self._get_metadata_version_format(), self.metadata_major_version_qti, self.metadata_minor_version_qti)
                    metadata_fields = MbnHdrV6.QTI_VERSION_TO_METADATA_MAP[(self.metadata_major_version_qti, self.metadata_minor_version_qti)]
                    flag_fields = MbnHdrV6.QTI_VERSION_TO_FLAGS_MAP[(self.metadata_major_version_qti, self.metadata_minor_version_qti)]
                else:
                    raise RuntimeError("MBN header to pack contains invalid QTI metadata version {0}.{1}".format(self.metadata_major_version_qti, self.metadata_minor_version_qti))
                # Pack QTI metadata
                data = self._pack_metadata(data, self.metadata_qti, metadata_fields, flag_fields, AUTHORITY_QTI, self.metadata_major_version_qti, self.metadata_minor_version_qti)
        if self.metadata_size > 0:
            # Pack OEM metadata
            if self.contains_zero_filled_oem_metadata:
                # Pack oem metadata with zeros (zero filled when generating QTI-signed double-signable image and when creating QTI Multi-Image hash entry)
                data += self.get_zero_filled_oem_metadata()
            else:
                # Pack OEM metadata version
                if (self.metadata_major_version, self.metadata_minor_version) in MbnHdrV6.VERSION_TO_METADATA_MAP.keys():
                    data += struct.pack(self._get_metadata_version_format(), self.metadata_major_version, self.metadata_minor_version)
                    metadata_fields = MbnHdrV6.VERSION_TO_METADATA_MAP[(self.metadata_major_version, self.metadata_minor_version)]
                    flag_fields = MbnHdrV6.VERSION_TO_FLAGS_MAP[(self.metadata_major_version, self.metadata_minor_version)]
                else:
                    raise RuntimeError("MBN header to pack contains invalid OEM metadata version {0}.{1}".format(self.metadata_major_version, self.metadata_minor_version))
                data = self._pack_metadata(data, self.metadata, metadata_fields, flag_fields, AUTHORITY_OEM, self.metadata_major_version, self.metadata_minor_version)
        return data

    def _pack_metadata(self, data, metadata, metadata_fields, flag_fields, authority, major_version, minor_version):
        for field in metadata_fields:
            field_format = field[0]
            field_name = field[1]
            field_count = len(field_format)
            if field_name != "flags":
                # Pack non-flag metadata values
                value = metadata[field_name]
                value_count = 1
                try:
                    if isinstance(value, list):
                        value_count = len(value)
                        data += struct.pack(field_format[:value_count], *value)
                    else:
                        data += struct.pack(field_format, value)
                except Exception as e:
                    raise RuntimeError("Failed to pack {0} value into MBN v6 {1} metadata version {2}.{3}".format(field_name, authority, major_version, minor_version) +
                                       "\nValue: {0}".format(value) +
                                       "\nERROR: " + e.message)
                # Pad remaining space for multi-value metadata value such as soc_vers and multi_serial_numbers
                for i in range(field_count - value_count):
                    data += struct.pack(field_format[value_count+i], 0)
            else:
                # Pack flags
                data += MbnHdrV6._pack_metadata_flags(metadata, flag_fields)
        return data

    @staticmethod
    def _pack_metadata_flags(metadata, flag_fields):
        # Initialize all flags to zero
        flags = 0
        for flag in flag_fields:
            field_position = flag[0]
            field_size_in_bits = flag[1]
            field_name = flag[2]
            byte_value = convert_int_to_bits(metadata[field_name], field_position, field_size_in_bits)
            flags |= byte_value
        return struct.pack('I', flags)

    @staticmethod
    def supports_qti_signing():
        return True

    @staticmethod
    def supports_metadata():
        return True

    @classmethod
    def _get_metadata_version_format(cls):
        return 'II'

    @classmethod
    def _get_fixed_format(cls):
        return MbnHdr40BV5.get_format() + 'II'

    @classmethod
    def _get_fixed_size(cls):
        return struct.Struct(cls._get_fixed_format()).size

    def get_size(self):
        size = self._get_fixed_size()
        size += self.metadata_size_qti
        size += self.metadata_size
        return size

    def get_original_size(self):
        return self.original_size

    def _get_metadata_size(self, authority=AUTHORITY_QTI):
        if authority == AUTHORITY_QTI:
            metadata_fields = MbnHdrV6.QTI_VERSION_TO_METADATA_MAP[(self.metadata_major_version_qti, self.metadata_minor_version_qti)]
        else:
            metadata_fields = MbnHdrV6.VERSION_TO_METADATA_MAP[(self.metadata_major_version, self.metadata_minor_version)]
        metadata_format = "".join(metadata[0] for metadata in metadata_fields)
        return struct.Struct(self._get_metadata_version_format() + metadata_format).size

    def get_version_in_bin(self):
        return self.version

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_6

    def get_metadata_version_in_bin(self):
        return self.metadata_major_version, self.metadata_minor_version

    def get_metadata_version_qti_in_bin(self):
        return self.metadata_major_version_qti, self.metadata_minor_version_qti

    @staticmethod
    def get_metadata_version():
        return METADATA_MAJOR_VERSION_0, METADATA_MINOR_VERSION_0

    @staticmethod
    def get_metadata_version_qti():
        return METADATA_MAJOR_VERSION_QTI_0, METADATA_MINOR_VERSION_QTI_0

    def set_metadata_version_in_bin(self, version):
        if version in MbnHdrV6.VERSION_TO_FLAGS_MAP.keys():
            self.metadata_major_version = version[0]
            self.metadata_minor_version = version[1]
        else:
            raise RuntimeError("Cannot set OEM metadata version to {0}.{1}.\nSupported versions are:".format(version[0], version[1], MbnHdrV6.VERSION_TO_FLAGS_MAP.keys()))

    def set_metadata_version_qti_in_bin(self, version):
        if version in MbnHdrV6.QTI_VERSION_TO_FLAGS_MAP.keys():
            self.metadata_major_version_qti = version[0]
            self.metadata_minor_version_qti = version[1]
        else:
            raise RuntimeError("Cannot set QTI metadata version to {0}.{1}.\nSupported versions are:".format(version[0], version[1], MbnHdrV6.QTI_VERSION_TO_FLAGS_MAP.keys()))

    def set_metadata(self, imageinfo, version=None):
        if version is None:
            version = self.get_metadata_version()
        self.set_metadata_version_in_bin(version)
        self.metadata_size = self._get_metadata_size(AUTHORITY_OEM)
        if imageinfo is not None:
            self.metadata = {}
            self.contains_zero_filled_oem_metadata = False
            metadata_fields = MbnHdrV6.VERSION_TO_METADATA_MAP[(self.metadata_major_version, self.metadata_minor_version)]
            flag_fields = MbnHdrV6.VERSION_TO_FLAGS_MAP[(self.metadata_major_version, self.metadata_minor_version)]
            MbnHdrV6._set_metadata(self.metadata, imageinfo, metadata_fields, flag_fields, self.metadata_major_version, self.metadata_minor_version, AUTHORITY_OEM)
        else:
            self.contains_zero_filled_oem_metadata = True

    def set_metadata_qti(self, imageinfo, version=None):
        if version is None:
            version = self.get_metadata_version_qti()
        self.set_metadata_version_qti_in_bin(version)
        self.metadata_size_qti = self._get_metadata_size(AUTHORITY_QTI)
        if imageinfo is not None:
            self.metadata_qti = {}
            self.contains_zero_filled_qti_metadata = False
            metadata_fields = MbnHdrV6.QTI_VERSION_TO_METADATA_MAP[(self.metadata_major_version_qti, self.metadata_minor_version_qti)]
            flag_fields = MbnHdrV6.QTI_VERSION_TO_FLAGS_MAP[(self.metadata_major_version_qti, self.metadata_minor_version_qti)]
            MbnHdrV6._set_metadata(self.metadata_qti, imageinfo, metadata_fields, flag_fields, self.metadata_major_version_qti, self.metadata_minor_version_qti, AUTHORITY_QTI)
        else:
            self.contains_zero_filled_qti_metadata = True

    @staticmethod
    def _set_metadata(metadata, imageinfo, metadata_fields, flag_fields, major_version, minor_version, authority):
        # Set metadata dictionary values
        for field in metadata_fields:
            field_name = field[1]
            if field_name == "hw_id":
                if hasattr(imageinfo, field_name):
                    metadata[field_name] = getattr(imageinfo, field_name)
                else:
                    metadata[field_name] = getattr(imageinfo, "msm_part")
            elif field_name != "flags":
                if hasattr(imageinfo, field_name):
                    metadata[field_name] = getattr(imageinfo, field_name)
                else:
                    raise RuntimeError("Config does not contain {0} value which is required by MBN v6 {1} metadata version {2}.{3}".format(field_name, authority, major_version, minor_version))
        # Set metadata dictionary flag values
        for field in flag_fields:
            field_name = field[2]
            if hasattr(imageinfo, field_name):
                metadata[field_name] = getattr(imageinfo, field_name)
            else:
                raise RuntimeError("Config does not contain {0} value which is required by MBN v6 {1} metadata version {2}.{3}".format(field_name, authority, major_version, minor_version))
        # Sanitize metadata values
        for field in metadata_fields:
            field_format = field[0]
            field_count = len(field_format)
            field_name = field[1]
            if field_name != "flags":
                value = metadata[field_name]
                # Sanitize non-flag metadata values
                value_count = 1
                if field_name in metadata.keys():
                    if value is None:
                        if field_name == "soc_vers" or field_name == "multi_serial_numbers":
                            value = list()
                        else:
                            value = 0
                    elif isinstance(value, int):
                        pass
                    elif isinstance(value, long):
                        pass
                    elif isinstance(value, list):
                        value = [int(x, 16) if isinstance(x, basestring) or isinstance(x, long) else x for x in value]
                        value_count = len(value)
                    elif field_name == "soc_vers":
                        value = [int(soc_ver, 16) for soc_ver in value.split()]
                        value_count = len(value)
                    elif field_name == "multi_serial_numbers":
                        value = [int(serial, 16) for serial in value.serial]
                        value_count = len(value)
                    elif isinstance(value, basestring):
                        value = int(value, 16)
                    else:
                        raise RuntimeError("Cannot pack {0} into MBN v6 {1} metadata because it is not of type int, str, or list. Type: {2}".format(field_name, authority, metadata[field_name].__class__.__name__))
                    if value_count > field_count:
                        raise RuntimeError("Config contains {0} {1} values but MBN v6 {2} metadata version {3}.{4} only supports {5} {1} values".format(value_count, field_name, authority, major_version, minor_version, field_count))
                else:
                    raise RuntimeError("Config does not contain {0} value which is required by MBN v6 {1} metadata version {2}.{3}".format(field_name, authority, major_version, minor_version))
                # Update metadata value to reflect integer value packed in header
                metadata[field_name] = value
        # Sanitize metadata flag values
        for flag in flag_fields:
            field_name = flag[2]
            value = metadata[field_name]
            if field_name in metadata.keys():
                if value is None:
                    value = 0
                elif isinstance(value, basestring):
                    value = int(value, 16)
                elif isinstance(value, int):
                    pass
                else:
                    raise RuntimeError("Cannot pack {0} into MBN v6 {1} metadata because it is not of type int or str".format(field_name, authority))
            else:
                raise RuntimeError("Config does not contain {0} value which is required by MBN v6 {1} metadata version {2}.{3}".format(field_name, authority, major_version, minor_version))
            # Update metadata flag value to reflect integer value packed in header
            metadata[field_name] = value

    def get_metadata(self):
        if hasattr(self, "metadata"):
            return self.metadata
        return None

    def get_metadata_qti(self):
        if hasattr(self, "metadata_qti"):
            return self.metadata_qti
        return None

    def get_zero_filled_oem_metadata(self):
        return struct.pack('B', 0) * self.metadata_size

    def get_zero_filled_qti_metadata(self):
        return struct.pack('B', 0) * self.metadata_size_qti

    def set_version_in_bin(self, val):
        self.version = val

    def validate(self):
        if self.sig_size:
            if self.get_version_in_bin() != self.get_version():
                raise RuntimeError('MBN header version {0} does not match expected version {1}.'.format(self.get_version_in_bin(), self.get_version()))
            bin_version = self.get_metadata_version_in_bin()
            version = self.get_metadata_version()
            if bin_version != (None, None) and bin_version != version:
                raise RuntimeError('MBN OEM metadata version {0}.{1} does not match expected version {2}.{3}.'.format(bin_version[0], bin_version[1], version[0], version[1]))
            bin_version = self.get_metadata_version_qti_in_bin()
            version = self.get_metadata_version_qti()
            if bin_version != (None, None) and bin_version != version:
                raise RuntimeError('MBN QTI metadata version {0}.{1} does not match expected version {2}.{3}.'.format(bin_version[0], bin_version[1], version[0], version[1]))

    @staticmethod
    def repr_ignore_attr():
        return ["original_size", "contains_zero_filled_oem_metadata", "contains_zero_filled_qti_metadata", "metadata", "metadata_qti"]

    def __repr__(self):
        str_repr = obj_repr(self, self.repr_ignore_attr())
        if hasattr(self, "metadata") and self.metadata:
            str_repr += "Metadata:\n" + obj_repr(self.metadata, filter_list_zeros=True)
        if hasattr(self, "metadata_qti") and self.metadata_qti:
            str_repr += "Metadata QTI:\n" + obj_repr(self.metadata_qti, filter_list_zeros=True)
        return str_repr


"""
Map of: (MbnHeaderSize, MbnHeaderVersion) -> Mbn Header Class

Dictionary containing a mapping of the class to use for parsing the mbn header
based on the header type specified
"""
MBN_HDR_OBJS = [MbnHdr80B, MbnHdr40B, MbnHdr40BV5]
MBN_HDRS = dict([((m.get_size(), m.get_version()), m) for m in MBN_HDR_OBJS])


def copy_header(header):
    size = header.get_size()
    version = header.get_version()
    data = header.pack()
    if version == MBN_HDR_VERSION_6:
        header = MbnHdrV6(data)
    # mbn header v3 and v5 do have fixed sizes (40/80 and 40 bytes respectively)
    elif (size, version) in MBN_HDRS.keys():
        header = MBN_HDRS[(size, version)](data)
    else:
        raise RuntimeError('Unexpected header size, version provided:\n' +
                           '    Header size - ' + str(size) + ', version - ' + str(version) + '\n' +
                           '    Supported header types: ' + str(MBN_HDRS.keys()))
    return header


def extract_header(data, header_size, version=MBN_HDR_VERSION_3):
    # mbn header v6 does not have fixed size because of metadata so size can not be used to select header class
    if version == MBN_HDR_VERSION_6:
        header = MbnHdrV6(data)
    # mbn header v3 and v5 do have fixed sizes (40/80 and 40 bytes respectively)
    elif (header_size, version) in MBN_HDRS.keys():
        header = MBN_HDRS[(header_size, version)](data)
    else:
        raise RuntimeError('Unexpected header size, version provided:\n' +
                           '    Header size - ' + str(header_size) + ', version - ' + str(version) + '\n' +
                           '    Supported header types: ' + str(MBN_HDRS.keys()))
    header_size = header.get_original_size()
    data = data[header_size:]
    return data, header


def create_empty_header(header_size, version=MBN_HDR_VERSION_3):
    # mbn header v6 does not have fixed size because of metadata so size can not be used to select header class
    if version == MBN_HDR_VERSION_6:
        header = MbnHdrV6()
    # mbn header v3 and v5 do have fixed sizes (40/80 and 40 bytes respectively)
    elif (header_size, version) in MBN_HDRS.keys():
        header = MBN_HDRS[(header_size, version)]()
    else:
        raise RuntimeError('Unexpected header size, version provided:\n' +
                           '    Header size - ' + str(header_size) + ', version - ' + str(version) + '\n' +
                           '    Supported header types: ' + str(MBN_HDRS.keys()))
    header.set_version_in_bin(header.get_version())
    return header.pack()


def mask_oem_metadata_and_sizes(header):
    # Mask oem metadata if oem metadata exists in header
    if header.get_metadata():
        header.set_metadata(None)
        logger.debug("Masked OEM metadata")
    # Mask oem sig size and cert size
    header.sig_size = 0
    header.cert_chain_size = 0
    logger.debug("Masked OEM sig size and cert chain size")


def mask_qti_metadata_and_sizes(header):
    # Mask qti metadata if qti metadata exists in header
    if header.get_metadata_qti():
        header.set_metadata_qti(None)
        logger.debug("Masked QTI metadata")
    # Mask qti sig size and cert size
    header.sig_size_qti = 0
    header.cert_chain_size_qti = 0
    logger.debug("Masked QTI sig size and cert chain size")


def mask_header_values(header, authority):
    if header.supports_metadata():
        # Mask header fields of other authority
        if authority == AUTHORITY_OEM:
            mask_qti_metadata_and_sizes(header)
        else:
            mask_oem_metadata_and_sizes(header)
    return header
