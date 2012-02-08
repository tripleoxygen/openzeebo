
# 
# Zeebo NAND Util - **** PRELIMINARY HACKY/TESTING VERSION ****
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

import socket
import struct
import time

class PacketFactory:
    """Creates and formats packets for OpenOCD."""
    
    def query(self, command):
        return ("$" + command + "#" + self.checksum(command))

    def read_mem(self, address, size):
        packet = "m%08x,%x" % (address, size)
        return ("$" + packet + "#" + self.checksum(packet))
        
    def write_mem(self, address, size, buf):
        packet = "M%08x,%x:%s" % (address, size, buf)
        return ("$" + packet + "#" + self.checksum(packet))
  
    def checksum(self, packet):
        return "%02x" % (sum(ord(c) for c in packet) % 256)    
        

class OpenOcdClient:
    """Client for communicating with OpenOCD's GDB server."""
    
    TIMEOUT = 1.0    
    MAX_PACKET_SIZE = 1024

    def __init__(self, host, port):
        self.host = host;
        self.port = port;
        self.packet_factory = PacketFactory()
        
    def connect(self):
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.settimeout(self.TIMEOUT)
            self.s.connect((self.host, self.port))
            return True
        except:
            print "Couldn't connect to OpenOCD."
            return False
    
    def close(self):
        if self.s:
            self.s.close()

    def no_ack(self):
         packet = self.packet_factory.query("QStartNoAckMode")         
         try:
             self.s.send(packet)
             buf = self.s.recv(16)
             print "no_ack: ", buf
             buf = self.s.recv(16)
             print "no_ack: ", buf
         except:
             print "Failed to set noack"
            
    def read_word(self, address):
        word = self.read(address, 4)
        #print "> ", word.encode("hex")
        return struct.unpack('<L', word)[0]
        
    def read(self, address, size):
        #time.sleep(0.001)
        packet = self.packet_factory.read_mem(address, size)
        
        #print "read: {0:X}, {1:X}".format(address, size)
        #print "read: packet {0}".format(packet)
        
        try:
            self.s.send(packet)
                
            packet_size = size * 2 + 4;
            
            #buf = self.s.recv(packet_size)
            # Max single packet size is 1024. More than that, it is splitted
            # in 3 packets: $ + data + checksum
            
            if packet_size <= self.MAX_PACKET_SIZE:
                
                
                buf = self.s.recv(packet_size)
                data = buf[1:-3]
                
                if data == "OK":
                    #print "ijiwjiwjiwj"
                    buf = self.s.recv(packet_size)
                    data = buf[1:-3]
                    
                #print "read: < MAX buf = {0}, data = {1}".format(buf, data)
                
            else:
                #pass
                buf = self.s.recv(1)
                data = self.s.recv(size * 2)
                buf = self.s.recv(3)
                
            crc = self.packet_factory.checksum(data)
           
            if not buf.endswith(crc):
                print "Corrupted data on read"

            return data.decode("hex")
        except Exception as ex:
            print ex

    def write_word(self, address, value):
        word = struct.pack('<L', value)
        self.write(address, 4, word)
        
    def write(self, address, size, data):
        #time.sleep(0.001)
        enc_data = data.encode("hex")
        packet = self.packet_factory.write_mem(address, size, enc_data)        
        try:
            self.s.send(packet)
            buf = self.s.recv(16)

            #if buf[1:-3] != 'OK':
            #    raise Exception("OpenOCD failed to ack.")
        except Exception as ex:
            print "Exception on write [ {0:X}, {1:X}, {2} ]".format(address, size, data)
            print ex

    
if __name__ == "__main__":
    pf = PacketFactory()
    #print pf.read_mem(0xa0a00000,0x48)
    f = open('test.bin', 'rb')
    buf = f.read()
    #print pf.write_mem(0xa0a00100, len(buf), buf.encode("hex"))
    #print pf.query("QStartNoAckMode")
    f.close()
    
    #print "bif>", buf
    
    oocd_client = OpenOcdClient('127.0.0.1', 3333)
    oocd_client.connect()
    oocd_client.no_ack()
    
    #print oocd_client.read(0x10002000, 20)
    #oocd_client.write(0x10002000, len(buf), buf)
    #print oocd_client.read(0x10002000, 200)

    #print oocd_client.read(0xa0a00100, 20)
    #print oocd_client.read(0xa0a00100, 200)

    print oocd_client.read(0x10002000, 0x20)
    print oocd_client.read(0x10002000, 0x200)
    #print oocd_client.read(0x10002200, 0x200)
    #print oocd_client.read(0x10002400, 0x2000)
    print hex(oocd_client.read_word(0xa0a00000))

    #print oocd_client.read(0x10003000, 20)
    oocd_client.write(0x10004000, len(buf), buf)
    #print oocd_client.read(0x10003000, 0x200)

    oocd_client.close()
