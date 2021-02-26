"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
#
# MPGEN Measurement Parameter Database
#
# This database is programmatically populated based on mpgen catalogs
# using virtual memory address and DWARF structure data
# extracted from a compiled target image (i.e. vmlinux or kernel image)
#
# These structures define various measurement types:
#
# -- Read Only or Execute Only code or data
# -- Write Once data
# 	 -- Where the run-time sha hash is computed after first write
# 	 -- Where the run-time sha hash is known at compile time
# -- Write Authenticated data
# 	 -- Where the authorized writer is defined by a code range
# 	 -- Where the authorized value is defined by a given memory range
#

# MP_TYPE
RD_ONLY = 1
WR_ONCE_KNOWN = 2
WR_ONCE_UNKNOWN = 3
WR_AUTH_WRITER = 4
WR_AUTH_VALUE = 5
KCONFIG = 6

#Section vs. symbol
SYMBOL = 0
SECTION = 1

class MP():
	def __init__(self, name, vaddr, size, attrs):
		self.mp_type = None
		self.name = name
		self.vaddr = vaddr
		self.size = size
		self.attrs = attrs
		return
	def dump(self):
		print('MP({0}:{1}:{2})'
			   .format(self.mp_type, self.vaddr, self.size))
		return

class MP_RD_ONLY(MP):
	def __init__(self, name, vaddr, size, attrs, sha256):
		# super(MP_RD_ONLY, self).__init__()
		MP.__init__(self, name, vaddr, size, attrs)
		self.mp_type = RD_ONLY  # MP_TYPE
		self.sha256 = sha256
		return
	def dump(self):
		print('MP_RD_ONLY({0}:{1}:{2}:{3})'
			   .format(self.mp_type, self.vaddr, self.size, self.sha256))
		return

class MP_WR_ONCE_KNOWN(MP):
	def __init__(self, name, vaddr, size, attrs, sha256):
		MP.__init__(self, name, vaddr, size, attrs)
		self.mp_type = WR_ONCE_KNOWN
		self.sha256 = sha256
		return
	def dump(self):
		print('MP_WR_ONCE_KNOWN({0}:{1}:{2}:{3})'
			   .format(self.mp_type, self.vaddr, self.size, self.sha256))
		return

class MP_WR_ONCE_UNKNOWN(MP):
	def __init__(self, name, vaddr, size, attrs, sha256):
		MP.__init__(self, name, vaddr, size, attrs)
		self.mp_type = WR_ONCE_UNKNOWN
		self.sha256 = sha256
		return
	def dump(self):
		print('MP_WR_ONCE_UNKNOWN({0}:{1}:{2}:{3})'
			   .format(self.mp_type, self.vaddr, self.size))
		return

class MP_WR_AUTH_WRITERS(MP):
	def __init__(self, name, vaddr, size, attrs, writers):
		MP.__init__(self, name, vaddr, size, attrs)
		self.mp_type = WR_AUTH_WRITER
		self.writers = writers
		return
	def dump(self):
		print('MP_WR_AUTH_WRITER({0}:{1}:{2}:{3})'
			   .format(self.name, self.mp_type, self.vaddr, self.size))
		for writer in self.writers:
			writer.dump()
#		print('MP_WR_AUTH_WRITER({0}:{1}:{2}:{3})'
#			   .format(self.mp_type, self.vaddr, self.size))
#					   self.vaddr, self.size))
#					   self.writer_vaddr, self.writer_size))
		return

class MP_WR_AUTH_VALUE(MP):
	def __init__(self, name, vaddr, size, attrs, value_vaddr, value_size):
		MP.__init__(self, vaddr, size, attrs)
		self.mp_type = WR_AUTH_VALUE
		self.value_vaddr = value_vaddr
		self.value_size = value_size
		return
	def dump(self):
		print('MP_WR_AUTH_VALUE({0}:{1}:{2}:{3})'
			   .format(self.mp_type, self.vaddr, self.size))
		return

class MP_KCONFIG():
	def __init__(self, name, value):
		self.mp_type = KCONFIG
		self.name = name
		self.value = value
		return
	def dump(self):
		print('MP_KCONFIG({0}:{1}))'
				.format(self.mp_type, self.name, self.value))
		return

class MPDB(list):

	def __init__(self, catalog = None):
		"""
			Accept original catalog object (optional)
			Catalog may contain other elements that may be used by the templates - e.g. mp warning msg			
		"""

		list.__init__(self)
		self.catalog = catalog
		return

	def get_rd_only_count(self):
		count = 0
		a = iter(self)
		for mp in a:
			if (mp.mp_type is RD_ONLY):
				count = count + 1
		return count

	def dump(self):
		print('========== mpgen_mpdb.dump() START ============')
		a = iter(self)
		for s in a:
			s.dump()
		print('========== mpgen_mpdb.dump() END   ============')
		return


