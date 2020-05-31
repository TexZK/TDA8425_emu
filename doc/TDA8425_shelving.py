# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
from IPython.display import display
from sympy import init_printing
from sympy import symbols
from sympy import simplify
from sympy import collect
from sympy import expand
from sympy import numer
from sympy import denom
from sympy import Poly
from sympy import pi
from sympy import tan


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

s, z, w, g, T, k = symbols('s z w g T k')

w1 = w * g
w2 = w / g

Hs = (s + w2) / (s + w1)
spprint('H(s):', Hs)

# k = T / 2
# k = tan(w * T / 2)
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
