#!/usr/bin/python

#******************************************************************************
#* Copyright (C) 2008-2013  Michael Hofmann <mh21@mh21.de>                    *
#*                                                                            *
#* This program is free software; you can redistribute it and/or modify       *
#* it under the terms of the GNU General Public License as published by       *
#* the Free Software Foundation; either version 3 of the License, or          *
#* (at your option) any later version.                                        *
#*                                                                            *
#* This program is distributed in the hope that it will be useful,            *
#* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
#* GNU General Public License for more details.                               *
#*                                                                            *
#* You should have received a copy of the GNU General Public License along    *
#* with this program; if not, write to the Free Software Foundation, Inc.,    *
#* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
#******************************************************************************

import protoslave_pb2

import os
import socket
import struct
import sys

class ProtoSlaveCommands:
    def ProtoSlaveCommands(self):
        pass

    def initialize(self):
        pass

    def connect(self):
        pass

    def disconnect(self):
        pass

    def transducers(self):
        return 'transducer'

    def setTransducer(self, name):
        pass

    def instruments(self):
        return 'instrument'

    def setInstrument(self, instrument):
        pass

    def startMeasurement(self, measurementtype, fweighting, tweighting, percentile):
        pass

    def stopMeasurement(self):
        pass

    def measure0(self):
        pass

    def measure3(self, measurementtype, fweighting, tweighting):
        pass

    def measure1(self, what):
        pass

    def result(self):
        return str(64.90)

commands = ProtoSlaveCommands()

if os.name == 'nt':
    f = open(r'\\.\pipe\%s' % sys.argv[1], 'r+b', 0)

    def send(d):
        f.write(d)
        f.seek(0)

    def recv(size):
        data = f.read(size)
        f.seek(0)
        return data

else:
    f = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    f.connect("/tmp/%s" % sys.argv[1])

    def send(d):
        f.send(d)

    def recv(size):
        data = f.recv(size)
        if len(data) == 0:
            sys.exit(0)
        return data

data = ''
while 1:
    if len(data) < 4:
        data = data + recv(4 - len(data))
        continue

    size, = struct.unpack_from('<I', data)
    if size > sys.maxint:
        raise Exception('Command packet is too big')

    data = data + recv(size + 4 - len(data))

    if len(data) < size + 4:
        continue

    request = protoslave_pb2.ProtoSlaveRequest()
    request.ParseFromString(data[4:])

    data = data[size + 4:]

    print 'Received request datagram:'
    print '  Name: %s' % request.function
    print '  Parameters: %s' % str([str(s) for s in request.parameter])
    print '  Output: %i' % request.output

    reply = protoslave_pb2.ProtoSlaveReply()
    try:
        result = getattr(commands, request.function)(*[str(s) for s in request.parameter])
        if request.output:
            reply.result = result
    except Exception, e:
        reply.error = str(e)

    print 'Sending reply datagram:'
    print '  Result: %s' % reply.result
    print '  Error: %s' % reply.error

    datagram = reply.SerializeToString()
    send(struct.pack('<I', len(datagram)) + datagram)
