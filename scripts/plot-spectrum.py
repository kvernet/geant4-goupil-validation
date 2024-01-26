#!/usr/bin/env python3
import os
import argparse
import numpy
import matplotlib.pyplot as plt
import pickle

import sys
sys.path.append("../geant4-goupil/scripts")
from plot import Histogramed

plt.style.use("scripts/paper.mplstyle")


ACTIVITY = 1E+04    # in Bq
EDGES = numpy.logspace(-2, 1, 61)

source_energies = numpy.array(
    (0.242, 0.295, 0.352, 0.609, 0.768, 0.934, 1.120, 1.238, 1.378, 1.764, 2.204),
    dtype = "f8"
)

HeaderType = numpy.dtype(
    [
        ("model", "S32"),
        ("energy", "f8"),
        ("requested", "u8"),
        ("events", "u8"),
    ],
    align=True
)

StateType = numpy.dtype(
    [
        ("energy", "f8"),
        ("position", "3f8"),
        ("direction", "3f8")
    ],
    align=True
)
EventType = numpy.dtype(
    [
        ("eventid", "i4"),
        ("tid", "i4"),
        ("parent", "i4"),
        ("pid", "i4"),
        ("primary", StateType),
        ("detected", StateType),
        ("creator", "S8")
    ],
    align=True
)


def getData(paths):
    events = 0
    data = []
    for path in paths:
        with open(path) as f:
            header = numpy.fromfile(f, dtype=HeaderType, count=1)[0]
            if len(data) == 0:
                data = numpy.fromfile(f, dtype=EventType)
            else:
                data = numpy.concatenate(
                        (data, numpy.fromfile(f, dtype=EventType)),
                        axis=0
                )
        
        events += header["events"]
    
    for source_energy in source_energies:
        sel = data["detected"]["energy"] == source_energy
        data["primary"]["energy"][sel] = source_energy
    
    sel0 = (data["primary"]["energy"] <= 0.0) & (data["tid"] == 1)
    sel1 = (data["primary"]["energy"] > 0.0) & (data["tid"] == 1)
    sel2 = data["tid"] > 1
    
    p0 = len(data[sel0]) * ACTIVITY / events
    p1 = len(data[sel1]) * ACTIVITY / events
    p2 = len(data[sel2]) * ACTIVITY / events
        
    print(f"direct rate:    {p0:.3E} Hz")
    print(f"secondary rate: {p1:.3E} Hz")   
    print(f"indirect rate:  {p2:.3E} Hz")

    header["events"] = events
    
    return header, data


def histogram(energies, events, **kwargs):
    n, edges = numpy.histogram(energies, bins=EDGES)
    center = numpy.sqrt(edges[1:] * edges[:-1])
    width = edges[1:] - edges[:-1]
    xerr = [center - edges[:-1], edges[1:] - center]
    nrm = ACTIVITY / (width*events) 
    p = n * nrm
    dp = numpy.sqrt(n) * nrm
    
    plt.errorbar(x=center, y=p, xerr=xerr, yerr=dp, **kwargs)
    
    return Histogramed(center, p, xerr, dp)
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Plot simulation data.")
    
    parser.add_argument("-f",
        dest="files",
        help="input files",
        nargs="+")
    
    args = parser.parse_args()
    
    os.makedirs("share/plots", exist_ok=True)
    
    header, data = getData(args.files)
    
    sel0 = (data["primary"]["energy"] <= 0.0) & (data["tid"] == 1)
    sel1 = data["tid"] > 1
    print(f"primaries = {sum(sel0)}")
    print(f"secondaries = {sum(sel1)}")
    
    def histogram_energy(sel):
        events = header["events"]
        n, edges = numpy.histogram(data["detected"]["energy"][sel], bins=EDGES)
        center = numpy.sqrt(edges[1:] * edges[:-1])
        width = edges[1:] - edges[:-1]
        xerr = [center - edges[:-1], edges[1:] - center]
        nrm = ACTIVITY / (width*events) 
        p = n * nrm
        dp = numpy.sqrt(n) * nrm
        return Histogramed(center, p, xerr, dp)
        
    primaries_energy = histogram_energy(sel0)
    secondaries_energy = histogram_energy(sel1)
    
    def histogram_distance(sel):
        distance = numpy.linalg.norm(
            data["detected"]["position"][sel] - data["primary"]["position"][sel],
            axis = 1
        )

        hist, center = numpy.histogram(distance, bins=numpy.linspace(0.0, 1e5, 101))
        x = 0.5 * (center[1:] + center[:-1])
        width = center[1] - center[0]
        xerr = 0.5 * width
        y = ACTIVITY * hist / (header["events"] * width)
        yerr = ACTIVITY * numpy.sqrt(hist) / (header["events"] * width)
        return Histogramed(x, y, xerr, yerr)
        
    primaries_distance = histogram_distance(sel0)
    secondaries_distance = histogram_distance(sel1)
    
    def histogram_cos_theta(sel): 
        cos_theta = numpy.sum(data["detected"]["direction"][sel] * data["primary"]["direction"][sel], axis=1)  
        hist, center = numpy.histogram(cos_theta, bins=numpy.linspace(-1.0, 1.0, 41))
        x = 0.5 * (center[1:] + center[:-1])
        width = center[1] - center[0]
        xerr = 0.5 * width
        y = ACTIVITY * hist / (header["events"] * width)
        yerr = ACTIVITY * numpy.sqrt(hist) / (header["events"] * width)
        return Histogramed(x, y, xerr, yerr)
        
    primaries_cos_theta = histogram_cos_theta(sel0)
    secondaries_cos_theta = histogram_cos_theta(sel1)
    
    os.makedirs("plots", exist_ok=True)
    
    plt.figure(figsize=(12, 7))
    primaries_energy.errorbar(fmt="ko", label="Primaries")
    secondaries_energy.errorbar(fmt="ro", label="Secondaries") 
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("energy [MeV]")
    plt.ylabel("rate [Hz/MeV]")
    plt.legend()
    plt.savefig("plots/energy-absolute.pdf")
    
    plt.figure(figsize=(12, 7))
    primaries_distance.errorbar(fmt="ko", label="Primaries")
    secondaries_distance.errorbar(fmt="ro", label="Secondaries") 
    plt.yscale("log")
    plt.xlabel("distance [cm]")
    plt.legend()
    
    plt.figure(figsize=(12, 7))
    primaries_cos_theta.errorbar(fmt="ko", label="Primaries")
    secondaries_cos_theta.errorbar(fmt="ro", label="Secondaries")  
    plt.xlabel(r"$\cos\theta$")
    plt.ylabel("rate [Hz]")
    plt.legend()
    plt.savefig("plots/angle-absolute.pdf")
    
    intensities = numpy.empty((source_energies.size, 2))
    for i, source_energy in enumerate(source_energies):
        sel = data["detected"]["energy"] == source_energy
        n = sum(sel)
        intensities[i,0] = n * ACTIVITY / header["events"]
        intensities[i,1] = numpy.sqrt(n) * ACTIVITY / header["events"]
        
    plt.figure(figsize=(12, 7))
    plt.errorbar(source_energies, intensities[:,0], yerr=intensities[:,1], fmt="ko")
    plt.xlabel(r"energy [MeV]")
    plt.ylabel("rate [Hz]")
    
    plt.show()
