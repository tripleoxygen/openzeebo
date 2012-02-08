#!/usr/bin/env python
# 
# Bootloader uploader
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

import sys, getopt, serial, os, struct
from crc import crc16
from qcserial import QCSerial

DOWNLOAD_CMD = "\x3a\xa1\x6e\x7e"
REPLY_PACKET = "\x7e\x02\x6a\xd3\x7e"

UPLOAD_BLOCK_SIZE = 512 + 256
UPLOAD_ADDR = 0xa00000

def header():
	print ""
	print "Booloader uploader"
	print "(c) 2012 OpenZeebo.org"
	print ""

def usage():
	print "Usage: ./blupload.py [OPTIONS] [FILE]"
	print ""
	print "Options:\t-p\tPort number"
	print "\t\t-d\tReboot device in download mode (needed before sending BL)"
	print "\t\t-f\tBootloader code"
	print ""
	print "Example:\t./blupload.py -p/dev/ttyUSB0 -d"
	print "\t\t./blupload.py -pCOM8 -f/tmp/bl.bin"
	print ""

def msgerr(str):
	print "[-] {0}".format(str)

def msginfo(str):
	print "[+] {0}".format(str)

def build_packet(data):
	packet  = data
	packet += struct.pack('<H', crc16(data))
	packet  = packet.replace('\x7d', '\x7d\x5d').replace('\x7e', '\x7d\x5e')
	packet  = '\x7e' + packet + '\x7e'
	return packet

def write_command(addr, buf, size):
	data  = '\x0f'
	data += struct.pack('>L', addr)
	data += struct.pack('>H', size)
	data += buf	
	return build_packet(data)

def ping_command():
	data = '\x06'
	return build_packet(data)

def run_command(addr):
	data  = '\x05'
	data += struct.pack('>L', addr)
	return build_packet(data)

def main(args):
	
	header()
	
	if len(sys.argv) != 3:
		usage()
		sys.exit(1)
		
	try:
		opts, args = getopt.getopt(args, "p:df:")
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(2)
	
	port = "0"
	blfile = ""
	done = False
	send_dl = False
	
	for o, a in opts:
		if o == "-p":
			port = a
		elif o == "-d":
			send_dl = True
		elif o == "-f":
			blfile = a
		else:
			assert False, "Unhandled option."

	ser = QCSerial(port, 115200, 1)
	
	if(ser.connect() != 0):
		msgerr("Connection to device failed.")
		sys.exit(-1)
		
	if send_dl:
		ser.send(DOWNLOAD_CMD)
		status = ser.recv(4)
		if status == DOWNLOAD_CMD:
			msginfo("Download mode command sent.")
			sys.exit(0)
		else:
			msgerr("Failed to send command mode.")
			sys.exit(-1)
	
	fd = open(blfile, 'rb')

	msginfo("Pinging target...");

	ser.send(ping_command())
	if ser.recv(5) != REPLY_PACKET:
		msgerr("Device didn't reply or in wrong mode.")
		fd.close()
		ser.close()
		sys.exit(-1)

	addr = UPLOAD_ADDR
	size = UPLOAD_BLOCK_SIZE

	buf = fd.read(UPLOAD_BLOCK_SIZE)
	size = len(buf)
	
	while size > 0:
		msginfo("Writing {0} bytes @ 0x{1:08x} ...".format(size, addr))
		packet = write_command(addr, buf, size)
		addr  += size
		ser.send(packet)

		if ser.recv(5) != REPLY_PACKET:
			msgerr("Device NACKed packet or went offline.")
			break

		buf = fd.read(UPLOAD_BLOCK_SIZE)
		size = len(buf)

	msginfo("Sending RUN command...")
	ser.send(run_command(UPLOAD_ADDR))
	if ser.recv(5) != REPLY_PACKET:
		msgerr("Device NACKed packet or went offline.")
	fd.close()
	ser.close()
	msginfo("Done.")

if __name__ == "__main__":
	main(sys.argv[1:])
