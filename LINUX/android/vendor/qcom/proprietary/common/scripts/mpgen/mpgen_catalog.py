# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

from __future__ import print_function
import mpgen_mpdb
import sys

class Entry():
	def __init__(self, mp_type, name, attrs, section_or_symbol = mpgen_mpdb.SYMBOL):
		self.mp_type = mp_type
		self.name = name
		self.attrs = attrs
		self.section_or_symbol = section_or_symbol
		return

	def dump(self):
		print('self.mp_type == {0}'.format(self.mp_type))
		print('self.name == {0}'.format(self.name))
		print('self.section_or_symbol == {0}'.format(self.section_or_symbol))
		return

class Catalog(list):
	def __init__(self):
		list.__init__(self)
		self.mp_warning_msg = ""  # the warning that will be passed to the TA from the MP
		self.is_checkable_kernel = 1  # the flag that will be passed to the TA and HT
		return

	def ro_section(self, section_name, attrs = '0x0'):
		el = Entry(mpgen_mpdb.RD_ONLY, section_name, attrs, mpgen_mpdb.SECTION)
		self.append(el)
		return

	def wu_section(self, section_name, attrs = '0x0'):
		el = Entry(mpgen_mpdb.WR_ONCE_UNKNOWN, section_name, attrs, mpgen_mpdb.SECTION)
		self.append(el)
		return

	def ro_symbol(self, symbol_name, attrs = '0x0'):
		el = Entry(mpgen_mpdb.RD_ONLY, symbol_name, attrs)
		self.append(el)
		return

	def write_once_known(self, symbol_name, attrs = '0x0', value = None):
		el = Entry(mpgen_mpdb.WR_ONCE_KNOWN, symbol_name, attrs)
		self.append(el)
		return

	def write_once_unknown(self, symbol_name, attrs = '0x0'):
		el = Entry(mpgen_mpdb.WR_ONCE_UNKNOWN, symbol_name, attrs)
		self.append(el)
		return

	def auth_writer(self, symbol_name, writer, attrs = '0x0'):
		el = Entry(mpgen_mpdb.WR_AUTH_WRITER, symbol_name, attrs)
		el.writer = writer
		self.append(el)
		return

	def kconfig(self, name, attrs):
		el = Entry(mpgen_mpdb.KCONFIG, name, attrs)
		self.append(el)
		return

	def mp_warning(self, msg):
		""" 
			Prints the warning to stderr and adds this to MP. New line character will separate the TA messages
			
			The MP structure capable of holding a short warning/error message
			This message is printed during the TA startup to QSEE log
			
			Note: try to keep it short as this var is about 239b long 
		"""
		print("WARNING: ", msg, file = sys.stderr)  # print to stderr
		# escape special C characters but not /n
		tmp = ""
		for c in msg + "\n":
			if c in ('\\', '"', "%") or not (32 <= ord(c) < 127):
				tmp += '\\x%02x' % ord(c)
			else:
				tmp += c
		self.mp_warning_msg += tmp
		self.mp_warning_msg = self.mp_warning_msg[0:238]  # slice it just in case

	def mark_kernel_non_checkable(self):
		""" 
			Sets is_checkable_kernel in the MP template header
		"""
		self.is_checkable_kernel = 0


def kprobes(config):
	if (   config.is_parameter('CONFIG_FTRACE', 'y')
		or config.is_parameter('CONFIG_KPROBES', 'y')) :
		return True
	else :
		return False

