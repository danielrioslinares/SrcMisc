#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import math

"""LaTeX integration"""
plt.rc('font', **{'family': 'serif', 'serif': ['Computer Modern']}, size = '16')
plt.rc('text', usetex=True)

##########################[ INSERT CODE HERE ]##########################

def get_powerSIM(D):
	voltage = D;
	if D < 100:
		current = 1
	else:
		current = 1-(voltage-100)/155
	return [voltage,current,voltage*current]

D = np.linspace(0,255,255)
V = []
I = []
P = []


for d in D:
	V.append(get_powerSIM(d)[0])
	I.append(get_powerSIM(d)[1])
	P.append(get_powerSIM(d)[2])

plt.plot(D,I)
plt.twinx().plot(D,P)
plt.show()




