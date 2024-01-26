#!/usr/bin/env python3
import os
import argparse
import numpy
import matplotlib.pyplot as plt
import pickle

from goupil_analysis import DataSummary, Histogramed

plt.style.use("scripts/paper.mplstyle")


ACTIVITY = 1E+04    # in Bq

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
        
    def histograms(sel):
        energies = data["detected"]["energy"][sel]
        cos_theta = numpy.sum(data["detected"]["direction"][sel] * data["primary"]["direction"][sel], axis=1)
        distances = numpy.linalg.norm(
            data["detected"]["position"][sel] - data["primary"]["position"][sel],
            axis = 1
        )
        return DataSummary.new(header["events"], energies, cos_theta, distances)
        
    primaries = histograms(sel0)
    secondaries = histograms(sel1)
    
    os.makedirs("plots", exist_ok=True)
    
    plt.figure(figsize=(12, 7))
    primaries.energy.errorbar(fmt="ko", label="Primaries")
    secondaries.energy.errorbar(fmt="ro", label="Secondaries") 
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("energy [MeV]")
    plt.ylabel("rate [Hz/MeV]")
    plt.legend()
    plt.savefig("plots/energy-absolute.pdf")
    
    plt.figure(figsize=(12, 7))
    primaries.distance.errorbar(fmt="ko", label="Primaries")
    secondaries.distance.errorbar(fmt="ro", label="Secondaries") 
    plt.yscale("log")
    plt.xlabel("distance [cm]")
    plt.legend()
    
    plt.figure(figsize=(12, 7))
    primaries.cos_theta.errorbar(fmt="ko", label="Primaries")
    secondaries.cos_theta.errorbar(fmt="ro", label="Secondaries")  
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
    rays = Histogramed(
        source_energies, intensities[:,0], numpy.zeros(source_energies.size), intensities[:,1]
    )
        
    plt.figure(figsize=(12, 7))
    rays.errorbar(fmt="ko")
    plt.xlabel(r"energy [MeV]")
    plt.ylabel("rate [Hz]")
    
    # Export results.
    data = {
        "primaries": primaries,
        "secondaries": secondaries,
        "rays": rays
    }
 
    with open("geant4.pkl", "wb") as f:
        pickle.dump(data, f)
    
    plt.show()
