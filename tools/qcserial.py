# 
# Serial Utils
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

import serial
from crc import crc16

class QCSerial:
    
    __ser  = None
    __port = None
    __baud = None
    __timeout = None
    
    def __init__(self, port, baud, timeout):
        self.__port = port
        self.__baud = baud
        self.__timeout = timeout
        
    def connect(self):
        if (self.__port is None) or (self.__baud is None) or (self.__timeout is None):
            return -1
        
        try:
            self.__ser = serial.Serial(self.__port, self.__baud, timeout=self.__timeout)
        except serial.serialutil.SerialException:
            return -1
        
        return 0
        
    def close(self):
        if(self.__ser is not None):
            self.__ser.close()
            
    def send(self, buf):
        if(not(self.__ser.isOpen())):
            return -1
            
        self.__ser.write(buf)
        return 0
        
    def recv(self, size=0):
        if(not(self.__ser.isOpen())):
            return ""
            
        reply = ""
            
        if(size == 0):
            reply = self.__ser.read(self.__ser.inWaiting())
        else:
            reply = self.__ser.read(size)
            
        return reply
