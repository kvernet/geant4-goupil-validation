#!/usr/bin/env python3

import numpy
import matplotlib.pyplot as plt

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

def getData(path="geant4-goupil-validation.bin"):
    with open(path) as f:
        header = numpy.fromfile(f, dtype=HeaderType, count=1)
        data = numpy.fromfile(f, dtype=EventType)
    
    return header, data


if __name__ == "__main__":
    header, data = getData()
    print(data)
