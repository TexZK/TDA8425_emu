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
fig, ax1 = plt.subplots()
plt.title(f'Pseudo-stereo: R1 {r1} Ω & R2 {r2} Ω')
plt.xscale('log')
plt.xlabel('frequency [Hz]')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-20, 20)
ax1.set_ylabel('gain [dB]', color='b')
ax1.grid(True, which='both')

ax2 = ax1.twinx()
ax2.set_ylim(-400, 0)
ax2.set_ylabel('phase [°]', color='g')
ax2.grid(True, which='both')

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

    # mags = 20*np.log10(np.abs(y))
    # ax1.plot(x, mags)

    angles = np.unwrap(np.angle(y)) * (180/np.pi)
    ax2.plot(x, angles, label=f'{i+1}')

plt.legend()
plt.show()
