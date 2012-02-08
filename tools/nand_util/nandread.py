#!/usr/bin/env python
# 
# Zeebo NAND Util - Read **** PRELIMINARY HACKY/TESTING VERSION ****
#
# Copyright (C) 2011 OpenZeebo
# Copyright (C) 2011 Triple Oxygen <mailme@tripleoxygen.net>
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
import classes
import time

def main(args):

	if len(sys.argv) != 4:
		print """
Zeebo NAND Util - Read v{0} - **** PRELIMINARY HACKY/TESTING VERSION ****
Copyright (C) 2011 OpenZeebo
Copyright (C) 2011 Triple Oxygen

Usage: \t{1} -[mode] [page_start] [page_end] [file]
\t{1} 0x1000 0x1200 file.bin
		""".format("0.1", sys.argv[0])
		sys.exit(1)

	page_start = int(args[0], 16)
	page_end = int(args[1], 16)
	fname = args[2]

	oocd = classes.OpenOcdClient('127.0.0.1', 3333)

	print "[+] Opening OpenOCD connection."

	if not oocd.connect():
		print "[-] Failed to connect."
		sys.exit(1)

	oocd.no_ack()

	config = open('CONFIG', 'rb')
	config_buf = config.read()
	config.close()

	print "[+] Configurating Zeebo NAND controller."

	oocd.write_word(0xa0b00000, 0x0)
	oocd.write_word(0xa0a0000c, 0x4)

	config1 = oocd.read_word(0xA0A00020)
	config2 = oocd.read_word(0xA0A00024)

	if ((config1 != 0xA25400C0) and (config1 != 0xAA5400C0)) or (config2 != 0x0004745E):
		print "[-] Error, invalid config"
		oocd.write(0xa0a00000, len(config_buf), config_buf)
		sys.exit(1)

	oocd.write_word(0xa0a00040, 0x0)
	oocd.write_word(0xa0a00000, 0x31)
	oocd.write_word(0xa0a00010, 0x1)
	oocd.write_word(0xa0a00000, 0xD)
	oocd.write_word(0xa0a00010, 0x1)
	oocd.write_word(0xa0a00000, 0xB)
	oocd.write_word(0xa0a00010, 0x1)

	nand_id = oocd.read_word(0xa0a00040)

	if nand_id != 0x5580b1ad:
		print "[-] Error, invalid NAND ID"
		sys.exit(1)

	f = open(fname, 'wb')
	x = time.time()

	oocd.write_word(0xa0a00000, 0x34) # 0x33 ECC / 0x34 ECC+Spare

	print "[+] Reading (page_start = 0x{0:X} / page_end = 0x{1:X}) ...".format(page_start, page_end - 1)

	while(page_start <= page_end):
		oocd.write_word(0xa0a00004, (page_start << 16))
		oocd.write_word(0xa0a00008, 0x0)
		
		print "[+] Reading page 0x{0:X}...".format(page_start)

		block_count = 0
		num_page = 3
		
		while(num_page >= 0):

			oocd.write_word(0xa0a00010, 0x1)
			buf = oocd.read(0xa0a00100, 0x210) # 0x200 w/o space / 0x210 w/ spare
			f.write(buf)

			st = oocd.read_word(0xa0a00014)

			num_page -= 1

			if(st & (1 << 4)):
				print "[-] Failed to read page segment %d, from page %x" % (block_count, page_start) 

			block_count += 1

		page_start += 1
	
	f.close()
	
	x = time.time() - x
	print "[+] Done in {0} s.".format(x)

if __name__ == "__main__":
	main(sys.argv[1:])
