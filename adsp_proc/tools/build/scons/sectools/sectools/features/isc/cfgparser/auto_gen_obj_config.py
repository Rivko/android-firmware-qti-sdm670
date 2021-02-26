#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
"""
Auto generated classes
"""

class Cfg_Secimage:

    def __init__(self):
        self.parsegen = Cfg_Parsegen()
        self.signing = Cfg_Signing()
        self.images_list = Cfg_Images_List()
        self.data_provisioning = Cfg_Data_Provisioning()
        self.general_properties = Cfg_General_Properties()
        self.post_process = Cfg_Post_Process()
        self.metadata = Cfg_Metadata()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Parsegen:

    def __init__(self):
        self.image_types_list = Cfg_Image_Types_List()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Image_Types_List:

    def __init__(self):
        self.image_type = []

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Image_Type:

    def __init__(self):
        self.file_type = ''
        self.ewm_properties = Cfg_Ewm_Properties()
        self.mbn_properties = Cfg_Mbn_Properties()
        self.pmbl_properties = Cfg_Pmbl_Properties()
        self.elf_properties = Cfg_Elf_Properties()
        self.id = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Ewm_Properties:

    def __init__(self):
        self.image_entry = ''
        self.relocatable = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Mbn_Properties:

    def __init__(self):
        self.header_size = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Pmbl_Properties:

    def __init__(self):
        self.min_size_with_pad = 0
        self.preamble_size = 0
        self.page_size = 0
        self.has_magic_num = False
        self.num_of_pages = 0
        self.ota_enabled = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Elf_Properties:

    def __init__(self):
        self.image_type = 0
        self.max_elf_segments = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Signing:

    def __init__(self):
        self.signer_attributes = Cfg_Signer_Attributes()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Signer_Attributes:

    def __init__(self):
        self.cass_signer_attributes = Cfg_Cass_Signer_Attributes()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Cass_Signer_Attributes:

    def __init__(self):
        self.server = Cfg_Server()
        self.user_identity = Cfg_User_Identity()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Server:

    def __init__(self):
        self.trust_keystore = ''
        self.trust_keystore_password = ''
        self.host = ''
        self.host_validation_mode = ''
        self.trust_keystore_type = ''
        self.port = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_User_Identity:

    def __init__(self):
        self.token_identity_slot = 0
        self.keystore_type = ''
        self.keystore_file = ''
        self.token_password = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Images_List:

    def __init__(self):
        self.image = []

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Image:

    def __init__(self):
        self.post_process_commands = ''
        self.name = ''
        self.general_properties_overrides = Cfg_General_Properties_Overrides()
        self.meta_build_location = ''
        self.image_type = ''
        self.pil_split = False
        self.cert_config = ''
        self.sign_id = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_General_Properties_Overrides:

    def __init__(self):
        self.model_id = ''
        self.mask_soc_hw_version = ''
        self.sw_id = ''
        self.soc_hw_version = ''
        self.exponent = 3
        self.rot_en = ''
        self.num_certs_in_certchain = 2
        self.crash_dump = ''
        self.use_serial_number_in_signing = 0
        self.object_id = Cfg_Object_Id()
        self.num_root_certs = 0
        self.msm_part = ''
        self.max_cert_size = 2048
        self.in_use_soc_hw_version = 0
        self.key_size = 2048
        self.debug = ''
        self.oem_id = ''
        self.testsig_serialnum = ''
        self.app_id = ''
        self.mrc_index = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Object_Id:

    def __init__(self):
        self.max = ''
        self.name = ''
        self.min = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Data_Provisioning:

    def __init__(self):
        self.base_path = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_General_Properties:

    def __init__(self):
        self.model_id = ''
        self.cass_capability = ''
        self.app_id = ''
        self.msm_part = ''
        self.max_cert_size = 2048
        self.selected_cert_config = ''
        self.use_serial_number_in_signing = 0
        self.in_use_soc_hw_version = 0
        self.exponent = 3
        self.num_certs_in_certchain = 2
        self.object_id = Cfg_Object_Id()
        self.key_size = 2048
        self.crash_dump = ''
        self.rot_en = ''
        self.mask_soc_hw_version = ''
        self.num_root_certs = 0
        self.sw_id = ''
        self.soc_hw_version = ''
        self.hash_pageseg_as_segment = False
        self.selected_encryptor = ''
        self.selected_signer = ''
        self.mrc_index = 0
        self.debug = ''
        self.oem_id = ''
        self.testsig_serialnum = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Post_Process:

    def __init__(self):
        self.pil_splitter = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


class Cfg_Metadata:

    def __init__(self):
        self.chipset = ''
        self.version = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


