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
from sympy import sqrt
from sympy.codegen.cfunctions import log10
from sympy import Abs as sabs
from sympy import cos
from sympy import sin
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

s, z, w, g, k, wb, wt = symbols('s z w g k wb wt')

# D = sqrt(2)
# M = 0.8
# p1 = -w/g*D + 1j*w/g*D - 1j*w*(1/g-1)*D*M
# p2 = -w/g*D - 1j*w/g*D + 1j*w*(1/g-1)*D*M
# z1 = -w*g*D + 1j*w*g*D - 1j*w*(g-1)*D*M
# z2 = -w*g*D - 1j*w*g*D + 1j*w*(g-1)*D*M

r = sqrt(1/sabs(20*log10(g)))
gn = log10(g)*0.85
ph = pi*(3/4)
p1 = r*w * (cos(ph - gn) + 1j*sin(ph - gn))
p2 = r*w * (cos(ph - gn) - 1j*sin(ph - gn))
z1 = r*w * (cos(ph + gn) + 1j*sin(ph + gn))
z2 = r*w * (cos(ph + gn) - 1j*sin(ph + gn))

Hs = 1
Hs = Hs * ((s - z1) * (s - z2)) / ((s - p1) * (s - p2))  # resonance
# Hs = Hs * s / (s + 2*pi*12)  # DC removal
# Hs = Hs * (s + wb*g) / (s + wb/g)  # basic bass
# Hs = Hs * g*g * (s + wt/g) / (s + wt*g)  # basic treble
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

dbs = list(range(-12, 15 + 1, 3))
Fs = 48000
Fc = 180
dtype = np.float64

plt.close('all')
plt.xlabel('Re')
plt.xlabel('Im')
plt.grid(True)
plt.plot([-1000, 0], [+1000, 0], color='k')
plt.plot([-1000, 0], [-1000, 0], color='k')

for db in dbs:
    values = {
        'w': 2*np.pi * Fc,
        'wb': 2*np.pi * 300,
        'wt': 2*np.pi * 4500,
        'k': 0.5 / Fs,
        'g': 10**(db / (20 * 4)),
    }

    pc1 = complex(p1.subs(values))
    pc2 = complex(p2.subs(values))
    zc1 = complex(z1.subs(values))
    zc2 = complex(z2.subs(values))

    plt.scatter([pc1.real], [pc1.imag], marker='x', color='b')
    plt.scatter([pc2.real], [pc2.imag], marker='x', color='b')
    plt.scatter([zc1.real], [zc1.imag], marker='o', color='r')
    plt.scatter([zc2.real], [zc2.imag], marker='o', color='r')

plt.show()

#%%

f = np.logspace(np.log10(20), np.log10(20000), 1000)
# plt.close('all')
fig, ax1 = plt.subplots()
plt.xscale('log')
plt.xlabel('frequency [Hz]')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-20, 20)
ax1.set_ylabel('gain [dB]', color='b')
ax1.grid(True, which='both')

ax2 = ax1.twinx()
ax2.set_ylabel('phase [°]', color='g')

for db in dbs:
    values = {
        'w': 2*np.pi * Fc,
        'wb': 2*np.pi * 300,
        'wt': 2*np.pi * 4500,
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