def errata(config):
	if (   config.is_parameter('CONFIG_ARM64_ERRATUM_826319', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_827319', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_824069', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_819472', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_832075', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_845719', 'y')
		or config.is_parameter('CONFIG_ARM64_ERRATUM_843419', 'y')
		or config.is_parameter('CONFIG_CAVIUM_ERRATUM_22375', 'y')
		or config.is_parameter('CONFIG_CAVIUM_ERRATUM_23154', 'y')
		or config.is_parameter('CONFIG_CAVIUM_ERRATUM_27456', 'y')
		or config.is_parameter('CONFIG_QCOM_QDF2400_ERRATUM_0065', 'y')) :
		return True
	else : return False

def get_catalog(config):
	cat = Catalog()

	# TEST CATALOG
	# For production catalog contact Qualcomm Technologies, Inc.

	if 1 == 0 :
		cat.ro_section('populate_all_ro_sections')
	else :
		if (kprobes(config)):
			cat.mark_kernel_non_checkable()
			cat.mp_warning('report is not trusted due to kprobes or tracing')
			cat.ro_section('.head.text', attrs = 'MP_SECTION_ATTRIBUTE_NQHEE | MP_SECTION_ATTRIBUTE_NONSTRICT')
			
			# Boot up performance will be severely affected if we enable the code sections with FTRACE enabled.
			cat.auth_writer('.text', '__apply_alternatives __arch_copy_to_user', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
			cat.auth_writer('.head.text', '__apply_alternatives __arch_copy_to_user', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
			cat.auth_writer('.init.text', '__apply_alternatives __arch_copy_to_user', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
		elif (errata(config)):
			# TODO: test this case
			cat.mark_kernel_non_checkable()
			cat.mp_warning('treat as write once due to errata')
			cat.wu_section('.head.text', attrs = 'MP_SECTION_ATTRIBUTE_NQHEE')
			cat.wu_section('.text',      attrs = 'MP_SECTION_ATTRIBUTE_NQHEE')
			cat.wu_section('.init.text', attrs = 'MP_SECTION_ATTRIBUTE_NQHEE')

			# Boot up performance will be severely affected if we enable the code sections with ERRATA enabled.
			cat.auth_writer('.text', '__apply_alternatives', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
			cat.auth_writer('.head.text', '__apply_alternatives', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
			cat.auth_writer('.init.text', '__apply_alternatives', attrs = 'MP_SECTION_ATTRIBUTE_CODE')
		else :
			cat.mp_warning('read only')
			cat.ro_section('.head.text', attrs = 0)
			cat.ro_section('.text',      attrs = 0)
			cat.ro_section('.init.text', attrs = 0)

	cat.ro_symbol('linux_banner',      attrs = 'MP_SECTION_ATTRIBUTE_NQHEE')
	cat.ro_symbol('linux_proc_banner', attrs = 'MP_SECTION_ATTRIBUTE_NQHEE')

	# NOTE: below symbols contain pointers that are affected by KASLR
	cat.write_once_unknown('selinux_hooks', attrs = 'MP_SECTION_ATTRIBUTE_ENFORCE | MP_SECTION_ATTRIBUTE_NONSTRICT')
	cat.write_once_unknown('selinux_ops', attrs = 'MP_SECTION_ATTRIBUTE_ENFORCE | MP_SECTION_ATTRIBUTE_NONSTRICT')  # Note: selinux_ops may not present on recent kernels (i.g. 4.9)
	# Good WU's
	cat.write_once_unknown('ss_initialized')
	# TODO: fix WK generation: Alex Oct 10 2017 - ss_initialized hash does not match if it is WK asset.
	# cat.write_once_known('ss_initialized', '1', 0)

	cat.auth_writer('ss_initialized', 'security_load_policy', attrs = 'MP_SECTION_ATTRIBUTE_ENFORCE')
	cat.auth_writer('selinux_enforcing', 'selinux', attrs = 'MP_SECTION_ATTRIBUTE_ENFORCE')

	cat.kconfig('CONFIG_ARM64_ICACHE_DISABLE', 0)
	cat.kconfig('CONFIG_ARM64_DCACHE_DISABLE', 0)
	cat.kconfig('CONFIG_FTRACE', 0)
	cat.kconfig('CONFIG_KPROBES', 0)
	cat.kconfig('CONFIG_ARM64', 0)
	cat.kconfig('CONFIG_SYSTEM_TRUSTED_KEYS', 0)
	cat.kconfig('CONFIG_DM_VERITY', 0)
	cat.kconfig('CONFIG_DM_ANDROID_VERITY', 0)
	cat.kconfig('CONFIG_DM_VERITY_FEC', 0)
	cat.kconfig('CONFIG_DM_VERITY_AVB', 0)
	cat.kconfig('CONFIG_IMA', 0)
	cat.kconfig('CONFIG_SECURITY', 0)
	cat.kconfig('CONFIG_INTEGRITY', 0)

	cat.kconfig('CONFIG_SECURITYFS', 0)
	cat.kconfig('CONFIG_EVM', 0)
	cat.kconfig('CONFIG_MODULES', 0)
	cat.kconfig('CONFIG_', 0)

	return cat

