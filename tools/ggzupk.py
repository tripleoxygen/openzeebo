#!/usr/bin/env python
# 
# Double Dragon Zeebo GGZ Unpacker
#
# Copyright (C) 2012 OpenZeebo
# Copyright (C) 2012 Triple Oxygen <mailme@tripleoxygen.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 or version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

import sys
import struct
import zlib
import os

MAGIC_BE = 0x1f8b0808

def main():

	print
	print "[+] Double Dragon Zeebo GGZ Unpacker - 2012 OpenZeebo\n"
	
	if(len(sys.argv) != 3):
		usage(sys.argv[0])
		sys.exit(-1)
	
	f = open(sys.argv[1], 'rb')
	out = sys.argv[2]

	if not os.path.exists(out):
		os.mkdir(out)

	os.chdir(out)

	flist = parse_table(f)

	if(len(flist) < 1):
		print "[-] No files found in GGZ file."
		sys.exit(-1)

	process(f, flist)
		
	print "[+] Finished.\n"

	f.close()


def process(f, lst):

	size = 0
	count = len(lst)

	for i in range(count):

		name = ""
		ch = 0xff
		
		ofs = lst[i][0]
		f.seek(ofs)

		buf = f.read(4 + 4 + 2)
		
		magic, unk, padding = struct.unpack(">LLH", buf)

		if magic != MAGIC_BE:
			print "[-] Entry magic is invalid. Skipping."
			continue

		# Null terminated string
		while ch != '\x00':
			ch = f.read(1)
			if ch != '\x00':
				name += ch

		cur = f.tell()

		if i < (count - 1):
			# Use next file offset to calculate the size instead of
			# marker. Subtract 4 to remove it.
			size = lst[i + 1][0] - cur - 4
		else:
			# No next entry, use total file length.
			f.seek(0, os.SEEK_END)
			size = f.tell() - cur - 4

		f.seek(cur)
		
		print "[+] Unpacking file {0}: '{1}' at 0x{2:x} ({3} bytes)".format(i + 1, name, cur, size)

		unpack(f, cur, size, name)
		
		

def parse_table(f):

	entry = ""
	lst = []

	f.seek(0)

	while True:
		entry = f.read(8)

		if len(entry) < 8:
			print "[-] EOF reached and magic wasn't found. Invalid GGZ file?"
			return []
			
		ofs, marker = struct.unpack(">LL", entry)
		
		if(ofs == MAGIC_BE):
			break

		lst.append([ofs, marker])

	return lst

def unpack(f, ofs, size, name):
	f.seek(ofs)
	buf = f.read(size)

	o = open(name, 'wb')
	# Raw deflated stream
	o.write(zlib.decompress(buf, -zlib.MAX_WBITS))
	o.close()


def usage(app):
	print "{0} <file.ggz> <output dir>\n".format(app)


if __name__ == "__main__":
    main()
