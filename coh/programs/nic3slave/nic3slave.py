#!/usr/bin/python

#******************************************************************************
#* Copyright (C) 2008-2016  Michael Hofmann <mh21@mh21.de>                    *
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

from __future__ import with_statement

import protoslave_pb2
import protocoh_pb2

import cochlear.nic3

import os
import socket
import struct
import sys
import contextlib

try:
    import multiprocessing
    import signal

    def watchdogProcess(pid, sem):
        while 1:
            sem.acquire()
            if not sem.acquire(timeout=5): #5 second timeout
                print('Watchdog triggered')
                os.kill(pid, signal.SIGTERM)
                sys.exit(0)

    @contextlib.contextmanager
    def watchdog(sem):
        sem.release()
        yield
        sem.release()

    if __name__ == '__main__':
        semaphore = multiprocessing.Semaphore(0)
        multiprocessing.Process(target=watchdogProcess, args=(os.getpid(), semaphore)).start()
        print('watchdog online')
except Exception, e:
    semaphore = ""
    @contextlib.contextmanager
    def watchdog(sem):
        yield
    print('watchdog offline: ' + str(e))

if __name__ == '__main__':
    # acknowledge Codacs warning automatically
    import StringIO
    oldstdin = sys.stdin
    sys.stdin = StringIO.StringIO('\10')
    extendedamplituderange = 'extendedamplituderange' in sys.argv[1:]
    flexibleframeperiods = 'flexibleframeperiods' in sys.argv[1:]
    allowimplantcommands = 'allowimplantcommands' in sys.argv[1:]
    cmdlinefile = 'cmdlinefile' in sys.argv[1:]
    codacsperiod = 51 * 5
    try:
        import cochlear.nic3.AcousticStimulus
        if extendedamplituderange:
            cochlear.nic3.AcousticStimulus.allowExtendedAmplitudeRange()
        acousticsupported = True
        print('AcousticStimulus support: yes')
    except Exception, e:
        acousticsupported = False
        print('AcousticStimulus support: no')
    sys.stdin = oldstdin

