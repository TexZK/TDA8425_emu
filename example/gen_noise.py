# -*- coding: utf-8 -*-
from random import gauss
from struct import calcsize
from struct import pack_into

SAMPLE_RATE = 192000  # [Hz]
NUM_CHANNELS = 2
SAMPLE_SCALE = 32767
STRUCT_FORMAT = '<h'
STRUCT_SIZE = calcsize(STRUCT_FORMAT)
BUFFER_LENGTH = SAMPLE_RATE * NUM_CHANNELS

buffer = bytearray(BUFFER_LENGTH * STRUCT_SIZE)
offset = 0
for _ in range(BUFFER_LENGTH):
    value = int(gauss(0, .1) * SAMPLE_SCALE)
    pack_into(STRUCT_FORMAT, buffer, offset, value)
    offset += STRUCT_SIZE

with open('noise.raw', 'wb') as f:
    f.write(buffer)
