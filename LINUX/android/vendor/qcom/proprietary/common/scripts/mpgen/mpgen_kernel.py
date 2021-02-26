"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
from __future__ import absolute_import

import sys
import os
import mpgen_shell
import mpgen_symbols
import mpgen_sections

OBJCOPY = None
OBJDUMP = None
SYMBOLS_FILENAME = 'symbols_tmp.txt'
HEADERS_FILENAME = 'headers_tmp.txt'
DWARF_FILENAME = 'dwarf_tmp.txt'

KERNEL_IMAGE = 'kernel_image'

# CUSTOM EXCEPTIONS
class ExceptionSymbolNotFound(Exception):
	pass


def outdated(target, dependency) :
	if (not os.path.isfile(dependency)) :
		print('|-------------------|\n'
			  '| ERROR ERROR ERROR |\n'
			  '|-------------------|')
		print('unable to locate %s' % dependency)
		print('exiting ...')
		sys.exit(1)
	if (not os.path.isfile(target)) :
		return True
	attrib_dependency = os.stat(dependency)
	attrib_target = os.stat(target)
	if (attrib_target.st_mtime > attrib_dependency.st_mtime) :
		return False
	return True

def get_dd_sha1(MODULE_DD_BIN) :
	sha1 = mpgen_shell.dsystemf('openssl dgst -c -sha1 %s', MODULE_DD_BIN)
	return sha1

def get_dd_sha256(MODULE_DD_BIN) :
	sha256 = None
	digest = mpgen_shell.dsystemf('openssl dgst -c -sha256 %s', MODULE_DD_BIN)
	ll = digest.split('=')
	if (len(ll) == 2):
		sha256 = ll[1].strip(' \n\r')
	return sha256

class Kernel:
	symbol_table = None
	section_table = None

	def __init__(self):
		return

	def get_symbols_filename(self) :
		return SYMBOLS_FILENAME

	def get_sections_filename(self) :
		return HEADERS_FILENAME

	def get_symbol_file_pos(self, symbol_name):
		symbol = self.symbol_tbl.find_by_name(symbol_name)
		if symbol is None: return None
		section = self.section_tbl.find_by_name(symbol.section)
		if section is None: return None
		section_offset_in_file = int(section.file_off, 16)
		symbol_offset_in_section = int(symbol.value, 16) - int(section.VMA, 16)
		symbol_offset_in_file = symbol_offset_in_section + section_offset_in_file
		return symbol_offset_in_file

	def get_kernel(self, vmlinux) :
		if (OBJCOPY is None):
				print("ERROR: objcopy not defined\n")
				sys.exit(1)
		if (OBJDUMP is None):
				print("ERROR: objdump not defined\n")
				sys.exit(1)
		if (outdated(KERNEL_IMAGE, vmlinux)) :
			print("OBJCOPY\n")
			mpgen_shell.dsystemf("%s %s %s", OBJCOPY, vmlinux, KERNEL_IMAGE)
			# mv(kernel_path, ".");
		if (outdated(SYMBOLS_FILENAME, KERNEL_IMAGE)) :
			print("SYMBOLS\n")
			mpgen_shell.dsystemf('%s -t %s > %s', OBJDUMP, KERNEL_IMAGE, SYMBOLS_FILENAME)
		if (outdated(HEADERS_FILENAME, KERNEL_IMAGE)) :
			print("SECTIONS\n")
			mpgen_shell.dsystemf("%s -h %s > %s", OBJDUMP, KERNEL_IMAGE, HEADERS_FILENAME)

		# mpgen_shell.dsystemf('%s --dwarf %s > %s', OBJDUMP, KERNEL_IMAGE, DWARF_FILENAME);

		self.symbol_tbl = mpgen_symbols.symbol_table()
		self.symbol_tbl.read_symbol_table(self.get_symbols_filename())

		self.section_tbl = mpgen_sections.Section_Table()
		self.section_tbl.read_section_table(self.get_sections_filename())
		return

	def get_section(self, section_name):
		section = self.section_tbl.find_by_name(section_name)
		if (section is None):
			print("ERROR could not find section %s\n" % section_name)
			sys.exit(1)
		return section

	def superset(self, group):
		print("kernel superset")
		ss = self.symbol_tbl.superset(group)
		print("kernel superset count == %d" % len(ss))
		return ss

	def get_symbol(self, symbol_name):
		# section table search takes precedence - symbol table also contains section items
		symbol = self.section_tbl.find_by_name(symbol_name)
		if (symbol is None):
			# now search sybmol table
			symbol = self.symbol_tbl.find_by_name(symbol_name)
			if (symbol is None):
				# print("WARNING could not find symbol %s\n" % symbol_name)
				raise ExceptionSymbolNotFound(symbol_name)

		return symbol

	def get_symbal_sha256(self, symbol_name):
		MODULE_DD_BIN = self.dd_symbol(symbol_name)
		sha256 = get_dd_sha256(MODULE_DD_BIN)
		mpgen_shell.rm(MODULE_DD_BIN)
		return sha256

	def get_symbol_contents(self, symbol_name):
		MODULE_DD_BIN = self.dd_symbol(symbol_name)
		f = open(MODULE_DD_BIN)
		try:
			contents = bytearray(f.read())
		finally:
			f.close()
		f.close()

		mpgen_shell.rm(MODULE_DD_BIN)
		return contents

	def get_symbol_contents_as_str(self, symbol_name):
		MODULE_DD_BIN = self.dd_symbol(symbol_name)
		f = open(MODULE_DD_BIN)
		try:
			contents = str(f.read())
		except Exception as ex:
			contents = None
			print("\n\nget_symbol_contents_as_str(\"%s\")\n"%symbol_name)
			template = "An exception of type {0} occurred. Arguments:\n{1!r}"
			message = template.format(type(ex).__name__, ex.args)
			print message
			sys.exit()
		finally:
			f.close()
		f.close()

		mpgen_shell.rm(MODULE_DD_BIN)
		return contents

	def get_section_sha256(self, section_name):
		MODULE_DD_BIN = self.dd_section(section_name)
		sha256 = get_dd_sha256(MODULE_DD_BIN)
		mpgen_shell.dsystemf('rm %s', MODULE_DD_BIN)
		return sha256

	def dd_symbol(self, symbol_name):
		symbol = self.symbol_tbl.find_by_name(symbol_name)
		if symbol is None: return None

		MODULE_DD_BIN = 'tmp_%s_dd.bin' % symbol_name
		start = self.get_symbol_file_pos(symbol_name)
		count = int(symbol.size, 16)

		mpgen_shell.dd(start, count, KERNEL_IMAGE, MODULE_DD_BIN)
		return MODULE_DD_BIN

	def dd_section(self, section_name):
		section = self.section_tbl.find_by_name(section_name)
		if section is None: return None

		MODULE_DD_BIN = 'tmp_%s_dd.bin' % section_name
		start = int(section.file_off, 16)
		count = int(section.size, 16)

		mpgen_shell.dd(start, count, KERNEL_IMAGE, MODULE_DD_BIN)
		return MODULE_DD_BIN

	def get_banner(self) :
		banner = None
		banner = self.get_symbol_contents_as_str("linux_banner")
		if banner is None : banner = "unknown kernel version"
		return banner


