#!/usr/bin/env python3
import os
import argparse
import numpy
import matplotlib.pyplot as plt

import sys
sys.path.append("../geant4-goupil/scripts")
from plot import Processor, Histogramed


ACTIVITY = 1E+04    # in Bq
EDGES = numpy.logspace(-2, 0, 21)

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
    
    sel0 = (data["detected"]["energy"] == data["primary"]["energy"]) & (data["tid"] == 1)
    sel1 = (data["detected"]["energy"] < data["primary"]["energy"]) & (data["tid"] == 1)
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
    
    forward = Processor("../geant4-goupil/share/goupil/forward.1.1.pkl.gz", EDGES, source_index = 1)
    backward = Processor("../geant4-goupil/share/goupil/backward.1.pkl.gz", EDGES, source_index = 1)
    
    sel = (data["detected"]["energy"] < data["primary"]["energy"]) & (data["tid"] == 1)
    print(f"selected = {sum(sel)}")
    
    distance = numpy.linalg.norm(
        data["detected"]["position"][sel] - data["primary"]["position"][sel],
        axis = 1
    )
    hist, center = numpy.histogram(distance, bins=numpy.linspace(0.0, 100.0, 51))
    x = 0.5 * (center[1:] + center[:-1])
    width = center[1] - center[0]
    xerr = 0.5 * width
    y = ACTIVITY * hist / (header["events"] * width)
    yerr = ACTIVITY * numpy.sqrt(hist) / (header["events"] * width)
    geant4_distance = Histogramed(x, y, xerr, yerr)
    
    cos_theta = numpy.sum(data["detected"]["direction"][sel] * data["primary"]["direction"][sel], axis=1)
    # cos_theta = data["primary"]["direction"][data["tid"] == 1][:, 2]   
    hist, center = numpy.histogram(cos_theta, bins=numpy.linspace(-1.0, 1.0, len(backward.costheta.x) + 1))
    x = 0.5 * (center[1:] + center[:-1])
    width = center[1] - center[0]
    xerr = 0.5 * width
    y = ACTIVITY * hist / (header["events"] * width)
    yerr = ACTIVITY * numpy.sqrt(hist) / (header["events"] * width)
    geant4_cos_theta = Histogramed(x, y, xerr, yerr)
    
    
    
    plt.figure(figsize=(12, 7))
    geant4 = histogram(data["detected"]["energy"][sel], header["events"], fmt='k.', label="Geant4")
    forward.energy.errorbar(fmt="bo", label="Forward") 
    backward.energy.errorbar(fmt="ro", label="Backward") 
    plt.xscale("log")
    plt.xlabel("energy [MeV]")
    plt.legend()
    
    plt.figure(figsize=(12, 7))
    geant4_distance.errorbar(fmt="ko", label="Geant4")
    plt.yscale("log")
    plt.xlabel("distance [cm]")
    plt.legend()
    
    plt.figure(figsize=(12, 7))
    geant4_cos_theta.errorbar(fmt="k.", label="Geant4")
    forward.costheta.errorbar(fmt="bo", label="Forward") 
    backward.costheta.errorbar(fmt="ro", label="Backward") 
    plt.xlabel(r"$\cos\theta$")
    plt.legend()
    
    # rel dif
    delta_f = geant4.relative_difference(forward.energy)
    delta_b = geant4.relative_difference(backward.energy)
    plt.figure(figsize=(12, 7))
    plt.plot(geant4.x, numpy.zeros(geant4.x.shape), "k-")
    delta_f.errorbar(fmt="bo", label="Forward")
    delta_b.errorbar(fmt="ro", label="Backward")
    plt.xscale("log")
    plt.xlabel("energy [MeV]")
    plt.legend()
    
    plt.show()
