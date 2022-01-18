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
    binary = f"elf/crypto_kem_{scheme}_{impl}_f_speed.elf"

    try:
        subprocess.check_call(f"openocd -f nucleo-f2.cfg -c \"program {binary} reset exit\" ", shell=True)
    except:
        print("openocd failed --> retry")
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
        f"MatrixVectorMul_A": get(lines, "32-bit MatrixVectorMul cycles:"),
        f"InnerProd (Encrypt)": get(lines, "32-bit InnderProd (Encrypt) cycles:"),
        f"InnerProd (Decrypt)": get(lines, "32-bit InnderProd (Decrypt) cycles:"),
        f"32-bit NTT": get(lines, "32-bit NTT cycles:"),
        f"32-bit NTT_leak": get(lines, "32-bit NTT_leak cycles:"),
        f"32-bit iNTT": get(lines, "32-bit iNTT cycles:"),
        f"32-bit base_mul": get(lines, "32-bit basemul cycles:"),
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
    print(f"M3 results for {scheme} (impl={impl})", file=outfile)

    for key, value in avgResults.items():
        macro = toLog(f"{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)

def makeAll(iterations):
    subprocess.check_call(f"make clean", shell=True)
    subprocess.check_call(f"make -j4 ITERATIONS={iterations}", shell=True)


with open(f"f_32_benchmarks.txt", "w") as outfile:
    iterations = 100

    now = datetime.datetime.now(datetime.timezone.utc)
    print(f"% Benchmarking measurements written on {now}; iterations={iterations}\n", file=outfile)

    makeAll(iterations)

    for scheme in ["lightsaber", "saber", "firesaber"]:
        for imple in ["_32bit"]:
            bench(scheme, scheme, "m3" + imple, iterations, outfile)




