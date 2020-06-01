# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import numpy as np
from IPython.display import display
from scipy.signal import freqz
from sympy import init_printing
from sympy import symbols
from sympy import simplify
from sympy import collect
from sympy import expand
from sympy import numer
from sympy import denom
from sympy import Poly
from sympy import pi


def spprint(name, symbol, plain=False):
    print('=' * 120)
    print()
    print(name)
    print()
    if plain:
        print(str(symbol))
    else:
        display(symbol)
    print()


init_printing(use_latex='mathjax')


#%%

s, z, w, g, k = symbols('s z w g k')

w1 = w / g
w2 = w * g

Hs = (s + w2) / (s + w1)  # bass
# Hs = w2/w1 * (s + w1) / (s + w2)  # treble
spprint('H(s):', Hs)

Hz = Hs.subs(s, (1/k * (z - 1) / (z + 1)))
spprint('H(z):', Hz)

Hzr = simplify(Hz)
spprint('H(z) simp:', Hzr)

Hzrn = collect(expand(numer(Hzr)), z)
spprint('H(z) numer:', Hzrn)

Hzrd = collect(expand(denom(Hzr)), z)
spprint('H(z) denom:', Hzrd)

a = Poly(Hzrd, z).as_list()
spprint('H(z) denom coeffs:', a, plain=True)
for i, x in enumerate(a):
    print(f'a{i}: {x}')

b = Poly(Hzrn, z).as_list()
spprint('H(z) numer coeffs:', b, plain=True)
for i, x in enumerate(b):
    print(f'b{i}: {x}')

#%%

dbs = list(range(-12, 12 + 1, 3))
Fs = 48000
Fc = 300  # bass
# Fc = 4500  # treble
dtype = np.float64

f = np.logspace(np.log10(20), np.log10(20000))
fig, ax1 = plt.subplots()
plt.xscale('log')
plt.xlabel('frequency [Hz]')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-20, 20)
ax1.set_ylabel('gain [dB]', color='b')
ax1.grid(True, which='both')

ax2 = ax1.twinx()
ax2.set_ylim(-180, 180)
ax2.set_ylabel('angle [°]', color='g')

for db in dbs:
    values = {
        'w': 2*np.pi * Fc,
        'k': 0.5 / Fs,
        'g': np.sqrt(10**(db / 20)),
    }

    ak = np.array([x.subs(values) for x in a], dtype=dtype)
    bk = np.array([x.subs(values) for x in b], dtype=dtype)
    x, y = freqz(bk, a=ak, worN=f, fs=Fs)

    mags = 20*np.log10(np.abs(y))
    ax1.plot(x, mags, 'b')

    # angles = np.unwrap(np.angle(y)) * (180/np.pi)
    # ax2.plot(x, angles, 'g')

plt.show()
