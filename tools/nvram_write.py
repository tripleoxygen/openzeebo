#!/usr/bin/env python
# 
# NVRAM Write
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

import sys, getopt, serial, os, struct
from crc import crc16
from qcserial import QCSerial

NVITEM_SIZE = (1 + 2 + 128)

def header():
    print ""
    print "NVRAM Item writer"
    print "(c) 2011 OpenZeebo.org"
    print ""

def usage():
    print "Usage: ./nvram_write.py [OPTIONS] [FILE]"
    print ""
    print "Options:     -p              Port number"
    print "             -n              NVRAM item number to write in hex (use * to write every item from file)"
    print "             -f              File containing NVRAM items"
    print ""
    print "Example:     ./nvram_write.py -p/dev/ttyUSB0 -n* -fnvitems.bin"
    print "             ./nvram_write.py -pCOM8 -n0x3A -f/tmp/nvitems.bin"
    print ""

def msgerr(str):
    print "[-] {0}".format(str)
    
def msginfo(str):
    print "[+] {0}".format(str)

def nvram_write_command(num, data):
    
    packet  = '\x27'
    packet += struct.pack('<H', num)
    packet += data
    packet += '\x00\x00'
    packet += struct.pack('<H', crc16(packet))
    packet  = packet.replace('\x7d', '\x7d\x5d')
    packet  = packet.replace('\x7e', '\x7d\x5e')
    packet += '\x7e'
    
    return packet

def main(args):
    
    header()
    
    if len(sys.argv) != 4:
        usage()
        sys.exit(1)
        
    try:   
        opts, args = getopt.getopt(args, "p:n:f:")
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)
    
    port = "0"
    write_all = False
    nv_file = ""
    done = False
    
    for o, a in opts:
        if o == "-p":
            port = a
        elif o == "-n":
            if a == "*":
                write_all = True
            else:
                item = int(a)
        elif o == "-f":
            nv_file = a
        else:
            assert False, "Unhandled option."

    try:
        filesize = os.stat(nv_file).st_size
    except os.OSError, err:
        msgerr(str(err))
        sys.exit(1)
        
    if ((filesize % NVITEM_SIZE) != 0):
        msgerr("NV Items file isn't multiple of {0} bytes.".format(NVITEM_SIZE))
        sys.exit(1)
        
    ser = QCSerial(port, 115200, 1)
    
    if(ser.connect() != 0):
        msgerr("Connection to device failed.")
        sys.exit(-1)
    
    fd = open(nv_file, 'rb')
    
    for i in range(0, filesize / NVITEM_SIZE):
        
        nv_item = fd.read(NVITEM_SIZE)
        
        item_cmd, item_num, item_data = struct.unpack("<cH128s", nv_item)
               
        if(write_all or (item_num == item)):
            
            packet = nvram_write_command(item_num, item_data)
            
            ser.send(packet)
            status = ser.recv(1);
            
            if (len(status) == 0):
                msgerr("Reply packet size is 0 bytes. Something is wrong.")
                break
            
            if (status == '\x42'):
                msgerr("Unknown reply code.")
                continue
            
            reply = ser.recv(len(packet) - 1)
            status = reply[-5]
            
            if   (status == '\x07'):
                msgerr("NV item {0} is read-only.".format(item_num))
            elif (status == '\x05'):
                msgerr("NV item {0} is inactive.".format(item_num))
            else:
                msginfo("Wrote NV item number {0}.".format(item_num))
            
    fd.close()
    ser.close()
    
    msginfo("Done!")
    
if __name__ == "__main__":
    main(sys.argv[1:])
