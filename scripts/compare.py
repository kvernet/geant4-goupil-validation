#!/usr/bin/env python3
import os
import argparse
import numpy
import matplotlib.pyplot as plt

import sys
sys.path.append("../geant4-goupil/scripts")
from plot import Processor


ACTIVITY = 1E+04    # in Bq
EDGES = numpy.logspace(-2, 0, 21)

HeaderType = numpy.dtype(
    [
        ("model", "S32"),
        ("energy", "f8"),
        ("events", "i4")
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
        ("pid", "i4"),
        ("primary", StateType),
        ("detected", StateType)
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
    
    print(events)
    
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
    
    sel = (data["detected"]["energy"] < data["primary"]["energy"]) & (data["tid"] == 1)
    print(f"selected = {sum(sel)}")
    
    backward = Processor("../geant4-goupil/share/goupil/backward.pkl.gz", EDGES)
    
    plt.figure(figsize=(12, 7))
    histogram(data["detected"]["energy"][sel], header["events"], fmt='k.', label="Geant4")
    backward.energy.errorbar(fmt="ro", label="Backward") 
    plt.xscale("log")
    plt.legend()
    
    plt.show()