class ProtoSlaveClient:
    def __init__(self, address):
        p = cochlear.nic3.Properties()
        for part in address.split(','):
            key, value = part.split('=', 1)
            p.add(key, value)
        if allowimplantcommands:
            p.add('allow_implant_commands_password', 'spicy_tuna_hand_roll')
        self.streamer = cochlear.nic3.Streamer(p)
        self.stimulustype = 'unknown'

    def destroy(self):
        self.stop()
        del self.streamer

    def send(self, sequence):
        command = protocoh_pb2.ProtoSlaveCommand()
        command.ParseFromString(sequence)
        self.root = None
        self.parent = None
        self.parse(command)
        self.streamer.sendData(self.root)

    def stimtype(self, what):
        if self.stimulustype == what:
            return
        if self.stimulustype == 'unknown':
            self.stimulustype = what
            return
        raise Exception('Mixed stimuli')

    def parse(self, command):
        if command.HasField('metadata'):
            pass
        elif command.HasField('null'):
            self.stimtype('electric')
            stimulus = command.stimulus
            null = command.null
            nicstimulus = cochlear.nic3.NullStimulus(
                null.period_cycles / 5.0)
            if stimulus.trigger:
                niccommand = cochlear.nic3.TriggerCommand(nicstimulus)
            else:
                niccommand = cochlear.nic3.StimulusCommand(nicstimulus)
            self.parent.append(niccommand)
        elif command.HasField('biphasic'):
            self.stimtype('electric')
            stimulus = command.stimulus
            biphasic = command.biphasic
            nicstimulus = cochlear.nic3.BiphasicStimulus(
                biphasic.active_electrode,
                biphasic.reference_electrode,
                biphasic.current_level,
                biphasic.phase_width_cycles / 5.0,
                biphasic.phase_gap_cycles / 5.0,
                biphasic.period_cycles / 5.0)
            if stimulus.trigger:
                niccommand = cochlear.nic3.TriggerCommand(nicstimulus)
            else:
                niccommand = cochlear.nic3.StimulusCommand(nicstimulus)
            self.parent.append(niccommand)
        elif command.HasField('codacs'):
            self.stimtype('codacs')
            stimulus = command.stimulus
            codacs = command.codacs
            if not flexibleframeperiods and codacs.period_cycles != codacsperiod:
                raise Exception('Wrong frame period')
            if acousticsupported:
                cochlear.nic3.AcousticStimulus._Tc_us = codacs.period_cycles / 5.0
                nicstimulus = cochlear.nic3.AcousticStimulus.AcousticStimulus(
                    codacs.amplitude,
                    False)
                cochlear.nic3.AcousticStimulus._Tc_us = codacsperiod / 5.0
            else:
                basicrange = codacs.amplitude >= -128 and codacs.amplitude <= 127
                if not extendedamplituderange and not basicrange:
                    raise Exception('Stimulus exceeds range')
                nicstimulus = cochlear.nic3.BiphasicStimulus(
                    (codacs.amplitude & 0xf80) >> 7,
                    -3,
                    (codacs.amplitude & 0x07f) << 1,
                    12.0,
                    6.0,
                    codacs.period_cycles / 5.0)
            if stimulus.trigger:
                niccommand = cochlear.nic3.TriggerCommand(nicstimulus)
            else:
                niccommand = cochlear.nic3.StimulusCommand(nicstimulus)
            self.parent.append(niccommand)
        elif command.HasField('sequence'):
            sequence = command.sequence
            nicsequence = cochlear.nic3.Sequence(sequence.number_repeats)
            originalparent = self.parent
            if self.root is None:
                self.root = nicsequence
            for command in sequence.command:
                self.parent = nicsequence
                self.parse(command)
            if originalparent is not None:
                originalparent.append(nicsequence)
        else:
            raise Exception('Unknown pulse type')

    def start(self, trigger):
        # check disabled for now to make RFGenXS work
        if trigger == 'Immediate' or True:
            self.streamer.start()
        else:
            raise Exception('Unsupported trigger type')

    def stop(self):
        self.streamer.stop()

    def status(self):
        return {
            cochlear.nic3.status.ABORTED: 'Error',
            cochlear.nic3.status.NIL: 'Idle',
            cochlear.nic3.status.NOT_STREAMING: 'Idle',
            cochlear.nic3.status.STREAMING_FINISHED: 'Stopped',
            cochlear.nic3.status.STREAMING_PUFS: 'Streaming',
            cochlear.nic3.status.STREAMING_USER_DATA: 'Streaming',
            cochlear.nic3.status.UNDERRUN: 'Underflow',
            cochlear.nic3.status.USER_DATA_ENDED: 'Streaming',
            cochlear.nic3.status.WAITING_FOR_SYNC: 'Waiting',
        }.get(self.streamer.streamStatus(), 'Unknown')

class ProtoSlaveCommands:
    def __init__(self):
        self.deviceindex = 0
        self.devices = {}

    def create(self, address):
        self.deviceindex += 1
        self.devices[str(self.deviceindex)] = ProtoSlaveClient(address)
        return str(self.deviceindex)

    def destroy(self, device):
        with watchdog(semaphore):
            self.devices.pop(self.check(device)).destroy()

    def send(self, device, sequence):
        self.devices[self.check(device)].send(sequence)

    def start(self, device, trigger):
        self.devices[self.check(device)].start(trigger)

    def stop(self, device):
        with watchdog(semaphore):
            self.devices[self.check(device)].stop()

    def status(self, device):
        with watchdog(semaphore):
            return self.devices[self.check(device)].status()

    def needsReloadForStop(self, device):
        return 'false'

    def check(self, device):
        if device not in self.devices:
            raise Exception('Device %s not found' % device)
        return device

if __name__ == '__main__':
    if cmdlinefile:
        f = open(sys.argv[-1], 'r+b', 0)
        data = f.read()
        client = ProtoSlaveClient(sys.argv[-2])
        client.send(data)
        client.start('Immediate')
        client.streamer.waitUntilFinished()
        client.stop()
        sys.exit(0)

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

        reply = protoslave_pb2.ProtoSlaveReply()
        try:
            result = getattr(commands, request.function)(*[str(s) for s in request.parameter])
            if request.output:
                reply.result = result
        except Exception, e:
            reply.error = str(e)

        datagram = reply.SerializeToString()
        send(struct.pack('<I', len(datagram)) + datagram)
