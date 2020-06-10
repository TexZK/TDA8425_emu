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
from sympy import sqrt
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

MODE = {
    'dc_removal',
    'bass',
    'treble',
}

PLOT_PHASE = False

#%%

s, z, wd, wb, wt, gb, gt, k = symbols('s z wd wb wt gb gt k')
Hs = 1

if 'dc_removal' in MODE:
    Hs *= s / (s + wd)

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


f = np.logspace(np.log10(20), np.log10(20000), 100)
plt.close('all')
fig, ax1 = plt.subplots(figsize=(7, 2.8))
plt.title('Tone control')
plt.xscale('log')
plt.xlabel('frequency [Hz]')

ax1.set_xlim(10**1, 10**5)
ax1.set_ylim(-20, 20)
ax1.set_ylabel('gain [dB]', color='b')
ax1.grid(True, which='both')

if PLOT_PHASE:
    ax2 = ax1.twinx()
    ax2.set_ylim(-90, 90)
    ax2.set_ylabel('phase [°]', color='g')

#%%

for i in range(16):
    values = {
        'k': 0.5 / Fs,
        'gb': 10**(dbb[i] / 20),
        'gt': 10**(dbt[i] / 20),
        'wd': 2*np.pi * fd,
        'wb': 2*np.pi * fb,
        'wt': 2*np.pi * ft,
    }

    ak = np.array([x.subs(values) for x in a], dtype=dtype)
    bk = np.array([x.subs(values) for x in b], dtype=dtype)
    x, y = freqz(bk, a=ak, worN=f, fs=Fs)

    mags = 20*np.log10(np.abs(y))
    ax1.plot(x, mags, 'b')

    if PLOT_PHASE:
        angles = np.unwrap(np.angle(y)) * (180/np.pi)
        ax2.plot(x, angles, 'g')

#%% PCem
if 0:
    bass_attenuation = [
            1.995,
            1.995,
            1.995,
            1.413,
            1.000,
            0.708,
            0.000,
            0.708,
            1.000,
            1.413,
            1.995,
            2.819,
            2.819,
            2.819,
            2.819,
            2.819,
    ]

    bass_cut = [
            0.126,
            0.126,
            0.126,
            0.178,
            0.251,
            0.354,
    ]

    treble_attenuation = [
            1.995,
            1.995,
            1.995,
            1.413,
            1.000,
            0.708,
            0.000,
            0.708,
            1.000,
            1.413,
            1.995,
            1.995,
            1.995,
            1.995,
            1.995,
            1.995,
    ]

    treble_cut = [
            0.126,
            0.126,
            0.126,
            0.178,
            0.251,
            0.354,
    ]

    # Note: A and B coefficients are swapped in PCem source code

    lowpass_a = np.array([
        +1.00000000000000000000,
        -1.97223372919526560000,
        +0.97261396931306277000,
    ])

    lowpass_b = np.array([
        +0.00009159473951071446,
        +0.00018318947902142891,
        +0.00009159473951071446,
    ])

    highpass_a = np.array([
        +1.00000000000000000000,
        -1.97223372919758360000,
        +0.97261396931534050000,
    ])

    highpass_b = np.array([
        +0.98657437157334349000,
        -1.97314874314668700000,
        +0.98657437157334349000,
    ])

    # {{{
    # temp = ((int32_t)adgold_buffer[c] * adgold->vol_l) >> 17;
    # lowpass = adgold_lowpass_iir(0, temp);
    # highpass = adgold_highpass_iir(0, temp);
    # if (adgold->bass > 6)
    #         temp += (lowpass * bass_attenuation[adgold->bass]) >> 14;
    # else if (adgold->bass < 6)
    #         temp = highpass + ((temp * bass_cut[adgold->bass]) >> 14);
    # if (adgold->treble > 6)
    #         temp += (highpass * treble_attenuation[adgold->treble]) >> 14;
    # else if (adgold->treble < 6)
    #         temp = lowpass + ((temp * treble_cut[adgold->treble]) >> 14);
    # }}}

    for mode in MODE:
        for i in range(16):
            ak = 1.0
            bk = 1.0

            if mode == 'bass':
                if i > 6:
                    ak *= np.array(lowpass_a)
                    bk *= np.array(lowpass_b)
                elif i < 6:
                    ak *= np.array(highpass_a)
                    bk *= np.array(highpass_b)

            elif mode == 'treble':
                if i > 6:
                    ak *= np.array(highpass_a)
                    bk *= np.array(highpass_b)
                elif i < 6:
                    ak *= np.array(lowpass_a)
                    bk *= np.array(lowpass_b)

            ak = np.array(ak, dtype=dtype)
            bk = np.array(bk, dtype=dtype)
            x, y = freqz(bk, a=ak, worN=f, fs=Fs)

            if mode == 'bass':
                if i > 6:
                    # H(z) = LPF(z)*att + 1 = (bk*att + ak)/ak
                    bk = bk*bass_attenuation[i] + ak
                elif i < 6:
                    # H(z) = HPF(z) + cut = (bk + ak*cut)/ak
                    bk = bk + ak*bass_cut[i]

            elif mode == 'treble':
                if i > 6:
                    # H(z) = HPF(z)*att + 1 = (bk*att + ak)/ak
                    bk = bk*treble_attenuation[i] + ak
                elif i < 6:
                    # H(z) = LPF(z) + cut = (bk + ak*cut)/ak
                    bk = bk + ak*treble_cut[i]

            x, y = freqz(bk, a=ak, worN=f, fs=Fs)
            mags = np.abs(y)

            mags *= 0.5
            mags = 20*np.log10(mags)
            ax1.plot(x, mags, 'r')

            if PLOT_PHASE:
                angles = np.unwrap(np.angle(y)) * (180/np.pi)
                ax2.plot(x, angles, 'y')

#%%

plt.show()
