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

PLOT_MAGNITUDE = False

#%%

s, z, R1, C1, R2, C2, T, k = symbols('s z R1 C1 R2 C2 T k')

Hs1 = (1 - s*R1*C1) / (1 + s*R1*C1)
Hs2 = (1 - s*R2*C2) / (1 + s*R2*C2)
Hs = Hs1 * Hs2
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

caps = [
    (15e-9, 15e-9),
    (5.6e-9, 47e-9),
    (5.6e-9, 68e-9),
]
Fs = 48000
r1 = 15000
r2 = 15000
dtype = np.float64

f = np.logspace(np.log10(10), np.log10(20000))
fig, ax1 = plt.subplots(figsize=(3.7, 3.6))
plt.title('Pseudo-stereo')
plt.xscale('log')
plt.xlabel('frequency [Hz]')
plt.grid(True, which='both')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-400, 0)
ax1.set_ylabel('phase [°]', color='g')
ax1.grid(True, which='both')

if PLOT_MAGNITUDE:
    ax2 = ax1.twinx()
    ax2.set_ylim(-20, 20)
    ax2.set_ylabel('gain [dB]', color='b')
    ax2.grid(True, which='both')

plt.tight_layout()

#%%

for i, (c1, c2) in enumerate(caps):
    values = {
        'C1': c1,
        'C2': c2,
        'R1': r1,
        'R2': r2,
        'k': 0.5 / Fs,
    }

    ak = np.array([x.subs(values) for x in a], dtype=dtype)
    bk = np.array([x.subs(values) for x in b], dtype=dtype)
    x, y = freqz(bk, a=ak, worN=f, fs=Fs)

    angles = np.unwrap(np.angle(y)) * (180/np.pi)
    ax1.plot(x, angles, label=f'Preset {i+1}')

    if PLOT_MAGNITUDE:
        mags = 20*np.log10(np.abs(y))
        ax2.plot(x, mags)

#%%

if 0:  # PCem
    # Note: A and B coefficients are swapped in PCem source code
    ak = np.array([
        +1.00000000000000000000,
        -1.98733021473466760000,
        +0.98738361004063568000,
    ], dtype=dtype)

    bk = np.array([
        +0.00001409030866231767,
        +0.00002818061732463533,
        +0.00001409030866231767,
    ], dtype=dtype)

    x, y = freqz(bk, a=ak, worN=f, fs=Fs)

    angles = np.unwrap(np.angle(y)) * (180/np.pi)
    ax2.plot(x, angles, label=f'PCem')

    if PLOT_MAGNITUDE:
        mags = 20*np.log10(np.abs(y))
        ax1.plot(x, mags)

#%%

plt.legend()
plt.show()
