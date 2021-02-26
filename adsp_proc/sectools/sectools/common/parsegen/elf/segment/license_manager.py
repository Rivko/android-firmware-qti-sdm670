# ===============================================================================
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import struct

from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import properties_repr, hex_addr, create_mismatch_table


NAMESZ = 8
DESCSZ = 16
TYPE = 0
NAME = "LICMNGR\0"
VERSION_0 = 0


class LicenseManagerSegment(object):

    def __init__(self, client_id, lib_id, signer=None, data=None, checking_if_licmngr_seg=False, validating=False):
        # Validate that required information is provided
        if client_id is None or lib_id is None:
            raise RuntimeError("When creating or validating a License Manager segment both client_id and lib_id must be provided.")
        self._initialize(client_id, lib_id, signer, validating)
        if data:
            self._unpack(data, checking_if_licmngr_seg)

    def _initialize(self, client_id, lib_id, signer, validating):
        from sectools.features.isc.signer import signer_mgr
        self.validating = validating
        self.namesz = NAMESZ
        self.descsz = DESCSZ
        self.type = TYPE
        self.name = NAME
        self.version = VERSION_0
        self.client_id = int(client_id, 16)
        self.lib_id = int(lib_id, 16)
        self.is_cass_signed = int(signer_mgr.get_signer(signer).is_prod_signer()) if signer else 0

    def _unpack(self, data, checking_if_licmngr_seg):
        # Validate size of segment matches expected size
        if len(data) != self.get_size():
            raise RuntimeError("License Manager segment is of invalid size {0} bytes."
                               "\n       Allowed size is {1} bytes.".format(len(data), self.get_size()))

        # Extract segment values
        (namesz,
         descsz,
         type,
         name,
         version,
         client_id,
         lib_id,
         is_cass_signed,) = struct.unpack(self._get_format(), data)

        # Validate extracted values
        error = list()
        if namesz != self.namesz:
            error += ["License Manager segment contains invalid namesz value {0}.".format(namesz),
                      "namesz must be {0}.".format(self.namesz)]
        if descsz != self.descsz:
            error += ["License Manager segment contains invalid descsz value {0}.".format(descsz),
                      "descsz must be {0}.".format(self.descsz)]
        if type != self.type:
            error += ["License Manager segment contains invalid type value {0}.".format(type),
                      "type must be {0}.".format(self.type)]
        if name != self.name:
            error += ["License Manager segment contains invalid name value {0}.".format(name),
                      "name must be {0}.".format(self.name)]
        if error:
            raise RuntimeError("\n       ".join(error))

        if not checking_if_licmngr_seg:
            # Update existing segment
            if not self.validating:
                if version != self.version:
                    logger.warning("License Manager segment contains unknown version value {0}.\n         "
                                   "Updating version value to {1}".format(version, self.version))
                if client_id != self.client_id:
                    logger.warning("License Manager segment's client_id value {0} does not match configured value {1}.\n         "
                                   "Updating client_id value to {1}".format(hex(client_id), hex(self.client_id)))
                if lib_id != self.lib_id:
                    logger.warning("License Manager segment's lib_id value {0} does not match configured value {1}.\n         "
                                   "Updating lib_id value to {1}".format(hex(lib_id), hex(self.lib_id)))
                if is_cass_signed != self.is_cass_signed:
                    logger.warning("License Manager segment's is_cass_signed value {0} does not match configured value {1}.\n         "
                                   "Updating is_cass_signed value to {1}".format(is_cass_signed, self.is_cass_signed))
            # Validate segment against config
            else:
                errstr = list()
                mismatches = list()
                if client_id != self.client_id:
                    mismatches.append(("client_id", hex(client_id), hex(self.client_id)))
                if lib_id != self.lib_id:
                    mismatches.append(("lib_id", hex(lib_id), hex(self.lib_id)))
                create_mismatch_table(mismatches, errstr, operation="license management", data_type_to_compare="Attribute", image_region="License Manager Segment")
                if errstr:
                    raise RuntimeError('Following validations failed for the image:\n       ' +
                                       '\n       '.join([(str(i + 1) + '. ' + e) for i, e in enumerate(errstr)]))
                self.version = version
                self.client_id = client_id
                self.lib_id = lib_id
                self.is_cass_signed = is_cass_signed

    def pack(self):
        return struct.pack(self._get_format(),
                           self.namesz,
                           self.descsz,
                           self.type,
                           self.name,
                           self.version,
                           self.client_id,
                           self.lib_id,
                           self.is_cass_signed)

    @classmethod
    def _get_format(cls):
        return 'III8sIIII'

    @classmethod
    def get_size(cls):
        return struct.Struct(cls._get_format()).size

    @staticmethod
    def is_valid(client_id, lib_id, data=None):
        valid = True
        try:
            LicenseManagerSegment(client_id, lib_id, data=data, checking_if_licmngr_seg=True)
        except:
            valid = False
        return valid

    def __repr__(self):
        return properties_repr([("Segment Size", str(self.get_size())+"B"),
                                ("namesz", self.namesz),
                                ("descsz", self.descsz),
                                ("type", self.type),
                                ("name", self.name),
                                ("version", self.version),
                                ("client_id", hex_addr(self.client_id)),
                                ("lib_id", hex_addr(self.lib_id)),
                                ("is_cass_signed", self.is_cass_signed)
                                ])
