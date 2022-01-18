#!/usr/bin/env python3

import datetime
import subprocess
import sys

import serial
import numpy as np
from config import Settings


def toLog(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000)}k"
  else:
    value = f"{value}"
  return f"{name}: {value}\n"

def run_bench(scheme, impl, iterations):
    binary = f"bin/crypto_kem_{scheme}_{impl}_speed.bin"

    try:
        subprocess.check_call(f"st-flash write {binary} 0x8000000", shell=True)
    except:
        print("st-flash failed --> retry")
        return run_bench(scheme, impl, iterations)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, Settings.BAUD_RATE, timeout=10) as dev:
        logs = []
        iteration = 0
        log = b""
        while iteration < iterations:
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_bench(scheme, impl, iterations)
            sys.stdout.buffer.write(device_output)
            sys.stdout.flush()
            log += device_output
            if device_output == b'#':
                logs.append(log)
                log = b""
                iteration += 1
    return logs


def parseLogSpeed(log, ignoreErrors):
    log = log.decode(errors="ignore")
    if "error" in log.lower() and not ignoreErrors:
        raise Exception("error in scheme. this is very bad.")
    lines = str(log).splitlines()

    def get(lines, key):
        if key in lines:
            return int(lines[1+lines.index(key)])
        else:
            return None

    def cleanNullTerms(d):
        return {
            k:v
            for k, v in d.items()
            if v is not None
        }

    return cleanNullTerms({
        f"keygen":  get(lines, "keypair cycles:"),
        f"encaps":  get(lines, "encaps cycles:"),
        f"decaps":  get(lines, "decaps cycles:"),
    })

def average(results):
    avgs = dict()
    for key in results[0].keys():
        avgs[key] = int(np.array([results[i][key] for i in range(len(results))]).mean())
    return avgs


def bench(scheme, texName, impl, iterations, outfile, ignoreErrors=False):
    logs    = run_bench(scheme, impl, iterations)
    results = []
    for log in logs:
        try:
            result = parseLogSpeed(log, ignoreErrors)
        except:
            breakpoint()
            print("parsing log failed -> retry")
            return bench(scheme, texName, impl, iterations, outfile)
        results.append(result)

    avgResults = average(results)
    print(f"M4 results for {scheme} (impl={impl})", file=outfile)

    for key, value in avgResults.items():
        macro = toLog(f"{texName}{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)

def makeAll(iterations):
    subprocess.check_call(f"make clean", shell=True)
    subprocess.check_call(f"make -j4 ITERATIONS={iterations}", shell=True)


with open(f"benchmarks.txt", "w") as outfile:
    iterations = 100

    now = datetime.datetime.now(datetime.timezone.utc)
    print(f"% Benchmarking measurements written on {now}; iterations=100\n", file=outfile)

    makeAll(iterations)

    for scheme in ["lightsaber", "saber", "firesaber"]:
        for imple in ["speed", "stack"]:
            bench(scheme, scheme + "m4f" + imple, "m4f" + imple, 100, outfile)




