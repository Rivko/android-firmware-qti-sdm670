"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
# COLUMN ONE: the symbol's value (virtual address)
# COLUMN TWO: a set of characters and spaces indicating the flag bits
# 			 that are set on the symbol. There are seven groupings
# 			 which are listed below:
#  - group one:   (l,g,,!) local, global, neither, both.
#  - group two:   (w,) weak or strong symbol.
#  - group three: (C,) symbol denotes a constructor or an ordinary symbol.
#  - group four:  (W,) symbol is warning or normal symbol.
#  - group five:  (I,) indirect reference to another symbol or normal symbol.
#  - group six:   (d,D,) debugging symbol, dynamic symbol or normal symbol.
#  - group seven: (F,f,O,) symbol is the name of function, file, object or normal symbol.
# COLUMN THREE: the section in which the symbol lives, ABS means not associated with a certain section
# COLUMN FOUR: the symbol's size or alignment.
# COLUMN FIVE: the symbol's name.

def is_int(s, base):
	try:
		int(s, base)
		return True
	except ValueError:
		return False

class Symbol:
	def __init__(self):
		self.value = None
		self.uvalue = None
		self.flags = None
		self.section = None
		self.size = None
		self.usize = None
		self.name = None
		self.name_upper = None
		return

	@classmethod
	def read_symbol(cls, line):
		s = Symbol()
		t = line.split()
		if len(t) <= 0 : return None
		s.value = t[0]
		# how many column 2 groups are present
		group_count = len(t) - 4
		if group_count <= 0 : return None
		s.flags = []
		for i in range(group_count):
			s.flags.append(t[1 + i])
		s.section = t[1 + group_count]
		s.size = t[2 + group_count]
		if not is_int(s.size, 16): return None
		s.name = t[3 + group_count]
		s.name_lower = s.name
		return s

	def dump(self):
		print('--- Symbol %s ---' % self.name)
		print('\tvalue:   %s' % self.value)
		print('\tflags:   %s' % self.flags)
		print('\tsection: %s' % self.section)
		if type(self.size) is str :
			print('\tsize:	{0}'.format(int(self.size, 16)))
		else :
			print('\tsize:	{0}'.format(self.size))
		return

class symbol_table(list):
	def __init__(self):
		return

	def dump(self):
		a = iter(self)
		for s in a:
			s.dump()
		return

	def read_symbol_table(self, filename):
		with open(filename) as f:
			for line in f:
				s = Symbol.read_symbol(line)
				if s is not None:
					self.append(s)
		return

	def all_containing(self, name) :
		ss = symbol_table()
		name = name.upper()
		i = iter(self)
		for s in i:
			if name in s.name_lower.upper() :
				ss.append(s)
		return ss

	def all_in_section(self, section_name) :
		ss = symbol_table()
		name = section_name.upper()
		i = iter(self)
		for s in i:
			if name == s.section.upper() :
				ss.append(s)
		return ss

	def first_n_in_section(self, section_name, n) :
		ss = symbol_table()
		name = section_name.upper()
		i = iter(self)
		for s in i:
			if name == s.section.upper() :
				ss.append(s)
			if n == len(ss):
				return ss
		return ss

	def first_in_section(self, section_name) :
		name = section_name.upper()
		i = iter(self)
		for s in i:
			if name == s.section.upper() :
				return s

	def gapsize(sy1, sy2):
		return n

	def isuperset(self, substr):
		print("superset(\"%s\")"%substr)
		ss = None
		group = [s for s in self if substr in s.name]
		for s in group:
			# print(" ... including %s"%s.name)
			# TODO: lookup section for CODE tag
			if ('text' not in s.section):
				# print("not in text")
				continue
			if (ss is None):
				# s.dump()
				print("starts with {0}@{1}".format(s.name, s.value))
				ss = Symbol()
				ss.name = substr
				ss.value = s.value
				ss.size = s.size
				ss.flags = []
				# ss.dump()
				continue
			if (int(s.value, 16) < int(ss.value, 16)):
				# s.dump()
				grow = int(ss.value, 16) - int(s.value, 16)
				print("grow down {0}={1}".format(s.name,grow))
				ss.size = str(int(ss.size, 16) + grow)
				print("ss.size=%s"%ss.size)
				ss.value = s.value
				print("ss.value=%s"%ss.value)
				# ss.dump()
				continue
			if ((int(s.value, 16)+int(s.size, 16)) > (int(ss.value, 16)+int(ss.size, 16))):
				# s.dump()
				grow = (int(s.value, 16)+int(s.size, 16)) - (int(ss.value, 16)+int(ss.size, 16))
				print("grow up({0}) {0}".format(s.name, grow))
				oldsize = int(ss.size, 16)
				print("oldsize=%d"%oldsize)
				ss.size = hex((int(ss.size, 16) + grow)).strip('0xL')
				print("ss.size=%s"%ss.size)
				# ss.dump()
				continue
			# print("{0} is already in {1} ... {2}".format(s.value, ss.value, ss.value + ss.size))			
		if not ss is None : ss.dump()
		return ss

	def superset(self, group):
		print("superset(\"%s\")"%group)
		split = group.split();
		sss = None
		for name in split :
			print("... name: \"%s\""%name)
			ss = self.isuperset(name)
			if (not ss is None) :
					if sss is None : sss = []
					sss.append(ss)

		return sss


	def find_by_name(self, name) :
		i = iter(self)
		for s in i:
			if s.name == name :
				return s
		return None

