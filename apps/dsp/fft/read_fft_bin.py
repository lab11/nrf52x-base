#!/usr/bin/env python3

import serial
import struct
from matplotlib import pyplot as plt

data = []
with serial.Serial('/dev/ttyUSB0', 115200) as s:
    while(1):
        header = s.read(2)
        [x,y] = struct.unpack('BB', header)
        if x == 0xAA and y == 0xBB:
            data = []
            print('found')
            length = s.read(4)
            [length] = struct.unpack('<I', length)
            for x in range(int(length/4)):
                [y] = struct.unpack('<f', s.read(4))
                data.append(y)
            print("done")
            break

plt.plot([48E3 / 1024 * x for x in range(int(len(data)/2))], data[:int(len(data)/2)])
plt.show()
