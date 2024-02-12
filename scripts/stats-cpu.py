#! /usr/bin/env python3
import argparse
import os
import numpy
import subprocess


def get_stats(path):
    HeaderType = numpy.dtype(
        [
            ("model", "S32"),
            ("energy", "f8"),
            ("events", "u8"),
            ("generated", "u8")
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

    with open(path) as f:
        header = numpy.fromfile(f, dtype=HeaderType, count=1)[0]
        data = numpy.fromfile(f, dtype=EventType)
    return (int(header["generated"]), len(data["detected"]))


def get_elapsed(jobid):
    r = subprocess.run(f"sacct -j {jobid} --format='JobIDRaw,Elapsed'",
        shell=True, capture_output=True, check=True)
    elapsed = []
    for line in r.stdout.decode().split(os.linesep)[2::2]:
        if not line: continue
        raw, line = line.split()
        raw = int(raw)
        if "-" in line:
            d, hms = line.split("-", 1)
            d = int(d)
        else:
            d = 0
            hms = line
        h, m, s = map(int, hms.split(":"))
        
        elapsed.append((raw, s  + 60 * (m + 60 * (h + 24 * d))))
    return elapsed


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Compute cpu.")
    
    parser.add_argument("-f",
        dest="files",
        help="input files",
        nargs="+")
    
    args = parser.parse_args()

    generated, selected = 0, 0
    elapsed = {}
    for filename in args.files:
        ng, ns = get_stats(filename)
        generated += ng
        selected += ns
        filename = os.path.basename(filename)
        name, ext = os.path.splitext(filename)
        jobid = int(name.split("-")[1])
        for (raw, el) in get_elapsed(jobid):
            if raw in elapsed:
                assert(elapsed[raw] == el)
            elapsed[raw] = el
    assert(len(elapsed) == len(args.files))
    total_elapsed = sum(elapsed.values())

    print(f"generated = {generated}")
    print(f"selected  = {selected}")
    print(f"elapsed   = {total_elapsed} s")
