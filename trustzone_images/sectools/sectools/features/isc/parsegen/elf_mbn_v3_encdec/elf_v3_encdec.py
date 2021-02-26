# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen import elf
from sectools.common.parsegen.mbn.versions.v3_encdec.parsegen_mbn import ParseGenMbnV3EncDec
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_logging import logger
from sectools.features.isc.encryption_service.unified.encrypted_key_provider_mgr import \
    encrypted_key_provider_id_supported
from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec
from sectools.features.isc.parsegen.elf_mbn_v3_base.elf_v3_base import ElfV3Base
from sectools.features.isc.defines import MAX_SIG_SIZE
from sectools.features.isc.parsegen.elf_mbn_v3_encdec.encdec import ParseGenEncDec
from sectools.features.isc.parsegen.elf_support.elf_delegate import ElfDelegate
from sectools.features.isc.parsegen.elf_support.elf_vars import FILE_DECRYPTED_IN


class ElfV3EncDec(ElfV3Base):

    def __init__(self,
                 data,
                 parsegen_mbn_class=ParseGenMbnV3EncDec,
                 imageinfo=None,
                 elf_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        # Check the arguments
        if imageinfo is not None:
            elf_properties = imageinfo.image_type.elf_properties
            general_properties = imageinfo.general_properties
            encdec = imageinfo.encdec
        if elf_properties is None:
            raise RuntimeError('ELF properties must not be None.')

        self._serial_num = None

        super(ElfV3EncDec, self).__init__(data,
                                          parsegen_mbn_class=parsegen_mbn_class,
                                          imageinfo=imageinfo,
                                          elf_properties=elf_properties,
                                          general_properties=general_properties,
                                          encdec=encdec,
                                          debug_dir=debug_dir,
                                          debug_prefix=debug_prefix,
                                          debug_suffix=debug_suffix,
                                          validating=validating, signing=signing,
                                          parsegens=parsegens,
                                          sign_attr=sign_attr)

        self.hash_pageseg_as_segment = general_properties.hash_pageseg_as_segment

        self.oem_encryption_enabled = self.encdec is not None

        self.check_if_file_encrypted(general_properties)

        self.serial_num = general_properties.testsig_serialnum

    def _repr_properties(self):
        return SecParseGenBaseEncDec._repr_properties(self)

    @property
    def data_to_sign(self):
        # Add the header and hash segment
        prog_phdr_entry, hash_phdr_entry, _ =\
            self._add_phdr_and_hash_segs(self.sign, encrypt=self.encrypt)

        # Get the data to sign from the hash segment
        data = self.get_hash_segment_data_to_sign()

        # Remove the prog and hash
        self._elf_parsegen.remove_segment(prog_phdr_entry)
        self._elf_parsegen.remove_segment(hash_phdr_entry)

        return data

    @property
    def data_to_sign_format(self):
        # Add the header and hash segment
        prog_phdr_entry, hash_phdr_entry, _ =\
            self._add_phdr_and_hash_segs(self.sign, encrypt=self.encrypt)

        # Get the data to sign from the hash segment
        fmt = self.get_hash_segment_format()

        # Remove the prog and hash
        self._elf_parsegen.remove_segment(prog_phdr_entry)
        self._elf_parsegen.remove_segment(hash_phdr_entry)

        return fmt

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'serial_num'
    #--------------------------------------------------------------------------
    @property
    def serial_num(self):
        return self._serial_num

    @serial_num.setter
    def serial_num(self, value):
        if value is not None:
            self.validate_serial_num(value)
            value = Attribute.init(num_bits=32, string=value).value
        self._serial_num = value

    def validate_serial_num(self, value):
        if Attribute.validate(num_bits=32, string=value) is False:
            raise RuntimeError('testsig serial number ' + str(value) + ' is not a valid 32 bit number')

    #--------------------------------------------------------------------------
    # methods related to inserting serial number as testsig
    #--------------------------------------------------------------------------
    def insert_testsig(self, hash_table, get_hash_count):
        # Insert testsig at this point if its needed
        if not get_hash_count and self.serial_num is not None:
            hash_table = self._insert_serial_in_ht(hash_table, self._serial_num)
        return hash_table

    def _insert_serial_in_ht(self, hash_table, serial):
        hash_len = len(self._get_hash(''))
        serial_packed = struct.pack('L', serial)
        return (hash_table[:hash_len] +
                serial_packed +
                hash_table[hash_len + len(serial_packed):])

    #--------------------------------------------------------------------------
    # Methods related to encryption parameters
    #--------------------------------------------------------------------------
    def _compute_hash_segment_size(self, sign, encrypt=False):
        hash_table_len = ((len(self._get_hash('')) * 2) +
                          len(self.get_hash_table()))

        # Check configuration
        signatures = 1

        if (sign and signatures == 2 and self.oem_encryption_enabled) or encrypt:
            encrypt_params = len(self.encdec.get_encryption_parameters_blob())
        else:
            encrypt_params = 0

        cert_chain_size = 0
        if sign:
            if signatures == 1:
                cert_chain_size = self.get_cert_chain_size(self.authority)
            else:
                cert_chain_size = (self.get_cert_chain_size(self.authority) +
                                   self.get_cert_chain_size(
                                       self.get_other_authority(self.authority)))

        sig_size = MAX_SIG_SIZE * (signatures - 1) if sign else 0
        if sign:
            sig_size += self.sig_size

        # Sum
        return (self._mbn_parsegen.get_header(self.authority,
                                              self.imageinfo,
                                              self.validating,
                                              self.signing,
                                              self.add_signing_attr_to_hash_seg)
                .get_size() + hash_table_len + sig_size + cert_chain_size +
                encrypt_params)

    #--------------------------------------------------------------------------
    # Encryption & Decryption methods
    #--------------------------------------------------------------------------
    def check_if_file_encrypted(self, general_properties):
        # Check if the file is encrypted
        if self.is_encrypted():
            if encrypted_key_provider_id_supported(general_properties.UIE_key):
                raise RuntimeError(
                    "Cannot decrypt an encrypted image while using encrypted key provider")

            self._decrypt_data()
            self.store_debug_data(FILE_DECRYPTED_IN, self._elf_parsegen.get_data())

            # Get the original data
            self._elf_parsegen = elf.ParseGenElf(self._elf_parsegen.get_data(),
                                                 self.debug_dir,
                                                 self.debug_prefix,
                                                 self.debug_suffix)

            # Ensure that hashes match. If they don't we can assume that decryption failed
            if (self._mbn_parsegen.code[self.HASH_ALGO_TO_SIZE_MAP[
                    self._mbn_parsegen.extracted_segment_hash_algorithm]*2:] != self.get_hash_table()):
                raise RuntimeError("Decryption of image failed. This can be caused by:"
                                   "\n\t1) use of an invalid L3 key"
                                   "\n\t2) use of an incorrect segment hash algorithm")

            # Set the elf parsegen delegate
            delegate = ElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
            self._elf_parsegen.delegate = delegate

    def get_hash_segment(self, sign, encrypt=None):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        return self._get_data_int(sign, encrypt)

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
        integrity_check = True if (integrity_check or sign or encrypt) else False

        # Allow base to do any checks
        SecParseGenBaseEncDec.get_data(self, integrity_check, sign, encrypt=encrypt)

        if not (integrity_check or sign or encrypt):
            data = self._elf_parsegen.get_data()
        else:
            # Add the header and hash segment
            prog_phdr_entry, hash_phdr_entry, hash_segment_size =\
                self._add_phdr_and_hash_segs(sign, encrypt=encrypt)

            try:
                # Generate the hash segment now
                hash_segment = self.get_hash_segment(sign, encrypt)

                # padding 0xFF for XBL SEC use case
                hash_segment += PAD_BYTE_1 * int(self._hash_padding_size)

                # Check here for sizes mismatching just in case
                if len(hash_segment) != hash_segment_size:
                    raise RuntimeError('Estimated hash table size was wrong. Estimate - ' +
                                       str(hash_segment_size) + ', Actual - ' + str(len(hash_segment)))

                # Re-add the hash segment, this time with the real data
                self._elf_parsegen.remove_segment(hash_phdr_entry)
                self._elf_parsegen.remove_segment(prog_phdr_entry)
                self._elf_parsegen.add_segment(prog_phdr_entry, '', toalign=self._align)
                self._elf_parsegen.add_segment(hash_phdr_entry, hash_segment, index=1, toalign=self._align)

                # If encrypting, change the process segment data
                if encrypt:
                    parsegen_updater = ParseGenEncDec(self.store_debug_data, self.encrypt_segment)
                    parsegen_updater.update_parsegen(self.encdec.get_segment_num_scheme(), self._elf_parsegen)
                try:
                    # Get the elf data
                    data = self._elf_parsegen.get_data()
                finally:
                    # Restore the process segment data
                    if encrypt:
                        try:
                            parsegen_updater.revert_parsegen(self._elf_parsegen)
                        except Exception as e:
                            logger.warning(str(e))
            finally:
                # Remove the prog
                try:
                    self._elf_parsegen.remove_segment(prog_phdr_entry)
                except Exception as e:
                    logger.warning(str(e))

                # Remove the hash
                try:
                    self._elf_parsegen.remove_segment(hash_phdr_entry)
                except Exception as e:
                    logger.warning(str(e))
        return data

    def _decrypt_data(self):
        if self.encdec is None:
            logger.error("Input image is encrypted but decryption of image failed. There are two possible causes:\n"
                         + "\t1) Selected_encryptor is not set in config file or command line args.\n"
                         + "\t2) You are attempting to sign but not encrypt a previously encrypted image. This is not allowed.")
            raise RuntimeError('Image is encrypted. Cannot proceed without a decryptor.')

        decryptor = self.encdec.get_decryptor(encryption_parameters_blob=self.encryption_params,
                                              key=self.encdec.get_dummy_key())

        parsegen_updater = ParseGenEncDec(self.store_debug_data, self.encrypt_segment)
        parsegen_updater.decrypt(self.encdec.get_segment_num_scheme(), self._elf_parsegen, decryptor)
