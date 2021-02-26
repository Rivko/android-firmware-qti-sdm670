"""
Auto generated classes
"""


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
        self.has_multi_image_segment = False
        self.has_license_manager_segment = False
        self.image_type = 0
        self.validate_ph_addrs = True
        self.validate_vir_addrs = False
        self.has_sections = False
        self.max_elf_segments = 0
        self.hash_seg_placement = ''
        self.persist_encr = False

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


