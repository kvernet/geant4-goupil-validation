#! /usr/bin/env python3
import endf
import goupil
import numpy
import matplotlib.pyplot as plt


# Load computed Geant4 cross-sections.
geant4 = numpy.loadtxt("share/data/cross-sections-standard.txt", comments="#")
energies = geant4[:,0]

"""
# Load Geant4 input data.
geant4_input = numpy.loadtxt("/home/kinson/software/tools/geant4/10.7.4/share/Geant4-10.7.4/data/G4EMLOW7.13/phot/pe-cs-7.dat")
geant4_input = geant4_input[:-2,:]

# Load EPDL data.
endf = endf.Material('share/data/N.endf')
"""

# Compute cross-sections with Goupil.
geometry = goupil.ExternalGeometry("../geant4-goupil/lib/libgeometry.so")
material = geometry.materials[0]
registry = goupil.MaterialRegistry((material,))
registry.load_elements()
registry.compute()
material = registry["G4_AIR"]

absorption = material.absorption_cross_section()
compton = material.compton_cross_section()
rayleigh = material.rayleigh_cross_section()

goupil = numpy.zeros(geant4.shape)
goupil[:,0] = energies
barn = 1E+24
for i, energy in enumerate(energies):
    goupil[i,1] = absorption(energy) * barn
    goupil[i,2] = compton(energy) * barn
    goupil[i,4] = rayleigh(energy) * barn
    
geant4[:,1] += geant4[:,3]

    
for i in range(1, 5):
    if i == 3: continue
    plt.figure()
    plt.plot(energies, geant4[:,i], "k-")
    plt.plot(energies, goupil[:,i], "r-")
    """
    if i == 1:
        phot = endf[23,522]["sigma"]
        plt.plot(phot.x * 1E-06, phot.y, "b-")
        plt.plot(geant4_input[:,0], geant4_input[:,1], "m.")
    """
    plt.xscale("log")
    plt.yscale("log")
    plt.legend()
    
for i in range(1, 5):
    if i == 3: continue
    plt.figure()
    delta = goupil[:,i] / geant4[:,i] - 1.0
    plt.plot(energies, 100 * delta, "k-")
    plt.xscale("log")
    plt.legend()
    
plt.show()

