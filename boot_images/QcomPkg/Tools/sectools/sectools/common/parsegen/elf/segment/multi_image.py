# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import struct
from collections import OrderedDict

from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import properties_repr, hex_addr, multi_image_string

MULT_IMAGE_VERSION_0 = 0
SUPPORTED_VERSIONS = [MULT_IMAGE_VERSION_0]
MAX_NUM_IMAGES = 100
ENCODE_HASH_ALGO = {
    "sha256": 2,
    "sha384": 3
}
DECODE_HASH_ALGO = dict((v, k) for k, v in ENCODE_HASH_ALGO.iteritems())
HASH_ALGO_TO_SIZE_MAP = {
    "sha256": 32,
    "sha384": 48
}
MAGIC_NUM = "MULT"


class MultiImageSegment(object):

    def __init__(self, hash_algorithm, data=None, images=None):
        self._initialize(hash_algorithm, images)

        # Validate hash algorithm
        if self.hash_algorithm not in ENCODE_HASH_ALGO:
            raise RuntimeError(multi_image_string() + " segment does not support configured segment_hash_algorithm {0}".format(self.hash_algorithm))

        # Validate that required information is provided
        if data is None and not images:
            raise RuntimeError("When creating a " + multi_image_string() + " segment at least one of the following must be provided:\n"
                               "\t1) An existing " + multi_image_string() + " segment\n"
                               "\t2) Images to add to (or update) in the " + multi_image_string() + " segment")

        if data:
            self._unpack(data)
        if images:
            self._update()

    def _initialize(self, hash_algorithm, images):
        self.image_hash_dict = OrderedDict()
        self.images_to_add_or_update = images if images is not None else list()
        self.hash_algorithm = hash_algorithm if hash_algorithm is not None else "sha256"
        self.magic_number = MAGIC_NUM
        self.version = MULT_IMAGE_VERSION_0
        num_reserved = 32
        for i in range(num_reserved):
            setattr(self, "res"+str(i), 0)

    def _unpack(self, data):
        # Validate size of segment is not smaller than allowed minimum
        min_size_of_hash_entry = min(HASH_ALGO_TO_SIZE_MAP.values())
        min_size = self._get_fixed_size() + self._get_id_size() + min_size_of_hash_entry
        if len(data) < min_size:
            raise RuntimeError(multi_image_string() + " segment is of invalid size {0} bytes."
                               "\nMinimum allowed size is {1} bytes.".format(len(data), min_size))

        # Extract segment header
        offset = 0
        end = self._get_fixed_size()
        (self.magic_number,
         self.version,
         self.res0, self.res1, self.res2, self.res3, self.res4, self.res5, self.res6, self.res7, self.res8, self.res9,
         self.res10, self.res11, self.res12, self.res13, self.res14, self.res15, self.res16, self.res17, self.res18, self.res19, self.res20,
         self.res21, self.res22, self.res23, self.res24, self.res25, self.res26, self.res27, self.res28, self.res29, self.res30, self.res31,
         num_images,
         hash_algorithm,) = struct.unpack(self._get_fixed_format(), data[offset:end])

        # Validate extracted values
        if self.magic_number != MAGIC_NUM:
            raise RuntimeError(multi_image_string() + " segment contains invalid magic number {0}."
                               "\nMagic number must be {1}.".format(self.magic_number, MAGIC_NUM))
        if self.version != MULT_IMAGE_VERSION_0:
            raise RuntimeError(multi_image_string() + " segment contains invalid version number {0}."
                               "\nSupported version are: {1}.".format(self.version, SUPPORTED_VERSIONS))
        if num_images > MAX_NUM_IMAGES:
            raise RuntimeError(multi_image_string() + " segment contains {0} image entries."
                               "\nMaximum allowed number of images entries is {1}.".format(num_images, MAX_NUM_IMAGES))
        if hash_algorithm not in DECODE_HASH_ALGO:
            raise RuntimeError(multi_image_string() + " segment contains invalid hash algorithm value {0}."
                               "\nAllowed hash algorithm values are {1}.".format(hash_algorithm, DECODE_HASH_ALGO.keys()))
        elif DECODE_HASH_ALGO[hash_algorithm] != self.hash_algorithm:
            raise RuntimeError(multi_image_string() + " segment was created using {0} but segment_hash_algorithm is configured to {1}."
                               "\nChange configured segment_hash_algorithm value to {0}.".format(DECODE_HASH_ALGO[hash_algorithm], self.hash_algorithm))
        # Validate segment size
        expected_size = self._get_fixed_size() + (self._get_id_size() + HASH_ALGO_TO_SIZE_MAP[self.hash_algorithm]) * num_images
        if expected_size < len(data):
            raise RuntimeError(multi_image_string() + " segment is of invalid size {0} bytes."
                               "\nSegment containing {1} {2} image entries should be {} bytes.".format(len(data), num_images, self.hash_algorithm, expected_size))

        # Extract sw_id/app_id and hash values
        for i in range(num_images):
            offset = end
            end += self._get_id_size()
            (sw_id, app_id,) = struct.unpack(self._get_id_format(), data[offset:end])
            hash_size = HASH_ALGO_TO_SIZE_MAP[self.hash_algorithm]
            offset = end
            end += hash_size
            image_hash = data[offset:end]
            self.image_hash_dict[(sw_id, app_id)] = image_hash

    def _update(self):
        def get_hash(data):
            from sectools.features.isc.signer.utils.hmac import HMAC
            h = HMAC.HASH_ALGO_MAP[self.hash_algorithm]()
            h.update(data)
            return h.digest()

        for image in self.images_to_add_or_update:
            sw_id = int(image.imageinfo.sw_id, 16)
            app_id = image.imageinfo.app_id
            app_id_normalized = int(app_id, 16) if app_id is not None else 0
            data = image.get_multi_image_entry_data(image.authority)
            if not data:
                raise RuntimeError("Cannot add image entry to " + multi_image_string() + " segment for image with SW_ID={0} because it has no hash segment.".format(hex(sw_id)))
            # Update existing hash entry
            if (sw_id, app_id_normalized) in self.image_hash_dict.keys():
                if app_id is None:
                    logger.info("Updated " + multi_image_string() + " entry for image with SW_ID={0}".format(hex(sw_id)))
                else:
                    logger.info("Updated " + multi_image_string() + " entry for image with SW_ID={0} and APP_ID={1}".format(hex(sw_id), hex(app_id_normalized)))
            # Add new hash entry
            else:
                if app_id is None:
                    logger.info("Added " + multi_image_string() + " entry for image with SW_ID={0}".format(hex(sw_id)))
                else:
                    logger.info("Added " + multi_image_string() + " entry for image with SW_ID={0} and APP_ID={1}".format(hex(sw_id), hex(app_id_normalized)))
            self.image_hash_dict[(sw_id, app_id_normalized)] = get_hash(data)
        if len(self.image_hash_dict) > MAX_NUM_IMAGES:
            raise RuntimeError(multi_image_string() + " segment cannot contains {0} image entries."
                               "\nMaximum allowed number of images entries is {1}.".format(len(self.image_hash_dict), MAX_NUM_IMAGES))

    def pack(self):
        # Pack segment header
        data = struct.pack(self._get_fixed_format(),
                           self.magic_number,
                           self.version,
                           self.res0, self.res1, self.res2, self.res3, self.res4, self.res5, self.res6, self.res7, self.res8, self.res9,
                           self.res10, self.res11, self.res12, self.res13, self.res14, self.res15, self.res16, self.res17, self.res18, self.res19, self.res20,
                           self.res21, self.res22, self.res23, self.res24, self.res25, self.res26, self.res27, self.res28, self.res29, self.res30, self.res31,
                           len(self.image_hash_dict),
                           ENCODE_HASH_ALGO[self.hash_algorithm])

        # Pack sw_id/app_id and hash values
        for (sw_id, app_id), image_hash in self.image_hash_dict.iteritems():
            data += struct.pack(self._get_id_format(), sw_id, app_id)
            data += image_hash

        return data

    @classmethod
    def _get_fixed_format(cls):
        return '4sI' + 'Q'*32 + 'II'

    @classmethod
    def _get_fixed_size(cls):
        return struct.Struct(cls._get_fixed_format()).size

    @classmethod
    def _get_id_format(cls):
        return 'II'

    @classmethod
    def _get_id_size(cls):
        return struct.Struct(cls._get_id_format()).size

    def get_size(self):
        size = self._get_fixed_size()
        size += (self._get_id_size() + HASH_ALGO_TO_SIZE_MAP[self.hash_algorithm]) * len(self.image_hash_dict)
        return size

    def __repr__(self):
        image_entries = list()
        index = 1
        for (sw_id, app_id), _ in self.image_hash_dict.iteritems():
            image_entries.append(("Entry " + str(index), "SW_ID: " + hex_addr(sw_id) + " APP_ID: " + hex_addr(app_id)))
            index += 1
        return "Header:\n" + properties_repr([("Segment Size", str(self.get_size())+"B"),
                                              ("Magic", self.magic_number),
                                              ("Version", self.version),
                                              ("Hash Algorithm", self.hash_algorithm),
                                              ("Num Image Entries", len(self.image_hash_dict)),
                                              ]) + \
               'Image Entries: ' + '\n' + properties_repr(image_entries)
