import struct, sys
from cochlear.nic3 import *

sequence = Sequence(1)

psequence = Sequence(60000)
psequence.append(StimulusCommand(BiphasicStimulus(0, -3, 0, 12.0, 6.0, 51.0)))
sequence.append(psequence)

ssequence = Sequence(int(sys.argv[1]))
f = open(sys.argv[2], 'rb')
for data in iter(lambda: f.read(4), ''):
    (amplitude, trigger) = struct.unpack('<hh', data)
    if amplitude < 0:
        amplitude = amplitude + 4096
    cl = (amplitude & 127) << 1
    el = amplitude >> 7
    stim = BiphasicStimulus(el, -3, cl, 12.0, 6.0, 51.0)
    command = TriggerCommand(stim) if trigger else StimulusCommand(stim)
    ssequence.append(command)
f.close()
sequence.append(ssequence)

p = Properties()
p.add("platform", "L34")
p.add("implant", "CIC4")
p.add("min_pulse_width_us", "12.0")
p.add("go_live", "on")
p.add("auto_pufs", "off")
p.add("allow_implant_commands_password", "spicy_tuna_hand_roll")

streamer = Streamer(p)
streamer.start()
streamer.sendData(sequence)
streamer.waitUntilFinished()
streamer.stop()
