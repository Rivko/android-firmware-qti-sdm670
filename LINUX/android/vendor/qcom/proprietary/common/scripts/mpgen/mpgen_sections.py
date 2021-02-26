"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
#
# kernel_image:	 file format elf64-littleaarch64
#
# Sections:
# Idx Name		  Size	  VMA			   LMA			   File off  Algn
#   0 .head.text	00000300  ffffffc000080000  ffffffc000080000  00010000  2**6
# 				   CONTENTS, ALLOC, LOAD, READONLY, CODE
#   1 .text		 00ce6358  ffffffc000200000  ffffffc000200000  00020000  2**12
# 				   CONTENTS, ALLOC, LOAD, READONLY, CODE
#   2 .text.init	00000060  ffffffc000ee6360  ffffffc000ee6360  00d06360  2**4
# 				   CONTENTS, ALLOC, LOAD, READONLY, CODE
# ...
#

def is_int(s, base=10):
	try:
		int(s, base)
		return True
	except ValueError:
		return False

class Section:
	idx = None
	name = None
	size = None
	VMA = None
	value = None
	LMA = None
	file_off = None
	align = None
	flags = None

	def __init__(self):
		return

	@classmethod
	def read_section(cls, line):
		s = Section()
		t = line.split()
		if len(t) <= 5 : return None
		if not is_int(t[0]): return None
		if not int(t[0]) >= 0: return None
		s.idx = t[0]
		s.name = t[1]
		s.size = t[2]
		s.VMA = t[3]
		s.value = s.VMA  # duplicate property to make it symbol like
		s.LMA = t[4]
		s.file_off = t[5]
		s.align = t[6]
		return s

	def dump(self):
		print('{0:>3} {1:<14} {2} {3} {4} {5} {6}'
			  .format(self.idx, self.name, self.size,
					  self.VMA, self.LMA, self.file_off, self.align))
		print('{0}'.format(self.flags))
		return

class Section_Table(list):
	def __init__(self):
		return

	def dump(self):
		print('========== mpgen_sections Section_Table.dump() START ============')
		a = iter(self)
		for s in a:
			s.dump()
		print('========== mpgen_sections Section_Table.dump() END   ============')
		return

	def read_section_table(self, filename):
		with open(filename) as f:
			while True:
				line = f.readline()
				if not line: break  # EOF
				s = Section.read_section(line)
				if s is not None:
					flags = f.readline()
					if not flags: break  # EOF
					s.flags = flags
					self.append(s)
		return

	def find_by_name(self, name) :
		i = iter(self)
		for s in i:
			if s.name == name :
				return s
		return None

