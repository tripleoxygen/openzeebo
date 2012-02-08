#!/usr/bin/env python
# 
# Zeebo NAND Util - Write **** PRELIMINARY HACKY/TESTING VERSION ****
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

	if len(sys.argv) != 5:
		print """
Zeebo NAND Util - Write v{0} - **** PRELIMINARY HACKY/TESTING VERSION ****
Copyright (C) 2011 OpenZeebo
Copyright (C) 2011 Triple Oxygen

Usage: \t{1} -[mode] [page_start] [page_end] [file]
\t{1} -e 0x1000 0x1200 file.bin
\t{1} -w 0x1000 0x1200 file.bin

\tcommand:

\t\tw - Write NAND page interval
\t\te - Erase NAND page interval
		""".format("0.1", sys.argv[0])
		sys.exit(1)

	mode = args[0]
	page_start = int(args[1], 16)
	page_end = int(args[2], 16)
	fname = args[3]

	if page_start > page_end:
		print "[-] Start page can't be greater than end page."
		sys.exit(1)

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
		print "[-] Error, invalid config, uploading default..."
		oocd.write(0xa0a00000, len(config_buf), config_buf)
		oocd.write_word(0xa0a00040, 0x0)
		oocd.write_word(0xa0a0000c, 0x4)
		sys.exit(1)

	oocd.write_word(0xa0a00000, 0x31)
	oocd.write_word(0xa0a00010, 0x1)
	oocd.write_word(0xa0a00000, 0xD)
	oocd.write_word(0xa0a00010, 0x1)
	oocd.write_word(0xa0a00000, 0xB)
	oocd.write_word(0xa0a00010, 0x1)

	nand_id = oocd.read_word(0xa0a00040)

	print "[+] Checking NAND ID..."

	if nand_id != 0x5580b1ad:
		print "[-] Error, invalid NAND ID"
		sys.exit(1)

	if(mode == "e"):
		oocd.write_word(0xA0A00020, config1 & ~(7 << 6))
		print "Config 1 {0:X}".format(config1 & ~(7 << 6))

		last_block = 0xFFFFFFFF
		block = 0

		while(page_start <= page_end):
			block = page_start / 64

			if block != last_block:
				last_block = block

				oocd.write_word(0xa0a00004, page_start)
				oocd.write_word(0xa0a00008, 0x0)
				oocd.write_word(0xa0a00000, 0x3A)
				oocd.write_word(0xa0a00010, 0x1)

				time.sleep(.1)

				st = oocd.read_word(0xa0a00014)

				if((st & 0x110) or (not(st & 0x80))):
					msg = "[-] Error erasing block {0:X}. Status: {1:X}".format(page_start / 64, st)
					print msg
				else:
					msg = "[+] Block 0x{0:03X} erased. NAND Status: 0x{1:X}".format(page_start / 64, st)
					print msg
			else:
				pass

			page_start += 1 # should be += 0x40, since its block-wide

		oocd.write_word(0xA0A00020, config1)
		sys.exit(1)

	f = open(fname, 'rb')
	x = time.time()

	oocd.write_word(0xa0a00000, 0x36)
	page_yay = 0

	while(page_start <= page_end):
		oocd.write_word(0xa0a00004, (page_start << 16))
		oocd.write_word(0xa0a00008, 0x0)

		msg = "[+] Writing page 0x{0:04X}...".format(page_start)
		print msg

		block_count = 0
		num_page = 3

		while(num_page >= 0):
			#page_offset = page_start * 2112 + block_count * 528
			page_offset = page_start * 2048 + block_count * 512
			f.seek(page_offset, 0) 
			#page_buf = f.read(528)
			page_buf = f.read(512)

			if (page_buf[240:248] == '\xff\xff\xffv\xff\xff\xff\xff'):
				msg = "[-] Unprogrammed segment {0} on page 0x{1:04X} found.".format(block_count, page_start)
				print msg
				block_count += 1
				num_page -= 1
				continue

			oocd.write(0xa0a00100, len(page_buf), page_buf)
			oocd.write_word(0xa0a00010, 0x1)

			st = oocd.read_word(0xa0a00014)

			num_page -= 1
			msg = "[+]\tNAND Status: 0x{0:X}".format(st)

			if((st & 0x110) or (not(st & 0x80))):
				msg = "[-] Failed to write page segment %d, from page %X" % (block_count, page_start)
				print msg
			else:
				pass
				
			block_count += 1

		page_start += 1
		page_yay += 1

	f.close()

	x = time.time() - x
	msg = "[+] Done in {0} s.".format(x)
	print msg

if __name__ == "__main__":
	main(sys.argv[1:])
