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
from sympy import Abs
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

MODE = {
    'dc_removal',
    'resonance',
    'bass',
    'treble',
}

PLOT_PHASE = False

#%%

s, z, gb, gt, k, wd, wr, wb, wt = symbols('s z gb gt k wd wr wb wt')

gg = log10(sqrt(gb))
r = sqrt(1 / Abs(20 * gg))
gn = gg * 0.85
ph = pi * 3/4
p1 = r*wr * (cos(ph - gn) + sin(ph - gn)*1j)
p2 = r*wr * (cos(ph - gn) - sin(ph - gn)*1j)
z1 = r*wr * (cos(ph + gn) + sin(ph + gn)*1j)
z2 = r*wr * (cos(ph + gn) - sin(ph + gn)*1j)

Hs = 1

if 'dc_removal' in MODE:
    Hs *= s / (s + wd)

if 'resonance' in MODE:
    Hs *= ((s - z1) * (s - z2)) / ((s - p1) * (s - p2))

if 'bass' in MODE:
    w1 = wb / sqrt(gb)
    w2 = wb * sqrt(gb)
    Hs *= (s + w2) / (s + w1)

if 'treble' in MODE:
    w1 = wt / sqrt(gt)
    w2 = wt * sqrt(gt)
    Hs *= w2/w1 * (s + w1) / (s + w2)

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

dtype = np.float64
Fs = 48000  # [Hz]

fd = 10  # [Hz]

fr = 180  # [Hz]
fb = 300  # [Hz]
dbb = [
    -12,
    -12,
    -12,
    - 9,
    - 6,
    - 3,
      0,
    + 3,
    + 6,
    + 9,
    +12,
    +15,
    +15,
    +15,
    +15,
    +15,
]

ft = 4500  # [Hz]
dbt = [
    -12,
    -12,
    -12,
    - 9,
    - 6,
    - 3,
      0,
    + 3,
    + 6,
    + 9,
    +12,
    +12,
    +12,
    +12,
    +12,
    +12,
]

f = np.logspace(np.log10(20), np.log10(20000), 1000)
plt.close('all')

#%%

plt.figure(figsize=(6, 6))
plt.xlabel('Re [rad/s]')
plt.ylabel('Im [rad/s]')
plt.grid(True)
plt.plot([-1000, +1000], [+1000, -1000], color='k')
plt.plot([-1000, +1000], [-1000, +1000], color='k')

for i in range(16):
    values = {
        'wd': 2*np.pi * fd,
        'wr': 2*np.pi * fr,
        'wb': 2*np.pi * fb,
        'wt': 2*np.pi * ft,
        'k': 0.5 / Fs,
        'gb': 10**(dbb[i] / 20),
        'gt': 10**(dbt[i] / 20),
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

fig, ax1 = plt.subplots(figsize=(6.5, 3.25))
plt.title('T-filter')
plt.xscale('log')
plt.xlabel('frequency [Hz]')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-20, 20)
ax1.set_ylabel('gain [dB]', color='b')
ax1.grid(True, which='both')

if PLOT_PHASE:
    ax2 = ax1.twinx()
    ax2.set_ylim(-60, 60)
    ax2.set_ylabel('phase [°]', color='g')

plt.tight_layout()

for i in range(16):
    values = {
        'wd': 2*np.pi * fd,
        'wr': 2*np.pi * fr,
        'wb': 2*np.pi * fb,
        'wt': 2*np.pi * ft,
        'k': 0.5 / Fs,
        'gb': 10**(dbb[i] / 20),
        'gt': 10**(dbt[i] / 20),
    }

    ak = np.array([x.subs(values) for x in a], dtype=dtype)
    bk = np.array([x.subs(values) for x in b], dtype=dtype)
    x, y = freqz(bk, a=ak, worN=f, fs=Fs)

    mags = 20*np.log10(np.abs(y))
    ax1.plot(x, mags, 'b')

    if PLOT_PHASE:
        angles = np.unwrap(np.angle(y)) * (180/np.pi)
        ax2.plot(x, angles, 'g')

#%%

plt.show()
