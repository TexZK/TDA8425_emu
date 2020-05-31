# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
# import sounddevice as sd
# import soundfile as sf
from IPython.display import display
from sympy import *


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

Fs = 48000
D = 3  # [s]
dtype = np.float64
# dtype = np.float32

# x, Fs = sf.read(r'E:\Downloads\doom.wav', dtype=stype.__name__)
# x = x[:Fs*D]


values = {
    C1: 15e-9,
#    C1: 5.6e-9,
#    C1: 5.6e-9,
    R1: 13e3,

    C2: 15e-9,
#    C2: 47e-9,
#    C2: 68e-9,
    R2: 13e3,

    T: 1 / Fs,
    k: 0.5 / Fs,
}

f1 = 1/(2*pi*values[R1]*values[C1])
f2 = 1/(2*pi*values[R2]*values[C2])

t = np.linspace(0, D, D*Fs, dtype=dtype)
f = dtype(dtype(f1)*.1)
x = np.sin((2*np.pi*f)*t, dtype=dtype) / f**.5

xz, yz, z = symbols('x y z')

yz = 1/(z**0 * a[0]) * (yz * z**-1 * -a[1] +
                        yz * z**-2 * -a[2] +
                        xz * z**-0 *  b[0] +
                        xz * z**-1 *  b[1] +
                        xz * z**-2 *  b[2])
spprint('y(z):', yz)

yzv = yz.subs(values)
spprint('y(z):', yzv)


#%%

av = np.array([dtype(v.subs(values)) for v in a], dtype=dtype)
bv = np.array([dtype(v.subs(values)) for v in b], dtype=dtype)
c = dtype(1/av[0]) * np.array([-av[1], -av[2], bv[0], bv[1], bv[2]], dtype=dtype)

dtype = np.float32
c = np.array(c, dtype=dtype)
c0, c1, c2, c3, c4 = c

t = np.linspace(0, len(x), len(x), dtype=dtype)

y = np.zeros_like(x, dtype=dtype)

for ti in range(2, len(t)):
    y[ti] = (y[ti-1]*c0 + y[ti-2]*c1) + (x[ti]*c2 + x[ti-1]*c3 + x[ti-2]*c4)

#%%

plt.figure()
plt.grid(1)
plt.plot(t, x)
plt.plot(t, y)

# plt.plot(t, y2)
# plt.grid(1); plt.plot(t, np.log2(np.abs(y2-y)))


#%%

# x_y = np.empty((x.size + y.size,), dtype=x.dtype)
# x_y[0::2] = x
# x_y[1::2] = y
# x_y = np.vstack((x, x)).T
x_y = np.vstack((x, y)).T

# sd.play(x_y, samplerate=Fs, loop=False, blocking=False)
# sd.stop()
